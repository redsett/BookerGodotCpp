#include "bg_battle_board_3d_builder.hpp"

#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector3i.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace godot;

namespace
{
constexpr float HEX_WIDTH = 1.0f;
constexpr float ROW_SPACING = HEX_WIDTH * 0.8660254f;
constexpr float HEX_RADIUS = HEX_WIDTH * 0.5f / 0.8660254f;
constexpr float LEVEL_HEIGHT = 0.35f;
constexpr float PI_F = 3.14159265358979323846f;
constexpr float TAU_F = 6.28318530717958647692f;
constexpr float DEG2RAD_F = PI_F / 180.0f;
constexpr float INV_255 = 1.0f / 255.0f;

constexpr float NOISE_CELLS = 16.0f;
constexpr int MUD_LAYER = 2;
constexpr float NO_BORDER = 99.0f;
constexpr int ALL_IN_BOUNDS = 127;

// Skirt constants (matching BG_BattleBoard3DSkirt.gd).
constexpr float CORNER_ROUND = 0.015f;
constexpr int SIDE_POINTS = 5;
constexpr float OUTSET = 0.004f;
constexpr float LIP_IN = 0.03f;
constexpr float LIP_DOWN = 0.006f;
constexpr float APRON_IN = 0.02f;
constexpr float APRON_DOWN = 0.003f;
constexpr float RIM_DEPTH = 1.3f;
constexpr float RAGGED = 0.25f;
constexpr int SMOOTH_REACH = 7;
constexpr int SOFTEN_REACH = 3;
constexpr float EROSION = 0.035f;
constexpr float INTERIOR_TUCK = 0.10f;
constexpr float RIM_ROWS[9] = {0.0f, 0.06f, 0.14f, 0.24f, 0.36f, 0.52f, 0.72f, 0.96f, 1.0f};
constexpr float INTERIOR_ROWS[6] = {0.0f, 0.15f, 0.35f, 0.6f, 0.85f, 1.0f};

constexpr float SIDE_DX[6] = {1.0f, 0.5f, -0.5f, -1.0f, -0.5f, 0.5f};
constexpr float SIDE_DZ[6] = {0.0f, 0.8660254f, 0.8660254f, 0.0f, -0.8660254f, -0.8660254f};

constexpr int NEIGHBOR_DQ_SHIFTED[6] = {0, 1, 1, 0, -1, -1};
constexpr int NEIGHBOR_DR_SHIFTED[6] = {1, 1, 0, -1, 0, 1};
constexpr int NEIGHBOR_DQ_UNSHIFTED[6] = {0, 1, 1, 0, -1, -1};
constexpr int NEIGHBOR_DR_UNSHIFTED[6] = {1, 0, -1, -1, -1, 0};

inline uint64_t pack_qr(int32_t q, int32_t r)
{
	return (static_cast<uint64_t>(static_cast<uint32_t>(q)) << 32) | static_cast<uint64_t>(static_cast<uint32_t>(r));
}

inline int posmod_i(int a, int b)
{
	int m = a % b;
	return m < 0 ? m + b : m;
}

inline float smoothstep_f(float edge0, float edge1, float x)
{
	if (edge1 <= edge0)
		return x < edge0 ? 0.0f : 1.0f;
	float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

inline float lerpf_c(float a, float b, float t)
{
	return a + (b - a) * t;
}

inline Vector2 qr_to_local_2d(int q, int r, int parity)
{
	float x = static_cast<float>(r) * HEX_WIDTH + (((q & 1) == parity) ? (HEX_WIDTH * 0.5f) : 0.0f);
	float z = static_cast<float>(q) * ROW_SPACING;
	return Vector2(x, z);
}

// Fast deterministic RNG seeded per cell (SplitMix64).
struct FastRng
{
	uint64_t state;

	void seed(int64_t s, int32_t q, int32_t r)
	{
		uint64_t h = static_cast<uint64_t>(s) * 0x9e3779b97f4a7c15ULL;
		h ^= static_cast<uint64_t>(static_cast<uint32_t>(q)) * 0xbf58476d1ce4e5b9ULL;
		h ^= static_cast<uint64_t>(static_cast<uint32_t>(r)) * 0x94d049bb133111ebULL;
		state = h ? h : 0x123456789abcdef0ULL;
	}

	uint64_t next_u64()
	{
		uint64_t z = (state += 0x9e3779b97f4a7c15ULL);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
		z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
		return z ^ (z >> 31);
	}

	float randf()
	{
		return static_cast<float>(next_u64() >> 40) * (1.0f / 16777216.0f);
	}

	int rand_range_0_5()
	{
		return static_cast<int>(next_u64() % 6ULL);
	}
};

inline Vector2 sample_noise(const uint8_t *bytes, int w, int h, float u, float v, bool second)
{
	float x = u * static_cast<float>(w) - 0.5f;
	float y = v * static_cast<float>(h) - 0.5f;
	int x0 = static_cast<int>(std::floor(x));
	int y0 = static_cast<int>(std::floor(y));
	float fx = x - static_cast<float>(x0);
	float fy = y - static_cast<float>(y0);
	int ix0 = posmod_i(x0, w);
	int ix1 = posmod_i(x0 + 1, w);
	int iy0 = posmod_i(y0, h) * w;
	int iy1 = posmod_i(y0 + 1, h) * w;
	int ch = second ? 2 : 0;
	int o00 = (iy0 + ix0) * 4 + ch;
	int o10 = (iy0 + ix1) * 4 + ch;
	int o01 = (iy1 + ix0) * 4 + ch;
	int o11 = (iy1 + ix1) * 4 + ch;
	float r0 = lerpf_c(static_cast<float>(bytes[o00]), static_cast<float>(bytes[o10]), fx);
	float r1 = lerpf_c(static_cast<float>(bytes[o01]), static_cast<float>(bytes[o11]), fx);
	float g0 = lerpf_c(static_cast<float>(bytes[o00 + 1]), static_cast<float>(bytes[o10 + 1]), fx);
	float g1 = lerpf_c(static_cast<float>(bytes[o01 + 1]), static_cast<float>(bytes[o11 + 1]), fx);
	return Vector2(lerpf_c(r0, r1, fy) * INV_255, lerpf_c(g0, g1, fy) * INV_255);
}

inline float dict_float(const Dictionary &d, const char *key, float fallback)
{
	Variant v = d.get(key, Variant());
	return v.get_type() == Variant::NIL ? fallback : static_cast<float>(v);
}

inline int dict_int(const Dictionary &d, const char *key, int fallback)
{
	Variant v = d.get(key, Variant());
	return v.get_type() == Variant::NIL ? fallback : static_cast<int>(v);
}

inline bool dict_bool(const Dictionary &d, const char *key, bool fallback)
{
	Variant v = d.get(key, Variant());
	return v.get_type() == Variant::NIL ? fallback : static_cast<bool>(v);
}

// ---------- Skirt helpers ----------

struct RimEdge
{
	Vector2i qr;
	Vector2 center;
	int side;
	uint64_t start_key;
	uint64_t end_key;
};

inline uint64_t corner_key(const Vector2 &center, float angle_deg)
{
	float rad = angle_deg * DEG2RAD_F;
	float px = center.x + std::cos(rad) * HEX_RADIUS;
	float py = center.y + std::sin(rad) * HEX_RADIUS;
	return pack_qr(static_cast<int32_t>(std::round(px * 1000.0f)), static_cast<int32_t>(std::round(py * 1000.0f)));
}

inline Vector2 fillet_point(const Vector2 &center, float angle_deg, float offset_deg)
{
	static const float INSET = CORNER_ROUND / std::sin(60.0f * DEG2RAD_F);
	float rad = angle_deg * DEG2RAD_F;
	float rad2 = (angle_deg + offset_deg) * DEG2RAD_F;
	return Vector2(
		center.x + std::cos(rad) * (HEX_RADIUS - INSET) + std::cos(rad2) * CORNER_ROUND,
		center.y + std::sin(rad) * (HEX_RADIUS - INSET) + std::sin(rad2) * CORNER_ROUND);
}

inline float skirt_noise(float x)
{
	return std::sin(x * 1.0f) * 0.5f + std::sin(x * 2.37f + 1.3f) * 0.3f + std::sin(x * 5.11f + 4.1f) * 0.2f;
}

inline float hash3(int a, int b, int c)
{
	int64_t n = (static_cast<int64_t>(a) * 374761393LL + static_cast<int64_t>(b) * 668265263LL + static_cast<int64_t>(c) * 2147483647LL) & 0x7fffffffLL;
	n = ((n ^ (n >> 13)) * 1274126177LL) & 0x7fffffffLL;
	return static_cast<float>(n & 0xffffLL) / 65535.0f;
}

struct SkirtBuffers
{
	Vector<Vector3> verts;
	Vector<Vector3> normals;
	Vector<Color> colors;
	Vector<Vector2> uv;
	Vector<Vector2> uv2;
	Vector<int32_t> indices;

	Vector<Vector3> apron_verts;
	Vector<Color> apron_colors;
	Vector<int32_t> apron_indices;

	Vector<Vector3> cliff_verts;
	Vector<Vector3> cliff_normals;
	Vector<Color> cliff_colors;
	Vector<Vector2> cliff_uv;
	Vector<Vector2> cliff_uv2;
	Vector<int32_t> cliff_indices;

	void add_vert(const Vector3 &at, const Vector3 &n, const Color &col, float along, float depth, float dim)
	{
		verts.push_back(at);
		normals.push_back(n);
		colors.push_back(col);
		uv.push_back(Vector2(along, depth));
		uv2.push_back(Vector2(dim, 0.0f));
	}

	void add_cliff_vert(const Vector3 &at, const Vector3 &n, const Color &col, float along, float depth, float dim, float is_rock)
	{
		cliff_verts.push_back(at);
		cliff_normals.push_back(n);
		cliff_colors.push_back(col);
		cliff_uv.push_back(Vector2(along, depth));
		cliff_uv2.push_back(Vector2(dim, is_rock));
	}

	void add_cliff_quad(int32_t i0, int32_t i1, int32_t i2, int32_t i3, const Vector3 &out_dir)
	{
		Vector3 face = (cliff_verts[i1] - cliff_verts[i0]).cross(cliff_verts[i2] - cliff_verts[i0]);
		if (face.dot(out_dir) > 0.0f)
		{
			cliff_indices.push_back(i0);
			cliff_indices.push_back(i2);
			cliff_indices.push_back(i1);
			cliff_indices.push_back(i0);
			cliff_indices.push_back(i3);
			cliff_indices.push_back(i2);
		}
		else
		{
			cliff_indices.push_back(i0);
			cliff_indices.push_back(i1);
			cliff_indices.push_back(i2);
			cliff_indices.push_back(i0);
			cliff_indices.push_back(i2);
			cliff_indices.push_back(i3);
		}
	}
};

void add_masonry_block(SkirtBuffers &b, const Vector2 &p_left, const Vector2 &p_right, const Vector2 &normal,
	float protrude, float top_y, float bot_y, float uv_top, float uv_bot, float dim, const Color &color)
{
	Vector2 back_l = p_left - normal * 0.02f;
	Vector2 back_r = p_right - normal * 0.02f;
	Vector2 front_l = p_left + normal * protrude;
	Vector2 front_r = p_right + normal * protrude;
	Vector3 n3(normal.x, 0.0f, normal.y);
	int32_t f = static_cast<int32_t>(b.cliff_verts.size());
	b.add_cliff_vert(Vector3(front_l.x, top_y, front_l.y), n3, color, 0.0f, uv_top, dim, 1.0f);
	b.add_cliff_vert(Vector3(front_r.x, top_y, front_r.y), n3, color, 0.25f, uv_top, dim, 1.0f);
	b.add_cliff_vert(Vector3(front_r.x, bot_y, front_r.y), n3, color, 0.25f, uv_bot, dim, 1.0f);
	b.add_cliff_vert(Vector3(front_l.x, bot_y, front_l.y), n3, color, 0.0f, uv_bot, dim, 1.0f);
	b.add_cliff_vert(Vector3(back_l.x, top_y, back_l.y), Vector3(0, 1, 0), color, 0.0f, uv_top, dim, 1.0f);
	b.add_cliff_vert(Vector3(back_r.x, top_y, back_r.y), Vector3(0, 1, 0), color, 0.25f, uv_top, dim, 1.0f);
	b.add_cliff_vert(Vector3(back_r.x, bot_y, back_r.y), n3, color, 0.25f, uv_bot, dim, 1.0f);
	b.add_cliff_vert(Vector3(back_l.x, bot_y, back_l.y), n3, color, 0.0f, uv_bot, dim, 1.0f);
	Vector3 tan3(-normal.y, 0.0f, normal.x);
	b.add_cliff_quad(f + 0, f + 1, f + 2, f + 3, n3);
	b.add_cliff_quad(f + 4, f + 5, f + 1, f + 0, Vector3(0, 1, 0));
	b.add_cliff_quad(f + 4, f + 0, f + 3, f + 7, -tan3);
	b.add_cliff_quad(f + 1, f + 5, f + 6, f + 2, tan3);
}

void add_road_masonry(SkirtBuffers &buf, const Vector2i &qr, const Vector2 &center, int side,
	float y0, float y1, float b0, float b1, float dim)
{
	float a0 = static_cast<float>(side) * 60.0f - 30.0f;
	float a1 = static_cast<float>(side) * 60.0f + 30.0f;
	float nr = static_cast<float>(side) * 60.0f * DEG2RAD_F;
	Vector2 normal(std::cos(nr), std::sin(nr));
	Vector2 tangent(-normal.y, normal.x);
	Vector2 edge_start = center + Vector2(std::cos(a0 * DEG2RAD_F), std::sin(a0 * DEG2RAD_F)) * HEX_RADIUS;
	Vector2 edge_end = center + Vector2(std::cos(a1 * DEG2RAD_F), std::sin(a1 * DEG2RAD_F)) * HEX_RADIUS;
	float max_drop = std::max(y0 - (b0 + INTERIOR_TUCK), y1 - (b1 + INTERIOR_TUCK));
	if (max_drop < 0.05f)
		return;
	constexpr float COURSE_H = 0.175f;
	int courses = std::max(static_cast<int>(std::ceil((max_drop + INTERIOR_TUCK) / COURSE_H)), 2);
	constexpr int BLOCKS_PER_COURSE = 3;
	constexpr float MORTAR = 0.012f;
	float denom = std::max(max_drop, 0.1f);
	for (int row = 0; row < courses; row++)
	{
		float stagger = (row & 1) ? (0.5f / static_cast<float>(BLOCKS_PER_COURSE)) : 0.0f;
		int count = BLOCKS_PER_COURSE + ((row & 1) ? 1 : 0);
		for (int bi = 0; bi < count; bi++)
		{
			float t0 = std::clamp(static_cast<float>(bi) / static_cast<float>(BLOCKS_PER_COURSE) - stagger, 0.0f, 1.0f);
			float t1 = std::clamp(static_cast<float>(bi + 1) / static_cast<float>(BLOCKS_PER_COURSE) - stagger, 0.0f, 1.0f);
			if (t1 - t0 < 0.08f)
				continue;
			float t_mid = (t0 + t1) * 0.5f;
			float top_here = lerpf_c(y0, y1, t_mid);
			float bot_here = lerpf_c(b0, b1, t_mid);
			float block_top = top_here - static_cast<float>(row) * COURSE_H - (row == 0 ? 0.01f : MORTAR * 0.5f);
			float block_bot = std::max(top_here - static_cast<float>(row + 1) * COURSE_H + MORTAR * 0.5f, bot_here);
			if (block_top - block_bot < 0.03f)
				continue;
			float h0 = hash3(qr.x, qr.y, side * 37 + row * 11 + bi * 3 + 1);
			float h1 = hash3(qr.x, qr.y, side * 37 + row * 11 + bi * 3 + 2);
			Vector2 p_left = edge_start.lerp(edge_end, t0) + tangent * MORTAR;
			Vector2 p_right = edge_start.lerp(edge_end, t1) - tangent * MORTAR;
			float protrude = 0.042f + h0 * 0.018f + static_cast<float>(row) * 0.008f;
			float shade = 0.88f + h1 * 0.2f;
			Color col(0.55f * shade, 0.52f * shade, 0.47f * shade, 1.0f);
			float uv_top = std::clamp((top_here - block_top) / denom, 0.0f, 1.0f);
			float uv_bot = std::clamp((top_here - block_bot) / denom, 0.0f, 1.0f);
			add_masonry_block(buf, p_left, p_right, normal, protrude, block_top, block_bot, uv_top, uv_bot, dim, col);
		}
	}
}

inline Color cliff_rock_color(bool is_slate, float v1, float v2)
{
	float shade = 0.86f + v1 * 0.24f;
	float warm = (v2 - 0.5f) * 0.08f;
	if (is_slate)
	{
		// Warm weathered limestone / buff sandstone blocks matching the paintover's chiseled rock terraces.
		return Color((0.52f + warm) * shade, (0.46f + warm * 0.6f) * shade, (0.38f - warm * 0.4f) * shade, 1.0f);
	}
	// Embedded fieldstones on earthy grass embankments: warm ochre-taupe stone.
	return Color((0.50f + warm) * shade, (0.43f + warm * 0.5f) * shade, (0.33f - warm * 0.4f) * shade, 1.0f);
}

void add_rock_pillar(SkirtBuffers &buf, const Vector2 &at, const Vector2 &normal, const Vector2 &tangent,
	float half_w, float protrude, float top_y, float bottom_y, float dim, const Color &color, int seed_val)
{
	// Chiseled flat-fronted stone block profile: wide flat face with crisp chamfered side shoulders so
	// adjacent blocks meet in clean vertical masonry/strata crevices rather than pinched pill tubes.
	static const Vector2 PROFILE[5] = {
		Vector2(-1.0f, -0.10f),
		Vector2(-0.82f, 0.88f),
		Vector2(0.0f, 1.0f),
		Vector2(0.82f, 0.88f),
		Vector2(1.0f, -0.10f),
	};
	static const float TALL_FRACS[4] = {0.0f, 0.24f, 0.66f, 1.0f};
	static const float LOW_FRACS[3] = {0.0f, 0.42f, 1.0f};
	static const float TALL_BULGE[4] = {0.58f, 0.88f, 1.04f, 1.18f};
	static const float LOW_BULGE[3] = {0.62f, 0.92f, 1.14f};

	float drop = std::max(top_y - bottom_y, 0.06f);
	bool tall = drop > LEVEL_HEIGHT * 1.4f;
	const float *v_fracs = tall ? TALL_FRACS : LOW_FRACS;
	const float *v_bulge = tall ? TALL_BULGE : LOW_BULGE;
	int rings = tall ? 4 : 3;
	constexpr int COLS = 5;
	int32_t first = static_cast<int32_t>(buf.cliff_verts.size());
	for (int r = 0; r < rings; r++)
	{
		float vf = v_fracs[r];
		float y_base = lerpf_c(top_y, bottom_y, vf);
		float b_scale = v_bulge[r];
		for (int p_i = 0; p_i < COLS; p_i++)
		{
			Vector2 prof = PROFILE[p_i];
			float jx = (hash3(seed_val, r, p_i * 3 + 1) - 0.5f) * 0.12f;
			float jz = (hash3(seed_val, r, p_i * 3 + 2) - 0.5f) * 0.16f;
			float jy = (r > 0 && r < rings - 1) ? ((hash3(seed_val, r, p_i * 3 + 3) - 0.5f) * 0.025f) : 0.0f;
			float along_t = (prof.x + jx * (1.0f - std::fabs(prof.x) * 0.5f)) * half_w * (0.90f + 0.14f * b_scale);
			float out_n = (prof.y * b_scale + jz * std::max(prof.y, 0.0f)) * protrude;
			Vector2 pos2 = at + tangent * along_t + normal * out_n;
			Vector2 n2 = (normal * std::max(prof.y, 0.15f) + tangent * prof.x * 0.55f).normalized();
			Vector3 n3 = Vector3(n2.x, 0.25f, n2.y).normalized();
			float u_block = static_cast<float>(p_i) / static_cast<float>(COLS - 1);
			buf.add_cliff_vert(Vector3(pos2.x, y_base + jy, pos2.y), n3, color, u_block, vf, dim, 1.0f);
		}
	}
	for (int r = 0; r < rings - 1; r++)
	{
		for (int p_i = 0; p_i < COLS - 1; p_i++)
		{
			int32_t a = first + r * COLS + p_i;
			int32_t b = first + r * COLS + p_i + 1;
			int32_t c = first + (r + 1) * COLS + p_i + 1;
			int32_t d = first + (r + 1) * COLS + p_i;
			buf.add_cliff_quad(a, b, c, d, buf.cliff_normals[a] + buf.cliff_normals[c]);
		}
	}
	Vector2 cap_back = at - normal * 0.04f;
	int32_t cap_center = static_cast<int32_t>(buf.cliff_verts.size());
	buf.add_cliff_vert(Vector3(cap_back.x, top_y - 0.004f, cap_back.y), Vector3(0, 1, 0), color, 0.5f, 0.0f, dim, 1.0f);
	for (int p_i = 0; p_i < COLS - 1; p_i++)
	{
		int32_t a = first + p_i;
		int32_t b = first + p_i + 1;
		float turn = (buf.cliff_verts[b] - buf.cliff_verts[cap_center]).cross(buf.cliff_verts[a] - buf.cliff_verts[cap_center]).y;
		if (turn < 0.0f)
		{
			buf.cliff_indices.push_back(cap_center);
			buf.cliff_indices.push_back(b);
			buf.cliff_indices.push_back(a);
		}
		else
		{
			buf.cliff_indices.push_back(cap_center);
			buf.cliff_indices.push_back(a);
			buf.cliff_indices.push_back(b);
		}
	}
}

void add_cliff_rocks(SkirtBuffers &buf, const Vector2i &qr, const Vector2 &center, int side,
	float y0, float y1, float b0, float b1, float dim, bool is_slate, bool has_prev_raised, bool has_next_raised)
{
	float a0 = static_cast<float>(side) * 60.0f - 30.0f;
	float a1 = static_cast<float>(side) * 60.0f + 30.0f;
	float nr = static_cast<float>(side) * 60.0f * DEG2RAD_F;
	Vector2 normal(std::cos(nr), std::sin(nr));
	Vector2 tangent(-normal.y, normal.x);
	Vector2 edge_start = center + Vector2(std::cos(a0 * DEG2RAD_F), std::sin(a0 * DEG2RAD_F)) * HEX_RADIUS;
	Vector2 edge_end = center + Vector2(std::cos(a1 * DEG2RAD_F), std::sin(a1 * DEG2RAD_F)) * HEX_RADIUS;
	if (!is_slate)
	{
		// Non-rock grass/earth embankment: leave the sloped earth & overhanging turf lip visible, embedding
		// only an occasional low-profile weathered stone into the lower bank.
		float h_pick = hash3(qr.x, qr.y, side * 23 + 5);
		if (h_pick < 0.35f)
			return;
		float t_mid = 0.28f + h_pick * 0.44f;
		float local_top = lerpf_c(y0, y1, t_mid);
		float local_bot = lerpf_c(b0, b1, t_mid);
		float local_drop = local_top - (local_bot + INTERIOR_TUCK);
		if (local_drop < 0.08f)
			return;
		float h1 = hash3(qr.x, qr.y, side * 17 + 2);
		float h2 = hash3(qr.x, qr.y, side * 17 + 3);
		Vector2 base_pos = edge_start.lerp(edge_end, t_mid) + normal * (local_drop * 0.14f);
		float half_w = 0.14f + h1 * 0.05f;
		float protrude = 0.055f + h2 * 0.03f;
		float rock_top = local_top - local_drop * (0.28f + h1 * 0.18f);
		Color col = cliff_rock_color(false, h1, h2);
		add_rock_pillar(buf, base_pos, normal, tangent, half_w, protrude, rock_top, local_bot, dim, col,
			qr.x * 31 + qr.y * 17 + side * 7 + 1);
		return;
	}
	for (int c = 0; c < 2; c++)
	{
		float h0 = hash3(qr.x, qr.y, side * 17 + c * 5 + 1);
		float h1 = hash3(qr.x, qr.y, side * 17 + c * 5 + 2);
		float h2 = hash3(qr.x, qr.y, side * 17 + c * 5 + 3);
		float h3 = hash3(qr.x, qr.y, side * 17 + c * 5 + 4);
		float t_mid = (static_cast<float>(c) + 0.5f + (h0 - 0.5f) * 0.06f) * 0.5f;
		float local_top = lerpf_c(y0, y1, t_mid);
		float local_bot = lerpf_c(b0, b1, t_mid);
		float local_drop = local_top - (local_bot + INTERIOR_TUCK);
		if (local_drop < 0.03f)
			continue;
		float drop_scale = std::clamp(local_drop / LEVEL_HEIGHT, 0.35f, 1.5f);
		Vector2 base_pos = edge_start.lerp(edge_end, t_mid);
		float half_w = (0.235f + h1 * 0.035f) * std::clamp(drop_scale, 0.8f, 1.15f);
		float protrude = (0.078f + h2 * 0.038f) * std::clamp(drop_scale, 0.6f, 1.3f);
		float rock_top = local_top - (0.022f + h3 * 0.016f);
		Color col = cliff_rock_color(true, h1, h3);
		add_rock_pillar(buf, base_pos, normal, tangent, half_w, protrude, rock_top, local_bot, dim, col,
			qr.x * 31 + qr.y * 17 + side * 7 + c * 13);
	}
	if ((y1 - (b1 + INTERIOR_TUCK)) >= 0.04f)
	{
		Vector2 corner_dir = has_next_raised ? Vector2(std::cos(a1 * DEG2RAD_F), std::sin(a1 * DEG2RAD_F)) : normal;
		Vector2 corner_tan(-corner_dir.y, corner_dir.x);
		float hc = hash3(qr.x, qr.y, side * 29 + 99);
		Color col = cliff_rock_color(true, hc, 1.0f - hc);
		add_rock_pillar(buf, edge_end, corner_dir, corner_tan, 0.145f, 0.068f + hc * 0.032f, y1 - 0.022f, b1, dim, col,
			qr.x * 13 + qr.y * 41 + side * 19);
	}
	if (!has_prev_raised && (y0 - (b0 + INTERIOR_TUCK)) >= 0.04f)
	{
		float hc0 = hash3(qr.x, qr.y, side * 31 + 77);
		Color col0 = cliff_rock_color(true, hc0, 1.0f - hc0);
		add_rock_pillar(buf, edge_start, normal, tangent, 0.135f, 0.062f + hc0 * 0.028f, y0 - 0.022f, b0, dim, col0,
			qr.x * 19 + qr.y * 23 + side * 11);
	}
}

void average_v2(const Vector<Vector2> &values, bool closed, int reach, Vector<Vector2> &out)
{
	int count = static_cast<int>(values.size());
	out.resize(count);
	float inv = 1.0f / static_cast<float>(reach * 2 + 1);
	for (int i = 0; i < count; i++)
	{
		Vector2 sum(0.0f, 0.0f);
		for (int j = i - reach; j <= i + reach; j++)
		{
			int k = closed ? posmod_i(j, count) : std::clamp(j, 0, count - 1);
			sum += values[k];
		}
		out.write[i] = sum * inv;
	}
}

void envelope_v2(const Vector<Vector2> &averaged, const Vector<Vector2> &points, const Vector<Vector2> &smooth_normals,
	bool closed, int reach, Vector<Vector2> &out)
{
	int count = static_cast<int>(points.size());
	out.resize(count);
	for (int i = 0; i < count; i++)
	{
		Vector2 n = smooth_normals[i];
		float push = 0.0f;
		for (int j = i - reach; j <= i + reach; j++)
		{
			int k = closed ? posmod_i(j, count) : std::clamp(j, 0, count - 1);
			float d = (points[k] - averaged[i]).dot(n);
			if (d > push)
				push = d;
		}
		out.write[i] = averaged[i] + n * push;
	}
}

template <typename T, typename PackedT>
PackedT to_packed(const Vector<T> &v)
{
	PackedT out;
	int n = static_cast<int>(v.size());
	out.resize(n);
	if (n > 0)
	{
		T *dst = out.ptrw();
		const T *src = v.ptr();
		for (int i = 0; i < n; i++)
			dst[i] = src[i];
	}
	return out;
}
} // namespace

void BG_BattleBoard3DBuilder::_bind_methods()
{
	ClassDB::bind_static_method("BG_BattleBoard3DBuilder",
		D_METHOD("build_scatter_pool", "cell_qrs", "cell_grounds", "cell_flags", "cell_elevations",
			"pattern", "noise_bytes", "noise_w", "noise_h", "params"),
		&BG_BattleBoard3DBuilder::build_scatter_pool);
	ClassDB::bind_static_method("BG_BattleBoard3DBuilder",
		D_METHOD("build_scatter_instances", "point_cell", "point_x", "point_z", "point_center",
			"point_flags", "point_border", "point_color", "point_y", "point_normal",
			"point_pick", "point_turn", "point_size", "point_dim",
			"cell_out", "cell_relief", "cell_mults", "cell_chunk_x", "cell_chunk_z",
			"rates", "e_layers", "e_border_keep", "e_scale_min", "e_scale_max", "e_tall", "e_follow_slope",
			"params"),
		&BG_BattleBoard3DBuilder::build_scatter_instances);
	ClassDB::bind_static_method("BG_BattleBoard3DBuilder",
		D_METHOD("build_skirt_mesh", "cell_qrs", "cell_grounds", "cell_elevations", "cell_flags", "cell_ramps", "params"),
		&BG_BattleBoard3DBuilder::build_skirt_mesh);
}

Dictionary BG_BattleBoard3DBuilder::build_scatter_pool(
	const TypedArray<Vector2i> &p_cell_qrs,
	const PackedColorArray &p_cell_grounds,
	const PackedByteArray &p_cell_flags,
	const PackedFloat32Array &p_cell_elevations,
	const PackedVector2Array &p_pattern,
	const PackedByteArray &p_noise_bytes,
	int p_noise_w,
	int p_noise_h,
	const Dictionary &p_params)
{
	const int all_count = static_cast<int>(p_cell_qrs.size());
	const int pool_size = static_cast<int>(p_pattern.size());
	if (all_count == 0 || pool_size == 0)
		return Dictionary();

	const int parity = dict_int(p_params, "parity", 0);
	const int64_t scatter_seed = static_cast<int64_t>(dict_int(p_params, "scatter_seed", 0));
	const float warp = dict_float(p_params, "warp", 0.4f);
	const float warp_scale = dict_float(p_params, "warp_scale", 1.1f);
	const float warp_detail = dict_float(p_params, "warp_detail", 0.12f);
	const float warp_detail_scale = dict_float(p_params, "warp_detail_scale", 4.5f);
	const float shore_spread = dict_float(p_params, "shore_spread", 0.2f);
	const float paved_round = dict_float(p_params, "paved_round", 0.42f);
	const float paved_edge = dict_float(p_params, "paved_edge", 0.62f);
	const float paved_rim = dict_float(p_params, "paved_rim", 0.14f);
	const float bounds_round = dict_float(p_params, "bounds_round", 0.42f);
	const int paved_layer_mask = dict_int(p_params, "paved_layer_mask", 1 << 3);
	const int road_edges_flag = dict_int(p_params, "road_edges_flag", 0);
	const float oob_dim = dict_float(p_params, "oob_dim", 0.45f);
	const PackedInt32Array layer_flags_arr = p_params.get("layer_flags", PackedInt32Array());
	const int layer_flags_n = static_cast<int>(layer_flags_arr.size());
	const int32_t *layer_flags = layer_flags_arr.ptr();

	const float inv_2_bounds_sq = 1.0f / (2.0f * bounds_round * bounds_round);
	const float inv_2_paved_sq = 1.0f / (2.0f * paved_round * paved_round);
	const float inv_noise_w_scale = warp_scale / NOISE_CELLS;
	const float inv_noise_d_scale = warp_detail_scale / NOISE_CELLS;

	const Color *grounds_ptr = p_cell_grounds.ptr();
	const uint8_t *flags_ptr = p_cell_flags.ptr();
	const float *elev_ptr = p_cell_elevations.ptr();
	const Vector2 *pat_ptr = p_pattern.ptr();
	const uint8_t *noise_ptr = p_noise_bytes.ptr();

	Vector<Vector2i> all_qrs;
	all_qrs.resize(all_count);
	HashMap<uint64_t, int> cell_index;
	cell_index.reserve(all_count * 2);
	Vector<int> detail_indices;

	for (int i = 0; i < all_count; i++)
	{
		Vector2i qr = p_cell_qrs[i];
		all_qrs.write[i] = qr;
		cell_index.insert(pack_qr(qr.x, qr.y), i);
		if (flags_ptr[i] & 1)
			detail_indices.push_back(i);
	}

	// Multi-source BFS from in-bounds cells so out-of-bounds scatter points know their distance to the play-space edge.
	constexpr int MAX_BOUNDS_RINGS = 5;
	Vector<int> cell_ring;
	Vector<Vector2> nearest_in_pos;
	cell_ring.resize(all_count);
	nearest_in_pos.resize(all_count);
	Vector<int> bfs_queue;
	for (int i = 0; i < all_count; i++)
	{
		if ((flags_ptr[i] & 2) == 0)
		{
			cell_ring.write[i] = 0;
			nearest_in_pos.write[i] = qr_to_local_2d(all_qrs[i].x, all_qrs[i].y, parity);
			bfs_queue.push_back(i);
		}
		else
		{
			cell_ring.write[i] = -1;
			nearest_in_pos.write[i] = Vector2(0.0f, 0.0f);
		}
	}
	int bfs_head = 0;
	while (bfs_head < static_cast<int>(bfs_queue.size()))
	{
		const int cur = bfs_queue[bfs_head++];
		const int next_ring = cell_ring[cur] + 1;
		if (next_ring > MAX_BOUNDS_RINGS)
			continue;
		const Vector2i cqr = all_qrs[cur];
		const bool c_shifted = (cqr.x & 1) == parity;
		const int *cdq = c_shifted ? NEIGHBOR_DQ_SHIFTED : NEIGHBOR_DQ_UNSHIFTED;
		const int *cdr = c_shifted ? NEIGHBOR_DR_SHIFTED : NEIGHBOR_DR_UNSHIFTED;
		const Vector2 src_pos = nearest_in_pos[cur];
		for (int side = 0; side < 6; side++)
		{
			const int *ni = cell_index.getptr(pack_qr(cqr.x + cdq[side], cqr.y + cdr[side]));
			if (!ni)
				continue;
			const int n_idx = *ni;
			if (cell_ring[n_idx] < 0)
			{
				cell_ring.write[n_idx] = next_ring;
				nearest_in_pos.write[n_idx] = src_pos;
				bfs_queue.push_back(n_idx);
			}
			else if (cell_ring[n_idx] == next_ring)
			{
				const Vector2 n_pos = qr_to_local_2d(all_qrs[n_idx].x, all_qrs[n_idx].y, parity);
				if (n_pos.distance_squared_to(src_pos) < n_pos.distance_squared_to(nearest_in_pos[n_idx]))
					nearest_in_pos.write[n_idx] = src_pos;
			}
		}
	}

	const int cell_count = static_cast<int>(detail_indices.size());
	const int total_points = cell_count * pool_size;

	Vector<float> pat_len;
	pat_len.resize(pool_size);
	for (int pi = 0; pi < pool_size; pi++)
		pat_len.write[pi] = pat_ptr[pi].length();

	TypedArray<Vector2i> out_cell_qrs;
	out_cell_qrs.resize(cell_count);
	PackedByteArray out_cell_out;
	PackedByteArray out_cell_relief;
	out_cell_out.resize(cell_count);
	out_cell_relief.resize(cell_count);
	uint8_t *cell_out_w = out_cell_out.ptrw();
	uint8_t *cell_relief_w = out_cell_relief.ptrw();

	PackedInt32Array p_cell;
	PackedFloat32Array p_x, p_z, p_center, p_border, p_y, p_pick, p_turn, p_size, p_dim, p_bounds_dist;
	PackedInt32Array p_flags;
	PackedColorArray p_color;
	PackedVector3Array p_normal;
	p_cell.resize(total_points);
	p_x.resize(total_points);
	p_z.resize(total_points);
	p_center.resize(total_points);
	p_flags.resize(total_points);
	p_border.resize(total_points);
	p_color.resize(total_points);
	p_y.resize(total_points);
	p_normal.resize(total_points);
	p_pick.resize(total_points);
	p_turn.resize(total_points);
	p_size.resize(total_points);
	p_dim.resize(total_points);
	p_bounds_dist.resize(total_points);

	int32_t *p_cell_w = p_cell.ptrw();
	float *p_x_w = p_x.ptrw();
	float *p_z_w = p_z.ptrw();
	float *p_center_w = p_center.ptrw();
	int32_t *p_flags_w = p_flags.ptrw();
	float *p_border_w = p_border.ptrw();
	Color *p_color_w = p_color.ptrw();
	float *p_y_w = p_y.ptrw();
	Vector3 *p_normal_w = p_normal.ptrw();
	float *p_pick_w = p_pick.ptrw();
	float *p_turn_w = p_turn.ptrw();
	float *p_size_w = p_size.ptrw();
	float *p_dim_w = p_dim.ptrw();
	float *p_bounds_dist_w = p_bounds_dist.ptrw();

	Vector<int32_t> surface_indices;
	const Color empty_color(0.0f, 0.0f, 0.0f, 0.0f);
	auto is_paved = [paved_layer_mask](int layer) -> bool {
		return layer >= 0 && ((paved_layer_mask >> layer) & 1) != 0;
	};

	FastRng rng;
	int idx = 0;
	for (int cell = 0; cell < cell_count; cell++)
	{
		const int ai = detail_indices[cell];
		const Vector2i qr = all_qrs[ai];
		const uint8_t cflags = flags_ptr[ai];
		const bool is_out = (cflags & 2) != 0;
		const bool has_relief = (cflags & 4) != 0;
		const bool has_surface = has_relief || ((cflags & 8) != 0);
		const float cell_elev = elev_ptr[ai];
		out_cell_qrs[cell] = qr;
		cell_out_w[cell] = is_out ? 1 : 0;
		cell_relief_w[cell] = has_relief ? 1 : 0;

		const Vector2 center = qr_to_local_2d(qr.x, qr.y, parity);
		const Color own = grounds_ptr[ai];
		const int own_layer = static_cast<int>(std::round(own.a * 255.0f)) - 1;
		const bool own_paved = is_paved(own_layer);
		bool plain = own_layer >= 0 && !own_paved;
		bool near_paved = own_paved;
		const bool own_in = !is_out;
		int bounds_mask = own_in ? 64 : 0;
		int paved_mask = own_paved ? 64 : 0;

		const bool shifted = (qr.x & 1) == parity;
		const int *dq_tab = shifted ? NEIGHBOR_DQ_SHIFTED : NEIGHBOR_DQ_UNSHIFTED;
		const int *dr_tab = shifted ? NEIGHBOR_DR_SHIFTED : NEIGHBOR_DR_UNSHIFTED;
		for (int side = 0; side < 6; side++)
		{
			const int *ni = cell_index.getptr(pack_qr(qr.x + dq_tab[side], qr.y + dr_tab[side]));
			if (!ni)
			{
				if (own_in)
					bounds_mask |= (1 << side);
				continue;
			}
			const int n_idx = *ni;
			if ((flags_ptr[n_idx] & 2) == 0)
				bounds_mask |= (1 << side);
			const int n_layer = static_cast<int>(std::round(grounds_ptr[n_idx].a * 255.0f)) - 1;
			if (n_layer != own_layer)
				plain = false;
			if (is_paved(n_layer))
			{
				near_paved = true;
				paved_mask |= (1 << side);
			}
		}

		rng.seed(scatter_seed, qr.x, qr.y);
		const float turn = (PI_F / 3.0f) * static_cast<float>(rng.rand_range_0_5());
		const float mirror = rng.randf() < 0.5f ? -1.0f : 1.0f;
		const float turn_cos = std::cos(turn);
		const float turn_sin = std::sin(turn);
		const int own_flags = (own_layer >= 0 && own_layer < layer_flags_n) ? layer_flags[own_layer] : 0;
		const float simple_dim = (bounds_mask == ALL_IN_BOUNDS) ? 1.0f : (bounds_mask == 0 ? oob_dim : -1.0f);

		for (int pi = 0; pi < pool_size; pi++)
		{
			const float lx = pat_ptr[pi].x * mirror;
			const float ly = pat_ptr[pi].y;
			const float px = center.x + lx * turn_cos - ly * turn_sin;
			const float pz = center.y + lx * turn_sin + ly * turn_cos;

			int flags = own_flags;
			float border = NO_BORDER;
			Color color = own;

			if (!plain && p_noise_w > 0 && p_noise_h > 0)
			{
				Vector2 n1 = sample_noise(noise_ptr, p_noise_w, p_noise_h, px * inv_noise_w_scale, pz * inv_noise_w_scale, false);
				Vector2 n2 = sample_noise(noise_ptr, p_noise_w, p_noise_h, px * inv_noise_d_scale + 0.37f, pz * inv_noise_d_scale + 0.61f, true);
				float qx = px + (n1.x * 2.0f - 1.0f) * warp + (n2.x * 2.0f - 1.0f) * warp_detail;
				float qy = pz + (n1.y * 2.0f - 1.0f) * warp + (n2.y * 2.0f - 1.0f) * warp_detail;

				float d1 = 1e30f, d2 = 1e30f;
				Color p1 = empty_color, p2 = empty_color;
				int row0 = static_cast<int>(std::round(qy / ROW_SPACING));
				for (int dr = -1; dr <= 1; dr++)
				{
					int row = row0 + dr;
					float shift = ((row & 1) == parity) ? 0.5f : 0.0f;
					int col0 = static_cast<int>(std::round(qx - shift));
					float dz = qy - static_cast<float>(row) * ROW_SPACING;
					float dz2 = dz * dz;
					for (int dc = -1; dc <= 1; dc++)
					{
						int col = col0 + dc;
						float dx = qx - (static_cast<float>(col) + shift);
						float dist = dx * dx + dz2;
						const int *ni = cell_index.getptr(pack_qr(row, col));
						Color pixel = ni ? grounds_ptr[*ni] : empty_color;
						if (dist < d1)
						{
							d2 = d1;
							p2 = p1;
							d1 = dist;
							p1 = pixel;
						}
						else if (dist < d2)
						{
							d2 = dist;
							p2 = pixel;
						}
					}
				}
				int l1 = static_cast<int>(std::round(p1.a * 255.0f)) - 1;
				int l2 = static_cast<int>(std::round(p2.a * 255.0f)) - 1;
				if (is_paved(l1) || is_paved(l2))
				{
					d1 = 1e30f;
					d2 = 1e30f;
					p1 = empty_color;
					p2 = empty_color;
					for (int dr = -1; dr <= 1; dr++)
					{
						int row = row0 + dr;
						float shift = ((row & 1) == parity) ? 0.5f : 0.0f;
						int col0 = static_cast<int>(std::round(qx - shift));
						float dz = qy - static_cast<float>(row) * ROW_SPACING;
						float dz2 = dz * dz;
						for (int dc = -1; dc <= 1; dc++)
						{
							int col = col0 + dc;
							const int *ni = cell_index.getptr(pack_qr(row, col));
							Color pixel = ni ? grounds_ptr[*ni] : empty_color;
							int pl = static_cast<int>(std::round(pixel.a * 255.0f)) - 1;
							if (is_paved(pl))
								continue;
							float dx = qx - (static_cast<float>(col) + shift);
							float dist = dx * dx + dz2;
							if (dist < d1)
							{
								d2 = d1;
								p2 = p1;
								d1 = dist;
								p1 = pixel;
							}
							else if (dist < d2)
							{
								d2 = dist;
								p2 = pixel;
							}
						}
					}
					l1 = static_cast<int>(std::round(p1.a * 255.0f)) - 1;
					l2 = static_cast<int>(std::round(p2.a * 255.0f)) - 1;
				}
				float inside = (d2 - d1) * 0.5f;
				if (l1 < 0 && l2 >= 0)
				{
					p1 = p2;
					l1 = l2;
					l2 = -1;
				}
				if (l2 == MUD_LAYER && l1 != MUD_LAYER)
					inside -= shore_spread;
				else if (l1 == MUD_LAYER && l2 != MUD_LAYER)
					inside += shore_spread;
				if (inside < 0.0f)
				{
					Color sc = p1;
					p1 = p2;
					p2 = sc;
					int sl = l1;
					l1 = l2;
					l2 = sl;
					inside = -inside;
				}
				flags = (l1 >= 0 && l1 < layer_flags_n) ? layer_flags[l1] : 0;
				border = (l2 >= 0 && l2 != l1) ? inside : NO_BORDER;
				color = p1;
			}

			if (near_paved)
			{
				float dx0 = px - center.x;
				float dz0 = pz - center.y;
				float w6 = std::exp(-(dx0 * dx0 + dz0 * dz0) * inv_2_paved_sq);
				float total = w6;
				float paved = (paved_mask & 64) ? w6 : 0.0f;
				for (int k = 0; k < 6; k++)
				{
					float dkx = dx0 - SIDE_DX[k];
					float dkz = dz0 - SIDE_DZ[k];
					float w = std::exp(-(dkx * dkx + dkz * dkz) * inv_2_paved_sq);
					total += w;
					if ((paved_mask >> k) & 1)
						paved += w;
				}
				float on_path = paved / total;
				if (on_path > paved_edge)
					flags = 0;
				else if (on_path > paved_edge - paved_rim)
					flags = road_edges_flag;
			}

			float dim = simple_dim;
			if (dim < 0.0f)
			{
				float dx0 = px - center.x;
				float dz0 = pz - center.y;
				float w6 = std::exp(-(dx0 * dx0 + dz0 * dz0) * inv_2_bounds_sq);
				float total = w6;
				float inside_w = (bounds_mask & 64) ? w6 : 0.0f;
				for (int k = 0; k < 6; k++)
				{
					float dkx = dx0 - SIDE_DX[k];
					float dkz = dz0 - SIDE_DZ[k];
					float w = std::exp(-(dkx * dkx + dkz * dkz) * inv_2_bounds_sq);
					total += w;
					if ((bounds_mask >> k) & 1)
						inside_w += w;
				}
				dim = (inside_w >= total * 0.5f) ? 1.0f : oob_dim;
			}

			p_cell_w[idx] = cell;
			p_x_w[idx] = px;
			p_z_w[idx] = pz;
			p_center_w[idx] = pat_len[pi];
			p_flags_w[idx] = flags;
			p_border_w[idx] = border;
			p_color_w[idx] = color;
			p_y_w[idx] = cell_elev;
			p_normal_w[idx] = Vector3(0.0f, 1.0f, 0.0f);
			p_pick_w[idx] = rng.randf();
			p_turn_w[idx] = rng.randf() * TAU_F;
			p_size_w[idx] = rng.randf();
			p_dim_w[idx] = dim;
			if (!is_out)
				p_bounds_dist_w[idx] = 0.0f;
			else if (cell_ring[ai] < 0)
				p_bounds_dist_w[idx] = NO_BORDER;
			else
				p_bounds_dist_w[idx] = std::max(0.0f, Vector2(px, pz).distance_to(nearest_in_pos[ai]) - HEX_WIDTH * 0.5f);
			if (has_surface)
				surface_indices.push_back(idx);
			idx++;
		}
	}

	Dictionary result;
	result["cell_qrs"] = out_cell_qrs;
	result["cell_out"] = out_cell_out;
	result["cell_relief"] = out_cell_relief;
	result["point_cell"] = p_cell;
	result["point_x"] = p_x;
	result["point_z"] = p_z;
	result["point_center"] = p_center;
	result["point_flags"] = p_flags;
	result["point_border"] = p_border;
	result["point_color"] = p_color;
	result["point_y"] = p_y;
	result["point_normal"] = p_normal;
	result["point_pick"] = p_pick;
	result["point_turn"] = p_turn;
	result["point_size"] = p_size;
	result["point_dim"] = p_dim;
	result["point_bounds_dist"] = p_bounds_dist;
	result["surface_point_indices"] = to_packed<int32_t, PackedInt32Array>(surface_indices);
	return result;
}

Dictionary BG_BattleBoard3DBuilder::build_scatter_instances(
	const PackedInt32Array &p_point_cell,
	const PackedFloat32Array &p_point_x,
	const PackedFloat32Array &p_point_z,
	const PackedFloat32Array &p_point_center,
	const PackedInt32Array &p_point_flags,
	const PackedFloat32Array &p_point_border,
	const PackedColorArray &p_point_color,
	const PackedFloat32Array &p_point_y,
	const PackedVector3Array &p_point_normal,
	const PackedFloat32Array &p_point_pick,
	const PackedFloat32Array &p_point_turn,
	const PackedFloat32Array &p_point_size,
	const PackedFloat32Array &p_point_dim,
	const PackedByteArray &p_cell_out,
	const PackedByteArray &p_cell_relief,
	const PackedFloat32Array &p_cell_mults,
	const PackedInt32Array &p_cell_chunk_x,
	const PackedInt32Array &p_cell_chunk_z,
	const PackedFloat32Array &p_rates,
	const PackedInt32Array &p_e_layers,
	const PackedFloat32Array &p_e_border_keep,
	const PackedFloat32Array &p_e_scale_min,
	const PackedFloat32Array &p_e_scale_max,
	const PackedByteArray &p_e_tall,
	const PackedByteArray &p_e_follow_slope,
	const Dictionary &p_params)
{
	const int point_count = static_cast<int>(p_point_x.size());
	const int entry_count = static_cast<int>(p_rates.size());
	if (point_count == 0 || entry_count == 0)
		return Dictionary();

	const float min_up = dict_float(p_params, "min_up", 0.0f);
	const float center_clearance = dict_float(p_params, "center_clearance", 0.0f);
	const float tall_clearance = dict_float(p_params, "tall_clearance", 0.0f);
	const float border_width = dict_float(p_params, "border_width", 0.0f);
	const float root_sink = dict_float(p_params, "root_sink", 0.0f);
	const bool smooth_bounds = dict_bool(p_params, "smooth_bounds", true);
	const float oob_dim = dict_float(p_params, "oob_dim", 0.45f);
	const PackedByteArray e_oob_only_arr = p_params.get("e_oob_only", PackedByteArray());
	const PackedFloat32Array e_bounds_margin_arr = p_params.get("e_bounds_margin", PackedFloat32Array());
	const PackedFloat32Array e_min_spacing_arr = p_params.get("e_min_spacing", PackedFloat32Array());
	const PackedFloat32Array p_bounds_dist_arr = p_params.get("point_bounds_dist", PackedFloat32Array());
	const uint8_t *e_oob_only_ptr = (static_cast<int>(e_oob_only_arr.size()) == entry_count) ? e_oob_only_arr.ptr() : nullptr;
	const float *e_margin_ptr = (static_cast<int>(e_bounds_margin_arr.size()) == entry_count) ? e_bounds_margin_arr.ptr() : nullptr;
	const float *e_spacing_ptr = (static_cast<int>(e_min_spacing_arr.size()) == entry_count) ? e_min_spacing_arr.ptr() : nullptr;
	const float *bounds_dist_ptr = (static_cast<int>(p_bounds_dist_arr.size()) == point_count) ? p_bounds_dist_arr.ptr() : nullptr;

	float max_spacing = 0.0f;
	if (e_spacing_ptr)
	{
		for (int e = 0; e < entry_count; e++)
		{
			if (e_spacing_ptr[e] > max_spacing)
				max_spacing = e_spacing_ptr[e];
		}
	}
	const float inv_space_cell = (max_spacing > 0.0f) ? (1.0f / std::max(max_spacing, 0.1f)) : 0.0f;
	struct PlacedSpaced
	{
		float x;
		float z;
		float rad;
	};
	HashMap<uint64_t, Vector<PlacedSpaced>> space_grid;
	auto is_spaced_clear = [&](float px, float pz, float rad) -> bool {
		const int32_t gx = static_cast<int32_t>(std::floor(px * inv_space_cell));
		const int32_t gz = static_cast<int32_t>(std::floor(pz * inv_space_cell));
		for (int32_t dz = -1; dz <= 1; dz++)
		{
			for (int32_t dx = -1; dx <= 1; dx++)
			{
				const Vector<PlacedSpaced> *bucket = space_grid.getptr(pack_qr(gx + dx, gz + dz));
				if (!bucket)
					continue;
				const int bn = static_cast<int>(bucket->size());
				const PlacedSpaced *bp = bucket->ptr();
				for (int bi = 0; bi < bn; bi++)
				{
					const float ddx = px - bp[bi].x;
					const float ddz = pz - bp[bi].z;
					const float min_d = rad + bp[bi].rad;
					if (ddx * ddx + ddz * ddz < min_d * min_d)
						return false;
				}
			}
		}
		return true;
	};

	const int32_t *cell_ptr = p_point_cell.ptr();
	const float *x_ptr = p_point_x.ptr();
	const float *z_ptr = p_point_z.ptr();
	const float *center_ptr = p_point_center.ptr();
	const int32_t *flags_ptr = p_point_flags.ptr();
	const float *border_ptr = p_point_border.ptr();
	const Color *color_ptr = p_point_color.ptr();
	const float *y_ptr = p_point_y.ptr();
	const Vector3 *normal_ptr = p_point_normal.ptr();
	const float *pick_ptr = p_point_pick.ptr();
	const float *turn_ptr = p_point_turn.ptr();
	const float *size_ptr = p_point_size.ptr();
	const float *dim_ptr = p_point_dim.ptr();

	const uint8_t *cell_out_ptr = p_cell_out.ptr();
	const uint8_t *cell_relief_ptr = p_cell_relief.ptr();
	const float *cell_mults_ptr = p_cell_mults.ptr();
	const int32_t *chunk_x_ptr = p_cell_chunk_x.ptr();
	const int32_t *chunk_z_ptr = p_cell_chunk_z.ptr();

	const float *rates_ptr = p_rates.ptr();
	const int32_t *e_layers_ptr = p_e_layers.ptr();
	const float *e_bkeep_ptr = p_e_border_keep.ptr();
	const float *e_smin_ptr = p_e_scale_min.ptr();
	const float *e_smax_ptr = p_e_scale_max.ptr();
	const uint8_t *e_tall_ptr = p_e_tall.ptr();
	const uint8_t *e_slope_ptr = p_e_follow_slope.ptr();

	Vector<float> weights;
	weights.resize(entry_count);
	float *w_ptr = weights.ptrw();

	struct ChunkBuf
	{
		Vector3i key;
		Vector<float> data;
	};
	HashMap<uint64_t, int> chunk_map;
	Vector<ChunkBuf> chunks;

	for (int i = 0; i < point_count; i++)
	{
		const int flags = flags_ptr[i];
		if (flags == 0)
			continue;
		const int cell = cell_ptr[i];
		const float mult = cell_mults_ptr[cell];
		if (mult <= 0.0f)
			continue;
		const bool out = cell_out_ptr[cell] != 0;
		const float from_center = center_ptr[i];
		if (!out && from_center < center_clearance)
			continue;
		const Vector3 normal = normal_ptr[i];
		if (normal.y < min_up)
			continue;
		const float pb = border_ptr[i];
		const float near_border = (pb >= border_width) ? 0.0f : (1.0f - smoothstep_f(0.0f, border_width, pb));
		const float bounds_dist = bounds_dist_ptr ? bounds_dist_ptr[i] : (out ? NO_BORDER : 0.0f);

		float total = 0.0f;
		for (int e = 0; e < entry_count; e++)
		{
			float w = 0.0f;
			if ((e_layers_ptr[e] & flags) != 0)
			{
				const bool oob_only = e_oob_only_ptr && e_oob_only_ptr[e] != 0;
				const bool oob_ok = !oob_only || out;
				const bool margin_ok = !oob_only || !e_margin_ptr || e_margin_ptr[e] <= 0.0f || bounds_dist >= e_margin_ptr[e];
				const bool tall_ok = out || e_tall_ptr[e] == 0 || from_center >= tall_clearance;
				if (oob_ok && margin_ok && tall_ok)
				{
					const float spacing = e_spacing_ptr ? e_spacing_ptr[e] : 0.0f;
					if (spacing <= 0.0f || is_spaced_clear(x_ptr[i], z_ptr[i], spacing * 0.5f))
						w = rates_ptr[e] * mult * (e_bkeep_ptr[e] + (1.0f - e_bkeep_ptr[e]) * near_border);
				}
			}
			w_ptr[e] = w;
			total += w;
		}
		if (total <= 0.0f)
			continue;
		float pick = pick_ptr[i] * (total > 1.0f ? total : 1.0f);
		if (pick >= total)
			continue;
		int chosen = 0;
		while (chosen < entry_count - 1 && pick >= w_ptr[chosen])
		{
			pick -= w_ptr[chosen];
			chosen++;
		}
		if (e_spacing_ptr && e_spacing_ptr[chosen] > 0.0f)
		{
			const int32_t gx = static_cast<int32_t>(std::floor(x_ptr[i] * inv_space_cell));
			const int32_t gz = static_cast<int32_t>(std::floor(z_ptr[i] * inv_space_cell));
			space_grid[pack_qr(gx, gz)].push_back(PlacedSpaced{x_ptr[i], z_ptr[i], e_spacing_ptr[chosen] * 0.5f});
		}

		const int32_t cx = chunk_x_ptr[cell];
		const int32_t cz = chunk_z_ptr[cell];
		const uint64_t ckey = (static_cast<uint64_t>(static_cast<uint16_t>(chosen)) << 48)
			| (static_cast<uint64_t>(static_cast<uint32_t>(cx + 32768) & 0xffffffU) << 24)
			| static_cast<uint64_t>(static_cast<uint32_t>(cz + 32768) & 0xffffffU);
		int *ci = chunk_map.getptr(ckey);
		int chunk_idx;
		if (!ci)
		{
			chunk_idx = static_cast<int>(chunks.size());
			chunk_map.insert(ckey, chunk_idx);
			ChunkBuf cb;
			cb.key = Vector3i(chosen, cx, cz);
			chunks.push_back(cb);
		}
		else
		{
			chunk_idx = *ci;
		}

		const float item_scale = lerpf_c(e_smin_ptr[chosen], e_smax_ptr[chosen], size_ptr[i]);
		const float turn = turn_ptr[i];
		const float y = y_ptr[i] - (cell_relief_ptr[cell] ? root_sink : 0.0f);
		const Color color = color_ptr[i];
		const float dim = smooth_bounds ? dim_ptr[i] : (out ? oob_dim : 1.0f);

		Vector<float> &buf = chunks.write[chunk_idx].data;
		int off = static_cast<int>(buf.size());
		buf.resize(off + 16);
		float *dst = buf.ptrw() + off;
		if (e_slope_ptr[chosen] && normal.y < 0.9999f)
		{
			Basis b = Basis(Quaternion(Vector3(0, 1, 0), normal)) * Basis(Vector3(0, 1, 0), turn).scaled(Vector3(item_scale, item_scale, item_scale));
			dst[0] = b.rows[0].x;
			dst[1] = b.rows[0].y;
			dst[2] = b.rows[0].z;
			dst[3] = x_ptr[i];
			dst[4] = b.rows[1].x;
			dst[5] = b.rows[1].y;
			dst[6] = b.rows[1].z;
			dst[7] = y;
			dst[8] = b.rows[2].x;
			dst[9] = b.rows[2].y;
			dst[10] = b.rows[2].z;
			dst[11] = z_ptr[i];
		}
		else
		{
			float cs = std::cos(turn) * item_scale;
			float sn = std::sin(turn) * item_scale;
			dst[0] = cs;
			dst[1] = 0.0f;
			dst[2] = sn;
			dst[3] = x_ptr[i];
			dst[4] = 0.0f;
			dst[5] = item_scale;
			dst[6] = 0.0f;
			dst[7] = y;
			dst[8] = -sn;
			dst[9] = 0.0f;
			dst[10] = cs;
			dst[11] = z_ptr[i];
		}
		dst[12] = color.r;
		dst[13] = color.g;
		dst[14] = color.b;
		dst[15] = dim;
	}

	Dictionary result;
	for (int i = 0; i < static_cast<int>(chunks.size()); i++)
		result[chunks[i].key] = to_packed<float, PackedFloat32Array>(chunks[i].data);
	return result;
}

Dictionary BG_BattleBoard3DBuilder::build_skirt_mesh(
	const TypedArray<Vector2i> &p_cell_qrs,
	const PackedColorArray &p_cell_grounds,
	const PackedFloat32Array &p_cell_elevations,
	const PackedByteArray &p_cell_flags,
	const PackedInt32Array &p_cell_ramps,
	const Dictionary &p_params)
{
	const int cell_count = static_cast<int>(p_cell_qrs.size());
	if (cell_count == 0)
		return Dictionary();

	const int parity = dict_int(p_params, "parity", 0);
	const float min_elevation = dict_float(p_params, "min_elevation", 0.0f);
	const bool smooth_rim = dict_bool(p_params, "smooth_rim", true);
	const float oob_dim = dict_float(p_params, "oob_dim", 0.45f);

	const Color *grounds_ptr = p_cell_grounds.ptr();
	const float *elev_ptr = p_cell_elevations.ptr();
	const uint8_t *flags_ptr = p_cell_flags.ptr();
	const int32_t *ramps_ptr = p_cell_ramps.ptr();

	Vector<Vector2i> qrs;
	Vector<Vector2> centers;
	qrs.resize(cell_count);
	centers.resize(cell_count);
	HashMap<uint64_t, int> cell_index;
	cell_index.reserve(cell_count * 2);
	bool any_heights = false;
	bool any_ramps = false;
	for (int i = 0; i < cell_count; i++)
	{
		Vector2i qr = p_cell_qrs[i];
		qrs.write[i] = qr;
		centers.write[i] = qr_to_local_2d(qr.x, qr.y, parity);
		cell_index.insert(pack_qr(qr.x, qr.y), i);
		if (elev_ptr[i] != 0.0f)
			any_heights = true;
		if (ramps_ptr[i] >= 0)
			any_ramps = true;
	}

	auto surface_y_at = [&](int ci, const Vector2 &pt) -> float {
		float base_y = elev_ptr[ci];
		int rdir = ramps_ptr[ci];
		if (rdir < 0)
			return base_y;
		Vector2 rel = pt - centers[ci];
		float a = static_cast<float>(rdir) * 60.0f * DEG2RAD_F;
		float along = rel.x * std::cos(a) + rel.y * std::sin(a);
		return base_y + (0.5f + along / HEX_WIDTH) * LEVEL_HEIGHT;
	};

	Vector2 corner_offsets[6];
	for (int s = 0; s < 6; s++)
	{
		float a = (static_cast<float>(s) * 60.0f - 30.0f) * DEG2RAD_F;
		corner_offsets[s] = Vector2(std::cos(a), std::sin(a)) * HEX_RADIUS;
	}

	SkirtBuffers buf;
	Vector<RimEdge> rim_edges;

	for (int ci = 0; ci < cell_count; ci++)
	{
		const Vector2i qr = qrs[ci];
		const Vector2 center = centers[ci];
		const float own_y = elev_ptr[ci];
		const bool own_ramp = ramps_ptr[ci] >= 0;
		bool raised_sides[6] = {false, false, false, false, false, false};
		float top_starts[6] = {}, top_ends[6] = {}, bot_starts[6] = {}, bot_ends[6] = {};
		bool any_raised = false;

		const bool shifted = (qr.x & 1) == parity;
		const int *dq_tab = shifted ? NEIGHBOR_DQ_SHIFTED : NEIGHBOR_DQ_UNSHIFTED;
		const int *dr_tab = shifted ? NEIGHBOR_DR_SHIFTED : NEIGHBOR_DR_UNSHIFTED;

		for (int side = 0; side < 6; side++)
		{
			const int *ni = cell_index.getptr(pack_qr(qr.x + dq_tab[side], qr.y + dr_tab[side]));
			if (!ni)
			{
				RimEdge e;
				e.qr = qr;
				e.center = center;
				e.side = side;
				e.start_key = corner_key(center, static_cast<float>(side) * 60.0f - 30.0f);
				e.end_key = corner_key(center, static_cast<float>(side) * 60.0f + 30.0f);
				rim_edges.push_back(e);
			}
			else if (any_heights || any_ramps)
			{
				const int n_idx = *ni;
				if (!own_ramp && ramps_ptr[n_idx] < 0)
				{
					float ny = elev_ptr[n_idx];
					if (own_y <= ny + 0.01f)
						continue;
					raised_sides[side] = true;
					top_starts[side] = own_y;
					top_ends[side] = own_y;
					bot_starts[side] = ny - INTERIOR_TUCK;
					bot_ends[side] = ny - INTERIOR_TUCK;
					any_raised = true;
				}
				else
				{
					Vector2 p0 = center + corner_offsets[side];
					Vector2 p1 = center + corner_offsets[(side + 1) % 6];
					float y0 = surface_y_at(ci, p0);
					float y1 = surface_y_at(ci, p1);
					float ny0 = surface_y_at(n_idx, p0);
					float ny1 = surface_y_at(n_idx, p1);
					if (y0 > ny0 + 0.01f || y1 > ny1 + 0.01f)
					{
						raised_sides[side] = true;
						top_starts[side] = y0;
						top_ends[side] = y1;
						bot_starts[side] = std::min(y0, ny0) - INTERIOR_TUCK;
						bot_ends[side] = std::min(y1, ny1) - INTERIOR_TUCK;
						any_raised = true;
					}
				}
			}
		}

		if (any_raised)
		{
			const Color ground = grounds_ptr[ci];
			const Color color = ground.a > 0.0f ? Color(ground.r, ground.g, ground.b, 1.0f) : Color(0.36f, 0.29f, 0.2f, 1.0f);
			const float dim = (flags_ptr[ci] & 1) ? oob_dim : 1.0f;
			const bool is_rock_cell = (flags_ptr[ci] & 2) != 0;
			const float backing_rock_flag = is_rock_cell ? 0.3f : 0.0f;
			const bool is_paved = (flags_ptr[ci] & 4) != 0;
			constexpr int INT_ROW_COUNT = 7;

			for (int side = 0; side < 6; side++)
			{
				if (!raised_sides[side])
					continue;
				int prev_side = (side + 5) % 6;
				int next_side = (side + 1) % 6;
				float a0 = static_cast<float>(side) * 60.0f - 30.0f;
				float a1 = static_cast<float>(side) * 60.0f + 30.0f;
				float nr = static_cast<float>(side) * 60.0f * DEG2RAD_F;
				Vector2 normal(std::cos(nr), std::sin(nr));
				Vector2 start = fillet_point(center, a0, 30.0f);
				Vector2 end = fillet_point(center, a1, -30.0f);

				Vector<Vector2> pts, nrms;
				Vector<float> tops, bots;
				float y0 = top_starts[side], y1 = top_ends[side];
				float b0 = bot_starts[side], b1 = bot_ends[side];
				if (raised_sides[prev_side])
				{
					pts.push_back(fillet_point(center, a0, 0.0f));
					nrms.push_back(Vector2(std::cos(a0 * DEG2RAD_F), std::sin(a0 * DEG2RAD_F)));
					tops.push_back(y0);
					bots.push_back(std::min(b0, bot_ends[prev_side]));
				}
				else
				{
					Vector2 tan_dir(-normal.y, normal.x);
					pts.push_back(center + Vector2(std::cos(a0 * DEG2RAD_F), std::sin(a0 * DEG2RAD_F)) * HEX_RADIUS - tan_dir * 0.012f);
					nrms.push_back(normal);
					tops.push_back(y0);
					bots.push_back(b0);
				}
				pts.push_back(start);
				nrms.push_back(normal);
				tops.push_back(y0);
				bots.push_back(b0);
				for (int k = 0; k < SIDE_POINTS; k++)
				{
					float frac = static_cast<float>(k + 1) / static_cast<float>(SIDE_POINTS + 1);
					pts.push_back(start.lerp(end, frac));
					nrms.push_back(normal);
					tops.push_back(lerpf_c(y0, y1, frac));
					bots.push_back(lerpf_c(b0, b1, frac));
				}
				pts.push_back(end);
				nrms.push_back(normal);
				tops.push_back(y1);
				bots.push_back(b1);
				if (raised_sides[next_side])
				{
					pts.push_back(fillet_point(center, a1, 0.0f));
					nrms.push_back(Vector2(std::cos(a1 * DEG2RAD_F), std::sin(a1 * DEG2RAD_F)));
					tops.push_back(y1);
					bots.push_back(std::min(b1, bot_starts[next_side]));
				}
				else
				{
					Vector2 tan_dir(-normal.y, normal.x);
					pts.push_back(center + Vector2(std::cos(a1 * DEG2RAD_F), std::sin(a1 * DEG2RAD_F)) * HEX_RADIUS + tan_dir * 0.012f);
					nrms.push_back(normal);
					tops.push_back(y1);
					bots.push_back(b1);
				}

				int count = static_cast<int>(pts.size());
				int32_t first = static_cast<int32_t>(buf.cliff_verts.size());
				float along = static_cast<float>(side) * 0.6f + static_cast<float>(qr.x) * 1.37f + static_cast<float>(qr.y) * 2.19f;
				for (int i = 0; i < count; i++)
				{
					if (i > 0)
						along += pts[i].distance_to(pts[i - 1]);
					Vector2 out_dir = nrms[i];
					float slope_up = (!is_rock_cell && !is_paved) ? 0.28f : 0.0f;
					Vector3 n3 = Vector3(out_dir.x, slope_up, out_dir.y).normalized();
					float top_i = tops[i];
					float drop = std::max(top_i - bots[i], 0.02f);
					Vector2 lip = pts[i] - out_dir * (LIP_IN + 0.008f);
					buf.add_cliff_vert(Vector3(lip.x, top_i - 0.001f, lip.y), Vector3(0, 1, 0), color, along, 0.0f, dim, backing_rock_flag);
					for (int r = 0; r < 6; r++)
					{
						float depth = INTERIOR_ROWS[r] * drop;
						float wear_env = smoothstep_f(0.05f, 0.2f, depth) * (1.0f - smoothstep_f(std::max(drop - 0.1f, 0.0f), drop, depth));
						float wear = EROSION * 0.6f * wear_env * (skirt_noise(along * 3.1f + depth * 5.3f) * 0.5f + 0.5f);
						float batter = (!is_rock_cell && !is_paved) ? (INTERIOR_ROWS[r] * std::min(drop * 0.24f, 0.11f)) : 0.0f;
						Vector2 p = pts[i] + out_dir * (OUTSET + 0.002f + wear + batter);
						buf.add_cliff_vert(Vector3(p.x, top_i - depth, p.y), n3, color, along, depth, dim, backing_rock_flag);
					}
				}
				for (int i = 0; i < count - 1; i++)
				{
					for (int r = 0; r < INT_ROW_COUNT - 1; r++)
					{
						int32_t a = first + i * INT_ROW_COUNT + r;
						int32_t b = first + (i + 1) * INT_ROW_COUNT + r;
						int32_t c = first + (i + 1) * INT_ROW_COUNT + r + 1;
						int32_t d = first + i * INT_ROW_COUNT + r + 1;
						buf.add_cliff_quad(a, b, c, d, buf.cliff_normals[a] + buf.cliff_normals[c]);
					}
				}
				if (is_paved)
					add_road_masonry(buf, qr, center, side, y0, y1, b0, b1, dim);
				else
					add_cliff_rocks(buf, qr, center, side, y0, y1, b0, b1, dim, is_rock_cell, raised_sides[prev_side], raised_sides[next_side]);
			}
		}
	}

	// Chain rim edges.
	const int edge_count = static_cast<int>(rim_edges.size());
	HashMap<uint64_t, Vector<int>> by_start;
	HashMap<uint64_t, bool> has_incoming;
	for (int i = 0; i < edge_count; i++)
	{
		by_start[rim_edges[i].start_key].push_back(i);
		has_incoming[rim_edges[i].end_key] = true;
	}
	Vector<uint8_t> used;
	used.resize(edge_count);
	for (int i = 0; i < edge_count; i++)
		used.write[i] = 0;

	Vector<int> order;
	for (int i = 0; i < edge_count; i++)
		if (!has_incoming.has(rim_edges[i].start_key))
			order.push_back(i);
	for (int i = 0; i < edge_count; i++)
		if (has_incoming.has(rim_edges[i].start_key))
			order.push_back(i);

	for (int oi = 0; oi < edge_count; oi++)
	{
		int first_e = order[oi];
		if (used[first_e])
			continue;
		Vector<int> chain;
		int cur = first_e;
		while (cur >= 0 && !used[cur])
		{
			used.write[cur] = 1;
			chain.push_back(cur);
			uint64_t end_k = rim_edges[cur].end_key;
			cur = -1;
			Vector<int> *nexts = by_start.getptr(end_k);
			if (nexts)
			{
				for (int ni = 0; ni < static_cast<int>(nexts->size()); ni++)
				{
					int cand = (*nexts)[ni];
					if (!used[cand])
					{
						cur = cand;
						break;
					}
				}
			}
		}
		int chain_len = static_cast<int>(chain.size());
		bool closed = chain_len > 1 && rim_edges[chain[chain_len - 1]].end_key == rim_edges[chain[0]].start_key;

		Vector<Vector2> points, normals;
		Vector<int> cells;
		for (int ei = 0; ei < chain_len; ei++)
		{
			const RimEdge &edge = rim_edges[chain[ei]];
			float a0 = static_cast<float>(edge.side) * 60.0f - 30.0f;
			float a1 = static_cast<float>(edge.side) * 60.0f + 30.0f;
			float nr = static_cast<float>(edge.side) * 60.0f * DEG2RAD_F;
			Vector2 normal(std::cos(nr), std::sin(nr));
			Vector2 start = fillet_point(edge.center, a0, 30.0f);
			Vector2 end = fillet_point(edge.center, a1, -30.0f);
			int ci = *cell_index.getptr(pack_qr(edge.qr.x, edge.qr.y));

			points.push_back(start);
			normals.push_back(normal);
			cells.push_back(ci);
			for (int k = 0; k < SIDE_POINTS; k++)
			{
				float frac = static_cast<float>(k + 1) / static_cast<float>(SIDE_POINTS + 1);
				points.push_back(start.lerp(end, frac));
				normals.push_back(normal);
				cells.push_back(ci);
			}
			points.push_back(end);
			normals.push_back(normal);
			cells.push_back(ci);

			if (ei + 1 < chain_len || closed)
			{
				const RimEdge &next_e = rim_edges[chain[(ei + 1) % chain_len]];
				if (next_e.qr == edge.qr)
				{
					points.push_back(fillet_point(edge.center, a1, 0.0f));
					normals.push_back(Vector2(std::cos(a1 * DEG2RAD_F), std::sin(a1 * DEG2RAD_F)));
					cells.push_back(ci);
				}
			}
		}

		const int count = static_cast<int>(points.size());
		if (count < 2)
			continue;

		Vector<float> along;
		along.resize(count);
		along.write[0] = 0.0f;
		for (int i = 1; i < count; i++)
			along.write[i] = along[i - 1] + points[i].distance_to(points[i - 1]);

		Vector<Vector2> smooth_normals = normals;
		Vector<Vector2> curve = points;
		if (smooth_rim)
		{
			average_v2(normals, closed, SMOOTH_REACH, smooth_normals);
			for (int i = 0; i < count; i++)
				smooth_normals.write[i] = smooth_normals[i].normalized();
			Vector<Vector2> avg1, env1, avg2;
			average_v2(points, closed, SMOOTH_REACH, avg1);
			envelope_v2(avg1, points, smooth_normals, closed, SMOOTH_REACH, env1);
			average_v2(env1, closed, SOFTEN_REACH, avg2);
			envelope_v2(avg2, points, smooth_normals, closed, SOFTEN_REACH, curve);
		}

		constexpr int RIM_ROW_COUNT = 10;
		const int32_t first = static_cast<int32_t>(buf.verts.size());
		Vector<float> tops;
		tops.resize(count);
		for (int i = 0; i < count; i++)
		{
			int ci = cells[i];
			float top = elev_ptr[ci];
			tops.write[i] = top;
			Color ground = grounds_ptr[ci];
			Color color = ground.a > 0.0f ? Color(ground.r, ground.g, ground.b, 1.0f) : Color(0.36f, 0.29f, 0.2f, 1.0f);
			float dim = (flags_ptr[ci] & 1) ? oob_dim : 1.0f;
			Vector2 out_dir = smooth_normals[i];
			Vector3 n3(out_dir.x, 0.0f, out_dir.y);
			float bottom_y = min_elevation - RIM_DEPTH * (1.0f + RAGGED * skirt_noise(along[i] * 0.9f));
			float total_drop = std::max(top - bottom_y, 0.1f);

			Vector2 apron_in = points[i] - normals[i] * APRON_IN;
			buf.apron_verts.push_back(Vector3(apron_in.x, top - APRON_DOWN, apron_in.y));
			buf.apron_verts.push_back(Vector3(curve[i].x, top - APRON_DOWN, curve[i].y));
			buf.apron_colors.push_back(Color(color.r, color.g, color.b, dim));
			buf.apron_colors.push_back(Color(color.r, color.g, color.b, dim));

			Vector2 lip = curve[i] - out_dir * LIP_IN;
			buf.add_vert(Vector3(lip.x, top - LIP_DOWN, lip.y), Vector3(0, 1, 0), color, along[i], 0.0f, dim);
			for (int r = 0; r < 9; r++)
			{
				float depth = RIM_ROWS[r] * total_drop;
				float wear = EROSION * smoothstep_f(0.05f, 0.3f, depth) * (skirt_noise(along[i] * 3.1f + depth * 5.3f) * 0.5f + 0.5f);
				Vector2 p = curve[i] + out_dir * (OUTSET + wear);
				buf.add_vert(Vector3(p.x, top - depth, p.y), n3, color, along[i], depth, dim);
			}
		}

		for (int i = 0; i < count; i++)
		{
			int prev_i = closed ? posmod_i(i - 1, count) : std::max(i - 1, 0);
			int next_i = closed ? posmod_i(i + 1, count) : std::min(i + 1, count - 1);
			Vector3 outward(smooth_normals[i].x, 0.0f, smooth_normals[i].y);
			for (int r = 1; r < RIM_ROW_COUNT; r++)
			{
				int32_t at = first + i * RIM_ROW_COUNT + r;
				Vector3 tangent = buf.verts[first + next_i * RIM_ROW_COUNT + r] - buf.verts[first + prev_i * RIM_ROW_COUNT + r];
				Vector3 down = buf.verts[first + i * RIM_ROW_COUNT + std::min(r + 1, RIM_ROW_COUNT - 1)]
					- buf.verts[first + i * RIM_ROW_COUNT + std::max(r - 1, 1)];
				Vector3 n = tangent.cross(down).normalized();
				if (n.dot(outward) < 0.0f)
					n = -n;
				if (n.length_squared() > 0.5f)
					buf.normals.write[at] = n;
			}
		}

		const int segments = closed ? count : count - 1;
		const int32_t apron_first = static_cast<int32_t>(buf.apron_verts.size()) - count * 2;
		for (int i = 0; i < segments; i++)
		{
			int j = (i + 1) % count;
			if (std::fabs(tops[i] - tops[j]) > 0.001f)
				continue;
			int32_t ia = apron_first + i * 2;
			int32_t ib = apron_first + j * 2;
			float turn = (buf.apron_verts[ib] - buf.apron_verts[ia]).cross(buf.apron_verts[ia + 1] - buf.apron_verts[ia]).y;
			if (turn < 0.0f)
			{
				buf.apron_indices.push_back(ia);
				buf.apron_indices.push_back(ib);
				buf.apron_indices.push_back(ib + 1);
				buf.apron_indices.push_back(ia);
				buf.apron_indices.push_back(ib + 1);
				buf.apron_indices.push_back(ia + 1);
			}
			else
			{
				buf.apron_indices.push_back(ia);
				buf.apron_indices.push_back(ib + 1);
				buf.apron_indices.push_back(ib);
				buf.apron_indices.push_back(ia);
				buf.apron_indices.push_back(ia + 1);
				buf.apron_indices.push_back(ib + 1);
			}
		}

		for (int i = 0; i < segments; i++)
		{
			int j = (i + 1) % count;
			for (int r = 0; r < RIM_ROW_COUNT - 1; r++)
			{
				int32_t a = first + i * RIM_ROW_COUNT + r;
				int32_t b = first + j * RIM_ROW_COUNT + r;
				int32_t c = first + j * RIM_ROW_COUNT + r + 1;
				int32_t d = first + i * RIM_ROW_COUNT + r + 1;
				Vector3 face = (buf.verts[b] - buf.verts[a]).cross(buf.verts[c] - buf.verts[a]);
				Vector3 out_dir = buf.normals[a] + buf.normals[c];
				if (face.dot(out_dir) > 0.0f)
				{
					buf.indices.push_back(a);
					buf.indices.push_back(c);
					buf.indices.push_back(b);
					buf.indices.push_back(a);
					buf.indices.push_back(d);
					buf.indices.push_back(c);
				}
				else
				{
					buf.indices.push_back(a);
					buf.indices.push_back(b);
					buf.indices.push_back(c);
					buf.indices.push_back(a);
					buf.indices.push_back(c);
					buf.indices.push_back(d);
				}
			}
		}
	}

	Dictionary result;
	if (!buf.indices.is_empty())
	{
		Array surface;
		surface.resize(Mesh::ARRAY_MAX);
		surface[Mesh::ARRAY_VERTEX] = to_packed<Vector3, PackedVector3Array>(buf.verts);
		surface[Mesh::ARRAY_NORMAL] = to_packed<Vector3, PackedVector3Array>(buf.normals);
		surface[Mesh::ARRAY_COLOR] = to_packed<Color, PackedColorArray>(buf.colors);
		surface[Mesh::ARRAY_TEX_UV] = to_packed<Vector2, PackedVector2Array>(buf.uv);
		surface[Mesh::ARRAY_TEX_UV2] = to_packed<Vector2, PackedVector2Array>(buf.uv2);
		surface[Mesh::ARRAY_INDEX] = to_packed<int32_t, PackedInt32Array>(buf.indices);
		result["rim"] = surface;
	}
	if (!buf.apron_indices.is_empty())
	{
		Array apron;
		apron.resize(Mesh::ARRAY_MAX);
		PackedVector3Array apron_verts = to_packed<Vector3, PackedVector3Array>(buf.apron_verts);
		PackedVector3Array apron_normals;
		apron_normals.resize(apron_verts.size());
		apron_normals.fill(Vector3(0.0f, 1.0f, 0.0f));
		apron[Mesh::ARRAY_VERTEX] = apron_verts;
		apron[Mesh::ARRAY_NORMAL] = apron_normals;
		apron[Mesh::ARRAY_COLOR] = to_packed<Color, PackedColorArray>(buf.apron_colors);
		apron[Mesh::ARRAY_INDEX] = to_packed<int32_t, PackedInt32Array>(buf.apron_indices);
		result["apron"] = apron;
	}
	if (!buf.cliff_indices.is_empty())
	{
		Array cliffs;
		cliffs.resize(Mesh::ARRAY_MAX);
		cliffs[Mesh::ARRAY_VERTEX] = to_packed<Vector3, PackedVector3Array>(buf.cliff_verts);
		cliffs[Mesh::ARRAY_NORMAL] = to_packed<Vector3, PackedVector3Array>(buf.cliff_normals);
		cliffs[Mesh::ARRAY_COLOR] = to_packed<Color, PackedColorArray>(buf.cliff_colors);
		cliffs[Mesh::ARRAY_TEX_UV] = to_packed<Vector2, PackedVector2Array>(buf.cliff_uv);
		cliffs[Mesh::ARRAY_TEX_UV2] = to_packed<Vector2, PackedVector2Array>(buf.cliff_uv2);
		cliffs[Mesh::ARRAY_INDEX] = to_packed<int32_t, PackedInt32Array>(buf.cliff_indices);
		result["cliffs"] = cliffs;
	}
	return result;
}

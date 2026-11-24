#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

// Native builders for the 3D battle board's scatter pool/instances (BG_BattleBoard3DScatter) and rim/interior cliff
// skirt mesh (BG_BattleBoard3DSkirt). Keeps the per-cell and per-point inner loops out of GDScript.
class BG_BattleBoard3DBuilder : public Object
{
	GDCLASS(BG_BattleBoard3DBuilder, Object);

protected:
	static void _bind_methods();

public:
	static Dictionary build_scatter_pool(
		const TypedArray<Vector2i> &p_cell_qrs,
		const PackedColorArray &p_cell_grounds,
		const PackedByteArray &p_cell_flags,
		const PackedFloat32Array &p_cell_elevations,
		const PackedVector2Array &p_pattern,
		const PackedByteArray &p_noise_bytes,
		int p_noise_w,
		int p_noise_h,
		const Dictionary &p_params);

	static Dictionary build_scatter_instances(
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
		const Dictionary &p_params);

	static Dictionary build_skirt_mesh(
		const TypedArray<Vector2i> &p_cell_qrs,
		const PackedColorArray &p_cell_grounds,
		const PackedFloat32Array &p_cell_elevations,
		const PackedByteArray &p_cell_flags,
		const PackedInt32Array &p_cell_ramps,
		const Dictionary &p_params);
};

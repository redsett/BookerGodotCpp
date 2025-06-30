#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

class BG_Hex : public Object
{
	GDCLASS(BG_Hex, Object);

protected:
    static void _bind_methods();

public:
    int q = 0;
    int get_q() const { return q; }
    void set_q(int v) { q = v; }

    int r = 0;
    int get_r() const { return r; }
    void set_r(int v) { r = v; }

    Vector2i get_coords() const { return Vector2i(r, q); }
    Vector3i get_full_coords() const { return Vector3i(r, q, s); }

    int s = 0;
    
    bool empty = false;
    bool get_empty() const { return empty; }
    void set_empty(bool v) { empty = v; }

    Vector2 location;
    Vector2 get_location() const { return location; }

    bool operator==(const BG_Hex &other) const {
        return q == other.q && r == other.r && s == other.s;
    }

    int get_move_cost(const BG_Hex *to) const {
        return 1;
    }

    int get_hex_cost() const {
        if (get_empty()) return 0;
        return 1;
    }
};

class BG_HexGrid : public Object
{
	GDCLASS(BG_HexGrid, Object);

protected:
	static void _bind_methods();

    Vector2i get_direction_difference(const BG_Hex *hex, Vector2i d) const;

public:
    BG_HexGrid();
    
	enum OffsetType : int32_t {
        ODD_R,
		EVEN_R,
		ODD_Q,
        EVEN_Q
	};
    
	enum HexDirections : int32_t {
        TOP,
		RIGHT,
		BOTTOM,
        BOTTOM_LEFT,
        LEFT,
        TOP_LEFT
	};
    
    HashMap<HexDirections, Vector2i> hex_directions;
    Vector2i get_hex_direction_vec(HexDirections dir) const { return hex_directions[dir]; }

    OffsetType offset_type = EVEN_R;
	OffsetType get_offset_type() const { return offset_type; }
	void set_offset_type(OffsetType v) { offset_type = v; }

    TypedArray<BG_Hex> grid;
    TypedArray<BG_Hex> get_grid() const { return grid; };

    float size_per_hex = 10.0;
    float get_size_per_hex() const { return size_per_hex; }
    void set_size_per_hex(float v) { size_per_hex = v; }

    float offset_between_hexes = 0.0;
    float get_offset_between_hexes() const { return offset_between_hexes; }
    void set_offset_between_hexes(float v) { offset_between_hexes = v; }

    Vector2 get_center_of_hex_location(const BG_Hex *hex) const {
        const float half_size = size_per_hex * 0.5;
        return hex->get_location() + Vector2(half_size, half_size);
    }

    BG_Hex *get_hex_in_direction(const BG_Hex *from_hex, Vector2i d) const;
    HashMap<HexDirections, BG_Hex *> get_hex_neighbors_fast(const BG_Hex *from_hex) const;
    Dictionary get_hex_neighbors(const BG_Hex *from_hex) const;

    BG_Hex *get_hex_by_coords(Vector2i coords) const;

    void add_hex(const BG_Hex *hex);
    void add_row(int column_index, int initial_emptys, int count);

    void update_locations(float x_offset_percent, float y_offset_percent);

    TypedArray<BG_Hex> find_path(BG_Hex *start, BG_Hex *goal);
    bool comp_priority_item(Dictionary a, Dictionary b) const;
};

VARIANT_ENUM_CAST(BG_HexGrid::OffsetType);
VARIANT_ENUM_CAST(BG_HexGrid::HexDirections);
#pragma once

#include "bg_booker_data_base.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

////
//// BG_HexVisualAssetData
////
class BG_HexVisualAssetData : public Resource
{
	GDCLASS(BG_HexVisualAssetData, Resource);

protected:
    static void _bind_methods();

public:
	enum HexVisualAssetTypes : int32_t {
        CITY,
		REST,
		MONSTER_SPAWN,
        WALL,
        SECTION,
        TOWN,
        RESOURCE,
        BAND_SPAWN,
        BARRICADE,
        TURRET,
        NO_STOP_CELL,
        MISC_VISUAL_1,
	};

    static PackedStringArray get_hex_type_names() {
        PackedStringArray result;
        result.append("CITY");
        result.append("REST");
        result.append("MONSTER_SPAWN");
        result.append("WALL");
        result.append("SECTION");
        result.append("TOWN");
        result.append("RESOURCE");
        result.append("BAND_SPAWN");
        result.append("BARRICADE");
        result.append("TURRET");
        result.append("NO_STOP_CELL");
        result.append("MISC_VISUAL_1");
        return result;
    }

    HexVisualAssetTypes hex_type = MONSTER_SPAWN;
    HexVisualAssetTypes get_hex_type() const { return hex_type; }
    void set_hex_type(HexVisualAssetTypes v) { hex_type = v; }

    float rotation = 0.0;
    float get_rotation() const { return rotation; }
    void set_rotation(float v) { rotation = v; }

    float scale_multiplier = 1.0;
    float get_scale_multiplier() const { return scale_multiplier; }
    void set_scale_multiplier(float v) { scale_multiplier = v; }

    int section_index = 0;
    int get_section_index() const { return section_index; }
    void set_section_index(int v) { section_index = v; }

    int seed = 0;
    int get_seed() const { return seed; }
    void set_seed(int v) { seed = v; }

    int seed2 = 0;
    int get_seed2() const { return seed2; }
    void set_seed2(int v) { seed2 = v; }

    float asset_health_normalized_percent = 1.0;
    float get_asset_health_normalized_percent() const { return asset_health_normalized_percent; }
    void set_asset_health_normalized_percent(float v) { asset_health_normalized_percent = v; }

    bool force_disable_targeting = false;
    bool get_force_disable_targeting() const { return force_disable_targeting; }
    void set_force_disable_targeting(bool v) { force_disable_targeting = v; }
};

VARIANT_ENUM_CAST(BG_HexVisualAssetData::HexVisualAssetTypes);

////
//// BG_HexVisualData
////
class BG_HexVisualData : public Resource
{
	GDCLASS(BG_HexVisualData, Resource);

protected:
    static void _bind_methods();

public:
    Vector2i qr = Vector2i();
    Vector2i get_qr() const { return qr; }
    void set_qr(Vector2i v) { qr = v; }

    TypedArray<BG_HexVisualAssetData> hex_asset_datas;
    TypedArray<BG_HexVisualAssetData> get_hex_asset_datas() const { return hex_asset_datas; }
    void set_hex_asset_datas(TypedArray<BG_HexVisualAssetData> v) { hex_asset_datas = v; }
};

////
//// BG_HexGameSaveData
////
class BG_HexGameSaveData : public Resource
{
	GDCLASS(BG_HexGameSaveData, Resource);

protected:
    static void _bind_methods();

public:
	enum HexGameAssetTypes : int32_t {
        BAND,
		JOB,
		CITY,
		TOWN,
		RESOURCE,
		BARRICADE,
		TURRET,
	};

    static PackedStringArray get_game_asset_type_names() {
        PackedStringArray result;
        result.append("BAND");
        result.append("JOB");
        result.append("CITY");
        result.append("TOWN");
        result.append("RESOURCE");
        result.append("BARRICADE");
        result.append("TURRET");
        return result;
    }

    HexGameAssetTypes asset_type = BAND;
    HexGameAssetTypes get_asset_type() const { return asset_type; }
    void set_asset_type(HexGameAssetTypes v) { asset_type = v; }

    int get_asset_type_cost() const {
        if (asset_type == BG_HexGameSaveData::HexGameAssetTypes::BAND ||
            asset_type == BG_HexGameSaveData::HexGameAssetTypes::JOB ||
            asset_type == BG_HexGameSaveData::HexGameAssetTypes::CITY ||
            asset_type == BG_HexGameSaveData::HexGameAssetTypes::TOWN ||
            asset_type == BG_HexGameSaveData::HexGameAssetTypes::BARRICADE ||
            asset_type == BG_HexGameSaveData::HexGameAssetTypes::TURRET ||
            asset_type == BG_HexGameSaveData::HexGameAssetTypes::RESOURCE) {
                return 0;
        }
        return 1;
    }

    bool is_newly_added_to_board = true;
    bool get_is_newly_added_to_board() const { return is_newly_added_to_board; }
    void set_is_newly_added_to_board(bool v) { is_newly_added_to_board = v; }

    bool has_attacked = false;
    bool get_has_attacked() const { return has_attacked; }
    void set_has_attacked(bool v) { has_attacked = v; }

    bool can_move = true;
    bool get_can_move() const { return can_move; }
    void set_can_move(bool v) { can_move = v; }

    Vector2i qr = Vector2i();
    Vector2i get_qr() const { return qr; }
    void set_qr(Vector2i v) { qr = v; }

    bool use_moved_from_qr = false;
    bool get_use_moved_from_qr() const { return use_moved_from_qr; }
    void set_use_moved_from_qr(bool v) { use_moved_from_qr = v; }

    Vector2i moved_from_qr = Vector2i();
    Vector2i get_moved_from_qr() const { return moved_from_qr; }
    void set_moved_from_qr(Vector2i v) { moved_from_qr = v; }

    int unique_id_reference = 0;
    int get_unique_id_reference() const { return unique_id_reference; }
    void set_unique_id_reference(int v) { unique_id_reference = v; }

    bool is_destroyed = false;
    bool get_is_destroyed() const { return is_destroyed; }
    void set_is_destroyed(bool v) { is_destroyed = v; }

    bool has_played_destroyed_vfx = false;
    bool get_has_played_destroyed_vfx() const { return has_played_destroyed_vfx; }
    void set_has_played_destroyed_vfx(bool v) { has_played_destroyed_vfx = v; }

    float asset_health_normalized_percent = 1.0;
    float get_asset_health_normalized_percent() const { return asset_health_normalized_percent; }
    void set_asset_health_normalized_percent(float v) { asset_health_normalized_percent = v; }

    // Objectives
    int objective_unique_id_reference = 0;
    int get_objective_unique_id_reference() const { return objective_unique_id_reference; }
    void set_objective_unique_id_reference(int v) { objective_unique_id_reference = v; }

    bool is_newly_added_objective = false; // Not saved.
    bool get_is_newly_added_objective() const { return is_newly_added_objective; }
    void set_is_newly_added_objective(bool v) { is_newly_added_objective = v; }

    int week_of_last_active_objective = 0;
    int get_week_of_last_active_objective() const { return week_of_last_active_objective; }
    void set_week_of_last_active_objective(int v) { week_of_last_active_objective = v; } 
};

VARIANT_ENUM_CAST(BG_HexGameSaveData::HexGameAssetTypes);

////
//// BG_BattleBoardGameSaveData
////
class BG_BattleBoardGameSaveData : public Resource
{
	GDCLASS(BG_BattleBoardGameSaveData, Resource);

protected:
    static void _bind_methods();

public:
    TypedArray<BG_HexGameSaveData> hex_slots;
    TypedArray<BG_HexGameSaveData> get_hex_slots() const { return hex_slots; }
    void set_hex_slots(TypedArray<BG_HexGameSaveData> v) { hex_slots = v; }
};

////
//// BG_Hex
////
class BG_Hex : public Resource
{
	GDCLASS(BG_Hex, Resource);

protected:
    static void _bind_methods();

public:
    int q = 0;
    int get_q() const { return q; }
    void set_q(int v) { q = v; }

    int r = 0;
    int get_r() const { return r; }
    void set_r(int v) { r = v; }

    int s = 0;

    Vector2i get_qr() const { return Vector2i(q, r); }
    Vector3i get_full_qr() const { return Vector3i(q, r, s); }
    Vector2i get_coords() const { return Vector2i(r, q); }
    Vector3i get_full_coords() const { return Vector3i(r, q, s); }
    
    bool empty = false;
    bool get_empty() const { return empty; }
    void set_empty(bool v) { empty = v; }

    Vector2 location;
    Vector2 get_location() const { return location; }
};

////
//// BG_HexGrid
////
class BG_HexGrid : public Object
{
	GDCLASS(BG_HexGrid, Object);

protected:
	static void _bind_methods();

    float x_offset_percent = 0.0;
    float y_offset_percent = 0.0;

    Vector2i get_direction_difference(const Ref<BG_Hex> hex, Vector2i d) const;

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
    
    HashMap<Vector2i, Ref<BG_Hex>> grid_map;
    TypedArray<BG_Hex> grid;
    TypedArray<BG_Hex> get_grid() const { return grid; };
    void clear_grid();
    
    HashMap<Vector2i, Ref<BG_HexVisualData>> base_grid_visual_data;
    Dictionary get_base_grid_visual_data() const {
        Dictionary result;
        for (const auto &pair : base_grid_visual_data) {
            result[pair.key] = pair.value;
        }
        return result;
    }
	void set_base_grid_visual_data(Vector2i qr, Ref<BG_HexVisualData> v) {
        base_grid_visual_data[qr] = v;
    }
    
    TypedArray<BG_HexGameSaveData> game_data;
    TypedArray<BG_HexGameSaveData> get_game_data() const { return game_data; };
	void set_game_data(TypedArray<BG_HexGameSaveData> v) { game_data = v; }
    Ref<BG_HexGameSaveData> get_game_data_from_qr(Vector2i qr) const {
        for (int i = 0; i < game_data.size(); ++i) {
            const Ref<BG_HexGameSaveData> h = game_data[i];
            if (h.is_null()) continue;
            if (h->get_qr() != qr) continue;
            return h;
        }
        return nullptr;
    }

    int get_hex_cost(const Ref<BG_Hex> from_hex, Vector2i qr, bool do_pass_through_check) const;
    
    float size_per_hex = 10.0;
    float get_size_per_hex() const { return size_per_hex; }
    void set_size_per_hex(float v) { size_per_hex = v; }

    float offset_between_hexes = 0.0;
    float get_offset_between_hexes() const { return offset_between_hexes; }
    void set_offset_between_hexes(float v) { offset_between_hexes = v; }

    Vector2 get_center_of_hex_location(const Ref<BG_Hex> hex) const {
        const Vector2 half_size = Vector2(
            size_per_hex * 0.5,// * (1.0 - x_offset_percent), 
            size_per_hex * 0.5// * (1.0 - y_offset_percent)
        );
        return hex->get_location() + half_size;
    }

    int get_disance_between_hexes(const Ref<BG_Hex> from_hex, const Ref<BG_Hex> to_hex);

    Ref<BG_Hex> get_hex_in_direction(const Ref<BG_Hex> from_hex, Vector2i d) const;
    inline HashMap<HexDirections, Ref<BG_Hex>> get_hex_neighbors_fast(const Ref<BG_Hex> from_hex) const;
    Dictionary get_hex_neighbors_directions(const Ref<BG_Hex> from_hex) const;
    Dictionary get_hex_neighbors_qr(const Ref<BG_Hex> instigator, const Ref<BG_Hex> from_hex, int cell_distance, bool do_pathing_checks) const;

    Ref<BG_Hex> get_hex_by_coords(Vector2i coords) const;
    Ref<BG_Hex> get_hex_by_qr(Vector2i qr) const;

    void add_hex(const Ref<BG_Hex> hex);
    void add_row(int column_index, int initial_emptys, int count);

    void update_locations(float x_offset_percent, float y_offset_percent);

    Ref<BG_HexGameSaveData> get_nearest_job_attackable(const Ref<BG_Hex> from_job_hex, const TypedArray<int> attackable_types, const TypedArray<BG_Band> bands) const;
    Ref<BG_Hex> get_nearest_empty_cell_neighoring_target(const Ref<BG_Hex> instigator, const Ref<BG_Hex> target) const;
    Ref<BG_Hex> get_nearest_empty_cell(const Ref<BG_Hex> instigator, const Ref<BG_Hex> target, const TypedArray<BG_Hex> cells_to_check) const;

    TypedArray<BG_Hex> find_path(const Ref<BG_Hex> instigator, const Ref<BG_Hex> start, const Ref<BG_Hex> goal, bool include_start, int travel_distance) const;
    TypedArray<BG_Hex> find_reachable_cells_in_distance(const Ref<BG_Hex> instigator, const Ref<BG_Hex> start, int distance) const;
    bool comp_priority_item(Dictionary a, Dictionary b) const;
};

VARIANT_ENUM_CAST(BG_HexGrid::OffsetType);
VARIANT_ENUM_CAST(BG_HexGrid::HexDirections);
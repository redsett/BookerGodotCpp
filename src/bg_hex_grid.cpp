#include "bg_hex_grid.hpp"
#include <queue>

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

////
//// BG_HexVisualAssetData
////
void BG_HexVisualAssetData::_bind_methods()
{
    ClassDB::bind_static_method("BG_HexVisualAssetData", D_METHOD("get_hex_type_names"), &BG_HexVisualAssetData::get_hex_type_names);

	ClassDB::bind_method(D_METHOD("get_hex_type"), &BG_HexVisualAssetData::get_hex_type);
	ClassDB::bind_method(D_METHOD("set_hex_type"), &BG_HexVisualAssetData::set_hex_type);
	ClassDB::bind_method(D_METHOD("get_rotation"), &BG_HexVisualAssetData::get_rotation);
	ClassDB::bind_method(D_METHOD("set_rotation"), &BG_HexVisualAssetData::set_rotation);
	ClassDB::bind_method(D_METHOD("get_scale_multiplier"), &BG_HexVisualAssetData::get_scale_multiplier);
	ClassDB::bind_method(D_METHOD("set_scale_multiplier"), &BG_HexVisualAssetData::set_scale_multiplier);
	ClassDB::bind_method(D_METHOD("get_section_index"), &BG_HexVisualAssetData::get_section_index);
	ClassDB::bind_method(D_METHOD("set_section_index"), &BG_HexVisualAssetData::set_section_index);
	ClassDB::bind_method(D_METHOD("get_seed"), &BG_HexVisualAssetData::get_seed);
	ClassDB::bind_method(D_METHOD("set_seed"), &BG_HexVisualAssetData::set_seed);
	ClassDB::bind_method(D_METHOD("get_asset_health_normalized_percent"), &BG_HexVisualAssetData::get_asset_health_normalized_percent);
	ClassDB::bind_method(D_METHOD("set_asset_health_normalized_percent"), &BG_HexVisualAssetData::set_asset_health_normalized_percent);
	ClassDB::bind_method(D_METHOD("get_force_disable_targeting"), &BG_HexVisualAssetData::get_force_disable_targeting);
	ClassDB::bind_method(D_METHOD("set_force_disable_targeting"), &BG_HexVisualAssetData::set_force_disable_targeting);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "hex_type", PROPERTY_HINT_ENUM, 
        "CITY:0,REST:1,MONSTER_SPAWN:2,WALL:3,SECTION:4,TOWN:5,RESOURCE:6,BAND_SPAWN:7,BARRICADE:8,TURRET:9,NO_STOP_CELL:10,MISC_VISUAL_1:11"), 
        "set_hex_type", "get_hex_type");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation"), "set_rotation", "get_rotation");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "scale_multiplier"), "set_scale_multiplier", "get_scale_multiplier");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "section_index"), "set_section_index", "get_section_index");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "asset_health_normalized_percent"), "set_asset_health_normalized_percent", "get_asset_health_normalized_percent");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "force_disable_targeting"), "set_force_disable_targeting", "get_force_disable_targeting");

	BIND_ENUM_CONSTANT(CITY);
	BIND_ENUM_CONSTANT(REST);
	BIND_ENUM_CONSTANT(MONSTER_SPAWN);
	BIND_ENUM_CONSTANT(WALL);
	BIND_ENUM_CONSTANT(SECTION);
	BIND_ENUM_CONSTANT(TOWN);
	BIND_ENUM_CONSTANT(RESOURCE);
	BIND_ENUM_CONSTANT(BAND_SPAWN);
	BIND_ENUM_CONSTANT(BARRICADE);
	BIND_ENUM_CONSTANT(TURRET);
	BIND_ENUM_CONSTANT(NO_STOP_CELL);
	BIND_ENUM_CONSTANT(MISC_VISUAL_1);
}

////
//// BG_HexVisualData
////
void BG_HexVisualData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_qr"), &BG_HexVisualData::get_qr);
	ClassDB::bind_method(D_METHOD("set_qr"), &BG_HexVisualData::set_qr);
	ClassDB::bind_method(D_METHOD("get_hex_asset_datas"), &BG_HexVisualData::get_hex_asset_datas);
	ClassDB::bind_method(D_METHOD("set_hex_asset_datas"), &BG_HexVisualData::set_hex_asset_datas);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "qr"), "set_qr", "get_qr");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "hex_asset_datas"), "set_hex_asset_datas", "get_hex_asset_datas");
}

////
//// BG_HexGameSaveData
////
void BG_HexGameSaveData::_bind_methods()
{
    ClassDB::bind_static_method("BG_HexGameSaveData", D_METHOD("get_game_asset_type_names"), &BG_HexGameSaveData::get_game_asset_type_names);

	ClassDB::bind_method(D_METHOD("get_asset_type"), &BG_HexGameSaveData::get_asset_type);
	ClassDB::bind_method(D_METHOD("set_asset_type"), &BG_HexGameSaveData::set_asset_type);
	ClassDB::bind_method(D_METHOD("get_asset_type_cost"), &BG_HexGameSaveData::get_asset_type_cost);
	ClassDB::bind_method(D_METHOD("get_is_newly_added_to_board"), &BG_HexGameSaveData::get_is_newly_added_to_board);
	ClassDB::bind_method(D_METHOD("set_is_newly_added_to_board"), &BG_HexGameSaveData::set_is_newly_added_to_board);
	ClassDB::bind_method(D_METHOD("get_has_attacked"), &BG_HexGameSaveData::get_has_attacked);
	ClassDB::bind_method(D_METHOD("set_has_attacked"), &BG_HexGameSaveData::set_has_attacked);
	ClassDB::bind_method(D_METHOD("get_can_move"), &BG_HexGameSaveData::get_can_move);
	ClassDB::bind_method(D_METHOD("set_can_move"), &BG_HexGameSaveData::set_can_move);
	ClassDB::bind_method(D_METHOD("get_qr"), &BG_HexGameSaveData::get_qr);
	ClassDB::bind_method(D_METHOD("set_qr"), &BG_HexGameSaveData::set_qr);
	ClassDB::bind_method(D_METHOD("get_use_moved_from_qr"), &BG_HexGameSaveData::get_use_moved_from_qr);
	ClassDB::bind_method(D_METHOD("set_use_moved_from_qr"), &BG_HexGameSaveData::set_use_moved_from_qr);
	ClassDB::bind_method(D_METHOD("get_moved_from_qr"), &BG_HexGameSaveData::get_moved_from_qr);
	ClassDB::bind_method(D_METHOD("set_moved_from_qr"), &BG_HexGameSaveData::set_moved_from_qr);
	ClassDB::bind_method(D_METHOD("get_unique_id_reference"), &BG_HexGameSaveData::get_unique_id_reference);
	ClassDB::bind_method(D_METHOD("set_unique_id_reference"), &BG_HexGameSaveData::set_unique_id_reference);
	ClassDB::bind_method(D_METHOD("get_is_destroyed"), &BG_HexGameSaveData::get_is_destroyed);
	ClassDB::bind_method(D_METHOD("set_is_destroyed"), &BG_HexGameSaveData::set_is_destroyed);
	ClassDB::bind_method(D_METHOD("get_asset_health_normalized_percent"), &BG_HexGameSaveData::get_asset_health_normalized_percent);
	ClassDB::bind_method(D_METHOD("set_asset_health_normalized_percent"), &BG_HexGameSaveData::set_asset_health_normalized_percent);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "asset_type", PROPERTY_HINT_ENUM, "BAND:0,JOB:1,CITY:2,TOWN:3,RESOURCE:4,BARRICADE:5,TURRET:6"), "set_asset_type", "get_asset_type");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_newly_added_to_board"), "set_is_newly_added_to_board", "get_is_newly_added_to_board");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_attacked"), "set_has_attacked", "get_has_attacked");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "can_move"), "set_can_move", "get_can_move");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "qr"), "set_qr", "get_qr");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_moved_from_qr"), "set_use_moved_from_qr", "get_use_moved_from_qr");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "moved_from_qr"), "set_moved_from_qr", "get_moved_from_qr");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_id_reference"), "set_unique_id_reference", "get_unique_id_reference");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_destroyed"), "set_is_destroyed", "get_is_destroyed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "asset_health_normalized_percent"), "set_asset_health_normalized_percent", "get_asset_health_normalized_percent");

    BIND_ENUM_CONSTANT(BAND);
    BIND_ENUM_CONSTANT(JOB);
    BIND_ENUM_CONSTANT(CITY);
    BIND_ENUM_CONSTANT(TOWN);
    BIND_ENUM_CONSTANT(RESOURCE);
    BIND_ENUM_CONSTANT(BARRICADE);
    BIND_ENUM_CONSTANT(TURRET);
}

////
//// BG_BattleBoardGameSaveData
////
void BG_BattleBoardGameSaveData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_hex_slots"), &BG_BattleBoardGameSaveData::get_hex_slots);
	ClassDB::bind_method(D_METHOD("set_hex_slots"), &BG_BattleBoardGameSaveData::set_hex_slots);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "hex_slots"), "set_hex_slots", "get_hex_slots");
}

////
//// BG_Hex
////
void BG_Hex::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_q"), &BG_Hex::get_q);
	ClassDB::bind_method(D_METHOD("set_q"), &BG_Hex::set_q);
	ClassDB::bind_method(D_METHOD("get_r"), &BG_Hex::get_r);
	ClassDB::bind_method(D_METHOD("set_r"), &BG_Hex::set_r);
	ClassDB::bind_method(D_METHOD("get_qr"), &BG_Hex::get_qr);
	ClassDB::bind_method(D_METHOD("get_empty"), &BG_Hex::get_empty);
	ClassDB::bind_method(D_METHOD("set_empty"), &BG_Hex::set_empty);
	ClassDB::bind_method(D_METHOD("get_location"), &BG_Hex::get_location);
}

////
//// BG_HexGrid
////
void BG_HexGrid::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_offset_type"), &BG_HexGrid::get_offset_type);
	ClassDB::bind_method(D_METHOD("set_offset_type"), &BG_HexGrid::set_offset_type);
	ClassDB::bind_method(D_METHOD("get_grid"), &BG_HexGrid::get_grid);
	ClassDB::bind_method(D_METHOD("clear_grid"), &BG_HexGrid::clear_grid);
	ClassDB::bind_method(D_METHOD("get_base_grid_visual_data"), &BG_HexGrid::get_base_grid_visual_data);
	ClassDB::bind_method(D_METHOD("set_base_grid_visual_data", "qr", "data"), &BG_HexGrid::set_base_grid_visual_data);
	ClassDB::bind_method(D_METHOD("get_game_data"), &BG_HexGrid::get_game_data);
	ClassDB::bind_method(D_METHOD("set_game_data"), &BG_HexGrid::set_game_data);
	ClassDB::bind_method(D_METHOD("get_game_data_from_qr", "qr"), &BG_HexGrid::get_game_data_from_qr);
	ClassDB::bind_method(D_METHOD("get_hex_cost", "from_hex", "qr", "do_friendly_check"), &BG_HexGrid::get_hex_cost);
	ClassDB::bind_method(D_METHOD("get_size_per_hex"), &BG_HexGrid::get_size_per_hex);
	ClassDB::bind_method(D_METHOD("set_size_per_hex"), &BG_HexGrid::set_size_per_hex);
	ClassDB::bind_method(D_METHOD("get_offset_between_hexes"), &BG_HexGrid::get_offset_between_hexes);
	ClassDB::bind_method(D_METHOD("set_offset_between_hexes"), &BG_HexGrid::set_offset_between_hexes);
	ClassDB::bind_method(D_METHOD("get_center_of_hex_location"), &BG_HexGrid::get_center_of_hex_location);
	ClassDB::bind_method(D_METHOD("get_hex_direction_vec", "direction"), &BG_HexGrid::get_hex_direction_vec);
	ClassDB::bind_method(D_METHOD("get_hex_in_direction", "from_hex", "direction"), &BG_HexGrid::get_hex_in_direction);
	ClassDB::bind_method(D_METHOD("get_hex_neighbors_directions", "from_hex"), &BG_HexGrid::get_hex_neighbors_directions);
	ClassDB::bind_method(D_METHOD("get_hex_neighbors_qr", "instigator", "from_hex", "cell_distance", "do_pathing_checks"), &BG_HexGrid::get_hex_neighbors_qr);
	ClassDB::bind_method(D_METHOD("get_hex_by_coords", "coords"), &BG_HexGrid::get_hex_by_coords);
	ClassDB::bind_method(D_METHOD("get_hex_by_qr", "qr"), &BG_HexGrid::get_hex_by_qr);
	ClassDB::bind_method(D_METHOD("add_hex", "hex"), &BG_HexGrid::add_hex);
	ClassDB::bind_method(D_METHOD("add_row", "column_index", "initial_emptys", "count"), &BG_HexGrid::add_row);
	ClassDB::bind_method(D_METHOD("update_locations", "x_offset_percent", "y_offset_percent"), &BG_HexGrid::update_locations);
	ClassDB::bind_method(D_METHOD("get_nearest_job_attackable", "from_job_hex", "attackable_types"), &BG_HexGrid::get_nearest_job_attackable);
	ClassDB::bind_method(D_METHOD("find_path", "instigator", "start", "goal", "include_start"), &BG_HexGrid::find_path);
	ClassDB::bind_method(D_METHOD("comp_priority_item"), &BG_HexGrid::comp_priority_item);

	BIND_ENUM_CONSTANT(ODD_R);
	BIND_ENUM_CONSTANT(EVEN_R);
	BIND_ENUM_CONSTANT(ODD_Q);
	BIND_ENUM_CONSTANT(EVEN_Q);

    BIND_ENUM_CONSTANT(TOP);
    BIND_ENUM_CONSTANT(RIGHT);
    BIND_ENUM_CONSTANT(BOTTOM);
    BIND_ENUM_CONSTANT(BOTTOM_LEFT);
    BIND_ENUM_CONSTANT(LEFT);
    BIND_ENUM_CONSTANT(TOP_LEFT);
}

BG_HexGrid::BG_HexGrid()
{
    hex_directions[HexDirections::TOP] = Vector2i(0, -1);
    hex_directions[HexDirections::RIGHT] = Vector2i(1, 0);
    hex_directions[HexDirections::BOTTOM] = Vector2i(0, 1);
    hex_directions[HexDirections::BOTTOM_LEFT] = Vector2i(-1, 1);
    hex_directions[HexDirections::LEFT] = Vector2i(-1, 0);
    hex_directions[HexDirections::TOP_LEFT] = Vector2i(-1, -1);
}

void BG_HexGrid::clear_grid()
{
    grid.clear();
    grid_map.clear();
}

Vector2i BG_HexGrid::get_direction_difference(const Ref<BG_Hex> hex, Vector2i d) const
{
    if (hex.is_null()) return Vector2i(0, 0);
    const bool column_is_odd = hex->q & 1; // Bitwise instead of modulo to support negative numbers.
    const bool row_is_odd = hex->r & 1; // Bitwise instead of modulo to support negative numbers.
    switch (offset_type) {
        case ODD_R:
            if (column_is_odd) {
                if (d == Vector2i(0, -1)) return Vector2i(1, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, 0);
                if (d == Vector2i(0, 1)) return Vector2i(1, 1);
                if (d == Vector2i(-1, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 0);
                if (d == Vector2i(-1, -1)) return Vector2i(0, -1);
            } else {
                return d;
            }
        case EVEN_R:
            if (!column_is_odd) {
                if (d == Vector2i(0, -1)) return Vector2i(1, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, 0);
                if (d == Vector2i(0, 1)) return Vector2i(1, 1);
                if (d == Vector2i(-1, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 0);
                if (d == Vector2i(-1, -1)) return Vector2i(0, -1);
            } else {
                if (d == Vector2i(0, -1)) return Vector2i(0, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, 0);
                if (d == Vector2i(0, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 1)) return Vector2i(-1, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 0);
                if (d == Vector2i(-1, -1)) return Vector2i(-1, -1);
            }
        case ODD_Q:
            if (row_is_odd) {
                if (d == Vector2i(0, -1)) return Vector2i(0, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, 0);
                if (d == Vector2i(0, 1)) return Vector2i(1, 1);
                if (d == Vector2i(-1, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 1);
                if (d == Vector2i(-1, -1)) return Vector2i(-1, 0);
            } else {
                if (d == Vector2i(0, -1)) return Vector2i(0, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, -1);
                if (d == Vector2i(0, 1)) return Vector2i(1, 0);
                if (d == Vector2i(-1, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 0);
                if (d == Vector2i(-1, -1)) return Vector2i(-1, -1);
            }
        case EVEN_Q:
            if (!row_is_odd) {
                if (d == Vector2i(0, -1)) return Vector2i(0, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, 0);
                if (d == Vector2i(0, 1)) return Vector2i(1, 1);
                if (d == Vector2i(-1, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 1);
                if (d == Vector2i(-1, -1)) return Vector2i(-1, 0);
            } else {
                if (d == Vector2i(0, -1)) return Vector2i(0, -1);
                if (d == Vector2i(1, 0)) return Vector2i(1, -1);
                if (d == Vector2i(0, 1)) return Vector2i(1, 0);
                if (d == Vector2i(-1, 1)) return Vector2i(0, 1);
                if (d == Vector2i(-1, 0)) return Vector2i(-1, 0);
                if (d == Vector2i(-1, -1)) return Vector2i(-1, -1);
            }
    }
    return d;
}

Ref<BG_Hex> BG_HexGrid::get_hex_by_coords(Vector2i coords) const
{
	for (int i = 0; i < grid.size(); ++i)
	{
		Ref<BG_Hex> h = grid[i];
        if (coords == h->get_coords())
            return h;
    }
    return nullptr;
}

Ref<BG_Hex> BG_HexGrid::get_hex_by_qr(Vector2i qr) const
{
    return grid_map[qr];
}

Ref<BG_Hex> BG_HexGrid::get_hex_in_direction(const Ref<BG_Hex> from_hex, Vector2i d) const
{
    const Vector2i direction_diff = get_direction_difference(from_hex, d);
    const Vector2i v = direction_diff + from_hex->get_coords();
	for (int i = 0; i < grid.size(); ++i)
	{
		Ref<BG_Hex> h = grid[i];
        if (h->q == v.y && h->r == v.x) {
            // if (h->get_empty()) return nullptr;
            return h;
        }
    }
    return nullptr;
}

HashMap<BG_HexGrid::HexDirections, Ref<BG_Hex>> BG_HexGrid::get_hex_neighbors_fast(const Ref<BG_Hex> from_hex) const
{
    HashMap<HexDirections, Ref<BG_Hex>> result;
    result[BG_HexGrid::HexDirections::TOP] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::TOP]);
    result[BG_HexGrid::HexDirections::RIGHT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::RIGHT]);
    result[BG_HexGrid::HexDirections::BOTTOM] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::BOTTOM]);
    result[BG_HexGrid::HexDirections::BOTTOM_LEFT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::BOTTOM_LEFT]);
    result[BG_HexGrid::HexDirections::LEFT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::LEFT]);
    result[BG_HexGrid::HexDirections::TOP_LEFT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::TOP_LEFT]);
    return result;
}

Dictionary BG_HexGrid::get_hex_neighbors_directions(const Ref<BG_Hex> from_hex) const
{
    Dictionary result;
    const HashMap<BG_HexGrid::HexDirections, Ref<BG_Hex>> neighbors = get_hex_neighbors_fast(from_hex);
    for (const auto &pair : neighbors) {
        result[pair.key] = pair.value;
    }
    return result;
}

Dictionary BG_HexGrid::get_hex_neighbors_qr(const Ref<BG_Hex> instigator, const Ref<BG_Hex> from_hex, int cell_distance, bool do_pathing_checks) const
{
    if (cell_distance < 1 || from_hex.is_null()) return {};

    Dictionary result;
    if (do_pathing_checks)
    {
        TypedArray<BG_Hex> cells = find_reachable_cells_in_distance(instigator, from_hex, cell_distance);

        for (uint16_t i = 0; i < cells.size(); ++i) {
            const Ref<BG_Hex> h = cells[i];
            if (h->get_empty()) continue;
            result[h->get_qr()] = h;
        }
        return result;
    }

    const HashMap<BG_HexGrid::HexDirections, Ref<BG_Hex>> neighbors = get_hex_neighbors_fast(from_hex);
    for (const auto &pair : neighbors) {
        if (pair.value != nullptr) {
            result[pair.value->get_qr()] = pair.value;
        }
    }

    if (cell_distance > 1) {
        for (uint8_t i = 1; i < cell_distance; ++i) {
            const TypedArray<Vector2i> keys = result.keys();
            for (uint16_t v = 0; v < keys.size(); ++v)
            {
                const Ref<BG_Hex> h = result[keys[v]];
                result.merge(get_hex_neighbors_qr(instigator, h, 1, false));
            }
        }

        // Remove from_hex from the result.
        for (uint16_t k = 0; k < result.keys().size(); ++k)
        {
            const Ref<BG_Hex> h = result[result.keys()[k]];
            if (h == from_hex) {
                const Vector2i key = result.keys()[k];
                result.erase(key);
                break;
            }
        }
    }

    return result;
}

void BG_HexGrid::add_hex(const Ref<BG_Hex> hex)
{
    grid_map[hex->get_qr()] = hex;

    if (hex.is_null()) return;
    if (grid.is_empty()) {
        grid.append(hex);
        return;
    }

	for (int i = 0; i < grid.size(); ++i)
	{
		const Ref<BG_Hex> h = grid[i];
        if (h->q > hex->q) {
            grid.insert(i, hex);
            return;
        }
        else if (h->q == hex->q) {
            if (h->r > hex->r) {
                grid.insert(i, hex);
                return;
            }
        }
    }

    // Didn't find a place to insert, so append at the end.
    grid.append(hex);
}

void BG_HexGrid::add_row(int column_index, int initial_emptys, int count)
{
    for (int i = 0; i < count + initial_emptys; ++i)
    {
        Ref<BG_Hex> new_hex = memnew(BG_Hex);
        new_hex->q = column_index;
        new_hex->r = i;
        new_hex->s = (-new_hex->q - new_hex->r);
        new_hex->empty = i < initial_emptys;
        // add_hex(new_hex);
        grid_map[new_hex->get_qr()] = new_hex;
        grid.append(new_hex);
    }
}

void BG_HexGrid::update_locations(float p_x_offset_percent, float p_y_offset_percent)
{
    x_offset_percent = p_x_offset_percent;
    y_offset_percent = p_y_offset_percent;
	for (int i = 0; i < grid.size(); ++i)
	{
		Ref<BG_Hex> h = grid[i];
        const float x_s = size_per_hex * (1.0 - x_offset_percent);
        const float y_s = size_per_hex * (1.0 - y_offset_percent);
        h->location.x = (float(h->r) * x_s) + (float(h->r) * offset_between_hexes);
        h->location.y = (float(h->q) * y_s) + (float(h->q) * offset_between_hexes);

        const bool column_is_odd = h->q & 1; // Bitwise instead of modulo to support negative numbers.
        const bool row_is_odd = h->r & 1; // Bitwise instead of modulo to support negative numbers.
        switch (offset_type) {
            case ODD_R:
                if (column_is_odd)
                    h->location.x += ((x_s + offset_between_hexes) * 0.5);
                break;
            case EVEN_R:
                if (!column_is_odd)
                    h->location.x += ((x_s + offset_between_hexes) * 0.5);
                break;
            case ODD_Q:
                if (row_is_odd)
                    h->location.y += ((y_s + offset_between_hexes) * 0.5);
                break;
            case EVEN_Q:
                if (!row_is_odd)
                    h->location.y += ((y_s + offset_between_hexes) * 0.5);
                break;
        }
    }
}

inline static Dictionary make_priority_item(const Ref<BG_Hex> h, int priority)
{
    Dictionary result;
    result["h"] = h;
    result["p"] = priority;
    return result;
}

bool BG_HexGrid::comp_priority_item(Dictionary a, Dictionary b) const {
	return int(a["p"]) < int(b["p"]);
}

inline static Vector3i offset_to_cube(const Vector3i &a, BG_HexGrid::OffsetType offset_type)
{
    int q, r, s;
    if (offset_type == BG_HexGrid::OffsetType::ODD_R) {
        q = a.x;
        r = a.y - (a.x - (a.x & 1)) / 2;
        s = -q - r;
    } else if (offset_type == BG_HexGrid::OffsetType::EVEN_R) {
        q = a.x;
        r = a.y - (a.x + (a.x & 1)) / 2;
        s = -q - r;
    } else if (offset_type == BG_HexGrid::OffsetType::ODD_Q) {
        q = a.x - (a.y - (a.y & 1)) / 2;
        r = a.y;
        s = -q - r;
    } else {
        q = a.x - (a.y + (a.y & 1)) / 2;
        r = a.y;
        s = -q - r;
    }
    return Vector3i(q, r, s);
}

inline static Vector3i hex_subtract(const Vector3i &a, const Vector3i &b)
{
    return Vector3i(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline static int hex_length(const Vector3i &hex)
{
    return int((abs(hex.x) + abs(hex.y) + abs(hex.z)) / 2);
}

inline static int hex_distance(const Vector3i &a, const Vector3i &b, BG_HexGrid::OffsetType offset_type)
{
    return hex_length(hex_subtract(offset_to_cube(a, offset_type), offset_to_cube(b, offset_type) ));
}

inline int BG_HexGrid::get_hex_cost(const Ref<BG_Hex> instigator, Vector2i qr, bool do_pass_through_check) const
{
    const Ref<BG_HexGameSaveData> hgsd = get_game_data_from_qr(qr);
    const Ref<BG_HexGameSaveData> instigator_hgsd = instigator.is_valid() ? get_game_data_from_qr(instigator->get_qr()) : nullptr;
    const bool is_band = instigator_hgsd.is_valid() ? instigator_hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::BAND : false;
    const bool is_job = instigator_hgsd.is_valid() ? instigator_hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::JOB : false;

    const Ref<BG_Hex> hex = get_hex_by_qr(qr);
    if (hex.is_null() || hex->get_empty()) return false;

    if (base_grid_visual_data.has(qr))
    {
        const Ref<BG_HexVisualData> h = base_grid_visual_data[qr];
        if (h.is_valid()) {
            for (int i = 0; i < h->hex_asset_datas.size(); ++i) {
                const Ref<BG_HexVisualAssetData> hvad = h->hex_asset_datas[i];
                if (hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::CITY || 
                    hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::WALL) {
                    return 0;
                }

                // If this asset can be destroyed, then allow a job to pass through it if it is destroyed. Otherwise always allow bands to pass through it.
                if (is_job) {
                    if (hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::TOWN || 
                        hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::BARRICADE || 
                        hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::TURRET) {
                        if (do_pass_through_check && hgsd.is_valid() && hgsd->get_asset_type_cost() == 0) {
                            if (hgsd->is_destroyed) return 1;
                        }
                        return 0;
                    }
                }

                if (!do_pass_through_check) {
                    if (hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::BARRICADE || 
                        hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::TURRET || 
                        hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::TOWN || 
                        hvad->hex_type == BG_HexVisualAssetData::HexVisualAssetTypes::NO_STOP_CELL) {
                        return 0;
                    }
                }
            }
        }
    }
    
    if (hgsd.is_valid() && hgsd->get_asset_type_cost() == 0) {
        // Pass through check.
        if (do_pass_through_check && instigator_hgsd.is_valid()) {
            // Band
            if (is_band) {

                if (hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::BAND || // Allow these if band is moving.
                    hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::TOWN ||
                    hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::BARRICADE ||
                    hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::TURRET ||
                    hgsd->asset_type == BG_HexGameSaveData::HexGameAssetTypes::CITY) {
                    // Do nothing.
                } else {
                    return 0;
                }
            // Job
            } else if (is_job) {

                if (hgsd->asset_type != BG_HexGameSaveData::HexGameAssetTypes::TOWN ||
                    hgsd->asset_type != BG_HexGameSaveData::HexGameAssetTypes::BARRICADE ||
                    hgsd->asset_type != BG_HexGameSaveData::HexGameAssetTypes::TURRET) {
                    if (!hgsd->is_destroyed) {
                        return 0;
                    }
                } else if (hgsd->asset_type != BG_HexGameSaveData::HexGameAssetTypes::JOB) {
                    return 0;
                }
            }
        } else {
            return 0;
        }
    }

    return 1;
}

static int heuristic(const Ref<BG_Hex> a, const Ref<BG_Hex> b) {
    return (Math::abs(a->q - b->q) + Math::abs(a->r - b->r) + Math::abs(a->s - b->s)) / 2;
}

Ref<BG_HexGameSaveData> BG_HexGrid::get_nearest_job_attackable(const Ref<BG_Hex> from_job_hex, const TypedArray<int> attackable_types) const
{
    if (from_job_hex.is_null()) return nullptr;

    Array attackable_types_converted;
    for (int i = 0; i < attackable_types.size(); i++) {
        attackable_types_converted.append( static_cast<BG_HexGameSaveData::HexGameAssetTypes>(attackable_types[i].operator int()) );
    }

    Ref<BG_HexGameSaveData> result;
    int nearest_distance = 999999;

    for (uint32_t i = 1; i < game_data.size(); ++i) {
        const Ref<BG_HexGameSaveData> data = game_data[i];
        if (data.is_null()) continue;

        if (data->get_is_destroyed() || !attackable_types_converted.has(data->asset_type)) continue;

        const int d = hex_distance(from_job_hex->get_full_qr(), get_hex_by_qr(data->get_qr())->get_full_qr(), offset_type);
        if (d < nearest_distance) {
            nearest_distance = d;
            result = data;
        }
    }

    return result;
}

Ref<BG_Hex> BG_HexGrid::get_nearest_empty_cell_neighoring_target(const Ref<BG_Hex> instigator, const Ref<BG_Hex> target) const
{
    if (instigator.is_null()) return nullptr;
    if (target.is_null()) return nullptr;

    Ref<BG_Hex> result;
    int nearest_distance = 999999;

    const HashMap<HexDirections, Ref<BG_Hex>> neighbors = get_hex_neighbors_fast(target);
    for (const auto &pair : neighbors) {
        const Ref<BG_Hex> hex = pair.value;

        if (!hex.is_valid()) continue;
        const int hex_cost = get_hex_cost(/* from_hex */ instigator, /* qr */ hex->get_qr(), /* do_pass_through_check */ false);
        if (hex_cost == 0) continue; // Can't move into cell?

        const int d = hex_distance(instigator->get_full_qr(), get_hex_by_qr(hex->get_qr())->get_full_qr(), offset_type);
        if (d < nearest_distance) {
            nearest_distance = d;
            result = hex;
        }
    }

    return result;
}

TypedArray<BG_Hex> BG_HexGrid::find_path(const Ref<BG_Hex> instigator, const Ref<BG_Hex> start, const Ref<BG_Hex> goal, bool include_start) const
{
    if (start.is_null()) return {};
    if (goal.is_null()) return {};
    if (start == goal) {
        if (include_start) return {start};
        return {};
    }

    // If the goal is not a empty cell, then find a neighoring one that is empty.
    const int goal_hex_cost = get_hex_cost(/* from_hex */ instigator, /* qr */ goal->get_qr(), /* do_pass_through_check */ false);
    if (goal_hex_cost == 0) {
        Ref<BG_Hex> better_goal_cell = get_nearest_empty_cell_neighoring_target(start, goal);
        if (better_goal_cell.is_valid()) {
            return find_path(instigator, start, better_goal_cell, include_start);
        }
    }

    TypedArray<BG_Hex> frontier;
    // std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    frontier.append(start);
    HashMap<Ref<BG_Hex>, Ref<BG_Hex>> came_from;
    came_from[start] = start;
    Ref<BG_Hex> result_goal = goal;

    int index = 0;
    while (index < frontier.size())
    {
        const Ref<BG_Hex> current_hex = frontier[index++];
        if (current_hex == goal) break; // Early exit.

        const HashMap<HexDirections, Ref<BG_Hex>> neighbors = get_hex_neighbors_fast(current_hex);
        for (const auto &pair : neighbors) {
            const Ref<BG_Hex> next_hex = pair.value;

            if (!next_hex.is_valid()) continue;
            const int hex_cost = get_hex_cost(/* from_hex */ instigator, /* qr */ next_hex->get_qr(), /* do_pass_through_check */ true);
            if (hex_cost == 0) { // Can't move into cell?
                if (next_hex == goal) { // Is the cell the goal, if so, make the previous cell the goal.

                    // Ensure the whole path isn't just walk throughs without any landings.
                    bool has_valid_cell = false;
                    Ref<BG_Hex> current_pass_through_check_hex = current_hex;
                    int pass_through_count = 0;
                    while (current_pass_through_check_hex != start) {
                        pass_through_count++;
                        if (get_hex_cost(/* from_hex */ nullptr, /* qr */ current_pass_through_check_hex->get_qr(), /* do_pass_through_check */ false) != 0) {
                            has_valid_cell = true;
                            break;
                        }
                        current_pass_through_check_hex = came_from[current_pass_through_check_hex];
                    }
                    if (!has_valid_cell && pass_through_count > 0) continue;

                    // We have a valid path to the goal, so bail out.
                    result_goal = current_hex;
                    frontier.clear();
                    break;
                }
                continue;
            }

            if (!came_from.has(next_hex)) {
                const int priority = heuristic(goal, next_hex);
                frontier.append(next_hex);
                came_from[next_hex] = current_hex;
            }
        }
    }

    TypedArray<BG_Hex> result;
    Ref<BG_Hex> current = result_goal;
    while (current != start) {
        if (!came_from.has(current)) {
            if (include_start) return {start};
            return {};
        }
        result.append(current);
        current = came_from[current];
    }

    // Since we allow building paths through friendlies, let's ensure that the path doesn't end on a friendly.
    if (!result.is_empty()) {
        while (get_hex_cost(/* from_hex */ nullptr, /* qr */ Ref<BG_Hex>(result[0])->get_qr(), /* do_pass_through_check */ false) == 0) {
            result.remove_at(0);
            if (result.is_empty()) break;
        }
    }

    if (include_start) result.append(start);
    result.reverse();

    return result;
}

TypedArray<BG_Hex> BG_HexGrid::find_reachable_cells_in_distance(const Ref<BG_Hex> instigator, const Ref<BG_Hex> start, int distance) const
{
    TypedArray<BG_Hex> frontier;
    frontier.append(start);
    HashMap<Ref<BG_Hex>, Ref<BG_Hex>> came_from;
    came_from[start] = start;

    // Get all cells within the distance, that are not obstucted.
    int index = 0;
    while (index < frontier.size())
    {
        const Ref<BG_Hex> current_hex = frontier[index++];

        const HashMap<HexDirections, Ref<BG_Hex>> neighbors = get_hex_neighbors_fast(current_hex);
        for (const auto &pair : neighbors) {
            const Ref<BG_Hex> next_hex = pair.value;

            if (!next_hex.is_valid()) continue;
            const int hex_cost = get_hex_cost(/* from_hex */ instigator, /* qr */ next_hex->get_qr(), /* do_pass_through_check */ true);
            if (hex_cost == 0) continue;

            const int d = hex_distance(next_hex->get_full_qr(), start->get_full_qr(), offset_type);
            if (d > distance) continue;

            if (!came_from.has(next_hex)) {
                frontier.append(next_hex);
                came_from[next_hex] = current_hex;
            }
        }
    }

    // Only pick the cells that are X distance from the start.
    frontier.clear();
    for (const auto &pair : came_from) {
        Ref<BG_Hex> current_hex = pair.key;

        int cell_distance = 0;
        while (current_hex != start) {
            cell_distance++;
            if (cell_distance > distance) break;
            current_hex = came_from[current_hex];
        }

        if (cell_distance <= distance) {
            frontier.append(pair.key);
        }
    }

    // Remove cells that are blocked. This would be the case for a cell with a friendly unit.
    for (int i = (frontier.size() - 1); i >= 0; --i) {
        const Ref<BG_Hex> h = frontier[i];
        const int hex_cost = get_hex_cost(/* from_hex */ nullptr, /* qr */ h->get_qr(), /* do_pass_through_check */ false);
        if (hex_cost == 0) {
            frontier.remove_at(i);
        }
    }

    // Remove the starting cell.
    // if (frontier.has(start)) frontier.erase(start);

    return frontier;
}
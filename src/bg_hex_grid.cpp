#include "bg_hex_grid.hpp"
#include <queue>

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

////
//// BG_HexVisualAssetData
////
void BG_HexVisualAssetData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_hex_type"), &BG_HexVisualAssetData::get_hex_type);
	ClassDB::bind_method(D_METHOD("set_hex_type"), &BG_HexVisualAssetData::set_hex_type);
	ClassDB::bind_method(D_METHOD("get_rotation"), &BG_HexVisualAssetData::get_rotation);
	ClassDB::bind_method(D_METHOD("set_rotation"), &BG_HexVisualAssetData::set_rotation);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "hex_type", PROPERTY_HINT_ENUM, "CITY:0,REST:1,MONSTER_SPAWN:2,WALL:3"), "set_hex_type", "get_hex_type");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation"), "set_rotation", "get_rotation");

    // ClassDB::bind_integer_constant("BG_HexVisualAssetData", "HexVisualAssetTypes", "CITY", CITY);
    // ClassDB::bind_integer_constant("BG_HexVisualAssetData", "HexVisualAssetTypes", "REST", REST);
    // ClassDB::bind_integer_constant("BG_HexVisualAssetData", "HexVisualAssetTypes", "MONSTER_SPAWN", MONSTER_SPAWN);
    // ClassDB::bind_integer_constant("BG_HexVisualAssetData", "HexVisualAssetTypes", "WALL", WALL);
	BIND_ENUM_CONSTANT(CITY);
	BIND_ENUM_CONSTANT(REST);
	BIND_ENUM_CONSTANT(MONSTER_SPAWN);
	BIND_ENUM_CONSTANT(WALL);
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
//// BG_HexSaveData
////
void BG_HexSaveData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_q"), &BG_HexSaveData::get_q);
	ClassDB::bind_method(D_METHOD("set_q"), &BG_HexSaveData::set_q);
	ClassDB::bind_method(D_METHOD("get_r"), &BG_HexSaveData::get_r);
	ClassDB::bind_method(D_METHOD("set_r"), &BG_HexSaveData::set_r);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "q"), "set_q", "get_q");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "r"), "set_r", "get_r");
}

////
//// BG_BattleBoardSaveData
////
void BG_BattleBoardSaveData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_hex_slots"), &BG_BattleBoardSaveData::get_hex_slots);
	ClassDB::bind_method(D_METHOD("set_hex_slots"), &BG_BattleBoardSaveData::set_hex_slots);

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
	ClassDB::bind_method(D_METHOD("get_size_per_hex"), &BG_HexGrid::get_size_per_hex);
	ClassDB::bind_method(D_METHOD("set_size_per_hex"), &BG_HexGrid::set_size_per_hex);
	ClassDB::bind_method(D_METHOD("get_offset_between_hexes"), &BG_HexGrid::get_offset_between_hexes);
	ClassDB::bind_method(D_METHOD("set_offset_between_hexes"), &BG_HexGrid::set_offset_between_hexes);
	ClassDB::bind_method(D_METHOD("get_center_of_hex_location"), &BG_HexGrid::get_center_of_hex_location);
	ClassDB::bind_method(D_METHOD("get_hex_direction_vec", "direction"), &BG_HexGrid::get_hex_direction_vec);
	ClassDB::bind_method(D_METHOD("get_hex_in_direction", "from_hex", "direction"), &BG_HexGrid::get_hex_in_direction);
	ClassDB::bind_method(D_METHOD("get_hex_neighbors", "from_hex"), &BG_HexGrid::get_hex_neighbors);
	ClassDB::bind_method(D_METHOD("get_hex_by_coords", "coords"), &BG_HexGrid::get_hex_by_coords);
	ClassDB::bind_method(D_METHOD("add_hex", "hex"), &BG_HexGrid::add_hex);
	ClassDB::bind_method(D_METHOD("add_row", "column_index", "initial_emptys", "count"), &BG_HexGrid::add_row);
	ClassDB::bind_method(D_METHOD("update_locations", "x_offset_percent", "y_offset_percent"), &BG_HexGrid::update_locations);
	ClassDB::bind_method(D_METHOD("find_path", "start", "goal"), &BG_HexGrid::find_path);
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

Vector2i BG_HexGrid::get_direction_difference(const BG_Hex *hex, Vector2i d) const
{
    if (hex == nullptr) return Vector2i(0, 0);
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


BG_Hex *BG_HexGrid::get_hex_by_coords(Vector2i coords) const
{
	for (int i = 0; i < grid.size(); ++i)
	{
		BG_Hex *h = cast_to<BG_Hex>(grid[i]);
        if (coords == h->get_coords())
            return h;
    }
    return nullptr;
}

BG_Hex *BG_HexGrid::get_hex_in_direction(const BG_Hex *from_hex, Vector2i d) const
{
    const Vector2i direction_diff = get_direction_difference(from_hex, d);
    const Vector2i v = direction_diff + Vector2i(from_hex->r, from_hex->q);
	for (int i = 0; i < grid.size(); ++i)
	{
		BG_Hex *h = cast_to<BG_Hex>(grid[i]);
        if (h->q == v.y && h->r == v.x) {
            // if (h->get_empty()) return nullptr;
            return h;
        }
    }
    return nullptr;
}

HashMap<BG_HexGrid::HexDirections, BG_Hex *> BG_HexGrid::get_hex_neighbors_fast(const BG_Hex *from_hex) const
{
    HashMap<HexDirections, BG_Hex *> result;
    result[BG_HexGrid::HexDirections::TOP] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::TOP]);
    result[BG_HexGrid::HexDirections::RIGHT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::RIGHT]);
    result[BG_HexGrid::HexDirections::BOTTOM] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::BOTTOM]);
    result[BG_HexGrid::HexDirections::BOTTOM_LEFT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::BOTTOM_LEFT]);
    result[BG_HexGrid::HexDirections::LEFT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::LEFT]);
    result[BG_HexGrid::HexDirections::TOP_LEFT] = get_hex_in_direction(from_hex, hex_directions[BG_HexGrid::HexDirections::TOP_LEFT]);
    return result;
}

Dictionary BG_HexGrid::get_hex_neighbors(const BG_Hex *from_hex) const
{
    Dictionary result;
    const HashMap<BG_HexGrid::HexDirections, BG_Hex *> neighbors = get_hex_neighbors_fast(from_hex);
    for (const auto &pair : neighbors) {
        result[pair.key] = pair.value;
    }
    return result;
}

void BG_HexGrid::add_hex(const BG_Hex *hex)
{
    if (hex == nullptr) return;
    if (grid.is_empty()) {
        grid.append(hex);
        return;
    }

	for (int i = 0; i < grid.size(); ++i)
	{
		const BG_Hex *h = cast_to<BG_Hex>(grid[i]);
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
        BG_Hex *new_hex = memnew(BG_Hex);
        new_hex->q = column_index;
        new_hex->r = i;
        new_hex->s = (-new_hex->q - new_hex->r);
        new_hex->empty = i < initial_emptys;
        // add_hex(new_hex);
        grid.append(new_hex);
    }
}

void BG_HexGrid::update_locations(float p_x_offset_percent, float p_y_offset_percent)
{
    x_offset_percent = p_x_offset_percent;
    y_offset_percent = p_y_offset_percent;
	for (int i = 0; i < grid.size(); ++i)
	{
		BG_Hex *h = cast_to<BG_Hex>(grid[i]);
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

static Dictionary make_priority_item(const BG_Hex *h, int priority)
{
    Dictionary result;
    result["h"] = h;
    result["p"] = priority;
    return result;
}

bool BG_HexGrid::comp_priority_item(Dictionary a, Dictionary b) const {
	return int(a["p"]) < int(b["p"]);
}

Vector3i hex_subtract(const Vector3i &a, const Vector3i &b)
{
    return Vector3i(a.x - b.x, a.y - b.y, a.z - b.z);
}

int hex_length(const Vector3i &hex)
{
    return int((abs(hex.x) + abs(hex.y) + abs(hex.z)) / 2);
}

int hex_distance(const Vector3i &a, const Vector3i &b)
{
    return hex_length(hex_subtract(a, b));
}

TypedArray<BG_Hex> BG_HexGrid::find_path(BG_Hex *start, BG_Hex *goal)
{
    TypedArray<Dictionary> frontier;
    frontier.append(make_priority_item(start, 0));
    Dictionary came_from;
    came_from[start->get_coords()] = nullptr;
    HashMap<const BG_Hex *, int> cost_so_far;
    cost_so_far[start] = 0;

    const Callable callable = Callable(this, StringName("comp_priority_item"));

    while (!frontier.is_empty()) {
        const Dictionary dict = frontier.pop_front();
        const BG_Hex *current_hex = cast_to<BG_Hex>(dict["h"]);
        if (current_hex == goal) break;
        const Vector2i current_coords = current_hex->get_coords();
        
        const HashMap<HexDirections, BG_Hex *> neighbors = get_hex_neighbors_fast(current_hex);
        for (const auto &pair : neighbors) {
            const BG_Hex *next_hex = pair.value;
            if (next_hex == nullptr || next_hex->get_empty()) continue;
            int next_cost = current_hex->get_move_cost(next_hex);

            if ((next_hex == goal) && (next_hex->get_hex_cost() == 0)) {
                // Our goal is an obstacle, but we're next to it
                // so our work here is done
                came_from[next_hex->get_coords()] = current_coords;
                frontier.clear();
                break;
            }
            next_cost += cost_so_far[current_hex];
            if ((!cost_so_far.has(next_hex)) || (next_cost < cost_so_far[next_hex])) {
                // New shortest path to that node
                cost_so_far[next_hex] = next_cost;
                const int distance = hex_distance(next_hex->get_full_coords(), goal->get_full_coords());
                const int priority = next_cost + distance;
                // Insert into the frontier
                const Dictionary item = make_priority_item(next_hex, priority);
                const int idx = frontier.bsearch_custom(item, callable, true);
                frontier.insert(idx, item);
                came_from[next_hex->get_coords()] = current_coords;
            }
        }
    }

    if (!came_from.has(goal->get_coords())) return {}; // No path found.

    // Follow the path back where we came_from
    TypedArray<BG_Hex> path;
    if (goal->get_hex_cost() != 0) {
        // We only include the goal if it's traversable
        path.append(goal);
    }
    Vector2i current_coords = goal->get_coords();
    while (current_coords != start->get_coords()) {
        current_coords = came_from[current_coords];
        const BG_Hex *hex = get_hex_by_coords(current_coords);
        path.push_front(hex);
    }
    return path;
}
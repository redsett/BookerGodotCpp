// Data classes for units: band members, formations, bands, castes, jobs, monsters, job details.
#include "bg_booker_data_base.hpp"
#include "bg_dber_read.hpp"
#include "bg_focus_layers.hpp"
#include "bg_hex_grid.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <string>

using namespace godot;

////
//// BG_BandMember
////
void BG_BandMember::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_first_name_id"), &BG_BandMember::get_first_name_id);
	ClassDB::bind_method(D_METHOD("set_first_name_id"), &BG_BandMember::set_first_name_id);
	ClassDB::bind_method(D_METHOD("get_last_name_id"), &BG_BandMember::get_last_name_id);
	ClassDB::bind_method(D_METHOD("set_last_name_id"), &BG_BandMember::set_last_name_id);
	ClassDB::bind_method(D_METHOD("get_band_member_type"), &BG_BandMember::get_band_member_type);
	ClassDB::bind_method(D_METHOD("set_band_member_type"), &BG_BandMember::set_band_member_type);
	ClassDB::bind_method(D_METHOD("get_current_percent_health"), &BG_BandMember::get_current_percent_health);
	ClassDB::bind_method(D_METHOD("set_current_percent_health"), &BG_BandMember::set_current_percent_health);
	ClassDB::bind_method(D_METHOD("get_percent_health_additive"), &BG_BandMember::get_percent_health_additive);
	ClassDB::bind_method(D_METHOD("set_percent_health_additive"), &BG_BandMember::set_percent_health_additive);
	ClassDB::bind_method(D_METHOD("get_current_percent_action_points"), &BG_BandMember::get_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("set_current_percent_action_points"), &BG_BandMember::set_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("get_current_action_points_fatigue"), &BG_BandMember::get_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("set_current_action_points_fatigue"), &BG_BandMember::set_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("get_slot_index"), &BG_BandMember::get_slot_index);
	ClassDB::bind_method(D_METHOD("set_slot_index"), &BG_BandMember::set_slot_index);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_BandMember::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_BandMember::set_random_variation);
	ClassDB::bind_method(D_METHOD("get_is_band_leader"), &BG_BandMember::get_is_band_leader);
	ClassDB::bind_method(D_METHOD("set_is_band_leader"), &BG_BandMember::set_is_band_leader);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_BandMember::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_BandMember::set_level);
	ClassDB::bind_method(D_METHOD("get_experience"), &BG_BandMember::get_experience);
	ClassDB::bind_method(D_METHOD("set_experience"), &BG_BandMember::set_experience);
	ClassDB::bind_method(D_METHOD("get_scale"), &BG_BandMember::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale"), &BG_BandMember::set_scale);
	ClassDB::bind_method(D_METHOD("get_caste_id"), &BG_BandMember::get_caste_id);
	ClassDB::bind_method(D_METHOD("set_caste_id"), &BG_BandMember::set_caste_id);
	ClassDB::bind_method(D_METHOD("get_equipment"), &BG_BandMember::get_equipment);
	ClassDB::bind_method(D_METHOD("set_equipment"), &BG_BandMember::set_equipment);
	ClassDB::bind_method(D_METHOD("get_inventory"), &BG_BandMember::get_inventory);
	ClassDB::bind_method(D_METHOD("set_inventory"), &BG_BandMember::set_inventory);
	ClassDB::bind_method(D_METHOD("get_is_city_asset"), &BG_BandMember::get_is_city_asset);
	ClassDB::bind_method(D_METHOD("set_is_city_asset"), &BG_BandMember::set_is_city_asset);
	ClassDB::bind_method(D_METHOD("get_band"), &BG_BandMember::get_band);
	ClassDB::bind_method(D_METHOD("set_band"), &BG_BandMember::set_band);
	ClassDB::bind_method(D_METHOD("get_battle_board_hex_type_details"), &BG_BandMember::get_battle_board_hex_type_details);
	ClassDB::bind_method(D_METHOD("set_battle_board_hex_type_details"), &BG_BandMember::set_battle_board_hex_type_details);
	ClassDB::bind_method(D_METHOD("get_attribute_upgrades"), &BG_BandMember::get_attribute_upgrades);
	ClassDB::bind_method(D_METHOD("set_attribute_upgrades"), &BG_BandMember::set_attribute_upgrades);
	ClassDB::bind_method(D_METHOD("get_consumable_upgrades"), &BG_BandMember::get_consumable_upgrades);
	ClassDB::bind_method(D_METHOD("set_consumable_upgrades"), &BG_BandMember::set_consumable_upgrades);
	ClassDB::bind_method(D_METHOD("get_equipped_ability_ids"), &BG_BandMember::get_equipped_ability_ids);
	ClassDB::bind_method(D_METHOD("set_equipped_ability_ids"), &BG_BandMember::set_equipped_ability_ids);
	ClassDB::bind_method(D_METHOD("get_equipped_passive_ids"), &BG_BandMember::get_equipped_passive_ids);
	ClassDB::bind_method(D_METHOD("set_equipped_passive_ids"), &BG_BandMember::set_equipped_passive_ids);
	ClassDB::bind_method(D_METHOD("is_dead"), &BG_BandMember::is_dead);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandMember::get_name);
	ClassDB::bind_method(D_METHOD("get_first_name"), &BG_BandMember::get_first_name);
	ClassDB::bind_method(D_METHOD("get_last_name"), &BG_BandMember::get_last_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "first_name_id"), "set_first_name_id", "get_first_name_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "last_name_id"), "set_last_name_id", "get_last_name_id");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "band_member_type", PROPERTY_HINT_ENUM, 
        "DEFAULT:0,PROTAGONIST:1,ALLY_MAIN_CHARACTER:2,ENEMY_MAIN_CHARACTER:3"), 
        "set_band_member_type", "get_band_member_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_health"), "set_current_percent_health", "get_current_percent_health");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "percent_health_additive"), "set_percent_health_additive", "get_percent_health_additive");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_action_points"), "set_current_percent_action_points", "get_current_percent_action_points");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_action_points_fatigue"), "set_current_action_points_fatigue", "get_current_action_points_fatigue");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "slot_index"), "set_slot_index", "get_slot_index");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_band_leader"), "set_is_band_leader", "get_is_band_leader");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "experience"), "set_experience", "get_experience");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "caste_id"), "set_caste_id", "get_caste_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipment"), "set_equipment", "get_equipment");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "inventory"), "set_inventory", "get_inventory");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "attribute_upgrades"), "set_attribute_upgrades", "get_attribute_upgrades");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "consumable_upgrades"), "set_consumable_upgrades", "get_consumable_upgrades");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipped_ability_ids", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_equipped_ability_ids", "get_equipped_ability_ids");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipped_passive_ids", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_equipped_passive_ids", "get_equipped_passive_ids");

	BIND_ENUM_CONSTANT(DEFAULT);
	BIND_ENUM_CONSTANT(PROTAGONIST);
	BIND_ENUM_CONSTANT(ALLY_MAIN_CHARACTER);
	BIND_ENUM_CONSTANT(ENEMY_MAIN_CHARACTER);
}

StringName BG_BandMember::get_name()
{
	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return cached_name;

	// The cache is cleared when a name id changes, and redone when the language changes.
	if (!cached_name.is_empty() && cached_name_language == booker_db->get_current_language_key()) return cached_name;
	cached_name_language = booker_db->get_current_language_key();
	
	String first_name = "";
	if (!get_first_name_id().is_empty())
		first_name = booker_db->get_localize_string(StringName("band_member"), get_first_name_id(), booker_db->get_current_language_key(), true);
	String last_name = "";
	if (!get_last_name_id().is_empty())
		last_name = booker_db->get_localize_string(StringName("band_member"), get_last_name_id(), booker_db->get_current_language_key(), true);
	
	cached_name = StringName(first_name + " " + last_name);
	return cached_name;
}

StringName BG_BandMember::get_first_name()
{
	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return "";
	
	StringName first_name;
	if (!get_first_name_id().is_empty())
		first_name = StringName(booker_db->get_localize_string(StringName("band_member"), get_first_name_id(), booker_db->get_current_language_key(), true));
	
	return first_name;
}

StringName BG_BandMember::get_last_name()
{
	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return "";
	
	StringName last_name;
	if (!get_last_name_id().is_empty())
		last_name = StringName(booker_db->get_localize_string(StringName("band_member"), get_last_name_id(), booker_db->get_current_language_key(), true));
	
	return last_name;
}

BG_BandMember::~BG_BandMember()
{
	battle_board_hex_type_details = nullptr;
}

////
//// BG_Formation
////
void BG_Formation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Formation::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Formation::set_id);
	ClassDB::bind_method(D_METHOD("get_formation"), &BG_Formation::get_formation);
	ClassDB::bind_method(D_METHOD("set_formation"), &BG_Formation::set_formation);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "id", PROPERTY_HINT_MULTILINE_TEXT), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "formation"), "set_formation", "get_formation");
}

////
//// BG_DefaultFormations
////
void BG_DefaultFormations::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_formations"), &BG_DefaultFormations::get_formations);
	ClassDB::bind_method(D_METHOD("set_formations"), &BG_DefaultFormations::set_formations);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "formations"), "set_formations", "get_formations");
}

////
//// BG_Band
////
void BG_Band::_bind_methods()
{
	ClassDB::bind_static_method("BG_Band", D_METHOD("get_unique_band_id", "existing_bands"), &BG_Band::get_unique_band_id);

	ClassDB::bind_method(D_METHOD("get_unique_id"), &BG_Band::get_unique_id);
	ClassDB::bind_method(D_METHOD("set_unique_id"), &BG_Band::set_unique_id);
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Band::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Band::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Band::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Band::set_name);
	ClassDB::bind_method(D_METHOD("get_preset_band_id"), &BG_Band::get_preset_band_id);
	ClassDB::bind_method(D_METHOD("set_preset_band_id"), &BG_Band::set_preset_band_id);
	ClassDB::bind_method(D_METHOD("get_resting"), &BG_Band::get_resting);
	ClassDB::bind_method(D_METHOD("set_resting"), &BG_Band::set_resting);
	ClassDB::bind_method(D_METHOD("get_is_on_battle_board"), &BG_Band::get_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("set_is_on_battle_board"), &BG_Band::set_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("get_distance_traveled_this_turn"), &BG_Band::get_distance_traveled_this_turn);
	ClassDB::bind_method(D_METHOD("set_distance_traveled_this_turn"), &BG_Band::set_distance_traveled_this_turn);
	ClassDB::bind_method(D_METHOD("get_has_done_combat_this_turn"), &BG_Band::get_has_done_combat_this_turn);
	ClassDB::bind_method(D_METHOD("set_has_done_combat_this_turn"), &BG_Band::set_has_done_combat_this_turn);
	ClassDB::bind_method(D_METHOD("get_knocked_out_turns"), &BG_Band::get_knocked_out_turns);
	ClassDB::bind_method(D_METHOD("set_knocked_out_turns"), &BG_Band::set_knocked_out_turns);
	ClassDB::bind_method(D_METHOD("get_band_members"), &BG_Band::get_band_members);
	ClassDB::bind_method(D_METHOD("set_band_members"), &BG_Band::set_band_members);
	ClassDB::bind_method(D_METHOD("get_current_unique_job_id"), &BG_Band::get_current_unique_job_id);
	ClassDB::bind_method(D_METHOD("set_current_unique_job_id"), &BG_Band::set_current_unique_job_id);
	ClassDB::bind_method(D_METHOD("get_band_formation"), &BG_Band::get_band_formation);
	ClassDB::bind_method(D_METHOD("set_band_formation"), &BG_Band::set_band_formation);

	ClassDB::bind_method(D_METHOD("has_job"), &BG_Band::has_job);
	ClassDB::bind_method(D_METHOD("clear_job"), &BG_Band::clear_job);
	ClassDB::bind_method(D_METHOD("is_band_alive"), &BG_Band::is_band_alive);
	ClassDB::bind_method(D_METHOD("get_band_leader"), &BG_Band::get_band_leader);
	ClassDB::bind_method(D_METHOD("set_band_leader", "band_member"), &BG_Band::set_band_leader);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_id"), "set_unique_id", "get_unique_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "preset_band_id"), "set_preset_band_id", "get_preset_band_id");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "resting"), "set_resting", "get_resting");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_on_battle_board"), "set_is_on_battle_board", "get_is_on_battle_board");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "distance_traveled_this_turn"), "set_distance_traveled_this_turn", "get_distance_traveled_this_turn");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_done_combat_this_turn"), "set_has_done_combat_this_turn", "get_has_done_combat_this_turn");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "knocked_out_turns"), "set_knocked_out_turns", "get_knocked_out_turns");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "band_members"), "set_band_members", "get_band_members");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_unique_job_id"), "set_current_unique_job_id", "get_current_unique_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "band_formation"), "set_band_formation", "get_band_formation");
}

StringName BG_Band::get_name()
{
	if (!name.is_empty()) return name; // A custom name always wins.

	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return cached_name;

	// The cache is cleared when the id changes, and redone when the language changes.
	if (!cached_name.is_empty() && cached_name_language == booker_db->get_current_language_key()) return cached_name;
	cached_name_language = booker_db->get_current_language_key();

	if (!get_id().is_empty())
		cached_name = StringName(booker_db->get_localize_string(StringName("band_member"), get_id(), booker_db->get_current_language_key(), true));
	
	return cached_name;
}

bool BG_Band::is_band_alive() const
{
	if (get_knocked_out_turns() > 1)
		return false;
	
	for (uint32_t i = 0; i < get_band_members().size(); ++i) {
		const Ref<BG_BandMember> band_member = get_band_members()[i];
		if (band_member.is_null()) continue;
		if (!band_member->is_dead()) {
			return true;
		}
	}
	return false;
}

/* static */ int BG_Band::get_unique_band_id(const TypedArray<BG_Band> &existing_bands)
{
	// Randomly pick a unique id.
	int unique_id = 0;
	while (unique_id == 0) {
		int random_id = UtilityFunctions::randi_range(100, 99999);
		for (uint32_t i = 0; i < existing_bands.size(); ++i) {
			const Ref<BG_Band> b = existing_bands[i];
			if (b->get_unique_id() == random_id) {
				random_id = 0;
				break;
			}
		}
		if (random_id != 0) {
			unique_id = random_id;
		}
	}
	
	return unique_id;
}

////
//// BG_UnitCaste
////
void BG_UnitCaste::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitCaste::get_id);
	ClassDB::bind_method(D_METHOD("get_parent_caste"), &BG_UnitCaste::get_parent_caste);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitCaste::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_is_unique_caste"), &BG_UnitCaste::get_is_unique_caste);
	ClassDB::bind_method(D_METHOD("get_lod_mesh_paths"), &BG_UnitCaste::get_lod_mesh_paths);
	ClassDB::bind_method(D_METHOD("get_scale_min"), &BG_UnitCaste::get_scale_min);
	ClassDB::bind_method(D_METHOD("get_scale_max"), &BG_UnitCaste::get_scale_max);
	ClassDB::bind_method(D_METHOD("get_scale_min_extreme"), &BG_UnitCaste::get_scale_min_extreme);
	ClassDB::bind_method(D_METHOD("get_scale_max_extreme"), &BG_UnitCaste::get_scale_max_extreme);
	ClassDB::bind_method(D_METHOD("get_travel_distance"), &BG_UnitCaste::get_travel_distance);
	ClassDB::bind_method(D_METHOD("get_inventory_space"), &BG_UnitCaste::get_inventory_space);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_UnitCaste::get_stats);
	ClassDB::bind_method(D_METHOD("get_starting_item_ids"), &BG_UnitCaste::get_starting_item_ids);
	ClassDB::bind_method(D_METHOD("get_element_availability_ids"), &BG_UnitCaste::get_element_availability_ids);
	ClassDB::bind_method(D_METHOD("get_attribute_skill_tree_datas"), &BG_UnitCaste::get_attribute_skill_tree_datas);
	ClassDB::bind_method(D_METHOD("get_random_unit_caste_scale", "x_z"), &BG_UnitCaste::get_random_unit_caste_scale);
}

float BG_UnitCaste::get_random_unit_caste_scale(bool x_z) const {
	if (UtilityFunctions::randf() > 0.75) { // 25% chance of allowing the chance of being extreme.
		if (x_z) {
			return UtilityFunctions::randf_range(get_scale_min_extreme().x, get_scale_max_extreme().x);
		} else {
			return UtilityFunctions::randf_range(get_scale_min_extreme().y, get_scale_max_extreme().y);
		}
	} else {
		if (x_z) {
			return UtilityFunctions::randf_range(get_scale_min().x, get_scale_max().x);
		} else {
			return UtilityFunctions::randf_range(get_scale_min().y, get_scale_max().y);
		}
	}
}

BG_UnitCaste::~BG_UnitCaste()
{
}

////
//// BG_BandNameInfo
////
void BG_BandNameInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandNameInfo::get_name);
	ClassDB::bind_method(D_METHOD("get_allowed_in_random"), &BG_BandNameInfo::get_allowed_in_random);
}

////
//// BG_Job
////
void BG_Job::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_job_id"), &BG_Job::get_job_id);
	ClassDB::bind_method(D_METHOD("set_job_id"), &BG_Job::set_job_id);
	ClassDB::bind_method(D_METHOD("get_unique_job_id"), &BG_Job::get_unique_job_id);
	ClassDB::bind_method(D_METHOD("set_unique_job_id"), &BG_Job::set_unique_job_id);
	ClassDB::bind_method(D_METHOD("get_random_seed"), &BG_Job::get_random_seed);
	ClassDB::bind_method(D_METHOD("set_random_seed"), &BG_Job::set_random_seed);
	ClassDB::bind_method(D_METHOD("get_weeks_remaining_unless_removed"), &BG_Job::get_weeks_remaining_unless_removed);
	ClassDB::bind_method(D_METHOD("set_weeks_remaining_unless_removed"), &BG_Job::set_weeks_remaining_unless_removed);
	ClassDB::bind_method(D_METHOD("get_monsters"), &BG_Job::get_monsters);
	ClassDB::bind_method(D_METHOD("set_monsters"), &BG_Job::set_monsters);
	ClassDB::bind_method(D_METHOD("get_formation"), &BG_Job::get_formation);
	ClassDB::bind_method(D_METHOD("set_formation"), &BG_Job::set_formation);
	ClassDB::bind_method(D_METHOD("get_is_on_battle_board"), &BG_Job::get_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("set_is_on_battle_board"), &BG_Job::set_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("try_set_all_monsters_to_stoned", "is_stoned"), &BG_Job::try_set_all_monsters_to_stoned);
	ClassDB::bind_method(D_METHOD("is_job_alive"), &BG_Job::is_job_alive);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "job_id"), "set_job_id", "get_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_job_id"), "set_unique_job_id", "get_unique_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_seed"), "set_random_seed", "get_random_seed");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "weeks_remaining_unless_removed"), "set_weeks_remaining_unless_removed", "get_weeks_remaining_unless_removed");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "monsters"), "set_monsters", "get_monsters");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "formation"), "set_formation", "get_formation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_on_battle_board"), "set_is_on_battle_board", "get_is_on_battle_board");
}

void BG_Job::try_set_all_monsters_to_stoned(bool stoned)
{
	for (int i = 0; i < monsters.size(); ++i) {
		BG_Monster *m = cast_to<BG_Monster>(monsters[i]);
		m->set_is_turned_to_stone(stoned);
	}
}

bool BG_Job::is_job_alive() const
{
	for (int i = 0; i < monsters.size(); ++i) {
		const BG_Monster *m = cast_to<BG_Monster>(monsters[i]);
		if (BG_Booker_DB::bg_is_instance_valid(m) && !m->is_dead()) {
			return true;
		}
	}
	return false;
}

////
//// BG_Monster
////
void BG_Monster::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Monster::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Monster::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Monster::get_name);
	ClassDB::bind_method(D_METHOD("get_preferred_row"), &BG_Monster::get_preferred_row);
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_Monster::get_max_health);
	ClassDB::bind_method(D_METHOD("get_base_action_points"), &BG_Monster::get_base_action_points);
	ClassDB::bind_method(D_METHOD("get_bonus_action_points"), &BG_Monster::get_bonus_action_points);
	ClassDB::bind_method(D_METHOD("get_base_action_points_cost"), &BG_Monster::get_base_action_points_cost);
	ClassDB::bind_method(D_METHOD("get_current_percent_action_points"), &BG_Monster::get_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("set_current_percent_action_points"), &BG_Monster::set_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("get_current_action_points_fatigue"), &BG_Monster::get_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("set_current_action_points_fatigue"), &BG_Monster::set_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("get_travel_distance"), &BG_Monster::get_travel_distance);
	ClassDB::bind_method(D_METHOD("get_current_percent_health"), &BG_Monster::get_current_percent_health);
	ClassDB::bind_method(D_METHOD("set_current_percent_health"), &BG_Monster::set_current_percent_health);
	ClassDB::bind_method(D_METHOD("get_percent_health_additive"), &BG_Monster::get_percent_health_additive);
	ClassDB::bind_method(D_METHOD("set_percent_health_additive"), &BG_Monster::set_percent_health_additive);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_Monster::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_Monster::set_random_variation);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_Monster::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_Monster::set_level);
	ClassDB::bind_method(D_METHOD("get_monster_type"), &BG_Monster::get_monster_type);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_Monster::get_stats);
	ClassDB::bind_method(D_METHOD("get_element_availability_ids"), &BG_Monster::get_element_availability_ids);
	ClassDB::bind_method(D_METHOD("get_job"), &BG_Monster::get_job);
	ClassDB::bind_method(D_METHOD("set_job"), &BG_Monster::set_job);
	
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_Monster::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_model_path"), &BG_Monster::get_model_path);
	ClassDB::bind_method(D_METHOD("get_effect_ids"), &BG_Monster::get_effect_ids);
	ClassDB::bind_method(D_METHOD("get_out_of_combat_effect_ids"), &BG_Monster::get_out_of_combat_effect_ids);
	ClassDB::bind_method(D_METHOD("get_hue_shift_data"), &BG_Monster::get_hue_shift_data);
	ClassDB::bind_method(D_METHOD("get_can_be_turned_to_stone"), &BG_Monster::get_can_be_turned_to_stone);
	ClassDB::bind_method(D_METHOD("get_is_turned_to_stone"), &BG_Monster::get_is_turned_to_stone);
	ClassDB::bind_method(D_METHOD("set_is_turned_to_stone"), &BG_Monster::set_is_turned_to_stone);
	ClassDB::bind_method(D_METHOD("is_dead"), &BG_Monster::is_dead);
	
	ClassDB::bind_method(D_METHOD("get_effectiveness"), &BG_Monster::get_effectiveness);
	ClassDB::bind_method(D_METHOD("get_effectiveness_stats"), &BG_Monster::get_effectiveness_stats);
	ClassDB::bind_method(D_METHOD("get_level_range"), &BG_Monster::get_level_range);
	ClassDB::bind_method(D_METHOD("get_animations"), &BG_Monster::get_animations);
	ClassDB::bind_method(D_METHOD("get_drops"), &BG_Monster::get_drops);
	ClassDB::bind_method(D_METHOD("get_action_points_fatigue_rate_multiplier"), &BG_Monster::get_action_points_fatigue_rate_multiplier);
	
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_action_points"), "set_current_percent_action_points", "get_current_percent_action_points");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_action_points_fatigue"), "set_current_action_points_fatigue", "get_current_action_points_fatigue");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_health"), "set_current_percent_health", "get_current_percent_health");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "percent_health_additive"), "set_percent_health_additive", "get_percent_health_additive");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_turned_to_stone"), "set_is_turned_to_stone", "get_is_turned_to_stone");
}

BG_Monster::~BG_Monster()
{
	for (int i = 0; i < hue_shift_data.size(); ++i) {
		BG_HueShiftData *d = cast_to<BG_HueShiftData>(hue_shift_data[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < effectiveness_stats.size(); ++i) {
		BG_ContentStat *d = cast_to<BG_ContentStat>(effectiveness_stats[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < animations.size(); ++i) {
		BG_AnimationDetails *d = cast_to<BG_AnimationDetails>(animations[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < drops.size(); ++i) {
		BG_RewardItem *d = cast_to<BG_RewardItem>(drops[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

void BG_Monster::set_is_turned_to_stone(bool v) {
	if (!can_be_turned_to_stone) {
		v = false;
	}
	if (v != is_turned_to_stone) {
		is_turned_to_stone = v;
		BG_Booker_DB::get_singleton()->emit_signal("monster_stoned_changed", this, v);
	}
}

////
//// BG_BandInfo
////
void BG_BandInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_band_names"), &BG_BandInfo::get_band_names);
	ClassDB::bind_method(D_METHOD("get_icon_paths"), &BG_BandInfo::get_icon_paths);
	ClassDB::bind_method(D_METHOD("get_first_names"), &BG_BandInfo::get_first_names);
	ClassDB::bind_method(D_METHOD("get_last_names"), &BG_BandInfo::get_last_names);
	ClassDB::bind_method(D_METHOD("get_band_size_min_max"), &BG_BandInfo::get_band_size_min_max);
	ClassDB::bind_method(D_METHOD("get_num_band_members_for_hire"), &BG_BandInfo::get_num_band_members_for_hire);
	ClassDB::bind_method(D_METHOD("get_unit_castes"), &BG_BandInfo::get_unit_castes);
	ClassDB::bind_method(D_METHOD("get_caste_by_id"), &BG_BandInfo::get_caste_by_id);
	ClassDB::bind_method(D_METHOD("get_rest_recovery_speed"), &BG_BandInfo::get_rest_recovery_speed);
	ClassDB::bind_method(D_METHOD("get_knocked_out_turns"), &BG_BandInfo::get_knocked_out_turns);
}

BG_BandInfo::~BG_BandInfo()
{
	for (int i = 0; i < band_names.size(); ++i) {
		BG_BandNameInfo *d = cast_to<BG_BandNameInfo>(band_names[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < first_names.size(); ++i) {
		BG_BandNameInfo *d = cast_to<BG_BandNameInfo>(first_names[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < last_names.size(); ++i) {
		BG_BandNameInfo *d = cast_to<BG_BandNameInfo>(last_names[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < unit_castes.size(); ++i) {
		BG_UnitCaste *d = cast_to<BG_UnitCaste>(unit_castes[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

BG_UnitCaste *BG_BandInfo::get_caste_by_id(const StringName &id, bool return_parent) const
{
	auto find_caste = [this](const StringName &caste_id) -> BG_UnitCaste * {
		for (int i = 0; i < unit_castes.size(); ++i) {
			BG_UnitCaste *caste = cast_to<BG_UnitCaste>(unit_castes[i]);
			if (caste != nullptr && caste->id == caste_id)
				return caste;
		}
		return nullptr;
	};

	BG_UnitCaste *caste = find_caste(id);
	if (!return_parent)
		return caste;

	// Walk up to the top parent. Capped, so a parent loop in the data (A -> B -> A) can't hang or overflow the stack.
	for (int steps = 0; caste != nullptr && !caste->get_parent_caste().is_empty(); ++steps) {
		if (steps > unit_castes.size()) {
			UtilityFunctions::push_error("BG_BandInfo::get_caste_by_id - parent castes loop around for caste \"", id, "\". Check parent_caste in the data.");
			return caste;
		}
		caste = find_caste(caste->get_parent_caste()); // nullptr if the parent doesn't exist, same as before.
	}
	return caste;
}

////
//// BG_JobDistributionForAct
////
void BG_JobDistributionForAct::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_act"), &BG_JobDistributionForAct::get_act);
	ClassDB::bind_method(D_METHOD("get_min_week"), &BG_JobDistributionForAct::get_min_week);
	ClassDB::bind_method(D_METHOD("get_max_week"), &BG_JobDistributionForAct::get_max_week);
	ClassDB::bind_method(D_METHOD("get_min_job_spawn_count"), &BG_JobDistributionForAct::get_min_job_spawn_count);
	ClassDB::bind_method(D_METHOD("get_max_job_spawn_count"), &BG_JobDistributionForAct::get_max_job_spawn_count);
}

////
//// BG_EventResourceDetails
////
void BG_EventResourceDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_maelstrite_amount_of_drops_during_range"), &BG_EventResourceDetails::get_maelstrite_amount_of_drops_during_range);
	ClassDB::bind_method(D_METHOD("get_maelstrite_total_amount_by_end_of_weeks"), &BG_EventResourceDetails::get_maelstrite_total_amount_by_end_of_weeks);
}

////
//// BG_JobMonsterDetails
////
void BG_JobMonsterDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_monster_id"), &BG_JobMonsterDetails::get_monster_id);
	ClassDB::bind_method(D_METHOD("get_monster_count_range"), &BG_JobMonsterDetails::get_monster_count_range);
	ClassDB::bind_method(D_METHOD("get_drops"), &BG_JobMonsterDetails::get_drops);
	ClassDB::bind_method(D_METHOD("get_always_drop_at_least_one"), &BG_JobMonsterDetails::get_always_drop_at_least_one);
}

BG_JobMonsterDetails::~BG_JobMonsterDetails()
{
	for (int i = 0; i < drops.size(); ++i) {
		BG_RewardItem *d = cast_to<BG_RewardItem>(drops[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

////
//// BG_JobDetails
////
void BG_JobDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_JobDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_battle_board_event_type"), &BG_JobDetails::get_battle_board_event_type);
	ClassDB::bind_method(D_METHOD("get_weeks_before_expire"), &BG_JobDetails::get_weeks_before_expire);
	ClassDB::bind_method(D_METHOD("get_monster_details"), &BG_JobDetails::get_monster_details);
	ClassDB::bind_method(D_METHOD("get_event_resource_details"), &BG_JobDetails::get_event_resource_details);
	ClassDB::bind_method(D_METHOD("get_can_spawn_in_day_time"), &BG_JobDetails::get_can_spawn_in_day_time);
	ClassDB::bind_method(D_METHOD("get_is_unique"), &BG_JobDetails::get_is_unique);
	ClassDB::bind_method(D_METHOD("get_is_boss"), &BG_JobDetails::get_is_boss);
	ClassDB::bind_method(D_METHOD("get_distribution_per_act"), &BG_JobDetails::get_distribution_per_act);

	BIND_ENUM_CONSTANT(JOB);
	BIND_ENUM_CONSTANT(RESOURCE);
	BIND_ENUM_CONSTANT(EVENT);
}

BG_JobDetails::~BG_JobDetails()
{
	for (int i = 0; i < monster_details.size(); ++i) {
		BG_JobMonsterDetails *d = cast_to<BG_JobMonsterDetails>(monster_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	if (BG_Booker_DB::bg_is_instance_valid(event_resource_details))
		memdelete(event_resource_details);
	for (int i = 0; i < distribution_per_act.size(); ++i) {
		BG_JobDistributionForAct *d = cast_to<BG_JobDistributionForAct>(distribution_per_act[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

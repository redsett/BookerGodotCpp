#pragma once

// Data classes for units: band members, formations, bands, castes, jobs, monsters, job details.
// Part of bg_booker_data_base.hpp: include that one, not this.

#include "bg_booker_types_fwd.hpp"


////
//// BG_JobDistributionForAct
////
class BG_JobDistributionForAct : public Object
{
	GDCLASS(BG_JobDistributionForAct, Object);

protected:
	static void _bind_methods();

public:
	int act = 0;
	int get_act() const { return act; }

	int min_week = 0;
	int get_min_week() const { return min_week; }

	int max_week = 0;
	int get_max_week() const { return max_week; }
	
	int min_job_spawn_count = 0;
	int get_min_job_spawn_count() const { return min_job_spawn_count; }

	int max_job_spawn_count = 0;
	int get_max_job_spawn_count() const { return max_job_spawn_count; }
};


////
//// BG_JobMonsterDetails
////
class BG_JobMonsterDetails : public Object
{
	GDCLASS(BG_JobMonsterDetails, Object);

protected:
	static void _bind_methods();

public:
	~BG_JobMonsterDetails();
	
	StringName monster_id;
	StringName get_monster_id() const { return monster_id; }

	Vector2i monster_count_range;
	Vector2i get_monster_count_range() const { return monster_count_range; }

	TypedArray<BG_RewardItem> drops;
	TypedArray<BG_RewardItem> get_drops() { return drops; }

	bool always_drop_at_least_one = false;
	bool get_always_drop_at_least_one() { return always_drop_at_least_one; };
};


////
//// BG_EventResourceDetails
////
class BG_EventResourceDetails : public Object
{
	GDCLASS(BG_EventResourceDetails, Object);

protected:
	static void _bind_methods();

public:
	int maelstrite_amount_of_drops_during_range = 0;
	int get_maelstrite_amount_of_drops_during_range() const { return maelstrite_amount_of_drops_during_range; }

	int maelstrite_total_amount_by_end_of_weeks = 0;
	int get_maelstrite_total_amount_by_end_of_weeks() const { return maelstrite_total_amount_by_end_of_weeks; }
};


////
//// BG_JobDetails
////
class BG_JobDetails : public Object
{
	GDCLASS(BG_JobDetails, Object);

protected:
	static void _bind_methods();

public:
	~BG_JobDetails();

	enum BattleBoardEventTypes : int32_t {
		JOB,
		RESOURCE,
		EVENT,
	};

	StringName id;
	StringName get_id() const { return id; }

	BattleBoardEventTypes battle_board_event_type = JOB;
	BattleBoardEventTypes get_battle_board_event_type() const { return battle_board_event_type; }

	int weeks_before_expire = 0;
	int get_weeks_before_expire() const { return weeks_before_expire; }

	TypedArray<BG_JobMonsterDetails> monster_details;
	TypedArray<BG_JobMonsterDetails> get_monster_details() const { return monster_details; }

	BG_EventResourceDetails *event_resource_details;
	BG_EventResourceDetails *get_event_resource_details() const { return event_resource_details; }

	bool can_spawn_in_day_time = false;
	bool get_can_spawn_in_day_time() const { return can_spawn_in_day_time; }

	bool is_unique = false;
	bool get_is_unique() const { return is_unique; }

	bool is_boss = false;
	bool get_is_boss() const { return is_boss; }

	TypedArray<BG_JobDistributionForAct> distribution_per_act;
	TypedArray<BG_JobDistributionForAct> get_distribution_per_act() const { return distribution_per_act; }
};
VARIANT_ENUM_CAST(BG_JobDetails::BattleBoardEventTypes);


////
//// BG_UnitCaste
////
class BG_UnitCaste : public Object
{
	GDCLASS(BG_UnitCaste, Object);

protected:
	static void _bind_methods();

public:
	~BG_UnitCaste();

	StringName id;
	StringName get_id() const { return id; }

	StringName parent_caste;
	StringName get_parent_caste() const { return parent_caste; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	bool is_unique_caste;
	bool get_is_unique_caste() const { return is_unique_caste; }

	TypedArray<StringName> lod_mesh_paths;
	TypedArray<StringName> get_lod_mesh_paths() const { return lod_mesh_paths; }

	Vector2 scale_min = Vector2();
	Vector2 get_scale_min() const { return scale_min; }

	Vector2 scale_max = Vector2();
	Vector2 get_scale_max() const { return scale_max; }

	Vector2 scale_min_extreme = Vector2();
	Vector2 get_scale_min_extreme() const { return scale_min_extreme; }

	Vector2 scale_max_extreme = Vector2();
	Vector2 get_scale_max_extreme() const { return scale_max_extreme; }

	int travel_distance = 0;
	int get_travel_distance() const { return travel_distance; }

	int inventory_space = 0;
	int get_inventory_space() const { return inventory_space; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	TypedArray<StringName> starting_item_ids;
	TypedArray<StringName> get_starting_item_ids() const { return starting_item_ids; }

	TypedArray<StringName> element_availability_ids;
	TypedArray<StringName> get_element_availability_ids() const { return element_availability_ids; }

	TypedArray<BG_AttributeTreeSkillData> attribute_skill_tree_datas;
	TypedArray<BG_AttributeTreeSkillData> get_attribute_skill_tree_datas() const { return attribute_skill_tree_datas; }

	float get_random_unit_caste_scale(bool x_z) const;
};


////
//// BG_BandMember
////
class BG_BandMember : public Resource
{
	GDCLASS(BG_BandMember, Resource);

protected:
	static void _bind_methods();

public:
	~BG_BandMember();

	// void _init() {
	// 	if (equipment.is_null()) {
	// 		BG_ReactiveArray *new_array = memnew(BG_ReactiveArray);
	// 		equipment = Ref(new_array);
	// 	}
	// }

	enum BandMemberTypes : int32_t {
        DEFAULT,
        PROTAGONIST,
		ALLY_MAIN_CHARACTER,
		ENEMY_MAIN_CHARACTER
	};

	StringName first_name_id;
	StringName get_first_name_id() const { return first_name_id; }
	void set_first_name_id(const StringName &value) { first_name_id = value; cached_name = StringName(); }
	
	StringName last_name_id;
	StringName get_last_name_id() const { return last_name_id; }
	void set_last_name_id(const StringName &value) { last_name_id = value; cached_name = StringName(); }

	BandMemberTypes band_member_type = BandMemberTypes::DEFAULT;
	BandMemberTypes get_band_member_type() const { return band_member_type; }
	void set_band_member_type(BandMemberTypes value) { band_member_type = value; }

	float current_percent_health = 1.0;
	float get_current_percent_health() const { return current_percent_health; }
	void set_current_percent_health(float value) { current_percent_health = value; }

	float percent_health_additive = 0.0;
	float get_percent_health_additive() const { return percent_health_additive; }
	void set_percent_health_additive(float value) { percent_health_additive = value; }

	float current_percent_action_points = 1.0;
	float get_current_percent_action_points() const { return current_percent_action_points; }
	void set_current_percent_action_points(float value) { current_percent_action_points = value; }

	int current_action_points_fatigue = 0;
	int get_current_action_points_fatigue() const { return current_action_points_fatigue; }
	void set_current_action_points_fatigue(int value) { current_action_points_fatigue = value; }

	int slot_index = 0;
	int get_slot_index() const { return slot_index; }
	void set_slot_index(int value) { slot_index = value; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }

	bool is_band_leader = false;
	bool get_is_band_leader() const { return is_band_leader; }
	void set_is_band_leader(bool v) { is_band_leader = v; }

	int level = 1;
	int get_level() const { return level; }
	void set_level(int v) { level = v; }

	int experience = 0;
	int get_experience() const { return experience; }
	void set_experience(int v) { experience = v; }

	Vector3 scale;
	Vector3 get_scale() const { return scale; }
	void set_scale(const Vector3 &value) { scale = value; }

	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }
	void set_caste_id(const StringName &value) { caste_id = value; }

	TypedArray<BG_Item> equipment;
	TypedArray<BG_Item> get_equipment() const { return equipment; }
	void set_equipment(const TypedArray<BG_Item> &value) { equipment = value; }

	TypedArray<BG_Item> inventory;
	TypedArray<BG_Item> get_inventory() const { return inventory; }
	void set_inventory(const TypedArray<BG_Item> &value) { inventory = value; }

	bool is_city_asset = false;
	bool get_is_city_asset() const { return is_city_asset; }
	void set_is_city_asset(bool v) { is_city_asset = v; }

	Ref<class BG_Band> band;
	Ref<BG_Band> get_band() const { return band; }
	void set_band(const Ref<BG_Band> &value) { band = value; }

	BG_BattleBoard_HexTypeDetails *battle_board_hex_type_details = nullptr;
	BG_BattleBoard_HexTypeDetails *get_battle_board_hex_type_details() const { return battle_board_hex_type_details; }
	void set_battle_board_hex_type_details(BG_BattleBoard_HexTypeDetails *v) { battle_board_hex_type_details = v; }

	Dictionary attribute_upgrades; //<StringName, int>
	Dictionary get_attribute_upgrades() const { return attribute_upgrades; }
	void set_attribute_upgrades(const Dictionary &v) { attribute_upgrades = v; }

	Dictionary consumable_upgrades; //<StringName(consumable_id), int(how_many)>
	Dictionary get_consumable_upgrades() const { return consumable_upgrades; }
	void set_consumable_upgrades(const Dictionary &v) { consumable_upgrades = v; }

	// Loadout: effect ids per slot, &"" for an empty slot. The slot counts live in GDScript (they can change with gear).
	TypedArray<StringName> equipped_ability_ids; // The same ability can be in more than one slot.
	TypedArray<StringName> get_equipped_ability_ids() const { return equipped_ability_ids; }
	void set_equipped_ability_ids(const TypedArray<StringName> &v) { equipped_ability_ids = v; }

	TypedArray<StringName> equipped_passive_ids; // No duplicates.
	TypedArray<StringName> get_equipped_passive_ids() const { return equipped_passive_ids; }
	void set_equipped_passive_ids(const TypedArray<StringName> &v) { equipped_passive_ids = v; }

	bool is_dead() const { return current_percent_health <= 0.0; }

	StringName cached_name;
	StringName cached_name_language; // The language cached_name was made in.
	StringName get_name();
	StringName get_first_name();
	StringName get_last_name();
};
VARIANT_ENUM_CAST(BG_BandMember::BandMemberTypes);


////
//// BG_Formation
////
class BG_Formation : public Resource
{
	GDCLASS(BG_Formation, Resource);

protected:
	static void _bind_methods();

public:
	String id;
	String get_id() const { return id; }
	void set_id(const String &value) {
		id = value;
		if (!Engine::get_singleton()->is_editor_hint()) {
			return;
		}

		Array new_formation;

		if (id.contains("\n")) {
			PackedStringArray split_id = id.split("\n");
			for (int i = 0; i < split_id.size(); ++i) {
				TypedArray<int> this_line;
				String s = split_id[i];
				for (int x = 0; x < s.length(); ++x) {
					String character = String::chr(s[x]);
					this_line.append(character == "1" ? 1 : 0);
				}
				new_formation.append(this_line);
			}
		}
		set_formation(new_formation);
	}

	Array formation;
	Array get_formation() const { return formation; }
	void set_formation(Array value) { formation = value; }
};


////
//// BG_DefaultFormations
////
class BG_DefaultFormations : public Resource
{
	GDCLASS(BG_DefaultFormations, Resource);

protected:
	static void _bind_methods();

public:
	TypedArray<BG_Formation> formations;
	Array get_formations() const { return formations; }
	void set_formations(const Array &value) { formations = value; }
};


////
//// BG_Band
////
class BG_Band : public Resource
{
	GDCLASS(BG_Band, Resource);

protected:
	static void _bind_methods();

public:
	int unique_id = 0;
	int get_unique_id() const { return unique_id; }
	void set_unique_id(int value) { unique_id = value; }

	StringName id;
	StringName get_id() const { return id; }
	void set_id(const StringName &value) { id = value; cached_name = StringName(); }

	StringName cached_name;
	StringName cached_name_language; // The language cached_name was made in.
	StringName name;
	StringName get_name();
	void set_name(const StringName &value) { name = value; }

	StringName preset_band_id;
	StringName get_preset_band_id() const { return preset_band_id; }
	void set_preset_band_id(const StringName &value) { preset_band_id = value; }

	bool resting = false;
	bool get_resting() const { return resting; }
	void set_resting(bool value) { resting = value; }

	bool is_on_battle_board = false;
	bool get_is_on_battle_board() const { return is_on_battle_board; }
	void set_is_on_battle_board(bool value) { is_on_battle_board = value; }

	int distance_traveled_this_turn = 0;
	int get_distance_traveled_this_turn() const { return distance_traveled_this_turn; }
	void set_distance_traveled_this_turn(int value) { distance_traveled_this_turn = value; }

	bool has_done_combat_this_turn = false;
	bool get_has_done_combat_this_turn() const { return has_done_combat_this_turn; }
	void set_has_done_combat_this_turn(bool value) { has_done_combat_this_turn = value; }

	int knocked_out_turns = 0;
	int get_knocked_out_turns() const { return knocked_out_turns; }
	void set_knocked_out_turns(int value) { knocked_out_turns = value; }

	TypedArray<BG_BandMember> band_members;
	TypedArray<BG_BandMember> get_band_members() const { return band_members; }
	void set_band_members(const TypedArray<BG_BandMember> &value) { band_members = value; }

	int current_unique_job_id = -1; // -1 means that they don't have a job.
	int get_current_unique_job_id() const { return current_unique_job_id; }
	void set_current_unique_job_id(int value) { current_unique_job_id = value; }

	Dictionary band_formation; // BG_BandMember : int(cell index)
	Dictionary get_band_formation() const { return band_formation; }
	void set_band_formation(const Dictionary &value) { band_formation = value; }	

	bool has_job() const { return current_unique_job_id != -1; };

	void clear_job() { current_unique_job_id = -1; };

	bool is_band_alive() const;

	Ref<BG_BandMember> get_band_leader() const {
		if (band_members.is_empty()) return nullptr;
		for (int i = 0; i < band_members.size(); ++i) {
			Ref<BG_BandMember> bm = band_members[i];
			if (!bm.is_valid()) continue;
			if (bm->get_is_band_leader()) return bm;
		}

		// There should always be a band leader. So let's just make a random band member the leader.
		Ref<BG_BandMember> bm = band_members.pick_random();
		if (!bm.is_valid()) return nullptr;
		bm->set_is_band_leader(true);
		return bm;
	}

	void set_band_leader(const Ref<BG_BandMember> &p_band_member) {
		if (!p_band_member.is_valid()) return;
		for (int i = 0; i < band_members.size(); ++i) {
			Ref<BG_BandMember> bm = band_members[i];
			if (!bm.is_valid()) continue;
			bm->set_is_band_leader(false);
		}
		p_band_member->set_is_band_leader(true);
	}

	static int get_unique_band_id(const TypedArray<BG_Band> &existing_bands);
};


////
//// BG_BandNameInfo
////
class BG_BandNameInfo : public Object
{
	GDCLASS(BG_BandNameInfo, Object);

protected:
	static void _bind_methods();

public:
	StringName name;
	StringName get_name() const { return name; }

	bool allowed_in_random = false;
	bool get_allowed_in_random() const { return allowed_in_random; }
};


////
//// BG_Job
////
class BG_Job : public Resource
{
	GDCLASS(BG_Job, Resource);

protected:
	static void _bind_methods();

public:
	StringName job_id;
	StringName get_job_id() const { return job_id; }
	void set_job_id(const StringName &value) { job_id = value; }

	int unique_job_id = 0;
	int get_unique_job_id() const { return unique_job_id; }
	void set_unique_job_id(int value) { unique_job_id = value; }
	
	int random_seed = 0;
	int get_random_seed() const { return random_seed; }
	void set_random_seed(int value) { random_seed = value; }

	int weeks_remaining_unless_removed = 0;
	int get_weeks_remaining_unless_removed() const { return weeks_remaining_unless_removed; }
	void set_weeks_remaining_unless_removed(int value) { weeks_remaining_unless_removed = value; }

	TypedArray<class BG_Monster> monsters;
	TypedArray<BG_Monster> get_monsters() const { return monsters; }
	void set_monsters(const TypedArray<BG_Monster> &value) { monsters = value; }

	Dictionary formation; // BG_Monster : int(cell index)
	Dictionary get_formation() const { return formation; }
	void set_formation(const Dictionary &value) { formation = value; }

	bool is_on_battle_board = false;
	bool get_is_on_battle_board() const { return is_on_battle_board; }
	void set_is_on_battle_board(bool value) { is_on_battle_board = value; }

	void try_set_all_monsters_to_stoned(bool stoned);

	bool is_job_alive() const;
};


////
//// BG_Monster
////
class BG_Monster : public Resource
{
	GDCLASS(BG_Monster, Resource);

protected:
	static void _bind_methods();

public:
	~BG_Monster();

	StringName id;
	StringName get_id() const { return id; }
	void set_id(const StringName &value) { id = value; }

	StringName name;
	StringName get_name() const { return name; }

	int preferred_row = 0;
	int get_preferred_row() const { return preferred_row; }

	int max_health = 0;
	int get_max_health() const { return max_health; }

	int base_action_points = 0;
	int get_base_action_points() const { return base_action_points; }
	
	int bonus_action_points = 0;
	int get_bonus_action_points() const { return bonus_action_points; }

	int base_action_points_cost = 0;
	int get_base_action_points_cost() const { return base_action_points_cost; }

	float current_percent_action_points = 1.0;
	float get_current_percent_action_points() const { return current_percent_action_points; }
	void set_current_percent_action_points(float value) { current_percent_action_points = value; }

	int current_action_points_fatigue = 0;
	int get_current_action_points_fatigue() const { return current_action_points_fatigue; }
	void set_current_action_points_fatigue(int value) { current_action_points_fatigue = value; }

	int travel_distance = 0;
	int get_travel_distance() const { return travel_distance; }

	float current_percent_health = 1.0;
	float get_current_percent_health() const { return current_percent_health; }
	void set_current_percent_health(float value) { current_percent_health = value; }

	float percent_health_additive = 0.0;
	float get_percent_health_additive() const { return percent_health_additive; }
	void set_percent_health_additive(float value) { percent_health_additive = value; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }
	
	int level = 1;
	int get_level() const { return level; }
	void set_level(int v) { level = v; }

	int monster_type = 0;
	int get_monster_type() const { return monster_type; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	TypedArray<StringName> element_availability_ids;
	TypedArray<StringName> get_element_availability_ids() const { return element_availability_ids; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	StringName model_path;
	StringName get_model_path() const { return model_path; }

	TypedArray<StringName> effect_ids;
	TypedArray<StringName> get_effect_ids() const { return effect_ids; }

	TypedArray<StringName> out_of_combat_effect_ids;
	TypedArray<StringName> get_out_of_combat_effect_ids() const { return out_of_combat_effect_ids; }

	TypedArray<BG_HueShiftData> hue_shift_data;
	TypedArray<BG_HueShiftData> get_hue_shift_data() const { return hue_shift_data; };

	Ref<BG_Job> job;
	Ref<BG_Job> get_job() const { return job; };
	void set_job(const Ref<BG_Job> &value) { job = value; };

	bool can_be_turned_to_stone = true;
	bool get_can_be_turned_to_stone() const { return can_be_turned_to_stone; }

	bool is_turned_to_stone = false;
	bool get_is_turned_to_stone() const { return is_turned_to_stone; }
	void set_is_turned_to_stone(bool v);

	bool is_dead() const { return current_percent_health <= 0.0; }

	float effectiveness = 0.0;
	float get_effectiveness() const { return effectiveness; }

	TypedArray<BG_ContentStat> effectiveness_stats;
	TypedArray<BG_ContentStat> get_effectiveness_stats() const { return effectiveness_stats; }

	Vector2 level_range = Vector2(1, 1);
	Vector2 get_level_range() const { return level_range; }

	TypedArray<BG_AnimationDetails> animations;
	TypedArray<BG_AnimationDetails> get_animations() const { return animations; }

	TypedArray<BG_RewardItem> drops;
	TypedArray<BG_RewardItem> get_drops() const { return drops; }

	float action_points_fatigue_rate_multiplier = 1.0;
	float get_action_points_fatigue_rate_multiplier() const { return action_points_fatigue_rate_multiplier; }
};


////
//// BG_BandInfo
////
class BG_BandInfo : public Object
{
	GDCLASS(BG_BandInfo, Object);

protected:
	static void _bind_methods();

public:
	~BG_BandInfo();

	TypedArray<BG_BandNameInfo> band_names;
	TypedArray<BG_BandNameInfo> get_band_names() const { return band_names; }

	TypedArray<StringName> icon_paths;
	TypedArray<StringName> get_icon_paths() const { return icon_paths; }

	TypedArray<BG_BandNameInfo> first_names;
	TypedArray<BG_BandNameInfo> get_first_names() const { return first_names; }

	TypedArray<BG_BandNameInfo> last_names;
	TypedArray<BG_BandNameInfo> get_last_names() const { return last_names; }

	Vector2 band_size_min_max;
	Vector2 get_band_size_min_max() const { return band_size_min_max; }

	int num_band_members_for_hire = 0;
	int get_num_band_members_for_hire() const { return num_band_members_for_hire; }

	TypedArray<BG_UnitCaste> unit_castes;
	TypedArray<BG_UnitCaste> get_unit_castes() const { return unit_castes; }
	BG_UnitCaste *get_caste_by_id(const StringName &id, bool return_parent) const;

	float rest_recovery_speed = 0.0;
	float get_rest_recovery_speed() const { return rest_recovery_speed; }

	int knocked_out_turns = 0;
	int get_knocked_out_turns() const { return knocked_out_turns; }

};

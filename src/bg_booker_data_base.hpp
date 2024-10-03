#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

////
//// BG_AudioData
////
class BG_AudioData : public Object
{
	GDCLASS(BG_AudioData, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	TypedArray<StringName> act_file_paths;
	TypedArray<StringName> get_act_file_paths() const { return act_file_paths; }

	TypedArray<int> act_volume_db_base_values;
	TypedArray<int> get_act_volume_db_base_values() const { return act_volume_db_base_values; }
};

////
//// BG_HueShiftData
////
class BG_HueShiftData : public Object
{
	GDCLASS(BG_HueShiftData, Object);

protected:
	static void _bind_methods();

public:
	StringName mask_path;
	StringName get_mask_path() const { return mask_path; }

	Color from_color = Color(1.0, 1.0, 1.0);
	Color get_from_color() const { return from_color; }

	float multiplier;
	float get_multiplier() const { return multiplier; }
};

////
//// BG_EffectRarityDetails
////
class BG_EffectRarityDetails : public Object
{
	GDCLASS(BG_EffectRarityDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName rarity_id;
	StringName get_rarity_id() const { return rarity_id; }

	StringName description;
	StringName get_description() const { return description; }

	StringName script_path;
	StringName get_script_path() const { return script_path; }

	Dictionary value_attributes;
	Dictionary get_value_attributes() const { return value_attributes; }
};

////
//// BG_Effect
////
class BG_Effect : public Object
{
	GDCLASS(BG_Effect, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	TypedArray<BG_EffectRarityDetails> details_per_rarity;
	TypedArray<BG_EffectRarityDetails> get_details_per_rarity() const { return details_per_rarity; }

	bool use_owning_item_icon = false;
	bool get_use_owning_item_icon() const { return use_owning_item_icon; }

	StringName status_icon_path;
	StringName get_status_icon_path() const { return status_icon_path; }
};

////
//// BG_Dice
////
class BG_Dice : public Object
{
	GDCLASS(BG_Dice, Object);

protected:
	static void _bind_methods();

public:
	int roll_count = 0;
	int get_roll_count() const { return roll_count; }
	void set_roll_count(int value) { roll_count = value; }

	int amount_of_sides = 0;
	int get_amount_of_sides() const { return amount_of_sides; }
	void set_amount_of_sides(int value) { amount_of_sides = value; }

	int additive = 0;
	int get_additive() const { return additive; }
	void set_additive(int value) { additive = value; }

	static int calculate_dice(const TypedArray<BG_Dice> dice, RandomNumberGenerator *random_num_generator = nullptr);
	static int get_dice_max_roll(const TypedArray<BG_Dice> dice);
	static String dice_to_nice_name(const TypedArray<BG_Dice> dice);
	static String dice_to_string(const BG_Dice *dice);
	static BG_Dice *string_to_dice(String string);
	static TypedArray<BG_Dice> string_to_dice_options(String string);
};

////
//// BG_RarityDetails
////
class BG_RarityDetails : public Object
{
	GDCLASS(BG_RarityDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName equipment_prefix_name;
	StringName get_equipment_prefix_name() const { return equipment_prefix_name; }

	StringName beast_part_prefix_name;
	StringName get_beast_part_prefix_name() const { return beast_part_prefix_name; }

	Color color;
	Color get_color() const { return color; }

	float damage_multiplier = 0.0f;
	float get_damage_multiplier() const { return damage_multiplier; }

	TypedArray<float> percentage_of_all_items_dropped_per_act;
	TypedArray<float> get_percentage_of_all_items_dropped_per_act() const { return percentage_of_all_items_dropped_per_act; }
};

////
//// BG_LoreRarity
////
class BG_LoreRarity : public Object
{
	GDCLASS(BG_LoreRarity, Object);

protected:
	static void _bind_methods();

public:
	StringName rarity_id;
	StringName get_rarity_id() const { return rarity_id; }

	StringName description;
	StringName get_description() const { return description; }
};

////
//// BG_UnitStatDetails
////
class BG_UnitStatDetails : public Object
{
	GDCLASS(BG_UnitStatDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	String icon_path = "";
	String get_icon_path() const { return icon_path; }

	bool is_damage_type = false;
	bool get_is_damage_type() const { return is_damage_type; }

	Color color = Color(1.0, 1.0, 1.0);
	Color get_color() const { return color; }

	Color text_color = Color(1.0, 1.0, 1.0);
	Color get_text_color() const { return text_color; }

	Color in_world_color = Color(1.0, 1.0, 1.0);
	Color get_in_world_color() const { return in_world_color; }
};

////
//// BG_UnitStat
////
class BG_UnitStat : public Object
{
	GDCLASS(BG_UnitStat, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	float bonus_percentage = 0.0f;
	float get_bonus_percentage() const { return bonus_percentage; }

	int offensive_value = 0;
	int get_offensive_value() const { return offensive_value; }

	int defensive_value = 0;
	int get_defensive_value() const { return defensive_value; }

	int resistant_unsaved_stored_value = 0;
	int get_resistant_unsaved_stored_value() const { return resistant_unsaved_stored_value; }
	void set_resistant_unsaved_stored_value(int v) { resistant_unsaved_stored_value = v; }

	StringName resistant_value_text;
	StringName get_resistant_value_text() const { return resistant_value_text; }

	Vector2i resistant_value_min_max;
	Vector2i get_resistant_value_min_max() const { return resistant_value_min_max; }
	void set_resistant_value_min_max(Vector2i v) { resistant_value_min_max = v; }

	StringName dice_string;
	StringName get_dice_string() const { return dice_string; }

	TypedArray<BG_Dice> dice_options;
	TypedArray<BG_Dice> get_dice_options() const { return dice_options; }

	BG_Dice *dice = nullptr;
	BG_Dice *get_dice() const { return dice; }
	void set_dice(BG_Dice *value) { dice = value; }

	static Vector2i string_to_resistant_value_min_max(String string);
};

////
//// BG_ItemSlotType
////
class BG_ItemSlotType : public Object
{
	GDCLASS(BG_ItemSlotType, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName name;
	StringName get_name() const { return name; }

	TypedArray<float> percentage_of_all_items_dropped_per_act;
	TypedArray<float> get_percentage_of_all_items_dropped_per_act() const { return percentage_of_all_items_dropped_per_act; }
};

////
//// BG_Item
////
class BG_Item : public Resource
{
	GDCLASS(BG_Item, Resource);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName v) { id = v; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }
	
	bool on_shelf = false;
	bool get_on_shelf() const { return on_shelf; }
	void set_on_shelf(bool v) { on_shelf = v; }

	int shelf_index = 0;
	int get_shelf_index() const { return shelf_index; }
	void set_shelf_index(int v) { shelf_index = v; }

	bool has_bid = false;
	bool get_has_bid() const { return has_bid; }
	void set_has_bid(bool v) { has_bid = v; }

	int bid_amount = 0;
	int get_bid_amount() const { return bid_amount; }
	void set_bid_amount(int v) { bid_amount = v; }

	bool is_equipped = false;
	bool get_is_equipped() const { return is_equipped; }
	void set_is_equipped(bool v) { is_equipped = v; }

	float current_durability = 0.0f;
	float get_current_durability() const { return current_durability; }
	void set_current_durability(float v) { current_durability = v; }

	float current_fame = 0.0f;
	float get_current_fame() const { return current_fame; }
	void set_current_fame(float v) { current_fame = v; }

	int weeks_left_to_finish_graft = 0;
	int get_weeks_left_to_finish_graft() const { return weeks_left_to_finish_graft; }
	void set_weeks_left_to_finish_graft(int v) { weeks_left_to_finish_graft = v; }

	int number_of_jobs_participated_in = 0;
	int get_number_of_jobs_participated_in() const { return number_of_jobs_participated_in; }
	void set_number_of_jobs_participated_in(int v) { number_of_jobs_participated_in = v; }

	int consecutive_number_of_jobs_participated_in = 0;
	int get_consecutive_number_of_jobs_participated_in() const { return consecutive_number_of_jobs_participated_in; }
	void set_consecutive_number_of_jobs_participated_in(int v) { consecutive_number_of_jobs_participated_in = v; }

	bool should_count_in_item_rarity_disparity = true;
	bool get_should_count_in_item_rarity_disparity() const { return should_count_in_item_rarity_disparity; }
	void set_should_count_in_item_rarity_disparity(bool v) { should_count_in_item_rarity_disparity = v; }

	StringName rarity_id;
	StringName get_rarity_id() const { return rarity_id; }
	void set_rarity_id(StringName value) { rarity_id = value; }

	TypedArray<BG_Item> grafts;
	TypedArray<BG_Item> get_grafts() const { return grafts; }
	void set_grafts(TypedArray<BG_Item> value) { grafts = value; }
};

////
//// BG_ItemDetails
////
class BG_ItemDetails : public Object
{
	GDCLASS(BG_ItemDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName name;
	StringName get_name() const { return name; }

	bool is_beast_part = false;
	bool get_is_beast_part() const { return is_beast_part; }

	StringName slot_type_id;
	StringName get_slot_type_id() { return slot_type_id; }

	bool is_useable_item = false;
	bool get_is_useable_item() { return is_useable_item; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	BG_HueShiftData *hue_shift_data = nullptr;
	BG_HueShiftData *get_hue_shift_data() const { return hue_shift_data; };

	StringName mesh_path;
	StringName get_mesh_path() const { return mesh_path; }

	TypedArray<BG_LoreRarity> lore;
	TypedArray<BG_LoreRarity> get_lore() const { return lore; }

	TypedArray<StringName> caste_ids;
	TypedArray<StringName> get_caste_ids() const { return caste_ids; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	int animation_attach_socket = 0;
	int get_animation_attach_socket() const { return animation_attach_socket; }

	TypedArray<StringName> effect_ids;
	TypedArray<StringName> get_effect_ids() const { return effect_ids; }

	int sell_value_tier = 0;
	int get_sell_value_tier() const { return sell_value_tier; }

	int fame_value_tier = 0;
	int get_fame_value_tier() const { return fame_value_tier; }

	int durability_value_tier = 0;
	int get_durability_value_tier() const { return durability_value_tier; }	
};

////
//// BG_RewardItem
////
class BG_RewardItem : public Object
{
	GDCLASS(BG_RewardItem, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	TypedArray<StringName> rarity_availabilities;
	TypedArray<StringName> get_rarity_availabilities() const { return rarity_availabilities; }

	float drop_weight = 0.0;
	float get_drop_weight() const { return drop_weight; }
};

////
//// BG_ItemDropPool
////
class BG_ItemDropPool : public Object
{
	GDCLASS(BG_ItemDropPool, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_RewardItem> item_drops;
	TypedArray<BG_RewardItem> get_item_drops() const { return item_drops; }
};

////
//// BG_JobDistributionForAct
////
class BG_JobDistributionForAct : public Object
{
	GDCLASS(BG_JobDistributionForAct, Object);

protected:
	static void _bind_methods();

public:
	int act;
	int get_act() const { return act; }

	int min_week;
	int get_min_week() const { return min_week; }

	int max_week;
	int get_max_week() const { return max_week; }
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
//// BG_JobDetails
////
class BG_JobDetails : public Object
{
	GDCLASS(BG_JobDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	int weeks_to_complete = 0;
	int get_weeks_to_complete() const { return weeks_to_complete; }

	int weeks_before_expire = 0;
	int get_weeks_before_expire() const { return weeks_before_expire; }

	TypedArray<BG_JobMonsterDetails> monster_details;
	TypedArray<BG_JobMonsterDetails> get_monster_details() const { return monster_details; }

	bool is_unique = false;
	bool get_is_unique() const { return is_unique; }

	bool is_boss = false;
	bool get_is_boss() const { return is_boss; }

	TypedArray<BG_JobDistributionForAct> distribution_per_act;
	TypedArray<BG_JobDistributionForAct> get_distribution_per_act() const { return distribution_per_act; }
};

////
//// BG_UnitCaste
////
class BG_UnitCaste : public Object
{
	GDCLASS(BG_UnitCaste, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName name;
	StringName get_name() const { return name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	BG_HueShiftData *hue_shift_data = nullptr;
	BG_HueShiftData *get_hue_shift_data() const { return hue_shift_data; };

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

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	TypedArray<StringName> starting_item_ids;
	TypedArray<StringName> get_starting_item_ids() const { return starting_item_ids; }

	TypedArray<StringName> element_availability_ids;
	TypedArray<StringName> get_element_availability_ids() const { return element_availability_ids; }
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
	StringName name;
	StringName get_name() const { return name; }
	void set_name(StringName value) { name = value; }

	int current_health = 0;
	int get_current_health() const { return current_health; }
	void set_current_health(int value) { current_health = value; }

	int slot_index = 0;
	int get_slot_index() const { return slot_index; }
	void set_slot_index(int value) { slot_index = value; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }

	Vector3 scale;
	Vector3 get_scale() const { return scale; }
	void set_scale(Vector3 value) { scale = value; }

	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }
	void set_caste_id(StringName value) { caste_id = value; }

	TypedArray<BG_Item> equipment;
	TypedArray<BG_Item> get_equipment() const { return equipment; }
	void set_equipment(TypedArray<BG_Item> value) { equipment = value; }

	bool is_city_asset = false;
	bool get_is_city_asset() const { return is_city_asset; }
	void set_is_city_asset(bool v) { is_city_asset = v; }
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
	StringName name;
	StringName get_name() const { return name; }
	void set_name(StringName value) { name = value; }

	bool resting = false;
	bool get_resting() const { return resting; }
	void set_resting(bool value) { resting = value; }

	TypedArray<BG_BandMember> band_members;
	TypedArray<BG_BandMember> get_band_members() const { return band_members; }
	void set_band_members(TypedArray<BG_BandMember> value) { band_members = value; }

	int current_unique_job_id = -1; // -1 means that they don't have a job.
	int get_current_unique_job_id() const { return current_unique_job_id; }
	void set_current_unique_job_id(int value) { current_unique_job_id = value; }

	bool has_job() const { return current_unique_job_id != -1; };

	void clear_job() { current_unique_job_id = -1; };
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
	StringName band_name;
	StringName get_band_name() const { return band_name; }

	TypedArray<String> hiring_dialogue_choices;
	TypedArray<String> get_hiring_dialogue_choices() const { return hiring_dialogue_choices; }
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
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	StringName name;
	StringName get_name() const { return name; }

	int max_health = 0;
	int get_max_health() const { return max_health; }

	int current_health = 0;
	int get_current_health() const { return current_health; }
	void set_current_health(int value) { current_health = value; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }

	float challenge_rating = 0.0f;
	float get_challenge_rating() const { return challenge_rating; }

	int monster_type = 0;
	int get_monster_type() const { return monster_type; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	TypedArray<StringName> element_availability_ids;
	TypedArray<StringName> get_element_availability_ids() const { return element_availability_ids; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	TypedArray<StringName> effect_ids;
	TypedArray<StringName> get_effect_ids() const { return effect_ids; }

	BG_HueShiftData *hue_shift_data = nullptr;
	BG_HueShiftData *get_hue_shift_data() const { return hue_shift_data; };

	String get_challenge_rating_fraction_string() const;
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
	TypedArray<BG_BandNameInfo> band_names;
	TypedArray<BG_BandNameInfo> get_band_names() const { return band_names; }

	TypedArray<StringName> icon_paths;
	TypedArray<StringName> get_icon_paths() const { return icon_paths; }

	TypedArray<StringName> first_names;
	TypedArray<StringName> get_first_names() const { return first_names; }

	TypedArray<StringName> last_names;
	TypedArray<StringName> get_last_names() const { return last_names; }

	Vector2 band_size_min_max;
	Vector2 get_band_size_min_max() const { return band_size_min_max; }

	int num_bands_for_hire = 0;
	int get_num_bands_for_hire() const { return num_bands_for_hire; }

	TypedArray<BG_UnitCaste> unit_castes;
	TypedArray<BG_UnitCaste> get_unit_castes() const { return unit_castes; }
};

////
//// BG_ChallengeRatingGuide
////
class BG_ChallengeRatingGuide : public Object
{
	GDCLASS(BG_ChallengeRatingGuide, Object);

protected:
	static void _bind_methods();

public:
	Vector2 cr_min_max = Vector2();
	Vector2 get_cr_min_max() const { return cr_min_max; }

	int job_rep_reward = 0;
	int get_job_rep_reward() const { return job_rep_reward; }

	int job_duralation = 0;
	int get_job_duralation() const { return job_duralation; }

	float item_durability_consumption = 0.0f;
	float get_item_durability_consumption() const { return item_durability_consumption; }

	float item_fame_addition = 0.0f;
	float get_item_fame_addition() const { return item_fame_addition; }
};

////
//// BG_ActStats
////
class BG_ActStats : public Object
{
	GDCLASS(BG_ActStats, Object);

protected:
	static void _bind_methods();

public:
	int total_week_count = 0;
	int get_total_week_count() const { return total_week_count; }

	int total_job_count = 0;
	int get_total_job_count() const { return total_job_count; }

	StringName job_handout_curve_path;
	StringName get_job_handout_curve_path() const { return job_handout_curve_path; }
};

////
//// BG_EquipmentAnimationDetails
////
class BG_EquipmentAnimationDetails : public Object
{
	GDCLASS(BG_EquipmentAnimationDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }

	TypedArray<StringName> equipment_ids;
	TypedArray<StringName> get_equipment_ids() const { return equipment_ids; }

	StringName in_game_animation_name;
	StringName get_in_game_animation_name() const { return in_game_animation_name; }
};

////
//// BG_BookerSkillTreeSlotDetails
////
class BG_BookerSkillTreeSlotDetails : public Object
{
	GDCLASS(BG_BookerSkillTreeSlotDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	bool is_parent_button;
	bool get_is_parent_button() const { return is_parent_button; }

	StringName skill_name;
	StringName get_skill_name() const { return skill_name; }

	StringName skill_description;
	StringName get_skill_description() const { return skill_description; }

	int required_rep;
	int get_required_rep() const { return required_rep; }

	StringName parent_skill_id;
	StringName get_parent_skill_id() const { return parent_skill_id; }
};

////
//// BG_CityInfo
////
class BG_CityInfo : public Object
{
	GDCLASS(BG_CityInfo, Object);

protected:
	static void _bind_methods();

public:
	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	int max_health;
	int get_max_health() const { return max_health; }

	TypedArray<StringName> equipment_ids;
	TypedArray<StringName> get_equipment_ids() const { return equipment_ids; }
};

////
//// BG_Booker_Globals
////
class BG_Booker_Globals : public Object
{
	GDCLASS(BG_Booker_Globals, Object);

protected:
	static void _bind_methods();

public:
	BG_Booker_Globals() {};
	~BG_Booker_Globals() {};

	int starting_reputation = 0;
	int get_starting_reputation() const { return starting_reputation; }

	float chance_of_no_drop = 0.0;
	float get_chance_of_no_drop() const { return chance_of_no_drop; }

	TypedArray<BG_CityInfo> city_info;
	TypedArray<BG_CityInfo> get_city_info() const { return city_info; }

	//MyCustomClass* result = dynamic_cast<MyCustomClass*>(container.object());
	TypedArray<BG_ActStats> act_stats;
	TypedArray<BG_ActStats> get_act_stats() const { return act_stats; }
	
	TypedArray<BG_ChallengeRatingGuide> challenge_rating_guide;
	TypedArray<BG_ChallengeRatingGuide> get_challenge_rating_guide() const { return challenge_rating_guide; }

	TypedArray<float> monster_element_distribution;
	TypedArray<float> get_monster_element_distribution() const { return monster_element_distribution; }

	// Teir Values
	TypedArray<int> sell_tier_values;
	TypedArray<int> get_sell_tier_values() const { return sell_tier_values; }

	TypedArray<int> fame_tier_values;
	TypedArray<int> get_fame_tier_values() const { return fame_tier_values; }

	TypedArray<int> durability_teir_values;
	TypedArray<int> get_durability_teir_values() const { return durability_teir_values; }

	// Teir Value Multipliers
	TypedArray<float> equipment_value_rarity_multiplier;
	TypedArray<float> get_equipment_value_rarity_multiplier() const { return equipment_value_rarity_multiplier; }

	TypedArray<float> beast_part_value_rarity_multiplier;
	TypedArray<float> get_beast_part_value_rarity_multiplier() const { return beast_part_value_rarity_multiplier; }

	TypedArray<float> extra_beast_part_value_rarity_multiplier_while_grafted;
	TypedArray<float> get_extra_beast_part_value_rarity_multiplier_while_grafted() const { return extra_beast_part_value_rarity_multiplier_while_grafted; }

	TypedArray<float> equipment_durability_rarity_multiplier;
	TypedArray<float> get_equipment_durability_rarity_multiplier() const { return equipment_durability_rarity_multiplier; }

	TypedArray<float> beast_part_durability_rarity_multiplier;
	TypedArray<float> get_beast_part_durability_rarity_multiplier() const { return beast_part_durability_rarity_multiplier; }

	TypedArray<float> equipment_fame_rarity_multiplier;
	TypedArray<float> get_equipment_fame_rarity_multiplier() const { return equipment_fame_rarity_multiplier; }

	TypedArray<float> beast_part_fame_rarity_multiplier;
	TypedArray<float> get_beast_part_fame_rarity_multiplier() const { return beast_part_fame_rarity_multiplier; }

	float inventory_sell_multiplier = 0.0;
	float get_inventory_sell_multiplier() const { return inventory_sell_multiplier; }

	float item_passive_income_multiplier = 0.0;
	float get_item_passive_income_multiplier() const { return item_passive_income_multiplier; }
};

////
//// BG_Booker_DB
////
class BG_Booker_DB : public Object
{
	GDCLASS(BG_Booker_DB, Object);

	static BG_Booker_DB *singleton;

protected:
	static void _bind_methods();

public:
	BG_Booker_DB();
	~BG_Booker_DB();

	static BG_Booker_DB *get_singleton();

	void refresh_data();

	static void timer_test(Callable callable);

	String modding_path = "";
	String get_modding_path() const { return modding_path; }

	BG_Booker_Globals *globals = nullptr;
	BG_Booker_Globals *get_globals() const { return globals; }

	TypedArray<BG_AudioData> audio_data;
	TypedArray<BG_AudioData> get_audio_data() const { return audio_data; }

	TypedArray<BG_BookerSkillTreeSlotDetails> booker_skill_tree_details;
	TypedArray<BG_BookerSkillTreeSlotDetails> get_booker_skill_tree_details() const { return booker_skill_tree_details; }

	TypedArray<BG_JobDetails> jobs;
	TypedArray<BG_JobDetails> get_jobs() const { return jobs; }

	TypedArray<BG_ItemDetails> items;
	TypedArray<BG_ItemDetails> get_items() const { return items; }

	TypedArray<BG_ItemDropPool> item_drop_pools;
	TypedArray<BG_ItemDropPool> get_item_drop_pools() const { return item_drop_pools; }

	TypedArray<BG_Effect> effects;
	TypedArray<BG_Effect> get_effects() const { return effects; }

	TypedArray<BG_EquipmentAnimationDetails> equipment_animation_details;
	TypedArray<BG_EquipmentAnimationDetails> get_equipment_animation_details() const { return equipment_animation_details; }

	BG_BandInfo *band_info = nullptr;
	BG_BandInfo *get_band_info() const { return band_info; }

	TypedArray<BG_ItemSlotType> item_slot_types;
	TypedArray<BG_ItemSlotType> get_item_slot_types() const { return item_slot_types; }

	TypedArray<BG_RarityDetails> rarity_types;
	TypedArray<BG_RarityDetails> get_rarity_types() const { return rarity_types; }

	TypedArray<BG_UnitStatDetails> stat_types;
	TypedArray<BG_UnitStatDetails> get_stat_types() const { return stat_types; }

	TypedArray<BG_Monster> monster_types;
	TypedArray<BG_Monster> get_monster_types() const { return monster_types; }

	static float get_job_challenge_rating_value(const TypedArray<BG_Monster> monsters);
	static String get_job_challenge_rating(const TypedArray<BG_Monster> monsters);

private:
	Dictionary data;

	void try_parse_data(const String &file_path);
};

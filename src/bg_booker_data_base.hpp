#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

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

	StringName description;
	StringName get_description() const { return description; }

	StringName script_path;
	StringName get_script_path() const { return script_path; }

	StringName status_icon_path;
	StringName get_status_icon_path() const { return status_icon_path; }

	Dictionary value_attributes;
	Dictionary get_value_attributes() const { return value_attributes; }
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
	static String dice_to_nice_name(const TypedArray<BG_Dice> dice);
	static String dice_to_string(const BG_Dice *dice);
	static BG_Dice *string_to_dice(String string);
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

	String icon_path = "";
	String get_icon_path() const { return icon_path; }

	bool is_damage_type = false;
	bool get_is_damage_type() const { return is_damage_type; }

	Color color = Color(1.0, 1.0, 1.0);
	Color get_color() const { return color; }
};

////
//// BG_UnitStat
////
class BG_UnitStat : public Resource
{
	GDCLASS(BG_UnitStat, Resource);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	float bonus_percentage = 0.0f;
	float get_bonus_percentage() const { return bonus_percentage; }
	void set_bonus_percentage(float v) { bonus_percentage = v; }

	int offensive_value = 0;
	int get_offensive_value() const { return offensive_value; }
	void set_offensive_value(int v) { offensive_value = v; }

	int defensive_value = 0;
	int get_defensive_value() const { return defensive_value; }
	void set_defensive_value(int v) { defensive_value = v; }

	int resistant_value = 0;
	int get_resistant_value() const { return resistant_value; }
	void set_resistant_value(int v) { resistant_value = v; }

	BG_Dice *dice = nullptr;
	BG_Dice *get_dice() const { return dice; }
	void set_dice(BG_Dice *value) { dice = value; }
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

	int number_of_jobs_participated_in = 0;
	int get_number_of_jobs_participated_in() const { return number_of_jobs_participated_in; }
	void set_number_of_jobs_participated_in(int v) { number_of_jobs_participated_in = v; }

	int consecutive_number_of_jobs_participated_in = 0;
	int get_consecutive_number_of_jobs_participated_in() const { return consecutive_number_of_jobs_participated_in; }
	void set_consecutive_number_of_jobs_participated_in(int v) { consecutive_number_of_jobs_participated_in = v; }

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

	int act_introduced_in = 0;
	int get_act_introduced_in() const { return act_introduced_in; }

	int base_value_override = 0;
	int get_base_value_override() const { return base_value_override; }

	int hands = 0;
	int get_hands() const { return hands; }

	bool is_beast_part = false;
	bool get_is_beast_part() const { return is_beast_part; }

	StringName slot_type_id;
	StringName get_slot_type_id() { return slot_type_id; }

	bool is_useable_item = false;
	bool get_is_useable_item() { return is_useable_item; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	StringName mesh_path;
	StringName get_mesh_path() const { return mesh_path; }

	TypedArray<BG_LoreRarity> lore;
	TypedArray<BG_LoreRarity> get_lore() const { return lore; }

	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	StringName ability_id;
	StringName get_ability_id() const { return ability_id; }

	TypedArray<StringName> effect_ids;
	TypedArray<StringName> get_effect_ids() const { return effect_ids; }
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

	float drop_rate = 0.0;
	float get_drop_rate() const { return drop_rate; }
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

	StringName name;
	StringName get_name() const { return name; }

	int level = 0;
	int get_level() const { return level; }

	StringName description;
	StringName get_description() const { return description; }

	//FString clanmember;

	int weeks = 0;
	int get_weeks() const { return weeks; }

	int weeks_before_expire = 0;
	int get_weeks_before_expire() const { return weeks_before_expire; }

	TypedArray<StringName> monsters_ids;
	TypedArray<StringName> get_monsters_ids() const { return monsters_ids; }

	TypedArray<float> monsters_spawn_chances;
	TypedArray<float> get_monsters_spawn_chances() const { return monsters_spawn_chances; }

	Vector2i monster_count_range;
	Vector2i get_monster_count_range() const { return monster_count_range; }

	bool is_unique = false;
	bool get_is_unique() const { return is_unique; }

	bool is_boss = false;
	bool get_is_boss() const { return is_boss; }

	TypedArray<int> acts_allowed_in;
	TypedArray<int> get_acts_allowed_in() const { return acts_allowed_in; }

	float drop_rate_adder = 0.0f;
	float get_drop_rate_adder() const { return drop_rate_adder; }
};

////
//// BG_UnitCaste
////
class BG_UnitCaste : public Resource
{
	GDCLASS(BG_UnitCaste, Resource);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName p_value) { id = p_value; }

	StringName name;
	StringName get_name() const { return name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	StringName mesh_path;
	StringName get_mesh_path() const { return mesh_path; }

	StringName proxy_mesh_path;
	StringName get_proxy_mesh_path() const { return proxy_mesh_path; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }
	void set_stats(TypedArray<BG_UnitStat> p_value) { stats = p_value; }

	TypedArray<StringName> starting_item_ids;
	TypedArray<StringName> get_starting_item_ids() const { return starting_item_ids; }
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

	int level = 1;
	int get_level() const { return level; }
	void set_level(int value) { level = value; }

	float experience = 0.0;
	float get_experience() const { return experience; }
	void set_experience(float value) { experience = value; }

	int slot_index = 0;
	int get_slot_index() const { return slot_index; }
	void set_slot_index(int value) { slot_index = value; }

	Vector3 scale;
	Vector3 get_scale() const { return scale; }
	void set_scale(Vector3 value) { scale = value; }

	StringName personality_dialgue_id;
	StringName get_personality_dialgue_id() const { return personality_dialgue_id; }
	void set_personality_dialgue_id(StringName value) { personality_dialgue_id = value; }

	Ref<BG_UnitCaste> caste = nullptr;
	Ref<BG_UnitCaste> get_caste() const { return caste; }
	void set_caste(Ref<BG_UnitCaste> value) { caste = value; }

	TypedArray<BG_Item> equipment;
	TypedArray<BG_Item> get_equipment() const { return equipment; }
	void set_equipment(TypedArray<BG_Item> value) { equipment = value; }
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
//// BG_PersonalityDialgue
////
class BG_PersonalityDialgue : public Object
{
	GDCLASS(BG_PersonalityDialgue, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	// TArray<FString> willWin;
	// TArray<FString> barelyWillWin;
	// TArray<FString> bandTooWeak;
	// TArray<FString> fatigued;
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

	float challenge_rating = 0.0f;
	float get_challenge_rating() const { return challenge_rating; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	TypedArray<StringName> effect_ids;
	TypedArray<StringName> get_effect_ids() const { return effect_ids; }

	TypedArray<BG_RewardItem> beast_part_rewards;
	TypedArray<BG_RewardItem> get_beast_part_rewards() const { return beast_part_rewards; }

	TypedArray<BG_RewardItem> equipment_rewards;
	TypedArray<BG_RewardItem> get_equipment_rewards() const { return equipment_rewards; }

	String get_challenge_rating_faction_string() const;
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

	TypedArray<BG_PersonalityDialgue> personality_dialgue;
	TypedArray<BG_PersonalityDialgue> get_personality_dialgue() const { return personality_dialgue; }

	TypedArray<int> level_min;
	TypedArray<int> get_level_min() const { return level_min; }

	TypedArray<int> level_max;
	TypedArray<int> get_level_max() const { return level_max; }

	TypedArray<float> monthly_cost_level_multiplier;
	TypedArray<float> get_monthly_cost_level_multiplier() const { return monthly_cost_level_multiplier; }

	Vector2 band_size_min_max;
	Vector2 get_band_size_min_max() const { return band_size_min_max; }

	int num_bands_for_hire = 0;
	int get_num_bands_for_hire() const { return num_bands_for_hire; }

	TypedArray<BG_UnitCaste> unit_castes;
	TypedArray<BG_UnitCaste> get_unit_castes() const { return unit_castes; }

	int max_band_member_level = 0;
	int get_max_band_member_level() const { return max_band_member_level; }

	int max_number_of_bands = 0;
	int get_max_number_of_bands() const { return max_number_of_bands; }
};

////
//// BG_LevelGuide
////
class BG_LevelGuide : public Object
{
	GDCLASS(BG_LevelGuide, Object);

protected:
	static void _bind_methods();

public:
	int job_rep_reward = 0;
	int get_job_rep_reward() const { return job_rep_reward; }

	int job_duralation = 0;
	int get_job_duralation() const { return job_duralation; }

	float rest_duralation = 0.0f;
	float get_rest_duralation() const { return rest_duralation; }

	float leveling_speed = 0.0f;
	float get_leveling_speed() const { return leveling_speed; }

	int band_member_upkeep = 0;
	int get_band_member_upkeep() const { return band_member_upkeep; }

	int monster_health = 0;
	int get_monster_health() const { return monster_health; }

	int monster_base_off_stat = 0;
	int get_monster_base_off_stat() const { return monster_base_off_stat; }

	int monster_base_def_stat = 0;
	int get_monster_base_def_stat() const { return monster_base_def_stat; }

	float item_durability_consumption_per_job_level = 0.0f;
	float get_item_durability_consumption_per_job_level() const { return item_durability_consumption_per_job_level; }

	float item_fame_addition_per_job_level = 0.0f;
	float get_item_fame_addition_per_job_level() const { return item_fame_addition_per_job_level; }
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
	int reputation_needed = 0;
	int get_reputation_needed() const { return reputation_needed; }
	void set_reputation_needed(int p_value) { reputation_needed = p_value; }

	String description = "";
	String get_description() const { return description; }
	void set_description(String p_value) { description = p_value; }
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

	int starting_job_count = 0;
	int get_starting_job_count() const { return starting_job_count; }

	TypedArray<int> jobs_per_month;
	TypedArray<int> get_jobs_per_month() const { return jobs_per_month; }

	//MyCustomClass* result = dynamic_cast<MyCustomClass*>(container.object());
	TypedArray<BG_ActStats> act_stats;
	TypedArray<BG_ActStats> get_act_stats() const { return act_stats; }
	
	TypedArray<BG_LevelGuide> level_guide;
	TypedArray<BG_LevelGuide> get_level_guide() const { return level_guide; }

	TypedArray<Vector2i> job_level_range_min_max;
	TypedArray<Vector2i> get_job_level_range_min_max() const { return job_level_range_min_max; }

	// Base Values
	TypedArray<int> base_equipment_value_for_act;
	TypedArray<int> get_base_equipment_value_for_act() const { return base_equipment_value_for_act; }

	TypedArray<int> base_beast_part_value_for_act;
	TypedArray<int> get_base_beast_part_value_for_act() const { return base_beast_part_value_for_act; }

	// Value Multipliers
	TypedArray<float> equipment_value_rarity_multiplier;
	TypedArray<float> get_equipment_value_rarity_multiplier() const { return equipment_value_rarity_multiplier; }

	TypedArray<float> beast_part_value_rarity_multiplier;
	TypedArray<float> get_beast_part_value_rarity_multiplier() const { return beast_part_value_rarity_multiplier; }

	TypedArray<float> extra_beast_part_value_rarity_multiplier_while_grafted;
	TypedArray<float> get_extra_beast_part_value_rarity_multiplier_while_grafted() const { return extra_beast_part_value_rarity_multiplier_while_grafted; }

	// Fame and Durability
	TypedArray<float> equipment_max_durability_per_act;
	TypedArray<float> get_equipment_max_durability_per_act() const { return equipment_max_durability_per_act; }

	TypedArray<float> equipment_max_fame_per_act;
	TypedArray<float> get_equipment_max_fame_per_act() const { return equipment_max_fame_per_act; }

	TypedArray<float> beast_part_max_durability_per_act;
	TypedArray<float> get_beast_part_max_durability_per_act() const { return beast_part_max_durability_per_act; }

	TypedArray<float> beast_part_max_fame_per_act;
	TypedArray<float> get_beast_part_max_fame_per_act() const { return beast_part_max_fame_per_act; }

	// Fame and Durability Multipliers
	TypedArray<float> equipment_durability_rarity_multiplier;
	TypedArray<float> get_equipment_durability_rarity_multiplier() const { return equipment_durability_rarity_multiplier; }

	TypedArray<float> beast_part_durability_rarity_multiplier;
	TypedArray<float> get_beast_part_durability_rarity_multiplier() const { return beast_part_durability_rarity_multiplier; }

	TypedArray<float> equipment_fame_rarity_multiplier;
	TypedArray<float> get_equipment_fame_rarity_multiplier() const { return equipment_fame_rarity_multiplier; }

	TypedArray<float> beast_part_fame_rarity_multiplier;
	TypedArray<float> get_beast_part_fame_rarity_multiplier() const { return beast_part_fame_rarity_multiplier; }
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

	TypedArray<BG_JobDetails> jobs;
	TypedArray<BG_JobDetails> get_jobs() const { return jobs; }

	TypedArray<BG_ItemDetails> items;
	TypedArray<BG_ItemDetails> get_items() const { return items; }
	
	TypedArray<BG_Effect> abilities;
	TypedArray<BG_Effect> get_abilities() const { return abilities; }

	TypedArray<BG_Effect> effects;
	TypedArray<BG_Effect> get_effects() const { return effects; }

	BG_BandInfo *band_info = nullptr;
	BG_BandInfo *get_band_info() const { return band_info; }

	TypedArray<StringName> item_slot_types;
	TypedArray<StringName> get_item_slot_types() const { return item_slot_types; }

	TypedArray<BG_RarityDetails> rarity_types;
	TypedArray<BG_RarityDetails> get_rarity_types() const { return rarity_types; }

	TypedArray<BG_UnitStatDetails> stat_types;
	TypedArray<BG_UnitStatDetails> get_stat_types() const { return stat_types; }

	TypedArray<BG_Monster> monster_types;
	TypedArray<BG_Monster> get_monster_types() const { return monster_types; }

	static String get_job_challenge_rating(TypedArray<BG_Monster> monsters);

private:
	Dictionary data;

	void try_parse_data(const String &file_path);
};

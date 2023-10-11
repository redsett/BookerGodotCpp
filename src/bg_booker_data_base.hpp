#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

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
	void set_id(StringName value) { id = value; }

	StringName name;
	StringName get_name() const { return name; }
	void set_name(StringName value) { name = value; }

	StringName description;
	StringName get_description() const { return description; }
	void set_description(StringName value) { description = value; }

	bool is_beast_part = false;
	bool get_is_beast_part() const { return is_beast_part; }
	void set_is_beast_part(bool value) { is_beast_part = value; }

	// bool is_permanent = false;
	// bool get_is_permanent() { return is_permanent; }

	StringName slot_type_id;
	StringName get_slot_type_id() { return slot_type_id; }
	void set_slot_type_id(StringName value) { slot_type_id = value; }

	bool is_useable_item = false;
	bool get_is_useable_item() { return is_useable_item; }
	void set_is_useable_item(bool value) { is_useable_item = value; }
};

////
//// BG_RewardItem
////
class BG_RewardItem : public Resource
{
	GDCLASS(BG_RewardItem, Resource);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	float drop_rate = 0.0;
	float get_drop_rate() const { return drop_rate; }
	void set_drop_rate(float value) { drop_rate = value; }
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
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	StringName name;
	StringName get_name() const { return name; }
	void set_name(StringName value) { name = value; }

	int level = 0;
	int get_level() const { return level; }
	void set_level(int value) { level = value; }

	StringName description;
	StringName get_description() const { return description; }
	void set_description(StringName value) { description = value; }

	//FString clanmember;

	int weeks = 0;
	int get_weeks() const { return weeks; }
	void set_weeks(int value) { weeks = value; }

	TypedArray<StringName> monsters_ids;
	TypedArray<StringName> get_monsters_ids() const { return monsters_ids; }
	void set_monsters_ids(TypedArray<StringName> value) { monsters_ids = value; }

	TypedArray<float> monsters_spawn_chances;
	TypedArray<float> get_monsters_spawn_chances() const { return monsters_spawn_chances; }
	void set_monsters_spawn_chances(TypedArray<float> value) { monsters_spawn_chances = value; }

	Vector2i monster_count_range;
	Vector2i get_monster_count_range() const { return monster_count_range; }
	void set_monster_count_range(Vector2i value) { monster_count_range = value; }

	bool is_unique = false;
	bool get_is_unique() const { return is_unique; }
	void set_is_unique(bool value) { is_unique = value; }

	TypedArray<int> acts_allowed_in;
	TypedArray<int> get_acts_allowed_in() const { return acts_allowed_in; }
	void set_acts_allowed_in(TypedArray<int> value) { acts_allowed_in = value; }
};

////
//// BG_JobBoardItem
////
class BG_JobBoardItem : public Object
{
	GDCLASS(BG_JobBoardItem, Object);

protected:
	static void _bind_methods();

public:
	StringName title;
	StringName get_title() const { return title; }

	StringName description;
	StringName get_description() const { return description; }

	int opens_job = 0;
	int get_opens_job() const { return opens_job; }
};

////
//// BG_JobBoardItems
////
class BG_JobBoardItems : public Object
{
	GDCLASS(BG_JobBoardItems, Object);

protected:
	static void _bind_methods();

public:
	TypedArray<BG_JobBoardItem> jobs;
	TypedArray<BG_JobBoardItem> get_jobs() const { return jobs; }
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

	int defensive_value = 0;
	int get_defensive_value() const { return defensive_value; }
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



	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }
	void set_stats(TypedArray<BG_UnitStat> p_value) { stats = p_value; }
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

	Vector3 scale;
	Vector3 get_scale() const { return scale; }
	void set_scale(Vector3 value) { scale = value; }

	StringName personality_dialgue_id;
	StringName get_personality_dialgue_id() const { return personality_dialgue_id; }
	void set_personality_dialgue_id(StringName value) { personality_dialgue_id = value; }

	Ref<BG_UnitCaste> caste = nullptr;
	Ref<BG_UnitCaste> get_caste() const { return caste; }
	void set_caste(Ref<BG_UnitCaste> value) { caste = value; }

	TypedArray<String> equipment;
	TypedArray<String> get_equipment() const { return equipment; }
	void set_equipment(TypedArray<String> value) { equipment = value; }
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
	
	int level = 1;
	int get_level() const { return level; }
	void set_level(int value) { level = value; }

	float experience = 0.0;
	float get_experience() const { return experience; }
	void set_experience(float value) { experience = value; }

	float health = 0.0;
	float get_health() const { return health; }
	void set_health(float value) { health = value; }

	float max_health = 0.0;
	float get_max_health() const { return max_health; }
	void set_max_health(float value) { max_health = value; }

	bool resting = false;
	bool get_resting() const { return resting; }
	void set_resting(bool value) { resting = value; }

	TypedArray<BG_BandMember> band_members;
	TypedArray<BG_BandMember> get_band_members() const { return band_members; }
	void set_band_members(TypedArray<BG_BandMember> value) { band_members = value; }

	Ref<BG_Job> current_job = nullptr;
	Ref<BG_Job> get_current_job() const { return current_job; }
	void set_current_job(Ref<BG_Job> value) { current_job = value; }
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
	void set_name(StringName value) { name = value; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }
	void set_stats(TypedArray<BG_UnitStat> value) { stats = value; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	TypedArray<BG_RewardItem> beast_part_rewards;
	TypedArray<BG_RewardItem> get_beast_part_rewards() const { return beast_part_rewards; }
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

	float leveling_speed = 0.0f;
	float get_leveling_speed() const { return leveling_speed; }
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
	void set_starting_reputation(int p_value) { starting_reputation = p_value; }

	int seconds_per_week = 0;
	int get_seconds_per_week() const { return seconds_per_week; }
	void set_seconds_per_week(int p_value) { seconds_per_week = p_value; }

	TypedArray<Vector2> jobs_per_month_min_max;
	TypedArray<Vector2> get_jobs_per_month_min_max() const { return jobs_per_month_min_max; }
	void set_jobs_per_month_min_max(TypedArray<Vector2> p_value) { jobs_per_month_min_max = p_value; }
	
	TypedArray<int> job_rerolls_per_month;
	TypedArray<int> get_job_rerolls_per_month() const { return job_rerolls_per_month; }
	void set_job_rerolls_per_month(TypedArray<int> p_value) { job_rerolls_per_month = p_value; }

	//MyCustomClass* result = dynamic_cast<MyCustomClass*>(container.object());
	TypedArray<BG_ActStats> act_stats;
	TypedArray<BG_ActStats> get_act_stats() const { return act_stats; }
	void set_act_stats(TypedArray<BG_ActStats> p_value) { act_stats = p_value; }
	
	TypedArray<BG_LevelGuide> level_guide;
	TypedArray<BG_LevelGuide> get_level_guide() const { return level_guide; }
	void set_level_guide(TypedArray<BG_LevelGuide> p_value) { level_guide = p_value; }
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

	String modding_path = "";
	String get_modding_path() const { return modding_path; }

	BG_Booker_Globals *globals = nullptr;
	BG_Booker_Globals *get_globals() const { return globals; }

	TypedArray<BG_Job> jobs;
	TypedArray<BG_Job> get_jobs() const { return jobs; }

	TypedArray<BG_JobBoardItems> job_board;
	TypedArray<BG_JobBoardItems> get_job_board() const { return job_board; }

	TypedArray<BG_Item> items;
	TypedArray<BG_Item> get_items() const { return items; }

	BG_BandInfo *band_info = nullptr;
	BG_BandInfo *get_band_info() const { return band_info; }

	TypedArray<StringName> item_slot_types;
	TypedArray<StringName> get_item_slot_types() const { return item_slot_types; }

	TypedArray<BG_UnitStatDetails> stat_types;
	TypedArray<BG_UnitStatDetails> get_stat_types() const { return stat_types; }

	TypedArray<BG_Monster> monster_types;
	TypedArray<BG_Monster> get_monster_types() const { return monster_types; }

private:
	Dictionary data;

	void try_parse_data(const String &file_path);
};

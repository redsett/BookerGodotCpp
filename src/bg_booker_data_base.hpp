#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

////
//// BG_UnitStat
////
class BG_UnitStat : public Object
{
	GDCLASS(BG_UnitStat, Object);

protected:
	static void _bind_methods();

public:
	String name;
	String get_name() { return name; }

	int value;
	int get_value() { return value; }
};

////
//// BG_BandMemberClass
////
class BG_BandMemberClass : public Object
{
	GDCLASS(BG_BandMemberClass, Object);

protected:
	static void _bind_methods();

public:
	String name;
	String get_name() { return name; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() { return stats; }
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
	String band_name;
	String get_band_name() { return band_name; }

	TypedArray<String> hiring_dialogue_choices;
	TypedArray<String> get_hiring_dialogue_choices() { return hiring_dialogue_choices; }
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
	TypedArray<BG_BandNameInfo> get_band_names() { return band_names; }

	TypedArray<String> first_names;
	TypedArray<String> get_first_names() { return first_names; }

	TypedArray<String> last_names;
	TypedArray<String> get_last_names() { return last_names; }

	TypedArray<int> level_min;
	TypedArray<int> get_level_min() { return level_min; }

	TypedArray<int> level_max;
	TypedArray<int> get_level_max() { return level_max; }

	TypedArray<float> monthly_cost_level_multiplier;
	TypedArray<float> get_monthly_cost_level_multiplier() { return monthly_cost_level_multiplier; }

	Vector2 band_size_min_max;
	Vector2 get_band_size_min_max() { return band_size_min_max; }

	int num_bands_for_hire;
	TypedArray<int> get_num_bands_for_hire() { return num_bands_for_hire; }

	TypedArray<BG_BandMemberClass> unit_classes;
	TypedArray<BG_BandMemberClass> get_unit_classes() { return unit_classes; }
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
	int get_job_rep_reward() { return job_rep_reward; }

	int job_duralation = 0;
	int get_job_duralation() { return job_duralation; }

	float leveling_speed = 0.0f;
	float get_leveling_speed() { return leveling_speed; }
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
	void set_reputation_needed(int p_value)  { reputation_needed = p_value; }
	int get_reputation_needed() { return reputation_needed; }

	String description = "";
	void set_description(String p_value)  { description = p_value; }
	String get_description() { return description; }
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
	void set_starting_reputation(int p_value)  { starting_reputation = p_value; }
	int get_starting_reputation() { return starting_reputation; }

	int seconds_per_week = 0;
	void set_seconds_per_week(int p_value)  { seconds_per_week = p_value; }
	int get_seconds_per_week() { return seconds_per_week; }

	TypedArray<Vector2> jobs_per_month_min_max;
	void set_jobs_per_month_min_max(TypedArray<Vector2> p_value)  { jobs_per_month_min_max = p_value; }
	TypedArray<Vector2> get_jobs_per_month_min_max() { return jobs_per_month_min_max; }
	
	TypedArray<int> job_rerolls_per_month;
	void set_job_rerolls_per_month(TypedArray<int> p_value)  { job_rerolls_per_month = p_value; }
	TypedArray<int> get_job_rerolls_per_month() { return job_rerolls_per_month; }

	TypedArray<BG_ActStats> act_stats;
	void set_act_stats(TypedArray<BG_ActStats> p_value) { act_stats = p_value; }
	//MyCustomClass* result = dynamic_cast<MyCustomClass*>(container.object());
	TypedArray<BG_ActStats> get_act_stats() { return act_stats; }
	
	TypedArray<BG_LevelGuide> level_guide;
	void set_level_guide(TypedArray<BG_LevelGuide> p_value)  { level_guide = p_value; }
	TypedArray<BG_LevelGuide> get_level_guide() { return level_guide; }
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

	BG_Booker_Globals *globals;
	BG_Booker_Globals *get_globals() const { return globals; }

	// TypedArray<FJob> jobs;
	// TypedArray<FJobBoardItems> jobBoard;
	// TypedArray<FItem> items;

	BG_BandInfo *band_info;
	BG_BandInfo *get_band_info() const { return band_info; }

private:
	Dictionary data;
};

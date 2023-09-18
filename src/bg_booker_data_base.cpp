#include "bg_booker_data_base.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

////
//// BG_BandInfo
////
void BG_BandInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_band_names"), &BG_BandInfo::get_band_names);
	ClassDB::bind_method(D_METHOD("get_first_names"), &BG_BandInfo::get_first_names);
	ClassDB::bind_method(D_METHOD("get_last_names"), &BG_BandInfo::get_last_names);
	ClassDB::bind_method(D_METHOD("get_level_min"), &BG_BandInfo::get_level_min);
	ClassDB::bind_method(D_METHOD("get_level_max"), &BG_BandInfo::get_level_max);
	ClassDB::bind_method(D_METHOD("get_monthly_cost_level_multiplier"), &BG_BandInfo::get_monthly_cost_level_multiplier);
	ClassDB::bind_method(D_METHOD("get_band_size"), &BG_BandInfo::get_band_size);
	ClassDB::bind_method(D_METHOD("get_num_bands_for_hire"), &BG_BandInfo::get_num_bands_for_hire);
}

////
//// BG_LevelGuide
////
void BG_LevelGuide::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_job_rep_reward"), &BG_LevelGuide::get_job_rep_reward);
	ClassDB::bind_method(D_METHOD("get_job_duralation"), &BG_LevelGuide::get_job_duralation);
	ClassDB::bind_method(D_METHOD("get_leveling_speed"), &BG_LevelGuide::get_leveling_speed);
}

////
//// BG_ActStats
////
void BG_ActStats::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_reputation_needed", "int"), &BG_ActStats::set_reputation_needed);
	ClassDB::bind_method(D_METHOD("get_reputation_needed"), &BG_ActStats::get_reputation_needed);
	ClassDB::bind_method(D_METHOD("set_description", "string"), &BG_ActStats::set_description);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_ActStats::get_description);
}

////
//// BG_Booker_Globals
////
void BG_Booker_Globals::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_starting_reputation", "int"), &BG_Booker_Globals::set_starting_reputation);
	ClassDB::bind_method(D_METHOD("get_starting_reputation"), &BG_Booker_Globals::get_starting_reputation);
	ClassDB::bind_method(D_METHOD("set_seconds_per_week", "int"), &BG_Booker_Globals::set_seconds_per_week);
	ClassDB::bind_method(D_METHOD("get_seconds_per_week"), &BG_Booker_Globals::get_seconds_per_week);
	ClassDB::bind_method(D_METHOD("set_jobs_per_month_min_max", "vector2[min, max]"), &BG_Booker_Globals::set_jobs_per_month_min_max);
	ClassDB::bind_method(D_METHOD("get_jobs_per_month_min_max"), &BG_Booker_Globals::get_jobs_per_month_min_max);
	ClassDB::bind_method(D_METHOD("set_job_rerolls_per_month", "array[int]"), &BG_Booker_Globals::set_job_rerolls_per_month);
	ClassDB::bind_method(D_METHOD("get_job_rerolls_per_month"), &BG_Booker_Globals::get_job_rerolls_per_month);
	ClassDB::bind_method(D_METHOD("set_act_stats", "array[BG_ActStats]"), &BG_Booker_Globals::set_act_stats);
	ClassDB::bind_method(D_METHOD("get_act_stats"), &BG_Booker_Globals::get_act_stats);
	ClassDB::bind_method(D_METHOD("set_level_guide", "array[BG_LevelGuide]"), &BG_Booker_Globals::set_level_guide);
	ClassDB::bind_method(D_METHOD("get_level_guide"), &BG_Booker_Globals::get_level_guide);
}

////
//// BG_Booker_DB
////
BG_Booker_DB *BG_Booker_DB::singleton = nullptr;

void BG_Booker_DB::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("refresh_data"), &BG_Booker_DB::refresh_data);

	ClassDB::bind_method(D_METHOD("get_globals"), &BG_Booker_DB::get_globals);
	ClassDB::bind_method(D_METHOD("get_band_info"), &BG_Booker_DB::get_band_info);
	
	// ADD_GROUP("Bread", "");
	// ADD_PROPERTY(PropertyInfo(Variant::COLOR, "spline_color"), "set_spline_color", "get_spline_color");
	// ADD_PROPERTY(PropertyInfo(Variant::INT, "spline_width"), "set_spline_width", "get_spline_width");
}

BG_Booker_DB *BG_Booker_DB::get_singleton()
{
	return singleton;
}

void BG_Booker_DB::refresh_data()
{
	globals = memnew(BG_Booker_Globals);
	band_info = memnew(BG_BandInfo);

	data = BG_JsonUtils::ParseJsonCBDDataFile("bookerData");
	//UtilityFunctions::print(BG_JsonUtils::GetCBDSheet(data, "globals"));

	const Dictionary globals_sheet = BG_JsonUtils::GetCBDSheet(data, "globals");
	const Dictionary lines = Array(globals_sheet["lines"])[0];
	globals->seconds_per_week = lines["seconds_per_week"];
	globals->starting_reputation = lines["starting_coin"];

	{
		const Array rep_needed_per_act_array = Array(lines["reputation_needed_per_act"]);
		for (int i = 0; i < rep_needed_per_act_array.size(); i++)
		{
			const Dictionary act = rep_needed_per_act_array[i];
			BG_ActStats *new_act_stats = memnew(BG_ActStats);
			new_act_stats->reputation_needed = act["reputation"];
			new_act_stats->description = act["description"];
			globals->act_stats.append(new_act_stats);
		}
	}

	{
		const Array jobs_rerolls_per_month_array = Array(lines["job_rerolls_per_month"]);
		for (int i = 0; i < jobs_rerolls_per_month_array.size(); i++)
		{
			const Dictionary month = jobs_rerolls_per_month_array[i];
			globals->job_rerolls_per_month.append(int(month["rerolls"]));
		}
	}

	{
		const Array jobs_per_month_array = Array(lines["jobs_per_month"]);
		for (int i = 0; i < jobs_per_month_array.size(); i++)
		{
			const Dictionary month = jobs_per_month_array[i];
			globals->jobs_per_month_min_max.append(Vector2(month["min"], month["max"]));
		}
	}
}

BG_Booker_DB::BG_Booker_DB()
{
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;
}

BG_Booker_DB::~BG_Booker_DB()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;

	if (globals)
	{
		memdelete(globals);
		globals = nullptr;
	}
	if (band_info)
	{
		memdelete(band_info);
		band_info = nullptr;
	}
}
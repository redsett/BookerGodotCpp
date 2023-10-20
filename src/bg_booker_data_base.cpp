#include "bg_booker_data_base.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

#define TIME_FUNC(func) \
    do { \
        clock_t start, end; \
        double elapsed; \
        start = clock(); \
        func.call(); \
        end = clock(); \
        elapsed = (double)(end - start) / CLOCKS_PER_SEC; \
        UtilityFunctions::print("The function '", func.get_method(), "' took ", elapsed, " seconds to execute."); \
    } while (0)

////
//// BG_UnitStatDetails
////
void BG_UnitStatDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitStatDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitStatDetails::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_is_damage_type"), &BG_UnitStatDetails::get_is_damage_type);
}

////
//// BG_UnitStat
////
void BG_UnitStat::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitStat::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_UnitStat::set_id);
	ClassDB::bind_method(D_METHOD("get_bonus_percentage"), &BG_UnitStat::get_bonus_percentage);
	ClassDB::bind_method(D_METHOD("set_bonus_percentage"), &BG_UnitStat::set_bonus_percentage);

	ClassDB::bind_method(D_METHOD("get_offensive_value"), &BG_UnitStat::get_offensive_value);
	ClassDB::bind_method(D_METHOD("set_offensive_value"), &BG_UnitStat::set_offensive_value);
	ClassDB::bind_method(D_METHOD("get_defensive_value"), &BG_UnitStat::get_defensive_value);
	ClassDB::bind_method(D_METHOD("set_defensive_value"), &BG_UnitStat::set_defensive_value);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bonus_percentage"), "set_bonus_percentage", "get_bonus_percentage");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "offensive_value"), "set_offensive_value", "get_offensive_value");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "defensive_value"), "set_defensive_value", "get_defensive_value");
}

////
//// BG_Item
////
void BG_Item::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Item::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Item::set_id);
	ClassDB::bind_method(D_METHOD("get_on_shelf"), &BG_Item::get_on_shelf);
	ClassDB::bind_method(D_METHOD("set_on_shelf"), &BG_Item::set_on_shelf);
	ClassDB::bind_method(D_METHOD("get_shelf_index"), &BG_Item::get_shelf_index);
	ClassDB::bind_method(D_METHOD("set_shelf_index"), &BG_Item::set_shelf_index);
	ClassDB::bind_method(D_METHOD("get_has_bid"), &BG_Item::get_has_bid);
	ClassDB::bind_method(D_METHOD("set_has_bid"), &BG_Item::set_has_bid);
	ClassDB::bind_method(D_METHOD("get_bid_amount"), &BG_Item::get_bid_amount);
	ClassDB::bind_method(D_METHOD("set_bid_amount"), &BG_Item::set_bid_amount);
	ClassDB::bind_method(D_METHOD("get_is_equipped"), &BG_Item::get_is_equipped);
	ClassDB::bind_method(D_METHOD("set_is_equipped"), &BG_Item::set_is_equipped);
	ClassDB::bind_method(D_METHOD("get_graft"), &BG_Item::get_graft);
	ClassDB::bind_method(D_METHOD("set_graft"), &BG_Item::set_graft);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "on_shelf"), "set_on_shelf", "get_on_shelf");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "shelf_index"), "set_shelf_index", "get_shelf_index");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_bid"), "set_has_bid", "get_has_bid");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bid_amount"), "set_bid_amount", "get_bid_amount");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_equipped"), "set_is_equipped", "get_is_equipped");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "graft"), "set_graft", "get_graft");
}

////
//// BG_ItemDetails
////
void BG_ItemDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ItemDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_ItemDetails::get_name);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_ItemDetails::get_description);
	ClassDB::bind_method(D_METHOD("get_is_beast_part"), &BG_ItemDetails::get_is_beast_part);
	ClassDB::bind_method(D_METHOD("get_is_useable_item"), &BG_ItemDetails::get_is_useable_item);
	ClassDB::bind_method(D_METHOD("get_slot_type_id"), &BG_ItemDetails::get_slot_type_id);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_ItemDetails::get_stats);
	// ClassDB::bind_static_method("BG_ItemDetails", D_METHOD("get_slot_types"), &BG_ItemDetails::get_slot_types);
}

////
//// BG_BandMember
////
void BG_BandMember::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandMember::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_BandMember::set_name);
	ClassDB::bind_method(D_METHOD("get_scale"), &BG_BandMember::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale"), &BG_BandMember::set_scale);
	ClassDB::bind_method(D_METHOD("get_personality_dialgue_id"), &BG_BandMember::get_personality_dialgue_id);
	ClassDB::bind_method(D_METHOD("set_personality_dialgue_id"), &BG_BandMember::set_personality_dialgue_id);
	ClassDB::bind_method(D_METHOD("get_caste"), &BG_BandMember::get_caste);
	ClassDB::bind_method(D_METHOD("set_caste"), &BG_BandMember::set_caste);
	ClassDB::bind_method(D_METHOD("get_equipment"), &BG_BandMember::get_equipment);
	ClassDB::bind_method(D_METHOD("set_equipment"), &BG_BandMember::set_equipment);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "personality_dialgue_id"), "set_personality_dialgue_id", "get_personality_dialgue_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "caste", PROPERTY_HINT_RESOURCE_TYPE, "BG_UnitCaste"), "set_caste", "get_caste");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipment"), "set_equipment", "get_equipment");
}

////
//// BG_Band
////
void BG_Band::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Band::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Band::set_name);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_Band::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_Band::set_level);
	ClassDB::bind_method(D_METHOD("get_experience"), &BG_Band::get_experience);
	ClassDB::bind_method(D_METHOD("set_experience"), &BG_Band::set_experience);
	ClassDB::bind_method(D_METHOD("get_health"), &BG_Band::get_health);
	ClassDB::bind_method(D_METHOD("set_health"), &BG_Band::set_health);
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_Band::get_max_health);
	ClassDB::bind_method(D_METHOD("set_max_health"), &BG_Band::set_max_health);
	ClassDB::bind_method(D_METHOD("get_resting"), &BG_Band::get_resting);
	ClassDB::bind_method(D_METHOD("set_resting"), &BG_Band::set_resting);
	ClassDB::bind_method(D_METHOD("get_band_members"), &BG_Band::get_band_members);
	ClassDB::bind_method(D_METHOD("set_band_members"), &BG_Band::set_band_members);
	ClassDB::bind_method(D_METHOD("get_current_job_id"), &BG_Band::get_current_job_id);
	ClassDB::bind_method(D_METHOD("set_current_job_id"), &BG_Band::set_current_job_id);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "experience"), "set_experience", "get_experience");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "health"), "set_health", "get_health");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_health"), "set_max_health", "get_max_health");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "resting"), "set_resting", "get_resting");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "band_members"), "set_band_members", "get_band_members");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "current_job_id"), "set_current_job_id", "get_current_job_id");
}

////
//// BG_UnitCaste
////
void BG_UnitCaste::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitCaste::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_UnitCaste::set_id);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitCaste::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_UnitCaste::get_stats);
	ClassDB::bind_method(D_METHOD("set_stats"), &BG_UnitCaste::set_stats);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "stats", PROPERTY_HINT_RESOURCE_TYPE, "BG_UnitStat"), "set_stats", "get_stats");
}

////
//// BG_BandNameInfo
////
void BG_BandNameInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_band_name"), &BG_BandNameInfo::get_band_name);
	ClassDB::bind_method(D_METHOD("get_hiring_dialogue_choices"), &BG_BandNameInfo::get_hiring_dialogue_choices);
}

////
//// BG_PersonalityDialgue
////
void BG_PersonalityDialgue::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_PersonalityDialgue::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_PersonalityDialgue::set_id);
}

////
//// BG_Monster
////
void BG_Monster::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Monster::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Monster::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Monster::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Monster::set_name);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_Monster::get_stats);
	ClassDB::bind_method(D_METHOD("set_stats"), &BG_Monster::set_stats);

	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_Monster::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_beast_part_rewards"), &BG_Monster::get_beast_part_rewards);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "stats", PROPERTY_HINT_RESOURCE_TYPE, "BG_UnitStat"), "set_stats", "get_stats");
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
	ClassDB::bind_method(D_METHOD("get_personality_dialgue"), &BG_BandInfo::get_personality_dialgue);
	ClassDB::bind_method(D_METHOD("get_level_min"), &BG_BandInfo::get_level_min);
	ClassDB::bind_method(D_METHOD("get_level_max"), &BG_BandInfo::get_level_max);
	ClassDB::bind_method(D_METHOD("get_monthly_cost_level_multiplier"), &BG_BandInfo::get_monthly_cost_level_multiplier);
	ClassDB::bind_method(D_METHOD("get_band_size_min_max"), &BG_BandInfo::get_band_size_min_max);
	ClassDB::bind_method(D_METHOD("get_num_bands_for_hire"), &BG_BandInfo::get_num_bands_for_hire);
	ClassDB::bind_method(D_METHOD("get_unit_castes"), &BG_BandInfo::get_unit_castes);
}

////
//// BG_RewardItem
////
void BG_RewardItem::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_RewardItem::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_RewardItem::set_id);
	ClassDB::bind_method(D_METHOD("get_drop_rate"), &BG_RewardItem::get_drop_rate);
	ClassDB::bind_method(D_METHOD("set_drop_rate"), &BG_RewardItem::set_drop_rate);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "drop_rate"), "set_drop_rate", "get_drop_rate");
}

////
//// BG_JobDetails
////
void BG_JobDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_JobDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_JobDetails::get_name);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_JobDetails::get_level);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_JobDetails::get_description);
	ClassDB::bind_method(D_METHOD("get_weeks"), &BG_JobDetails::get_weeks);
	ClassDB::bind_method(D_METHOD("get_monsters_ids"), &BG_JobDetails::get_monsters_ids);
	ClassDB::bind_method(D_METHOD("get_monsters_spawn_chances"), &BG_JobDetails::get_monsters_spawn_chances);
	ClassDB::bind_method(D_METHOD("get_monster_count_range"), &BG_JobDetails::get_monster_count_range);
	ClassDB::bind_method(D_METHOD("get_is_unique"), &BG_JobDetails::get_is_unique);
	ClassDB::bind_method(D_METHOD("get_acts_allowed_in"), &BG_JobDetails::get_acts_allowed_in);
}

////
//// BG_JobBoardItem
////
void BG_JobBoardItem::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_title"), &BG_JobBoardItem::get_title);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_JobBoardItem::get_description);
	ClassDB::bind_method(D_METHOD("get_opens_job"), &BG_JobBoardItem::get_opens_job);
}

////
//// BG_JobBoardItems
////
void BG_JobBoardItems::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_jobs"), &BG_JobBoardItems::get_jobs);
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
	ClassDB::bind_method(D_METHOD("get_starting_reputation"), &BG_Booker_Globals::get_starting_reputation);
	ClassDB::bind_method(D_METHOD("get_seconds_per_week"), &BG_Booker_Globals::get_seconds_per_week);
	ClassDB::bind_method(D_METHOD("get_jobs_per_month_min_max"), &BG_Booker_Globals::get_jobs_per_month_min_max);
	ClassDB::bind_method(D_METHOD("get_job_rerolls_per_month"), &BG_Booker_Globals::get_job_rerolls_per_month);
	ClassDB::bind_method(D_METHOD("get_act_stats"), &BG_Booker_Globals::get_act_stats);
	ClassDB::bind_method(D_METHOD("get_level_guide"), &BG_Booker_Globals::get_level_guide);
}

////
//// BG_Booker_DB
////
BG_Booker_DB *BG_Booker_DB::singleton = nullptr;

void BG_Booker_DB::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("refresh_data"), &BG_Booker_DB::refresh_data);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("timer_test"), &BG_Booker_DB::timer_test);

	ClassDB::bind_method(D_METHOD("get_modding_path"), &BG_Booker_DB::get_modding_path);
	ClassDB::bind_method(D_METHOD("get_globals"), &BG_Booker_DB::get_globals);
	ClassDB::bind_method(D_METHOD("get_jobs"), &BG_Booker_DB::get_jobs);
	ClassDB::bind_method(D_METHOD("get_job_board"), &BG_Booker_DB::get_job_board);
	ClassDB::bind_method(D_METHOD("get_items"), &BG_Booker_DB::get_items);
	ClassDB::bind_method(D_METHOD("get_band_info"), &BG_Booker_DB::get_band_info);
	ClassDB::bind_method(D_METHOD("get_item_slot_types"), &BG_Booker_DB::get_item_slot_types);
	ClassDB::bind_method(D_METHOD("get_stat_types"), &BG_Booker_DB::get_stat_types);
	ClassDB::bind_method(D_METHOD("get_monster_types"), &BG_Booker_DB::get_monster_types);
}

BG_Booker_DB *BG_Booker_DB::get_singleton()
{
	return singleton;
}

void BG_Booker_DB::refresh_data()
{
	globals = memnew(BG_Booker_Globals);
	band_info = memnew(BG_BandInfo);

	const String booker_data_file_name = "bookerData.cdb";
	const String exe_path = OS::get_singleton()->get_executable_path().get_base_dir() + "/";
	modding_path = exe_path + String("mod/");
	const String modding_data_path = modding_path + booker_data_file_name;

	try_parse_data("res://" + booker_data_file_name);

	// If the mod data exists, then let it override any data that it has.
	if (FileAccess::file_exists(modding_data_path))
	{
		UtilityFunctions::print("Log - Using modding booker data.");
		try_parse_data(modding_data_path);
	}
}

/* static */ void BG_Booker_DB::timer_test(Callable callable)
{
	TIME_FUNC(callable);
}

void BG_Booker_DB::try_parse_data(const String &file_path)
{
	data = BG_JsonUtils::ParseJsonFile(file_path);

	//UtilityFunctions::print(BG_JsonUtils::GetCBDSheet(data, "globals"));

	/////
	///// Globals
	/////
	{
		const Dictionary globals_sheet = BG_JsonUtils::GetCBDSheet(data, "globals");
		if (globals_sheet.has("lines"))
		{
			const Dictionary lines = Array(globals_sheet["lines"])[0];
			if (lines.has("seconds_per_week"))
				globals->seconds_per_week = lines["seconds_per_week"];
			if (lines.has("starting_coin"))
				globals->starting_reputation = lines["starting_coin"];

			if (lines.has("reputation_needed_per_act"))
			{
				globals->act_stats.clear();

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

			if (lines.has("job_rerolls_per_month"))
			{
				globals->job_rerolls_per_month.clear();

				const Array jobs_rerolls_per_month_array = Array(lines["job_rerolls_per_month"]);
				for (int i = 0; i < jobs_rerolls_per_month_array.size(); i++)
				{
					const Dictionary month = jobs_rerolls_per_month_array[i];
					globals->job_rerolls_per_month.append(int(month["rerolls"]));
				}
			}

			if (lines.has("jobs_per_month"))
			{
				globals->jobs_per_month_min_max.clear();

				const Array jobs_per_month_array = Array(lines["jobs_per_month"]);
				for (int i = 0; i < jobs_per_month_array.size(); i++)
				{
					const Dictionary month = jobs_per_month_array[i];
					globals->jobs_per_month_min_max.append(Vector2i(int(month["min"]), int(month["max"])));
				}
			}
		}
	}

	/////
	///// Stat Types
	/////
	{
		const Dictionary stat_types_sheet = BG_JsonUtils::GetCBDSheet(data, "stat_types");
		if (stat_types_sheet.has("lines"))
		{
			stat_types.clear();

			const Array lines = Array(stat_types_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];

				BG_UnitStatDetails *new_stat_types = memnew(BG_UnitStatDetails);
				new_stat_types->id = entry["id"];
				new_stat_types->icon_path = entry["icon_path"];
				new_stat_types->is_damage_type = bool(entry["is_damage_type"]);

				stat_types.append(new_stat_types);
			}
		}
	}

	/////
	///// Band Info
	/////
	{
		const Dictionary bands_sheet = BG_JsonUtils::GetCBDSheet(data, "bands");
		if (bands_sheet.has("lines"))
		{
			const Dictionary lines = Array(bands_sheet["lines"])[0];
			if (lines.has("num_bands_for_hire"))
				band_info->num_bands_for_hire = lines["num_bands_for_hire"];

			if (lines.has("names"))
			{
				band_info->band_names.clear();

				const Array band_names_array = Array(lines["names"]);
				for (int i = 0; i < band_names_array.size(); i++)
				{
					const Dictionary entry = band_names_array[i];
					BG_BandNameInfo *new_band_name_info = memnew(BG_BandNameInfo);
					new_band_name_info->band_name = entry["name"];
					band_info->icon_paths.append(entry["icon_path"]);

					const Array band_hiring_dialogue_choices_array = Array(entry["hiring_dialogue_choices"]);
					for (int x = 0; x < band_hiring_dialogue_choices_array.size(); x++)
					{
						const Dictionary hiring_dialogue_choices_entry = band_hiring_dialogue_choices_array[x];
						new_band_name_info->hiring_dialogue_choices.append(hiring_dialogue_choices_entry["entry"]);
					}
					band_info->band_names.append(new_band_name_info);
				}
			}

			if (lines.has("first_names"))
			{
				band_info->first_names.clear();

				const Array first_names_array = Array(lines["first_names"]);
				for (int i = 0; i < first_names_array.size(); i++)
				{
					const Dictionary entry = first_names_array[i];
					band_info->first_names.append(entry["name"]);
				}
			}

			if (lines.has("last_names"))
			{
				band_info->last_names.clear();

				const Array last_names_array = Array(lines["last_names"]);
				for (int i = 0; i < last_names_array.size(); i++)
				{
					const Dictionary entry = last_names_array[i];
					band_info->last_names.append(entry["name"]);
				}
			}

			if (lines.has("band_sizes"))
			{
				const Array band_sizes_array = Array(lines["band_sizes"]);
				const Dictionary entry = band_sizes_array[0];
				band_info->band_size_min_max = Vector2(entry["band_size_min"], entry["band_size_max"]);
			}
		}
	}

	/////
	///// Caste Types
	/////
	{
		const Dictionary caste_types_sheet = BG_JsonUtils::GetCBDSheet(data, "caste_types");
		if (caste_types_sheet.has("lines"))
		{
			band_info->unit_castes.clear();

			const Array lines = Array(caste_types_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];

				BG_UnitCaste *new_unit_caste = memnew(BG_UnitCaste);
				new_unit_caste->id = entry["id"];
				new_unit_caste->icon_path = entry["icon_path"];

				const Array damage_type_lines = Array(entry["base_damage_type_stats"]);
				for (int y = 0; y < damage_type_lines.size(); y++)
				{
					const Dictionary damage_type_entry = damage_type_lines[y];

					BG_UnitStat *new_stat = memnew(BG_UnitStat);
					new_stat->id = damage_type_entry["damage_type"];
					new_stat->bonus_percentage = float(damage_type_entry["base_bonus_percentage"]);

					new_unit_caste->stats.append(new_stat);
				}
				band_info->unit_castes.append(new_unit_caste);
			}
		}
	}

	/////
	///// Monster Types
	/////
	{
		const Dictionary monster_types_sheet = BG_JsonUtils::GetCBDSheet(data, "monster_types");
		if (monster_types_sheet.has("lines"))
		{
			monster_types.clear();

			const Array lines = Array(monster_types_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];

				BG_Monster *new_monster_type = memnew(BG_Monster);
				new_monster_type->id = entry["id"];
				new_monster_type->name = entry["name"];
				new_monster_type->icon_path = entry["icon_path"];

				// Stats
				const Array stats_lines = Array(entry["stats"]);
				for (int y = 0; y < stats_lines.size(); y++)
				{
					const Dictionary stat_entry = stats_lines[y];

					BG_UnitStat *new_stat = memnew(BG_UnitStat);
					new_stat->id = stat_entry["stat"];
					new_stat->offensive_value = int(stat_entry["offensive_value"]);
					new_stat->defensive_value = int(stat_entry["defensive_value"]);

					new_monster_type->stats.append(new_stat);
				}

				// Rewards
				const Array beast_part_rewards_lines = Array(entry["beast_part_rewards"]);
				for (int y = 0; y < beast_part_rewards_lines.size(); y++)
				{
					const Dictionary beast_part_reward_entry = beast_part_rewards_lines[y];

					BG_RewardItem *new_beast_part_reward = memnew(BG_RewardItem);
					new_beast_part_reward->id = beast_part_reward_entry["beast_part"];
					new_beast_part_reward->drop_rate = float(beast_part_reward_entry["drop_rate"]);

					new_monster_type->beast_part_rewards.append(new_beast_part_reward);
				}
				monster_types.append(new_monster_type);
			}
		}
	}

	/////
	///// Jobs
	/////
	{
		const Dictionary jobs_sheet = BG_JsonUtils::GetCBDSheet(data, "jobs");
		if (jobs_sheet.has("lines"))
		{
			jobs.clear();

			const Array lines = Array(jobs_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_JobDetails *new_job_class = memnew(BG_JobDetails);
				new_job_class->id = entry["id"];
				new_job_class->name = entry["name"];
				new_job_class->level = int(entry["level"]);
				new_job_class->description = entry["description"];
				new_job_class->weeks = int(entry["weeks"]);
				new_job_class->is_unique = bool(entry["is_unique"]);

				const Array monsters_lines = Array(entry["monsters"]);
				for (int i = 0; i < monsters_lines.size(); i++)
				{
					const Dictionary monster_entry = monsters_lines[i];
					new_job_class->monsters_ids.append(monster_entry["monster"]);
					new_job_class->monsters_spawn_chances.append(float(monster_entry["spawn_chance"]));
				}

				const Array monster_count_range_lines = Array(entry["monster_count_range"]);
				for (int i = 0; i < monster_count_range_lines.size(); i++)
				{
					const Dictionary monster_count_range_entry = monster_count_range_lines[i];
					new_job_class->monster_count_range = Vector2i(int(monster_count_range_entry["min"]), int(monster_count_range_entry["max"]));
					break;
				}

				// TypedArray<String> item_types;
				// item_types.append("weapon_rewards");
				// item_types.append("beast_part_rewards");
				// item_types.append("item_rewards");
				// for (int x = 0; x < item_types.size(); x++)
				// {
				// 	const Array reward_lines = Array(entry[item_types[x]]);
				// 	for (int y = 0; y < reward_lines.size(); y++)
				// 	{
				// 		const Dictionary reward_entry = reward_lines[y];
				// 		BG_RewardItem *new_reward_class = memnew(BG_RewardItem);
				// 		new_reward_class->id = reward_entry["reward"];
				// 		new_reward_class->drop_rate = float(reward_entry["drop_rate"]);
				// 		new_job_class->rewards.append(new_reward_class);
				// 	}
				// }

				const Array acts_lines = Array(entry["acts"]);
				for (int i = 0; i < acts_lines.size(); i++)
				{
					const Dictionary acts_entry = acts_lines[i];
					new_job_class->acts_allowed_in.append(int(acts_entry["act"]));
				}

				jobs.append(new_job_class);
			}
		}
	}

	/////
	///// Item Slot Types
	/////
	{
		const Dictionary item_slot_types_sheet = BG_JsonUtils::GetCBDSheet(data, "item_slot_types");
		if (item_slot_types_sheet.has("lines"))
		{
			const Array lines = Array(item_slot_types_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				item_slot_types.append(entry["id"]);
			}
		}
	}

	/////
	///// Items
	/////
	{
		{
			const Dictionary equipment_sheet = BG_JsonUtils::GetCBDSheet(data, "equipment");
			if (equipment_sheet.has("lines"))
			{
				const Array lines = Array(equipment_sheet["lines"]);
				for (int i = 0; i < lines.size(); i++)
				{
					const Dictionary entry = lines[i];
					BG_ItemDetails *new_item_class = memnew(BG_ItemDetails);
					new_item_class->id = entry["id"];
					new_item_class->name = entry["name"];
					new_item_class->description = entry["description"];
					new_item_class->slot_type_id = entry["slot_type"];
					items.append(new_item_class);

					// Stats
					const Array stats_lines = Array(entry["stats"]);
					for (int y = 0; y < stats_lines.size(); y++)
					{
						const Dictionary stat_entry = stats_lines[y];

						BG_UnitStat *new_stat = memnew(BG_UnitStat);
						new_stat->id = stat_entry["stat"];
						new_stat->offensive_value = int(stat_entry["offensive_value"]);
						new_stat->defensive_value = int(stat_entry["defensive_value"]);

						new_item_class->stats.append(new_stat);
					}
				}
			}

			// const Array columns = Array(equipment_sheet["columns"]);
			// for (int i = 0; i < columns.size(); i++)
			// {
			// 	const Dictionary entry = columns[i];
			// 	if (entry["name"] == "slot_type")
			// 	{
			// 		String slot_types = entry["typeStr"];
			// 		PackedStringArray splt_count = slot_types.split(":");
			// 		PackedStringArray splt_types = splt_count[1].split(",");
			// 		for (int x = 0; x < splt_types.size(); x++)
			// 		{
			// 			globals->item_slot_types.append(splt_types[x]);
			// 		}
			// 		break;
			// 	}
			// }
		}
		{
			const Dictionary beast_parts_sheet = BG_JsonUtils::GetCBDSheet(data, "beast_parts");
			if (beast_parts_sheet.has("lines"))
			{
				const Array lines = Array(beast_parts_sheet["lines"]);
				for (int i = 0; i < lines.size(); i++)
				{
					const Dictionary entry = lines[i];
					BG_ItemDetails *new_item_class = memnew(BG_ItemDetails);
					new_item_class->id = entry["id"];
					new_item_class->name = entry["name"];
					new_item_class->description = entry["description"];
					new_item_class->is_beast_part = true;
					items.append(new_item_class);
				}
			}
		}
		{
			const Dictionary items_sheet = BG_JsonUtils::GetCBDSheet(data, "items");
			if (items_sheet.has("lines"))
			{
				const Array lines = Array(items_sheet["lines"]);
				for (int i = 0; i < lines.size(); i++)
				{
					const Dictionary entry = lines[i];
					BG_ItemDetails *new_item_class = memnew(BG_ItemDetails);
					new_item_class->id = entry["id"];
					new_item_class->name = entry["name"];
					new_item_class->description = entry["description"];
					new_item_class->is_useable_item = true;
					items.append(new_item_class);
				}
			}
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
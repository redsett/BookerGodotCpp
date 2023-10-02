#include "bg_booker_data_base.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

////
//// BG_Item
////
void BG_Item::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Item::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Item::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Item::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Item::set_name);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_Item::get_description);
	ClassDB::bind_method(D_METHOD("set_description"), &BG_Item::set_description);
	ClassDB::bind_method(D_METHOD("get_is_beast_part"), &BG_Item::get_is_beast_part);
	ClassDB::bind_method(D_METHOD("set_is_beast_part"), &BG_Item::set_is_beast_part);
	ClassDB::bind_method(D_METHOD("get_is_useable_item"), &BG_Item::get_is_useable_item);
	ClassDB::bind_method(D_METHOD("set_is_useable_item"), &BG_Item::set_is_useable_item);
	ClassDB::bind_method(D_METHOD("get_slot_type_id"), &BG_Item::get_slot_type_id);
	ClassDB::bind_method(D_METHOD("set_slot_type_id"), &BG_Item::set_slot_type_id);
	// ClassDB::bind_static_method("BG_Item", D_METHOD("get_slot_types"), &BG_Item::get_slot_types);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "description"), "set_description", "get_description");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_beast_part"), "set_is_beast_part", "get_is_beast_part");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_useable_item"), "set_is_useable_item", "get_is_useable_item");
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
	ClassDB::bind_method(D_METHOD("get_specialization"), &BG_BandMember::get_specialization);
	ClassDB::bind_method(D_METHOD("set_specialization"), &BG_BandMember::set_specialization);
	ClassDB::bind_method(D_METHOD("get_equipment"), &BG_BandMember::get_equipment);
	ClassDB::bind_method(D_METHOD("set_equipment"), &BG_BandMember::set_equipment);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "personality_dialgue_id"), "set_personality_dialgue_id", "get_personality_dialgue_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "specialization", PROPERTY_HINT_RESOURCE_TYPE, "BG_BandMemberClass"), "set_specialization", "get_specialization");
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
	ClassDB::bind_method(D_METHOD("get_current_job"), &BG_Band::get_current_job);
	ClassDB::bind_method(D_METHOD("set_current_job"), &BG_Band::set_current_job);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "experience"), "set_experience", "get_experience");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "health"), "set_health", "get_health");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_health"), "set_max_health", "get_max_health");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "resting"), "set_resting", "get_resting");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "band_members"), "set_band_members", "get_band_members");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_job", PROPERTY_HINT_NODE_TYPE, "BG_Job"), "set_current_job", "get_current_job");
}

////
//// BG_UnitStat
////
void BG_UnitStat::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_UnitStat::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_UnitStat::set_name);
	ClassDB::bind_method(D_METHOD("get_value"), &BG_UnitStat::get_value);
	ClassDB::bind_method(D_METHOD("set_value"), &BG_UnitStat::set_value);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
}

////
//// BG_BandMemberClass
////
void BG_BandMemberClass::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandMemberClass::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_BandMemberClass::set_name);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_BandMemberClass::get_stats);
	ClassDB::bind_method(D_METHOD("set_stats"), &BG_BandMemberClass::set_stats);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
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
//// BG_BandInfo
////
void BG_BandInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_band_names"), &BG_BandInfo::get_band_names);
	ClassDB::bind_method(D_METHOD("get_first_names"), &BG_BandInfo::get_first_names);
	ClassDB::bind_method(D_METHOD("get_last_names"), &BG_BandInfo::get_last_names);
	ClassDB::bind_method(D_METHOD("get_personality_dialgue"), &BG_BandInfo::get_personality_dialgue);
	ClassDB::bind_method(D_METHOD("get_level_min"), &BG_BandInfo::get_level_min);
	ClassDB::bind_method(D_METHOD("get_level_max"), &BG_BandInfo::get_level_max);
	ClassDB::bind_method(D_METHOD("get_monthly_cost_level_multiplier"), &BG_BandInfo::get_monthly_cost_level_multiplier);
	ClassDB::bind_method(D_METHOD("get_band_size_min_max"), &BG_BandInfo::get_band_size_min_max);
	ClassDB::bind_method(D_METHOD("get_num_bands_for_hire"), &BG_BandInfo::get_num_bands_for_hire);
	ClassDB::bind_method(D_METHOD("get_unit_classes"), &BG_BandInfo::get_unit_classes);
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
//// BG_Job
////
void BG_Job::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Job::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Job::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Job::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Job::set_name);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_Job::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_Job::set_level);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_Job::get_description);
	ClassDB::bind_method(D_METHOD("set_description"), &BG_Job::set_description);
	ClassDB::bind_method(D_METHOD("get_weeks"), &BG_Job::get_weeks);
	ClassDB::bind_method(D_METHOD("set_weeks"), &BG_Job::set_weeks);
	ClassDB::bind_method(D_METHOD("get_pay"), &BG_Job::get_pay);
	ClassDB::bind_method(D_METHOD("set_pay"), &BG_Job::set_pay);
	ClassDB::bind_method(D_METHOD("get_rewards"), &BG_Job::get_rewards);
	ClassDB::bind_method(D_METHOD("set_rewards"), &BG_Job::set_rewards);
	ClassDB::bind_method(D_METHOD("get_is_unique"), &BG_Job::get_is_unique);
	ClassDB::bind_method(D_METHOD("set_is_unique"), &BG_Job::set_is_unique);
	ClassDB::bind_method(D_METHOD("get_acts_allowed_in"), &BG_Job::get_acts_allowed_in);
	ClassDB::bind_method(D_METHOD("set_acts_allowed_in"), &BG_Job::set_acts_allowed_in);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "description"), "set_description", "get_description");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "weeks"), "set_weeks", "get_weeks");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "pay"), "set_pay", "get_pay");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "rewards", PROPERTY_HINT_RESOURCE_TYPE, "BG_RewardItem"), "set_rewards", "get_rewards");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_unique"), "set_is_unique", "get_is_unique");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "acts_allowed_in"), "set_acts_allowed_in", "get_acts_allowed_in");
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
	ClassDB::bind_method(D_METHOD("get_item_slot_types"), &BG_Booker_Globals::get_item_slot_types);
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
	ClassDB::bind_method(D_METHOD("get_jobs"), &BG_Booker_DB::get_jobs);
	ClassDB::bind_method(D_METHOD("get_job_board"), &BG_Booker_DB::get_job_board);
	ClassDB::bind_method(D_METHOD("get_items"), &BG_Booker_DB::get_items);
	
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

	/////
	///// Globals
	/////
	{
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

	/////
	///// Band Info
	/////
	{
		const Dictionary bands_sheet = BG_JsonUtils::GetCBDSheet(data, "bands");
		const Dictionary lines = Array(bands_sheet["lines"])[0];
		band_info->num_bands_for_hire = lines["num_bands_for_hire"];

		{
			const Array band_names_array = Array(lines["names"]);
			for (int i = 0; i < band_names_array.size(); i++)
			{
				const Dictionary entry = band_names_array[i];
				BG_BandNameInfo *new_band_name_info = memnew(BG_BandNameInfo);
				new_band_name_info->band_name = entry["name"];

				const Array band_hiring_dialogue_choices_array = Array(entry["hiring_dialogue_choices"]);
				for (int x = 0; x < band_hiring_dialogue_choices_array.size(); x++)
				{
					const Dictionary hiring_dialogue_choices_entry = band_hiring_dialogue_choices_array[x];
					new_band_name_info->hiring_dialogue_choices.append(hiring_dialogue_choices_entry["entry"]);
				}
				band_info->band_names.append(new_band_name_info);
			}
		}

		{
			const Array first_names_array = Array(lines["first_names"]);
			for (int i = 0; i < first_names_array.size(); i++)
			{
				const Dictionary entry = first_names_array[i];
				band_info->first_names.append(entry["name"]);
			}
		}

		{
			const Array last_names_array = Array(lines["last_names"]);
			for (int i = 0; i < last_names_array.size(); i++)
			{
				const Dictionary entry = last_names_array[i];
				band_info->last_names.append(entry["name"]);
			}
		}

		{
			const Array band_sizes_array = Array(lines["band_sizes"]);
			const Dictionary entry = band_sizes_array[0];
			band_info->band_size_min_max = Vector2(entry["band_size_min"], entry["band_size_max"]);
		}

		{
			const Array unit_classes_array = Array(lines["classes"]);
			for (int i = 0; i < unit_classes_array.size(); i++)
			{
				const Dictionary entry = unit_classes_array[i];
				BG_BandMemberClass *new_unit_class = memnew(BG_BandMemberClass);
				new_unit_class->name = entry["name"];

				const Array unit_class_keys_array = entry.keys();
				for (int x = 0; x < unit_class_keys_array.size(); x++)
				{
					if (unit_class_keys_array[x] != "name")
					{
						BG_UnitStat *new_stat = memnew(BG_UnitStat);
						new_stat->name = unit_class_keys_array[x];
						new_stat->value = int(entry[unit_class_keys_array[x]]);

						new_unit_class->stats.append(new_stat);
					}
				}
				band_info->unit_classes.append(new_unit_class);
			}
		}
	}

	/////
	///// Jobs
	/////
	{
		const Dictionary jobs_sheet = BG_JsonUtils::GetCBDSheet(data, "jobs");
		const Array lines = Array(jobs_sheet["lines"]);
		for (int i = 0; i < lines.size(); i++)
		{
			const Dictionary entry = lines[i];
			BG_Job *new_job_class = memnew(BG_Job);
			new_job_class->id = entry["id"];
			new_job_class->name = entry["name"];
			new_job_class->level = int(entry["level"]);
			new_job_class->description = entry["description"];
			new_job_class->weeks = int(entry["weeks"]);
			new_job_class->pay = int(entry["pay"]);
			new_job_class->is_unique = bool(entry["is_unique"]);

			TypedArray<String> item_types;
			item_types.append("weapon_rewards");
			item_types.append("beast_part_rewards");
			item_types.append("item_rewards");
			for (int x = 0; x < item_types.size(); x++)
			{
				const Array reward_lines = Array(entry[item_types[x]]);
				for (int y = 0; y < reward_lines.size(); y++)
				{
					const Dictionary reward_entry = reward_lines[y];
					BG_RewardItem *new_reward_class = memnew(BG_RewardItem);
					new_reward_class->id = reward_entry["reward"];
					new_reward_class->drop_rate = float(reward_entry["drop_rate"]);
					new_job_class->rewards.append(new_reward_class);
				}
			}

			const Array acts_lines = Array(entry["acts"]);
			for (int i = 0; i < acts_lines.size(); i++)
			{
				const Dictionary acts_entry = acts_lines[i];
				new_job_class->acts_allowed_in.append(int(acts_entry["act"]) + 1);
			}

			jobs.append(new_job_class);
		}
	}

	/////
	///// Item Slot Types
	/////
	{
		{
			const Dictionary item_slot_types_sheet = BG_JsonUtils::GetCBDSheet(data, "item_slot_types");
			const Array lines = Array(item_slot_types_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				globals->item_slot_types.append(entry["id"]);
			}
		}
	}

	/////
	///// Items
	/////
	{
		{
			const Dictionary equipment_sheet = BG_JsonUtils::GetCBDSheet(data, "equipment");
			const Array lines = Array(equipment_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_Item *new_item_class = memnew(BG_Item);
				new_item_class->id = entry["id"];
				new_item_class->name = entry["name"];
				new_item_class->description = entry["description"];
				new_item_class->slot_type_id = entry["slot_type"];
				items.append(new_item_class);
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
			const Array lines = Array(beast_parts_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_Item *new_item_class = memnew(BG_Item);
				new_item_class->id = entry["id"];
				new_item_class->name = entry["name"];
				new_item_class->description = entry["description"];
				new_item_class->is_beast_part = true;
				items.append(new_item_class);
			}
		}
		{
			const Dictionary items_sheet = BG_JsonUtils::GetCBDSheet(data, "items");
			const Array lines = Array(items_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_Item *new_item_class = memnew(BG_Item);
				new_item_class->id = entry["id"];
				new_item_class->name = entry["name"];
				new_item_class->description = entry["description"];
				new_item_class->is_useable_item = true;
				items.append(new_item_class);
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
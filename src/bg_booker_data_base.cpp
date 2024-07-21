#include "bg_booker_data_base.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

struct counter
{
	double total_time = 0.0f;
	uint32_t total_times = 0;
};


#define TIME_FUNC(func, cont) \
    do { \
        clock_t start, end; \
        double elapsed; \
        start = clock(); \
        func.call(); \
        end = clock(); \
        elapsed = (double)(end - start) / CLOCKS_PER_SEC; \
		cont.total_time += elapsed; \
		cont.total_times++; \
        UtilityFunctions::print("The function '", func.get_method(), "' took ", elapsed, " seconds to execute. Average Time : ", cont.total_time / cont.total_times); \
    } while (0)

static Color convert_int_to_color(int color_int)
{
	const int r = (color_int >> 16) & 255;
	const int g = (color_int >> 8) & 255;
	const int b = color_int & 255;
	return Color(
		r > 0 ? r/255.0 : 0.0, 
		g > 0 ? g/255.0 : 0.0, 
		b > 0 ? b/255.0 : 0.0
	);
}

struct fraction_struct
{
	double integral = 0.0f;
	long numerator = 0;
	long denominator = 0;
};

static long gcd(long a, long b)
{
    if (a == 0)
        return b;
    else if (b == 0)
        return a;

    if (a < b)
        return gcd(a, b % a);
    else
        return gcd(b, a % b);
}

static fraction_struct get_fract(double input)
{
    double integral = std::floor(input);
    double frac = input - integral;

    const long precision = 1000000000; // This is the accuracy.

    long gcd_ = gcd(round(frac * precision), precision);

    long denominator = precision / gcd_;
    long numerator = round(frac * precision) / gcd_;

	fraction_struct frac_struct = fraction_struct();
	frac_struct.integral = integral;
	frac_struct.numerator = numerator;
	frac_struct.denominator = denominator;
	return frac_struct;
    // std::cout << integral << " + ";
    // std::cout << numerator << " / " << denominator << std::endl;
}

////
//// BG_AudioData
////
void BG_AudioData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_AudioData::get_id);
	ClassDB::bind_method(D_METHOD("get_act_file_paths"), &BG_AudioData::get_act_file_paths);
	ClassDB::bind_method(D_METHOD("get_act_volume_db_base_values"), &BG_AudioData::get_act_volume_db_base_values);
}

////
//// BG_Effect
////
void BG_Effect::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Effect::get_id);
	ClassDB::bind_method(D_METHOD("get_nice_name"), &BG_Effect::get_nice_name);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_Effect::get_description);
	ClassDB::bind_method(D_METHOD("get_script_path"), &BG_Effect::get_script_path);
	ClassDB::bind_method(D_METHOD("get_status_icon_path"), &BG_Effect::get_status_icon_path);
	ClassDB::bind_method(D_METHOD("get_value_attributes"), &BG_Effect::get_value_attributes);
}

////
//// BG_Dice
////
void BG_Dice::_bind_methods()
{
	ClassDB::bind_static_method("BG_Dice", D_METHOD("calculate_dice", "dice", "random_number_generator"), &BG_Dice::calculate_dice);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("dice_to_nice_name", "dice"), &BG_Dice::dice_to_nice_name);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("dice_to_string", "dice"), &BG_Dice::dice_to_string);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("string_to_dice", "string"), &BG_Dice::string_to_dice);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("string_to_dice_options", "string"), &BG_Dice::string_to_dice_options);

	ClassDB::bind_method(D_METHOD("get_roll_count"), &BG_Dice::get_roll_count);
	ClassDB::bind_method(D_METHOD("set_roll_count"), &BG_Dice::set_roll_count);
	ClassDB::bind_method(D_METHOD("get_amount_of_sides"), &BG_Dice::get_amount_of_sides);
	ClassDB::bind_method(D_METHOD("set_amount_of_sides"), &BG_Dice::set_amount_of_sides);
	ClassDB::bind_method(D_METHOD("get_additive"), &BG_Dice::get_additive);
	ClassDB::bind_method(D_METHOD("set_additive"), &BG_Dice::set_additive);
}

////
//// BG_RarityDetails
////
void BG_RarityDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_RarityDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_equipment_prefix_name"), &BG_RarityDetails::get_equipment_prefix_name);
	ClassDB::bind_method(D_METHOD("get_beast_part_prefix_name"), &BG_RarityDetails::get_beast_part_prefix_name);
	ClassDB::bind_method(D_METHOD("get_color"), &BG_RarityDetails::get_color);
	ClassDB::bind_method(D_METHOD("get_damage_multiplier"), &BG_RarityDetails::get_damage_multiplier);
	ClassDB::bind_method(D_METHOD("get_percentage_of_all_items_dropped_per_act"), &BG_RarityDetails::get_percentage_of_all_items_dropped_per_act);
}

////
//// BG_LoreRarity
////
void BG_LoreRarity::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_rarity_id"), &BG_LoreRarity::get_rarity_id);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_LoreRarity::get_description);
}

////
//// BG_UnitStatDetails
////
void BG_UnitStatDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitStatDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_nice_name"), &BG_UnitStatDetails::get_nice_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitStatDetails::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_is_damage_type"), &BG_UnitStatDetails::get_is_damage_type);
	ClassDB::bind_method(D_METHOD("get_color"), &BG_UnitStatDetails::get_color);
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
	ClassDB::bind_method(D_METHOD("get_resistant_value"), &BG_UnitStat::get_resistant_value);
	ClassDB::bind_method(D_METHOD("set_resistant_value"), &BG_UnitStat::set_resistant_value);

	ClassDB::bind_method(D_METHOD("get_dice_string"), &BG_UnitStat::get_dice_string);
	ClassDB::bind_method(D_METHOD("get_dice_options"), &BG_UnitStat::get_dice_options);
	ClassDB::bind_method(D_METHOD("get_dice"), &BG_UnitStat::get_dice);
	ClassDB::bind_method(D_METHOD("set_dice"), &BG_UnitStat::set_dice);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bonus_percentage"), "set_bonus_percentage", "get_bonus_percentage");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "offensive_value"), "set_offensive_value", "get_offensive_value");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "defensive_value"), "set_defensive_value", "get_defensive_value");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "resistant_value"), "set_resistant_value", "get_resistant_value");
}

////
//// BG_Item
////
void BG_Item::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Item::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Item::set_id);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_Item::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_Item::set_random_variation);
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
	ClassDB::bind_method(D_METHOD("get_current_durability"), &BG_Item::get_current_durability);
	ClassDB::bind_method(D_METHOD("set_current_durability"), &BG_Item::set_current_durability);
	ClassDB::bind_method(D_METHOD("get_current_fame"), &BG_Item::get_current_fame);
	ClassDB::bind_method(D_METHOD("set_current_fame"), &BG_Item::set_current_fame);
	ClassDB::bind_method(D_METHOD("get_number_of_jobs_participated_in"), &BG_Item::get_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("set_number_of_jobs_participated_in"), &BG_Item::set_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("get_consecutive_number_of_jobs_participated_in"), &BG_Item::get_consecutive_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("set_consecutive_number_of_jobs_participated_in"), &BG_Item::set_consecutive_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("get_rarity_id"), &BG_Item::get_rarity_id);
	ClassDB::bind_method(D_METHOD("set_rarity_id"), &BG_Item::set_rarity_id);
	ClassDB::bind_method(D_METHOD("get_grafts"), &BG_Item::get_grafts);
	ClassDB::bind_method(D_METHOD("set_grafts"), &BG_Item::set_grafts);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "on_shelf"), "set_on_shelf", "get_on_shelf");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "shelf_index"), "set_shelf_index", "get_shelf_index");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_bid"), "set_has_bid", "get_has_bid");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bid_amount"), "set_bid_amount", "get_bid_amount");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_equipped"), "set_is_equipped", "get_is_equipped");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_durability"), "set_current_durability", "get_current_durability");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_fame"), "set_current_fame", "get_current_fame");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "number_of_jobs_participated_in"), "set_number_of_jobs_participated_in", "get_number_of_jobs_participated_in");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "consecutive_number_of_jobs_participated_in"), "set_consecutive_number_of_jobs_participated_in", "get_consecutive_number_of_jobs_participated_in");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "rarity_id"), "set_rarity_id", "get_rarity_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "grafts"), "set_grafts", "get_grafts");
}

////
//// BG_ItemDetails
////
void BG_ItemDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ItemDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_ItemDetails::get_name);
	ClassDB::bind_method(D_METHOD("get_act_introduced_in"), &BG_ItemDetails::get_act_introduced_in);
	ClassDB::bind_method(D_METHOD("get_base_value_override"), &BG_ItemDetails::get_base_value_override);
	ClassDB::bind_method(D_METHOD("get_is_beast_part"), &BG_ItemDetails::get_is_beast_part);
	ClassDB::bind_method(D_METHOD("get_is_useable_item"), &BG_ItemDetails::get_is_useable_item);
	ClassDB::bind_method(D_METHOD("get_slot_type_id"), &BG_ItemDetails::get_slot_type_id);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_ItemDetails::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_mesh_path"), &BG_ItemDetails::get_mesh_path);
	ClassDB::bind_method(D_METHOD("get_lore"), &BG_ItemDetails::get_lore);
	ClassDB::bind_method(D_METHOD("get_caste_ids"), &BG_ItemDetails::get_caste_ids);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_ItemDetails::get_stats);
	ClassDB::bind_method(D_METHOD("get_animation_attach_socket"), &BG_ItemDetails::get_animation_attach_socket);
	ClassDB::bind_method(D_METHOD("get_ability_id"), &BG_ItemDetails::get_ability_id);
	ClassDB::bind_method(D_METHOD("get_effect_ids"), &BG_ItemDetails::get_effect_ids);
}

////
//// BG_BandMember
////
void BG_BandMember::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandMember::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_BandMember::set_name);
	ClassDB::bind_method(D_METHOD("get_current_health"), &BG_BandMember::get_current_health);
	ClassDB::bind_method(D_METHOD("set_current_health"), &BG_BandMember::set_current_health);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_BandMember::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_BandMember::set_level);
	ClassDB::bind_method(D_METHOD("get_experience"), &BG_BandMember::get_experience);
	ClassDB::bind_method(D_METHOD("set_experience"), &BG_BandMember::set_experience);
	ClassDB::bind_method(D_METHOD("get_slot_index"), &BG_BandMember::get_slot_index);
	ClassDB::bind_method(D_METHOD("set_slot_index"), &BG_BandMember::set_slot_index);
	ClassDB::bind_method(D_METHOD("get_scale"), &BG_BandMember::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale"), &BG_BandMember::set_scale);
	ClassDB::bind_method(D_METHOD("get_personality_dialgue_id"), &BG_BandMember::get_personality_dialgue_id);
	ClassDB::bind_method(D_METHOD("set_personality_dialgue_id"), &BG_BandMember::set_personality_dialgue_id);
	ClassDB::bind_method(D_METHOD("get_caste"), &BG_BandMember::get_caste);
	ClassDB::bind_method(D_METHOD("set_caste"), &BG_BandMember::set_caste);
	ClassDB::bind_method(D_METHOD("get_equipment"), &BG_BandMember::get_equipment);
	ClassDB::bind_method(D_METHOD("set_equipment"), &BG_BandMember::set_equipment);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_health"), "set_current_health", "get_current_health");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "experience"), "set_experience", "get_experience");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "slot_index"), "set_slot_index", "get_slot_index");
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
	ClassDB::bind_method(D_METHOD("get_resting"), &BG_Band::get_resting);
	ClassDB::bind_method(D_METHOD("set_resting"), &BG_Band::set_resting);
	ClassDB::bind_method(D_METHOD("get_band_members"), &BG_Band::get_band_members);
	ClassDB::bind_method(D_METHOD("set_band_members"), &BG_Band::set_band_members);
	ClassDB::bind_method(D_METHOD("get_current_unique_job_id"), &BG_Band::get_current_unique_job_id);
	ClassDB::bind_method(D_METHOD("set_current_unique_job_id"), &BG_Band::set_current_unique_job_id);

	ClassDB::bind_method(D_METHOD("has_job"), &BG_Band::has_job);
	ClassDB::bind_method(D_METHOD("clear_job"), &BG_Band::clear_job);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "resting"), "set_resting", "get_resting");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "band_members"), "set_band_members", "get_band_members");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_unique_job_id"), "set_current_unique_job_id", "get_current_unique_job_id");
}

////
//// BG_UnitCaste
////
void BG_UnitCaste::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitCaste::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_UnitCaste::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_UnitCaste::get_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitCaste::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_lod_mesh_paths"), &BG_UnitCaste::get_lod_mesh_paths);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_UnitCaste::get_stats);
	ClassDB::bind_method(D_METHOD("set_stats"), &BG_UnitCaste::set_stats);
	ClassDB::bind_method(D_METHOD("get_starting_item_ids"), &BG_UnitCaste::get_starting_item_ids);

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
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_Monster::get_max_health);
	ClassDB::bind_method(D_METHOD("get_current_health"), &BG_Monster::get_current_health);
	ClassDB::bind_method(D_METHOD("set_current_health"), &BG_Monster::set_current_health);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_Monster::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_Monster::set_random_variation);
	ClassDB::bind_method(D_METHOD("get_challenge_rating"), &BG_Monster::get_challenge_rating);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_Monster::get_stats);

	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_Monster::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_effect_ids"), &BG_Monster::get_effect_ids);
	ClassDB::bind_method(D_METHOD("get_beast_part_rewards"), &BG_Monster::get_beast_part_rewards);
	ClassDB::bind_method(D_METHOD("get_equipment_rewards"), &BG_Monster::get_equipment_rewards);
	ClassDB::bind_method(D_METHOD("get_challenge_rating_faction_string"), &BG_Monster::get_challenge_rating_faction_string);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_health"), "set_current_health", "get_current_health");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
}

String BG_Monster::get_challenge_rating_faction_string() const
{
	String result = "";
	fraction_struct fract_struct = get_fract(get_challenge_rating());
	if (fract_struct.integral > 0.0f)
	{
		result += String::num_int64(int(fract_struct.integral)) + " ";
	}
	if (fract_struct.numerator > 0)
	{
		result += String::num_int64(fract_struct.numerator);
		result += "/";
		result += String::num_int64(fract_struct.denominator);
	}
	return result;
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
	ClassDB::bind_method(D_METHOD("get_max_band_member_level"), &BG_BandInfo::get_max_band_member_level);
	ClassDB::bind_method(D_METHOD("get_max_number_of_bands"), &BG_BandInfo::get_max_number_of_bands);
}

////
//// BG_RewardItem
////
void BG_RewardItem::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_RewardItem::get_id);
	ClassDB::bind_method(D_METHOD("get_drop_rate"), &BG_RewardItem::get_drop_rate);
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
	ClassDB::bind_method(D_METHOD("get_weeks_before_expire"), &BG_JobDetails::get_weeks_before_expire);
	ClassDB::bind_method(D_METHOD("get_monsters_ids"), &BG_JobDetails::get_monsters_ids);
	ClassDB::bind_method(D_METHOD("get_monsters_spawn_chances"), &BG_JobDetails::get_monsters_spawn_chances);
	ClassDB::bind_method(D_METHOD("get_monster_count_range"), &BG_JobDetails::get_monster_count_range);
	ClassDB::bind_method(D_METHOD("get_is_unique"), &BG_JobDetails::get_is_unique);
	ClassDB::bind_method(D_METHOD("get_is_boss"), &BG_JobDetails::get_is_boss);
	ClassDB::bind_method(D_METHOD("get_acts_allowed_in"), &BG_JobDetails::get_acts_allowed_in);
	ClassDB::bind_method(D_METHOD("get_drop_rate_adder"), &BG_JobDetails::get_drop_rate_adder);
}

////
//// BG_LevelGuide
////
void BG_LevelGuide::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_job_rep_reward"), &BG_LevelGuide::get_job_rep_reward);
	ClassDB::bind_method(D_METHOD("get_job_duralation"), &BG_LevelGuide::get_job_duralation);
	ClassDB::bind_method(D_METHOD("get_rest_duralation"), &BG_LevelGuide::get_rest_duralation);
	ClassDB::bind_method(D_METHOD("get_leveling_speed"), &BG_LevelGuide::get_leveling_speed);
	ClassDB::bind_method(D_METHOD("get_band_member_upkeep"), &BG_LevelGuide::get_band_member_upkeep);
	ClassDB::bind_method(D_METHOD("get_monster_health"), &BG_LevelGuide::get_monster_health);
	ClassDB::bind_method(D_METHOD("get_monster_base_off_stat"), &BG_LevelGuide::get_monster_base_off_stat);
	ClassDB::bind_method(D_METHOD("get_monster_base_def_stat"), &BG_LevelGuide::get_monster_base_def_stat);
	ClassDB::bind_method(D_METHOD("get_item_durability_consumption_per_job_level"), &BG_LevelGuide::get_item_durability_consumption_per_job_level);
	ClassDB::bind_method(D_METHOD("get_item_fame_addition_per_job_level"), &BG_LevelGuide::get_item_fame_addition_per_job_level);
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
//// BG_EquipmentAnimationDetails
////
void BG_EquipmentAnimationDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_caste_id"), &BG_EquipmentAnimationDetails::get_caste_id);
	ClassDB::bind_method(D_METHOD("get_equipment_id"), &BG_EquipmentAnimationDetails::get_equipment_id);
	ClassDB::bind_method(D_METHOD("get_in_game_animation_name"), &BG_EquipmentAnimationDetails::get_in_game_animation_name);
}

////
//// BG_Booker_Globals
////
void BG_Booker_Globals::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_starting_reputation"), &BG_Booker_Globals::get_starting_reputation);
	ClassDB::bind_method(D_METHOD("get_starting_job_count"), &BG_Booker_Globals::get_starting_job_count);
	ClassDB::bind_method(D_METHOD("get_jobs_per_month"), &BG_Booker_Globals::get_jobs_per_month);
	ClassDB::bind_method(D_METHOD("get_act_stats"), &BG_Booker_Globals::get_act_stats);
	ClassDB::bind_method(D_METHOD("get_level_guide"), &BG_Booker_Globals::get_level_guide);
	ClassDB::bind_method(D_METHOD("get_job_level_range_min_max"), &BG_Booker_Globals::get_job_level_range_min_max);
	ClassDB::bind_method(D_METHOD("get_base_equipment_value_for_act"), &BG_Booker_Globals::get_base_equipment_value_for_act);
	ClassDB::bind_method(D_METHOD("get_base_beast_part_value_for_act"), &BG_Booker_Globals::get_base_beast_part_value_for_act);
	ClassDB::bind_method(D_METHOD("get_equipment_value_rarity_multiplier"), &BG_Booker_Globals::get_equipment_value_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_beast_part_value_rarity_multiplier"), &BG_Booker_Globals::get_beast_part_value_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_extra_beast_part_value_rarity_multiplier_while_grafted"), &BG_Booker_Globals::get_extra_beast_part_value_rarity_multiplier_while_grafted);
	ClassDB::bind_method(D_METHOD("get_equipment_max_durability_per_act"), &BG_Booker_Globals::get_equipment_max_durability_per_act);
	ClassDB::bind_method(D_METHOD("get_equipment_max_fame_per_act"), &BG_Booker_Globals::get_equipment_max_fame_per_act);
	ClassDB::bind_method(D_METHOD("get_beast_part_max_durability_per_act"), &BG_Booker_Globals::get_beast_part_max_durability_per_act);
	ClassDB::bind_method(D_METHOD("get_beast_part_max_fame_per_act"), &BG_Booker_Globals::get_beast_part_max_fame_per_act);
	ClassDB::bind_method(D_METHOD("get_equipment_durability_rarity_multiplier"), &BG_Booker_Globals::get_equipment_durability_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_beast_part_durability_rarity_multiplier"), &BG_Booker_Globals::get_beast_part_durability_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_equipment_fame_rarity_multiplier"), &BG_Booker_Globals::get_equipment_fame_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_beast_part_fame_rarity_multiplier"), &BG_Booker_Globals::get_beast_part_fame_rarity_multiplier);
}

////
//// BG_Booker_DB
////
BG_Booker_DB *BG_Booker_DB::singleton = nullptr;

void BG_Booker_DB::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("refresh_data"), &BG_Booker_DB::refresh_data);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("timer_test"), &BG_Booker_DB::timer_test);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("get_job_challenge_rating", "monsters"), &BG_Booker_DB::get_job_challenge_rating);

	ClassDB::bind_method(D_METHOD("get_modding_path"), &BG_Booker_DB::get_modding_path);
	ClassDB::bind_method(D_METHOD("get_globals"), &BG_Booker_DB::get_globals);
	ClassDB::bind_method(D_METHOD("get_audio_data"), &BG_Booker_DB::get_audio_data);
	ClassDB::bind_method(D_METHOD("get_jobs"), &BG_Booker_DB::get_jobs);
	ClassDB::bind_method(D_METHOD("get_items"), &BG_Booker_DB::get_items);
	ClassDB::bind_method(D_METHOD("get_abilities"), &BG_Booker_DB::get_abilities);
	ClassDB::bind_method(D_METHOD("get_effects"), &BG_Booker_DB::get_effects);
	ClassDB::bind_method(D_METHOD("get_equipment_animation_details"), &BG_Booker_DB::get_equipment_animation_details);
	ClassDB::bind_method(D_METHOD("get_band_info"), &BG_Booker_DB::get_band_info);
	ClassDB::bind_method(D_METHOD("get_item_slot_types"), &BG_Booker_DB::get_item_slot_types);
	ClassDB::bind_method(D_METHOD("get_rarity_types"), &BG_Booker_DB::get_rarity_types);
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
	static counter cont = counter();
	TIME_FUNC(callable, cont);
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
			if (lines.has("starting_coin"))
				globals->starting_reputation = lines["starting_coin"];
			if (lines.has("starting_job_count"))
				globals->starting_job_count = lines["starting_job_count"];

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

			if (lines.has("jobs_per_month"))
			{
				globals->jobs_per_month.clear();

				const Array jobs_per_month_array = Array(lines["jobs_per_month"]);
				for (int i = 0; i < jobs_per_month_array.size(); i++)
				{
					const Dictionary month = jobs_per_month_array[i];
					globals->jobs_per_month.append(int(month["count"]));
				}
			}

			if (lines.has("job_level_range"))
			{
				globals->job_level_range_min_max.clear();

				const Array job_level_range_array = Array(lines["job_level_range"]);
				for (int i = 0; i < job_level_range_array.size(); i++)
				{
					const Dictionary month = job_level_range_array[i];
					globals->job_level_range_min_max.append(Vector2i(int(month["min"]), int(month["max"])));
				}
			}

			if (lines.has("item_value_per_act"))
			{
				globals->base_equipment_value_for_act.clear();
				globals->base_beast_part_value_for_act.clear();

				const Array item_value_per_act_array = Array(lines["item_value_per_act"]);
				for (int i = 0; i < item_value_per_act_array.size(); i++)
				{
					const Dictionary values = item_value_per_act_array[i];
					globals->base_equipment_value_for_act.append(int(values["base_equipment_value_for_act"]));
					globals->base_beast_part_value_for_act.append(int(values["base_beast_part_value_for_act"]));
				}
			}

			if (lines.has("item_rarity_multiplier"))
			{
				globals->equipment_value_rarity_multiplier.clear();
				globals->beast_part_value_rarity_multiplier.clear();
				globals->extra_beast_part_value_rarity_multiplier_while_grafted.clear();
				globals->equipment_durability_rarity_multiplier.clear();
				globals->beast_part_durability_rarity_multiplier.clear();
				globals->equipment_fame_rarity_multiplier.clear();
				globals->beast_part_fame_rarity_multiplier.clear();

				const Array item_rarity_multiplier_array = Array(lines["item_rarity_multiplier"]);
				for (int i = 0; i < item_rarity_multiplier_array.size(); i++)
				{
					const Dictionary values = item_rarity_multiplier_array[i];
					globals->equipment_value_rarity_multiplier.append(float(values["equipment_value_rarity_multiplier"]));
					globals->beast_part_value_rarity_multiplier.append(float(values["beast_part_value_rarity_multiplier"]));
					globals->extra_beast_part_value_rarity_multiplier_while_grafted.append(float(values["extra_beast_part_value_rarity_multiplier_while_grafted"]));
					globals->equipment_durability_rarity_multiplier.append(float(values["equipment_durability_rarity_multiplier"]));
					globals->beast_part_durability_rarity_multiplier.append(float(values["beast_part_durability_rarity_multiplier"]));
					globals->equipment_fame_rarity_multiplier.append(float(values["equipment_fame_rarity_multiplier"]));
					globals->beast_part_fame_rarity_multiplier.append(float(values["beast_part_fame_rarity_multiplier"]));
				}
			}

			if (lines.has("item_durability_per_act"))
			{
				globals->equipment_max_durability_per_act.clear();
				globals->beast_part_max_durability_per_act.clear();

				const Array item_durability_per_act_array = Array(lines["item_durability_per_act"]);
				for (int i = 0; i < item_durability_per_act_array.size(); i++)
				{
					const Dictionary values = item_durability_per_act_array[i];
					globals->equipment_max_durability_per_act.append(float(values["equipment"]));
					globals->beast_part_max_durability_per_act.append(float(values["beast_part"]));
				}
			}

			if (lines.has("item_fame_per_act"))
			{
				globals->equipment_max_fame_per_act.clear();
				globals->beast_part_max_fame_per_act.clear();

				const Array item_fame_per_act_array = Array(lines["item_fame_per_act"]);
				for (int i = 0; i < item_fame_per_act_array.size(); i++)
				{
					const Dictionary values = item_fame_per_act_array[i];
					globals->equipment_max_fame_per_act.append(float(values["equipment"]));
					globals->beast_part_max_fame_per_act.append(float(values["beast_part"]));
				}
			}
		}
	}

	/////
	///// Level Guide
	/////
	{
		const Dictionary level_guide_sheet = BG_JsonUtils::GetCBDSheet(data, "level_guide");
		if (level_guide_sheet.has("lines"))
		{
			globals->level_guide.clear();

			const Array lines = Array(level_guide_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_LevelGuide *level_guide_class = memnew(BG_LevelGuide);
				level_guide_class->job_rep_reward = int(entry["job_rep_reward"]);
				level_guide_class->job_duralation = int(entry["job_duralation"]);
				level_guide_class->rest_duralation = float(entry["rest_duralation"]);
				level_guide_class->leveling_speed = float(entry["leveling_speed"]);
				level_guide_class->band_member_upkeep = int(entry["band_member_upkeep"]);
				level_guide_class->monster_health = int(entry["monster_health"]);
				level_guide_class->monster_base_off_stat = int(entry["monster_base_off_stat"]);
				level_guide_class->monster_base_def_stat = int(entry["monster_base_def_stat"]);
				level_guide_class->item_durability_consumption_per_job_level = float(entry["item_durability_consumption_per_job_level"]);
				level_guide_class->item_fame_addition_per_job_level = float(entry["item_fame_addition_per_job_level"]);
				globals->level_guide.append(level_guide_class);
			}
		}
	}

	/////
	///// Audio
	/////
	{
		const Dictionary audio_sheet = BG_JsonUtils::GetCBDSheet(data, "audio");
		if (audio_sheet.has("lines"))
		{
			audio_data.clear();
			const Array lines = Array(audio_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_AudioData *new_audio_class = memnew(BG_AudioData);
				new_audio_class->id = entry["id"];

				const Array per_act_lines = Array(entry["per_act"]);
				for (int y = 0; y < per_act_lines.size(); y++)
				{
					const Dictionary per_act_entry = per_act_lines[y];
					new_audio_class->act_file_paths.append(per_act_entry["path"]);
					new_audio_class->act_volume_db_base_values.append(int(per_act_entry["volume_db"]));
				}
				audio_data.append(new_audio_class);
			}
		}
	}

	/////
	///// Rarity Types
	/////
	{
		const Dictionary rarity_types_sheet = BG_JsonUtils::GetCBDSheet(data, "rarity_types");
		if (rarity_types_sheet.has("lines"))
		{
			rarity_types.clear();

			const Array lines = Array(rarity_types_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				
				BG_RarityDetails *new_rarity_type = memnew(BG_RarityDetails);
				new_rarity_type->id = entry["id"];
				new_rarity_type->equipment_prefix_name = entry["equipment_prefix_name"];
				new_rarity_type->beast_part_prefix_name = entry["beast_part_prefix_name"];
				new_rarity_type->color = convert_int_to_color(int(entry["color"]));
				const float color_muitiplier = float(entry["color_muitiplier"]);
				new_rarity_type->color.r *= color_muitiplier;
				new_rarity_type->color.g *= color_muitiplier;
				new_rarity_type->color.b *= color_muitiplier;
				new_rarity_type->damage_multiplier = float(entry["damage_multiplier"]);

				const Array percentage_of_all_items_dropped_per_act_lines = Array(entry["percentage_of_all_items_dropped_per_act"]);
				for (int y = 0; y < percentage_of_all_items_dropped_per_act_lines.size(); y++)
				{
					const Dictionary percentage_of_all_items_dropped_entry = percentage_of_all_items_dropped_per_act_lines[y];
					new_rarity_type->percentage_of_all_items_dropped_per_act.append(float(percentage_of_all_items_dropped_entry["percent"]));
				}

				rarity_types.append(new_rarity_type);
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
				new_stat_types->nice_name = entry["name"];
				new_stat_types->icon_path = entry["icon_path"];
				new_stat_types->is_damage_type = bool(entry["is_damage_type"]);
				new_stat_types->color = convert_int_to_color(int(entry["color"]));

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
			if (lines.has("max_band_member_level"))
				band_info->max_band_member_level = lines["max_band_member_level"];
			if (lines.has("max_number_of_bands"))
				band_info->max_number_of_bands = lines["max_number_of_bands"];

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
				if (bool(entry["disabled"]))
					continue;

				BG_UnitCaste *new_unit_caste = memnew(BG_UnitCaste);
				new_unit_caste->id = entry["id"];
				new_unit_caste->name = entry["name"];
				new_unit_caste->icon_path = entry["icon_path"];

				new_unit_caste->lod_mesh_paths.clear();
				const Array lod_mesh_path_lines = Array(entry["lod_mesh_paths"]);
				for (int y = 0; y < lod_mesh_path_lines.size(); y++)
				{
					const Dictionary lod_mesh_path_entry = lod_mesh_path_lines[y];
					new_unit_caste->lod_mesh_paths.append(lod_mesh_path_entry["path"]);
				}

				const Array damage_type_lines = Array(entry["base_damage_type_stats"]);
				for (int y = 0; y < damage_type_lines.size(); y++)
				{
					const Dictionary damage_type_entry = damage_type_lines[y];

					BG_UnitStat *new_stat = memnew(BG_UnitStat);
					new_stat->id = damage_type_entry["damage_type"];
					new_stat->bonus_percentage = float(damage_type_entry["base_bonus_percentage"]);
					new_stat->defensive_value = int(damage_type_entry["starting_value"]);

					new_unit_caste->stats.append(new_stat);
				}

				const Array starting_items_lines = Array(entry["starting_items"]);
				for (int y = 0; y < starting_items_lines.size(); y++)
				{
					const Dictionary starting_item_entry = starting_items_lines[y];
					new_unit_caste->starting_item_ids.append(starting_item_entry["item"]);
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
				new_monster_type->challenge_rating = float(entry["challenge_rating"]);
				//new_monster_type->level = int(entry["level"]);
				new_monster_type->max_health = int(entry["health"]);
				new_monster_type->icon_path = entry["icon_path"];

				// Effects
				const Array effect_lines = Array(entry["effects"]);
				for (int y = 0; y < effect_lines.size(); y++)
				{
					const Dictionary effect_entry = effect_lines[y];
					new_monster_type->effect_ids.append(effect_entry["effect"]);
				}

				// Stats
				// int defensive_stat_count = 0;
				const Array stats_lines = Array(entry["stats"]);
				for (int y = 0; y < stats_lines.size(); y++)
				{
					const Dictionary stat_entry = stats_lines[y];

					BG_UnitStat *new_stat = memnew(BG_UnitStat);
					new_stat->id = stat_entry["stat"];

					// bool is_defensive_stat = false;
					// for (int z = 0; z < stat_types.size(); z++) {
					// 	BG_UnitStatDetails *stat = cast_to<BG_UnitStatDetails>(stat_types[z]);
					// 	if (stat->id == new_stat->id && !stat->is_damage_type) {
					// 		is_defensive_stat = true;
					// 		defensive_stat_count += 1;
					// 	}
					// }

					new_stat->resistant_value = int(stat_entry["resistant_value"]);

					// float offensive_percentage = float(stat_entry["offensive_percentage"]);
					// float defensive_percentage = float(stat_entry["defensive_percentage"]);
					// if (is_defensive_stat && defensive_stat_count == 1) {
					// 	new_stat->defensive_value = defensive_percentage * cast_to<BG_LevelGuide>(globals->level_guide[new_monster_type->level - 1])->monster_health;
					// } else {
					// 	new_stat->offensive_value = offensive_percentage * cast_to<BG_LevelGuide>(globals->level_guide[new_monster_type->level - 1])->monster_base_off_stat;
					// 	new_stat->defensive_value = defensive_percentage * cast_to<BG_LevelGuide>(globals->level_guide[new_monster_type->level - 1])->monster_base_def_stat;
					// }
					new_stat->dice_string = stat_entry["damage_dice"];
					new_stat->dice_options = BG_Dice::string_to_dice_options(new_stat->dice_string);
					
					new_monster_type->stats.append(new_stat);
				}

				// Beast Part Rewards
				const Array beast_part_rewards_lines = Array(entry["beast_part_rewards"]);
				for (int y = 0; y < beast_part_rewards_lines.size(); y++)
				{
					const Dictionary beast_part_reward_entry = beast_part_rewards_lines[y];

					BG_RewardItem *new_beast_part_reward = memnew(BG_RewardItem);
					new_beast_part_reward->id = beast_part_reward_entry["beast_part"];
					new_beast_part_reward->drop_rate = float(beast_part_reward_entry["drop_rate"]);

					new_monster_type->beast_part_rewards.append(new_beast_part_reward);
				}

				// Equipment Rewards
				const Array equipment_rewards_lines = Array(entry["equipment_rewards"]);
				for (int y = 0; y < equipment_rewards_lines.size(); y++)
				{
					const Dictionary equipment_reward_entry = equipment_rewards_lines[y];

					BG_RewardItem *new_equipment_reward = memnew(BG_RewardItem);
					new_equipment_reward->id = equipment_reward_entry["equipment"];
					new_equipment_reward->drop_rate = float(equipment_reward_entry["drop_rate"]);

					new_monster_type->equipment_rewards.append(new_equipment_reward);
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
				new_job_class->drop_rate_adder = entry["drop_rate_adder"];
				new_job_class->weeks = int(entry["weeks"]);
				new_job_class->weeks_before_expire = int(entry["weeks_before_expire"]);
				new_job_class->is_unique = bool(entry["is_unique"]);
				new_job_class->is_boss = bool(entry["is_boss"]);

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
			item_slot_types.clear();
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
				items.clear();
				const Array lines = Array(equipment_sheet["lines"]);
				for (int i = 0; i < lines.size(); i++)
				{
					const Dictionary entry = lines[i];
					if (bool(entry["disabled"]))
						continue;

					BG_ItemDetails *new_item_class = memnew(BG_ItemDetails);
					new_item_class->id = entry["id"];
					new_item_class->name = entry["name"];
					new_item_class->icon_path = entry["icon_path"];
					new_item_class->mesh_path = entry["mesh_path"];
					new_item_class->act_introduced_in = int(entry["act_introduced_in"]);
					new_item_class->slot_type_id = entry["slot_type"];
					new_item_class->ability_id = entry["ability"];
					new_item_class->animation_attach_socket = entry["anim_attach_socket"];

					// Lore
					new_item_class->lore.clear();
					const Array lore_lines = Array(entry["lore"]);
					for (int y = 0; y < lore_lines.size(); y++)
					{
						const Dictionary lore_entry = lore_lines[y];
						BG_LoreRarity *new_lore = memnew(BG_LoreRarity);
						new_lore->rarity_id = lore_entry["rarity_id"];
						new_lore->description = lore_entry["description"];
						new_item_class->lore.append(new_lore);
					}

					// Caste
					new_item_class->caste_ids.clear();
					const Array caste_lines = Array(entry["caste"]);
					for (int y = 0; y < caste_lines.size(); y++)
					{
						const Dictionary caste_entry = caste_lines[y];
						new_item_class->caste_ids.append(caste_entry["id"]);
					}

					// Stats
					const Array stats_lines = Array(entry["stats"]);
					for (int y = 0; y < stats_lines.size(); y++)
					{
						const Dictionary stat_entry = stats_lines[y];

						BG_UnitStat *new_stat = memnew(BG_UnitStat);
						new_stat->id = stat_entry["stat"];
						new_stat->resistant_value = int(stat_entry["resistant_value"]);
						new_stat->dice_string = stat_entry["damage_dice"];
						new_stat->dice_options = BG_Dice::string_to_dice_options(new_stat->dice_string);

						new_item_class->stats.append(new_stat);
					}

					// Effects
					const Array effect_lines = Array(entry["effects"]);
					for (int y = 0; y < effect_lines.size(); y++)
					{
						const Dictionary effect_entry = effect_lines[y];
						new_item_class->effect_ids.append(effect_entry["effect"]);
					}

					items.append(new_item_class);
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
					new_item_class->icon_path = entry["icon_path"];
					new_item_class->act_introduced_in = int(entry["act_introduced_in"]);
					new_item_class->base_value_override = int(entry["base_value_override"]);
					new_item_class->is_beast_part = true;

					// Stats
					const Array stats_lines = Array(entry["stats"]);
					for (int y = 0; y < stats_lines.size(); y++)
					{
						const Dictionary stat_entry = stats_lines[y];

						BG_UnitStat *new_stat = memnew(BG_UnitStat);
						new_stat->id = stat_entry["stat"];
						new_stat->bonus_percentage = float(stat_entry["offensive_bonus"]);
						new_stat->defensive_value = int(stat_entry["defensive_value"]);
						new_stat->set_dice(BG_Dice::string_to_dice(stat_entry["damage_dice"]));

						new_item_class->stats.append(new_stat);
					}

					// Effects
					const Array effect_lines = Array(entry["effects"]);
					for (int y = 0; y < effect_lines.size(); y++)
					{
						const Dictionary effect_entry = effect_lines[y];
						new_item_class->effect_ids.append(effect_entry["effect"]);
					}
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
					new_item_class->icon_path = entry["icon_path"];
					new_item_class->is_useable_item = true;
					items.append(new_item_class);
				}
			}
		}
	}

	/////
	///// Abilities
	/////
	{
		const Dictionary abilities_sheet = BG_JsonUtils::GetCBDSheet(data, "abilities");
		if (abilities_sheet.has("lines"))
		{
			abilities.clear();
			const Array lines = Array(abilities_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_Effect *new_ability_class = memnew(BG_Effect);
				new_ability_class->id = entry["id"];
				new_ability_class->description = entry["description"];
				new_ability_class->script_path = entry["script_path"];
				new_ability_class->status_icon_path = entry["status_icon"];
				abilities.append(new_ability_class);
			}
		}
	}

	/////
	///// Effects
	/////
	{
		const Dictionary effects_sheet = BG_JsonUtils::GetCBDSheet(data, "effects");
		if (effects_sheet.has("lines"))
		{
			effects.clear();
			const Array lines = Array(effects_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_Effect *new_effect_class = memnew(BG_Effect);
				new_effect_class->id = entry["id"];
				new_effect_class->nice_name = entry["name"];
				new_effect_class->description = entry["description"];
				new_effect_class->script_path = entry["script_path"];
				new_effect_class->status_icon_path = entry["status_icon"];

				// Value Attributes
				const Array value_attribute_lines = Array(entry["value_attributes"]);
				for (int y = 0; y < value_attribute_lines.size(); y++)
				{
					const Dictionary value_attribute_entry = value_attribute_lines[y];

					String name = value_attribute_entry["name"];
					String value1 = value_attribute_entry["value_1"];
					String value2 = value_attribute_entry["value_2"];

					Array values;
					if (!value1.is_empty())
						values.append(value1);
					if (!value2.is_empty())
						values.append(value2);

					if (!values.is_empty())
						new_effect_class->value_attributes[name] = values;
				}

				effects.append(new_effect_class);
			}
		}
	}

	/////
	///// Equipment Animation Details
	/////
	{
		const Dictionary animation_sheet = BG_JsonUtils::GetCBDSheet(data, "animations");
		if (animation_sheet.has("lines"))
		{
			equipment_animation_details.clear();
			const Array lines = Array(animation_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_EquipmentAnimationDetails *new_anim_details_class = memnew(BG_EquipmentAnimationDetails);
				new_anim_details_class->caste_id = entry["caste_id"];
				new_anim_details_class->equipment_id = entry["equipment_id"];
				new_anim_details_class->in_game_animation_name = entry["in_game_animation_name"];
				equipment_animation_details.append(new_anim_details_class);
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

/* static */ String BG_Booker_DB::get_job_challenge_rating(TypedArray<BG_Monster> monsters)
{
	String result;

	const TypedArray<BG_Monster> global_monster_types = BG_Booker_DB::get_singleton()->get_monster_types();

	TypedArray<BG_Monster> unique_monsters;
	TypedArray<float> challenge_ratings;
	TypedArray<int> monster_counts;
	for (int i = 0; i < monsters.size(); i++)
	{
		const BG_Monster *monster = nullptr;
		const BG_Monster *m = cast_to<BG_Monster>(monsters[i]);
		// Find the global version of this monster, as it has more details that were not saved in the resource.
		for (int x = 0; x < global_monster_types.size(); x++)
		{
			const BG_Monster *gm = cast_to<BG_Monster>(global_monster_types[x]);
			if (m->get_id() == gm->get_id())
			{
				monster = gm;
				break;
			}
		}

		if (monster == nullptr)
			break;

		if (unique_monsters.count(monster) > 0)
		{
			const int array_index = unique_monsters.find(monster);
			monster_counts[array_index] = int(monster_counts[array_index]) + 1;
		}
		else
		{
			unique_monsters.append(monster);
			challenge_ratings.append(monster->get_challenge_rating());
			monster_counts.append(1);
		}
	}

	float challenge_rating = 0.0f;
	for (int i = 0; i < challenge_ratings.size(); i++)
	{
		challenge_rating += float(challenge_ratings[i]) * float(monster_counts[i]);
	}
	const int total_monster_count = monsters.size();
	challenge_rating = challenge_rating + total_monster_count * 0.25f;

	fraction_struct fract_struct = get_fract(challenge_rating);
	if (fract_struct.integral > 0.0f)
	{
		result += String::num_int64(int(fract_struct.integral)) + " ";
	}
	if (fract_struct.numerator > 0)
	{
		result += String::num_int64(fract_struct.numerator);
		result += "/";
		result += String::num_int64(fract_struct.denominator);
	}
	return result;
}

////
//// BG_Dice
////
/* static */ int BG_Dice::calculate_dice(const TypedArray<BG_Dice> dice, RandomNumberGenerator *random_num_generator)
{
	int result = 0;
	for (int i = 0; i < dice.size(); i++)
	{
		if (!dice[i])
		{
			continue;
		}
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			if (UtilityFunctions::is_instance_valid(random_num_generator))
				result += random_num_generator->randi_range(1, die->get_amount_of_sides());
			else
				result += UtilityFunctions::randi_range(1, die->get_amount_of_sides());
		}
		result += die->get_additive();
	}

	return Math::max(0, result);
}

/* static */ String BG_Dice::dice_to_nice_name(const TypedArray<BG_Dice> dice)
{
	int minimum_damage = 0;
	int maximum_damage = 0;
	for (int i = 0; i < dice.size(); i++)
	{
		if (!dice[i])
		{
			continue;
		}
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			minimum_damage += 1;
			maximum_damage += die->get_amount_of_sides();
		}
		minimum_damage += die->get_additive();
		maximum_damage += die->get_additive();
	}

	return String::num_int64(minimum_damage) + "~" + String::num_int64(maximum_damage);
}

/* static */ String BG_Dice::dice_to_string(const BG_Dice *dice)
{
	if (!UtilityFunctions::is_instance_valid(dice))
	{
		return "-";
	}
	
	String result = String::num_int64(dice->get_roll_count()) + "d" + String::num_int64(dice->get_amount_of_sides());
	if (dice->get_additive() > 0)
		result += "+" + String::num_int64(dice->get_additive());
	else if (dice->get_additive() < 0)
		result += "-" + String::num_int64(abs(dice->get_additive()));
	return result;
}

/* static */ BG_Dice *BG_Dice::string_to_dice(String string)
{
	if (string.is_empty())
	{
		return nullptr;
	}
	BG_Dice *result = memnew(BG_Dice);
	result->set_roll_count(string.split("d")[0].to_int());

	const String after_d = string.split("d")[1].replace(" ", ""); // I.e:   6 or 6+1 or 6-1
	if (after_d.contains("+") || after_d.contains("-"))
	{
		if (after_d.contains("+"))
		{
			result->set_amount_of_sides(after_d.split("+")[0].to_int());
			result->set_additive(after_d.split("+")[1].to_int());
		}
		else
		{
			result->set_amount_of_sides(after_d.split("-")[0].to_int());
			result->set_additive(after_d.split("-")[1].to_int() * -1); // Negative the number since it's going to subtract.
		}
	}
	else
	{
		result->set_amount_of_sides(after_d.to_int());
	}
	
	return result;
}

/* static */ TypedArray<BG_Dice> BG_Dice::string_to_dice_options(String string)
{
	TypedArray<BG_Dice> result;
	if (string.is_empty())
	{
		return result;
	}
	if (!string.contains("["))
	{
		result.append(BG_Dice::string_to_dice(string));
		return result;
	}
	else
	{
		const String bracets_removed = string.split("[")[1].split("]")[0].replace(" ", ""); // [1d3-1d6] to 1d3-1d6
		const String start_dice = bracets_removed.split("-")[0];
		const String end_dice = bracets_removed.split("-")[1];
		// Roll count.
		const int start_dice_rolls = start_dice.split("d")[0].to_int();
		const int end_dice_rolls = end_dice.split("d")[0].to_int();
		// Side count.
		const int start_dice_sides = start_dice.split("d")[1].to_int();
		const int end_dice_sides = end_dice.split("d")[1].to_int();

		// Additive
		int additive = 0;
		const String after_d = string.split("d")[string.split("d").size() - 1].replace(" ", "");
		if (after_d.contains("+") || after_d.contains("-"))
		{
			if (after_d.contains("+"))
				additive = after_d.split("+")[1].to_int();
			else
				additive = after_d.split("-")[1].to_int() * -1;
		}
		const bool has_additive = additive != 0;

		for (int r = start_dice_rolls; r <= end_dice_rolls; r++)
		{
			for (int s = start_dice_sides; s <= end_dice_sides; s++)
			{
				if (has_additive)
				{
					if (additive > 0)
						result.append(BG_Dice::string_to_dice( String::num_int64(r) + "d" + String::num_int64(s) + "+" + String::num_int64(additive) ));
					else
						result.append(BG_Dice::string_to_dice( String::num_int64(r) + "d" + String::num_int64(s) + "-" + String::num_int64(abs(additive)) ));
				}
				else
				{
					result.append(BG_Dice::string_to_dice( String::num_int64(r) + "d" + String::num_int64(s) ));
				}
			}
		}
	}
	return result;
}
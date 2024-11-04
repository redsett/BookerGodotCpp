#include "bg_booker_data_base.hpp"
#include "bg_focus_layers.hpp"

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
//// BG_HueShiftData
////
void BG_HueShiftData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_mask_path"), &BG_HueShiftData::get_mask_path);
	ClassDB::bind_method(D_METHOD("get_from_color"), &BG_HueShiftData::get_from_color);
	ClassDB::bind_method(D_METHOD("get_multiplier"), &BG_HueShiftData::get_multiplier);
}

////
//// BG_EffectRarityDetails
////
void BG_EffectRarityDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_rarity_id"), &BG_EffectRarityDetails::get_rarity_id);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_EffectRarityDetails::get_description);
	ClassDB::bind_method(D_METHOD("get_script_path"), &BG_EffectRarityDetails::get_script_path);
	ClassDB::bind_method(D_METHOD("get_value_attributes"), &BG_EffectRarityDetails::get_value_attributes);
}

////
//// BG_Effect
////
void BG_Effect::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Effect::get_id);
	ClassDB::bind_method(D_METHOD("get_nice_name"), &BG_Effect::get_nice_name);
	ClassDB::bind_method(D_METHOD("get_details_per_rarity"), &BG_Effect::get_details_per_rarity);
	ClassDB::bind_method(D_METHOD("get_use_owning_item_icon"), &BG_Effect::get_use_owning_item_icon);
	ClassDB::bind_method(D_METHOD("get_status_icon_path"), &BG_Effect::get_status_icon_path);
}

////
//// BG_Dice
////
void BG_Dice::_bind_methods()
{
	ClassDB::bind_static_method("BG_Dice", D_METHOD("calculate_dice", "dice", "random_number_generator"), &BG_Dice::calculate_dice);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("get_dice_max_roll", "dice"), &BG_Dice::get_dice_max_roll);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("get_dice_average_roll", "dice"), &BG_Dice::get_dice_average_roll);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("dice_to_nice_name", "dice"), &BG_Dice::dice_to_nice_name);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("dice_to_string", "dice"), &BG_Dice::dice_to_string);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("string_to_dice", "string"), &BG_Dice::string_to_dice);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("string_to_dice_options", "string"), &BG_Dice::string_to_dice_options);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("add_bonus_to_die", "die", "bonus"), &BG_Dice::add_bonus_to_die);
	ClassDB::bind_static_method("BG_Dice", D_METHOD("duplicate_dice", "dice"), &BG_Dice::duplicate_dice);

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
	ClassDB::bind_method(D_METHOD("get_weak_to_element"), &BG_UnitStatDetails::get_weak_to_element);
	ClassDB::bind_method(D_METHOD("get_widget_color"), &BG_UnitStatDetails::get_widget_color);
	ClassDB::bind_method(D_METHOD("get_text_color"), &BG_UnitStatDetails::get_text_color);
	ClassDB::bind_method(D_METHOD("get_in_world_color"), &BG_UnitStatDetails::get_in_world_color);
}

////
//// BG_UnitStat
////
void BG_UnitStat::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitStat::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_UnitStat::set_id);
	ClassDB::bind_method(D_METHOD("get_bonus_percentage"), &BG_UnitStat::get_bonus_percentage);

	ClassDB::bind_method(D_METHOD("get_offensive_value"), &BG_UnitStat::get_offensive_value);
	ClassDB::bind_method(D_METHOD("set_offensive_value"), &BG_UnitStat::set_offensive_value);
	ClassDB::bind_method(D_METHOD("get_defensive_value"), &BG_UnitStat::get_defensive_value);
	ClassDB::bind_method(D_METHOD("set_defensive_value"), &BG_UnitStat::set_defensive_value);
	ClassDB::bind_method(D_METHOD("get_resistant_unsaved_stored_value"), &BG_UnitStat::get_resistant_unsaved_stored_value);
	ClassDB::bind_method(D_METHOD("set_resistant_unsaved_stored_value"), &BG_UnitStat::set_resistant_unsaved_stored_value);
	ClassDB::bind_method(D_METHOD("get_resistant_value_text"), &BG_UnitStat::get_resistant_value_text);
	ClassDB::bind_method(D_METHOD("get_resistant_value_min_max"), &BG_UnitStat::get_resistant_value_min_max);
	ClassDB::bind_method(D_METHOD("set_resistant_value_min_max"), &BG_UnitStat::set_resistant_value_min_max);

	ClassDB::bind_method(D_METHOD("get_dice_string"), &BG_UnitStat::get_dice_string);
	ClassDB::bind_method(D_METHOD("get_dice_options"), &BG_UnitStat::get_dice_options);
	ClassDB::bind_method(D_METHOD("get_dice"), &BG_UnitStat::get_dice);
	ClassDB::bind_method(D_METHOD("set_dice"), &BG_UnitStat::set_dice);
}

BG_UnitStat::~BG_UnitStat()
{
	// if (BG_Focus_Layer_Properties::bg_is_instance_valid(dice))
	// {
	// 	memdelete(dice);
	// }
	// for (int i = 0; i < dice_options.size(); i++)
	// {
	// 	const BG_Dice *die = cast_to<BG_Dice>(dice_options[i]);
	// 	if (BG_Focus_Layer_Properties::bg_is_instance_valid(die))
	// 		memdelete(die);
	// }
}

////
//// BG_ItemSlotType
////
void BG_ItemSlotType::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ItemSlotType::get_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_ItemSlotType::get_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_ItemSlotType::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_percentage_of_all_items_dropped_per_act"), &BG_ItemSlotType::get_percentage_of_all_items_dropped_per_act);
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
	ClassDB::bind_method(D_METHOD("get_weeks_left_to_finish_graft"), &BG_Item::get_weeks_left_to_finish_graft);
	ClassDB::bind_method(D_METHOD("set_weeks_left_to_finish_graft"), &BG_Item::set_weeks_left_to_finish_graft);
	ClassDB::bind_method(D_METHOD("get_number_of_jobs_participated_in"), &BG_Item::get_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("set_number_of_jobs_participated_in"), &BG_Item::set_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("get_consecutive_number_of_jobs_participated_in"), &BG_Item::get_consecutive_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("set_consecutive_number_of_jobs_participated_in"), &BG_Item::set_consecutive_number_of_jobs_participated_in);
	ClassDB::bind_method(D_METHOD("get_should_count_in_item_rarity_disparity"), &BG_Item::get_should_count_in_item_rarity_disparity);
	ClassDB::bind_method(D_METHOD("set_should_count_in_item_rarity_disparity"), &BG_Item::set_should_count_in_item_rarity_disparity);
	ClassDB::bind_method(D_METHOD("get_rarity_id"), &BG_Item::get_rarity_id);
	ClassDB::bind_method(D_METHOD("set_rarity_id"), &BG_Item::set_rarity_id);
	ClassDB::bind_method(D_METHOD("get_grafts"), &BG_Item::get_grafts);
	ClassDB::bind_method(D_METHOD("set_grafts"), &BG_Item::set_grafts);
	ClassDB::bind_method(D_METHOD("get_forced_damage_element"), &BG_Item::get_forced_damage_element);
	ClassDB::bind_method(D_METHOD("set_forced_damage_element"), &BG_Item::set_forced_damage_element);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "on_shelf"), "set_on_shelf", "get_on_shelf");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "shelf_index"), "set_shelf_index", "get_shelf_index");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_bid"), "set_has_bid", "get_has_bid");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bid_amount"), "set_bid_amount", "get_bid_amount");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_equipped"), "set_is_equipped", "get_is_equipped");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_durability"), "set_current_durability", "get_current_durability");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_fame"), "set_current_fame", "get_current_fame");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "weeks_left_to_finish_graft"), "set_weeks_left_to_finish_graft", "get_weeks_left_to_finish_graft");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "number_of_jobs_participated_in"), "set_number_of_jobs_participated_in", "get_number_of_jobs_participated_in");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "consecutive_number_of_jobs_participated_in"), "set_consecutive_number_of_jobs_participated_in", "get_consecutive_number_of_jobs_participated_in");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "should_count_in_item_rarity_disparity"), "set_should_count_in_item_rarity_disparity", "get_should_count_in_item_rarity_disparity");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "rarity_id"), "set_rarity_id", "get_rarity_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "grafts"), "set_grafts", "get_grafts");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "forced_damage_element"), "set_forced_damage_element", "get_forced_damage_element");
}

////
//// BG_ItemDetails
////
void BG_ItemDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ItemDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_ItemDetails::get_name);
	ClassDB::bind_method(D_METHOD("get_is_beast_part"), &BG_ItemDetails::get_is_beast_part);
	ClassDB::bind_method(D_METHOD("get_is_useable_item"), &BG_ItemDetails::get_is_useable_item);
	ClassDB::bind_method(D_METHOD("get_slot_type_id"), &BG_ItemDetails::get_slot_type_id);
	ClassDB::bind_method(D_METHOD("get_beast_part_available_item_slot_types"), &BG_ItemDetails::get_beast_part_available_item_slot_types);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_ItemDetails::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_hue_shift_data"), &BG_ItemDetails::get_hue_shift_data);
	ClassDB::bind_method(D_METHOD("get_mesh_path"), &BG_ItemDetails::get_mesh_path);
	ClassDB::bind_method(D_METHOD("get_lore"), &BG_ItemDetails::get_lore);
	ClassDB::bind_method(D_METHOD("get_caste_ids"), &BG_ItemDetails::get_caste_ids);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_ItemDetails::get_stats);
	ClassDB::bind_method(D_METHOD("get_animation_attach_socket"), &BG_ItemDetails::get_animation_attach_socket);
	ClassDB::bind_method(D_METHOD("get_effect_ids"), &BG_ItemDetails::get_effect_ids);
	ClassDB::bind_method(D_METHOD("get_sell_value_tier"), &BG_ItemDetails::get_sell_value_tier);
	ClassDB::bind_method(D_METHOD("get_fame_value_tier"), &BG_ItemDetails::get_fame_value_tier);
	ClassDB::bind_method(D_METHOD("get_durability_value_tier"), &BG_ItemDetails::get_durability_value_tier);
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
	ClassDB::bind_method(D_METHOD("get_slot_index"), &BG_BandMember::get_slot_index);
	ClassDB::bind_method(D_METHOD("set_slot_index"), &BG_BandMember::set_slot_index);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_BandMember::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_BandMember::set_random_variation);
	ClassDB::bind_method(D_METHOD("get_scale"), &BG_BandMember::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale"), &BG_BandMember::set_scale);
	ClassDB::bind_method(D_METHOD("get_caste_id"), &BG_BandMember::get_caste_id);
	ClassDB::bind_method(D_METHOD("set_caste_id"), &BG_BandMember::set_caste_id);
	ClassDB::bind_method(D_METHOD("get_equipment"), &BG_BandMember::get_equipment);
	ClassDB::bind_method(D_METHOD("set_equipment"), &BG_BandMember::set_equipment);
	ClassDB::bind_method(D_METHOD("get_infused_element_id"), &BG_BandMember::get_infused_element_id);
	ClassDB::bind_method(D_METHOD("set_infused_element_id"), &BG_BandMember::set_infused_element_id);
	ClassDB::bind_method(D_METHOD("get_is_city_asset"), &BG_BandMember::get_is_city_asset);
	ClassDB::bind_method(D_METHOD("set_is_city_asset"), &BG_BandMember::set_is_city_asset);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_health"), "set_current_health", "get_current_health");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "slot_index"), "set_slot_index", "get_slot_index");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "caste_id"), "set_caste_id", "get_caste_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipment"), "set_equipment", "get_equipment");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "infused_element_id"), "set_infused_element_id", "get_infused_element_id");
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
	ClassDB::bind_method(D_METHOD("get_name"), &BG_UnitCaste::get_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitCaste::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_hue_shift_data"), &BG_UnitCaste::get_hue_shift_data);
	ClassDB::bind_method(D_METHOD("get_lod_mesh_paths"), &BG_UnitCaste::get_lod_mesh_paths);
	ClassDB::bind_method(D_METHOD("get_scale_min"), &BG_UnitCaste::get_scale_min);
	ClassDB::bind_method(D_METHOD("get_scale_max"), &BG_UnitCaste::get_scale_max);
	ClassDB::bind_method(D_METHOD("get_scale_min_extreme"), &BG_UnitCaste::get_scale_min_extreme);
	ClassDB::bind_method(D_METHOD("get_scale_max_extreme"), &BG_UnitCaste::get_scale_max_extreme);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_UnitCaste::get_stats);
	ClassDB::bind_method(D_METHOD("get_starting_item_ids"), &BG_UnitCaste::get_starting_item_ids);
	ClassDB::bind_method(D_METHOD("get_element_availability_ids"), &BG_UnitCaste::get_element_availability_ids);
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
	ClassDB::bind_method(D_METHOD("get_monster_type"), &BG_Monster::get_monster_type);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_Monster::get_stats);
	ClassDB::bind_method(D_METHOD("get_element_availability_ids"), &BG_Monster::get_element_availability_ids);

	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_Monster::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_effect_ids"), &BG_Monster::get_effect_ids);
	ClassDB::bind_method(D_METHOD("get_hue_shift_data"), &BG_Monster::get_hue_shift_data);
	ClassDB::bind_method(D_METHOD("get_challenge_rating_fraction_string"), &BG_Monster::get_challenge_rating_fraction_string);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_health"), "set_current_health", "get_current_health");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
}

String BG_Monster::get_challenge_rating_fraction_string() const
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
	ClassDB::bind_method(D_METHOD("get_rarity_availabilities"), &BG_RewardItem::get_rarity_availabilities);
	ClassDB::bind_method(D_METHOD("get_drop_weight"), &BG_RewardItem::get_drop_weight);
}

////
//// BG_ItemDropPool
////
void BG_ItemDropPool::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ItemDropPool::get_id);
	ClassDB::bind_method(D_METHOD("get_item_drops"), &BG_ItemDropPool::get_item_drops);
}

////
//// BG_JobDistributionForAct
////
void BG_JobDistributionForAct::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_act"), &BG_JobDistributionForAct::get_act);
	ClassDB::bind_method(D_METHOD("get_min_week"), &BG_JobDistributionForAct::get_min_week);
	ClassDB::bind_method(D_METHOD("get_max_week"), &BG_JobDistributionForAct::get_max_week);
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

////
//// BG_JobDetails
////
void BG_JobDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_JobDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_weeks_before_expire"), &BG_JobDetails::get_weeks_before_expire);
	ClassDB::bind_method(D_METHOD("get_monster_details"), &BG_JobDetails::get_monster_details);
	ClassDB::bind_method(D_METHOD("get_is_unique"), &BG_JobDetails::get_is_unique);
	ClassDB::bind_method(D_METHOD("get_is_boss"), &BG_JobDetails::get_is_boss);
	ClassDB::bind_method(D_METHOD("get_distribution_per_act"), &BG_JobDetails::get_distribution_per_act);
}

////
//// BG_ChallengeRatingGuide
////
void BG_ChallengeRatingGuide::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_cr_min_max"), &BG_ChallengeRatingGuide::get_cr_min_max);
	ClassDB::bind_method(D_METHOD("get_job_rep_reward"), &BG_ChallengeRatingGuide::get_job_rep_reward);
	ClassDB::bind_method(D_METHOD("get_job_duralation"), &BG_ChallengeRatingGuide::get_job_duralation);
	ClassDB::bind_method(D_METHOD("get_item_durability_consumption"), &BG_ChallengeRatingGuide::get_item_durability_consumption);
	ClassDB::bind_method(D_METHOD("get_item_fame_addition"), &BG_ChallengeRatingGuide::get_item_fame_addition);
}

////
//// BG_ActStats
////
void BG_ActStats::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_total_week_count"), &BG_ActStats::get_total_week_count);
	ClassDB::bind_method(D_METHOD("get_total_job_count"), &BG_ActStats::get_total_job_count);
	ClassDB::bind_method(D_METHOD("get_job_handout_curve_path"), &BG_ActStats::get_job_handout_curve_path);
}

////
//// BG_EquipmentAnimationDetails
////
void BG_EquipmentAnimationDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_caste_id"), &BG_EquipmentAnimationDetails::get_caste_id);
	ClassDB::bind_method(D_METHOD("get_equipment_ids"), &BG_EquipmentAnimationDetails::get_equipment_ids);
	ClassDB::bind_method(D_METHOD("get_in_game_animation_name"), &BG_EquipmentAnimationDetails::get_in_game_animation_name);
}

////
//// BG_BookerSkillTreeSlotDetails
////
void BG_BookerSkillTreeSlotDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_BookerSkillTreeSlotDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_is_parent_button"), &BG_BookerSkillTreeSlotDetails::get_is_parent_button);
	ClassDB::bind_method(D_METHOD("get_skill_name"), &BG_BookerSkillTreeSlotDetails::get_skill_name);
	ClassDB::bind_method(D_METHOD("get_skill_description"), &BG_BookerSkillTreeSlotDetails::get_skill_description);
	ClassDB::bind_method(D_METHOD("get_required_rep"), &BG_BookerSkillTreeSlotDetails::get_required_rep);
	ClassDB::bind_method(D_METHOD("get_parent_skill_id"), &BG_BookerSkillTreeSlotDetails::get_parent_skill_id);
}

////
//// BG_CityInfo
////
void BG_CityInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_nice_name"), &BG_CityInfo::get_nice_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_CityInfo::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_CityInfo::get_max_health);
	ClassDB::bind_method(D_METHOD("get_equipment_ids"), &BG_CityInfo::get_equipment_ids);
}

////
//// BG_Booker_Globals
////
void BG_Booker_Globals::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_starting_reputation"), &BG_Booker_Globals::get_starting_reputation);
	ClassDB::bind_method(D_METHOD("get_chance_of_no_drop"), &BG_Booker_Globals::get_chance_of_no_drop);
	ClassDB::bind_method(D_METHOD("get_city_info"), &BG_Booker_Globals::get_city_info);
	ClassDB::bind_method(D_METHOD("get_act_stats"), &BG_Booker_Globals::get_act_stats);
	ClassDB::bind_method(D_METHOD("get_challenge_rating_guide"), &BG_Booker_Globals::get_challenge_rating_guide);
	ClassDB::bind_method(D_METHOD("get_monster_element_distribution"), &BG_Booker_Globals::get_monster_element_distribution);

	ClassDB::bind_method(D_METHOD("get_sell_tier_values"), &BG_Booker_Globals::get_sell_tier_values);
	ClassDB::bind_method(D_METHOD("get_fame_tier_values"), &BG_Booker_Globals::get_fame_tier_values);
	ClassDB::bind_method(D_METHOD("get_durability_teir_values"), &BG_Booker_Globals::get_durability_teir_values);

	ClassDB::bind_method(D_METHOD("get_equipment_value_rarity_multiplier"), &BG_Booker_Globals::get_equipment_value_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_beast_part_value_rarity_multiplier"), &BG_Booker_Globals::get_beast_part_value_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_extra_beast_part_value_rarity_multiplier_while_grafted"), &BG_Booker_Globals::get_extra_beast_part_value_rarity_multiplier_while_grafted);
	ClassDB::bind_method(D_METHOD("get_equipment_durability_rarity_multiplier"), &BG_Booker_Globals::get_equipment_durability_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_beast_part_durability_rarity_multiplier"), &BG_Booker_Globals::get_beast_part_durability_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_equipment_fame_rarity_multiplier"), &BG_Booker_Globals::get_equipment_fame_rarity_multiplier);
	ClassDB::bind_method(D_METHOD("get_beast_part_fame_rarity_multiplier"), &BG_Booker_Globals::get_beast_part_fame_rarity_multiplier);

	ClassDB::bind_method(D_METHOD("get_inventory_sell_multiplier"), &BG_Booker_Globals::get_inventory_sell_multiplier);
	ClassDB::bind_method(D_METHOD("get_item_passive_income_multiplier"), &BG_Booker_Globals::get_item_passive_income_multiplier);

	ClassDB::bind_method(D_METHOD("get_percent_amount_to_add_on_same_element_per_damage_value"), &BG_Booker_Globals::get_percent_amount_to_add_on_same_element_per_damage_value);
	ClassDB::bind_method(D_METHOD("get_percent_amount_to_subtract_on_weak_element_per_damage_value"), &BG_Booker_Globals::get_percent_amount_to_subtract_on_weak_element_per_damage_value);
}

////
//// BG_Booker_DB
////
BG_Booker_DB *BG_Booker_DB::singleton = nullptr;

void BG_Booker_DB::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("refresh_data"), &BG_Booker_DB::refresh_data);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("timer_test"), &BG_Booker_DB::timer_test);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("get_job_challenge_rating_value", "monsters"), &BG_Booker_DB::get_job_challenge_rating_value);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("get_job_challenge_rating", "monsters"), &BG_Booker_DB::get_job_challenge_rating);

	ClassDB::bind_method(D_METHOD("get_modding_path"), &BG_Booker_DB::get_modding_path);
	ClassDB::bind_method(D_METHOD("get_globals"), &BG_Booker_DB::get_globals);
	ClassDB::bind_method(D_METHOD("get_audio_data"), &BG_Booker_DB::get_audio_data);
	ClassDB::bind_method(D_METHOD("get_booker_skill_tree_details"), &BG_Booker_DB::get_booker_skill_tree_details);
	ClassDB::bind_method(D_METHOD("get_jobs"), &BG_Booker_DB::get_jobs);
	ClassDB::bind_method(D_METHOD("get_items"), &BG_Booker_DB::get_items);
	ClassDB::bind_method(D_METHOD("get_item_drop_pools"), &BG_Booker_DB::get_item_drop_pools);
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
			if (lines.has("starting_reputation"))
				globals->starting_reputation = int(lines["starting_reputation"]);

			if (lines.has("job_globals") && lines.has("weeks_per_act"))
			{
				globals->act_stats.clear();

				const Array weeks_per_act_array = Array(lines["weeks_per_act"]);
				const Array job_globals_array = Array(lines["job_globals"]);
				for (int i = 0; i < weeks_per_act_array.size(); i++)
				{
					BG_ActStats *new_act_stats = memnew(BG_ActStats);

					const Dictionary weeks_per_act = weeks_per_act_array[i];
					new_act_stats->total_week_count = int(weeks_per_act["weeks_count"]);

					const Dictionary job_globals_entry = job_globals_array[i];
					const Array jobs_distribution_per_act_array = Array(job_globals_entry["jobs_distribution_per_act"]);
					const Dictionary jobs_distribution_per_act_entry = jobs_distribution_per_act_array[0];
					new_act_stats->total_job_count = int(jobs_distribution_per_act_entry["job_count"]);
					new_act_stats->job_handout_curve_path = jobs_distribution_per_act_entry["curve"];

					globals->act_stats.append(new_act_stats);
				}
			}

			if (lines.has("monster_element_distribution"))
			{
				globals->monster_element_distribution.clear();

				const Array monster_element_distribution_array = Array(lines["monster_element_distribution"]);
				for (int i = 0; i < monster_element_distribution_array.size(); i++)
				{
					const Dictionary values = monster_element_distribution_array[i];
					globals->monster_element_distribution.append(float(values["percentage"]));
				}
			}

			if (lines.has("damage_resistance_modifiers"))
			{
				const Array damage_resistance_modifiers_array = Array(lines["damage_resistance_modifiers"]);
				for (int i = 0; i < damage_resistance_modifiers_array.size(); i++)
				{
					const Dictionary values = damage_resistance_modifiers_array[i];
					globals->percent_amount_to_add_on_same_element_per_damage_value = float(values["percent_amount_to_add_on_same_element_per_damage_value"]);
					globals->percent_amount_to_subtract_on_weak_element_per_damage_value = float(values["percent_amount_to_subtract_on_weak_element_per_damage_value"]);
				}
			}

			if (lines.has("item_teir_values"))
			{
				globals->sell_tier_values.clear();
				globals->fame_tier_values.clear();
				globals->durability_teir_values.clear();

				const Array item_value_per_act_array = Array(lines["item_teir_values"]);
				for (int i = 0; i < item_value_per_act_array.size(); i++)
				{
					const Dictionary values = item_value_per_act_array[i];
					globals->sell_tier_values.append(int(values["sell_tier_values"]));
					globals->fame_tier_values.append(int(values["fame_tier_values"]));
					globals->durability_teir_values.append(int(values["durability_teir_values"]));
				}
			}

			if (lines.has("item_teir_rarity_multipliers"))
			{
				globals->equipment_value_rarity_multiplier.clear();
				globals->beast_part_value_rarity_multiplier.clear();
				globals->extra_beast_part_value_rarity_multiplier_while_grafted.clear();
				globals->equipment_durability_rarity_multiplier.clear();
				globals->beast_part_durability_rarity_multiplier.clear();
				globals->equipment_fame_rarity_multiplier.clear();
				globals->beast_part_fame_rarity_multiplier.clear();

				const Array item_rarity_multiplier_array = Array(lines["item_teir_rarity_multipliers"]);
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
			
			if (lines.has("inventory_sell_multiplier"))
				globals->inventory_sell_multiplier = float(lines["inventory_sell_multiplier"]);

			if (lines.has("item_passive_income_multiplier"))
				globals->item_passive_income_multiplier = float(lines["item_passive_income_multiplier"]);
			
			if (lines.has("chance_of_no_drop"))
				globals->chance_of_no_drop = float(lines["chance_of_no_drop"]);
		}
	}

	/////
	///// City Info
	/////
	{
		const Dictionary city_info_sheet = BG_JsonUtils::GetCBDSheet(data, "city_info");
		if (city_info_sheet.has("lines"))
		{
			globals->city_info.clear();

			const Array lines = Array(city_info_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];

				BG_CityInfo *new_city_info = memnew(BG_CityInfo);
				new_city_info->nice_name = entry["name"];
				new_city_info->icon_path = entry["icon_path"];
				new_city_info->max_health = int(entry["health"]);

				// Equipment
				const Array equipment_lines = Array(entry["equipment"]);
				for (int y = 0; y < equipment_lines.size(); y++)
				{
					const Dictionary equipment_entry = equipment_lines[y];
					new_city_info->equipment_ids.append(equipment_entry["equipment"]);
				}
				
				globals->city_info.append(new_city_info);
			}
		}
	}

	/////
	///// Challenge Rating Guide
	/////
	{
		const Dictionary challenge_rating_guide_sheet = BG_JsonUtils::GetCBDSheet(data, "challenge_rating_guide");
		if (challenge_rating_guide_sheet.has("lines"))
		{
			globals->challenge_rating_guide.clear();

			const Array lines = Array(challenge_rating_guide_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				BG_ChallengeRatingGuide *challenge_rating_guide_class = memnew(BG_ChallengeRatingGuide);
				challenge_rating_guide_class->cr_min_max = Vector2(float(entry["cr_min"]), float(entry["cr_max"]));
				challenge_rating_guide_class->job_rep_reward = int(entry["job_rep_reward"]);
				challenge_rating_guide_class->job_duralation = int(entry["job_duralation"]);
				challenge_rating_guide_class->item_durability_consumption = float(entry["item_durability_consumption"]);
				challenge_rating_guide_class->item_fame_addition = float(entry["item_fame_addition"]);
				globals->challenge_rating_guide.append(challenge_rating_guide_class);
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
	///// Booker Skill Tree
	/////
	{
		const Dictionary booker_skill_tree_sheet = BG_JsonUtils::GetCBDSheet(data, "booker_skill_tree");
		if (booker_skill_tree_sheet.has("lines"))
		{
			booker_skill_tree_details.clear();
			const Array lines = Array(booker_skill_tree_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				if (bool(entry["disabled"]))
					continue;
				BG_BookerSkillTreeSlotDetails *new_booker_skill_tree_slot_details_class = memnew(BG_BookerSkillTreeSlotDetails);
				new_booker_skill_tree_slot_details_class->id = entry["id"];
				new_booker_skill_tree_slot_details_class->is_parent_button = bool(entry["is_parent_btn"]);
				new_booker_skill_tree_slot_details_class->skill_name = entry["skill_name"];
				new_booker_skill_tree_slot_details_class->skill_description = entry["skill_description"];
				new_booker_skill_tree_slot_details_class->required_rep = int(entry["required_rep"]);
				new_booker_skill_tree_slot_details_class->parent_skill_id = entry["parent_skill"];

				booker_skill_tree_details.append(new_booker_skill_tree_slot_details_class);
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
				if (bool(entry["disabled"]))
					continue;

				BG_UnitStatDetails *new_stat_types = memnew(BG_UnitStatDetails);
				new_stat_types->id = entry["id"];
				new_stat_types->nice_name = entry["name"];
				new_stat_types->icon_path = entry["icon_path"];
				new_stat_types->is_damage_type = bool(entry["is_damage_type"]);
				new_stat_types->weak_to_element = entry["weak_to_element"];
				new_stat_types->widget_color = convert_int_to_color(int(entry["widget_color"]));
				new_stat_types->text_color = convert_int_to_color(int(entry["text_color"]));
				new_stat_types->in_world_color = convert_int_to_color(int(entry["in_world_color"]));

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
				if (bool(entry["disabled"]))
					continue;

				BG_UnitCaste *new_unit_caste = memnew(BG_UnitCaste);
				new_unit_caste->id = entry["id"];
				new_unit_caste->name = entry["name"];
				new_unit_caste->icon_path = entry["icon_path"];

				// Hue Shifting
				const Array hue_shifting_lines = Array(entry["hue_shifting"]);
				for (int y = 0; y < hue_shifting_lines.size(); y++)
				{
					const Dictionary hue_shifting_entry = hue_shifting_lines[y];

					BG_HueShiftData *new_hue_shift_data = memnew(BG_HueShiftData);
					new_hue_shift_data->mask_path = hue_shifting_entry["mask_path"];
					new_hue_shift_data->from_color = convert_int_to_color(int(hue_shifting_entry["from_color"]));
					new_hue_shift_data->multiplier = float(hue_shifting_entry["multiplier"]);

					new_unit_caste->hue_shift_data = new_hue_shift_data;
				}

				new_unit_caste->lod_mesh_paths.clear();
				const Array lod_mesh_path_lines = Array(entry["lod_mesh_paths"]);
				for (int y = 0; y < lod_mesh_path_lines.size(); y++)
				{
					const Dictionary lod_mesh_path_entry = lod_mesh_path_lines[y];
					new_unit_caste->lod_mesh_paths.append(lod_mesh_path_entry["path"]);
				}

				const Array scale_limits_lines = Array(entry["scale_limits"]);
				for (int y = 0; y < scale_limits_lines.size(); y++)
				{
					const Dictionary scale_limits_entry = scale_limits_lines[y];
					new_unit_caste->scale_min = Vector2(float(scale_limits_entry["min_x"]), float(scale_limits_entry["min_y"]));
					new_unit_caste->scale_max = Vector2(float(scale_limits_entry["max_x"]), float(scale_limits_entry["max_y"]));
					new_unit_caste->scale_min_extreme = Vector2(float(scale_limits_entry["extreme_min_x"]), float(scale_limits_entry["extreme_min_y"]));
					new_unit_caste->scale_max_extreme = Vector2(float(scale_limits_entry["extreme_max_x"]), float(scale_limits_entry["extreme_max_y"]));
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

				const Array starting_items_lines = Array(entry["random_starting_items"]);
				for (int y = 0; y < starting_items_lines.size(); y++)
				{
					const Dictionary starting_item_entry = starting_items_lines[y];
					new_unit_caste->starting_item_ids.append(starting_item_entry["item"]);
				}

				const Array element_availability_lines = Array(entry["element_availability"]);
				for (int y = 0; y < element_availability_lines.size(); y++)
				{
					const Dictionary element_availability_entry = element_availability_lines[y];
					new_unit_caste->element_availability_ids.append(element_availability_entry["element"]);
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
				new_monster_type->max_health = int(entry["health"]);
				new_monster_type->icon_path = entry["icon_path"];
				new_monster_type->monster_type = int(entry["type"]);

				// Hue Shifting
				const Array hue_shifting_lines = Array(entry["hue_shifting"]);
				for (int y = 0; y < hue_shifting_lines.size(); y++)
				{
					const Dictionary hue_shifting_entry = hue_shifting_lines[y];

					BG_HueShiftData *new_hue_shift_data = memnew(BG_HueShiftData);
					new_hue_shift_data->mask_path = hue_shifting_entry["mask_path"];
					new_hue_shift_data->from_color = convert_int_to_color(int(hue_shifting_entry["from_color"]));
					new_hue_shift_data->multiplier = float(hue_shifting_entry["multiplier"]);

					new_monster_type->hue_shift_data = new_hue_shift_data;
				}

				// Stats
				// int defensive_stat_count = 0;
				const Array stats_lines = Array(entry["stats"]);
				for (int y = 0; y < stats_lines.size(); y++)
				{
					const Dictionary stat_entry = stats_lines[y];

					BG_UnitStat *new_stat = memnew(BG_UnitStat);
					new_stat->id = stat_entry["stat"];
					new_stat->resistant_value_text = stat_entry["resistant_value"];
					new_stat->resistant_value_min_max = BG_UnitStat::string_to_resistant_value_min_max(stat_entry["resistant_value"]);
					new_stat->dice_string = stat_entry["damage_dice"];
					new_stat->dice_options = BG_Dice::string_to_dice_options(new_stat->dice_string);
					
					new_monster_type->stats.append(new_stat);
				}

				// Element Availability
				const Array element_availability_lines = Array(entry["element_availability"]);
				for (int y = 0; y < element_availability_lines.size(); y++)
				{
					const Dictionary element_availability_entry = element_availability_lines[y];
					new_monster_type->element_availability_ids.append(element_availability_entry["element"]);
				}
				
				// Effects
				const Array effect_lines = Array(entry["effects"]);
				for (int y = 0; y < effect_lines.size(); y++)
				{
					const Dictionary effect_entry = effect_lines[y];
					new_monster_type->effect_ids.append(effect_entry["effect"]);
				}

				monster_types.append(new_monster_type);
			}
		}
	}

	/////
	///// Item Drop Pools
	/////
	{
		const Dictionary item_drop_pools_sheet = BG_JsonUtils::GetCBDSheet(data, "item_drop_pools");
		if (item_drop_pools_sheet.has("lines"))
		{
			item_drop_pools.clear();
			const Array lines = Array(item_drop_pools_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				BG_ItemDropPool *new_item_drop_pool_class = memnew(BG_ItemDropPool);

				const Dictionary entry = lines[i];
				new_item_drop_pool_class->id = entry["id"];

				const Array items_lines = Array(entry["items"]);
				for (int y = 0; y < items_lines.size(); y++)
				{
					const Dictionary drops_entry = items_lines[y];
					if (bool(drops_entry["disabled"]))
						continue;

					TypedArray<String> item_types;
					item_types.append("equipment_id");
					item_types.append("beast_part_id");
					for (int it = 0; it < item_types.size(); it++)
					{
						const String item_type = item_types[it];
						const StringName item_type_id = drops_entry[item_type];
						if (item_type_id.is_empty()) continue;
						
						BG_RewardItem *new_reward_item = memnew(BG_RewardItem);
						new_reward_item->id = item_type_id;
						new_reward_item->drop_weight = float(drops_entry["drop_weight"]);

						const Array forced_rarity_availabilities_lines = Array(drops_entry["rarity_availability"]);
						for (int f = 0; f < forced_rarity_availabilities_lines.size(); f++)
						{
							const Dictionary forced_rarity_availabilities_entry = forced_rarity_availabilities_lines[f];
							new_reward_item->rarity_availabilities.append(forced_rarity_availabilities_entry["rarity_id"]);
						}

						new_item_drop_pool_class->item_drops.append(new_reward_item);
					}
				}

				item_drop_pools.append(new_item_drop_pool_class);
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
				new_job_class->weeks_before_expire = int(entry["weeks_before_expire"]);
				new_job_class->is_unique = bool(entry["is_unique"]);
				new_job_class->is_boss = bool(entry["is_boss"]);

				const Array monsters_lines = Array(entry["monsters"]);
				for (int y = 0; y < monsters_lines.size(); y++)
				{
					const Dictionary monster_entry = monsters_lines[y];
					BG_JobMonsterDetails *new_job_monster_details_class = memnew(BG_JobMonsterDetails);
					new_job_monster_details_class->monster_id = monster_entry["monster"];
					new_job_monster_details_class->always_drop_at_least_one = bool(monster_entry["always_drop_at_least_one"]);

					const Array monster_count_range_lines = Array(monster_entry["monster_count_range"]);
					for (int z = 0; z < monster_count_range_lines.size(); z++)
					{
						const Dictionary monster_count_range_entry = monster_count_range_lines[z];
						new_job_monster_details_class->monster_count_range = Vector2i(int(monster_count_range_entry["min"]), int(monster_count_range_entry["max"]));
						break;
					}

					const Array drops_lines = Array(monster_entry["drops"]);
					for (int z = 0; z < drops_lines.size(); z++)
					{
						const Dictionary drops_entry = drops_lines[z];

						TypedArray<String> item_types;
						item_types.append("beast_part");
						item_types.append("equipment");
						for (int it = 0; it < item_types.size(); it++)
						{
							const String item_type = item_types[it];
							const StringName item_type_id = drops_entry[item_type];
							if (item_type_id.is_empty()) continue;
							
							BG_RewardItem *new_job_reward_item_class = memnew(BG_RewardItem);
							new_job_reward_item_class->id = drops_entry[item_type];
							new_job_reward_item_class->drop_weight = float(drops_entry["drop_weight"]);

							const Array forced_rarity_availabilities_lines = Array(drops_entry["forced_rarity_availabilities"]);
							for (int f = 0; f < forced_rarity_availabilities_lines.size(); f++)
							{
								const Dictionary forced_rarity_availabilities_entry = forced_rarity_availabilities_lines[f];
								new_job_reward_item_class->rarity_availabilities.append(forced_rarity_availabilities_entry["rarity_id"]);
							}

							new_job_monster_details_class->drops.append(new_job_reward_item_class);
						}

						const StringName item_drop_pool_id = drops_entry["item_drop_pool"];
						if (!item_drop_pool_id.is_empty())
						{
							for (int pool_index = 0; pool_index < item_drop_pools.size(); pool_index++)
							{
								const BG_ItemDropPool *pool = cast_to<BG_ItemDropPool>(item_drop_pools[pool_index]);
								if (pool == nullptr || String(pool->id) != String(drops_entry["item_drop_pool"]))
									continue;
								
								for (int item_index = 0; item_index < pool->item_drops.size(); item_index++)
								{
									const BG_RewardItem *reward_item = cast_to<BG_RewardItem>(pool->item_drops[item_index]);
									if (reward_item == nullptr) continue;

									BG_RewardItem *new_job_reward_item_class = memnew(BG_RewardItem);
									new_job_reward_item_class->id = reward_item->id;
									new_job_reward_item_class->drop_weight = float(drops_entry["drop_weight"]) * reward_item->drop_weight;

									const Array forced_rarity_availabilities_lines = Array(drops_entry["forced_rarity_availabilities"]);
									if (forced_rarity_availabilities_lines.size() > 0)
									{
										for (int f = 0; f < forced_rarity_availabilities_lines.size(); f++)
										{
											const Dictionary forced_rarity_availabilities_entry = forced_rarity_availabilities_lines[f];
											new_job_reward_item_class->rarity_availabilities.append(forced_rarity_availabilities_entry["rarity_id"]);
										}
									}
									else
										new_job_reward_item_class->rarity_availabilities = reward_item->rarity_availabilities.duplicate();

									new_job_monster_details_class->drops.append(new_job_reward_item_class);
								}
								break;
							}
						}

					}

					new_job_class->monster_details.append(new_job_monster_details_class);
				}

				const Array distribution_per_act_lines = Array(entry["distribution_per_act"]);
				for (int y = 0; y < distribution_per_act_lines.size(); y++)
				{
					const Dictionary distribution_per_act_entry = distribution_per_act_lines[y];

					BG_JobDistributionForAct *new_job_distribution_for_act_class = memnew(BG_JobDistributionForAct);
					new_job_distribution_for_act_class->act = int(distribution_per_act_entry["act"]);
					new_job_distribution_for_act_class->min_week = int(distribution_per_act_entry["min_week"]);
					new_job_distribution_for_act_class->max_week = int(distribution_per_act_entry["max_week"]);

					new_job_class->distribution_per_act.append(new_job_distribution_for_act_class);
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
				BG_ItemSlotType *new_item_slot_type_class = memnew(BG_ItemSlotType);

				const Dictionary entry = lines[i];
				new_item_slot_type_class->id = entry["id"];
				new_item_slot_type_class->name = entry["name"];
				new_item_slot_type_class->icon_path = entry["icon_path"];

				const Array percentage_of_all_items_dropped_per_act_lines = Array(entry["percentage_of_all_items_dropped_per_act"]);
				for (int y = 0; y < percentage_of_all_items_dropped_per_act_lines.size(); y++)
				{
					const Dictionary percentage_of_all_items_dropped_entry = percentage_of_all_items_dropped_per_act_lines[y];
					new_item_slot_type_class->percentage_of_all_items_dropped_per_act.append(float(percentage_of_all_items_dropped_entry["percent"]));
				}

				item_slot_types.append(new_item_slot_type_class);
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
					new_item_class->slot_type_id = entry["slot_type"];
					new_item_class->animation_attach_socket = entry["anim_attach_socket"];
					new_item_class->sell_value_tier = int(entry["sell_value_tier"]);
					new_item_class->fame_value_tier = int(entry["fame_value_tier"]);

					// Hue Shifting
					const Array hue_shifting_lines = Array(entry["hue_shifting"]);
					for (int y = 0; y < hue_shifting_lines.size(); y++)
					{
						const Dictionary hue_shifting_entry = hue_shifting_lines[y];

						BG_HueShiftData *new_hue_shift_data = memnew(BG_HueShiftData);
						new_hue_shift_data->mask_path = hue_shifting_entry["mask_path"];
						new_hue_shift_data->from_color = convert_int_to_color(int(hue_shifting_entry["from_color"]));
						new_hue_shift_data->multiplier = float(hue_shifting_entry["multiplier"]);

						new_item_class->hue_shift_data = new_hue_shift_data;
					}

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
						new_stat->resistant_value_text = stat_entry["resistant_value"];
						new_stat->resistant_value_min_max = BG_UnitStat::string_to_resistant_value_min_max(stat_entry["resistant_value"]);
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
					new_item_class->slot_type_id = entry["slot_type"];
					new_item_class->is_beast_part = true;
					new_item_class->sell_value_tier = int(entry["sell_value_tier"]);
					new_item_class->fame_value_tier = int(entry["fame_value_tier"]);

					// Hue Shifting
					const Array hue_shifting_lines = Array(entry["hue_shifting"]);
					for (int y = 0; y < hue_shifting_lines.size(); y++)
					{
						const Dictionary hue_shifting_entry = hue_shifting_lines[y];

						BG_HueShiftData *new_hue_shift_data = memnew(BG_HueShiftData);
						new_hue_shift_data->mask_path = hue_shifting_entry["mask_path"];
						new_hue_shift_data->from_color = convert_int_to_color(int(hue_shifting_entry["from_color"]));
						new_hue_shift_data->multiplier = float(hue_shifting_entry["multiplier"]);

						new_item_class->hue_shift_data = new_hue_shift_data;
					}

					// Available Item Slot Types
					const Array available_item_slot_types_lines = Array(entry["available_item_slot_types"]);
					for (int y = 0; y < available_item_slot_types_lines.size(); y++)
					{
						const Dictionary slot_type_entry = available_item_slot_types_lines[y];
						new_item_class->beast_part_available_item_slot_types.append(slot_type_entry["slot_type"]);
					}

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
				new_effect_class->use_owning_item_icon = bool(entry["use_owning_item_icon"]);
				new_effect_class->status_icon_path = entry["status_icon"];

				// Rarities
				const Array rarity_lines = Array(entry["details_per_rarity"]);
				for (int r = 0; r < rarity_lines.size(); r++)
				{
					const Dictionary rarity_entry = rarity_lines[r];
					BG_EffectRarityDetails *new_rarity_class = memnew(BG_EffectRarityDetails);

					new_rarity_class->rarity_id = rarity_entry["rarity"];
					new_rarity_class->description = rarity_entry["description"];
					new_rarity_class->script_path = rarity_entry["script_path"];

					// Value Attributes
					const Array value_attribute_lines = Array(rarity_entry["value_attributes"]);
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

						new_rarity_class->value_attributes[name] = values;
					}

					new_effect_class->details_per_rarity.append(new_rarity_class);
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
				new_anim_details_class->in_game_animation_name = entry["in_game_animation_name"];

				const Array equipment_id_lines = Array(entry["equipment_ids"]);
				for (int y = 0; y < equipment_id_lines.size(); y++)
				{
					const Dictionary equipment_id_entry = equipment_id_lines[y];
					new_anim_details_class->equipment_ids.append(equipment_id_entry["equipment_id"]);
				}

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

/* static */ float BG_Booker_DB::get_job_challenge_rating_value(const TypedArray<BG_Monster> monsters)
{
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

	return challenge_rating + total_monster_count * 0.25f;
}

/* static */ String BG_Booker_DB::get_job_challenge_rating(const TypedArray<BG_Monster> monsters)
{
	String result = "";

	fraction_struct fract_struct = get_fract(BG_Booker_DB::get_job_challenge_rating_value(monsters));
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
//// BG_UnitStat
////
/* static */ Vector2i BG_UnitStat::string_to_resistant_value_min_max(String string)
{
	if (string.is_empty())
	{
		return Vector2i(0, 0);
	}
	if (!string.contains("["))
	{
		return Vector2i(string.to_int(), string.to_int());
	}

	const String with_bracets = string.split("[")[1].split("]")[0].replace(" ", ""); // I.e: [1-4] to 1-4
	const PackedStringArray tilda_split = with_bracets.split("~"); // Using tilda here so that it's easier to use - for negative numbers.
	return Vector2i(tilda_split[0].to_int(), tilda_split[1].to_int());;
}

////
//// BG_Dice
////
/* static */ int BG_Dice::calculate_dice(const TypedArray<BG_Dice> dice, RandomNumberGenerator *random_num_generator)
{
	int result = 0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice[i]))
			continue;
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			if ( (random_num_generator != nullptr) && UtilityFunctions::is_instance_id_valid(random_num_generator->get_instance_id()) )
				result += random_num_generator->randi_range(1, die->get_amount_of_sides());
			else
				result += UtilityFunctions::randi_range(1, die->get_amount_of_sides());
		}
		result += die->get_additive();
	}

	return Math::max(0, result);
}

/* static */ int BG_Dice::get_dice_max_roll(const TypedArray<BG_Dice> dice)
{
	int result = 0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(die))
			continue;
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			result += die->get_amount_of_sides();
		}
		result += die->get_additive();
	}

	return result;
}

/* static */ int BG_Dice::get_dice_average_roll(const TypedArray<BG_Dice> dice)
{
	int max_roll = 0;
	int additives = 0;
	int roll_counts = 0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice[i]))
			continue;
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			roll_counts += 1;
			max_roll += die->get_amount_of_sides();
		}
		additives += die->get_additive();
	}

	return int(float(max_roll + roll_counts) * 0.5f) + additives;
}

/* static */ String BG_Dice::dice_to_nice_name(const TypedArray<BG_Dice> dice)
{
	int minimum_damage = 0;
	int maximum_damage = 0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice[i]))
			continue;
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
	if ( !BG_Focus_Layer_Properties::bg_is_instance_valid(dice) )
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

/* static */ BG_Dice *BG_Dice::add_bonus_to_die(BG_Dice *die, const float bonus)
{
	if (!BG_Focus_Layer_Properties::bg_is_instance_valid(die)) return nullptr;
	die->amount_of_sides += int(float(die->amount_of_sides) * bonus);
	die->additive += int(float(die->additive) * bonus);
	return die;
}

/* static */ BG_Dice *BG_Dice::duplicate_dice(const BG_Dice *dice)
{
	if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice)) return nullptr;
	BG_Dice *result = memnew(BG_Dice);
	result->roll_count = dice->roll_count;
	result->amount_of_sides = dice->amount_of_sides;
	result->additive = dice->additive;
	return result;
}
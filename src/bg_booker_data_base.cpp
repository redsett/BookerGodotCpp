#include "bg_booker_data_base.hpp"
#include "bg_focus_layers.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <string>

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

static Color convert_hex_to_color(const String &hex)
{
	return Color(hex);
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
//// BG_TwoDer_DataEntry
////
void BG_TwoDer_DataEntry::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_TwoDer_DataEntry::get_id);
	ClassDB::bind_method(D_METHOD("get_data_path"), &BG_TwoDer_DataEntry::get_data_path);
	ClassDB::bind_method(D_METHOD("get_asset_name"), &BG_TwoDer_DataEntry::get_asset_name);
	ClassDB::bind_method(D_METHOD("get_prop_name"), &BG_TwoDer_DataEntry::get_prop_name);
	ClassDB::bind_method(D_METHOD("get_anim_name"), &BG_TwoDer_DataEntry::get_anim_name);
	ClassDB::bind_method(D_METHOD("get_camera_name"), &BG_TwoDer_DataEntry::get_camera_name);
	ClassDB::bind_method(D_METHOD("get_anim_speed_mult"), &BG_TwoDer_DataEntry::get_anim_speed_mult);
	ClassDB::bind_method(D_METHOD("get_scale_multiplier"), &BG_TwoDer_DataEntry::get_scale_multiplier);
}

////
//// BG_LocalizeEntryData
////
void BG_LocalizeEntryData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_code_start"), &BG_LocalizeEntryData::get_code_start);
	ClassDB::bind_method(D_METHOD("get_text"), &BG_LocalizeEntryData::get_text);
	ClassDB::bind_method(D_METHOD("get_code_end"), &BG_LocalizeEntryData::get_code_end);
}

////
//// BG_BattleBoard_HexTypeVisualDetails
////
void BG_BattleBoard_HexTypeVisualDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_BattleBoard_HexTypeVisualDetails::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_two_der_id"), &BG_BattleBoard_HexTypeVisualDetails::get_two_der_id);
	ClassDB::bind_method(D_METHOD("get_scene_path"), &BG_BattleBoard_HexTypeVisualDetails::get_scene_path);
	ClassDB::bind_method(D_METHOD("get_destroyed_icon_path"), &BG_BattleBoard_HexTypeVisualDetails::get_destroyed_icon_path);
	ClassDB::bind_method(D_METHOD("get_destroyed_two_der_id"), &BG_BattleBoard_HexTypeVisualDetails::get_destroyed_two_der_id);
	ClassDB::bind_method(D_METHOD("get_destroyed_scene_path"), &BG_BattleBoard_HexTypeVisualDetails::get_destroyed_scene_path);
	ClassDB::bind_method(D_METHOD("get_hex_tint"), &BG_BattleBoard_HexTypeVisualDetails::get_hex_tint);
}

////
//// BG_BattleBoard_HexTypeDetails
////
void BG_BattleBoard_HexTypeDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_BattleBoard_HexTypeDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_hex_type"), &BG_BattleBoard_HexTypeDetails::get_hex_type);
	ClassDB::bind_method(D_METHOD("get_visuals"), &BG_BattleBoard_HexTypeDetails::get_visuals);
	ClassDB::bind_method(D_METHOD("get_destroyed_vfx_scene_path"), &BG_BattleBoard_HexTypeDetails::get_destroyed_vfx_scene_path);
	ClassDB::bind_method(D_METHOD("get_destroyed_sfx_id"), &BG_BattleBoard_HexTypeDetails::get_destroyed_sfx_id);
	ClassDB::bind_method(D_METHOD("get_hex_visual_scene_path_override"), &BG_BattleBoard_HexTypeDetails::get_hex_visual_scene_path_override);
	ClassDB::bind_method(D_METHOD("get_health_effectiveness"), &BG_BattleBoard_HexTypeDetails::get_health_effectiveness);
	ClassDB::bind_method(D_METHOD("get_equipment_ids"), &BG_BattleBoard_HexTypeDetails::get_equipment_ids);
}

////
//// BG_BattleBoardDetails
////
void BG_BattleBoardDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_BattleBoardDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_unique_save_name"), &BG_BattleBoardDetails::get_unique_save_name);
	ClassDB::bind_method(D_METHOD("get_board_path"), &BG_BattleBoardDetails::get_board_path);
	ClassDB::bind_method(D_METHOD("get_default_hex_visual_path"), &BG_BattleBoardDetails::get_default_hex_visual_path);
	ClassDB::bind_method(D_METHOD("get_hex_types"), &BG_BattleBoardDetails::get_hex_types);

	ClassDB::bind_method(D_METHOD("get_hex_types_by_type", "type"), &BG_BattleBoardDetails::get_hex_types_by_type);
	ClassDB::bind_method(D_METHOD("get_hex_type_by_id", "id"), &BG_BattleBoardDetails::get_hex_type_by_id);
}

////
//// BG_ObjectiveTimeline
////
void BG_ObjectiveTimeline::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_act"), &BG_ObjectiveTimeline::get_act);
	ClassDB::bind_method(D_METHOD("get_min_week"), &BG_ObjectiveTimeline::get_min_week);
	ClassDB::bind_method(D_METHOD("get_max_week"), &BG_ObjectiveTimeline::get_max_week);
}

////
//// BG_ObjectiveDetails
////
void BG_ObjectiveDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ObjectiveDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_localization_key"), &BG_ObjectiveDetails::get_localization_key);
	ClassDB::bind_method(D_METHOD("get_is_main_objective"), &BG_ObjectiveDetails::get_is_main_objective);
	ClassDB::bind_method(D_METHOD("get_is_scripted"), &BG_ObjectiveDetails::get_is_scripted);
	ClassDB::bind_method(D_METHOD("get_objective_repeatable_type"), &BG_ObjectiveDetails::get_objective_repeatable_type);
	ClassDB::bind_method(D_METHOD("get_is_auto_complete"), &BG_ObjectiveDetails::get_is_auto_complete);
	ClassDB::bind_method(D_METHOD("get_is_turn_in"), &BG_ObjectiveDetails::get_is_turn_in);
	ClassDB::bind_method(D_METHOD("get_is_event"), &BG_ObjectiveDetails::get_is_event);
	ClassDB::bind_method(D_METHOD("get_timeline"), &BG_ObjectiveDetails::get_timeline);
	ClassDB::bind_method(D_METHOD("get_expires_in"), &BG_ObjectiveDetails::get_expires_in);
	ClassDB::bind_method(D_METHOD("get_script_path"), &BG_ObjectiveDetails::get_script_path);
	ClassDB::bind_method(D_METHOD("get_reputation_drop"), &BG_ObjectiveDetails::get_reputation_drop);
	ClassDB::bind_method(D_METHOD("get_maelstrite_drop"), &BG_ObjectiveDetails::get_maelstrite_drop);
	ClassDB::bind_method(D_METHOD("get_drops"), &BG_ObjectiveDetails::get_drops);
	ClassDB::bind_method(D_METHOD("get_beast_part_drop_count"), &BG_ObjectiveDetails::get_beast_part_drop_count);
	ClassDB::bind_method(D_METHOD("get_equipment_drop_count"), &BG_ObjectiveDetails::get_equipment_drop_count);
	ClassDB::bind_method(D_METHOD("get_misc_attributes"), &BG_ObjectiveDetails::get_misc_attributes);

	BIND_ENUM_CONSTANT(UNIQUE);
	BIND_ENUM_CONSTANT(REPEATABLE_UNIQUE);
	BIND_ENUM_CONSTANT(REPEATABLE);
}

////
//// BG_ResourceTypeDetails
////
void BG_ResourceTypeDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ResourceTypeDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_ResourceTypeDetails::get_icon_path);
}

////
//// BG_MailData
////
void BG_MailData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_MailData::get_id);
	ClassDB::bind_method(D_METHOD("get_act"), &BG_MailData::get_act);
	ClassDB::bind_method(D_METHOD("get_week"), &BG_MailData::get_week);
}

////
//// BG_MarketplaceData
////
void BG_MarketplaceData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_items_per_month_min_max"), &BG_MarketplaceData::get_items_per_month_min_max);
	ClassDB::bind_method(D_METHOD("get_entries"), &BG_MarketplaceData::get_entries);
}

////
//// BG_MarketplaceEntryData
////
void BG_MarketplaceEntryData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_act"), &BG_MarketplaceEntryData::get_act);
	ClassDB::bind_method(D_METHOD("get_starting_month"), &BG_MarketplaceEntryData::get_starting_month);
	ClassDB::bind_method(D_METHOD("get_item_drop_pool_ids"), &BG_MarketplaceEntryData::get_item_drop_pool_ids);
}

////
//// BG_MarketplaceSaveData
////
void BG_MarketplaceSaveData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_seed"), &BG_MarketplaceSaveData::get_seed);
	ClassDB::bind_method(D_METHOD("set_seed"), &BG_MarketplaceSaveData::set_seed);
	ClassDB::bind_method(D_METHOD("get_items_sold_indexies"), &BG_MarketplaceSaveData::get_items_sold_indexies);
	ClassDB::bind_method(D_METHOD("set_items_sold_indexies"), &BG_MarketplaceSaveData::set_items_sold_indexies);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "items_sold_indexies"), "set_items_sold_indexies", "get_items_sold_indexies");
}

////
//// BG_AudioDataDetails
////
void BG_AudioDataDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_file_path"), &BG_AudioDataDetails::get_file_path);
	ClassDB::bind_method(D_METHOD("get_volume_db_base_value"), &BG_AudioDataDetails::get_volume_db_base_value);
	ClassDB::bind_method(D_METHOD("get_restrict_to_acts"), &BG_AudioDataDetails::get_restrict_to_acts);
}

////
//// BG_AudioData
////
void BG_AudioData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_AudioData::get_id);
	ClassDB::bind_method(D_METHOD("get_audio_details"), &BG_AudioData::get_audio_details);
}

////
//// BG_HueShiftData
////
void BG_HueShiftData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_stat_id"), &BG_HueShiftData::get_stat_id);
	ClassDB::bind_method(D_METHOD("get_mask_path"), &BG_HueShiftData::get_mask_path);
	ClassDB::bind_method(D_METHOD("get_from_color"), &BG_HueShiftData::get_from_color);
	ClassDB::bind_method(D_METHOD("get_multiplier"), &BG_HueShiftData::get_multiplier);
}

////
//// BG_BaseStat
////
void BG_BaseStat::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_unique_id"), &BG_BaseStat::get_unique_id);
	ClassDB::bind_method(D_METHOD("get_is_base_value"), &BG_BaseStat::get_is_base_value);
	ClassDB::bind_method(D_METHOD("get_value"), &BG_BaseStat::get_value);
	// ClassDB::bind_method(D_METHOD("get_parent_stat_unique_id"), &BG_BaseStat::get_parent_stat_unique_id);
	ClassDB::bind_method(D_METHOD("get_parent_stat_reference"), &BG_BaseStat::get_parent_stat_reference);
	ClassDB::bind_method(D_METHOD("get_stat_id_name"), &BG_BaseStat::get_stat_id_name);
}

////
//// BG_ContentStat
////
void BG_ContentStat::_bind_methods()
{
	ClassDB::bind_static_method("BG_ContentStat", D_METHOD("get_total_stat_value", "stat"), &BG_ContentStat::get_total_stat_value);

	ClassDB::bind_method(D_METHOD("get_stat_reference"), &BG_ContentStat::get_stat_reference);
	ClassDB::bind_method(D_METHOD("get_value"), &BG_ContentStat::get_value);
	ClassDB::bind_method(D_METHOD("get_randomize_damage_type"), &BG_ContentStat::get_randomize_damage_type);
	ClassDB::bind_method(D_METHOD("get_randomize_resistance_type"), &BG_ContentStat::get_randomize_resistance_type);
}

/* static */ float BG_ContentStat::get_total_stat_value(const BG_BaseStat *stat)
{
	if (stat == nullptr) return 0.0;
	if (stat->get_is_base_value())
		return stat->get_value();
	
	float result = 0.0;
	float multiplier = 1.0;
	multiplier *= stat->get_value();
	
	const BG_BaseStat *current_stat = stat;
	while (current_stat->get_parent_stat_reference() != nullptr) {
		current_stat = current_stat->get_parent_stat_reference();
		
		if (current_stat->get_is_base_value())
			result += current_stat->get_value();
		else
			multiplier *= current_stat->get_value();
	}
	
	float amount_to_mult = 1.0 / multiplier;
	result *= amount_to_mult;
	return result;
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
	ClassDB::bind_static_method("BG_Dice", D_METHOD("duplicate_dice", "dice"), &BG_Dice::duplicate_dice);

	ClassDB::bind_method(D_METHOD("get_roll_count"), &BG_Dice::get_roll_count);
	ClassDB::bind_method(D_METHOD("set_roll_count"), &BG_Dice::set_roll_count);
	ClassDB::bind_method(D_METHOD("get_amount_of_sides"), &BG_Dice::get_amount_of_sides);
	ClassDB::bind_method(D_METHOD("set_amount_of_sides"), &BG_Dice::set_amount_of_sides);
	ClassDB::bind_method(D_METHOD("get_additive"), &BG_Dice::get_additive);
	ClassDB::bind_method(D_METHOD("set_additive"), &BG_Dice::set_additive);
	ClassDB::bind_method(D_METHOD("get_multiplier"), &BG_Dice::get_multiplier);
	ClassDB::bind_method(D_METHOD("set_multiplier"), &BG_Dice::set_multiplier);
	ClassDB::bind_method(D_METHOD("get_use_raw_numbers"), &BG_Dice::get_use_raw_numbers);
	ClassDB::bind_method(D_METHOD("set_use_raw_numbers"), &BG_Dice::set_use_raw_numbers);
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

////
//// BG_AnimationTagDetails
////
void BG_AnimationTagDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_tag"), &BG_AnimationTagDetails::get_tag);
	ClassDB::bind_method(D_METHOD("get_frame"), &BG_AnimationTagDetails::get_frame);
}

////
//// BG_AnimationDetails
////
void BG_AnimationDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_caste_or_monster_id"), &BG_AnimationDetails::get_caste_or_monster_id);
	ClassDB::bind_method(D_METHOD("get_anim_type"), &BG_AnimationDetails::get_anim_type);
	ClassDB::bind_method(D_METHOD("get_anim_name"), &BG_AnimationDetails::get_anim_name);
	ClassDB::bind_method(D_METHOD("get_traverse_to"), &BG_AnimationDetails::get_traverse_to);
	ClassDB::bind_method(D_METHOD("get_traverse_back"), &BG_AnimationDetails::get_traverse_back);
	ClassDB::bind_method(D_METHOD("get_anim_blend_time"), &BG_AnimationDetails::get_anim_blend_time);
	ClassDB::bind_method(D_METHOD("get_tags"), &BG_AnimationDetails::get_tags);

	ClassDB::bind_static_method("BG_AnimationDetails", D_METHOD("get_all_anim_details_of_id", "from", "id"), &BG_AnimationDetails::get_all_anim_details_of_id);
	ClassDB::bind_static_method("BG_AnimationDetails", D_METHOD("get_anim_details", "from", "id", "anim_type"), &BG_AnimationDetails::get_anim_details);

	BIND_ENUM_CONSTANT(IDLE);
	BIND_ENUM_CONSTANT(COMBAT_IDLE);
	BIND_ENUM_CONSTANT(ATTACK1);
	BIND_ENUM_CONSTANT(ATTACK2);
	BIND_ENUM_CONSTANT(ATTACK3);
	BIND_ENUM_CONSTANT(UNIQUE_SKILL1);
	BIND_ENUM_CONSTANT(RUN);
	BIND_ENUM_CONSTANT(GUARD);
	BIND_ENUM_CONSTANT(EVADE);
	BIND_ENUM_CONSTANT(TAKING_DAMAGE);
	BIND_ENUM_CONSTANT(AT_LOW_HP);
	BIND_ENUM_CONSTANT(DEATH);
	BIND_ENUM_CONSTANT(TRIUMPH);
	BIND_ENUM_CONSTANT(FLOURISH);
	BIND_ENUM_CONSTANT(TRAVERSE_TO_INTRO);
	BIND_ENUM_CONSTANT(TRAVERSE_TO_LOOP);
	BIND_ENUM_CONSTANT(TRAVERSE_TO_OUTRO);
	BIND_ENUM_CONSTANT(TRAVERSE_BACK_INTRO);
	BIND_ENUM_CONSTANT(TRAVERSE_BACK_LOOP);
	BIND_ENUM_CONSTANT(TRAVERSE_BACK_OUTRO);

	BIND_ENUM_CONSTANT(TRAVERSE_TYPE_NONE);
	BIND_ENUM_CONSTANT(TRAVERSE_TYPE_RUN);
	BIND_ENUM_CONSTANT(TRAVERSE_TYPE_RUN_INTRO_LOOP_OUTRO);
	BIND_ENUM_CONSTANT(TRAVERSE_TYPE_HOP);
	BIND_ENUM_CONSTANT(TRAVERSE_TYPE_HOP_INTRO_LOOP_OUTRO);
}

/* static */ TypedArray<BG_AnimationDetails> BG_AnimationDetails::get_all_anim_details_of_id(TypedArray<BG_AnimationDetails> from, StringName id)
{
	TypedArray<BG_AnimationDetails> result;
	for (int i = 0; i < from.size(); ++i) {
		BG_AnimationDetails *anim_dets = cast_to<BG_AnimationDetails>(from[i]);
		if (anim_dets == nullptr) continue;
		if (anim_dets->get_caste_or_monster_id() == id) {
			result.append(anim_dets);
		}
	}
	return result;
}

/* static */ BG_AnimationDetails *BG_AnimationDetails::get_anim_details(TypedArray<BG_AnimationDetails> from, StringName id, AnimationType anim_type)
{
	for (int i = 0; i < from.size(); ++i) {
		BG_AnimationDetails *anim_dets = cast_to<BG_AnimationDetails>(from[i]);
		if (anim_dets->get_caste_or_monster_id() != id) continue;
		if (anim_dets->get_anim_type() == anim_type) {
			return anim_dets;
		}
	}
	return nullptr;
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
	ClassDB::bind_method(D_METHOD("get_level"), &BG_Item::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_Item::set_level);
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
	ClassDB::bind_method(D_METHOD("get_nested_items"), &BG_Item::get_nested_items);
	ClassDB::bind_method(D_METHOD("set_nested_items"), &BG_Item::set_nested_items);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "on_shelf"), "set_on_shelf", "get_on_shelf");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "shelf_index"), "set_shelf_index", "get_shelf_index");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_bid"), "set_has_bid", "get_has_bid");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bid_amount"), "set_bid_amount", "get_bid_amount");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_equipped"), "set_is_equipped", "get_is_equipped");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_durability"), "set_current_durability", "get_current_durability");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_fame"), "set_current_fame", "get_current_fame");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "weeks_left_to_finish_graft"), "set_weeks_left_to_finish_graft", "get_weeks_left_to_finish_graft");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "number_of_jobs_participated_in"), "set_number_of_jobs_participated_in", "get_number_of_jobs_participated_in");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "consecutive_number_of_jobs_participated_in"), "set_consecutive_number_of_jobs_participated_in", "get_consecutive_number_of_jobs_participated_in");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "should_count_in_item_rarity_disparity"), "set_should_count_in_item_rarity_disparity", "get_should_count_in_item_rarity_disparity");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "rarity_id"), "set_rarity_id", "get_rarity_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "grafts"), "set_grafts", "get_grafts");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "forced_damage_element"), "set_forced_damage_element", "get_forced_damage_element");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "nested_items"), "set_nested_items", "get_nested_items");
}

////
//// BG_ItemDetails
////
void BG_ItemDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_ItemDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_ItemDetails::get_name);
	ClassDB::bind_method(D_METHOD("get_slot_type"), &BG_ItemDetails::get_slot_type);
	ClassDB::bind_method(D_METHOD("is_gear"), &BG_ItemDetails::is_gear);
	ClassDB::bind_method(D_METHOD("is_beast_part"), &BG_ItemDetails::is_beast_part);
	ClassDB::bind_method(D_METHOD("is_consumable"), &BG_ItemDetails::is_consumable);
	ClassDB::bind_method(D_METHOD("get_slot_type_id"), &BG_ItemDetails::get_slot_type_id);
	ClassDB::bind_method(D_METHOD("get_beast_part_available_item_slot_types"), &BG_ItemDetails::get_beast_part_available_item_slot_types);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_ItemDetails::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_hue_shift_data"), &BG_ItemDetails::get_hue_shift_data);
	ClassDB::bind_method(D_METHOD("get_mesh_path"), &BG_ItemDetails::get_mesh_path);
	ClassDB::bind_method(D_METHOD("get_lore"), &BG_ItemDetails::get_lore);
	ClassDB::bind_method(D_METHOD("get_caste_ids"), &BG_ItemDetails::get_caste_ids);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_ItemDetails::get_stats);
	ClassDB::bind_method(D_METHOD("get_animation_attach_socket"), &BG_ItemDetails::get_animation_attach_socket);
	ClassDB::bind_method(D_METHOD("get_effects"), &BG_ItemDetails::get_effects);
	ClassDB::bind_method(D_METHOD("get_sell_value_tier"), &BG_ItemDetails::get_sell_value_tier);
	ClassDB::bind_method(D_METHOD("get_fame_value_tier"), &BG_ItemDetails::get_fame_value_tier);
	ClassDB::bind_method(D_METHOD("get_durability_value_tier"), &BG_ItemDetails::get_durability_value_tier);

	ClassDB::bind_method(D_METHOD("get_use_dber_data"), &BG_ItemDetails::get_use_dber_data);
	ClassDB::bind_method(D_METHOD("get_item_effectiveness_stats"), &BG_ItemDetails::get_item_effectiveness_stats);
	ClassDB::bind_method(D_METHOD("get_effectiveness"), &BG_ItemDetails::get_effectiveness);
	ClassDB::bind_method(D_METHOD("get_use_stat_requirements"), &BG_ItemDetails::get_use_stat_requirements);
	ClassDB::bind_method(D_METHOD("get_item_stat_requirements"), &BG_ItemDetails::get_item_stat_requirements);
	ClassDB::bind_method(D_METHOD("get_level_range"), &BG_ItemDetails::get_level_range);
	ClassDB::bind_method(D_METHOD("get_item_animations"), &BG_ItemDetails::get_item_animations);

	BIND_ENUM_CONSTANT(GEAR);
	BIND_ENUM_CONSTANT(BEAST_PART);
	BIND_ENUM_CONSTANT(CONSUMABLE);

	BIND_ENUM_CONSTANT(SINGLE_FRONT);
	BIND_ENUM_CONSTANT(SINGLE_BACK);
	BIND_ENUM_CONSTANT(ALL_FRONT_ROW);
	BIND_ENUM_CONSTANT(ALL_BACK_ROW);
	BIND_ENUM_CONSTANT(ALL_RANDOM_ROW);
	BIND_ENUM_CONSTANT(ALL_RANDOM_COLUMN);
	BIND_ENUM_CONSTANT(ALL);
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
	ClassDB::bind_method(D_METHOD("get_is_band_leader"), &BG_BandMember::get_is_band_leader);
	ClassDB::bind_method(D_METHOD("set_is_band_leader"), &BG_BandMember::set_is_band_leader);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_BandMember::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_BandMember::set_level);
	ClassDB::bind_method(D_METHOD("get_experience"), &BG_BandMember::get_experience);
	ClassDB::bind_method(D_METHOD("set_experience"), &BG_BandMember::set_experience);
	ClassDB::bind_method(D_METHOD("get_scale"), &BG_BandMember::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale"), &BG_BandMember::set_scale);
	ClassDB::bind_method(D_METHOD("get_caste_id"), &BG_BandMember::get_caste_id);
	ClassDB::bind_method(D_METHOD("set_caste_id"), &BG_BandMember::set_caste_id);
	ClassDB::bind_method(D_METHOD("get_equipment"), &BG_BandMember::get_equipment);
	ClassDB::bind_method(D_METHOD("set_equipment"), &BG_BandMember::set_equipment);
	ClassDB::bind_method(D_METHOD("get_is_city_asset"), &BG_BandMember::get_is_city_asset);
	ClassDB::bind_method(D_METHOD("set_is_city_asset"), &BG_BandMember::set_is_city_asset);
	ClassDB::bind_method(D_METHOD("get_band"), &BG_BandMember::get_band);
	ClassDB::bind_method(D_METHOD("set_band"), &BG_BandMember::set_band);
	ClassDB::bind_method(D_METHOD("get_turret_info"), &BG_BandMember::get_turret_info);
	ClassDB::bind_method(D_METHOD("set_turret_info"), &BG_BandMember::set_turret_info);
	ClassDB::bind_method(D_METHOD("get_element_upgrades"), &BG_BandMember::get_element_upgrades);
	ClassDB::bind_method(D_METHOD("set_element_upgrades"), &BG_BandMember::set_element_upgrades);
	ClassDB::bind_method(D_METHOD("get_consumable_upgrades"), &BG_BandMember::get_consumable_upgrades);
	ClassDB::bind_method(D_METHOD("set_consumable_upgrades"), &BG_BandMember::set_consumable_upgrades);
	ClassDB::bind_method(D_METHOD("is_dead"), &BG_BandMember::is_dead);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_health"), "set_current_health", "get_current_health");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "slot_index"), "set_slot_index", "get_slot_index");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_band_leader"), "set_is_band_leader", "get_is_band_leader");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "experience"), "set_experience", "get_experience");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "caste_id"), "set_caste_id", "get_caste_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipment"), "set_equipment", "get_equipment");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "element_upgrades"), "set_element_upgrades", "get_element_upgrades");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "consumable_upgrades"), "set_consumable_upgrades", "get_consumable_upgrades");
}

////
//// BG_Formation
////
void BG_Formation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Formation::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Formation::set_id);
	ClassDB::bind_method(D_METHOD("get_formation"), &BG_Formation::get_formation);
	ClassDB::bind_method(D_METHOD("set_formation"), &BG_Formation::set_formation);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "id", PROPERTY_HINT_MULTILINE_TEXT), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "formation"), "set_formation", "get_formation");
}

////
//// BG_DefaultFormations
////
void BG_DefaultFormations::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_formations"), &BG_DefaultFormations::get_formations);
	ClassDB::bind_method(D_METHOD("set_formations"), &BG_DefaultFormations::set_formations);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "formations"), "set_formations", "get_formations");
}

////
//// BG_Band
////
void BG_Band::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_unique_id"), &BG_Band::get_unique_id);
	ClassDB::bind_method(D_METHOD("set_unique_id"), &BG_Band::set_unique_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Band::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Band::set_name);
	ClassDB::bind_method(D_METHOD("get_resting"), &BG_Band::get_resting);
	ClassDB::bind_method(D_METHOD("set_resting"), &BG_Band::set_resting);
	ClassDB::bind_method(D_METHOD("get_is_on_battle_board"), &BG_Band::get_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("set_is_on_battle_board"), &BG_Band::set_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("get_distance_traveled_this_turn"), &BG_Band::get_distance_traveled_this_turn);
	ClassDB::bind_method(D_METHOD("set_distance_traveled_this_turn"), &BG_Band::set_distance_traveled_this_turn);
	ClassDB::bind_method(D_METHOD("get_has_done_combat_this_turn"), &BG_Band::get_has_done_combat_this_turn);
	ClassDB::bind_method(D_METHOD("set_has_done_combat_this_turn"), &BG_Band::set_has_done_combat_this_turn);
	ClassDB::bind_method(D_METHOD("get_knocked_out_turns"), &BG_Band::get_knocked_out_turns);
	ClassDB::bind_method(D_METHOD("set_knocked_out_turns"), &BG_Band::set_knocked_out_turns);
	ClassDB::bind_method(D_METHOD("get_band_members"), &BG_Band::get_band_members);
	ClassDB::bind_method(D_METHOD("set_band_members"), &BG_Band::set_band_members);
	ClassDB::bind_method(D_METHOD("get_current_unique_job_id"), &BG_Band::get_current_unique_job_id);
	ClassDB::bind_method(D_METHOD("set_current_unique_job_id"), &BG_Band::set_current_unique_job_id);
	ClassDB::bind_method(D_METHOD("get_band_formation"), &BG_Band::get_band_formation);
	ClassDB::bind_method(D_METHOD("set_band_formation"), &BG_Band::set_band_formation);

	ClassDB::bind_method(D_METHOD("has_job"), &BG_Band::has_job);
	ClassDB::bind_method(D_METHOD("clear_job"), &BG_Band::clear_job);
	ClassDB::bind_method(D_METHOD("is_band_alive"), &BG_Band::is_band_alive);
	ClassDB::bind_method(D_METHOD("get_band_leader"), &BG_Band::get_band_leader);
	ClassDB::bind_method(D_METHOD("set_band_leader", "band_member"), &BG_Band::set_band_leader);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_id"), "set_unique_id", "get_unique_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "resting"), "set_resting", "get_resting");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_on_battle_board"), "set_is_on_battle_board", "get_is_on_battle_board");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "distance_traveled_this_turn"), "set_distance_traveled_this_turn", "get_distance_traveled_this_turn");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_done_combat_this_turn"), "set_has_done_combat_this_turn", "get_has_done_combat_this_turn");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "knocked_out_turns"), "set_knocked_out_turns", "get_knocked_out_turns");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "band_members"), "set_band_members", "get_band_members");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_unique_job_id"), "set_current_unique_job_id", "get_current_unique_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "band_formation"), "set_band_formation", "get_band_formation");
}

bool BG_Band::is_band_alive() const
{
	if (get_knocked_out_turns() > 1)
		return false;
	
	for (uint32_t i = 0; i < get_band_members().size(); ++i) {
		const Ref<BG_BandMember> band_member = get_band_members()[i];
		if (band_member.is_null()) continue;
		if (band_member->get_current_health() > 0) {
			return true;
		}
	}
	return false;
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
	ClassDB::bind_method(D_METHOD("get_travel_distance"), &BG_UnitCaste::get_travel_distance);
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
//// BG_Job
////
void BG_Job::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_job_id"), &BG_Job::get_job_id);
	ClassDB::bind_method(D_METHOD("set_job_id"), &BG_Job::set_job_id);
	ClassDB::bind_method(D_METHOD("get_unique_job_id"), &BG_Job::get_unique_job_id);
	ClassDB::bind_method(D_METHOD("set_unique_job_id"), &BG_Job::set_unique_job_id);
	ClassDB::bind_method(D_METHOD("get_random_seed"), &BG_Job::get_random_seed);
	ClassDB::bind_method(D_METHOD("set_random_seed"), &BG_Job::set_random_seed);
	ClassDB::bind_method(D_METHOD("get_weeks_remaining_unless_removed"), &BG_Job::get_weeks_remaining_unless_removed);
	ClassDB::bind_method(D_METHOD("set_weeks_remaining_unless_removed"), &BG_Job::set_weeks_remaining_unless_removed);
	ClassDB::bind_method(D_METHOD("get_monsters"), &BG_Job::get_monsters);
	ClassDB::bind_method(D_METHOD("set_monsters"), &BG_Job::set_monsters);
	ClassDB::bind_method(D_METHOD("get_formation"), &BG_Job::get_formation);
	ClassDB::bind_method(D_METHOD("set_formation"), &BG_Job::set_formation);
	ClassDB::bind_method(D_METHOD("get_is_on_battle_board"), &BG_Job::get_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("set_is_on_battle_board"), &BG_Job::set_is_on_battle_board);
	ClassDB::bind_method(D_METHOD("try_set_all_monsters_to_stoned", "is_stoned"), &BG_Job::try_set_all_monsters_to_stoned);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "job_id"), "set_job_id", "get_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_job_id"), "set_unique_job_id", "get_unique_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_seed"), "set_random_seed", "get_random_seed");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "weeks_remaining_unless_removed"), "set_weeks_remaining_unless_removed", "get_weeks_remaining_unless_removed");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "monsters"), "set_monsters", "get_monsters");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "formation"), "set_formation", "get_formation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_on_battle_board"), "set_is_on_battle_board", "get_is_on_battle_board");
}

void BG_Job::try_set_all_monsters_to_stoned(bool stoned)
{
	for (int i = 0; i < monsters.size(); ++i) {
		BG_Monster *m = cast_to<BG_Monster>(monsters[i]);
		m->set_is_turned_to_stone(stoned);
	}
}

////
//// BG_Monster
////
void BG_Monster::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Monster::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Monster::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Monster::get_name);
	ClassDB::bind_method(D_METHOD("get_preferred_row"), &BG_Monster::get_preferred_row);
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_Monster::get_max_health);
	ClassDB::bind_method(D_METHOD("get_travel_distance"), &BG_Monster::get_travel_distance);
	ClassDB::bind_method(D_METHOD("get_current_health"), &BG_Monster::get_current_health);
	ClassDB::bind_method(D_METHOD("set_current_health"), &BG_Monster::set_current_health);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_Monster::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_Monster::set_random_variation);
	ClassDB::bind_method(D_METHOD("get_challenge_rating"), &BG_Monster::get_challenge_rating);
	ClassDB::bind_method(D_METHOD("get_level"), &BG_Monster::get_level);
	ClassDB::bind_method(D_METHOD("set_level"), &BG_Monster::set_level);
	ClassDB::bind_method(D_METHOD("get_monster_type"), &BG_Monster::get_monster_type);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_Monster::get_stats);
	ClassDB::bind_method(D_METHOD("get_element_availability_ids"), &BG_Monster::get_element_availability_ids);
	ClassDB::bind_method(D_METHOD("get_job"), &BG_Monster::get_job);
	ClassDB::bind_method(D_METHOD("set_job"), &BG_Monster::set_job);
	
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_Monster::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_model_path"), &BG_Monster::get_model_path);
	ClassDB::bind_method(D_METHOD("get_effect_ids"), &BG_Monster::get_effect_ids);
	ClassDB::bind_method(D_METHOD("get_out_of_combat_effect_ids"), &BG_Monster::get_out_of_combat_effect_ids);
	ClassDB::bind_method(D_METHOD("get_hue_shift_data"), &BG_Monster::get_hue_shift_data);
	ClassDB::bind_method(D_METHOD("get_challenge_rating_fraction_string"), &BG_Monster::get_challenge_rating_fraction_string);
	ClassDB::bind_method(D_METHOD("get_can_be_turned_to_stone"), &BG_Monster::get_can_be_turned_to_stone);
	ClassDB::bind_method(D_METHOD("get_is_turned_to_stone"), &BG_Monster::get_is_turned_to_stone);
	ClassDB::bind_method(D_METHOD("set_is_turned_to_stone"), &BG_Monster::set_is_turned_to_stone);
	ClassDB::bind_method(D_METHOD("is_dead"), &BG_Monster::is_dead);
	
	ClassDB::bind_method(D_METHOD("get_use_dber_data"), &BG_Monster::get_use_dber_data);
	ClassDB::bind_method(D_METHOD("get_effectiveness"), &BG_Monster::get_effectiveness);
	ClassDB::bind_method(D_METHOD("get_effectiveness_stats"), &BG_Monster::get_effectiveness_stats);
	ClassDB::bind_method(D_METHOD("get_level_range"), &BG_Monster::get_level_range);
	ClassDB::bind_method(D_METHOD("get_animations"), &BG_Monster::get_animations);
	
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_health"), "set_current_health", "get_current_health");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_turned_to_stone"), "set_is_turned_to_stone", "get_is_turned_to_stone");
}

void BG_Monster::set_is_turned_to_stone(bool v) {
	if (!can_be_turned_to_stone) {
		v = false;
	}
	if (v != is_turned_to_stone) {
		is_turned_to_stone = v;
		BG_Booker_DB::get_singleton()->emit_signal("monster_stoned_changed", this, v);
	}
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
	ClassDB::bind_method(D_METHOD("get_rest_recovery_speed"), &BG_BandInfo::get_rest_recovery_speed);
	ClassDB::bind_method(D_METHOD("get_knocked_out_turns"), &BG_BandInfo::get_knocked_out_turns);
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
	ClassDB::bind_method(D_METHOD("get_min_job_spawn_count"), &BG_JobDistributionForAct::get_min_job_spawn_count);
	ClassDB::bind_method(D_METHOD("get_max_job_spawn_count"), &BG_JobDistributionForAct::get_max_job_spawn_count);
}

////
//// BG_EventResourceDetails
////
void BG_EventResourceDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_maelstrite_amount_of_drops_during_range"), &BG_EventResourceDetails::get_maelstrite_amount_of_drops_during_range);
	ClassDB::bind_method(D_METHOD("get_maelstrite_total_amount_by_end_of_weeks"), &BG_EventResourceDetails::get_maelstrite_total_amount_by_end_of_weeks);
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
	ClassDB::bind_method(D_METHOD("get_battle_board_event_type"), &BG_JobDetails::get_battle_board_event_type);
	ClassDB::bind_method(D_METHOD("get_weeks_before_expire"), &BG_JobDetails::get_weeks_before_expire);
	ClassDB::bind_method(D_METHOD("get_monster_details"), &BG_JobDetails::get_monster_details);
	ClassDB::bind_method(D_METHOD("get_event_resource_details"), &BG_JobDetails::get_event_resource_details);
	ClassDB::bind_method(D_METHOD("get_can_spawn_in_day_time"), &BG_JobDetails::get_can_spawn_in_day_time);
	ClassDB::bind_method(D_METHOD("get_is_unique"), &BG_JobDetails::get_is_unique);
	ClassDB::bind_method(D_METHOD("get_is_boss"), &BG_JobDetails::get_is_boss);
	ClassDB::bind_method(D_METHOD("get_distribution_per_act"), &BG_JobDetails::get_distribution_per_act);

	BIND_ENUM_CONSTANT(JOB);
	BIND_ENUM_CONSTANT(RESOURCE);
	BIND_ENUM_CONSTANT(EVENT);
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
	ClassDB::bind_method(D_METHOD("get_value_attributes"), &BG_BookerSkillTreeSlotDetails::get_value_attributes);
}

////
//// BG_TurretInfo
////
void BG_TurretInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_TurretInfo::get_id);
	ClassDB::bind_method(D_METHOD("get_nice_name"), &BG_TurretInfo::get_nice_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_TurretInfo::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_twoder_icons"), &BG_TurretInfo::get_twoder_icons);
	ClassDB::bind_method(D_METHOD("get_destroyed_icon_path"), &BG_TurretInfo::get_destroyed_icon_path);
	ClassDB::bind_method(D_METHOD("get_twoder_destroyed_icons"), &BG_TurretInfo::get_twoder_destroyed_icons);
	ClassDB::bind_method(D_METHOD("get_destroyed_vfx_path"), &BG_TurretInfo::get_destroyed_vfx_path);
	ClassDB::bind_method(D_METHOD("get_destroyed_sfx_id"), &BG_TurretInfo::get_destroyed_sfx_id);
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_TurretInfo::get_max_health);
	ClassDB::bind_method(D_METHOD("get_equipment_ids"), &BG_TurretInfo::get_equipment_ids);
}

////
//// BG_CityInfo
////
void BG_CityInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_CityInfo::get_id);
	ClassDB::bind_method(D_METHOD("get_nice_name"), &BG_CityInfo::get_nice_name);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_CityInfo::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_scene_path"), &BG_CityInfo::get_scene_path);
	ClassDB::bind_method(D_METHOD("get_battle_board_id"), &BG_CityInfo::get_battle_board_id);
	ClassDB::bind_method(D_METHOD("get_max_health"), &BG_CityInfo::get_max_health);
	ClassDB::bind_method(D_METHOD("get_equipment_ids"), &BG_CityInfo::get_equipment_ids);
	ClassDB::bind_method(D_METHOD("get_barracades"), &BG_CityInfo::get_barracades);
	ClassDB::bind_method(D_METHOD("get_turrets"), &BG_CityInfo::get_turrets);
	ClassDB::bind_method(D_METHOD("get_towns"), &BG_CityInfo::get_towns);
	ClassDB::bind_method(D_METHOD("get_misc_attributes"), &BG_CityInfo::get_misc_attributes);
}

////
//// BG_Booker_Globals
////
void BG_Booker_Globals::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_global_curves"), &BG_Booker_Globals::get_global_curves);
	ClassDB::bind_method(D_METHOD("get_starting_reputation"), &BG_Booker_Globals::get_starting_reputation);
	ClassDB::bind_method(D_METHOD("get_chance_of_no_drop"), &BG_Booker_Globals::get_chance_of_no_drop);
	ClassDB::bind_method(D_METHOD("get_combat_rounds_per_combat"), &BG_Booker_Globals::get_combat_rounds_per_combat);
	ClassDB::bind_method(D_METHOD("get_max_resistance_soft_cap"), &BG_Booker_Globals::get_max_resistance_soft_cap);
	ClassDB::bind_method(D_METHOD("get_max_resistance_hard_cap"), &BG_Booker_Globals::get_max_resistance_hard_cap);
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

	ClassDB::bind_method(D_METHOD("get_hit_perfect_timing_multiplier"), &BG_Booker_Globals::get_hit_perfect_timing_multiplier);
	ClassDB::bind_method(D_METHOD("get_hit_good_timing_multiplier"), &BG_Booker_Globals::get_hit_good_timing_multiplier);
	ClassDB::bind_method(D_METHOD("get_parry_perfect_timing_multiplier"), &BG_Booker_Globals::get_parry_perfect_timing_multiplier);
	ClassDB::bind_method(D_METHOD("get_parry_good_timing_multiplier"), &BG_Booker_Globals::get_parry_good_timing_multiplier);

	ClassDB::bind_method(D_METHOD("get_day_night_cycle_turns_per_day"), &BG_Booker_Globals::get_day_night_cycle_turns_per_day);
	ClassDB::bind_method(D_METHOD("get_percentage_of_day_is_day_time"), &BG_Booker_Globals::get_percentage_of_day_is_day_time);
	ClassDB::bind_method(D_METHOD("get_day_cycle_offset_per_act"), &BG_Booker_Globals::get_day_cycle_offset_per_act);
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

    ADD_SIGNAL(MethodInfo("monster_stoned_changed",
        PropertyInfo(Variant::OBJECT, "monster"),
        PropertyInfo(Variant::BOOL, "is_stoned")
    ));

	ClassDB::bind_method(D_METHOD("get_modding_path"), &BG_Booker_DB::get_modding_path);
	ClassDB::bind_method(D_METHOD("get_globals"), &BG_Booker_DB::get_globals);
	ClassDB::bind_method(D_METHOD("get_base_stats"), &BG_Booker_DB::get_base_stats);
	ClassDB::bind_method(D_METHOD("get_objectives"), &BG_Booker_DB::get_objectives);
	ClassDB::bind_method(D_METHOD("get_battle_boards_details"), &BG_Booker_DB::get_battle_boards_details);
	ClassDB::bind_method(D_METHOD("get_battle_board_by_id", "id"), &BG_Booker_DB::get_battle_board_by_id);
	ClassDB::bind_method(D_METHOD("get_resource_type_details_by_id", "resource_id"), &BG_Booker_DB::get_resource_type_details_by_id);
	ClassDB::bind_method(D_METHOD("get_audio_data"), &BG_Booker_DB::get_audio_data);
	ClassDB::bind_method(D_METHOD("get_audio_details", "id", "act"), &BG_Booker_DB::get_audio_details);
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
	ClassDB::bind_method(D_METHOD("get_market_place_data"), &BG_Booker_DB::get_market_place_data);
	ClassDB::bind_method(D_METHOD("get_monster_types"), &BG_Booker_DB::get_monster_types);
	ClassDB::bind_method(D_METHOD("get_mail_data"), &BG_Booker_DB::get_mail_data);
	ClassDB::bind_method(D_METHOD("get_two_der_data_entries"), &BG_Booker_DB::get_two_der_data_entries);
	ClassDB::bind_method(D_METHOD("get_two_der_data_from_id", "id"), &BG_Booker_DB::get_two_der_data_from_id);
	ClassDB::bind_method(D_METHOD("set_revert_localization_to_english"), &BG_Booker_DB::set_revert_localization_to_english);
	ClassDB::bind_method(D_METHOD("get_localize_data", "sheet_name", "key", "language"), &BG_Booker_DB::get_localize_data);
	ClassDB::bind_method(D_METHOD("get_localize_string", "sheet_name", "key", "language", "ignore_code_data"), &BG_Booker_DB::get_localize_string);
	ClassDB::bind_method(D_METHOD("get_stat_from_stat_id_name", "stat_id_name"), &BG_Booker_DB::get_stat_from_stat_id_name);
}

BG_Booker_DB *BG_Booker_DB::get_singleton()
{
	return singleton;
}

void BG_Booker_DB::refresh_data()
{
	if (globals != nullptr) memfree(globals);
	globals = memnew(BG_Booker_Globals);
	if (band_info != nullptr) memfree(band_info);
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

	//
	// Parse Booker DBer data.
	//
	const String booker_dber_data_file_name = "booker_dber_data.json";
	const String modding_dber_data_path = modding_path + booker_dber_data_file_name;

	try_parse_bder_data("res://" + booker_dber_data_file_name);

	// If the mod data exists, then let it override any data that it has.
	if (FileAccess::file_exists(modding_dber_data_path))
	{
		UtilityFunctions::print("Log - Using modding booker data.");
		try_parse_data(modding_dber_data_path);
	}
}

/* static */ void BG_Booker_DB::timer_test(Callable callable)
{
	static counter cont = counter();
	TIME_FUNC(callable, cont);
}

void BG_Booker_DB::try_parse_data(const String &file_path)
{
	const Dictionary data = BG_JsonUtils::ParseJsonFile(file_path);

	//UtilityFunctions::print(BG_JsonUtils::GetCBDSheet(data, "globals"));

	/////
	///// Marketplace
	/////
	{
		const Dictionary marketplace_data_sheet = BG_JsonUtils::GetCBDSheet(data, "marketplace");
		if (marketplace_data_sheet.has("lines"))
		{
			if (market_place_data != nullptr) {
				memdelete(market_place_data);
			}
			market_place_data = memnew(BG_MarketplaceData);

			const Array lines = Array(marketplace_data_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];

				// Items Per Month
				const Array items_per_month_lines = Array(entry["items_per_month"]);
				for (int y = 0; y < items_per_month_lines.size(); y++)
				{
					const Dictionary items_per_month_lines_entry = items_per_month_lines[y];
					market_place_data->items_per_month_min_max = Vector2i(int(items_per_month_lines_entry["min"]), int(items_per_month_lines_entry["max"]));
				}

				// Entries
				const Array entries_lines = Array(entry["entries"]);
				for (int y = 0; y < entries_lines.size(); y++)
				{
					const Dictionary entries_entry = entries_lines[y];

					BG_MarketplaceEntryData *new_marketplace_entry_data = memnew(BG_MarketplaceEntryData);
					new_marketplace_entry_data->act = int(entries_entry["act"]);
					new_marketplace_entry_data->starting_month = int(entries_entry["starting_month"]);

					const Array item_drop_pool_lines = Array(entries_entry["item_drop_pools"]);
					for (int x = 0; x < item_drop_pool_lines.size(); x++)
					{
						const Dictionary item_drop_pool_entry = item_drop_pool_lines[x];
						new_marketplace_entry_data->item_drop_pool_ids.append(item_drop_pool_entry["pool"]);
					}

					market_place_data->entries.append(new_marketplace_entry_data);
				}
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
				new_monster_type->icon_path = entry["icon_path"];
				new_monster_type->monster_type = int(entry["type"]);

				// Hue Shifting
				const Array hue_shifting_lines = Array(entry["hue_shifting"]);
				for (int y = 0; y < hue_shifting_lines.size(); y++)
				{
					const Dictionary hue_shifting_entry = hue_shifting_lines[y];

					BG_HueShiftData *new_hue_shift_data = memnew(BG_HueShiftData);
					new_hue_shift_data->stat_id = hue_shifting_entry["stat_type"];
					new_hue_shift_data->mask_path = hue_shifting_entry["mask_path"];
					new_hue_shift_data->from_color = convert_int_to_color(int(hue_shifting_entry["from_color"]));
					new_hue_shift_data->multiplier = float(hue_shifting_entry["multiplier"]);

					new_monster_type->hue_shift_data.append(new_hue_shift_data);
				}

				// Model Info
				const Array model_lines = Array(entry["model"]);
				for (int y = 0; y < model_lines.size(); y++)
				{
					const Dictionary model_entry = model_lines[y];

					new_monster_type->model_path = StringName(model_entry["path"]);
				}

				// Misc Stats
				const Array misc_stats_lines = Array(entry["misc_stats"]);
				for (int y = 0; y < misc_stats_lines.size(); y++)
				{
					const Dictionary misc_stats_entry = misc_stats_lines[y];
					new_monster_type->max_health = int(misc_stats_entry["health"]);
					new_monster_type->travel_distance = int(misc_stats_entry["travel_distance"]);
					new_monster_type->preferred_row = int(misc_stats_entry["preferred_row"]);
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
					if (effect_entry.has("effect"))
						new_monster_type->effect_ids.append(effect_entry["effect"]);
					if (effect_entry.has("out_of_combat_effect"))
						new_monster_type->out_of_combat_effect_ids.append(effect_entry["out_of_combat_effect"]);
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
					item_types.append("consumable_id");
					for (int it = 0; it < item_types.size(); it++)
					{
						const String item_type = item_types[it];
						if (!drops_entry.has(item_type)) continue;
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
	///// Battle Board Events
	/////
	{
		const Dictionary jobs_sheet = BG_JsonUtils::GetCBDSheet(data, "battle_board_events");
		if (jobs_sheet.has("lines"))
		{
			jobs.clear();

			const Array lines = Array(jobs_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				if (bool(entry["disabled"])) continue;

				BG_JobDetails *new_job_class = memnew(BG_JobDetails);
				new_job_class->id = entry["id"];
				new_job_class->battle_board_event_type = BG_JobDetails::BattleBoardEventTypes(int(entry["type"]));
				new_job_class->weeks_before_expire = int(entry["weeks_before_expire"]);
				new_job_class->can_spawn_in_day_time = bool(entry["can_spawn_in_day_time"]);
				new_job_class->is_unique = bool(entry["unique"]);
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
						item_types.append("consumable");
						for (int it = 0; it < item_types.size(); it++)
						{
							const String item_type = item_types[it];
							if (!drops_entry.has(item_type)) continue;
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

						if (!drops_entry.has("item_drop_pool")) continue;
						const StringName item_drop_pool_id = drops_entry["item_drop_pool"];
						if (item_drop_pool_id.is_empty()) continue;

						for (int pool_index = 0; pool_index < item_drop_pools.size(); pool_index++)
						{
							const BG_ItemDropPool *pool = cast_to<BG_ItemDropPool>(item_drop_pools[pool_index]);
							if (pool == nullptr || String(pool->id) != String(item_drop_pool_id))
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

					new_job_class->monster_details.append(new_job_monster_details_class);
				}

				const Array resources_lines = Array(entry["resources"]);
				for (int y = 0; y < resources_lines.size(); y++)
				{
					const Dictionary resources_entry = resources_lines[y];

					BG_EventResourceDetails *new_resources_class = memnew(BG_EventResourceDetails);
					new_resources_class->maelstrite_amount_of_drops_during_range = int(resources_entry["maelstrite_amount_of_drops_during_range"]);
					new_resources_class->maelstrite_total_amount_by_end_of_weeks = int(resources_entry["maelstrite_total_amount_by_end_of_weeks"]);

					new_job_class->event_resource_details = new_resources_class;
				}

				const Array distribution_per_act_lines = Array(entry["distribution_per_act"]);
				for (int y = 0; y < distribution_per_act_lines.size(); y++)
				{
					const Dictionary distribution_per_act_entry = distribution_per_act_lines[y];

					BG_JobDistributionForAct *new_job_distribution_for_act_class = memnew(BG_JobDistributionForAct);
					new_job_distribution_for_act_class->act = int(distribution_per_act_entry["act"]);
					new_job_distribution_for_act_class->min_week = int(distribution_per_act_entry["min_week"]);
					new_job_distribution_for_act_class->max_week = int(distribution_per_act_entry["max_week"]);
					new_job_distribution_for_act_class->min_job_spawn_count = int(distribution_per_act_entry["min_job_spawn_count"]);
					new_job_distribution_for_act_class->max_job_spawn_count = int(distribution_per_act_entry["max_job_spawn_count"]);

					new_job_class->distribution_per_act.append(new_job_distribution_for_act_class);
				}

				jobs.append(new_job_class);
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
						TypedArray<BG_UnitStat> rarity_stats;

						const Array stat_lines = Array(stat_entry["stats"]);
						for (int x = 0; x < stat_lines.size(); x++)
						{
							const Dictionary stats_entry = stat_lines[x];

							BG_UnitStat *new_stat = memnew(BG_UnitStat);
							new_stat->id = stats_entry["stat"];
							new_stat->resistant_value_text = stats_entry["resistant_value"];
							new_stat->resistant_value_min_max = BG_UnitStat::string_to_resistant_value_min_max(stats_entry["resistant_value"]);
							new_stat->dice_string = stats_entry["damage_dice"];
							new_stat->dice_options = BG_Dice::string_to_dice_options(new_stat->dice_string);
							rarity_stats.append(new_stat);
						}

						new_item_class->stats[StringName(stat_entry["rarity"])] = rarity_stats;
					}

					// Effects
					const Array effect_lines = Array(entry["effects"]);
					for (int y = 0; y < effect_lines.size(); y++)
					{
						const Dictionary effect_entry = effect_lines[y];
						TypedArray<StringName> effect_ids;

						const Array effects_lines = Array(effect_entry["effects"]);
						for (int x = 0; x < effects_lines.size(); x++)
						{
							const Dictionary effects_entry = effects_lines[x];
							effect_ids.append(StringName(effects_entry["effect"]));
						}
						new_item_class->effects[StringName(effect_entry["rarity"])] = effect_ids;
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
					new_item_class->slot_type = BG_ItemDetails::BEAST_PART;
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
						TypedArray<BG_UnitStat> rarity_stats;

						const Array stat_lines = Array(stat_entry["stats"]);
						for (int x = 0; x < stat_lines.size(); x++)
						{
							const Dictionary stats_entry = stat_lines[x];

							BG_UnitStat *new_stat = memnew(BG_UnitStat);
							new_stat->id = stats_entry["stat"];
							new_stat->resistant_value_text = stats_entry["resistant_value"];
							new_stat->resistant_value_min_max = BG_UnitStat::string_to_resistant_value_min_max(stats_entry["resistant_value"]);
							new_stat->dice_string = stats_entry["damage_dice"];
							new_stat->dice_options = BG_Dice::string_to_dice_options(new_stat->dice_string);
	
							rarity_stats.append(new_stat);
						}
						new_item_class->stats[StringName(stat_entry["rarity"])] = rarity_stats;
					}

					// Effects
					const Array effect_lines = Array(entry["effects"]);
					for (int y = 0; y < effect_lines.size(); y++)
					{
						const Dictionary effect_entry = effect_lines[y];
						TypedArray<StringName> effect_ids;

						const Array effects_lines = Array(effect_entry["effects"]);
						for (int x = 0; x < effects_lines.size(); x++)
						{
							const Dictionary effects_entry = effects_lines[x];
							effect_ids.append(StringName(effects_entry["effect"]));
						}
						new_item_class->effects[StringName(effect_entry["rarity"])] = effect_ids;
					}
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
				if (entry.has("status_icon")) {
					new_effect_class->status_icon_path = entry["status_icon"];
				}

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
	///// Animations
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
					if (equipment_id_entry.has("equipment_id")) {
						new_anim_details_class->equipment_ids.append(equipment_id_entry["equipment_id"]);
					}
				}

				equipment_animation_details.append(new_anim_details_class);
			}
		}
	}

	/////
	///// Objectives
	/////
	{
		const Dictionary objectives_sheet = BG_JsonUtils::GetCBDSheet(data, "objectives");
		if (objectives_sheet.has("lines"))
		{
			objectives.clear();
			const Array lines = Array(objectives_sheet["lines"]);
			for (int i = 0; i < lines.size(); i++)
			{
				const Dictionary entry = lines[i];
				if (bool(entry["disabled"]))
					continue;
				
				BG_ObjectiveDetails *new_objective_class = memnew(BG_ObjectiveDetails);
				new_objective_class->id = entry["id"];
				new_objective_class->localization_key = entry["localization_key"];
				if (new_objective_class->localization_key.is_empty()) {
					new_objective_class->localization_key = new_objective_class->id;
				}
				new_objective_class->is_main_objective = bool(entry["is_main_objective"]);
				new_objective_class->is_scripted = bool(entry["is_scripted"]);
				new_objective_class->objective_repeatable_type = static_cast<BG_ObjectiveDetails::ObjectiveRepeatableType>(int(entry["repeatable_type"]));
				new_objective_class->is_auto_complete = bool(entry["is_auto_complete"]);
				new_objective_class->expires_in = int(entry["expires_in"]);
				new_objective_class->is_turn_in = bool(entry["is_turn_in"]);
				new_objective_class->is_event = bool(entry["is_event"]);
				new_objective_class->script_path = entry["script_path"];

				// Timeline Attributes
				const Array timeline_lines = Array(entry["timeline"]);
				for (int y = 0; y < timeline_lines.size(); y++)
				{
					const Dictionary timeline_entry = timeline_lines[y];
					BG_ObjectiveTimeline *new_objective_timeline_class = memnew(BG_ObjectiveTimeline);
					new_objective_timeline_class->act = int(timeline_entry["act"]);
					new_objective_timeline_class->min_week = int(timeline_entry["min_week"]);
					new_objective_timeline_class->max_week = int(timeline_entry["max_week"]);

					new_objective_class->timeline = new_objective_timeline_class;
					break;
				}

				// Rewards
				const Array reward_lines = Array(entry["rewards"]);
				for (int y = 0; y < reward_lines.size(); y++)
				{
					const Dictionary reward_entry = reward_lines[y];

					new_objective_class->reputation_drop = int(reward_entry["reputation"]);
					new_objective_class->maelstrite_drop = int(reward_entry["maelstrite"]);
					new_objective_class->beast_part_drop_count = int(reward_entry["beast_part_drop_count"]);
					new_objective_class->equipment_drop_count = int(reward_entry["equipment_drop_count"]);

					if (!reward_entry.has("item_drop_pool")) continue;
					const StringName item_drop_pool_id = reward_entry["item_drop_pool"];
					if (item_drop_pool_id.is_empty()) continue;

					for (int pool_index = 0; pool_index < item_drop_pools.size(); pool_index++)
					{
						const BG_ItemDropPool *pool = cast_to<BG_ItemDropPool>(item_drop_pools[pool_index]);
						if (pool == nullptr || String(pool->id) != String(item_drop_pool_id))
							continue;
						
						for (int item_index = 0; item_index < pool->item_drops.size(); item_index++)
						{
							const BG_RewardItem *reward_item = cast_to<BG_RewardItem>(pool->item_drops[item_index]);
							if (reward_item == nullptr) continue;

							BG_RewardItem *new_job_reward_item_class = memnew(BG_RewardItem);
							new_job_reward_item_class->id = reward_item->id;
							new_job_reward_item_class->drop_weight = reward_item->drop_weight;

							const Array forced_rarity_availabilities_lines = Array(reward_entry["forced_rarity_availabilities"]);
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

							new_objective_class->drops.append(new_job_reward_item_class);
						}
						break;
					}
				}

				// Misc Attributes
				const Array misc_attributes_lines = Array(entry["misc_attributes"]);
				for (int y = 0; y < misc_attributes_lines.size(); y++)
				{
					const Dictionary misc_attributes_entry = misc_attributes_lines[y];

					String name = misc_attributes_entry["name"];
					String value1 = misc_attributes_entry["value_1"];
					String value2 = misc_attributes_entry["value_2"];

					Array values;
					if (!value1.is_empty())
						values.append(value1);
					if (!value2.is_empty())
						values.append(value2);

					new_objective_class->misc_attributes[name] = values;
				}

				objectives.append(new_objective_class);
			}
		}
	}
}

void BG_Booker_DB::try_parse_bder_data(const String &file_path)
{
	const Dictionary data = BG_JsonUtils::ParseJsonFile(file_path);
	// UtilityFunctions::prints(data["all_base_stats"]);

	auto get_find_data_by_param_name = [](const String& param_name, const Array& array_to_parse) -> Dictionary {
		for (int i = 0; i < array_to_parse.size(); ++i) {
			const Dictionary entry = array_to_parse[i];
			const String para_name = entry["param_name"];
			if (param_name == para_name) {
				return entry;
			}
		}
		return {};
	};

	auto get_sheet_by_name = [](const String& name, const Dictionary& d) -> Array {
		const Array lines = Array(d["sheets"]);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary sheet = lines[i];
			const String sheet_name = String(sheet["param_name"]);
			if (name != sheet_name) continue;
			return sheet["array_values"];
		}
		return {};
	};

	auto ensure_clean_path = [](const String& path) -> StringName {
		return StringName(path.trim_prefix("/"));
	};

	HashMap<String, TypedArray<StringName>> global_enums;
	{ // Global Enums
		const Array lines = Array(data["global_enums"]);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];
			TypedArray<StringName> enums;
			
			const Array value_line = Array(entry["values"]);
			for (int y = 0; y < value_line.size(); ++y) {
				const StringName v = StringName(value_line[y]);
				enums.append(v);
			}

			global_enums[entry["enum_name"]] = enums;
		}
	}

	{ // Global Curves
		globals->global_curves.clear();

		const Array lines = Array(data["global_curves"]);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];
			const Dictionary curve_data = entry["curve_data"];
			globals->global_curves[StringName(curve_data["param_name"])] = ensure_clean_path(curve_data["path"]);
		}
	}

	{ // Base Stats
		base_stats.clear();

		const Array lines = Array(data["all_base_stats"]);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];
			BG_BaseStat *new_class = memnew(BG_BaseStat);
			new_class->unique_id = int(entry["unique_id"]);
			new_class->is_base_value = bool(entry["is_base_value"]);
			new_class->parent_stat_unique_id = int(entry["parent_stat_id"]);
			new_class->value = float(entry["value"]);
			new_class->stat_id_name = StringName(entry["stat_id_name"]);
			base_stats.append(new_class);
		}

		{ // Set a reference to each of their parent stats.
			for (int i = 0; i < base_stats.size(); ++i) {
				BG_BaseStat *stat = cast_to<BG_BaseStat>(base_stats[i]);
				if (stat == nullptr || stat->parent_stat_unique_id < 0) continue;

				stat->parent_stat_reference = get_stat_from_unique_id(stat->parent_stat_unique_id);
			}
		}
	}

	auto get_animation_detail = [](const HashMap<String, TypedArray<StringName>> &global_enums, const Array &animations, int index) -> BG_AnimationDetails *{
		const Array aa = animations[index];
		
		BG_AnimationDetails *new_class = memnew(BG_AnimationDetails);
		const Dictionary caste_dict = aa[0];
		new_class->caste_or_monster_id = global_enums["caste_types"][int(caste_dict["value"])];

		const Dictionary anim_type_dict = aa[1];
		new_class->anim_type = static_cast<BG_AnimationDetails::AnimationType>(int(anim_type_dict["value"]));

		const Dictionary anim_name_dict = aa[2];
		new_class->anim_name = StringName(anim_name_dict["value"]);

		const Dictionary anim_blend_time_dict = aa[3];
		new_class->anim_blend_time = float(anim_blend_time_dict["value"]);
		
		{ // Traverse Types
			const Dictionary anim_traverse_types_dict = aa[4];
			const Array anim_traverse_types_array = anim_traverse_types_dict["array_values"];
			if (!anim_traverse_types_array.is_empty()) {
				const Array anim_traverse_types = anim_traverse_types_array[0];
				const Dictionary d1 = anim_traverse_types[0];
				new_class->traverse_to = static_cast<BG_AnimationDetails::AnimationTraverseType>(int(d1["value"]));
				const Dictionary d2 = anim_traverse_types[1];
				new_class->traverse_back = static_cast<BG_AnimationDetails::AnimationTraverseType>(int(d2["value"]));
			}
		}

		{ // Tags
			const Dictionary anim_tags_dict = aa[5];
			const Array anim_tags_array = anim_tags_dict["array_values"];
			for (int i = 0; i < anim_tags_array.size(); ++i) {
				const Array anim_tag_entry = anim_tags_array[i];

				const Dictionary tag_dict = anim_tag_entry[0];
				const Dictionary frame_dict = anim_tag_entry[1];

				BG_AnimationTagDetails *new_tag_class = memnew(BG_AnimationTagDetails);
				new_tag_class->tag = StringName(tag_dict["value"]);
				new_tag_class->frame = int(frame_dict["value"]);

				new_class->tags.append(new_tag_class);
			}
		}

		return new_class;
	};

	{ // Stat Types
		stat_types.clear();
		const Array lines = get_sheet_by_name("Stat_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			if (bool(get_find_data_by_param_name("disabled", entry)["value"]))
				continue;
			
			BG_UnitStatDetails *new_stat_types = memnew(BG_UnitStatDetails);
			new_stat_types->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_stat_types->nice_name = StringName(get_find_data_by_param_name("name", entry)["value"]);
			new_stat_types->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", entry)["path"]);
			new_stat_types->is_damage_type = bool(get_find_data_by_param_name("is_damage_type", entry)["value"]);
			StringName weak_to_element = StringName(get_find_data_by_param_name("weak_to_element", entry)["element_id_name_value"]);
			if (!weak_to_element.is_empty())
				new_stat_types->weak_to_element = weak_to_element;
			
			new_stat_types->widget_color = convert_hex_to_color(get_find_data_by_param_name("widget_color", entry)["value"]);
			new_stat_types->text_color = convert_hex_to_color(get_find_data_by_param_name("text_color", entry)["value"]);
			new_stat_types->in_world_color = convert_hex_to_color(get_find_data_by_param_name("in_world_color", entry)["value"]);
			
			stat_types.append(new_stat_types);
		}
	}

	{ // Content
		const Array lines = Array(data["all_content"]);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];
			const StringName content_id = StringName(entry["id"]);

			const Array misc_params = entry["misc_params"];
			const int content_type = int(get_find_data_by_param_name("content_type", misc_params)["value"]);
			
			//
			// Equipment
			//
			if (content_type == 0) {
				// For now, we're going to append this info to an existing entry from the old Booker DB entry. So let's get that old entry.
				BG_ItemDetails *item_details = nullptr;
				for (int x = 0; x < items.size(); ++x) {
					BG_ItemDetails *item_dets = cast_to<BG_ItemDetails>(items[x]);
					if (item_dets->id == content_id) {
						item_details = item_dets;
						break;
					}
				}
				if (item_details == nullptr) {
					UtilityFunctions::print("Could not find data for id : " + String(content_id));
					continue;
				}

				item_details->use_dber_data = true;
				item_details->effectiveness = float(entry["effectiveness"]);
				item_details->use_stat_requirements = bool(entry["use_stat_requirements"]);
				item_details->icon_path = ensure_clean_path(get_find_data_by_param_name("icon", misc_params)["path"]);
				item_details->mesh_path = ensure_clean_path(get_find_data_by_param_name("mesh_scene_path", misc_params)["path"]);
				item_details->animation_attach_socket= int(get_find_data_by_param_name("anim_attach_socket", misc_params)["value"]) + 1;
				const Dictionary level_range = get_find_data_by_param_name("level_range", misc_params);
				item_details->level_range = Vector2(int(level_range["value_x"]), int(level_range["value_y"]));

				{ // Available Castes
					item_details->caste_ids.clear();
					const Array available_castes = Array(get_find_data_by_param_name("available_castes", misc_params)["array_values"]);
					for (int x = 0; x < available_castes.size(); ++x) {
						const Array aa = available_castes[x];
						const Dictionary e = aa[0];
						const StringName caste_id = global_enums["caste_types"][int(e["value"])];
						item_details->caste_ids.append(caste_id);
					}
				}

				{ // Item Animations
					item_details->item_animations.clear();
					const Array item_animations = Array(get_find_data_by_param_name("animations", misc_params)["array_values"]);
					for (int x = 0; x < item_animations.size(); ++x) {
						item_details->item_animations.append(get_animation_detail(global_enums, item_animations, x));
					}
				}

				{ // Item Effectiveness Stats
					const bool randomize_damage_type = bool(entry["randomize_damage_type"]);
					const bool randomize_resistance_type = bool(entry["randomize_resistance_type"]);

					const Array item_effectiveness_stats_lines = Array(entry["item_effectiveness_stats"]);
					for (int x = 0; x < item_effectiveness_stats_lines.size(); ++x) {
						const Dictionary item_effectiveness_stat_entry = item_effectiveness_stats_lines[x];
						
						const float value = float(item_effectiveness_stat_entry["value"]);
						if (value == 0.0) continue; // No need to store empty values.

						BG_ContentStat *new_class = memnew(BG_ContentStat);
						new_class->value = value;

						// Store a reference to its relative stat.
						const int unique_id = int(item_effectiveness_stat_entry["stat_unique_id"]);
						new_class->stat_reference = get_stat_from_unique_id(unique_id);

						new_class->randomize_damage_type = randomize_damage_type;
						new_class->randomize_resistance_type = randomize_resistance_type;

						item_details->item_effectiveness_stats.append(new_class);
					}
				}

				{ // Item Stat Requirements
					if (item_details->use_stat_requirements) {
						const Array item_stat_requirement_lines = Array(entry["item_stat_requirements"]);
						for (int x = 0; x < item_stat_requirement_lines.size(); ++x) {
							const Dictionary item_stat_requirement_entry = item_stat_requirement_lines[x];

							const float value = float(item_stat_requirement_entry["value"]);
							if (value == 0.0) continue; // No need to store empty values.

							BG_ContentStat *new_class = memnew(BG_ContentStat);
							new_class->value = value;

							// Store a reference to its relative stat.
							const int unique_id = int(item_stat_requirement_entry["stat_unique_id"]);
							new_class->stat_reference = get_stat_from_unique_id(unique_id);

							item_details->item_stat_requirements.append(new_class);
						}
					}
				}
			}

			//
			// Monsters
			//
			if (content_type == 1) {
				// For now, we're going to append this info to an existing entry from the old Booker DB entry. So let's get that old entry.
				BG_Monster *monster_details = nullptr;
				for (int x = 0; x < monster_types.size(); ++x) {
					BG_Monster *monster_dets = cast_to<BG_Monster>(monster_types[x]);
					if (monster_dets->id == content_id) {
						monster_details = monster_dets;
						break;
					}
				}
				if (monster_details == nullptr) {
					UtilityFunctions::print("Could not find data for id : " + String(content_id));
					continue;
				}

				monster_details->use_dber_data = true;
				monster_details->effectiveness = float(entry["effectiveness"]);
				monster_details->icon_path = ensure_clean_path(get_find_data_by_param_name("icon", misc_params)["path"]);
				monster_details->model_path = ensure_clean_path(get_find_data_by_param_name("mesh_scene_path", misc_params)["path"]);
				const Dictionary level_range = get_find_data_by_param_name("level_range", misc_params);
				monster_details->level_range = Vector2(int(level_range["value_x"]), int(level_range["value_y"]));
				monster_details->can_be_turned_to_stone = bool(get_find_data_by_param_name("can_be_turned_to_stone", misc_params)["value"]);

				{ // Effectiveness Stats
					const Array effectiveness_stats_lines = Array(entry["effectiveness_stats"]);
					for (int x = 0; x < effectiveness_stats_lines.size(); ++x) {
						const Dictionary effectiveness_stat_entry = effectiveness_stats_lines[x];
						
						const float value = float(effectiveness_stat_entry["value"]);
						if (value == 0.0) continue; // No need to store empty values.

						BG_ContentStat *new_class = memnew(BG_ContentStat);
						new_class->value = value;

						// Store a reference to its relative stat.
						const int unique_id = int(effectiveness_stat_entry["stat_unique_id"]);
						new_class->stat_reference = get_stat_from_unique_id(unique_id);

						monster_details->effectiveness_stats.append(new_class);
					}
				}

				{ // Animations
					monster_details->animations.clear();
					const Array animations = Array(get_find_data_by_param_name("animations", misc_params)["array_values"]);
					for (int x = 0; x < animations.size(); ++x) {
						monster_details->animations.append(get_animation_detail(global_enums, animations, x));
					}
				}
			}
		}

	}

	{ // Audio
		const Array lines = get_sheet_by_name("Audio", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			
			BG_AudioData *new_class = memnew(BG_AudioData);
			new_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			
			const Dictionary entry_audios = get_find_data_by_param_name("audios", entry);
			const Array entry_audios_values = entry_audios["array_values"];
			for (int x = 0; x < entry_audios_values.size(); ++x) {
				const Array audio_details_entry = entry_audios_values[x];

				BG_AudioDataDetails *new_audio_details = memnew(BG_AudioDataDetails);
				new_audio_details->file_path = ensure_clean_path(get_find_data_by_param_name("path", audio_details_entry)["path"]);
				new_audio_details->volume_db_base_value = int(get_find_data_by_param_name("volume_db", audio_details_entry)["value"]);

				const Dictionary restrict_to_cities_dict = get_find_data_by_param_name("restrict_to_cities", audio_details_entry);
				const Array restrict_to_cities = restrict_to_cities_dict["array_values"];
				for (int y = 0; y < restrict_to_cities.size(); ++y) {
					const Array restrict_to_cities_entry = restrict_to_cities[y];
					new_audio_details->restrict_to_acts.append(int(get_find_data_by_param_name("city", restrict_to_cities_entry)["value"]));
				}

				new_class->audio_details.append(new_audio_details);
			}

			audio_data.append(new_class);
		}
	}

	{ // Bands
		const Array lines = get_sheet_by_name("Bands", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			band_info->num_bands_for_hire = int(get_find_data_by_param_name("num_bands_for_hire", entry)["value"]);
			band_info->rest_recovery_speed = float(get_find_data_by_param_name("rest_recovery_speed", entry)["value"]);
			band_info->knocked_out_turns = int(get_find_data_by_param_name("knocked_out_turns", entry)["value"]);
			const Dictionary band_sizes_min_max = get_find_data_by_param_name("band_sizes_min_max", entry);
			band_info->band_size_min_max = Vector2(float(band_sizes_min_max["value_x"]), float(band_sizes_min_max["value_y"]));

			const Dictionary first_names = get_find_data_by_param_name("first_names", entry);
			const Array first_names_array = first_names["array_values"];
			for (int x = 0; x < first_names_array.size(); ++x) {
				const Array first_names_entry = first_names_array[x];
				band_info->first_names.append(StringName(get_find_data_by_param_name("name", first_names_entry)["value"]));
			}
			
			const Dictionary last_names = get_find_data_by_param_name("last_names", entry);
			const Array last_names_array = last_names["array_values"];
			for (int x = 0; x < last_names_array.size(); ++x) {
				const Array last_names_entry = last_names_array[x];
				band_info->last_names.append(StringName(get_find_data_by_param_name("name", last_names_entry)["value"]));
			}

			const Dictionary bands = get_find_data_by_param_name("bands", entry);
			const Array bands_array = bands["array_values"];
			for (int x = 0; x < bands_array.size(); ++x) {
				const Array bands_entry = bands_array[x];

				BG_BandNameInfo *new_band_name_info = memnew(BG_BandNameInfo);
				new_band_name_info->band_name = StringName(get_find_data_by_param_name("name", bands_entry)["value"]);
				band_info->band_names.append(new_band_name_info);

				band_info->icon_paths.append(ensure_clean_path(get_find_data_by_param_name("icon_path", bands_entry)["path"]));
			}
		}
	}

	{ // Battle Boards
		const Array lines = get_sheet_by_name("Battle_Boards", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			
			BG_BattleBoardDetails *new_class = memnew(BG_BattleBoardDetails);
			new_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_class->unique_save_name = StringName(get_find_data_by_param_name("unique_save_name", entry)["value"]);
			new_class->board_path = ensure_clean_path(get_find_data_by_param_name("board_path", entry)["path"]);
			new_class->default_hex_visual_path = ensure_clean_path(get_find_data_by_param_name("default_hex_visual_path", entry)["path"]);

			// Hex Types
			const Dictionary hex_types_values = get_find_data_by_param_name("hex_types", entry);
			const Array hex_types_values_array = hex_types_values["array_values"];
			for (int x = 0; x < hex_types_values_array.size(); ++x) {
				const Array hex_types_entry = hex_types_values_array[x];

				BG_BattleBoard_HexTypeDetails *new_hex_type_class = memnew(BG_BattleBoard_HexTypeDetails);
				new_hex_type_class->id = StringName(get_find_data_by_param_name("id", hex_types_entry)["value"]);
				new_hex_type_class->hex_type = int(get_find_data_by_param_name("type", hex_types_entry)["value"]);
				new_hex_type_class->destroyed_vfx_scene_path = ensure_clean_path(get_find_data_by_param_name("destroyed_vfx_scene", hex_types_entry)["path"]);
				new_hex_type_class->destroyed_sfx_id = StringName(get_find_data_by_param_name("destroyed_sfx", hex_types_entry)["element_id_name_value"]);
				new_hex_type_class->hex_visual_scene_path_override = ensure_clean_path(get_find_data_by_param_name("hex_visual_file_path_override", hex_types_entry)["path"]);
				new_hex_type_class->health_effectiveness = float(get_find_data_by_param_name("health_effectiveness", hex_types_entry)["value"]);

				// Visuals
				const Dictionary visuals_values = get_find_data_by_param_name("visuals", hex_types_entry);
				const Array visuals_values_array = visuals_values["array_values"];
				for (int y = 0; y < visuals_values_array.size(); ++y) {
					const Array visuals_entry = visuals_values_array[y];

					BG_BattleBoard_HexTypeVisualDetails *new_hex_type_visuals_class = memnew(BG_BattleBoard_HexTypeVisualDetails);
					new_hex_type_visuals_class->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", visuals_entry)["path"]);
					new_hex_type_visuals_class->two_der_id = StringName(get_find_data_by_param_name("two_der_id", visuals_entry)["element_id_name_value"]);
					new_hex_type_visuals_class->scene_path = ensure_clean_path(get_find_data_by_param_name("scene_path", visuals_entry)["path"]);
					new_hex_type_visuals_class->destroyed_icon_path = ensure_clean_path(get_find_data_by_param_name("destroyed_icon_path", visuals_entry)["path"]);
					new_hex_type_visuals_class->destroyed_two_der_id = StringName(get_find_data_by_param_name("destroyed_two_der_id", visuals_entry)["element_id_name_value"]);
					new_hex_type_visuals_class->destroyed_scene_path = ensure_clean_path(get_find_data_by_param_name("destroyed_scene_path", visuals_entry)["path"]);
					new_hex_type_visuals_class->tint = convert_hex_to_color(get_find_data_by_param_name("tint", visuals_entry)["value"]);

					new_hex_type_class->visuals.append(new_hex_type_visuals_class);
				}

				// Equipment
				const Dictionary equipment_values = get_find_data_by_param_name("equipment", hex_types_entry);
				const Array equipment_values_array = equipment_values["array_values"];
				for (int y = 0; y < equipment_values_array.size(); ++y) {
					const Array equipment_entry = equipment_values_array[y];
					new_hex_type_class->equipment_ids.append(StringName(get_find_data_by_param_name("equipment", equipment_entry)["element_id_name_value"]));
				}

				new_class->hex_types.append(new_hex_type_class);
			}

			battle_boards_details.append(new_class);
		}
	}

	{ // Booker Skill Tree
		booker_skill_tree_details.clear();
		const Array lines = get_sheet_by_name("Booker_Skill_Tree", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			if (bool(get_find_data_by_param_name("disabled", entry)["value"]))
				continue;
			
			BG_BookerSkillTreeSlotDetails *new_booker_skill_tree_slot_details_class = memnew(BG_BookerSkillTreeSlotDetails);
			new_booker_skill_tree_slot_details_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_booker_skill_tree_slot_details_class->is_parent_button = bool(get_find_data_by_param_name("is_parent_btn", entry)["value"]);
			new_booker_skill_tree_slot_details_class->skill_name = StringName(get_find_data_by_param_name("skill_name", entry)["value"]);
			new_booker_skill_tree_slot_details_class->skill_description = StringName(get_find_data_by_param_name("skill_description", entry)["value"]);
			new_booker_skill_tree_slot_details_class->required_rep = int(get_find_data_by_param_name("required_rep", entry)["value"]);
			StringName parent_skill = StringName(get_find_data_by_param_name("parent_skill", entry)["element_id_name_value"]);
			if (!parent_skill.is_empty())
				new_booker_skill_tree_slot_details_class->parent_skill_id = parent_skill;
			
			// Value Attributes
			const Dictionary value_attribute_values = get_find_data_by_param_name("value_attributes", entry);
			const Array value_attribute_values_array = value_attribute_values["array_values"];
			for (int x = 0; x < value_attribute_values_array.size(); ++x) {
				const Array value_attribute_values_entry = value_attribute_values_array[x];

				const float value1 = float(get_find_data_by_param_name("value_1", value_attribute_values_entry)["value"]);
				const float value2 = float(get_find_data_by_param_name("value_2", value_attribute_values_entry)["value"]);
				new_booker_skill_tree_slot_details_class->value_attributes.append(value1);
				new_booker_skill_tree_slot_details_class->value_attributes.append(value2);
			}

			booker_skill_tree_details.append(new_booker_skill_tree_slot_details_class);
		}
	}

	{ // Caste Types
		band_info->unit_castes.clear();
		const Array lines = get_sheet_by_name("Caste_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			if (bool(get_find_data_by_param_name("disabled", entry)["value"]))
				continue;

			BG_UnitCaste *new_unit_caste = memnew(BG_UnitCaste);
			new_unit_caste->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_unit_caste->name = StringName(get_find_data_by_param_name("name", entry)["value"]);
			new_unit_caste->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", entry)["path"]);
			
			// Hue Shifting
			const Dictionary hue_shifting_values = get_find_data_by_param_name("hue_shifting", entry);
			const Array hue_shifting_values_array = hue_shifting_values["array_values"];
			for (int x = 0; x < hue_shifting_values_array.size(); ++x) {
				const Array hue_shifting_values_entry = hue_shifting_values_array[x];

				BG_HueShiftData *new_hue_shift_data = memnew(BG_HueShiftData);
				new_hue_shift_data->mask_path = ensure_clean_path(get_find_data_by_param_name("mask_path", hue_shifting_values_entry)["path"]);
				new_hue_shift_data->from_color = convert_hex_to_color(get_find_data_by_param_name("from_color", hue_shifting_values_entry)["value"]);
				new_hue_shift_data->multiplier = float(get_find_data_by_param_name("multiplier", hue_shifting_values_entry)["value"]);
				new_unit_caste->hue_shift_data = new_hue_shift_data;
			}

			// LOD Mesh Paths
			new_unit_caste->lod_mesh_paths.clear();
			const Dictionary lod_mesh_paths_values = get_find_data_by_param_name("lod_mesh_paths", entry);
			const Array lod_mesh_paths_values_array = lod_mesh_paths_values["array_values"];
			for (int x = 0; x < lod_mesh_paths_values_array.size(); ++x) {
				const Array lod_mesh_paths_values_entry = lod_mesh_paths_values_array[x];
				new_unit_caste->lod_mesh_paths.append(ensure_clean_path(get_find_data_by_param_name("path", lod_mesh_paths_values_entry)["path"]));
			}

			// Scale Limits
			const Dictionary scale_limits_values = get_find_data_by_param_name("scale_limits", entry);
			const Array scale_limits_values_array = scale_limits_values["array_values"];
			for (int x = 0; x < scale_limits_values_array.size(); ++x) {
				const Array scale_limits_values_entry = scale_limits_values_array[x];

				new_unit_caste->scale_min = Vector2(
					float(get_find_data_by_param_name("min_x", scale_limits_values_entry)["value"]),
					float(get_find_data_by_param_name("min_y", scale_limits_values_entry)["value"])
				);
				new_unit_caste->scale_max = Vector2(
					float(get_find_data_by_param_name("max_x", scale_limits_values_entry)["value"]),
					float(get_find_data_by_param_name("max_y", scale_limits_values_entry)["value"])
				);
				new_unit_caste->scale_min_extreme = Vector2(
					float(get_find_data_by_param_name("extreme_min_x", scale_limits_values_entry)["value"]),
					float(get_find_data_by_param_name("extreme_min_y", scale_limits_values_entry)["value"])
				);
				new_unit_caste->scale_max_extreme = Vector2(
					float(get_find_data_by_param_name("extreme_max_x", scale_limits_values_entry)["value"]),
					float(get_find_data_by_param_name("extreme_max_y", scale_limits_values_entry)["value"])
				);
			}

			// Misc Stats
			const Dictionary misc_stats_values = get_find_data_by_param_name("misc_stats", entry);
			const Array misc_stats_values_array = misc_stats_values["array_values"];
			for (int x = 0; x < misc_stats_values_array.size(); ++x) {
				const Array misc_stats_values_entry = misc_stats_values_array[x];
				new_unit_caste->travel_distance = int(get_find_data_by_param_name("travel_distance", misc_stats_values_entry)["value"]);
			}

			// Base Damage Type Stats
			const Dictionary base_damage_type_stats_values = get_find_data_by_param_name("base_damage_type_stats", entry);
			const Array base_damage_type_stats_values_array = base_damage_type_stats_values["array_values"];
			for (int x = 0; x < base_damage_type_stats_values_array.size(); ++x) {
				const Array base_damage_type_stats_values_entry = base_damage_type_stats_values_array[x];

				BG_UnitStat *new_stat = memnew(BG_UnitStat);
				new_stat->id = StringName(get_find_data_by_param_name("damage_type", base_damage_type_stats_values_entry)["element_id_name_value"]);
				new_stat->bonus_percentage = float(get_find_data_by_param_name("base_bonus_percentage", base_damage_type_stats_values_entry)["value"]);
				new_stat->defensive_value = int(get_find_data_by_param_name("starting_value", base_damage_type_stats_values_entry)["value"]);

				new_unit_caste->stats.append(new_stat);
			}

			// Random Starting Items
			const Dictionary random_starting_items_values = get_find_data_by_param_name("random_starting_items", entry);
			const Array random_starting_items_values_array = random_starting_items_values["array_values"];
			for (int x = 0; x < random_starting_items_values_array.size(); ++x) {
				const Array random_starting_items_values_entry = random_starting_items_values_array[x];
				new_unit_caste->starting_item_ids.append(
					StringName(get_find_data_by_param_name("item", random_starting_items_values_entry)["element_id_name_value"])
				);
			}

			// Element Availability
			const Dictionary element_availability_values = get_find_data_by_param_name("element_availability", entry);
			const Array element_availability_values_array = element_availability_values["array_values"];
			for (int x = 0; x < element_availability_values_array.size(); ++x) {
				const Array element_availability_values_entry = element_availability_values_array[x];
				new_unit_caste->element_availability_ids.append(
					StringName(get_find_data_by_param_name("element", element_availability_values_entry)["element_id_name_value"])
				);
			}

			band_info->unit_castes.append(new_unit_caste);
		}
	}

	{ // Challenge Rating Guide

		globals->challenge_rating_guide.clear();
		const Array lines = get_sheet_by_name("Audio", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			BG_ChallengeRatingGuide *challenge_rating_guide_class = memnew(BG_ChallengeRatingGuide);
			challenge_rating_guide_class->cr_min_max = Vector2(
				float(get_find_data_by_param_name("cr_min", entry)["value"]),
				float(get_find_data_by_param_name("cr_max", entry)["value"])
			);
			challenge_rating_guide_class->job_rep_reward = int(get_find_data_by_param_name("job_rep_reward", entry)["value"]);
			challenge_rating_guide_class->job_duralation = int(get_find_data_by_param_name("job_duralation", entry)["value"]);
			challenge_rating_guide_class->item_durability_consumption = float(get_find_data_by_param_name("item_durability_consumption", entry)["value"]);
			challenge_rating_guide_class->item_fame_addition = float(get_find_data_by_param_name("item_fame_addition", entry)["value"]);
			globals->challenge_rating_guide.append(challenge_rating_guide_class);
		}
	}

	{ // City Info
		globals->city_info.clear();
		const Array lines = get_sheet_by_name("City_Info", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			BG_CityInfo *new_city_info = memnew(BG_CityInfo);
			new_city_info->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_city_info->nice_name = StringName(get_find_data_by_param_name("name", entry)["value"]);
			new_city_info->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", entry)["path"]);
			new_city_info->scene_path = ensure_clean_path(get_find_data_by_param_name("scene_path", entry)["path"]);
			new_city_info->battle_board_id = StringName(get_find_data_by_param_name("battle_board_id", entry)["element_id_name_value"]);
			new_city_info->max_health = int(get_find_data_by_param_name("health", entry)["value"]);

			// Equipment
			const Dictionary equipment_values = get_find_data_by_param_name("equipment", entry);
			const Array equipment_values_array = equipment_values["array_values"];
			for (int x = 0; x < equipment_values_array.size(); ++x) {
				const Array equipment_values_entry = equipment_values_array[x];
				new_city_info->equipment_ids.append(StringName(get_find_data_by_param_name("equipment", equipment_values_entry)["element_id_name_value"]));
			}

			// Barracades
			const Dictionary barracades_values = get_find_data_by_param_name("barracades", entry);
			const Array barracades_values_array = barracades_values["array_values"];
			for (int x = 0; x < barracades_values_array.size(); ++x) {
				const Array barracades_values_entry = barracades_values_array[x];

				BG_TurretInfo *new_turret_info = memnew(BG_TurretInfo);
				new_turret_info->nice_name = StringName(get_find_data_by_param_name("name", barracades_values_entry)["value"]);
				new_turret_info->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", barracades_values_entry)["path"]);
				new_turret_info->destroyed_icon_path = ensure_clean_path(get_find_data_by_param_name("destroyed_icon_path", barracades_values_entry)["path"]);
				new_turret_info->max_health = int(get_find_data_by_param_name("health", barracades_values_entry)["value"]);
				new_turret_info->destroyed_vfx_path = ensure_clean_path(get_find_data_by_param_name("vfx", barracades_values_entry)["path"]);
				new_turret_info->destroyed_sfx_id = StringName(get_find_data_by_param_name("sfx", barracades_values_entry)["element_id_name_value"]);

				// 2der Icons
				const Dictionary twoder_icons_values = get_find_data_by_param_name("2der_icons", barracades_values_entry);
				const Array twoder_icons_values_array = twoder_icons_values["array_values"];
				for (int y = 0; y < twoder_icons_values_array.size(); ++y) {
					const Array twoder_icons_values_entry = twoder_icons_values_array[y];
					new_turret_info->twoder_icons.append(StringName(get_find_data_by_param_name("reference", twoder_icons_values_entry)["element_id_name_value"]));
				}

				// 2der Destroyed Icons
				const Dictionary twoder_destroyed_icons_values = get_find_data_by_param_name("2der_destroyed_icons", barracades_values_entry);
				const Array twoder_destroyed_icons_values_array = twoder_destroyed_icons_values["array_values"];
				for (int y = 0; y < twoder_destroyed_icons_values_array.size(); ++y) {
					const Array twoder_destroyed_icons_values_entry = twoder_destroyed_icons_values_array[y];
					new_turret_info->twoder_destroyed_icons.append(StringName(get_find_data_by_param_name("reference", twoder_destroyed_icons_values_entry)["element_id_name_value"]));
				}

				// Equipment
				const Dictionary barracades_equipment_values = get_find_data_by_param_name("equipment", barracades_values_entry);
				const Array barracades_equipment_values_array = barracades_equipment_values["array_values"];
				for (int y = 0; y < barracades_equipment_values_array.size(); ++y) {
					const Array barracades_equipment_values_entry = barracades_equipment_values_array[y];
					new_turret_info->equipment_ids.append(StringName(get_find_data_by_param_name("equipment", barracades_equipment_values_entry)["element_id_name_value"]));
				}

				const int turret_type = int(get_find_data_by_param_name("type", barracades_values_entry)["value"]);
				switch (turret_type) {
					case 0:
						new_city_info->barracades.append(new_turret_info);
						break;
					case 1:
						new_city_info->turrets.append(new_turret_info);
						break;
					case 2:
						new_city_info->towns.append(new_turret_info);
						break;
				}
			}

			// Misc Attributes
			const Dictionary misc_attributes_values = get_find_data_by_param_name("misc_attributes", entry);
			const Array misc_attributes_values_array = misc_attributes_values["array_values"];
			for (int x = 0; x < misc_attributes_values_array.size(); ++x) {
				const Array misc_attributes_values_entry = misc_attributes_values_array[x];

				const String name = String(get_find_data_by_param_name("name", misc_attributes_values_entry)["value"]);
				const String value1 = String(get_find_data_by_param_name("value_1", misc_attributes_values_entry)["value"]);
				const String value2 = String(get_find_data_by_param_name("value_2", misc_attributes_values_entry)["value"]);

				Array values;
				if (!value1.is_empty())
					values.append(value1);
				if (!value2.is_empty())
					values.append(value2);

				new_city_info->misc_attributes[name] = values;
			}

			globals->city_info.append(new_city_info);
		}
	}

	{ // Consumables
		const Array lines = get_sheet_by_name("Consumables", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			if (bool(get_find_data_by_param_name("disabled", entry)["value"]))
				continue;
			
			BG_ItemDetails *new_item_class = memnew(BG_ItemDetails);
			new_item_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_item_class->name = StringName(get_find_data_by_param_name("name", entry)["value"]);
			new_item_class->slot_type_id = StringName(get_find_data_by_param_name("slot_type", entry)["element_id_name_value"]);
			new_item_class->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", entry)["path"]);
			new_item_class->slot_type = BG_ItemDetails::CONSUMABLE;

			// Effects
			const Dictionary effect_values = get_find_data_by_param_name("effects", entry);
			const Array effect_values_array = effect_values["array_values"];
			for (int x = 0; x < effect_values_array.size(); ++x) {
				const Array effect_values_entry = effect_values_array[x];

				TypedArray<StringName> effect_ids;

				const Dictionary effects_values = get_find_data_by_param_name("effects", effect_values_entry);
				const Array effects_values_array = effects_values["array_values"];
				for (int y = 0; y < effects_values_array.size(); ++y) {
					const Array effects_values_entry = effects_values_array[y];
					effect_ids.append(StringName(get_find_data_by_param_name("effect", effects_values_entry)["element_id_name_value"]));
				}
				
				new_item_class->effects[StringName(get_find_data_by_param_name("rarity", effect_values_entry)["element_id_name_value"])] = effect_ids;
			}

			items.append(new_item_class);
		}
	}

	{ // Item Slot Types
		item_slot_types.clear();
		const Array lines = get_sheet_by_name("Item_Slot_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			BG_ItemSlotType *new_item_slot_type_class = memnew(BG_ItemSlotType);
			new_item_slot_type_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_item_slot_type_class->name = StringName(get_find_data_by_param_name("name", entry)["value"]);
			new_item_slot_type_class->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", entry)["path"]);

			// Percentage of all items dropped per act.
			const Dictionary percentage_of_all_items_dropped_per_act_values = get_find_data_by_param_name("percentage_of_all_items_dropped_per_act", entry);
			const Array percentage_of_all_items_dropped_per_act_values_array = percentage_of_all_items_dropped_per_act_values["array_values"];
			for (int x = 0; x < percentage_of_all_items_dropped_per_act_values_array.size(); ++x) {
				const Array percentage_of_all_items_dropped_per_act_values_entry = percentage_of_all_items_dropped_per_act_values_array[x];
				new_item_slot_type_class->percentage_of_all_items_dropped_per_act.append(
					float(get_find_data_by_param_name("percent", percentage_of_all_items_dropped_per_act_values_entry)["value"])
				);
			}

			item_slot_types.append(new_item_slot_type_class);
		}
	}

	{ // Mail Data
		mail_data.clear();
		const Array lines = get_sheet_by_name("Item_Slot_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			BG_MailData *new_mail_data = memnew(BG_MailData);
			new_mail_data->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_mail_data->act = int(get_find_data_by_param_name("act", entry)["value"]);
			new_mail_data->week = int(get_find_data_by_param_name("week", entry)["value"]);

			mail_data.append(new_mail_data);
		}
	}

	{ // Misc Globals
		const Array lines = get_sheet_by_name("Misc_Globals", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			globals->starting_reputation = int(get_find_data_by_param_name("starting_reputation", entry)["value"]);
			globals->inventory_sell_multiplier = float(get_find_data_by_param_name("inventory_sell_multiplier", entry)["value"]);
			globals->item_passive_income_multiplier = float(get_find_data_by_param_name("item_passive_income_multiplier", entry)["value"]);
			globals->chance_of_no_drop = float(get_find_data_by_param_name("chance_of_no_drop", entry)["value"]);

			// Weeks per act & Job globals.
			const Dictionary weeks_per_act = get_find_data_by_param_name("weeks_per_act", entry);
			const Array weeks_per_act_array = weeks_per_act["array_values"];
			const Dictionary job_globals = get_find_data_by_param_name("job_globals", entry);
			const Array job_globals_array = job_globals["array_values"];
			for (int x = 0; x < weeks_per_act_array.size(); ++x) {
				const Array weeks_per_act_entry = weeks_per_act_array[x];
				const Array job_globals_entry = job_globals_array[x];

				BG_ActStats *new_act_stats = memnew(BG_ActStats);
				new_act_stats->total_week_count = int(get_find_data_by_param_name("weeks_count", weeks_per_act_entry)["value"]);

				if (globals->combat_rounds_per_combat == 0)
					globals->combat_rounds_per_combat = int(get_find_data_by_param_name("combat_rounds_per_combat", job_globals_entry)["value"]);

				const Dictionary jobs_distribution_per_act = get_find_data_by_param_name("jobs_distribution_per_act", job_globals_entry);
				const Array jobs_distribution_per_act_array = jobs_distribution_per_act["array_values"];
				for (int y = 0; y < jobs_distribution_per_act_array.size(); ++y) {
					const Array jobs_distribution_per_act_entry = jobs_distribution_per_act_array[y];

					new_act_stats->total_job_count = int(get_find_data_by_param_name("job_count", jobs_distribution_per_act_entry)["value"]);
					new_act_stats->job_handout_curve_path = StringName(get_find_data_by_param_name("curve", jobs_distribution_per_act_entry)["path"]);
				}
				globals->act_stats.append(new_act_stats);
			}

			// QTE Values
			const Dictionary qte_values = get_find_data_by_param_name("qte_values", entry);
			const Array qte_values_array = qte_values["array_values"];
			for (int x = 0; x < qte_values_array.size(); ++x) {
				const Array qte_values_entry = qte_values_array[x];

				globals->hit_perfect_timing_multiplier = float(get_find_data_by_param_name("hit_perfect_timing_multiplier", qte_values_entry)["value"]);
				globals->hit_good_timing_multiplier = float(get_find_data_by_param_name("hit_good_timing_multiplier", qte_values_entry)["value"]);
				globals->parry_perfect_timing_multiplier = float(get_find_data_by_param_name("parry_perfect_timing_multiplier", qte_values_entry)["value"]);
				globals->parry_good_timing_multiplier = float(get_find_data_by_param_name("parry_good_timing_multiplier", qte_values_entry)["value"]);
			}

			// Monster Element Distribution
			const Dictionary monster_element_distribution = get_find_data_by_param_name("monster_element_distribution", entry);
			const Array monster_element_distribution_array = monster_element_distribution["array_values"];
			for (int x = 0; x < monster_element_distribution_array.size(); ++x) {
				const Array monster_element_distribution_entry = monster_element_distribution_array[x];

				globals->monster_element_distribution.append(float(get_find_data_by_param_name("percentage", monster_element_distribution_entry)["value"]));
			}

			// Max Resistance
			const Dictionary max_resistance = get_find_data_by_param_name("max_resistance", entry);
			const Array max_resistance_array = max_resistance["array_values"];
			for (int x = 0; x < max_resistance_array.size(); ++x) {
				const Array max_resistance_entry = max_resistance_array[x];

				globals->max_resistance_soft_cap = int(get_find_data_by_param_name("max_resistance_soft_cap", max_resistance_entry)["value"]);
				globals->max_resistance_hard_cap = int(get_find_data_by_param_name("max_resistance_hard_cap", max_resistance_entry)["value"]);
			}

			// Damage Resistance Modifiers
			const Dictionary damage_resistance_modifiers = get_find_data_by_param_name("damage_resistance_modifiers", entry);
			const Array damage_resistance_modifiers_array = damage_resistance_modifiers["array_values"];
			for (int x = 0; x < damage_resistance_modifiers_array.size(); ++x) {
				const Array damage_resistance_modifiers_entry = damage_resistance_modifiers_array[x];

				globals->percent_amount_to_add_on_same_element_per_damage_value = float(get_find_data_by_param_name("percent_amount_to_add_on_same_element_per_damage_value", damage_resistance_modifiers_entry)["value"]);
				globals->percent_amount_to_subtract_on_weak_element_per_damage_value = float(get_find_data_by_param_name("percent_amount_to_subtract_on_weak_element_per_damage_value", damage_resistance_modifiers_entry)["value"]);
			}

			// Item Teir Values
			const Dictionary item_teir_values = get_find_data_by_param_name("item_teir_values", entry);
			const Array item_teir_values_array = item_teir_values["array_values"];
			for (int x = 0; x < item_teir_values_array.size(); ++x) {
				const Array item_teir_values_entry = item_teir_values_array[x];

				globals->sell_tier_values.append(int(get_find_data_by_param_name("sell_tier_values", item_teir_values_entry)["value"]));
				globals->fame_tier_values.append(int(get_find_data_by_param_name("fame_tier_values", item_teir_values_entry)["value"]));
				globals->durability_teir_values.append(int(get_find_data_by_param_name("durability_teir_values", item_teir_values_entry)["value"]));
			}

			// Item Teir Rarity Multipliers
			const Dictionary item_teir_rarity_multipliers = get_find_data_by_param_name("item_teir_rarity_multipliers", entry);
			const Array item_teir_rarity_multipliers_array = item_teir_rarity_multipliers["array_values"];
			for (int x = 0; x < item_teir_rarity_multipliers_array.size(); ++x) {
				const Array item_teir_rarity_multipliers_entry = item_teir_rarity_multipliers_array[x];

				globals->equipment_value_rarity_multiplier.append(float(get_find_data_by_param_name("equipment_value_rarity_multiplier", item_teir_rarity_multipliers_entry)["value"]));
				globals->beast_part_value_rarity_multiplier.append(float(get_find_data_by_param_name("beast_part_value_rarity_multiplier", item_teir_rarity_multipliers_entry)["value"]));
				globals->extra_beast_part_value_rarity_multiplier_while_grafted.append(float(get_find_data_by_param_name("extra_beast_part_value_rarity_multiplier_while_grafted", item_teir_rarity_multipliers_entry)["value"]));
				globals->equipment_durability_rarity_multiplier.append(float(get_find_data_by_param_name("equipment_durability_rarity_multiplier", item_teir_rarity_multipliers_entry)["value"]));
				globals->beast_part_durability_rarity_multiplier.append(float(get_find_data_by_param_name("beast_part_durability_rarity_multiplier", item_teir_rarity_multipliers_entry)["value"]));
				globals->equipment_fame_rarity_multiplier.append(float(get_find_data_by_param_name("equipment_fame_rarity_multiplier", item_teir_rarity_multipliers_entry)["value"]));
				globals->beast_part_fame_rarity_multiplier.append(float(get_find_data_by_param_name("beast_part_fame_rarity_multiplier", item_teir_rarity_multipliers_entry)["value"]));
			}

			// Day Night Cycle
			const Dictionary day_night_cycle = get_find_data_by_param_name("day_night_cycle", entry);
			const Array day_night_cycle_array = day_night_cycle["array_values"];
			for (int x = 0; x < day_night_cycle_array.size(); ++x) {
				const Array day_night_cycle_entry = day_night_cycle_array[x];

				globals->day_night_cycle_turns_per_day = int(get_find_data_by_param_name("turns_per_day", day_night_cycle_entry)["value"]);
				// UtilityFunctions::print(globals->day_night_cycle_turns_per_day);
				globals->percentage_of_day_is_day_time = float(get_find_data_by_param_name("percentage_of_day_is_day_time", day_night_cycle_entry)["value"]);

				const Dictionary day_cycle_offset_per_act = get_find_data_by_param_name("day_cycle_offset_per_act", day_night_cycle_entry);
				const Array day_cycle_offset_per_act_array = day_cycle_offset_per_act["array_values"];
				for (int y = 0; y < day_cycle_offset_per_act_array.size(); ++y) {
					const Array day_cycle_offset_per_act_entry = day_cycle_offset_per_act_array[y];

					globals->day_cycle_offset_per_act.append(int(get_find_data_by_param_name("offset", day_cycle_offset_per_act_entry)["value"]));
				}
			}
		}
	}

	{ // Rarity Type
		rarity_types.clear();
		const Array lines = get_sheet_by_name("Rarity_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			BG_RarityDetails *new_rarity_type = memnew(BG_RarityDetails);
			new_rarity_type->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_rarity_type->equipment_prefix_name = StringName(get_find_data_by_param_name("equipment_prefix_name", entry)["value"]);
			new_rarity_type->beast_part_prefix_name = StringName(get_find_data_by_param_name("beast_part_prefix_name", entry)["value"]);
			new_rarity_type->color = convert_hex_to_color(get_find_data_by_param_name("color", entry)["value"]);
			const float color_muitiplier = float(get_find_data_by_param_name("color_muitiplier", entry)["value"]);
			new_rarity_type->color.r *= color_muitiplier;
			new_rarity_type->color.g *= color_muitiplier;
			new_rarity_type->color.b *= color_muitiplier;
			new_rarity_type->damage_multiplier = float(get_find_data_by_param_name("damage_multiplier", entry)["value"]);

			// Percentage of all items dropped per act
			const Dictionary percentage_of_all_items_dropped_per_act_values = get_find_data_by_param_name("percentage_of_all_items_dropped_per_act", entry);
			const Array percentage_of_all_items_dropped_per_act_values_array = percentage_of_all_items_dropped_per_act_values["array_values"];
			for (int x = 0; x < percentage_of_all_items_dropped_per_act_values_array.size(); ++x) {
				const Array percentage_of_all_items_dropped_per_act_values_entry = percentage_of_all_items_dropped_per_act_values_array[x];
				new_rarity_type->percentage_of_all_items_dropped_per_act.append(float(get_find_data_by_param_name("percent", percentage_of_all_items_dropped_per_act_values_entry)["value"]));
			}

			rarity_types.append(new_rarity_type);
		}
	}

	{ // Resource Types
		resource_type_details.clear();
		const Array lines = get_sheet_by_name("Resource_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];

			BG_ResourceTypeDetails *new_resource_type_class = memnew(BG_ResourceTypeDetails);
			new_resource_type_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_resource_type_class->icon_path = ensure_clean_path(get_find_data_by_param_name("icon_path", entry)["path"]);

			resource_type_details[new_resource_type_class->id] = new_resource_type_class;
		}
	}

	{ // 2der
		two_der_data_entries.clear();
		const Array lines = get_sheet_by_name("Two_Der", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Array entry = lines[i];
			BG_TwoDer_DataEntry *new_twoder_class = memnew(BG_TwoDer_DataEntry);

			new_twoder_class->id = StringName(get_find_data_by_param_name("id", entry)["value"]);
			new_twoder_class->data_path = ensure_clean_path(get_find_data_by_param_name("data_path", entry)["path"]);
			new_twoder_class->asset_name = StringName(get_find_data_by_param_name("asset_name", entry)["value"]);
			new_twoder_class->prop_name = StringName(get_find_data_by_param_name("prop_name", entry)["value"]);
			new_twoder_class->anim_name = StringName(get_find_data_by_param_name("anim_name", entry)["value"]);
			new_twoder_class->camera_name = StringName(get_find_data_by_param_name("camera_name", entry)["value"]);
			new_twoder_class->anim_speed_mult = float(get_find_data_by_param_name("anim_speed_mult", entry)["value"]);
			new_twoder_class->scale_multiplier = float(get_find_data_by_param_name("scale_multiplier", entry)["value"]);

			two_der_data_entries.append(new_twoder_class);
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

TypedArray<BG_LocalizeEntryData> BG_Booker_DB::get_localize_data(const StringName sheet_name, const StringName key, const StringName language)
{
	// Check if we already cached it.
	if (localize_data.has(sheet_name)) {
		if (localize_data[sheet_name].has(key)) {
			return localize_data[sheet_name][key];
		}
	}
	else {
		localize_data[sheet_name] = HashMap<StringName, TypedArray<BG_LocalizeEntryData>>();
	}

	// Open and read the file.
	const String key_as_string = String(key);
	ERR_FAIL_COND_V_EDMSG(key_as_string == "", {}, "ERROR : Localization key is \"\", sheet: " + sheet_name);

	const String actual_sheet_name = "BookerText - " + String(sheet_name) + ".tsv";
	const String file_path = "res://Localization/" + actual_sheet_name;
	const Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	ERR_FAIL_COND_V_EDMSG(file.is_null(), {}, "ERROR : Could not open localization file : " + file_path);

	
	// Lambda for creating a new entry.
    auto create_new_entry = [](const PackedStringArray &sd, const uint8_t lang_index) -> BG_LocalizeEntryData * {
		BG_LocalizeEntryData *new_localization_class = memnew(BG_LocalizeEntryData);
		new_localization_class->code_start = sd[1];
		new_localization_class->code_end = sd[2];
		new_localization_class->text = sd[lang_index];
		return new_localization_class;
    };
	
	// Go through line by line.
	const String language_as_string = String(language);
	uint8_t language_index = 0;
	uint8_t number_of_columns = 0;
	TypedArray<BG_LocalizeEntryData> result = TypedArray<BG_LocalizeEntryData>();
	bool found_key = false;
	bool is_first_line = true;
	while (!file->eof_reached()) {
		const String line = file->get_line();
		if (found_key && !line.contains("\t")) {
			BG_LocalizeEntryData *d = cast_to<BG_LocalizeEntryData>(result[result.size() - 1]);
			if (line == "\"")
				d->text += line.replace("\"", "\n\n");
			else
				d->text += "\n" + line.replace("\"", "");
			continue;
		}
		const PackedStringArray line_data = line.replace("\"", "").split("\t");

		if (is_first_line) {
			ERR_FAIL_COND_V_EDMSG(line_data.size() < 1, {}, "ERROR : Localization data has no lines?, sheet: " + sheet_name);
			is_first_line = false;

			number_of_columns = line_data.size();

			for (uint8_t i = 3; i < number_of_columns; ++i) {
				if (line_data[i] == language_as_string) {
					language_index = i;
					break;
				}
			}

			if (revert_localization_to_english && language_index == 0) {
				file->close();
				ERR_FAIL_COND_V_EDMSG(true, get_localize_data(sheet_name, key, "en"), "ERROR : Could not find localization data for given language, sheet: " + sheet_name + ", key: " + key + ", langauge: " + language + ". Returning english version instead.");
			}
			else {
				if (language_index == 0) {
					file->close();
					ERR_FAIL_COND_V_EDMSG(true, {}, "ERROR : Localization language not found in sheet: " + sheet_name + ", langauge: " + language);
				}
			}
			continue;
		}
		
		if (found_key && !line_data[0].is_empty()) {
			break;
		}

		if (!found_key && key_as_string != line_data[0]) continue; // Check if it's the correct key.
		result.append(create_new_entry(line_data, language_index));
		found_key = true;
	}
	file->close();

	if (revert_localization_to_english && result.is_empty() && language_as_string != "en") {
		ERR_FAIL_COND_V_EDMSG(true, get_localize_data(sheet_name, key, "en"), "ERROR : Could not find localization data for given language, sheet: " + sheet_name + ", key: " + key + ", langauge: " + language + ". Returning english version instead.");
	}

	ERR_FAIL_COND_V_EDMSG(result.is_empty(), {}, "ERROR : Could not find localization data for, sheet: " + sheet_name + ", key: " + key + ", langauge: " + language);
	localize_data[sheet_name][key] = result;
	return result;
}

String BG_Booker_DB::get_localize_string(const StringName sheet_name, const StringName key, const StringName language, bool ignore_code_data)
{
	TypedArray<BG_LocalizeEntryData> data = get_localize_data(sheet_name, key, language);
	ERR_FAIL_COND_V_EDMSG(data.size() < 1, 
		"LOCALIZATION NOT FOUND", "ERROR : Could not find localization data for, sheet: " + sheet_name + ", key: " + key + ", langauge: " + language);
	
	String result = "";
	for (uint16_t i = 0; i < data.size(); ++i) {
		const BG_LocalizeEntryData *d = cast_to<BG_LocalizeEntryData>(data[i]);
		if (ignore_code_data) {
			result += d->get_text();
		}
		else {
			result += (d->get_code_start() + d->get_text() + d->get_code_end());
		}
	}

	return result;
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
	float result = 0.0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice[i]))
			continue;
		
		float die_result = 0.0;
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			if ( (random_num_generator != nullptr) && UtilityFunctions::is_instance_id_valid(random_num_generator->get_instance_id()) )
				die_result += random_num_generator->randf_range(1.0, float(die->get_amount_of_sides()));
			else
				die_result += UtilityFunctions::randf_range(1.0, float(die->get_amount_of_sides()));
		}
		die_result += float(die->get_additive());
		die_result *= die->get_multiplier();
		result += die_result;
	}

	const int result_int = int(Math::round(result));
	return Math::max(0, result_int);
}

/* static */ int BG_Dice::get_dice_max_roll(const TypedArray<BG_Dice> dice)
{
	float result = 0.0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(die))
			continue;
		
		float die_result = 0.0;
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			die_result += float(die->get_amount_of_sides());
		}
		die_result += float(die->get_additive());
		die_result *= die->get_multiplier();
		result += die_result;
	}

	const int result_int = int(Math::round(result));
	return result_int;
}

/* static */ int BG_Dice::get_dice_average_roll(const TypedArray<BG_Dice> dice)
{
	float max_roll = 0.0;
	float additives = 0.0;
	float roll_counts = 0.0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice[i]))
			continue;
		
		roll_counts += die->get_roll_count();
		for (int x = 0; x < die->get_roll_count(); x++)
		{
			max_roll += (float(die->get_amount_of_sides()) * die->get_multiplier());
		}
		additives += (float(die->get_additive()) * die->get_multiplier());
	}

	return int(Math::round((max_roll + roll_counts * 0.5f) + additives));
}

/* static */ String BG_Dice::dice_to_nice_name(const TypedArray<BG_Dice> dice)
{
	float minimum_damage = 0.0;
	float maximum_damage = 0.0;
	for (int i = 0; i < dice.size(); i++)
	{
		const BG_Dice *die = cast_to<BG_Dice>(dice[i]);
		if (!BG_Focus_Layer_Properties::bg_is_instance_valid(dice[i]))
			continue;
		
		if (die->get_use_raw_numbers()) {
			minimum_damage += (float(die->get_amount_of_sides()) * die->get_multiplier());
			maximum_damage += (float(die->get_roll_count()) * die->get_multiplier());
		} else {
			for (int x = 0; x < die->get_roll_count(); x++)
			{
				minimum_damage += (float(1) * die->get_multiplier());
				maximum_damage += (float(die->get_amount_of_sides()) * die->get_multiplier());
			}
			minimum_damage += (float(die->get_additive()) * die->get_multiplier());
			maximum_damage += (float(die->get_additive()) * die->get_multiplier());
		}
	}

	const int minimum_damage_int = int(Math::round(minimum_damage));
	const int maximum_damage_int = int(Math::round(maximum_damage));
	if (minimum_damage_int == maximum_damage_int)
	{
		return String::num_int64(minimum_damage_int);
	}
	return String::num_int64(minimum_damage_int) + "~" + String::num_int64(maximum_damage_int);
}

/* static */ String BG_Dice::dice_to_string(const Ref<BG_Dice> dice)
{
	if ( dice == nullptr || !BG_Focus_Layer_Properties::bg_is_instance_valid(dice.ptr()) )
	{
		return "-";
	}
	
	const int roll_count = dice->get_roll_count();
	const int amount_of_sides = int(Math::round(float(dice->get_amount_of_sides()) * dice->get_multiplier()));
	String result = String::num_int64(roll_count) + "d" + String::num_int64(amount_of_sides);
	if (dice->get_additive() > 0) {
		const int additive = int(Math::round(float(dice->get_additive()) * dice->get_multiplier()));
		result += "+" + String::num_int64(dice->get_additive());
	}
	else if (dice->get_additive() < 0) {
		const int additive = int(Math::round(float(dice->get_additive()) * dice->get_multiplier()));
		result += "-" + String::num_int64(abs(dice->get_additive()));
	}
	return result;
}

/* static */ Ref<BG_Dice> BG_Dice::string_to_dice(String string)
{
	if (string.is_empty()) {
		return nullptr;
	}
	Ref<BG_Dice> result = memnew(BG_Dice);
	if (string.contains("d")) {
		
		result->set_roll_count(string.split("d")[0].to_int());

		const String after_d = string.split("d")[1].replace(" ", ""); // I.e:   6 or 6+1 or 6-1
		if (after_d.contains("+") || after_d.contains("-")) {

			if (after_d.contains("+")) {
				result->set_amount_of_sides(after_d.split("+")[0].to_int());
				result->set_additive(after_d.split("+")[1].to_int());
			}
			else {
				result->set_amount_of_sides(after_d.split("-")[0].to_int());
				result->set_additive(after_d.split("-")[1].to_int() * -1); // Negative the number since it's going to subtract.
			}
		}
		// Not dice. Parse it like it's a base number instead.
		else {
			result->set_amount_of_sides(after_d.to_int());
		}
	}
	else {
		result->set_use_raw_numbers(true);

		static const String splitter = "-";
		if (string.contains(splitter)) {
			result->set_amount_of_sides(string.split(splitter)[0].to_int());
			result->set_roll_count(string.split(splitter)[1].to_int());
		} else {
			result->set_amount_of_sides(string.to_int()); // I.e:    6
			result->set_roll_count(string.to_int()); // I.e:    6
		}
	}
	
	return result;}

/* static */ TypedArray<BG_Dice> BG_Dice::string_to_dice_options(String string)
{
	TypedArray<BG_Dice> result;
	if (string.is_empty())
	{
		return result;
	}
	if (!string.contains("[") && !string.contains("|"))
	{
		result.append(BG_Dice::string_to_dice(string));
		return result;
	}
	else if (string.contains("d"))
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
	else // We're expecting raw numbers instead of dice. I.e    [6-8], meaning 6 through 8.
	{
		String bracets_removed = string;
		if (string.contains("[")) {
			bracets_removed = string.split("[")[1].split("]")[0].replace(" ", ""); // [6-8] to 6-8
		}
		const String left_side = bracets_removed.split("|")[0];
		const String right_side = bracets_removed.split("|")[1];

		static const String splitter = "-";
		Vector2i left_side_min_max;
		Vector2i right_side_min_max;
		if (left_side.contains(splitter)) {
			left_side_min_max = Vector2i(left_side.split(splitter)[0].to_int(), left_side.split(splitter)[1].to_int()); // 6-8 to Vector2(6, 8)
		} else {
			left_side_min_max = Vector2i(left_side.to_int(), left_side.to_int());
		}
		if (right_side.contains(splitter)) {
			right_side_min_max = Vector2i(right_side.split(splitter)[0].to_int(), right_side.split(splitter)[1].to_int());
		} else {
			right_side_min_max = Vector2i(right_side.to_int(), right_side.to_int());
		}

		// Get how many variations we want to store. Since we don't want to store a variation for every single number.
		static const int ideal_variations = 5;
		int min_variation_count = 1;
		int max_variation_count = 1;

		// Get how many variations for the minimum.
		const int min_difference = right_side_min_max.x - left_side_min_max.x;
		if (min_difference > 0) {
			int variation_count = ideal_variations + 1;
			while (variation_count >= 1) {
				variation_count -= 1;
				const float split_amount = float(min_difference) / variation_count;
				if (split_amount >= 1.0) {
					min_variation_count = variation_count;
					break;
				}
			}
		}
		const int min_split_amount = int(Math::round(float(min_difference) / float(min_variation_count)));

		// Get how many variations for the maximum.
		const int max_difference = right_side_min_max.y - left_side_min_max.y;
		if (max_difference > 0) {
			int variation_count = ideal_variations + 1;
			while (variation_count >= 1) {
				variation_count -= 1;
				const float split_amount = float(max_difference) / variation_count;
				if (split_amount >= 1.0) {
					max_variation_count = variation_count;
					break;
				}
			}
		}
		const int max_split_amount = int(Math::round(float(max_difference) / float(max_variation_count)));

		const int best_variation_count = Math::max(min_variation_count, max_variation_count);
		for (int i = 0; i < best_variation_count; ++i)
		{
			int min = 0;
			int max = 0;
			if (i > 0) {
				min = int(Math::round(float(min_variation_count) / float(i)));
				max = int(Math::round(float(max_variation_count) / float(i)));
			}

			Ref<BG_Dice> new_dice = memnew(BG_Dice);
			new_dice->set_use_raw_numbers(true);
			new_dice->set_amount_of_sides(left_side_min_max.x + (min * min_split_amount));
			new_dice->set_roll_count(left_side_min_max.y + (max * max_split_amount));
			result.append(new_dice);
			// UtilityFunctions::prints(min, max, "min :", new_dice->amount_of_sides, "max :", new_dice->roll_count, float(max_variation_count) / float(i));
		}
	}
	return result;
}

/* static */ Ref<BG_Dice> BG_Dice::duplicate_dice(const Ref<BG_Dice> dice)
{
	if (dice == nullptr || !BG_Focus_Layer_Properties::bg_is_instance_valid(dice.ptr())) return nullptr;
	Ref<BG_Dice> result = memnew(BG_Dice);
	result->roll_count = dice->roll_count;
	result->amount_of_sides = dice->amount_of_sides;
	result->additive = dice->additive;
	result->multiplier = dice->multiplier;
	result->use_raw_numbers = dice->use_raw_numbers;
	return result;
}
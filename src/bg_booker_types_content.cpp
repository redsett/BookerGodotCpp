// Data classes for content: stats, effects, damage, rarities, stat details, animations, items, drop pools.
#include "bg_booker_data_base.hpp"
#include "bg_dber_read.hpp"
#include "bg_focus_layers.hpp"
#include "bg_hex_grid.hpp"

#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <string>

using namespace godot;

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

BG_BaseStat::~BG_BaseStat()
{
	parent_stat_reference = nullptr;
}

////
//// BG_ContentStat
////
void BG_ContentStat::_bind_methods()
{
	ClassDB::bind_static_method("BG_ContentStat", D_METHOD("get_total_stat_value", "stat"), &BG_ContentStat::get_total_stat_value);

	ClassDB::bind_method(D_METHOD("get_stat_reference"), &BG_ContentStat::get_stat_reference);
	ClassDB::bind_method(D_METHOD("get_value"), &BG_ContentStat::get_value);
	ClassDB::bind_method(D_METHOD("get_level_value"), &BG_ContentStat::get_level_value);
	ClassDB::bind_method(D_METHOD("get_randomize_damage_type"), &BG_ContentStat::get_randomize_damage_type);
	ClassDB::bind_method(D_METHOD("get_randomize_resistance_type"), &BG_ContentStat::get_randomize_resistance_type);
}

BG_ContentStat::~BG_ContentStat()
{
	stat_reference = nullptr;
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
	
	if (multiplier == 0.0) {
		UtilityFunctions::push_error("BG_ContentStat::get_total_stat_value - multiplier is 0.0???");
	}
	float amount_to_mult = 1.0 / multiplier;
	result *= amount_to_mult;
	return result;
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
//// BG_EffectRarityDetails
////
void BG_EffectRarityDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_rarity_id"), &BG_EffectRarityDetails::get_rarity_id);
	ClassDB::bind_method(D_METHOD("get_description"), &BG_EffectRarityDetails::get_description);
	ClassDB::bind_method(D_METHOD("get_script_path"), &BG_EffectRarityDetails::get_script_path);
	ClassDB::bind_method(D_METHOD("get_action_points_cost"), &BG_EffectRarityDetails::get_action_points_cost);
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

BG_Effect::~BG_Effect()
{
	for (int i = 0; i < details_per_rarity.size(); ++i) {
		BG_EffectRarityDetails *d = cast_to<BG_EffectRarityDetails>(details_per_rarity[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

////
//// BG_DamageContainer
////
void BG_DamageContainer::_bind_methods()
{
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("get_damage_containers_min_max", "damage_containers", "multiplier"), &BG_DamageContainer::get_damage_containers_min_max);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("calculate_damage", "damage_containers", "multiplier", "random_number_generator"), &BG_DamageContainer::calculate_damage);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("get_damage_containers_max_damage", "damage_containers", "multiplier"), &BG_DamageContainer::get_damage_containers_max_damage);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("get_average_damage_from_damage_containers", "damage_containers", "multiplier"), &BG_DamageContainer::get_average_damage_from_damage_containers);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("damage_containers_to_nice_name", "damage_containers", "multiplier"), &BG_DamageContainer::damage_containers_to_nice_name);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("string_to_damage_container", "string"), &BG_DamageContainer::string_to_damage_container);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("string_to_damage_containers", "string"), &BG_DamageContainer::string_to_damage_containers);
	ClassDB::bind_static_method("BG_DamageContainer", D_METHOD("duplicate_damage_container", "damage_container"), &BG_DamageContainer::duplicate_damage_container);

	ClassDB::bind_method(D_METHOD("get_min"), &BG_DamageContainer::get_min);
	ClassDB::bind_method(D_METHOD("set_min"), &BG_DamageContainer::set_min);
	ClassDB::bind_method(D_METHOD("get_max"), &BG_DamageContainer::get_max);
	ClassDB::bind_method(D_METHOD("set_max"), &BG_DamageContainer::set_max);
}

/* static */ Vector2i BG_DamageContainer::get_damage_containers_min_max(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier)
{
	Vector2i min_max;
	for (int i = 0; i < damage_containers.size(); ++i) {
		const BG_DamageContainer *dmg_con = cast_to<BG_DamageContainer>(damage_containers[i]);
		if (!BG_Booker_DB::bg_is_instance_valid(dmg_con))
			continue;
		
		min_max.x += dmg_con->get_min();
		min_max.y += dmg_con->get_max();
	}

	const float min_multiplied = float(min_max.x) * multiplier;
	const float max_multiplied = float(min_max.y) * multiplier;	
	min_max.x = int(Math::round(min_multiplied));
	min_max.y = int(Math::round(max_multiplied));
	return min_max;
}

/* static */ int BG_DamageContainer::calculate_damage(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier, RandomNumberGenerator *random_num_generator)
{
	const Vector2i min_max = BG_DamageContainer::get_damage_containers_min_max(damage_containers, multiplier);
	const int result_int = UtilityFunctions::randi_range(min_max.x, min_max.y);
	return Math::max(0, result_int);
}

/* static */ int BG_DamageContainer::get_damage_containers_max_damage(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier)
{
	const Vector2i min_max = BG_DamageContainer::get_damage_containers_min_max(damage_containers, multiplier);
	return min_max.y;
}

/* static */ int BG_DamageContainer::get_average_damage_from_damage_containers(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier)
{
	const Vector2i min_max = BG_DamageContainer::get_damage_containers_min_max(damage_containers, multiplier);
	return int(Math::round((min_max.x + min_max.y) * 0.5f));
}

/* static */ String BG_DamageContainer::damage_containers_to_nice_name(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier)
{
	const Vector2i min_max = BG_DamageContainer::get_damage_containers_min_max(damage_containers, multiplier);
	if (min_max.x == min_max.y)
		return String::num_int64(min_max.x);
	return String::num_int64(min_max.x) + "-" + String::num_int64(min_max.y);
}

/* static */ Ref<BG_DamageContainer> BG_DamageContainer::string_to_damage_container(const String &string)
{
	if (string.is_empty())
		return nullptr;
	
	Ref<BG_DamageContainer> result = memnew(BG_DamageContainer);
	static const String splitter = "-";
	if (string.contains(splitter)) {
		result->set_min(string.split(splitter)[0].to_int());
		result->set_max(string.split(splitter)[1].to_int());
	} else {
		result->set_min(string.to_int()); // I.e:    6
		result->set_max(string.to_int()); // I.e:    6
	}
	
	return result;
}

/* static */ TypedArray<BG_DamageContainer> BG_DamageContainer::string_to_damage_containers(const String &string)
{
	TypedArray<BG_DamageContainer> result;
	if (string.is_empty())
	{
		return result;
	}
	// Only numbers, ranges and variations (i.e. "6", "6-8", "1-3|2-4", "[1-3|2-4]"). Anything else (like the old dice
	// "[1d8-1d12]") would read as wrong numbers, so report it and use 0 instead.
	for (int i = 0; i < string.length(); ++i)
	{
		const char32_t c = string[i];
		if (!((c >= '0' && c <= '9') || c == '-' || c == '|' || c == '[' || c == ']' || c == ' '))
		{
			UtilityFunctions::push_error("BG_DamageContainer: can't read damage \"", string, "\". Use min-max, or min-max|min-max for variations.");
			result.append(BG_DamageContainer::string_to_damage_container("0"));
			return result;
		}
	}
	if (string.contains("["))
	{
		const String bracets_removed = string.split("[")[1].split("]")[0].replace(" ", ""); // [6-8] to 6-8
		if (!bracets_removed.contains("|"))
		{
			result.append(BG_DamageContainer::string_to_damage_container(bracets_removed));
			return result;
		}
	}
	if (!string.contains("[") && !string.contains("|"))
	{
		result.append(BG_DamageContainer::string_to_damage_container(string));
		return result;
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

		// The variations go evenly from the left range to the right one, both included (i.e. "1-3|2-4" is 1-3 and 2-4).
		// One per number of difference, up to max_variations, since we don't want one for every single number.
		static const int max_variations = 5;
		const int difference = MAX(ABS(right_side_min_max.x - left_side_min_max.x), ABS(right_side_min_max.y - left_side_min_max.y));
		const int variation_count = CLAMP(difference + 1, 1, max_variations);
		for (int i = 0; i < variation_count; ++i)
		{
			const float weight = variation_count > 1 ? float(i) / float(variation_count - 1) : 0.0f;
			const int min = int(Math::round(Math::lerp(float(left_side_min_max.x), float(right_side_min_max.x), weight)));
			const int max = int(Math::round(Math::lerp(float(left_side_min_max.y), float(right_side_min_max.y), weight)));

			Ref<BG_DamageContainer> new_dmg_con = memnew(BG_DamageContainer);
			new_dmg_con->set_min(min);
			new_dmg_con->set_max(Math::max(min, max));
			result.append(new_dmg_con);
		}
	}
	return result;
}

/* static */ Ref<BG_DamageContainer> BG_DamageContainer::duplicate_damage_container(const Ref<BG_DamageContainer> &damage_container)
{
	if (!damage_container.is_valid() || !BG_Booker_DB::bg_is_instance_valid(damage_container.ptr())) return nullptr;
	Ref<BG_DamageContainer> result = memnew(BG_DamageContainer);
	result->set_min(damage_container->get_min());
	result->set_max(damage_container->get_max());
	return result;
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
	ClassDB::bind_method(D_METHOD("get_icon_id"), &BG_UnitStatDetails::get_icon_id);
	ClassDB::bind_method(D_METHOD("get_is_damage_type"), &BG_UnitStatDetails::get_is_damage_type);
	ClassDB::bind_method(D_METHOD("get_is_attribute_type"), &BG_UnitStatDetails::get_is_attribute_type);
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
	ClassDB::bind_method(D_METHOD("get_resistant_value_min_max"), &BG_UnitStat::get_resistant_value_min_max);
	ClassDB::bind_method(D_METHOD("set_resistant_value_min_max"), &BG_UnitStat::set_resistant_value_min_max);

	ClassDB::bind_method(D_METHOD("get_damage_containers"), &BG_UnitStat::get_damage_containers);
	ClassDB::bind_method(D_METHOD("get_damage_container"), &BG_UnitStat::get_damage_container);
	ClassDB::bind_method(D_METHOD("set_damage_container"), &BG_UnitStat::set_damage_container);
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

BG_AnimationDetails::~BG_AnimationDetails()
{
	for (int i = 0; i < tags.size(); ++i) {
		BG_AnimationTagDetails *d = cast_to<BG_AnimationTagDetails>(tags[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

/* static */ TypedArray<BG_AnimationDetails> BG_AnimationDetails::get_all_anim_details_of_id(const TypedArray<BG_AnimationDetails> &from, const StringName &id)
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

/* static */ BG_AnimationDetails *BG_AnimationDetails::get_anim_details(const TypedArray<BG_AnimationDetails> &from, const StringName &id, AnimationType anim_type)
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
	ClassDB::bind_method(D_METHOD("get_audio_data_by_id", "id", "act"), &BG_ItemDetails::get_audio_data_by_id);
	ClassDB::bind_method(D_METHOD("get_audio_id_by_id", "id"), &BG_ItemDetails::get_audio_id_by_id);
	ClassDB::bind_method(D_METHOD("get_sell_value_tier"), &BG_ItemDetails::get_sell_value_tier);
	ClassDB::bind_method(D_METHOD("get_fame_value_tier"), &BG_ItemDetails::get_fame_value_tier);
	ClassDB::bind_method(D_METHOD("get_durability_value_tier"), &BG_ItemDetails::get_durability_value_tier);

	ClassDB::bind_method(D_METHOD("get_item_effectiveness_stats"), &BG_ItemDetails::get_item_effectiveness_stats);
	ClassDB::bind_method(D_METHOD("get_effectiveness"), &BG_ItemDetails::get_effectiveness);
	ClassDB::bind_method(D_METHOD("get_base_action_points"), &BG_ItemDetails::get_base_action_points);
	ClassDB::bind_method(D_METHOD("get_bonus_action_points"), &BG_ItemDetails::get_bonus_action_points);
	ClassDB::bind_method(D_METHOD("get_base_action_points_cost"), &BG_ItemDetails::get_base_action_points_cost);
	ClassDB::bind_method(D_METHOD("get_level_range"), &BG_ItemDetails::get_level_range);
	ClassDB::bind_method(D_METHOD("get_item_animations"), &BG_ItemDetails::get_item_animations);
	ClassDB::bind_method(D_METHOD("get_weapon_category"), &BG_ItemDetails::get_weapon_category);
	ClassDB::bind_method(D_METHOD("get_armor_category"), &BG_ItemDetails::get_armor_category);

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

BG_ItemDetails::~BG_ItemDetails()
{
	for (int i = 0; i < lore.size(); ++i) {
		BG_LoreRarity *d = cast_to<BG_LoreRarity>(lore[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	if (BG_Booker_DB::bg_is_instance_valid(hue_shift_data))
		memdelete(hue_shift_data);
	for (int i = 0; i < item_effectiveness_stats.size(); ++i) {
		BG_ContentStat *d = cast_to<BG_ContentStat>(item_effectiveness_stats[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < item_animations.size(); ++i) {
		BG_AnimationDetails *d = cast_to<BG_AnimationDetails>(item_animations[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

TypedArray<BG_AudioDataDetails> BG_ItemDetails::get_audio_data_by_id(const StringName &id, int act) const {
	if (audio_events.has(id)) {
		const BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
		return booker_db->get_audio_details(id, 0);
	}
	return {};
}

StringName BG_ItemDetails::get_audio_id_by_id(const StringName &id) const {
	if (audio_events.has(id)) return audio_events[id];
	return StringName("");
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

BG_ItemDropPool::~BG_ItemDropPool()
{
	for (int i = 0; i < item_drops.size(); ++i) {
		BG_RewardItem *d = cast_to<BG_RewardItem>(item_drops[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

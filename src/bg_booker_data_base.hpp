#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

// #include <bg_hex_grid.hpp>

using namespace godot;

////
//// BG_TwoDer_DataEntry
////
class BG_TwoDer_DataEntry : public Object
{
	GDCLASS(BG_TwoDer_DataEntry, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName data_path;
	StringName get_data_path() const { return data_path; }

	StringName asset_name;
	StringName get_asset_name() const { return asset_name; }

	StringName prop_name;
	StringName get_prop_name() const { return prop_name; }

	StringName anim_name;
	StringName get_anim_name() const { return anim_name; }

	StringName camera_name;
	StringName get_camera_name() const { return camera_name; }

	float anim_speed_mult = 1.0;
	float get_anim_speed_mult() const { return anim_speed_mult; }

	float scale_multiplier = 1.0;
	float get_scale_multiplier() const { return scale_multiplier; }
};

////
//// BG_LocalizeEntryData
////
class BG_LocalizeEntryData : public Object
{
	GDCLASS(BG_LocalizeEntryData, Object);

protected:
	static void _bind_methods();

public:
	String code_start;
	String get_code_start() const { return code_start; }

	String text;
	String get_text() const { return text; }

	String code_end;
	String get_code_end() const { return code_end; }
};

////
//// BG_BaseStat
////
class BG_BaseStat : public Object
{
	GDCLASS(BG_BaseStat, Object);

protected:
	static void _bind_methods();

public:
	int unique_id = -1;
	int get_unique_id() const { return unique_id; }

	bool is_base_value = false;
	bool get_is_base_value() const { return is_base_value; }

	float value;
	float get_value() const { return value; }

	int parent_stat_unique_id = -1;
	// int get_parent_stat_unique_id() const { return parent_stat_unique_id; }

	BG_BaseStat *parent_stat_reference = nullptr;
	BG_BaseStat *get_parent_stat_reference() const { return parent_stat_reference; }

	StringName stat_id_name;
	StringName get_stat_id_name() const { return stat_id_name; }
};

////
//// BG_ContentStat
////
class BG_ContentStat : public Object
{
	GDCLASS(BG_ContentStat, Object);

protected:
	static void _bind_methods();

public:
	BG_BaseStat *stat_reference = nullptr;
	BG_BaseStat *get_stat_reference() const { return stat_reference; }

	float value = 0.0;
	float get_value() const { return value; }
	
	int level_value = 0;
	int get_level_value() const { return level_value; }

	bool randomize_damage_type = false;
	bool get_randomize_damage_type() const { return randomize_damage_type; }

	bool randomize_resistance_type = false;
	bool get_randomize_resistance_type() const { return randomize_resistance_type; }

	static float get_total_stat_value(const BG_BaseStat *stat);
};

////
//// BG_BattleBoard_HexTypeVisualDetails
////
class BG_BattleBoard_HexTypeVisualDetails : public Object
{
	GDCLASS(BG_BattleBoard_HexTypeVisualDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	StringName two_der_id;
	StringName get_two_der_id() const { return two_der_id; }

	StringName scene_path;
	StringName get_scene_path() const { return scene_path; }

	StringName destroyed_icon_path;
	StringName get_destroyed_icon_path() const { return destroyed_icon_path; }

	StringName destroyed_two_der_id;
	StringName get_destroyed_two_der_id() const { return destroyed_two_der_id; }

	StringName destroyed_scene_path;
	StringName get_destroyed_scene_path() const { return destroyed_scene_path; }

	Color tint = Color(1.0, 1.0, 1.0);
	Color get_hex_tint() const { return tint; }
};

////
//// BG_BattleBoard_HexTypeDetails
////
class BG_BattleBoard_HexTypeDetails : public Object
{
	GDCLASS(BG_BattleBoard_HexTypeDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	bool is_dynamic_type = false;
	bool get_is_dynamic_type() const { return is_dynamic_type; }

	int hex_type = 0;
	int get_hex_type() const { return hex_type; }

	TypedArray<BG_BattleBoard_HexTypeVisualDetails> visuals;
	TypedArray<BG_BattleBoard_HexTypeVisualDetails> get_visuals() const { return visuals; }

	StringName destroyed_vfx_scene_path;
	StringName get_destroyed_vfx_scene_path() const { return destroyed_vfx_scene_path; }

	StringName destroyed_sfx_id;
	StringName get_destroyed_sfx_id() const { return destroyed_sfx_id; }

	StringName hex_visual_scene_path_override;
	StringName get_hex_visual_scene_path_override() const { return hex_visual_scene_path_override; }

	BG_ContentStat *health_effectiveness = nullptr;
	BG_ContentStat *get_health_effectiveness() const { return health_effectiveness; }

	TypedArray<StringName> equipment_ids;
	TypedArray<StringName> get_equipment_ids() const { return equipment_ids; }

	bool is_actionable = false;
	bool get_is_actionable() const { return is_actionable; }

	bool actionable_requires_move = false;
	bool get_actionable_requires_move() const { return actionable_requires_move; }

	bool pass_through_by_ally = false;
	bool get_pass_through_by_ally() const { return pass_through_by_ally; }
	
	bool pass_through_by_enemy = false;
	bool get_pass_through_by_enemy() const { return pass_through_by_enemy; }
};

////
//// BG_BattleBoardDetails
////
class BG_BattleBoardDetails : public Object
{
	GDCLASS(BG_BattleBoardDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName unique_save_name;
	StringName get_unique_save_name() const { return unique_save_name; }

	StringName board_path;
	StringName get_board_path() const { return board_path; }

	StringName default_hex_visual_path;
	StringName get_default_hex_visual_path() const { return default_hex_visual_path; }

	TypedArray<BG_BattleBoard_HexTypeDetails> hex_types;
	TypedArray<BG_BattleBoard_HexTypeDetails> get_hex_types() const { return hex_types; }

	TypedArray<BG_BattleBoard_HexTypeDetails> get_hex_types_by_type(int type, bool is_game_type = true) const;
	BG_BattleBoard_HexTypeDetails *get_hex_type_by_id(const StringName &id) const;
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

	TypedArray<StringName> rarity_availabilities;
	TypedArray<StringName> get_rarity_availabilities() const { return rarity_availabilities; }

	float drop_weight = 0.0;
	float get_drop_weight() const { return drop_weight; }
};

////
//// BG_ObjectiveTimeline
////
class BG_ObjectiveTimeline : public Object
{
	GDCLASS(BG_ObjectiveTimeline, Object);

protected:
	static void _bind_methods();

public:
	int act;
	int get_act() const { return act; }

	int min_week;
	int get_min_week() const { return min_week; }

	int max_week;
	int get_max_week() const { return max_week; }
};

////
//// BG_ObjectiveDetails
////
class BG_ObjectiveDetails : public Object
{
	GDCLASS(BG_ObjectiveDetails, Object);

protected:
	static void _bind_methods();

public:
	enum ObjectiveRepeatableType : int32_t {
		UNIQUE,
		REPEATABLE_UNIQUE,
		REPEATABLE
	};

	StringName id;
	StringName get_id() const { return id; }

	StringName localization_key;
	StringName get_localization_key() const { return localization_key; }

	bool is_main_objective;
	bool get_is_main_objective() const { return is_main_objective; }

	bool is_scripted;
	bool get_is_scripted() const { return is_scripted; }

	ObjectiveRepeatableType objective_repeatable_type = ObjectiveRepeatableType::UNIQUE;
	ObjectiveRepeatableType get_objective_repeatable_type() const { return objective_repeatable_type; }

	bool is_auto_complete;
	bool get_is_auto_complete() const { return is_auto_complete; }

	bool is_turn_in;
	bool get_is_turn_in() const { return is_turn_in; }

	bool is_event;
	bool get_is_event() const { return is_event; }

	BG_ObjectiveTimeline *timeline;
	BG_ObjectiveTimeline *get_timeline() const { return timeline; }

	int expires_in;
	int get_expires_in() const { return expires_in; }

	String script_path;
	String get_script_path() const { return script_path; }

	int reputation_drop;
	int get_reputation_drop() const { return reputation_drop; }

	int maelstrite_drop;
	int get_maelstrite_drop() const { return maelstrite_drop; }

	TypedArray<BG_RewardItem> drops;
	TypedArray<BG_RewardItem> get_drops() { return drops; }

	int beast_part_drop_count;
	int get_beast_part_drop_count() const { return beast_part_drop_count; }

	int equipment_drop_count;
	int get_equipment_drop_count() const { return equipment_drop_count; }

	Dictionary misc_attributes;
	Dictionary get_misc_attributes() const { return misc_attributes; }
};
VARIANT_ENUM_CAST(BG_ObjectiveDetails::ObjectiveRepeatableType);

////
//// BG_ResourceTypeDetails
////
class BG_ResourceTypeDetails : public Object
{
	GDCLASS(BG_ResourceTypeDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }
};

////
//// BG_MailData
////
class BG_MailData : public Object
{
	GDCLASS(BG_MailData, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	int act;
	int get_act() const { return act; }

	int week;
	int get_week() const { return week; }
};

////
//// BG_MarketplaceEntryData
////
class BG_MarketplaceEntryData : public Object
{
	GDCLASS(BG_MarketplaceEntryData, Object);

protected:
	static void _bind_methods();

public:
	int act;
	int get_act() const { return act; }

	int starting_month;
	int get_starting_month() const { return starting_month; }

	TypedArray<StringName> item_drop_pool_ids;
	TypedArray<StringName> get_item_drop_pool_ids() const { return item_drop_pool_ids; }
};

////
//// BG_MarketplaceData
////
class BG_MarketplaceData : public Object
{
	GDCLASS(BG_MarketplaceData, Object);

protected:
	static void _bind_methods();

public:
	Vector2i items_per_month_min_max;
	Vector2i get_items_per_month_min_max() const { return items_per_month_min_max; }

	TypedArray<BG_MarketplaceEntryData> entries;
	TypedArray<BG_MarketplaceEntryData> get_entries() const { return entries; }
};

////
//// BG_MarketplaceSaveData
////
class BG_MarketplaceSaveData : public Resource
{
	GDCLASS(BG_MarketplaceSaveData, Resource);

protected:
	static void _bind_methods();

public:
	int seed;
	int get_seed() const { return seed; }
	void set_seed(int new_seed) { seed = new_seed; }

	TypedArray<int> items_sold_indexies;
	TypedArray<int> get_items_sold_indexies() const { return items_sold_indexies; }
	void set_items_sold_indexies(const TypedArray<int> &v) { items_sold_indexies = v; }
};

////
//// BG_AudioDataDetails
////
class BG_AudioDataDetails : public Object
{
	GDCLASS(BG_AudioDataDetails, Object);

protected:
	static void _bind_methods();

public:

	StringName file_path;
	StringName get_file_path() const { return file_path; }

	int volume_db_base_value;
	int get_volume_db_base_value() const { return volume_db_base_value; }

	TypedArray<int> restrict_to_acts;
	TypedArray<int> get_restrict_to_acts() const { return restrict_to_acts; }
};

////
//// BG_AudioData
////
class BG_AudioData : public Object
{
	GDCLASS(BG_AudioData, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_AudioDataDetails> audio_details;
	TypedArray<BG_AudioDataDetails> get_audio_details() const { return audio_details; }
};

////
//// BG_HueShiftData
////
class BG_HueShiftData : public Object
{
	GDCLASS(BG_HueShiftData, Object);

protected:
	static void _bind_methods();

public:
	StringName stat_id;
	StringName get_stat_id() const { return stat_id; }

	StringName mask_path;
	StringName get_mask_path() const { return mask_path; }

	Color from_color = Color(1.0, 1.0, 1.0);
	Color get_from_color() const { return from_color; }

	float multiplier;
	float get_multiplier() const { return multiplier; }
};

////
//// BG_EffectRarityDetails
////
class BG_EffectRarityDetails : public Object
{
	GDCLASS(BG_EffectRarityDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName rarity_id;
	StringName get_rarity_id() const { return rarity_id; }

	StringName description;
	StringName get_description() const { return description; }

	StringName script_path;
	StringName get_script_path() const { return script_path; }

	Dictionary value_attributes;
	Dictionary get_value_attributes() const { return value_attributes; }
};

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

	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	TypedArray<BG_EffectRarityDetails> details_per_rarity;
	TypedArray<BG_EffectRarityDetails> get_details_per_rarity() const { return details_per_rarity; }

	bool use_owning_item_icon = false;
	bool get_use_owning_item_icon() const { return use_owning_item_icon; }

	StringName status_icon_path;
	StringName get_status_icon_path() const { return status_icon_path; }
};

////
//// BG_Dice
////
class BG_Dice : public RefCounted
{
	GDCLASS(BG_Dice, RefCounted);

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

	float multiplier = 1.0;
	float get_multiplier() const { return multiplier; }
	void set_multiplier(float value) { multiplier = value; }

	bool use_raw_numbers = false;
	bool get_use_raw_numbers() const { return use_raw_numbers; }
	void set_use_raw_numbers(bool value) { use_raw_numbers = value; }

	static int calculate_dice(const TypedArray<BG_Dice> &dice, RandomNumberGenerator *random_num_generator = nullptr);
	static int get_dice_max_roll(const TypedArray<BG_Dice> &dice);
	static int get_dice_average_roll(const TypedArray<BG_Dice> &dice);
	static String dice_to_nice_name(const TypedArray<BG_Dice> &dice);
	static String dice_to_string(const Ref<BG_Dice> &dice);
	static Ref<BG_Dice> string_to_dice(const String &string);
	static TypedArray<BG_Dice> string_to_dice_options(const String &string);
	static Ref<BG_Dice> duplicate_dice(const Ref<BG_Dice> &dice);
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

	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	String icon_path = "";
	String get_icon_path() const { return icon_path; }

	bool is_damage_type = false;
	bool get_is_damage_type() const { return is_damage_type; }

	StringName weak_to_element;
	StringName get_weak_to_element() const { return weak_to_element; }

	Color widget_color = Color(1.0, 1.0, 1.0);
	Color get_widget_color() const { return widget_color; }

	Color text_color = Color(1.0, 1.0, 1.0);
	Color get_text_color() const { return text_color; }

	Color in_world_color = Color(1.0, 1.0, 1.0);
	Color get_in_world_color() const { return in_world_color; }
};

////
//// BG_UnitStat
////
class BG_UnitStat : public RefCounted
{
	GDCLASS(BG_UnitStat, RefCounted);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }
	void set_id(StringName value) { id = value; }

	float bonus_percentage = 0.0f;
	float get_bonus_percentage() const { return bonus_percentage; }

	int offensive_value = 0;
	int get_offensive_value() const { return offensive_value; }
	void set_offensive_value(int v) { offensive_value = v; }

	int defensive_value = 0;
	int get_defensive_value() const { return defensive_value; }
	void set_defensive_value(int v) { defensive_value = v; }

	int resistant_unsaved_stored_value = 0;
	int get_resistant_unsaved_stored_value() const { return resistant_unsaved_stored_value; }
	void set_resistant_unsaved_stored_value(int v) { resistant_unsaved_stored_value = v; }

	StringName resistant_value_text;
	StringName get_resistant_value_text() const { return resistant_value_text; }

	Vector2i resistant_value_min_max;
	Vector2i get_resistant_value_min_max() const { return resistant_value_min_max; }
	void set_resistant_value_min_max(Vector2i v) { resistant_value_min_max = v; }

	StringName dice_string;
	StringName get_dice_string() const { return dice_string; }

	TypedArray<BG_Dice> dice_options;
	TypedArray<BG_Dice> get_dice_options() const { return dice_options; }

	Ref<BG_Dice> dice = nullptr;
	Ref<BG_Dice> get_dice() const { return dice; }
	void set_dice(const Ref<BG_Dice> &value) { dice = value; }

	static Vector2i string_to_resistant_value_min_max(const String &string);
};

////
//// BG_AnimationTagDetails
////
class BG_AnimationTagDetails : public Object
{
	GDCLASS(BG_AnimationTagDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName tag;
	StringName get_tag() const { return tag; }

	int frame = 0;
	int get_frame() const { return frame; }
};

////
//// BG_AnimationDetails
////
class BG_AnimationDetails : public Object
{
	GDCLASS(BG_AnimationDetails, Object);

protected:
	static void _bind_methods();

public:
	enum AnimationType : int32_t {
		IDLE,
		COMBAT_IDLE,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		UNIQUE_SKILL1,
		RUN,
		GUARD,
		EVADE,
		TAKING_DAMAGE,
		AT_LOW_HP,
		DEATH,
		TRIUMPH,
		FLOURISH,
		TRAVERSE_TO_INTRO,
		TRAVERSE_TO_LOOP,
		TRAVERSE_TO_OUTRO,
		TRAVERSE_BACK_INTRO,
		TRAVERSE_BACK_LOOP,
		TRAVERSE_BACK_OUTRO
	};

	enum AnimationTraverseType : int32_t {
		TRAVERSE_TYPE_NONE,
		TRAVERSE_TYPE_RUN,
		TRAVERSE_TYPE_RUN_INTRO_LOOP_OUTRO,
		TRAVERSE_TYPE_HOP,
		TRAVERSE_TYPE_HOP_INTRO_LOOP_OUTRO
	};

	StringName caste_or_monster_id;
	StringName get_caste_or_monster_id() const { return caste_or_monster_id; }

	AnimationType anim_type = AnimationType::IDLE;
	AnimationType get_anim_type() const { return anim_type; }

	StringName anim_name;
	StringName get_anim_name() const { return anim_name; }
	
	AnimationTraverseType traverse_to = AnimationTraverseType::TRAVERSE_TYPE_NONE;
	AnimationTraverseType get_traverse_to() const { return traverse_to; }

	AnimationTraverseType traverse_back = AnimationTraverseType::TRAVERSE_TYPE_NONE;
	AnimationTraverseType get_traverse_back() const { return traverse_back; }

	float anim_blend_time = 0.0;
	float get_anim_blend_time() const { return anim_blend_time; }
	
	TypedArray<BG_AnimationTagDetails> tags;
	TypedArray<BG_AnimationTagDetails> get_tags() const { return tags; }

	static TypedArray<BG_AnimationDetails> get_all_anim_details_of_id(const TypedArray<BG_AnimationDetails> &from, const StringName &id);
	static BG_AnimationDetails *get_anim_details(const TypedArray<BG_AnimationDetails> &from, const StringName &id, AnimationType anim_type);
};

VARIANT_ENUM_CAST(BG_AnimationDetails::AnimationType);
VARIANT_ENUM_CAST(BG_AnimationDetails::AnimationTraverseType);

////
//// BG_ItemSlotType
////
class BG_ItemSlotType : public Object
{
	GDCLASS(BG_ItemSlotType, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName name;
	StringName get_name() const { return name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	TypedArray<float> percentage_of_all_items_dropped_per_act;
	TypedArray<float> get_percentage_of_all_items_dropped_per_act() const { return percentage_of_all_items_dropped_per_act; }
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
	void set_id(const StringName &v) { id = v; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }
	
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

	int level = 1;
	int get_level() const { return level; }
	void set_level(int v) { level = v; }

	int weeks_left_to_finish_graft = 0;
	int get_weeks_left_to_finish_graft() const { return weeks_left_to_finish_graft; }
	void set_weeks_left_to_finish_graft(int v) { weeks_left_to_finish_graft = v; }

	int number_of_jobs_participated_in = 0;
	int get_number_of_jobs_participated_in() const { return number_of_jobs_participated_in; }
	void set_number_of_jobs_participated_in(int v) { number_of_jobs_participated_in = v; }

	int consecutive_number_of_jobs_participated_in = 0;
	int get_consecutive_number_of_jobs_participated_in() const { return consecutive_number_of_jobs_participated_in; }
	void set_consecutive_number_of_jobs_participated_in(int v) { consecutive_number_of_jobs_participated_in = v; }

	bool should_count_in_item_rarity_disparity = false;
	bool get_should_count_in_item_rarity_disparity() const { return should_count_in_item_rarity_disparity; }
	void set_should_count_in_item_rarity_disparity(bool v) { should_count_in_item_rarity_disparity = v; }

	StringName rarity_id;
	StringName get_rarity_id() const { return rarity_id; }
	void set_rarity_id(const StringName &value) { rarity_id = value; }

	TypedArray<BG_Item> grafts;
	TypedArray<BG_Item> get_grafts() const { return grafts; }
	void set_grafts(const TypedArray<BG_Item> &value) { grafts = value; }

	StringName forced_damage_element;
	StringName get_forced_damage_element() const { return forced_damage_element; }
	void set_forced_damage_element(const StringName &v) { forced_damage_element = v; }

	TypedArray<BG_Item> nested_items;
	TypedArray<BG_Item> get_nested_items() const { return nested_items; }
	void set_nested_items(const TypedArray<BG_Item> &value) { nested_items = value; }
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
	enum ItemType : int32_t {
		GEAR,
		BEAST_PART,
		CONSUMABLE
	};

	enum AttackTargetType : int32_t {
		SINGLE_FRONT,
		SINGLE_BACK,
		ALL_FRONT_ROW,
		ALL_BACK_ROW,
		ALL_RANDOM_ROW,
		ALL_RANDOM_COLUMN,
		ALL
	};

	StringName id;
	StringName get_id() const { return id; }

	StringName name;
	StringName get_name() const { return name; }

	ItemType slot_type = GEAR;
	ItemType get_slot_type() const { return slot_type; }

	bool is_gear() const { return slot_type == GEAR; }
	bool is_beast_part() const { return slot_type == BEAST_PART; }
	bool is_consumable() const { return slot_type == CONSUMABLE; }

	StringName slot_type_id;
	StringName get_slot_type_id() { return slot_type_id; }

	TypedArray<StringName> beast_part_available_item_slot_types;
	TypedArray<StringName> get_beast_part_available_item_slot_types() const { return beast_part_available_item_slot_types; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	BG_HueShiftData *hue_shift_data = nullptr;
	BG_HueShiftData *get_hue_shift_data() const { return hue_shift_data; };

	StringName mesh_path;
	StringName get_mesh_path() const { return mesh_path; }

	TypedArray<BG_LoreRarity> lore;
	TypedArray<BG_LoreRarity> get_lore() const { return lore; }

	TypedArray<StringName> caste_ids;
	TypedArray<StringName> get_caste_ids() const { return caste_ids; }

	Dictionary stats; // StringName(rarity id): TypedArray<BG_UnitStat>
	Dictionary get_stats() const { return stats; }

	int animation_attach_socket = 0;
	int get_animation_attach_socket() const { return animation_attach_socket; }

	Dictionary effects; // StringName(rarity id): TypedArray<StringName>(effect ids)
	Dictionary get_effects() const { return effects; }

	int sell_value_tier = 0;
	int get_sell_value_tier() const { return sell_value_tier; }

	int fame_value_tier = 0;
	int get_fame_value_tier() const { return fame_value_tier; }

	int durability_value_tier = 0;
	int get_durability_value_tier() const { return durability_value_tier; }
	
	
	// New DBer Data
	bool use_dber_data = false;
	bool get_use_dber_data() const { return use_dber_data; }
	
	float effectiveness = 0.0;
	float get_effectiveness() const { return effectiveness; }

	TypedArray<BG_ContentStat> item_effectiveness_stats;
	TypedArray<BG_ContentStat> get_item_effectiveness_stats() const { return item_effectiveness_stats; }

	bool use_stat_requirements = false;
	bool get_use_stat_requirements() const { return use_stat_requirements; }

	TypedArray<BG_ContentStat> item_stat_requirements;
	TypedArray<BG_ContentStat> get_item_stat_requirements() const { return item_stat_requirements; }

	Vector2 level_range = Vector2(1, 1);
	Vector2 get_level_range() const { return level_range; }

	TypedArray<BG_AnimationDetails> item_animations;
	TypedArray<BG_AnimationDetails> get_item_animations() const { return item_animations; }
};

VARIANT_ENUM_CAST(BG_ItemDetails::ItemType);
VARIANT_ENUM_CAST(BG_ItemDetails::AttackTargetType);

////
//// BG_ItemDropPool
////
class BG_ItemDropPool : public Object
{
	GDCLASS(BG_ItemDropPool, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_RewardItem> item_drops;
	TypedArray<BG_RewardItem> get_item_drops() const { return item_drops; }
};

////
//// BG_JobDistributionForAct
////
class BG_JobDistributionForAct : public Object
{
	GDCLASS(BG_JobDistributionForAct, Object);

protected:
	static void _bind_methods();

public:
	int act;
	int get_act() const { return act; }

	int min_week;
	int get_min_week() const { return min_week; }

	int max_week;
	int get_max_week() const { return max_week; }
	
	int min_job_spawn_count;
	int get_min_job_spawn_count() const { return min_job_spawn_count; }

	int max_job_spawn_count;
	int get_max_job_spawn_count() const { return max_job_spawn_count; }
};

////
//// BG_JobMonsterDetails
////
class BG_JobMonsterDetails : public Object
{
	GDCLASS(BG_JobMonsterDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName monster_id;
	StringName get_monster_id() const { return monster_id; }

	Vector2i monster_count_range;
	Vector2i get_monster_count_range() const { return monster_count_range; }

	TypedArray<BG_RewardItem> drops;
	TypedArray<BG_RewardItem> get_drops() { return drops; }

	bool always_drop_at_least_one = false;
	bool get_always_drop_at_least_one() { return always_drop_at_least_one; };
};

////
//// BG_EventResourceDetails
////
class BG_EventResourceDetails : public Object
{
	GDCLASS(BG_EventResourceDetails, Object);

protected:
	static void _bind_methods();

public:
	int maelstrite_amount_of_drops_during_range;
	int get_maelstrite_amount_of_drops_during_range() const { return maelstrite_amount_of_drops_during_range; }

	int maelstrite_total_amount_by_end_of_weeks;
	int get_maelstrite_total_amount_by_end_of_weeks() const { return maelstrite_total_amount_by_end_of_weeks; }
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
	enum BattleBoardEventTypes : int32_t {
		JOB,
		RESOURCE,
		EVENT,
	};

	StringName id;
	StringName get_id() const { return id; }

	BattleBoardEventTypes battle_board_event_type = JOB;
	BattleBoardEventTypes get_battle_board_event_type() const { return battle_board_event_type; }

	int weeks_before_expire = 0;
	int get_weeks_before_expire() const { return weeks_before_expire; }

	TypedArray<BG_JobMonsterDetails> monster_details;
	TypedArray<BG_JobMonsterDetails> get_monster_details() const { return monster_details; }

	BG_EventResourceDetails *event_resource_details;
	BG_EventResourceDetails *get_event_resource_details() const { return event_resource_details; }

	bool can_spawn_in_day_time = false;
	bool get_can_spawn_in_day_time() const { return can_spawn_in_day_time; }

	bool is_unique = false;
	bool get_is_unique() const { return is_unique; }

	bool is_boss = false;
	bool get_is_boss() const { return is_boss; }

	TypedArray<BG_JobDistributionForAct> distribution_per_act;
	TypedArray<BG_JobDistributionForAct> get_distribution_per_act() const { return distribution_per_act; }
};
VARIANT_ENUM_CAST(BG_JobDetails::BattleBoardEventTypes);

////
//// BG_UnitCaste
////
class BG_UnitCaste : public Object
{
	GDCLASS(BG_UnitCaste, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName name;
	StringName get_name() const { return name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	BG_HueShiftData *hue_shift_data = nullptr;
	BG_HueShiftData *get_hue_shift_data() const { return hue_shift_data; };

	TypedArray<StringName> lod_mesh_paths;
	TypedArray<StringName> get_lod_mesh_paths() const { return lod_mesh_paths; }

	Vector2 scale_min = Vector2();
	Vector2 get_scale_min() const { return scale_min; }

	Vector2 scale_max = Vector2();
	Vector2 get_scale_max() const { return scale_max; }

	Vector2 scale_min_extreme = Vector2();
	Vector2 get_scale_min_extreme() const { return scale_min_extreme; }

	Vector2 scale_max_extreme = Vector2();
	Vector2 get_scale_max_extreme() const { return scale_max_extreme; }

	int travel_distance = 0;
	int get_travel_distance() const { return travel_distance; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	TypedArray<StringName> starting_item_ids;
	TypedArray<StringName> get_starting_item_ids() const { return starting_item_ids; }

	TypedArray<StringName> element_availability_ids;
	TypedArray<StringName> get_element_availability_ids() const { return element_availability_ids; }
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
	// void _init() {
	// 	if (equipment.is_null()) {
	// 		BG_ReactiveArray *new_array = memnew(BG_ReactiveArray);
	// 		equipment = Ref(new_array);
	// 	}
	// }

	StringName name;
	StringName get_name() const { return name; }
	void set_name(const StringName &value) { name = value; }

	int current_health = 0;
	int get_current_health() const { return current_health; }
	void set_current_health(int value) { current_health = value; }

	int slot_index = 0;
	int get_slot_index() const { return slot_index; }
	void set_slot_index(int value) { slot_index = value; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }

	bool is_band_leader = false;
	bool get_is_band_leader() const { return is_band_leader; }
	void set_is_band_leader(bool v) { is_band_leader = v; }

	int level = 1;
	int get_level() const { return level; }
	void set_level(int v) { level = v; }

	float experience = 0.0;
	float get_experience() const { return experience; }
	void set_experience(float v) { experience = v; }

	Vector3 scale;
	Vector3 get_scale() const { return scale; }
	void set_scale(const Vector3 &value) { scale = value; }

	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }
	void set_caste_id(const StringName &value) { caste_id = value; }

	// Ref<BG_ReactiveArray> equipment;
	// Ref<BG_ReactiveArray> get_equipment() const { return equipment; }
	// void set_equipment(Ref<BG_ReactiveArray> value) { equipment = value; }

	TypedArray<BG_Item> equipment;
	TypedArray<BG_Item> get_equipment() const { return equipment; }
	void set_equipment(const TypedArray<BG_Item> &value) { equipment = value; }

	bool is_city_asset = false;
	bool get_is_city_asset() const { return is_city_asset; }
	void set_is_city_asset(bool v) { is_city_asset = v; }

	Ref<class BG_Band> band;
	Ref<BG_Band> get_band() const { return band; }
	void set_band(const Ref<BG_Band> &value) { band = value; }

	BG_BattleBoard_HexTypeDetails *battle_board_hex_type_details = nullptr;
	BG_BattleBoard_HexTypeDetails *get_battle_board_hex_type_details() const { return battle_board_hex_type_details; }
	void set_battle_board_hex_type_details(BG_BattleBoard_HexTypeDetails *v) { battle_board_hex_type_details = v; }

	Dictionary element_upgrades; //<StringName, int>
	Dictionary get_element_upgrades() const { return element_upgrades; }
	void set_element_upgrades(const Dictionary &v) { element_upgrades = v; }

	Dictionary consumable_upgrades; //<StringName(consumable_id), int(how_many)>
	Dictionary get_consumable_upgrades() const { return consumable_upgrades; }
	void set_consumable_upgrades(const Dictionary &v) { consumable_upgrades = v; }

	bool is_dead() { return current_health <= 0; }
};

////
//// BG_Formation
////
class BG_Formation : public Resource
{
	GDCLASS(BG_Formation, Resource);

protected:
	static void _bind_methods();

public:
	String id;
	String get_id() const { return id; }
	void set_id(const String &value) {
		id = value;
		if (!Engine::get_singleton()->is_editor_hint()) {
			return;
		}

		Array new_formation;

		if (id.contains("\n")) {
			PackedStringArray split_id = id.split("\n");
			for (int i = 0; i < split_id.size(); ++i) {
				TypedArray<int> this_line;
				String s = split_id[i];
				for (int x = 0; x < s.length(); ++x) {
					String character = String::chr(s[x]);
					this_line.append(character == "1" ? 1 : 0);
				}
				new_formation.append(this_line);
			}
		}
		set_formation(new_formation);
	}

	Array formation;
	Array get_formation() const { return formation; }
	void set_formation(Array value) { formation = value; }
};

////
//// BG_DefaultFormations
////
class BG_DefaultFormations : public Resource
{
	GDCLASS(BG_DefaultFormations, Resource);

protected:
	static void _bind_methods();

public:
	TypedArray<BG_Formation> formations;
	Array get_formations() const { return formations; }
	void set_formations(const Array &value) { formations = value; }
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
	int unique_id = 0;
	int get_unique_id() const { return unique_id; }
	void set_unique_id(int value) { unique_id = value; }

	StringName name;
	StringName get_name() const { return name; }
	void set_name(const StringName &value) { name = value; }

	bool resting = false;
	bool get_resting() const { return resting; }
	void set_resting(bool value) { resting = value; }

	bool is_on_battle_board = false;
	bool get_is_on_battle_board() const { return is_on_battle_board; }
	void set_is_on_battle_board(bool value) { is_on_battle_board = value; }

	int distance_traveled_this_turn = 0;
	int get_distance_traveled_this_turn() const { return distance_traveled_this_turn; }
	void set_distance_traveled_this_turn(int value) { distance_traveled_this_turn = value; }

	bool has_done_combat_this_turn = false;
	bool get_has_done_combat_this_turn() const { return has_done_combat_this_turn; }
	void set_has_done_combat_this_turn(bool value) { has_done_combat_this_turn = value; }

	int knocked_out_turns = 0;
	int get_knocked_out_turns() const { return knocked_out_turns; }
	void set_knocked_out_turns(int value) { knocked_out_turns = value; }

	TypedArray<BG_BandMember> band_members;
	TypedArray<BG_BandMember> get_band_members() const { return band_members; }
	void set_band_members(const TypedArray<BG_BandMember> &value) { band_members = value; }

	int current_unique_job_id = -1; // -1 means that they don't have a job.
	int get_current_unique_job_id() const { return current_unique_job_id; }
	void set_current_unique_job_id(int value) { current_unique_job_id = value; }

	Dictionary band_formation; // BG_BandMember : int(cell index)
	Dictionary get_band_formation() const { return band_formation; }
	void set_band_formation(const Dictionary &value) { band_formation = value; }	

	bool has_job() const { return current_unique_job_id != -1; };

	void clear_job() { current_unique_job_id = -1; };

	bool is_band_alive() const;

	Ref<BG_BandMember> get_band_leader() const {
		for (int i = 0; i < band_members.size(); ++i) {
			Ref<BG_BandMember> bm = band_members[i];
			if (!bm.is_valid()) continue;
			if (bm->get_is_band_leader()) return bm;
		}

		// There should always be a band leader. So let's just make a random band member the leader.
		Ref<BG_BandMember> bm = band_members.pick_random();
		if (!bm.is_valid()) return nullptr;
		bm->set_is_band_leader(true);
		return bm;
	}

	void set_band_leader(const Ref<BG_BandMember> &p_band_member) {
		if (!p_band_member.is_valid()) return;
		for (int i = 0; i < band_members.size(); ++i) {
			Ref<BG_BandMember> bm = band_members[i];
			if (!bm.is_valid()) continue;
			bm->set_is_band_leader(false);
		}
		p_band_member->set_is_band_leader(true);
	}
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
//// BG_Job
////
class BG_Job : public Resource
{
	GDCLASS(BG_Job, Resource);

protected:
	static void _bind_methods();

public:
	StringName job_id;
	StringName get_job_id() const { return job_id; }
	void set_job_id(const StringName &value) { job_id = value; }

	int unique_job_id = 0;
	int get_unique_job_id() const { return unique_job_id; }
	void set_unique_job_id(int value) { unique_job_id = value; }
	
	int random_seed = 0;
	int get_random_seed() const { return random_seed; }
	void set_random_seed(int value) { random_seed = value; }

	int weeks_remaining_unless_removed = 0;
	int get_weeks_remaining_unless_removed() const { return weeks_remaining_unless_removed; }
	void set_weeks_remaining_unless_removed(int value) { weeks_remaining_unless_removed = value; }
	
	TypedArray<class BG_Monster> monsters;
	TypedArray<BG_Monster> get_monsters() const { return monsters; }
	void set_monsters(const TypedArray<BG_Monster> &value) { monsters = value; }

	Dictionary formation; // BG_Monster : int(cell index)
	Dictionary get_formation() const { return formation; }
	void set_formation(const Dictionary &value) { formation = value; }

	bool is_on_battle_board = false;
	bool get_is_on_battle_board() const { return is_on_battle_board; }
	void set_is_on_battle_board(bool value) { is_on_battle_board = value; }

	void try_set_all_monsters_to_stoned(bool stoned);
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

	int preferred_row = 0;
	int get_preferred_row() const { return preferred_row; }

	int max_health = 0;
	int get_max_health() const { return max_health; }

	int travel_distance = 0;
	int get_travel_distance() const { return travel_distance; }

	int current_health = 0;
	int get_current_health() const { return current_health; }
	void set_current_health(int value) { current_health = value; }

	int random_variation = 0;
	int get_random_variation() const { return random_variation; }
	void set_random_variation(int v) { random_variation = v; }

	float challenge_rating = 0.0f;
	float get_challenge_rating() const { return challenge_rating; }
	
	int level = 1;
	int get_level() const { return level; }
	void set_level(int v) { level = v; }

	int monster_type = 0;
	int get_monster_type() const { return monster_type; }

	TypedArray<BG_UnitStat> stats;
	TypedArray<BG_UnitStat> get_stats() const { return stats; }

	TypedArray<StringName> element_availability_ids;
	TypedArray<StringName> get_element_availability_ids() const { return element_availability_ids; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	StringName model_path;
	StringName get_model_path() const { return model_path; }

	TypedArray<StringName> effect_ids;
	TypedArray<StringName> get_effect_ids() const { return effect_ids; }

	TypedArray<StringName> out_of_combat_effect_ids;
	TypedArray<StringName> get_out_of_combat_effect_ids() const { return out_of_combat_effect_ids; }

	TypedArray<BG_HueShiftData> hue_shift_data;
	TypedArray<BG_HueShiftData> get_hue_shift_data() const { return hue_shift_data; };

	Ref<BG_Job> job;
	Ref<BG_Job> get_job() const { return job; };
	void set_job(Ref<BG_Job> value) { job = value; };

	String get_challenge_rating_fraction_string() const;

	bool can_be_turned_to_stone = true;
	bool get_can_be_turned_to_stone() const { return can_be_turned_to_stone; }

	bool is_turned_to_stone = false;
	bool get_is_turned_to_stone() const { return is_turned_to_stone; }
	void set_is_turned_to_stone(bool v);

	bool is_dead() { return current_health <= 0; }


	// New DBer Data
	bool use_dber_data = false;
	bool get_use_dber_data() const { return use_dber_data; }
	
	float effectiveness = 0.0;
	float get_effectiveness() const { return effectiveness; }

	TypedArray<BG_ContentStat> effectiveness_stats;
	TypedArray<BG_ContentStat> get_effectiveness_stats() const { return effectiveness_stats; }

	Vector2 level_range = Vector2(1, 1);
	Vector2 get_level_range() const { return level_range; }

	TypedArray<BG_AnimationDetails> animations;
	TypedArray<BG_AnimationDetails> get_animations() const { return animations; }
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

	Vector2 band_size_min_max;
	Vector2 get_band_size_min_max() const { return band_size_min_max; }

	int num_bands_for_hire = 0;
	int get_num_bands_for_hire() const { return num_bands_for_hire; }

	TypedArray<BG_UnitCaste> unit_castes;
	TypedArray<BG_UnitCaste> get_unit_castes() const { return unit_castes; }

	float rest_recovery_speed = 0.0;
	float get_rest_recovery_speed() const { return rest_recovery_speed; }

	int knocked_out_turns = 0;
	int get_knocked_out_turns() const { return knocked_out_turns; }

};

////
//// BG_ChallengeRatingGuide
////
class BG_ChallengeRatingGuide : public Object
{
	GDCLASS(BG_ChallengeRatingGuide, Object);

protected:
	static void _bind_methods();

public:
	Vector2 cr_min_max = Vector2();
	Vector2 get_cr_min_max() const { return cr_min_max; }

	int job_rep_reward = 0;
	int get_job_rep_reward() const { return job_rep_reward; }

	int job_duralation = 0;
	int get_job_duralation() const { return job_duralation; }

	float item_durability_consumption = 0.0f;
	float get_item_durability_consumption() const { return item_durability_consumption; }

	float item_fame_addition = 0.0f;
	float get_item_fame_addition() const { return item_fame_addition; }
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
	int total_week_count = 0;
	int get_total_week_count() const { return total_week_count; }

	int total_job_count = 0;
	int get_total_job_count() const { return total_job_count; }

	StringName job_handout_curve_path;
	StringName get_job_handout_curve_path() const { return job_handout_curve_path; }
};

////
//// BG_EquipmentAnimationDetails
////
class BG_EquipmentAnimationDetails : public Object
{
	GDCLASS(BG_EquipmentAnimationDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }

	TypedArray<StringName> equipment_ids;
	TypedArray<StringName> get_equipment_ids() const { return equipment_ids; }

	StringName in_game_animation_name;
	StringName get_in_game_animation_name() const { return in_game_animation_name; }
};

////
//// BG_BookerSkillTreeSlotDetails
////
class BG_BookerSkillTreeSlotDetails : public Object
{
	GDCLASS(BG_BookerSkillTreeSlotDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	bool is_parent_button;
	bool get_is_parent_button() const { return is_parent_button; }

	StringName skill_name;
	StringName get_skill_name() const { return skill_name; }

	StringName skill_description;
	StringName get_skill_description() const { return skill_description; }

	int required_rep;
	int get_required_rep() const { return required_rep; }

	StringName parent_skill_id;
	StringName get_parent_skill_id() const { return parent_skill_id; }

	TypedArray<float> value_attributes;
	TypedArray<float> get_value_attributes() const { return value_attributes; }
};

////
//// BG_CityInfo
////
class BG_CityInfo : public Object
{
	GDCLASS(BG_CityInfo, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	StringName scene_path;
	StringName get_scene_path() const { return scene_path; }

	StringName battle_board_id;
	StringName get_battle_board_id() const { return battle_board_id; }

	Dictionary misc_attributes;
	Dictionary get_misc_attributes() const { return misc_attributes; }
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

	Dictionary global_curves;
	Dictionary get_global_curves() const { return global_curves; }

	int starting_reputation = 0;
	int get_starting_reputation() const { return starting_reputation; }

	float chance_of_no_drop = 0.0;
	float get_chance_of_no_drop() const { return chance_of_no_drop; }

	int combat_rounds_per_combat = 0;
	int get_combat_rounds_per_combat() const { return combat_rounds_per_combat; }
	
	int max_resistance_soft_cap = 0;
	int get_max_resistance_soft_cap() const { return max_resistance_soft_cap; }

	int max_resistance_hard_cap = 0;
	int get_max_resistance_hard_cap() const { return max_resistance_hard_cap; }

	TypedArray<BG_CityInfo> city_info;
	TypedArray<BG_CityInfo> get_city_info() const { return city_info; }

	//MyCustomClass* result = dynamic_cast<MyCustomClass*>(container.object());
	TypedArray<BG_ActStats> act_stats;
	TypedArray<BG_ActStats> get_act_stats() const { return act_stats; }
	
	TypedArray<BG_ChallengeRatingGuide> challenge_rating_guide;
	TypedArray<BG_ChallengeRatingGuide> get_challenge_rating_guide() const { return challenge_rating_guide; }

	TypedArray<float> monster_element_distribution;
	TypedArray<float> get_monster_element_distribution() const { return monster_element_distribution; }

	// Teir Values
	TypedArray<int> sell_tier_values;
	TypedArray<int> get_sell_tier_values() const { return sell_tier_values; }

	TypedArray<int> fame_tier_values;
	TypedArray<int> get_fame_tier_values() const { return fame_tier_values; }

	TypedArray<int> durability_teir_values;
	TypedArray<int> get_durability_teir_values() const { return durability_teir_values; }

	// Teir Value Multipliers
	TypedArray<float> equipment_value_rarity_multiplier;
	TypedArray<float> get_equipment_value_rarity_multiplier() const { return equipment_value_rarity_multiplier; }

	TypedArray<float> beast_part_value_rarity_multiplier;
	TypedArray<float> get_beast_part_value_rarity_multiplier() const { return beast_part_value_rarity_multiplier; }

	TypedArray<float> extra_beast_part_value_rarity_multiplier_while_grafted;
	TypedArray<float> get_extra_beast_part_value_rarity_multiplier_while_grafted() const { return extra_beast_part_value_rarity_multiplier_while_grafted; }

	TypedArray<float> equipment_durability_rarity_multiplier;
	TypedArray<float> get_equipment_durability_rarity_multiplier() const { return equipment_durability_rarity_multiplier; }

	TypedArray<float> beast_part_durability_rarity_multiplier;
	TypedArray<float> get_beast_part_durability_rarity_multiplier() const { return beast_part_durability_rarity_multiplier; }

	TypedArray<float> equipment_fame_rarity_multiplier;
	TypedArray<float> get_equipment_fame_rarity_multiplier() const { return equipment_fame_rarity_multiplier; }

	TypedArray<float> beast_part_fame_rarity_multiplier;
	TypedArray<float> get_beast_part_fame_rarity_multiplier() const { return beast_part_fame_rarity_multiplier; }

	float inventory_sell_multiplier = 0.0;
	float get_inventory_sell_multiplier() const { return inventory_sell_multiplier; }

	float item_passive_income_multiplier = 0.0;
	float get_item_passive_income_multiplier() const { return item_passive_income_multiplier; }

	float percent_amount_to_add_on_same_element_per_damage_value = 0.0;
	float get_percent_amount_to_add_on_same_element_per_damage_value() const { return percent_amount_to_add_on_same_element_per_damage_value; }
	
	float percent_amount_to_subtract_on_weak_element_per_damage_value = 0.0;
	float get_percent_amount_to_subtract_on_weak_element_per_damage_value() const { return percent_amount_to_subtract_on_weak_element_per_damage_value; }

	// QTE multipliers
	float hit_perfect_timing_multiplier = 0.0;
	float get_hit_perfect_timing_multiplier() const { return hit_perfect_timing_multiplier; }

	float hit_good_timing_multiplier = 0.0;
	float get_hit_good_timing_multiplier() const { return hit_good_timing_multiplier; }

	float parry_perfect_timing_multiplier = 0.0;
	float get_parry_perfect_timing_multiplier() const { return parry_perfect_timing_multiplier; }

	float parry_good_timing_multiplier = 0.0;
	float get_parry_good_timing_multiplier() const { return parry_good_timing_multiplier; }
	
	// Day Night Cycle
	int day_night_cycle_turns_per_day = 0;
	int get_day_night_cycle_turns_per_day() const { return day_night_cycle_turns_per_day; }

	float percentage_of_day_is_day_time = 0.0;
	float get_percentage_of_day_is_day_time() const { return percentage_of_day_is_day_time; }

	TypedArray<int> day_cycle_offset_per_act;
	TypedArray<int> get_day_cycle_offset_per_act() const { return day_cycle_offset_per_act; }
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

	static void timer_test(const Callable &callable);

	String modding_path = "";
	String get_modding_path() const { return modding_path; }

	BG_Booker_Globals *globals = nullptr;
	BG_Booker_Globals *get_globals() const { return globals; }

	TypedArray<BG_ObjectiveDetails> objectives;
	TypedArray<BG_ObjectiveDetails> get_objectives() const { return objectives; }	

	TypedArray<BG_BaseStat> base_stats;
	TypedArray<BG_BaseStat> get_base_stats() const { return base_stats; }
	
	TypedArray<BG_BattleBoardDetails> battle_boards_details;
	TypedArray<BG_BattleBoardDetails> get_battle_boards_details() const { return battle_boards_details; }

	BG_BattleBoardDetails *get_battle_board_by_id(const StringName &id) const;
	TypedArray<BG_BattleBoard_HexTypeDetails> get_battle_board_details_for_bb(const StringName &parent_bb_id, const StringName &bb_id) const;
	TypedArray<BG_BattleBoard_HexTypeDetails> get_battle_board_hex_types_by_type(const StringName &parent_bb_id, const StringName &bb_id, int type, bool is_game_type = true) const;
	BG_BattleBoard_HexTypeDetails *get_battle_board_hex_type_by_id(const StringName &parent_bb_id, const StringName &bb_id, const StringName &id) const;

	HashMap<StringName, BG_ResourceTypeDetails *> resource_type_details;
	BG_ResourceTypeDetails *get_resource_type_details_by_id(const StringName &resource_id) const;

	TypedArray<BG_AudioData> audio_data;
	TypedArray<BG_AudioData> get_audio_data() const { return audio_data; }
	TypedArray<BG_AudioDataDetails> get_audio_details(const StringName &id, int act) const;

	TypedArray<BG_BookerSkillTreeSlotDetails> booker_skill_tree_details;
	TypedArray<BG_BookerSkillTreeSlotDetails> get_booker_skill_tree_details() const { return booker_skill_tree_details; }

	TypedArray<BG_JobDetails> jobs;
	TypedArray<BG_JobDetails> get_jobs() const { return jobs; }

	TypedArray<BG_ItemDetails> items;
	TypedArray<BG_ItemDetails> get_items() const { return items; }

	TypedArray<BG_ItemDropPool> item_drop_pools;
	TypedArray<BG_ItemDropPool> get_item_drop_pools() const { return item_drop_pools; }

	TypedArray<BG_Effect> effects;
	TypedArray<BG_Effect> get_effects() const { return effects; }

	TypedArray<BG_EquipmentAnimationDetails> equipment_animation_details;
	TypedArray<BG_EquipmentAnimationDetails> get_equipment_animation_details() const { return equipment_animation_details; }

	BG_BandInfo *band_info = nullptr;
	BG_BandInfo *get_band_info() const { return band_info; }

	TypedArray<BG_ItemSlotType> item_slot_types;
	TypedArray<BG_ItemSlotType> get_item_slot_types() const { return item_slot_types; }

	TypedArray<BG_RarityDetails> rarity_types;
	TypedArray<BG_RarityDetails> get_rarity_types() const { return rarity_types; }

	TypedArray<BG_UnitStatDetails> stat_types;
	TypedArray<BG_UnitStatDetails> get_stat_types() const { return stat_types; }

	BG_MarketplaceData *market_place_data = nullptr;
	BG_MarketplaceData *get_market_place_data() const { return market_place_data; }

	TypedArray<BG_Monster> monster_types;
	TypedArray<BG_Monster> get_monster_types() const { return monster_types; }

	TypedArray<BG_MailData> mail_data;
	TypedArray<BG_MailData> get_mail_data() const { return mail_data; }
	
	TypedArray<BG_TwoDer_DataEntry> two_der_data_entries;
	TypedArray<BG_TwoDer_DataEntry> get_two_der_data_entries() const { return two_der_data_entries; }
	BG_TwoDer_DataEntry *get_two_der_data_from_id(const StringName &id) const;

	bool revert_localization_to_english = true;
	void set_revert_localization_to_english(bool revert) { revert_localization_to_english = revert; }
	// Sheet Name, Key, Data
	HashMap<StringName, HashMap<StringName, TypedArray<BG_LocalizeEntryData>>> localize_data;
	TypedArray<BG_LocalizeEntryData> get_localize_data(const StringName &sheet_name, const StringName &key, const StringName &language);
	String get_localize_string(const StringName &sheet_name, const StringName &key, const StringName &language, bool ignore_code_data = false);

	static float get_job_challenge_rating_value(const TypedArray<BG_Monster> &monsters);
	static String get_job_challenge_rating(const TypedArray<BG_Monster> &monsters);

	BG_BaseStat *get_stat_from_stat_id_name(const StringName &stat_id_name) const;

private:
	void try_parse_data(const String &file_path);
	void try_parse_bder_data(const String &file_path);

	BG_BaseStat *get_stat_from_unique_id(int unique_id) const;
};

#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include <bg_reactive.hpp>

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
	void set_items_sold_indexies(TypedArray<int> v) { items_sold_indexies = v; }
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

	TypedArray<StringName> act_file_paths;
	TypedArray<StringName> get_act_file_paths() const { return act_file_paths; }

	TypedArray<int> act_volume_db_base_values;
	TypedArray<int> get_act_volume_db_base_values() const { return act_volume_db_base_values; }
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

	static int calculate_dice(const TypedArray<BG_Dice> dice, RandomNumberGenerator *random_num_generator = nullptr);
	static int get_dice_max_roll(const TypedArray<BG_Dice> dice);
	static int get_dice_average_roll(const TypedArray<BG_Dice> dice);
	static String dice_to_nice_name(const TypedArray<BG_Dice> dice);
	static String dice_to_string(const Ref<BG_Dice> dice);
	static Ref<BG_Dice> string_to_dice(String string);
	static TypedArray<BG_Dice> string_to_dice_options(String string);
	static Ref<BG_Dice> duplicate_dice(const Ref<BG_Dice> dice);
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
	void set_dice(Ref<BG_Dice> value) { dice = value; }

	static Vector2i string_to_resistant_value_min_max(String string);
};

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
	void set_id(StringName v) { id = v; }

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
	void set_rarity_id(StringName value) { rarity_id = value; }

	TypedArray<BG_Item> grafts;
	TypedArray<BG_Item> get_grafts() const { return grafts; }
	void set_grafts(TypedArray<BG_Item> value) { grafts = value; }

	StringName forced_damage_element;
	StringName get_forced_damage_element() const { return forced_damage_element; }
	void set_forced_damage_element(StringName v) { forced_damage_element = v; }

	TypedArray<BG_Item> nested_items;
	TypedArray<BG_Item> get_nested_items() const { return nested_items; }
	void set_nested_items(TypedArray<BG_Item> value) { nested_items = value; }
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
	void set_name(StringName value) { name = value; }

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

	int fame = 0;
	int get_fame() const { return fame; }
	void set_fame(int v) { fame = v; }

	Vector3 scale;
	Vector3 get_scale() const { return scale; }
	void set_scale(Vector3 value) { scale = value; }

	StringName caste_id;
	StringName get_caste_id() const { return caste_id; }
	void set_caste_id(StringName value) { caste_id = value; }

	// Ref<BG_ReactiveArray> equipment;
	// Ref<BG_ReactiveArray> get_equipment() const { return equipment; }
	// void set_equipment(Ref<BG_ReactiveArray> value) { equipment = value; }

	TypedArray<BG_Item> equipment;
	TypedArray<BG_Item> get_equipment() const { return equipment; }
	void set_equipment(TypedArray<BG_Item> value) { equipment = value; }

	bool is_city_asset = false;
	bool get_is_city_asset() const { return is_city_asset; }
	void set_is_city_asset(bool v) { is_city_asset = v; }

	Ref<class BG_Band> band;
	Ref<BG_Band> get_band() const { return band; }
	void set_band(Ref<BG_Band> value) { band = value; }

	class BG_TurretInfo *turret_info = nullptr;
	BG_TurretInfo *get_turret_info() const { return turret_info; }
	void set_turret_info(BG_TurretInfo *v) { turret_info = v; }

	Dictionary element_upgrades; //<StringName, int>
	Dictionary get_element_upgrades() const { return element_upgrades; }
	void set_element_upgrades(Dictionary v) { element_upgrades = v; }

	Dictionary consumable_upgrades; //<StringName(consumable_id), int(how_many)>
	Dictionary get_consumable_upgrades() const { return consumable_upgrades; }
	void set_consumable_upgrades(Dictionary v) { consumable_upgrades = v; }
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
	void set_id(String value) {
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
	void set_formations(Array value) { formations = value; }
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
	void set_name(StringName value) { name = value; }

	bool resting = false;
	bool get_resting() const { return resting; }
	void set_resting(bool value) { resting = value; }

	bool has_done_combat_this_turn = false;
	bool get_has_done_combat_this_turn() const { return has_done_combat_this_turn; }
	void set_has_done_combat_this_turn(bool value) { has_done_combat_this_turn = value; }

	int knocked_out_turns = 0;
	int get_knocked_out_turns() const { return knocked_out_turns; }
	void set_knocked_out_turns(int value) { knocked_out_turns = value; }

	TypedArray<BG_BandMember> band_members;
	TypedArray<BG_BandMember> get_band_members() const { return band_members; }
	void set_band_members(TypedArray<BG_BandMember> value) { band_members = value; }

	int current_unique_job_id = -1; // -1 means that they don't have a job.
	int get_current_unique_job_id() const { return current_unique_job_id; }
	void set_current_unique_job_id(int value) { current_unique_job_id = value; }

	Dictionary band_formation; // BG_BandMember : int(cell index)
	Dictionary get_band_formation() const { return band_formation; }
	void set_band_formation(Dictionary value) { band_formation = value; }	

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

	void set_band_leader(Ref<BG_BandMember> p_band_member) {
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
	void set_job_id(StringName value) { job_id = value; }

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
	void set_monsters(TypedArray<BG_Monster> value) { monsters = value; }

	Dictionary formation; // BG_Monster : int(cell index)
	Dictionary get_formation() const { return formation; }
	void set_formation(Dictionary value) { formation = value; }	
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
	StringName battle_board_scene_path;
	StringName get_battle_board_scene_path() const { return battle_board_scene_path; }

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
//// BG_TurretInfo
////
class BG_TurretInfo : public Object
{
	GDCLASS(BG_TurretInfo, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName nice_name;
	StringName get_nice_name() const { return nice_name; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }

	TypedArray<StringName> twoder_icons;
	TypedArray<StringName> get_twoder_icons() const { return twoder_icons; }
	
	StringName destroyed_icon_path;
	StringName get_destroyed_icon_path() const { return destroyed_icon_path; }
	
	TypedArray<StringName> twoder_destroyed_icons;
	TypedArray<StringName> get_twoder_destroyed_icons() const { return twoder_destroyed_icons; }

	StringName destroyed_vfx_path;
	StringName get_destroyed_vfx_path() const { return destroyed_vfx_path; }

	StringName destroyed_sfx_id;
	StringName get_destroyed_sfx_id() const { return destroyed_sfx_id; }

	int max_health = 0;
	int get_max_health() const { return max_health; }

	TypedArray<StringName> equipment_ids;
	TypedArray<StringName> get_equipment_ids() const { return equipment_ids; }
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

	int max_health = 0;
	int get_max_health() const { return max_health; }

	TypedArray<StringName> equipment_ids;
	TypedArray<StringName> get_equipment_ids() const { return equipment_ids; }

	TypedArray<BG_TurretInfo> barracades;
	TypedArray<BG_TurretInfo> get_barracades() const { return barracades; }

	TypedArray<BG_TurretInfo> turrets;
	TypedArray<BG_TurretInfo> get_turrets() const { return turrets; }

	TypedArray<BG_TurretInfo> towns;
	TypedArray<BG_TurretInfo> get_towns() const { return towns; }

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

	static void timer_test(Callable callable);

	String modding_path = "";
	String get_modding_path() const { return modding_path; }

	BG_Booker_Globals *globals = nullptr;
	BG_Booker_Globals *get_globals() const { return globals; }

	TypedArray<BG_ObjectiveDetails> objectives;
	TypedArray<BG_ObjectiveDetails> get_objectives() const { return objectives; }

	HashMap<StringName, BG_ResourceTypeDetails *> resource_type_details;
	BG_ResourceTypeDetails *get_resource_type_details_by_id(StringName resource_id) const {
		if (resource_type_details.has(resource_id)) {
			return resource_type_details[resource_id];
		}
		return nullptr;
	}

	TypedArray<BG_AudioData> audio_data;
	TypedArray<BG_AudioData> get_audio_data() const { return audio_data; }

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
	BG_TwoDer_DataEntry *get_two_der_data_from_id(StringName id) const {
		for (int i = 0; i < two_der_data_entries.size(); i++) {
			BG_TwoDer_DataEntry *data = cast_to<BG_TwoDer_DataEntry>(two_der_data_entries[i]);
			if (data == nullptr) continue;
			if (data->get_id() == id) return data;
		}
		return nullptr;
	}

	bool revert_localization_to_english = true;
	void set_revert_localization_to_english(bool revert) { revert_localization_to_english = revert; }
	// Sheet Name, Key, Data
	HashMap<StringName, HashMap<StringName, TypedArray<BG_LocalizeEntryData>>> localize_data;
	TypedArray<BG_LocalizeEntryData> get_localize_data(const StringName sheet_name, const StringName key, const StringName language);
	String get_localize_string(const StringName sheet_name, const StringName key, const StringName language, bool ignore_code_data = false);

	static float get_job_challenge_rating_value(const TypedArray<BG_Monster> monsters);
	static String get_job_challenge_rating(const TypedArray<BG_Monster> monsters);

private:
	Dictionary data;

	void try_parse_data(const String &file_path);
};

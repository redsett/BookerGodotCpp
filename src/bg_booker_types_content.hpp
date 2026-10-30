#pragma once

// Data classes for content: stats, effects, damage, rarities, stat details, animations, items, drop pools.
// Part of bg_booker_data_base.hpp: include that one, not this.

#include "bg_booker_types_fwd.hpp"


////
//// BG_BaseStat
////
class BG_BaseStat : public Object
{
	GDCLASS(BG_BaseStat, Object);

protected:
	static void _bind_methods();

public:
	~BG_BaseStat();

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
	~BG_ContentStat();

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

	int action_points_cost = 0;
	int get_action_points_cost() const { return action_points_cost; }

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
	~BG_Effect();

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
//// BG_DamageContainer
////
class BG_DamageContainer : public RefCounted
{
	GDCLASS(BG_DamageContainer, RefCounted);

protected:
	static void _bind_methods();

public:
	int min = 0;
	int get_min() const { return min; }
	void set_min(int value) { min = value; }

	int max = 0;
	int get_max() const { return max; }
	void set_max(int value) { max = value; }

	static Vector2i get_damage_containers_min_max(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier);
	static int calculate_damage(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier, RandomNumberGenerator *random_num_generator = nullptr);
	static int get_damage_containers_max_damage(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier);
	static int get_average_damage_from_damage_containers(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier);
	static String damage_containers_to_nice_name(const TypedArray<BG_DamageContainer> &damage_containers, float multiplier);
	static Ref<BG_DamageContainer> string_to_damage_container(const String &string);
	static TypedArray<BG_DamageContainer> string_to_damage_containers(const String &string);
	static Ref<BG_DamageContainer> duplicate_damage_container(const Ref<BG_DamageContainer> &damage_container);
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

	StringName icon_id;
	StringName get_icon_id() const { return icon_id; }

	bool is_damage_type = false;
	bool get_is_damage_type() const { return is_damage_type; }

	bool is_attribute_type = false;
	bool get_is_attribute_type() const { return is_attribute_type; }

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

	Vector2i resistant_value_min_max;
	Vector2i get_resistant_value_min_max() const { return resistant_value_min_max; }
	void set_resistant_value_min_max(Vector2i v) { resistant_value_min_max = v; }

	TypedArray<BG_DamageContainer> damage_containers;
	TypedArray<BG_DamageContainer> get_damage_containers() const { return damage_containers; }

	Ref<BG_DamageContainer> damage_container = nullptr;
	Ref<BG_DamageContainer> get_damage_container() const { return damage_container; }
	void set_damage_container(const Ref<BG_DamageContainer> &value) { damage_container = value; }
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
	~BG_AnimationDetails();

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
	~BG_ItemDetails();

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

	HashMap<StringName, StringName> audio_events; // id: audio_id
	TypedArray<BG_AudioDataDetails> get_audio_data_by_id(const StringName &id, int act) const;
	StringName get_audio_id_by_id(const StringName &id) const;

	int sell_value_tier = 0;
	int get_sell_value_tier() const { return sell_value_tier; }

	int fame_value_tier = 0;
	int get_fame_value_tier() const { return fame_value_tier; }

	int durability_value_tier = 0;
	int get_durability_value_tier() const { return durability_value_tier; }
	
	// New DBer Data
	float effectiveness = 0.0;
	float get_effectiveness() const { return effectiveness; }

	TypedArray<BG_ContentStat> item_effectiveness_stats;
	TypedArray<BG_ContentStat> get_item_effectiveness_stats() const { return item_effectiveness_stats; }

	int base_action_points = 0;
	int get_base_action_points() const { return base_action_points; }
	
	int bonus_action_points = 0;
	int get_bonus_action_points() const { return bonus_action_points; }

	int base_action_points_cost = 0;
	int get_base_action_points_cost() const { return base_action_points_cost; }

	Vector2 level_range = Vector2(1, 1);
	Vector2 get_level_range() const { return level_range; }

	// This item's own animations: they override its weapon category's animation class (see
	// BG_Booker_DB::get_animation_class_animations), for the rows they have.
	TypedArray<BG_AnimationDetails> item_animations;
	TypedArray<BG_AnimationDetails> get_item_animations() const { return item_animations; }

	// An id from the Weapon_Categories / Armor_Categories sheets, or "" (BG_Booker_DB::get_weapon_categories).
	StringName weapon_category;
	StringName get_weapon_category() const { return weapon_category; }

	StringName armor_category;
	StringName get_armor_category() const { return armor_category; }
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
	~BG_ItemDropPool();

	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_RewardItem> item_drops;
	TypedArray<BG_RewardItem> get_item_drops() const { return item_drops; }
};

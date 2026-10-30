// Booker_DB sheet parsers: stats, stat types, animations, content (items, monsters, ...), effects,
// rarities, item slots, drop pools.
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

// Base Stats: the "Base_Stats" sheet, one row per stat (id, name, value, is_base_value, parent).
void BG_Booker_DB::parse_base_stats(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Base_Stats", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		BG_BaseStat *new_class = memnew(BG_BaseStat);
		new_class->unique_id = i + 1; // Rows have no unique id: their place in the sheet.
		new_class->is_base_value = bool(dber_get(entry, "is_base_value"));
		new_class->value = float(dber_get(entry, "value"));
		new_class->stat_id_name = StringName(dber_get(entry, "id"));
		base_stats.append(new_class);
	}

	{ // Set a reference to each of their parent stats ("parent": its id, or "").
		for (int i = 0; i < lines.size(); ++i) {
			const StringName parent_stat = StringName(dber_get(lines[i], "parent"));
			BG_BaseStat *stat = cast_to<BG_BaseStat>(base_stats[i]);
			if (stat == nullptr || parent_stat.is_empty()) continue;

			stat->parent_stat_reference = get_stat_from_stat_id_name(parent_stat);
			if (stat->parent_stat_reference != nullptr)
				stat->parent_stat_unique_id = stat->parent_stat_reference->unique_id;
		}
	}
}

// Stat Types
void BG_Booker_DB::parse_stat_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Stat_Types", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		
		BG_UnitStatDetails *new_stat_types = memnew(BG_UnitStatDetails);
		new_stat_types->id = StringName(dber_get(entry, "id"));
		new_stat_types->nice_name = StringName(dber_get(entry, "name"));
		new_stat_types->icon_id = StringName(dber_get(entry, "icon_id"));
		new_stat_types->is_damage_type = bool(dber_get(entry, "is_damage_type"));
		new_stat_types->is_attribute_type = bool(dber_get(entry, "is_attribute_type"));
		StringName weak_to_element = StringName(dber_get(entry, "weak_to_element"));
		if (!weak_to_element.is_empty())
			new_stat_types->weak_to_element = weak_to_element;
		
		new_stat_types->widget_color = convert_hex_to_color(dber_get(entry, "widget_color"));
		new_stat_types->text_color = convert_hex_to_color(dber_get(entry, "text_color"));
		new_stat_types->in_world_color = convert_hex_to_color(dber_get(entry, "in_world_color"));
		
		stat_types.append(new_stat_types);
	}
}

// Animation classes, then the weapon and armor categories (rows by id)
void BG_Booker_DB::parse_animation_classes(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	animation_classes.clear();
	const Array class_lines = get_sheet_by_name("Animation_Classes", data);
	for (int i = 0; i < class_lines.size(); ++i) {
		const Dictionary entry = class_lines[i];
		TypedArray<BG_AnimationDetails> class_animations;
		const Array animations = Array(dber_get(entry, "animations"));
		for (int x = 0; x < animations.size(); ++x) {
			class_animations.append(get_animation_detail(global_enums, animations, x));
		}
		animation_classes[StringName(dber_get(entry, "id"))] = class_animations;
	}

	weapon_categories.clear();
	const Array weapon_lines = get_sheet_by_name("Weapon_Categories", data);
	for (int i = 0; i < weapon_lines.size(); ++i) {
		const Dictionary entry = weapon_lines[i];
		weapon_categories[StringName(dber_get(entry, "id"))] = entry;
	}

	armor_categories.clear();
	const Array armor_lines = get_sheet_by_name("Armor_Categories", data);
	for (int i = 0; i < armor_lines.size(); ++i) {
		const Dictionary entry = armor_lines[i];
		armor_categories[StringName(dber_get(entry, "id"))] = entry;
	}
}

// Content
void BG_Booker_DB::parse_content(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = Array(data["all_content"]);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		const StringName content_id = StringName(entry["id"]);

		const Dictionary &misc_params = entry; // A content entry's misc params are fields of the entry.
		const int content_type = int(dber_get(misc_params, "content_type"));
		
		//
		// Equipment
		//
		if (content_type == 0) {
			BG_ItemDetails *item_details = memnew(BG_ItemDetails);
			items.append(item_details);

			item_details->id = content_id;
			item_details->name = StringName(entry["nice_name"]);
			
			const int slot_type = int(dber_get(misc_params, "slot_type"));
			item_details->slot_type_id = global_enums["equipment_slot_types"][slot_type];
			if (slot_type == 3) item_details->slot_type = BG_ItemDetails::ItemType::BEAST_PART;
			else if (slot_type > 3) item_details->slot_type = BG_ItemDetails::ItemType::CONSUMABLE;
			// const int attack_target_type = int(dber_get(misc_params, "attack_target_type"));
			// item_details->attack_target_type = global_enums["attack_target_types"][attack_target_type];

			item_details->effectiveness = float(entry["effectiveness"]);
			item_details->icon_path = ensure_clean_path(dber_get(misc_params, "icon"));
			const Variant mesh_path_dict = dber_find(misc_params, "mesh_scene_path");
			if (mesh_path_dict.get_type() != Variant::NIL) item_details->mesh_path = ensure_clean_path(mesh_path_dict);
			const Variant animation_attach_socket_dict = dber_find(misc_params, "anim_attach_socket");
			if (animation_attach_socket_dict.get_type() != Variant::NIL) item_details->animation_attach_socket = int(animation_attach_socket_dict) + 1;
			const Variant level_range = dber_find(misc_params, "level_range");
			if (level_range.get_type() != Variant::NIL) item_details->level_range = Vector2(int(Array(level_range)[0]), int(Array(level_range)[1]));
			item_details->sell_value_tier = int(dber_get(misc_params, "sell_value_tier"));
			item_details->fame_value_tier = int(dber_get(misc_params, "fame_value_tier"));
			item_details->base_action_points = int(dber_get(misc_params, "action_points"));
			item_details->bonus_action_points = int(dber_get(misc_params, "bonus_action_points"));
			item_details->base_action_points_cost = int(dber_get(misc_params, "action_points_cost"));

			{ // Available Castes
				item_details->caste_ids.clear();
				const Variant available_castes_dict = dber_find(misc_params, "available_castes");
				if (available_castes_dict.get_type() != Variant::NIL) {
					const Array available_castes = Array(available_castes_dict);
					for (int x = 0; x < available_castes.size(); ++x) {
						const Dictionary aa = available_castes[x];
						const StringName caste_id = global_enums["caste_types"][int(dber_get(aa, "caste"))];
						item_details->caste_ids.append(caste_id);
					}
				}
			}

			{ // Item Animations
				item_details->item_animations.clear();
				const Array item_animations = Array(dber_get(misc_params, "animations"));
				for (int x = 0; x < item_animations.size(); ++x) {
					item_details->item_animations.append(get_animation_detail(global_enums, item_animations, x));
				}
			}

			{ // Weapon and armor categories (ids from their sheets, "" when not set)
				const Variant weapon_category = dber_find(misc_params, "weapon_category");
				if (weapon_category.get_type() != Variant::NIL) item_details->weapon_category = StringName(weapon_category);
				const Variant armor_category = dber_find(misc_params, "armor_category");
				if (armor_category.get_type() != Variant::NIL) item_details->armor_category = StringName(armor_category);
			}

			{ // Item Effectiveness Stats
				const bool randomize_damage_type = bool(entry["randomize_damage_type"]);
				const bool randomize_resistance_type = bool(entry["randomize_resistance_type"]);

				const Dictionary item_effectiveness_stats = dber_get(entry, "item_effectiveness_stats"); // {stat name: value}
				const Array item_effectiveness_stats_names = item_effectiveness_stats.keys();
				for (int x = 0; x < item_effectiveness_stats_names.size(); ++x) {
					const float value = float(item_effectiveness_stats[item_effectiveness_stats_names[x]]);
					if (value == 0.0) continue; // No need to store empty values.

					BG_ContentStat *new_class = memnew(BG_ContentStat);
					new_class->value = value;

					// Store a reference to its relative stat.
					new_class->stat_reference = get_stat_from_stat_id_name(StringName(item_effectiveness_stats_names[x]));

					new_class->randomize_damage_type = randomize_damage_type;
					new_class->randomize_resistance_type = randomize_resistance_type;

					item_details->item_effectiveness_stats.append(new_class);
				}
			}


			{ // Effects
				const Array effects_lines = Array(dber_get(misc_params, "effects"));
				for (int x = 0; x < effects_lines.size(); ++x) {
					const Dictionary effects_entry = effects_lines[x];

					TypedArray<StringName> effect_ids;
					const Array effect_array = Array(dber_get(effects_entry, "effects"));
					for (int y = 0; y < effect_array.size(); ++y) {
						const Dictionary effect_entry = effect_array[y];
						effect_ids.append(StringName(dber_get(effect_entry, "effect")));
					}
					
					const StringName rarity_id = StringName(dber_get(effects_entry, "rarity"));
					item_details->effects[rarity_id] = effect_ids;
				}
			}

			{ // Audio Events
				item_details->audio_events.clear();
				const Variant audio_events_dict = dber_find(misc_params, "audio_events");
				if (audio_events_dict.get_type() != Variant::NIL) {
					const Array audio_events = Array(audio_events_dict);
					for (int x = 0; x < audio_events.size(); ++x) {
						const Dictionary audio_events_entry = audio_events[x];
						const StringName audio_id_key = StringName(dber_get(audio_events_entry, "id"));
						item_details->audio_events[audio_id_key] = StringName(dber_get(audio_events_entry, "audio_id"));
					}
				}
			}
		}

		//
		// Monsters
		//
		if (content_type == 1) {
			BG_Monster *monster_details = memnew(BG_Monster);
			monster_types.append(monster_details);

			monster_details->id = content_id;
			monster_details->name = StringName(entry["nice_name"]);
			monster_details->effectiveness = float(entry["effectiveness"]);
			monster_details->icon_path = ensure_clean_path(dber_get(misc_params, "icon"));
			monster_details->model_path = ensure_clean_path(dber_get(misc_params, "mesh_scene_path"));
			const Variant level_range = dber_get(misc_params, "level_range");
			monster_details->level_range = Vector2(int(Array(level_range)[0]), int(Array(level_range)[1]));
			monster_details->can_be_turned_to_stone = bool(dber_get(misc_params, "can_be_turned_to_stone"));
			monster_details->travel_distance = int(dber_get(misc_params, "travel_distance"));
			monster_details->preferred_row = int(dber_get(misc_params, "preferred_row"));
			monster_details->base_action_points = int(dber_get(misc_params, "action_points"));
			monster_details->bonus_action_points = int(dber_get(misc_params, "bonus_action_points"));
			monster_details->base_action_points_cost = int(dber_get(misc_params, "action_points_cost"));
			monster_details->action_points_fatigue_rate_multiplier = float(dber_get(misc_params, "action_points_fatigue_rate_multiplier"));

			{ // Stats
				const Array stats_lines = Array(dber_get(misc_params, "stats"));
				for (int x = 0; x < stats_lines.size(); ++x) {
					const Dictionary stats_entry = stats_lines[x];

					BG_UnitStat *new_class = memnew(BG_UnitStat);
					new_class->id = StringName(dber_get(stats_entry, "stat_id"));
					new_class->defensive_value = int(dber_get(stats_entry, "value"));

					monster_details->stats.append(new_class);
				}
			}

			{ // Effectiveness Stats
				const Dictionary effectiveness_stats = dber_get(entry, "effectiveness_stats"); // {stat name: value}
				const Array effectiveness_stats_names = effectiveness_stats.keys();
				for (int x = 0; x < effectiveness_stats_names.size(); ++x) {
					const float value = float(effectiveness_stats[effectiveness_stats_names[x]]);
					if (value == 0.0) continue; // No need to store empty values.

					BG_ContentStat *new_class = memnew(BG_ContentStat);
					new_class->value = value;

					// Store a reference to its relative stat.
					new_class->stat_reference = get_stat_from_stat_id_name(StringName(effectiveness_stats_names[x]));

					monster_details->effectiveness_stats.append(new_class);
				}
			}

			{ // Animations
				monster_details->animations.clear();
				const Array animations = Array(dber_get(misc_params, "animations"));
				for (int x = 0; x < animations.size(); ++x) {
					monster_details->animations.append(get_animation_detail(global_enums, animations, x));
				}
			}

			{ // Effects
				const Array effects_lines = Array(dber_get(misc_params, "effects"));
				for (int x = 0; x < effects_lines.size(); ++x) {
					const Dictionary effects_entry = effects_lines[x];

					TypedArray<StringName> effect_ids;
					const Array effect_array = Array(dber_get(effects_entry, "effects"));
					for (int y = 0; y < effect_array.size(); ++y) {
						const Dictionary effect_entry = effect_array[y];
						effect_ids.append(StringName(dber_get(effect_entry, "effect")));
					}
					
					monster_details->effect_ids = effect_ids;
				}
			}

			{ // Drops (optional: monsters in DBer don't have this param yet)
				const Array drops_lines = Array(dber_find(misc_params, "drops"));
				for (int x = 0; x < drops_lines.size(); ++x) {
					const Dictionary drops_entry = drops_lines[x];

					BG_RewardItem *new_class = memnew(BG_RewardItem);
					new_class->id = StringName(dber_get(drops_entry, "item_id"));
					new_class->drop_weight = float(dber_get(drops_entry, "drop_weight"));
					monster_details->drops.append(new_class);
				}
			}
		}

		//
		// Beast Parts
		//
		if (content_type == 3) {
			BG_ItemDetails *item_details = memnew(BG_ItemDetails);
			items.append(item_details);

			item_details->id = content_id;
			item_details->name = StringName(entry["nice_name"]);
			
			const int slot_type = int(dber_get(misc_params, "slot_type"));
			item_details->slot_type_id = global_enums["equipment_slot_types"][slot_type];
			item_details->slot_type = BG_ItemDetails::ItemType::BEAST_PART;

			item_details->effectiveness = float(entry["effectiveness"]);
			item_details->icon_path = ensure_clean_path(dber_get(misc_params, "icon"));
			const Variant animation_attach_socket_dict = dber_find(misc_params, "anim_attach_socket");
			if (animation_attach_socket_dict.get_type() != Variant::NIL) item_details->animation_attach_socket = int(animation_attach_socket_dict) + 1;
			// const Variant level_range = dber_find(misc_params, "level_range");
			// if (level_range.get_type() != Variant::NIL) item_details->level_range = Vector2(int(Array(level_range)[0]), int(Array(level_range)[1]));
			item_details->sell_value_tier = int(dber_get(misc_params, "sell_value_tier"));
			item_details->fame_value_tier = int(dber_get(misc_params, "fame_value_tier"));
			item_details->base_action_points = int(dber_get(misc_params, "action_points"));
			item_details->bonus_action_points = int(dber_get(misc_params, "bonus_action_points"));
			item_details->base_action_points_cost = int(dber_get(misc_params, "action_points_cost"));

			{ // Item Effectiveness Stats
				const bool randomize_damage_type = bool(entry["randomize_damage_type"]);
				const bool randomize_resistance_type = bool(entry["randomize_resistance_type"]);

				const Dictionary item_effectiveness_stats = dber_get(entry, "item_effectiveness_stats"); // {stat name: value}
				const Array item_effectiveness_stats_names = item_effectiveness_stats.keys();
				for (int x = 0; x < item_effectiveness_stats_names.size(); ++x) {
					const float value = float(item_effectiveness_stats[item_effectiveness_stats_names[x]]);
					if (value == 0.0) continue; // No need to store empty values.

					BG_ContentStat *new_class = memnew(BG_ContentStat);
					new_class->value = value;

					// Store a reference to its relative stat.
					new_class->stat_reference = get_stat_from_stat_id_name(StringName(item_effectiveness_stats_names[x]));

					new_class->randomize_damage_type = randomize_damage_type;
					new_class->randomize_resistance_type = randomize_resistance_type;

					item_details->item_effectiveness_stats.append(new_class);
				}
			}


			{ // Effects
				const Array effects_lines = Array(dber_get(misc_params, "effects"));
				for (int x = 0; x < effects_lines.size(); ++x) {
					const Dictionary effects_entry = effects_lines[x];

					TypedArray<StringName> effect_ids;
					const Array effect_array = Array(dber_get(effects_entry, "effects"));
					for (int y = 0; y < effect_array.size(); ++y) {
						const Dictionary effect_entry = effect_array[y];
						effect_ids.append(StringName(dber_get(effect_entry, "effect")));
					}
					
					const StringName rarity_id = StringName(dber_get(effects_entry, "rarity"));
					item_details->effects[rarity_id] = effect_ids;
				}
			}

			{ // Available Item Slot Types
				const Array available_item_slot_types_lines = Array(dber_get(misc_params, "available_item_slot_types"));
				for (int x = 0; x < available_item_slot_types_lines.size(); ++x) {
					const Dictionary available_item_slot_types_entry = available_item_slot_types_lines[x];

					const int slot_type = int(dber_get(available_item_slot_types_entry, "slot_type"));
					item_details->beast_part_available_item_slot_types.append(global_enums["equipment_slot_types"][slot_type]);
				}
			}
		}
	}

}

// Effects
void BG_Booker_DB::parse_effects(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Effects", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_Effect *new_effect_class = memnew(BG_Effect);
		new_effect_class->id = StringName(dber_get(entry, "id"));
		new_effect_class->nice_name = StringName(dber_get(entry, "name"));
		new_effect_class->use_owning_item_icon = bool(dber_get(entry, "use_owning_item_icon"));
		new_effect_class->status_icon_path = ensure_clean_path(dber_get(entry, "status_icon"));

		// Rarities
		const Array rarity_array = dber_get(entry, "details_per_rarity");
		for (int x = 0; x < rarity_array.size(); ++x) {
			const Dictionary rarity_entry = rarity_array[x];

			BG_EffectRarityDetails *new_rarity_class = memnew(BG_EffectRarityDetails);

			new_rarity_class->rarity_id = StringName(dber_get(rarity_entry, "rarity"));
			new_rarity_class->description = StringName(dber_get(rarity_entry, "description"));
			new_rarity_class->action_points_cost = int(dber_get(rarity_entry, "action_points_cost"));
			new_rarity_class->script_path = ensure_clean_path(dber_get(rarity_entry, "script_path"));

			// Value Attributes
			const Array value_attributes_array = dber_get(rarity_entry, "value_attributes");
			for (int y = 0; y < value_attributes_array.size(); ++y) {
				const Dictionary value_attributes_entry = value_attributes_array[y];

				String name = StringName(dber_get(value_attributes_entry, "name"));
				String value1 = StringName(dber_get(value_attributes_entry, "value_1"));
				String value2 = StringName(dber_get(value_attributes_entry, "value_2"));

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

// Item Drop Pools
void BG_Booker_DB::parse_item_drop_pools(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Item_Drop_Pools", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_ItemDropPool *new_item_drop_pool_class = memnew(BG_ItemDropPool);
		new_item_drop_pool_class->id = StringName(dber_get(entry, "id"));

		// Items
		const Array items_array = dber_get(entry, "items");
		for (int x = 0; x < items_array.size(); ++x) {
			const Dictionary items_entry = items_array[x];

			BG_RewardItem *new_reward_item = memnew(BG_RewardItem);
			new_reward_item->id = StringName(dber_get(items_entry, "item_id"));
			new_reward_item->drop_weight = float(dber_get(items_entry, "drop_weight"));

			// Rarity Availability
			const Array rarity_availability_array = dber_get(items_entry, "rarity_availability");
			for (int x = 0; x < rarity_availability_array.size(); ++x) {
				const Dictionary rarity_availability_entry = rarity_availability_array[x];
				new_reward_item->rarity_availabilities.append(StringName(dber_get(rarity_availability_entry, "rarity_id")));
			}

			new_item_drop_pool_class->item_drops.append(new_reward_item);
		}

		item_drop_pools.append(new_item_drop_pool_class);
	}
}

// Item Slot Types
void BG_Booker_DB::parse_item_slot_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Item_Slot_Types", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_ItemSlotType *new_item_slot_type_class = memnew(BG_ItemSlotType);
		new_item_slot_type_class->id = StringName(dber_get(entry, "id"));
		new_item_slot_type_class->name = StringName(dber_get(entry, "name"));
		new_item_slot_type_class->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));

		// Percentage of all items dropped per act.
		const Array percentage_of_all_items_dropped_per_act_values_array = dber_get(entry, "percentage_of_all_items_dropped_per_act");
		for (int x = 0; x < percentage_of_all_items_dropped_per_act_values_array.size(); ++x) {
			const Dictionary percentage_of_all_items_dropped_per_act_values_entry = percentage_of_all_items_dropped_per_act_values_array[x];
			new_item_slot_type_class->percentage_of_all_items_dropped_per_act.append(
				float(dber_get(percentage_of_all_items_dropped_per_act_values_entry, "percent"))
			);
		}

		item_slot_types.append(new_item_slot_type_class);
	}
}

// Rarity Type
void BG_Booker_DB::parse_rarity_type(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Rarity_Types", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_RarityDetails *new_rarity_type = memnew(BG_RarityDetails);
		new_rarity_type->id = StringName(dber_get(entry, "id"));
		new_rarity_type->equipment_prefix_name = StringName(dber_get(entry, "equipment_prefix_name"));
		new_rarity_type->beast_part_prefix_name = StringName(dber_get(entry, "beast_part_prefix_name"));
		new_rarity_type->color = convert_hex_to_color(dber_get(entry, "color"));
		const float color_muitiplier = float(dber_get(entry, "color_muitiplier"));
		new_rarity_type->color.r *= color_muitiplier;
		new_rarity_type->color.g *= color_muitiplier;
		new_rarity_type->color.b *= color_muitiplier;
		new_rarity_type->damage_multiplier = float(dber_get(entry, "damage_multiplier"));

		// Percentage of all items dropped per act
		const Array percentage_of_all_items_dropped_per_act_values_array = dber_get(entry, "percentage_of_all_items_dropped_per_act");
		for (int x = 0; x < percentage_of_all_items_dropped_per_act_values_array.size(); ++x) {
			const Dictionary percentage_of_all_items_dropped_per_act_values_entry = percentage_of_all_items_dropped_per_act_values_array[x];
			new_rarity_type->percentage_of_all_items_dropped_per_act.append(float(dber_get(percentage_of_all_items_dropped_per_act_values_entry, "percent")));
		}

		rarity_types.append(new_rarity_type);
	}
}

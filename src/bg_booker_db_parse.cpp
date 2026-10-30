// Booker_DB: parsing booker_dber_data.json. try_parse_bder_data runs every sheet parser; the world ones
// (maps, battle boards, objectives, audio, globals, ...) are here, content in bg_booker_db_parse_content.cpp,
// bands, castes and skills in bg_booker_db_parse_units.cpp. try_parse_data is the old format (unused).
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

void BG_Booker_DB::try_parse_bder_data(const String &file_path)
{
	// Kept for the loaders that read sheets later (storyboards, presets, ...), so the file is only parsed once.
	dber_data = BG_JsonUtils::ParseJsonFile(file_path);
	const Dictionary &data = dber_data;

	HashMap<String, TypedArray<StringName>> global_enums = get_global_enums(data);

	// Each sheet's parser (bg_booker_db_parse*.cpp). Keep the order: some use what earlier ones filled in
	// (castes read the stat types).
	parse_global_curves(data, global_enums);
	parse_base_stats(data, global_enums);
	parse_stat_types(data, global_enums);
	parse_animation_classes(data, global_enums);
	parse_content(data, global_enums);
	parse_audio(data, global_enums);
	parse_bands(data, global_enums);
	parse_battle_boards(data, global_enums);
	parse_booker_skill_tree(data, global_enums);
	parse_caste_types(data, global_enums);
	parse_characters(data, global_enums);
	parse_city_info(data, global_enums);
	parse_effects(data, global_enums);
	parse_game_maps(data, global_enums);
	parse_game_map_nodes(data, global_enums);
	parse_icons(data, global_enums);
	parse_item_drop_pools(data, global_enums);
	parse_item_slot_types(data, global_enums);
	parse_mail_data(data, global_enums);
	parse_misc_globals(data, global_enums);
	parse_puzzles(data, global_enums);
	parse_objectives(data, global_enums);
	parse_rarity_type(data, global_enums);
	parse_resource_types(data, global_enums);
	parse_two_der(data, global_enums);

	// Skills last: they fill the attribute trees of the castes parsed above.
	try_parse_skills_data(data.get("skills_data", Dictionary()));

	try_parse_localization_data(data.get("localization_data", Dictionary()));
}

// Global Curves: {curve name: path}
void BG_Booker_DB::parse_global_curves(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Dictionary curves = data.get("global_curves", Dictionary());
	const Array curve_names = curves.keys();
	for (int i = 0; i < curve_names.size(); ++i) {
		globals->global_curves[StringName(curve_names[i])] = ensure_clean_path(curves[curve_names[i]]);
	}
}

// Audio
void BG_Booker_DB::parse_audio(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Audio", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		
		BG_AudioData *new_class = memnew(BG_AudioData);
		new_class->id = StringName(dber_get(entry, "id"));
		
		const Array entry_audios_values = dber_get(entry, "audios");
		for (int x = 0; x < entry_audios_values.size(); ++x) {
			const Dictionary audio_details_entry = entry_audios_values[x];

			BG_AudioDataDetails *new_audio_details = memnew(BG_AudioDataDetails);
			new_audio_details->file_path = ensure_clean_path(dber_get(audio_details_entry, "path"));
			new_audio_details->volume_db_base_value = int(dber_get(audio_details_entry, "volume_db"));
			new_audio_details->random_pitch_scale = float(dber_get(audio_details_entry, "random_pitch_scale"));

			const Array restrict_to_cities = dber_get(audio_details_entry, "restrict_to_cities");
			for (int y = 0; y < restrict_to_cities.size(); ++y) {
				const Dictionary restrict_to_cities_entry = restrict_to_cities[y];
				new_audio_details->restrict_to_acts.append(int(dber_get(restrict_to_cities_entry, "city")));
			}

			new_class->audio_details.append(new_audio_details);
		}

		audio_data.append(new_class);
	}
}

// Battle Boards
void BG_Booker_DB::parse_battle_boards(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Battle_Boards", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		
		BG_BattleBoardDetails *new_class = memnew(BG_BattleBoardDetails);
		new_class->id = StringName(dber_get(entry, "id"));
		new_class->unique_save_name = StringName(dber_get(entry, "unique_save_name"));
		new_class->board_path = ensure_clean_path(dber_get(entry, "board_path"));
		new_class->battle_board_game_script_path = ensure_clean_path(dber_get(entry, "battle_board_game_script_path"));
		new_class->default_hex_visual_path = ensure_clean_path(dber_get(entry, "default_hex_visual_path"));
		new_class->parent_data_id = StringName(dber_get(entry, "parent_data_id"));

		// Hex Types
		const Array hex_types_values_array = dber_get(entry, "hex_types");
		for (int x = 0; x < hex_types_values_array.size(); ++x) {
			const Dictionary hex_types_entry = hex_types_values_array[x];
			
			BG_BattleBoard_HexTypeDetails *new_hex_type_class = memnew(BG_BattleBoard_HexTypeDetails);
			new_hex_type_class->id = StringName(dber_get(hex_types_entry, "id"));
			new_hex_type_class->hex_type = int(dber_get(hex_types_entry, "type"));
			new_hex_type_class->destroyed_vfx_scene_path = ensure_clean_path(dber_get(hex_types_entry, "destroyed_vfx_scene"));
			new_hex_type_class->destroyed_sfx_id = StringName(dber_get(hex_types_entry, "destroyed_sfx"));
			new_hex_type_class->hex_visual_scene_path_override = ensure_clean_path(dber_get(hex_types_entry, "hex_visual_file_path_override"));

			// Settings
			const Array settings_array = dber_get(hex_types_entry, "settings");
			for (int y = 0; y < settings_array.size(); ++y) {
				const Dictionary settings_entry = settings_array[y];

				new_hex_type_class->is_dynamic_type = bool(dber_get(settings_entry, "is_dynamic_type"));
				new_hex_type_class->is_game_type = bool(dber_get(settings_entry, "is_game_type"));
				new_hex_type_class->is_actionable = bool(dber_get(settings_entry, "is_actionable"));
				new_hex_type_class->actionable_requires_move = bool(dber_get(settings_entry, "actionable_requires_move"));
				new_hex_type_class->pass_through_by_ally = bool(dber_get(settings_entry, "pass_through_by_ally"));
				new_hex_type_class->pass_through_by_enemy = bool(dber_get(settings_entry, "pass_through_by_enemy"));
				break; // Should only ever be one entry.
			}

			// Visuals
			const Array visuals_values_array = dber_get(hex_types_entry, "visuals");
			for (int y = 0; y < visuals_values_array.size(); ++y) {
				const Dictionary visuals_entry = visuals_values_array[y];

				BG_BattleBoard_HexTypeVisualDetails *new_hex_type_visuals_class = memnew(BG_BattleBoard_HexTypeVisualDetails);
				new_hex_type_visuals_class->icon_path = ensure_clean_path(dber_get(visuals_entry, "icon_path"));
				new_hex_type_visuals_class->two_der_id = StringName(dber_get(visuals_entry, "two_der_id"));
				new_hex_type_visuals_class->scene_path = ensure_clean_path(dber_get(visuals_entry, "scene_path"));
				new_hex_type_visuals_class->destroyed_icon_path = ensure_clean_path(dber_get(visuals_entry, "destroyed_icon_path"));
				new_hex_type_visuals_class->destroyed_two_der_id = StringName(dber_get(visuals_entry, "destroyed_two_der_id"));
				new_hex_type_visuals_class->destroyed_scene_path = ensure_clean_path(dber_get(visuals_entry, "destroyed_scene_path"));
				new_hex_type_visuals_class->tint = convert_hex_to_color(dber_get(visuals_entry, "tint"));

				new_hex_type_class->visuals.append(new_hex_type_visuals_class);
			}

			{ // Health Effectiveness: {"stat": stat name, "level_value", "value"}
				const Dictionary health_effectiveness = dber_get(hex_types_entry, "health_effectiveness");
				const float value = float(health_effectiveness.get("value", 0.0));
				if (value != 0.0) { // No need to store empty values.
					BG_ContentStat *new_class = memnew(BG_ContentStat);
					new_hex_type_class->health_effectiveness = new_class;
					
					new_class->value = value;
					new_class->level_value = int(health_effectiveness.get("level_value", 0));

					// Store a reference to its relative stat.
					new_class->stat_reference = get_stat_from_stat_id_name(StringName(health_effectiveness.get("stat", "")));
				}
			}

			// Extra Types
			const Array extra_types_array = dber_get(hex_types_entry, "extra_types");
			for (int y = 0; y < extra_types_array.size(); ++y) {
				const Dictionary extra_types_entry = extra_types_array[y];

				const StringName type_name = StringName(dber_get(extra_types_entry, "type_name"));
				const int type_index = int(dber_get(extra_types_entry, "type"));
				new_hex_type_class->extra_types[type_name] = type_index;
			}

			// Equipment
			const Array equipment_values_array = dber_get(hex_types_entry, "equipment");
			for (int y = 0; y < equipment_values_array.size(); ++y) {
				const Dictionary equipment_entry = equipment_values_array[y];
				new_hex_type_class->equipment_ids.append(StringName(dber_get(equipment_entry, "equipment")));
			}

			// Misc Attributes
			const Array misc_attributes_array = dber_get(hex_types_entry, "misc_attributes");
			for (int y = 0; y < misc_attributes_array.size(); ++y) {
				const Dictionary misc_attributes_entry = misc_attributes_array[y];

				const String name = String(dber_get(misc_attributes_entry, "name"));
				const String value1 = String(dber_get(misc_attributes_entry, "value_1"));
				const String value2 = String(dber_get(misc_attributes_entry, "value_2"));

				PackedStringArray values;
				if (!value1.is_empty())
					values.append(value1);
				if (!value2.is_empty())
					values.append(value2);

				new_hex_type_class->misc_attributes[name] = values;
			}

			new_class->hex_types.append(new_hex_type_class);
		}

		// Audio
		const Array audio_array = dber_get(entry, "audio");
		for (int x = 0; x < audio_array.size(); ++x) {
			const Dictionary audio_entry = audio_array[x];
			
			new_class->music_id = StringName(dber_get(audio_entry, "music_id"));
			new_class->ambient_id = StringName(dber_get(audio_entry, "ambient_id"));
			break;
		}

		battle_boards_details.append(new_class);
	}
}

// Booker Skill Tree
void BG_Booker_DB::parse_booker_skill_tree(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Booker_Skill_Tree", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		
		BG_BookerSkillTreeSlotDetails *new_booker_skill_tree_slot_details_class = memnew(BG_BookerSkillTreeSlotDetails);
		new_booker_skill_tree_slot_details_class->id = StringName(dber_get(entry, "id"));
		new_booker_skill_tree_slot_details_class->is_parent_button = bool(dber_get(entry, "is_parent_btn"));
		new_booker_skill_tree_slot_details_class->skill_description_key = StringName(dber_get(entry, "skill_description_key"));
		new_booker_skill_tree_slot_details_class->required_rep = int(dber_get(entry, "required_rep"));
		StringName parent_skill = StringName(dber_get(entry, "parent_skill"));
		if (!parent_skill.is_empty())
			new_booker_skill_tree_slot_details_class->parent_skill_id = parent_skill;
		
		// Value Attributes
		const Array value_attribute_values_array = dber_get(entry, "value_attributes");
		for (int x = 0; x < value_attribute_values_array.size(); ++x) {
			const Dictionary value_attribute_values_entry = value_attribute_values_array[x];

			const float value1 = float(dber_get(value_attribute_values_entry, "value_1"));
			const float value2 = float(dber_get(value_attribute_values_entry, "value_2"));
			new_booker_skill_tree_slot_details_class->value_attributes.append(value1);
			new_booker_skill_tree_slot_details_class->value_attributes.append(value2);
		}

		booker_skill_tree_details.append(new_booker_skill_tree_slot_details_class);
	}
}

// Characters
void BG_Booker_DB::parse_characters(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Characters", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_CharacterDetails *new_char_class = memnew(BG_CharacterDetails);
		new_char_class->id = StringName(dber_get(entry, "id"));

		// Portraits
		const Array portraits_array = dber_get(entry, "portraits");
		for (int x = 0; x < portraits_array.size(); ++x) {
			const Dictionary portraits_entry = portraits_array[x];

			BG_PortraitDetails *new_portrait_class = memnew(BG_PortraitDetails);
			new_portrait_class->portrait_type = static_cast<BG_PortraitDetails::PortraitType>(int(dber_get(portraits_entry, "type")));
			new_portrait_class->icon_path = ensure_clean_path(dber_get(portraits_entry, "icon_path"));

			new_char_class->portraits.append(new_portrait_class);
		}

		character_details.append(new_char_class);
	}
}

// City Info
void BG_Booker_DB::parse_city_info(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("City_Info", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_CityInfo *new_city_info = memnew(BG_CityInfo);
		new_city_info->id = StringName(dber_get(entry, "id"));
		new_city_info->nice_name = StringName(dber_get(entry, "name"));
		new_city_info->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));
		new_city_info->scene_path = ensure_clean_path(dber_get(entry, "scene_path"));
		new_city_info->game_script_path = ensure_clean_path(dber_get(entry, "game_script_path"));
		new_city_info->game_map_id = StringName(dber_get(entry, "game_map_id"));

		// Misc Attributes
		const Array misc_attributes_values_array = dber_get(entry, "misc_attributes");
		for (int x = 0; x < misc_attributes_values_array.size(); ++x) {
			const Dictionary misc_attributes_values_entry = misc_attributes_values_array[x];

			const String name = String(dber_get(misc_attributes_values_entry, "name"));
			const String value1 = String(dber_get(misc_attributes_values_entry, "value_1"));
			const String value2 = String(dber_get(misc_attributes_values_entry, "value_2"));

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

// Game Maps
void BG_Booker_DB::parse_game_maps(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Game_Maps", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_GameMapDetails *new_game_map_class = memnew(BG_GameMapDetails);
		new_game_map_class->id = StringName(dber_get(entry, "id"));
		new_game_map_class->scene_path = ensure_clean_path(dber_get(entry, "scene_path"));

		game_map_details.append(new_game_map_class);
	}
}

// Game Map Nodes
void BG_Booker_DB::parse_game_map_nodes(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Game_Map_Nodes", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_GameMapNodeDetails *new_game_map_node_class = memnew(BG_GameMapNodeDetails);
		new_game_map_node_class->id = StringName(dber_get(entry, "id"));
		new_game_map_node_class->type = int(dber_get(entry, "type"));
		new_game_map_node_class->script_path = ensure_clean_path(dber_get(entry, "script_path"));
		new_game_map_node_class->battle_board_id = StringName(dber_get(entry, "battle_board_id"));
		new_game_map_node_class->storyboard_id_pre = StringName(dber_get(entry, "storyboard_id_pre"));
		new_game_map_node_class->storyboard_id_post = StringName(dber_get(entry, "storyboard_id_post"));

		// Enable Nodes On Complete
		const Array enable_nodes_on_complete_array = dber_get(entry, "enable_nodes_on_complete");
		for (int x = 0; x < enable_nodes_on_complete_array.size(); ++x) {
			const Dictionary enable_nodes_on_complete_entry = enable_nodes_on_complete_array[x];

			new_game_map_node_class->enable_nodes_on_complete[
				StringName(dber_get(enable_nodes_on_complete_entry, "node"))] = bool(dber_get(enable_nodes_on_complete_entry, "enable"));
		}

		game_map_node_details.append(new_game_map_node_class);
	}
}

// Icons
void BG_Booker_DB::parse_icons(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Icons", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_IconDetails *new_icons_class = memnew(BG_IconDetails);
		new_icons_class->id = StringName(dber_get(entry, "id"));
		new_icons_class->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));

		icon_details.append(new_icons_class);
	}
}

// Mail Data
void BG_Booker_DB::parse_mail_data(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Mail_Entries", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_MailData *new_mail_data = memnew(BG_MailData);
		new_mail_data->id = StringName(dber_get(entry, "id"));
		new_mail_data->act = int(dber_get(entry, "act"));
		new_mail_data->week = int(dber_get(entry, "week"));

		mail_data.append(new_mail_data);
	}
}

// Misc Globals
void BG_Booker_DB::parse_misc_globals(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Misc_Globals", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		globals->starting_reputation = int(dber_get(entry, "starting_reputation"));
		globals->inventory_sell_multiplier = float(dber_get(entry, "inventory_sell_multiplier"));
		globals->item_passive_income_multiplier = float(dber_get(entry, "item_passive_income_multiplier"));
		globals->chance_of_no_drop = float(dber_get(entry, "chance_of_no_drop"));

		// Weeks per act & Job globals.
		const Array weeks_per_act_array = dber_get(entry, "weeks_per_act");
		const Array job_globals_array = dber_get(entry, "job_globals");
		for (int x = 0; x < weeks_per_act_array.size(); ++x) {
			const Dictionary weeks_per_act_entry = weeks_per_act_array[x];
			const Dictionary job_globals_entry = job_globals_array[x];

			BG_ActStats *new_act_stats = memnew(BG_ActStats);
			new_act_stats->total_week_count = int(dber_get(weeks_per_act_entry, "weeks_count"));

			if (globals->combat_rounds_per_combat == 0)
				globals->combat_rounds_per_combat = int(dber_get(job_globals_entry, "combat_rounds_per_combat"));

			const Array jobs_distribution_per_act_array = dber_get(job_globals_entry, "jobs_distribution_per_act");
			for (int y = 0; y < jobs_distribution_per_act_array.size(); ++y) {
				const Dictionary jobs_distribution_per_act_entry = jobs_distribution_per_act_array[y];

				new_act_stats->total_job_count = int(dber_get(jobs_distribution_per_act_entry, "job_count"));
				new_act_stats->job_handout_curve_path = StringName(dber_get(jobs_distribution_per_act_entry, "curve"));
			}
			globals->act_stats.append(new_act_stats);
		}

		// QTE Values
		const Array qte_values_array = dber_get(entry, "qte_values");
		for (int x = 0; x < qte_values_array.size(); ++x) {
			const Dictionary qte_values_entry = qte_values_array[x];

			globals->hit_perfect_timing_multiplier = float(dber_get(qte_values_entry, "hit_perfect_timing_multiplier"));
			globals->hit_good_timing_multiplier = float(dber_get(qte_values_entry, "hit_good_timing_multiplier"));
			globals->parry_perfect_timing_multiplier = float(dber_get(qte_values_entry, "parry_perfect_timing_multiplier"));
			globals->parry_good_timing_multiplier = float(dber_get(qte_values_entry, "parry_good_timing_multiplier"));
			globals->crit_qte_perfect_window = float(dber_get(qte_values_entry, "crit_qte_perfect_window"));
			globals->crit_qte_window_additive = float(dber_get(qte_values_entry, "crit_qte_window_additive"));
		}

		// Monster Element Distribution
		const Variant monster_element_distribution = dber_get(entry, "monster_element_distribution");
		const Array monster_element_distribution_array = monster_element_distribution;
		for (int x = 0; x < monster_element_distribution_array.size(); ++x) {
			const Dictionary monster_element_distribution_entry = monster_element_distribution_array[x];

			globals->monster_element_distribution.append(float(dber_get(monster_element_distribution_entry, "percentage")));
		}

		// Max Resistance
		const Array max_resistance_array = dber_get(entry, "max_resistance");
		for (int x = 0; x < max_resistance_array.size(); ++x) {
			const Dictionary max_resistance_entry = max_resistance_array[x];

			globals->max_resistance_soft_cap = int(dber_get(max_resistance_entry, "max_resistance_soft_cap"));
			globals->max_resistance_hard_cap = int(dber_get(max_resistance_entry, "max_resistance_hard_cap"));
		}

		// Damage Resistance Modifiers
		const Array damage_resistance_modifiers_array = dber_get(entry, "damage_resistance_modifiers");
		for (int x = 0; x < damage_resistance_modifiers_array.size(); ++x) {
			const Dictionary damage_resistance_modifiers_entry = damage_resistance_modifiers_array[x];

			globals->percent_amount_to_add_on_same_element_per_damage_value = float(dber_get(damage_resistance_modifiers_entry, "percent_amount_to_add_on_same_element_per_damage_value"));
			globals->percent_amount_to_subtract_on_weak_element_per_damage_value = float(dber_get(damage_resistance_modifiers_entry, "percent_amount_to_subtract_on_weak_element_per_damage_value"));
		}

		// Item Teir Values
		const Array item_teir_values_array = dber_get(entry, "item_teir_values");
		for (int x = 0; x < item_teir_values_array.size(); ++x) {
			const Dictionary item_teir_values_entry = item_teir_values_array[x];

			globals->sell_tier_values.append(int(dber_get(item_teir_values_entry, "sell_tier_values")));
			globals->fame_tier_values.append(int(dber_get(item_teir_values_entry, "fame_tier_values")));
			globals->durability_teir_values.append(int(dber_get(item_teir_values_entry, "durability_teir_values")));
		}

		// Item Teir Rarity Multipliers
		const Array item_teir_rarity_multipliers_array = dber_get(entry, "item_teir_rarity_multipliers");
		for (int x = 0; x < item_teir_rarity_multipliers_array.size(); ++x) {
			const Dictionary item_teir_rarity_multipliers_entry = item_teir_rarity_multipliers_array[x];

			globals->equipment_value_rarity_multiplier.append(float(dber_get(item_teir_rarity_multipliers_entry, "equipment_value_rarity_multiplier")));
			globals->beast_part_value_rarity_multiplier.append(float(dber_get(item_teir_rarity_multipliers_entry, "beast_part_value_rarity_multiplier")));
			globals->extra_beast_part_value_rarity_multiplier_while_grafted.append(float(dber_get(item_teir_rarity_multipliers_entry, "extra_beast_part_value_rarity_multiplier_while_grafted")));
			globals->equipment_durability_rarity_multiplier.append(float(dber_get(item_teir_rarity_multipliers_entry, "equipment_durability_rarity_multiplier")));
			globals->beast_part_durability_rarity_multiplier.append(float(dber_get(item_teir_rarity_multipliers_entry, "beast_part_durability_rarity_multiplier")));
			globals->equipment_fame_rarity_multiplier.append(float(dber_get(item_teir_rarity_multipliers_entry, "equipment_fame_rarity_multiplier")));
			globals->beast_part_fame_rarity_multiplier.append(float(dber_get(item_teir_rarity_multipliers_entry, "beast_part_fame_rarity_multiplier")));
		}

		// Day Night Cycle
		const Array day_night_cycle_array = dber_get(entry, "day_night_cycle");
		for (int x = 0; x < day_night_cycle_array.size(); ++x) {
			const Dictionary day_night_cycle_entry = day_night_cycle_array[x];

			globals->day_night_cycle_turns_per_day = int(dber_get(day_night_cycle_entry, "turns_per_day"));
			// UtilityFunctions::print(globals->day_night_cycle_turns_per_day);
			globals->percentage_of_day_is_day_time = float(dber_get(day_night_cycle_entry, "percentage_of_day_is_day_time"));

			const Variant day_cycle_offset_per_act = dber_get(day_night_cycle_entry, "day_cycle_offset_per_act");
			const Array day_cycle_offset_per_act_array = day_cycle_offset_per_act;
			for (int y = 0; y < day_cycle_offset_per_act_array.size(); ++y) {
				const Dictionary day_cycle_offset_per_act_entry = day_cycle_offset_per_act_array[y];

				globals->day_cycle_offset_per_act.append(int(dber_get(day_cycle_offset_per_act_entry, "offset")));
			}
		}

		// Marketplace
		market_place_data = memnew(BG_MarketplaceData);

		const Array marketplace_array = dber_get(entry, "marketplace");
		for (int x = 0; x < marketplace_array.size(); ++x) {
			const Dictionary marketplace_entry = marketplace_array[x];

			const Array items_per_month_array = dber_get(marketplace_entry, "items_per_month");
			for (int y = 0; y < items_per_month_array.size(); ++y) {
				const Dictionary items_per_month_entry = items_per_month_array[y];

				market_place_data->items_per_month_min_max = Vector2i(
					int(dber_get(items_per_month_entry, "min")), 
					int(dber_get(items_per_month_entry, "max"))
				);
			}

			// Entries
			const Variant entries = dber_get(marketplace_entry, "entries");
			const Array entries_array = entries;
			for (int x = 0; x < entries_array.size(); ++x) {
				const Dictionary entries_entry = entries_array[x];

				BG_MarketplaceEntryData *new_marketplace_entry_data = memnew(BG_MarketplaceEntryData);
				new_marketplace_entry_data->act = int(dber_get(entries_entry, "act"));
				new_marketplace_entry_data->starting_month = int(dber_get(entries_entry, "starting_month"));

				const Array item_drop_pools_array = dber_get(entries_entry, "item_drop_pools");
				for (int y = 0; y < item_drop_pools_array.size(); ++y) {
					const Dictionary item_drop_pools_entry = item_drop_pools_array[y];

					new_marketplace_entry_data->item_drop_pool_ids.append(
						StringName(dber_get(item_drop_pools_entry, "pool"))
					);
				}
				market_place_data->entries.append(new_marketplace_entry_data);
			}
		}

		// Action Points
		const Array action_points_array = dber_get(entry, "action_points");
		for (int x = 0; x < action_points_array.size(); ++x) {
			const Dictionary action_points_entry = action_points_array[x];

			globals->band_action_points_fatigue_rate = float(dber_get(action_points_entry, "band_action_points_fatigue_rate"));
			globals->monster_action_points_fatigue_rate = float(dber_get(action_points_entry, "monster_action_points_fatigue_rate"));
			globals->action_points_rest_rate = float(dber_get(action_points_entry, "action_points_rest_rate"));
		}

		// Experience Points
		const Array experience_points_array = dber_get(entry, "experience_points");
		for (int x = 0; x < experience_points_array.size(); ++x) {
			const Dictionary experience_points_entry = experience_points_array[x];

			globals->exp_when_source_unit_damaged = int(dber_get(experience_points_entry, "exp_when_source_unit_damaged"));
			globals->exp_when_enemy_killed = int(dber_get(experience_points_entry, "exp_when_enemy_killed"));
			globals->exp_when_ap_consumed = int(dber_get(experience_points_entry, "exp_when_ap_consumed"));
		}

		// Level Up
		const Array level_up_array = dber_get(entry, "level_up");
		for (int x = 0; x < level_up_array.size(); ++x) {
			const Dictionary level_up_entry = level_up_array[x];

			globals->attribute_points_per_level = int(dber_get(level_up_entry, "attribute_points_per_level"));
		}
	}
}

// Puzzles
void BG_Booker_DB::parse_puzzles(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Puzzles", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_PuzzleDetails *new_data = memnew(BG_PuzzleDetails);
		new_data->id = StringName(dber_get(entry, "id"));
		new_data->scene_path = ensure_clean_path(dber_get(entry, "scene_path"));

		// Misc Params
		const Array misc_params_array = dber_get(entry, "misc_params");
		for (int x = 0; x < misc_params_array.size(); ++x) {
			const Dictionary misc_params_entry = misc_params_array[x];

			BG_PuzzleDetails_MiscParams *new_params_data = memnew(BG_PuzzleDetails_MiscParams);
			new_params_data->param_name = StringName(dber_get(misc_params_entry, "param_name"));
			new_params_data->value1 = StringName(dber_get(misc_params_entry, "value1"));
			new_params_data->value2 = StringName(dber_get(misc_params_entry, "value2"));

			new_data->misc_params.append(new_params_data);
		}

		puzzles.append(new_data);
	}
}

// Objectives
void BG_Booker_DB::parse_objectives(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Objectives", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		
		BG_ObjectiveDetails *new_objective_class = memnew(BG_ObjectiveDetails);
		new_objective_class->id = StringName(dber_get(entry, "id"));
		new_objective_class->localization_key = StringName(dber_get(entry, "localization_key"));
		if (new_objective_class->localization_key.is_empty()) {
			new_objective_class->localization_key = new_objective_class->id;
		}
		new_objective_class->is_main_objective = bool(dber_get(entry, "is_main_objective"));
		new_objective_class->is_scripted = bool(dber_get(entry, "is_scripted"));
		new_objective_class->objective_repeatable_type = static_cast<BG_ObjectiveDetails::ObjectiveRepeatableType>(int(dber_get(entry, "repeatable_type")));
		new_objective_class->is_auto_complete = bool(dber_get(entry, "is_auto_complete"));
		new_objective_class->expires_in = int(dber_get(entry, "expires_in"));
		new_objective_class->is_turn_in = bool(dber_get(entry, "is_turn_in"));
		new_objective_class->is_event = bool(dber_get(entry, "is_event"));
		new_objective_class->script_path = ensure_clean_path(dber_get(entry, "script_path"));

		// Rewards
		const Array rewards_array = dber_get(entry, "rewards");
		for (int x = 0; x < rewards_array.size(); ++x) {
			const Dictionary rewards_entry = rewards_array[x];

			new_objective_class->reputation_drop = int(dber_get(rewards_entry, "reputation"));
			new_objective_class->maelstrite_drop = int(dber_get(rewards_entry, "maelstrite"));
			new_objective_class->beast_part_drop_count = int(dber_get(rewards_entry, "beast_part_drop_count"));
			new_objective_class->equipment_drop_count = int(dber_get(rewards_entry, "equipment_drop_count"));

			const StringName item_drop_pool_id = StringName(dber_get(rewards_entry, "item_drop_pool"));
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

					const Array forced_rarity_availabilities_lines = dber_get(rewards_entry, "forced_rarity_availabilities");
					if (forced_rarity_availabilities_lines.size() > 0)
					{
						for (int f = 0; f < forced_rarity_availabilities_lines.size(); f++)
						{
							const Dictionary forced_rarity_availabilities_entry = forced_rarity_availabilities_lines[f];
							new_job_reward_item_class->rarity_availabilities.append(global_enums["rarity_types"][int(dber_get(forced_rarity_availabilities_entry, "rarity_id"))]);
						}
					}
					else
						new_job_reward_item_class->rarity_availabilities = reward_item->rarity_availabilities.duplicate();

					new_objective_class->drops.append(new_job_reward_item_class);
				}
				break;
			}
		}

		// Timeline
		const Array timeline_array = dber_get(entry, "timeline");
		for (int x = 0; x < timeline_array.size(); ++x) {
			const Dictionary timeline_entry = timeline_array[x];

				BG_ObjectiveTimeline *new_objective_timeline_class = memnew(BG_ObjectiveTimeline);
				new_objective_timeline_class->act = int(dber_get(timeline_entry, "act"));
				new_objective_timeline_class->min_week = int(dber_get(timeline_entry, "min_week"));
				new_objective_timeline_class->max_week = int(dber_get(timeline_entry, "max_week"));

				new_objective_class->timeline = new_objective_timeline_class;
				break;
		}

		// Misc Attributes
		const Array misc_attributes_array = dber_get(entry, "misc_attributes");
		for (int x = 0; x < misc_attributes_array.size(); ++x) {
			const Dictionary misc_attributes_entry = misc_attributes_array[x];

			String name = dber_get(misc_attributes_entry, "name");
			String value1 = dber_get(misc_attributes_entry, "value_1");
			String value2 = dber_get(misc_attributes_entry, "value_2");

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

// Resource Types
void BG_Booker_DB::parse_resource_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Resource_Types", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		BG_ResourceTypeDetails *new_resource_type_class = memnew(BG_ResourceTypeDetails);
		new_resource_type_class->id = StringName(dber_get(entry, "id"));
		new_resource_type_class->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));

		resource_type_details[new_resource_type_class->id] = new_resource_type_class;
	}
}

// 2der
void BG_Booker_DB::parse_two_der(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
	const Array lines = get_sheet_by_name("Two_Der", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];
		BG_TwoDer_DataEntry *new_twoder_class = memnew(BG_TwoDer_DataEntry);

		new_twoder_class->id = StringName(dber_get(entry, "id"));
		new_twoder_class->data_path = ensure_clean_path(dber_get(entry, "data_path"));
		new_twoder_class->asset_name = StringName(dber_get(entry, "asset_name"));
		new_twoder_class->prop_name = StringName(dber_get(entry, "prop_name"));
		new_twoder_class->anim_name = StringName(dber_get(entry, "anim_name"));
		new_twoder_class->camera_name = StringName(dber_get(entry, "camera_name"));
		new_twoder_class->anim_speed_mult = float(dber_get(entry, "anim_speed_mult"));
		new_twoder_class->scale_multiplier = float(dber_get(entry, "scale_multiplier"));

		two_der_data_entries.append(new_twoder_class);
	}
}

void BG_Booker_DB::try_parse_data(const String &file_path)
{
	const Dictionary data = BG_JsonUtils::ParseJsonFile(file_path);

	//UtilityFunctions::print(BG_JsonUtils::GetCBDSheet(data, "globals"));

	/////
	///// Battle Board Events
	/////
	{
		const Dictionary jobs_sheet = BG_JsonUtils::GetCBDSheet(data, "battle_board_events");
		if (jobs_sheet.has("lines"))
		{
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
}

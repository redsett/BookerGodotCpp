// Booker_DB: bindings, getters, refresh, the singleton, freeing, localization lookups.
// Parsing is in bg_booker_db_parse*.cpp, preset bands/monster groups in bg_booker_db_presets.cpp, the data
// classes in bg_booker_types_*.cpp.
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
//// BG_Booker_DB
////
BG_Booker_DB *BG_Booker_DB::singleton = nullptr;

void BG_Booker_DB::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("refreshed_data"));

	ClassDB::bind_method(D_METHOD("refresh_data"), &BG_Booker_DB::refresh_data);
	ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("timer_test"), &BG_Booker_DB::timer_test);

    ADD_SIGNAL(MethodInfo("monster_stoned_changed",
        PropertyInfo(Variant::OBJECT, "monster"),
        PropertyInfo(Variant::BOOL, "is_stoned")
    ));

	ClassDB::bind_method(D_METHOD("get_current_language_key"), &BG_Booker_DB::get_current_language_key);
	ClassDB::bind_method(D_METHOD("set_current_language_key"), &BG_Booker_DB::set_current_language_key);
	ClassDB::bind_method(D_METHOD("get_modding_path"), &BG_Booker_DB::get_modding_path);
	ClassDB::bind_method(D_METHOD("get_globals"), &BG_Booker_DB::get_globals);
	ClassDB::bind_method(D_METHOD("get_character_details"), &BG_Booker_DB::get_character_details);
	ClassDB::bind_method(D_METHOD("get_character_details_by_id", "id"), &BG_Booker_DB::get_character_details_by_id);
	ClassDB::bind_method(D_METHOD("get_base_stats"), &BG_Booker_DB::get_base_stats);
	ClassDB::bind_method(D_METHOD("import_and_get_storyboard_details_by_id", "id"), &BG_Booker_DB::import_and_get_storyboard_details_by_id);
	ClassDB::bind_method(D_METHOD("import_and_get_tutorial_cards_details_by_id", "id"), &BG_Booker_DB::import_and_get_tutorial_cards_details_by_id);
	ClassDB::bind_method(D_METHOD("get_objectives"), &BG_Booker_DB::get_objectives);
	ClassDB::bind_method(D_METHOD("get_battle_boards_details"), &BG_Booker_DB::get_battle_boards_details);
	ClassDB::bind_method(D_METHOD("get_battle_board_by_id", "id"), &BG_Booker_DB::get_battle_board_by_id);
	ClassDB::bind_method(D_METHOD("get_battle_board_details_for_bb", "bb_id"), &BG_Booker_DB::get_battle_board_details_for_bb);
	ClassDB::bind_method(D_METHOD("get_battle_board_hex_types_by_type", "bb_id", "type"), &BG_Booker_DB::get_battle_board_hex_types_by_type);
	ClassDB::bind_method(D_METHOD("get_battle_board_hex_type_by_id", "bb_id", "id"), &BG_Booker_DB::get_battle_board_hex_type_by_id);
	ClassDB::bind_method(D_METHOD("get_battle_board_music_id", "bb_id"), &BG_Booker_DB::get_battle_board_music_id);
	ClassDB::bind_method(D_METHOD("get_battle_board_ambient_id", "bb_id"), &BG_Booker_DB::get_battle_board_ambient_id);
	ClassDB::bind_method(D_METHOD("get_game_map_node_details"), &BG_Booker_DB::get_game_map_node_details);
	ClassDB::bind_method(D_METHOD("get_game_map_node_details_by_id", "id"), &BG_Booker_DB::get_game_map_node_details_by_id);
	ClassDB::bind_method(D_METHOD("get_game_map_details"), &BG_Booker_DB::get_game_map_details);
	ClassDB::bind_method(D_METHOD("get_game_map_details_by_id", "id"), &BG_Booker_DB::get_game_map_details_by_id);
	ClassDB::bind_method(D_METHOD("get_resource_type_details_by_id", "resource_id"), &BG_Booker_DB::get_resource_type_details_by_id);
	ClassDB::bind_method(D_METHOD("get_audio_data"), &BG_Booker_DB::get_audio_data);
	ClassDB::bind_method(D_METHOD("get_audio_details", "id", "act"), &BG_Booker_DB::get_audio_details);
	ClassDB::bind_method(D_METHOD("get_icon_details"), &BG_Booker_DB::get_icon_details);
	ClassDB::bind_method(D_METHOD("get_icon_details_by_id", "id"), &BG_Booker_DB::get_icon_details_by_id);
	ClassDB::bind_method(D_METHOD("get_booker_skill_tree_details"), &BG_Booker_DB::get_booker_skill_tree_details);
	ClassDB::bind_method(D_METHOD("get_jobs"), &BG_Booker_DB::get_jobs);
	ClassDB::bind_method(D_METHOD("get_items"), &BG_Booker_DB::get_items);
	ClassDB::bind_method(D_METHOD("get_item_max_durability", "item", "full_value"), &BG_Booker_DB::get_item_max_durability);
	ClassDB::bind_method(D_METHOD("get_item_details_by_id"), &BG_Booker_DB::get_item_details_by_id);
	ClassDB::bind_method(D_METHOD("get_animation_class_animations", "id"), &BG_Booker_DB::get_animation_class_animations);
	ClassDB::bind_method(D_METHOD("get_weapon_categories"), &BG_Booker_DB::get_weapon_categories);
	ClassDB::bind_method(D_METHOD("get_armor_categories"), &BG_Booker_DB::get_armor_categories);
	ClassDB::bind_method(D_METHOD("get_sheet_rows", "sheet_name"), &BG_Booker_DB::get_sheet_rows);
	ClassDB::bind_method(D_METHOD("get_global_enum_values", "enum_name"), &BG_Booker_DB::get_global_enum_values);
	ClassDB::bind_method(D_METHOD("create_preset_item_by_id", "id"), &BG_Booker_DB::create_preset_item_by_id);
	ClassDB::bind_method(D_METHOD("get_item_drop_pools"), &BG_Booker_DB::get_item_drop_pools);
	ClassDB::bind_method(D_METHOD("get_item_drop_pool_by_id"), &BG_Booker_DB::get_item_drop_pool_by_id);
	ClassDB::bind_method(D_METHOD("get_effects"), &BG_Booker_DB::get_effects);
	ClassDB::bind_method(D_METHOD("get_band_info"), &BG_Booker_DB::get_band_info);
	ClassDB::bind_method(D_METHOD("get_skills_data"), &BG_Booker_DB::get_skills_data);
	ClassDB::bind_method(D_METHOD("create_preset_band_by_id", "id", "existing_bands"), &BG_Booker_DB::create_preset_band_by_id);
	ClassDB::bind_method(D_METHOD("get_preset_band_params_by_id", "id"), &BG_Booker_DB::get_preset_band_params_by_id);
	ClassDB::bind_method(D_METHOD("get_item_slot_types"), &BG_Booker_DB::get_item_slot_types);
	ClassDB::bind_method(D_METHOD("get_rarity_types"), &BG_Booker_DB::get_rarity_types);
	ClassDB::bind_method(D_METHOD("get_rarity_index", "id"), &BG_Booker_DB::get_rarity_index);
	ClassDB::bind_method(D_METHOD("get_stat_types"), &BG_Booker_DB::get_stat_types);
	ClassDB::bind_method(D_METHOD("get_market_place_data"), &BG_Booker_DB::get_market_place_data);
	ClassDB::bind_method(D_METHOD("get_monster_types"), &BG_Booker_DB::get_monster_types);
	ClassDB::bind_method(D_METHOD("get_monster_by_id", "id"), &BG_Booker_DB::get_monster_by_id);
	ClassDB::bind_method(D_METHOD("create_preset_monster_group_by_id", "id"), &BG_Booker_DB::create_preset_monster_group_by_id);
	ClassDB::bind_method(D_METHOD("get_drop_rewards_from_monster_group_preset", "job"), &BG_Booker_DB::get_drop_rewards_from_monster_group_preset);
	ClassDB::bind_method(D_METHOD("get_preset_monster_group_params_by_id", "id"), &BG_Booker_DB::get_preset_monster_group_params_by_id);
	ClassDB::bind_method(D_METHOD("get_mail_data"), &BG_Booker_DB::get_mail_data);
	ClassDB::bind_method(D_METHOD("get_puzzles"), &BG_Booker_DB::get_puzzles);
	ClassDB::bind_method(D_METHOD("get_puzzle_details_by_id", "id"), &BG_Booker_DB::get_puzzle_details_by_id);
	ClassDB::bind_method(D_METHOD("get_two_der_data_entries"), &BG_Booker_DB::get_two_der_data_entries);
	ClassDB::bind_method(D_METHOD("get_two_der_data_from_id", "id"), &BG_Booker_DB::get_two_der_data_from_id);
	ClassDB::bind_method(D_METHOD("set_revert_localization_to_english"), &BG_Booker_DB::set_revert_localization_to_english);
	ClassDB::bind_method(D_METHOD("get_localize_data", "sheet_name", "key", "language"), &BG_Booker_DB::get_localize_data);
	ClassDB::bind_method(D_METHOD("get_localize_string", "sheet_name", "key", "language", "ignore_code_data"), &BG_Booker_DB::get_localize_string);
	ClassDB::bind_method(D_METHOD("get_stat_from_stat_id_name", "stat_id_name"), &BG_Booker_DB::get_stat_from_stat_id_name);
}

BG_CharacterDetails *BG_Booker_DB::get_character_details_by_id(const StringName &id) const
{
	for (int i = 0; i < character_details.size(); ++i) {
		BG_CharacterDetails *cd = cast_to<BG_CharacterDetails>(character_details[i]);
		if (cd->get_id() == id)
			return cd;
	}
	return nullptr;
}

Ref<BG_StoryboardDetails> BG_Booker_DB::import_and_get_storyboard_details_by_id(const StringName &id)
{
	const Dictionary &data = dber_data;

	Ref<BG_StoryboardDetails> result = memnew(BG_StoryboardDetails);

	// Storyboards
	const Array lines = get_sheet_by_name("Storyboards", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName sb_id = StringName(dber_get(entry, "id"));
		if (sb_id != id) continue;

		result->id = sb_id;
		result->localization_sheet = StringName(dber_get(entry, "localization_sheet"));

		// Data
		const Array data_array = dber_get(entry, "data");
		for (int x = 0; x < data_array.size(); ++x) {
			const Dictionary data_entry = data_array[x];

			Ref<BG_StoryboardDataDetails> sb_dets = memnew(BG_StoryboardDataDetails);
			sb_dets->character_key = StringName(dber_get(data_entry, "character_key"));
			sb_dets->character_emotion = static_cast<BG_PortraitDetails::PortraitType>(int(dber_get(data_entry, "character_emotion")));
			sb_dets->text_key = StringName(UtilityFunctions::str(int(dber_get(data_entry, "text_key"))));
			
			sb_dets->code = StringName(dber_get(data_entry, "code"));
			sb_dets->texture_path = ensure_clean_path(dber_get(data_entry, "texture_path"));

			// Audio IDs
			const Array audio_ids_array = dber_get(data_entry, "audio_ids");
			for (int x = 0; x < audio_ids_array.size(); ++x) {
				const Dictionary audio_ids_entry = audio_ids_array[x];

				sb_dets->audio_ids.append( StringName(dber_get(audio_ids_entry, "audio_id")) );
			}		

			result->data.append(sb_dets);
		}

		// Character Text Locations
		const Array character_text_locations_array = dber_get(entry, "character_text_locations");
		for (int x = 0; x < character_text_locations_array.size(); ++x) {
			const Dictionary character_text_locations_entry = character_text_locations_array[x];

			Ref<BG_StoryboardCharacterTextLocationDetails> sb_dets = memnew(BG_StoryboardCharacterTextLocationDetails);
			sb_dets->character_key = StringName(dber_get(character_text_locations_entry, "character_key"));

			// Text Locations
			const Array txt_locs_array = dber_get(character_text_locations_entry, "txt_locations");
			for (int x = 0; x < txt_locs_array.size(); ++x) {
				const Dictionary txt_locs_entry = txt_locs_array[x];

				const Variant txt_location = dber_get(txt_locs_entry, "txt_location");
				sb_dets->txt_locations.append(Vector2(float(Array(txt_location)[0]), float(Array(txt_location)[1])));
			}

			result->character_text_locations.append(sb_dets);
		}
		break;
	}

	if (!result->get_id().is_empty())
		return result;
	return nullptr;
}

Ref<BG_TutorialCardsDetails> BG_Booker_DB::import_and_get_tutorial_cards_details_by_id(const StringName &id)
{
	const Dictionary &data = dber_data;

	Ref<BG_TutorialCardsDetails> result = memnew(BG_TutorialCardsDetails);

	// Tutorial Cards
	const Array lines = get_sheet_by_name("Tutorial_Cards", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName sb_id = StringName(dber_get(entry, "id"));
		if (sb_id != id) continue;

		result->id = sb_id;

		// Cards
		const Array cards_array = dber_get(entry, "cards");
		for (int x = 0; x < cards_array.size(); ++x) {
			const Dictionary card_entry = cards_array[x];

			Ref<BG_TutorialCardDetails> t_dets = memnew(BG_TutorialCardDetails);
			t_dets->description_key = StringName(dber_get(card_entry, "description_key"));
			t_dets->image_path = ensure_clean_path(dber_get(card_entry, "image_path"));

			result->cards.append(t_dets);
		}
	}

	if (!result->get_id().is_empty())
		return result;
	return nullptr;
}

BG_BattleBoardDetails *BG_Booker_DB::get_battle_board_by_id(const StringName &id) const {
	for (int i = 0; i < battle_boards_details.size(); ++i) {
		BG_BattleBoardDetails *bb = cast_to<BG_BattleBoardDetails>(battle_boards_details[i]);
		if (bb->get_id() == id)
			return bb;
	}
	return nullptr;
}

TypedArray<BG_BattleBoard_HexTypeDetails> BG_Booker_DB::get_battle_board_details_for_bb(const StringName &bb_id) const
{
	HashMap<StringName, TypedArray<BG_BattleBoard_HexTypeDetails>> sorted;
	get_battle_board_details_for_bb_from_sorted(bb_id, sorted);

	TypedArray<BG_BattleBoard_HexTypeDetails> result;
	for (const auto &pair : sorted) {
		result.append_array(pair.value);
	}

	return result;
}

void BG_Booker_DB::get_battle_board_details_for_bb_from_sorted(const StringName &bb_id, HashMap<StringName, TypedArray<BG_BattleBoard_HexTypeDetails>> &sorted) const
{
	const BG_BattleBoardDetails *bb = get_battle_board_by_id(bb_id);
	if (bb != nullptr && !bb->get_parent_data_id().is_empty()) {
		get_battle_board_details_for_bb_from_sorted(bb->get_parent_data_id(), sorted);
	}

	// Add battle board details. If the child has an existing Id from the parent, then override it.
	if (bb != nullptr) {
		TypedArray<StringName> child_ids_hit;
		for (int i = 0; i < bb->get_hex_types().size(); ++i) {
			BG_BattleBoard_HexTypeDetails *hex_type_details = cast_to<BG_BattleBoard_HexTypeDetails>(bb->get_hex_types()[i]);
			if (!sorted.has(hex_type_details->get_id())) {
				TypedArray<BG_BattleBoard_HexTypeDetails> a;
				sorted[hex_type_details->get_id()] = a;
			} else if (!child_ids_hit.has(hex_type_details->get_id())) {
				sorted[hex_type_details->get_id()].clear();
			}
			sorted[hex_type_details->get_id()].append(hex_type_details);
			child_ids_hit.append(hex_type_details->get_id());
		}
	}
}

TypedArray<BG_BattleBoard_HexTypeDetails> BG_Booker_DB::get_battle_board_hex_types_by_type(const StringName &bb_id, int type, bool is_game_type) const {
	TypedArray<BG_BattleBoard_HexTypeDetails> result;
	const BG_BattleBoardDetails *bb = get_battle_board_by_id(bb_id);
	if (bb != nullptr) {
		result = bb->get_hex_types_by_type(type, is_game_type);

		if (result.is_empty() && !bb->get_parent_data_id().is_empty()) {
			result = get_battle_board_hex_types_by_type(bb->get_parent_data_id(), type, is_game_type);
		}
	}
	return result;
}

BG_BattleBoard_HexTypeDetails *BG_Booker_DB::get_battle_board_hex_type_by_id(const StringName &bb_id, const StringName &id) const {
	BG_BattleBoard_HexTypeDetails *result = nullptr;
	const BG_BattleBoardDetails *bb = get_battle_board_by_id(bb_id);
	if (bb != nullptr) {
		result = bb->get_hex_type_by_id(id);

		if (result == nullptr && !bb->get_parent_data_id().is_empty()) {
			result = get_battle_board_hex_type_by_id(bb->get_parent_data_id(), id);
		}
	}
	// if (result == nullptr) {
	// 	UtilityFunctions::prints(parent_bb_id, bb_id, id);
	// 	UtilityFunctions::prints("uh oh");
	// }
	return result;
}

StringName BG_Booker_DB::get_battle_board_music_id(const StringName &bb_id) const
{
	StringName result;
	const BG_BattleBoardDetails *bb = get_battle_board_by_id(bb_id);
	if (bb != nullptr) {
		result = bb->get_music_id();
		if (result.is_empty() && !bb->get_parent_data_id().is_empty()) {
			result = get_battle_board_music_id(bb->get_parent_data_id());
		}
	}
	return result;
}
StringName BG_Booker_DB::get_battle_board_ambient_id(const StringName &bb_id) const
{
	StringName result;
	const BG_BattleBoardDetails *bb = get_battle_board_by_id(bb_id);
	if (bb != nullptr) {
		result = bb->get_ambient_id();
		if (result.is_empty() && !bb->get_parent_data_id().is_empty()) {
			result = get_battle_board_ambient_id(bb->get_parent_data_id());
		}
	}
	return result;
}

BG_GameMapNodeDetails *BG_Booker_DB::get_game_map_node_details_by_id(const StringName &id) const {
	for (int i = 0; i < game_map_node_details.size(); ++i) {
		BG_GameMapNodeDetails *gmd = cast_to<BG_GameMapNodeDetails>(game_map_node_details[i]);
		if (gmd->get_id() == id)
			return gmd;
	}
	return nullptr;
}

BG_GameMapDetails *BG_Booker_DB::get_game_map_details_by_id(const StringName &id) const {
	for (int i = 0; i < game_map_details.size(); ++i) {
		BG_GameMapDetails *gmd = cast_to<BG_GameMapDetails>(game_map_details[i]);
		if (gmd->get_id() == id)
			return gmd;
	}
	return nullptr;
}

BG_ResourceTypeDetails *BG_Booker_DB::get_resource_type_details_by_id(const StringName &resource_id) const {
	if (resource_type_details.has(resource_id)) {
		return resource_type_details[resource_id];
	}
	return nullptr;
}

BG_ItemDropPool *BG_Booker_DB::get_item_drop_pool_by_id(const StringName &id) const
{
	for (int i = 0; i < item_drop_pools.size(); ++i) {
		BG_ItemDropPool *idp = cast_to<BG_ItemDropPool>(item_drop_pools[i]);
		if (idp->id == id)
			return idp;
	}
	return nullptr;
}

TypedArray<BG_AudioDataDetails> BG_Booker_DB::get_audio_details(const StringName &id, int act) const {
	TypedArray<BG_AudioDataDetails> result;
	for (int i = 0; i < audio_data.size(); ++i) {
		BG_AudioData *ad = cast_to<BG_AudioData>(audio_data[i]);
		if (ad->id != id) continue;
		for (int x = 0; x < ad->audio_details.size(); ++x) {
			BG_AudioDataDetails *audio_details = cast_to<BG_AudioDataDetails>(ad->audio_details[x]);
			if (audio_details->restrict_to_acts.is_empty() || audio_details->restrict_to_acts.has(act)) {
				result.append(audio_details);
			}
		}
		break;
	}
	return result;
}

BG_IconDetails *BG_Booker_DB::get_icon_details_by_id(const StringName &id) const {
	for (int i = 0; i < icon_details.size(); ++i) {
		BG_IconDetails *icon_dets = cast_to<BG_IconDetails>(icon_details[i]);
		if (icon_dets->get_id() == id)
			return icon_dets;
	}
	return nullptr;
}

BG_ItemDetails *BG_Booker_DB::get_item_details_by_id(const StringName &id) const {
	for (int i = 0; i < items.size(); ++i) {
		BG_ItemDetails *dets = cast_to<BG_ItemDetails>(items[i]);
		if (dets->get_id() == id)
			return dets;
	}
	return nullptr;
}

Array BG_Booker_DB::get_global_enum_values(const String &enum_name) const {
	const Dictionary enums = dber_data.get("global_enums", Dictionary());
	return enums.get(enum_name, Array());
}

Array BG_Booker_DB::get_sheet_rows(const String &sheet_name) const {
	const Dictionary sheets = dber_data.get("sheets", Dictionary());
	return sheets.get(sheet_name, Array());
}

TypedArray<BG_AnimationDetails> BG_Booker_DB::get_animation_class_animations(const StringName &id) const {
	if (!animation_classes.has(id)) return TypedArray<BG_AnimationDetails>();
	return animation_classes[id];
}

float BG_Booker_DB::get_item_max_durability(Ref<BG_Item> item, bool full_value) const {
	if (!item.is_valid()) return 0.0;
	BG_ItemDetails *item_details = get_item_details_by_id(item->get_id());
	if (item_details == nullptr) return 0.0;

	float result = float( get_globals()->get_durability_teir_values()[item_details->get_durability_value_tier()] );
	if (item_details->is_beast_part()) {
		result *= float( get_globals()->get_beast_part_durability_rarity_multiplier()[get_rarity_index(item->get_rarity_id())] );
	} else {
		result *= float( get_globals()->get_equipment_durability_rarity_multiplier()[get_rarity_index(item->get_rarity_id())] );
	}
	
	if (!full_value)
		return result;
	
	for (int i = 0; i < item->get_grafts().size(); ++i) {
		Ref<BG_Item> g = cast_to<BG_Item>(item->get_grafts()[i]);
		if (!g.is_valid()) continue;
		BG_ItemDetails *graft_item_details = get_item_details_by_id(g->get_id());
		float graft_value = float( get_globals()->get_durability_teir_values()[graft_item_details->get_durability_value_tier()] );
		graft_value *= float( get_globals()->get_beast_part_durability_rarity_multiplier()[get_rarity_index(g->get_rarity_id())] );
		result += graft_value;
	}
	
	return result;
}


BG_PuzzleDetails *BG_Booker_DB::get_puzzle_details_by_id(const StringName &id) const {
	for (int i = 0; i < puzzles.size(); ++i) {
		BG_PuzzleDetails *puzzle_dets = cast_to<BG_PuzzleDetails>(puzzles[i]);
		if (puzzle_dets != nullptr && puzzle_dets->get_id() == id)
			return puzzle_dets;
	}
	return nullptr;
}

BG_TwoDer_DataEntry *BG_Booker_DB::get_two_der_data_from_id(const StringName &id) const {
	for (int i = 0; i < two_der_data_entries.size(); ++i) {
		BG_TwoDer_DataEntry *data = cast_to<BG_TwoDer_DataEntry>(two_der_data_entries[i]);
		if (data == nullptr) continue;
		if (data->get_id() == id) return data;
	}
	return nullptr;
}

BG_BaseStat *BG_Booker_DB::get_stat_from_stat_id_name(const StringName &stat_id_name) const {
	for (int i = 0; i < base_stats.size(); ++i) {
		BG_BaseStat *stat = cast_to<BG_BaseStat>(base_stats[i]);
		if (stat == nullptr) continue;

		if (stat->stat_id_name == stat_id_name) return stat;
	}
	return nullptr;
}

BG_BaseStat *BG_Booker_DB::get_stat_from_unique_id(int unique_id) const {
	for (int i = 0; i < base_stats.size(); ++i) {
		BG_BaseStat *stat = cast_to<BG_BaseStat>(base_stats[i]);
		if (stat == nullptr) continue;

		if (stat->unique_id == unique_id) return stat;
	}
	return nullptr;
}

BG_Booker_DB *BG_Booker_DB::get_singleton()
{
	return singleton;
}

void BG_Booker_DB::refresh_data()
{
	// static bool has_done_once = false;
	// if (has_done_once) return;
	// has_done_once = true;
	// UtilityFunctions::print("BG_Booker_DB::refresh_data()");
	free_all_params();
	globals = memnew(BG_Booker_Globals);
	band_info = memnew(BG_BandInfo);

	// const String booker_data_file_name = "bookerData.cdb";
	const String exe_path = OS::get_singleton()->get_executable_path().get_base_dir() + "/";
	modding_path = exe_path + String("mod/");
	// const String modding_data_path = modding_path + booker_data_file_name;

	// try_parse_data("res://" + booker_data_file_name);

	// If the mod data exists, then let it override any data that it has.
	// if (FileAccess::file_exists(modding_data_path))
	// {
	// 	UtilityFunctions::print("Log - Using modding booker data.");
	// 	try_parse_data(modding_data_path);
	// }

	//
	// Parse Booker DBer data.
	//
	const String modding_dber_data_path = modding_path + booker_dber_data_file_name;

	try_parse_bder_data("res://" + booker_dber_data_file_name); // Also parses the skills (try_parse_skills_data).

	// If the mod data exists, then let it override any data that it has.
	// if (FileAccess::file_exists(modding_dber_data_path))
	// {
	// 	UtilityFunctions::print("Log - Using modding booker data.");
	// 	try_parse_bder_data(modding_dber_data_path);
	// }

	emit_signal(refreshed_data_signal_name);
}

/* static */ void BG_Booker_DB::timer_test(const Callable &callable)
{
	static counter cont = counter();
	TIME_FUNC(callable, cont);
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

	clear_localize_data();

	free_all_params();
}

void BG_Booker_DB::free_all_params()
{
	modding_path = "";

	if (BG_Booker_DB::bg_is_instance_valid(globals)) {
		memdelete(globals);
	}
	globals = nullptr;
	for (int i = 0; i < character_details.size(); ++i) {
		BG_CharacterDetails *d = cast_to<BG_CharacterDetails>(character_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	character_details.clear();
	for (int i = 0; i < objectives.size(); ++i) {
		BG_ObjectiveDetails *d = cast_to<BG_ObjectiveDetails>(objectives[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	objectives.clear();
	for (int i = 0; i < base_stats.size(); ++i) {
		BG_BaseStat *d = cast_to<BG_BaseStat>(base_stats[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	base_stats.clear();
	for (int i = 0; i < battle_boards_details.size(); ++i) {
		BG_BattleBoardDetails *d = cast_to<BG_BattleBoardDetails>(battle_boards_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	battle_boards_details.clear();
	for (int i = 0; i < game_map_node_details.size(); ++i) {
		BG_GameMapNodeDetails *d = cast_to<BG_GameMapNodeDetails>(game_map_node_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	game_map_node_details.clear();
	for (int i = 0; i < game_map_details.size(); ++i) {
		BG_GameMapDetails *d = cast_to<BG_GameMapDetails>(game_map_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	game_map_details.clear();
	for (const auto &pair : resource_type_details) {
		if (BG_Booker_DB::bg_is_instance_valid(pair.value))
			memdelete(pair.value);
	}
	resource_type_details.clear();
	for (int i = 0; i < audio_data.size(); ++i) {
		BG_AudioData *d = cast_to<BG_AudioData>(audio_data[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	audio_data.clear();
	for (int i = 0; i < icon_details.size(); ++i) {
		BG_IconDetails *d = cast_to<BG_IconDetails>(icon_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	icon_details.clear();
	for (int i = 0; i < booker_skill_tree_details.size(); ++i) {
		BG_BookerSkillTreeSlotDetails *d = cast_to<BG_BookerSkillTreeSlotDetails>(booker_skill_tree_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	booker_skill_tree_details.clear();
	for (int i = 0; i < jobs.size(); ++i) {
		BG_JobDetails *d = cast_to<BG_JobDetails>(jobs[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	jobs.clear();
	for (int i = 0; i < items.size(); ++i) {
		BG_ItemDetails *d = cast_to<BG_ItemDetails>(items[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	items.clear();
	for (int i = 0; i < item_drop_pools.size(); ++i) {
		BG_ItemDropPool *d = cast_to<BG_ItemDropPool>(item_drop_pools[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	item_drop_pools.clear();
	for (int i = 0; i < effects.size(); ++i) {
		BG_Effect *d = cast_to<BG_Effect>(effects[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	effects.clear();
	if (BG_Booker_DB::bg_is_instance_valid(band_info)) {
		memdelete(band_info);
	}
	band_info = nullptr;
	for (int i = 0; i < item_slot_types.size(); ++i) {
		BG_ItemSlotType *d = cast_to<BG_ItemSlotType>(item_slot_types[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	item_slot_types.clear();
	for (int i = 0; i < rarity_types.size(); ++i) {
		BG_RarityDetails *d = cast_to<BG_RarityDetails>(rarity_types[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	rarity_types.clear();
	for (int i = 0; i < stat_types.size(); ++i) {
		BG_UnitStatDetails *d = cast_to<BG_UnitStatDetails>(stat_types[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	stat_types.clear();
	if (BG_Booker_DB::bg_is_instance_valid(market_place_data)) {
		memdelete(market_place_data);
	}
	market_place_data = nullptr;
	monster_types.clear();
	for (int i = 0; i < mail_data.size(); ++i) {
		BG_MailData *d = cast_to<BG_MailData>(mail_data[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	mail_data.clear();
	for (int i = 0; i < puzzles.size(); ++i) {
		BG_PuzzleDetails *d = cast_to<BG_PuzzleDetails>(puzzles[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	puzzles.clear();
	for (int i = 0; i < two_der_data_entries.size(); ++i) {
		BG_TwoDer_DataEntry *d = cast_to<BG_TwoDer_DataEntry>(two_der_data_entries[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	two_der_data_entries.clear();
}

// A key's lines in one language, from the DBer data (localization_source). Cached per sheet and language.
// A missing or empty translation falls back to English when revert_localization_to_english is on.
TypedArray<BG_LocalizeEntryData> BG_Booker_DB::get_localize_data(const StringName &sheet_name, const StringName &key, const StringName &language)
{
	const StringName cache_name = String(sheet_name) + "|" + String(language);
	if (localize_data.has(cache_name) && localize_data[cache_name].has(key)) {
		return localize_data[cache_name][key];
	}

	const String key_as_string = String(key);
	ERR_FAIL_COND_V_EDMSG(key_as_string == "", {}, "ERROR : Localization key is \"\", sheet: " + sheet_name);
	const Dictionary sheet = localization_source.get(String(sheet_name), Dictionary());
	ERR_FAIL_COND_V_EDMSG(sheet.is_empty(), {}, "ERROR : No localization sheet: " + sheet_name + " (DBer's Localization tab).");
	const Array lines = sheet.get(key_as_string, Array());
	ERR_FAIL_COND_V_EDMSG(lines.is_empty(), {}, "ERROR : Could not find localization data for, sheet: " + sheet_name + ", key: " + key + ", langauge: " + language);

	const String language_as_string = String(language);
	TypedArray<BG_LocalizeEntryData> result = TypedArray<BG_LocalizeEntryData>();
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary line = lines[i];
		BG_LocalizeEntryData *entry = memnew(BG_LocalizeEntryData);
		entry->code_start = line.get("code_start", "");
		entry->code_end = line.get("code_end", "");
		String text = line.get(language_as_string, "");
		if (text.is_empty() && revert_localization_to_english && language_as_string != "en") {
			text = line.get("en", "");
		}
		else if (!line.has(language_as_string)) {
			ERR_PRINT_ED("ERROR : Localization language not found in sheet: " + sheet_name + ", langauge: " + language);
		}
		entry->text = text;
		result.append(entry);
	}

	localize_data[cache_name][key] = result;
	return result;
}

void BG_Booker_DB::clear_localize_data()
{
	for (const auto &pair : localize_data) {
		for (const auto &pair_two : pair.value) {
			for (int i = 0; i < pair_two.value.size(); ++i) {
				BG_LocalizeEntryData *d = cast_to<BG_LocalizeEntryData>(pair_two.value[i]);
				if (BG_Booker_DB::bg_is_instance_valid(d))
					memdelete(d);
			}
		}
	}
	localize_data.clear();
}

// The text from DBer's Localization tab. Called at the end of try_parse_bder_data.
void BG_Booker_DB::try_parse_localization_data(const Dictionary &p_localization_data)
{
	clear_localize_data();
	localization_source = p_localization_data;
	ERR_FAIL_COND_EDMSG(localization_source.is_empty(), "ERROR : The DBer data has no localization_data (DBer's Localization tab).");
}

String BG_Booker_DB::get_localize_string(const StringName &sheet_name, const StringName &key, const StringName &language, bool ignore_code_data)
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

	// A "\n" typed in the localization data becomes a real line break.
	return result.replace("\\n", "\n");
}

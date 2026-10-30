// Data classes for the world: characters, storyboards, tutorials, icons, puzzles, battle boards, game maps,
// objectives, resources, mail, marketplace, audio, acts, booker skills, cities, globals.
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
//// BG_PortraitDetails
////
void BG_PortraitDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_portrait_type"), &BG_PortraitDetails::get_portrait_type);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_PortraitDetails::get_icon_path);

	BIND_ENUM_CONSTANT(DEFAULT);
	BIND_ENUM_CONSTANT(HAPPY);
	BIND_ENUM_CONSTANT(SAD);
	BIND_ENUM_CONSTANT(ANGER);
	BIND_ENUM_CONSTANT(SUPRISE);
	BIND_ENUM_CONSTANT(FEAR);
	BIND_ENUM_CONSTANT(DISGUST);
	BIND_ENUM_CONSTANT(CONTEMPT);
}

////
//// BG_CharacterDetails
////
void BG_CharacterDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_CharacterDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_portraits"), &BG_CharacterDetails::get_portraits);
	ClassDB::bind_method(D_METHOD("get_portrait_icon_by_type", "portrait_type"), &BG_CharacterDetails::get_portrait_icon_by_type);
}

BG_CharacterDetails::~BG_CharacterDetails()
{
	for (int i = 0; i < portraits.size(); ++i) {
		BG_PortraitDetails *d = cast_to<BG_PortraitDetails>(portraits[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

StringName BG_CharacterDetails::get_portrait_icon_by_type(BG_PortraitDetails::PortraitType portrait_type) const
{
	for (int i = 0; i < portraits.size(); ++i) {
		BG_PortraitDetails *p = cast_to<BG_PortraitDetails>(portraits[i]);
		if (p->get_portrait_type() == portrait_type)
			return p->get_icon_path();
	}
	// If the type doesn't exist, then just return the first one.
	if (!portraits.is_empty()) {
		BG_PortraitDetails *p = cast_to<BG_PortraitDetails>(portraits[0]);
		return p->get_icon_path();
	}
	return StringName("");
}

////
//// BG_StoryboardCharacterDefaultTextLocationDetails
////
void BG_StoryboardCharacterTextLocationDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_character_key"), &BG_StoryboardCharacterTextLocationDetails::get_character_key);
	ClassDB::bind_method(D_METHOD("get_txt_locations"), &BG_StoryboardCharacterTextLocationDetails::get_txt_locations);
}

////
//// BG_StoryboardDataDetails
////
void BG_StoryboardDataDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_character_key"), &BG_StoryboardDataDetails::get_character_key);
	ClassDB::bind_method(D_METHOD("get_character_emotion"), &BG_StoryboardDataDetails::get_character_emotion);
	ClassDB::bind_method(D_METHOD("get_text_key"), &BG_StoryboardDataDetails::get_text_key);
	ClassDB::bind_method(D_METHOD("get_code"), &BG_StoryboardDataDetails::get_code);
	ClassDB::bind_method(D_METHOD("get_texture_path"), &BG_StoryboardDataDetails::get_texture_path);
	ClassDB::bind_method(D_METHOD("get_audio_ids"), &BG_StoryboardDataDetails::get_audio_ids);
}

////
//// BG_StoryboardDetails
////
void BG_StoryboardDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_StoryboardDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_localization_sheet"), &BG_StoryboardDetails::get_localization_sheet);
	ClassDB::bind_method(D_METHOD("get_data"), &BG_StoryboardDetails::get_data);
	ClassDB::bind_method(D_METHOD("get_character_text_locations"), &BG_StoryboardDetails::get_character_text_locations);
	ClassDB::bind_method(D_METHOD("get_character_text_locations_by_key", "key"), &BG_StoryboardDetails::get_character_text_locations_by_key);
}

TypedArray<Vector2> BG_StoryboardDetails::get_character_text_locations_by_key(const StringName &key) const
{
	for (int i = 0; i < character_text_locations.size(); ++i) {
		Ref<BG_StoryboardCharacterTextLocationDetails> cdtl = cast_to<BG_StoryboardCharacterTextLocationDetails>(character_text_locations[i]);
		if (cdtl->get_character_key() == key)
			return cdtl->get_txt_locations();
	}
	return {};
}

// BG_StoryboardDetails::~BG_StoryboardDetails()
// {
// 	for (int i = 0; i < data.size(); ++i) {
// 		BG_StoryboardDataDetails *d = cast_to<BG_StoryboardDataDetails>(data[i]);
// 		if (BG_Booker_DB::bg_is_instance_valid(d))
// 			memdelete(d);
// 	}
// 	for (int i = 0; i < character_default_text_locations.size(); ++i) {
// 		BG_StoryboardCharacterDefaultTextLocationDetails *d = cast_to<BG_StoryboardCharacterDefaultTextLocationDetails>(character_default_text_locations[i]);
// 		if (BG_Booker_DB::bg_is_instance_valid(d))
// 			memdelete(d);
// 	}
// }

////
//// BG_TutorialCardDetails
////
void BG_TutorialCardDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_description_key"), &BG_TutorialCardDetails::get_description_key);
	ClassDB::bind_method(D_METHOD("get_image_path"), &BG_TutorialCardDetails::get_image_path);
}

////
//// BG_TutorialCardsDetails
////
void BG_TutorialCardsDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_TutorialCardsDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_cards"), &BG_TutorialCardsDetails::get_cards);
}

////
//// BG_IconDetails
////
void BG_IconDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_IconDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_IconDetails::get_icon_path);
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
//// BG_PuzzleDetails_MiscParams
////
void BG_PuzzleDetails_MiscParams::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_param_name"), &BG_PuzzleDetails_MiscParams::get_param_name);
	ClassDB::bind_method(D_METHOD("get_value1"), &BG_PuzzleDetails_MiscParams::get_value1);
	ClassDB::bind_method(D_METHOD("get_value2"), &BG_PuzzleDetails_MiscParams::get_value2);
}

////
//// BG_PuzzleDetails
////
void BG_PuzzleDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_PuzzleDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_scene_path"), &BG_PuzzleDetails::get_scene_path);
	ClassDB::bind_method(D_METHOD("get_misc_params"), &BG_PuzzleDetails::get_misc_params);
	ClassDB::bind_method(D_METHOD("get_misc_param_by_id", "id"), &BG_PuzzleDetails::get_misc_param_by_id);
}

BG_PuzzleDetails::~BG_PuzzleDetails()
{
	for (int i = 0; i < misc_params.size(); ++i) {
		BG_PuzzleDetails_MiscParams *d = cast_to<BG_PuzzleDetails_MiscParams>(misc_params[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

BG_PuzzleDetails_MiscParams *BG_PuzzleDetails::get_misc_param_by_id(const StringName &id) const
{
	for (int i = 0; i < misc_params.size(); ++i) {
		BG_PuzzleDetails_MiscParams *param = cast_to<BG_PuzzleDetails_MiscParams>(misc_params[i]);
		if (param != nullptr && param->get_param_name() == id)
			return param;
	}
	return nullptr;
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
	ClassDB::bind_method(D_METHOD("get_is_dynamic_type"), &BG_BattleBoard_HexTypeDetails::get_is_dynamic_type);
	ClassDB::bind_method(D_METHOD("get_is_game_type"), &BG_BattleBoard_HexTypeDetails::get_is_game_type);
	ClassDB::bind_method(D_METHOD("get_hex_type"), &BG_BattleBoard_HexTypeDetails::get_hex_type);
	ClassDB::bind_method(D_METHOD("get_visuals"), &BG_BattleBoard_HexTypeDetails::get_visuals);
	ClassDB::bind_method(D_METHOD("get_destroyed_vfx_scene_path"), &BG_BattleBoard_HexTypeDetails::get_destroyed_vfx_scene_path);
	ClassDB::bind_method(D_METHOD("get_destroyed_sfx_id"), &BG_BattleBoard_HexTypeDetails::get_destroyed_sfx_id);
	ClassDB::bind_method(D_METHOD("get_hex_visual_scene_path_override"), &BG_BattleBoard_HexTypeDetails::get_hex_visual_scene_path_override);
	ClassDB::bind_method(D_METHOD("get_extra_types"), &BG_BattleBoard_HexTypeDetails::get_extra_types);
	ClassDB::bind_method(D_METHOD("get_health_effectiveness"), &BG_BattleBoard_HexTypeDetails::get_health_effectiveness);
	ClassDB::bind_method(D_METHOD("get_equipment_ids"), &BG_BattleBoard_HexTypeDetails::get_equipment_ids);
	ClassDB::bind_method(D_METHOD("get_is_actionable"), &BG_BattleBoard_HexTypeDetails::get_is_actionable);
	ClassDB::bind_method(D_METHOD("get_actionable_requires_move"), &BG_BattleBoard_HexTypeDetails::get_actionable_requires_move);
	ClassDB::bind_method(D_METHOD("get_pass_through_by_ally"), &BG_BattleBoard_HexTypeDetails::get_pass_through_by_ally);
	ClassDB::bind_method(D_METHOD("get_pass_through_by_enemy"), &BG_BattleBoard_HexTypeDetails::get_pass_through_by_enemy);
	ClassDB::bind_method(D_METHOD("get_misc_attributes"), &BG_BattleBoard_HexTypeDetails::get_misc_attributes);
}

BG_BattleBoard_HexTypeDetails::~BG_BattleBoard_HexTypeDetails()
{
	for (int i = 0; i < visuals.size(); ++i) {
		BG_BattleBoard_HexTypeVisualDetails *d = cast_to<BG_BattleBoard_HexTypeVisualDetails>(visuals[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	if (BG_Booker_DB::bg_is_instance_valid(health_effectiveness))
		memdelete(health_effectiveness);
	health_effectiveness = nullptr;
}

////
//// BG_BattleBoardDetails
////
void BG_BattleBoardDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_BattleBoardDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_unique_save_name"), &BG_BattleBoardDetails::get_unique_save_name);
	ClassDB::bind_method(D_METHOD("get_board_path"), &BG_BattleBoardDetails::get_board_path);
	ClassDB::bind_method(D_METHOD("get_battle_board_game_script_path"), &BG_BattleBoardDetails::get_battle_board_game_script_path);
	ClassDB::bind_method(D_METHOD("get_default_hex_visual_path"), &BG_BattleBoardDetails::get_default_hex_visual_path);
	ClassDB::bind_method(D_METHOD("get_hex_types"), &BG_BattleBoardDetails::get_hex_types);
	ClassDB::bind_method(D_METHOD("get_parent_data_id"), &BG_BattleBoardDetails::get_parent_data_id);
	ClassDB::bind_method(D_METHOD("get_music_id"), &BG_BattleBoardDetails::get_music_id);
	ClassDB::bind_method(D_METHOD("get_ambient_id"), &BG_BattleBoardDetails::get_ambient_id);

	ClassDB::bind_method(D_METHOD("get_hex_types_by_type", "type"), &BG_BattleBoardDetails::get_hex_types_by_type);
	ClassDB::bind_method(D_METHOD("get_hex_type_by_id", "id"), &BG_BattleBoardDetails::get_hex_type_by_id);
}

BG_BattleBoardDetails::~BG_BattleBoardDetails()
{
	for (int i = 0; i < hex_types.size(); ++i) {
		BG_BattleBoard_HexTypeDetails *d = cast_to<BG_BattleBoard_HexTypeDetails>(hex_types[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

TypedArray<BG_BattleBoard_HexTypeDetails> BG_BattleBoardDetails::get_hex_types_by_type(int type, bool is_game_type) const
{
	const int type_remapped = is_game_type ? static_cast<int>(BG_HexGameSaveData::map_game_type_to_asset_type(static_cast<BG_HexGameSaveData::HexGameAssetTypes>(type))) : type;

	TypedArray<BG_BattleBoard_HexTypeDetails> result;
	for (int i = 0; i < hex_types.size(); ++i) {
		BG_BattleBoard_HexTypeDetails *ht = cast_to<BG_BattleBoard_HexTypeDetails>(hex_types[i]);
		if (ht->hex_type == type_remapped)
			result.append(ht);
	}
	return result;
}

BG_BattleBoard_HexTypeDetails *BG_BattleBoardDetails::get_hex_type_by_id(const StringName &id) const
{
	for (int i = 0; i < hex_types.size(); ++i) {
		BG_BattleBoard_HexTypeDetails *ht = cast_to<BG_BattleBoard_HexTypeDetails>(hex_types[i]);
		// UtilityFunctions::prints(i, ht->id);
		if (ht->id == id)
			return ht;
	}
	return nullptr;
}

////
//// BG_GameMapNodeDetails
////
void BG_GameMapNodeDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_GameMapNodeDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_type"), &BG_GameMapNodeDetails::get_type);
	ClassDB::bind_method(D_METHOD("get_script_path"), &BG_GameMapNodeDetails::get_script_path);
	ClassDB::bind_method(D_METHOD("get_battle_board_id"), &BG_GameMapNodeDetails::get_battle_board_id);
	ClassDB::bind_method(D_METHOD("get_storyboard_id_pre"), &BG_GameMapNodeDetails::get_storyboard_id_pre);
	ClassDB::bind_method(D_METHOD("get_storyboard_id_post"), &BG_GameMapNodeDetails::get_storyboard_id_post);
	ClassDB::bind_method(D_METHOD("get_enable_nodes_on_complete"), &BG_GameMapNodeDetails::get_enable_nodes_on_complete);
}

BG_GameMapNodeDetails::~BG_GameMapNodeDetails()
{
}

////
//// BG_GameMapDetails
////
void BG_GameMapDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_GameMapDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_scene_path"), &BG_GameMapDetails::get_scene_path);
}

BG_GameMapDetails::~BG_GameMapDetails()
{
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

BG_ObjectiveDetails::~BG_ObjectiveDetails()
{
	for (int i = 0; i < drops.size(); ++i) {
		BG_RewardItem *d = cast_to<BG_RewardItem>(drops[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	if (BG_Booker_DB::bg_is_instance_valid(timeline))
		memdelete(timeline);
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

BG_MarketplaceData::~BG_MarketplaceData()
{
	for (int i = 0; i < entries.size(); ++i) {
		BG_MarketplaceEntryData *d = cast_to<BG_MarketplaceEntryData>(entries[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
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
	ClassDB::bind_method(D_METHOD("get_random_pitch_scale"), &BG_AudioDataDetails::get_random_pitch_scale);
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

BG_AudioData::~BG_AudioData()
{
	for (int i = 0; i < audio_details.size(); ++i) {
		BG_AudioDataDetails *d = cast_to<BG_AudioDataDetails>(audio_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
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
//// BG_BookerSkillTreeSlotDetails
////
void BG_BookerSkillTreeSlotDetails::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_BookerSkillTreeSlotDetails::get_id);
	ClassDB::bind_method(D_METHOD("get_is_parent_button"), &BG_BookerSkillTreeSlotDetails::get_is_parent_button);
	ClassDB::bind_method(D_METHOD("get_skill_description_key"), &BG_BookerSkillTreeSlotDetails::get_skill_description_key);
	ClassDB::bind_method(D_METHOD("get_required_rep"), &BG_BookerSkillTreeSlotDetails::get_required_rep);
	ClassDB::bind_method(D_METHOD("get_parent_skill_id"), &BG_BookerSkillTreeSlotDetails::get_parent_skill_id);
	ClassDB::bind_method(D_METHOD("get_value_attributes"), &BG_BookerSkillTreeSlotDetails::get_value_attributes);
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
	ClassDB::bind_method(D_METHOD("get_game_script_path"), &BG_CityInfo::get_game_script_path);
	ClassDB::bind_method(D_METHOD("get_game_map_id"), &BG_CityInfo::get_game_map_id);
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
	ClassDB::bind_method(D_METHOD("get_crit_qte_perfect_window"), &BG_Booker_Globals::get_crit_qte_perfect_window);
	ClassDB::bind_method(D_METHOD("get_crit_qte_window_additive"), &BG_Booker_Globals::get_crit_qte_window_additive);

	ClassDB::bind_method(D_METHOD("get_day_night_cycle_turns_per_day"), &BG_Booker_Globals::get_day_night_cycle_turns_per_day);
	ClassDB::bind_method(D_METHOD("get_percentage_of_day_is_day_time"), &BG_Booker_Globals::get_percentage_of_day_is_day_time);
	ClassDB::bind_method(D_METHOD("get_day_cycle_offset_per_act"), &BG_Booker_Globals::get_day_cycle_offset_per_act);

	ClassDB::bind_method(D_METHOD("get_band_action_points_fatigue_rate"), &BG_Booker_Globals::get_band_action_points_fatigue_rate);
	ClassDB::bind_method(D_METHOD("get_monster_action_points_fatigue_rate"), &BG_Booker_Globals::get_monster_action_points_fatigue_rate);
	ClassDB::bind_method(D_METHOD("get_action_points_rest_rate"), &BG_Booker_Globals::get_action_points_rest_rate);
	
	ClassDB::bind_method(D_METHOD("get_exp_when_source_unit_damaged"), &BG_Booker_Globals::get_exp_when_source_unit_damaged);
	ClassDB::bind_method(D_METHOD("get_exp_when_enemy_killed"), &BG_Booker_Globals::get_exp_when_enemy_killed);
	ClassDB::bind_method(D_METHOD("get_exp_when_ap_consumed"), &BG_Booker_Globals::get_exp_when_ap_consumed);

	ClassDB::bind_method(D_METHOD("get_attribute_points_per_level"), &BG_Booker_Globals::get_attribute_points_per_level);
}

BG_Booker_Globals::~BG_Booker_Globals()
{
	for (int i = 0; i < city_info.size(); ++i) {
		BG_CityInfo *d = cast_to<BG_CityInfo>(city_info[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < act_stats.size(); ++i) {
		BG_ActStats *d = cast_to<BG_ActStats>(act_stats[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

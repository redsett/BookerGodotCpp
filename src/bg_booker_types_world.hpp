#pragma once

// Data classes for the world: characters, storyboards, tutorials, icons, puzzles, battle boards, game maps, objectives, resources, mail, marketplace, audio, acts, booker skills, cities.
// Part of bg_booker_data_base.hpp: include that one, not this.

#include "bg_booker_types_fwd.hpp"


////
//// BG_PortraitDetails
////
class BG_PortraitDetails : public Object
{
	GDCLASS(BG_PortraitDetails, Object);

protected:
	static void _bind_methods();

public:
	enum PortraitType : int32_t {
		DEFAULT,
		HAPPY,
		SAD,
		ANGER,
		SUPRISE,
		FEAR,
		DISGUST,
		CONTEMPT
	};

	PortraitType portrait_type;
	PortraitType get_portrait_type() const { return portrait_type; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }
};
VARIANT_ENUM_CAST(BG_PortraitDetails::PortraitType);


////
//// BG_CharacterDetails
////
class BG_CharacterDetails : public Object
{
	GDCLASS(BG_CharacterDetails, Object);

protected:
	static void _bind_methods();

public:
	~BG_CharacterDetails();

	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_PortraitDetails> portraits;
	TypedArray<BG_PortraitDetails> get_portraits() const { return portraits; }

	StringName get_portrait_icon_by_type(BG_PortraitDetails::PortraitType portrait_type) const;
};


////
//// BG_StoryboardCharacterDefaultTextLocationDetails
////
class BG_StoryboardCharacterTextLocationDetails : public Resource
{
	GDCLASS(BG_StoryboardCharacterTextLocationDetails, Resource);

protected:
	static void _bind_methods();

public:
	StringName character_key;
	StringName get_character_key() const { return character_key; }

	TypedArray<Vector2> txt_locations;
	TypedArray<Vector2> get_txt_locations() const { return txt_locations; }
};


////
//// BG_StoryboardDataDetails
////
class BG_StoryboardDataDetails : public Resource
{
	GDCLASS(BG_StoryboardDataDetails, Resource);

protected:
	static void _bind_methods();

public:
	StringName character_key;
	StringName get_character_key() const { return character_key; }
	
	BG_PortraitDetails::PortraitType character_emotion = BG_PortraitDetails::PortraitType::DEFAULT;
	BG_PortraitDetails::PortraitType get_character_emotion() const { return character_emotion; }

	StringName text_key;
	StringName get_text_key() const { return text_key; }
	
	StringName code;
	StringName get_code() const { return code; }

	StringName texture_path;
	StringName get_texture_path() const { return texture_path; }
	
	TypedArray<StringName> audio_ids;
	TypedArray<StringName> get_audio_ids() const { return audio_ids; }
};


////
//// BG_StoryboardDetails
////
class BG_StoryboardDetails : public Resource
{
	GDCLASS(BG_StoryboardDetails, Resource);

protected:
	static void _bind_methods();

public:
	// ~BG_StoryboardDetails();

	StringName id;
	StringName get_id() const { return id; }

	StringName localization_sheet;
	StringName get_localization_sheet() const { return localization_sheet; }
	
	TypedArray<BG_StoryboardDataDetails> data;
	TypedArray<BG_StoryboardDataDetails> get_data() const { return data; }
	
	TypedArray<BG_StoryboardCharacterTextLocationDetails> character_text_locations;
	TypedArray<BG_StoryboardCharacterTextLocationDetails> get_character_text_locations() const { return character_text_locations; }
	TypedArray<Vector2> get_character_text_locations_by_key(const StringName &key) const;
};


////
//// BG_TutorialCardDetails
////
class BG_TutorialCardDetails : public Resource
{
	GDCLASS(BG_TutorialCardDetails, Resource);

protected:
	static void _bind_methods();

public:
	StringName description_key;
	StringName get_description_key() const { return description_key; }

	StringName image_path;
	StringName get_image_path() const { return image_path; }
};


////
//// BG_TutorialCardsDetails
////
class BG_TutorialCardsDetails : public Resource
{
	GDCLASS(BG_TutorialCardsDetails, Resource);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_TutorialCardDetails> cards;
	TypedArray<BG_TutorialCardDetails> get_cards() const { return cards; }
};


////
//// BG_IconDetails
////
class BG_IconDetails : public Object
{
	GDCLASS(BG_IconDetails, Object);

protected:
	static void _bind_methods();

public:
	StringName id;
	StringName get_id() const { return id; }

	StringName icon_path;
	StringName get_icon_path() const { return icon_path; }
};


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
//// BG_PuzzleDetails_MiscParams
////
class BG_PuzzleDetails_MiscParams : public Object
{
	GDCLASS(BG_PuzzleDetails_MiscParams, Object);

protected:
	static void _bind_methods();

public:
	StringName param_name;
	StringName get_param_name() const { return param_name; }

	StringName value1;
	StringName get_value1() const { return value1; }

	StringName value2;
	StringName get_value2() const { return value2; }
};


////
//// BG_PuzzleDetails
////
class BG_PuzzleDetails : public Object
{
	GDCLASS(BG_PuzzleDetails, Object);

protected:
	static void _bind_methods();

public:
	~BG_PuzzleDetails();

	StringName id;
	StringName get_id() const { return id; }

	StringName scene_path;
	StringName get_scene_path() const { return scene_path; }

	TypedArray<BG_PuzzleDetails_MiscParams> misc_params;
	TypedArray<BG_PuzzleDetails_MiscParams> get_misc_params() const { return misc_params; }

	BG_PuzzleDetails_MiscParams *get_misc_param_by_id(const StringName &id) const;
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
	~BG_BattleBoard_HexTypeDetails();

	StringName id;
	StringName get_id() const { return id; }

	bool is_dynamic_type = false;
	bool get_is_dynamic_type() const { return is_dynamic_type; }

	bool is_game_type = false;
	bool get_is_game_type() const { return is_game_type; }

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

	Dictionary extra_types;
	Dictionary get_extra_types() const { return extra_types; }

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

	Dictionary misc_attributes;
	Dictionary get_misc_attributes() const { return misc_attributes; }
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
	~BG_BattleBoardDetails();

	StringName id;
	StringName get_id() const { return id; }

	StringName unique_save_name;
	StringName get_unique_save_name() const { return unique_save_name; }

	StringName board_path;
	StringName get_board_path() const { return board_path; }

	StringName battle_board_game_script_path;
	StringName get_battle_board_game_script_path() const { return battle_board_game_script_path; }

	StringName default_hex_visual_path;
	StringName get_default_hex_visual_path() const { return default_hex_visual_path; }

	TypedArray<BG_BattleBoard_HexTypeDetails> hex_types;
	TypedArray<BG_BattleBoard_HexTypeDetails> get_hex_types() const { return hex_types; }

	StringName parent_data_id;
	StringName get_parent_data_id() const { return parent_data_id; }

	StringName music_id;
	StringName get_music_id() const { return music_id; }

	StringName ambient_id;
	StringName get_ambient_id() const { return ambient_id; }

	TypedArray<BG_BattleBoard_HexTypeDetails> get_hex_types_by_type(int type, bool is_game_type = true) const;
	BG_BattleBoard_HexTypeDetails *get_hex_type_by_id(const StringName &id) const;
};


////
//// BG_GameMapNodeDetails
////
class BG_GameMapNodeDetails : public Object
{
	GDCLASS(BG_GameMapNodeDetails, Object);

protected:
	static void _bind_methods();

public:
	~BG_GameMapNodeDetails();

	StringName id;
	StringName get_id() const { return id; }

	int type;
	int get_type() const { return type; }

	StringName script_path;
	StringName get_script_path() const { return script_path; }
	
	StringName battle_board_id;
	StringName get_battle_board_id() const { return battle_board_id; }
	
	StringName storyboard_id_pre;
	StringName get_storyboard_id_pre() const { return storyboard_id_pre; }
	
	StringName storyboard_id_post;
	StringName get_storyboard_id_post() const { return storyboard_id_post; }

	Dictionary enable_nodes_on_complete; // StringName (Game Map Node ID) : bool (enabled or not)
	Dictionary get_enable_nodes_on_complete() const { return enable_nodes_on_complete; }
};


////
//// BG_GameMapDetails
////
class BG_GameMapDetails : public Object
{
	GDCLASS(BG_GameMapDetails, Object);

protected:
	static void _bind_methods();

public:
	~BG_GameMapDetails();

	StringName id;
	StringName get_id() const { return id; }

	StringName scene_path;
	StringName get_scene_path() const { return scene_path; }
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
	~BG_ObjectiveDetails();

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
	~BG_MarketplaceData();

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

	int volume_db_base_value = 0;
	int get_volume_db_base_value() const { return volume_db_base_value; }
	
	float random_pitch_scale = 0.0;
	float get_random_pitch_scale() const { return random_pitch_scale; }

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
	~BG_AudioData();

	StringName id;
	StringName get_id() const { return id; }

	TypedArray<BG_AudioDataDetails> audio_details;
	TypedArray<BG_AudioDataDetails> get_audio_details() const { return audio_details; }
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

	StringName skill_description_key;
	StringName get_skill_description_key() const { return skill_description_key; }

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

	StringName game_script_path;
	StringName get_game_script_path() const { return game_script_path; }

	StringName game_map_id;
	StringName get_game_map_id() const { return game_map_id; }

	Dictionary misc_attributes;
	Dictionary get_misc_attributes() const { return misc_attributes; }
};

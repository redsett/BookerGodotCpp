#include "bg_booker_data_base.hpp"
#include "bg_focus_layers.hpp"
#include "bg_hex_grid.hpp"

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

// Reading the DBer data (booker_dber_data.json, written by DBer's StaticFuncs.export_data). It's flat:
// - Rows are Dictionaries, {column name: value}. A content entry's misc params are fields of the entry itself.
// - Values are plain: numbers, strings and bools; enums as their index (see "global_enums" for their names); paths
//   project-relative ("Textures/icon.png"); Vector2 as [x, y]; arrays as lists of rows.
// - Stat references are keyed by the stat's name: {"health": 0.0, "physical_damage": 1.0}.

// A field of a DBer row. A missing field is reported (DBer renamed or removed the column?) and reads as Nil.
static Variant dber_get(const Dictionary &row, const String &name) {
	const Variant value = row.get(name, Variant());
	if (value.get_type() == Variant::NIL && !row.has(name)) {
		ERR_PRINT("ERROR : The DBer data has no \"" + name + "\" field where it's read. Was the column renamed or removed in DBer?");
	}
	return value;
}

// A field that only some rows have (e.g. misc params only some content types have). Nil when it's missing.
static Variant dber_find(const Dictionary &row, const String &name) {
	return row.get(name, Variant());
}

// A sheet's rows, {sheet name: [row, ...]} in "sheets".
static Array get_sheet_by_name(const String &name, const Dictionary &d) {
	const Dictionary sheets = d.get("sheets", Dictionary());
	ERR_FAIL_COND_V_MSG(!sheets.has(name), Array(), "ERROR : The DBer data has no sheet \"" + name + "\".");
	return sheets[name];
}


static auto ensure_clean_path = [](const String &path) -> StringName {
	return StringName(path.trim_prefix("/"));
};

static HashMap<String, TypedArray<StringName>> get_global_enums(const Dictionary &data) { // Global Enums: {enum name: [value names]}
	HashMap<String, TypedArray<StringName>> global_enums;
	const Dictionary enums_data = data.get("global_enums", Dictionary());
	const Array enum_names = enums_data.keys();
	for (int i = 0; i < enum_names.size(); ++i) {
		const Array values = enums_data[enum_names[i]];
		TypedArray<StringName> enums;
		for (int y = 0; y < values.size(); ++y) {
			enums.append(StringName(values[y]));
		}
		global_enums[enum_names[i]] = enums;
	}
	return global_enums;
}

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
//// BG_BandMember
////
void BG_BandMember::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_first_name_id"), &BG_BandMember::get_first_name_id);
	ClassDB::bind_method(D_METHOD("set_first_name_id"), &BG_BandMember::set_first_name_id);
	ClassDB::bind_method(D_METHOD("get_last_name_id"), &BG_BandMember::get_last_name_id);
	ClassDB::bind_method(D_METHOD("set_last_name_id"), &BG_BandMember::set_last_name_id);
	ClassDB::bind_method(D_METHOD("get_band_member_type"), &BG_BandMember::get_band_member_type);
	ClassDB::bind_method(D_METHOD("set_band_member_type"), &BG_BandMember::set_band_member_type);
	ClassDB::bind_method(D_METHOD("get_current_percent_health"), &BG_BandMember::get_current_percent_health);
	ClassDB::bind_method(D_METHOD("set_current_percent_health"), &BG_BandMember::set_current_percent_health);
	ClassDB::bind_method(D_METHOD("get_percent_health_additive"), &BG_BandMember::get_percent_health_additive);
	ClassDB::bind_method(D_METHOD("set_percent_health_additive"), &BG_BandMember::set_percent_health_additive);
	ClassDB::bind_method(D_METHOD("get_current_percent_action_points"), &BG_BandMember::get_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("set_current_percent_action_points"), &BG_BandMember::set_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("get_current_action_points_fatigue"), &BG_BandMember::get_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("set_current_action_points_fatigue"), &BG_BandMember::set_current_action_points_fatigue);
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
	ClassDB::bind_method(D_METHOD("get_inventory"), &BG_BandMember::get_inventory);
	ClassDB::bind_method(D_METHOD("set_inventory"), &BG_BandMember::set_inventory);
	ClassDB::bind_method(D_METHOD("get_is_city_asset"), &BG_BandMember::get_is_city_asset);
	ClassDB::bind_method(D_METHOD("set_is_city_asset"), &BG_BandMember::set_is_city_asset);
	ClassDB::bind_method(D_METHOD("get_band"), &BG_BandMember::get_band);
	ClassDB::bind_method(D_METHOD("set_band"), &BG_BandMember::set_band);
	ClassDB::bind_method(D_METHOD("get_battle_board_hex_type_details"), &BG_BandMember::get_battle_board_hex_type_details);
	ClassDB::bind_method(D_METHOD("set_battle_board_hex_type_details"), &BG_BandMember::set_battle_board_hex_type_details);
	ClassDB::bind_method(D_METHOD("get_attribute_upgrades"), &BG_BandMember::get_attribute_upgrades);
	ClassDB::bind_method(D_METHOD("set_attribute_upgrades"), &BG_BandMember::set_attribute_upgrades);
	ClassDB::bind_method(D_METHOD("get_consumable_upgrades"), &BG_BandMember::get_consumable_upgrades);
	ClassDB::bind_method(D_METHOD("set_consumable_upgrades"), &BG_BandMember::set_consumable_upgrades);
	ClassDB::bind_method(D_METHOD("get_equipped_ability_ids"), &BG_BandMember::get_equipped_ability_ids);
	ClassDB::bind_method(D_METHOD("set_equipped_ability_ids"), &BG_BandMember::set_equipped_ability_ids);
	ClassDB::bind_method(D_METHOD("get_equipped_passive_ids"), &BG_BandMember::get_equipped_passive_ids);
	ClassDB::bind_method(D_METHOD("set_equipped_passive_ids"), &BG_BandMember::set_equipped_passive_ids);
	ClassDB::bind_method(D_METHOD("is_dead"), &BG_BandMember::is_dead);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandMember::get_name);
	ClassDB::bind_method(D_METHOD("get_first_name"), &BG_BandMember::get_first_name);
	ClassDB::bind_method(D_METHOD("get_last_name"), &BG_BandMember::get_last_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "first_name_id"), "set_first_name_id", "get_first_name_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "last_name_id"), "set_last_name_id", "get_last_name_id");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "band_member_type", PROPERTY_HINT_ENUM, 
        "DEFAULT:0,PROTAGONIST:1,ALLY_MAIN_CHARACTER:2,ENEMY_MAIN_CHARACTER:3"), 
        "set_band_member_type", "get_band_member_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_health"), "set_current_percent_health", "get_current_percent_health");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "percent_health_additive"), "set_percent_health_additive", "get_percent_health_additive");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_action_points"), "set_current_percent_action_points", "get_current_percent_action_points");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_action_points_fatigue"), "set_current_action_points_fatigue", "get_current_action_points_fatigue");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "slot_index"), "set_slot_index", "get_slot_index");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_band_leader"), "set_is_band_leader", "get_is_band_leader");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "experience"), "set_experience", "get_experience");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "caste_id"), "set_caste_id", "get_caste_id");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipment"), "set_equipment", "get_equipment");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "inventory"), "set_inventory", "get_inventory");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "attribute_upgrades"), "set_attribute_upgrades", "get_attribute_upgrades");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "consumable_upgrades"), "set_consumable_upgrades", "get_consumable_upgrades");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipped_ability_ids", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_equipped_ability_ids", "get_equipped_ability_ids");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "equipped_passive_ids", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_equipped_passive_ids", "get_equipped_passive_ids");

	BIND_ENUM_CONSTANT(DEFAULT);
	BIND_ENUM_CONSTANT(PROTAGONIST);
	BIND_ENUM_CONSTANT(ALLY_MAIN_CHARACTER);
	BIND_ENUM_CONSTANT(ENEMY_MAIN_CHARACTER);
}

StringName BG_BandMember::get_name()
{
	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return cached_name;

	// The cache is cleared when a name id changes, and redone when the language changes.
	if (!cached_name.is_empty() && cached_name_language == booker_db->get_current_language_key()) return cached_name;
	cached_name_language = booker_db->get_current_language_key();
	
	String first_name = "";
	if (!get_first_name_id().is_empty())
		first_name = booker_db->get_localize_string(StringName("band_member"), get_first_name_id(), booker_db->get_current_language_key(), true);
	String last_name = "";
	if (!get_last_name_id().is_empty())
		last_name = booker_db->get_localize_string(StringName("band_member"), get_last_name_id(), booker_db->get_current_language_key(), true);
	
	cached_name = StringName(first_name + " " + last_name);
	return cached_name;
}

StringName BG_BandMember::get_first_name()
{
	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return "";
	
	StringName first_name;
	if (!get_first_name_id().is_empty())
		first_name = StringName(booker_db->get_localize_string(StringName("band_member"), get_first_name_id(), booker_db->get_current_language_key(), true));
	
	return first_name;
}

StringName BG_BandMember::get_last_name()
{
	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return "";
	
	StringName last_name;
	if (!get_last_name_id().is_empty())
		last_name = StringName(booker_db->get_localize_string(StringName("band_member"), get_last_name_id(), booker_db->get_current_language_key(), true));
	
	return last_name;
}

BG_BandMember::~BG_BandMember()
{
	battle_board_hex_type_details = nullptr;
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
	ClassDB::bind_static_method("BG_Band", D_METHOD("get_unique_band_id", "existing_bands"), &BG_Band::get_unique_band_id);

	ClassDB::bind_method(D_METHOD("get_unique_id"), &BG_Band::get_unique_id);
	ClassDB::bind_method(D_METHOD("set_unique_id"), &BG_Band::set_unique_id);
	ClassDB::bind_method(D_METHOD("get_id"), &BG_Band::get_id);
	ClassDB::bind_method(D_METHOD("set_id"), &BG_Band::set_id);
	ClassDB::bind_method(D_METHOD("get_name"), &BG_Band::get_name);
	ClassDB::bind_method(D_METHOD("set_name"), &BG_Band::set_name);
	ClassDB::bind_method(D_METHOD("get_preset_band_id"), &BG_Band::get_preset_band_id);
	ClassDB::bind_method(D_METHOD("set_preset_band_id"), &BG_Band::set_preset_band_id);
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
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "preset_band_id"), "set_preset_band_id", "get_preset_band_id");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "resting"), "set_resting", "get_resting");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_on_battle_board"), "set_is_on_battle_board", "get_is_on_battle_board");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "distance_traveled_this_turn"), "set_distance_traveled_this_turn", "get_distance_traveled_this_turn");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_done_combat_this_turn"), "set_has_done_combat_this_turn", "get_has_done_combat_this_turn");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "knocked_out_turns"), "set_knocked_out_turns", "get_knocked_out_turns");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "band_members"), "set_band_members", "get_band_members");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_unique_job_id"), "set_current_unique_job_id", "get_current_unique_job_id");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "band_formation"), "set_band_formation", "get_band_formation");
}

StringName BG_Band::get_name()
{
	if (!name.is_empty()) return name; // A custom name always wins.

	BG_Booker_DB *booker_db = BG_Booker_DB::get_singleton();
	if (!BG_Booker_DB::bg_is_instance_valid(booker_db)) return cached_name;

	// The cache is cleared when the id changes, and redone when the language changes.
	if (!cached_name.is_empty() && cached_name_language == booker_db->get_current_language_key()) return cached_name;
	cached_name_language = booker_db->get_current_language_key();

	if (!get_id().is_empty())
		cached_name = StringName(booker_db->get_localize_string(StringName("band_member"), get_id(), booker_db->get_current_language_key(), true));
	
	return cached_name;
}

bool BG_Band::is_band_alive() const
{
	if (get_knocked_out_turns() > 1)
		return false;
	
	for (uint32_t i = 0; i < get_band_members().size(); ++i) {
		const Ref<BG_BandMember> band_member = get_band_members()[i];
		if (band_member.is_null()) continue;
		if (!band_member->is_dead()) {
			return true;
		}
	}
	return false;
}

/* static */ int BG_Band::get_unique_band_id(const TypedArray<BG_Band> &existing_bands)
{
	// Randomly pick a unique id.
	int unique_id = 0;
	while (unique_id == 0) {
		int random_id = UtilityFunctions::randi_range(100, 99999);
		for (uint32_t i = 0; i < existing_bands.size(); ++i) {
			const Ref<BG_Band> b = existing_bands[i];
			if (b->get_unique_id() == random_id) {
				random_id = 0;
				break;
			}
		}
		if (random_id != 0) {
			unique_id = random_id;
		}
	}
	
	return unique_id;
}

////
//// BG_UnitCaste
////
void BG_UnitCaste::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_id"), &BG_UnitCaste::get_id);
	ClassDB::bind_method(D_METHOD("get_parent_caste"), &BG_UnitCaste::get_parent_caste);
	ClassDB::bind_method(D_METHOD("get_icon_path"), &BG_UnitCaste::get_icon_path);
	ClassDB::bind_method(D_METHOD("get_is_unique_caste"), &BG_UnitCaste::get_is_unique_caste);
	ClassDB::bind_method(D_METHOD("get_lod_mesh_paths"), &BG_UnitCaste::get_lod_mesh_paths);
	ClassDB::bind_method(D_METHOD("get_scale_min"), &BG_UnitCaste::get_scale_min);
	ClassDB::bind_method(D_METHOD("get_scale_max"), &BG_UnitCaste::get_scale_max);
	ClassDB::bind_method(D_METHOD("get_scale_min_extreme"), &BG_UnitCaste::get_scale_min_extreme);
	ClassDB::bind_method(D_METHOD("get_scale_max_extreme"), &BG_UnitCaste::get_scale_max_extreme);
	ClassDB::bind_method(D_METHOD("get_travel_distance"), &BG_UnitCaste::get_travel_distance);
	ClassDB::bind_method(D_METHOD("get_inventory_space"), &BG_UnitCaste::get_inventory_space);
	ClassDB::bind_method(D_METHOD("get_stats"), &BG_UnitCaste::get_stats);
	ClassDB::bind_method(D_METHOD("get_starting_item_ids"), &BG_UnitCaste::get_starting_item_ids);
	ClassDB::bind_method(D_METHOD("get_element_availability_ids"), &BG_UnitCaste::get_element_availability_ids);
	ClassDB::bind_method(D_METHOD("get_attribute_skill_tree_datas"), &BG_UnitCaste::get_attribute_skill_tree_datas);
	ClassDB::bind_method(D_METHOD("get_random_unit_caste_scale", "x_z"), &BG_UnitCaste::get_random_unit_caste_scale);
}

float BG_UnitCaste::get_random_unit_caste_scale(bool x_z) const {
	if (UtilityFunctions::randf() > 0.75) { // 25% chance of allowing the chance of being extreme.
		if (x_z) {
			return UtilityFunctions::randf_range(get_scale_min_extreme().x, get_scale_max_extreme().x);
		} else {
			return UtilityFunctions::randf_range(get_scale_min_extreme().y, get_scale_max_extreme().y);
		}
	} else {
		if (x_z) {
			return UtilityFunctions::randf_range(get_scale_min().x, get_scale_max().x);
		} else {
			return UtilityFunctions::randf_range(get_scale_min().y, get_scale_max().y);
		}
	}
}

BG_UnitCaste::~BG_UnitCaste()
{
}

////
//// BG_BandNameInfo
////
void BG_BandNameInfo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_name"), &BG_BandNameInfo::get_name);
	ClassDB::bind_method(D_METHOD("get_allowed_in_random"), &BG_BandNameInfo::get_allowed_in_random);
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
	ClassDB::bind_method(D_METHOD("is_job_alive"), &BG_Job::is_job_alive);

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

bool BG_Job::is_job_alive() const
{
	for (int i = 0; i < monsters.size(); ++i) {
		const BG_Monster *m = cast_to<BG_Monster>(monsters[i]);
		if (BG_Booker_DB::bg_is_instance_valid(m) && !m->is_dead()) {
			return true;
		}
	}
	return false;
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
	ClassDB::bind_method(D_METHOD("get_base_action_points"), &BG_Monster::get_base_action_points);
	ClassDB::bind_method(D_METHOD("get_bonus_action_points"), &BG_Monster::get_bonus_action_points);
	ClassDB::bind_method(D_METHOD("get_base_action_points_cost"), &BG_Monster::get_base_action_points_cost);
	ClassDB::bind_method(D_METHOD("get_current_percent_action_points"), &BG_Monster::get_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("set_current_percent_action_points"), &BG_Monster::set_current_percent_action_points);
	ClassDB::bind_method(D_METHOD("get_current_action_points_fatigue"), &BG_Monster::get_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("set_current_action_points_fatigue"), &BG_Monster::set_current_action_points_fatigue);
	ClassDB::bind_method(D_METHOD("get_travel_distance"), &BG_Monster::get_travel_distance);
	ClassDB::bind_method(D_METHOD("get_current_percent_health"), &BG_Monster::get_current_percent_health);
	ClassDB::bind_method(D_METHOD("set_current_percent_health"), &BG_Monster::set_current_percent_health);
	ClassDB::bind_method(D_METHOD("get_percent_health_additive"), &BG_Monster::get_percent_health_additive);
	ClassDB::bind_method(D_METHOD("set_percent_health_additive"), &BG_Monster::set_percent_health_additive);
	ClassDB::bind_method(D_METHOD("get_random_variation"), &BG_Monster::get_random_variation);
	ClassDB::bind_method(D_METHOD("set_random_variation"), &BG_Monster::set_random_variation);
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
	ClassDB::bind_method(D_METHOD("get_can_be_turned_to_stone"), &BG_Monster::get_can_be_turned_to_stone);
	ClassDB::bind_method(D_METHOD("get_is_turned_to_stone"), &BG_Monster::get_is_turned_to_stone);
	ClassDB::bind_method(D_METHOD("set_is_turned_to_stone"), &BG_Monster::set_is_turned_to_stone);
	ClassDB::bind_method(D_METHOD("is_dead"), &BG_Monster::is_dead);
	
	ClassDB::bind_method(D_METHOD("get_effectiveness"), &BG_Monster::get_effectiveness);
	ClassDB::bind_method(D_METHOD("get_effectiveness_stats"), &BG_Monster::get_effectiveness_stats);
	ClassDB::bind_method(D_METHOD("get_level_range"), &BG_Monster::get_level_range);
	ClassDB::bind_method(D_METHOD("get_animations"), &BG_Monster::get_animations);
	ClassDB::bind_method(D_METHOD("get_drops"), &BG_Monster::get_drops);
	ClassDB::bind_method(D_METHOD("get_action_points_fatigue_rate_multiplier"), &BG_Monster::get_action_points_fatigue_rate_multiplier);
	
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_action_points"), "set_current_percent_action_points", "get_current_percent_action_points");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_action_points_fatigue"), "set_current_action_points_fatigue", "get_current_action_points_fatigue");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_percent_health"), "set_current_percent_health", "get_current_percent_health");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "percent_health_additive"), "set_percent_health_additive", "get_percent_health_additive");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "random_variation"), "set_random_variation", "get_random_variation");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_turned_to_stone"), "set_is_turned_to_stone", "get_is_turned_to_stone");
}

BG_Monster::~BG_Monster()
{
	for (int i = 0; i < hue_shift_data.size(); ++i) {
		BG_HueShiftData *d = cast_to<BG_HueShiftData>(hue_shift_data[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < effectiveness_stats.size(); ++i) {
		BG_ContentStat *d = cast_to<BG_ContentStat>(effectiveness_stats[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < animations.size(); ++i) {
		BG_AnimationDetails *d = cast_to<BG_AnimationDetails>(animations[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < drops.size(); ++i) {
		BG_RewardItem *d = cast_to<BG_RewardItem>(drops[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
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
	ClassDB::bind_method(D_METHOD("get_num_band_members_for_hire"), &BG_BandInfo::get_num_band_members_for_hire);
	ClassDB::bind_method(D_METHOD("get_unit_castes"), &BG_BandInfo::get_unit_castes);
	ClassDB::bind_method(D_METHOD("get_caste_by_id"), &BG_BandInfo::get_caste_by_id);
	ClassDB::bind_method(D_METHOD("get_rest_recovery_speed"), &BG_BandInfo::get_rest_recovery_speed);
	ClassDB::bind_method(D_METHOD("get_knocked_out_turns"), &BG_BandInfo::get_knocked_out_turns);
}

BG_BandInfo::~BG_BandInfo()
{
	for (int i = 0; i < band_names.size(); ++i) {
		BG_BandNameInfo *d = cast_to<BG_BandNameInfo>(band_names[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < first_names.size(); ++i) {
		BG_BandNameInfo *d = cast_to<BG_BandNameInfo>(first_names[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < last_names.size(); ++i) {
		BG_BandNameInfo *d = cast_to<BG_BandNameInfo>(last_names[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	for (int i = 0; i < unit_castes.size(); ++i) {
		BG_UnitCaste *d = cast_to<BG_UnitCaste>(unit_castes[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
}

BG_UnitCaste *BG_BandInfo::get_caste_by_id(const StringName &id, bool return_parent) const
{
	auto find_caste = [this](const StringName &caste_id) -> BG_UnitCaste * {
		for (int i = 0; i < unit_castes.size(); ++i) {
			BG_UnitCaste *caste = cast_to<BG_UnitCaste>(unit_castes[i]);
			if (caste != nullptr && caste->id == caste_id)
				return caste;
		}
		return nullptr;
	};

	BG_UnitCaste *caste = find_caste(id);
	if (!return_parent)
		return caste;

	// Walk up to the top parent. Capped, so a parent loop in the data (A -> B -> A) can't hang or overflow the stack.
	for (int steps = 0; caste != nullptr && !caste->get_parent_caste().is_empty(); ++steps) {
		if (steps > unit_castes.size()) {
			UtilityFunctions::push_error("BG_BandInfo::get_caste_by_id - parent castes loop around for caste \"", id, "\". Check parent_caste in the data.");
			return caste;
		}
		caste = find_caste(caste->get_parent_caste()); // nullptr if the parent doesn't exist, same as before.
	}
	return caste;
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

BG_JobMonsterDetails::~BG_JobMonsterDetails()
{
	for (int i = 0; i < drops.size(); ++i) {
		BG_RewardItem *d = cast_to<BG_RewardItem>(drops[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
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

BG_JobDetails::~BG_JobDetails()
{
	for (int i = 0; i < monster_details.size(); ++i) {
		BG_JobMonsterDetails *d = cast_to<BG_JobMonsterDetails>(monster_details[i]);
		if (BG_Booker_DB::bg_is_instance_valid(d))
			memdelete(d);
	}
	if (BG_Booker_DB::bg_is_instance_valid(event_resource_details))
		memdelete(event_resource_details);
	for (int i = 0; i < distribution_per_act.size(); ++i) {
		BG_JobDistributionForAct *d = cast_to<BG_JobDistributionForAct>(distribution_per_act[i]);
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

Ref<BG_Item> BG_Booker_DB::create_preset_item_by_id(const StringName &id) const {
	const Dictionary &data = dber_data;
	return create_preset_item_by_id_interal(id, data);
}

Ref<BG_Item> BG_Booker_DB::create_preset_item_by_id_interal(const StringName &id, const Dictionary &data) const {
	// Preset Items
	const Array lines = get_sheet_by_name("Presets_Items", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName i_id = StringName(dber_get(entry, "id"));
		if (i_id != id) continue;

		Ref<BG_Item> result = memnew(BG_Item);
		result->id = StringName(dber_get(entry, "item_id"));
		result->random_variation = int(dber_get(entry, "variation"));

		const HashMap<String, TypedArray<StringName>> global_enums = get_global_enums(data);
		const int rarity_index = int(dber_get(entry, "rarity"));
		result->rarity_id = global_enums["rarity_types"][rarity_index];

		result->current_durability = get_item_max_durability(result, true);

		
		// Grafts
		result->grafts.clear();
		for (int x = 0; x < 4; ++x)
			result->grafts.append(nullptr);
		const Array grafts_array = dber_get(entry, "grafts");
		for (int x = 0; x < grafts_array.size(); ++x) {
			const Dictionary grafts_entry = grafts_array[x];

			Ref<BG_Item> new_graft = memnew(BG_Item);
			new_graft->id = StringName(dber_get(grafts_entry, "beast_part_id"));
			new_graft->random_variation = int(dber_get(grafts_entry, "variation"));

			const int graft_rarity_index = int(dber_get(grafts_entry, "rarity"));
			new_graft->rarity_id = global_enums["rarity_types"][graft_rarity_index];

			result->current_durability += get_item_max_durability(new_graft, false);
			result->grafts[x] = new_graft;
		}
		return result;
	}

	return nullptr;

}

Ref<BG_Band> BG_Booker_DB::create_preset_band_by_id(const StringName &id, const TypedArray<BG_Band> &existing_bands) const {
	const Dictionary &data = dber_data;

	// Preset Bands
	const Array lines = get_sheet_by_name("Presets_Bands", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName i_id = StringName(dber_get(entry, "id"));
		if (i_id != id) continue;

		const HashMap<String, TypedArray<StringName>> global_enums = get_global_enums(data);

		Ref<BG_Band> result = memnew(BG_Band);
		result->id = StringName(dber_get(entry, "band_name"));
		result->preset_band_id = id;
		result->set_unique_id(BG_Band::get_unique_band_id(existing_bands));

		// Band Members
		const Array band_members_array = dber_get(entry, "band_members");
		for (int x = 0; x < band_members_array.size(); ++x) {
			const Dictionary band_members_entry = band_members_array[x];

			Ref<BG_BandMember> new_bm = memnew(BG_BandMember);
			result->band_members.append(new_bm);
			if (x == 0) {
				result->set_band_leader(new_bm);
			}

			const int caste_index = int(dber_get(band_members_entry, "caste"));
			new_bm->caste_id = global_enums["caste_types"][caste_index];
			new_bm->level = int(dber_get(band_members_entry, "level"));
			new_bm->experience = int(dber_get(band_members_entry, "experience_points"));
			new_bm->random_variation = int(dber_get(band_members_entry, "variation"));
			new_bm->band_member_type = static_cast<BG_BandMember::BandMemberTypes>(int(dber_get(band_members_entry, "band_member_type")));
			BG_UnitCaste *unit_caste = get_band_info()->get_caste_by_id(new_bm->get_caste_id(), false);
			ERR_FAIL_COND_V_EDMSG(unit_caste == nullptr, nullptr, "ERROR : BG_Booker_DB::create_preset_band_by_id no unit caste found for:" + id);
			result->band_formation[new_bm] = int(dber_get(band_members_entry, "formation_index"));
			new_bm->band = result;

			float scale_x_z = unit_caste->get_random_unit_caste_scale(true);
			new_bm->set_scale(Vector3(scale_x_z, unit_caste->get_random_unit_caste_scale(false), scale_x_z));

			// Band Member Name
			const Array name_array = dber_get(band_members_entry, "name");
			for (int y = 0; y < name_array.size(); ++y) {
				const Dictionary name_entry = name_array[y];

				new_bm->set_first_name_id(StringName(dber_get(name_entry, "first_name")));
				new_bm->set_last_name_id(StringName(dber_get(name_entry, "last_name")));
				break;
			}

			// Item Presets
			new_bm->equipment.clear();
			for (int x = 0; x < 3; ++x)
				new_bm->equipment.append(nullptr);
			const Array item_presets_array = dber_get(band_members_entry, "item_presets");
			for (int y = 0; y < item_presets_array.size(); ++y) {
				const Dictionary item_presets_entry = item_presets_array[y];

				const int slot_type = int(dber_get(item_presets_entry, "slot_type"));
				const StringName item_preset_id = StringName(dber_get(item_presets_entry, "item_preset"));
				const Ref<BG_Item> new_item = create_preset_item_by_id_interal(item_preset_id, data);

				if (slot_type == 0)
					new_bm->equipment[1] = new_item;
				else if (slot_type == 1)
					new_bm->equipment[2] = new_item;
				else if (slot_type == 2)
					new_bm->equipment[0] = new_item;
				else
					godot::print_error("ERROR: BG_Booker_DB::create_preset_band_by_id could not slot type for item:" + String(item_preset_id));
			}
		}

		return result;
	}

	return nullptr;
}

static void get_shared_behavior_params(Dictionary &output, const Dictionary &behavior_entry) {
	const StringName ai_controller_script_name = StringName("ai_controller_script");
	const StringName ai_controller_script = ensure_clean_path(dber_get(behavior_entry, "ai_controller_script"));
	if (!ai_controller_script.is_empty() || !output.has(ai_controller_script_name)) {
		output[ai_controller_script_name] = ai_controller_script;
	}

	const StringName aggro_distance_name = StringName("aggro_distance_start_end");
	const Variant aggro_distance_dict = dber_get(behavior_entry, "aggro_distance_start_end");
	const Vector2i aggro_distance = Vector2i(int(Array(aggro_distance_dict)[0]), int(Array(aggro_distance_dict)[1]));
	if (aggro_distance.x > 0 || !output.has(aggro_distance_name)) {
		output[aggro_distance_name] = aggro_distance;
	}

	{ // Targetable Types
		TypedArray<int> targetable_types;
		const Array targetable_types_array = dber_get(behavior_entry, "targetable_types");
		for (int y = 0; y < targetable_types_array.size(); ++y) {
			const Dictionary targetable_types_entry = targetable_types_array[y];
			
			const int hex_type_index = int(dber_get(targetable_types_entry, "hex_type"));
			targetable_types.append(hex_type_index);
		}

		const StringName targetable_types_name = StringName("targetable_types");
		if (!targetable_types.is_empty() || !output.has(targetable_types_name)) {
			output[targetable_types_name] = targetable_types;
		}
	}

	{ // Dyn Targetable Types
		TypedArray<StringName> dyn_targetable_types_types;
		const Array dyn_targetable_types_array = dber_get(behavior_entry, "dyn_targetable_types");
		for (int y = 0; y < dyn_targetable_types_array.size(); ++y) {
			const Dictionary dyn_targetable_types_entry = dyn_targetable_types_array[y];
			
			dyn_targetable_types_types.append(StringName(dber_get(dyn_targetable_types_entry, "dyn_type_id")));
		}

		const StringName dyn_targetable_types_name = StringName("dyn_targetable_types");
		if (!dyn_targetable_types_types.is_empty() || !output.has(dyn_targetable_types_name)) {
			output[dyn_targetable_types_name] = dyn_targetable_types_types;
		}
	}
}

Dictionary BG_Booker_DB::get_preset_band_params_by_id(const StringName &id) const {
	const Dictionary &data = dber_data;
	Dictionary result;

	// Preset Bands
	const Array lines = get_sheet_by_name("Presets_Bands", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName i_id = StringName(dber_get(entry, "id"));
		if (i_id != id) continue;

		// Behavior
		const Array behavior_array = dber_get(entry, "behavior");
		for (int x = 0; x < behavior_array.size(); ++x) {
			const Dictionary behavior_entry = behavior_array[x];

			get_shared_behavior_params(result, behavior_entry);
		}
		
		break;
	}

	return result;
}

int BG_Booker_DB::get_rarity_index(const StringName &id) const {
	for (int i = 0; i < rarity_types.size(); ++i) {
		BG_RarityDetails *dets = cast_to<BG_RarityDetails>(rarity_types[i]);
		if (dets->get_id() == id)
			return i;
	}
	ERR_FAIL_COND_V_EDMSG(true, 0, "ERROR : BG_Booker_DB::get_rarity_index could not find index for:" + id);
	return 0;
}

Ref<BG_Monster> BG_Booker_DB::get_monster_by_id(const StringName &id) const {
	for (int i = 0; i < monster_types.size(); ++i) {
		Ref<BG_Monster> dets = cast_to<BG_Monster>(monster_types[i]);
		if (dets->get_id() == id)
			return dets;
	}
	return nullptr;
}

Ref<BG_Job> BG_Booker_DB::create_preset_monster_group_by_id(const StringName &id) const {
	const Dictionary &data = dber_data;
	const HashMap<String, TypedArray<StringName>> global_enums = get_global_enums(data);
	Ref<BG_Job> result = memnew(BG_Job);
	create_preset_monster_group_by_id_interal(result, id, Vector2i(0, 0), data, global_enums);
	return result;
}

Ref<BG_Job> BG_Booker_DB::create_preset_monster_group_by_id_interal(Ref<BG_Job> &result_out, const StringName &id, Vector2i &level_range, const Dictionary &data, const HashMap<String, TypedArray<StringName>> &global_enums) const {
	RandomNumberGenerator *rnd_gen = memnew(RandomNumberGenerator);

	// Preset Monster Groups
	const Array lines = get_sheet_by_name("Presets_Monster_Groups", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName i_id = StringName(dber_get(entry, "id"));
		if (i_id != id) continue;

		// Level Range
		const Variant level_range_dict = dber_get(entry, "level_range");
		const Vector2i level_range_this = Vector2i(int(Array(level_range_dict)[0]), int(Array(level_range_dict)[1]));
		if (level_range.x == 0 && level_range_this.x != 0) {
			level_range = level_range_this;
		}

		// Seed
		if (result_out->random_seed == 0) {
			const Array seed_array = dber_get(entry, "seed");
			for (int x = 0; x < seed_array.size(); ++x) {
				const Dictionary seed_entry = seed_array[x];

				const int seed = int(dber_get(seed_entry, "seed"));
				if (seed != 0) {
					result_out->random_seed = seed;
				}
				const bool randomize_seed = bool(dber_get(seed_entry, "randomize_seed"));
				if (randomize_seed) {
					result_out->random_seed = UtilityFunctions::randi_range(1000, 9999);
				}
			}
		}

		// Parent Monster Group
		const StringName parent_monster_group_id = StringName(dber_get(entry, "parent"));
		if (!parent_monster_group_id.is_empty()) {
			result_out = create_preset_monster_group_by_id_interal(result_out, parent_monster_group_id, level_range, data, global_enums);
		}

		result_out->job_id = id;

		rnd_gen->set_seed(result_out->random_seed);
		
		// Monsters		
		const Array monsters_array = dber_get(entry, "monsters");

		if (monsters_array.size() > 0)
			result_out->monsters.clear();
		
		for (int x = 0; x < monsters_array.size(); ++x) {
			const Dictionary monsters_entry = monsters_array[x];

			// Level Range
			const Variant spawn_count_range_dict = dber_get(monsters_entry, "spawn_count_range");
			const Vector2i spawn_count_range = Vector2i(int(Array(spawn_count_range_dict)[0]), int(Array(spawn_count_range_dict)[1]));
			const int spawn_count = rnd_gen->randi_range(spawn_count_range.x, spawn_count_range.y);
			for (int y = 0; y < spawn_count; ++y) {

				BG_Monster *m = memnew(BG_Monster);
				result_out->monsters.append(m);
				m->id = StringName(dber_get(monsters_entry, "monster"));
				m->set_job(result_out);

				const int seed_override = int(dber_get(monsters_entry, "seed_override"));
				if (seed_override != 0) {
					m->random_variation = seed_override;
				} else {
					rnd_gen->randi_range(1000, 9999);
				}

				const int level_override = int(dber_get(monsters_entry, "level_override"));
				if (level_override != 0) {
					m->level = level_override;
				} else {
					m->level = rnd_gen->randi_range(level_range.x, level_range.y);
				}
			}
		}

		return result_out;
	}

	return nullptr;
}

Dictionary BG_Booker_DB::get_preset_monster_group_params_by_id(const StringName &id) const {
	const Dictionary &data = dber_data;
	Dictionary result;
	get_preset_monster_group_params_by_id_interal(id, result, data);
	return result;
}

void BG_Booker_DB::get_preset_monster_group_params_by_id_interal(const StringName &id, Dictionary &output, const Dictionary &data) const {
	// Preset Monster Groups
	const Array lines = get_sheet_by_name("Presets_Monster_Groups", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName i_id = StringName(dber_get(entry, "id"));
		if (i_id != id) continue;

		// Parent Monster Group
		const StringName parent_monster_group_id = StringName(dber_get(entry, "parent"));
		if (!parent_monster_group_id.is_empty()) {
			get_preset_monster_group_params_by_id_interal(parent_monster_group_id, output, data);
		}

		// Behavior
		const Array behavior_array = dber_get(entry, "behavior");
		for (int x = 0; x < behavior_array.size(); ++x) {
			const Dictionary behavior_entry = behavior_array[x];

			get_shared_behavior_params(output, behavior_entry);
		}

		break;
	}
}

TypedArray<BG_RewardItem> BG_Booker_DB::get_drop_rewards_from_monster_group_preset(Ref<BG_Job> job) const {
	if (!job.is_valid()) return {};
	if (job->get_job_id().is_empty()) return {};
	const Dictionary &data = dber_data;
	const HashMap<String, TypedArray<StringName>> global_enums = get_global_enums(data);
	return get_drop_rewards_from_monster_group_preset_interal(job, job->get_job_id(), data, global_enums);
}

TypedArray<BG_RewardItem> BG_Booker_DB::get_drop_rewards_from_monster_group_preset_interal(const Ref<BG_Job> &job, const StringName &id, const Dictionary &data, const HashMap<String, TypedArray<StringName>> &global_enums) const {
	TypedArray<BG_RewardItem> result;

	// Preset Monster Groups
	const Array lines = get_sheet_by_name("Presets_Monster_Groups", data);
	for (int i = 0; i < lines.size(); ++i) {
		const Dictionary entry = lines[i];

		const StringName i_id = StringName(dber_get(entry, "id"));
		if (i_id != id) continue;

		// Parent Monster Group
		const StringName parent_monster_group_id = StringName(dber_get(entry, "parent"));
		if (!parent_monster_group_id.is_empty()) {
			result.append_array(get_drop_rewards_from_monster_group_preset_interal(job, parent_monster_group_id, data, global_enums));
		}

		// Base Monster Drops
		if (job->get_job_id() == id) {
			for (int x = 0; x < job->get_monsters().size(); ++x) {
				const Ref<BG_Monster> monster = cast_to<BG_Monster>(job->get_monsters()[x]);
				if (monster == nullptr) continue;
				const Ref<BG_Monster> global_monster = get_monster_by_id(monster->get_id());

				for (int y = 0; y < global_monster->get_drops().size(); ++y) {
					const BG_RewardItem *reward_item = cast_to<BG_RewardItem>(global_monster->get_drops()[y]);
					if (reward_item == nullptr) continue;
					result.append(reward_item);
				}
			}
		}

		// Drops
		const Variant drops_values = dber_get(entry, "drops");
		const Array drops_array = drops_values;
		for (int x = 0; x < drops_array.size(); ++x) {
			const Dictionary drops_entry = drops_array[x];

			const float drop_weight = float(dber_get(drops_entry, "drop_weight"));

			const StringName item_id = StringName(dber_get(drops_entry, "item_id"));
			if (!item_id.is_empty()) {
				BG_RewardItem *new_job_reward = memnew(BG_RewardItem);
				new_job_reward->drop_weight = drop_weight;

				// Forced Rarity Availabilities
				const Array forced_rarity_availabilities_array = dber_get(drops_entry, "forced_rarity_availabilities");
				for (int y = 0; y < forced_rarity_availabilities_array.size(); ++y) {
					const Dictionary forced_rarity_availabilities_entry = forced_rarity_availabilities_array[y];

					const int rarity_index = int(dber_get(forced_rarity_availabilities_entry, "rarity"));
					new_job_reward->rarity_availabilities.append(global_enums["rarity_types"][rarity_index]);
				}

				result.append(new_job_reward);
			}

			// Item Drop Pool Items
			const StringName item_drop_pool_id = StringName(dber_get(drops_entry, "item_drop_pool_id"));
			if (item_drop_pool_id.is_empty()) continue;
			for (int pool_index = 0; pool_index < item_drop_pools.size(); ++pool_index)
			{
				const BG_ItemDropPool *pool = cast_to<BG_ItemDropPool>(item_drop_pools[pool_index]);
				if (pool == nullptr || pool->id != item_drop_pool_id) continue;
				
				for (int item_index = 0; item_index < pool->item_drops.size(); ++item_index)
				{
					const BG_RewardItem *reward_item = cast_to<BG_RewardItem>(pool->item_drops[item_index]);
					if (reward_item == nullptr) continue;

					BG_RewardItem *new_job_reward = memnew(BG_RewardItem);
					new_job_reward->id = reward_item->id;
					new_job_reward->drop_weight = drop_weight;

					// Forced Rarity Availabilities
					const Array forced_rarity_availabilities_array = dber_get(drops_entry, "forced_rarity_availabilities");
					for (int y = 0; y < forced_rarity_availabilities_array.size(); ++y) {
						const Dictionary forced_rarity_availabilities_entry = forced_rarity_availabilities_array[y];

						const int rarity_index = int(dber_get(forced_rarity_availabilities_entry, "rarity"));
						new_job_reward->rarity_availabilities.append(global_enums["rarity_types"][rarity_index]);
					}
					if (new_job_reward->rarity_availabilities.is_empty())
						new_job_reward->rarity_availabilities = reward_item->rarity_availabilities.duplicate();

					result.append(new_job_reward);
				}
				break;
			}
		}

		break;
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

// The skills from DBer's Skills tab ("skills_data" in the DBer data) are where tree skills live, so they replace every
// caste's attribute tree from the Caste_Types sheet. Each skill goes in the tree of its "caste". Child castes also get
// their parent's skills, on top (StaticGlobals.get_caste_attribute_tree_skills). Unlock and lockout values are ">="
// conditions. Called at the end of try_parse_bder_data, after the castes exist.
void BG_Booker_DB::try_parse_skills_data(const Dictionary &p_skills_data)
{
	skills_data = p_skills_data;
	if (skills_data.is_empty())
		return; // No skills in the DBer data: castes keep their trees from the Caste_Types sheet.
	const Array skills = skills_data.get("skills", Array());

	for (int c = 0; c < band_info->unit_castes.size(); ++c)
	{
		BG_UnitCaste *caste = cast_to<BG_UnitCaste>(band_info->unit_castes[c]);
		if (caste != nullptr)
			caste->attribute_skill_tree_datas.clear();
	}

	auto add_conditions = [](const Dictionary &values, TypedArray<BG_Skill_LockUnLock_Condition> &conditions) {
		const Array attribute_ids = values.keys();
		for (int k = 0; k < attribute_ids.size(); ++k)
		{
			BG_Skill_LockUnLock_Condition *condition = memnew(BG_Skill_LockUnLock_Condition);
			condition->condition_attribute = StringName(attribute_ids[k]);
			condition->condition_type = BG_Skill_LockUnLock_Condition::EQUAL_TO_OR_GREATER_THAN;
			condition->condition_value = int(values[attribute_ids[k]]);
			conditions.append(condition);
		}
	};

	for (int i = 0; i < skills.size(); ++i)
	{
		const Dictionary skill = skills[i];
		// Only class skills are in trees. Monster skills (monsters' loadouts) and base skills (e.g. the basic attack,
		// everyone has it) aren't (BG_SkillData.owner).
		if (String(skill.get("owner", "class")) != "class")
			continue;
		const StringName skill_id = StringName(skill.get("id", ""));
		const StringName caste_id = StringName(skill.get("caste", ""));
		BG_UnitCaste *caste = nullptr;
		for (int c = 0; c < band_info->unit_castes.size(); ++c)
		{
			BG_UnitCaste *candidate = cast_to<BG_UnitCaste>(band_info->unit_castes[c]);
			if (candidate != nullptr && candidate->id == caste_id)
			{
				caste = candidate;
				break;
			}
		}
		if (caste == nullptr)
		{
			UtilityFunctions::push_warning("DBer skills: skill \"", skill_id, "\" has caste \"", caste_id, "\", which doesn't exist. It won't be in any tree.");
			continue;
		}

		BG_AttributeTreeSkillData *tree_skill = memnew(BG_AttributeTreeSkillData);
		tree_skill->effect_id = skill_id;
		tree_skill->is_passive = String(skill.get("kind", "active")) != "active";
		add_conditions(skill.get("unlock", Dictionary()), tree_skill->unlock_conditions);
		add_conditions(skill.get("lockout", Dictionary()), tree_skill->lock_conditions);
		caste->attribute_skill_tree_datas.append(tree_skill);
	}
}

/* static */ void BG_Booker_DB::timer_test(const Callable &callable)
{
	static counter cont = counter();
	TIME_FUNC(callable, cont);
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

void BG_Booker_DB::try_parse_bder_data(const String &file_path)
{
	// Kept for the loaders that read sheets later (storyboards, presets, ...), so the file is only parsed once.
	dber_data = BG_JsonUtils::ParseJsonFile(file_path);
	const Dictionary &data = dber_data;

	HashMap<String, TypedArray<StringName>> global_enums = get_global_enums(data);

	{ // Global Curves: {curve name: path}
		const Dictionary curves = data.get("global_curves", Dictionary());
		const Array curve_names = curves.keys();
		for (int i = 0; i < curve_names.size(); ++i) {
			globals->global_curves[StringName(curve_names[i])] = ensure_clean_path(curves[curve_names[i]]);
		}
	}

	{ // Base Stats: the "Base_Stats" sheet, one row per stat (id, name, value, is_base_value, parent).
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

	auto get_animation_detail = [](const HashMap<String, TypedArray<StringName>> &global_enums, const Array &animations, int index) -> BG_AnimationDetails *{
		const Dictionary aa = animations[index];
		
		BG_AnimationDetails *new_class = memnew(BG_AnimationDetails);
		new_class->caste_or_monster_id = global_enums["caste_types"][int(dber_get(aa, "caste"))];
		new_class->anim_type = static_cast<BG_AnimationDetails::AnimationType>(int(dber_get(aa, "anim_type")));
		new_class->anim_name = StringName(dber_get(aa, "anim_name"));
		new_class->anim_blend_time = float(dber_get(aa, "anim_blend_time"));
		
		{ // Traverse Types
			const Array anim_traverse_types_array = dber_get(aa, "animation_traverse_types");
			if (!anim_traverse_types_array.is_empty()) {
				const Dictionary anim_traverse_types = anim_traverse_types_array[0];
				new_class->traverse_to = static_cast<BG_AnimationDetails::AnimationTraverseType>(int(dber_get(anim_traverse_types, "traverse_to")));
				new_class->traverse_back = static_cast<BG_AnimationDetails::AnimationTraverseType>(int(dber_get(anim_traverse_types, "traverse_back")));
			}
		}

		{ // Tags
			const Array anim_tags_array = dber_get(aa, "tags");
			for (int i = 0; i < anim_tags_array.size(); ++i) {
				const Dictionary anim_tag_entry = anim_tags_array[i];

				BG_AnimationTagDetails *new_tag_class = memnew(BG_AnimationTagDetails);
				new_tag_class->tag = StringName(dber_get(anim_tag_entry, "tag"));
				new_tag_class->frame = int(dber_get(anim_tag_entry, "frame"));

				new_class->tags.append(new_tag_class);
			}
		}

		return new_class;
	};

	{ // Stat Types
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

	{ // Animation classes, then the weapon and armor categories (rows by id)
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

	{ // Content
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

	{ // Audio
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

	{ // Bands
		const Array lines = get_sheet_by_name("Bands", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];

			band_info->num_band_members_for_hire = int(dber_get(entry, "num_band_members_for_hire"));
			band_info->rest_recovery_speed = float(dber_get(entry, "rest_recovery_speed"));
			band_info->knocked_out_turns = int(dber_get(entry, "knocked_out_turns"));
			const Variant band_sizes_min_max = dber_get(entry, "band_sizes_min_max");
			band_info->band_size_min_max = Vector2(float(Array(band_sizes_min_max)[0]), float(Array(band_sizes_min_max)[1]));

			const Variant first_names = dber_get(entry, "first_names");
			const Array first_names_array = first_names;
			for (int x = 0; x < first_names_array.size(); ++x) {
				const Dictionary first_names_entry = first_names_array[x];

				BG_BandNameInfo *new_band_name_info = memnew(BG_BandNameInfo);
				new_band_name_info->name = StringName(dber_get(first_names_entry, "name"));
				new_band_name_info->allowed_in_random = bool(dber_get(first_names_entry, "allowed_in_random"));
				band_info->first_names.append(new_band_name_info);
			}
			
			const Variant last_names = dber_get(entry, "last_names");
			const Array last_names_array = last_names;
			for (int x = 0; x < last_names_array.size(); ++x) {
				const Dictionary last_names_entry = last_names_array[x];

				BG_BandNameInfo *new_band_name_info = memnew(BG_BandNameInfo);
				new_band_name_info->name = StringName(dber_get(last_names_entry, "name"));
				new_band_name_info->allowed_in_random = bool(dber_get(last_names_entry, "allowed_in_random"));
				band_info->last_names.append(new_band_name_info);
			}
			
			const Array bands_array = dber_get(entry, "bands");
			for (int x = 0; x < bands_array.size(); ++x) {
				const Dictionary bands_entry = bands_array[x];

				BG_BandNameInfo *new_band_name_info = memnew(BG_BandNameInfo);
				new_band_name_info->name = StringName(dber_get(bands_entry, "name"));
				band_info->band_names.append(new_band_name_info);
				band_info->icon_paths.append(ensure_clean_path(dber_get(bands_entry, "icon_path")));
			}
		}
	}

	{ // Battle Boards
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

	{ // Booker Skill Tree
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

	{ // Caste Types
		const Array lines = get_sheet_by_name("Caste_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];

			BG_UnitCaste *new_unit_caste = memnew(BG_UnitCaste);
			new_unit_caste->id = StringName(dber_get(entry, "id"));
			new_unit_caste->parent_caste = StringName(dber_get(entry, "parent_caste"));
			new_unit_caste->is_unique_caste = bool(dber_get(entry, "is_unique_caste"));
			new_unit_caste->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));

			// LOD Mesh Paths
			new_unit_caste->lod_mesh_paths.clear();
			const Array lod_mesh_paths_values_array = dber_get(entry, "lod_mesh_paths");
			for (int x = 0; x < lod_mesh_paths_values_array.size(); ++x) {
				const Dictionary lod_mesh_paths_values_entry = lod_mesh_paths_values_array[x];
				new_unit_caste->lod_mesh_paths.append(ensure_clean_path(dber_get(lod_mesh_paths_values_entry, "path")));
			}

			// Scale Limits
			const Array scale_limits_values_array = dber_get(entry, "scale_limits");
			for (int x = 0; x < scale_limits_values_array.size(); ++x) {
				const Dictionary scale_limits_values_entry = scale_limits_values_array[x];

				new_unit_caste->scale_min = Vector2(
					float(dber_get(scale_limits_values_entry, "min_x")),
					float(dber_get(scale_limits_values_entry, "min_y"))
				);
				new_unit_caste->scale_max = Vector2(
					float(dber_get(scale_limits_values_entry, "max_x")),
					float(dber_get(scale_limits_values_entry, "max_y"))
				);
				new_unit_caste->scale_min_extreme = Vector2(
					float(dber_get(scale_limits_values_entry, "extreme_min_x")),
					float(dber_get(scale_limits_values_entry, "extreme_min_y"))
				);
				new_unit_caste->scale_max_extreme = Vector2(
					float(dber_get(scale_limits_values_entry, "extreme_max_x")),
					float(dber_get(scale_limits_values_entry, "extreme_max_y"))
				);
			}

			// Misc Stats
			const Array misc_stats_values_array = dber_get(entry, "misc_stats");
			for (int x = 0; x < misc_stats_values_array.size(); ++x) {
				const Dictionary misc_stats_values_entry = misc_stats_values_array[x];
				new_unit_caste->travel_distance = int(dber_get(misc_stats_values_entry, "travel_distance"));
				new_unit_caste->inventory_space = int(dber_get(misc_stats_values_entry, "inventory_space"));
			}

			// Base Stat Type Values
			const Array base_stat_type_values_array = dber_get(entry, "base_stat_type_values");
			for (int x = 0; x < base_stat_type_values_array.size(); ++x) {
				const Dictionary base_stat_type_values_entry = base_stat_type_values_array[x];

				BG_UnitStat *new_stat = memnew(BG_UnitStat);
				new_stat->id = StringName(dber_get(base_stat_type_values_entry, "damage_type"));
				new_stat->bonus_percentage = float(dber_get(base_stat_type_values_entry, "base_bonus_percentage"));
				new_stat->defensive_value = int(dber_get(base_stat_type_values_entry, "starting_value"));

				new_unit_caste->stats.append(new_stat);
			}

			// Random Starting Items
			const Array random_starting_items_values_array = dber_get(entry, "random_starting_items");
			for (int x = 0; x < random_starting_items_values_array.size(); ++x) {
				const Dictionary random_starting_items_values_entry = random_starting_items_values_array[x];
				new_unit_caste->starting_item_ids.append(
					StringName(dber_get(random_starting_items_values_entry, "item"))
				);
			}

			// Element Availability
			const Array element_availability_values_array = dber_get(entry, "element_availability");
			for (int x = 0; x < element_availability_values_array.size(); ++x) {
				const Dictionary element_availability_values_entry = element_availability_values_array[x];
				new_unit_caste->element_availability_ids.append(
					StringName(dber_get(element_availability_values_entry, "element"))
				);
			}

			// Attribute Skill Tree
			auto is_attribute_stat_id = [this](const StringName &stat_id) -> bool {
				for (int s = 0; s < stat_types.size(); ++s) {
					const BG_UnitStatDetails *stat = cast_to<BG_UnitStatDetails>(stat_types[s]);
					if (stat != nullptr && stat->is_attribute_type && stat->id == stat_id)
						return true;
				}
				return false;
			};
			const Array attribute_skill_tree_array = dber_get(entry, "attribute_skill_tree");
			for (int x = 0; x < attribute_skill_tree_array.size(); ++x)
			{
				const Dictionary attribute_skill_tree_entry = attribute_skill_tree_array[x];

				BG_AttributeTreeSkillData *attr_tree_skill_data = memnew(BG_AttributeTreeSkillData);
				attr_tree_skill_data->effect_id = StringName(dber_get(attribute_skill_tree_entry, "effect_id"));
				attr_tree_skill_data->is_passive = bool(dber_get(attribute_skill_tree_entry, "is_passive"));

				// Conditions
				const Array conditions_array = dber_get(attribute_skill_tree_entry, "conditions");
				for (int y = 0; y < conditions_array.size(); ++y)
				{
					const Dictionary conditions_entry = conditions_array[y];

					BG_Skill_LockUnLock_Condition *lock_unlock_condition = memnew(BG_Skill_LockUnLock_Condition);

					const String where = "caste \"" + String(new_unit_caste->id) + "\", skill \"" + String(attr_tree_skill_data->effect_id) + "\"";

					lock_unlock_condition->condition_attribute = StringName(dber_get(conditions_entry, "condition_attribute"));
					if (!is_attribute_stat_id(lock_unlock_condition->condition_attribute))
						UtilityFunctions::push_warning("Attribute tree: ", where, " has a condition on \"", lock_unlock_condition->condition_attribute, "\", which isn't an attribute stat id. It will never match.");

					const int condition_type = int(dber_get(conditions_entry, "condition_type"));
					if (condition_type < BG_Skill_LockUnLock_Condition::EQUAL_TO_OR_GREATER_THAN || condition_type > BG_Skill_LockUnLock_Condition::LESSER_THAN)
						UtilityFunctions::push_warning("Attribute tree: ", where, " has an unknown condition_type ", condition_type, ". Using EQUAL_TO_OR_GREATER_THAN.");
					else
						lock_unlock_condition->condition_type = static_cast<BG_Skill_LockUnLock_Condition::ConditionTypes>(condition_type);
					lock_unlock_condition->condition_value = int(dber_get(conditions_entry, "condition_value"));

					const Variant is_unlock_param = dber_find(conditions_entry, "is_unlock_condition");
					if (is_unlock_param.get_type() == Variant::NIL)
						UtilityFunctions::push_warning("Attribute tree: ", where, " has a condition without is_unlock_condition. Treating it as an unlock condition.");
					if (is_unlock_param.get_type() == Variant::NIL || bool(is_unlock_param))
						attr_tree_skill_data->unlock_conditions.append(lock_unlock_condition);
					else
						attr_tree_skill_data->lock_conditions.append(lock_unlock_condition);
				}

				new_unit_caste->attribute_skill_tree_datas.append(attr_tree_skill_data);
			}

			band_info->unit_castes.append(new_unit_caste);
		}
	}

	{ // Characters
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

	{ // City Info
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

	{ // Effects
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

	{ // Game Maps
		const Array lines = get_sheet_by_name("Game_Maps", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];

			BG_GameMapDetails *new_game_map_class = memnew(BG_GameMapDetails);
			new_game_map_class->id = StringName(dber_get(entry, "id"));
			new_game_map_class->scene_path = ensure_clean_path(dber_get(entry, "scene_path"));

			game_map_details.append(new_game_map_class);
		}
	}

	{ // Game Map Nodes
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

	{ // Icons
		const Array lines = get_sheet_by_name("Icons", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];

			BG_IconDetails *new_icons_class = memnew(BG_IconDetails);
			new_icons_class->id = StringName(dber_get(entry, "id"));
			new_icons_class->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));

			icon_details.append(new_icons_class);
		}
	}

	{ // Item Drop Pools
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

	{ // Item Slot Types
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

	{ // Mail Data
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

	{ // Misc Globals
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

	{ // Puzzles
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

	{ // Objectives
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

	{ // Rarity Type
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

	{ // Resource Types
		const Array lines = get_sheet_by_name("Resource_Types", data);
		for (int i = 0; i < lines.size(); ++i) {
			const Dictionary entry = lines[i];

			BG_ResourceTypeDetails *new_resource_type_class = memnew(BG_ResourceTypeDetails);
			new_resource_type_class->id = StringName(dber_get(entry, "id"));
			new_resource_type_class->icon_path = ensure_clean_path(dber_get(entry, "icon_path"));

			resource_type_details[new_resource_type_class->id] = new_resource_type_class;
		}
	}

	{ // 2der
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

	// Skills last: they fill the attribute trees of the castes parsed above.
	try_parse_skills_data(data.get("skills_data", Dictionary()));

	try_parse_localization_data(data.get("localization_data", Dictionary()));
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

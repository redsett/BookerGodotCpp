#pragma once

// Booker_DB and every data class it parses from booker_dber_data.json. The data classes are split by area:
// bg_booker_types_content.hpp (stats, effects, rarities, animations, items, drops), bg_booker_types_world.hpp
// (storyboards, battle boards, maps, objectives, marketplace, audio, acts, cities) and bg_booker_types_units.hpp
// (band members, bands, castes, jobs, monsters). Their methods are in the matching .cpp files.

#include "bg_booker_types_fwd.hpp"
#include "bg_booker_types_content.hpp"
#include "bg_booker_types_world.hpp"
#include "bg_booker_types_units.hpp"


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
	~BG_Booker_Globals();

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

	float crit_qte_perfect_window = 0.0;
	float get_crit_qte_perfect_window() const { return crit_qte_perfect_window; }

	float crit_qte_window_additive = 0.0;
	float get_crit_qte_window_additive() const { return crit_qte_window_additive; }
	
	// Day Night Cycle
	int day_night_cycle_turns_per_day = 0;
	int get_day_night_cycle_turns_per_day() const { return day_night_cycle_turns_per_day; }

	float percentage_of_day_is_day_time = 0.0;
	float get_percentage_of_day_is_day_time() const { return percentage_of_day_is_day_time; }

	TypedArray<int> day_cycle_offset_per_act;
	TypedArray<int> get_day_cycle_offset_per_act() const { return day_cycle_offset_per_act; }

	float band_action_points_fatigue_rate = 0.0;
	float get_band_action_points_fatigue_rate() const { return band_action_points_fatigue_rate; }

	float monster_action_points_fatigue_rate = 0.0;
	float get_monster_action_points_fatigue_rate() const { return monster_action_points_fatigue_rate; }

	float action_points_rest_rate = 0.0;
	float get_action_points_rest_rate() const { return action_points_rest_rate; }

	int exp_when_source_unit_damaged = 0;
	int get_exp_when_source_unit_damaged() const { return exp_when_source_unit_damaged; }

	int exp_when_enemy_killed = 0;
	int get_exp_when_enemy_killed() const { return exp_when_enemy_killed; }

	int exp_when_ap_consumed = 0;
	int get_exp_when_ap_consumed() const { return exp_when_ap_consumed; }

	int attribute_points_per_level = 0;
	int get_attribute_points_per_level() const { return attribute_points_per_level; }
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

	const String booker_dber_data_file_name = "booker_dber_data.json";
	// The parsed DBer data, kept after loading for the loaders that read it later (storyboards, presets, ...).
	Dictionary dber_data;

public:
	BG_Booker_DB();
	~BG_Booker_DB();

	const StringName refreshed_data_signal_name = "refreshed_data";

    // static bool bg_is_instance_valid(const Variant &p_object) {
	// 	try {
	// 		Object* obj = p_object.get_validated_object();
	// 		if (!obj) {
	// 			return false;
	// 		}

	// 		Object* node = Object::cast_to<Object>(obj);
	// 		if (node) {
	// 			return true;
	// 		}
	// 	}
	// };
    static bool bg_is_instance_valid(const Object *obj) {
        try {
            // return (obj && UtilityFunctions::is_instance_id_valid(obj->get_instance_id()));
			return (obj && ObjectDB::get_instance(obj->get_instance_id()) != nullptr);
        }
        catch (...) { }
        return false;
    };

	static BG_Booker_DB *get_singleton();

	void refresh_data();

	static void timer_test(const Callable &callable);

	StringName current_language_key = "en";
	StringName get_current_language_key() const { return current_language_key; }
	void set_current_language_key(const StringName &v) { current_language_key = v; }

	String modding_path = "";
	String get_modding_path() const { return modding_path; }

	BG_Booker_Globals *globals = nullptr;
	BG_Booker_Globals *get_globals() const { return globals; }

	TypedArray<BG_CharacterDetails> character_details;
	TypedArray<BG_CharacterDetails> get_character_details() const { return character_details; }
	BG_CharacterDetails *get_character_details_by_id(const StringName &id) const;

	Ref<BG_StoryboardDetails> import_and_get_storyboard_details_by_id(const StringName &id);

	Ref<BG_TutorialCardsDetails> import_and_get_tutorial_cards_details_by_id(const StringName &id);

	TypedArray<BG_ObjectiveDetails> objectives;
	TypedArray<BG_ObjectiveDetails> get_objectives() const { return objectives; }	

	TypedArray<BG_BaseStat> base_stats;
	TypedArray<BG_BaseStat> get_base_stats() const { return base_stats; }
	
	TypedArray<BG_BattleBoardDetails> battle_boards_details;
	TypedArray<BG_BattleBoardDetails> get_battle_boards_details() const { return battle_boards_details; }
	BG_BattleBoardDetails *get_battle_board_by_id(const StringName &id) const;
	TypedArray<BG_BattleBoard_HexTypeDetails> get_battle_board_details_for_bb(const StringName &bb_id) const;
	void get_battle_board_details_for_bb_from_sorted(const StringName &bb_id, HashMap<StringName, TypedArray<BG_BattleBoard_HexTypeDetails>> &sorted) const;
	TypedArray<BG_BattleBoard_HexTypeDetails> get_battle_board_hex_types_by_type(const StringName &bb_id, int type, bool is_game_type = true) const;
	BG_BattleBoard_HexTypeDetails *get_battle_board_hex_type_by_id(const StringName &bb_id, const StringName &id) const;
	StringName get_battle_board_music_id(const StringName &bb_id) const;
	StringName get_battle_board_ambient_id(const StringName &bb_id) const;

	TypedArray<BG_GameMapNodeDetails> game_map_node_details;
	TypedArray<BG_GameMapNodeDetails> get_game_map_node_details() const { return game_map_node_details; }
	BG_GameMapNodeDetails *get_game_map_node_details_by_id(const StringName &id) const;

	TypedArray<BG_GameMapDetails> game_map_details;
	TypedArray<BG_GameMapDetails> get_game_map_details() const { return game_map_details; }
	BG_GameMapDetails *get_game_map_details_by_id(const StringName &id) const;

	HashMap<StringName, BG_ResourceTypeDetails *> resource_type_details;
	BG_ResourceTypeDetails *get_resource_type_details_by_id(const StringName &resource_id) const;

	TypedArray<BG_AudioData> audio_data;
	TypedArray<BG_AudioData> get_audio_data() const { return audio_data; }
	TypedArray<BG_AudioDataDetails> get_audio_details(const StringName &id, int act) const;

	TypedArray<BG_IconDetails> icon_details;
	TypedArray<BG_IconDetails> get_icon_details() const { return icon_details; }
	BG_IconDetails *get_icon_details_by_id(const StringName &id) const;

	TypedArray<BG_BookerSkillTreeSlotDetails> booker_skill_tree_details;
	TypedArray<BG_BookerSkillTreeSlotDetails> get_booker_skill_tree_details() const { return booker_skill_tree_details; }

	TypedArray<BG_JobDetails> jobs;
	TypedArray<BG_JobDetails> get_jobs() const { return jobs; }

	TypedArray<BG_ItemDetails> items;
	TypedArray<BG_ItemDetails> get_items() const { return items; }
	BG_ItemDetails *get_item_details_by_id(const StringName &id) const;

	// How each animation class moves ("Animation_Classes" sheet): {class id: TypedArray<BG_AnimationDetails>}. A weapon
	// category names its class; an item's own animations override it.
	Dictionary animation_classes;
	TypedArray<BG_AnimationDetails> get_animation_class_animations(const StringName &id) const;

	// The "Weapon_Categories" / "Armor_Categories" sheets' rows by id: {id: {column: value}}. Weapon rows have
	// "animation_class". Kept as rows, so new columns need no C++.
	Dictionary weapon_categories;
	Dictionary get_weapon_categories() const { return weapon_categories; }
	Dictionary armor_categories;
	Dictionary get_armor_categories() const { return armor_categories; }

	// Any DBer sheet's rows as exported ([{column: value}]), or [] if there's no such sheet. For sheets the game reads
	// in GDScript (e.g. "Cues"), so a new sheet needs no C++.
	Array get_sheet_rows(const String &sheet_name) const;

	// A DBer global enum's value names, in order ([] if there's none), e.g. "animation_types": the animation roles.
	// Animation rows store a role as its index in that list, so roles added in DBer work without C++ (past the end of
	// BG_AnimationDetails::AnimationType, which only names the built-in ones).
	Array get_global_enum_values(const String &enum_name) const;
	float get_item_max_durability(Ref<BG_Item> item, bool full_value) const;
	Ref<BG_Item> create_preset_item_by_id(const StringName &id) const;
	Ref<BG_Item> create_preset_item_by_id_interal(const StringName &id, const Dictionary &data) const;

	TypedArray<BG_ItemDropPool> item_drop_pools;
	TypedArray<BG_ItemDropPool> get_item_drop_pools() const { return item_drop_pools; }
	BG_ItemDropPool *get_item_drop_pool_by_id(const StringName &id) const;

	TypedArray<BG_Effect> effects;
	TypedArray<BG_Effect> get_effects() const { return effects; }

	BG_BandInfo *band_info = nullptr;
	BG_BandInfo *get_band_info() const { return band_info; }

	// The skills from DBer's Skills tab ("skills_data" in booker_dber_data.json): {"version", "classes", "skills": [...]}.
	// Empty if the DBer data has none. GDScript builds the runtime skills from it (BG_SkillLibrary). It also fills
	// every caste's attribute tree.
	Dictionary skills_data;
	Dictionary get_skills_data() const { return skills_data; }
	Ref<BG_Band> create_preset_band_by_id(const StringName &id, const TypedArray<BG_Band> &existing_bands) const;
	Dictionary get_preset_band_params_by_id(const StringName &id) const;

	TypedArray<BG_ItemSlotType> item_slot_types;
	TypedArray<BG_ItemSlotType> get_item_slot_types() const { return item_slot_types; }

	TypedArray<BG_RarityDetails> rarity_types;
	TypedArray<BG_RarityDetails> get_rarity_types() const { return rarity_types; }
	int get_rarity_index(const StringName &id) const;

	TypedArray<BG_UnitStatDetails> stat_types;
	TypedArray<BG_UnitStatDetails> get_stat_types() const { return stat_types; }

	BG_MarketplaceData *market_place_data = nullptr;
	BG_MarketplaceData *get_market_place_data() const { return market_place_data; }

	TypedArray<BG_Monster> monster_types;
	TypedArray<BG_Monster> get_monster_types() const { return monster_types; }
	Ref<BG_Monster> get_monster_by_id(const StringName &id) const;
	Ref<BG_Job> create_preset_monster_group_by_id(const StringName &id) const;
	Ref<BG_Job> create_preset_monster_group_by_id_interal(Ref<BG_Job> &result_out, const StringName &id, Vector2i &level_range, const Dictionary &data, const HashMap<String, TypedArray<StringName>> &global_enums) const;
	TypedArray<BG_RewardItem> get_drop_rewards_from_monster_group_preset(Ref<BG_Job> job) const;
	TypedArray<BG_RewardItem> get_drop_rewards_from_monster_group_preset_interal(const Ref<BG_Job> &job, const StringName &id, const Dictionary &data, const HashMap<String, TypedArray<StringName>> &global_enums) const;
	Dictionary get_preset_monster_group_params_by_id(const StringName &id) const;
	void get_preset_monster_group_params_by_id_interal(const StringName &id, Dictionary &output, const Dictionary &data) const;

	TypedArray<BG_MailData> mail_data;
	TypedArray<BG_MailData> get_mail_data() const { return mail_data; }

	TypedArray<BG_PuzzleDetails> puzzles;
	TypedArray<BG_PuzzleDetails> get_puzzles() const { return puzzles; }
	BG_PuzzleDetails *get_puzzle_details_by_id(const StringName &id) const;
	
	TypedArray<BG_TwoDer_DataEntry> two_der_data_entries;
	TypedArray<BG_TwoDer_DataEntry> get_two_der_data_entries() const { return two_der_data_entries; }
	BG_TwoDer_DataEntry *get_two_der_data_from_id(const StringName &id) const;

	bool revert_localization_to_english = true;
	void set_revert_localization_to_english(bool revert) { revert_localization_to_english = revert; }
	// The text from DBer's Localization tab ("localization_data" in the DBer data): {sheet: {key: [line, ...]}}, where a
	// line is {"code_start", "code_end", language: text}.
	Dictionary localization_source;
	// Entries made from localization_source, by "sheet|language", then key.
	HashMap<StringName, HashMap<StringName, TypedArray<BG_LocalizeEntryData>>> localize_data;
	void clear_localize_data();
	TypedArray<BG_LocalizeEntryData> get_localize_data(const StringName &sheet_name, const StringName &key, const StringName &language);
	String get_localize_string(const StringName &sheet_name, const StringName &key, const StringName &language, bool ignore_code_data = false);

	BG_BaseStat *get_stat_from_stat_id_name(const StringName &stat_id_name) const;

private:
	void free_all_params();

	void try_parse_data(const String &file_path);
	void try_parse_bder_data(const String &file_path);
	// try_parse_bder_data's sheet parsers, in bg_booker_db_parse*.cpp.
	void parse_global_curves(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_base_stats(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_stat_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_animation_classes(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_content(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_audio(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_bands(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_battle_boards(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_booker_skill_tree(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_caste_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_characters(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_city_info(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_effects(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_game_maps(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_game_map_nodes(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_icons(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_item_drop_pools(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_item_slot_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_mail_data(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_misc_globals(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_puzzles(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_objectives(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_rarity_type(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_resource_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void parse_two_der(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums);
	void try_parse_skills_data(const Dictionary &p_skills_data);
	void try_parse_localization_data(const Dictionary &p_localization_data);

	BG_BaseStat *get_stat_from_unique_id(int unique_id) const;
};

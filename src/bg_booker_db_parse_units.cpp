// Booker_DB sheet parsers: bands and caste types, then the skills (attribute trees) from DBer's Skills tab.
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

// Bands
void BG_Booker_DB::parse_bands(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
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

// Caste Types
void BG_Booker_DB::parse_caste_types(const Dictionary &data, HashMap<String, TypedArray<StringName>> &global_enums)
{
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

		band_info->unit_castes.append(new_unit_caste);
	}
}

// Builds every caste's attribute tree from DBer's Skills tab ("skills_data" in the DBer data), where tree skills
// live. Each skill goes in the tree of its "caste". Child castes also get
// their parent's skills, on top (BG_Attributes.get_caste_attribute_tree_skills). Unlock and lockout values are ">="
// conditions. Called at the end of try_parse_bder_data, after the castes exist.
void BG_Booker_DB::try_parse_skills_data(const Dictionary &p_skills_data)
{
	skills_data = p_skills_data;
	if (skills_data.is_empty())
		return; // No skills in the DBer data: every caste's tree stays empty.
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

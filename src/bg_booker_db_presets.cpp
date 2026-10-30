// Booker_DB: preset items, bands and monster groups, and their drop rewards, built from DBer rows.
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

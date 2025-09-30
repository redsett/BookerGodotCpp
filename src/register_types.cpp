#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include "bg_booker_data_base.hpp"
#include "bg_focus_layers.hpp"
#include "bg_hex_grid.hpp"
#include "bg_reactive.hpp"
#include "bg_animated_atlas_texture_rect.hpp"
// #include "bg_animation_mirror.hpp"

using namespace godot;

static BG_Booker_DB *_booker_db;
static BG_Focus_Layers *_focus_layers;

void gdextension_initialize(ModuleInitializationLevel p_level)
{
	// if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	// {
	// 	ClassDB::register_class<BG_AnimatedAtlasTextureRect>();
	// }
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<BG_Reactive>();
		ClassDB::register_class<BG_ReactiveObject>();
		ClassDB::register_class<BG_ReactiveInt>();
		ClassDB::register_class<BG_ReactiveString>();
		ClassDB::register_class<BG_ReactiveArray>();
		ClassDB::register_class<BG_AnimatedAtlasTextureRect>();
		ClassDB::register_class<BG_TwoDer_DataEntry>();
		ClassDB::register_class<BG_LocalizeEntryData>();
		ClassDB::register_class<BG_MailData>();
		ClassDB::register_class<BG_MarketplaceData>();
		ClassDB::register_class<BG_MarketplaceEntryData>();
		ClassDB::register_class<BG_MarketplaceSaveData>();
		ClassDB::register_class<BG_AudioData>();
		ClassDB::register_class<BG_HueShiftData>();
		ClassDB::register_class<BG_EffectRarityDetails>();
		ClassDB::register_class<BG_Effect>();
		ClassDB::register_class<BG_Dice>();
		ClassDB::register_class<BG_RarityDetails>();
		ClassDB::register_class<BG_LoreRarity>();
		ClassDB::register_class<BG_Item>();
		ClassDB::register_class<BG_ItemDetails>();
		ClassDB::register_class<BG_ItemSlotType>();
		ClassDB::register_class<BG_BandMember>();
		ClassDB::register_class<BG_Band>();
		ClassDB::register_class<BG_UnitStat>();
		ClassDB::register_class<BG_UnitStatDetails>();
		ClassDB::register_class<BG_Monster>();
		ClassDB::register_class<BG_UnitCaste>();
		ClassDB::register_class<BG_BandNameInfo>();
		ClassDB::register_class<BG_BandInfo>();
		ClassDB::register_class<BG_RewardItem>();
		ClassDB::register_class<BG_ItemDropPool>();
		ClassDB::register_class<BG_JobDistributionForAct>();
		ClassDB::register_class<BG_EventResourceDetails>();
		ClassDB::register_class<BG_JobMonsterDetails>();
		ClassDB::register_class<BG_JobDetails>();
		ClassDB::register_class<BG_ChallengeRatingGuide>();
		ClassDB::register_class<BG_ActStats>();
		ClassDB::register_class<BG_EquipmentAnimationDetails>();
		ClassDB::register_class<BG_BookerSkillTreeSlotDetails>();
		ClassDB::register_class<BG_CityInfo>();
		ClassDB::register_class<BG_TurretInfo>();
		ClassDB::register_class<BG_Booker_Globals>();
		ClassDB::register_class<BG_Booker_DB>();

		ClassDB::register_class<BG_Focus_Layer_Properties>();
		ClassDB::register_class<BG_Focus_Layers>();


		ClassDB::register_class<BG_HexVisualAssetData>();
		ClassDB::register_class<BG_HexVisualData>();
		ClassDB::register_class<BG_HexGameSaveData>();
		ClassDB::register_class<BG_BattleBoardGameSaveData>();
		ClassDB::register_class<BG_Hex>();
		ClassDB::register_class<BG_HexGrid>();

		// ClassDB::register_class<BG_AnimationMirrorDataMap>();
		// ClassDB::register_class<BG_AnimationMirrorData>();
		// ClassDB::register_class<BG_AnimationMirror>();

		_booker_db = memnew(BG_Booker_DB);
		_focus_layers = memnew(BG_Focus_Layers);
		Engine::get_singleton()->register_singleton("Booker_DB", BG_Booker_DB::get_singleton());
		Engine::get_singleton()->register_singleton("Focus_Layers", BG_Focus_Layers::get_singleton());
	}
}

void gdextension_terminate(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		Engine::get_singleton()->unregister_singleton("Booker_DB");
		memdelete(_booker_db);
		Engine::get_singleton()->unregister_singleton("Focus_Layers");
		memdelete(_focus_layers);
	}
}

extern "C" {
	// Initialization.
	GDExtensionBool GDE_EXPORT gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(gdextension_initialize);
		init_obj.register_terminator(gdextension_terminate);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
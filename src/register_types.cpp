#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

// #include "my_node.hpp"
// #include "card_to_mouse_spline.hpp"
// #include "my_singleton.hpp"

// #include "gsg_state.hpp"
// #include "gsg_state_machine.hpp"
#include "bg_booker_data_base.hpp"
#include "bg_focus_layers.hpp"
// #include "bg_animation_mirror.hpp"

using namespace godot;

static BG_Booker_DB *_booker_db;
static BG_Focus_Layers *_focus_layers;

void gdextension_initialize(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		// ClassDB::register_class<MyNode>();
		// ClassDB::register_class<CardToMouseSpline>();

		// ClassDB::register_class<GSGState>();
		// ClassDB::register_class<GSGStateMachine>();

		ClassDB::register_class<BG_Effect>();
		ClassDB::register_class<BG_Dice>();
		ClassDB::register_class<BG_RarityDetails>();
		ClassDB::register_class<BG_Item>();
		ClassDB::register_class<BG_ItemDetails>();
		ClassDB::register_class<BG_BandMember>();
		ClassDB::register_class<BG_Band>();
		ClassDB::register_class<BG_UnitStat>();
		ClassDB::register_class<BG_UnitStatDetails>();
		ClassDB::register_class<BG_Monster>();
		ClassDB::register_class<BG_UnitCaste>();
		ClassDB::register_class<BG_BandNameInfo>();
		ClassDB::register_class<BG_PersonalityDialgue>();
		ClassDB::register_class<BG_BandInfo>();
		ClassDB::register_class<BG_RewardItem>();
		ClassDB::register_class<BG_JobDetails>();
		ClassDB::register_class<BG_LevelGuide>();
		ClassDB::register_class<BG_ActStats>();
		ClassDB::register_class<BG_Booker_Globals>();
		ClassDB::register_class<BG_Booker_DB>();

		ClassDB::register_class<BG_Focus_Layer_Properties>();
		ClassDB::register_class<BG_Focus_Layers>();

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
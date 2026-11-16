#pragma once

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/editor_scene_post_import_plugin.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// A 3D scene import option ("booker/merge_skinned_meshes", off by default) that joins the mesh parts skinned to the
// same skeleton into one mesh, with one surface per material, so a character exported as many pieces draws in as
// few calls as it has materials. It runs before the importer builds LODs and shadow meshes, so those come from the
// merged mesh.
//
// A part stays separate when its node name ends in "_keep", it has blend shapes, it has child nodes, or its transform
// differs from the other parts'. Parts with their own skins get one shared skin, with bone indices remapped by bind
// name (or bone) and bind pose. Surfaces that aren't triangles or use custom vertex arrays keep their own surface in
// the merged mesh.
class BG_MergeSkinnedMeshesImport : public EditorScenePostImportPlugin
{
	GDCLASS(BG_MergeSkinnedMeshesImport, EditorScenePostImportPlugin);

protected:
	static void _bind_methods() {}

public:
	static constexpr const char *OPTION = "booker/merge_skinned_meshes";
	static constexpr const char *KEEP_SUFFIX = "_keep";

	void _get_import_options(const String &p_path) override;
	void _pre_process(Node *p_scene) override;
};

// Adds BG_MergeSkinnedMeshesImport to the editor's scene importer.
class BG_MergeSkinnedMeshesEditorPlugin : public EditorPlugin
{
	GDCLASS(BG_MergeSkinnedMeshesEditorPlugin, EditorPlugin);

	Ref<BG_MergeSkinnedMeshesImport> import_plugin;

protected:
	static void _bind_methods() {}

public:
	void _enter_tree() override;
	void _exit_tree() override;
};

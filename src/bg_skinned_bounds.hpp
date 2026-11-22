#pragma once

#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

// Exact bounds of meshes in the pose they're in right now. MeshInstance3D.get_aabb() is the mesh's bind pose box and
// never follows animation, so a skinned model's box can be far off (a snake built stretched out, coiled in its idle).
// This runs every vertex through the same skinning the renderer does and boxes the result:
//   point = space_inverse * mesh_global * sum(weight * bone_global_pose * bind_pose) * vertex
// The skeleton node's own transform isn't part of it (Godot skins relative to the mesh instance). A mesh with no Skin
// uses bind poses from the skeleton's rest (what Godot builds for it). Unskinned meshes box their vertices through
// the mesh instance's transform, which is tighter than transforming their AABB. Blend shapes are ignored.
//
// Vertex, bone and weight arrays are read from each mesh once and cached by mesh, since reading them back from the
// RenderingServer is the slow part; meshes shared by several instances share the entry. An ArrayMesh whose surfaces
// change is read again; call clear_cache after editing a mesh's vertices in place.
//
// A call costs one pass over the vertices (about 1.6 ms for a 16k vertex model in a debug build): fine for fitting
// a shape or framing a camera now and then, not for every model every frame. To cover a clip, call it on several
// frames and merge the boxes.
class BG_SkinnedBounds : public Object
{
	GDCLASS(BG_SkinnedBounds, Object);

	struct Surface
	{
		PackedVector3Array vertices;
		PackedInt32Array bones;
		PackedFloat32Array weights;
		int width = 0; // Bones per vertex (4 or 8), or 0 when the surface has no skinning data.
	};

	struct MeshData
	{
		Vector<Surface> surfaces;
		Vector<int> array_lens; // Each surface's vertex count when read, to notice a changed ArrayMesh.
	};

	static HashMap<uint64_t, MeshData> cache; // Mesh instance ID: its arrays.

	static const MeshData *get_mesh_data(const Ref<Mesh> &mesh);
	static void prune_cache();
	static bool merge_mesh_instance(MeshInstance3D *mesh_instance, const Transform3D &to_space, AABB &r_box, bool &r_has_box);

protected:
	static void _bind_methods();

public:
	// The mesh instance's box in its current pose, in `space`'s coordinates (world space when null). An empty AABB
	// when it has no mesh.
	static AABB get_mesh_instance_aabb(MeshInstance3D *mesh_instance, Node3D *space);
	// The box around every MeshInstance3D under `root` (and root itself), in `space`'s coordinates (world space when
	// null). Hidden meshes are skipped unless include_hidden. An empty AABB when there are none.
	static AABB get_aabb(Node *root, Node3D *space, bool include_hidden);
	static void clear_cache();
};

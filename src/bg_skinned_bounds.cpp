#include "bg_skinned_bounds.hpp"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/skin.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

HashMap<uint64_t, BG_SkinnedBounds::MeshData> BG_SkinnedBounds::cache;

namespace
{
// Entries past this count trigger a sweep for freed meshes.
constexpr int PRUNE_AT = 256;

void add_point(const Vector3 &point, AABB &r_box, bool &r_has_box)
{
	if (r_has_box)
		r_box.expand_to(point);
	else
	{
		r_box = AABB(point, Vector3());
		r_has_box = true;
	}
}

// Each surface's vertex count, or empty when the mesh isn't an ArrayMesh (whose arrays can't change under us).
Vector<int> get_array_lens(const Ref<Mesh> &mesh)
{
	Vector<int> lens;
	const Ref<ArrayMesh> array_mesh = mesh;
	if (array_mesh.is_valid())
		for (int s = 0; s < array_mesh->get_surface_count(); s++)
			lens.push_back(array_mesh->surface_get_array_len(s));
	return lens;
}
} // namespace

void BG_SkinnedBounds::_bind_methods()
{
	ClassDB::bind_static_method("BG_SkinnedBounds", D_METHOD("get_mesh_instance_aabb", "mesh_instance", "space"), &BG_SkinnedBounds::get_mesh_instance_aabb);
	ClassDB::bind_static_method("BG_SkinnedBounds", D_METHOD("get_aabb", "root", "space", "include_hidden"), &BG_SkinnedBounds::get_aabb, DEFVAL(false));
	ClassDB::bind_static_method("BG_SkinnedBounds", D_METHOD("clear_cache"), &BG_SkinnedBounds::clear_cache);
}

const BG_SkinnedBounds::MeshData *BG_SkinnedBounds::get_mesh_data(const Ref<Mesh> &mesh)
{
	const uint64_t id = mesh->get_instance_id();
	const Vector<int> lens = get_array_lens(mesh);
	MeshData *found = cache.getptr(id);
	if (found != nullptr && found->surfaces.size() == mesh->get_surface_count() && found->array_lens == lens)
		return found;

	if (found == nullptr && cache.size() >= PRUNE_AT)
		prune_cache();

	MeshData data;
	data.array_lens = lens;
	for (int s = 0; s < mesh->get_surface_count(); s++)
	{
		const Array arrays = mesh->surface_get_arrays(s);
		Surface surface;
		if (arrays.size() > Mesh::ARRAY_VERTEX && arrays[Mesh::ARRAY_VERTEX].get_type() == Variant::PACKED_VECTOR3_ARRAY)
			surface.vertices = arrays[Mesh::ARRAY_VERTEX];
		const int count = surface.vertices.size();
		if (count > 0 && arrays.size() > Mesh::ARRAY_WEIGHTS && arrays[Mesh::ARRAY_BONES].get_type() != Variant::NIL
				&& arrays[Mesh::ARRAY_WEIGHTS].get_type() != Variant::NIL)
		{
			surface.bones = arrays[Mesh::ARRAY_BONES];
			surface.weights = arrays[Mesh::ARRAY_WEIGHTS];
			const int width = surface.bones.size() / count;
			if ((width == 4 || width == 8) && surface.weights.size() == surface.bones.size())
				surface.width = width;
			else
			{
				surface.bones.clear();
				surface.weights.clear();
			}
		}
		data.surfaces.push_back(surface);
	}
	cache[id] = data;
	return cache.getptr(id);
}

void BG_SkinnedBounds::prune_cache()
{
	Vector<uint64_t> dead;
	for (const KeyValue<uint64_t, MeshData> &entry : cache)
		if (ObjectDB::get_instance(entry.key) == nullptr)
			dead.push_back(entry.key);
	for (const uint64_t id : dead)
		cache.erase(id);
}

void BG_SkinnedBounds::clear_cache()
{
	cache.clear();
}

bool BG_SkinnedBounds::merge_mesh_instance(MeshInstance3D *mesh_instance, const Transform3D &to_space, AABB &r_box, bool &r_has_box)
{
	if (mesh_instance == nullptr)
		return false;
	const Ref<Mesh> mesh = mesh_instance->get_mesh();
	if (mesh.is_null() || mesh->get_surface_count() == 0)
		return false;
	const MeshData *data = get_mesh_data(mesh);
	const Transform3D mesh_to_space = to_space * mesh_instance->get_global_transform();

	// Each bind's full transform, vertex to space. Binds whose bone can't be found keep the mesh's own transform.
	Skeleton3D *skeleton = nullptr;
	const NodePath skeleton_path = mesh_instance->get_skeleton_path();
	if (!skeleton_path.is_empty())
		skeleton = Object::cast_to<Skeleton3D>(mesh_instance->get_node_or_null(skeleton_path));
	Vector<Transform3D> binds;
	if (skeleton != nullptr)
	{
		const int bone_count = skeleton->get_bone_count();
		const Ref<Skin> skin = mesh_instance->get_skin();
		if (skin.is_valid())
		{
			for (int b = 0; b < skin->get_bind_count(); b++)
			{
				const StringName bind_name = skin->get_bind_name(b);
				const int bone = bind_name.is_empty() ? skin->get_bind_bone(b) : skeleton->find_bone(bind_name);
				binds.push_back(bone >= 0 && bone < bone_count
								? mesh_to_space * skeleton->get_bone_global_pose(bone) * skin->get_bind_pose(b)
								: mesh_to_space);
			}
		}
		else
		{
			for (int b = 0; b < bone_count; b++)
				binds.push_back(mesh_to_space * skeleton->get_bone_global_pose(b) * skeleton->get_bone_global_rest(b).affine_inverse());
		}
	}

	bool any = false;
	const Transform3D *bind_ptr = binds.ptr();
	const int bind_count = binds.size();
	for (const Surface &surface : data->surfaces)
	{
		const int count = surface.vertices.size();
		const Vector3 *vertices = surface.vertices.ptr();
		if (surface.width == 0 || bind_count == 0)
		{
			for (int v = 0; v < count; v++)
				add_point(mesh_to_space.xform(vertices[v]), r_box, r_has_box);
			any |= count > 0;
			continue;
		}

		// Blends the binds' matrices by weight, as the skinning shader does.
		const int width = surface.width;
		const int32_t *bones = surface.bones.ptr();
		const float *weights = surface.weights.ptr();
		for (int v = 0; v < count; v++)
		{
			Basis basis;
			basis.rows[0] = Vector3();
			basis.rows[1] = Vector3();
			basis.rows[2] = Vector3();
			Vector3 origin;
			float total = 0.0f;
			for (int j = 0; j < width; j++)
			{
				const float weight = weights[v * width + j];
				const int bone = bones[v * width + j];
				if (weight <= 0.0f || bone < 0 || bone >= bind_count)
					continue;
				const Transform3D &bind = bind_ptr[bone];
				basis.rows[0] += bind.basis.rows[0] * weight;
				basis.rows[1] += bind.basis.rows[1] * weight;
				basis.rows[2] += bind.basis.rows[2] * weight;
				origin += bind.origin * weight;
				total += weight;
			}
			if (total <= 0.0f)
				continue; // No influence: the shader collapses it to the origin; leave it out of the box.
			add_point(Transform3D(basis, origin).xform(vertices[v]), r_box, r_has_box);
			any = true;
		}
	}
	return any;
}

AABB BG_SkinnedBounds::get_mesh_instance_aabb(MeshInstance3D *mesh_instance, Node3D *space)
{
	const Transform3D to_space = space != nullptr ? space->get_global_transform().affine_inverse() : Transform3D();
	AABB box;
	bool has_box = false;
	merge_mesh_instance(mesh_instance, to_space, box, has_box);
	return box;
}

AABB BG_SkinnedBounds::get_aabb(Node *root, Node3D *space, bool include_hidden)
{
	if (root == nullptr)
		return AABB();
	const Transform3D to_space = space != nullptr ? space->get_global_transform().affine_inverse() : Transform3D();
	AABB box;
	bool has_box = false;
	TypedArray<Node> nodes = root->find_children("*", "MeshInstance3D", true, false);
	nodes.push_front(root);
	for (int i = 0; i < nodes.size(); i++)
	{
		MeshInstance3D *mesh_instance = Object::cast_to<MeshInstance3D>(nodes[i]);
		if (mesh_instance == nullptr || (!include_hidden && !mesh_instance->is_visible_in_tree()))
			continue;
		merge_mesh_instance(mesh_instance, to_space, box, has_box);
	}
	return box;
}

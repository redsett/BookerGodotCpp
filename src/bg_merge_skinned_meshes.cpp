#include "bg_merge_skinned_meshes.hpp"

#include <godot_cpp/classes/importer_mesh.hpp>
#include <godot_cpp/classes/importer_mesh_instance3d.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/skin.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

namespace
{
// Format bits that describe custom arrays, kept when a surface is copied as is.
constexpr uint64_t CUSTOM_FORMAT_MASK = ((uint64_t(1) << (Mesh::ARRAY_FORMAT_CUSTOM_BITS * 4)) - 1) << Mesh::ARRAY_FORMAT_CUSTOM_BASE;

struct Part
{
	ImporterMeshInstance3D *node = nullptr;
	Ref<ImporterMesh> mesh;
	Vector<int> bone_remap; // The part's bind index -> the merged skin's bind index. Empty when there is no skin.
};

// One surface of one part.
struct Piece
{
	const Part *part = nullptr;
	Array arrays;
	uint64_t format = 0;
	Mesh::PrimitiveType primitive = Mesh::PRIMITIVE_TRIANGLES;
};

struct Group
{
	Node *parent = nullptr;
	Skeleton3D *skeleton = nullptr;
	bool skinned = false;
	Transform3D transform;
	Vector<Part> parts;
};

bool has_array(const Array &arrays, int type)
{
	return arrays.size() > type && arrays[type].get_type() != Variant::NIL;
}

int vertex_count(const Array &arrays)
{
	return has_array(arrays, Mesh::ARRAY_VERTEX) ? PackedVector3Array(arrays[Mesh::ARRAY_VERTEX]).size() : 0;
}

int bones_per_vertex(const Array &arrays)
{
	const int count = vertex_count(arrays);
	if (count == 0 || !has_array(arrays, Mesh::ARRAY_BONES))
		return 0;
	return PackedInt32Array(arrays[Mesh::ARRAY_BONES]).size() / count;
}

bool has_custom_arrays(const Array &arrays)
{
	for (int i = Mesh::ARRAY_CUSTOM0; i <= Mesh::ARRAY_CUSTOM3; i++)
		if (has_array(arrays, i))
			return true;
	return false;
}

// Copies a piece's bones and weights into `bones`/`weights` at `width` per vertex, through the part's remap.
void append_bones(const Piece &piece, int width, PackedInt32Array &bones, PackedFloat32Array &weights)
{
	const int count = vertex_count(piece.arrays);
	const int piece_width = bones_per_vertex(piece.arrays);
	const PackedInt32Array src_bones = piece_width > 0 ? PackedInt32Array(piece.arrays[Mesh::ARRAY_BONES]) : PackedInt32Array();
	const PackedFloat32Array src_weights = piece_width > 0 ? PackedFloat32Array(piece.arrays[Mesh::ARRAY_WEIGHTS]) : PackedFloat32Array();
	const Vector<int> &remap = piece.part->bone_remap;
	for (int v = 0; v < count; v++)
	{
		for (int j = 0; j < width; j++)
		{
			int bone = 0;
			float weight = 0.0f;
			if (j < piece_width)
			{
				bone = src_bones[v * piece_width + j];
				weight = src_weights[v * piece_width + j];
				if (!remap.is_empty())
					bone = (bone >= 0 && bone < remap.size()) ? remap[bone] : 0;
			}
			bones.push_back(bone);
			weights.push_back(weight);
		}
	}
}

// Joins triangle pieces that share a material into one surface's arrays. Arrays only some pieces have are filled
// with defaults for the others.
Array join_pieces(const Vector<Piece> &pieces, int &r_bone_width)
{
	bool any_normal = false, any_tangent = false, any_color = false, any_uv = false, any_uv2 = false;
	r_bone_width = 0;
	for (const Piece &p : pieces)
	{
		any_normal |= has_array(p.arrays, Mesh::ARRAY_NORMAL);
		any_tangent |= has_array(p.arrays, Mesh::ARRAY_TANGENT);
		any_color |= has_array(p.arrays, Mesh::ARRAY_COLOR);
		any_uv |= has_array(p.arrays, Mesh::ARRAY_TEX_UV);
		any_uv2 |= has_array(p.arrays, Mesh::ARRAY_TEX_UV2);
		r_bone_width = MAX(r_bone_width, bones_per_vertex(p.arrays));
	}
	if (r_bone_width > 0)
		r_bone_width = r_bone_width > 4 ? 8 : 4;

	PackedVector3Array vertices, normals;
	PackedFloat32Array tangents, weights;
	PackedColorArray colors;
	PackedVector2Array uvs, uv2s;
	PackedInt32Array bones, indices;

	for (const Piece &p : pieces)
	{
		const int base = vertices.size();
		const int count = vertex_count(p.arrays);
		vertices.append_array(PackedVector3Array(p.arrays[Mesh::ARRAY_VERTEX]));

		if (any_normal)
		{
			if (has_array(p.arrays, Mesh::ARRAY_NORMAL))
				normals.append_array(PackedVector3Array(p.arrays[Mesh::ARRAY_NORMAL]));
			else
				for (int v = 0; v < count; v++)
					normals.push_back(Vector3(0, 1, 0));
		}
		if (any_tangent)
		{
			if (has_array(p.arrays, Mesh::ARRAY_TANGENT))
				tangents.append_array(PackedFloat32Array(p.arrays[Mesh::ARRAY_TANGENT]));
			else
				for (int v = 0; v < count; v++)
				{
					tangents.push_back(1.0f);
					tangents.push_back(0.0f);
					tangents.push_back(0.0f);
					tangents.push_back(1.0f);
				}
		}
		if (any_color)
		{
			if (has_array(p.arrays, Mesh::ARRAY_COLOR))
				colors.append_array(PackedColorArray(p.arrays[Mesh::ARRAY_COLOR]));
			else
				for (int v = 0; v < count; v++)
					colors.push_back(Color(1, 1, 1, 1));
		}
		if (any_uv)
		{
			if (has_array(p.arrays, Mesh::ARRAY_TEX_UV))
				uvs.append_array(PackedVector2Array(p.arrays[Mesh::ARRAY_TEX_UV]));
			else
				for (int v = 0; v < count; v++)
					uvs.push_back(Vector2());
		}
		if (any_uv2)
		{
			if (has_array(p.arrays, Mesh::ARRAY_TEX_UV2))
				uv2s.append_array(PackedVector2Array(p.arrays[Mesh::ARRAY_TEX_UV2]));
			else
				for (int v = 0; v < count; v++)
					uv2s.push_back(Vector2());
		}
		if (r_bone_width > 0)
			append_bones(p, r_bone_width, bones, weights);

		if (has_array(p.arrays, Mesh::ARRAY_INDEX))
		{
			const PackedInt32Array src = p.arrays[Mesh::ARRAY_INDEX];
			for (int i = 0; i < src.size(); i++)
				indices.push_back(src[i] + base);
		}
		else
			for (int v = 0; v < count; v++)
				indices.push_back(base + v);
	}

	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	arrays[Mesh::ARRAY_VERTEX] = vertices;
	if (any_normal)
		arrays[Mesh::ARRAY_NORMAL] = normals;
	if (any_tangent)
		arrays[Mesh::ARRAY_TANGENT] = tangents;
	if (any_color)
		arrays[Mesh::ARRAY_COLOR] = colors;
	if (any_uv)
		arrays[Mesh::ARRAY_TEX_UV] = uvs;
	if (any_uv2)
		arrays[Mesh::ARRAY_TEX_UV2] = uv2s;
	if (r_bone_width > 0)
	{
		arrays[Mesh::ARRAY_BONES] = bones;
		arrays[Mesh::ARRAY_WEIGHTS] = weights;
	}
	arrays[Mesh::ARRAY_INDEX] = indices;
	return arrays;
}

// A copy of a piece's arrays with its bone indices remapped, for surfaces kept as they are.
Array remapped_copy(const Piece &piece)
{
	Array arrays = piece.arrays.duplicate();
	const int width = bones_per_vertex(arrays);
	if (width > 0 && !piece.part->bone_remap.is_empty())
	{
		PackedInt32Array bones;
		PackedFloat32Array weights;
		append_bones(piece, width, bones, weights);
		arrays[Mesh::ARRAY_BONES] = bones;
	}
	return arrays;
}

// Builds one skin from all parts' skins and fills each part's bone remap. Binds match by name (or bone, for unnamed
// binds) and pose; a bind with the same bone but another pose gets its own entry.
Ref<Skin> merge_skins(Vector<Part> &parts)
{
	Ref<Skin> merged;
	merged.instantiate();
	for (Part &part : parts)
	{
		Ref<Skin> skin = part.node->get_skin();
		part.bone_remap.clear();
		for (int b = 0; b < skin->get_bind_count(); b++)
		{
			const StringName bind_name = skin->get_bind_name(b);
			const int bind_bone = skin->get_bind_bone(b);
			const Transform3D pose = skin->get_bind_pose(b);
			int found = -1;
			for (int m = 0; m < merged->get_bind_count() && found < 0; m++)
			{
				const bool same_bone = bind_name.is_empty() ? merged->get_bind_bone(m) == bind_bone : merged->get_bind_name(m) == bind_name;
				if (same_bone && merged->get_bind_pose(m).is_equal_approx(pose))
					found = m;
			}
			if (found < 0)
			{
				found = merged->get_bind_count();
				if (bind_name.is_empty())
					merged->add_bind(bind_bone, pose);
				else
				{
					merged->add_named_bind(bind_name, pose);
					merged->set_bind_bone(found, bind_bone);
				}
			}
			part.bone_remap.push_back(found);
		}
	}
	return merged;
}

bool can_merge(ImporterMeshInstance3D *node)
{
	Ref<ImporterMesh> mesh = node->get_mesh();
	if (mesh.is_null() || mesh->get_surface_count() == 0)
		return false;
	if (String(node->get_name()).ends_with(BG_MergeSkinnedMeshesImport::KEEP_SUFFIX))
		return false;
	if (mesh->get_blend_shape_count() > 0 || node->get_child_count() > 0)
		return false;
	return true;
}

void merge_group(Group &group, Node *scene)
{
	Ref<Skin> skin = group.skinned ? merge_skins(group.parts) : Ref<Skin>();

	// Triangle surfaces bucket by material, in the order materials first appear. Others stay as their own surface.
	Vector<Ref<Material>> bucket_materials;
	Vector<String> bucket_names;
	Vector<Vector<Piece>> buckets;
	Vector<Piece> kept;
	Vector<Ref<Material>> kept_materials;
	Vector<String> kept_names;
	for (const Part &part : group.parts)
	{
		for (int s = 0; s < part.mesh->get_surface_count(); s++)
		{
			Piece piece;
			piece.part = &part;
			piece.arrays = part.mesh->get_surface_arrays(s);
			piece.format = part.mesh->get_surface_format(s);
			piece.primitive = part.mesh->get_surface_primitive_type(s);
			const Ref<Material> material = part.mesh->get_surface_material(s);
			if (piece.primitive != Mesh::PRIMITIVE_TRIANGLES || has_custom_arrays(piece.arrays))
			{
				kept.push_back(piece);
				kept_materials.push_back(material);
				kept_names.push_back(part.mesh->get_surface_name(s));
				continue;
			}
			int bucket = bucket_materials.find(material);
			if (bucket < 0)
			{
				bucket = buckets.size();
				bucket_materials.push_back(material);
				bucket_names.push_back(material.is_valid() && !material->get_name().is_empty() ? material->get_name() : part.mesh->get_surface_name(s));
				buckets.push_back(Vector<Piece>());
			}
			buckets.write[bucket].push_back(piece);
		}
	}

	Ref<ImporterMesh> merged_mesh;
	merged_mesh.instantiate();
	merged_mesh->set_name(String(group.skeleton->get_name()) + "_merged");
	for (int b = 0; b < buckets.size(); b++)
	{
		int bone_width = 0;
		const Array arrays = join_pieces(buckets[b], bone_width);
		const uint64_t flags = bone_width == 8 ? uint64_t(Mesh::ARRAY_FLAG_USE_8_BONE_WEIGHTS) : 0;
		merged_mesh->add_surface(Mesh::PRIMITIVE_TRIANGLES, arrays, TypedArray<Array>(), Dictionary(), bucket_materials[b], bucket_names[b], flags);
	}
	for (int k = 0; k < kept.size(); k++)
	{
		const Piece &piece = kept[k];
		const uint64_t flags = piece.format & (CUSTOM_FORMAT_MASK | uint64_t(Mesh::ARRAY_FLAG_USE_8_BONE_WEIGHTS));
		merged_mesh->add_surface(piece.primitive, remapped_copy(piece), TypedArray<Array>(), Dictionary(), kept_materials[k], kept_names[k], flags);
	}

	ImporterMeshInstance3D *first = group.parts[0].node;
	ImporterMeshInstance3D *merged = memnew(ImporterMeshInstance3D);
	merged->set_name(merged_mesh->get_name());
	merged->set_mesh(merged_mesh);
	merged->set_skin(skin);
	merged->set_transform(group.transform);
	merged->set_layer_mask(first->get_layer_mask());
	merged->set_cast_shadows_setting(first->get_cast_shadows_setting());
	const int index = first->get_index();
	group.parent->add_child(merged, true);
	group.parent->move_child(merged, index);
	merged->set_owner(scene);
	merged->set_skeleton_path(merged->get_path_to(group.skeleton));

	for (const Part &part : group.parts)
	{
		group.parent->remove_child(part.node);
		memdelete(part.node);
	}
}
} // namespace

void BG_MergeSkinnedMeshesImport::_get_import_options(const String &p_path)
{
	add_import_option_advanced(Variant::BOOL, OPTION, false);
}

void BG_MergeSkinnedMeshesImport::_pre_process(Node *p_scene)
{
	if (!bool(get_option_value(OPTION)))
		return;

	Vector<Group> groups;
	TypedArray<Node> nodes = p_scene->find_children("*", "ImporterMeshInstance3D", true, false);
	for (int i = 0; i < nodes.size(); i++)
	{
		ImporterMeshInstance3D *node = Object::cast_to<ImporterMeshInstance3D>(nodes[i]);
		if (node == nullptr || !can_merge(node) || node->get_skeleton_path().is_empty())
			continue;
		Skeleton3D *skeleton = Object::cast_to<Skeleton3D>(node->get_node_or_null(node->get_skeleton_path()));
		if (skeleton == nullptr)
			continue;
		const bool skinned = node->get_skin().is_valid();

		int g = -1;
		for (int j = 0; j < groups.size() && g < 0; j++)
			if (groups[j].skeleton == skeleton && groups[j].parent == node->get_parent() && groups[j].skinned == skinned
					&& groups[j].transform.is_equal_approx(node->get_transform()))
				g = j;
		if (g < 0)
		{
			Group group;
			group.parent = node->get_parent();
			group.skeleton = skeleton;
			group.skinned = skinned;
			group.transform = node->get_transform();
			g = groups.size();
			groups.push_back(group);
		}
		Part part;
		part.node = node;
		part.mesh = node->get_mesh();
		groups.write[g].parts.push_back(part);
	}

	for (int g = 0; g < groups.size(); g++)
	{
		if (groups[g].parts.size() < 2)
			continue;
		const int part_count = groups[g].parts.size();
		merge_group(groups.write[g], p_scene);
		UtilityFunctions::print("Merged ", part_count, " skinned meshes on ", groups[g].skeleton->get_name(), ".");
	}
}

void BG_MergeSkinnedMeshesEditorPlugin::_enter_tree()
{
	import_plugin.instantiate();
	add_scene_post_import_plugin(import_plugin);
}

void BG_MergeSkinnedMeshesEditorPlugin::_exit_tree()
{
	if (import_plugin.is_valid())
		remove_scene_post_import_plugin(import_plugin);
	import_plugin.unref();
}

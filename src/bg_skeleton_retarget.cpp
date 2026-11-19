#include "bg_skeleton_retarget.hpp"

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

static constexpr float MIN_DIR_LENGTH_SQ = 0.0000001f;

void BG_SkeletonRetarget::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("refresh"), &BG_SkeletonRetarget::refresh);

	ClassDB::bind_method(D_METHOD("get_source_skeleton"), &BG_SkeletonRetarget::get_source_skeleton);
	ClassDB::bind_method(D_METHOD("set_source_skeleton", "value"), &BG_SkeletonRetarget::set_source_skeleton);
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "source_skeleton", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Skeleton3D"), "set_source_skeleton", "get_source_skeleton");

	ClassDB::bind_method(D_METHOD("get_bone_map"), &BG_SkeletonRetarget::get_bone_map);
	ClassDB::bind_method(D_METHOD("set_bone_map", "value"), &BG_SkeletonRetarget::set_bone_map);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "bone_map"), "set_bone_map", "get_bone_map");

	ClassDB::bind_method(D_METHOD("get_hip_bone"), &BG_SkeletonRetarget::get_hip_bone);
	ClassDB::bind_method(D_METHOD("set_hip_bone", "value"), &BG_SkeletonRetarget::set_hip_bone);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "hip_bone"), "set_hip_bone", "get_hip_bone");

	ClassDB::bind_method(D_METHOD("get_align_bones"), &BG_SkeletonRetarget::get_align_bones);
	ClassDB::bind_method(D_METHOD("set_align_bones", "value"), &BG_SkeletonRetarget::set_align_bones);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "align_bones"), "set_align_bones", "get_align_bones");

	ClassDB::bind_method(D_METHOD("get_absolute_bones"), &BG_SkeletonRetarget::get_absolute_bones);
	ClassDB::bind_method(D_METHOD("set_absolute_bones", "value"), &BG_SkeletonRetarget::set_absolute_bones);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "absolute_bones"), "set_absolute_bones", "get_absolute_bones");

	ClassDB::bind_method(D_METHOD("get_target_axes"), &BG_SkeletonRetarget::get_target_axes);
	ClassDB::bind_method(D_METHOD("set_target_axes", "value"), &BG_SkeletonRetarget::set_target_axes);
	ADD_PROPERTY(PropertyInfo(Variant::BASIS, "target_axes"), "set_target_axes", "get_target_axes");

	ClassDB::bind_method(D_METHOD("get_position_scale"), &BG_SkeletonRetarget::get_position_scale);
	ClassDB::bind_method(D_METHOD("set_position_scale", "value"), &BG_SkeletonRetarget::set_position_scale);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "position_scale", PROPERTY_HINT_RANGE, "0,10,0.001,or_greater"), "set_position_scale", "get_position_scale");
}

void BG_SkeletonRetarget::refresh()
{
	dirty = true;
}

Skeleton3D *BG_SkeletonRetarget::_get_source() const
{
	if (source_skeleton.is_empty())
		return nullptr;
	return Object::cast_to<Skeleton3D>(get_node_or_null(source_skeleton));
}

bool BG_SkeletonRetarget::_ensure_cache()
{
	Skeleton3D *target = get_skeleton();
	Skeleton3D *src = _get_source();
	if (target == nullptr || src == nullptr)
		return false;

	if (dirty || cached_target != target->get_instance_id() || cached_source != src->get_instance_id()
			|| cached_target_bone_count != target->get_bone_count() || cached_source_bone_count != src->get_bone_count())
		_build_cache(target, src);
	return !entries.is_empty();
}

void BG_SkeletonRetarget::_build_cache(Skeleton3D *target, Skeleton3D *src)
{
	dirty = false;
	cached_target = target->get_instance_id();
	cached_source = src->get_instance_id();
	cached_target_bone_count = target->get_bone_count();
	cached_source_bone_count = src->get_bone_count();

	const int bone_count = target->get_bone_count();
	entries.clear();
	entry_of_bone.resize(bone_count);
	entry_of_bone.fill(-1);
	globals.resize(bone_count);
	source_globals.resize(src->get_bone_count());
	to_target = Transform3D(Basis(target_axes.get_rotation_quaternion().inverse()), Vector3());

	// Parents before children, so each bone's parent pose is final when the bone is solved.
	bone_order.clear();
	PackedInt32Array roots = target->get_parentless_bones();
	for (int i = 0; i < roots.size(); i++)
		bone_order.push_back(roots[i]);
	for (int i = 0; i < bone_order.size(); i++)
	{
		PackedInt32Array children = target->get_bone_children(bone_order[i]);
		for (int c = 0; c < children.size(); c++)
			bone_order.push_back(children[c]);
	}

	Array keys = bone_map.keys();
	for (int i = 0; i < keys.size(); i++)
	{
		const String target_name = keys[i];
		const String source_name = bone_map[keys[i]];
		Entry e;
		e.target = target->find_bone(target_name);
		e.source = src->find_bone(source_name);
		if (e.target < 0 || e.source < 0)
		{
			UtilityFunctions::push_warning("BG_SkeletonRetarget: no bone pair '", target_name, "' -> '", source_name, "'.");
			continue;
		}
		e.source_parent = src->get_bone_parent(e.source);
		const Quaternion source_rest = (to_target * src->get_bone_global_rest(e.source)).basis.get_rotation_quaternion();
		const Quaternion target_rest = target->get_bone_global_rest(e.target).basis.get_rotation_quaternion();
		e.rest_delta = (source_rest.inverse() * target_rest).normalized();
		e.is_hip = StringName(target_name) == hip_bone;
		e.is_absolute = absolute_bones.has(target_name);
		entry_of_bone.set(e.target, entries.size());
		entries.push_back(e);
	}

	// Aligned bones aim at their first mapped direct child.
	for (int i = 0; i < entries.size(); i++)
	{
		Entry &e = entries.write[i];
		if (!align_bones.has(target->get_bone_name(e.target)))
			continue;
		PackedInt32Array children = target->get_bone_children(e.target);
		for (int c = 0; c < children.size(); c++)
		{
			const int child_entry = entry_of_bone[children[c]];
			if (child_entry < 0)
				continue;
			const Transform3D rest = target->get_bone_global_rest(e.target);
			const Vector3 to_child = target->get_bone_global_rest(children[c]).origin - rest.origin;
			e.align_local_dir = Basis(rest.basis.get_rotation_quaternion()).inverse().xform(to_child);
			e.align_source_child = entries[child_entry].source;
			e.is_aligned = e.align_local_dir.length_squared() > MIN_DIR_LENGTH_SQ;
			break;
		}
	}

	auto_position_scale = 1.0f;
	const int hip_target = hip_bone.is_empty() ? -1 : target->find_bone(hip_bone);
	const int hip_entry = hip_target < 0 ? -1 : entry_of_bone[hip_target];
	if (hip_entry >= 0)
	{
		hip_target_rest = target->get_bone_global_rest(hip_target);
		hip_source_rest = to_target * src->get_bone_global_rest(entries[hip_entry].source);
		// Hip heights along the target's up axis (both rests are in target skeleton space here).
		const Vector3 up = to_target.basis.xform(Vector3(0, 1, 0));
		const float source_height = hip_source_rest.origin.dot(up);
		if (Math::abs(source_height) > 0.0001f)
			auto_position_scale = hip_target_rest.origin.dot(up) / source_height;
	}
}

void BG_SkeletonRetarget::_process_modification_with_delta(double p_delta)
{
	if (!_ensure_cache())
		return;
	Skeleton3D *target = get_skeleton();
	Skeleton3D *src = _get_source();
	const float pos_scale = position_scale > 0.0f ? position_scale : auto_position_scale;

	// Read the source once, turned into target skeleton space; the first global pose read brings its dirty bones up
	// to date.
	for (int i = 0; i < entries.size(); i++)
	{
		const Entry &e = entries[i];
		source_globals.set(e.source, to_target * src->get_bone_global_pose(e.source));
		if (e.is_absolute && e.source_parent >= 0)
			source_globals.set(e.source_parent, to_target * src->get_bone_global_pose(e.source_parent));
		if (e.is_aligned)
			source_globals.set(e.align_source_child, to_target * src->get_bone_global_pose(e.align_source_child));
	}

	for (int i = 0; i < bone_order.size(); i++)
	{
		const int b = bone_order[i];
		const int p = target->get_bone_parent(b);
		const Transform3D parent_global = p >= 0 ? globals[p] : Transform3D();
		Transform3D local = target->get_bone_pose(b);
		const int ei = entry_of_bone[b];
		if (ei < 0)
		{
			globals.set(b, parent_global * local);
			continue;
		}

		const Entry &e = entries[ei];
		const Transform3D &sg = source_globals[e.source];
		const Quaternion source_rot = sg.basis.get_rotation_quaternion();
		Quaternion rot = e.is_absolute ? source_rot : (source_rot * e.rest_delta).normalized();

		if (e.is_aligned)
		{
			const Vector3 want = source_globals[e.align_source_child].origin - sg.origin;
			const Vector3 have = Basis(rot).xform(e.align_local_dir);
			if (want.length_squared() > MIN_DIR_LENGTH_SQ && have.length_squared() > MIN_DIR_LENGTH_SQ)
				rot = (Quaternion(have.normalized(), want.normalized()) * rot).normalized();
		}

		const Quaternion local_rot = (parent_global.basis.get_rotation_quaternion().inverse() * rot).normalized();
		target->set_bone_pose_rotation(b, local_rot);

		Vector3 local_pos = local.origin;
		bool moves = false;
		if (e.is_hip)
		{
			const Vector3 global_pos = hip_target_rest.origin + (sg.origin - hip_source_rest.origin) * pos_scale;
			local_pos = parent_global.affine_inverse().xform(global_pos);
			moves = true;
		}
		else if (e.is_absolute && e.source_parent >= 0 && p >= 0)
		{
			const Vector3 offset = (sg.origin - source_globals[e.source_parent].origin) * pos_scale;
			local_pos = parent_global.affine_inverse().xform(parent_global.origin + offset);
			moves = true;
		}
		if (moves)
			target->set_bone_pose_position(b, local_pos);

		local = Transform3D(Basis(local_rot, local.basis.get_scale()), local_pos);
		globals.set(b, parent_global * local);
	}
}

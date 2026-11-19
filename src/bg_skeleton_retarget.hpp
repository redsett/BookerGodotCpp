#pragma once

#include <godot_cpp/classes/skeleton_modifier3d.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

// Poses its skeleton from another skeleton with different bone names, axes and proportions, so one rig's clips can
// play on another (e.g. Brute clips on Skyfell). The source skeleton is usually a hidden copy of the clip's own rig
// that an AnimationPlayer drives; this node is a child of the target skeleton.
//
// Each mapped bone gets the source bone's rotation away from its rest pose, applied in skeleton space:
//   target_global = source_global_pose * inverse(source_global_rest) * target_global_rest
// so local axis conventions (Unreal vs Maya, mirrored right sides) don't matter. The source skeleton's own space
// must be Y up and face +Z, which is how Godot imports most humanoids. A target skeleton whose space differs (a
// Z-up rig whose axis turn sits on the Armature node rather than its root bone) sets `target_axes`; source poses
// are turned into the target's space before all of the above, so the rest of the solve is unchanged.
// - `hip_bone` also follows the source's position, scaled by the two rigs' hip heights (or `position_scale`).
// - `align_bones` turn afterwards so they point at their mapped child the way the source bone does. Limbs use this
//   so a difference in rest pose (arms lower in one rig's A-pose) doesn't carry into every frame.
// - `absolute_bones` copy the source bone's skeleton-space rotation outright, and its offset from its parent bone
//   (scaled). Prop bones use this so weapons sit in the hand the way they were authored on the source rig.
// Unmapped target bones keep their own pose and follow their parents.
class BG_SkeletonRetarget : public SkeletonModifier3D
{
	GDCLASS(BG_SkeletonRetarget, SkeletonModifier3D);

	struct Entry
	{
		int target = -1;
		int source = -1;
		int source_parent = -1;
		Quaternion rest_delta; // inverse(source global rest rotation) * target global rest rotation.
		bool is_hip = false;
		bool is_absolute = false;
		bool is_aligned = false;
		int align_source_child = -1; // Source bone the aligned bone aims at.
		Vector3 align_local_dir; // Toward the mapped child, in the target bone's own rest space.
	};

	Vector<Entry> entries;
	Vector<int> entry_of_bone; // Target bone index -> index into `entries`, or -1.
	Vector<int> bone_order; // Target bones, parents before children.
	Vector<Transform3D> globals; // Scratch: target skeleton-space poses this frame.
	Vector<Transform3D> source_globals; // Scratch: source skeleton-space poses this frame.
	Transform3D to_target; // Rotation only: source skeleton space -> target skeleton space (inverse of target_axes).
	Transform3D hip_target_rest;
	Transform3D hip_source_rest;
	float auto_position_scale = 1.0f;
	uint64_t cached_target = 0; // Instance IDs of the skeletons the cache was built for.
	uint64_t cached_source = 0;
	int cached_target_bone_count = -1;
	int cached_source_bone_count = -1;
	bool dirty = true;

	Skeleton3D *_get_source() const;
	bool _ensure_cache();
	void _build_cache(Skeleton3D *target, Skeleton3D *src);

protected:
	static void _bind_methods();

public:
	void _process_modification_with_delta(double p_delta) override;

	// Marks the bone lookups stale; they rebuild on the next frame.
	void refresh();

	NodePath source_skeleton;
	NodePath get_source_skeleton() const { return source_skeleton; }
	void set_source_skeleton(const NodePath &value) { source_skeleton = value; dirty = true; }

	// Target bone name -> source bone name.
	Dictionary bone_map;
	Dictionary get_bone_map() const { return bone_map; }
	void set_bone_map(const Dictionary &value) { bone_map = value; dirty = true; }

	StringName hip_bone;
	StringName get_hip_bone() const { return hip_bone; }
	void set_hip_bone(const StringName &value) { hip_bone = value; dirty = true; }

	PackedStringArray align_bones;
	PackedStringArray get_align_bones() const { return align_bones; }
	void set_align_bones(const PackedStringArray &value) { align_bones = value; dirty = true; }

	PackedStringArray absolute_bones;
	PackedStringArray get_absolute_bones() const { return absolute_bones; }
	void set_absolute_bones(const PackedStringArray &value) { absolute_bones = value; dirty = true; }

	// Rotation from the target skeleton's own space to Y up, facing +Z. Identity when the target already is; for a
	// Z-up rig this is the rotation its Armature node carries. Only its rotation is used.
	Basis target_axes;
	Basis get_target_axes() const { return target_axes; }
	void set_target_axes(const Basis &value) { target_axes = value; dirty = true; }

	// Source distance -> target distance for positions. 0 works it out from the hip bones' rest heights.
	float position_scale = 0.0f;
	float get_position_scale() const { return position_scale; }
	void set_position_scale(float value) { position_scale = value; dirty = true; }
};

#pragma once

#include <godot_cpp/classes/animation_node_sync.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BG_AnimationMirrorDataMap : public Object
{
	GDCLASS(BG_AnimationMirrorDataMap, Object);

protected:
	static void _bind_methods();

	StringName left_side_bone_name;
	StringName get_left_side_bone_name() const { return left_side_bone_name; }
	void set_left_side_bone_name(StringName p_left_side_bone_name) { left_side_bone_name = p_left_side_bone_name; }

	StringName right_side_bone_name;
	StringName get_right_side_bone_name() const { return right_side_bone_name; }
	void set_right_side_bone_name(StringName p_right_side_bone_name) { right_side_bone_name = p_right_side_bone_name; }

	Vector3 mirror_multiplier = Vector3(-1.0f, 1.0f, -1.0f);
	Vector3 get_mirror_multiplier() const { return mirror_multiplier; }
	void set_mirror_multiplier(Vector3 p_mirror_multiplier) { mirror_multiplier = p_mirror_multiplier; }
};

class BG_AnimationMirrorData : public Resource
{
	GDCLASS(BG_AnimationMirrorData, Resource);

protected:
	static void _bind_methods();

	TypedArray<BG_AnimationMirrorDataMap> bone_maps;
	TypedArray<BG_AnimationMirrorDataMap> get_bone_maps() const { return bone_maps; }
	void set_bone_maps(TypedArray<BG_AnimationMirrorDataMap> p_bone_maps) { bone_maps = p_bone_maps; }
};

class BG_AnimationMirror : public AnimationNodeSync
{
	GDCLASS(BG_AnimationMirror, AnimationNodeSync);

protected:
	static void _bind_methods();

public:
	BG_AnimationMirror();
	// ~BG_AnimationMirror();

	StringName blend_amount = "blend_amount";

	BG_AnimationMirrorData *mirror_data = nullptr;
	BG_AnimationMirrorData *get_mirror_data() const { return mirror_data; }
	void set_mirror_data(BG_AnimationMirrorData *p_mirror_data) { mirror_data = p_mirror_data; }

	Array _get_parameter_list() const override;

	virtual String _get_caption() const override;

	// virtual double _process(double time, bool seek, bool is_external_seeking, bool test_only) override;
};

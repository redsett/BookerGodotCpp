#include "bg_animation_mirror.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void BG_AnimationMirrorDataMap::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_left_side_bone_name", "left_side_bone_name"), &BG_AnimationMirrorDataMap::set_left_side_bone_name);
	ClassDB::bind_method(D_METHOD("get_left_side_bone_name"), &BG_AnimationMirrorDataMap::get_left_side_bone_name);
	ClassDB::bind_method(D_METHOD("set_right_side_bone_name", "right_side_bone_name"), &BG_AnimationMirrorDataMap::set_right_side_bone_name);
	ClassDB::bind_method(D_METHOD("get_right_side_bone_name"), &BG_AnimationMirrorDataMap::get_right_side_bone_name);
	ClassDB::bind_method(D_METHOD("set_mirror_multiplier", "mirror_multiplier"), &BG_AnimationMirrorDataMap::set_mirror_multiplier);
	ClassDB::bind_method(D_METHOD("get_mirror_multiplier"), &BG_AnimationMirrorDataMap::get_mirror_multiplier);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "left_side_bone_name"), "set_left_side_bone_name", "get_left_side_bone_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "right_side_bone_name"), "set_right_side_bone_name", "get_right_side_bone_name");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "mirror_multiplier"), "set_mirror_multiplier", "get_mirror_multiplier");
}

#define BG_MAKE_RESOURCE_TYPE_HINT(m_type) vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, m_type)

void BG_AnimationMirrorData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_bone_maps", "bone_maps"), &BG_AnimationMirrorData::set_bone_maps);
	ClassDB::bind_method(D_METHOD("get_bone_maps"), &BG_AnimationMirrorData::get_bone_maps);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "bone_maps", PROPERTY_HINT_ARRAY_TYPE, BG_MAKE_RESOURCE_TYPE_HINT("BG_AnimationMirrorDataMap")), "set_bone_maps", "get_bone_maps");
}


void BG_AnimationMirror::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_mirror_data", "mirror_data"), &BG_AnimationMirror::set_mirror_data);
	ClassDB::bind_method(D_METHOD("get_mirror_data"), &BG_AnimationMirror::get_mirror_data);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mirror_data", PROPERTY_HINT_RESOURCE_TYPE, "BG_AnimationMirrorData"), "set_mirror_data", "get_mirror_data");
}

String BG_AnimationMirror::_get_caption() const {
	return "Mirror";
}

Array BG_AnimationMirror::_get_parameter_list() const {
	Array result;
	Dictionary blend_amount_param;
	blend_amount_param["name"] = blend_amount;
	blend_amount_param["type"] = Variant::FLOAT;
	blend_amount_param["hint"] = PROPERTY_HINT_RANGE;
	blend_amount_param["hint_string"] = "0,1,0.01,or_less,or_greater";
	result.append(blend_amount_param);
	return result;
}

BG_AnimationMirror::BG_AnimationMirror()
{
	add_input("in");
	add_input("reference_pose");
}

// double BG_AnimationMirror::_process(double time, bool seek, bool is_external_seeking, bool test_only)
// {
// 	// AnimationNodeBlendTree *blend_tree = Object::cast_to<AnimationNodeBlendTree>(get_parent);
// 	// StringName node_name = get_input_name(0);

// 	return 0.0f;
// }
#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

////
//// BG_Skill_LockUnLock_Condition
////
class BG_Skill_LockUnLock_Condition : public RefCounted
{
    GDCLASS(BG_Skill_LockUnLock_Condition, RefCounted);

protected:
    static void _bind_methods();

public:
    enum ConditionTypes : int32_t
    {
        EQUAL_TO_OR_GREATER_THAN,
        GREATER_THAN,
        EQUAL_TO_OR_LESSER_THAN,
        LESSER_THAN,
    };

    StringName condition_attribute;
    StringName get_condition_attribute() const { return condition_attribute; }

    ConditionTypes condition_type = ConditionTypes::EQUAL_TO_OR_GREATER_THAN;
    ConditionTypes get_condition_type() const { return condition_type; }
    void set_condition_type(ConditionTypes v) { condition_type = v; }

    int condition_value = 0;
    int get_condition_value() const { return condition_value; }
};
VARIANT_ENUM_CAST(BG_Skill_LockUnLock_Condition::ConditionTypes);

////
//// BG_AttributeTreeSkillData
////
class BG_AttributeTreeSkillData : public RefCounted
{
    GDCLASS(BG_AttributeTreeSkillData, RefCounted);

protected:
    static void _bind_methods();

public:
    StringName effect_id;
    StringName get_effect_id() const { return effect_id; }

    TypedArray<BG_Skill_LockUnLock_Condition> unlock_conditions;
    TypedArray<BG_Skill_LockUnLock_Condition> get_unlock_conditions() const { return unlock_conditions; }

    TypedArray<BG_Skill_LockUnLock_Condition> lock_conditions;
    TypedArray<BG_Skill_LockUnLock_Condition> get_lock_conditions() const { return lock_conditions; }
};
#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

// All of the attribute tree functions take attributes as a Dictionary: {attribute_id(StringName): total value(int)}.
// Missing attributes count as 0. The totals (base + upgrades + items + pending points) are built in GDScript.

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

    // True if `value` meets this condition.
    bool is_met(int value) const;
    bool is_met_by_attributes(const Dictionary &attributes) const;

    // How much `value` has to change (up or down) before this condition is met. 0 if it's already met.
    int get_points_until_met(int value) const;

    // The lowest value that meets this condition, for the "greater than" types. 0 for the "lesser than" types.
    int get_minimum_required_value() const;
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
    enum SkillState : int32_t
    {
        LOCKED,     // Unlock conditions aren't met.
        UNLOCKED,   // Unlock conditions are met and no lock condition is.
        LOCKED_OUT, // A lock condition is met. Wins over everything else.
    };

    StringName effect_id;
    StringName get_effect_id() const { return effect_id; }

    TypedArray<BG_Skill_LockUnLock_Condition> unlock_conditions;
    TypedArray<BG_Skill_LockUnLock_Condition> get_unlock_conditions() const { return unlock_conditions; }

    TypedArray<BG_Skill_LockUnLock_Condition> lock_conditions;
    TypedArray<BG_Skill_LockUnLock_Condition> get_lock_conditions() const { return lock_conditions; }

    bool is_passive = false;
    bool get_is_passive() const { return is_passive; }

    bool are_unlock_conditions_met(const Dictionary &attributes) const;
    bool is_locked_out(const Dictionary &attributes) const;
    SkillState get_state(const Dictionary &attributes) const;

    // The smallest attribute change that would meet a lock condition. 0 if already locked out, -1 if there are no lock conditions.
    int get_points_until_locked_out(const Dictionary &attributes) const;

    // Sum of the minimum required values of the unlock conditions. Used for the tree's height.
    int get_unlock_cost() const;

    // The attribute with the largest unlock requirement. Ties go to the later attribute in `column_order`.
    // Falls back to the first attribute in `column_order` if there are no "greater than" unlock conditions.
    StringName get_column_attribute(const TypedArray<StringName> &column_order) const;
};
VARIANT_ENUM_CAST(BG_AttributeTreeSkillData::SkillState);
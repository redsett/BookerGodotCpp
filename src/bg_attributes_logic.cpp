#include "bg_attributes_logic.hpp"

static int get_attribute_value(const Dictionary &attributes, const StringName &attribute_id)
{
    return int(attributes.get(attribute_id, 0));
}

////
//// BG_Skill_LockUnLock_Condition
////
void BG_Skill_LockUnLock_Condition::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_condition_attribute"), &BG_Skill_LockUnLock_Condition::get_condition_attribute);
    ClassDB::bind_method(D_METHOD("get_condition_type"), &BG_Skill_LockUnLock_Condition::get_condition_type);
    ClassDB::bind_method(D_METHOD("set_condition_type"), &BG_Skill_LockUnLock_Condition::set_condition_type);
    ClassDB::bind_method(D_METHOD("get_condition_value"), &BG_Skill_LockUnLock_Condition::get_condition_value);
    ClassDB::bind_method(D_METHOD("is_met", "value"), &BG_Skill_LockUnLock_Condition::is_met);
    ClassDB::bind_method(D_METHOD("is_met_by_attributes", "attributes"), &BG_Skill_LockUnLock_Condition::is_met_by_attributes);
    ClassDB::bind_method(D_METHOD("get_points_until_met", "value"), &BG_Skill_LockUnLock_Condition::get_points_until_met);
    ClassDB::bind_method(D_METHOD("get_minimum_required_value"), &BG_Skill_LockUnLock_Condition::get_minimum_required_value);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "condition_type", PROPERTY_HINT_ENUM,
                              "EQUAL_TO_OR_GREATER_THAN:0,GREATER_THAN:1,EQUAL_TO_OR_LESSER_THAN:2,LESSER_THAN:3"),
                 "set_condition_type", "get_condition_type");

    BIND_ENUM_CONSTANT(EQUAL_TO_OR_GREATER_THAN);
    BIND_ENUM_CONSTANT(GREATER_THAN);
    BIND_ENUM_CONSTANT(EQUAL_TO_OR_LESSER_THAN);
    BIND_ENUM_CONSTANT(LESSER_THAN);
}

bool BG_Skill_LockUnLock_Condition::is_met(int value) const
{
    switch (condition_type)
    {
    case EQUAL_TO_OR_GREATER_THAN: return value >= condition_value;
    case GREATER_THAN:             return value > condition_value;
    case EQUAL_TO_OR_LESSER_THAN:  return value <= condition_value;
    case LESSER_THAN:              return value < condition_value;
    }
    return false;
}

bool BG_Skill_LockUnLock_Condition::is_met_by_attributes(const Dictionary &attributes) const
{
    return is_met(get_attribute_value(attributes, condition_attribute));
}

int BG_Skill_LockUnLock_Condition::get_points_until_met(int value) const
{
    if (is_met(value))
        return 0;

    switch (condition_type)
    {
    case EQUAL_TO_OR_GREATER_THAN: return condition_value - value;
    case GREATER_THAN:             return condition_value + 1 - value;
    case EQUAL_TO_OR_LESSER_THAN:  return value - condition_value;
    case LESSER_THAN:              return value - condition_value + 1;
    }
    return 0;
}

int BG_Skill_LockUnLock_Condition::get_minimum_required_value() const
{
    switch (condition_type)
    {
    case EQUAL_TO_OR_GREATER_THAN: return condition_value;
    case GREATER_THAN:             return condition_value + 1;
    default:                       return 0;
    }
}

////
//// BG_AttributeTreeSkillData
////
void BG_AttributeTreeSkillData::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_effect_id"), &BG_AttributeTreeSkillData::get_effect_id);
    ClassDB::bind_method(D_METHOD("get_unlock_conditions"), &BG_AttributeTreeSkillData::get_unlock_conditions);
    ClassDB::bind_method(D_METHOD("get_lock_conditions"), &BG_AttributeTreeSkillData::get_lock_conditions);
    ClassDB::bind_method(D_METHOD("get_is_passive"), &BG_AttributeTreeSkillData::get_is_passive);
    ClassDB::bind_method(D_METHOD("are_unlock_conditions_met", "attributes"), &BG_AttributeTreeSkillData::are_unlock_conditions_met);
    ClassDB::bind_method(D_METHOD("is_locked_out", "attributes"), &BG_AttributeTreeSkillData::is_locked_out);
    ClassDB::bind_method(D_METHOD("get_state", "attributes"), &BG_AttributeTreeSkillData::get_state);
    ClassDB::bind_method(D_METHOD("get_points_until_locked_out", "attributes"), &BG_AttributeTreeSkillData::get_points_until_locked_out);
    ClassDB::bind_method(D_METHOD("get_unlock_cost"), &BG_AttributeTreeSkillData::get_unlock_cost);
    ClassDB::bind_method(D_METHOD("get_column_attribute", "column_order"), &BG_AttributeTreeSkillData::get_column_attribute);

    BIND_ENUM_CONSTANT(LOCKED);
    BIND_ENUM_CONSTANT(UNLOCKED);
    BIND_ENUM_CONSTANT(LOCKED_OUT);
}

bool BG_AttributeTreeSkillData::are_unlock_conditions_met(const Dictionary &attributes) const
{
    for (int i = 0; i < unlock_conditions.size(); ++i)
    {
        const BG_Skill_LockUnLock_Condition *condition = Object::cast_to<BG_Skill_LockUnLock_Condition>(unlock_conditions[i]);
        if (condition && !condition->is_met_by_attributes(attributes))
            return false;
    }
    return true;
}

bool BG_AttributeTreeSkillData::is_locked_out(const Dictionary &attributes) const
{
    for (int i = 0; i < lock_conditions.size(); ++i)
    {
        const BG_Skill_LockUnLock_Condition *condition = Object::cast_to<BG_Skill_LockUnLock_Condition>(lock_conditions[i]);
        if (condition && condition->is_met_by_attributes(attributes))
            return true;
    }
    return false;
}

BG_AttributeTreeSkillData::SkillState BG_AttributeTreeSkillData::get_state(const Dictionary &attributes) const
{
    if (is_locked_out(attributes))
        return LOCKED_OUT;
    if (are_unlock_conditions_met(attributes))
        return UNLOCKED;
    return LOCKED;
}

int BG_AttributeTreeSkillData::get_points_until_locked_out(const Dictionary &attributes) const
{
    int result = -1;
    for (int i = 0; i < lock_conditions.size(); ++i)
    {
        const BG_Skill_LockUnLock_Condition *condition = Object::cast_to<BG_Skill_LockUnLock_Condition>(lock_conditions[i]);
        if (!condition)
            continue;
        const int points = condition->get_points_until_met(get_attribute_value(attributes, condition->get_condition_attribute()));
        if (result < 0 || points < result)
            result = points;
    }
    return result;
}

int BG_AttributeTreeSkillData::get_unlock_cost() const
{
    int result = 0;
    for (int i = 0; i < unlock_conditions.size(); ++i)
    {
        const BG_Skill_LockUnLock_Condition *condition = Object::cast_to<BG_Skill_LockUnLock_Condition>(unlock_conditions[i]);
        if (condition)
            result += condition->get_minimum_required_value();
    }
    return result;
}

StringName BG_AttributeTreeSkillData::get_column_attribute(const TypedArray<StringName> &column_order) const
{
    if (column_order.is_empty())
        return StringName();

    // Total required value per attribute, in case one attribute has more than one unlock condition.
    Dictionary required_per_attribute;
    for (int i = 0; i < unlock_conditions.size(); ++i)
    {
        const BG_Skill_LockUnLock_Condition *condition = Object::cast_to<BG_Skill_LockUnLock_Condition>(unlock_conditions[i]);
        if (!condition)
            continue;
        const StringName attribute_id = condition->get_condition_attribute();
        required_per_attribute[attribute_id] = get_attribute_value(required_per_attribute, attribute_id) + condition->get_minimum_required_value();
    }

    StringName result = column_order[0];
    int highest = 0;
    for (int i = 0; i < column_order.size(); ++i)
    {
        const StringName attribute_id = column_order[i];
        const int required = get_attribute_value(required_per_attribute, attribute_id);
        if (required > 0 && required >= highest) // `>=` so ties go to the later (right-most) column.
        {
            highest = required;
            result = attribute_id;
        }
    }
    return result;
}

#include "bg_attributes_logic.hpp"

////
//// BG_Skill_LockUnLock_Condition
////
void BG_Skill_LockUnLock_Condition::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_condition_attribute"), &BG_Skill_LockUnLock_Condition::get_condition_attribute);
    ClassDB::bind_method(D_METHOD("get_condition_type"), &BG_Skill_LockUnLock_Condition::get_condition_type);
    ClassDB::bind_method(D_METHOD("set_condition_type"), &BG_Skill_LockUnLock_Condition::set_condition_type);
    ClassDB::bind_method(D_METHOD("get_condition_value"), &BG_Skill_LockUnLock_Condition::get_condition_value);
    ClassDB::bind_method(D_METHOD("get_is_passive"), &BG_Skill_LockUnLock_Condition::get_is_passive);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "condition_type", PROPERTY_HINT_ENUM,
                              "EQUAL_TO_OR_GREATER_THAN:0,GREATER_THAN:1,EQUAL_TO_OR_LESSER_THAN:2,LESSER_THAN:3"),
                 "set_condition_type", "get_condition_type");

    BIND_ENUM_CONSTANT(EQUAL_TO_OR_GREATER_THAN);
    BIND_ENUM_CONSTANT(GREATER_THAN);
    BIND_ENUM_CONSTANT(EQUAL_TO_OR_LESSER_THAN);
    BIND_ENUM_CONSTANT(LESSER_THAN);
}

////
//// BG_AttributeTreeSkillData
////
void BG_AttributeTreeSkillData::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_effect_id"), &BG_AttributeTreeSkillData::get_effect_id);
    ClassDB::bind_method(D_METHOD("get_unlock_conditions"), &BG_AttributeTreeSkillData::get_unlock_conditions);
    ClassDB::bind_method(D_METHOD("get_lock_conditions"), &BG_AttributeTreeSkillData::get_lock_conditions);
}
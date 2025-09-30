#include "bg_reactive.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

////
//// BG_Reactive
////
void BG_Reactive::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_owner"), &BG_Reactive::get_owner);
	ClassDB::bind_method(D_METHOD("set_owner", "value"), &BG_Reactive::set_owner);
	ClassDB::bind_method(D_METHOD("manually_emit"), &BG_Reactive::manually_emit);
	
    ADD_SIGNAL(MethodInfo("reactive_changed", PropertyInfo(Variant::OBJECT, "reactive")));
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "owner", PROPERTY_HINT_RESOURCE_TYPE, "BG_Reactive"), "set_owner", "get_owner");
}

void BG_Reactive::_init(Ref<BG_Reactive> initial_owner)
{
	owner = initial_owner;
}

void BG_Reactive::_propagate(Ref<BG_Reactive> _reactive)
{
	emit_reactive_changed();
}

void BG_Reactive::manually_emit()
{
	emit_reactive_changed();
}

void BG_Reactive::set_owner(Ref<BG_Reactive> value)
{
	if (owner.is_valid()) {
		disconnect_reactive_changed(this);
	}
	owner = value;
	if (owner.is_valid()) {
		connect_reactive_changed(this);
	}
}

////
//// BG_ReactiveObject
////
void BG_ReactiveObject::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_value"), &BG_ReactiveObject::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &BG_ReactiveObject::set_value);
	
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "value"), "set_value", "get_value");
}

void BG_ReactiveObject::_init(Object *initial_value, Ref<BG_Reactive> initial_owner)
{
	BG_Reactive::_init(initial_owner);
	value = initial_value;
}

void BG_ReactiveObject::set_value(Object *v)
{
	Ref<BG_Reactive> reactive_ptr = cast_to<BG_Reactive>(value);
	if (reactive_ptr.is_valid()) {
		disconnect_reactive_changed(reactive_ptr);
	}
	value = v;

	Ref<BG_Reactive> reactive_ptrr = cast_to<BG_Reactive>(value);
	if (reactive_ptrr.is_valid()) {
		connect_reactive_changed(reactive_ptrr);
	}
	emit_reactive_changed();
}

////
//// BG_ReactiveInt
////
void BG_ReactiveInt::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_value"), &BG_ReactiveInt::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &BG_ReactiveInt::set_value);
	
	ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
}

void BG_ReactiveInt::_init(int initial_value, Ref<BG_Reactive> initial_owner)
{
	BG_Reactive::_init(initial_owner);
	value = initial_value;
}

void BG_ReactiveInt::set_value(int v)
{
	value = v;
	emit_reactive_changed();
}

////
//// BG_ReactiveString
////
void BG_ReactiveString::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_value"), &BG_ReactiveString::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &BG_ReactiveString::set_value);
	
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "value"), "set_value", "get_value");
}

void BG_ReactiveString::_init(String initial_value, Ref<BG_Reactive> initial_owner)
{
	BG_Reactive::_init(initial_owner);
	value = initial_value;
}

void BG_ReactiveString::set_value(String v)
{
	value = v;
	emit_reactive_changed();
}

////
//// BG_ReactiveArray
////
void BG_ReactiveArray::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_value"), &BG_ReactiveArray::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &BG_ReactiveArray::set_value);

	ClassDB::bind_method(D_METHOD("get_at"), &BG_ReactiveArray::get_at);
	ClassDB::bind_method(D_METHOD("append"), &BG_ReactiveArray::append);
	ClassDB::bind_method(D_METHOD("append_array"), &BG_ReactiveArray::append_array);
	ClassDB::bind_method(D_METHOD("assign"), &BG_ReactiveArray::assign);
	ClassDB::bind_method(D_METHOD("clear"), &BG_ReactiveArray::clear);
	ClassDB::bind_method(D_METHOD("erase"), &BG_ReactiveArray::erase);
	ClassDB::bind_method(D_METHOD("insert"), &BG_ReactiveArray::insert);
	ClassDB::bind_method(D_METHOD("pop_at"), &BG_ReactiveArray::pop_at);
	ClassDB::bind_method(D_METHOD("pop_back"), &BG_ReactiveArray::pop_back);
	ClassDB::bind_method(D_METHOD("pop_front"), &BG_ReactiveArray::pop_front);
	ClassDB::bind_method(D_METHOD("push_back"), &BG_ReactiveArray::push_back);
	ClassDB::bind_method(D_METHOD("remove_at"), &BG_ReactiveArray::remove_at);
	ClassDB::bind_method(D_METHOD("shuffle"), &BG_ReactiveArray::shuffle);
	ClassDB::bind_method(D_METHOD("sort"), &BG_ReactiveArray::sort);
	ClassDB::bind_method(D_METHOD("sort_custom"), &BG_ReactiveArray::sort_custom);
	
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "value"), "set_value", "get_value");
}

void BG_ReactiveArray::_init(Array initial_value)
{
	value = initial_value;
}
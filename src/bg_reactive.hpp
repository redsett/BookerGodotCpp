#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>

using namespace godot;

////
//// BG_Reactive
////
class BG_Reactive : public Resource
{
	GDCLASS(BG_Reactive, Resource);

protected:
	static void _bind_methods();

	inline void emit_reactive_changed() { emit_signal("reactive_changed", this); }
	inline void disconnect_reactive_changed(Ref<BG_Reactive> r) { r->disconnect("reactive_changed", Callable(this, "_propagate")); }
	inline void connect_reactive_changed(Ref<BG_Reactive> r) { r->connect("reactive_changed", Callable(this, "_propagate")); }

public:
	void _init(Ref<BG_Reactive> initial_owner = nullptr);
	void _propagate(Ref<BG_Reactive> _reactive = nullptr);
	void manually_emit();
	
	Ref<BG_Reactive> owner;
	Ref<BG_Reactive> get_owner() const { return owner; }
	void set_owner(Ref<BG_Reactive> value);
};

////
//// BG_ReactiveObject
////
class BG_ReactiveObject : public BG_Reactive
{
	GDCLASS(BG_ReactiveObject, BG_Reactive);

protected:
	static void _bind_methods();

public:
	void _init(Object *initial_value, Ref<BG_Reactive> initial_owner = nullptr);

	Object *value;
	Object *get_value() const { return value; }
	void set_value(Object *v);
};

////
//// BG_ReactiveInt
////
class BG_ReactiveInt : public BG_Reactive
{
	GDCLASS(BG_ReactiveInt, BG_Reactive);

protected:
	static void _bind_methods();

public:
	void _init(int initial_value, Ref<BG_Reactive> initial_owner = nullptr);

	int value;
	int get_value() const { return value; }
	void set_value(int v);
};

////
//// BG_ReactiveString
////
class BG_ReactiveString : public BG_Reactive
{
	GDCLASS(BG_ReactiveString, BG_Reactive);

protected:
	static void _bind_methods();

public:
	void _init(String initial_value, Ref<BG_Reactive> initial_owner = nullptr);

	String value;
	String get_value() const { return value; }
	void set_value(String v);
};

////
//// BG_ReactiveArray
////
class BG_ReactiveArray : public BG_Reactive
{
	GDCLASS(BG_ReactiveArray, BG_Reactive);

protected:
	static void _bind_methods();

public:
	void _init(Array initial_value = {});
	
	Variant get_at(int i) { return value[i]; }
	void set_at(int i, Variant v) {
		value[i] = v;
		emit_reactive_changed();
	}

	void append(Variant v) {
		value.append(v);
		emit_reactive_changed();
	}
	void append_array(Array array) {
		value.append_array(array);
		emit_reactive_changed();
	}
	void assign(Array array) {
		value.assign(array);
		emit_reactive_changed();
	}
	void clear() {
		value.clear();
		emit_reactive_changed();
	}
	void erase(Variant v) {
		value.erase(v);
		emit_reactive_changed();
	}
	void insert(int position, Variant v) {
		value.insert(position, v);
		emit_reactive_changed();
	}
	Variant pop_at(int position) {
		Variant result = value.pop_at(position);
		emit_reactive_changed();
		return result;
	}
	Variant pop_back() {
		Variant result = value.pop_back();
		emit_reactive_changed();
		return result;
	}
	Variant pop_front() {
		Variant result = value.pop_front();
		emit_reactive_changed();
		return result;
	}
	void push_back(Variant v) {
		value.push_back(v);
		emit_reactive_changed();
	}
	void remove_at(int position) {
		value.remove_at(position);
		emit_reactive_changed();
	}
	void shuffle() {
		value.shuffle();
		emit_reactive_changed();
	}
	void sort() {
		value.sort();
		emit_reactive_changed();
	}
	void sort_custom(Callable callable) {
		value.sort_custom(callable);
		emit_reactive_changed();
	}

	Array value;
	Array get_value() const { return value; }
	void set_value(Array v) {
		value = v;
		emit_reactive_changed();
	}
};
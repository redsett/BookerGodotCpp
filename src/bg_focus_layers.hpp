#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class BG_Focus_Layers : public Object
{
	GDCLASS(BG_Focus_Layers, Object);

	static BG_Focus_Layers *singleton;

protected:
	static void _bind_methods();

    TypedArray<StringName> _focus_layer_stack;
    Dictionary _focus_layer_controls;

    void _focus_active_control();
    bool _is_control_top(const Control *ctrl);
    bool _is_control_bottom(const Control *ctrl);

public:
	BG_Focus_Layers();
	~BG_Focus_Layers();

    void try_set_focused_control(const Control *p_ctrl);
    void set_focus_layer(const StringName &p_layer_name);
    void remove_focus_layer(const StringName &p_layer_name, bool p_full_remove = false);
    void add_focus_layer(const StringName &p_layer_name, TypedArray<Control> p_controls, const Control *p_focused_control, bool p_should_loop_vertically, bool p_select_layer = true);
    void find_control_in_direction(Vector2 direction);

    static BG_Focus_Layers *get_singleton();
};
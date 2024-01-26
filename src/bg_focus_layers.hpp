#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class BG_Focus_Layers : public Object
{
	GDCLASS(BG_Focus_Layers, Object);

	static BG_Focus_Layers *singleton;

protected:
	static void _bind_methods();

    float _minimum_angle = 0.5f;

    bool _is_using_gamepad = true;
    TypedArray<StringName> _focus_layer_stack;
    Dictionary _focus_layer_controls; // {[StringName(focus layer name) : [Control(parent control), Control(focused control), Control(back/close button), bool(should loop vertically)]]}

    inline void _set_control_default_focus_static(Control *p_control);
    void _focus_active_control();
    bool _is_control_top(const Control *ctrl, const TypedArray<Control> &all_ctrls);
    bool _is_control_bottom(const Control *ctrl, const TypedArray<Control> &all_ctrls);
    Control *_get_active_control() const;
    Button *_get_active_back_button() const;
    static bool _check_if_valid_control(const Control *c);

public:
	BG_Focus_Layers();
	~BG_Focus_Layers();

    void try_set_focused_control(const Control *p_ctrl);
    void set_focus_layer(const StringName &p_layer_name);
    void remove_focus_layer(const StringName &p_layer_name, bool p_full_remove = false);
    void add_focus_layer(
        const StringName &p_layer_name, 
        const Control *p_parent_control, 
        const Control *p_focused_control, 
        Control *p_back_button, 
        bool p_should_loop_vertically, 
        bool p_select_layer = true
    );
    void find_control_in_direction(Vector2 direction);
    void input_type_updated(bool using_gamepad);
    void press_back_button() const;

    void set_minimum_angle(float angle) { _minimum_angle = angle; };

    static Control *find_valid_control(const TypedArray<Control> p_controls);
    static TypedArray<Control> get_all_focusable_controls_under_control(const Control *p_control);
    static BG_Focus_Layers *get_singleton();
};
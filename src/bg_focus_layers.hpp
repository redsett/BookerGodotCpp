#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

class BG_Focus_Layer_Properties : public Object
{
	GDCLASS(BG_Focus_Layer_Properties, Object);

protected:
    static void _bind_methods();

    static bool bg_is_instance_valid(const Object *obj) {
        try {
            return UtilityFunctions::is_instance_id_valid(obj->get_instance_id());
        }
        catch (...) {
        }
        return false;
    };

public:
    StringName _focus_layer_name;
    const StringName &get_focus_layer_name() const { return _focus_layer_name; }
    void set_focus_layer_name(const StringName &p_focus_layer_name) { _focus_layer_name = p_focus_layer_name; }

    Control *_parent_control;
    Control *get_parent_control() const {
        if (bg_is_instance_valid(_parent_control))
            return _parent_control;
        return nullptr;
    }
    void set_parent_control(Control *p_parent_control) { _parent_control = p_parent_control; }

    Control *_focused_control;
    Control *get_focused_control() const {
        if (bg_is_instance_valid(_focused_control))
            return _focused_control;
        return nullptr;
    }
    void set_focused_control(Control *p_focused_control) { _focused_control = p_focused_control; }

    Button *_back_button;
    Button *get_back_button() const {
        if (bg_is_instance_valid(_back_button))
            return _back_button;
        return nullptr;
    }
    void set_back_button(Button *p_back_button) { _back_button = p_back_button; }

    bool _should_loop_vertically = false;
    bool get_should_loop_vertically() const { return _should_loop_vertically; }
    void set_should_loop_vertically(bool p_should_loop_vertically) { _should_loop_vertically = p_should_loop_vertically; }

    static bool has_focus_layer(const TypedArray<BG_Focus_Layer_Properties> &p_focus_layers, const StringName &p_focus_layer_name);
};

class BG_Focus_Layers : public Object
{
	GDCLASS(BG_Focus_Layers, Object);

	static BG_Focus_Layers *singleton;

protected:
    static bool bg_is_instance_valid(const Object *obj) {
        return (obj == nullptr) ? false : UtilityFunctions::is_instance_id_valid(obj->get_instance_id());
    };

	static void _bind_methods();

    float _minimum_angle = 0.5f;

    bool _is_using_gamepad = true;
    TypedArray<String> _focus_layer_stack;
    TypedArray<BG_Focus_Layer_Properties> _focus_layers;

    inline void _set_control_default_focus_static(Control *p_control);
    void _focus_active_control();
    bool _is_control_top(const Control *ctrl, const TypedArray<Control> &all_ctrls);
    bool _is_control_bottom(const Control *ctrl, const TypedArray<Control> &all_ctrls);
    Control *_get_active_control();
    Button *_get_active_back_button() const;
    static bool _check_if_valid_control(const Control *c);

public:
	BG_Focus_Layers();
	~BG_Focus_Layers();

    TypedArray<StringName> get_focus_layer_stack() const { return _focus_layer_stack; }
    TypedArray<BG_Focus_Layer_Properties> get_focus_layers() const { return _focus_layers; }
    BG_Focus_Layer_Properties *get_active_focus_layer() const;
    const BG_Focus_Layer_Properties *get_active_focus_layer_const() const { return get_active_focus_layer(); }

    void try_set_focused_control(Control *p_ctrl);
    void clear_focus_layers(const TypedArray<StringName> &except_for);
    void set_focus_layer(const StringName &p_layer_name);
    void remove_focus_layer(const StringName &p_layer_name, bool p_full_remove = false);
    void add_focus_layer(
        const StringName &p_layer_name, 
        Control *p_parent_control, 
        Control *p_focused_control, 
        Control *p_back_button, 
        bool p_should_loop_vertically, 
        bool p_select_layer = true
    );
    Control *get_current_parent_control() const;
    void find_control_in_direction(Vector2 direction);
    void input_type_updated(bool using_gamepad);
    void press_back_button() const;

    void set_minimum_angle(float angle) { _minimum_angle = angle; };

    static Control *find_valid_control(const TypedArray<Control> &p_controls);
    static TypedArray<Control> get_all_focusable_controls_under_control(const Control *p_control);
    static TypedArray<Control> get_current_focusable_controls_under_control(const Control *p_control);
    static BG_Focus_Layers *get_singleton();
};
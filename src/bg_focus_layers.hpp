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

public:
    static bool bg_is_instance_valid(const Object *obj) {
        try {
            // return (obj && UtilityFunctions::is_instance_id_valid(obj->get_instance_id()));
			return (obj && ObjectDB::get_instance(obj->get_instance_id()) != nullptr);
        }
        catch (...) { }
        return false;
    };

    StringName _focus_layer_name;
    const StringName &get_focus_layer_name() const { return _focus_layer_name; }
    void set_focus_layer_name(const StringName &p_focus_layer_name) { _focus_layer_name = p_focus_layer_name; }

    // The controls are stored as instance ids, not pointers. These nodes can be freed while the layer still exists
    // (scene changes, queue_free), and a stale pointer can't be checked safely. ObjectDB returns nullptr for a freed id.
    static uint64_t _get_id(const Object *obj) { return obj ? uint64_t(obj->get_instance_id()) : 0; }
    template <typename T>
    static T *_get_from_id(uint64_t id) { return id == 0 ? nullptr : Object::cast_to<T>(ObjectDB::get_instance(id)); }

    uint64_t _parent_control_id = 0;
    Control *get_parent_control() const { return _get_from_id<Control>(_parent_control_id); }
    void set_parent_control(Control *p_parent_control) { _parent_control_id = _get_id(p_parent_control); }

    uint64_t _focused_control_id = 0;
    Control *get_focused_control() const { return _get_from_id<Control>(_focused_control_id); }
    void set_focused_control(Control *p_focused_control) { _focused_control_id = _get_id(p_focused_control); }

    uint64_t _back_button_id = 0;
    Button *get_back_button() const { return _get_from_id<Button>(_back_button_id); }
    void set_back_button(Button *p_back_button) { _back_button_id = _get_id(p_back_button); }

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
	static void _bind_methods();

    // Direction search tuning (see find_control_in_direction). Mirrored by BG_FocusLayers.gd's search_settings.
    float _max_angle_degrees = 80.0f; // Straight presses: how far off the direction a control can be.
    float _diagonal_max_angle_degrees = 35.0f; // Diagonal presses: narrower, so they don't steal straight moves.
    float _side_weight = 2.5f; // Cost of a pixel sideways, relative to a pixel forward.
    float _aligned_side_factor = 0.25f; // Sideways cost multiplier for controls in the same row/column (rects overlap sideways).

    bool _is_using_gamepad = true;
    TypedArray<String> _focus_layer_stack;
    TypedArray<BG_Focus_Layer_Properties> _focus_layers;
    
    struct ScreenRect {
        Rect2 aabb; // Screen-space bounds (handles the window scale, rotation and SubViewports).
        Vector2 center;
    };
    static ScreenRect _get_screen_rect(const Control *c);
    static int _get_side(const Vector2 &dir);
    static bool _overlaps_sideways(const Rect2 &a, const Rect2 &b, const Vector2 &dir);
    Control *_find_in_direction(BG_Focus_Layer_Properties *prop, Control *from, const Vector2 &dir, Dictionary *report);

    void _focus_active_control();
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
    void find_control_in_direction(const Vector2 &direction);
    Dictionary preview_direction(const Vector2 &direction);
    void input_type_updated(bool using_gamepad);
    void press_back_button() const;

    void set_search_settings(const Dictionary &p_settings);
    Dictionary get_search_settings() const;

    static Control *find_valid_control(const TypedArray<Control> &p_controls);
    static TypedArray<Control> get_all_focusable_controls_under_control(const Control *p_control);
    static TypedArray<Control> get_current_focusable_controls_under_control(const Control *p_control);
    static BG_Focus_Layers *get_singleton();
};
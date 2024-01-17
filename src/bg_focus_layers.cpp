#include "bg_focus_layers.hpp"

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/input.hpp>

////
//// BG_Focus_Layers
////
BG_Focus_Layers *BG_Focus_Layers::singleton = nullptr;

void BG_Focus_Layers::_bind_methods()
{
	// ClassDB::bind_static_method("BG_Booker_DB", D_METHOD("timer_test"), &BG_Booker_DB::timer_test);
	ClassDB::bind_method(D_METHOD("try_set_focused_control", "control_to_focus"), &BG_Focus_Layers::try_set_focused_control);
	ClassDB::bind_method(D_METHOD("set_focus_layer", "layer_name"), &BG_Focus_Layers::set_focus_layer);
	ClassDB::bind_method(D_METHOD("remove_focus_layer", "layer_name", "should_fully_remove_layer"), &BG_Focus_Layers::remove_focus_layer);
	ClassDB::bind_method(D_METHOD("add_focus_layer", "layer_name", "controls_in_layer", "control_to_focus", "should_loop_vertically", "select_layer"), &BG_Focus_Layers::add_focus_layer);
	ClassDB::bind_method(D_METHOD("find_control_in_direction", "direction"), &BG_Focus_Layers::find_control_in_direction);
	ClassDB::bind_method(D_METHOD("input_type_updated", "is_using_gamepad"), &BG_Focus_Layers::input_type_updated);
}

BG_Focus_Layers *BG_Focus_Layers::get_singleton()
{
	return singleton;
}

BG_Focus_Layers::BG_Focus_Layers()
{
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;
}

BG_Focus_Layers::~BG_Focus_Layers()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;
}

void BG_Focus_Layers::try_set_focused_control(const Control *p_ctrl)
{
	if (_focus_layer_stack.is_empty())
		return;

    const Array &keys = _focus_layer_controls.keys();
	for (int i = 0; i < keys.size(); i++)
    {
        Array layer_values = _focus_layer_controls[keys[i]];
        const TypedArray<Control> &layer_controls = layer_values[0];
		if (layer_controls.has(p_ctrl))
        {
			layer_values[1] = p_ctrl;
            break;
        }
    }
}

void BG_Focus_Layers::set_focus_layer(const StringName &p_layer_name)
{
	if (_focus_layer_stack.has(p_layer_name))
    {
		_focus_layer_stack.erase(p_layer_name);
    }
	_focus_layer_stack.push_front(p_layer_name);
	_focus_active_control();
}

void BG_Focus_Layers::remove_focus_layer(const StringName &p_layer_name, bool p_full_remove)
{
    //ERR_FAIL_COND(!_focus_layer_controls.has(p_layer_name));
	if (_focus_layer_stack.has(p_layer_name))
    {
		_focus_layer_stack.erase(p_layer_name);
    }
	if (p_full_remove && _focus_layer_controls.has(p_layer_name))
    {
		_focus_layer_controls.erase(p_layer_name);
    }
	if (!_focus_layer_stack.is_empty())
    {
		_focus_active_control();
    }
}

Control *BG_Focus_Layers::_get_active_control() const
{
    const Array &layer_values = _focus_layer_controls[_focus_layer_stack[0]];
    return cast_to<Control>(layer_values[1]);
}

void BG_Focus_Layers::_focus_active_control()
{
    Control *control_to_focus = _get_active_control();
    if(!_check_if_valid_control(control_to_focus))
    {
        find_control_in_direction(Vector2(0, 1));
    }
    else
    {
        control_to_focus->grab_focus();
        if (!_is_using_gamepad)
            control_to_focus->release_focus();
    }
}

void BG_Focus_Layers::add_focus_layer(const StringName &p_layer_name, TypedArray<Control> p_controls, const Control *p_focused_control, bool p_should_loop_vertically, bool p_select_layer)
{
	// Basically ignore the built in method of finding controls to focus. We will do this ourselves in find_control_in_direction().
    for (int i = 0; i < p_controls.size(); i++)
    {
        Control *control = cast_to<Control>(p_controls[i]);
		control->set_focus_neighbor(godot::Side::SIDE_LEFT, control->get_path());
		control->set_focus_neighbor(godot::Side::SIDE_BOTTOM, control->get_path());
		control->set_focus_neighbor(godot::Side::SIDE_RIGHT, control->get_path());
		control->set_focus_neighbor(godot::Side::SIDE_TOP, control->get_path());
    }
	_focus_layer_controls[p_layer_name] = Array::make(p_controls, p_focused_control, p_should_loop_vertically);
	if (p_select_layer)
    {
		set_focus_layer(p_layer_name);
    }
}

bool BG_Focus_Layers::_check_if_valid_control(const Control *c) const
{
    if (c->is_visible_in_tree() && c->get_mouse_filter() != Control::MouseFilter::MOUSE_FILTER_IGNORE)
    {
        const Button *btn = cast_to<Button>(c);
        if (btn == nullptr || !btn->is_disabled())
        {
            return true;
        }
    }
    return false;
}


bool BG_Focus_Layers::_is_control_top(const Control *ctrl)
{
    const float c_top_y = ctrl->get_global_position().y;
    const Array &layer_values = _focus_layer_controls[_focus_layer_stack[0]];
    const TypedArray<Control> &ctrls = layer_values[0];
    for (int i = 0; i < ctrls.size(); i++)
    {
        const Control *child = cast_to<Control>(ctrls[i]);
        if (c_top_y > child->get_global_position().y && _check_if_valid_control(child))
        {
            return false;
        }
    }
    return true;
}

bool BG_Focus_Layers::_is_control_bottom(const Control *ctrl)
{
    const float c_bottom_y = ctrl->get_global_position().y + ctrl->get_global_rect().size.y;
    const Array &layer_values = _focus_layer_controls[_focus_layer_stack[0]];
    const TypedArray<Control> &ctrls = layer_values[0];
    for (int i = 0; i < ctrls.size(); i++)
    {
        const Control *child = cast_to<Control>(ctrls[i]);
        if (c_bottom_y < (child->get_global_position().y + child->get_global_rect().size.y) && _check_if_valid_control(child))
        {
            return false;
        }
    }
    return true;
}

void BG_Focus_Layers::find_control_in_direction(Vector2 direction)
{
	if (_focus_layer_stack.is_empty() || !_focus_layer_controls.has(_focus_layer_stack[0]))
    {
		return;
    }

    Array layer_values = _focus_layer_controls[_focus_layer_stack[0]];
    const TypedArray<Control> &ctrls = layer_values[0];
	Control *last_selected_control = _get_active_control();
	if (last_selected_control != nullptr)
    {
		const bool should_loop_vertically = bool(layer_values[2]);
		bool should_get_farthest_control = false;
		Vector2 last_control_dir_location = Vector2();
		if (direction == Vector2(0, -1))
        {
			last_control_dir_location = last_selected_control->get_global_position() + (last_selected_control->get_global_rect().size * Vector2(0.5, 0.0));
			if (should_loop_vertically)
				should_get_farthest_control = _is_control_top(last_selected_control);
        } else if (direction == Vector2(0, 1))
        {
			if (should_loop_vertically)
				should_get_farthest_control = _is_control_bottom(last_selected_control);
			last_control_dir_location = last_selected_control->get_global_position() + (last_selected_control->get_global_rect().size * Vector2(0.5, 1.0));
        } else if (direction == Vector2(1, 0))
        {
			last_control_dir_location = last_selected_control->get_global_position() + (last_selected_control->get_global_rect().size * Vector2(1.0, 0.5));
        } else
        {
			last_control_dir_location = last_selected_control->get_global_position() + (last_selected_control->get_global_rect().size * Vector2(0.0, 0.5));
        }

		const Control *new_selected_control = nullptr;
		float best_distance = should_get_farthest_control ? 0.0f : 9999.0f;
        for (int i = 0; i < ctrls.size(); i++)
        {
            const Control *c = cast_to<Control>(ctrls[i]);
            if (c != nullptr && c != last_selected_control && _check_if_valid_control(c))
            {
                const Vector2 c_center_location = c->get_global_position() + (c->get_global_rect().size * 0.5);
                static const float PI = 3.14159f;
                const float angle = abs(direction.angle_to(last_control_dir_location - c_center_location) / PI);
                static const float MIN_ANGLE = 0.5f;
                if ((should_get_farthest_control && angle < MIN_ANGLE) || (!should_get_farthest_control && angle > MIN_ANGLE))
                {
                    float distance = last_control_dir_location.distance_to(c_center_location);
                    if (!should_get_farthest_control)
                        distance *= angle; // Favor controls in a better angle.
                    if ((should_get_farthest_control && distance > best_distance) || (!should_get_farthest_control && distance < best_distance))
                    {
                        best_distance = distance;
                        new_selected_control = c;
                    }
                }
            }
        }

		if (new_selected_control != nullptr)
			layer_values[1] = new_selected_control;
    }

	// await get_tree().process_frame # Without this, sometimes the controls don't get unfocused visually.
	if (last_selected_control != layer_values[1])
    {
		last_selected_control->release_focus();
		_focus_active_control();
    }
}

void BG_Focus_Layers::input_type_updated(bool using_gamepad)
{
    _is_using_gamepad = using_gamepad;
    if (_is_using_gamepad)
    {
        Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_HIDDEN);
        if (!_focus_layer_stack.is_empty())
            _focus_active_control();
    }
    else
    {
        Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
        if (!_focus_layer_stack.is_empty())
            _get_active_control()->release_focus();
    }
}
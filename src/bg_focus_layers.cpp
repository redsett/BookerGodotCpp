#include "bg_focus_layers.hpp"

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

////
//// BG_Focus_Layer_Properties
////
void BG_Focus_Layer_Properties::_bind_methods()
{
    ClassDB::bind_static_method("BG_Focus_Layer_Properties", D_METHOD("has_focus_layer", "focus_layers", "focus_layer_name"), &BG_Focus_Layer_Properties::has_focus_layer);

	ClassDB::bind_method(D_METHOD("get_focus_layer_name"), &BG_Focus_Layer_Properties::get_focus_layer_name);
	ClassDB::bind_method(D_METHOD("set_focus_layer_name"), &BG_Focus_Layer_Properties::set_focus_layer_name);
    ClassDB::bind_method(D_METHOD("get_parent_control"), &BG_Focus_Layer_Properties::get_parent_control);
	ClassDB::bind_method(D_METHOD("set_parent_control"), &BG_Focus_Layer_Properties::set_parent_control);
	ClassDB::bind_method(D_METHOD("get_focused_control"), &BG_Focus_Layer_Properties::get_focused_control);
	ClassDB::bind_method(D_METHOD("set_focused_control"), &BG_Focus_Layer_Properties::set_focused_control);
	ClassDB::bind_method(D_METHOD("get_back_button"), &BG_Focus_Layer_Properties::get_back_button);
	ClassDB::bind_method(D_METHOD("set_back_button"), &BG_Focus_Layer_Properties::set_back_button);
	ClassDB::bind_method(D_METHOD("get_should_loop_vertically"), &BG_Focus_Layer_Properties::get_should_loop_vertically);
	ClassDB::bind_method(D_METHOD("set_should_loop_vertically"), &BG_Focus_Layer_Properties::set_should_loop_vertically);
}

/* static */ bool BG_Focus_Layer_Properties::has_focus_layer(const TypedArray<BG_Focus_Layer_Properties> &p_focus_layers, const StringName &p_focus_layer_name)
{
    for (int i = 0; i < p_focus_layers.size(); i++)
    {
        const BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(p_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop) && prop->get_focus_layer_name() == p_focus_layer_name)
        {
            return true;
        }
    }
    return false;
}

////
//// BG_Focus_Layers
////
BG_Focus_Layers *BG_Focus_Layers::singleton = nullptr;

void BG_Focus_Layers::_bind_methods()
{
    ClassDB::bind_static_method("BG_Focus_Layers", D_METHOD("find_valid_control", "controls"), &BG_Focus_Layers::find_valid_control);
    ClassDB::bind_static_method("BG_Focus_Layers", D_METHOD("get_all_focusable_controls_under_control", "control"), &BG_Focus_Layers::get_all_focusable_controls_under_control);
    ClassDB::bind_static_method("BG_Focus_Layers", D_METHOD("get_current_focusable_controls_under_control", "control"), &BG_Focus_Layers::get_current_focusable_controls_under_control);

	ClassDB::bind_method(D_METHOD("get_focus_layer_stack"), &BG_Focus_Layers::get_focus_layer_stack);
	ClassDB::bind_method(D_METHOD("get_focus_layers"), &BG_Focus_Layers::get_focus_layers);
	ClassDB::bind_method(D_METHOD("get_active_focus_layer"), &BG_Focus_Layers::get_active_focus_layer);
	ClassDB::bind_method(D_METHOD("try_set_focused_control", "control_to_focus"), &BG_Focus_Layers::try_set_focused_control);
	ClassDB::bind_method(D_METHOD("clear_focus_layers", "except_for"), &BG_Focus_Layers::clear_focus_layers);
	ClassDB::bind_method(D_METHOD("set_focus_layer", "layer_name"), &BG_Focus_Layers::set_focus_layer);
	ClassDB::bind_method(D_METHOD("remove_focus_layer", "layer_name", "should_fully_remove_layer"), &BG_Focus_Layers::remove_focus_layer);
	ClassDB::bind_method(D_METHOD("add_focus_layer", "layer_name", "parent_control", "control_to_focus", "back_button", "should_loop_vertically", "select_layer"), &BG_Focus_Layers::add_focus_layer);
	ClassDB::bind_method(D_METHOD("get_current_parent_control"), &BG_Focus_Layers::get_current_parent_control);
	ClassDB::bind_method(D_METHOD("find_control_in_direction", "direction"), &BG_Focus_Layers::find_control_in_direction);
	ClassDB::bind_method(D_METHOD("input_type_updated", "is_using_gamepad"), &BG_Focus_Layers::input_type_updated);
	ClassDB::bind_method(D_METHOD("press_back_button"), &BG_Focus_Layers::press_back_button);
	ClassDB::bind_method(D_METHOD("set_minimum_angle", "angle"), &BG_Focus_Layers::set_minimum_angle);
}

BG_Focus_Layers *BG_Focus_Layers::get_singleton()
{
	return singleton;
}

BG_Focus_Layers::BG_Focus_Layers()
{
	ERR_FAIL_COND(BG_Focus_Layer_Properties::bg_is_instance_valid(singleton));
	singleton = this;
}

BG_Focus_Layers::~BG_Focus_Layers()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;
}

void BG_Focus_Layers::_set_control_default_focus_static(Control *p_control)
{
		p_control->set_focus_neighbor(godot::Side::SIDE_LEFT, p_control->get_path());
		p_control->set_focus_neighbor(godot::Side::SIDE_BOTTOM, p_control->get_path());
		p_control->set_focus_neighbor(godot::Side::SIDE_RIGHT, p_control->get_path());
		p_control->set_focus_neighbor(godot::Side::SIDE_TOP, p_control->get_path());
}

void BG_Focus_Layers::try_set_focused_control(Control *p_ctrl)
{
	if (_focus_layer_stack.is_empty())
		return;

    for (int i = 0; i < _focus_layers.size(); i++)
    {
        BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop))
        {
            const TypedArray<Control> layer_controls = BG_Focus_Layers::get_all_focusable_controls_under_control(prop->get_parent_control());
            if (layer_controls.has(p_ctrl))
            {
                prop->set_focused_control(p_ctrl);
                for (int i = 0; i < layer_controls.size(); i++)
                    _set_control_default_focus_static(cast_to<Control>(layer_controls[i]));
                _focus_active_control();
                break;
            }
        }
    }
}

void BG_Focus_Layers::clear_focus_layers(const TypedArray<StringName> &except_for)
{
	if (_focus_layer_stack.is_empty())
		return;

    for (int i = _focus_layers.size() - 1; i >= 0; --i) // Reverse loop since we are removing layers as we go.
    {
        const BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop) && !except_for.has(prop->get_focus_layer_name()))
        {
            remove_focus_layer(prop->get_focus_layer_name(), true);
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
	if (p_full_remove)
    {
        for (int i = 0; i < _focus_layers.size(); i++)
        {
            const BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop) && prop->get_focus_layer_name() == p_layer_name)
            {
                _focus_layers.erase(prop);
                memdelete(prop);
                break;
            }
        }
    }
	if (!_focus_layer_stack.is_empty())
    {
		_focus_active_control();
    }
}

BG_Focus_Layer_Properties *BG_Focus_Layers::get_active_focus_layer() const
{
    if (_focus_layer_stack.is_empty())
        return nullptr;
    
    for (int i = 0; i < _focus_layers.size(); i++)
    {
        BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop) && prop->get_focus_layer_name() == String(_focus_layer_stack[0]))
        {
            return prop;
        }
    }
    return nullptr;
}

Control *BG_Focus_Layers::_get_active_control()
{
    if (_focus_layer_stack.is_empty())
        return nullptr;
    
    BG_Focus_Layer_Properties *prop = get_active_focus_layer();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(prop))
    {
        UtilityFunctions::print("ERROR: Could not find control to focus in _get_active_control(), this is probably very bad. Did not focus a control.");
        return nullptr;
    }
    
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop->get_focused_control()) && _check_if_valid_control(prop->get_focused_control())) {
        return prop->get_focused_control();
    } else {
        // Let's find a new control to focus.
        const TypedArray<Control> focusable_ctrls = BG_Focus_Layers::get_all_focusable_controls_under_control(prop->get_parent_control());
        for (int i = 0; i < focusable_ctrls.size(); i++) {
            if (_check_if_valid_control(cast_to<Control>(focusable_ctrls[i]))) {
                prop->set_focused_control(cast_to<Control>(focusable_ctrls[i]));
                return prop->get_focused_control();
            }
        }
    }
    return nullptr;
}

Button *BG_Focus_Layers::_get_active_back_button() const
{
    const BG_Focus_Layer_Properties *prop = get_active_focus_layer_const();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(prop))
        return nullptr;
    return prop->get_back_button();
}

void BG_Focus_Layers::_focus_active_control()
{
    Control *control_to_focus = _get_active_control();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(control_to_focus))
        return;
    
    control_to_focus->grab_focus();
    if (!_is_using_gamepad)
        control_to_focus->release_focus();
}

void BG_Focus_Layers::add_focus_layer(
    const StringName &p_layer_name, 
    Control *p_parent_control, 
    Control *p_focused_control, 
    Control *p_back_button, 
    bool p_should_loop_vertically, 
    bool p_select_layer
)
{
	// Basically ignore the built in method of finding controls to focus. We will do this ourselves in find_control_in_direction().
    TypedArray<Control> focusable_ctrls = BG_Focus_Layers::get_all_focusable_controls_under_control(p_parent_control);
    for (int i = 0; i < focusable_ctrls.size(); i++)
		_set_control_default_focus_static(cast_to<Control>(focusable_ctrls[i]));

    Button *back_btn = cast_to<Button>(p_back_button);
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(back_btn))
        back_btn->set_visible(!_is_using_gamepad);


    BG_Focus_Layer_Properties *new_prop_layer = nullptr;

    // Reuse the existing focus layer if it already exists.
    if (BG_Focus_Layer_Properties::has_focus_layer(_focus_layers, p_layer_name))
    {
        for (int i = 0; i < _focus_layers.size(); i++)
        {
            BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop) && prop->get_focus_layer_name() == p_layer_name)
            {
                new_prop_layer = prop;
                _focus_layers.erase(prop);
                _focus_layers.append(prop);
                break;
            }
        }
    }

    if (new_prop_layer == nullptr)
    {
        new_prop_layer = memnew(BG_Focus_Layer_Properties);
        _focus_layers.append(new_prop_layer);
    }

    new_prop_layer->set_focus_layer_name(p_layer_name);
    new_prop_layer->set_parent_control(p_parent_control);
    new_prop_layer->set_focused_control(p_focused_control);
    new_prop_layer->set_back_button(back_btn);
    new_prop_layer->set_should_loop_vertically(p_should_loop_vertically);

	if (p_select_layer)
		set_focus_layer(p_layer_name);
}

Control *BG_Focus_Layers::get_current_parent_control() const
{
    const BG_Focus_Layer_Properties *prop = get_active_focus_layer_const();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(prop))
        return nullptr;
    return prop->get_parent_control();
}

/* static */ bool BG_Focus_Layers::_check_if_valid_control(const Control *c)
{
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(c) && c->is_visible_in_tree() && c->get_mouse_filter() != Control::MouseFilter::MOUSE_FILTER_IGNORE)
    {
        const Button *btn = cast_to<Button>(c);
        if (!BG_Focus_Layer_Properties::bg_is_instance_valid(btn) || !btn->is_disabled())
            return true;
    }
    return false;
}


bool BG_Focus_Layers::_is_control_top(const Vector2 &direction, const Control *ctrl, const TypedArray<Control> &all_ctrls)
{
    const Vector2 &ctrl_pos = ctrl->get_screen_transform().get_origin();
    const float c_top_y = ctrl_pos.y;
    for (int i = 0; i < all_ctrls.size(); i++)
    {
        const Control *child = cast_to<Control>(all_ctrls[i]);
        const Vector2 &child_ctrl_pos = child->get_screen_transform().get_origin();
        if (c_top_y > child_ctrl_pos.y && _check_if_valid_control(child))
        {
            const Vector2 c_center_location = ctrl_pos + (ctrl->get_global_rect().size * 0.5);
            const Vector2 child_center_location = child_ctrl_pos + (child->get_global_rect().size * 0.5);
            static const float PI = 3.14159f;
            const float angle = Math::abs(direction.angle_to(child_center_location - c_center_location) / PI);
            if (Math::abs(angle) < _minimum_angle)
                return false;
        }
    }
    return true;
}
 
bool BG_Focus_Layers::_is_control_bottom(const Vector2 &direction, const Control *ctrl, const TypedArray<Control> &all_ctrls)
{
    const Vector2 &ctrl_pos = ctrl->get_screen_transform().get_origin();
    const float c_bottom_y = ctrl_pos.y + ctrl->get_global_rect().size.y;
    for (int i = 0; i < all_ctrls.size(); i++)
    {
        const Control *child = cast_to<Control>(all_ctrls[i]);
        const Vector2 &child_ctrl_pos = child->get_screen_transform().get_origin();
        if (c_bottom_y < (child_ctrl_pos.y + child->get_global_rect().size.y) && _check_if_valid_control(child))
        {
            const Vector2 c_center_location = ctrl_pos + (ctrl->get_global_rect().size * 0.5);
            const Vector2 child_center_location = child_ctrl_pos + (child->get_global_rect().size * 0.5);
            static const float PI = 3.14159f;
            const float angle = Math::abs(direction.angle_to(child_center_location - c_center_location) / PI);
            if (Math::abs(angle) < _minimum_angle)
                return false;
        }
    }
    return true;
}

void BG_Focus_Layers::find_control_in_direction(Vector2 direction)
{
    BG_Focus_Layer_Properties *prop = get_active_focus_layer();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(prop))
        return;
    
    const TypedArray<Control> ctrls = BG_Focus_Layers::get_all_focusable_controls_under_control(prop->get_parent_control());
	Control *last_selected_control = _get_active_control();
	if (BG_Focus_Layer_Properties::bg_is_instance_valid(last_selected_control))
    {
		bool should_get_farthest_control = false;
		Vector2 last_control_dir_location = Vector2();
		if (direction == Vector2(0, -1))
        {
			last_control_dir_location = last_selected_control->get_screen_transform().get_origin() + (last_selected_control->get_global_rect().size * Vector2(0.5, 0.0));
			if (prop->get_should_loop_vertically())
				should_get_farthest_control = _is_control_top(direction, last_selected_control, ctrls);
        } else if (direction == Vector2(0, 1))
        {
			if (prop->get_should_loop_vertically())
				should_get_farthest_control = _is_control_bottom(direction, last_selected_control, ctrls);
			last_control_dir_location = last_selected_control->get_screen_transform().get_origin() + (last_selected_control->get_global_rect().size * Vector2(0.5, 1.0));
        } else if (direction == Vector2(1, 0))
        {
			last_control_dir_location = last_selected_control->get_screen_transform().get_origin() + (last_selected_control->get_global_rect().size * Vector2(1.0, 0.5));
        } else
        {
			last_control_dir_location = last_selected_control->get_screen_transform().get_origin() + (last_selected_control->get_global_rect().size * Vector2(0.0, 0.5));
        }

		Control *new_selected_control = nullptr;
		float best_distance = should_get_farthest_control ? 0.0f : 9999.0f;
        for (int i = 0; i < ctrls.size(); i++)
        {
            Control *c = cast_to<Control>(ctrls[i]);
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(c) && c != last_selected_control && _check_if_valid_control(c))
            {
                const Vector2 c_center_location = c->get_screen_transform().get_origin() + (c->get_global_rect().size * 0.5);
                static const float PI = 3.14159f;
                const float angle = Math::abs(direction.angle_to(last_control_dir_location - c_center_location) / PI);
                if ((should_get_farthest_control && angle < _minimum_angle) || (!should_get_farthest_control && angle > _minimum_angle))
                {
                    float distance = last_control_dir_location.distance_to(c_center_location);
                    if (!should_get_farthest_control)
                        distance *= Math::max(1.0f, (Math::abs(angle - 1.0f) + 0.75f) ); // Favor controls in a better angle.
                    else
                        distance *= Math::abs(angle - 1.0) - _minimum_angle; // Favor controls in a better angle.

                    if ((should_get_farthest_control && distance > best_distance) || (!should_get_farthest_control && distance < best_distance))
                    {
                        best_distance = distance;
                        new_selected_control = c;
                    }
                }
            }
        }

		if (BG_Focus_Layer_Properties::bg_is_instance_valid(new_selected_control))
            prop->set_focused_control(new_selected_control);
    }

	if (last_selected_control != prop->get_focused_control())
    {
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(last_selected_control) && last_selected_control->has_focus())
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
        if (!_focus_layer_stack.is_empty() && BG_Focus_Layer_Properties::bg_is_instance_valid(_get_active_control()))
            _get_active_control()->release_focus();
    }

    Button *btn = _get_active_back_button();
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(btn))
        btn->set_visible(!_is_using_gamepad);
}

void BG_Focus_Layers::press_back_button() const
{
    Button *btn = _get_active_back_button();
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(btn))
        btn->emit_signal("pressed");
}

/* static */ Control *BG_Focus_Layers::find_valid_control(const TypedArray<Control> &p_controls)
{
    for (int i = 0; i < p_controls.size(); i++)
    {
        Control *ctrl = cast_to<Control>(p_controls[i]);
        if (BG_Focus_Layers::_check_if_valid_control(ctrl))
            return ctrl;
    }
    return nullptr;
}

/* static */ TypedArray<Control> BG_Focus_Layers::get_all_focusable_controls_under_control(const Control *p_control)
{
    TypedArray<Control> result;
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(p_control))
    {
        const TypedArray<Node> children = p_control->get_children();
        for (int i = 0; i < children.size(); i++)
        {
            const Control *ctrl = cast_to<Control>(children[i]);
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(ctrl))
            {
                if (ctrl->get_focus_mode() != Control::FocusMode::FOCUS_NONE) {
                    result.append(ctrl);
                }
            }
            else {
                const SubViewport *sub_viewport = cast_to<SubViewport>(children[i]);
                if (BG_Focus_Layer_Properties::bg_is_instance_valid(sub_viewport)) {
                    const TypedArray<Node> s_children = sub_viewport->get_children();
                    for (int si = 0; si < s_children.size(); si++)
                    {
                        result.append_array(BG_Focus_Layers::get_all_focusable_controls_under_control(cast_to<Control>(s_children[si])));
                    }
                }
            }
            result.append_array(BG_Focus_Layers::get_all_focusable_controls_under_control(ctrl));
        }
    }
    return result;
}

/* static */ TypedArray<Control> BG_Focus_Layers::get_current_focusable_controls_under_control(const Control *p_control)
{
    TypedArray<Control> result;
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(p_control))
    {
        const TypedArray<Node> children = p_control->get_children();
        for (int i = 0; i < children.size(); i++)
        {
            const Control *ctrl = cast_to<Control>(children[i]);
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(ctrl)) {
                if (_check_if_valid_control(ctrl) && ctrl->get_focus_mode() != Control::FocusMode::FOCUS_NONE)
                {
                    result.append(ctrl);
                }
            }
            else {
                const SubViewport *sub_viewport = cast_to<SubViewport>(children[i]);
                if (BG_Focus_Layer_Properties::bg_is_instance_valid(sub_viewport)) {
                    const TypedArray<Node> s_children = sub_viewport->get_children();
                    for (int si = 0; si < s_children.size(); si++)
                    {
                        result.append_array(BG_Focus_Layers::get_current_focusable_controls_under_control(cast_to<Control>(s_children[si])));
                    }
                }
            }
            result.append_array(BG_Focus_Layers::get_current_focusable_controls_under_control(ctrl));
        }
    }
    return result;
}
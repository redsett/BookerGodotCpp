#include "bg_focus_layers.hpp"

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/node_path.hpp>

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
    for (int i = 0; i < p_focus_layers.size(); ++i) {
        const BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(p_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop) && prop->get_focus_layer_name() == p_focus_layer_name) {
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
	ClassDB::bind_method(D_METHOD("preview_direction", "direction"), &BG_Focus_Layers::preview_direction);
	ClassDB::bind_method(D_METHOD("set_search_settings", "settings"), &BG_Focus_Layers::set_search_settings);
	ClassDB::bind_method(D_METHOD("get_search_settings"), &BG_Focus_Layers::get_search_settings);
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

	for (int i = 0; i < _focus_layers.size(); ++i) {
		BG_Focus_Layer_Properties *d = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
		if (d != nullptr)
			memdelete(d);
	}
}

void BG_Focus_Layers::try_set_focused_control(Control *p_ctrl)
{
	if (_focus_layer_stack.is_empty())
		return;
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(p_ctrl)) {
        // UtilityFunctions::print("ERROR: BG_Focus_Layers::try_set_focused_control() Trying to set focus on an invalid object.");
        return;
    }

    for (int i = 0; i < _focus_layers.size(); ++i)
    {
        BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop))
        {
            const TypedArray<Control> layer_controls = BG_Focus_Layers::get_all_focusable_controls_under_control(prop->get_parent_control());
            if (layer_controls.has(p_ctrl))
            {
                prop->set_focused_control(p_ctrl);
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
    const bool was_in_stack = _focus_layer_stack.has(p_layer_name);
	if (was_in_stack) {
        _focus_layer_stack.erase(p_layer_name);
    } else if (!p_full_remove) {
        return;
    }

	// A layer can exist without being on the stack (added with select_layer = false), so a full remove still deletes it.
	if (p_full_remove) {
        for (int i = 0; i < _focus_layers.size(); ++i)
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
	if (was_in_stack && !_focus_layer_stack.is_empty()) {
		_focus_active_control();
    }
}

BG_Focus_Layer_Properties *BG_Focus_Layers::get_active_focus_layer() const
{
    if (_focus_layer_stack.is_empty())
        return nullptr;
    
    for (int i = 0; i < _focus_layers.size(); ++i)
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
        for (int i = 0; i < focusable_ctrls.size(); ++i) {
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
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(control_to_focus)) {
        return;
    }
    if (control_to_focus->get_focus_mode() == Control::FOCUS_NONE) {
        control_to_focus->get_viewport()->gui_release_focus();
        // UtilityFunctions::prints("Released focus on: ", control_to_focus->get_name());
        return;
    }
    
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
    Button *back_btn = cast_to<Button>(p_back_button);
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(back_btn))
        back_btn->set_visible(!_is_using_gamepad);


    BG_Focus_Layer_Properties *new_prop_layer = nullptr;

    // Reuse the existing focus layer if it already exists.
    if (BG_Focus_Layer_Properties::has_focus_layer(_focus_layers, p_layer_name))
    {
        for (int i = 0; i < _focus_layers.size(); ++i)
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


/* static */ BG_Focus_Layers::ScreenRect BG_Focus_Layers::_get_screen_rect(const Control *c)
{
    // The screen transform covers the window scale, rotated/scaled controls and SubViewports, so every rect is in the same space.
    const Transform2D xform = c->get_screen_transform();
    const Vector2 size = c->get_size();
    Rect2 aabb(xform.xform(Vector2()), Vector2());
    aabb = aabb.expand(xform.xform(Vector2(size.x, 0.0f)));
    aabb = aabb.expand(xform.xform(Vector2(0.0f, size.y)));
    aabb = aabb.expand(xform.xform(size));
    return { aabb, xform.xform(size * 0.5f) };
}

/* static */ int BG_Focus_Layers::_get_side(const Vector2 &dir)
{
    if (Math::is_zero_approx(dir.x))
        return dir.y < 0.0f ? godot::Side::SIDE_TOP : godot::Side::SIDE_BOTTOM;
    if (Math::is_zero_approx(dir.y))
        return dir.x < 0.0f ? godot::Side::SIDE_LEFT : godot::Side::SIDE_RIGHT;
    return -1; // Diagonal.
}

/* static */ bool BG_Focus_Layers::_overlaps_sideways(const Rect2 &a, const Rect2 &b, const Vector2 &dir)
{
    if (Math::is_zero_approx(dir.x))
        return a.position.x < b.get_end().x && b.position.x < a.get_end().x;
    return a.position.y < b.get_end().y && b.position.y < a.get_end().y;
}

// Finds the control a press in `dir` (normalized, any angle) goes to from `from`, or nullptr.
// 1. A manual focus neighbor wins for the four straight directions. One pointing at the control itself blocks that direction;
//    a hidden or disabled one is skipped.
// 2. Otherwise the best control in front: score = forward distance + sideways distance * side weight (sideways counts less
//    when the rects overlap sideways, i.e. the same row or column). Anything within the max angle counts, however far.
// 3. Nothing in front and the layer loops vertically: up/down wrap to the farthest control the other way.
// `report` (optional) gets every step for the debug UI. BG_FocusLayers.gd mirrors this function.
Control *BG_Focus_Layers::_find_in_direction(BG_Focus_Layer_Properties *prop, Control *from, const Vector2 &dir, Dictionary *report)
{
    const ScreenRect from_rect = _get_screen_rect(from);
    Array candidates;
    if (report != nullptr) {
        (*report)["direction"] = dir;
        (*report)["source"] = from;
        (*report)["source_rect"] = from_rect.aabb;
        (*report)["manual_neighbor"] = Variant();
        (*report)["blocked"] = false;
        (*report)["wrapped"] = false;
        (*report)["candidates"] = candidates;
        (*report)["result"] = Variant();
    }

    const int side = _get_side(dir);
    if (side >= 0) {
        const NodePath neighbor_path = from->get_focus_neighbor(godot::Side(side));
        if (!neighbor_path.is_empty()) {
            Control *neighbor = cast_to<Control>(from->get_node_or_null(neighbor_path));
            if (neighbor == from) {
                if (report != nullptr)
                    (*report)["blocked"] = true;
                return nullptr;
            }
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(neighbor) && _check_if_valid_control(neighbor)) {
                if (report != nullptr) {
                    (*report)["manual_neighbor"] = neighbor;
                    (*report)["result"] = neighbor;
                }
                return neighbor;
            }
        }
    }

    const bool is_straight = side >= 0;
    const float max_angle = Math::deg_to_rad(is_straight ? _max_angle_degrees : _diagonal_max_angle_degrees);
    constexpr float PI_F = 3.14159265f;
    const Vector2 side_dir = Vector2(-dir.y, dir.x);
    const bool can_wrap = prop->get_should_loop_vertically() && Math::is_zero_approx(dir.x);

    Control *best = nullptr;
    float best_score = 0.0f;
    Control *wrap_best = nullptr;
    float wrap_best_score = 0.0f;
    const TypedArray<Control> ctrls = BG_Focus_Layers::get_all_focusable_controls_under_control(prop->get_parent_control());
    for (int i = 0; i < ctrls.size(); ++i)
    {
        Control *c = cast_to<Control>(ctrls[i]);
        if (c == from || !_check_if_valid_control(c))
            continue;

        const ScreenRect rect = _get_screen_rect(c);
        const Vector2 offset = rect.center - from_rect.center;
        const float forward = offset.dot(dir);
        float sideways = Math::abs(offset.dot(side_dir));
        const float angle = Math::atan2(sideways, forward); // 0 = straight ahead, PI = straight behind.
        if (is_straight && _overlaps_sideways(from_rect.aabb, rect.aabb, dir))
            sideways *= _aligned_side_factor;
        const float score = forward + sideways * _side_weight;
        const bool accepted = forward > 0.0f && angle <= max_angle;

        if (accepted && (best == nullptr || score < best_score)) {
            best = c;
            best_score = score;
        }
        // Wrapping: the farthest control behind, still favoring the same column.
        if (can_wrap && forward < 0.0f && angle >= PI_F - max_angle && (wrap_best == nullptr || score < wrap_best_score)) {
            wrap_best = c;
            wrap_best_score = score;
        }

        if (report != nullptr) {
            Dictionary entry;
            entry["control"] = c;
            entry["rect"] = rect.aabb;
            entry["center"] = rect.center;
            entry["forward"] = forward;
            entry["side"] = sideways;
            entry["angle"] = Math::rad_to_deg(angle);
            entry["score"] = score;
            entry["accepted"] = accepted;
            candidates.append(entry);
        }
    }

    if (best == nullptr && wrap_best != nullptr) {
        best = wrap_best;
        if (report != nullptr)
            (*report)["wrapped"] = true;
    }
    if (report != nullptr)
        (*report)["result"] = best;
    return best;
}

void BG_Focus_Layers::find_control_in_direction(const Vector2 &direction)
{
    BG_Focus_Layer_Properties *prop = get_active_focus_layer();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(prop) || direction.is_zero_approx())
        return;

    Control *last_selected_control = _get_active_control();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(last_selected_control))
        return;

    Control *new_selected_control = _find_in_direction(prop, last_selected_control, direction.normalized(), nullptr);
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(new_selected_control) && new_selected_control != last_selected_control)
    {
        prop->set_focused_control(new_selected_control);
        if (last_selected_control->has_focus())
            last_selected_control->release_focus();
        _focus_active_control();
    }
}

// What a press in `direction` would do, without moving focus (for the debug UI). Empty when nothing is focused.
Dictionary BG_Focus_Layers::preview_direction(const Vector2 &direction)
{
    Dictionary report;
    BG_Focus_Layer_Properties *prop = get_active_focus_layer();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(prop) || direction.is_zero_approx())
        return report;
    Control *from = prop->get_focused_control();
    if (!BG_Focus_Layer_Properties::bg_is_instance_valid(from) || !_check_if_valid_control(from))
        return report;
    _find_in_direction(prop, from, direction.normalized(), &report);
    return report;
}

void BG_Focus_Layers::set_search_settings(const Dictionary &p_settings)
{
    _max_angle_degrees = p_settings.get("max_angle_degrees", _max_angle_degrees);
    _diagonal_max_angle_degrees = p_settings.get("diagonal_max_angle_degrees", _diagonal_max_angle_degrees);
    _side_weight = p_settings.get("side_weight", _side_weight);
    _aligned_side_factor = p_settings.get("aligned_side_factor", _aligned_side_factor);
}

Dictionary BG_Focus_Layers::get_search_settings() const
{
    Dictionary settings;
    settings["max_angle_degrees"] = _max_angle_degrees;
    settings["diagonal_max_angle_degrees"] = _diagonal_max_angle_degrees;
    settings["side_weight"] = _side_weight;
    settings["aligned_side_factor"] = _aligned_side_factor;
    return settings;
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

    for (int i = 0; i < _focus_layers.size(); ++i)
    {
        BG_Focus_Layer_Properties *prop = cast_to<BG_Focus_Layer_Properties>(_focus_layers[i]);
        if (BG_Focus_Layer_Properties::bg_is_instance_valid(prop)) {
            Button *btn = prop->get_back_button();
            if (BG_Focus_Layer_Properties::bg_is_instance_valid(btn)) {
                btn->set_visible(!_is_using_gamepad);
            }
        }
    }
}

void BG_Focus_Layers::press_back_button() const
{
    Button *btn = _get_active_back_button();
    if (BG_Focus_Layer_Properties::bg_is_instance_valid(btn))
        btn->emit_signal("pressed");
}

/* static */ Control *BG_Focus_Layers::find_valid_control(const TypedArray<Control> &p_controls)
{
    for (int i = 0; i < p_controls.size(); ++i) {
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
        for (int i = 0; i < children.size(); ++i)
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
                    for (int si = 0; si < s_children.size(); ++si)
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
        for (int i = 0; i < children.size(); ++i)
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
                    for (int si = 0; si < s_children.size(); ++si)
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
#include "gsg_state_machine.hpp"

#include "gsg_state.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GSGStateMachine::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("transition_to", "state_name", "msg"), &GSGStateMachine::transition_to);

	ClassDB::bind_method(D_METHOD("set_initial_state", "initial_state"), &GSGStateMachine::set_initial_state);
	ClassDB::bind_method(D_METHOD("get_initial_state"), &GSGStateMachine::get_initial_state);
	
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "initial_state", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "GSGState"), "set_initial_state", "get_initial_state");

	ADD_SIGNAL(MethodInfo("transitioned", PropertyInfo(Variant::STRING, "state_name")));
}

GSGStateMachine::GSGStateMachine()
{
}

GSGStateMachine::~GSGStateMachine()
{
}

void GSGStateMachine::_ready()
{
	// get_owner()->connect("ready", Callable(this, "_ready"));
	//yield(owner, "ready")

	// The state machine assigns itself to the State objects' state_machine property.
	for (int i = 0; i < get_child_count(); ++i) {
		GSGState *s = Object::cast_to<GSGState>(get_child(i));
		if (s) {
			s->set_state_machine(this);
		}
	}

	state = get_node<GSGState>(initial_state);
	if (state) {
		state->enter();
	}
}

void GSGStateMachine::_unhandled_input(const Ref<InputEvent> &p_event)
{
	if (state) {
		state->handle_input(p_event);
	}
}

void GSGStateMachine::_process(double p_delta)
{
	if (state) {
		state->update(p_delta);
	}
}

void GSGStateMachine::_physics_process(double p_delta)
{
	if (state) {
		state->physics_update(p_delta);
	}
}

void GSGStateMachine::transition_to(String p_state_name, Dictionary p_msg)
{
	GSGState *new_state = get_node<GSGState>(p_state_name);
	if (!new_state) {
		return;
	}

	if (state) {
		state->exit();
	}
	state = new_state;
	state->enter(p_msg);
	emit_signal("transitioned", p_state_name);
}
#include "gsg_state.hpp"

#include "gsg_state_machine.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GSGState::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("handle_input", "event"), &GSGState::handle_input);
	ClassDB::bind_method(D_METHOD("update"), &GSGState::update);
	ClassDB::bind_method(D_METHOD("physics_update", "delta"), &GSGState::physics_update);
	ClassDB::bind_method(D_METHOD("enter", "msg"), &GSGState::enter);
	ClassDB::bind_method(D_METHOD("exit"), &GSGState::exit);

	ClassDB::bind_method(D_METHOD("set_state_machine", "state_machine"), &GSGState::set_state_machine);
	ClassDB::bind_method(D_METHOD("get_state_machine"), &GSGState::get_state_machine);
}

GSGState::GSGState()
{
}

GSGState::~GSGState()
{
}
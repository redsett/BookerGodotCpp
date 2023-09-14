#ifndef GSGSTATE_H
#define GSGSTATE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/core/class_db.hpp>

class GSGStateMachine;

using namespace godot;

class GSGState : public Node
{
	GDCLASS(GSGState, Node);

protected:
	static void _bind_methods();

public:
	GSGState();
	~GSGState();

	GSGStateMachine *state_machine = nullptr;
	void set_state_machine(GSGStateMachine *p_state_machine) { state_machine = p_state_machine; }
	GSGStateMachine *get_state_machine() const { return state_machine; }

	virtual void handle_input(const Ref<InputEvent> &p_event) {}

	virtual void update(float p_delta) {}

	virtual void physics_update(float p_delta) {}

	virtual void enter(Dictionary p_msg = {}) {}

	virtual void exit() {}
};

#endif // GSGSTATE_H
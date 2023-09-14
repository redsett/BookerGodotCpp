#ifndef GSGSTATEMACHINE_H
#define GSGSTATEMACHINE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref.hpp>

class GSGState;

using namespace godot;

class GSGStateMachine : public Node
{
	GDCLASS(GSGStateMachine, Node);

protected:
	static void _bind_methods();

public:
	GSGStateMachine();
	~GSGStateMachine();

	StringName transitioned(String p_state_name);

	NodePath initial_state;
	void set_initial_state(const NodePath &p_initial_state) { initial_state = p_initial_state; }
	NodePath get_initial_state() const { return initial_state; }

	GSGState *state = nullptr;

	void _ready() override;

	void _unhandled_input(const Ref<InputEvent> &p_event) override;

	void _process(double p_delta) override;

	void _physics_process(double p_delta) override;

	void transition_to(String p_state_name, Dictionary p_msg = {});
};

#endif // GSGSTATEMACHINE_H
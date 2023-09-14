#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class CardToMouseSpline : public Control
{
	GDCLASS(CardToMouseSpline, Control);

protected:
	static void _bind_methods();

public:
	CardToMouseSpline();
	~CardToMouseSpline();

	void _ready() override;
	void _process(double p_delta) override;

	void _draw() override;

	Color spline_color = Color(50, 50, 255);
	void set_spline_color(Color p_spline_color)  { spline_color = p_spline_color; }
	Color get_spline_color() { return spline_color; }

	int spline_width = 4;
	void set_spline_width(int p_spline_width) { spline_width = p_spline_width; }
	int get_spline_width() { return spline_width; }

	void draw_spline(Vector2 p_starting_pos, Vector2 p_ending_pos);

	void clear_spline();

private:
	Vector2 starting_pos = Vector2(0, 0);
	Vector2 ending_pos = Vector2(0, 0);
};

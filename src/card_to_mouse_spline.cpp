#include "card_to_mouse_spline.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void CardToMouseSpline::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_spline_color", "color"), &CardToMouseSpline::set_spline_color);
	ClassDB::bind_method(D_METHOD("get_spline_color"), &CardToMouseSpline::get_spline_color);
	ClassDB::bind_method(D_METHOD("set_spline_width", "width"), &CardToMouseSpline::set_spline_width);
	ClassDB::bind_method(D_METHOD("get_spline_width"), &CardToMouseSpline::get_spline_width);

	ClassDB::bind_method(D_METHOD("draw_spline", "starting_pos", "ending_pos"), &CardToMouseSpline::draw_spline);
	ClassDB::bind_method(D_METHOD("clear_spline"), &CardToMouseSpline::clear_spline);
	
	ADD_GROUP("Bread", "");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "spline_color"), "set_spline_color", "get_spline_color");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "spline_width"), "set_spline_width", "get_spline_width");
}

CardToMouseSpline::CardToMouseSpline()
{
}

CardToMouseSpline::~CardToMouseSpline()
{
}

void CardToMouseSpline::_ready()
{
}

void CardToMouseSpline::_process(double p_delta)
{
}

void CardToMouseSpline::draw_spline(Vector2 p_starting_pos, Vector2 p_ending_pos)
{
	starting_pos = p_starting_pos;
	ending_pos = p_ending_pos;
	queue_redraw();
}

void CardToMouseSpline::clear_spline()
{
	starting_pos = Vector2(-100, -100);
	ending_pos = starting_pos;
	queue_redraw();
}

inline float _remap_range(float p_value, float p_range_min, float p_range_max, float p_output_min, float p_output_max) {
	return (p_value - p_range_min) / (p_range_max - p_range_min) * (p_output_max - p_output_min) + p_output_min;
}

void CardToMouseSpline::_draw()
{
	float alpha = 0.0f;
	const Vector2 endPos_2 = Vector2(starting_pos.x, ending_pos.y * -0.1f);
	Vector2 new_start_pos = starting_pos;
	Vector2 new_end_pos = starting_pos;

	const float increment_amount = MAX(0.01f, _remap_range((starting_pos - ending_pos).length(), 0.0f, 1000.0f, 0.05f, 0.01f));

	bool skip = false;
	while (alpha <= 1.0f) {
		new_start_pos = new_end_pos;
		
		const float alphaReversed = abs(alpha - 1);
		new_end_pos = (alphaReversed * starting_pos) + (alpha * ((alphaReversed * endPos_2) + (alpha * ending_pos)));
		alpha += increment_amount;
		
		if (skip) {
			skip = false;
			continue;
		}
		skip = true;
		draw_line(new_start_pos, new_end_pos, spline_color, spline_width);
	}
	
	draw_line(new_end_pos, ending_pos, spline_color, spline_width);
}
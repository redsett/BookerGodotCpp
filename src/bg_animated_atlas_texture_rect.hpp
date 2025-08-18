#pragma once

#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class BG_AnimatedAtlasTextureRect : public TextureRect
{
	GDCLASS(BG_AnimatedAtlasTextureRect, TextureRect);

protected:
	static void _bind_methods();

	float time_on_frame = 0.0;

	int current_frame = 0;

public:
	void _ready() override;
	void _process(double delta) override;
	
	Ref<Texture2D> atlas_texture;
	Ref<Texture2D> get_atlas_texture() const { return atlas_texture; }
	void set_atlas_texture(Ref<Texture2D> value) { atlas_texture = value; }
	
	Vector2 frame_size = Vector2(256.0, 256.0);
	Vector2 get_frame_size() const { return frame_size; }
	void set_frame_size(Vector2 value) { frame_size = value; }
	
	int frame_count = 15;
	int get_frame_count() const { return frame_count; }
	void set_frame_count(int value) { frame_count = value; }
	
	float anim_fps = 60.0;
	float get_anim_fps() const { return anim_fps; }
	void set_anim_fps(float value) { anim_fps = value; }

	Rect2 texture_margin;
	Rect2 get_texture_margin() const { return texture_margin; }
	void set_texture_margin(Rect2 value) { texture_margin = value; }

	bool randomize_starting_frame = false;
	bool get_randomize_starting_frame() const { return randomize_starting_frame; }
	void set_randomize_starting_frame(bool value) { randomize_starting_frame = value; }

	bool randomize_mirror_h = false;
	bool get_randomize_mirror_h() const { return randomize_mirror_h; }
	void set_randomize_mirror_h(bool value) { randomize_mirror_h = value; }

	float randomize_anim_speed_range = 0.0;
	float get_randomize_anim_speed_range() const { return randomize_anim_speed_range; }
	void set_randomize_anim_speed_range(float value) { randomize_anim_speed_range = value; }
};
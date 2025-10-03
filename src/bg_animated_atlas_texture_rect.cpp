#include "bg_animated_atlas_texture_rect.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

////
//// BG_AnimatedAtlasTextureRect
////
void BG_AnimatedAtlasTextureRect::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_atlas_texture"), &BG_AnimatedAtlasTextureRect::get_atlas_texture);
	ClassDB::bind_method(D_METHOD("set_atlas_texture"), &BG_AnimatedAtlasTextureRect::set_atlas_texture);
	ClassDB::bind_method(D_METHOD("get_frame_size"), &BG_AnimatedAtlasTextureRect::get_frame_size);
	ClassDB::bind_method(D_METHOD("set_frame_size"), &BG_AnimatedAtlasTextureRect::set_frame_size);
	ClassDB::bind_method(D_METHOD("get_frame_count"), &BG_AnimatedAtlasTextureRect::get_frame_count);
	ClassDB::bind_method(D_METHOD("set_frame_count"), &BG_AnimatedAtlasTextureRect::set_frame_count);
	ClassDB::bind_method(D_METHOD("get_anim_fps"), &BG_AnimatedAtlasTextureRect::get_anim_fps);
	ClassDB::bind_method(D_METHOD("set_anim_fps"), &BG_AnimatedAtlasTextureRect::set_anim_fps);
	ClassDB::bind_method(D_METHOD("get_texture_margin"), &BG_AnimatedAtlasTextureRect::get_texture_margin);
	ClassDB::bind_method(D_METHOD("set_texture_margin"), &BG_AnimatedAtlasTextureRect::set_texture_margin);
	ClassDB::bind_method(D_METHOD("get_randomize_starting_frame"), &BG_AnimatedAtlasTextureRect::get_randomize_starting_frame);
	ClassDB::bind_method(D_METHOD("set_randomize_starting_frame"), &BG_AnimatedAtlasTextureRect::set_randomize_starting_frame);
	ClassDB::bind_method(D_METHOD("get_randomize_mirror_h"), &BG_AnimatedAtlasTextureRect::get_randomize_mirror_h);
	ClassDB::bind_method(D_METHOD("set_randomize_mirror_h"), &BG_AnimatedAtlasTextureRect::set_randomize_mirror_h);
	ClassDB::bind_method(D_METHOD("get_randomize_anim_speed_range"), &BG_AnimatedAtlasTextureRect::get_randomize_anim_speed_range);
	ClassDB::bind_method(D_METHOD("set_randomize_anim_speed_range"), &BG_AnimatedAtlasTextureRect::set_randomize_anim_speed_range);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "atlas_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_atlas_texture", "get_atlas_texture");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "frame_size"), "set_frame_size", "get_frame_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "frame_count"), "set_frame_count", "get_frame_count");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "anim_fps"), "set_anim_fps", "get_anim_fps");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2, "texture_margin"), "set_texture_margin", "get_texture_margin");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "randomize_starting_frame"), "set_randomize_starting_frame", "get_randomize_starting_frame");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "randomize_mirror_h"), "set_randomize_mirror_h", "get_randomize_mirror_h");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "randomize_anim_speed_range"), "set_randomize_anim_speed_range", "get_randomize_anim_speed_range");
}

void BG_AnimatedAtlasTextureRect::_ready()
{
	if (!get_texture().is_valid() || !atlas_texture.is_valid()) return;

	AtlasTexture *at = cast_to<AtlasTexture>(get_texture().ptr());
	if (at == nullptr) return;
	at->set_atlas(atlas_texture);
	at->set_region(Rect2(0, 0, frame_size.x, frame_size.y));
	at->set_margin(texture_margin);
	if (randomize_starting_frame) {
		current_frame = UtilityFunctions::randi_range(0, frame_count - 1);
	}
	if (randomize_mirror_h) {
		set_flip_h(bool(UtilityFunctions::randi_range(0, 1)));
	}
	if (!Engine::get_singleton()->is_editor_hint()) {
		anim_fps = UtilityFunctions::randf_range(anim_fps - randomize_anim_speed_range, anim_fps + randomize_anim_speed_range);
	}
}

void BG_AnimatedAtlasTextureRect::_process(double delta)
{
	if (!get_texture().is_valid() || !atlas_texture.is_valid()) return;
	if (!is_visible_in_tree()) return;

	time_on_frame += (delta * anim_fps);
	if (time_on_frame < 1.0) return;
	time_on_frame -= 1.0;
	current_frame = (current_frame + 1) % frame_count;
	
	Vector2 frame_location = Vector2(frame_size.x * current_frame, 0.0);
	const int v_count = int(Math::floor(frame_location.x / atlas_texture->get_size().x));
	frame_location.x -= (atlas_texture->get_size().x * v_count);
	frame_location.y = frame_size.y * v_count;
	
	AtlasTexture *at = cast_to<AtlasTexture>(get_texture().ptr());
	if (at == nullptr) return;
	at->set_region(Rect2(frame_location.x, frame_location.y, frame_size.x, frame_size.y));
}
#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class BG_JsonUtils : public Object
{
	GDCLASS(BG_JsonUtils, Object);

protected:
	static void _bind_methods();

public:
	static Dictionary ParseJsonFile(const String &file_path);

	static Dictionary GetCBDSheet(const Dictionary &data, const String &sheet_name);
};
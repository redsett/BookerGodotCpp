#include "global_string.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// using namespace godot;

// void Global_String::_bind_methods()
// {
// 	ClassDB::bind_method(D_METHOD("get_value"), &Global_String::get_value);
// }

// Global_String::Global_String()
// {
// }

// /* static */ Global_String *Global_String::get(String value)
// {
// 	for (int i = 0; i < all_global_strings.size(); i++)
// 	{
// 		Global_String *entry = all_global_strings[i];
// 		if (entry->value == value)
// 		{
// 			return entry;
// 		}
// 	}

// 	Global_String *new_global_string = memnew(Global_String);
// 	new_global_string->value = value;
// 	all_global_strings.append(new_global_string);
// 	return new_global_string;
// }

// /* static */ int Global_String::get_count()
// {
// 	return all_global_strings.size();
// }

// Global_String::~Global_String()
// {
// }
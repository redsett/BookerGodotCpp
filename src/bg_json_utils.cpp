#include "bg_json_utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>

using namespace godot;

/* static */ Dictionary BG_JsonUtils::ParseJsonCBDDataFile(const String &relative_file_path)
{
	return ParseJsonFile("res://Data/" + relative_file_path + ".cdb");
}

/* static */ Dictionary BG_JsonUtils::ParseJsonFile(const String &file_path)
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	ERR_FAIL_NULL_V_EDMSG(file, {}, "Error! Could not open json file : " + file_path);

	String json_string = file->get_as_text();

	// Convert the file data into a dictionary.
	Ref<JSON> json;
	json.instantiate();
	ERR_FAIL_COND_V_EDMSG(json->parse(json_string) != OK, {}, "JSON Parse Error: " + json->get_error_message() + " in " + file_path + " at line " + json->get_error_line());
	
	Variant jsonData = json->get_data();
	ERR_FAIL_COND_V_EDMSG(jsonData.get_type() != Variant::Type::DICTIONARY, {}, "Data is not a dictionary in file : " + file_path);
	return jsonData;
}

/* static */ Dictionary BG_JsonUtils::GetCBDSheet(const Dictionary &data, const String &sheet_name)
{
	Array sheets = data["sheets"];
	for (int i = 0; i < sheets.size(); i++)
	{
		Dictionary sheet = sheets[i];
		if (sheet["name"] == sheet_name)
		{
			return sheet;
		}
	}
	ERR_FAIL_COND_V_EDMSG(false, {}, vformat("Sheet '%s' does not exist in the cbd data : ", sheet_name));
}
#pragma once

// Internal helpers shared by the bg_booker_*.cpp files: reading DBer rows (dber_get, dber_find, sheets, enums),
// colors, timing. Not part of the GDExtension API.

#include "bg_booker_data_base.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

struct counter
{
	double total_time = 0.0f;
	uint32_t total_times = 0;
};


#define TIME_FUNC(func, cont) \
    do { \
        clock_t start, end; \
        double elapsed; \
        start = clock(); \
        func.call(); \
        end = clock(); \
        elapsed = (double)(end - start) / CLOCKS_PER_SEC; \
		cont.total_time += elapsed; \
		cont.total_times++; \
        UtilityFunctions::print("The function '", func.get_method(), "' took ", elapsed, " seconds to execute. Average Time : ", cont.total_time / cont.total_times); \
    } while (0)

inline Color convert_int_to_color(int color_int)
{
	const int r = (color_int >> 16) & 255;
	const int g = (color_int >> 8) & 255;
	const int b = color_int & 255;
	return Color(
		r > 0 ? r/255.0 : 0.0, 
		g > 0 ? g/255.0 : 0.0, 
		b > 0 ? b/255.0 : 0.0
	);
}

inline Color convert_hex_to_color(const String &hex)
{
	return Color(hex);
}

struct fraction_struct
{
	double integral = 0.0f;
	long numerator = 0;
	long denominator = 0;
};

inline long gcd(long a, long b)
{
    if (a == 0)
        return b;
    else if (b == 0)
        return a;

    if (a < b)
        return gcd(a, b % a);
    else
        return gcd(b, a % b);
}

inline fraction_struct get_fract(double input)
{
    double integral = std::floor(input);
    double frac = input - integral;

    const long precision = 1000000000; // This is the accuracy.

    long gcd_ = gcd(round(frac * precision), precision);

    long denominator = precision / gcd_;
    long numerator = round(frac * precision) / gcd_;

	fraction_struct frac_struct = fraction_struct();
	frac_struct.integral = integral;
	frac_struct.numerator = numerator;
	frac_struct.denominator = denominator;
	return frac_struct;
    // std::cout << integral << " + ";
    // std::cout << numerator << " / " << denominator << std::endl;
}

// Reading the DBer data (booker_dber_data.json, written by DBer's StaticFuncs.export_data). It's flat:
// - Rows are Dictionaries, {column name: value}. A content entry's misc params are fields of the entry itself.
// - Values are plain: numbers, strings and bools; enums as their index (see "global_enums" for their names); paths
//   project-relative ("Textures/icon.png"); Vector2 as [x, y]; arrays as lists of rows.
// - Stat references are keyed by the stat's name: {"health": 0.0, "physical_damage": 1.0}.

// A field of a DBer row. A missing field is reported (DBer renamed or removed the column?) and reads as Nil.
inline Variant dber_get(const Dictionary &row, const String &name) {
	const Variant value = row.get(name, Variant());
	if (value.get_type() == Variant::NIL && !row.has(name)) {
		ERR_PRINT("ERROR : The DBer data has no \"" + name + "\" field where it's read. Was the column renamed or removed in DBer?");
	}
	return value;
}

// A field that only some rows have (e.g. misc params only some content types have). Nil when it's missing.
inline Variant dber_find(const Dictionary &row, const String &name) {
	return row.get(name, Variant());
}

// A sheet's rows, {sheet name: [row, ...]} in "sheets".
inline Array get_sheet_by_name(const String &name, const Dictionary &d) {
	const Dictionary sheets = d.get("sheets", Dictionary());
	ERR_FAIL_COND_V_MSG(!sheets.has(name), Array(), "ERROR : The DBer data has no sheet \"" + name + "\".");
	return sheets[name];
}


inline StringName ensure_clean_path(const String &path) {
	return StringName(path.trim_prefix("/"));
}

inline HashMap<String, TypedArray<StringName>> get_global_enums(const Dictionary &data) { // Global Enums: {enum name: [value names]}
	HashMap<String, TypedArray<StringName>> global_enums;
	const Dictionary enums_data = data.get("global_enums", Dictionary());
	const Array enum_names = enums_data.keys();
	for (int i = 0; i < enum_names.size(); ++i) {
		const Array values = enums_data[enum_names[i]];
		TypedArray<StringName> enums;
		for (int y = 0; y < values.size(); ++y) {
			enums.append(StringName(values[y]));
		}
		global_enums[enum_names[i]] = enums;
	}
	return global_enums;
}

// An animation row (Animation_Classes sheet, item and monster animations) as a BG_AnimationDetails.
inline BG_AnimationDetails *get_animation_detail(const HashMap<String, TypedArray<StringName>> &global_enums, const Array &animations, int index) {
	const Dictionary aa = animations[index];
	
	BG_AnimationDetails *new_class = memnew(BG_AnimationDetails);
	new_class->caste_or_monster_id = global_enums["caste_types"][int(dber_get(aa, "caste"))];
	new_class->anim_type = static_cast<BG_AnimationDetails::AnimationType>(int(dber_get(aa, "anim_type")));
	new_class->anim_name = StringName(dber_get(aa, "anim_name"));
	new_class->anim_blend_time = float(dber_get(aa, "anim_blend_time"));
	
	{ // Traverse Types
		const Array anim_traverse_types_array = dber_get(aa, "animation_traverse_types");
		if (!anim_traverse_types_array.is_empty()) {
			const Dictionary anim_traverse_types = anim_traverse_types_array[0];
			new_class->traverse_to = static_cast<BG_AnimationDetails::AnimationTraverseType>(int(dber_get(anim_traverse_types, "traverse_to")));
			new_class->traverse_back = static_cast<BG_AnimationDetails::AnimationTraverseType>(int(dber_get(anim_traverse_types, "traverse_back")));
		}
	}

	{ // Tags
		const Array anim_tags_array = dber_get(aa, "tags");
		for (int i = 0; i < anim_tags_array.size(); ++i) {
			const Dictionary anim_tag_entry = anim_tags_array[i];

			BG_AnimationTagDetails *new_tag_class = memnew(BG_AnimationTagDetails);
			new_tag_class->tag = StringName(dber_get(anim_tag_entry, "tag"));
			new_tag_class->frame = int(dber_get(anim_tag_entry, "frame"));

			new_class->tags.append(new_tag_class);
		}
	}

	return new_class;
}

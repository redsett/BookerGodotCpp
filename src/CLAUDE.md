# Booker C++ (GDExtension)

Data parsing: `C:\Godot\Booker\BookerGodot\Docs\Systems\data-pipeline.md`.

- The user builds the GDExtension. After changing C++, ask them to build, then continue.
- `Booker_DB` parses `booker_dber_data.json`. Read fields with `dber_get(row, "column")` (reports a missing column)
  or `dber_find` (optional); the names must match DBer's columns. Files (read only the one you need):
  - `bg_booker_data_base.hpp`: `BG_Booker_Globals` and `Booker_DB`; include it for any data class. It pulls in
    `bg_booker_types_fwd.hpp` (forward declarations) and the area headers `bg_booker_types_{content,world,units}.hpp`,
    which may only use other areas' classes by pointer, `Ref<>` or `TypedArray<>`. A new class needs a forward
    declaration in `_fwd.hpp`.
  - `bg_booker_data_base.cpp`: `Booker_DB` bindings, getters, refresh, localization lookups.
  - `bg_booker_db_parse.cpp`: `try_parse_bder_data` (calls a `parse_<sheet>` per sheet, in order) and the world
    sheets; `_parse_content.cpp`: stats, items, monsters, effects, rarities; `_parse_units.cpp`: bands, castes, skills.
  - `bg_booker_db_presets.cpp`: preset items, bands, monster groups, drop rewards.
  - `bg_booker_types_{world,content,units}.cpp`: data class methods and bindings.
  - `bg_dber_read.hpp`: shared helpers (`dber_get`, `get_sheet_by_name`, `ensure_clean_path`, colors).
  - A new sheet parser: add a `parse_<sheet>` member (declare it in the header) and call it from `try_parse_bder_data`.
- `bg_attributes_logic.*`: talent tree conditions and states. `bg_hex_grid.cpp`: hex pathfinding.
  `bg_focus_layers.cpp`: gamepad focus (the game's `BG_FocusLayers.gd` mirrors it; change both).
  `register_types.cpp`: class registration.

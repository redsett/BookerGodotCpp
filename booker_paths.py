"""Finds the Booker game project next to this repository.

The game folder's name differs between machines (e.g. `Booker` or `BookerGodot`), so it is found by its contents:
the sibling folder holding `addons/*/booker_godot_cpp.gdextension`. Set the `BOOKER_GAME_DIR` environment variable
to point at the game folder directly when it lives somewhere else.
"""
import os
from glob import glob
from pathlib import Path

EXTENSION_FILE = "booker_godot_cpp.gdextension"
REPO_DIR = Path(__file__).resolve().parent


def find_extension_path() -> Path:
    """Returns the path of the game's .gdextension file, or raises if it isn't found exactly once."""
    override = os.environ.get("BOOKER_GAME_DIR")
    if override:
        pattern = str(Path(override) / "addons" / "*" / EXTENSION_FILE)
    else:
        pattern = str(REPO_DIR.parent / "*" / "addons" / "*" / EXTENSION_FILE)
    matches = sorted(glob(pattern))
    if len(matches) != 1:
        raise RuntimeError(
            "Expected one '{}' matching '{}', found {}: {}. Set BOOKER_GAME_DIR to the game folder.".format(
                EXTENSION_FILE, pattern, len(matches), matches
            )
        )
    return Path(matches[0])


def find_addon_path() -> Path:
    """Returns the game's addon folder that holds the .gdextension file and its bin/ folder."""
    return find_extension_path().parent

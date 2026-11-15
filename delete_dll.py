import os, sys

from booker_paths import find_addon_path

DLL_NAME = "~libbooker_godot_cpp.windows.debug.x86_64.dll"


def main():
    path = find_addon_path() / "bin" / DLL_NAME
    if path.exists():
        os.remove(path)
        sys.stdout.write("Deleted dll : " + str(path))


if __name__ == '__main__':
    main()

import os, sys

def main():
    dllPath = r"..\Booker\addons\BookerGodotCpp\bin\~libbooker_godot_cpp.windows.debug.x86_64.dll"
    for path in ("..\\" + dllPath, dllPath):
        if os.path.exists(path):
            sys.stdout.write("Deleted dll : " + path)
            os.remove(path)
            break

if __name__ == '__main__':
    main()
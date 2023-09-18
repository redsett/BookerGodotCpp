### Expected project structure:
1. `../Booker`
2. `../BookerGodotCpp`

### Getting started:
1. Download and install `Git`, `Scons`, and `Visual Studio Code`
2. Clone this repository (or a new repository with this template) with submodules.
    - Open command prompt and go to the directory ABOVE the Booker project. E.g - Assuming that the Booker project is in C:\Projects : `cd C:\Projects` 
	- `git clone --recurse-submodules https://github.com/redsett/BookerGodotCpp.git`
    - `cd BookerGodotCpp`
3. Build a debug binary for the current platform.
    - `scons`
4. Copy bat files into local_bats and update the Godot exe path to where ever yours is.
    - Copy and paste `open_booker_in_editor.bat` and `play_booker.bat` into the lobal_bats folder.
    - Open each of the bat files that you copied over, then update the Godot .exe paths to yours.
5. Open `../BookerGodotCpp` in Visual Studio Code.
    - You can compile, open the editor, or play the game without running the editor by pressing `ctrl + shift + b` then up/down arrows to select what you want to do.
    - OR you can click the icons at the bottom right of the Visual Studio Code editor.

### Advanced:
To update gotdot-cpp to latest:
1. `git submodule update --remote --merge`
2. `git add godot-cpp`
3. `git commit -m 'https://github.com/redsett/BookerGodotCpp.git'`
4. `git push`

To update gotdot-cpp to a certain branch:
1. `cd godot-cpp`
2. `git checkout (branch-name)`
3. `cd..`
3. `git add godot-cpp`
3. `git commit`
4. `git push`
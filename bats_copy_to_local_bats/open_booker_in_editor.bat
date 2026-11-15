setlocal
cd /d %~dp0
python ../delete_dll.py
cd "..\..\"
REM The game folder is named Booker or BookerGodot depending on the machine.
set GAME=Booker
if exist BookerGodot\project.godot set GAME=BookerGodot
"Godot_v4.7.2-stable_win64.exe\Godot_v4.7.2-stable_win64.exe" --path %GAME% --editor
REM --verbose
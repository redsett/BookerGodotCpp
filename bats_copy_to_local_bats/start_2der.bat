setlocal
cd /d %~dp0
cd "..\..\"
cd "2der\source\godot\bin"
REM godot.windows.editor.dev.x86_64.exe --path ..\..\..\2der
call godot.windows.editor.dev.x86_64.exe -e ..\..\..\2der\project.godot
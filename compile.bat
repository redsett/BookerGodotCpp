setlocal
cd /d %~dp0
REM del ..\Booker\addons\BookerGodotCpp\bin\~libbooker_godot_cpp.windows.debug.x86_64.dll
REM scons dev_build=yes
scons target=template_debug dev_build=yes debug_symbols=yes
REM scons platform=windows target=editor dev_build=yes debug_symbols=yes
REM pause
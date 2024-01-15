setlocal
cd /d %~dp0
REM del ..\Booker\addons\BookerGodotCpp\bin\~libbooker_godot_cpp.windows.debug.x86_64.dll
REM scons dev_build=yes
REM scons platform=windows target=template_debug debug_symbols=yes -j12
scons
REM pause
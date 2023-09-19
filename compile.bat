setlocal
cd /d %~dp0
REM del ..\Booker\addons\BookerGodotCpp\bin\~libbooker_godot_cpp.windows.debug.x86_64.dll
scons dev_build=yes
REM dev_build=yes
REM pause
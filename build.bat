@echo off

if /i "%1" equ "clean" (
	del *.obj *.exe *.pdb>nul
	exit /b 0
)

set msvc_flags=/nologo /W3 /Fe:main.exe /EHsc /Zi
set link_flags=/incremental:no /opt:icf,ref /ignore:4099
set lib_files=user32.lib gdi32.lib opengl32.lib "glew-2.2.0\lib\Release\x64\glew32s.lib"
call cl euclid.cpp %msvc_flags% /link %link_flags% %lib_files%

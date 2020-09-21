@echo off 
setlocal EnableDelayedExpansion

set debug=0
set name=%1
set def_path=%2
set compiler=%3
set current=%cd%

cd %def_path%
if "%compiler%"=="IAR" (
	set compilerlib=Iar.a
) else (
if "%compiler%"=="GCC" (
	set compilerlib=Gcc.lib
) else (
	set compilerlib=.lib
)
)
set def_file=inc\abup_os.h
for /f "tokens=2,3" %%i in ('findstr "ABUP_BOOTLOADER_DEBUG" "%def_file%"') do if %%i==ABUP_BOOTLOADER_DEBUG set /A debug=%%j
cd Lib\bl
if "%debug%"=="0" (
	copy %name%Release%compilerlib% %name%%compilerlib%
	echo Release Mode
) else (
	copy %name%Debug%compilerlib% %name%%compilerlib%
	echo Debug Mode
)
cd %current%
echo complete
GOTO:EOF

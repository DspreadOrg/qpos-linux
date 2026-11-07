@echo off
rem ============================================================
rem  load_target_arch.bat [optional CMakeCache.txt]
rem
rem  Resolve TARGET_ARCH for the calling script.
rem  Priority: pkg_bat\target_arch.cfg -> given CMakeCache.txt -> ARM
rem ============================================================
setlocal EnableExtensions

set "TARGET_ARCH="

rem 1) Saved selection written by build.bat
set "ARCH_CONFIG_FILE=%~dp0target_arch.cfg"
if exist "%ARCH_CONFIG_FILE%" (
    for /f "usebackq delims=" %%A in ("%ARCH_CONFIG_FILE%") do (
        if not defined TARGET_ARCH set "TARGET_ARCH=%%A"
    )
)

rem 2) Optional CMake cache, kept for backward compatibility.
if not defined TARGET_ARCH if not "%~1"=="" (
    if exist "%~1" (
        for /f "tokens=1,* delims==" %%A in ('findstr /B /C:"TARGET_ARCH:STRING=" "%~1"') do set "TARGET_ARCH=%%B"
    )
)

if not defined TARGET_ARCH set "TARGET_ARCH=ARM"

rem Trim leading spaces that may come from the config file.
for /f "tokens=1 delims= " %%A in ("%TARGET_ARCH%") do set "TARGET_ARCH=%%A"

if /I "%TARGET_ARCH%"=="ARM" (
    set "TARGET_ARCH=ARM"
) else if /I "%TARGET_ARCH%"=="MIPS" (
    set "TARGET_ARCH=MIPS"
) else (
    echo [WARN] Unknown TARGET_ARCH="%TARGET_ARCH%", fallback to ARM.
    set "TARGET_ARCH=ARM"
)

endlocal & set "TARGET_ARCH=%TARGET_ARCH%"
exit /b 0

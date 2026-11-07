@echo off
rem ============================================================
rem  resolve_target_arch.bat [auto|always]
rem
rem  Decide which platform to build, in this order:
rem
rem    auto   (compile.bat)   1. connected device (adb)
rem                           2. saved pkg_bat\target_arch.cfg
rem                           3. platform menu
rem
rem    always (recompile.bat) 1. connected device (adb)
rem                           2. platform menu
rem
rem  Set FORCE_ARCH_MENU=1 to bypass adb detection and always
rem  show the menu.
rem
rem  Returns TARGET_ARCH and ARCH_SOURCE (device|saved|menu).
rem  The selection is saved to pkg_bat\target_arch.cfg.
rem ============================================================
setlocal EnableExtensions EnableDelayedExpansion

set "MODE=%~1"
if not defined MODE set "MODE=auto"
set "TARGET_ARCH="
set "ARCH_SOURCE="
set "SAVED="

if exist "%~dp0target_arch.cfg" (
    for /f "usebackq delims=" %%A in ("%~dp0target_arch.cfg") do (
        if not defined SAVED set "SAVED=%%A"
    )
)
for /f "tokens=1 delims= " %%A in ("!SAVED!") do set "SAVED=%%A"
if /I not "!SAVED!"=="ARM" if /I not "!SAVED!"=="MIPS" set "SAVED="

if "%FORCE_ARCH_MENU%"=="1" (
    echo [INFO] FORCE_ARCH_MENU=1, using the platform menu.
    goto :menu
)

rem ---------- 1. connected device wins ------------------------
call "%~dp0detect_target_arch.bat"
if errorlevel 1 goto :no_device
if /I not "%TARGET_ARCH%"=="ARM" if /I not "%TARGET_ARCH%"=="MIPS" goto :no_device
set "ARCH_SOURCE=device"
goto :resolved

:no_device
if /I "%MODE%"=="always" goto :menu
if defined SAVED goto :use_saved
goto :menu

:use_saved
set "TARGET_ARCH=%SAVED%"
set "ARCH_SOURCE=saved"
echo [INFO] Using saved platform: %TARGET_ARCH%
goto :resolved

:menu
call "%~dp0select_target_arch.bat"
if errorlevel 1 (
    echo [ERROR] No target platform selected.
    endlocal
    exit /b 1
)
set "ARCH_SOURCE=menu"

:resolved
for /f "tokens=1 delims= " %%A in ("%TARGET_ARCH%") do set "TARGET_ARCH=%%A"
if /I not "%TARGET_ARCH%"=="ARM" if /I not "%TARGET_ARCH%"=="MIPS" (
    echo [ERROR] Invalid target platform: "%TARGET_ARCH%"
    endlocal
    exit /b 1
)
if defined SAVED if /I not "%SAVED%"=="%TARGET_ARCH%" (
    echo [INFO] Platform changed: %SAVED% -^> %TARGET_ARCH% ^(%ARCH_SOURCE%^)
)
if /I not "%SAVED%"=="%TARGET_ARCH%" (
    > "%~dp0target_arch.cfg" echo %TARGET_ARCH%
)

endlocal & (
    set "TARGET_ARCH=%TARGET_ARCH%"
    set "ARCH_SOURCE=%ARCH_SOURCE%"
)
exit /b 0

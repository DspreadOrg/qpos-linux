@echo off
rem ============================================================
rem  select_target_arch.bat
rem
rem  Interactive platform menu, called by resolve_target_arch.bat
rem  when no device is connected over adb (or when the menu is
rem  forced with FORCE_ARCH_MENU=1).
rem
rem    1 -> ARM   (QposPlus / QposTrio)
rem    2 -> MIPS  (D70)
rem
rem  The selection is saved to pkg_bat\target_arch.cfg and
rem  returned to the caller as TARGET_ARCH.
rem  Exit code 1 means no platform was selected.
rem ============================================================
setlocal EnableExtensions

set "TARGET_ARCH="
set "CURRENT="
set "ATTEMPT=0"

rem Current saved platform, shown as the default choice.
if exist "%~dp0target_arch.cfg" (
    for /f "usebackq delims=" %%A in ("%~dp0target_arch.cfg") do (
        if not defined CURRENT set "CURRENT=%%A"
    )
)
for /f "tokens=1 delims= " %%A in ("%CURRENT%") do set "CURRENT=%%A"
if /I not "%CURRENT%"=="ARM" if /I not "%CURRENT%"=="MIPS" set "CURRENT="

:menu
set /a ATTEMPT+=1
if %ATTEMPT% GTR 5 goto :cancelled

echo.
echo ==========================================
echo    Select target platform
echo ==========================================
if defined CURRENT echo    Current platform: %CURRENT%
echo.
echo    1. ARM    (QposPlus / QposTrio)
echo    2. MIPS   (D70)
echo.
echo   NOTE: this step is SKIPPED when a device is connected by
echo         USB/adb - the platform is then read from the device.
echo         Connect the device to avoid choosing manually.
echo.

set "PROMPT=  Enter 1 or 2"
if defined CURRENT set "PROMPT=  Enter 1 or 2 (Enter = keep %CURRENT%)"
set "CHOICE="
set /p "CHOICE=%PROMPT%: "

rem Ignore leading/trailing spaces typed by the user.
for /f "tokens=1 delims= " %%A in ("%CHOICE%") do set "CHOICE=%%A"

if /I "%CHOICE%"=="1" set "TARGET_ARCH=ARM"
if /I "%CHOICE%"=="2" set "TARGET_ARCH=MIPS"
if /I "%CHOICE%"=="ARM" set "TARGET_ARCH=ARM"
if /I "%CHOICE%"=="MIPS" set "TARGET_ARCH=MIPS"
if not defined CHOICE if defined CURRENT set "TARGET_ARCH=%CURRENT%"

if not defined TARGET_ARCH (
    echo [ERROR] Invalid selection: "%CHOICE%"
    goto :menu
)

> "%~dp0target_arch.cfg" echo %TARGET_ARCH%
echo [INFO] Target platform: %TARGET_ARCH%
echo [INFO] Saved to pkg_bat\target_arch.cfg
endlocal & set "TARGET_ARCH=%TARGET_ARCH%"
exit /b 0

:cancelled
echo [ERROR] No target platform selected.
endlocal & set "TARGET_ARCH="
exit /b 1

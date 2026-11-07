@echo off
rem ============================================================
rem  detect_target_arch.bat
rem
rem  Determine the target platform of the connected device with a
rem  single command:
rem
rem    adb shell uname -m
rem      -> mips*           => MIPS  (D70)
rem      -> arm* / aarch64  => ARM   (QposPlus / QposTrio)
rem
rem  Returns: TARGET_ARCH
rem  Exit codes: 0 = platform detected
rem              1 = adb missing / no device / unknown architecture
rem ============================================================
setlocal EnableExtensions

set "TARGET_ARCH="

rem Allow the detection to be switched off (e.g. adb hangs or is not
rem wanted): set NO_ADB_DETECT=1 before running the build scripts.
if defined NO_ADB_DETECT (
    echo [INFO] NO_ADB_DETECT is set, device detection skipped.
    endlocal
    exit /b 1
)

where adb >nul 2>nul
if errorlevel 1 (
    echo [INFO] adb not found in PATH, device detection skipped.
    endlocal
    exit /b 1
)

set "ARCH_FILE=%TEMP%\lnxpos_uname_%RANDOM%%RANDOM%.txt"
rem "call" keeps this script alive even when adb is a .bat wrapper.
call adb shell uname -m > "%ARCH_FILE%" 2>nul

findstr /I /C:"mips" "%ARCH_FILE%" >nul 2>nul
if not errorlevel 1 set "TARGET_ARCH=MIPS"

if not defined TARGET_ARCH (
    findstr /I /C:"arm" "%ARCH_FILE%" >nul 2>nul
    if not errorlevel 1 set "TARGET_ARCH=ARM"
)
if not defined TARGET_ARCH (
    findstr /I /C:"aarch64" "%ARCH_FILE%" >nul 2>nul
    if not errorlevel 1 set "TARGET_ARCH=ARM"
)
del /Q "%ARCH_FILE%" >nul 2>nul

if not defined TARGET_ARCH (
    echo [INFO] No device platform detected from: adb shell uname -m
    endlocal
    exit /b 1
)

echo [INFO] Device platform: %TARGET_ARCH%  [adb shell uname -m]
endlocal & set "TARGET_ARCH=%TARGET_ARCH%"
exit /b 0

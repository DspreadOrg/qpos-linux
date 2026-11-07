@echo off
rem ============================================================
rem  install.bat [APK_NAME]
rem
rem  Install the APK produced by compile.bat / recompile.bat onto
rem  the device connected over adb.
rem
rem  Before installing, the device platform is read with
rem    adb shell uname -m
rem  and compared with the platform of the APK. A mismatch stops
rem  the installation with a clear message.
rem
rem  Usage:
rem    pkg_bat\install.bat                  install the APK that
rem                                        matches the device
rem    pkg_bat\install.bat D70_Linux_App    install that APK
rem                                        (name with or without .apk)
rem
rem  FORCE_INSTALL=1 installs even when the platforms differ.
rem ============================================================
setlocal EnableExtensions EnableDelayedExpansion

pushd "%~dp0.."
if errorlevel 1 exit /b 1

set "APK_ARG=%~1"
set "APK_FILE="
set "APK_ARCH="
set "DEV_PLATFORM="
set "PM_CMD="

rem ---------- 1. which platform is the device? ----------------
where adb >nul 2>nul
if errorlevel 1 (
    echo [ERROR] adb was not found in PATH, cannot install.
    popd
    exit /b 1
)

set "TMP_ARCH=%TEMP%\lnxpos_inst_arch_%RANDOM%%RANDOM%.txt"
call adb shell uname -m > "%TMP_ARCH%" 2>nul

findstr /I /C:"mips" "%TMP_ARCH%" >nul 2>nul
if not errorlevel 1 set "DEV_PLATFORM=MIPS"
if not defined DEV_PLATFORM (
    findstr /I /C:"arm" "%TMP_ARCH%" >nul 2>nul
    if not errorlevel 1 set "DEV_PLATFORM=ARM"
)
if not defined DEV_PLATFORM (
    findstr /I /C:"aarch64" "%TMP_ARCH%" >nul 2>nul
    if not errorlevel 1 set "DEV_PLATFORM=ARM"
)
del /Q "%TMP_ARCH%" >nul 2>nul

if not defined DEV_PLATFORM (
    echo [ERROR] No device detected by: adb shell uname -m
    echo         Connect the device over USB, then run this script again.
    popd
    exit /b 1
)
echo [INFO] Device platform: %DEV_PLATFORM%  [adb shell uname -m]

rem ---------- 2. APK names per platform ---------------------
set "NAMES_ARM="
set "NAMES_MIPS="
if exist "%~dp0apk_targets.cfg" (
    for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%~dp0apk_targets.cfg") do (
        if /I "%%A"=="ARM" set "NAMES_ARM=%%B"
        if /I "%%A"=="MIPS" set "NAMES_MIPS=%%B"
    )
)
set "NAMES_DEV=!NAMES_%DEV_PLATFORM%!"

rem ---------- 3. pick the APK --------------------------------
if defined APK_ARG goto :apk_from_arg

if not defined NAMES_DEV (
    echo [ERROR] No APK configured for %DEV_PLATFORM% in pkg_bat\apk_targets.cfg
    popd
    exit /b 1
)
for %%M in (%NAMES_DEV%) do (
    if not defined APK_FILE if exist "%%M.apk" set "APK_FILE=%%M.apk"
)
if not defined APK_FILE (
    echo [ERROR] No built APK for %DEV_PLATFORM% found in the project root.
    echo         Run pkg_bat\compile.bat first.
    popd
    exit /b 1
)
set "APK_ARCH=%DEV_PLATFORM%"
echo [INFO] APK: %APK_FILE%  [%APK_ARCH%]
if /I not "%NAMES_DEV%"=="%APK_FILE:~0,-4%" echo [INFO]   note: same build as %NAMES_DEV%
goto :check_match

:apk_from_arg
set "APK_FILE=%APK_ARG%"
if /I "%APK_FILE:~-4%"==".apk" set "APK_FILE=%APK_FILE:~0,-4%"
set "APK_FILE=%APK_FILE%.apk"
if not exist "%APK_FILE%" (
    echo [ERROR] APK not found: %APK_FILE%
    popd
    exit /b 1
)

rem Which platform does this APK belong to, per apk_targets.cfg?
for %%M in (%NAMES_ARM%) do if /I "%%M.apk"=="%APK_FILE%" set "APK_ARCH=ARM"
for %%M in (%NAMES_MIPS%) do if /I "%%M.apk"=="%APK_FILE%" set "APK_ARCH=MIPS"

if not defined APK_ARCH (
    echo [ERROR] Cannot tell the platform of %APK_FILE%
    echo         It is not listed in pkg_bat\apk_targets.cfg.
    echo         Add it there, or install the APK for %DEV_PLATFORM% instead.
    popd
    exit /b 1
)
echo [INFO] APK: %APK_FILE%  [%APK_ARCH%]

rem ---------- 4. platform match check ------------------------
:check_match
if /I "%APK_ARCH%"=="%DEV_PLATFORM%" goto :push_apk

echo.
echo [WARN] ================================================================
echo [WARN] Platform mismatch, installation stopped.
echo [WARN]   device platform : %DEV_PLATFORM%   [adb shell uname -m]
if /I "%DEV_PLATFORM%"=="MIPS" (
    echo [WARN]   this device needs : MIPS APK, e.g. D70_Linux_App.apk
) else (
    echo [WARN]   this device needs : ARM APK, e.g. QposPlus_Linux_App.apk
)
echo [WARN]   APK platform    : %APK_ARCH%   [%APK_FILE%]
echo [WARN] ================================================================
if "%FORCE_INSTALL%"=="1" (
    echo [WARN] FORCE_INSTALL=1, installing anyway.
    goto :push_apk
)
echo.
echo [INFO] Rebuild for this device, then install again:
echo          pkg_bat\compile.bat
echo          pkg_bat\install.bat
popd
exit /b 1

rem ---------- 5. install -------------------------------------
:push_apk
set "DEV_APK=/tmp/lnxpos_install_%RANDOM%.apk"
echo [INFO] Copying %APK_FILE% to the device...
call adb push "%APK_FILE%" %DEV_APK% >nul 2>nul
if errorlevel 1 (
    echo [ERROR] adb push failed.
    popd
    exit /b 1
)

rem The device package manager lives in /ymos/bin on this platform;
rem plain "pm" is used as a fallback.
set "PM_CMD=pm"
call adb shell test -x /ymos/bin/pm >nul 2>nul
if not errorlevel 1 set "PM_CMD=/ymos/bin/pm"

echo [INFO] Installing %APK_FILE% on the device...
call adb shell "%PM_CMD% install %DEV_APK%" > "%TEMP%\lnxpos_inst_out.txt" 2>&1

findstr /I /C:"Answer=0" "%TEMP%\lnxpos_inst_out.txt" >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Installation failed. Device output:
    type "%TEMP%\lnxpos_inst_out.txt"
    del /Q "%TEMP%\lnxpos_inst_out.txt" >nul 2>nul
    call adb shell rm -f %DEV_APK% >nul 2>nul
    popd
    exit /b 1
)
del /Q "%TEMP%\lnxpos_inst_out.txt" >nul 2>nul
call adb shell rm -f %DEV_APK% >nul 2>nul

echo.
echo [OK] Installed %APK_FILE% on the %DEV_PLATFORM% device.
popd
exit /b 0

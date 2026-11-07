@echo off
rem ============================================================
rem  build.bat [ARM^|MIPS]
rem
rem  Select the target platform, remember it, create build\<ARCH>,
rem  configure CMake and build the application.
rem ============================================================
setlocal EnableExtensions EnableDelayedExpansion

pushd "%~dp0.."
if errorlevel 1 exit /b 1

set "ARCH=%~1"

if /I "%ARCH%"=="/?" goto :usage
if /I "%ARCH%"=="-H" goto :usage
if /I "%ARCH%"=="--HELP" goto :usage

rem No argument: use the device over adb, then the saved platform,
rem then the menu (same rules as compile.bat).
if defined ARCH goto :arch_ready
call "%~dp0resolve_target_arch.bat" auto
if errorlevel 1 (
    echo [ERROR] No target platform selected.
    popd
    exit /b 1
)
set "ARCH=%TARGET_ARCH%"
goto :arch_ready

:arch_ready
for /f "tokens=1 delims= " %%A in ("%ARCH%") do set "ARCH=%%A"

if /I "%ARCH%"=="ARM" (
    set "ARCH=ARM"
) else if /I "%ARCH%"=="MIPS" (
    set "ARCH=MIPS"
) else (
    echo [ERROR] Invalid target platform: "%ARCH%"
    goto :usage
)

rem Persist the selection for compile.bat / recompile.bat / package.bat.
> "%~dp0target_arch.cfg" echo %ARCH%
echo [INFO] Target platform: %ARCH%
echo [INFO] Saved to pkg_bat\target_arch.cfg

set "BUILD_DIR=%CD%\build\%ARCH%"
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    if errorlevel 1 (
        echo [ERROR] Failed to create build directory: %BUILD_DIR%
        popd
        exit /b 1
    )
    echo [INFO] Created build directory: %BUILD_DIR%
) else (
    echo [INFO] Build directory: %BUILD_DIR%
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] cmake was not found in PATH.
    popd
    exit /b 1
)

rem Only choose a generator for a brand new build directory, otherwise
rem keep the generator stored in CMakeCache.txt.
set "GEN_ARGS="
if not exist "%BUILD_DIR%\CMakeCache.txt" set GEN_ARGS=-G "MinGW Makefiles"

echo [INFO] Configuring CMake...
cmake -S "%CD%" -B "%BUILD_DIR%" %GEN_ARGS% -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DTARGET_ARCH=%ARCH%
if errorlevel 1 (
    echo [ERROR] CMake configure failed.
    popd
    exit /b 1
)

echo [INFO] Building...
cmake --build "%BUILD_DIR%" -- -j32
if errorlevel 1 (
    echo [ERROR] Build failed.
    popd
    exit /b 1
)

echo.
echo [OK] %ARCH% build finished.
echo      Binary : release\%ARCH%\bin\linux_pos_app

set "APK_NAMES="
if exist "%~dp0apk_targets.cfg" (
    for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%~dp0apk_targets.cfg") do (
        if /I "%%A"=="%ARCH%" set "APK_NAMES=%%B"
    )
)
echo      Package: pkg_bat\package.bat
for %%M in (%APK_NAMES%) do echo               %%~M.apk
popd
exit /b 0

:usage
echo.
echo Usage: %~nx0 [ARM^|MIPS]
echo   ARM^|MIPS   Force that platform, save it and build.
echo   (no arg)   Auto: connected device (adb) -^> saved platform -^> menu.
echo.
echo Environment:
echo   FORCE_ARCH_MENU=1   ignore the device, always show the menu
echo   NO_ADB_DETECT=1     skip adb detection
popd
exit /b 1

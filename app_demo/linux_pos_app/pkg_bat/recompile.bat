@echo off
rem ============================================================
rem  recompile.bat
rem
rem  Switch platform / full rebuild: clean, rebuild and package.
rem
rem  Target platform is decided automatically:
rem    1. device connected over adb -> platform is read from the
rem       device (menu is skipped)
rem    2. otherwise -> platform menu (1 = ARM, 2 = MIPS)
rem       (the saved platform is shown as the default choice)
rem
rem  Set FORCE_ARCH_MENU=1 to always show the menu.
rem
rem  Use: pkg_bat\recompile.bat
rem ============================================================
setlocal EnableExtensions

pushd "%~dp0.."
if errorlevel 1 exit /b 1

rem ---------- 1. resolve the platform (device first) -----------
call "%~dp0resolve_target_arch.bat" always
if errorlevel 1 goto :fail_arch

set "BUILD_DIR=%CD%\build\%TARGET_ARCH%"
echo [INFO] Target platform: %TARGET_ARCH%
echo [INFO] Build directory: %BUILD_DIR%

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] cmake was not found in PATH.
    popd
    exit /b 1
)

rem ---------- 2. clean + full rebuild --------------------------
set "GEN_ARGS="
if not exist "%BUILD_DIR%\CMakeCache.txt" set GEN_ARGS=-G "MinGW Makefiles"

echo [INFO] Configuring CMake...
cmake -S "%CD%" -B "%BUILD_DIR%" %GEN_ARGS% -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DTARGET_ARCH=%TARGET_ARCH%
if errorlevel 1 (
    echo [ERROR] CMake configure failed.
    popd
    exit /b 1
)

echo [INFO] Cleaning previous build objects...
cmake --build "%BUILD_DIR%" --target clean
if errorlevel 1 (
    echo [ERROR] Clean failed.
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

echo [OK] Build finished: release\%TARGET_ARCH%\bin\linux_pos_app

rem ---------- 3. package the APK -------------------------------
echo.
echo [INFO] Packaging...
call "%~dp0package.bat"
if errorlevel 1 (
    echo [ERROR] Packaging failed.
    popd
    exit /b 1
)

echo [OK] recompile.bat done for %TARGET_ARCH%.
popd
exit /b 0

:fail_arch
echo [ERROR] No target platform selected.
popd
exit /b 1

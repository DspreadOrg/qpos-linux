@echo off
rem ============================================================
rem  clean.bat
rem
rem  Clean build objects for the platform saved by build.bat and
rem  remove the APKs of that platform.
rem  Use: pkg_bat\clean.bat
rem ============================================================
setlocal EnableExtensions EnableDelayedExpansion

pushd "%~dp0.."
if errorlevel 1 exit /b 1

call "%~dp0load_target_arch.bat"
if errorlevel 1 (
    echo [ERROR] Failed to load target platform.
    popd
    exit /b 1
)

set "BUILD_DIR=%CD%\build\%TARGET_ARCH%"
echo [INFO] Target platform: %TARGET_ARCH%

if exist "%BUILD_DIR%\CMakeCache.txt" (
    echo [INFO] Cleaning build objects in %BUILD_DIR% ...
    cmake --build "%BUILD_DIR%" --target clean
    if errorlevel 1 (
        echo [ERROR] Clean failed.
        popd
        exit /b 1
    )
) else (
    echo [INFO] No configured build directory: %BUILD_DIR%
)

rem Remove the APKs generated for this platform only.
set "APK_NAMES="
if exist "%~dp0apk_targets.cfg" (
    for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%~dp0apk_targets.cfg") do (
        if /I "%%A"=="%TARGET_ARCH%" set "APK_NAMES=%%B"
    )
)
for %%M in (%APK_NAMES%) do (
    if exist "%%M.apk" (
        del /Q "%%M.apk"
        echo [INFO] Removed %%M.apk
    )
)

echo [OK] Clean finished.
popd
exit /b 0

@echo off
rem ============================================================
rem  package.bat
rem
rem  Package the APK(s) for the platform saved by build.bat.
rem  APK names come from pkg_bat\apk_targets.cfg:
rem    ARM  -> QposPlus_Linux_App.apk  QposTrio_Linux_App.apk
rem    MIPS -> D70_Linux_App.apk
rem  Every APK of the same platform shares the same package content.
rem  <project>.apk always holds the last packaged result.
rem ============================================================
setlocal EnableExtensions EnableDelayedExpansion

pushd "%~dp0.."
if errorlevel 1 exit /b 1

for %%i in (.) do set "folder_name=%%~ni"

call "%~dp0load_target_arch.bat"
if errorlevel 1 (
    echo [ERROR] Failed to load target platform.
    popd
    exit /b 1
)

echo [INFO] TARGET_ARCH=%TARGET_ARCH%

rem Read the APK name list of this platform from apk_targets.cfg.
set "APK_NAMES="
if exist "%~dp0apk_targets.cfg" (
    for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%~dp0apk_targets.cfg") do (
        if /I "%%A"=="%TARGET_ARCH%" set "APK_NAMES=%%B"
    )
)
if not defined APK_NAMES (
    echo [WARN] No entry for %TARGET_ARCH% in pkg_bat\apk_targets.cfg, using %folder_name%.
    set "APK_NAMES=%folder_name%"
)
echo [INFO] APK targets:%APK_NAMES%

set "BIN_DIR=release\%TARGET_ARCH%\bin"
if not exist "%BIN_DIR%\linux_pos_app" (
    echo [ERROR] Executable not found: %BIN_DIR%\linux_pos_app
    echo         Run pkg_bat\compile.bat first.
    popd
    exit /b 1
)

if exist "pkg" rmdir /S /Q "pkg"
mkdir "pkg"
if errorlevel 1 goto :fail

xcopy /E /I /Y "%BIN_DIR%" "pkg\bin" >nul
if errorlevel 1 goto :fail

rem Libraries live in per-category folders: lib\<ARCH>\{emv,mqtt,os,sdk}.
rem   emv / mqtt / sdk  application libraries -> packed, flattened into pkg\lib
rem   os                OS-provided libraries   -> NOT packed (present in
rem                     /ymos/lib and /usr/lib on the device), link only
set "ARCH_LIB_ROOT=lib\%TARGET_ARCH%"
set "ARCH_LIB_SKIP=os"
if not exist "%ARCH_LIB_ROOT%" (
    echo [ERROR] Library directory not found: %ARCH_LIB_ROOT%
    goto :fail
)

set "LIB_DIR_COUNT=0"
set "LIB_FILE_COUNT=0"
set "LIB_PACKED="
for /d %%D in ("%ARCH_LIB_ROOT%\*") do (
    if /I not "%%~nxD"=="%ARCH_LIB_SKIP%" (
        xcopy /E /I /Y "%%D" "pkg\lib" >nul
        if errorlevel 1 goto :fail
        set /a LIB_DIR_COUNT+=1
        set "LIB_PACKED=!LIB_PACKED! %%~nxD"
        for %%F in ("%%D\*") do set /a LIB_FILE_COUNT+=1
    )
)
if "%LIB_DIR_COUNT%"=="0" (
    echo [ERROR] No library category found under %ARCH_LIB_ROOT%
    goto :fail
)
if "%LIB_FILE_COUNT%"=="0" (
    echo [ERROR] No library file found under %ARCH_LIB_ROOT%
    goto :fail
)
echo [INFO] Libraries packed from %ARCH_LIB_ROOT%:%LIB_PACKED%
echo [INFO]   %LIB_FILE_COUNT% file^(s^); "%ARCH_LIB_SKIP%" is OS-provided and not packed

xcopy /E /I /Y "res" "pkg\res" >nul
if errorlevel 1 goto :fail

if /I "%TARGET_ARCH%"=="MIPS" (
    set "HAS_MIPS_ARCH="
    findstr /I /C:"<ympkg_arch>mips</ympkg_arch>" "ympkg.xml" >nul && set "HAS_MIPS_ARCH=1"

    if defined HAS_MIPS_ARCH (
        copy /Y "ympkg.xml" "pkg\ympkg.xml" >nul
    ) else (
        >"pkg\ympkg.xml" (
            for /f "usebackq delims=" %%L in ("ympkg.xml") do (
                set "line=%%L"
                set "close=!line:</ympkg>=!"
                if not "!close!"=="!line!" echo(    ^<ympkg_arch^>mips^</ympkg_arch^>
                echo(!line!
            )
        )
    )
) else (
    >"pkg\ympkg.xml" (
        for /f "usebackq delims=" %%L in ("ympkg.xml") do (
            set "line=%%L"
            set "without_arch=!line:<ympkg_arch>mips</ympkg_arch>=!"
            if "!without_arch!"=="!line!" echo(!line!
        )
    )
)
if errorlevel 1 goto :fail

pushd "pkg"
zip -r -q "..\pkgtest.zip" bin lib res ympkg.xml
if errorlevel 1 (
    popd
    goto :fail
)
popd

rem One zip, several names: the package content is identical per platform.
for %%M in (%APK_NAMES%) do (
    copy /Y "pkgtest.zip" "%%M.apk" >nul
    if errorlevel 1 goto :fail
    echo [INFO] Created %%M.apk
)

del /Q "pkgtest.zip" 2>nul
if exist "pkg" rmdir /S /Q "pkg"

echo.
echo [OK] Package created for %TARGET_ARCH%:
for %%M in (%APK_NAMES%) do echo      %%M.apk
popd
exit /b 0

:fail
if exist "pkg" rmdir /S /Q "pkg"
if exist "pkgtest.zip" del /Q "pkgtest.zip"
echo [ERROR] Packaging failed.
popd
exit /b 1

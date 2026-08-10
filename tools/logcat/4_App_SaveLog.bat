@echo off  

for /F "tokens=1-4 delims=/ " %%a in ('date /T') do (  
    set "year=%%a"  
    set "month=%%b"  
    set "day=%%c"  
)  
for /F "tokens=1-2 delims=: " %%a in ('time /T') do (  
    set "hour=%%a"  
    set "minute=%%b"  
)  
set "timestamp=%year%%month%%day%%hour%%minute%"

set script_dir=%~dp0
set logcat_file=%script_dir%logcat_%timestamp%.txt

cd platform-tools/

adb devices | findstr /m "device$"
if %errorlevel%==0 (
    echo Locate the device
	echo Save log path: %logcat_file%
	echo Close the window to exit saving the log
	adb logcat -v time > %logcat_file%
) else (
    echo Device not found
	echo Please plug and unplug the USB cable
)

pause
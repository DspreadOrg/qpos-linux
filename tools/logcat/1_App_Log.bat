@echo off  

cd platform-tools/
echo cur path: %~dp0
adb logcat -b main -v time
pause

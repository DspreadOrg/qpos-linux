@echo off  

cd platform-tools/
adb logcat -b main -b system -v time
pause

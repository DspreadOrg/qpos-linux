@echo off 

cd ..
del *.apk
cd build
make

cd ..
cd release\bin

arm-none-linux-gnueabihf-strip.exe linux_pos_app
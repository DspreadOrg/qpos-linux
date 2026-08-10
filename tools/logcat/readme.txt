1_App_Log.bat  				Console output application logs
2_SystemAndApp_Log.bat  	Console output application and system logs
3_SystemAndApp_SaveLog.bat  Save application and system logs to the current directory. for example: logcat_202408121452.txt
4_App_SaveLog.bat               Save application logs to the current directory. for example: logcat_202408121452.txt


Note: 

If Windows cannot recognize USB, please install the driver program in the USB driver directory

If the device does not have USB, it can be connected through WIFI mode:
1. The device is connected to a WIFI hotspot, and the APP calls the OsGetWifiStatus interface to obtain the device's IP address and display it on the screen
2. Connect your computer and device to the same WIFI hotspot
3. Enter adb connect+device IP in the command console, for example adb connect 1.1.1.1
4. Run the bat 3_SystemAndApp_SaveLog.bat
5. start the test
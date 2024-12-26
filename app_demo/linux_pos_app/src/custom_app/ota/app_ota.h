#ifndef _APP_OTA_H
#define _APP_OTA_H

#include "app_includes.h"


//#define TMS_FW_HEART_CUSTOM_URL                "http://cs.dspread.net:9090/api/v1/heartbeat"
//#define TMS_FW_HEART_CUSTOM_URL 	    "http://www.dspreadserv.net:9997/api/v1/heartbeat"
#define TMS_FW_HEART_CUSTOM_URL                "http://211.157.135.52:10039/terminal/getFirmwareUpgradeInfo"


void DispOtaCheck();
void DispOtaUpdate();
void Firmware_OTA_Check();
void Firmware_OTA();
void DispDownloading();
void TmsDispCallback(u32 id,u32 offset,u32 Length,void *pMsg);
void dispOtaCheckResult();
void ui_version_not_found();
void ui_Downloading_Fail();
void ui_sn_not_found();
#endif
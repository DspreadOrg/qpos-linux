#ifndef _APP_OTA_H
#define _APP_OTA_H

#include "app_includes.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define TMS_FW_HEART_CUSTOM_URL               "https://www.dspreadser.net:9011/terminal"   

void DispOtaCheck();
void DispOtaUpdate();
void Firmware_OTA_Check();
void Firmware_OTA();
void DispDownloading();
void TmsDispCallback(u32 id, char *pMsg);
void dispOtaCheckResult();
void ui_version_not_found();
void ui_Downloading_Fail();
void ui_sn_not_found();

void larktms_init();

#ifdef __cplusplus
}
#endif
#endif
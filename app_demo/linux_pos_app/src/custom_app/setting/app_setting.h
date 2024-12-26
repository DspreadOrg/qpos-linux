#ifndef _APP_SETTING_H_
#define _APP_SETTING_H_
#include "app_includes.h"

#define APN_NAME                        "China Mobile Internet"
#define APN                            "cmnet"

/*tlv custom tag*/
typedef enum
{
    CUSTOM_TAG_START = APP_TAG_START,
    WIFI_NAME,
    WIFI_PASSWORD,
}Custom_tag_t;

u32 getWifiName(pu8 wifiName);
void Wifi_Disconnect();
void Wifi_Forget();
void OpenAirplaneMode();
void CloseAirplaneMode();
void SetCfgPool(u32 tag,T_U8_VIEW uvValue);
void Apn_cfg();
void Wifi_Search();
T_U8_VIEW GetCfgPool(u32 tag);
u32 AnalysisWifiList(pu8 pValue,T_U8_VIEW list[]);
#endif
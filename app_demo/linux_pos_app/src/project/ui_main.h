#ifndef _UI_MAIN_H
#define _UI_MAIN_H
#include "app_includes.h"
/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#define MIN_AMOUNT 				            1000
#define MAX_AMOUNT 				            30000000
#define MAX_LEN_AMOUNT 				        15

#define AMOUNT_CURRENCY_SYMBOL              "$"
#define PROMPT_DEVICE_INIT                  "DEVICE INIT ..."
#define PROMPT_MENE                         "Menu"
#define PROMPT_MENU_TRANSACTION                 "Transaction"
#define PROMPT_MENU_PAYMNET                         "Payment"
#define PROMPT_MENU_SETTING                     "Setting"
#define PROMPT_MENU_NETWORK                         "Network"
#define PROMPT_MENU_NETWORK_WIFI                            "Wifi Cfg"
#define PROMPT_MENU_WIFI_OPEN                                   "Wifi Open"
#define PROMPT_MENU_WIFI_CLOSE                                  "Wifi Close"
#define PROMPT_MENU_WIFI_SEACH                                  "Wifi Search"
#define PROMPT_MENU_WIFI_DISCONNECT                             "Wifi Disconnect"
#define PROMPT_MENU_NETWORK_SIM                             "Sim Cfg"
#define PROMPT_MENU_SIM_OPEN                                    "Sim Open"
#define PROMPT_MENU_SIM_CLOSE                                   "Sim Close"
#define PROMPT_MENU_SIM_INFO                                    "Sim Info"
#define PROMPT_MENU_SSL_CFG                         "Ssl cfg"
#define PROMPT_MENU_SSL_APN_INJECT                          "Apn Setting"
#define PROMPT_MENU_SSL_CERT                                "Cert Setting"
#define PROMPT_MENU_ABOUT                       "About"
#define PROMPT_MENU_ABOUT_PCI                       "Pci Version"
#define PROMPT_MENU_ABOUT_DEV                       "Dev Version"
#define PROMPT_MENU_ABOUT_APP                       "App Version"
#define PROMPT_MENU_ABOUT_BOOT                      "Boot Version"
#define PROMPT_MENU_ABOUT_LIB                      "Lib Version"
#define PROMPT_MENU_OTA                         "OTA"
#define PROMPT_MENU_FIRMWARE_OTA                    "Firmware OTA"
#define PROMPT_MENU_GPRS_OTA                        "Cellular OTA"


#define PROMPT_READING_CARD                 "READING CARD..."
#define PROMPT_PROCESSING                   "Processing"
#define PROMPT_PLS_USE_MAG_ICC_NFC          "USE MAG/ICC/NFC"
#define PROMPT_PLS_USE_MAG_ICC              "PLS USE MAG/ICC"
#define PROMPT_PLS_USE_MAG_NFC              "PLS USE MAG/NFC"
#define PROMPT_PLS_USE_ICC_NFC              "PLS USE ICC/NFC"
#define PROMPT_PLS_USE_MAG                  "PLS USE MAG"
#define PROMPT_PLS_USE_ICC                  "PLS USE ICC"
#define PROMPT_PLS_USE_NFC                  "PLS USE NFC"
#define PROMPT_UPKEY_TO_START_NFC           "PRESS \"▲\" START NFC"
#define PROMPT_TRANS_ONLINE_PIN             "ONLINE PIN"
#define PROMPT_TRANS_OFFLINE_PIN            "OFFLINE PIN"
#define PROMPT_TRANS_OFFLINE_LASTE_PIN      "OFFLINE LASTE PIN"
#define PROMPT_TIMEOUT                      "TIMEOUT"
#define PROMPT_USER_CANCEL                  "USER CANCLE"
#define PROMPT_TRANS_SUCCESS                "TRANS APPROVED"
#define PROMPT_TRANS_FAIL                   "TRANS TERMINATED"
#define PROMPT_WIFI_OPEN_SUCCESS            "Wifi Open Success"
#define PROMPT_WIFI_CLOSE_SUCCESS           "Wifi Close Success"
#define PROMPT_WIFI_DISCONNECT              "Wifi Disconnect"
#define PROMPT_WIFI_NOT_AVAILBLE            "Wifi Not Available"
#define PROMPT_SIM_OPEN_SUCCESS             "Cellular open Success"
#define PROMPT_SIM_CLOSE                    "Cellular Closed"
#define PROMPT_MENU_WIFI_CONNECT            "WIFI CONNECT..."
#define PROMPT_WIFI_CONNECT_SUCCESS         "Wifi Connect Success"
#define PROMPT_WIFI_CONNECT_FAIL            "Input Error"
#define PROMPT_MENU_SIM_INFO                "Sim Info"
#define PROMPT_SIGNATURE_TEST_INFO          "Signature Test"
#define POSIONT_OFFSET                      3
#define PROMPT_ENGINEERING_MODE             "ENGINEERING MODE"
#define PROMPT_DEVICE_DESTRUCT              "DEVICE DESTRUCT"
#define PROMPT_TRY_ANOTER_INTERFACE         "Pls Try another inteface"

#define PROMPT_TLS_TEST                 "Tls Test 1"
#define PROMPT_TLS_TEST1                "Tls Test 2"
#define PROMPT_TLS_TEST2                "Tls Test 3"
#define PROMPT_TLS_TEST3                 "Tls Test 4"
#define PROMPT_TLS_TEST4                "Tls Test 5"
#define PROMPT_TLS_TEST5                "Tls Test 6"

#define PROMPT_MENU_RECOVER_IPEK                      "Recover"


typedef enum
{
    ITEM_LOGO_TITLE,
    ITEM_LOGO_TRANSACTION,
    ITEM_TRANS_CARD_PAYMENT,
    ITEM_TRANS_QR_PAYMENT,

    ITEM_LOGO_SETTING,
    ITEM_SETTING_NETWORK,
    ITEM_NETWOTK_WIFI,
    ITEM_WIFI_OPEN,
    ITEM_WIFI_CLOSE,
    ITEM_WIFI_SEARCH,
    ITEM_WIFI_DISCONNECT,
    ITEM_NETWORK_SIM,
    ITEM_SIM_OPEN,
    ITEM_SIM_CLOSE,
    ITEM_SIM_INFO,
    ITEM_SETTING_SSL,
    ITEM_SSL_APN,
    ITEM_SSL_CERT,

    ITEM_LOGO_ABOUT,
    ITEM_ABOUT_PCI,
    ITEM_ABOUT_DEV,
    ITEM_ABOUT_APP,
    ITEM_ABOUT_BOOT,
    ITEM_ABOUT_LIB,

    ITEM_LOGO_OTA,
    ITEM_OTA_FIRMWARE,
    ITEM_OTA_MODULE,
    ITEM_SIGNATURE_TEST,
    TLS_SESSION_CACHE_TEST,
    TLS_SESSION_CACHE_TEST1,
    TLS_SESSION_CACHE_TEST2,
    TLS_SESSION_CACHE_TEST3,
    TLS_SESSION_CACHE_TEST4,
    TLS_SESSION_CACHE_TEST5,

    ITEM_RECOVER_IPEK,
}DISP_ITEM_T;

typedef enum
{
    MENU_LOGO=200,
    MENU_TRANS,
    MENU_SETTING,
    MENU_SETTING_NETWORK,
    MENU_WIFI_CFG,
    MENU_SIM_CFG,
    MENU_SETTING_SSL,
    MENU_ANOUT,
    MENU_SSL_CFG,
    MENU_OTA
}UI_MENU_T;

typedef enum
{
    LCD_DISP_OTA_DOWNLOAD_DISP          = 300,
    LCD_DISP_OTA_SYNC_TMS               = 301,
    LCD_DISP_OTA_FIRMWARE_DOWNLODING    = 302,
    LCD_DISP_FIRMWARE_DOWNLOADING       = 303,
    LCD_DISP_OTA_FIRMWARE_DOWNLAOD_FAIL = 304,
    LCD_DISP_OTA_UPDATE_UNKNOWN_ERROR   = 305,
    LCD_DISP_OTA_UPDATE_FAIL            = 306,
    LCD_DISP_OTA_UPDATE_FINISH          = 307,
    LCD_DISP_NO_AVAILABLE_FIRMWARE      = 308,
    LCD_DISP_FIRMWARE_VERIFY            = 309,
    LCD_DISP_FIRMWARE_VERDIFY_FAIL      = 310,
    LCD_DISP_SN_NOT_REGIESTER           = 311,
    LCD_DISP_OTA_PROCESS                = 312,
    LCD_DISP_OTA_OTHER                  = 313,
	LCD_DISP_OTA_SYNC,
    LCD_DISP_START_DOWNLOADING,
	LCD_DISP_OTA_FIRMWARE_NEED_DOWNLOAD,
	LCD_DISP_OTA_CONNECT_SERVER_ERROR,
	LCD_DISP_OTA_CHECK_ERROR,
    LCD_DISP_OTA_VERSION_NOT_FOUND,
    LCD_DISP_SINGNATURE_SUCCESS,
    LCD_DISP_SELECT_APP,
    LCD_DISP_TRANS_SUCCESS,
    LCD_DISP_TRANS_FAIL,
    LCD_DISP_WIFI_LIST,
    LCD_DISP_WIFI_CONNECT_SUCCESS,
    LCD_DISP_WIFI_CONNECT_FAIL,
}LCD_DISP_ID_T;

typedef struct
{
    DISP_ITEM_T item;
    char *disp_msg;
}DISP_DB_T;

typedef struct
{
    UI_MENU_T type;
    u32 Item_cnt;
    pu32 menu_item;
}MENU_ITEM_DB;

extern void DispWifiList();
extern void DispResult(const char * prompt);
extern void Enter_Amount();
extern void Online(const char *prompt);
extern void Download(const char * pTitle);
extern void UpdateOTAProcess(u32 progress,u32 max);
extern void DispWifiConnectionFail();
extern void DispWifiConnectionMessage(const char * message);
extern void DispWifiConnectionConfirmation();
extern void DispSelectPaymentMethod();

void GuiDisplay(u32 id);
void DispProcess(const char * prompt);
void DispMenuOptions();
void emvTransResult(App_Rc_Code_t reCode);

#endif

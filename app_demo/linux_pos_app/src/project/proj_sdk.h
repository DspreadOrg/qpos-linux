#ifndef _PROJ_SDK_H
#define _PROJ_SDK_H

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "appinc.h"

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/

#define LCD_LEFT_ALIGNMENT                 (0)
#define LCD_RIGHT_ALIGNMENT                (1)
#define LCD_CENTERED_ALIGNMENT             (2)

#define LCD_LINE_0                         (0)
#define LCD_LINE_1                         (1)
#define LCD_LINE_2                         (2)
#define LCD_LINE_3                         (3)
#define LCD_LINE_4                         (4)
#define LCD_LINE_5                         (5)
#define LCD_LINE_NUMS                      (6)
#define LCD_LINE_ICON                      (4)

#define LCD_DISP_DEVICE_INIT               (0)
#define LCD_DISP_WELCOME                   (1)
#define LCD_DISP_ENGINEERING_MODE          (2)
#define LCD_DISP_DEVICE_DESTRUCT           (3)
#define LCD_DISP_SLEEP_MODE                (4)
#define LCD_DISP_BATT_ICON                 (5)
#define LCD_DISP_USB_STATUS_ICON           (6)
#define LCD_DISP_BT_STATUS_ICON            (7)
#define LCD_DISP_TIME                      (8)
#define LCD_DISP_START_TRANSACTION         (9)
#define LCD_DISP_CHIP_CARD                 (10)
#define LCD_DISP_UPKEY_TO_START_NFC        (11)
#define LCD_DISP_SWIPE_ERROR               (12)
#define LCD_DISP_CHANGE_TO_CONTACT         (13)
#define LCD_DISP_TRANSACTION_TERMINATED    (14)
#define LCD_DISP_REMOVE_CARD               (15)
#define LCD_DISP_TRADE_SUCCESS             (16)
#define LCD_DISP_TIMEOUT                   (17)
#define LCD_DISP_RESET                     (18)
#define LCD_DISP_CANCEL                    (19)
#define LCD_DISP_INPUT_ONLINE_PIN          (20)
#define LCD_DISP_PIN_MASK                  (21)
#define LCD_DISP_CONFIRM_IF_NO_PIN         (22)
#define LCD_DISP_CONFIRM_IF_NO_PIN_CLEAR   (23)
#define LCD_DISP_CUSTOMIZE                 (24)
#define LCD_DISP_OFFLINE_BALANCE           (25)
#define LCD_DISP_READING_CARD              (26)
#define LCD_DISP_GO_ONLINE                 (27)
#define LCD_DISP_INPUT_AMOUNT              (28)
#define LCD_DISP_TRANS_COUNTER             (29)
#define LCD_DISP_WIFI_STATUS_ICON          (30)
#define LCD_DISP_INPUT_PIN_AGAIN           (31)
#define LCD_DISP_INPUT_OFFLINE_PIN         (32)
#define LCD_DISP_INPUT_LAST_OFFLINE_PIN    (33)
#define LCD_DISP_TEST_WHITE                (34)
#define LCD_DISP_TEST_BLACK                (35)
#define LCD_DISP_TEST_BACKLIGHT            (36)
#define LCD_DISP_TEST_LED_BLUE             (37)
#define LCD_DISP_TEST_LED_RED              (38)
#define LCD_DISP_TEST_BUZZER               (39)
#define LCD_DISP_TEST_LED_YELLOW           (40)
#define LCD_DISP_TEST_LED_GREEN            (41)
#define LCD_DISP_TEST_KEYBOARD             (42)
#define LCD_DISP_TEST_KBD_VALUE            (43)
#define LCD_DISP_GPRS_SIGNAL_LVL           (44)
#define LCD_DISP_PRINT_LINE_BLINK          (45)
#define LCD_DISP_PRINT_AMOUNT_CODE         (46)
#define LCD_DISP_WAITING_FOR_HOST          (47)
#define LCD_DISP_SHUTDOWN                  (48)
#define LCD_DISP_NET_DESTINATION_IP        (49)
#define LCD_DISP_NET_ERROR                 (50)
#define LCD_DISP_NET_COMMUNICATE           (51)
#define LCD_DISP_NET_DATA_RECEPTION        (52)
#define LCD_DISP_NFC_RETAP                 (53)
#define LCD_DISP_SEE_PHONE_PLS             (54)
#define LCD_DISP_BATTERY_ERROR             (55)
#define LCD_DISP_CREDIT_DEBIT              (56)
#define LCD_DISP_CUSTOMIZE_TRANS_DISP      (57)
#define LCD_DISP_LOW_POWER                 (99)
#define LCD_DISP_DECLINED_DISP             (100)
#define LCD_DISP_TRANSACTION_NOT_ALLOW     (101)
#define LCD_DISP_EMV_APP_BLOCK             (102)
#define LCD_DISP_AGING_TEST_MENU           (103)
#define LCD_DISP_AGING_TEST                (104)
#define LCD_DISP_VERIFY_OFFLINE_PIN        (105)
#define LCD_DISP_PIN_ERROR                 (106)
#define LCD_DISP_FREE_MSG                  (107)
#define LCD_DISP_CLERAD_ALL_LINE           (108)
#define LCD_DISP_CLERAD_LINE               (109)
#define LCD_DISP_USE_NFC                   (110)
#define LCD_DISP_PCI_VERSION               (111)
#define LCD_DISP_CHANGE_TO_MAG             (112)
#define LCD_DISP_FALL_BACK                 (113)
#define LCD_DISP_TRY_ANOTHER_INTERFACE     (114)
#define LCD_DISP_CARD_NOT_SUPPORT          (115)
#define LCD_DISP_PIN_CONFIRM               (116)
#define LCD_DISP_NFC_TRY_AGAIN             (117)

#define BUFF_CLEAR(s) memset(s, 0x00, sizeof(s))
/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/

typedef struct
{
    u32 ApnNameLen; //运营商名称长度
    u8 ApnName[32];
    u32 ApnLen;     // APN
    u8 Apn[32];
    u32 UserNameLen;  // 用户名
    u8 UserName[32];
    u32 UserPasswdLen;  // 用户密码
    u8 UserPasswd[32];
    u8 Mcc[5]; //3个数字
    u8 Mnc[5];  // 2~3个数字
}ApnParam_t,*pApnParam_t;
/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Display APIs
+----------------------------------------------------------------------------*/
extern void EventHandle(u32 event_id);
extern void EventRegister(u32 event_id);
extern void DisplayHandler(pmessage_body_t pmsg);
/*-----------------------------------------------------------------------------
|   transaction APIs
+----------------------------------------------------------------------------*/
extern u32 StartTrading(pu8 pTradingFile);
extern void StartTradingOnlineHandler(pu8 pTradingFile);
extern void TransCancle();
extern void TransKbdEvent(u32 key);
/*-----------------------------------------------------------------------------
|   connect server APIs
+----------------------------------------------------------------------------*/
extern s32 SslTransmit(pu8 pHost, u32 Port, pu8 pData, u32 DataLength, pu8 pReceivedData, u32 ReceivedDataLengthMax, u32 Timeout );

extern u32 Custom_GetEmvTag(u32 type,u32 tag,pu8 value);

/*-----------------------------------------------------------------------------
|   TLV  APIs
+----------------------------------------------------------------------------*/
extern T_U8_VIEW get_tlv_view(u8 *pTlvBuf, u32 dwTag);
extern void set_tlv_view(u8 *pTlvBuf, u32 dwTag, T_U8_VIEW uvVal);
extern u32 get_tlv_value(pu8 pool,u32 T,pu8 V );

/*-----------------------------------------------------------------------------
|   data Format covert  APIs
+----------------------------------------------------------------------------*/
extern u32 bcd_to_asc(pu8 bcd,u32 align,u32 asclen,pu8 asc );
extern u32 bcd_to_asc_sample(pu8 src,u32 len,pu8 dest );
extern u32 asc_to_bcd(pu8 src,u32 len,pu8 dest );
extern u32 asc_to_bcd_sample(pu8 src,u32 len,pu8 dest );

/*-----------------------------------------------------------------------------
|   amount Format covert  APIs
+----------------------------------------------------------------------------*/
extern u32 format_amt_ascn_to_bcd6(pu8 amt,u32 amtlen,pu8 amtOut );
extern u32 format_amt_bcd6_to_ascn(pu8 amt,u32 amtlen,pu8 amtOut );
extern u32 format_amt_asc_points_to_yuan(pu8 amt,u32 amtlen,pu8 amtOut );
extern u32 format_amt_int_to_str(u32 amt,pu8 amtIcon,bool amtPoint,pu8 pAmtDisp );

/*-----------------------------------------------------------------------------
|   lvgl Porting API
+----------------------------------------------------------------------------*/
extern void set_user_appver(char * ver);//No spaces allowed, maximum length 15

/*
*******************************************************************************
*   End of File
*******************************************************************************
*/

extern void disp_welcome();
extern void disp_init();
extern int Dev_nInit();
extern bool get_Lvgl_Kbd_Flag();
extern void Gprs_Init();
extern void getKernelInsLibVer(pu8 ver);

#endif  /* #ifndef _PROJ_SDK_H */


/*
 * app_emv.h
 */

#ifndef _APP_EMV_H_
#define _APP_EMV_H_

#include "appinc.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TAG_CONTACTLESS_REFUND_PROCESS                  0xDF8302

#define PED_PIN_IPEK_INDEX 0x01
#define PED_EMV_IPEK_INDEX 0x02
#define PED_TRK_IPEK_INDEX 0x03

#define  DspOsLog(lv, ...) {OsLog(LOOG_ERROR, "Dspread: ");OsLog(LOG_ERROR, ##__VA_ARGS__);} 

typedef enum
{
	APP_RC_SUCCESS = 0,
	APP_RC_FAIL = 1,

	APP_RC_TIMEOUT = 0x10000001,
	APP_RC_RESET,
	APP_RC_CANCEL,
	APP_RC_COMPLETED,
	APP_RC_TERMINAL,
	APP_RC_NFC_TERMINAL,

	APP_RC_MAG_READY,
	APP_RC_ICC_READY,
	APP_RC_NFC_READY,
	APP_RC_FALL_BACK,
	APP_RC_PSAM_READY,

	APP_RC_OTHER,
	APP_RC_RESET2,
	APP_RC_EMVE_DENAIL,
	APP_RC_NFC_MULTI_CARD,
	APP_RC_EMVE_GAC2_DENAIL,
	APP_RC_EMVE_GAC2_NULL,//第二次gac失败与检查网络冲突情况，联网失败不显示cmd timeout
	APP_RC_NFC_NOT_ALLOW,//不允许交易
	APP_RC_EMV_APP_BLOCK,//不允许交易
	APP_RC_EMV_APP_SEE_PHONE,
	APP_RC_EMV_TRANS_TRY_ANOTHER_INTERFACE,
	APP_RC_EMV_TRANS_GPO_NOT_SUPPORT,
	APP_RC_EMV_REVERSAL,//冲正
	APP_RC_EMV_CARD_BLOCK,

	APP_RC_NFC_DOUBLETAP_DENAIL,
	APP_RC_CARD_NOT_SUPPORT,

	APP_RC_NUMS,
}App_Rc_Code_t;

typedef	enum
{
	OFFLINEAPPROVED,
	OFFLINEDECLINED,
	ONLINEAPPROVED,
	ONLINEDECLINED,
	UNABLEONLINE_OFFLINEAPPROVED,
	UNABLEONINE_OFFLINEDECLINED
}TRANSRESULT;

#define INPUT_STRIPE   (1<<0)  //刷卡
#define INPUT_INSERTIC (1<<1)  //插卡
#define INPUT_RFCARD   (1<<2)  //挥卡

typedef struct tagCardDataInfo
{
	ST_MSR_DATA ptTracker1;
	ST_MSR_DATA ptTracker2;
	ST_MSR_DATA ptTracker3;
	PR_INT8 CardSN[3+1];
	PR_INT8 Pan[19+1+1];		//主账号
	PR_INT8 ExpDate[4+1];
    PR_INT8 ServiceCode[3+1];
    PR_INT8 CardCompany[3+1];	//信用卡公司

}CardDataInfo;

extern int app_emv_cbk(void* user_data, unsigned char* buf, unsigned int len);
extern int MultAidSelect(pu8 *pList, int listNum);
extern int GuiInputPasswd(char* pPin);
extern u32 App_Emv_Process(int nRetSwipeType,unsigned char *pTradFile);
extern u32 App_Mag_Process(CardDataInfo *pCardDataInfo, unsigned char *pTradFile);
extern void app_emv_init();
extern bool get_Pin_Input_Flag();
extern void set_Lvgl_Kbd_Flag(bool flag);


extern void StartTradingOnlineHandler(pu8 pTradingFile);
extern void fun_trans_finish(u32 rcCode );//App_Rc_Code_t rcCode

extern void kernelIns_init();
extern void set_KernelInsLib_mode(int mode);
#ifdef __cplusplus
}
#endif
#endif 
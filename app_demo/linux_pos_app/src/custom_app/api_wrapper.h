/*
 * app_wrapper.h
 *  Note:app new thread
 */

#ifndef _API_WRAPPER_H_
#define _API_WRAPPER_H_

#include "appinc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	TRANS_MODE_ICC = (0x01),
	TRANS_MODE_MAG,
	TRANS_MODE_ICC_MAG_NFC,
	TRANS_MODE_ICC_MAG_NFC_DONT_ALLOW_DOWNGRADE,
	TRANS_MODE_ICC_MAG,
	TRANS_MODE_ICC_MAG_DONT_ALLOW_DOWNGRADE,
	TRANS_MODE_NFC,
	TRANS_MODE_ICC_MAG_NFC_NOUPKEY,
	TRANS_MODE_ICC_MAG_NFC_NOUPKEY_DONT_ALLOW_DOWNGRADE,
	TRANS_MODE_ICC_NFC_DONT_USE_UPKEY=(0x0A),//仅支持IC/NFC,且禁止使用上翻键盘
	TRANS_MODE_ICC_NFC=(0x0B),//仅支持IC/NFC,需使用上翻键盘
	TRANS_MODE_ICC_NFC_DONT_ALLOW_DOWNGRADE,
	TRANS_MODE_ICC_MAG_NFC_DELAY
}Poll_Card_Mode_t;


typedef struct
{
	Card_Type_t  type;
	const char * amt;
    u8           money_type;
	u32          timeout;
}Lcd_Start_Transaction_t,*pLcd_Start_Transaction_t;

#ifdef TRANS_DBG

static inline void getInfo( const char* module, int line, const char* func ){
    OsLog(LOG_DEBUG, "\rDspread: %s | %s | %d | ", module, func,line);
}

#define DSP_Debug()  do{ getInfo(__FILE__, __LINE__, __FUNCTION__);}while(0)
#define DSP_Info(fmt, ...)  do{ getInfo(__FILE__, __LINE__, __FUNCTION__); OsLog(LOG_DEBUG,fmt,##__VA_ARGS__);}while(0)

#else
	#define DSP_Debug() ((void)0)
	#define DSP_Info(fmt, ...)  ((void)0)
#endif
void Set_dev_gprs_signal_lvl(u32 lvl);
void Set_dev_wifi_signal_lvl(u32 lvl);
u32 Get_dev_wifi_signal_lvl(void);
u32 Get_dev_gprs_signal_lvl(void);

bool Get_dev_UsbConnect_Status(void);
u32 Get_dev_sim_status(void);

int Ped_Dukpt_Init();
void dukpt_test();
void pinpadDupktTest1();
void pinpadMKSKTest();

extern void disp_format_type(int dispType     ,pvoid dispParam );
extern void transPoolSetTag(int tag,pu8 pValue,int Len);
extern void app_trace_value(unsigned char* pbuf, unsigned int cnt);

#ifdef __cplusplus
}
#endif
#endif 
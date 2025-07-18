/*
 * File Name:
 * lib_fun.h
 * Author:
 * jiaxin
 * Version:
 * V1.0
 * Description:
 */

#ifndef _LIB_FUN_H
#define _LIB_FUN_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*
 * -----------------------------------------------------------------------------
 * |   Includes
 * +----------------------------------------------------------------------------
 */
#include "appinc.h"

#if 0
#define CFG_LIB_DEBUG
#endif

#if 0
#define CFG_CARD_SIMULATION
#endif
/*
 * -----------------------------------------------------------------------------
 * |   Typedefs
 * +----------------------------------------------------------------------------
 */

typedef struct _applications
{
    unsigned int  aidLen;
    unsigned char  aid[32];
    unsigned int  appLabelLen;
    unsigned char  appLabel[32];
}s_AppInfo_t, *ps_AppInfo_t;


/*
 * EMVApplication Strcut
 */
typedef struct emv_application{
    unsigned int appnum;
    ps_AppInfo_t pAppinfo;
}s_EMV_AppList_t, *ps_EMV_AppList_t;

typedef enum
{
	EMV_PIN_TYPE_START = 0,

	EMV_PIN_TYPE_ONLINE,
	EMV_PIN_TYPE_OFFLINE,
	EMV_PIN_TYPE_LAST_OFFLINE,
	EMV_PIN_TYPE_MANAGE,
	EMV_PIN_TYPE_REINPUT,
	EMV_PIN_TYPE_INPUT_NEWPIN,
	EMV_PIN_TYPE_REINPUT_NEWPIN,
	EMV_PIN_TYPE_INPUT_OFFLINE_NEWPIN,
	EMV_PIN_TYPE_REINPUT_OFFLINE_NEWPIN,

	EMV_PIN_TYPE_NUMS,
}Emv_Pin_Type_t;


typedef enum{
    PIN_CONFIRM = 0,
    PIN_CANCEL,
    PIN_TIMEOUT,
    PIN_BYPASS,
}e_InPutPin_t;

extern u32 get_tlv_value(pu8 pool,u32 T,pu8 V );
extern void set_tlv_view(u8 *pTlvBuf, u32 dwTag, T_U8_VIEW uvVal);
extern T_U8_VIEW get_tlv_view(u8 *pTlvBuf, u32 dwTag);
extern int _asc_2_bcd(unsigned char * ascstr, int asclen, unsigned char * bcdstr, int align);
extern u32 asc_to_bcd(pu8 src,u32 len,pu8 dest );

extern u32 WaitEvents(u32 events,u32 timeout,pvoid pbuffer );

#ifdef __cplusplus
}
#endif//__cplusplus

#endif  /* #ifndef _TRANS_CBK_H */
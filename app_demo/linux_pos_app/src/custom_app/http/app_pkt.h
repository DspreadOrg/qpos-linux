/*
********************************************************************************
*
*   File Name:
*       app_server.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _APP_PKT_H
#define _APP_PKT_H

#include "app_includes.h"

/********GLOBLE TAG******/
typedef enum
{
    TAG_START = (APP_TAG_START+0x200),
    TAG_TRANS_TYPE,

    TAG_TRANS_AMOUNT,
    TAG_TRANS_TOTAL_AMOUNT,
    TAG_PAN,
    TAG_EMV_DATA,
    TAG_PIN_BLOCK,
    TAG_PIN_KSN,


    TAG_GATEWAY_RESPONSE_CODE,
    TAG_EMV_RESPONSE,

    TAG_END
}App_Tag_t;

typedef enum
{
    REQ_INIT=0x00,
    REQ_LOGIN,
    REQ_TRANS_SALE,
    REQ_TRANS_REFUND,
    REQ_TRANS_REVERSEL,
    REQ_TRANS_VOID,

    REQ_TRANS_END,

}Req_Trans_Type;


typedef enum
{
    STRINGS = 0,
    INTEGER,
    NUMBER,
}T_JSON_VALUE_MODE;

typedef struct
{
    Req_Trans_Type trans_type;
    char *url;

}T_TRANS_URL;

typedef struct
{
    App_Tag_t tag;
    char *jsonKeyName;
    u32 keyNameLen;
    T_JSON_VALUE_MODE mode;
}T_TRANS_TAG_JSON_DATA;

typedef struct{
	Rc_t Rc;
	int error_code;
	char title_msg[64+1];
	char description[64+1];
	char authorization_code[32+1];		
}ONLINE_STATUS;


Rc_t online_common(pu8 pTradingFile);

#endif
#ifndef _APP_TRANS_H
#define _APP_TRANS_H
#include "app_includes.h"

#define TRANS_POOL_MAX     (1024)


typedef struct
{
    u32 tag;
    u32 len;
    pu8 value;
}TRANS_T;

typedef enum
{
    CALLBACK_TRANS_MAG=0,
    CALLBACK_TRANS_ONLINE_PIN,
    CALLBACK_TRANS_OFFLINE_PIN,
    CALLBACK_TRANS_NFC_ONLINE,
    CALLBACK_TRANS_NFC_APPROVED,
    CALLBACK_TRANS_NFC_DECLINE,
    CALLBACK_TRANS_OFFLINE_LASTE_PIN,
    CALLBACK_TRANS_REVERSAL,
    CALLBACK_TRANS_APP_SELECT,
    CALLBACK_TRANS_MAG_ACCOUNT_TYPE,
    CALLBACK_TRANS_GAC2,
    CALLBACK_TRANS_ICC_ONLINE,
    CALLBACK_TRANS_PROCESS_CUSTOM,

    CALLBACK_END
}Callback_Type_t;

void Trans_Payment();
void TransactionProcess(unsigned char *pTradingFile);

int DoTrans();
int Req_token();
#endif
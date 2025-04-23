#pragma once

#include "osal.h"
#include "utility.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define POS_OFFLINE 1

#define POS_PAPER_TRADING 0  

#define dbg_printf(...)                    do{ \
    OsLog (LOG_DEBUG,__VA_ARGS__); \
}while(0);

#define dbg_printfWHex(pszHex,HexLen,...)      do{ \
	OsLog (LOG_DEBUG,__VA_ARGS__); \
    if(HexLen>0){OsLog(LOG_DEBUG,"%s",debug_hex2string(pszHex,HexLen));}\
}while(0); 

#define TERM_SN   "00004102EBA532"
#define SOFTWARE_VERSION  "V1.0-20230913"

// Modify according to the actual parameters provided by UnionPay
// UnionPay Master Key
#define YL_MK    "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33" 
// UnionPay Merchant Number
#define YL_MCH   "123456789012345"
// UnionPay terminal number
#define YL_TERM   "12345678"
// UnionPay TPDU
#define YL_TPDU  "6006010000"
// UnionPay Server IP
#define YL_IP    "120.204.69.139"
// UnionPay server port
#define YL_PORT  30000

#ifdef __cplusplus
}
#endif

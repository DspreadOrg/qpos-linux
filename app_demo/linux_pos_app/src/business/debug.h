#pragma once

#include "osal.h"
#include "utility.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define POS_OFFLINE 1

#define dbg_printf(...)                    do{ \
    OsLog (LOG_DEBUG,__VA_ARGS__); \
}while(0);

#define dbg_printfWHex(pszHex,HexLen,...)      do{ \
	OsLog (LOG_DEBUG,__VA_ARGS__); \
    if(HexLen>0){OsLog(LOG_DEBUG,"%s",debug_hex2string(pszHex,HexLen));}\
}while(0); 

#define TERM_SN   "00004102EBA532"
#define SOFTWARE_VERSION  "V1.0-20230913"

//按银联提供的实际参数修改
//银联主密钥
#define YL_MK    "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33" 
//银联商户号
#define YL_MCH   "123456789012345"
//银联终端号
#define YL_TERM   "12345678"
//银联TPDU
#define YL_TPDU  "6006010000"
//银联服务器IP
#define YL_IP    "120.204.69.139"
//银联服务器端口
#define YL_PORT  30000

#ifdef __cplusplus
}
#endif

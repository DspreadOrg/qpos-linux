#ifndef _APP_GLOBAL_H
#define _APP_GLOBAL_H

#include "appinc.h"
#include "ui_main.h"
#define SIZE_ARRAY(x)  						(sizeof(x)/sizeof(*x))

enum {
    LV_KEY_0 = '0',  
    LV_KEY_1,  
    LV_KEY_2,  
    LV_KEY_3,  
    LV_KEY_4,  
    LV_KEY_5,  
    LV_KEY_6, 
    LV_KEY_7,   
    LV_KEY_8,  
    LV_KEY_9,  
};
typedef uint8_t lv_key_num_t;

extern u32 str2num(char *strAmt, u32 dwLen);
extern char *uv2str(char *strRet, T_U8_VIEW *uv);
extern int uvcmpstr(T_U8_VIEW *uv, char *str);
extern u32 su2num(T_U8_VIEW *uv);
extern char *str_in_uv(T_U8_VIEW uvSrc, char *sub);
extern u32 uv2asc(u8 *pAsc, T_U8_VIEW uvBin);
extern u32 c2_2_uint(pu8  c2);
extern u32 c4_2_uint(pu8  c4);

extern void FormatAmount(T_U8_VIEW uvAmt);


extern char nameOperator[20];
#endif
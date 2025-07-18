#ifndef _APP_GLOBAL_H
#define _APP_GLOBAL_H

#include "appinc.h"
#include "lvgl_porting.h"
#include "ui_styles.h"
#include "ui_utils.h"
#include "ui_main.h"
#include "../image/lv_image.h"
#include "../lvgl/lvgl.h"

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

extern lv_group_t * s_group_keypad_indev;
extern lv_obj_t * Main_Panel;
extern lv_obj_t * Icon_wifi;
extern lv_obj_t * Icon_gprs;
extern lv_obj_t * Icon_usb;
extern lv_obj_t * Icon_time;
extern lv_obj_t * Icon_batt_percent;
extern lv_obj_t * Icon_Gprs_Conn_status;

extern lv_obj_t * imgBack;
extern lv_obj_t * imgDelete;
extern lv_obj_t * imgCollect;
extern lv_obj_t * imgContinue;
extern lv_obj_t * imgKnowMore;
extern lv_obj_t * imgSend;

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

extern u8 strApList[1024];
#endif
#ifndef _UI_STYLES_H
#define _UI_STYLES_H

#include "app_includes.h"
#include "lvgl_porting.h"

#define TITLE_COLOR         0x121E6C
#define CARD_COLOR          0xFFFFFF
#define BTN_HEADER_COLOR    0x969BBD
#define SHADOW_COLOR        0xD2D4E1
#define SIGN_FIELD_COLOR    0xF1F2F6
#define SIGN_LINE_COLOR     0x000000
#define DISABLED_COLOR      0x969696
#define OK_ICON             "OK_ICON"

LV_FONT_DECLARE(lv_symbol_signal_14);
LV_FONT_DECLARE(lv_symbol_signal_wifi);

extern lv_style_t title_style;
extern lv_style_t btn_pre_style;
extern lv_style_t Icon_style_signal;
extern lv_style_t input_style;
extern lv_style_t inputPin_style;
extern lv_style_t mediumMsg_style;
extern lv_style_t bigMsg_style;
extern lv_style_t lightLabel_style;
extern lv_style_t littleLabel_style;
extern lv_style_t bigLabel_style;
extern lv_style_t alertMessage_style;
extern lv_style_t message_style;
extern lv_style_t message_disabled_style;
extern lv_style_t btn_style;
extern lv_style_t btn_header_style;
extern lv_style_t Icon_style_signal_wifi;
extern lv_style_t Icon_style_conn_type;
extern lv_style_t style_keypad;
extern lv_style_t btn_list_style;
extern lv_style_t gStyleNotBorder;
extern lv_style_t gStyleSignature;
extern void ui_init_styles();

#endif

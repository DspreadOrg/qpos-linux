#include "ui_styles.h"
#include "app_global.h"
								
lv_style_t title_style;
lv_style_t btn_pre_style;
lv_style_t Icon_style_signal;
lv_style_t Icon_style_conn_type;
lv_style_t Icon_style_signal_wifi;
lv_style_t input_style;
lv_style_t inputPin_style;
lv_style_t mediumMsg_style;
lv_style_t bigMsg_style;
lv_style_t lightLabel_style;
lv_style_t littleLabel_style;
lv_style_t bigLabel_style;
lv_style_t alertMessage_style;
lv_style_t message_style;
lv_style_t message_disabled_style;
lv_style_t btn_style;
lv_style_t btn_header_style;
lv_style_t style_keypad;
lv_style_t btn_list_style;
lv_style_t gStyleNotBorder;
lv_style_t gStyleSignature;

void ui_init_styles();

/*----------------------------------------------------------------------------
|   Function Name:
|       ui_init_styles
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void ui_init_styles()
{
	lv_style_init(&Icon_style_signal);
    lv_style_set_text_font(&Icon_style_signal, &lv_symbol_signal_14);
    lv_style_set_text_color(&Icon_style_signal, lv_color_black());

    lv_style_init(&Icon_style_conn_type);
    lv_style_set_text_font(&Icon_style_conn_type, &lv_font_montserrat_20);
    lv_style_set_text_color(&Icon_style_conn_type, lv_color_black());

    lv_style_init(&Icon_style_signal_wifi);
    lv_style_set_text_font(&Icon_style_signal_wifi, &lv_symbol_signal_wifi);
    lv_style_set_text_color(&Icon_style_signal_wifi, lv_color_black());

	lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, &lv_font_montserrat_20);
    lv_style_set_text_align(&title_style, LV_TEXT_ALIGN_CENTER);
	lv_style_set_text_color(&title_style, lv_color_hex(TITLE_COLOR));
	
	lv_style_init(&btn_pre_style);
    lv_style_set_bg_color(&btn_pre_style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_width(&btn_pre_style, 6);
    lv_style_set_border_color(&btn_pre_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_bg_opa(&btn_pre_style, LV_OPA_COVER);
	
	lv_style_init(&lightLabel_style);
	lv_style_set_text_font(&lightLabel_style, &lv_font_montserrat_14);
	lv_style_set_text_color(&lightLabel_style, lv_palette_darken(LV_PALETTE_GREY, 1));
	
	lv_style_init(&littleLabel_style);
	lv_style_set_text_font(&littleLabel_style, &lv_font_montserrat_14);
	
	lv_style_init(&bigLabel_style);
	lv_style_set_text_font(&bigLabel_style, &lv_font_montserrat_30);
    
    lv_style_init(&mediumMsg_style);
    lv_style_set_text_font(&mediumMsg_style, &lv_font_montserrat_20);
    lv_style_set_text_align(&mediumMsg_style, LV_TEXT_ALIGN_CENTER);
    
    lv_style_init(&bigMsg_style);
    lv_style_set_text_font(&bigMsg_style, &lv_font_montserrat_30);
    lv_style_set_text_align(&bigMsg_style, LV_TEXT_ALIGN_CENTER);	
	
	lv_style_init(&inputPin_style);
    lv_style_set_text_font(&inputPin_style, &lv_font_montserrat_30);
    lv_style_set_text_align(&inputPin_style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_border_opa(&inputPin_style,LV_OPA_TRANSP);
    lv_style_set_outline_opa(&inputPin_style,LV_OPA_TRANSP);
	lv_style_set_bg_color(&inputPin_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_style_set_border_color(&inputPin_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
  
	lv_style_init(&message_style);
	lv_style_set_text_font(&message_style, &lv_font_montserrat_16);
	lv_style_set_text_align(&message_style, LV_TEXT_ALIGN_CENTER);	

    lv_style_init(&message_disabled_style);
	lv_style_set_text_font(&message_disabled_style, &lv_font_montserrat_16);
	lv_style_set_text_align(&message_disabled_style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&message_disabled_style, lv_color_hex(DISABLED_COLOR));	
	
	lv_style_init(&alertMessage_style);
	lv_style_set_text_font(&alertMessage_style, &lv_font_montserrat_16);
	lv_style_set_text_align(&alertMessage_style, LV_TEXT_ALIGN_CENTER);
	lv_style_set_text_color(&alertMessage_style, lv_palette_darken(LV_PALETTE_RED, 3));

	lv_style_init(&input_style);
    lv_style_set_text_font(&input_style, &lv_font_montserrat_30);
    lv_style_set_text_align(&input_style, LV_TEXT_ALIGN_CENTER);
	lv_style_set_bg_color(&input_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_style_set_border_color(&input_style, lv_palette_lighten(LV_PALETTE_GREY, 5));    

    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, 4);
    lv_style_set_bg_opa(&btn_style, LV_OPA_COVER);
	lv_style_set_bg_color(&btn_style, lv_color_hex(CARD_COLOR));
    lv_style_set_border_color(&btn_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
    lv_style_set_border_width(&btn_style, 1);
    lv_style_set_shadow_ofs_x(&btn_style, 0);
    lv_style_set_shadow_ofs_y(&btn_style, 4);
    lv_style_set_shadow_width(&btn_style, 10);
	lv_style_set_shadow_color(&btn_style, lv_color_hex(SHADOW_COLOR));

    lv_style_init(&btn_header_style);
    lv_style_set_text_font(&btn_header_style, &lv_font_montserrat_16);
    lv_style_set_text_color(&btn_header_style, lv_color_hex(BTN_HEADER_COLOR));

    lv_style_init(&style_keypad);
    lv_style_set_text_font(&style_keypad, &lv_font_montserrat_18);
    lv_style_set_text_align(&style_keypad, LV_TEXT_ALIGN_CENTER);
	lv_style_set_bg_color(&style_keypad, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_style_set_border_color(&style_keypad, lv_palette_lighten(LV_PALETTE_GREY, 5));  

    lv_style_init( &gStyleSignature );
	lv_style_set_radius( &gStyleSignature, 10 );
	lv_style_set_bg_opa( &gStyleSignature, LV_OPA_COVER );

    lv_style_init(&btn_list_style);
    lv_style_set_border_width(&btn_list_style,0);
    lv_style_set_bg_color(&btn_list_style, lv_color_hex(CARD_COLOR));

    lv_style_init( &gStyleNotBorder );
    lv_style_set_border_width( &gStyleNotBorder, 0 );
}

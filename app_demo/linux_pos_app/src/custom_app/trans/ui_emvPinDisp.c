#include "ui_emvPinDisp.h"


static char LocalMask[ 13 ] = { 0 };

static EPinType LocalPinType = PinTypeUndef;

void emvPinDisp(unsigned char *maskPin)
{
    memset(LocalMask,0,sizeof(LocalMask));
	strcpy(LocalMask,maskPin);
    GuiDisplay(LCD_DISP_PIN_MASK);
}

void SetPinType( EPinType type ){
    LocalPinType = type;
    memset( LocalMask, 0x00, sizeof( LocalMask ) );
}

void UpdatePinMask( pvoid param ){
    const pLcd_Pin_Mask_t pLcdPinMask = (pLcd_Pin_Mask_t)param;
    strcpy( LocalMask, pLcdPinMask->pinMask );
}

void DispPinMask( void ){
    DispPinEntry( );
}

void DispPinEntry( void ){
    if( LocalPinType == PinTypeUndef ){
        return;
    }

	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

    lv_text_create(Main_Panel, "Please input pin", &title_style, LV_ALIGN_TOP_MID, 0, 5);

	lv_obj_t * input = lv_textarea_create(Main_Panel);
	lv_textarea_set_max_length(input, 6);
	lv_obj_add_style(input, &input_style, LV_STATE_DEFAULT);
	lv_textarea_set_one_line(input, true);
	lv_textarea_set_align(input, LV_TEXT_ALIGN_CENTER);
	lv_obj_align(input, LV_ALIGN_CENTER, 0, 0);

    imgDelete = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_MID, 0, 0);
    imgCollect = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    u32 len = strlen( LocalMask );

    if(len > 0){
        lv_textarea_set_text( input, LocalMask );
    }

	lv_timer_enable(true);
}

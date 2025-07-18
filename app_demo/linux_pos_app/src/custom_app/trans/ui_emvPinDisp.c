#include "ui_emvPinDisp.h"


static char LocalMask[ 13 ] = { 0 };
static lv_obj_t * pin_lable = NULL;

void InputPinCallback(int pinLen)
{
    char start[] = "****************";
    memset(LocalMask,0,sizeof(LocalMask));
    if(pinLen > 0){
        memcpy(LocalMask,start,pinLen);
    }
    GuiEventRegister(LCD_DISP_PIN_MASK);
}

void ClearMaskPin(){
    memset( LocalMask, 0x00, sizeof( LocalMask ) );
}

void DispPinMask()
{
    if(strlen(LocalMask) > 0)
    {
        lv_textarea_set_text(pin_lable, LocalMask);
    }
    else
    {
        lv_textarea_set_text(pin_lable, " ");
    }
}

void DispPinEntry( void ){

	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

    lv_text_create(Main_Panel, "Please input pin", &title_style, LV_ALIGN_TOP_MID, 0, 5);

	pin_lable = lv_textarea_create(Main_Panel);
	lv_textarea_set_max_length(pin_lable, 12);
	lv_obj_add_style(pin_lable, &input_style, LV_STATE_DEFAULT);
	lv_textarea_set_one_line(pin_lable, true);
	lv_textarea_set_align(pin_lable, LV_TEXT_ALIGN_CENTER);
	lv_obj_align(pin_lable, LV_ALIGN_CENTER, 0, 0);

    imgDelete = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_MID, 0, 0);
    imgCollect = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_textarea_set_text( pin_lable,"");     
    
	lv_timer_enable(true);
}

static void OfflinePinEntry_cb(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
	lv_indev_t * indev = lv_indev_get_act();
	lv_indev_type_t indev_type = lv_indev_get_type(indev);
	u8 pwd[30 + 1]={0};
	 uint32_t key=-1;
	if((indev_type==LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY) )
	{
        key =lv_indev_get_key(lv_indev_get_act());

	}
    lv_obj_t * ta = lv_event_get_target(event);
    const char * txt = lv_textarea_get_text(ta);
    if(code == LV_EVENT_CANCEL ||key == LV_KEY_ESC){
        get_transaction_data()->emv_enter_offline_pin_result = 0;

    }else if(code == LV_EVENT_READY || key == LV_KEY_ENTER){
        if(strlen(txt)>0){
            get_transaction_data()->emv_enter_offline_pin_result = 1;
            strcat(get_transaction_data()->sPin,txt);
        }
    }

} 

void DispOfflinePinEntry( void ){

	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

    lv_text_create(Main_Panel, "Please input offline pin", &title_style, LV_ALIGN_TOP_MID, 0, 5);

	lv_obj_t * offlinePIn = lv_textarea_create(Main_Panel);
	lv_textarea_set_max_length(offlinePIn, 12);
	lv_obj_add_style(offlinePIn, &input_style, LV_STATE_DEFAULT);
	lv_textarea_set_one_line(offlinePIn, true);
	lv_textarea_set_align(offlinePIn, LV_TEXT_ALIGN_CENTER);
	lv_obj_align(offlinePIn, LV_ALIGN_CENTER, 0, 0);
    lv_textarea_set_password_mode(offlinePIn,true);

    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(offlinePIn, OfflinePinEntry_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(s_group_keypad_indev,offlinePIn);

    imgDelete = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_MID, 0, 0);
    imgCollect = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_RIGHT, 0, 0);

	lv_timer_enable(true);
}

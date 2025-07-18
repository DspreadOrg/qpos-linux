#include "app_ota.h"
#include "socket.h"
#include "tms_cfg.h"

u32 g_offset = 0;
static lv_obj_t *percentage_lable = NULL;
static lv_obj_t * Upgrade_bar = NULL;
static int process = 0;

void DispOtaCheck()
{
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_text_create(Main_Panel, "Ota Checking", &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);
	lv_timer_enable(true);
}

void DispOtaUpgrade()
{
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_text_create(Main_Panel, "Ota Upgrading", &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);
	lv_timer_enable(true);
}

void UpdateOTAProcess()
{
    DSP_Debug();
    unsigned char dispBuff[16] = {0};
    sprintf(dispBuff,"%d%%",process);
    if(percentage_lable)
        lv_label_set_text(percentage_lable, dispBuff);
    if(Upgrade_bar)
    {
        lv_bar_set_range(Upgrade_bar,0,100);
        lv_bar_set_value(Upgrade_bar, process, LV_ANIM_OFF);
    }

}
static void event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj= lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    lv_point_t txt_size;
    //lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;
}

void DispDownloading()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_text_create(Main_Panel, "Ota Downloading", &mediumMsg_style, LV_ALIGN_CENTER, 0, -20);

    percentage_lable = lv_label_create(Main_Panel);
    lv_label_set_text(percentage_lable, " ");
    lv_obj_align(percentage_lable, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_style_text_color(percentage_lable, lv_color_hex(0x1B1B1B ), 0);

    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));

    Upgrade_bar = lv_bar_create(Main_Panel);
    lv_obj_add_event_cb(Upgrade_bar, event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_size(Upgrade_bar, LV_PCT(80), 20);
    lv_obj_align(Upgrade_bar,LV_ALIGN_CENTER, 0,20);
    lv_obj_add_style(Upgrade_bar, &style_indic, LV_PART_INDICATOR);

    DSP_Debug();

	lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       download_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void download_cb(lv_event_t * event) 
{
	lv_event_code_t code = lv_event_get_code(event);
	lv_indev_t * indev = lv_indev_get_act();
	lv_indev_type_t indev_type = lv_indev_get_type(indev);
	if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY) 
	{
		uint32_t key=lv_indev_get_key(indev);
		switch(key) 
		{
            case LV_KEY_ESC:
                DispMenuOptions();
                break;
                
            case LV_KEY_ENTER:
                EventRegister(EVENT_OTA_DOWNLOAD);
				break;
            default:				
            	break;
		}
	}
}

void dispOtaCheckResult() 
{
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel); 

	lv_obj_t * Icon_btn = lv_imgbtn_create(Main_Panel);
	lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(Icon_btn, download_cb, LV_EVENT_ALL,NULL);
	lv_group_add_obj(s_group_keypad_indev,Icon_btn);

	lv_icon_create(Main_Panel, &lv_Alert_Icon, LV_ALIGN_TOP_MID, 0, 15);
	lv_text_create(Main_Panel, "OTA", &mediumMsg_style, LV_ALIGN_CENTER, 0, -20);
	
    lv_text_create(Main_Panel, "\n\nNew App needs to be updated. \n Update it now?", &message_style, LV_ALIGN_CENTER, 0, 5);

    // Suspend, Update icons and connect icon     
	lv_icon_create(Main_Panel, &lv_Suspend_Icon, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    
    lv_obj_t * imgUpdate = lv_img_create(Main_Panel);
    lv_img_set_src(imgUpdate, &lv_Update_Icon);
    lv_obj_align(imgUpdate, LV_ALIGN_BOTTOM_RIGHT, 0, 0); 

	lv_timer_enable(true);
}

static void timerOut_cb()
{
	DispMenuOptions();
}

void ui_version_not_found()
{
	u32 timeout=2000;
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_obj_t * imgOKIcon = lv_img_create(Main_Panel);
	lv_img_set_src(imgOKIcon, &lv_Fail_Icon);
	lv_obj_align(imgOKIcon, LV_ALIGN_TOP_MID, 0, 20);

	lv_text_create(Main_Panel, "There is no new version available", &mediumMsg_style, LV_ALIGN_CENTER, 0, -5);
	lv_timer_t * timer= lv_timer_create(timerOut_cb, timeout,(pvoid)timeout);
	lv_timer_set_repeat_count(timer, 1);
	lv_timer_enable(true);
}

void ui_sn_not_found()
{
	u32 timeout=2000;
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_obj_t * imgOKIcon = lv_img_create(Main_Panel);
	lv_img_set_src(imgOKIcon, &lv_Fail_Icon);
	lv_obj_align(imgOKIcon, LV_ALIGN_TOP_MID, 0, 20);

	lv_text_create(Main_Panel, "The device is not registered", &mediumMsg_style, LV_ALIGN_CENTER, 0, -5);
	lv_timer_t * timer= lv_timer_create(timerOut_cb, timeout,(pvoid)timeout);
	lv_timer_set_repeat_count(timer, 1);
	lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       ui_tms_fail
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void ui_tms_fail_cb(lv_event_t * event) 
{
	lv_event_code_t code = lv_event_get_code(event);
	lv_indev_t * indev = lv_indev_get_act();
	lv_indev_type_t indev_type = lv_indev_get_type(indev);
	if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY) 
	{
		uint32_t key=lv_indev_get_key(indev);
		switch(key) 
		{
		
           case LV_KEY_ESC:
                	DispMenuOptions();
                break;
		}
	}
}

/*----------------------------------------------------------------------------
|   Function Name:
|       ui_Downloading_Fail
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void ui_Downloading_Fail()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel); 
   
    lv_obj_t * Icon_btn = lv_imgbtn_create(Main_Panel);
	lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(Icon_btn, ui_tms_fail_cb, LV_EVENT_ALL,NULL);
	lv_group_add_obj(s_group_keypad_indev,Icon_btn);

	lv_icon_create(Main_Panel, &lv_Fail_Icon, LV_ALIGN_TOP_MID, 0, 15);
	lv_text_create(Main_Panel, "Download Fail", &mediumMsg_style, LV_ALIGN_CENTER, 0, -5);   	

    lv_timer_enable(true);	
}

void Firmware_OTA()
{
    Rc_t result = RC_FAIL;

    GuiEventRegister(LCD_DISP_OTA_DOWNLODING);
    //result = tms_client(TMS_FW_HEART_CUSTOM_URL, true);
    result = larktms_client(TMS_FW_HEART_CUSTOM_URL,APP_VERSION);
    if(result  != TMS_ERR_OK)
    {
        GuiEventRegister(LCD_DISP_OTA_UPDATE_FAIL);
    }

}

void Firmware_OTA_Check()
{
    Rc_t result = RC_FAIL;
    process = 0;

    static LarkTmsCallBack_t larktmsCbk;
    larktmsCbk.ssl_connect = ssl_server_connect;
    larktmsCbk.ssl_disconnect= ssl_server_disconnect;
    larktmsCbk.ssl_send = ssl_send_msg;
    larktmsCbk.ssl_recv = ssl_recv_msg;

    larktms_ssl_Init(&larktmsCbk);
    LarkTms_Disp_Callback_Register(TmsDispCallback);

    result = larktms_client_check(TMS_FW_HEART_CUSTOM_URL,APP_VERSION);

    switch(result)
    {
        case TMS_ERR_OK:
            GuiEventRegister(LCD_DISP_OTA_FIRMWARE_NEED_DOWNLOAD);
            break;
        case TMS_ERR_CONNECT:
            GuiEventRegister(LCD_DISP_OTA_UPDATE_FAIL);
            break;
        case TMS_ERR_NO_AVAIALABLE:
            GuiEventRegister(LCD_DISP_OTA_VERSION_NOT_FOUND);
            break;
        case TMS_ERR_SN_NOT_REGISTER:
            GuiEventRegister(LCD_DISP_SN_NOT_REGIESTER);
            break;
        default:
            GuiEventRegister(LCD_DISP_OTA_UPDATE_FAIL);
            break;    
    }
}

void TmsDispCallback(u32 id, char *pMsg)
{
    u32 i ;
    char dispBuff[8] = {0};
    int curValue = 0;
    OsLog(LOG_DEBUG,"Dspread: %s:id=%d",__FUNCTION__,id);
    switch(id)
    {
        case TMS_DISP_START_CHECK:
            GuiEventRegister(LCD_DISP_OTA_CHECK);
            break;
        case TMS_DISP_START_DOWNLOAD:
            GuiEventRegister(LCD_DISP_OTA_DOWNLOAD_DISP);
            break;
        case TMS_DISP_DOWNLOADING_PROGRESS:
            process = atoi(pMsg);
            GuiEventRegister(LCD_DISP_OTA_DOWNLODING);
            break;
        case TMS_DISP_UPGRADING:
            GuiEventRegister(LCD_DISP_OTA_UPGRADING);
        break;
        default:
            break;
    }

}
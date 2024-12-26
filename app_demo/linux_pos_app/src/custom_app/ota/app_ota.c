#include "app_ota.h"


void DispOtaCheck()
{
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_text_create(Main_Panel, "Ota Checking", &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);
	lv_timer_enable(true);

    EventRegister(EVENT_OTA_CHECK);
}

void DispDownloading()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_text_create(Main_Panel, "Ota Downloading", &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);
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
            case LV_KEY_BACKSPACE:
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
		
           case LV_KEY_BACKSPACE:
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

    GuiEventRegister(LCD_DISP_OTA_FIRMWARE_DOWNLODING);
    //result = tms_client(TMS_FW_HEART_CUSTOM_URL, true);
    result = larktms_client(TMS_FW_HEART_CUSTOM_URL,APP_VERSION);
    if(result == RC_FAIL)
    {
        GuiEventRegister(LCD_DISP_OTA_UPDATE_FAIL);
    }

}

void Firmware_OTA_Check()
{
    set_user_appver(APP_VERSION);
 
    Rc_t result = RC_FAIL;

    //result = tms_client_check(TMS_FW_HEART_CUSTOM_URL);
    result = larktms_client_check(TMS_FW_HEART_CUSTOM_URL,APP_VERSION);

    if(result == RC_DOWN)
        GuiEventRegister(LCD_DISP_OTA_FIRMWARE_NEED_DOWNLOAD);
    else if(result == RC_SUCCESS)
        GuiEventRegister(LCD_DISP_OTA_VERSION_NOT_FOUND);
    else if(result == RC_NET_ERR)
        GuiEventRegister(LCD_DISP_OTA_UPDATE_FAIL);
    else if(result == RC_QUIT)
        GuiEventRegister(LCD_DISP_SN_NOT_REGIESTER);    
    else 
        GuiEventRegister(LCD_DISP_OTA_UPDATE_FAIL);
}

void TmsDispCallback(u32 id,u32 offset,u32 Length,void *pMsg)
{
    u32 i ;
    OsLog(LOG_DEBUG,"Dspread: %s:id=%d",__FUNCTION__,id);
    switch(id)
    {
        case OTA_SYNC_SERVER:
            GuiEventRegister(LCD_DISP_OTA_SYNC_TMS);
            break;
        case OTA_PROCESSING:
            //i = offset*100/Length;
            GuiEventRegister(LCD_DISP_START_DOWNLOADING);
            break;
        case OTA_START_DOWNLOAD:
            GuiEventRegister(LCD_DISP_START_DOWNLOADING);
            break;
        case OTA_UPDATE_FINISH:
            break;
        case OTA_FIRMWARE_VERIFY:
            GuiEventRegister(LCD_DISP_FIRMWARE_VERIFY); 
            break;
        case OTA_FIRMWARE_VERIFY_FAIL:
            GuiEventRegister(LCD_DISP_FIRMWARE_VERDIFY_FAIL);
            break;
        case OTA_OTHER:
            GuiEventRegister(LCD_DISP_WELCOME);
            break;
        case OTA_SYNC_ERROR_MSG:
            break;
        case OTA_SYNC_PARAM_DATA:
            break;
        case OTA_NO_AVAILABLE_FIRMWARE:
                GuiEventRegister(LCD_DISP_NO_AVAILABLE_FIRMWARE);
            break;
        default:
            break;
    }

}
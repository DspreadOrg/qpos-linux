#include "ui_setting.h"
#include "ui_wifi.h"

static void Help_cb(lv_event_t * event);
/*----------------------------------------------------------------------------
|   Function Name:
|       SettingOptions_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void SettingOptions_cb(lv_event_t * event)
{	    
    lv_event_code_t code = lv_event_get_code(event);
    lv_indev_t * indev = lv_indev_get_act();
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    uint32_t key = EVENT_KEY_NONE;

    if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
        key = lv_indev_get_key(indev); 
    else if(indev_type==LV_INDEV_TYPE_POINTER && code == LV_EVENT_CLICKED)
        key = lv_get_btn_key(event);  

    switch(key)
    {	        
        case LV_KEY_1:
            DispWifiSetting();
            break;
        
        case LV_KEY_2:
            DispSimSetting();
            break;
        
        case LV_KEY_3:
            DispAbout();
            break;

        case LV_KEY_4:
                //ui_download_update();
            break;

        case LV_KEY_5:
            //DispConnectDisconnectSim();
            break;

        case LV_KEY_DOWN:
            DispMenuOptions();
            break;		   
        case LV_KEY_ENTER:
            Trans_Payment();
            break;

        default:				
            break;
    }
}

void DispSettingOptions()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
    lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_text_create(Main_Panel, "Setting", &title_style, LV_ALIGN_TOP_MID, 0, -3);

    lv_obj_t * btn_list = lv_btn_list_create(340, 155, LV_ALIGN_CENTER, 20, 5);
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_add_btn(btn_list, SettingOptions_cb, 86, 56, "1", "Wi-Fi", LV_ALIGN_CENTER, 0, 0, true);
    lv_add_btn(btn_list, SettingOptions_cb, 86, 56, "2", "GPRS", LV_ALIGN_CENTER, 0, 0, true);
    lv_add_btn(btn_list, SettingOptions_cb, 86, 56, "3", "About", LV_ALIGN_CENTER, 0, 0, true);

    lv_icon_create(Main_Panel, &lv_Menu_Icon, LV_ALIGN_BOTTOM_LEFT, 0, 0); 
    lv_icon_create(Main_Panel, "OK", LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	
    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       DispHelp
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispAbout()
{	    
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

    u8 ver[16] = {0};
    getKernelInsLibVer(ver);

    lv_text_create(Main_Panel, "About", &title_style, LV_ALIGN_TOP_MID, 0, -3);

	lv_obj_t * Icon_btn = lv_imgbtn_create(Main_Panel);    
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(Icon_btn,Help_cb, LV_EVENT_ALL,NULL);
    lv_group_add_obj(s_group_keypad_indev,Icon_btn);	    		

	// Left labels    
    lv_text_create(Main_Panel, "InsLib Ver:", &message_style, LV_ALIGN_TOP_LEFT, 0, 40);

	// Right labels
    lv_text_create(Main_Panel, ver, &message_style, LV_ALIGN_TOP_RIGHT, 0, 40);

    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       Help_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void Help_cb(lv_event_t * event)
{	    
    lv_event_code_t code = lv_event_get_code(event);

    lv_indev_t * indev = lv_indev_get_act();
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
    {
        uint32_t key=lv_indev_get_key(indev);
        if( key == LV_KEY_BACKSPACE )
        {
            DispSettingOptions();
        }
    }	
}
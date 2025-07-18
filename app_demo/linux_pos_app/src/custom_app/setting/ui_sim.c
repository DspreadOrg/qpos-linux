#include "app_global.h"
#include "app_setting.h"
#include "task_handle.h"
#include "ui_status_bar.h"
#include "ui_sim.h"
#include "ui_wifi.h"
#include "ui_main.h"

char* get_name_operator(char * nameOperator);
static void SimSetting_cb(lv_event_t * event);
static void SignalConnecting_cb(lv_timer_t * timer);
static void SimConnectionOK_cb();

/*----------------------------------------------------------------------------
 |   Function Name:
 |       get_name_operator
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
char* get_name_operator(char * nameOperator){
	char nameOP[32];
	memset(nameOperator,0x00,sizeof(nameOperator));
	memset(nameOP,0X00,sizeof(nameOP));
	OsWlGetSimOperator(nameOP);

	strcpy(nameOperator,nameOP);
	return nameOperator;
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DispSimConnectionOK
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void DispSimConnectionOK(const char * SimProviderName) 
{
	u32 timeout=1000;
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_text_create(Main_Panel, "GPRS Connect to", &mediumMsg_style, LV_ALIGN_CENTER, 0, -10);
	lv_text_create(Main_Panel, SimProviderName, &message_style, LV_ALIGN_CENTER, 0, 20);

    lv_timer_t * timer= lv_timer_create(SimConnectionOK_cb, timeout,(pvoid)timeout);
	lv_timer_set_repeat_count(timer, 1);

	lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       SimConnectionOK_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void SimConnectionOK_cb()
{
	DispMenuOptions();
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       SimConnecting_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void SimConnecting_cb(lv_timer_t * timer) 
{
	if(Get_dev_sim_status())
	{
		Apn_cfg();
		lv_timer_set_repeat_count(timer, 0);
		lv_timer_t * signalConnectingTimer= lv_timer_create(SignalConnecting_cb, 500, NULL);   
		lv_timer_set_repeat_count(signalConnectingTimer, 20);
	} 
	else 
	{
		if( timer->repeat_count == 0 )
			DispMenuOptions();
	}
}


/*----------------------------------------------------------------------------
 |   Function Name:
 |       SignalConnecting_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void SignalConnecting_cb(lv_timer_t * timer)
{
	if( Get_dev_gprs_signal_lvl() )
	{
		lv_timer_set_repeat_count(timer, 0);
		get_name_operator(nameOperator);
		DispSimConnectionOK(nameOperator);
	} 
	else 
	{
		if( timer->repeat_count == 0 ){
			DispMenuOptions();
		}
	}
}

/*----------------------------------------------------------------------------
|   Function Name:
|       DispConnectDisconnectSim
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispSimSetting()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE);

 	lv_text_create(Main_Panel, "Gprs Setting", &title_style, LV_ALIGN_TOP_MID, 0, 0);
 
	lv_obj_t * btn_list = lv_btn_list_create(330, 155, LV_ALIGN_CENTER, 30, 30);
    lv_group_remove_all_objs(s_group_keypad_indev);
	if(GetSimContnectStatus()){        
		lv_add_btn(btn_list, SimSetting_cb, 118, 62, "1", "Open\nAirplane", LV_ALIGN_CENTER, 0, 4, true);
    	lv_add_btn(btn_list, SimSetting_cb, 118, 62, "2", "Close\nAirplane", LV_ALIGN_CENTER, 0, 4, false);
    }else{        
		lv_add_btn(btn_list, SimSetting_cb, 118, 62, "1", "Open\nAirplane", LV_ALIGN_CENTER, 0, 4, false);
    	lv_add_btn(btn_list, SimSetting_cb, 118, 62, "2", "Close\nAirplane", LV_ALIGN_CENTER, 0, 4, true);    
    }

    lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       ConnectDisconnectSim_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void SimSetting_cb(lv_event_t * event)
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
		case LV_KEY_ESC:
			DispSettingOptions();
			break;
		
		case LV_KEY_1:	
			OpenAirplaneMode();
			break;
		case LV_KEY_2:
			CloseAirplaneMode();
			break;	
		default:				
			break;
	}
}


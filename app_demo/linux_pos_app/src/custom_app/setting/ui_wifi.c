#include "app_global.h"
#include "ui_wifi.h"
#include "ui_main.h"
#include "app_setting.h"
#include "ui_status_bar.h"
#include "task_handle.h"
#include "ui_sim.h"
#include "ui_setting.h"

void DispWifiList();
static void wifi_Select_cb(lv_event_t * event);

static void ConnectDisconnectWifi_cb(lv_event_t * event);

static void DispConnectwifi(const char * wifi_name);
static void WifiConnect_cb(lv_event_t * event);

static void DispWifiWarning(const char * info);
static void WifiWarning_cb(lv_event_t * event);

void DispWifiConnectionFail();
static void WifiConnectionFail_cb(lv_event_t * event);

static void DispWifiWarning(const char * info);
static void WifiWarning_cb(lv_event_t * event);

void DispWifiConnectionMessage(const char * message);
static void WifiConnectionOK_cb();

static void DispForgetWifi();
static void ForgetWifi_cb(lv_event_t * event);

void searchWifiNetworks();
void DispWifiConnected();

/*----------------------------------------------------------------------------
|   Function Name:
|       DispWifiList
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispWifiList()
{
    u32 i=0;
    lv_timer_enable(false);
    lv_obj_clean(Main_Panel);

    T_U8_VIEW   uvlist[60]= { {NULL, 0} };	
    int list_num = 0;
    memset(uvlist, 0x00, sizeof(uvlist));	
    list_num=AnalysisWifiList(strApList,uvlist);

    lv_text_create(Main_Panel, "Select Wi-Fi", &title_style, LV_ALIGN_TOP_MID, 0, -10);

    lv_obj_t * main_list = lv_obj_create(Main_Panel);
    lv_obj_set_size(main_list,310,155);
    lv_obj_add_style(main_list,&gStyleNotBorder,0);
    lv_obj_align(main_list,LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_flex_flow(main_list, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * btn_list;
    char wifi_disp[MAX_SIZE_NAME_WIFI]={0};
    lv_group_remove_all_objs(s_group_keypad_indev);
    for(i=0;i<list_num;i++)
    {
        memset(wifi_disp,0x00,MAX_SIZE_NAME_WIFI);
        memcpy(wifi_disp,uvlist[i].head,uvlist[i].len);
        OsLog(LOG_DEBUG,"--i=%d--%s",i,wifi_disp);
        btn_list = lv_list_btn(main_list, wifi_disp,NULL,NULL);
        lv_obj_add_style(btn_list,&btn_style, LV_STATE_DEFAULT);
        lv_obj_add_event_cb(btn_list, wifi_Select_cb, LV_EVENT_ALL, (pvoid)main_list);
        lv_group_add_obj(s_group_keypad_indev,btn_list);
    }

    lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       wifi_Select_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void wifi_Select_cb(lv_event_t * event)
{	    
    lv_event_code_t code = lv_event_get_code(event);
    lv_indev_t * indev = lv_indev_get_act();
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
	
    lv_obj_t * obj = lv_event_get_target(event);
    lv_obj_t * base=lv_event_get_user_data(event);
    const char * text=lv_list_get_btn_text(base, obj);
    unsigned char wifi_name[MAX_SIZE_NAME_WIFI]={0};
    if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
    {
        uint32_t key=lv_indev_get_key(indev);
        switch(key)
        {
            case LV_KEY_UP:
                lv_group_focus_prev(s_group_keypad_indev);
                break;
            
            case LV_KEY_DOWN:
                lv_group_focus_next(s_group_keypad_indev);
                break;
            
			case LV_KEY_ESC:
                DispSettingOptions();
                break;
            
            case LV_KEY_ENTER:
                strcat((char*)wifi_name,text);
                T_U8_VIEW uvSel={wifi_name,strlen((char*)wifi_name)};
                SetCfgPool(WIFI_NAME,uvSel);
                DispConnectwifi((char*)wifi_name);
                break;
				
        }
    }
    else if(indev_type==LV_INDEV_TYPE_POINTER && code == LV_EVENT_CLICKED)
    {
        strcat((char*)wifi_name,text);
        T_U8_VIEW uvSel={wifi_name,strlen((char*)wifi_name)};
        SetCfgPool(WIFI_NAME,uvSel);
        DispConnectwifi((char*)wifi_name);
    }	
}
/*----------------------------------------------------------------------------
|   Function Name:
|       DispConnectDisconnectWifi
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispWifiSetting()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
    lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_text_create(Main_Panel, "Wi-Fi", &title_style, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * btn_list = lv_btn_list_create(328, 155, LV_ALIGN_CENTER, 0, 30);
    lv_group_remove_all_objs(s_group_keypad_indev);

    if(get_wifi_conn_status())
    {
        char wifi_name[MAX_SIZE_NAME_WIFI];
        if(getWifiName(wifi_name) )
        {
            lv_obj_t * wifi_name_label = lv_text_create(Main_Panel, "", &message_style, LV_ALIGN_TOP_MID, 0, 30);
            lv_label_set_text_fmt(wifi_name_label, "Connect a %s", wifi_name);
            lv_obj_set_width(wifi_name_label, 250);
            lv_label_set_long_mode(wifi_name_label, LV_LABEL_LONG_SCROLL);
        }
    }

    lv_add_btn(btn_list, ConnectDisconnectWifi_cb, 78, 62, "1", "Connect\nWi-Fi", LV_ALIGN_CENTER, 0, 7, true);
    lv_add_btn(btn_list, ConnectDisconnectWifi_cb,  104, 62, "2", "Disconnect\nWi-Fi", LV_ALIGN_CENTER, 0, 7, true);
    lv_add_btn(btn_list, ConnectDisconnectWifi_cb, 78, 62, "3", "Forget\nWi-Fi", LV_ALIGN_CENTER, 0, 7, true);

    lv_obj_t * imgOK = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       ConnectDisconnectWifi_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void ConnectDisconnectWifi_cb(lv_event_t * event)
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
            DispProcess("Searching");
            EventRegister(EVENT_WIFI_SEARCH);
            break;

        case LV_KEY_2:
            Wifi_Disconnect();
            break;

        case LV_KEY_3:	
            DispForgetWifi();
            break;
        
        default:				
            break;
    }
}

/*----------------------------------------------------------------------------
|   Function Name:
|       Connect_wifi
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void DispConnectwifi(const char * wifi_name)
{
    lv_timer_enable(false);
    lv_obj_clean(Main_Panel);
    lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_text_create(Main_Panel, "Input Password", &title_style, LV_ALIGN_TOP_MID, 0, -10);
	        
    lv_obj_t *input = lv_textarea_create(Main_Panel);
    lv_obj_set_size(input,LV_PCT(101), 50);
    lv_style_set_bg_color(&mediumMsg_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_style_set_border_color(&mediumMsg_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_obj_add_style(input, &mediumMsg_style, LV_STATE_DEFAULT);

    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(input, WifiConnect_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(s_group_keypad_indev,input);

    lv_textarea_set_max_length(input, MAX_SIZE_PASSWORD_WIFI);
	lv_textarea_set_align(input, LV_TEXT_ALIGN_CENTER);
    lv_textarea_set_one_line(input, true);   
	lv_obj_align_to(input,Main_Panel,LV_ALIGN_TOP_MID, 0, 15);

    lv_obj_t *keypad=lv_keyboard_create(Main_Panel);
    lv_obj_add_style(keypad, &style_keypad, LV_STATE_DEFAULT);
    lv_obj_set_size(keypad,LV_PCT(108), 130);
    lv_obj_align(keypad, LV_ALIGN_BOTTOM_MID, 0, 10);
    lv_keyboard_set_textarea(keypad,input);	

    lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       WifiConnect_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void WifiConnect_cb(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
	lv_indev_t * indev = lv_indev_get_act();
	lv_indev_type_t indev_type = lv_indev_get_type(indev);
	u8 pwd[MAX_SIZE_PASSWORD_WIFI + 1]={0};
	 uint32_t key=-1;
	if((indev_type==LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY) )
	{ 
        key =lv_indev_get_key(lv_indev_get_act());

	}
    lv_obj_t * ta = lv_event_get_target(event);
    const char * txt = lv_textarea_get_text(ta);
    if(code == LV_EVENT_CANCEL ||key == LV_KEY_ESC){
        DispSettingOptions();

    }else if(code == LV_EVENT_READY || key == LV_KEY_ENTER){
        if(strlen(txt)>0){
            strcat((char*)pwd,txt);
            T_U8_VIEW uvPwd={(unsigned char*)txt,strlen(txt)};
            SetCfgPool(WIFI_PASSWORD,uvPwd);
            DispProcess("Connecting");
            EventRegister(EVENT_WIFI_CONNECT);
        }
    }

}
 
/*----------------------------------------------------------------------------
|   Function Name:
|       DispWifiWarning
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void DispWifiWarning(const char * info)
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);    

	lv_obj_t * Icon_btn = lv_imgbtn_create(Main_Panel);    
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(Icon_btn, WifiWarning_cb, LV_EVENT_ALL,NULL);
    lv_group_add_obj(s_group_keypad_indev,Icon_btn);
    
    lv_icon_create(Main_Panel, &lv_Alert_Icon, LV_ALIGN_CENTER, 0, -45);
    lv_text_create(Main_Panel, (char*)info, &mediumMsg_style, LV_ALIGN_CENTER, 0, 10);	

    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       WifiWarning_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void WifiWarning_cb(lv_event_t * event)
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
				DispSettingOptions();
				break;		   		
            
            default:				
                break;
        }
    }	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       DispWifiConnectionFail
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispWifiConnectionFail()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);    

	lv_obj_t * Icon_btn = lv_imgbtn_create(Main_Panel);    
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(Icon_btn, WifiConnectionFail_cb, LV_EVENT_ALL,NULL);
    lv_group_add_obj(s_group_keypad_indev,Icon_btn);
    
    lv_icon_create(Main_Panel, &lv_Fail_Icon, LV_ALIGN_TOP_MID, 0, 15);
    lv_text_create(Main_Panel, "Wi-Fi connect fail", &mediumMsg_style, LV_ALIGN_CENTER, 0, -10);
    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       WifiConnectionFail_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void WifiConnectionFail_cb(lv_event_t * event)
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
    				//DispSettingByScreen();			
				break;						   		
            
            default:				
                break;
        }
    }	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       DispWifiConnectionMessage
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispWifiConnectionMessage(const char * message)
{
    u32 timeout=1000;
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);    	

    T_U8_VIEW wifi=GetCfgPool(WIFI_NAME);
    char wifi_name[64] = {0};
    memcpy(wifi_name,wifi.head,wifi.len);
	    
    lv_text_create(Main_Panel, (char*)message, &mediumMsg_style, LV_ALIGN_CENTER, 0, -10);
    lv_obj_t * wifi_name_label = lv_text_create(Main_Panel, wifi_name, &message_style, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_width(wifi_name_label,250);
	lv_label_set_long_mode(wifi_name_label, LV_LABEL_LONG_WRAP);	  
    lv_timer_t * timer= lv_timer_create(WifiConnectionOK_cb, timeout,(pvoid)timeout);
    lv_timer_set_repeat_count(timer, 1);
    
    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       WifiConnectionOK_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void WifiConnectionOK_cb()
{   
   DispSettingOptions();    	    
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DispForgetWifi
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void DispForgetWifi()
{
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);    
	
	lv_obj_t * Icon_btn = lv_imgbtn_create(Main_Panel);    
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_add_event_cb(Icon_btn, ForgetWifi_cb, LV_EVENT_ALL,NULL);
    lv_group_add_obj(s_group_keypad_indev,Icon_btn);

    lv_text_create(Main_Panel, "Forget Wi-Fi?", &title_style, LV_ALIGN_TOP_MID, 0, 5);
    lv_text_create(Main_Panel, "The device will not \nautomatically connect to Wi-Fi", &message_style, LV_ALIGN_CENTER, 0, 0);        	
		  
    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
|   Function Name:
|       ForgetWifi_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void ForgetWifi_cb(lv_event_t * event)
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
                DispSettingOptions();
                break;

            case LV_KEY_ENTER:				
                Wifi_Forget();
                break;

			default:
                break;
        }
    }
}
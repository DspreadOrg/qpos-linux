#include "app_global.h"

#define LIMIT_BATT_PERCENT_INERVAL      60

void Show_StatusBar();
void Start_StatusBar();
static void timerUpdateICon();
void UpdateIcon();
static void updateTime();
void signal_icon_gprs();
void signal_icon_wifi();

/*----------------------------------------------------------------------------
|   Function Name:
|       Show_StatusBar
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void Show_StatusBar()
{	    
	lv_obj_clean(lv_scr_act());
	   
    Main_Panel=lv_obj_create(lv_scr_act());
    lv_obj_set_size(Main_Panel,319,210);
    lv_obj_set_size(Main_Panel,LV_PCT(100),210);
    lv_obj_align(Main_Panel,LV_ALIGN_TOP_MID,0,25);
    lv_obj_add_style(Main_Panel,&gStyleNotBorder, 0);
    lv_obj_set_scrollbar_mode(Main_Panel, LV_SCROLLBAR_MODE_OFF);
	    
    Icon_wifi = lv_text_create(lv_scr_act(), "", &Icon_style_signal_wifi, LV_ALIGN_TOP_LEFT, 0, 2);
    Icon_gprs = lv_text_create(lv_scr_act(), "", &Icon_style_signal, LV_ALIGN_TOP_LEFT, 35, 2);
    Icon_usb = lv_text_create(lv_scr_act(), "", &message_style, LV_ALIGN_TOP_MID, 55, 2);
    Icon_time = lv_text_create(lv_scr_act(), "", &message_style, LV_ALIGN_TOP_MID, 0, 2);

    // To do: Use this icon if you want see the battery percentage
    Icon_batt_percent = lv_text_create(lv_scr_act(), "", &littleLabel_style, LV_ALIGN_TOP_RIGHT, -42, 3);

    Icon_Gprs_Conn_status = lv_text_create(lv_scr_act(), "", &Icon_style_conn_type, LV_ALIGN_TOP_MID, -87, 2);

    UpdateIcon();
    Start_StatusBar();
}

/*----------------------------------------------------------------------------
|   Function Name:
|       Start_StatusBar
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void Start_StatusBar()
{	    
    lv_timer_t * Icon_timer = lv_timer_create(timerUpdateICon,500,NULL); 
    lv_timer_set_repeat_count(Icon_timer, -1);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       timerUpdateICon
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void timerUpdateICon()
{
    UpdateIcon();
}

/*----------------------------------------------------------------------------
|   Function Name:
|       UpdateIcon
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void UpdateIcon()
{
    signal_icon_wifi();
    signal_icon_gprs();

    if(Get_dev_UsbConnect_Status())
        lv_label_set_text(Icon_usb,LV_SYMBOL_USB);
    else
        lv_label_set_text(Icon_usb,"");

    updateTime();
    DispBattery();   
}

/*----------------------------------------------------------------------------
|   Function Name:
|       signal_icon_gprs
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void signal_icon_gprs(){
    int status = GetSimContnectStatus();
    int type = fun_gprs_get_connect_mode();
    switch (Get_dev_gprs_signal_lvl())
    {
        case 0:
            lv_label_set_text(Icon_gprs,"");
            break;
        case 1:
            lv_label_set_text(Icon_gprs, LV_SYMBOL_GPRS_1);
            break;
        case 2:
            lv_label_set_text(Icon_gprs, LV_SYMBOL_GPRS_2);
            break;
        case 3:
            lv_label_set_text(Icon_gprs, LV_SYMBOL_GPRS_3);
            break;
        case 4:
            lv_label_set_text(Icon_gprs, LV_SYMBOL_GPRS_4);
            break;
        case 5:
            lv_label_set_text(Icon_gprs, LV_SYMBOL_GPRS_5);
            break;
        default:
            lv_label_set_text(Icon_gprs, LV_SYMBOL_GPRS_3);
            break;
    }

    if( status)
    {
        switch (type)
        {
        case 1:
            lv_label_set_text(Icon_Gprs_Conn_status, "2G");
            break;
        case 2:
            lv_label_set_text(Icon_Gprs_Conn_status, "4G");
            break;
        default:
            lv_label_set_text(Icon_Gprs_Conn_status, "");
            break;
        }
 
    }
    else
    {
        lv_label_set_text(Icon_Gprs_Conn_status, "");
    }
}

/*----------------------------------------------------------------------------
|   Function Name:
|       signal_icon_wifi
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void signal_icon_wifi(){
    
    if(get_wifi_conn_status() == false)
    {
        lv_label_set_text(Icon_wifi,"");
        return;
    }
    
    switch (Get_dev_wifi_signal_lvl()){
        case 1:
            lv_label_set_text(Icon_wifi, LV_SYMBOL_WIFI_1);
            break;
        case 2:
            lv_label_set_text(Icon_wifi, LV_SYMBOL_WIFI_2);
            break;
        case 3:
            lv_label_set_text(Icon_wifi, LV_SYMBOL_WIFI_3);
            break;
        case 4:
            lv_label_set_text(Icon_wifi, LV_SYMBOL_WIFI_4);
            break;
        default:
            lv_label_set_text(Icon_wifi, LV_SYMBOL_WIFI_3);
            break;
    }
}

/*----------------------------------------------------------------------------
|   Function Name:
|       updateTime
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void updateTime()
{
    u8 buff[16]={0};
    u32 idx=0;

    ST_TIME Time;
	OsGetTime(&Time);
	sprintf(buff,"%02d:%02d",Time.Hour,Time.Minute);

    lv_label_set_text(Icon_time, (char*)buff);
}

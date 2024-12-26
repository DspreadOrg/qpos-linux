#include "ui_main.h"
#include "ui_styles.h"
#include "ui_status_bar.h"
#include "ui_sim.h"
#include "ui_card.h"
#include "ui_emvPinDisp.h"
#include "ui_setting.h"
#include "ui_signature.h"
#include "app_ota.h"
#include "app_trans.h"
#include "tms_cfg.h"
#include "ui_emvSelectMultiApp.h"
#include "app_pkt.h"


extern ONLINE_STATUS onlineStatus;

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DispInit
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void DispInit()
{
	lv_timer_enable(false);

	ui_init_styles();
	Show_StatusBar();
	lv_text_create(Main_Panel, "Device Init...", &title_style, LV_ALIGN_CENTER, 0, 0);
	lv_timer_enable(true);
	s_group_keypad_indev=Get_keypad_Handle();
}

/*----------------------------------------------------------------------------
|   Function Name:
|       MenuOptions_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void MenuOptions_cb(lv_event_t * event)
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
            Trans_Payment();
			//emvSelectMultiApp("\x7F\x06\x1E\x9F\x81\x14\x0A\x4D\x61\x73\x74\x65\x72\x43\x61\x72\x64\x9F\x81\x14\x07\x4D\x61\x65\x73\x74\x72\x6F\x9F\x81\x12\x01\x02");
            break;
        
        case LV_KEY_2:
            DispSettingOptions();
            break;
        
        case LV_KEY_3:
            DispOtaCheck();
            break;
        
        case LV_KEY_4:
			DispSignature();			
            break;
       
        default:				
            break;
    }    
}


/*----------------------------------------------------------------------------
|   Function Name:
|       DispMenuOptions
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispMenuOptions()
{
    char versionLine[30] = {0};
    lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

    lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_text_create(Main_Panel, "Menu", &title_style, LV_ALIGN_TOP_MID, 0, -3);

    lv_obj_t * btn_list = lv_btn_list_create(340, 155, LV_ALIGN_CENTER, 20, 5);
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "1", "Payment", LV_ALIGN_CENTER, 0, 0, true);
    lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "2", "Setting", LV_ALIGN_CENTER, 0, 0, true);
    lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "3", "OTA", LV_ALIGN_CENTER  , 0, 0, true);
    lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "4", "Sign Test", LV_ALIGN_CENTER, 0, 0, true);

	char sVer[32] = {0};
    OsGetSysVersion(0x01,sVer);
    sprintf(versionLine, "App %s Sys %s", APP_VERSION,sVer);
    lv_text_create(Main_Panel, versionLine, &lightLabel_style, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	    
    lv_timer_enable(true);	
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       InitialConnection_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void InitialConnection_cb(lv_timer_t * timer) 
{
	if(get_wifi_conn_status())
    {
		lv_timer_set_repeat_count(timer, 0);
        DispMenuOptions();
	}else{		
		lv_timer_t * timerSimConnection= lv_timer_create(SimConnecting_cb, 500, NULL);
		lv_timer_set_repeat_count(timerSimConnection, 10);
	}	
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DispInitialConnection
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void DispInitialConnection() 
{
	int status = 0;
	lv_timer_enable(false);
	OsWifiGetAutoConnectStatus(&status);
	if(status) // Supports automatic WiFi connection, requires waiting for WiFi
	{
		lv_timer_t * timer= lv_timer_create(InitialConnection_cb, 2000, NULL);
		lv_timer_set_repeat_count(timer, 5);
	}
	else  //No need to wait for WIFI connection, wait for SIM
	{
		lv_timer_t * timerSimConnection= lv_timer_create(SimConnecting_cb, 500, NULL);
		lv_timer_set_repeat_count(timerSimConnection, 20);
	}

	lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       set_empty_validateMsg
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void set_empty_validateMsg()
{
	lv_style_set_border_color(&input_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_obj_report_style_change(&input_style);
	lv_label_set_text(validateMsg, "");
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       Payment_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void Payment_cb(lv_event_t * event)
{
	lv_event_code_t code = lv_event_get_code(event);
	lv_indev_t * indev = lv_indev_get_act();
	lv_indev_type_t indev_type = lv_indev_get_type(indev);

    if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
    {
        uint32_t key=lv_indev_get_key(indev);
		lv_obj_t * ta = lv_event_get_target(event);
        const char * txt = lv_textarea_get_text(ta);
		u8 unformattedBuffer[MAX_LEN_AMOUNT+1]={0};
		u8 formattedBuffer[MAX_LEN_AMOUNT+1]={0};

        switch(key)
        {
			case LV_KEY_DOWN:
                set_empty_validateMsg();
				DispMenuOptions();
				break;

			case LV_KEY_ENTER:

				if(strlen((char*)txt)&& atol((char*)txt) > 0)
				{
                    T_U8_VIEW uvAmt={(char*)txt,strlen(txt)};
                    FormatAmount(uvAmt);
                    EventRegister(EVENT_PAYMENT);
				}
				break;

			default:
				break;
		}
	}

}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       Enter_Amount
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void Enter_Amount(void)
{
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_text_create(Main_Panel, "Input Amount", &title_style, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_t*input = lv_textarea_create(Main_Panel);
	validateMsg = lv_text_create(Main_Panel, "", &alertMessage_style, LV_ALIGN_BOTTOM_MID, 0, -35);

    lv_obj_add_event_cb(input, Payment_cb, LV_EVENT_KEY,NULL);

	lv_textarea_set_max_length(input, 11);
	lv_style_set_text_font(&input_style, &lv_font_montserrat_30);
	lv_obj_add_style(input, &input_style, LV_STATE_DEFAULT);
	lv_textarea_set_align(input, LV_TEXT_ALIGN_CENTER);
	lv_textarea_set_one_line(input, true);
	lv_textarea_set_placeholder_text(input,"$0       ");
	lv_textarea_set_accepted_chars(input, "$.0123456789");
	lv_group_remove_all_objs(s_group_keypad_indev);
    lv_group_add_obj(s_group_keypad_indev,input);
	lv_obj_center(input);

	lv_icon_create(Main_Panel, &lv_Menu_Icon, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	imgDelete = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_MID, 0, 0);
	imgContinue = lv_icon_create(Main_Panel, "", LV_ALIGN_BOTTOM_RIGHT, 0, 0);

	lv_timer_enable(true);
}

static void timeout_cb()
{
	Trans_Payment();
}


void DispResult(const char * prompt) 
{
	//to do: Enable to show cancel process message 
	u32 timeout=1000;
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_text_create(Main_Panel, prompt, &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);

    lv_timer_t * timer= lv_timer_create(timeout_cb, timeout,(pvoid)timeout);
	lv_timer_set_repeat_count(timer, 1);

	lv_timer_enable(true);
}

void DispProcess(const char * prompt) 
{
	//to do: Enable to show cancel process message 
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);
	lv_text_create(Main_Panel, prompt, &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);
	lv_timer_enable(true);
}


static void DispDeclineProcess( void ) {
	if( onlineStatus.Rc == RC_FAIL ){
		DispResult("Decline");
	}
	else {
		DispResult("Decline");
	}
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DispLoading
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void DispLoading(const char* str_Title)
{
	lv_timer_enable(false);
    lv_obj_clean(Main_Panel);

	lv_text_create(Main_Panel, str_Title, &title_style, LV_ALIGN_CENTER, 0, 0);

	// To do: Enable when you need show the spinner
	//lv_obj_t * spinner = lv_spinner_create(Main_Panel, 1000, 60);
	//lv_obj_set_size(spinner, 60, 60);
	lv_obj_t * spinner = lv_spinner_create(Main_Panel, 1000, 0);
	lv_obj_set_size(spinner, 0, 0);
	
	lv_obj_center(spinner);

    lv_timer_enable(true);
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       GuiDisplay
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void GuiDisplay(u32 id){
	DspDebug("Dspread: GuiDisplay dispId = %d",id);
    switch(id){
		case LCD_DISP_DEVICE_INIT:
			Tms_Disp_Callback_Register(TmsDispCallback);
			DispInit();
			break;

		case LCD_DISP_WELCOME:
           // DispMenuOptions();
		   	DispInitialConnection();	
			break;

		case LCD_DISP_START_TRANSACTION:
			DispCardsSupported();
			break;

		case LCD_DISP_GO_ONLINE:
			DispLoading(PROMPT_PROCESSING);
			break;
			
		case LCD_DISP_READING_CARD:
			DispLoading(PROMPT_READING_CARD);
			break;

		case LCD_DISP_TIMEOUT:
			DispResult(PROMPT_TIMEOUT);
			break;

		case LCD_DISP_TRADE_SUCCESS:
			DispResult(PROMPT_TRANS_SUCCESS);
			break;

		case LCD_DISP_TRANSACTION_TERMINATED:
			DispResult(PROMPT_TRANS_FAIL);
			break;

		case LCD_DISP_CANCEL:
			DispResult(PROMPT_USER_CANCEL);
			break;

		case LCD_DISP_OTA_FIRMWARE_DOWNLODING:
			DispDownloading();
            break;
		case LCD_DISP_OTA_FIRMWARE_NEED_DOWNLOAD:
            dispOtaCheckResult();
            break;

        case LCD_DISP_NO_AVAILABLE_FIRMWARE:
           	ui_version_not_found();
			break;

		case LCD_DISP_OTA_VERSION_NOT_FOUND:
			ui_version_not_found();
			break;	
		case LCD_DISP_SN_NOT_REGIESTER:
			ui_sn_not_found();
			break;

		case LCD_DISP_EMV_APP_BLOCK:
			//DispAppBlock();
			break;

		case LCD_DISP_DECLINED_DISP:
			DispDeclineProcess();
			break;

		case LCD_DISP_FALL_BACK:
		   	//startFallback();
			break;

	    case LCD_DISP_CARD_NOT_SUPPORT:
	    	//DispCardNotSupported();
	    	break;

		case LCD_DISP_TRY_ANOTHER_INTERFACE:
			DispResult(PROMPT_TRY_ANOTER_INTERFACE);
			break;

	    case LCD_DISP_SELECT_APP:
	    	DispMultiAppList( );
	    	break;

        case LCD_DISP_INPUT_ONLINE_PIN:
        case LCD_DISP_INPUT_OFFLINE_PIN:
        case LCD_DISP_INPUT_LAST_OFFLINE_PIN:
			DispPinEntry( );
            break;

        case LCD_DISP_PIN_MASK:
			DispPinMask( );
            break;

		case LCD_DISP_CHIP_CARD:
			DispCards(CARD_IC + CARD_NFC);
			break;

		case LCD_DISP_SEE_PHONE_PLS:
			DispSeePhone();
			break;

		case LCD_DISP_NFC_RETAP:
			DispCards(CARD_NFC);
			break;
			
		case LCD_DISP_WIFI_LIST:
			DispWifiList();
			break;

		case LCD_DISP_WIFI_CONNECT_SUCCESS:
			DispWifiConnectionMessage("Connect to Wi-Fi");
			break;	

		case LCD_DISP_WIFI_CONNECT_FAIL:
			DispWifiConnectionFail();
			break;	

		case LCD_DISP_OTA_UPDATE_FAIL:
			ui_Downloading_Fail();
			break;	

		default:
			break;
	}
}


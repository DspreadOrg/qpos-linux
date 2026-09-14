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
#include "ui_qrcode.h"

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
    lv_indev_t *indev = lv_indev_get_act();
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    uint32_t key = EVENT_KEY_NONE;

    if (indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY) {
        key = lv_indev_get_key(indev);
    }
    else if (indev_type == LV_INDEV_TYPE_POINTER && code == LV_EVENT_CLICKED) {
        /* 触摸点击：用容器的 user_data 或 obj 索引反查按键号 */
        key = lv_get_btn_key(event);          /* 你自己的实现 */
    }
    else if (indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_CLICKED) {
        key = LV_KEY_ENTER;                   /* 焦点按回车 */
    }
	OsLog(LOG_DEBUG,"key = %d",key);
    switch(key)
    {	        
		case LV_KEY_1:
            Trans_Payment();
            break;
        
        case LV_KEY_2:
            ui_create_qrcode();
            break;
        
        case LV_KEY_3:
			DispSignature();	
            break;  

        case LV_KEY_4:
            DispWifiSetting();
            break;  

		case LV_KEY_5:
			DispSimSetting();
			break;	

		case LV_KEY_6:
            PrintTest();
            break;

		case LV_KEY_7:
            DispAbout();
            break;

		case LV_KEY_ENTER:
			Trans_Payment();
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

    // lv_text_create(Main_Panel, "Menu", &title_style, LV_ALIGN_TOP_MID, 0, -3);

    lv_obj_t * btn_list = lv_btn_list_create(360, 200, LV_ALIGN_CENTER, 20, 0);
	lv_obj_set_layout(btn_list, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(btn_list, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_clear_flag(btn_list, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_style_pad_column(btn_list, 10, 0);   /* ★ 列间距（同排按钮之间） */
	lv_obj_set_style_pad_row(btn_list,    0, 0);   /* ★ 行间距（换行后两行之间） */
    lv_group_remove_all_objs(s_group_keypad_indev);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "1", "Payment", LV_ALIGN_CENTER, 0, 0, true);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "2", "Setting", LV_ALIGN_CENTER, 0, 0, true);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "3", "OTA", LV_ALIGN_CENTER  , 0, 0, true);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "4", "Sign Test", LV_ALIGN_CENTER, 0, 0, true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "Sale",		"menu_payment.png", LV_ALIGN_CENTER,true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "Qrcode",	"menu_qrcode.png", LV_ALIGN_CENTER,true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "Sign", 	"menu_sign.png", LV_ALIGN_CENTER,true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "Wifi", 	"menu_wifi.png", LV_ALIGN_CENTER,true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "Gprs", 	"menu_gprs.png", LV_ALIGN_CENTER,true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "Printer", 	"menu_printer.png", LV_ALIGN_CENTER,true);
	lv_add_imgBtn(btn_list, MenuOptions_cb, "About",	"menu_about.png", LV_ALIGN_CENTER,true);

    sprintf(versionLine, "%s", APP_VERSION);
    lv_text_create(Main_Panel, versionLine, &lightLabel_style, LV_ALIGN_BOTTOM_RIGHT, 0, 5);
	    
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
 |       Disp_Amt_CallBack
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/


void Disp_Amt_CallBack(lv_obj_t *label,lv_obj_t *ta,pu8 txt,u32 len)
{
    u8 disp[20] = {0},temp[20] = {0};
    if(txt != NULL && atol(txt) > 0)
    {
        disp[0] = '$';
        sprintf(&disp[1], "%ld.%02d", atol(txt)/100, atol(txt)%100);
        lv_textarea_set_text(label, disp);

    }
    else
    {
        lv_textarea_set_text(label, "$0.00");
    }
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
	lv_obj_t *label = lv_event_get_user_data(event);

    if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
    {
        uint32_t key=lv_indev_get_key(indev);
		lv_obj_t * ta = lv_event_get_target(event);
        const char * txt = lv_textarea_get_text(ta);
		OsLog(LOG_DEBUG,"txt = %s",txt);
		Disp_Amt_CallBack(label,ta,txt,strlen(txt));
		u8 unformattedBuffer[MAX_LEN_AMOUNT+1]={0};
		u8 formattedBuffer[MAX_LEN_AMOUNT+1]={0};

        switch(key)
        {              
			case LV_KEY_DOWN:
				DispMenuOptions();
				break;
				
			case LV_KEY_ENTER:
				if(strlen((char*)txt))
				{
					sprintf(get_transaction_data()->sAmount,"%012ld",atol(txt));
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

	lv_obj_t *dispInput = lv_textarea_create(Main_Panel);
    lv_textarea_set_align(dispInput,LV_TEXT_ALIGN_CENTER);
    lv_textarea_set_text(dispInput, "$0.00");
	lv_style_set_text_font(&input_style, &lv_font_montserrat_30);
	lv_obj_add_style(dispInput, &input_style, LV_STATE_DEFAULT);
    lv_obj_set_size(dispInput,290,30);
    lv_textarea_set_one_line(dispInput, true);
    lv_obj_center(dispInput);


   lv_obj_t*input = lv_textarea_create(Main_Panel);
    lv_obj_add_event_cb(input, Payment_cb, LV_EVENT_KEY,(pvoid )dispInput);

	lv_textarea_set_max_length(input, 11);
	lv_style_set_text_font(&input_style, &lv_font_montserrat_30);
	lv_obj_add_style(input, &input_style, LV_STATE_DEFAULT);
	lv_textarea_set_align(input, LV_TEXT_ALIGN_CENTER);
	lv_textarea_set_one_line(input, true);
	// lv_textarea_set_placeholder_text(input,"$0.00");
	lv_textarea_set_accepted_chars(input, "$.0123456789");
	lv_group_remove_all_objs(s_group_keypad_indev);
    lv_group_add_obj(s_group_keypad_indev,input);
	lv_obj_center(input);
	lv_obj_set_style_opa(input,LV_OPA_TRANSP,0);

	lv_icon_create(Main_Panel, &lv_Menu_Icon, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	imgDelete = lv_icon_create(Main_Panel, NULL, LV_ALIGN_BOTTOM_MID, 0, 0);
	imgContinue = lv_icon_create(Main_Panel, NULL, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

	lv_timer_enable(true);
}

static void timeout_cb()
{
	DispMenuOptions();
}


void DispResult(const char * prompt) 
{
	//to do: Enable to show cancel process message 
	u32 timeout=3000;
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
	DSP_Info("dispId = %d",id);
    switch(id){
		case LCD_DISP_DEVICE_INIT:
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
			disp_fail(PROMPT_TIMEOUT);
			break;

		case LCD_DISP_TRADE_SUCCESS:
			disp_success(PROMPT_TRANS_SUCCESS);
			break;
		case LCD_DISP_TRADE_FAIL:
			disp_fail(PROMPT_TRANS_FAIL);
			break;
		case LCD_DISP_TRANSACTION_TERMINATED:
			disp_fail(PROMPT_TRANS_TERMINATED);
			break;

		case LCD_DISP_CANCEL:
			disp_fail(PROMPT_USER_CANCEL);
			break;

		case LCD_DISP_OTA_CHECK:
			DispOtaCheck();
			break;	

		case LCD_DISP_OTA_DOWNLOAD_DISP:
			DispDownloading();
            break;

		case LCD_DISP_OTA_DOWNLODING:
			UpdateOTAProcess();
			break;	

		case LCD_DISP_OTA_UPGRADING:
			DispOtaUpgrade();
			break;

		case LCD_DISP_OTA_FIRMWARE_NEED_DOWNLOAD:
            dispOtaCheckResult();
            break;

		case LCD_DISP_OTA_VERSION_NOT_FOUND:
			ui_version_not_found();
			break;	
		case LCD_DISP_SN_NOT_REGIESTER:
			ui_sn_not_found();
			break;
		case LCD_DISP_OTA_TASK_INVALID:
			ui_task_invalid();
			break;
		case LCD_DISP_EMV_APP_BLOCK:
			DispAppBlock();
			break;

		case LCD_DISP_DECLINED_DISP:
			DispDeclineProcess();
			break;

		case LCD_DISP_FALL_BACK:
		   	DispResult(PROMPT_USE_SWIPE_CARD);
			break;

	    case LCD_DISP_CARD_NOT_SUPPORT:
	    	DispCardNotSupported();
	    	break;

		case LCD_DISP_TRY_ANOTHER_INTERFACE:
			DispResult(PROMPT_TRY_ANOTER_INTERFACE);
			break;

	    case LCD_DISP_SELECT_APP:
	    	DispMultiAppList( );
	    	break;

        case LCD_DISP_INPUT_ONLINE_PIN:
			DispPinEntry();
			break;
        case LCD_DISP_INPUT_OFFLINE_PIN:
        case LCD_DISP_INPUT_LAST_OFFLINE_PIN:
			DispPinEntry();
            break;
		case LCD_DISP_PIN_MASK:	
			DispPinMask();
			break;
		case LCD_DISP_CHIP_CARD:
			DispCards(CARD_IC|CARD_NFC);
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

		case LCD_DISP_NFC_TAP_TRY_AGAIN:
			DispResult(PROMPT_NFC_TAP_TRY_AGAIN);
			break;	

		case LCD_DISP_QR_PAYMENT_CHECK:
			DispLoading("Checking payment status...");
			break;

		case LCD_DISP_QR_PAYMENT_SUCCESS:
			disp_success("Qr Payment Suucess");
			break;
		case LCD_DISP_PRINTER_NOT_SUPPORT:
		   	DispResult("Printer not supported");
			break;
		case LCD_DISP_PRINTER_NOT_WORKING:
		   	DispResult("Printer error");
			break;
		case LCD_DISP_PRINTER_PRINTING:
		   	DispResult("Printing...");
			break;
		case LCD_DISP_PINTER_SUCCESS:
		   	DispResult("Print completed");
			break;	
		default:
			break;
	}
}

void disp_success(const char * prompt)
{
	u32 timeout=3000;
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_text_create(Main_Panel, prompt, &mediumMsg_style, LV_ALIGN_CENTER, 0, 40);

	lv_obj_t * result_icon = lv_img_create(Main_Panel);
	ui_lv_img_set_src(result_icon,"success_icon.png");
	lv_obj_align(result_icon, LV_ALIGN_CENTER, 0, -20);

    lv_timer_t * timer= lv_timer_create(timeout_cb, timeout,(pvoid)timeout);
	lv_timer_set_repeat_count(timer, 1);

	lv_timer_enable(true);
}

void disp_fail(const char * prompt)
{
	u32 timeout=3000;
	lv_timer_enable(false);
	lv_obj_clean(Main_Panel);

	lv_text_create(Main_Panel, prompt, &mediumMsg_style, LV_ALIGN_CENTER, 0, 40);

	lv_obj_t * result_icon = lv_img_create(Main_Panel);
	ui_lv_img_set_src(result_icon,"fail_icon.png");
	lv_obj_align(result_icon, LV_ALIGN_CENTER, 0, -20);

    lv_timer_t * timer= lv_timer_create(timeout_cb, timeout,(pvoid)timeout);
	lv_timer_set_repeat_count(timer, 1);

	lv_timer_enable(true);
}
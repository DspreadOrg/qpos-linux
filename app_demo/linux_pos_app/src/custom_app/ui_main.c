#include "ui_main.h"

#include "app_ota.h"
#include "app_trans.h"
#include "tms_cfg.h"
#include "ui_MultiApp.h"

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DispInit
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void DispInit()
{
	// lv_timer_enable(false);

	// ui_init_styles();
    // Show_StatusBar();
	// //lv_icon_create(lv_scr_act(), &lv_Splash_Image, LV_ALIGN_CENTER, 0, 0);

	// lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
	// lv_obj_set_size(spinner, 0, 0);	
	
	// lv_timer_enable(true);
	// s_group_keypad_indev=Get_keypad_Handle();
}

/*----------------------------------------------------------------------------
|   Function Name:
|       MenuOptions_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
// static void MenuOptions_cb(lv_event_t * event)
// {
//     lv_event_code_t code = lv_event_get_code(event);
//     lv_indev_t * indev = lv_indev_get_act();
//     lv_indev_type_t indev_type = lv_indev_get_type(indev);
//     uint32_t key = EVENT_KEY_NONE;

//     if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
//         key = lv_indev_get_key(indev);
//     else if(indev_type==LV_INDEV_TYPE_POINTER && code == LV_EVENT_CLICKED)
//         key = lv_get_btn_key(event);
  
//     switch(key)
//     {	        
// 		case LV_KEY_1:
//             Trans_Payment();
//             break;
        
//         case LV_KEY_2:
//             DispSettingOptions();
//             break;
        
//         case LV_KEY_3:
//             DispOtaCheck();
//             break;
        
//         case LV_KEY_4:
// 			DispSignature();			
//             break;

// 		case LV_KEY_5:
// 			break;	        
//         default:				
//             break;
//     }    
// }


/*----------------------------------------------------------------------------
|   Function Name:
|       DispMenuOptions
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void DispMenuOptions()
{
    // char versionLine[30];
    // lv_timer_enable(false);
	// lv_obj_clean(Main_Panel);

    // lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE);

    // lv_text_create(Main_Panel, "Menu", &title_style, LV_ALIGN_TOP_MID, 0, -3);

    // lv_obj_t * btn_list = lv_btn_list_create(340, 155, LV_ALIGN_CENTER, 20, 5);
    // lv_group_remove_all_objs(s_group_keypad_indev);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "1", "Payment", LV_ALIGN_CENTER, 0, 0, true);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "2", "Setting", LV_ALIGN_CENTER, 0, 0, true);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "3", "OTA", LV_ALIGN_CENTER  , 0, 0, true);
    // lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "4", "Sign Test", LV_ALIGN_CENTER, 0, 0, true);
	// //lv_add_btn(btn_list, MenuOptions_cb, 86, 56, "5", "Last Trans", LV_ALIGN_CENTER, 0, 0, true);
	// // To do: Put the user name here
    // //lv_text_create(Main_Panel, "Usuario: Juan Gonzalez", &message_style, LV_ALIGN_BOTTOM_MID, 0, 0);
    // lv_text_create(Main_Panel, "", &message_style, LV_ALIGN_BOTTOM_MID, 0, 0); 
	
	// //Sprintf_(versionLine, "%s V%s (%s)", APP_NAME, APP_VERSION, APP_BUILD);
    // Sprintf_(versionLine, "V%s", APP_VERSION);
    // lv_text_create(Main_Panel, versionLine, &lightLabel_style, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	    
    // lv_timer_enable(true);	


}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       InitialConnection_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
static void InitialConnection_cb() 
{
	// if(get_wifi_conn_status())
    // {
    //     DispMenuOptions();
	// }else{		
	// 	lv_timer_t * timerSimConnection= lv_timer_create(SimConnecting_cb, 500, NULL);
	// 	lv_timer_set_repeat_count(timerSimConnection, 10);
	// }	
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
	// char versionLine[30];
	// lv_timer_enable(false);
	
	// //Apn_cfg(); 

	// lv_text_create(Main_Panel, "Initialize...", &title_style, LV_ALIGN_CENTER, 0, 0);
	// sprintf(versionLine, "V%s", APP_VERSION);
	// lv_text_create(Main_Panel, versionLine, &lightLabel_style, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

	// lv_timer_t * timer= lv_timer_create(InitialConnection_cb, 1500, NULL);
	// lv_timer_set_repeat_count(timer, 1);

	// lv_timer_enable(true);
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
	// lv_style_set_border_color(&input_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	// lv_obj_report_style_change(&input_style);
	// lv_label_set_text(validateMsg, "");
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       Payment_cb
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
// static void Payment_cb(lv_event_t * event)
// {
// 	lv_event_code_t code = lv_event_get_code(event);
// 	lv_indev_t * indev = lv_indev_get_act();
// 	lv_indev_type_t indev_type = lv_indev_get_type(indev);

//     if(indev_type==LV_INDEV_TYPE_KEYPAD && code==LV_EVENT_KEY)
//     {
//         uint32_t key=lv_indev_get_key(indev);
// 		lv_obj_t * ta = lv_event_get_target(event);
//         const char * txt = lv_textarea_get_text(ta);
// 		u8 unformattedBuffer[MAX_LEN_AMOUNT+1]={0};
// 		u8 formattedBuffer[MAX_LEN_AMOUNT+1]={0};

//         switch(key)
//         {
// 			case LV_KEY_DOWN:
//                 set_empty_validateMsg();
// 				DispMenuOptions();
// 				break;

// 			case LV_KEY_ENTER:

// 				if(strlen((char*)txt)&& atol((char*)txt) > 0)
// 				{
//                     T_U8_VIEW uvAmt={(char*)txt,Strlen_(txt)};
//                     FormatAmount(uvAmt);
//                     EventRegister(EVENT_PAYMENT);
// 				}
// 				break;

// 			default:
// 				break;
// 		}
// 	}

// }

/*----------------------------------------------------------------------------
 |   Function Name:
 |       Enter_Amount
 |   Description:
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void Enter_Amount(void)
{	

}

static void timeout_cb()
{
	Trans_Payment();
}


void DispResult(const char * prompt) 
{
	// //to do: Enable to show cancel process message 
	// u32 timeout=1000;
	// lv_timer_enable(false);
	// lv_obj_clean(Main_Panel);

	// lv_text_create(Main_Panel, prompt, &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);

    // lv_timer_t * timer= lv_timer_create(timeout_cb, timeout,(pvoid)timeout);
	// lv_timer_set_repeat_count(timer, 1);

	// lv_timer_enable(true);
}

void DispProcess(const char * prompt) 
{
	// //to do: Enable to show cancel process message 
	// lv_timer_enable(false);
	// lv_obj_clean(Main_Panel);
	// lv_text_create(Main_Panel, prompt, &mediumMsg_style, LV_ALIGN_CENTER, 0, 0);
	// lv_timer_enable(true);
}


void drawRect(int startx,int starty,int endx,int endy){
		POINT_T lineStart;
		POINT_T lineEnd;

		lineStart.X = startx;
		lineStart.Y=  starty;
		lineEnd.X = endx;
		lineEnd.Y=  starty;
		
		Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);
		lineEnd.X = startx;
		lineEnd.Y=  endy;
		Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);
		lineStart.X = startx;
		lineStart.Y=  endy;
		lineEnd.X = endx;
		lineEnd.Y=  endy;		
		Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);
		lineStart.X = endx;
		lineStart.Y=  starty;
		Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       DisplayHandler
 |   Description: SDK call back function to handle display
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void DisplayHandler(pmessage_body_t pmsg){
	dbg_printf("111= %d",pmsg->wparam);
    switch(pmsg->wparam){
		case LCD_DISP_DEVICE_INIT:			
			TransView_vClearAllScreen();
			TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"QposTrio");
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Init...");
			break;

		case LCD_DISP_WELCOME:
			TransView_vClearAllScreen();
			TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"QposTrio");
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"welcome");
			break;

		case LCD_DISP_START_TRANSACTION:
			break;

		case LCD_DISP_READING_CARD:
			TransView_vClearLine(1);
			TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Reading card...");			
			break;

		case LCD_DISP_TIMEOUT:
			TransView_vClearAllScreen();			
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Input PIN Timeout");
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Trans cancel");
			KB_nWaitKeyMS(5*1000);
			break;

		case LCD_DISP_NET_COMMUNICATE:
			break;

		case LCD_DISP_TRADE_SUCCESS:
			TransView_vClearAllScreen();			
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Trans Success");
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Transaction Success");
			KB_nWaitKeyMS(5*1000);
			break;

		case LCD_DISP_TRANSACTION_TERMINATED:
			TransView_vClearAllScreen();			
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Trans fail");
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Transaction terminated");
			KB_nWaitKeyMS(5*1000);
			break;

		case LCD_DISP_CANCEL:
			TransView_vClearAllScreen();			
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Input PIN cancel");
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Trans cancel");
			KB_nWaitKeyMS(5*1000);
			break;

	    case LCD_DISP_EMV_APP_BLOCK:
	    	break;

	    case LCD_DISP_DECLINED_DISP:
			TransView_vClearAllScreen();			
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Trans fail");
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Decline");
			KB_nWaitKeyMS(5*1000);
			break;

	    case LCD_DISP_FALL_BACK:
	        break;

	    case LCD_DISP_CARD_NOT_SUPPORT:			
			TransView_vClearLine(2);
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Pls Try another inteface");
			KB_nWaitKeyMS(5*1000);
	    	break;

	    case LCD_DISP_TRY_ANOTHER_INTERFACE:
			TransView_vClearLine(2);
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Pls Try another inteface");
			KB_nWaitKeyMS(5*1000);
	    	break;

        case LCD_DISP_INPUT_ONLINE_PIN:
			TransView_vClearLine(1);
			TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz input online pin");
            break;

        case LCD_DISP_INPUT_OFFLINE_PIN:
			TransView_vClearLine(1);
			TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz input offline pin");
			drawRect(20,118,320-20,150);
            break;

        case LCD_DISP_INPUT_LAST_OFFLINE_PIN:
			TransView_vClearLine(1);
			TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Last input offline pin");
            break;

        case LCD_DISP_PIN_MASK:
			TransView_vClearLine(2);
			const pLcd_Pin_Mask_t pLcdPinMask = (pLcd_Pin_Mask_t)(pmsg->pparam);
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)(pLcdPinMask->pinMask));
			drawRect(20,118,320-20,150);
            break;
        case LCD_DISP_PIN_CONFIRM://TODO
			break;

		case LCD_DISP_CHIP_CARD:
			TransView_vClearLine(2);
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz use CHIP or NFC");
			break;
		case LCD_DISP_NFC_RETAP:
			TransView_vClearLine(2);
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz reTap card");
			break;
		case LCD_DISP_OTA_FIRMWARE_DOWNLODING:			
			TransView_vClearAllScreen();
			TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"QposTrio OTA");
			TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Downloading...");
			break;
		case LCD_DISP_OTA_FIRMWARE_NEED_DOWNLOAD:
            dispOtaCheckResult();
            break;
		case LCD_DISP_SEE_PHONE_PLS:			
			break;

		default:
			break;
	}
}

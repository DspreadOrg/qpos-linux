#include "ui_signature.h"
#include "ui_main.h"

static void signatureCallback() {
	
	int ret;
    int nElecSignRet;

    unsigned char *elceSignBuf = NULL; //[10*1024] ={0};
    PR_INT32 elceSignLen = 0;
    PR_INT8 szCode[] = "45323233";
    PR_INT32 elceSignTimeOut = 60;
	DSP_Debug();
	elceSignBuf = malloc(4*1024);
	memset(elceSignBuf,0,4*1024);
	disp_disable_update();
    //nElecSignRet = Disp_nElecSign(elceSignBuf,&elceSignLen,NULL,elceSignTimeOut,10,75,300,125,0);
	nElecSignRet = Disp_nElecSignEx(elceSignBuf,&elceSignLen,NULL,0.8,elceSignTimeOut,10,75,300,125,0);
	disp_enable_update();
	if(nElecSignRet != RET_OK)
	{
		OsLog(LOG_DEBUG,"Dspread: nElecSignRet = %d",nElecSignRet);

		free(elceSignBuf);
		TransKbdEvent( EVENT_KEY_CANCEL );
		GuiEventRegister(LCD_DISP_CANCEL);
	}
	else
	{
		OsLog(LOG_DEBUG,"Dspread: elceSignLen = %d",elceSignLen);
		OsLog(LOG_DEBUG,"Dspread: %s",elceSignBuf);
		app_trace_value(elceSignBuf,elceSignLen);
		free(elceSignBuf);
		DispMenuOptions();
	}
}

void DispSignature( void ) 
{
	u32 timeout = 100;
	lv_timer_enable( false );
	lv_obj_clean( Main_Panel );

	lv_timer_t* timer = lv_timer_create( signatureCallback, timeout, ( pvoid )timeout );
	lv_timer_set_repeat_count( timer, 1 );

	lv_text_create( Main_Panel, "Signature", &title_style, LV_ALIGN_TOP_MID, 0, 0 );

	lv_obj_t* label = lv_label_create( Main_Panel );
	lv_label_set_text( label, " " );
	lv_obj_set_size( label, 300, 120 );
	lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
	lv_obj_set_style_bg_color( label, lv_color_hex( SIGN_FIELD_COLOR ), 0 );
	lv_obj_set_style_bg_grad_color( label, lv_color_hex( SIGN_FIELD_COLOR ), 0 );
	lv_obj_add_style( label, &gStyleSignature, LV_STATE_DEFAULT );

	lv_timer_enable( true );
}
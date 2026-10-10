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
	elceSignBuf = malloc(10*1024);
	memset(elceSignBuf,0,10*1024);
	disp_disable_update();
	nElecSignRet = Disp_nElecSignEx(elceSignBuf,&elceSignLen,NULL,0.8,elceSignTimeOut,10,75,300,125,0);
	disp_enable_update();
	if(nElecSignRet != RET_OK)
	{
		OsLog(LOG_DEBUG,"Dspread: nElecSignRet = %d",nElecSignRet);
		free(elceSignBuf);
		GuiEventRegister(LCD_DISP_CANCEL);
	}
	else
	{
		OsLog(LOG_DEBUG,"Dspread: elceSignLen = %d",elceSignLen);
		free(elceSignBuf);
		// DispMenuOptions();
		EventRegister(EVENT_SIGN_PRINTER);
	}
}

void printSign()
{
	PR_INT32 ret;
    PR_INT32 nElecSignRet;
    PR_UINT8 temp[256];
    PR_INT8 FilePath[128] = {0};
	PR_INT8 AppPath[128] = {0};
    PR_INT8 hw_version[8] = {0};

    OsGetHwVersion(hw_version);
    if(hw_version[0] == '4'){
        GuiEventRegister(LCD_DISP_PRINTER_NOT_SUPPORT);
        return;
    }

    ret = OsPrnOpen(PRN_REAL,NULL);
    if(ret != RET_OK){
        GuiEventRegister(LCD_DISP_PRINTER_NOT_WORKING);
        return;
    }
    OsPrnSetGray(1);
    OsPrnSetSpace(0,4);
    ret = OsPrnCheck();
    if( ret != RET_OK){
        GuiEventRegister(LCD_DISP_PRINTER_NOT_WORKING);
        goto exit;
    }
    GuiEventRegister(LCD_DISP_PRINTER_PRINTING);
	OsPrnReset();
	// OsPrnSetPrintParamsEx(32,1,1,ALIGN_TYPE_CENTER,1);
	// OsPrnPrintf((char *)"به استفاده خوش آمدید");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_CENTER);
	OsPrnPrintf((char *)"MERCHANT COPY");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"================================================");
	OsPrnPrintf((char *)" ");
	OsPrnPrintf((char *)"MERCHANT NAME");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"START");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"MERCHANT NO",0,0);
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"1234567890");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"TERMINAL NO                OPERATOR NO");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"33333333                01");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"CARD NUMBER");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"6229 0** **** 6101       CUP");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"ISS NO                     ACQ NO");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"ICBC          19992900");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"TRANS TYPE");
	OsPrnSetPrintParams(24,3,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"SALE");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"EXP DATE");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"2015/02 ");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"BATCH NO                     VUCHER NO");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"000001              000016");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"AUTH NO ");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_RIGHT);
	OsPrnPrintf((char *)"867543234321");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"DATE/TIME");
	OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_RIGHT);
	OsPrnPrintf((char *)"2023/08/23 11:22:33");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"AMOUNT");
	OsPrnSetPrintParams(24,3,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"RMB 10.00");
	OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
	OsPrnPrintf((char *)"REFERENCE ");
	OsPrnPrintf((char *)"================================================");
	if(nElecSignRet == 0){
		OsPrnPutElecSignDataByJpg();
	}else{
		OsPrnPrintf((char *)"CARDHOLDER SIGNATURE ");
		OsPrnPrintf((char *)" ");// 5line
		OsPrnPrintf((char *)" ");// 5line
		OsPrnPrintf((char *)" ");// 5line
		OsPrnPrintf((char *)" ");// 5line
		OsPrnPrintf((char *)" ");// 5line
	}  
	OsPrnPrintf((char *)"================================================");
	OsPrnPrintf((char *)"I ACKNOWLEDGE SATISFACTORY RECEIPT OF FELATIUE GOODS/SERVICES");
	OsPrnPrintf((char *)" ");// 5line
	OsPrnPrintf((char *)" ");// 5line
	OsPrnPrintf((char *)" ");// 5line
	OsPrnPrintf((char *)" ");// 5line
	OsPrnPrintf((char *)" ");// 5line
	if((ret = OsPrnStart()) != RET_OK){
		GuiEventRegister(LCD_DISP_PRINTER_NOT_WORKING);
		goto exit;
	}

    OsPrnFeed(48);
    GuiEventRegister(LCD_DISP_PINTER_SUCCESS);
exit:
    OsPrnClose();
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
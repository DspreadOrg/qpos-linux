#include "appinc.h"

void PrintTest()
{
    EventRegister(EVENT_PRINTER);
}
void PrintStart(int nCount)
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
    for(int i = 0;i< nCount;i++){
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
    }
    OsPrnFeed(48);
    GuiEventRegister(LCD_DISP_PINTER_SUCCESS);
exit:
    OsPrnClose();
}
#include "appinc.h"
#include "printer.h"


void Print_Order(){
    PR_INT32 ret;
    PR_INT32 nElecSignRet;
    PR_UINT8 temp[256];
    PR_INT8 FilePath[128] = {0};
	PR_INT8 AppPath[128] = {0};

    PR_UINT8 elceSignBuf[10*1024] ={0};
    PR_INT32 elceSignLen = 0;
    PR_INT32 elceSignTimeOut = 60;
    Disp_vClearPort();
    Disp_vShowStr(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Print Ticket");
    ret = OsPrnOpen(PRN_REAL,NULL);
    if(ret != RET_OK){
        Disp_vShowStr(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open Device Failed");
        KB_nWaitKeyMS(5*1000);
        return;
    }
    OsPrnSetGray(1);
    OsPrnSetSpace(0,4);
    ret = OsPrnCheck();
    if( ret != RET_OK){
        Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,ret == ERR_PRN_BUSY?(char*)"Printer Busy":
                    ret == ERR_PRN_PAPEROUT?(char*)"Printer Paper Out":
                    ret == ERR_PRN_OVERHEAT?(char*)"Printer Overheat":
                    ret == ERR_PRN_OVERVOLTAGE?(char*)"Printer Overvoltage ":
					ret == ERR_BATTERY_VOLTAGE_TOO_LOW?(char*)"Printer Voltage Too Low":(char*)"Printer Exception");
        goto exit;
    }

    Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Printing");
    for(int i = 0;i< 1;i++){
        OsPrnReset();
        Business_getAppPath(AppPath,sizeof(AppPath));
        sprintf(FilePath,"%s/res/yl.bmp",AppPath);
        OsLog(LOG_WARN,"bmp path： %s",FilePath);
        OsPrnPutImage((unsigned char *)FilePath);        
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_RIGHT);
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
            OsPrnPutElecSignData(elceSignBuf,elceSignLen);
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
            Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,ret == ERR_PRN_BUSY?(char*)"Printer Busy":
                        ret == ERR_PRN_PAPEROUT?(char*)"Printer Paper Out":
                        ret == ERR_PRN_OVERHEAT?(char*)"Printer Overheat":
                        ret == ERR_PRN_OVERVOLTAGE?(char*)"Printer Overvoltage ":
                        ret == ERR_BATTERY_VOLTAGE_TOO_LOW?(char*)"Printer Voltage Too Low":(char*)"Printer Exception");
            goto exit;
        }
    }
    OsPrnFeed(48);
    Disp_vShowStr(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Print Complete");
exit:
    OsPrnClose();
    KB_nWaitKeyMS(5*1000);
}
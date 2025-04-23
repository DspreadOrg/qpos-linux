#include "appinc.h"

void MagcardTest()
{
    PR_INT32 ret;
    ST_MSR_DATA Track1; 
    ST_MSR_DATA Track2; 
    ST_MSR_DATA Track3;
    PR_INT32 line = 1;
    long long start = get_sys_tick();
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Magnetic");

    ret = OsMsrOpen();
    if(ret != RET_OK){
        OsMsrClose();
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open Device Failed");
        KB_nWaitKeyMS(3*1000);
        return;
    }else{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Pls Swipe Card");
    }
    
    while(get_sys_tick() - start < 30*1000){
        ret = OsMsrSwiped();
        if(ret){
            if(OsMsrRead(&Track1,&Track2,&Track3) == 0){

                if(Track1.DataLen > 0){
                    TransView_vShowLineEx(line,EM_ALIGN_LEFT,(char*)"Track1 %s",Track1.TeackData);
                    line+=2;
                }

                if(Track2.DataLen > 0){
                    TransView_vShowLineEx(line,EM_ALIGN_LEFT,(char*)"Track2 %s",Track2.TeackData);
                    line+=2;
                }

                if(Track3.DataLen > 0){
                    TransView_vShowLineEx(line,EM_ALIGN_LEFT,(char*)"Track3 %s",Track3.TeackData);
                }
                OsMsrClose();
                KB_nWaitKeyMS(5*1000);
                return;
            }else{
                TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Track Data Err");
                OsMsrReset();
            }
        }
        ret = KB_nWaitKeyMS(500);
		if(ret == EM_KEY_CANCEL){
            OsMsrClose();
            return;
        }
    }
    OsMsrClose();
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Read Card Timeout");
    KB_nWaitKeyMS(5*1000);
}

void IccTest()
{
    PR_INT32 ret;
    PR_UINT8 temp[1024];
    ST_APDU_REQ ApduReq;
    ST_APDU_RSP ApduRsp;
    long long start = get_sys_tick();
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"IC Card");

    memset(&ApduRsp,0x0,sizeof(ST_APDU_RSP));
    memset(&ApduReq,0x0,sizeof(ST_APDU_REQ));
    memcpy(ApduReq.Cmd,"\x00""\xA4""\x04""\x00",4);
    ApduReq.LC = 0;
    ApduReq.LE = 0;
    ret = OsIccOpen(0);
    if(ret != RET_OK){
        OsIccClose(0);
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open Device Failed");
        KB_nWaitKeyMS(3*1000);
        return;
    }else{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Pls Insert Card");
    }
    while(get_sys_tick() - start < 30*1000){
        ret = OsIccDetect(0);
        if(ret == 0){
            memset(temp,0x0,sizeof(temp));
            if(OsIccInit(0,0,temp) == 0){
                ret = OsIccExchange(0,0,&ApduReq,&ApduRsp);
                if(ret == 0){
                    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"APDU Communication Successful");
                }else{
                    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"APDU Communication Faild");
                }
                goto EXIT;
            }else{
                TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"IC Card PowerOn Failed");
                goto EXIT;
            }
        }
        ret = KB_nWaitKeyMS(500);
		if(ret == EM_KEY_CANCEL){
            OsIccClose(0);
            return;
        }
    }
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Read Card Timeout");
EXIT:
    OsIccClose(0);
    KB_nWaitKeyMS(5*1000);
}


void RfidTest()
{
    PR_INT32 ret;
    PR_UINT8 temp[256];
    PR_UINT8 temp2[256];
    PR_UINT8 cmd[256];
    ST_APDU_REQ ApduReq;
    ST_APDU_RSP ApduRsp;
    PR_INT32 outLen = 0;
    long long start = get_sys_tick();
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Rfid Card");

    memset(&ApduRsp,0x0,sizeof(ST_APDU_RSP));
    memset(&ApduReq,0x0,sizeof(ST_APDU_REQ));
    memcpy(ApduReq.Cmd,"\x00""\xA4""\x04""\x00",4);
    memcpy(ApduReq.DataIn,"\x32""\x50""\x41""\x59""\x2E""\x53""\x59""\x53""\x2E""\x44""\x44""\x46""\x30""\x31",14);
    ApduReq.LC = 14;
    ApduReq.LE = 0;
    memset(temp,0x0,sizeof(temp));
    memset(temp2,0x0,sizeof(temp2));
    ret = OsPiccOpen();
    if(ret != RET_OK){
        OsPiccClose();
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open Device Failed");
        KB_nWaitKeyMS(3*1000);
        return;
    }else{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Pls Tap Card");
    }
    while(get_sys_tick() - start < 30*1000){
        if(OsPiccPoll((char*)temp,temp2) == 0){
            if(strncmp((const char *)temp,"B",1) == 0){
                TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"B Card");
            }else{
                TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"A Card");
            }
            ret = OsPiccIsoCommand(0,&ApduReq,&ApduRsp);
            if(ret == 0){
                TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"APDU Communication Successful");
            }else{
                TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"APDU Communication Faild");
            }
            goto EXIT;
        }
         ret = KB_nWaitKeyMS(500);
		if(ret == EM_KEY_CANCEL){
            OsPiccClose();
            return;
        }
    }
    
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Read Card Timeout");
EXIT:
    OsPiccClose();
    KB_nWaitKeyMS(5*1000);
}

void PrintTest(int nCount)
{
    PR_INT32 ret;
    PR_INT32 nElecSignRet;
    PR_UINT8 temp[256];
    PR_INT8 FilePath[128] = {0};
	PR_INT8 AppPath[128] = {0};
    PR_INT8 hw_version[8] = {0};

    PR_UINT8 elceSignBuf[30*1024] ={0};
    PR_INT32 elceSignLen = 0;
    PR_INT8 szCode[] = "45323233";
    PR_INT32 elceSignTimeOut = 60;
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Print");
    nElecSignRet = Disp_nElecSignEx(elceSignBuf,&elceSignLen,NULL,0.5,elceSignTimeOut,10,60,300,140,1);
    if(nElecSignRet != 0){
        TransView_vClearAllScreen();
        TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Print");
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"ElecSign Failed");
        KB_nWaitKeyMS(3*1000);
    }
    OsGetHwVersion(hw_version);
    if(hw_version[0] == '4'){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Printer does not exist");
        KB_nWaitKeyMS(5*1000);
        return;
    }

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Print");
    ret = OsPrnOpen(PRN_REAL,NULL);
    if(ret != RET_OK){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open Device Failed");
        KB_nWaitKeyMS(5*1000);
        return;
    }
    OsPrnSetGray(1);
    OsPrnSetSpace(0,4);
    ret = OsPrnCheck();
    if( ret != RET_OK){
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,ret == ERR_PRN_BUSY?(char*)"Printer Busy":
                    ret == ERR_PRN_PAPEROUT?(char*)"Printer Paper Out":
                    ret == ERR_PRN_OVERHEAT?(char*)"Printer Overheat":
                    ret == ERR_PRN_OVERVOLTAGE?(char*)"Printer Overvoltage ":
					ret == ERR_BATTERY_VOLTAGE_TOO_LOW?(char*)"Printer Voltage Too Low":(char*)"Printer Exception");
        goto exit;
    }

    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Printing");
    for(int i = 0;i< nCount;i++){
        OsPrnReset();
        Business_getAppPath(AppPath,sizeof(AppPath));
        sprintf(FilePath,"%s/res/yl.bmp",AppPath);
        OsLog(LOG_WARN,"bmp path： %s",FilePath);
        OsPrnPutImage((unsigned char *)FilePath);
        OsPrnSetPrintParamsEx(32,1,1,ALIGN_TYPE_CENTER,1);
        OsPrnPrintf((char *)"به استفاده خوش آمدید");
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
            TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,ret == ERR_PRN_BUSY?(char*)"Printer Busy":
                        ret == ERR_PRN_PAPEROUT?(char*)"Printer Paper Out":
                        ret == ERR_PRN_OVERHEAT?(char*)"Printer Overheat":
                        ret == ERR_PRN_OVERVOLTAGE?(char*)"Printer Overvoltage ":
                        ret == ERR_BATTERY_VOLTAGE_TOO_LOW?(char*)"Printer Voltage Too Low":(char*)"Printer Exception");
            goto exit;
        }
    }
    OsPrnFeed(48);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Print Complete");
exit:
    OsPrnClose();
    KB_nWaitKeyMS(5*1000);
}

void wirelessTest(){
    PR_INT32 ret;
    PR_INT8 temp[128] = {0};
    PR_INT8 networkType[16] = {0};
    PR_INT8 oper[8] = {0};
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Wireless");
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Network Connecting");
    OsWlLock();
    ret = OsWlInit("");
    if(ret != RET_OK){
         TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sim Not Inserted");
         KB_nWaitKeyMS(5*1000);
         return;
    }
    ret = OsWlCheck();
    if(ret != RET_OK){
        ret = OsWlLogin(NULL,NULL,NULL,0,20*1000,0,0);
        if(ret != RET_OK){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"PPP Connect Failed");
            KB_nWaitKeyMS(5*1000);
            return;
        }
    }
    ret = OsWlGetQNetworkInfo(networkType, oper);
    if(ret != RET_OK){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Get Network Info Failed");
        KB_nWaitKeyMS(5*1000);
        return;
    }
    OsLog(LOG_DEBUG,"networkType  [%s]  oper  [%s]",networkType,oper);
    ppp_connect_test();
// OsWlLogout();
// OsWlUnLock();
    KB_nWaitKeyMS(5*1000);
}

char** ConvertList(int nItemCount,ST_WifiApInfo * Aps,char* pWifiName){
	char** p = (char**)malloc(nItemCount * sizeof(char*));  
	int len = 0;

	if (p == NULL) {  
        return NULL;  
    } 
	
    for (int i = 0; i < nItemCount; i++) {  
		len = strlen(Aps[i].Essid)+1;
        p[i] = (char*)malloc(len+16); 
  
        if (p[i] == NULL) { 
            return NULL;  
        }  
        memset(p[i],0x0,len);
		memcpy(p[i],Aps[i].Essid,len-1);
        if(strlen(pWifiName) > 0 && memcmp(Aps[i].Essid,pWifiName,len-1)== 0){
            strcat(p[i],"   Connected");
        }
    }
	return p;
}

void Freelist(int nItemCount,char** list){
	for (int i = 0; i < nItemCount; i++) {  
        free(list[i]);  
    }  
    free(list);  
}

void wifiTest(){
    PR_INT32 i, num,ret;
    ST_WifiApInfo * Aps;
    ST_WifiApSet Ap;
    PR_INT8 ssid[256] = {0};
    PR_INT8 bssid[20] = {0};
    PR_INT8 szpwd[64] = {0};
    
    PR_INT8** pszList = NULL;
    PR_INT32 rssi,nIndexOfSelect;
    PR_INT32 nselectNum;

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"WIFI");
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open WIFI");
    ret = OsWifiOpen();
    if(ret != RET_OK){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open WIFI Failed");
        KB_nWaitKeyMS(5*1000);
        goto EXIT;
    }
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Scanning");
    num = OsWifiScan (&Aps);
    if(num <= 0){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"No Available Hotspots");
        goto EXIT;
    }
    OsWifiCheck(ssid,bssid,&rssi);
    pszList = ConvertList(num,Aps,ssid);
    if(pszList == NULL){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Get WIFI List Failed");
        goto EXIT;
    }
    TransView_vClearPort();
    ret = TransView_nShowListViewEx((char*)"WIFI",pszList,num,30, &nIndexOfSelect);
    if(ret != PR_NORMAL){
        goto RETURN;
    }
    if(strlen(ssid) > 0 && memcmp(Aps[nIndexOfSelect].Essid,ssid,strlen(Aps[nIndexOfSelect].Essid))== 0){
        if(RET_DPORT_OK == TransView_nDoMessageBox((char*)"Warning",BOX_STYLE_OK|BOX_STYLE_CANCEL,VIEW_TIMEOUT_FOREVER,(char*)"Disconnect The WIFI Connection?")){
            OsWifiDisconnect();
            OsWifiClose();
            goto RETURN;
        }else{
            ppp_connect_test();
            goto EXIT;
        }
    }
    OsLog(LOG_ERROR,"nIndexOfSelect =  %d", nIndexOfSelect);
    memset(&Ap,0x0,sizeof(ST_WifiApSet));
    memcpy(Ap.Essid,Aps[nIndexOfSelect].Essid,strlen(Aps[nIndexOfSelect].Essid));
    Ap.AuthMode = Aps[nIndexOfSelect].AuthMode;
    Ap.SecMode = Aps[nIndexOfSelect].SecMode;
    OsLog(LOG_ERROR,"WIFI Name =  %s", Ap.Essid);
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"WIFI");
    ret = TransView_nInputPwd((char*)"Pls Enter Password",32,8,sizeof(szpwd),szpwd);
    if(ret < PR_NORMAL){
        goto RETURN;
    }

    memcpy(Ap.KeyUnion.PskKey.Key,szpwd,strlen(szpwd));
    Ap.KeyUnion.PskKey.KeyLen = strlen(szpwd);
    TransView_vClearPort();
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"WIFI Connecting");
    ret = OsWifiConnect(&Ap,15*1000);
    if(ret != 0){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"WIFI Connect Failed");
        goto EXIT;
    }
    ret = OsWifiCheck(ssid,bssid,&rssi);
    if(ret != 0){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Get WIFI Status Failed");
        goto EXIT;
    }
    OsLog(LOG_ERROR,"ssid 【%s】 bssid 【%s】 rssi 【%d】 \n",ssid,bssid,rssi);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"WIFI Connect Success");
    OsSleep(1000);
    ppp_connect_test();
EXIT:
    KB_nWaitKeyMS(5*1000);
RETURN:
    if(Aps != NULL){
        free(Aps);
    }
    if(pszList != NULL){
        Freelist(num,pszList);
    }    
}

void LedTest(){
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Led Test");

    TransView_vShowLine(2, EM_DTYPE_NORMAL, EM_ALIGN_CENTER,(char*)"Testing");
    OsLed(1,0,0,0);
    OsSleep(1*1000);
    OsLed(1,1,0,0);
    OsSleep(1*1000);
    OsLed(1,1,1,0);
    OsSleep(1*1000);
    OsLed(1,1,1,1);
    OsSleep(1*1000);
    OsLed(0,0,0,0);
    OsSleep(1*1000);
    OsLed(1,1,1,1);
    TransView_vShowLine(2, EM_DTYPE_NORMAL, EM_ALIGN_CENTER,(char*)"Tests Complete");
    KB_nWaitKeyMS(3*1000);
    OsLed(0,0,0,0);
}

void AboutTest(){
    PR_INT32 ret;
    PR_INT8 temp[64] = {0};
    PR_INT8 imei[32] = {0};
    PR_INT8 mcuVer[32] = {0};
    PR_INT8 sysVer[32] = {0};
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"About");
    
    OsGetTermSn(temp);
    OsWlGetImei(imei);
    OsGetSysVer(0x01,sysVer);
    OsGetSysVer(0x03,mcuVer);
    TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"SN:%s",temp);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"IMEI:%s",imei);
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"MCU Ver:%s",mcuVer);
    TransView_vShowLineEx(4,EM_ALIGN_LEFT,(char*)"SYS Ver:%s",sysVer);
    KB_nWaitKeyMS(10*1000);
}


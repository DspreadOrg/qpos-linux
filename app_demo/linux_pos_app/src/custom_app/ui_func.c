
#include "ui_func.h"

#define DEFAULT_UI_TIMEOUT			30


KB_KEYMAP_T KB_nWaitKeyMS(Int32 MS)
{
	Disp_ReleasKey(); //清除缓存键值
	return  Disp_GetKey(MS);
}

KB_KEYMAP_T KB_nWaitKeyMS_Ex(Int32 MS)
{
	return  Disp_GetKey(MS);
}

void KB_vReleasKey(){
	Disp_ReleasKey();
}

PR_INT32 	TransView_nShowUserInputViewEx_SupportDefaultIM(PR_INT32  nAtLine,
										   PR_INT32  nLines,
										   PR_UINT32  *nTypeOfIM,
										   PR_INT32  nTypeOfIMLen,
										   PR_INT32  nMaxLength,
										   PR_INT32  nMinLength,
										   PR_INT32  nLenOfOutputBuffer, // 传入Buffer长度
										   PR_INT8*  pcOutput)
{
	PR_INT32 nRet = 0;
	PR_INT32 nOffset = 0;
	PR_INT32 TimeOut = DEFAULT_UI_TIMEOUT;
	if (nLenOfOutputBuffer < nMinLength)
	{
		return PR_FAILD;
	}

	nOffset += 1;
	nRet =  Disp_nUserInputViewEx(DEFAULT_LEFT_MARGIN, DEFAULT_LINE_YPOINT(nAtLine), DEFAULT_PORT_WIDTH, (DEFAULT_HEIGHT_LINE*nLines),
							nMinLength,(nMaxLength + nOffset),nTypeOfIM, nTypeOfIMLen,nLenOfOutputBuffer, (PR_UINT8*)pcOutput, TimeOut, DPORT_DEFAULT_STYLE|DPORT_ENABLE_LANGEN);

	if (nRet == RET_DPORT_FAILD || nRet == RET_DPORT_TIMEOUT || nRet == RET_DRPOT_CANCEL){
		nRet = PR_FAILD;
	}

	return nRet;
}

PR_INT32 TransView_nShowPinpadView(PR_INT32 KeyIdx,PR_INT8* pszAmount,PR_UINT8 *DataIn, PR_INT8 *ExpPinLen, PR_INT32 Mode, PR_UINT64 TimeoutS, PR_UINT8 *pPinBlock)
{
    PR_INT32 nRet = PR_FAILD;
	PR_INT8 DisplayAmount[12+2] = {0};
	TransView_vClearPort();

	nRet = OsPedOpen();
	if(nRet != RET_OK){
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"SE Exception");
		return PR_FAILD;
	}
	PR_nUtilNumberToAmt(pszAmount,sizeof(DisplayAmount),DisplayAmount);
    TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"%s",DisplayAmount);
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"Pls Enter PIN");
	TransView_vShowLine(4,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"If Wrong Press [CLEAR]");
	OsPedIncreaseKsnDukpt(PIN_KEY_INDEX);
	nRet = OsPedGetPinBlock(KeyIdx,(PR_UINT8*)DataIn,(char *)"0,6",0x00,TimeoutS*1000, pPinBlock);
	dbg_printf("====%d",nRet);
	
	OsPedClose();
	switch(nRet)
	{
		case RET_OK:
			// GetPinBlock(pPinBlock,DataIn,DES_CALC_KEY_INDEX);//todo
			return PR_NORMAL;
		case ERR_PED_PIN_INPUT_CANCEL:
			return RET_DRPOT_CANCEL;
		case ERR_PED_NO_PIN_INPUT:
			return PR_NORMAL;
		case ERR_PED_INPUT_PIN_TIMEOUT:
			return RET_DPORT_TIMEOUT;
			
		default:
			return PR_FAILD;
	}
}

PR_INT32 TransView_nGetOfflinePin(PR_INT8* szAmount,
										PR_INT32 nOfflinePinType,
										PR_INT32 nAtLine,
										PR_INT32 nMaxOfPin,
										PR_INT32 nMinOfPin,
										PR_INT32 nLenOfOfflinePinBuffer,
										PR_INT8* pcOfflinePinOutput)
{
	PR_INT8* pTip = (nOfflinePinType == EMV_OFFLINEPIN_INPUT) ? "Pls Enter Offline PIN" :  "Pls Enter Last Offline PIN";
	TransView_vClearPort();
	TransView_vShowLine(4,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,"If Wrong Press [CLEAR]");
	if(szAmount != PR_NULL && strlen(szAmount) > 0)
	{
		PR_INT8 DisplayAmount[12+2] = {0};
		PR_nUtilNumberToAmt(szAmount,sizeof(DisplayAmount),DisplayAmount);
		TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"%s",DisplayAmount);
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"%s",pTip);
	}
	else
	{
		//查询类交易不需要显示金额
		TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_LEFT, (char*)"%s",pTip);
	}
	PR_INT8 szPan[19+1] = {0};
	InputPinPara szInputPin;
	memset(&szInputPin, 0x00, sizeof(InputPinPara));
	memset(szPan,0x30,16);

	strcpy(szInputPin.szPan, szPan);
	szInputPin.nPanLen = strlen(szPan);
	szInputPin.nKeyIndex = 1;
	szInputPin.nPinMode = 0x01; //固定01 脱机pin
	szInputPin.nMaxPinLen = nMaxOfPin;
	szInputPin.nMinPinLen = nMinOfPin; 
	PR_INT32 nRet = Disp_nShowPINPADInputView(DEFAULT_LEFT_MARGIN,
			   	   	   	   	   	   	   	   (DEFAULT_LINE_YPOINT(nAtLine+1)),
			   	   	   	   	   	   	   	   DEFAULT_PORT_WIDTH,
			   	   	   	   	   	   	   	   DEFAULT_PORT_HEIGHT,
										   &szInputPin, nLenOfOfflinePinBuffer,DEFAULT_UI_TIMEOUT,pcOfflinePinOutput);//默认30秒超时

	switch(nRet)
	{
		case RET_DPORT_FAILD:
			return PR_FAILD;

		case RET_DPORT_BYPASS:
			return PR_BYPASS;

		case RET_DRPOT_CANCEL:
			return PR_CANCLE;
		case RET_DPORT_TIMEOUT:
			return PR_TIMEOUT;

		default:
			return PR_NORMAL;
	}
}



#define PRINT_START_TO_BUFFER(BUFFER,MAX_BUFFER_LEN,FMT) 	 va_list va;\
                                                             va_start(va, FMT); \
                                                             vsnprintf((char *)(&BUFFER),MAX_BUFFER_LEN, FMT, va)

#define PRINT_STOP_TO_BUFFER() va_end(va)

//提示框控件
PR_INT32 TransView_nDoMessageBox(PR_INT8 *szTitle, PR_UINT32 theStype, PR_INT32 nTimeoutSec,PR_INT8 *args,...)
{
	PR_INT8 szBuffer[200+1]={0};
	PR_INT32 nRet; 
	PRINT_START_TO_BUFFER(szBuffer, sizeof(szBuffer), args);
	PRINT_STOP_TO_BUFFER();
	
	return Disp_nMessageBox(szTitle, theStype, nTimeoutSec,DPORT_DEFAULT_STYLE, "%s", szBuffer);
}


PR_INT32 TransView_nShowTableViewEx(PR_INT8* pszTitle,PR_INT8** pszListItem,PR_INT32 nItemCount,PR_INT32 nTimeoutMs, PR_INT32 *pnIndexOfSelect)
{
	PR_INT32 nRet;

	if (pszListItem == NULL || pnIndexOfSelect == NULL)
	{
		return PR_FAILD;
	}

	nRet = Disp_nShowTableViewEx(pszTitle, pszListItem, nItemCount, 10, *pnIndexOfSelect, nTimeoutMs, pnIndexOfSelect, DPORT_AUTO_INDEX_SEPARATELY);
	switch(nRet)
	{
		case RET_DPORT_NORMAL:
			return PR_NORMAL;

		case RET_DRPOT_CANCEL:
			return RET_DRPOT_CANCEL;
			
		case RET_DPORT_TIMEOUT:
			return RET_DPORT_TIMEOUT;
			
		default:
			return PR_FAILD;
	}

}

PR_INT32 TransView_nShowListViewEx(PR_INT8* pszTitle,PR_INT8** pszListItem,PR_INT32 nItemCount,PR_INT32 nTimeoutMs, PR_INT32 *pnIndexOfSelect)
{
	PR_INT32 nRet;

	if (pszListItem == NULL || pnIndexOfSelect == NULL)
	{
		return EM_KEY_INVALID;
	}
	
	nRet = Disp_nShowListViewEx(pszTitle, pszListItem, nItemCount, 7, *pnIndexOfSelect, nTimeoutMs, pnIndexOfSelect, DPORT_AUTO_INDEX_CONTINUITY);
	switch(nRet)
	{
		case RET_DPORT_NORMAL:
			return PR_NORMAL;

		case RET_DRPOT_CANCEL:
			return RET_DRPOT_CANCEL;
			
		case RET_DPORT_TIMEOUT:
			return RET_DPORT_TIMEOUT;
			
		default:
			return PR_FAILD;
	}
}


PR_INT32 TransView_nShowAmountInputViewEx(CurrencyIndicate_T nIndicate,
											 PR_INT32 nAtLine,
											 PR_INT32 nLenOfBuffer,
											 PR_INT8* pszBuffer)
{
	const PR_INT8* aszCurrencyIndicateStrings[]=
	{
			"RMB:",
			"USD:",
			"HKD:",
			"MOPD:",
			"",
			" ",
	};

//30, 30*(nRowIndex+1)
	PR_INT32 nMaxNumsOfIndicate = sizeof(aszCurrencyIndicateStrings)/sizeof(PR_INT8*)-1;
	PR_INT32 TimeOut = DEFAULT_UI_TIMEOUT;
	PR_INT32 nOffset = 0;

	nIndicate = ((nIndicate > nMaxNumsOfIndicate)||(nIndicate < 0))?0:nIndicate;


	//输入的长度限制为： 限制的数字+1个小数点+货币符号长度+1个空格
	nOffset = strlen((PR_INT8*)aszCurrencyIndicateStrings[nIndicate]) + 1 + 1;

	return (Disp_nShowAmountInputView(DEFAULT_LEFT_MARGIN,
									  DEFAULT_LINE_YPOINT(nAtLine),
									  DEFAULT_PORT_WIDTH,
									  DEFAULT_PORT_HEIGHT,
									  //(PR_INT8*)aszCurrencyIndicateStrings[nIndicate],
									  (PR_INT8*)"",
									  1,(9 + nOffset),nLenOfBuffer, pszBuffer, TimeOut, DPORT_DEFAULT_STYLE|DPORT_ENABLE_LANGEN));//默认超时5秒


}

void TransView_vShowLogoView()
{
	TransView_vClearPort();
	TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"%s","Welcome!");
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"%s","Dspread");
	TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"%s","V1.0_240702");
	ST_TIME time;
	OsGetTime(&time);
	
	TransView_vShowLine(5,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"%02d-%02d-%02d  %02d:%02d",time.Year,time.Month,time.Day,time.Hour,time.Minute);
}

PR_INT32 TransView_nShowMsgBox(PR_INT8* szTitle,PR_INT8* szDisplayMsg)
{
	return Disp_nMessageBoxEx(DEFAULT_LEFT_MARGIN, DEFAULT_LINE_YPOINT(2), DEFAULT_PORT_WIDTH, DEFAULT_PORT_HEIGHT*5,
							szTitle, EM_STYLE_OK|EM_STYLE_CANCEL,VIEW_TIMEOUT_FOREVER, DPORT_DEFAULT_STYLE , szDisplayMsg);
}

void  TransView_vClearPort(void)
{
	Disp_vClearLine(1);
	Disp_vCleanArea(0,(24 + 36),320, 200);
}

PR_INT32 TransView_nNumber_En_Symbol_InputViewEx(PR_INT8*  pszTip,
																	PR_INT32  MaxLength,
																	  PR_INT32  MinLength,
																	  PR_INT32  nLenOfOutputBuffer, // 传入Buffer长度
																	  PR_INT8*  pcOutput)
{
	TransView_vClearPort();
	TransView_vShowLine(1,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,"%s",pszTip);
	PR_UINT32 acIme[3]={DPORT_IME_NUM,DPORT_IME_EN,DPORT_IME_SYMBOL};
	return TransView_nShowUserInputViewEx_SupportDefaultIM(3,2,acIme,3,MaxLength,MinLength,nLenOfOutputBuffer, pcOutput);
}


PR_INT32 TransView_nShowAmountInputView(PR_INT32 nLenOfBuffer, PR_INT8* pszAmountoutput)
{	
	PR_INT8 szAmount[12+1]={0};

	TransView_vClearPort();
	TransView_vShowLine(1,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,"Pls Enter Amount: ");
	TransView_vShowLine(4,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,"If Wrong Press [CLEAR]");
	PR_INT32 nRet = TransView_nShowAmountInputViewEx(INDICATE_NODISP, 3, sizeof(szAmount), szAmount);
	if(nRet <= 0)
		return PR_FAILD;

	strncpy(pszAmountoutput,szAmount, nLenOfBuffer);

	dbg_printf("=amt = %d",nRet);
	dbg_printfWHex(szAmount,12,"hex amt");
	return nRet;
}



int DispWifiList2(const char * tip_msg,T_U8_VIEW list[],u32 list_cnt){

	char wifi_disp[MAX_SIZE_NAME_WIFI]={0};
	int i=0;

	TransView_vClearPort();

	TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,"Select Wi-Fi");
	for(i=0;i<list_cnt;i++)
	{
		memset(wifi_disp,0x00,MAX_SIZE_NAME_WIFI);
		memcpy(wifi_disp,list[i].head,list[i].len);
		OsLog(LOG_DEBUG,"--i=%d--%s",i,wifi_disp);


	}



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

void  wifiSet(){
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
            goto EXIT;
        }
    }
    OsLog(LOG_ERROR,"nIndexOfSelect =  %d", nIndexOfSelect);
    memset(&Ap,0x0,sizeof(ST_WifiApSet));
    memcpy(Ap.Essid,Aps[nIndexOfSelect].Essid,strlen(Aps[nIndexOfSelect].Essid));
    Ap.AuthMode = Aps[nIndexOfSelect].AuthMode;
    Ap.SecMode = Aps[nIndexOfSelect].SecMode;
    OsLog(LOG_ERROR,"WIFI名称 =  %s", Ap.Essid);
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"WIFI");
    ret = TransView_nNumber_En_Symbol_InputViewEx((char*)"Pls Enter Password",32,8,sizeof(szpwd),szpwd);
    if(ret < PR_NORMAL){
        goto RETURN;
    }

    memcpy(Ap.KeyUnion.PskKey.Key,szpwd,strlen(szpwd));
    Ap.KeyUnion.PskKey.KeyLen = strlen(szpwd);
    TransView_vClearPort();
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"WIFI Connecting");
    ret = OsWifiConnect(&Ap,10*1000);
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
EXIT:
    KB_nWaitKeyMS(5*1000);
RETURN:
    if(Aps != NULL){
        free(Aps);
    }
    if(pszList != NULL){
        Freelist(num,pszList);
    }    

	// TransView_vClearPort();
	// TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,"Wi-Fi");

	// if(get_wifi_conn_status())
    // {
    //     char wifi_name[MAX_SIZE_NAME_WIFI];
    //     if(getWifiName(wifi_name) )
    //     {
    // 		TransView_vShowLine(1,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,"Current Wifi Info: ");

	// 		TransView_vShowLine(2,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,"Wifi Name: %s",wifi_name);
	// 		TransView_vShowLine(5,EM_ALIGN_RIGHT, EM_DTYPE_NORMAL,"[ENTER]CONTINUE");
	// 		TransView_vShowLine(5,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,"[CANCEL]TERUN");
	// 		int key = 0;
	// 		while(1){
	// 			 key = KB_nWaitKeyMS(60*1000);
	// 			 if(key==EM_KEY_CANCEL ||key == EM_KEY_INVALID){
	// 				reutrn ;
	// 			 }
	// 			 if(key == EM_KEY_ENTER){
	// 				break;
	// 			 }
	// 		}
    //     }	
	// }
	// Wifi_Search();

	// Wifi_Connect();

	// Wifi_Disconnect();
}




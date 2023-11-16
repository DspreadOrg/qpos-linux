
#include "TrancProcView.h"

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
	nRet = OsPedGetPinBlock(KeyIdx,(PR_UINT8*)DataIn,(char *)"0,6",0x00,TimeoutS*1000, pPinBlock);
	OsPedClose();
	switch(nRet)
	{
		case RET_OK:
			return PR_NORMAL;
		case ERR_PED_PIN_INPUT_CANCEL:
			return RET_DRPOT_CANCEL;
			
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

	nRet = Disp_nShowTableViewEx(pszTitle, pszListItem, nItemCount, 8, *pnIndexOfSelect, nTimeoutMs, pnIndexOfSelect, DPORT_AUTO_INDEX_SEPARATELY);
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
									  (PR_INT8*)aszCurrencyIndicateStrings[nIndicate],
									  1,(9 + nOffset),nLenOfBuffer, pszBuffer, TimeOut, DPORT_DEFAULT_STYLE|DPORT_ENABLE_LANGEN));//默认超时5秒


}

void TransView_vShowLogoView()
{
	PR_INT8 FilePath[128] = {0};
	PR_INT8 szDisp[128] = {0};
	PR_INT8 AppPath[128] = {0};
	
	Business_getAppPath(AppPath,sizeof(AppPath));
	sprintf(FilePath,"%s/res/UnionPay.565",AppPath);
	sprintf(szDisp,"CUP\nV310004\nWelcome");
	Disp_vShowLogoView(FilePath, szDisp);
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
	PR_INT32 nRet = TransView_nShowAmountInputViewEx(INDICATE_FOR_RMB, 3, sizeof(szAmount), szAmount);
	if(nRet <= 0)
		return PR_FAILD;

	strncpy(pszAmountoutput,szAmount, nLenOfBuffer);
	return nRet;
}


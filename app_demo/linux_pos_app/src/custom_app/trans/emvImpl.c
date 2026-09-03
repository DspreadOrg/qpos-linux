#include "appinc.h"

extern void InputPinCallback(int pinLen);
static bool Lvgl_Kbd_Flag = false;

#define HIGH_NIBBLE(theByte)	((theByte>>4)&0x0F)	//high half byte
#define LOW_NIBBLE(theByte)	(theByte&0x0F)			//Low half byte
#define PR_ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))

#define EXP_DATE_LENGTH			4
#define SERVICE_CODE_LENGTH		3

PR_UINT8 PR_Bcd2Dec(PR_UINT8 theBcdValue)
{
	return ((theBcdValue>>4)&(0x0F))*10 + (theBcdValue&(0x0F));
}

PR_BOOL CardDataIsNum(PR_INT8 Data){
	if(Data>='0'&&Data<='9')
		return PR_TRUE;
	else
		return PR_FALSE;
}

#define RID_LENGTH 5
const PR_INT8* s_CardCompany[] = {
    "CUP",
    "VIS",
    "MCC",
    "JCB",
    "DCC",
    "AMX",
};

const PR_INT8 s_RidList[][RID_LENGTH] = {
    {0xA0, 0x00, 0x00, 0x03, 0x33},
    {0xA0, 0x00, 0x00, 0x00, 0x03},
    {0xA0, 0x00, 0x00, 0x00, 0x04},
    {0xA0, 0x00, 0x00, 0x00, 0x65},
    {0xA0, 0x00, 0x00, 0x00, 0x31},
    {0xA0, 0x00, 0x00, 0x00, 0x25},
};

typedef enum
{
	DUKPT_DEC_ECB = 0x00,
	DUKPT_ENC_ECB = 0x01,
    DUKPT_DEC_CBC = 0x02,
	DUKPT_ENC_CBC = 0x03,
} DUKPT_DES_MODE;

int DataEncrypt(int keyIndex,unsigned char *inData,int inDataLen,unsigned char *outData,unsigned char *ksn)
{
	/*update ksn*/
    OsPedIncreaseKsnDukpt(keyIndex);

	    int ret = -1;
    int reLen =  (inDataLen+7)/8*8;

    unsigned char *tempBuf = NULL;
    tempBuf = malloc(reLen+1);
    memset(tempBuf,0,reLen+1);
    memcpy(tempBuf,inData,inDataLen);

    ret = OsPedOpen();
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Dspread: -----------------%s-----%d   ret= %d",__FUNCTION__,__LINE__,ret);
        return 0;
    }

    ret = OsPedDesDukpt(keyIndex,0x01,NULL, reLen, tempBuf, outData, ksn,DUKPT_ENC_CBC);
    if(ret != RET_OK){
        OsPedClose();
        return 0;
    } 

    OsPedClose();   
    return reLen;
}

PR_Bool GetIccCardCompany(PR_INT8* theCompany)
{
	PR_UINT8* rid = 0;
	PR_INT32 length = 0;
	PR_Bool getCardCompanyOk = PR_FALSE;

	rid = Emv_GetCoreData(EMVTAG_AID, &length);
	if (rid && (length>=RID_LENGTH))
	{
		PR_INT32 i=0;

		for(; i<PR_ARRAY_SIZE(s_RidList); i++)
		{
			if (memcmp(s_RidList[i], rid, RID_LENGTH) == 0)
			{
				strcpy(theCompany, s_CardCompany[i]);
				getCardCompanyOk = PR_TRUE;
			}
		}
	}

	return getCardCompanyOk;
}

//Multiple application selection return value>=0. Return selected AID serial number -1 failed
int aidSelect(AidCandidate_t *pList, int listNum){

	unpackAppsName(listNum,pList);
	GuiEventRegister(LCD_DISP_SELECT_APP);
	get_transaction_data()->emv_multi_app_select_result = 0xFF;
	while(get_transaction_data()->emv_multi_app_select_result == 0xFF)
	{
		OsSleep(100);
	}
	GuiEventRegister(LCD_DISP_READING_CARD);
    return get_transaction_data()->emv_multi_app_select_result;
}
//Card number confirmation return value 0 Confirm-1 failed
int confirmCardInfo(char *pan,int len){
	OsLog(LOG_DEBUG,"pan [%s]",pan);
	memcpy(get_transaction_data()->sCardNo,pan,len);
    return 0;
}

int Emv_GetPanByTag5A(char* pan){
	int len = 0;
	unsigned char *value = NULL;
	char sepflag = 0;
	char *p = NULL;
	char szPan[32];

	if(pan == NULL){
		return -1;
	}
	value = Emv_GetCoreData(0x5A, &len);
	OsLog(LOG_DEBUG,"5A Len[%d]",len);
	if(!value || len < 0){
		value = Emv_GetCoreData(0x57, &len);
		sepflag = 1;
		OsLog(LOG_DEBUG,"5A Len[%d]",len);
	}
	if(len > 0){
		memset(szPan,0,sizeof(szPan));
		PR_nUtilBcd2Asc(len,(char *)value, 0,sizeof(szPan), szPan);
		OsLog(LOG_DEBUG,"szPan1 [%s]",szPan);
		p = strstr(szPan,"D");
		if(p){
			*p = 0x00;
		}
		OsLog(LOG_DEBUG,"szPan2 [%s]",szPan);
		strcpy(pan,szPan);
		return 0;
	}
	OsLog(LOG_DEBUG,"Emv_GetPanByTag5A Failed");
	return -1;
}

bool get_Lvgl_Kbd_Flag()
{
    return Lvgl_Kbd_Flag;
}

int inputOfflinePasswd(char *pPin)
{
    int ret,pinLength;
    int keyValue = EM_KEY_INVALID; 
    char pin[12] = {0};
    int pinLenMin = 0;
    int pinLenMax = 6;
    ST_TIMER Timer;
    int counter = 0;

    Lvgl_Kbd_Flag = true;

    if (( ret = OsTimerSet(&Timer, 60*1000)))
    {
		Lvgl_Kbd_Flag = false;
        return -1;
    }
    pinLength = 0;
    Disp_ReleasKey();
    while(1)
    {
	    keyValue = Disp_GetKey(10);
        switch(keyValue)
        {
        case EM_KEY_1:
        case EM_KEY_2:
        case EM_KEY_3:
        case EM_KEY_4:
        case EM_KEY_5:
        case EM_KEY_6:
        case EM_KEY_7:
        case EM_KEY_8:
        case EM_KEY_9:
            if(pinLength < pinLenMax)
            {
                pin[pinLength++] = '0' + (keyValue+1);
                pin[pinLength] = '\0';
                InputPinCallback(pinLength);
            }
            break;
        case EM_KEY_0:
            if(pinLength < pinLenMax)
            {
                pin[pinLength++] = '0';
                pin[pinLength] = '\0';
                InputPinCallback(pinLength);
            }
            break;
        case EM_KEY_CLEAR:
            if(pinLength > 0)
            {
                pinLength--;
                pin[pinLength] = '\0';
                InputPinCallback(pinLength);
            }
            break;
        case EM_KEY_CANCEL:
            ret = PR_CANCLE;
             Disp_ReleasKey();
            goto EXIT;
            break;
        case EM_KEY_ENTER:
            if(pinLength >=pinLenMin && pinLength <= pinLenMax)
            {
                memcpy(pPin,pin,pinLength);
                ret = pinLength;
                goto EXIT;
            }
            break;            
        default:
            break;    
        };
        if((long)OsTimerCheck(&Timer) <= 0)
        {
            ret = PR_TIMEOUT;
            break;
        }
    }
EXIT:
    Disp_ReleasKey();
    Lvgl_Kbd_Flag = false;

    return ret;
}

int inputPasswd(int type, char *pszPin){
	int ret ;
	unsigned char pinKsn[16] = {0};
	ClearMaskPin();
	if(type == EMV_ONLINEPIN_INPUT)
	{
		get_transaction_data()->emv_emter_online_pin_result =0xFF;
		GuiEventRegister(LCD_DISP_INPUT_ONLINE_PIN);
		ret = OsPedOpen();
		if(ret != RET_OK)
		{
			return PR_FAILD;
		}

		ret = OsPedIncreaseKsnDukpt(PED_PIN_IPEK_INDEX);
		if(ret != RET_OK){
			OsPedClose ();
			return PR_FAILD;
		}

		ret = OsPedGetPinDukptEx(PED_PIN_IPEK_INDEX,get_transaction_data()->sCardNo, (char *)"0,4,5,6", 0x20, 60*1000,get_transaction_data()->sPinKsn, pszPin,InputPinCallback);
		OsLog(LOG_DEBUG,"--------OsPedGetPinDukptEx ret = %d",ret);
		OsPedClose ();
		if(ret == RET_OK)
		{
			get_transaction_data()->emv_emter_online_pin_result = 1;
			memcpy(get_transaction_data()->sPin,pszPin,8);
			return 8;
		}
		else if (ret == ERR_PED_NO_PIN_INPUT)
		{
			return 0;
		}
		else
			return -1;
	}
	else
	{
		GuiEventRegister(LCD_DISP_INPUT_OFFLINE_PIN);
		ret = inputOfflinePasswd(pszPin);
	}
	GuiEventRegister(LCD_DISP_READING_CARD);
    return ret;
}

PR_BOOL Emv_ParseTrack2(TransactionData *pEmvTransData)
{
	PR_UINT32 i=0;
	PR_UINT32 cardNumLength = 0;

	if(pEmvTransData->nTracker2Len <= 0){
		return PR_FALSE;
	}
	for( ;i<pEmvTransData->nTracker2Len; i++)
	{
		if(CardDataIsNum(pEmvTransData->sTracker2[i])==PR_FALSE)
		{
			// The card number starts from the starting character (1 byte) and ends with the delimiter
			#define SIZE_OF_START_FLAG 0
			cardNumLength = i-SIZE_OF_START_FLAG;	
			if(strlen(pEmvTransData->sCardNo) <= 0){
				memcpy(pEmvTransData->sCardNo, pEmvTransData->sTracker2+SIZE_OF_START_FLAG, cardNumLength);
			}
			
			memcpy(pEmvTransData->sExpDate, pEmvTransData->sTracker2+i+1, EXP_DATE_LENGTH);
			memcpy(pEmvTransData->sServiceCode, pEmvTransData->sTracker2+i+1+EXP_DATE_LENGTH, SERVICE_CODE_LENGTH);
			break;
		}
	}

	return PR_TRUE;
}


PR_INT32 Emv_GetCardInfo(TransactionData *pEmvTransData){
	PR_UINT8* value = PR_NULL;
	PR_INT32 length = 0;
	PR_INT32 rawLength = 0;
	PR_Bool havePanTag;
	PR_UINT8 buf[256];

	value = Emv_GetCoreData(EMVTAG_APP_PAN_SN, &length);
	if (value){
		sprintf((PR_INT8*)pEmvTransData->sCardSN, "%03d", PR_Bcd2Dec(*value));
	}
	//TRACK2 DATA
	value = Emv_GetCoreData(EMVTAG_TRACK2, &length);
	if (value){
		//Determine if there is an additional right complement F
		PR_INT32 i;
		PR_INT32 offset = 0;
		for (i=0; i<length; i++){
			PR_UINT8 hinibble;
			PR_UINT8 lonibble;

			hinibble = HIGH_NIBBLE(value[i]);
			lonibble = LOW_NIBBLE(value[i]);
			if(hinibble== 0x0d){
				offset = i*2;
			}else if(lonibble==0x0d){
				offset = i*2+1;
			}
		}
		rawLength = (LOW_NIBBLE(value[length-1]) == 0xF) ? 2*length-1 : 2*length;
		if(PR_FAILD == PR_nUtilHex2Asc(length,value,PR_ARRAY_SIZE(buf), (char*)buf)){
			return PR_FAILD;
		}
		strcpy((PR_INT8*)pEmvTransData->sTracker2, (PR_INT8*)buf);
		// A regular magnetic stripe card is'='; Equivalent data bit 'D' of IC card, uniformly converted to '=';
		if (offset > 0){
			pEmvTransData->sTracker2[offset] = '=';
		}
		pEmvTransData->nTracker2Len = rawLength;
		// Obtain relevant data from equivalent 2 magnets.
		Emv_ParseTrack2(pEmvTransData);
		value = Emv_GetCoreData(EMVTAG_APP_EXPDATA, &length);
		if(value)
		{
			PR_INT8 acExpDate[EXP_DATE_LENGTH+1] = {0};
			PR_INT32 nAscLen = 0;
			nAscLen = PR_nUtilBcd2Asc(2,(PR_INT8*)value,EM_ALIGN_LEFT,sizeof(acExpDate),acExpDate);
			memcpy((PR_INT8*)pEmvTransData->sExpDate,acExpDate,nAscLen);
		}
	}
	if (PR_FALSE==GetIccCardCompany((PR_INT8*)pEmvTransData->sCardCompany))
	{
		strcpy((PR_INT8*)pEmvTransData->sCardCompany, "000");
	}
	return PR_NORMAL;
}

PR_INT32 Pack_EmvData(PR_UINT8* pEmvData,PR_INT32 *pEmvDataLen)
{
	PR_UINT8 buf[512];
	PR_UINT32 totalLength;
	PR_UINT32 tags[] = {EMVTAG_AC, EMVTAG_CID, EMVTAG_IAD, EMVTAG_RND_NUM,
						EMVTAG_ATC, EMVTAG_TVR, EMVTAG_TXN_DATE, EMVTAG_TXN_TYPE,
						EMVTAG_AMOUNT, EMVTAG_CURRENCY, EMVTAG_AIP, EMVTAG_COUNTRY_CODE,
						EMVTAG_OTHER_AMOUNT, EMVTAG_TERM_CAP, EMVTAG_CVM, EMVTAG_TERM_TYPE,
						EMVTAG_IFD, EMVTAG_DF, EMVTAG_APP_VER, EMVTAG_TXN_SN,
						EMVTAG_CARD_ID, EMVTAG_ARC, EMVTAG_EC_AUTH_CODE};
	totalLength = Emv_FetchData(tags, PR_ARRAY_SIZE(tags), buf, PR_ARRAY_SIZE(buf));

	nBcd2Asc(buf,totalLength*2,pEmvData,0);
	//memcpy(pEmvData,buf,totalLength);
	*pEmvDataLen = totalLength*2;
	
	return PR_NORMAL;
}

int onlineProcess(EmvOnlineData_t* pOnlineData){
	GuiEventRegister(LCD_DISP_GO_ONLINE);
	Emv_GetCardInfo(get_transaction_data());
    return sale_online_request(pOnlineData);
}

int certConfirm(unsigned char type, unsigned char *pcon, unsigned char len){
    return 1;
}

int emv_process_disp(EmvKernelDisp type)
{

	switch (type)
	{
	case EMV_DISP_READING_CARD:
		GuiEventRegister(LCD_DISP_READING_CARD);
		break;
	case EMV_DISP_SEE_PHONE:
		GuiEventRegister(LCD_DISP_SEE_PHONE_PLS);
		OsSleep(5000);
		break;
	case EMV_DISP_REMOVE_CARD:
		GuiEventRegister(LCD_DISP_REMOVE_CARD);
		break;
	case EMV_DISP_NFC_RETAP:
		GuiEventRegister(LCD_DISP_NFC_RETAP);
		break;
	default:
		break;
	}
	return 0;
} 
static void Initialize_EMV_CallBackFun(EmvCallBack_t *pcallbackfun)
{
	pcallbackfun->EMV_AidSelect = aidSelect;
	pcallbackfun->EMV_ConfirmCardInfo = confirmCardInfo;
	pcallbackfun->EMV_InputPasswd = inputPasswd;
	pcallbackfun->EMV_CertConfirm = certConfirm;
	pcallbackfun->EMV_ProcessDisp = emv_process_disp;
	pcallbackfun->EMV_OnlineProcess = onlineProcess;
	pcallbackfun->EMV_AfterReadRecord = NULL;
	pcallbackfun->EMV_AfterSelectApp = NULL;
	pcallbackfun->EMV_SetKernelId = NULL;
}

PR_INT32 EmvL2_Init(){
    EmvCallBack_t t_callbackfun;

    Initialize_EMV_CallBackFun(&t_callbackfun);
    if(Emv_Kernel_Init(t_callbackfun) < 0){
        OsLog(LOG_DEBUG,"emv init fail\n");
        return PR_FAILD;
    }
    return PR_NORMAL;
}

void led_disp_nfc_success_prompt()
{
    OsLed(0,0,0,1);
    OsSleep(30);
    OsLed(0,0,1,1);
    OsSleep(30);
    OsLed(0,1,1,1);
    OsSleep(30);
    OsLed(0,0,0,0);
}

void led_disp_nfc_fail_prompt()
{
    OsLed(0,0,0,1);
    OsSleep(30);
    OsLed(0,0,1,1);
    OsSleep(30);
    OsLed(1,1,1,1);
    OsSleep(30);
    OsLed(0,0,0,0);
}

PR_INT32 EmvL2_Proc(EmvTransParams_t emvTransParams){
    EMV_L2_Return nEmvRet = APP_RC_START;
	int ret = PR_FAILD;

    nEmvRet = Emv_Process(emvTransParams);
	if(emvTransParams.icc_type == CONTACTLESS_ICC && nEmvRet == APP_RC_COMPLETED)
	{
		led_disp_nfc_success_prompt();
		// do contactless trans online request
		EmvOnlineData_t emvOnlineData;
		memset(&emvOnlineData,0,sizeof(EmvOnlineData_t));
		ret = onlineProcess(&emvOnlineData);
		if(ret == PR_NORMAL)
		{
			if(memcmp(emvOnlineData.iccResponse,"00",2) == 0 && emvOnlineData.ackdatalen > 0)
				ret = Emv_SetContactlessOnlineResult(KERNEL_POLL_CTL_MODE,&emvOnlineData);
		}

		if(ret == PR_NORMAL)
			nEmvRet = APP_RC_COMPLETED;
		else
			nEmvRet = APP_RC_TERMINAL;
		
	}
#ifdef CFG_OFFLINE_TEST	
		nEmvRet = APP_RC_COMPLETED;
#endif	
    return nEmvRet;
}

int pin_require_check( )
{
	uint i=0x0;
	unsigned char serviceCode[8] = {0};
	OsLog(LOG_DEBUG,"stracker2: \r\n %s",get_transaction_data()->sTracker2);
	while(get_transaction_data()->sTracker2[i] != 0x3d){
 		i++;
	}
	i += 5;
	if(i + 3 <= get_transaction_data()->nTracker2Len){
		memcpy(serviceCode,&get_transaction_data()->sTracker2[i],3);
		if(serviceCode[2] == '0' || serviceCode[2] == '6')
		{
			return 1;
		}
	}
	
	return 0;
}

int swipcard_process(){
	int ret;
	EmvOnlineData_t pOnlineData;
	memset(&pOnlineData,0x0,sizeof(EmvOnlineData_t));

	do
	{
		if(pin_require_check() == 1)
		{
			get_transaction_data()->emv_emter_online_pin_result =0xFF;
			GuiEventRegister(LCD_DISP_INPUT_ONLINE_PIN);
			ret = OsPedOpen();
			if(ret != RET_OK)
			{
				break;
			}

			ret = OsPedIncreaseKsnDukpt(PED_PIN_IPEK_INDEX);
			if(ret != RET_OK){
				OsPedClose();
				break;
			}
			ret = OsPedGetPinDukptEx(PED_PIN_IPEK_INDEX,get_transaction_data()->sCardNo, (char *)"4,5,6", 0x20, 60*1000,get_transaction_data()->sPinKsn, get_transaction_data()->sPin,InputPinCallback);
			OsLog(LOG_DEBUG,"--------online pin enter ret = %d ",ret);
			OsPedClose();
			if(ret != RET_OK)
				break;

			get_transaction_data()->emv_emter_online_pin_result = 1;
		}

		GuiEventRegister(LCD_DISP_GO_ONLINE);
		ret = sale_online_request(&pOnlineData);

	}while(0);

	return ret;
}
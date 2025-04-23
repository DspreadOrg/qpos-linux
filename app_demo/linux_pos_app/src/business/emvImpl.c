#include "appinc.h"

#define HIGH_NIBBLE(theByte)	((theByte>>4)&0x0F)	//high nibble
#define LOW_NIBBLE(theByte)	(theByte&0x0F)			//Low half byte
#define PR_ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))

#define EXP_DATE_LENGTH			4
#define SERVICE_CODE_LENGTH		3

typedef struct tagEmvTransData
{
	IccType Icc_Type;			/*card type*/
	PR_INT8 Pan[19+1];		//pan
	PR_INT8 Tracker2[137 + 1];
	PR_INT32 Tracker2Len; 
	PR_INT8 CardSN[3+1];
	PR_INT8 ExpDate[4+1];
	PR_INT8 CardCompany[3+1];	//credit card companies
	PR_INT8 ServiceCode[3+1];
	PR_INT8 TransAmount[12+1];
	PR_INT8 PinBlock[8+1];
	PR_INT32 PinLen;          //Enter the length of the pin
}EmvTransData;
static EmvTransData emvTransData;

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

char** ConvertAppList(AidCandidate_t *pList,int listNum){
	char** p = (char**)malloc(listNum * sizeof(char*));  
	int len = 0;
	if (p == NULL) {  
        return NULL;  
    } 
    for (int i = 0; i < listNum; i++) {  
		len = strlen((char *)pList[i]._lable)+1;
        p[i] = (char*)malloc(len+16); 
        if (p[i] == NULL) { 
            return NULL;  
        }
        memset(p[i],0x0,len);
		memcpy(p[i],pList[i]._lable,len-1);  
    }
	return p;
}

// Multi-application selection Return value >=0 Return the selected AID serial number -1 failed
int aidSelect(AidCandidate_t *pList, int listNum){
    PR_INT8** appList = NULL;
    PR_INT32 ret;
	PR_INT32 nIndexOfSelect;

    appList = ConvertAppList(pList,listNum);

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Select App");

    TransView_vClearPort();
    ret = TransView_nShowListViewEx((char*)"Select App",appList,listNum,60, &nIndexOfSelect);
    if(ret != PR_NORMAL){
		nIndexOfSelect = -1;
    }

	if(appList != NULL){
        Freelist(listNum,appList);
    }
    return nIndexOfSelect;
}
// Card number confirmation, return value 0 confirmation -1 failed
int confirmCardInfo(char *pan,int len){
    memcpy(emvTransData.Pan,pan,len);
    return 0;
}
// Request pin cType: 1 offline password 2 last offline password 3 online password Return value - 1 input failed - 2 BYPASS not entered - 3 aborted transaction and Timeout > 0 input password length
int inputPasswd(int cType, char *pszPin){
    PR_INT32 nRet;
    PR_INT32 pinLen;
    PR_UINT8 PinBlock[16+1] = {0};
    if(cType == EMV_ONLINEPIN_INPUT){
        nRet = TransView_nShowPinpadView(1,emvTransData.TransAmount,(PR_UINT8*)emvTransData.Pan,(char *)"0,6",0x00,60,PinBlock);
        TransView_vClearPort();
		if(nRet != RET_OK){
            return PR_FAILD;
        }
        OsPedPinKeyNotify(&pinLen,NULL);
		emvTransData.PinLen = pinLen;
        if(pinLen == 0){
            return -2;
        }
		memcpy(emvTransData.PinBlock,PinBlock,8);
        return pinLen;
    }else{
		pinLen = 0;
        nRet = TransView_nGetOfflinePin(emvTransData.TransAmount,cType,3,12,4,sizeof(PinBlock),(PR_INT8*)PinBlock);
        TransView_vClearPort();
		switch(nRet)
		{
			case PR_FAILD:
            case PR_TIMEOUT:
				return -1;
			case PR_CANCLE:
				return -3;
			case PR_BYPASS:
				return -2;
// Online pin status = PINPAD_OK Return to 0
			default:
				memcpy(pszPin,PinBlock,strlen((PR_INT8*)PinBlock));
                printf("offline pin  %s  \n",(PR_INT8*)PinBlock);
				return strlen((PR_INT8*)PinBlock);
		}
    }
}

// Identity confirmation type: value 0-5, namely: ID card/official certificate/passport/entry certificate/temporary ID card/other pcon: ID number; return value 1 ID card holder identity confirmation 0 ID confirmation failed
int certConfirm(unsigned char type, unsigned char *pcon, unsigned char len){
    return 1;
}

int emv_process_disp(EmvKernelDisp type)
{

	switch (type)
	{
	case EMV_DISP_READING_CARD:
		TransView_vClearPort();
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Do Not Pull Out The Card");
		break;
	case EMV_DISP_SEE_PHONE:
		TransView_vClearPort();
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Please check your phone");
		OsSleep(5000);
		break;
	case EMV_DISP_REMOVE_CARD:
		TransView_vClearPort();
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Please remove card");
		break;
	case EMV_DISP_NFC_RETAP:
		TransView_vClearPort();
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Please retap card");
		break;
	default:
		break;
	}
	return 0;
} 

PR_BOOL Emv_ParseTrack2(EmvTransData *pEmvTransData)
{
	PR_UINT32 i=0;
	PR_UINT32 cardNumLength = 0;

	if(pEmvTransData->Tracker2Len <= 0){
		return PR_FALSE;
	}
	for( ;i<pEmvTransData->Tracker2Len; i++)
	{
		if(CardDataIsNum(pEmvTransData->Tracker2[i])==PR_FALSE)
		{
// The card number starts after the actual character (1 byte) and ends with the delimiter.
			#define SIZE_OF_START_FLAG 0
			cardNumLength = i-SIZE_OF_START_FLAG;	
			if(strlen(pEmvTransData->Pan) <= 0){
				memcpy(pEmvTransData->Pan, pEmvTransData->Tracker2+SIZE_OF_START_FLAG, cardNumLength);
			}
			
			memcpy(pEmvTransData->ExpDate, pEmvTransData->Tracker2+i+1, EXP_DATE_LENGTH);
			memcpy(pEmvTransData->ServiceCode, pEmvTransData->Tracker2+i+1+EXP_DATE_LENGTH, SERVICE_CODE_LENGTH);
			break;
		}
	}

	return PR_TRUE;
}


PR_INT32 Emv_GetCardInfo(EmvTransData *pEmvTransData){
	PR_UINT8* value = PR_NULL;
	PR_INT32 length = 0;
	PR_INT32 rawLength = 0;
	PR_Bool havePanTag;
	PR_UINT8 buf[256];

	value = Emv_GetCoreData(EMVTAG_APP_PAN_SN, &length);
	if (value){
		sprintf((PR_INT8*)pEmvTransData->CardSN, "%03d", PR_Bcd2Dec(*value));
	}
// 2 track data
	value = Emv_GetCoreData(EMVTAG_TRACK2, &length);
	if (value){
// Determine if there is too much right supplement F
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
		strcpy((PR_INT8*)pEmvTransData->Tracker2, (PR_INT8*)buf);
// The ordinary magnetic stripe card is '='; the equivalent data bit of IC card is 'D', and it is uniformly converted to '=';
		if (offset > 0){
			pEmvTransData->Tracker2[offset] = '=';
		}
		pEmvTransData->Tracker2Len = rawLength;
// Obtain relevant data from equivalent 2 magnets.
		Emv_ParseTrack2(pEmvTransData);
		value = Emv_GetCoreData(EMVTAG_APP_EXPDATA, &length);
		if(value)
		{
			PR_INT8 acExpDate[EXP_DATE_LENGTH+1] = {0};
			PR_INT32 nAscLen = 0;
			nAscLen = PR_nUtilBcd2Asc(2,(PR_INT8*)value,EM_ALIGN_LEFT,sizeof(acExpDate),acExpDate);
			memcpy((PR_INT8*)pEmvTransData->ExpDate,acExpDate,nAscLen);
		}
	}
	if (PR_FALSE==GetIccCardCompany((PR_INT8*)pEmvTransData->CardCompany))
	{
		strcpy((PR_INT8*)pEmvTransData->CardCompany, "000");
	}
	return PR_NORMAL;
}

PR_INT32 Pack_Field55Data(IccType Icc_Type,PR_UINT8* pField55,PR_INT32 *pField55Len)
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
	if (Icc_Type == CONTACTLESS_ICC){
		PR_TLV_T tlvObjs[32];
		PR_INT32 tlvPos;
		PR_nTlvUtilParser((PR_INT8 *)buf, totalLength, tlvObjs, PR_ARRAY_SIZE(tlvObjs), PR_DECODE_LEVEL1);
		tlvPos = PR_nTlvUtilFetch(0, EMVTAG_CID, tlvObjs, PR_SEARCH_ONLY_SON);
// If the QPBOC group package does not have 9F27 in the 55 domain, it needs to be manually
		if(tlvPos <= 0){
			PR_UINT8 tempBuf[512];
			memcpy(tempBuf, buf, 8+3);
			tempBuf[11] = 0x9F;
			tempBuf[12] = 0x27;
			tempBuf[13] = 0x01;
			tempBuf[14] = 0x80;
			memcpy(tempBuf+15, buf+11, totalLength-11);
			memcpy(pField55,tempBuf,(totalLength+4));
			*pField55Len = (totalLength+4);
		}else{
			memcpy(pField55,buf,totalLength);
			*pField55Len = totalLength;
		}
	}else{
		memcpy(pField55,buf,totalLength);
		*pField55Len = totalLength;
	}
	return PR_NORMAL;
}

PR_INT32 Emv_8583_Pack(EmvTransData *pEmvTransData,PR_INT8 * szPackStr, PR_INT32 nPackSize, PR_INT32 *nRetLen)
{
	char szBuffer[512],szTerminalNo[9],szCustNo[16];
	uchar szBatch[7];
	int nField55Len = 0;
    int nLength = 0;
	char szGetBuffer[30];
    ISODataStru m_iso; 
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
// 0 Domain
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0200", 4));
// 2 domains
	if(strlen(pEmvTransData->Pan) > 0){
		ASSERT_NORMAL(PubSetBit_separate(&m_iso, PAN, (char*)pEmvTransData->Pan, strlen(pEmvTransData->Pan)));
	}
// 3 domains
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, PROC_CODE, (char*)"000000", 6));
// 4 domains
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, AMOUNT, pEmvTransData->TransAmount, 12));
// 11 domains
	BUFCLR(szBuffer);
	ASSERT_NORMAL(Business_GetTradeNum(szBuffer,sizeof(szBuffer)-1));
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACE_NO, szBuffer, 6));
// 14 domains
	if(strlen(pEmvTransData->ExpDate) > 0){
		ASSERT_NORMAL(PubSetBit_separate(&m_iso, EXPIRY, pEmvTransData->ExpDate, strlen(pEmvTransData->ExpDate)));
	}
// 22 domains
    if(pEmvTransData->PinLen > 0){
		if(pEmvTransData->Icc_Type == CONTACTLESS_ICC){
			ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"071", 3));
		}else if(pEmvTransData->Icc_Type == CONTACT_ICC){
			ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"051", 3));
		}
    }else{
		if(pEmvTransData->Icc_Type == CONTACTLESS_ICC){
        	ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"072", 3));
		}else if(pEmvTransData->Icc_Type == CONTACT_ICC){
			ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"052", 3));
		}
    }
// 23 domains
	if(strlen(pEmvTransData->CardSN) > 0){
		ASSERT_NORMAL(PubSetBit_separate(&m_iso, 23, pEmvTransData->CardSN, strlen(pEmvTransData->CardSN)));
	}
// 25 domains
	ASSERT_NORMAL(PubSetBit_separate( &m_iso, 25, (char*)"00", 2));
// 26 domains
    if(pEmvTransData->PinLen > 0){
	    ASSERT_NORMAL(PubSetBit_separate( &m_iso, 26, (char*)"12", 2));
    }
// 35 domains
    if(pEmvTransData->Tracker2Len > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACK2, (char*)pEmvTransData->Tracker2, pEmvTransData->Tracker2Len));
    }
// 41 terminal number
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
// 42 Merchant Number
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
// 49 domains
	ASSERT_NORMAL(PubSetBit_separate( &m_iso, MONEYID, (char*)"156", 3));

// 52 domains
    if(pEmvTransData->PinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, PIN, pEmvTransData->PinBlock, 8));
    }
// 53 domains
    if(pEmvTransData->PinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"2600000000000000", 16));
    }else{
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"0600000000000000", 16));
    }

// 55 domains
	BUFCLR(szBuffer);
	Pack_Field55Data(pEmvTransData->Icc_Type,(PR_UINT8*)szBuffer,&nField55Len);
	if(nField55Len > 0){
		ASSERT_NORMAL(PubSetBit_separate( &m_iso, 55, szBuffer, nField55Len));
	}
// 60.1 Transaction type 60.2 Transaction batch number 60.3 Network management transaction type 60.4 Terminal reading capability 60.5 IC card condition code based on PBOC debit/credit standard 60.6 Flags that support partial deduction and return balance
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf (szBuffer, "22%6.6s000601", szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, strlen(szBuffer)));

// 64 domain MAC
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MAC, (char*)"11111111", 8));

	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr_separate(szPackStr, &m_iso, &nLength));
	ASSERT_NORMAL(Business_CalcMac(1, (uchar*)szPackStr, nLength - 8, (uchar*)(szPackStr + nLength - 8)));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 data");
	return 0;
}

PR_INT32 emv_onlineData_proc (PR_INT8 * szReadBuf, PR_INT32 iReadLen,EmvOnlineData_t* pOnlineData)
{
	if (szReadBuf == NULL || iReadLen == 0)
	{
		return -1;
	}
	PR_INT8 szBuffer[512] = {0};
	PR_INT8 szErrCode[3] = {0};
    PR_UINT8 szMac[8+1] = {0};
	PR_INT8 szErrInfo[128] = {0};
	PR_INT32 iLen = 0;
	ISODataStru m_iso;

	ASSERT_NORMAL(PubDelAllBit(&m_iso));
	ASSERT_NORMAL(PubStrToIso_separate(szReadBuf + PACK_WHEAD_LEN + 2, iReadLen - PACK_WHEAD_LEN - 2, &m_iso, NULL));
// Judgment error code
	BUFCLR(szErrCode);
	iLen = sizeof(szErrCode);
	ASSERT_NORMAL(PubGetBit(&m_iso, RET_CODE, szErrCode, &iLen));
	if (iLen != 2)
	{   
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Unpacking Exception");
		return PR_FAILD;
	}
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code:%s",szErrCode);
	if (memcmp(szErrCode,"00", 2) == 0)
	{
// MAC verification
		if(Business_CalcMac(1, (uchar*)szReadBuf + PACK_RHEAD_LEN + 2, iReadLen - 8 - PACK_RHEAD_LEN - 2,szMac))
		{
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mac Ckeck Err1");
            return PR_FAILD;
		}
        if(memcmp(szMac,szReadBuf + iReadLen - 8,8) != 0){
            TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mac Ckeck Err2");
            return PR_FAILD;
        }

		BUFCLR(szBuffer);
		iLen = sizeof(szBuffer);
		ASSERT_NORMAL(PubGetBit(&m_iso, 55, szBuffer, &iLen));
		memcpy(pOnlineData->iccResponse,szErrCode,2);
        memcpy(pOnlineData->ackdata,szBuffer,iLen);
        pOnlineData->ackdatalen = iLen;
        return PR_NORMAL;
	}
	else
	{
		memcpy(pOnlineData->iccResponse,szErrCode,2);
		return PR_FAILD;
	}
}

// Online result: Online result-1 Online failed >=0 Online successful (with message response)
int onlineProcess(EmvOnlineData_t* pOnlineData){
	PR_INT32 iRet = 0;
	PR_INT32 iLen = 0;
	PR_INT32 iSendLen = 0;
	PR_INT32 iRecvLen = 0;
	PR_INT8 szBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szSendBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szRecvBuf[READBUFF_SIZE + 1] = {0};

    if(pOnlineData == NULL){
        return -1;
    }
	TransView_vClearPort();
	Emv_GetCardInfo(&emvTransData);
    BUFCLR(szBuf);
	iRet = Emv_8583_Pack(&emvTransData,szBuf, sizeof(szBuf) - 1, &iLen);
	if ( iRet ) {
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Packing Exception");
		goto FAIL;
	}
	BUFCLR(szSendBuf);
	iRet  = pub_pack_header(szBuf, iLen, szSendBuf, &iSendLen);
	if ( iRet ) {
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Packing Exception2");
		goto FAIL;
	}
#ifndef POS_OFFLINE
	BUFCLR(szRecvBuf);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Connect To Server");
	iRet = network_normal_commu(szSendBuf, iSendLen, szRecvBuf, &iRecvLen);
	if ( iRet ) {
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Communication Failed");
		goto FAIL;
	}
	iRet = emv_onlineData_proc(szRecvBuf, iRecvLen,pOnlineData);
	if ( iRet ) {
		goto FAIL;
	}
	return PR_NORMAL;
#else

#ifdef POS_PAPER_TRADING
    if(Is_Valid_Network() == PR_NORMAL){
		iRet = ppp_connect_test();
        if(iRet == 0){
            memcpy(pOnlineData->iccResponse,"00",2);
			memcpy(pOnlineData->ackdata,"\x9F""\x36""\x02""\x02""\x5A",5);
			pOnlineData->ackdatalen = 5;
			return PR_NORMAL;
        }else{
			return PR_FAILD;
		}
	}else{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Network Not Connected");
		return PR_FAILD;
    }
#else
   
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Connect To Server");
    memcpy(pOnlineData->iccResponse,"00",2);
    memcpy(pOnlineData->ackdata,"\x9F""\x36""\x02""\x02""\x5A",5);
    pOnlineData->ackdatalen = 5;
    return PR_NORMAL;
#endif

#endif

FAIL:
    return PR_FAILD;
    
}

static void Initialize_EMV_TermConfig(EmvTermConfig_t *ptermconfig)
{
	int nStatus = 0;

	ptermconfig->_type = 0x22;
	memcpy(ptermconfig->_cap,"\x60""\xE9""\xC8", 3);
	memcpy(ptermconfig->_add_cap,"\xFF""\x80""\xF0""\xF0""\x01", 5);
	memcpy(ptermconfig->_ics,"\xF4""\xF0""\xF0""\xF8""\xAF""\xFE""\x80""\x00", 8);
	memcpy(ptermconfig->_ifd_serial_num,"8320ICC", 8);
// ptermconfig->_status = EMV_L2_PBOC2_ENB;
// ptermconfig->_status = nStatus==1?EMV_L2_BCTC_TEST_ENB:nStatus==2?EMV_L2_UICS_ENB:nStatus==3?EMV_L2_MASTERCARD_ENB:EMV_L2_BCTC_TEST_ENB;
	memcpy(ptermconfig->_term_country_code,"\x01""\x56", 2);
	memcpy(ptermconfig->_terminal_id,"12345678", 8);
	memcpy(ptermconfig->_trans_curr_code,"\x01""\x56", 2);
	ptermconfig->_trans_curr_exp = 0x00;
	memcpy(ptermconfig->_merchant_name,"test", 4);
}

static void Initialize_EMV_CallBackFun(EmvCallBack_t *pcallbackfun)
{
	pcallbackfun->EMV_AidSelect = aidSelect;
	pcallbackfun->EMV_ConfirmCardInfo = confirmCardInfo;
	pcallbackfun->EMV_InputPasswd = inputPasswd;
	pcallbackfun->EMV_CertConfirm = certConfirm;
	pcallbackfun->EMV_ProcessDisp = emv_process_disp;
	pcallbackfun->EMV_OnlineProcess = onlineProcess;
}

PR_INT32 EmvL2_Init(){
    EmvTermConfig_t termconfig;
    EmvCallBack_t t_callbackfun;

    Initialize_EMV_CallBackFun(&t_callbackfun);
    Initialize_EMV_TermConfig(&termconfig);
    if(Emv_KernelInit(termconfig,t_callbackfun) < 0){
        printf("emv init failed\n");
        return PR_FAILD;
    }
	dbg_printf("IC Param Count： %d",Emv_GetAidTotalNum());
	dbg_printf("IC Capk Count： %d",Emv_GetCapkTotalNum());

    return PR_NORMAL;
}

PR_INT32 EmvL2_Proc(EmvTransParams_t emvTransParams){
    EMV_L2_Return nEmvRet = APP_RC_START;

    memset(&emvTransData,0x0,sizeof(EmvTransData));
    memcpy(emvTransData.TransAmount,emvTransParams.trans_amount,strlen((char*)emvTransParams.trans_amount));
	emvTransData.Icc_Type = emvTransParams.icc_type;
    nEmvRet = Emv_Process(emvTransParams);
    if(nEmvRet == APP_RC_COMPLETED){
        return PR_NORMAL;
    }
    return PR_FAILD;
}

void EmvL2_SetDefaultAidAndCapk(){
    PR_UINT8 temp[1024];
    PR_INT8 aid1[] = "9F0610000000000000000000000000000000005F2A0203565F3601029F01060012345678909F150212349F160F4243544553543132333435363738399F1A0203569F1C084E4C2D47503733309F1E0838333230314943439F3303E0F8C89F3501229F3901059F40057000B0A0019F4E04616263649F5301529F811701019F811801009F814301019F814501019F814701009F830301017F154C7F16239F821801069F8208060000000007009F8209060000000002009F820A060000000004007F16239F8218010B9F8208060000000003009F8209060000000002009F820A06000000000100";
    PR_INT8 aid2[] = "9F0607A0000000031010DF0101009F0902008C9F1B04000000009F1E0838333230314943439F6604B60040009F8125039F37049F812701009F812801009F812904000000009F812A05D84000A8009F812B0500108000009F812C05D84004F8009F92810004050100009F92810101009F92810A02FE009F92810D069999999999999F92810E060000002000019F92810F06000000000000";
    PR_INT8 aid3[] = "9F0607A00000000410105F2A020356DF0101009F090200029F1A0203569F1B04000000009F1D086C000000000000009F1E0838333230314943439F3501229F5C087A45123EE59C7F409F6D0200019F7E009F8125039F37049F812701009F812801009F812904000000009F812A05FE50BCA0009F812B0500008000009F812C05FE50BCF800DF6000DF6200DF6300DF810800DF810900DF810A00DF810C0102DF810D0102DF81170120DF81180160DF81190108DF811A039F6A04DF811B01A0DF811C0100DF811D0100DF811E0110DF811F0108DF812005F45084800CDF8121050000800000DF812205F45084800CDF812306000000000000DF812406999999999999DF812506999999999999DF812606000000200000DF812C01007F11119C0120DF812105FFFFFFFFFFDF83020101";
    PR_INT32 aidCount = Emv_GetAidTotalNum();
    OsLog(LOG_ERROR,"aidCount = %d \n",aidCount);
    int capkCount = Emv_GetCapkTotalNum();
    OsLog(LOG_ERROR,"capkCount = %d \n",capkCount);
    if(aidCount <= 0){
        memset(temp,0x0,sizeof(temp));
        nAsc2Bcd(aid1, strlen((char *)aid1), temp, 0);
        if(Emv_PARAM_InputAIDData(temp,strlen((char *)aid1)/2) <0){
            OsLog(LOG_ERROR,"Set AID1 Failed \n");
            return;
        }

        memset(temp,0x0,sizeof(temp));
        nAsc2Bcd(aid2, strlen((char *)aid2), temp, 0);
        if(Emv_PARAM_InputAIDData(temp,strlen((char *)aid2)/2) <0){
            OsLog(LOG_ERROR,"Set AID2 Failed \n");
            return;
        }

        memset(temp,0x0,sizeof(temp));
        nAsc2Bcd(aid3, strlen((char *)aid3), temp, 0);
        if(Emv_PARAM_InputAIDData(temp,strlen((char *)aid3)/2) <0){
            OsLog(LOG_ERROR,"Set AID3 Failed \n");
            return;
        }
    }
    if(capkCount <= 0){
        PR_INT8 capk1[] = "9F0605A0000000039F220109DF0281F89D912248DE0A4E39C1A7DDE3F6D2588992C1A4095AFBD1824D1BA74847F2BC4926D2EFD904B4B54954CD189A54C5D1179654F8F9B0D2AB5F0357EB642FEDA95D3912C6576945FAB897E7062CAA44A4AA06B8FE6E3DBA18AF6AE3738E30429EE9BE03427C9D64F695FA8CAB4BFE376853EA34AD1D76BFCAD15908C077FFE6DC5521ECEF5D278A96E26F57359FFAEDA19434B937F1AD999DC5C41EB11935B44C18100E857F431A4A5A6BB65114F174C2D7B59FDF237D6BB1DD0916E644D709DED56481477C75D95CDD68254615F7740EC07F330AC5D67BCD75BF23D28A140826C026DBDE971A37CD3EF9B8DF644AC385010501EFC6509D7A41DF03141FF80A40173F52D7D27E0F26A146A1C8CCB29046DF0403000003DF050420311222DF060101DF07010100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        memset(temp,0x0,sizeof(temp));
        nAsc2Bcd(capk1, strlen((char *)capk1), temp, 0);
        if(Emv_PARAM_InputCAPKData(temp,strlen((char *)capk1)/2) <0){
            OsLog(LOG_ERROR,"Set CAPK1 Failed \n");
            return;
        }
    }
	EmvL2_Init();
}


/*
 * IC card parameters download
 */
typedef	struct
{
    PR_UINT8 Data[32];
    PR_UINT32 DataLength;
}QueryAidInfo;

typedef struct
{
	PR_UINT32 nCount;		//The number of parameter information received in the status report
	QueryAidInfo queryAidInfo[32];
}AidInfoList;

static AidInfoList aidInfoList;
int set_AidInfoList(unsigned char *szBuffer, int cLen)
{
	unsigned char *pTLVStart = NULL;
	int nOffset = 0, nRet;
    int nTempOffset,aidLen;
	do
	{   
        pTLVStart = szBuffer + nOffset;
        nTempOffset = 0;
		if (!memcmp (pTLVStart, "\x9F\x06", 2))	// AID
		{
			nTempOffset += 2;
			aidLen = *(pTLVStart + nTempOffset);
			nTempOffset += 1;
            nTempOffset += aidLen;
			memcpy (aidInfoList.queryAidInfo[aidInfoList.nCount].Data, pTLVStart, nTempOffset);
            aidInfoList.queryAidInfo[aidInfoList.nCount].DataLength = nTempOffset;
            aidInfoList.nCount++;
			nOffset += nTempOffset;
		}
		else
		{
			return -1;
		}
	}while (nOffset < cLen);

	return 0;
}

PR_INT32 query_emvpara_unionpay_pack_8583(PR_INT8 * szPackStr, PR_INT32 nPackSize, PR_INT32 *nRetLen)
{
	ISODataStru m_iso;  
	PR_INT32 nLength;
	PR_INT8 szBuffer[100],szTerminalNo[9],szCustNo[16];
	PR_UINT8 szBatch[7];
	PR_UINT32 nLen;

	if (szPackStr == NULL || nRetLen == NULL)
	{
		return -1;
	}
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
// 0 Domain 34
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0820", 4));
// 41 terminal number
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
// 42 Merchant Number
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
// 60.1 Transaction type 60.2 Transaction batch number 60.3 Network management transaction type
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s382",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
// 62 domains
	BUFCLR(szBuffer);
	szBuffer[0] = '1';
	sprintf(szBuffer + 1, "%02d", aidInfoList.nCount);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 62, szBuffer,3));
	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr_separate(szPackStr, &m_iso, &nLength));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 data");
	return 0;
}

PR_INT32 PubQueryEMVPara()
{
	PR_UINT8 szRetBuf[512];
	PR_INT32 nLen, i, nRet;
	PR_INT8 szTerminalNo[9],szCustNo[16];
	PR_INT8 szReadBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szSendBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szBuffer[READBUFF_SIZE + 1] = {0};
	PR_INT32 nSendLen;
	PR_INT32 nReadLen;
	ISODataStru m_iso;  

    memset(&aidInfoList,0x0,sizeof(AidInfoList));
// Obtain the merchant number terminal number for message verification
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"IC Card Params Download");
    i = 1;
	while (1)
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Query %02d", i);
		BUFCLR(szBuffer);
		nRet = query_emvpara_unionpay_pack_8583(szBuffer, sizeof(szBuffer) - 1, &nLen);
		if (nRet)
		{
			return -1;
		}
		BUFCLR(szSendBuf);
		nRet = pub_pack_header(szBuffer, nLen, szSendBuf,&nSendLen);
		if (nRet)
		{
			return -1;
		}
		BUFCLR(szReadBuf);
		nRet = network_normal_commu(szSendBuf, nSendLen, szReadBuf, &nReadLen);
		if (nRet)
		{
			return -1;
		}
		PubDelAllBit(&m_iso);
		if (PubStrToIso_separate(szReadBuf + PACK_RHEAD_LEN + 2, nReadLen - PACK_RHEAD_LEN - 2, &m_iso, NULL) != NORMAL)
		{
			return -1;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof (szRetBuf);
		PubGetBit(&m_iso, 39, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, "00", 2))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code:[%s]",szRetBuf);
			return -1;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 41, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szTerminalNo, 8))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mch No Wrong");
			return -1;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 42, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szCustNo, 15))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Term No Wrong");
			return -1;
		}
		nLen = sizeof(szRetBuf);
		BUFCLR(szRetBuf);
		PubGetBit(&m_iso, 62, (char *)szRetBuf, &nLen);
		if (nLen < 1)
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Field 62 Wrong[1]");
			return -1;
		}
// 30 There is no parameter information in the future 31 There is no parameter information in the future
		if (szRetBuf[0] == 0x30)
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"No Need To Download");
			return 0;
		}
		nRet = set_AidInfoList(szRetBuf + 1, nLen - 1);
		if (nRet)
		{
			return -1;
		}
		if (szRetBuf[0] == 0x32)
		{
			dbg_printf("Need Download");
			i++;
			continue;
		}
		else if (szRetBuf[0] == 0x31 || szRetBuf[0] == 0x33)
		{
			break;
		}
	}
	return 1;
}

static int download_emvpara_unionpay_pack_8583(uchar *szAID, int nAIDLen, char * szPackStr, int nPackSize, int *nRetLen)
{
	ISODataStru m_iso;  
	int nLength;
	char szBuffer[100],szTerminalNo[9],szCustNo[16];
	uchar szBatch[7];
	uint nLen;
	if (szPackStr == NULL || nRetLen == NULL || szAID == NULL)
	{
		return -1;
	}
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
// 0 Domain
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0800", 4));
// 41 terminal number
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
// 42 Merchant Number
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
// 60.1 Transaction type 60.2 Transaction batch number 60.3 Network management transaction type
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s380",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
// 62 domains
	BUFCLR(szBuffer);
	memcpy(szBuffer, szAID, nAIDLen);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 62, (char *)szBuffer, nAIDLen));
	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr_separate(szPackStr, &m_iso, &nLength));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 Data");
	return 0;
}

PR_INT32 PubDownREICPara()
{
	unsigned char i,  szGetBuf[16], szRetBuf[512], szAID[16], cTmp;
	unsigned char *pcConfig = NULL;
	int nLen, nLen62, nOffset, nRet;
	unsigned int nNum,nTmpLen;
	long lTmp;
	char szTerminalNo[9],szCustNo[16];
	char szReadBuf[READBUFF_SIZE + 1] = {0};
	char szSendBuf[READBUFF_SIZE + 1] = {0};
	char szBuffer[READBUFF_SIZE + 1] = {0};
	int nSendLen;
	int nReadLen;
	ISODataStru m_iso;  

	nNum = aidInfoList.nCount;
	if (nNum <= 0)
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"No Need To Download");
		return NORMAL;
	}
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
    Emv_ClrAIDFile();
	i = 0;
	while (i < nNum)
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Downloading: %02d", i+1);
		BUFCLR(szBuffer);
		nRet = download_emvpara_unionpay_pack_8583(aidInfoList.queryAidInfo[i].Data, aidInfoList.queryAidInfo[i].DataLength, szBuffer, sizeof(szBuffer) - 1, &nLen);
		if (nRet)
		{
			return EXCEPTION;
		}
		BUFCLR(szSendBuf);
		nRet = pub_pack_header(szBuffer, nLen, szSendBuf,&nSendLen);
		if (nRet)
		{
			return EXCEPTION;
		}
		BUFCLR(szReadBuf);
		nRet = network_normal_commu(szSendBuf, nSendLen, szReadBuf, &nReadLen);
		if (nRet)
		{
			return EXCEPTION;
		}
		PubDelAllBit(&m_iso);
		if (PubStrToIso_separate(szReadBuf + PACK_RHEAD_LEN + 2, nReadLen - PACK_RHEAD_LEN - 2, &m_iso, NULL) != NORMAL)
		{
			return EXCEPTION;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof (szRetBuf);
		PubGetBit(&m_iso, 39, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, "00", 2))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code Err[%s]",szRetBuf);
			return EXCEPTION;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 41, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szTerminalNo, 8))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Term No Wrong");
			return EXCEPTION;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 42, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szCustNo, 15))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mch No Wrong");
			return EXCEPTION;
		}
		nLen62 = sizeof(szRetBuf);
		BUFCLR(szRetBuf);
		PubGetBit(&m_iso, 62, (char *)szRetBuf, &nLen62);
		if (nLen62 <= 0)
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Field 62 Wrong");
			return EXCEPTION;
		}
		if (szRetBuf[0] == 0x30)
		{
			i++;
			continue;
		}
		else if (szRetBuf[0] == 0x31)
		{
			pcConfig = szRetBuf + 1;
			dbg_printfWHex((szRetBuf + 1), (nLen62 - 1), "IC Card Params");
			if(Emv_PARAM_InputAIDData(pcConfig,(nLen62 - 1)) < 0){
                TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"AID Setting Err");
                return EXCEPTION;
            }
			i++;
			continue;
		}
	}
	return NORMAL;
}


int download_over_emvpara_unionpay_pack_8583(uchar type, char * szPackStr, int nPackSize, int *nRetLen)
{
	ISODataStru m_iso;  
	int nLength;
	char szBuffer[100],szTerminalNo[9],szCustNo[16];
	uchar szBatch[7];
	uint nLen;
	if (szPackStr == NULL || nRetLen == NULL)
	{
		return -1;
	}
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
// 0 Domain
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0800", 4));
// 41 terminal number
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
// 42 Merchant Number
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
// 60.1 Transaction type 60.2 Transaction batch number 60.3 Network management transaction type
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	switch(type)
	{
	case 1://Public key download completed
		{
			sprintf(szBuffer, "00%6.6s371", szBatch);
		}
		break;
	case 2://Parameter End
		{
			sprintf(szBuffer, "00%6.6s381", szBatch);
		}
		break;
	default:
		return EXCEPTION;
	}
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));

	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr_separate( szPackStr, &m_iso, &nLength));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 Data");
	return 0;
}

/*
 * IC card public key download
 */
typedef	struct
{
    PR_UINT8 Data[32];
    PR_UINT32 DataLength;
}QueryCapkInfo;

typedef struct
{
	PR_UINT32 nCount;		//The number of parameter information received in the status report
	QueryCapkInfo queryCapkInfo[64];
}CapkInfoList;

static CapkInfoList capkInfoList;
int set_CapkInfoList(unsigned char *szBuffer, int cLen)
{
	unsigned char *pTLVStart = NULL;
	int nOffset = 0, nRet;
	do
	{   
        pTLVStart = szBuffer + nOffset;
		if (!memcmp (pTLVStart, "\x9F\x06", 2))	// AID
		{
			memcpy (capkInfoList.queryCapkInfo[capkInfoList.nCount].Data, pTLVStart, 23);
            capkInfoList.queryCapkInfo[capkInfoList.nCount].DataLength = 23;
            dbg_printf("Capk Count  %d ",capkInfoList.nCount+1);
            dbg_printfWHex((uchar *)capkInfoList.queryCapkInfo[capkInfoList.nCount].Data, 23, "Query Capk Data");
            capkInfoList.nCount++;
			nOffset += 23;
           
		}
		else
		{
			return -1;
		}
	}while (nOffset < cLen);

	return 0;
}
int query_pubkey_unionpay_pack_8583(char * szPackStr, int nPackSize, int *nRetLen)
{
	ISODataStru m_iso;  
	int nLength;
	char szBuffer[100],szTerminalNo[9],szCustNo[16];
	uchar szBatch[7];
	uint nLen;
	int canum = 0;
	if (szPackStr == NULL || nRetLen == NULL)
	{
		return -1;
	}
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
// 0 Domain
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0820", 4));
// 41 terminal number
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
// 42 Merchant Number
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
// 60.1 Transaction type 60.2 Transaction batch number 60.3 Network management transaction type
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s372",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
// 62 domains
	BUFCLR(szBuffer);
	szBuffer[0] = '1';
	sprintf(szBuffer + 1, "%02d", capkInfoList.nCount);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 62, szBuffer,3));
	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr_separate(szPackStr, &m_iso, &nLength));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 Data");
	return 0;
}

int PubQueryPubilcKey()
{
	char szRetBuf[512];
	int nLen, nRet, i;
	char szTerminalNo[9],szCustNo[16];
	char szReadBuf[READBUFF_SIZE + 1] = {0};
	char szSendBuf[READBUFF_SIZE + 1] = {0};
	char szBuffer[READBUFF_SIZE + 1] = {0};
	int nSendLen;
	int nReadLen;
	ISODataStru m_iso;  
	
// Obtain the merchant number terminal number for message verification
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
    memset(&capkInfoList,0x0,sizeof(CapkInfoList));

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"IC Card Capk Download");
    i = 1;
	while (1)
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Query %02d", i);
		BUFCLR(szBuffer);
		nRet = query_pubkey_unionpay_pack_8583(szBuffer, sizeof(szBuffer) - 1, &nLen);
		if (nRet)
		{
			return -1;
		}
		BUFCLR(szSendBuf);
		nRet = pub_pack_header(szBuffer, nLen, szSendBuf,&nSendLen);
		if (nRet)
		{
			return -1;
		}
		BUFCLR(szReadBuf);
		nRet = network_normal_commu(szSendBuf, nSendLen, szReadBuf, &nReadLen);
		if (nRet)
		{
			return -1;
		}
		PubDelAllBit(&m_iso);
		if (PubStrToIso_separate(szReadBuf + PACK_RHEAD_LEN + 2, nReadLen - PACK_RHEAD_LEN - 2, &m_iso, NULL) != NORMAL)
		{
			return -1;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof (szRetBuf);
		PubGetBit(&m_iso, 39, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, "00", 2))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code Err[%s]",szRetBuf);
			return -1;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 41, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szTerminalNo, 8))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Term No Wrong");
			return -1;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 42, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szCustNo, 15))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mch No Wrong");
			return -1;
		}

		nLen = sizeof (szRetBuf);
		BUFCLR (szRetBuf);
		nRet = PubGetBit (&m_iso, 62, szRetBuf, &nLen);
		if (nRet)
		{
			return -1;
		}
		if (nLen < 1)
			return -1;

// 30 There is no public key information in the future 31 There is no public key information in the future
		if (szRetBuf[0] == 0x30)
		{
			return (0);
		}
		if (szRetBuf[0] == 0x31 || szRetBuf[0] == 0x33)
		{
			set_CapkInfoList ((uchar *)(szRetBuf + 1), nLen - 1);
			return (1);
		}
		if ((nLen - 1) / 23 >= 1)
		{
			set_CapkInfoList ((uchar *)(szRetBuf + 1), nLen - 1);
			i++;
		}
		else
			return (1);

	}
	return (1);
}

int download_pubkey_unionpay_pack_8583(uchar *cRid, char * szPackStr, int nPackSize, int *nRetLen)
{
	ISODataStru m_iso;  
	char szBuffer[100],szTerminalNo[9],szCustNo[16];
	uchar szBatch[7];
	uint nLen;
	unsigned char szField62[30];
	int nLenField62,nLength;
	if (szPackStr == NULL || nRetLen == NULL || cRid == NULL)
	{
		return -1;
	}
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
// 0 Domain
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0800", 4));
// 41 terminal number
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
// 42 Merchant Number
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
// 60.1 Transaction type 60.2 Transaction batch number 60.3 Network management transaction type
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s370",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
// 62 domains
	BUFCLR (szField62);
	memcpy(szField62,cRid,12);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 62, (char *)szField62, 12));
	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr_separate( szPackStr, &m_iso, &nLength));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 Data");
	return 0;
}

int EMV_DownCAKey()
{
	int nRet, i;
	unsigned int nNum;
	unsigned char szRetBuf[512];
	int nLen;
	char szTerminalNo[9],szCustNo[16];
	char szReadBuf[READBUFF_SIZE + 1] = {0};
	char szSendBuf[READBUFF_SIZE + 1] = {0};
	char szBuffer[READBUFF_SIZE + 1] = {0};
	int nSendLen;
	int nReadLen;
	ISODataStru m_iso;  

// Obtain the merchant number terminal number for message verification
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	nNum = capkInfoList.nCount;
	dbg_printf("need download count：%d",nNum);
	i = 1;
	while (i < nNum)
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Downloading:%02d", i);
		BUFCLR(szBuffer);

        dbg_printfWHex((uchar *)capkInfoList.queryCapkInfo[i].Data, 23, "Capk Data");
		nRet = download_pubkey_unionpay_pack_8583(capkInfoList.queryCapkInfo[i].Data, szBuffer, sizeof(szBuffer) - 1, &nLen);
		if (nRet)
		{
			return EXCEPTION;
		}
		BUFCLR(szSendBuf);
		nRet = pub_pack_header(szBuffer, nLen, szSendBuf,&nSendLen);
		if (nRet)
		{
			return EXCEPTION;
		}
		BUFCLR(szReadBuf);
		nRet = network_normal_commu(szSendBuf, nSendLen, szReadBuf, &nReadLen);
		if (nRet)
		{
			return EXCEPTION;
		}
		PubDelAllBit(&m_iso);
		if (PubStrToIso_separate(szReadBuf + PACK_RHEAD_LEN + 2, nReadLen - PACK_RHEAD_LEN - 2, &m_iso, NULL) != NORMAL)
		{
			return EXCEPTION;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof (szRetBuf);
		PubGetBit(&m_iso, 39, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, "00", 2))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code Err[%s]",szRetBuf);
			return EXCEPTION;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 41, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szTerminalNo, 8))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Term No Wrong");
			return EXCEPTION;
		}
		BUFCLR(szRetBuf);
		nLen = sizeof(szRetBuf);
		PubGetBit(&m_iso, 42, (char *)szRetBuf, &nLen);
		if (memcmp(szRetBuf, szCustNo, 15))
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mch No Wrong");
			return EXCEPTION;
		}
		nLen = sizeof(szRetBuf);
		BUFCLR (szRetBuf);
		PubGetBit(&m_iso, 62, (char *)szRetBuf, &nLen);
		if (nLen < 1)
		{
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Field 62 Wrong");
			return EXCEPTION;
		}
// 30 There is no public key information in the future 31 There is no public key information in the future
		if (szRetBuf[0] == 0x30)
		{
			return EXCEPTION;
		}
		else if (szRetBuf[0] == 0x31)	// Processing 62 domain information
		{
// Obtain the length of the public key touch, the length of the public key touch, the length of the public key index, the public key index, and the expiration period from the 62 domain.
            Emv_PARAM_InputCAPKData(szRetBuf + 1,nLen - 1);
            i++;
            continue;
		}
		else
		{
			return EXCEPTION;
		}
	}
	return NORMAL;
}

/*
 * Download ends
 */
int Pub_DownEnd(unsigned char cRet)
{
	char szRetBuf[30];
	int nLen,nRet;
	char szTerminalNo[9],szCustNo[16];
	char szReadBuf[READBUFF_SIZE + 1] = {0};
	char szSendBuf[READBUFF_SIZE + 1] = {0};
	char szBuffer[READBUFF_SIZE + 1] = {0};
	int nSendLen;
	int nReadLen;
	ISODataStru m_iso;  

// Obtain the merchant number terminal number for message verification
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);

	BUFCLR(szBuffer);
	nRet = download_over_emvpara_unionpay_pack_8583(cRet, szBuffer, sizeof(szBuffer) -1, &nLen);
	if (nRet)
	{
		return EXCEPTION;
	}
	BUFCLR(szSendBuf);
	nRet = pub_pack_header(szBuffer, nLen, szSendBuf,&nSendLen);
	if (nRet)
	{
		return EXCEPTION;
	}
	BUFCLR(szReadBuf);
	nRet = network_normal_commu(szSendBuf, nSendLen, szReadBuf, &nReadLen);
	if (nRet)
	{
		return EXCEPTION;
	}
	PubDelAllBit(&m_iso);
	if (PubStrToIso_separate(szReadBuf + PACK_RHEAD_LEN + 2, nReadLen - PACK_RHEAD_LEN - 2, &m_iso, NULL) != NORMAL)
	{
		return EXCEPTION;
	}
	BUFCLR(szRetBuf);
	nLen = sizeof (szRetBuf);
	PubGetBit(&m_iso, 39, (char *)szRetBuf, &nLen);
	if (memcmp(szRetBuf, "00", 2))
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code Err[%s]",szRetBuf);
		return EXCEPTION;
	}
	BUFCLR(szRetBuf);
	nLen = sizeof(szRetBuf);
	PubGetBit(&m_iso, 41, (char *)szRetBuf, &nLen);
	if (memcmp(szRetBuf, szTerminalNo, 8))
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Term No Wrong");
		return EXCEPTION;
	}
	BUFCLR(szRetBuf);
	nLen = sizeof(szRetBuf);
	PubGetBit(&m_iso, 42, (char *)szRetBuf, &nLen);
	if (memcmp(szRetBuf, szCustNo, 15))
	{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mch No Wrong");
		return EXCEPTION;
	}

    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Download Success");
	return 0;
}

/*
 * IC card parameters and public key download external call interface
 */
PR_INT32  PubDownIcParams()
{
	int  nRet = 0;
// Query
	nRet = PubQueryEMVPara();
// No download parameters required
	if (nRet == 0)
	{   
		return 0;
	}
// Requires download parameters
	if (nRet != 1)
	{
		goto FAIL;
	}
	nRet = PubDownREICPara();
	if(nRet)
	{
		goto FAIL;
	}
// Upload download parameters to end transaction
	if (Pub_DownEnd(2) != 0)
	{
		goto FAIL;
	}
	EmvL2_Init();
	return 0;
FAIL:
	return EXCEPTION;
}

PR_INT32  PubDownPubilcKey ()
{
	int nRet;
	nRet = PubQueryPubilcKey();
	if (nRet == -1)
	{
		goto FAIL;
	}
	if (nRet == 1)
	{
		nRet = EMV_DownCAKey();
		if (nRet)
		{
			goto FAIL;
		}
		if (Pub_DownEnd( 1) != 0)
		{
			goto FAIL;
		}
	}
	EmvL2_Init();
	return NORMAL;
FAIL:
	return EXCEPTION;
}
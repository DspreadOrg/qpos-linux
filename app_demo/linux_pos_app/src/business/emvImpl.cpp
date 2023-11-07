#include "appinc.h"

#define HIGH_NIBBLE(theByte)	((theByte>>4)&0x0F)	//高半字节
#define LOW_NIBBLE(theByte)	(theByte&0x0F)			//低半字节
#define PR_ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))

#define EXP_DATE_LENGTH			4
#define SERVICE_CODE_LENGTH		3

typedef struct tagEmvTransData
{
	IccType Icc_Type;			/*刷卡类型*/
	PR_INT8 Pan[19+1];		//主账号
	PR_INT8 Tracker2[137 + 1];
	PR_INT32 Tracker2Len; 
	PR_INT8 CardSN[3+1];
	PR_INT8 ExpDate[4+1];
	PR_INT8 CardCompany[3+1];	//信用卡公司
	PR_INT8 ServiceCode[3+1];
	PR_INT8 TransAmount[12+1];
	PR_INT8 PinBlock[8+1];
	PR_INT32 PinLen;          //输入pin的长度
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

//多应用选择 返回值>=0 返回选择的AID序列号 -1失败
int aidSelect(AidCandidate_t *pList, int listNum){
    return 0;
}
//卡号确认 返回值0确认 -1失败
int confirmCardInfo(char *pan,int len){
    memcpy(emvTransData.Pan,pan,len);
    return 0;
}
//请求pin cType：1脱机密码 2最后一次脱机密码 3联机密码  返回值-1输入失败  -2未输入BYPASS  -3中止交易和Timeout  >0输入密码长度
int inputPasswd(int cType, char *pszPin){
    PR_INT32 nRet;
    PR_INT32 pinLen;
    PR_UINT8 PinBlock[16+1] = {0};
    if(cType == EMV_ONLINEPIN_INPUT){
        nRet = TransView_nShowPinpadView(1,emvTransData.TransAmount,(PR_UINT8*)emvTransData.Pan,(char *)"0,6",0x00,30,PinBlock);
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
			//联机pin状态 = PINPAD_OK 返回0
			default:
				memcpy(pszPin,PinBlock,strlen((PR_INT8*)PinBlock));
                printf("脱机pin明文  %s  \n",(PR_INT8*)PinBlock);
				return strlen((PR_INT8*)PinBlock);
		}
    }
}

//身份确认 type 取值0-5,分别为:身份证/军官证/护照/入境证/临时身份证/其他 pcon：证件号；返回值1	执卡人身份确认  0身份确认失败
int certConfirm(unsigned char type, unsigned char *pcon, unsigned char len){
    return 1;
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
			// 卡号由其实字符（1字节）之后开始，到分隔符结束
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
	//2磁道数据
	value = Emv_GetCoreData(EMVTAG_TRACK2, &length);
	if (value){
		//判断是否多了右补F
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
		// 普通磁条卡为'='；IC卡等效数据位 'D'，统一转成 '=';
		if (offset > 0){
			pEmvTransData->Tracker2[offset] = '=';
		}
		pEmvTransData->Tracker2Len = rawLength;
		// 从等效2磁获取相关的数据。
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
		//QPBOC 组包若55域没有9F27 则需要手动
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
	//0域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0200", 4));
	//2域
	if(strlen(pEmvTransData->Pan) > 0){
		ASSERT_NORMAL(PubSetBit_separate(&m_iso, PAN, (char*)pEmvTransData->Pan, strlen(pEmvTransData->Pan)));
	}
	//3域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, PROC_CODE, (char*)"000000", 6));
    //4域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, AMOUNT, pEmvTransData->TransAmount, 12));
	//11域
	BUFCLR(szBuffer);
	ASSERT_NORMAL(Business_GetTradeNum(szBuffer,sizeof(szBuffer)-1));
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACE_NO, szBuffer, 6));
	//14域
	if(strlen(pEmvTransData->ExpDate) > 0){
		ASSERT_NORMAL(PubSetBit_separate(&m_iso, EXPIRY, pEmvTransData->ExpDate, strlen(pEmvTransData->ExpDate)));
	}
    //22域
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
	//23域
	if(strlen(pEmvTransData->CardSN) > 0){
		ASSERT_NORMAL(PubSetBit_separate(&m_iso, 23, pEmvTransData->CardSN, strlen(pEmvTransData->CardSN)));
	}
	//25域
	ASSERT_NORMAL(PubSetBit_separate( &m_iso, 25, (char*)"00", 2));
    //26域
    if(pEmvTransData->PinLen > 0){
	    ASSERT_NORMAL(PubSetBit_separate( &m_iso, 26, (char*)"12", 2));
    }
    //35域
    if(pEmvTransData->Tracker2Len > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACK2, (char*)pEmvTransData->Tracker2, pEmvTransData->Tracker2Len));
    }
	//41终端号
	BUFCLR(szTerminalNo);
    memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
	//42商户号
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
	//49域
	ASSERT_NORMAL(PubSetBit_separate( &m_iso, MONEYID, (char*)"156", 3));

    //52域
    if(pEmvTransData->PinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, PIN, pEmvTransData->PinBlock, 8));
    }
    //53域
    if(pEmvTransData->PinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"2600000000000000", 16));
    }else{
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"0600000000000000", 16));
    }

	//55域
	BUFCLR(szBuffer);
	Pack_Field55Data(pEmvTransData->Icc_Type,(PR_UINT8*)szBuffer,&nField55Len);
	if(nField55Len > 0){
		ASSERT_NORMAL(PubSetBit_separate( &m_iso, 55, szBuffer, nField55Len));
	}
	//60.1交易类型		60.2交易批次号	60.3网管交易类型  60.4	终端读取能力 60.5	基于PBOC借/贷记标准的IC卡条件代码 60.6	支持部分扣款和返回余额的标志
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf (szBuffer, "22%6.6s000601", szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, strlen(szBuffer)));

	//64域MAC
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
	//判断错误码
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
		//MAC校验
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

//联机  result:联机结果-1 联机失败 >=0   联机成功(有报文应答)
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
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Connect To Server");
    memcpy(pOnlineData->iccResponse,"00",2);
    memcpy(pOnlineData->ackdata,"\x9F""\x36""\x02""\x02""\x5A",5);
    pOnlineData->ackdatalen = 5;
    return PR_NORMAL;
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
	pcallbackfun->EMV_OnlineProcess = onlineProcess;
}

PR_INT32 EmvL2_Init(){
    EmvTermConfig_t termconfig;
    EmvCallBack_t t_callbackfun;

    Initialize_EMV_CallBackFun(&t_callbackfun);
    Initialize_EMV_TermConfig(&termconfig);
    if(Emv_KernelInit(termconfig,t_callbackfun) < 0){
        printf("emv 初始化失败\n");
        return PR_FAILD;
    }
	dbg_printf("IC Param Count： %d",Emv_GetAidTotalNum());
	dbg_printf("IC Capk Count： %d",Emv_GetCapkTotalNum());

    return PR_NORMAL;
}

PR_INT32 EmvL2_Proc(EmvTransParams_t emvTransParams){
    EMV_L2_Return nEmvRet = EMV_L2_CONTINUE;

    memset(&emvTransData,0x0,sizeof(EmvTransData));
    memcpy(emvTransData.TransAmount,emvTransParams.trans_amount,strlen((char*)emvTransParams.trans_amount));
	emvTransData.Icc_Type = emvTransParams.icc_type;
    nEmvRet = Emv_Process(emvTransParams);
    if(nEmvRet == EMV_L2_ACCEPT){
        return PR_NORMAL;
    }
    return PR_FAILD;
}

void EmvL2_SetDefaultAidAndCapk(){
    PR_UINT8 temp[1024];
    PR_INT8 aid1[] = "9F0608A000000333010101DF0101019F08020020DF1105FC78F4F8F0DF1205FC78F4F8F0DF130500100000009F1B0400002710DF150400000000DF160120DF170110DF14039F3704DF1801019F7B06000000100000DF1906000000100000DF2006000000100000DF2106000000010000";
    PR_INT8 aid2[] = "9F0608A000000333010102DF0101019F08020020DF1105FC78F4F8F0DF1205FC78F4F8F0DF130500100000009F1B0400002710DF150400000000DF160120DF170110DF14039F3704DF1801019F7B06000000100000DF1906000000100000DF2006000000100000DF2106000000010000";
    PR_INT8 aid3[] = "9F0608A000000333010103DF0101019F08020020DF1105FC78F4F8F0DF1205FC78F4F8F0DF130500100000009F1B0400002710DF150400000000DF160120DF170110DF14039F3704DF1801019F7B06000000100000DF1906000000100000DF2006000000100000DF2106000000010000";
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
        PR_INT8 capk1[] = "9F0605A0000003339F220103DF05083230323431323331DF060101DF070101DF0281B0B0627DEE87864F9C18C13B9A1F025448BF13C58380C91F4CEBA9F9BCB214FF8414E9B59D6ABA10F941C7331768F47B2127907D857FA39AAF8CE02045DD01619D689EE731C551159BE7EB2D51A372FF56B556E5CB2FDE36E23073A44CA215D6C26CA68847B388E39520E0026E62294B557D6470440CA0AEFC9438C923AEC9B2098D6D3A1AF5E8B1DE36F4B53040109D89B77CAFAF70C26C601ABDF59EEC0FDC8A99089140CD2E817E335175B03B7AA33DDF040103DF031487F0CD7C0E86F38F89A66F8C47071A8B88586F26";
        memset(temp,0x0,sizeof(temp));
        nAsc2Bcd(capk1, strlen((char *)capk1), temp, 0);
        if(Emv_PARAM_InputCAPKData(temp,strlen((char *)capk1)/2) <0){
            OsLog(LOG_ERROR,"Set CAPK1 Failed \n");
            return;
        }
    }
	EmvL2_Init();
}


/***********************IC 卡参数下载**************************/
typedef	struct
{
    PR_UINT8 Data[32];
    PR_UINT32 DataLength;
}QueryAidInfo;

typedef struct
{
	PR_UINT32 nCount;		//状态上送报文中已收到的参数信息的个数
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
	//0域34
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0820", 4));
	//41终端号
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
	//42商户号
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
	//60.1交易类型		60.2交易批次号	60.3网管交易类型
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s382",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
	//62域
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
	//获取商户号终端号用于报文校验
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
		//30 后续没有参数信息 31 后续有参数信息
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
	//0域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0800", 4));
	//41终端号
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
	//42商户号
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
	//60.1交易类型		60.2交易批次号	60.3网管交易类型
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s380",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
	//62域
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
	//0域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0800", 4));
	//41终端号
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
	//42商户号
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
	//60.1交易类型		60.2交易批次号	60.3网管交易类型
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	switch(type)
	{
	case 1://公钥下载结束
		{
			sprintf(szBuffer, "00%6.6s371", szBatch);
		}
		break;
	case 2://参数结束
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

/***********************IC 卡公钥下载**************************/
typedef	struct
{
    PR_UINT8 Data[32];
    PR_UINT32 DataLength;
}QueryCapkInfo;

typedef struct
{
	PR_UINT32 nCount;		//状态上送报文中已收到的参数信息的个数
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
	//0域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0820", 4));
	//41终端号
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
	//42商户号
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
	//60.1交易类型		60.2交易批次号	60.3网管交易类型
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s372",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
	//62域
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
	
	//获取商户号终端号用于报文校验
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

		//30 后续没有公钥信息 31 后续有公钥信息
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
	//0域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0800", 4));
	//41终端号
	BUFCLR(szTerminalNo);
	memcpy(szTerminalNo,YL_TERM,8);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, POS_ID, szTerminalNo, 8));
	//42商户号
	BUFCLR(szCustNo);
	memcpy(szCustNo,YL_MCH,15);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, CUSTOM_ID, szCustNo, 15));
	//60.1交易类型		60.2交易批次号	60.3网管交易类型
	BUFCLR(szBatch);
    ASSERT_NORMAL(Business_GetBatchNum((char*)szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s370",szBatch);
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, 60, szBuffer, 11));
	//62域
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

	//获取商户号终端号用于报文校验
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
		//30 后续没有公钥信息 31 后续有公钥信息
		if (szRetBuf[0] == 0x30)
		{
			return EXCEPTION;
		}
		else if (szRetBuf[0] == 0x31)	// 处理62域信息
		{
			//从62域得到公钥摸长度、公钥摸、公钥指数的长度、公钥指数、失效期
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

/***********************下载结束**************************/
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

	//获取商户号终端号用于报文校验
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

/***********************IC卡参数和公钥下载外部调用接口**************************/
PR_INT32  PubDownIcParams()
{
	int  nRet = 0;
	//查询
	nRet = PubQueryEMVPara();
    //不需要下载参数
	if (nRet == 0)
	{   
		return 0;
	}
	//需要下载参数
	if (nRet != 1)
	{
		goto FAIL;
	}
	nRet = PubDownREICPara();
	if(nRet)
	{
		goto FAIL;
	}
	//上送下载参数结束交易
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
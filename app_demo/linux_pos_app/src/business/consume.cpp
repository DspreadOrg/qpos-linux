#include "appinc.h"

#define INPUT_STRIPE   (1<<0)  //刷卡
#define INPUT_INSERTIC (1<<1)  //插卡
#define INPUT_RFCARD   (1<<2)  //挥卡

typedef struct tagCardDataInfo
{
	ST_MSR_DATA ptTracker1;
	ST_MSR_DATA ptTracker2;
	ST_MSR_DATA ptTracker3;
	PR_INT8 CardSN[3+1];
	PR_INT8 Pan[19+1+1];		//主账号
	PR_INT8 ExpDate[4+1];
    PR_INT8 ServiceCode[3+1];
    PR_INT8 CardCompany[3+1];	//信用卡公司

}CardDataInfo;

PR_INT32 CardDev_Enable()
{
    PR_INT32 nRet;

    nRet = OsMsrOpen();
    if(nRet != RET_OK){
        goto EXP;
    }

    nRet = OsIccOpen(0);
    if(nRet != RET_OK){
        goto EXP;
    }

    nRet = OsPiccOpen();
    if(nRet != RET_OK){
        goto EXP;
    }
    return PR_NORMAL;
EXP:
    OsMsrClose();
    OsIccClose(0);
    OsPiccClose();
	return PR_FAILD;
}

void CardDev_Disable(PR_INT32 nSwipType){
    OsMsrClose();

	if (nSwipType != INPUT_INSERTIC){
        OsIccClose(0);
    }
		
	if (nSwipType != INPUT_RFCARD){
        OsPiccClose();
    }
}

static PR_BOOL CardDataIsNum(PR_INT8 Data){
	if(Data>='0'&&Data<='9')
		return PR_TRUE;
	else
		return PR_FALSE;
}

PR_BOOL ParseTrack2(const PR_INT8* theTrack2Data, PR_UINT32 theTrack2DataLength, CardDataInfo* theTrackDataInfo)
{
	PR_UINT32 i=0;
	PR_UINT32 cardNumLength = 0;

	for( ;i<theTrack2DataLength; i++)
	{
		if(CardDataIsNum(theTrack2Data[i])==PR_FALSE)
		{
			// 卡号由其实字符（1字节）之后开始，到分隔符结束
			#define SIZE_OF_START_FLAG 0
			cardNumLength = i-SIZE_OF_START_FLAG;	

			memcpy(theTrackDataInfo->Pan, theTrack2Data+SIZE_OF_START_FLAG, cardNumLength);
			memcpy(theTrackDataInfo->ExpDate, theTrack2Data+i+1, 4);
			memcpy(theTrackDataInfo->ServiceCode, theTrack2Data+i+1+4, 3);
			break;
		}
	}
	return PR_TRUE;
}

PR_INT32 ReadMagCard(CardDataInfo *DataInfo){

    if(OsMsrRead(&DataInfo->ptTracker1,&DataInfo->ptTracker2,&DataInfo->ptTracker3) != 0){
        return PR_FAILD;
    }
    if(DataInfo->ptTracker2.Status != 0 || DataInfo->ptTracker2.DataLen <= 22){
        //BCTC一张错误卡为22个字节LRC等均正确需要判断为错误卡
        return PR_FAILD;
    }
    
    ParseTrack2((PR_INT8*)DataInfo->ptTracker2.TeackData, DataInfo->ptTracker2.DataLen, DataInfo);
    return PR_NORMAL;
}

PR_INT32 ReadCardProc(PR_INT8 *pszAmount,PR_INT32 nTimeoutS,PR_INT32 *pRetSwipeType,CardDataInfo *pCardDataInfo){

    PR_INT8 szDisplayAmount[12+2] = {0};
    PR_INT8 szPiccType[8] = {0};
    PR_UINT8 ucATQx[16] = {0};
    PR_INT32 nRet = PR_FAILD;
    long start = get_sys_tick();

    TransView_vClearPort();
    if(CardDev_Enable() != PR_NORMAL){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Card Device Open Failed");
        return PR_FAILD;
    }
    TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"%s",szDisplayAmount);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Swipe/Insert/Tap");
    PR_nUtilNumberToAmt(pszAmount,sizeof(szDisplayAmount),szDisplayAmount);
    while (get_sys_tick() - start < nTimeoutS*1000)
    {
        nRet = OsPiccPoll(szPiccType,ucATQx);
        if(nRet == RET_OK){
            *pRetSwipeType = INPUT_RFCARD;
            nRet = PR_NORMAL;
            goto EXIT;
        }
        nRet = OsIccDetect(0);
        if(nRet == RET_OK){
            *pRetSwipeType = INPUT_INSERTIC;
            nRet = PR_NORMAL;
            goto EXIT;
        }
        nRet = OsMsrSwiped();
        if(nRet){
            if(ReadMagCard(pCardDataInfo) != PR_NORMAL){
                OsMsrReset();
            }else{
                *pRetSwipeType = INPUT_STRIPE;
                nRet = PR_NORMAL;
                goto EXIT;
            }
        }
        nRet = KB_nWaitKeyMS(300);
		if(nRet == EM_KEY_CANCEL){
            nRet = PR_CANCLE;
            goto EXIT;
        }
    }
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Read Card Timeout");
    nRet = PR_TIMEOUT;
EXIT:
    if(nRet == PR_NORMAL){
        CardDev_Disable(*pRetSwipeType);
    }else{
        //关闭所有读卡设备
        CardDev_Disable(0);
    }
    return nRet;
}

PR_INT32 MagCard_8583_Pack(CardDataInfo *DataInfo,PR_INT8* pszAmount,PR_INT8* pPinBlock,PR_INT32 nPinLen,PR_INT8 * szPackStr, PR_INT32 nPackSize, PR_INT32 *nRetLen)
{
	char szBuffer[100],szTerminalNo[9],szCustNo[16];
	uchar szBatch[7];
	uint nLen;
    int nLength;
	char szGetBuffer[30];
    ISODataStru m_iso;  
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
	//0域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0200", 4));
	//3域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, PROC_CODE, (char*)"000000", 6));
    //4域
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, AMOUNT, pszAmount, 12));
	//11域
	BUFCLR(szBuffer);
	ASSERT_NORMAL(Business_GetTradeNum(szBuffer,sizeof(szBuffer)-1));
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACE_NO, szBuffer, 6));
    //22域
    if(nPinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"021", 3));
    }else{
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"022", 3));
    }
	//25域
	ASSERT_NORMAL(PubSetBit_separate( &m_iso, 25, (char*)"00", 2));
    //26域
    if(nPinLen > 0){
	    ASSERT_NORMAL(PubSetBit_separate( &m_iso, 26, (char*)"12", 2));
    }
    //35域
    if(DataInfo->ptTracker2.Status == 0 && DataInfo->ptTracker2.DataLen > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACK2, (char*)DataInfo->ptTracker2.TeackData, DataInfo->ptTracker2.DataLen));
    }
     //36域
    if(DataInfo->ptTracker3.Status == 0 && DataInfo->ptTracker3.DataLen > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACK3, (char*)DataInfo->ptTracker3.TeackData, DataInfo->ptTracker3.DataLen));
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
    if(nPinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, PIN, pPinBlock, 8));
    }
    //53域
    if(nPinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"2600000000000000", 16));
    }else{
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"0600000000000000", 16));
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
	((uchar *)szPackStr, *nRetLen, "银联磁条卡8583包");
	return 0;
}

PR_INT32 qrcode_unionpay_dis_ret_msg (PR_INT8 * szReadBuf, PR_INT32 iReadLen )
{
	if (szReadBuf == NULL || iReadLen == 0)
	{
		return -1;
	}
	PR_INT8 szBuffer[100] = {0};
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
		return EXCEPTION;
	}
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code:%s",szErrCode);
	if (memcmp(szErrCode,"00", 2) == 0)
	{
		//MAC校验
		if(Business_CalcMac(1, (uchar*)szReadBuf + PACK_RHEAD_LEN + 2, iReadLen - 8 - PACK_RHEAD_LEN - 2,szMac))
		{
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mac Ckeck Err1");
            return EXCEPTION;
		}
        if(memcmp(szMac,szReadBuf + iReadLen - 8,8) != 0){
            TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Mac Ckeck Err2");
            return EXCEPTION;
        }
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale success");
        return PR_NORMAL;
	}
	else
	{
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Failed");
		return EXCEPTION;
	}
}

PR_INT32 Mag_Auth(PR_INT8* pszAmount,CardDataInfo *DataInfo){
    PR_UINT8 PinBlock[16+1] = {0};
    PR_INT32 keyIndex = 1;
    PR_INT32 iRet = PR_FAILD;
    PR_INT32 pinLen = 0;
    PR_INT8 DisplayAmount[12+2] = {0};
    PR_INT32 iLen = 0;
	PR_INT32 iSendLen = 0;
	PR_INT32 iRecvLen = 0;
	PR_INT8 szBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szSendBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szRecvBuf[READBUFF_SIZE + 1] = {0};

    OsLog(LOG_DEBUG,"Card Num  【%s】",DataInfo->Pan);
	iRet = TransView_nShowPinpadView(1,pszAmount,(PR_UINT8*)DataInfo->Pan,(char *)"0,6",0x00,30*1000,PinBlock);
	if(iRet != RET_OK){
        return PR_FAILD;
    }
    OsPedPinKeyNotify(&pinLen,NULL);
    OsLog(LOG_DEBUG,"pin len  【%d】",pinLen);

    TransView_vClearPort();
    BUFCLR(szBuf);
	iRet = MagCard_8583_Pack(DataInfo, pszAmount, (char*)PinBlock, pinLen,szBuf, sizeof(szBuf) - 1, &iLen);
	if ( iRet ) {
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Packing Exception");
		goto FAIL;
	}
	BUFCLR(szSendBuf);
	iRet  = pub_pack_header(szBuf, iLen, szSendBuf, &iSendLen);
	if ( iRet ) {
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Packing Exception1");
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
	iRet = qrcode_unionpay_dis_ret_msg(szRecvBuf, iRecvLen);
	if ( iRet ) {
		goto FAIL;
	}
    PrintOrder();
#else
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale success");
    PrintOrder();
#endif

FAIL:
    KB_nWaitKeyMS(5*1000);
    return PR_NORMAL;
}

PR_INT32 Emv_Auth(PR_INT8* pszAmount,PR_INT32 nRetSwipeType){
    EmvTransParams_t emvTransParams;
    PR_INT32 nEmvRet;

    memset(&emvTransParams,0x0,sizeof(EmvTransParams_t));
    emvTransParams.trans_type = EMV_L2_SALE;  
    memcpy((char *)emvTransParams.trans_amount,pszAmount,12);
    emvTransParams.ec_support = 1; 
    emvTransParams.sm_support = 0; 
    emvTransParams.force_online_enable = 0; 
    Business_GetTradeTsc((char*)emvTransParams.tsc,sizeof(emvTransParams.tsc));
    if(nRetSwipeType == INPUT_INSERTIC){
        emvTransParams.icc_type = CONTACT_ICC;
        emvTransParams.trans_flow = PBOC;  
    }else{
        emvTransParams.icc_type = CONTACTLESS_ICC;
        emvTransParams.trans_flow = QPBOC;  
    }
    nEmvRet = EmvL2_Proc(emvTransParams);
    if(nEmvRet != PR_NORMAL){
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Failed");
    }else{
#ifdef POS_OFFLINE
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Sucess");
#endif
        PrintOrder();
    }
    KB_nWaitKeyMS(3*1000);
    return nEmvRet;
}

PR_INT32 CousumeTransProc(){
    PR_INT8 Amount[12+1] = {0};
    PR_INT32 nRetSwipeType = 0;
    PR_INT32 nRet = PR_FAILD;
    CardDataInfo t_CardDataInfo;

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Sale");

    if(PR_FAILD == TransView_nShowAmountInputView(sizeof(Amount),Amount))
    {
    	return PR_FAILD;
    }
    memset(&t_CardDataInfo,0x0,sizeof(CardDataInfo));
    nRet = ReadCardProc(Amount,30,&nRetSwipeType,&t_CardDataInfo);
    if(nRet != PR_NORMAL){
        return nRet;
    }
    if(nRetSwipeType == INPUT_STRIPE){   
        Mag_Auth(Amount,&t_CardDataInfo);
    }else{
        if(nRetSwipeType == INPUT_INSERTIC){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Do Not Pull Out The Card");
        }
        Emv_Auth(Amount,nRetSwipeType);
    }
    return PR_NORMAL;
}


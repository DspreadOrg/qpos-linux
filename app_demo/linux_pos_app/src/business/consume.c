#include "appinc.h"

#define INPUT_STRIPE   (1<<0)  //swipe card
#define INPUT_INSERTIC (1<<1)  //insert card
#define INPUT_RFCARD   (1<<2)  //tap card

typedef struct tagCardDataInfo
{
	ST_MSR_DATA ptTracker1;
	ST_MSR_DATA ptTracker2;
	ST_MSR_DATA ptTracker3;
	PR_INT8 CardSN[3+1];
	PR_INT8 Pan[19+1+1];		//pan
	PR_INT8 ExpDate[4+1];
    PR_INT8 ServiceCode[3+1];
    PR_INT8 CardCompany[3+1];	//credit card companies

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
// The card number starts after the actual character (1 byte) and ends with the delimiter.
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
// BCTC error card is 22 bytes LRC, etc., all correct and need to be judged as an error card.
        return PR_FAILD;
    }
    
    ParseTrack2((PR_INT8*)DataInfo->ptTracker2.TeackData, DataInfo->ptTracker2.DataLen, DataInfo);
    return PR_NORMAL;
}

PR_INT32 ReadCardProc(PR_INT8 *pszAmount,PR_INT32 nTimeoutS,PR_INT32 *pRetSwipeType,CardDataInfo *pCardDataInfo,u32 cardsSupported){

    PR_INT8 szDisplayAmount[12+2] = {0};
    PR_INT8 szPiccType[8] = {0};
    PR_UINT8 ucATQx[16] = {0};
    PR_INT32 nRet = PR_FAILD;
    long long start = get_sys_tick();
    unsigned char atr[64] = {0};
    CardDev_Disable(0);
    TransView_vClearPort();
    if(CardDev_Enable() != PR_NORMAL){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Card Device Open Failed");
        return PR_FAILD;
    }

    if(pszAmount)
    {
        TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"%s",szDisplayAmount);
        PR_nUtilNumberToAmt(pszAmount,sizeof(szDisplayAmount),szDisplayAmount);
    }


    if(cardsSupported == (CARD_NFC|CARD_IC|CARD_MAG))
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Swipe/Insert/Tap");
    else if(cardsSupported == CARD_IC)
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Please Insert Card");    
    else if(cardsSupported == CARD_NFC)
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Please Tap Card");
    else if(cardsSupported == CARD_MAG)
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Please Swipe Card");    

    while (get_sys_tick() - start < nTimeoutS*1000)
    {
        if(cardsSupported&CARD_NFC)
        {
            nRet = OsPiccPoll(szPiccType,ucATQx);
            if(nRet == RET_OK){
                *pRetSwipeType = INPUT_RFCARD;
                nRet = PR_NORMAL;
                goto EXIT;
            }
        }
        if(cardsSupported&CARD_IC)
        {
            nRet = OsIccDetect(0);
            if(nRet == RET_OK){
                    if(OsIccInit(ICC_USER_SLOT,0,atr) == 0)
                    {
                        OsBeep(7,100);
                        #ifdef CFG_DBG
                        OsLog(LOG_INFO,"Dspread: ----------INPUT_INSERTIC-----------");
                        #endif
                        *pRetSwipeType = INPUT_INSERTIC;
                        nRet = PR_NORMAL;
                    }
                    else
                    {
                        OsBeep(7,100);
                        #ifdef CFG_DBG
                        OsLog(LOG_INFO,"Dspread: ----------INPUT_INSERTIC ERROR-----------");
                        #endif
                        nRet = PR_ICC_ERR;
                    }
                goto EXIT;
            }
        }
        if(cardsSupported&CARD_MAG)
        {
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
// Turn off all card reading devices
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
// 0 Domain
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, MSG_ID, (char*)"0200", 4));
// 3 domains
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, PROC_CODE, (char*)"000000", 6));
// 4 domains
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, AMOUNT, pszAmount, 12));
// 11 domains
	BUFCLR(szBuffer);
	ASSERT_NORMAL(Business_GetTradeNum(szBuffer,sizeof(szBuffer)-1));
	ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACE_NO, szBuffer, 6));
// 22 domains
    if(nPinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"021", 3));
    }else{
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, SERVE_MODE, (char*)"022", 3));
    }
// 25 domains
	ASSERT_NORMAL(PubSetBit_separate( &m_iso, 25, (char*)"00", 2));
// 26 domains
    if(nPinLen > 0){
	    ASSERT_NORMAL(PubSetBit_separate( &m_iso, 26, (char*)"12", 2));
    }
// 35 domains
    if(DataInfo->ptTracker2.Status == 0 && DataInfo->ptTracker2.DataLen > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACK2, (char*)DataInfo->ptTracker2.TeackData, DataInfo->ptTracker2.DataLen));
    }
// 36 domains
    if(DataInfo->ptTracker3.Status == 0 && DataInfo->ptTracker3.DataLen > 0){
        ASSERT_NORMAL(PubSetBit_separate(&m_iso, TRACK3, (char*)DataInfo->ptTracker3.TeackData, DataInfo->ptTracker3.DataLen));
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
    if(nPinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, PIN, pPinBlock, 8));
    }
// 53 domains
    if(nPinLen > 0){
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"2600000000000000", 16));
    }else{
        ASSERT_NORMAL(PubSetBit_separate( &m_iso, 53, (char*)"0600000000000000", 16));
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
// Judgment error code
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
// MAC verification
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
	iRet = TransView_nShowPinpadView(1,pszAmount,(PR_UINT8*)DataInfo->Pan,(char *)"0,6",0x00,60,PinBlock);
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
    PrintOrder(1);
#else

#ifdef POS_PAPER_TRADING
    if(Is_Valid_Network() == PR_NORMAL){
		iRet = ppp_connect_test();
        if(iRet == 0){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale success");
            PrintOrder(2);
        }
	}else{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Network Not Connected");
    }
#else
   
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale success");
    PrintOrder(1);
#endif

#endif

FAIL:
    KB_nWaitKeyMS(5*1000);
    return PR_NORMAL;
}

void setTransTime(PR_INT8 *szTime)
{
    ST_TIME Time;
    char *pTemp = NULL;
    pTemp = malloc(20);
    memset(pTemp,0,20);
    OsGetTime(&Time);
    sprintf(pTemp,"%04d",Time.Year);
    sprintf(pTemp+4,"%02d",Time.Month);
    sprintf(pTemp+6,"%02d",Time.Day);
    sprintf(pTemp+8,"%02d",Time.Hour);
    sprintf(pTemp+10,"%02d",Time.Minute);
    sprintf(pTemp+12,"%02d",Time.Second);
    asc_to_bcd(pTemp,14,szTime);
    free(pTemp);
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
    setTransTime(emvTransParams.trans_time);
    Business_GetTradeTsc((char*)emvTransParams.tsc,sizeof(emvTransParams.tsc));
    if(nRetSwipeType == INPUT_INSERTIC){
        emvTransParams.icc_type = CONTACT_ICC;
        emvTransParams.trans_flow = PBOC;  
    }else{
        emvTransParams.icc_type = CONTACTLESS_ICC;
        emvTransParams.trans_flow = QPBOC;  
    }
    OsLog(LOG_DEBUG,"====EmvL2_Proc====");
    nEmvRet = EmvL2_Proc(emvTransParams);
    if(nEmvRet != PR_NORMAL){
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Failed");
    }else{

        if(emvTransParams.icc_type == CONTACTLESS_ICC)
        {
            EmvOnlineData_t emvOnlineData;
            memset(&emvOnlineData,0,sizeof(EmvOnlineData_t));
            nEmvRet = onlineProcess(&emvOnlineData);
            if(nEmvRet == PR_NORMAL)
            {
                if(emvOnlineData.ackdatalen > 0)
                {
                    #ifdef APP_CTL_DOUBLE_TAP_POLL_CARD 
                    int RetSwipeType = 0;
                    CardDataInfo t_CardDataInfo;
                    memset(&t_CardDataInfo,0x0,sizeof(CardDataInfo));
                    if(ReadCardProc(NULL,30,&RetSwipeType,&t_CardDataInfo,CARD_NFC) == PR_NORMAL)
                    {
                        nEmvRet = Emv_SetOnlineResult(APP_POLL_CTL_MODE,&emvOnlineData);
                        if(nEmvRet == PR_NORMAL)
                        {
                            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Sucess");
                        }
                        else
                        {
                            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Failed");
                        }
                    }
                    else
                    {
                        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Failed");
                    }
                    #else
                        nEmvRet = Emv_SetOnlineResult(KERNEL_POLL_CTL_MODE,&emvOnlineData);
                        if(nEmvRet == PR_NORMAL)
                        {
                            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Sucess");
                        }
                        else
                        {
                            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Failed");
                        }
                    #endif

                }
               else
               {
                    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Sucess");
               }
            }
        }
        else
        {
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sale Sucess");
        }

#if POS_PAPER_TRADING
        PrintOrder(2);
#else   
        PrintOrder(1);
#endif 
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
    nRet = ReadCardProc(Amount,30,&nRetSwipeType,&t_CardDataInfo,CARD_IC|CARD_NFC|CARD_MAG);
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
    CardDev_Disable(0);
    return PR_NORMAL;
}


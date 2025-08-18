#include "debug.h"
#include "appinc.h"

TransactionData g_transactionData;
extern ONLINE_STATUS onlineStatus;
static int stop_read_cards = 0;

#ifdef CFG_DBG
static const char *EMV_RETURN_CODE[] =
{
	"APP_RC_COMPLETED",
	"APP_RC_TERMINAL",
	"APP_RC_CANCEL",
	"APP_RC_EMV_DENAIL",
	"APP_RC_EMV_GAC2_DENAIL",
	"APP_RC_NFC_NOT_ALLOW",
	"APP_RC_EMV_APP_BLOCK",
	"APP_RC_EMV_APP_SEE_PHONE",
	"APP_RC_EMV_TRANS_TRY_ANOTHER_INTERFACE",
	"APP_RC_EMV_TRANS_GPO_NOT_SUPPORT",
	"APP_RC_FALL_BACK",
	"APP_RC_EMV_CARD_BLOCK",
	"APP_RC_CARD_NOT_SUPPORT",
	"APP_RC_NFC_RETAP_TIMEOUT",
	"APP_RC_NFC_RETAP_CANCEL",
	"APP_RC_NFC_TERMINAL",
	"APP_RC_NFC_DOUBLETAP_DENAIL",
	"APP_RC_NFC_MULTI_CARD",
	"APP_RC_NFC_TRY_AGAIN",
	"APP_RC_TRANS_REVERSEL",
	"APP_RC_NUMS",
};
#endif

KB_KEYMAP_T KB_nWaitKeyMS(Int32 MS)
{
	Disp_ReleasKey(); //Clear cache key values
	return  Disp_GetKey(MS);
}

KB_KEYMAP_T KB_nWaitKeyMS_Ex(Int32 MS)
{
	return  Disp_GetKey(MS);
}

void KB_vReleasKey(){
	Disp_ReleasKey();
}

void clear_transaction_data(){
    memset(&g_transactionData,0x0,sizeof(TransactionData));
    memset(&onlineStatus,0x00,sizeof(ONLINE_STATUS));
}

TransactionData* get_transaction_data(){
    return &g_transactionData;
}

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

PR_INT32 ReadCardProc(PR_INT32 nTimeoutS,PR_INT32 *pRetSwipeType,CardDataInfo *pCardDataInfo,u32 cardsSupported){

    PR_INT8 szPiccType[8] = {0};
    PR_UINT8 ucATQx[16] = {0};
    PR_INT32 nRet = PR_FAILD;
    long long start = get_sys_tick();
    unsigned char atr[64] = {0};

     CardDev_Disable(0);

    if(CardDev_Enable() != PR_NORMAL){
        return PR_FAILD;
    }
    while (get_sys_tick() - start < nTimeoutS*1000)
    {   
// OsLog(LOG_DEBUG,"stop_read_cards =%d",stop_read_cards);
        if(stop_read_cards){
            nRet = PR_CANCLE;
            goto EXIT;
        }
        if(cardsSupported&CARD_NFC)
        {
            nRet = OsPiccPoll(szPiccType,ucATQx);
            if(nRet == RET_OK){
                OsBeep(7,100);
                *pRetSwipeType = INPUT_RFCARD;
                nRet = PR_NORMAL;
                OsLog(LOG_DEBUG,"OsPiccPoll success");
                get_transaction_data()->nPosEntryMode = INPUT_RFCARD;
                goto EXIT;
            }
        }
        if(cardsSupported&CARD_NFC)
        {
            nRet = OsIccDetect(0);
            if(nRet == RET_OK){
                if(OsIccInit(ICC_USER_SLOT,0,atr) == 0)
                {
                    OsBeep(7,100);
                    OsLog(LOG_DEBUG,"OsIccInit success");
                    *pRetSwipeType = INPUT_INSERTIC;
                    nRet = PR_NORMAL;
                    get_transaction_data()->nPosEntryMode = INPUT_INSERTIC;
                }
                else
                {
                    OsLog(LOG_DEBUG,"OsIccInit fail");
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
                    OsBeep(7,500);
                }else{
                    OsBeep(7,100);
                    *pRetSwipeType = INPUT_STRIPE;
                    nRet = PR_NORMAL;
                    get_transaction_data()->nPosEntryMode = INPUT_STRIPE;
                    memcpy(get_transaction_data()->sTracker2,pCardDataInfo->ptTracker2.TeackData,pCardDataInfo->ptTracker2.DataLen);
                    get_transaction_data()->nTracker2Len = pCardDataInfo->ptTracker2.DataLen;
                    memcpy(get_transaction_data()->sCardNo,pCardDataInfo->Pan,strlen(pCardDataInfo->Pan));
                    memcpy(get_transaction_data()->sExpDate,pCardDataInfo->ExpDate,strlen(pCardDataInfo->ExpDate));
                    goto EXIT;
                }
            }
        }
    }
    nRet = PR_TIMEOUT;
EXIT:
    if(nRet == PR_NORMAL){
        CardDev_Disable(*pRetSwipeType);
    }else{
// close all reader
        CardDev_Disable(0);
    }
    return nRet;
}

PR_INT32 Emv_Auth(PR_INT8* pszAmount,PR_INT32 nRetSwipeType){
    EmvTransParams_t emvTransParams;
    PR_INT32 nEmvRet;
    char szCardno[20] = {0};

    memset(&emvTransParams,0x0,sizeof(EmvTransParams_t));
    emvTransParams.trans_type = EMV_L2_SALE;  
    memcpy((char *)emvTransParams.trans_amount,pszAmount,12);
    Business_getSysTime((char*)emvTransParams.trans_time,sizeof(emvTransParams.trans_time));
    memcpy(emvTransParams.trans_currency_code,"\x01""\x56", 2);
    emvTransParams.force_online_enable = 0; 
    if(nRetSwipeType == INPUT_INSERTIC ){
        emvTransParams.icc_type = CONTACT_ICC;
    }else if(nRetSwipeType == INPUT_RFCARD){
        emvTransParams.icc_type = CONTACTLESS_ICC;
    }
    else{
        return -1;
    }
    nEmvRet = EmvL2_Proc(emvTransParams);
    return nEmvRet;
}

void sale_init(){
    clear_transaction_data();
    memset(&onlineStatus, 0x00, sizeof(onlineStatus));
	onlineStatus.Rc = RC_FAIL;
    g_transactionData.nTransType = TT_SALE;
    g_transactionData.nStatus = APP_RC_START;
    Business_GetTradeNum(g_transactionData.sTrace,sizeof(g_transactionData.sTrace));
    Business_GetBatchNum(g_transactionData.sBatch,sizeof(g_transactionData.sBatch));
    Business_getSysTimeStr(g_transactionData.sTransTime,sizeof(g_transactionData.sTransTime));
    strcpy(g_transactionData.sOrderNo,g_transactionData.sTransTime);
    strcat(g_transactionData.sOrderNo,g_transactionData.sTrace);
}

void read_cards_process(u32 cardsSupported){
    int nRetSwipeType = 0;
    int nRet = PR_FAILD;
    CardDataInfo gt_CardDataInfo;
    EMV_L2_Return nEmvRet = APP_RC_START;

    stop_read_cards = 0;
    memset(&gt_CardDataInfo,0x0,sizeof(CardDataInfo));
    nRet = ReadCardProc(60,&nRetSwipeType,&gt_CardDataInfo,cardsSupported);
    if(nRet != PR_NORMAL){
        if(nRet == PR_CANCLE || nRet == PR_TIMEOUT){
            GuiEventRegister(LCD_DISP_CANCEL);
        }
        else if(nRet == PR_ICC_ERR)
        {
            GuiEventRegister(LCD_DISP_FALL_BACK);
        }
        else if(nRet == PR_TIMEOUT)
        {
             GuiEventRegister(LCD_DISP_TIMEOUT);
        }
        else
        {
            GuiEventRegister(LCD_DISP_TRADE_FAIL);
        }
        return;
    }
    get_transaction_data()->icc_type = nRetSwipeType;
    if(nRetSwipeType == INPUT_STRIPE){   
       
        nRet = swipcard_process();
        if(nRet == RET_OK)
        {
            get_transaction_data()->nStatus = APP_RC_COMPLETED;
            GuiEventRegister(LCD_DISP_TRADE_SUCCESS);
        }
        else
        {
            GuiEventRegister(LCD_DISP_TRADE_FAIL);
        }
    }else{
        if(nRetSwipeType == INPUT_INSERTIC){
            GuiEventRegister(LCD_DISP_READING_CARD);
        }
        DSP_Info("amount   %s",g_transactionData.sAmount);
        nEmvRet = Emv_Auth(g_transactionData.sAmount,nRetSwipeType);
        #ifdef CFG_DBG
            if(nEmvRet < APP_RC_NUMS && nEmvRet != -1)
                OsLog(LOG_DEBUG,"emv kernel recode [%s]",EMV_RETURN_CODE[nEmvRet]);
            else
                OsLog(LOG_DEBUG,"emv kernel recode [%d]",nEmvRet);     
        #endif
        g_transactionData.nStatus = nEmvRet;
        switch(nEmvRet)
        {
            case APP_RC_COMPLETED:
                GuiEventRegister(LCD_DISP_TRADE_SUCCESS);
                break;

            case APP_RC_TERMINAL:
            case APP_RC_NFC_TERMINAL:
                GuiEventRegister(LCD_DISP_TRANSACTION_TERMINATED);
                break;

            case APP_RC_CANCEL:
                GuiEventRegister(LCD_DISP_CANCEL);
                break;

            case APP_RC_EMV_APP_BLOCK:
                GuiEventRegister(LCD_DISP_EMV_APP_BLOCK);
                break;

            case APP_RC_EMV_DENAIL:
                GuiEventRegister(LCD_DISP_DECLINED_DISP);
                break;

            case APP_RC_FALL_BACK:
                GuiEventRegister(LCD_DISP_FALL_BACK);
                break;

            case APP_RC_CARD_NOT_SUPPORT:
                GuiEventRegister(LCD_DISP_CARD_NOT_SUPPORT);
                break;

            case APP_RC_EMV_TRANS_TRY_ANOTHER_INTERFACE:
                GuiEventRegister(LCD_DISP_TRY_ANOTHER_INTERFACE);
                break;
            case APP_RC_NFC_TRY_AGAIN:
                GuiEventRegister(LCD_DISP_NFC_TAP_TRY_AGAIN);
                break;
            case APP_RC_TRANS_REVERSEL:
                //start reversel process
                GuiEventRegister(LCD_DISP_TRADE_SUCCESS); //just a demo
                break;
            default:
                GuiEventRegister(LCD_DISP_TRANSACTION_TERMINATED);
                break;
        }
    }
}

void stop_readcards(){
    stop_read_cards = 0x01;
}

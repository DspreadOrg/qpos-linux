
#include "app_trans.h"
#include "app_pkt.h"
#include "cjson.h"
#include "ui_MultiApp.h"

pu8 g_pTransBuffer = NULL;
extern Req_Trans_Type glb_trans_type;

char g_disp_msg[128]={0};

#ifdef CFG_DBG
static const char *strCallback[] =
{
    "SWIPE_CARD",
    "TRANS_ONLINE_PIN",
    "TRANS_OFFLINE_PIN",
    "TRANS_NFC_ONLINE",
    "TRANS_NFC_APPROVED",
    "TRANS_NFC_DECLINE",
    "TRANS_OFFLINE_LASTE_PIN",
    "TRANS_REVERSAL",
    "TRANS_APP_SELECT",
    "TRANS_ACCOUNT_TYPE",
    "TRANS_ICC_GAC2",
    "TRANS_ICC_ONLINE",
    "TRANS_PROCESS_CUSTOM",
};
#endif

/// Enumerador con el modo de lectura seleccionado para la transacci贸n
typedef enum TradMode {
	/// Modo de lectura solo chip
	READ_MODE_ICC = 0x01,
	/// Modo de lectura solo banda magn茅tica
	READ_MODE_MAG,
	/// Modo de lectura banda magn茅tica, chip y sin contacto
	READ_MODE_ICC_MAG_NFC,
	/// Modo de lectura banda magn茅tica, chip y sin contacto (sin downgrade)
	READ_MODE_ICC_MAG_NFC_DONT_ALLOW_DOWNGRADE,
	/// Modo de lectura solo banda magn茅tica y chip
	READ_MODE_ICC_MAG,
	/// Modo de lectura solo banda magn茅tica y chip (sin downgrade)
	READ_MODE_ICC_MAG_DONT_ALLOW_DOWNGRADE,
	/// Modo de lectura solo sin contacto
	READ_MODE_NFC,
	/// Modo de lectura solo banda magn茅tica y sin contacto (sin tecla arriba)
	READ_MODE_ICC_MAG_NFC_NOUPKEY,
	/// Modo de lectura banda magnetica, chip y sin contacto (sin tecla arriba)
	READ_MODE_ICC_MAG_NFC_NOUPKEY_DONT_ALLOW_DOWNGRADE,
	/// Modo de lectura chip y sin contacto (sin tecla arriba)
	READ_MODE_ICC_NFC_DONT_USE_UPKEY,
	/// Modo de lectura chip y sin contacto
	READ_MODE_ICC_NFC,
	/// Modo de lectura chip y sin contacto (con downgrade)
	READ_MODE_ICC_NFC_DONT_ALLOW_DOWNGRADE
} E_TRADE_MODE;

static TRANS_T Trans_init[]=
{
    {TAG_EMV_TRANSACTION_TYPE,          1,  "\x00"},
    {POS_TAG_TRADE_MODE,                1,  "\x09"},
    {TAG_EMV_TRANSACTION_CURRENCY_CODE, 2,  "\x05\x66"},
    {POS_TAG_TRANSACTION_PASSWORD_MIN,  1,  "\x04"},
    {POS_TAG_TRANSACTION_PASSWORD_MAX,  1,  "\x06"},

    /*Master card refund transaction test case needs to added.
	At the same time, the following content needs to be added under the AID tag in the EMV configuration file
	<7F11>9C0120DF812105FFFFFFFFFFDF83020101<7F11>
	Note:rember to set transaction type to 20
	*/
    {POS_TAG_CUSTOM_REQUEST_HOST,       1,  "\x04"},//support Master card refund transaction test case.
};

static const u32 g_CardTlv[]=
{
    POS_TAG_RES_RESULT,
    POS_TAG_RES_PAN,
    POS_TAG_RES_CARDHOLDER,
    TAG_EMV_AID_TERMINAL,
    POS_TAG_RES_SERVICE_CODE,
    TAG_EMV_AMOUNT_AUTH_NUM,
    TAG_EMV_TRANSACTION_TYPE,
    TAG_EMV_CVM_RESULTS,
    POS_TAG_CLEAR_TRACK2,
    POS_TAG_ASC_PIN,
    POS_TAG_RES_VALID_PERIOD,
    POS_TAG_RES_KN_ONLINE_DATA,
    TAG_EMV_CRYPTOGRAM_INFO_DATA,
    TAG_EMV_TERMINAL_CAPABILITIES
};

 char request_get[]="GET %s HTTP/1.1\r\n"\
"Host:%s\r\n"\
"User-Agent:QposTrio-requests\r\n"\
"Accept:*/*\r\n\r\n";

 char request_post[]="POST %s HTTP/1.1\r\n"\
"Host:%s\r\n"\
"User-Agent:QposTrio-requests\r\n"\
"Accept:*/*\r\n\r\n"\
"%s\r\n";



/*-----------------------------------------------------------------------------
|   Function
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
|   Function Name:
|       PaymentInit
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void PaymentInit(pu8 pTransBuffer)
{
    u32 i=0;
    tlv_new_(pTransBuffer, POS_TAG_HEAD, 0, NULL);
    for(i=0;i<SIZE_ARRAY(Trans_init);i++)
    {
        tlv_add_child_(pTransBuffer, Trans_init[i].tag,Trans_init[i].len,Trans_init[i].value);
    }
}

/*----------------------------------------------------------------------------
|   Function Name:
|       TransPoolSet
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void TransPoolSet(u32 tag,T_U8_VIEW tag_value)
{
    set_tlv_view(g_pTransBuffer,tag,tag_value);
}

void TransPoolUpdate(u32 tag,pu8 pValue,u32 Len)
{
    tlv_replace_(g_pTransBuffer,tag,Len,pValue);
}

u32 getDataFromTransPool(u32 tag,pu8 pData)
{
    return get_tlv_value(g_pTransBuffer, tag, pData);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       GetCardData
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void GetCardData(pu8 pTlv)
{
    u32 i=0;
    for(i=0;i<SIZE_ARRAY(g_CardTlv);i++)
    {
        tlv_move_(pTlv,g_pTransBuffer,g_CardTlv[i]);
    }
}

/*----------------------------------------------------------------------------
|   Function Name:
|       Get_callback_type
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static Callback_Type_t Get_callback_type(pu8 pTradingFile)
{
    Callback_Type_t result_type=CALLBACK_TRANS_MAG;
    T_U8_VIEW uvResult=get_tlv_view(pTradingFile,POS_TAG_RES_RESULT);
    #ifdef CFG_DBG
    OsLog(LOG_DEBUG,"RES_RESULT=[%02x]\r\n",uvResult.head[0]);
    #endif
    if(UV_OK(uvResult))
    {
        switch(uvResult.head[0])
        {
            case EMV_PROCESS_MAG_OK:
            case EMV_PROCESS_MAG_ACCOUNT_TYPE:
                result_type=CALLBACK_TRANS_MAG;
                break;
            case EMV_PROCESS_NFC_ONLINE:
                result_type=CALLBACK_TRANS_NFC_ONLINE;
                break;
            case EMV_PROCESS_NFC_APPROVED:
                result_type=CALLBACK_TRANS_NFC_APPROVED;
                break;
            case EMV_PROCESS_NFC_DECLINE:
                result_type=CALLBACK_TRANS_NFC_DECLINE;
                break;
            case EMV_PROCESS_ICC_ONLINE:
                result_type=CALLBACK_TRANS_ICC_ONLINE;
                break;
            case EMV_PROCESS_ICC_GAC2:
                result_type=CALLBACK_TRANS_GAC2;
                break;
            case EMV_PROCESS_ICC_APP_SELECT:
                result_type=CALLBACK_TRANS_APP_SELECT;
                break;
            case EMV_PROCESS_REVERSAL:
                result_type=CALLBACK_TRANS_REVERSAL;
                break;
            case EMV_PROCESS_ONLINE_PIN:
                result_type = CALLBACK_TRANS_ONLINE_PIN;
                break;
            case EMV_PROCESS_OFFLINE_PIN:
                result_type = CALLBACK_TRANS_OFFLINE_PIN;
                break;
            case EMV_PROCESS_OFFLINE_LASTE_PIN:
                result_type = CALLBACK_TRANS_OFFLINE_LASTE_PIN;
                break;
            case EMV_PROCESS_CUSTOM:
                result_type = CALLBACK_TRANS_PROCESS_CUSTOM;
                break;
            default:
                return CALLBACK_END;
                break;
        }
    }
    return result_type;

}

void TransactionProcess(unsigned char *pTransCallbackBuffer)
{
    Rc_t Rc = RC_FAIL;
    u32 events=0;
    pu8 pNode = NULL;
    Callback_Type_t  result_type=Get_callback_type(pTransCallbackBuffer);

    pNode = tlv_find_(pTransCallbackBuffer,POS_TAG_HEAD);
    OsLog(LOG_DEBUG, "%s--pTransCallbackBuffer[%d]:\r\n",__FUNCTION__,tlv_get_l_(pNode));
    TRACE_VALUE(DBG_TRACE_LVL, tlv_get_v_(pNode), tlv_get_l_(pNode));

    #ifdef CFG_DBG
    OsLog(LOG_DEBUG,"transaction callback type=%s-%d\r\n",strCallback[result_type],result_type);
    #endif
    GetCardData(pTransCallbackBuffer);

    switch(result_type)
    {
        case CALLBACK_TRANS_MAG:
            //POS_TAG_RES_PAN POS_TAG_RES_PIN_KSN POS_TAG_RES_PINBLOCK  POS_TAG_RES_SERVICE_CODE
            pNode = tlv_find_(pTransCallbackBuffer,POS_TAG_RES_PAN);
            Rc = online_common(g_pTransBuffer);
            break;
        case CALLBACK_TRANS_ICC_ONLINE:
            //If the encryption mode is DUKPT mode, the encrypted data can be obtained by the following methods,
            //C0 C1 C2 C4 C7
            pNode = tlv_find_(pTransCallbackBuffer,POS_TAG_RES_KN_ONLINE_DATA);
            if(pNode)
            {
                T_U8_VIEW uvOnlineKsn = get_tlv_view(pNode,0xC0);            //online ksn

                T_U8_VIEW uvPinKsn = get_tlv_view(pNode, 0xC1); //pin ksn

                T_U8_VIEW uvOnlineMsg = get_tlv_view(pNode, 0xC2); //online msg

                T_U8_VIEW uvMaskPan = get_tlv_view(pNode, 0xC4); //mask pan 62 30 61 FF FF FF FF 52 84

                T_U8_VIEW uvPinBlock = get_tlv_view(pNode, 0xC7); //pinblock
            }
            //other emv tag
            {
                u8 data[32] = {0};
                u32 len = 0;
                len = Custom_GetEmvTag(0, TAG_EMV_AID_CARD, data);
            }
            
            Rc = online_common(g_pTransBuffer); //card transaction test sample code
            if(Rc==RC_SUCCESS)
            {
                //callback online transaction result to card
                //2nd Gen AC,write response code / issue script back to IC card
                tlv_move_(g_pTransBuffer,pTransCallbackBuffer,POS_TAG_RES_EN_ONLINE_RESULT);
            }
            
            break;
        case CALLBACK_TRANS_NFC_ONLINE:
             Rc = online_common(g_pTransBuffer); //card transaction test sample code
            if(Rc==RC_SUCCESS)
            {
                //callback online transaction result to card
                //2nd Gen AC,write response code / issue script back to IC card
                tlv_move_(g_pTransBuffer,pTransCallbackBuffer,POS_TAG_RES_EN_ONLINE_RESULT);
            }
            break;
        case CALLBACK_TRANS_NFC_APPROVED:
        case CALLBACK_TRANS_NFC_DECLINE:
            Rc=RC_SUCCESS;
            break;
        case CALLBACK_TRANS_GAC2:
            break;
        case CALLBACK_TRANS_APP_SELECT:
            selectMultiApp( pTransCallbackBuffer );
            break;
        case CALLBACK_TRANS_REVERSAL:
            //reversal process
            break;
        default:
            break;
    }
}
void Trans_Payment()
{
    if(g_pTransBuffer)
    {
        lark_free_mem(g_pTransBuffer);
        g_pTransBuffer=NULL;
    }
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Sale");

    g_pTransBuffer=lark_alloc_mem(TRANS_POOL_MAX);
    glb_trans_type = REQ_TRANS_SALE;
    PaymentInit(g_pTransBuffer);

    PR_INT8 Amount[12+1] = {0};
    if(PR_FAILD == TransView_nShowAmountInputView(sizeof(Amount),Amount)){
        return ;
    }
    T_U8_VIEW uvAmt={Amount,Strlen_(Amount)};
    FormatAmount(uvAmt);
    
    StartTrading(g_pTransBuffer);
    // online_common(g_pTransBuffer);
    
    lark_free_mem(g_pTransBuffer);
    g_pTransBuffer = NULL;
    // DispTransResult();
}
int Parse_trans(PR_INT8* data){
    //todo 
    char  response[] = "{\"msg\":\"success\",\"code\":0,\"transResult\":\"approved\",\"mername\":\"dspread\",\"termid\":\"demo\"}";
    dbg_printf("renzp =====parse response====");
    cJSON *json = cJSON_Parse(response);
    if(NULL == json){
        dbg_printf("parse response error!");
        return -1;
    }
    cJSON *transResult = NULL;
    cJSON *json_code = NULL;

    json_code = cJSON_GetObjectItem(json, "code");
    transResult = cJSON_GetObjectItem(json, "transResult");
    dbg_printf("response code %d",json_code->valueint);
    
    dbg_printf("transResult %s",transResult->valuestring);

    cJSON_Delete(json);
    return 0;
}

int packEmvData(PR_INT8* pTransactionPool,PR_INT8 *outString){
    u32 len=0;
    pu8 pNode=NULL;
    char TagValue[256]={0};        
    cJSON * root   =  cJSON_CreateObject();
    //entrymode
    char card_Entrymode=0;

    pNode = tlv_find_(pTransactionPool,0xDF30);
	if(pNode != NULL){
        card_Entrymode = pNode[0];
    }
    
    //pin
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,POS_TAG_RES_PINBLOCK);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"pinBlock",item);
        free(item);
	}
    if(pNode != NULL){
        // ksn
        memset(TagValue,0,sizeof(TagValue));
        pNode = tlv_find_(pTransactionPool,POS_TAG_RES_PIN_KSN);
        if(pNode != NULL)
        {
            len=tlv_get_l_(pNode);
            Memcpy_(TagValue,tlv_get_v_(pNode),len);
            char * item = (char*)malloc(len*2+1);
            memset(item,0,sizeof(len*2+1));
            nBcd2Asc(TagValue,len*2,item,0);
            cJSON_AddStringToObject(root,"ksn",item);
            free(item);
        }
    }

    //ac
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F26);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);		
        cJSON_AddStringToObject(root,"Cryptogram",item);
        free(item);
	}

    // card sn  5f34
     memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x5f34);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1+1);
		memset(item,0,sizeof(len*2+1+1));        
		nBcd2Asc(TagValue,len*2,item+1,0);
        item[0] = 0x30;
        cJSON_AddStringToObject(root,"cardSequenceNumber",item);
        free(item);
	}
    // pan
     memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x5A);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"pan",item);
        free(item);
	}

    // track2 //57
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x57);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"Track2",item);
        free(item);
	}

    // amount 9F02 
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F02);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"AmountAuthorized",item);        
        free(item);
	}

    // cid    9F27
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F27);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"CryptoInfo",item);
        free(item);
	}
    // ran   9F37
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F37);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"UnpredictableNumber",item);
        free(item);
	}    
    // cvm   9F34
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F34);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"CvmResults",item);
        free(item);
	}
    // atc     9F36
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F36);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"atc",item);
        free(item);
	}
    
    // iad     9f10
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x9F10);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"iad",item);
        free(item);
	} 
    // AIP 82
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x82);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"aip",item);
        free(item);
	}
    // TVR 95
    memset(TagValue,0,sizeof(TagValue));
    pNode = tlv_find_(pTransactionPool,0x95);
	if(pNode != NULL)
    {
	    len=tlv_get_l_(pNode);
        Memcpy_(TagValue,tlv_get_v_(pNode),len);
    	char * item = (char*)malloc(len*2+1);
		memset(item,0,sizeof(len*2+1));
		nBcd2Asc(TagValue,len*2,item,0);
        cJSON_AddStringToObject(root,"tvr",item);
        free(item);
	}

    outString = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return 0;
}

int packData(PR_INT8* pTransactionPool,PR_INT8* data){
    //pack emv data
    char * jsondata=NULL;
    // packEmvData(pTransactionPool,jsondata);
    // dbg_printf("jsondata =%s",jsondata);
    sprintf(data,request_post,SERVER_HOST_URI,SERVER_HOST,"{\"hello\":\"Dspread!\"}");
    
    // if(jsondata){
    //     free(jsondata);
    // }
    
    return 0;
}
int Req_trans(char *pTransactionPool){
    PR_INT32 iRet = PR_FAILD;
	PR_INT8 szSendBuf[READBUFF_SIZE + 1] = {0};
	PR_INT8 szRecvBuf[READBUFF_SIZE + 1] = {0};

    PR_INT32 iLen = 0;
	PR_INT32 iSendLen = 0;
	PR_INT32 iRecvLen = 0;
    char temp[64] = {0};
    TermInfo info={0};
    // db_getTermInfo(&info);

    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Packing...");

    packData(pTransactionPool,szSendBuf);

    dbg_printf("request:=%s",szSendBuf);
    iSendLen = strlen(szSendBuf);

    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Send...");	
    iRet = SslTransmit(SERVER_HOST,SERVER_PORT, szSendBuf, iSendLen,szRecvBuf,  READBUFF_SIZE, 60*1000);
    if ( iRet <= 0 ) {
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Communication Failed");
		goto FAIL;
	}
  
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Parse...");
    iRet = Parse_trans(szRecvBuf);
    if(iRet < 0 ){
        tlv_replace_(pTransactionPool, POS_TAG_RES_EN_ONLINE_RESULT, 4, "\x8A\x02\x30\x35");
        goto FAIL;
    }
    //Simulate successful backend return 00
    
    tlv_replace_(pTransactionPool, POS_TAG_RES_EN_ONLINE_RESULT, 4, "\x8A\x02\x30\x30");
     
    return PR_NORMAL;
    FAIL:
        KB_nWaitKeyMS(5*1000);
        return PR_FAILD;
}



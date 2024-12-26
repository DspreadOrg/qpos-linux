
#include "app_pkt.h"
#include "app_server.h"
#include "app_http.h"
#include "cJSON.h"

Req_Trans_Type glb_trans_type = REQ_INIT;

ONLINE_STATUS onlineStatus;

static const int TAG_8A_LENGTH = 4;
static char* RC_ONLINE_APPROVE = "\x8A\x02\x30\x30";
static char* RC_ONLINE_DECLINE = "\x8A\x02\x30\x35";

T_TRANS_URL glb_trans_url[] =
{
    {REQ_INIT,                  NULL},
    {REQ_LOGIN,                 URL_TRANS_LOGIN},
    {REQ_TRANS_SALE,            URL_TRANS_SALE},
    {REQ_TRANS_REFUND,          URL_TRANS_REFUND},
    {REQ_TRANS_REVERSEL,        URL_TRANS_REVERSEL},
    {REQ_TRANS_VOID,            URL_TRANS_VOID},
};

const u32 arrTagDE55[] = {0x5F2A, 0x5F34, 0x71, 0x72, 0x82, 0x84, 0x91, 0x95, 0x9A, 0x9C, 0x9F02, 0x9F03, 0x9F09, 0x9F10,
							0x9F1A, 0x9F1E, 0x9F26, 0x9F27, 0x9F33, 0x9F34, 0x9F35, 0x9F36, 0x9F37, 0x9F41, 0x9F53,0x9F6E};
const u32 SIZE_TAG_DE55 = SIZE_ARRAY(arrTagDE55);

int get_json_data(char * strMsg, const char * strKey, char * pValue, unsigned int ValueLenMax)
{
    int Cnt = 0;
    char * pStr;

    pStr = strstr(strMsg, strKey);
    if(pStr)
    {
        pStr += strlen(strKey);
        pStr += 2;
        if(*pStr == '\"')
        {
            pStr++;
            while(*pStr != '\"')
            {
                if(*pStr != '\\')
                {
                    pValue[Cnt++] = *pStr;
                }
                else
                {
                    pStr++;
                    pValue[Cnt++] = *pStr;
                }
                pStr++;
            }
        }
        else
        {
            while(*pStr != ',' && *pStr != '}')
            {
                if(*pStr != '\\')
                {
                    pValue[Cnt++] = *pStr;

                }
                else
                {
                    pStr++;
                    pValue[Cnt++] = *pStr;

                }
                pStr++;
            }
        }
    }
    else
    {
        Cnt = 0;
    }
#ifdef CFG_DBG
    OsLog(LOG_DEBUG,"key:%s  value:%s\r\n",strKey,pValue);
#endif
    return Cnt;
}

u32 get_pan(pu8 track2dat,u32 track2length,pu8 cardNo)
{
    	u16 i=0x00;

	while(track2dat[i] != 0x3d){
		if(i<track2length){
			cardNo[i]=track2dat[i];
 			i++;
		}
		else{
			break;
		}
	}
	return i;

}

u32 SetEmvDataRequest(u8 *pTlvSrc)
{
	u8 *pTlvDe55 = malloc(256);
	if(pTlvDe55 == NULL)
	    return 0;
	memset(pTlvDe55,0,256);
	tlv_new_(pTlvDe55, 0xFF00, 0, NULL);
	u8 i;
	for(i = 0; i < SIZE_TAG_DE55; i++)
	{
		T_U8_VIEW uvTag = get_tlv_view(pTlvSrc, arrTagDE55[i]);
		if(UV_OK(uvTag))
			set_tlv_view(pTlvDe55, arrTagDE55[i], uvTag);
		else
			OsLog(LOG_DEBUG, " TAG %X NOT found in %s\r\n", arrTagDE55[i], __FUNCTION__);
	}

	T_U8_VIEW uvDe55 = {tlv_get_v_(pTlvDe55), tlv_get_l_(pTlvDe55)};
    u8 *tempbuff = malloc(512);
	memset(tempbuff,0,512);

    bcd_to_asc(uvDe55.head,0,uvDe55.len*2,tempbuff);

    //OsLog(LOG_DEBUG, " EMV DATA: %s\r\n", tempbuff);

    tlv_replace_(pTlvSrc, TAG_EMV_DATA, uvDe55.len*2,tempbuff);
	free(pTlvDe55);
    free(tempbuff);
    pTlvDe55 =NULL;
    tempbuff = NULL;

	return uvDe55.len*2;
}

int setSaleJsonBody(pu8 pTradingFile,pu8 pbody)
{
    u32 length = 0,i = 0,tmpLen = 0;
    pu8 tempBuffer = NULL;
    cJSON *jsonbody = NULL;
    tempBuffer = malloc(512);
    memset(tempBuffer,0,512);
    pu8 pNode = NULL;

    do
    {
        jsonbody = cJSON_CreateObject();
        if(!jsonbody)
        {
           length = 0;
           break;
        }

        pNode = tlv_find_(pTradingFile, POS_TAG_CLEAR_TRACK2);
        if(pNode)
        {
            memset(tempBuffer,0,512);
            tmpLen = get_pan(tlv_get_v_(pNode), tlv_get_l_(pNode),tempBuffer);
            tlv_replace_(pTradingFile,TAG_PAN,tmpLen,tempBuffer);
            cJSON_AddItemToObject(jsonbody, "pan", cJSON_CreateString(tempBuffer));
        }


        pNode = tlv_find_(pTradingFile, POS_TAG_RES_PINBLOCK);
        if(pNode)
        {
            memset(tempBuffer,0,512);
            T_U8_VIEW PIN = get_tlv_view( pTradingFile, POS_TAG_RES_PINBLOCK);
            tmpLen = bcd_to_asc(PIN.head,0, 16, tempBuffer);
            tlv_replace_(pTradingFile,TAG_PIN_BLOCK,16,tempBuffer);
            cJSON_AddItemToObject(jsonbody, "pinblock", cJSON_CreateString(tempBuffer));
        }

        SetEmvDataRequest(pTradingFile);
        pNode = tlv_find_(pTradingFile, TAG_EMV_DATA);
        if(pNode)
        {
            memset(tempBuffer,0,512);
            T_U8_VIEW emvData = get_tlv_view( pTradingFile, TAG_EMV_DATA);
            tmpLen = bcd_to_asc(emvData.head,0, emvData.len*2, tempBuffer);
            cJSON_AddItemToObject(jsonbody, "emv", cJSON_CreateString(tempBuffer));
        }
        pbody=cJSON_Print(jsonbody);
        length = strlen(pbody);
        /* code */
    } while (0);
    
    if(jsonbody)
    {
        cJSON_Delete(jsonbody);
        jsonbody = NULL;
    }

   if(tempBuffer)
   {
    free(tempBuffer);
    tempBuffer = NULL;
   }
    return length;
}

void setEmvResponse(pu8 pTradingFile,pu8 tag8A)
{
    u8 emvCompress[128] = { 0 };
    pu8 pNode = NULL;
    pu8 data = NULL;
    u32 dataLen = 0;
    u32 lenEmvCompress = 0;

    data = malloc(256);
    memset(data,0,256);
    pNode = tlv_find_(pTradingFile,TAG_EMV_RESPONSE);
    memcpy(data,tag8A,TAG_8A_LENGTH);
    dataLen += TAG_8A_LENGTH;
    if(pNode)
    {
        lenEmvCompress = asc_to_bcd( tlv_get_v_(pNode),tlv_get_l_(pNode), emvCompress );
        memcpy(data+dataLen,emvCompress,lenEmvCompress);
        dataLen += lenEmvCompress;
    }
    tlv_replace_( pTradingFile, POS_TAG_RES_EN_ONLINE_RESULT, dataLen, data );
    free(data);
}

Rc_t sale_online_request(pu8 pTradingFile)
{
    Rc_t rc = RC_FAIL;
    pu8 body=NULL;
    pu8 pSend=NULL;
    pu8 pRecv=NULL;
    s32    RecvLen=0;
    u32    HeadLen=0;
    u32    bodyLen = 0;
    DspDebug();
    u8 url[128] = {0};
    u32 urlLen = 0;

    body=(pu8)malloc(SERVER_REQUEST_BODY_LENGTH);
    pSend=(pu8)malloc(SERVER_REQUEST_LENGTH);
    memset(body,0,SERVER_REQUEST_BODY_LENGTH);
    memset(pSend,0,SERVER_REQUEST_LENGTH);

    bodyLen = setSaleJsonBody(pTradingFile,body);

    RequestPackHttpFrame(body,URL_TRANS_SALE,"POST",pSend,NULL,SERVER_REQUEST_LENGTH);
    free(body);

    pRecv=(pu8)malloc(SERVER_RECV_LENGTH);
    memset(pRecv,0,SERVER_RECV_LENGTH);
    RecvLen=RequestHttpTransmitNew(pSend,strlen(pSend),URL_TRANS_SALE,URL_SERVER_PORT,pRecv,SERVER_RECV_LENGTH);
    if(RecvLen>0)
    {
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"request success\r\n");
        #endif
        HeadLen=GetHttpHeadFramLen((char*)pRecv);
        T_U8_VIEW Http_head={pRecv,HeadLen};
        T_U8_VIEW Http_Response={pRecv+HeadLen,RecvLen-HeadLen};

        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"Http_head(%d)\r\n",Http_head.len);
        TRACE_ASCII(DBG_TRACE_LVL,Http_head.head,Http_head.len);
        OsLog(LOG_DEBUG,"Response(%d)\r\n",Http_Response.len);
        TRACE_ASCII(DBG_TRACE_LVL,Http_Response.head,Http_Response.len);
        #endif

 #if 1              // just a demo
                    setEmvResponse(pTradingFile,RC_ONLINE_APPROVE);
                    rc = RC_SUCCESS;
                    goto exit;
 #else  
        if(HttpLineStatusCheck(pTradingFile,Http_head.head)==RC_SUCCESS)
        {
            if(Http_Response.len > 0)
            {
             
                if(UpdateKeyDataToTLV_AndCompare(Http_Response.head,"gatewayResponseCode",TAG_GATEWAY_RESPONSE_CODE,pTradingFile,"00")==RC_SUCCESS)
                {
                    UpdateKeyDataToTLV(Http_Response.head,"emvResonse",TAG_EMV_RESPONSE,pTradingFile);
                    setEmvResponse(pTradingFile,RC_ONLINE_APPROVE);
                    rc = RC_SUCCESS;
                    goto exit;
                }
                else
                {
                    setEmvResponse(pTradingFile,RC_ONLINE_DECLINE);
                }              
            }
        }
        #endif
    }
    else
    {
        setEmvResponse(pTradingFile,RC_ONLINE_DECLINE);
    }
    
exit:    
    free(pSend);
    free(pRecv);
    onlineStatus.Rc = rc;
    return rc;
}


Rc_t online_common(pu8 pTradingFile)
{
    Rc_t rc = RC_FAIL;
    if(glb_trans_type == REQ_TRANS_SALE)
        rc = sale_online_request(pTradingFile);
    return rc;
}
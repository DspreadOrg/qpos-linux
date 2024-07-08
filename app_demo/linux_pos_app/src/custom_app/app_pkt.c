
#include "app_pkt.h"
#include "app_server.h"
#include "app_http.h"

Req_Trans_Type glb_trans_type = REQ_INIT;

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


static const App_Tag_t sale_tag[] = {TAG_TRANS_TOTAL_AMOUNT,TAG_PAN,TAG_PIN_BLOCK,TAG_PIN_KSN,TAG_EMV_DATA,POS_TAG_CLEAR_TRACK2};
static const u32 SIZE_TAG_SALE = SIZE_ARRAY(sale_tag);

static const T_TRANS_TAG_JSON_DATA Tag_to_JSON_Data[] =
{
    {TAG_START,                 NULL,                   0,      STRINGS },
    {TAG_TRANS_TOTAL_AMOUNT,    "totalAmount",          11,     NUMBER  },
    {POS_TAG_CLEAR_TRACK2,      "track2",               6,      STRINGS },
    {TAG_PAN,                   "pan",                  3,      STRINGS },
    {TAG_EMV_DATA,              "emvDataRequest",       14,     STRINGS },
    {TAG_PIN_BLOCK,             "pinBlock",             8,      STRINGS },
    {TAG_PIN_KSN,               "ksn",                  3,      STRINGS },
    {TAG_END,                   NULL,                   0,      STRINGS }
};

const u32 SIZE_TAG_JSON_DATA = SIZE_ARRAY(Tag_to_JSON_Data);


int get_json_data(char * strMsg, const char * strKey, char * pValue, unsigned int ValueLenMax)
{
    int Cnt = 0;
    char * pStr;

    pStr = Strstr_(strMsg, strKey);
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

    OsLog(LOG_DEBUG,"key:%s  value:%s\r\n",strKey,pValue);

    return Cnt;
}

u32 set_json_data(char * Key,u32 keyLen,pu8 keyValue,u32 length,pu8 OutData,T_JSON_VALUE_MODE mode)
{
    u32 OutDataLen = 0;

    OsLog(LOG_DEBUG,"%s<%d>:\t",Key,length);
    TRACE_ASCII(DBG_TRACE_LVL,keyValue,length);

    if(length == 0)
        return 0;

    memcpy(&OutData[OutDataLen],"\"",1);
    OutDataLen += 1;
    memcpy(&OutData[OutDataLen],Key,keyLen);
    OutDataLen += keyLen;
    memcpy(&OutData[OutDataLen],"\"",1);
    OutDataLen += 1;
    OutData[OutDataLen++] = ':';
    if(mode == STRINGS)
    {
        memcpy(&OutData[OutDataLen],"\"",1);
        OutDataLen += 1;
        memcpy(&OutData[OutDataLen],keyValue,length);
        OutDataLen += length;
        memcpy(&OutData[OutDataLen],"\"",1);
        OutDataLen += 1;
    }
    else
    {
        memcpy(&OutData[OutDataLen],keyValue,length);
        OutDataLen += length;
    }

    OutData[OutDataLen++] = ',';

    return OutDataLen;

}

u32 format_json_add_braces(pu8 strmsg,u32 length)
{
    strmsg[0] = '{';
    strmsg[length-1] ='}';//把最后一个逗号替换
    return length;
}

u32 packTLVtoJSON(pu8 pTradingFile,pu8 strmsg, App_Tag_t tag)
{
    pu8 pNode = NULL;
    u32 length = 0,i;

    pNode = tlv_find_(pTradingFile,tag);
    if(pNode)
    {
        for(i=0;i<SIZE_TAG_JSON_DATA;i++)
        {
            if(tag == Tag_to_JSON_Data[i].tag)
            {
                length += set_json_data(Tag_to_JSON_Data[i].jsonKeyName, Tag_to_JSON_Data[i].keyNameLen, tlv_get_v_(pNode), tlv_get_l_(pNode), strmsg, Tag_to_JSON_Data[i].mode);
                break;
            }
        }
    }

    return length;
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

int setSaleJsonBody(pu8 pTradingFile,pu8 body)
{
    u32 length = 0,i = 0,tmpLen = 0;
    pu8 tempBuffer = NULL;
    tempBuffer = malloc(512);
    memset(tempBuffer,0,512);
    pu8 pNode = NULL;

    pNode = tlv_find_(pTradingFile, POS_TAG_CLEAR_TRACK2);
    if(pNode)
    {
        memset(tempBuffer,0,512);
        tmpLen = get_pan(tlv_get_v_(pNode), tlv_get_l_(pNode),tempBuffer);
        tlv_replace_(pTradingFile,TAG_PAN,tmpLen,tempBuffer);
    }


    pNode = tlv_find_(pTradingFile, POS_TAG_RES_PINBLOCK);
    if(pNode)
    {
        memset(tempBuffer,0,512);
        T_U8_VIEW PIN = get_tlv_view( pTradingFile, POS_TAG_RES_PINBLOCK);
        tmpLen = bcd_to_asc(PIN.head,0, 16, tempBuffer);
        tlv_replace_(pTradingFile,TAG_PIN_BLOCK,16,tempBuffer);
    }

    length ++;// 空出第一个字节，后面放大括号
    if(glb_trans_type == REQ_TRANS_SALE)
    {
        for(i = 0;i< SIZE_TAG_SALE;i++)
        {
            length += packTLVtoJSON(pTradingFile, &body[length], sale_tag[i]);
        }
    }

    length = format_json_add_braces(body, length);
    free(tempBuffer);
    tempBuffer = NULL;

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
    RecvLen=RequestHttpTransmit(pSend,strlen(pSend),url,URL_SERVER_PORT,pRecv,SERVER_RECV_LENGTH);
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
            }
        }
    }
    setEmvResponse(pTradingFile,RC_ONLINE_DECLINE);
exit:    
    free(pSend);
    free(pRecv);
    return rc;
}


Rc_t online_common(pu8 pTradingFile)
{
    Rc_t rc = RC_FAIL;
    Req_trans(pTradingFile);
    return rc;
}




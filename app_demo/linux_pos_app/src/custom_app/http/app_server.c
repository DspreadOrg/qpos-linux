/*
********************************************************************************
*
*   File Name:
*       Req_Server.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "app_server.h"

#define URL_TEST    "http://spoc.dspread.com:8084/port/test"

/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Functions
+----------------------------------------------------------------------------*/
/*--------------------------------------
|   Function Name:
|       RequestPackHttpFrame
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
Rc_t RequestPackHttpFrame(char * strMsg, char * pURL, char * pMethod,char * pFrame,char * pRange, u32 FrameLengthMax )
{
    HttpPack_t HttpPack;

    memset((pu8)(&HttpPack), 0x00, sizeof(HttpPack));
    OsLog(LOG_DEBUG, "\r\nRequest_URL: %s\r\n\r\n",pURL);
    HttpPack.pRequestMessage = strMsg;
    HttpPack.pRequestBody = pFrame;
    HttpPack.Line.pMethod = malloc(32);
    HttpPack.Line.pUrl = malloc(256);
    HttpPack.Line.pVersion = malloc(32);
    HttpPack.Head.pConnection = malloc(32);
    HttpPack.Head.pHost = malloc(128);
    HttpPack.Head.pContentType = malloc(64);
    if(pRange!=NULL)
    {
        HttpPack.Head.pRange = malloc(64);
        memset(HttpPack.Head.pRange, 0x00, 64);
    }

    memset(HttpPack.pRequestBody, 0x00, FrameLengthMax);
    memset(HttpPack.Line.pMethod, 0x00, 32);
    memset(HttpPack.Line.pUrl, 0x00, 256);
    memset(HttpPack.Line.pVersion, 0x00, 32);
    memset(HttpPack.Head.pConnection, 0x00, 32);
    memset(HttpPack.Head.pHost, 0x00, 128);
    memset(HttpPack.Head.pContentType, 0x00, 64);


    strcat(HttpPack.Line.pMethod, pMethod);

    HttpUrlGetPath(pURL, HttpPack.Line.pUrl, 256);

    strcat(HttpPack.Line.pVersion, "HTTP/1.1");//HTTP/1.1

    strcat(HttpPack.Head.pConnection, "Keep-Alive");

    HttpUrlGetHost(pURL, HttpPack.Head.pHost, 128);

    if(pRange!=NULL)
    {
        strcat(HttpPack.Head.pRange, pRange);
    }

    strcat(HttpPack.Head.pContentType, "application/json");//application/json

    HttpFramePack(&HttpPack);

    free(HttpPack.Line.pMethod);
    free(HttpPack.Line.pUrl);
    free(HttpPack.Line.pVersion);
    free(HttpPack.Head.pConnection);
    free(HttpPack.Head.pHost);
    free(HttpPack.Head.pContentType);
    if(HttpPack.Head.pRange!=NULL)
    {
        free(HttpPack.Head.pRange);
    }

    return RC_SUCCESS;
}
/*--------------------------------------
|   Function Name:
|       GetHttpHeadFramLen
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
u32 GetHttpHeadFramLen(const char *strMsg)
{
    int Cnt = 0;
    char* pStr=NULL;
    if(strMsg==NULL)
    {
        return Cnt;
    }
    pStr = strstr(strMsg, "\r\n\r\n");
    if(pStr)
    {
        pStr += 4;
        Cnt=pStr-strMsg;
    }
    else
    {
        Cnt = 0;
    }
    pStr=NULL;
    return Cnt;
}
/*--------------------------------------
|   Function Name:
|       GetHttpContentFram
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static u32 GetHttpContentFram(const char *strMsg,s32 FramLen)
{
    s32 Cnt = 0;
    char* pStr=NULL;

    if(strMsg==NULL || FramLen==0)
    {
        return Cnt;
    }
    pStr = strstr(strMsg, "\r\n\r\n");
    if(pStr)
    {
        pStr += 4;

        Cnt=pStr-strMsg;
    }
    else
    {
        Cnt = 0;
    }
    return FramLen-Cnt;
}
/*--------------------------------------
|   Function Name:
|       RequestHttpMessageCheck
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static bool RequestHttpMessageCheck(char * pHttpBody,u32 ReceiveLen)
{
    HttpUnpack_t HttpUnpack;
    u32 ContentLen = 0;
    u32 HeadLen=0;
    bool res=false;
    u8 ContentLength[32] = {0};
    u8 Connection[32] = {0};
    if(pHttpBody==NULL || ReceiveLen==0)
    {
        return true;
    }

    if(!strstr(pHttpBody,"\r\n\r\n"))
    {
        return false;
    }

    memset((pu8)(&HttpUnpack), 0x00, sizeof(HttpUnpack));
    HttpUnpack.pResponseBody = pHttpBody;

    HttpUnpack.Head.pContentLength = ContentLength;
    HttpUnpack.Head.pConnection = Connection;
    memset(HttpUnpack.Head.pContentLength, 0x00,32);
    memset(HttpUnpack.Head.pConnection, 0x00,32);
    HttpFrameUnpack(&HttpUnpack);

    ContentLen = atoi(HttpUnpack.Head.pContentLength);
    if(ContentLen!=0)
    {
        HeadLen=GetHttpHeadFramLen(HttpUnpack.pResponseBody);
        #if defined(CFG_OTA_DEBUG)
        OsLog(LOG_DEBUG,"\r\n%s--ReceiveLen=%d contentlen=%d  HeadLen=%d\r\n",__FUNCTION__,ReceiveLen,ContentLen,HeadLen);
        #endif
        if(ContentLen+HeadLen==ReceiveLen)
        {
            res= true;
        }
    }
    else if(strcmp(HttpUnpack.Head.pConnection,"close"))
    {
        res=true;
    }
    return res;
}

/*--------------------------------------
|   Function Name:
|       HttpLineStatusCheck
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
Rc_t HttpLineStatusCheck(pu8 pTradingFile, const char * pHttpBody)
{
    Rc_t Rc = RC_FAIL;
    HttpUnpack_t HttpUnpack;
    memset((pu8)(&HttpUnpack), 0x00, sizeof(HttpUnpack));
    HttpUnpack.pResponseBody = (char * )pHttpBody;

    HttpUnpack.Line.pStatus = malloc(64);
    memset(HttpUnpack.Line.pStatus, 0x00, 64);

    HttpFrameUnpack(&HttpUnpack);
    Rc=((strcmp(HttpUnpack.Line.pStatus,"200")|| strcmp(HttpUnpack.Line.pStatus,"206"))?RC_SUCCESS:RC_FAIL);
    free(HttpUnpack.Line.pStatus);
    return Rc;
}

/*--------------------------------------
|   Function Name:
|       RequestHttpTransmit
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
s32 RequestHttpTransmit(char * pSend,u32 SendLen, char * pURL,u32 port, char * pReceived, u32 FrameLengthMax )
{
    char * pScheme = NULL;
    char * pHost = NULL;
    s32    Length = 0;
    if(pSend == NULL || pURL == NULL || FrameLengthMax == 0)
    {
        return 0;
    }

    OsLog(LOG_DEBUG, "\r\npSend(%d) : \r\n%s\r\n\r\n",SendLen,pSend);
    //TRACE_VALUE(DBG_TRACE_LVL,pSend,SendLen);

    pScheme = malloc(128);
    pHost = malloc(64);
    memset(pScheme, 0x00, 128);
    memset(pHost, 0x00,64);
    HttpUrlGetScheme(pURL, pScheme, 128);
    HttpUrlGetHost(pURL, pHost, 64);

    Length = SslTransmit((pu8)pHost, port, pSend, SendLen, pReceived, FrameLengthMax, ONLINE_REQUEST_TIMEOUT);
RequestHttpTransmit_end:
    if(pScheme!=NULL)
    {
        memset(pScheme, 0x00, 128);
        free(pScheme);
        pScheme=NULL;
    }
    if(pHost!=NULL)
    {
        memset(pHost, 0x00,64);
        free(pHost);
        pHost=NULL;
    }

    OsLog(LOG_DEBUG, "\r\nRECV(%d) : \r\n",Length);
    return Length;
}

/*--------------------------------------
|   Function Name:
|       ServerReques_test
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
Rc_t ServerReques_test(pu8 pTransPool)
{
    Rc_t Rc=RC_SUCCESS;

    char * strmsg=NULL;
    char * pRecv=NULL;
    s32    RecvLen=0;
    u32    HeadLen=0;

    strmsg=malloc(512);
    pRecv=malloc(1024);
    //DispHttpRequst();
    RequestPackHttpFrame(NULL,URL_TEST,"POST",strmsg,NULL,512);
    RecvLen=RequestHttpTransmit(strmsg,strlen(strmsg),URL_TEST,URL_SERVER_PORT,pRecv,1024);
    free(strmsg);
    #ifdef CFG_DBG
    OsLog(LOG_DEBUG,"recv(%d)\r\n",RecvLen);
    #endif
    if(RecvLen>0)
    {
        TRACE_VALUE(DBG_TRACE_LVL,pRecv,RecvLen);
        HeadLen=GetHttpHeadFramLen((char*)pRecv);
        T_U8_VIEW Http_head={pRecv,HeadLen};
        T_U8_VIEW Http_Response={pRecv+HeadLen,RecvLen-HeadLen};
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"Http_head(%d)\r\n",Http_head.len);
        TRACE_ASCII(DBG_TRACE_LVL,Http_head.head,Http_head.len);
        OsLog(LOG_DEBUG,"Response(%d)\r\n",Http_Response.len);
        TRACE_VALUE(DBG_TRACE_LVL,Http_Response.head,Http_Response.len);
        #endif

        if(HttpLineStatusCheck(NULL,Http_head.head)==RC_SUCCESS || memcmp(Http_Response.head,"hello",5)==0)
        {
            Rc=RC_SUCCESS;
            #ifdef CFG_DBG
            OsLog(LOG_DEBUG,"request success\r\n");
            #endif
            tlv_replace_(pTransPool,POS_TAG_RES_EN_ONLINE_RESULT, 4, (pu8)"\x8A\x02\x30\x30");
        }
        else
        {
            tlv_replace_(pTransPool,POS_TAG_RES_EN_ONLINE_RESULT, 4, (pu8)"\x8A\x02\x30\x35");
        }
    }
    else
    {
        tlv_replace_(pTransPool,POS_TAG_RES_EN_ONLINE_RESULT, 4, (pu8)"\x8A\x02\x30\x35");
        Rc=RC_FAIL;
    }
    free(pRecv);
    return Rc;
}
/*--------------------------------------
|   Function Name:
|       checkRepo
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static Rc_t checkRepo(pu8 pValue)
{
    return (strstr(pValue,"path")?RC_SUCCESS:RC_FAIL);
}

/*--------------------------------------
|   Function Name:
|       KeyDataCompare
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
Rc_t KeyDataCompare(char * strMsg, char * strKey,const char * strCompareValue)
{
    char * pTempBuffer = NULL;

    pTempBuffer = malloc(256);

    memset(pTempBuffer, 0x00, 256);

    HttpGetStrKey(strMsg, strKey, pTempBuffer, 256);

    if(strlen(pTempBuffer) == 0)
    {
        free(pTempBuffer);
        return RC_FAIL;
    }

    if(strcmp(strCompareValue, pTempBuffer) != 0)
    {
        free(pTempBuffer);
        return RC_FAIL;
    }
    else
    {
        free(pTempBuffer);
        return RC_SUCCESS;
    }
}

/*--------------------------------------
|   Function Name:
|       UpdateKeyDataToTLV
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
Rc_t UpdateKeyDataToTLV(char * strMsg, char * strKey, u32 Tag, pu8 pTradingFile )
{
    char * pTempBuffer = NULL;

    pTempBuffer = malloc(256);

    memset(pTempBuffer, 0x00, 256);

    HttpGetStrKey(strMsg, strKey, pTempBuffer, 256);

    if(strlen(pTempBuffer) == 0)
    {
        free(pTempBuffer);
        return RC_FAIL;
    }
    else
    {
        T_U8_VIEW uvValue={pTempBuffer,strlen(pTempBuffer)};
        //tlv_replace_(pTradingFile, Tag, strlen(pTempBuffer), (pu8)pTempBuffer);
        if(pTradingFile!=NULL)
        {
            set_tlv_view(pTradingFile,Tag, uvValue);
        }

        free(pTempBuffer);
        return RC_SUCCESS;
    }
}

/*--------------------------------------
|   Function Name:
|       UpdateKeyDataToTLV_AndCompare
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
Rc_t UpdateKeyDataToTLV_AndCompare(char * strMsg, char * strKey, u32 Tag, pu8 pTradingFile, char * strCompareValue )
{
    char * pTempBuffer = NULL;

    pTempBuffer = malloc(256);

    memset(pTempBuffer, 0x00, 256);

    HttpGetStrKey(strMsg, strKey, pTempBuffer, 256);

    if(strlen(pTempBuffer) == 0)
    {
        free(pTempBuffer);
        return RC_FAIL;
    }
    else
    {
        tlv_replace_(pTradingFile, Tag, strlen(pTempBuffer), (pu8)pTempBuffer);
    }

    if(strcmp(strCompareValue, pTempBuffer) != 0)
    {
        free(pTempBuffer);
        return RC_FAIL;
    }
    else
    {
        free(pTempBuffer);
        return RC_SUCCESS;
    }
}


s32 RequestHttpTransmitNew(char * pSend,u32 SendLen, char * pURL,u32 port, char * pReceived, u32 FrameLengthMax )
{
    char * pHost = NULL;
    s32    Length = 0;
    if(pSend == NULL || pURL == NULL || FrameLengthMax == 0)
    {
        return 0;
    }

    OsLog(LOG_DEBUG, "\r\npSend(%d) : \r\n%s\r\n\r\n",SendLen,pSend);
    pHost = malloc(64);
    memset(pHost, 0x00,64);

    HttpUrlGetHost(pURL, pHost, 64);

    if(HttpServerConnect(pHost,443,ONLINE_REQUEST_TIMEOUT) != RC_SUCCESS)
       return -1;

    Length = SslTransmit((pu8)pHost, port, pSend, SendLen, pReceived, FrameLengthMax, ONLINE_REQUEST_TIMEOUT);
RequestHttpTransmit_end:

    if(pHost!=NULL)
    {
        memset(pHost, 0x00,64);
        free(pHost);
        pHost=NULL;
    }
    HttpServerDisConnect();
    OsLog(LOG_DEBUG, "\r\nRECV(%d) : \r\n",Length);
    return Length;
}
#include "ota.h"

#define COM_WIFI 0
#define COM_CELLULAR 1

#define TMS_FW_HEART_CUSTOM_URL               "https://www.dspreadser.net:9011/terminal"  
static bool gUpdateFlag = false;
void ShowUpdateMsgBox()
{
    if(gUpdateFlag)
    {
        if (RET_DPORT_OK == TransView_nDoMessageBox((char *)"OTA UPDATE", BOX_STYLE_OK | BOX_STYLE_CANCEL, VIEW_TIMEOUT_FOREVER, (char *)"New Version,Update?"))
        {
            Ota_Process();
        }
        else
        {
            gUpdateFlag = false;
        }
    }
}
void TmsDispCallback(u32 id, char *pMsg)
{
    u32 i ;
    OsLog(LOG_DEBUG,"Dspread: %s:id=%d",__FUNCTION__,id);
    switch(id)
    {
        case TMS_DISP_START_CHECK:
            TransView_vClearPort();
		    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"TMS Checking...");
            break;
        case TMS_DISP_DOWNLOADING_PROGRESS:
            TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,pMsg);
            break;
        case TMS_DISP_START_DOWNLOAD:
            TransView_vClearPort();
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"TMS Download...");
            break;
        case TMS_DISP_UPGRADING:
            TransView_vClearPort();
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Upgrading...");
        break;
        case TMS_DISP_HAVE_UPDATE_TASE:
            gUpdateFlag = true;
            break;
        default:
            break;
    }

}

void larktms_init()
{
    static LarkTmsCallBack_t larktmsCbk;
    larktmsCbk.ssl_connect = ssl_server_connect;
    larktmsCbk.ssl_disconnect= ssl_server_disconnect;
    larktmsCbk.ssl_send = ssl_send_msg;
    larktmsCbk.ssl_recv = ssl_recv_msg;

    larktms_service_start(&larktmsCbk,TmsDispCallback,TMS_FW_HEART_CUSTOM_URL,APP_VERSION);
}

int Ota_Process()
{
    int result = -1,key;
    unsigned char dispMsg[64]= {0};

    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"OTA");

    result = larktms_client_check(TMS_FW_HEART_CUSTOM_URL,APP_VERSION);

    switch(result)
    {
        case TMS_ERR_OK:
            strcpy(dispMsg,"Update now?");
            break;
        case TMS_ERR_CONNECT:
            strcpy(dispMsg,"network error");
            break;
        case TMS_ERR_NO_AVAIALABLE:
            strcpy(dispMsg,"No updated tasks");
            break;
        case TMS_ERR_SN_NOT_REGISTER:
            strcpy(dispMsg,"Device not registered");
            break;
        default:
            strcpy(dispMsg,"Check failed");
            break;    
    }
    key = TransView_nDoMessageBox((char*)"Warning",BOX_STYLE_OK|BOX_STYLE_CANCEL,VIEW_TIMEOUT_FOREVER,(char*)dispMsg);

    if(key == RET_DPORT_OK && result == TMS_ERR_OK)
    {
        result = larktms_client(TMS_FW_HEART_CUSTOM_URL,APP_VERSION);
        if(result  != TMS_ERR_OK)
        {
            TransView_nDoMessageBox((char*)"Warning",BOX_STYLE_OK|BOX_STYLE_CANCEL,VIEW_TIMEOUT_FOREVER,(char*)"Upgrade Fail");
        }
    }

    return 0;
}

int Ota_Wifi(void)
{
    PR_INT32 ret, rssi, num = 0, nIndexOfSelect = 0xFF;    
    ST_WifiApInfo *Aps=NULL;
    ST_WifiApSet Select_Ap;
    PR_INT8 **pszList = NULL;
    PR_INT8 ssid[256] = { 0 };
    PR_INT8 bssid[20] = { 0 };
    PR_INT8 szpwd[64] = { 0 };
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER, (char *)"OTA Test");
    OsLog(LOG_ERROR,"Start Ota_Wifi ");
    ret = OsWifiCheck(ssid, bssid, &rssi);
    OsLog(LOG_ERROR,"OsWifiCheck ret = %d", ret);
    if (ret != RET_OK)
    {
        OsLog(LOG_ERROR,"Ota_Wifi need Open");
        TransView_vShowLine(2, EM_DTYPE_NORMAL, EM_ALIGN_CENTER, (char *)"Connect Wifi");
        OsWifiClose();

        ret = OsWifiOpen();
        if (ret != RET_OK)
        {
            goto EXIT;
        }

        num = OsWifiScan(&Aps);
        if (num <= 0)
        {
            TransView_vShowLine(2, EM_DTYPE_NORMAL, EM_ALIGN_CENTER, (char *)"No Available Hotspots");
            goto EXIT;       
        }

// OsWifiCheck(ssid,bssid,&rssi);
        pszList = ConvertList(num, Aps, ssid);
        if (pszList == NULL)
        {
            TransView_vShowLine(2, EM_DTYPE_NORMAL, EM_ALIGN_CENTER, (char *)"Get WIFI List Failed");
            goto EXIT;
        }
        TransView_vClearPort();
        ret = TransView_nShowListViewEx((char *)"WIFI", pszList, num, 30, &nIndexOfSelect);
        if (ret != PR_NORMAL)
        {
            TransView_vClearAllScreen();
            TransView_vShowLine(2, EM_DTYPE_NORMAL, EM_ALIGN_CENTER, (char *)"Cancel");
            goto EXIT;  
        }
        memset(&Select_Ap,0x0,sizeof(ST_WifiApSet));
        memcpy(Select_Ap.Essid,Aps[nIndexOfSelect].Essid,strlen(Aps[nIndexOfSelect].Essid));
        Select_Ap.AuthMode = Aps[nIndexOfSelect].AuthMode;
        Select_Ap.SecMode = Aps[nIndexOfSelect].SecMode;
        OsLog(LOG_ERROR,"WIFI名称 =  %s", Select_Ap.Essid);
        TransView_vClearLine(2);
        ret = TransView_nNumber_En_Symbol_InputViewEx((char*)"Pls Enter Password",32,8,sizeof(szpwd),szpwd);
        if(ret < PR_NORMAL){
            TransView_vClearPort();
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Cancel");
            goto EXIT;            
        }
        
        memcpy(Select_Ap.KeyUnion.PskKey.Key,szpwd,strlen(szpwd));
        Select_Ap.KeyUnion.PskKey.KeyLen = strlen(szpwd);
        TransView_vClearPort();
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"WIFI Connecting");
        ret = OsWifiConnect(&Select_Ap,5*1000);
        if(ret != 0){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"WIFI Connect Failed");
            goto EXIT;
        }
        ret = OsWifiCheck(ssid,bssid,&rssi);
        if(ret != 0){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Get WIFI Status Failed");
            goto EXIT;
        }
    }
    else
    {
// ret = OsWifiCheck(ssid, bssid, &rssi);
        OsLog(LOG_ERROR,"OsWifiCheck ssid = %s bssid =%s", ssid, bssid);
    }

    Ota_Process();

EXIT:
    OsLog(LOG_ERROR,"%s.%d Ota_Wifi EXIT",__FUNCTION__, __LINE__);
    if (Aps != NULL)
    {
        free(Aps);
    }
    if(pszList != NULL){
        Freelist(num,pszList);
    }

    return ret;

}

PR_INT32 Func_OTA_Test(void)
{
    PR_INT8 *pListItem[] = { (char *)"OTA WiFi", (char *)"OTA Cellular"};
    PR_INT32 nNumOfItems = sizeof(pListItem) / sizeof(PR_INT8 *);
    PR_INT32 nRet;
    PR_INT32 nGetIndex=0;

    while(1)
	{
        TransView_vClearAllScreen();
	    nRet = TransView_nShowTableViewEx((char*)"MAIN MENU",(PR_INT8**)pListItem, nNumOfItems, 60,  &nGetIndex);
        if(nRet == PR_NORMAL){
            switch(nGetIndex){
                case 0:
                    Ota_Wifi();
                    break;
                case 1:
                    Ota_Process();
                    break;
            }
        }else if(nRet == RET_DRPOT_CANCEL || nRet == RET_DPORT_TIMEOUT){
            break;
        }
    }
    return PR_NORMAL;
}

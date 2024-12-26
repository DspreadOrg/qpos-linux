#include "app_setting.h"

static pu8 g_SettingBuffer = NULL;


void SetCfgPool(u32 tag,T_U8_VIEW uvValue)
{
    set_tlv_view(g_SettingBuffer,tag,uvValue);
}

T_U8_VIEW GetCfgPool(u32 tag)
{
    return get_tlv_view(g_SettingBuffer,tag);
}

u32 getWifiName(pu8 wifiName)
{
	u8 ssid[256] = {0};
    u8 bssid[20] = {0};
	int ret,rssi;

	ret = OsWifiCheck(ssid,bssid,&rssi);
	if(ret != 0){
		OsLog(LOG_ERROR,"OsWifiCheck ret= %d \n",ret);
        return 0;
	}
	else
	{   
        OsLog(LOG_DEBUG,"Dspread: ssid[%d] = %s \n",strlen(ssid),ssid);
		memcpy(wifiName,ssid,strlen(ssid));
        return strlen(ssid);
	}
    
} 

u32 AnalysisWifiList(pu8 pValue,T_U8_VIEW list[])
{
    u32 i=0;
    u32 cnt=0;
    u32 len=0;
    u8 wifiNmae[64] = {0};

    while(pValue[cnt]!='{')
    {
        cnt++;
    }
    cnt++;
    list[i].head=&pValue[cnt];
    while(cnt<=strlen((char*)pValue))
    {
        len++;
        cnt++;
        if(pValue[cnt]=='|')
        {
            list[i].len=len;
            memset(wifiNmae,0,64);
            memcpy(wifiNmae,list[i].head,len);

            len=0;
            i+=1;
            cnt++;
            list[i].head=&pValue[cnt];
        }
        else if(pValue[cnt]=='}')
        {
            list[i].len=len;
            memset(wifiNmae,0,64);
            memcpy(wifiNmae,list[i].head,len);
            break;
        }
        memset(wifiNmae,0,64);
        memcpy(wifiNmae,list[i].head,len);
    }
    return i+1;
}

void Wifi_Search()
{
    u32 res=0;
    u32 list_num=0;
    char * prompt=NULL;
    T_U8_VIEW   uvlist[60]= { {NULL, 0} };
RESEARCH:		
    memset(strApList, 0x00, 1024);
    res=obtain_wifi_ap_list((char * )strApList, 1024);

    if(res==0x00){
        memset(uvlist, 0x00, sizeof(uvlist));
        list_num=AnalysisWifiList(strApList,uvlist);
    }

    if(list_num == 1)
        goto RESEARCH;

    if(g_SettingBuffer)
    {
        free(g_SettingBuffer);
        g_SettingBuffer=NULL;
    }
    g_SettingBuffer=malloc(128);
    tlv_new_(g_SettingBuffer, POS_TAG_HEAD, 0, NULL);
    GuiEventRegister(LCD_DISP_WIFI_LIST);
    //DispWifiList(prompt,uvlist,list_num);   
}

void  Wifi_Connect()
{
    Rc_t Rc=RC_FAIL;
	u8 strApSel[64]={0};
    u8 strPassword[32]={0};
    T_U8_VIEW wifi_name=get_tlv_view(g_SettingBuffer,WIFI_NAME);
    T_U8_VIEW wifi_pwd=get_tlv_view(g_SettingBuffer,WIFI_PASSWORD);
    if(UV_OK(wifi_name) && UV_OK(wifi_pwd))
    {
        memcpy(strApSel,wifi_name.head,wifi_name.len);
        memcpy(strPassword,wifi_pwd.head,wifi_pwd.len);
    }
    // if(g_SettingBuffer)
    // {
    //     free(g_SettingBuffer);
    //     g_SettingBuffer=NULL;
    // }
    Rc = connect_wifi(strApSel, strPassword);

    if(Rc==RC_SUCCESS)
    {
        OsWifiSetAutoConnectStatus(1);
		//DispWifiConnectionMessage("Connect to Wi-Fi");
        GuiEventRegister(LCD_DISP_WIFI_CONNECT_SUCCESS);
    }
    else
    {
		//DispWifiConnectionFail();
        GuiEventRegister(LCD_DISP_WIFI_CONNECT_FAIL);
    }
}

void Wifi_Disconnect()
{
    if(OsWifiDisconnect() == RET_OK)
    {
        set_wifi_conn_status(false);
        
        DispResult("Disconnect Success");
    }
    else
    {
        DispResult("Disconnect Fail");
    }
}

void Wifi_Forget()
{
    if(OsWifiDisconnect() == RET_OK)
    {
        OsWifiSetAutoConnectStatus(0);
        DispResult("Forget Success");
    }
    else
    {
        DispResult("Forget Fail");
    }
}

void OpenAirplaneMode()
{
    if(OsSetAirplaneMode(1) == RET_OK)
    {
        OsWlSetAutoConnectStatus(0);
        DispResult("Open Success");
    }
    else
    {
        DispResult("Open Fail");
    }
}


void CloseAirplaneMode()
{
    if(OsSetAirplaneMode(0) == RET_OK)
    {
        OsWlSetAutoConnectStatus(1);
        DispResult("Close Success");
    }
    else
    {
        DispResult("Close Fail");
    }
}

/*----------------------------------------------------------------------------
|   Function Name:
|       Apn_cfg
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void Apn_cfg()
{
    int ret = 0;
    char oper[8] = {0};
    char networkType[16] = {0};
    int count = 10;
 
    // char apn_mcc[4];
    // char apn_mnc[4];
    // int len = 4;
    // memset(apn_mcc,0x00,sizeof(apn_mcc));
    // memset(apn_mnc,0x00,sizeof(apn_mnc));

    // ret = OsWlGetQNetworkInfo(networkType,oper);
    // if(ret == RET_OK)
    // {
    //     memcpy(apn_mcc,&oper[0],3);
    //     memcpy(apn_mnc,&oper[3],3);
    // }
    // else
    // {
    //     return;
    // }


    ret = OsSetApnParams(APN,APN_NAME,NULL);
    OsLog(LOG_ERROR,"Dspread: OsSetApnParams ret = %d",ret);

    while(count > 0){
        ret = OsWlCheck();
        OsLog(LOG_ERROR,"Dspread: OsWlCheck = %d",ret);
        if(ret == ERR_WL_NORSP){
            OsSleep(1000);
            count --;
            continue;
        }else{
            break;
        }
    }
    if(ret == ERR_WL_NOSIM ){
        OsLog(LOG_ERROR,"Dspread: Sim Not Inserted\n");
    }else if(ret != ERR_WL_NORSP && ret != RET_OK){
        OsLog(LOG_ERROR,"Dspread: Network Connecting\n");
        ret = OsWlLogin(APN,APN_NAME,NULL,0,15*1000,0,0);
        if(ret == RET_OK)
        {
            OsLog(LOG_ERROR,"Dspread: Network Connected\n");
        }
    }

	
}
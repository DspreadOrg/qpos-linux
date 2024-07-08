#include "appinc.h"
#include "tlv_pro.h"
#include "api_wrapper.h"
#include "app_thread.h"

static ST_WifiApInfo * gAps = NULL;
static u32 dev_gprs_signal_lvl = 0;
static u32 dev_wifi_signal_lvl = 0;


u32 lark_rand(pu8 pRand,u32 randLen )
{
    OsGetRandom(pRand,randLen);
    return randLen;
}

void Set_dev_gprs_signal_lvl(u32 lvl)
{
	dev_gprs_signal_lvl = lvl;
}

u32 Get_dev_gprs_signal_lvl(void)
{
    return dev_gprs_signal_lvl;
}

void Set_dev_wifi_signal_lvl(u32 lvl)
{
	dev_wifi_signal_lvl =lvl;
}

u32 Get_dev_wifi_signal_lvl(void)
{
    return dev_wifi_signal_lvl;
}
/*--------------------------------------
|   Function Name:
|       Get_dev_sim_status
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
u32 Get_dev_sim_status(void)
{
    /**sim status:0:Not inserted  1:insert**/
    int ret;
    int simStatus = 1;
    ret = OsWlGetSimStatus(&simStatus);
    if(ret != RET_OK)
        simStatus = 0;
    return simStatus;
}

/*--------------------------------------
|   Function Name:
|       Get_dev_battery_lvl
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
u32 Get_dev_battery_lvl(void)
{
    return OsGetBatCap();
}

/*--------------------------------------
|   Function Name:
|       Get_dev_UsbConnect_Status
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
bool Get_dev_UsbConnect_Status(void)
{
	int nRet = OsGetBatChgState();
    //OsLog(LOG_DEBUG,"Dspread: OsGetBatChgState ret= %d",nRet);
    if(nRet < 0 || nRet == 3 || nRet == 0xFF)
        return false;
    return true;
}

/*--------------------------------------
|   Function Name:
|       SetposSleepTimeout
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void SetposSleepTimeout(u32 timeout)
{
	OsSysSleepTime((int)timeout);
}
/*--------------------------------------
|   Function Name:
|       SetposSleepTimeout
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
u32 GetposSleepTimeout(void)
{
    int Time;
	OsGetSysSleepTime(&Time);
    return (u32)Time;
}

/*--------------------------------------
|   Function Name:
|       GetBootVer
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
u32 GetBootVer(pu8 pVer)
{
    OsGetSysVer(0x01,(char *)pVer);
    return strlen((char *)pVer);
}

u32 bcd_to_asc(pu8 bcd,u32 align,u32 asclen,pu8 asc )
{
	u32  cnt;
	pu8  pbcd = bcd;
	pu8  pasc = asc;

	if((asclen & 0x01) && align){
		cnt = 1;
		asclen ++;
	}
	else{
		cnt = 0;
	}
	for(; cnt < asclen; cnt++, pasc++){
		*pasc = ((cnt & 0x01) ? (*pbcd ++ & 0x0f) : (*pbcd >> 4));
		*pasc += ((*pasc > 9) ? ('A' - 10) : '0');
	}
	*pasc = 0;
	return asclen;
}

Rc_t connect_wifi(unsigned char * pSSID, unsigned char * pPassword)
{
	Rc_t rc = RC_FAIL;
	ST_WifiApSet Ap;
	PR_INT32 rssi,ret,nIndexOfSelect,lvl;
	pu8 pValue = pSSID;
	u8 temp[5];
	u8 ssid[256] = {0};
    u8 bssid[20] = {0};

	OsLog(LOG_ERROR,"Dspread: connect to  【%s】 pwd 【%s】 \n",pSSID,pPassword);
	memset(&Ap,0x0,sizeof(ST_WifiApSet));

    u32 i=0;
    u32 cnt=0;
	memset(temp,0,5);

	if(gAps ==NULL)
		return rc;

    while(pValue[cnt]!='.')
    {
        cnt++;
    }
	memcpy(temp,pValue,cnt);
	nIndexOfSelect = atoi(temp);
	OsLog(LOG_ERROR,"Dspread: nIndexOfSelect  【%d】 \n",nIndexOfSelect);

	memcpy(Ap.Essid,gAps[nIndexOfSelect].Essid,strlen(gAps[nIndexOfSelect].Essid));
	memcpy(Ap.KeyUnion.PskKey.Key,pPassword,strlen(pPassword));
    Ap.KeyUnion.PskKey.KeyLen = strlen(pPassword);
	Ap.AuthMode = gAps[nIndexOfSelect].AuthMode;
	Ap.SecMode = gAps[nIndexOfSelect].SecMode;
	memcpy(Ap.Bssid,gAps[nIndexOfSelect].Bssid,strlen(gAps[nIndexOfSelect].Bssid));
	ret = OsWifiConnect(&Ap,10*1000);
    if(ret != 0){
        set_wifi_conn_status(false);
		OsLog(LOG_ERROR,"Dspread: OsWifiConnect ret= %d \n",ret);
        goto EXIT;
    }
	ret = OsWifiCheck(ssid,bssid,&rssi);
	if(ret != 0){
        set_wifi_conn_status(false);
		OsLog(LOG_ERROR,"Dspread: OsWifiCheck ret= %d \n",ret);
        goto EXIT;
    }

	rc = RC_SUCCESS;
	
	OsLog(LOG_ERROR,"Dspread: ssid 【%s】 rssi 【%d】 \n",pSSID,rssi);
    if(rssi < -85)
        lvl = 1;
    else if(rssi >=-85 && rssi < -75)
        lvl = 2;
    else if(rssi >=-75 && rssi < -65)
        lvl = 3;
    else 
        lvl = 4;		
    set_wifi_conn_status(true);	
	Set_dev_wifi_signal_lvl(lvl);
	OsLog(LOG_DEBUG,"Dspread: wifi signal = %d",lvl);
EXIT:
	if(gAps != NULL){
        free(gAps);
        gAps = NULL;
    }
	return rc;	
}
Rc_t disonnect_current_wifi(void)
{
	int ret;
	Rc_t rc = RC_FAIL;
	ret = OsWifiDisconnect();
	if(ret == RET_OK)
    {
        Set_dev_wifi_signal_lvl(0);
		rc = RC_SUCCESS;
    }
    if(gAps != NULL){
        free(gAps);
        gAps = NULL;
    }
	return rc;
}

Rc_t set_gprs_airplane_mode(int onoff)
{
    int ret;
    ret = OsSetAirplaneMode(onoff);
    if(ret == RET_OK)
        return RC_SUCCESS;
    return RC_FAIL;     
}
/*--------------------------------------
|   Function Name:
|       obtain_wifi_details
|   Description:
|   Parameters:
|   Returns: 软硬件不匹配则返回wifi不可用
|            执行成功返回wifi的详细信息
+-------------------------------------*/
void obtain_wifi_details(char *wifiName, u8 bytLenMax)
{
	PR_INT8 ssid[256] = {0};
    PR_INT8 bssid[20] = {0};
	PR_INT32 ret,rssi;

	ret = OsWifiCheck(ssid,bssid,&rssi);
	if(ret != 0){
		OsLog(LOG_ERROR,"OsWifiCheck ret= %d \n",ret);
	}
	else
	{
		memcpy(wifiName,ssid,strlen(ssid));
	}
}

u32 parse_ap_list(u32 nItemCount,ST_WifiApInfo * Aps,char *pList,u32 dwLenMax)
{
	u32 i = 0,len = 0;
	pList[len++] = '{';
	for (int i = 0; i < nItemCount; i++) 
	{
		sprintf(&pList[len],"%d",i);
		if(i < 10)
		{
			len += 1;
		}
		else if(i<=99)
		{
			len += 2;
		}
		else
			len += 3;
		pList[len++] = '.';	
		memcpy(&pList[len],Aps[i].Essid,strlen(Aps[i].Essid));
		len += strlen(Aps[i].Essid);
		if(i == (nItemCount - 1))
			break;
		pList[len++] = '|';
	}
	pList[len++] = '}';

	OsLog(LOG_ERROR,"Dspread: AP LIST\n");
	OsLog(LOG_ERROR,"Dspread: %s\n",pList);


	return len;
}

u32 obtain_wifi_ap_list(char * pList, u32 dwLenMax)
{
	PR_INT32 res = 1;
    PR_INT32 i, num,ret;

#if 1
    PR_INT32 rssi,nIndexOfSelect;
	if(gAps != NULL){
        free(gAps);
        gAps = NULL;
    }
	
	num = OsWifiScan (&gAps);
    if(num <= 0){
        OsLog(LOG_DEBUG,"Dspread: OsWifiScan ret = %d",num);
        return 1;
    }
	if(parse_ap_list(num,gAps,pList,dwLenMax) > 0)
	{
		res = 0;
	}
EXIT:
#else
    //memcpy(pList,"{0.Dspread|1.Dsp-Guest|2.Dsp-Guest|3.Dspread|4.Dsp-Guest|5.Dspread|6.Dsp-Guest|7.Dspread|8.WPA3-TEST|9.TP-LINK_6CF1|10.TP-LINK_366202|11.WBYL|12.DIRECT-D3EE77FC|13.ORION|14.ORION_Guest|15.ORION_VP|16.ORION_VP|17.ORION|18.ORION_VP|19.QAX-Guest|20.878_NLX-Family|21.YYKJ-MI|22.QAX-Staff|23.ORION|24.ORION_Guest|25.QAX_LX-Staff}",
    //    strlen("{0.Dspread|1.Dsp-Guest|2.Dsp-Guest|3.Dspread|4.Dsp-Guest|5.Dspread|6.Dsp-Guest|7.Dspread|8.WPA3-TEST|9.TP-LINK_6CF1|10.TP-LINK_366202|11.WBYL|12.DIRECT-D3EE77FC|13.ORION|14.ORION_Guest|15.ORION_VP|16.ORION_VP|17.ORION|18.ORION_VP|19.QAX-Guest|20.878_NLX-Family|21.YYKJ-MI|22.QAX-Staff|23.ORION|24.ORION_Guest|25.QAX_LX-Staff}"));
    memcpy(pList,"{0.Dspread|1.Dsp-Guest}",strlen("{0.Dspread|1.Dsp-Guest}"));

    res = 0;
    OsSleep(1000);
#endif
	return   res;
}


Rc_t QposUpdateApn(pApnParam_t pApn)
{
    PR_INT32 ret;
    PR_INT32 count = 10;
    Rc_t rc = RC_FAIL;
#ifdef CFG_LOCAL_TEST
    return rc;
#endif

    ret = OsSetApnParams(pApn->Apn,pApn->ApnName,NULL);
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
        ret = OsWlLogin(pApn->Apn,pApn->ApnName,NULL,0,15*1000,0,0);
        if(ret == RET_OK)
        {
            rc = RC_SUCCESS;
        }
    }
    return rc;
}


int init_socket_para(socket_parameters_t *socket_info,char *pHost,int port,char * szSendBuf, int nSendLen,int timeout)
{
	uchar ipAddr[32] = {0};
	struct hostent *host;
	int nRet;
	if (socket_info == NULL){
		return -1;
	}
	socket_info->custno = 0;
	//SSL flag
	socket_info->ssl_flag = 1;	
    OsLog(LOG_DEBUG,"Dspread: pHost = %s",pHost);
	memcpy(socket_info->host,pHost,strlen(pHost));
    host = gethostbyname(pHost);
	if(host == NULL) {
		return -1;
	}
	//IP地址
	for(int i=0; host->h_addr_list[i]; i++){
		OsLog(LOG_INFO,"IP addr %d: %s\n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
    
	memcpy(ipAddr,inet_ntoa(*(struct in_addr*)host->h_addr_list[0]),strlen(inet_ntoa(*(struct in_addr*)host->h_addr_list[0])));
	memcpy(socket_info->ip,ipAddr,strlen(ipAddr));
	socket_info->port = port;
    
	//send timeout
	socket_info->connect_time = timeout;
	//recoive time out
	socket_info->recv_time = timeout;

	socket_info->union_https_flag = 0;
	//打印
	//socket_info->is_show_title = 1;
	//send buf
	if (szSendBuf != NULL)
	{
		socket_info->sendlen = nSendLen;
		socket_info->sendbuf = szSendBuf;
	}
	//default socket id
	socket_info->socket_id = -1;
	return 0;
}

Rc_t HttpServerConnect(char * pScheme,u32 port,char * pHost,u32 timeout)
{
    return RC_SUCCESS;
    Rc_t rc = RC_FAIL;

    return rc;
}

void HttpServerDisConnect()
{
    #if 0
    if(http_cfg.connect_status)
    {
        SSL_shutdown(http_cfg.ssl);
        SSL_free(http_cfg.ssl);
        SSL_CTX_free(http_cfg.ctx);
        closesocket(http_cfg.socket_fd);
    }
    #endif
}

s32 SslTransmit(pu8 pHost, u32 Port, pu8 pData, u32 DataLength, pu8 pReceivedData, u32 ReceivedDataLengthMax, u32 Timeout )
{
	socket_parameters_t socket_info;
	s32 nLen,nRet;
	DSP_Debug();
	//初始化通讯参数
	ZERO_STRUCT(socket_info);
	nRet = init_socket_para(&socket_info,pHost,Port, pData, DataLength,Timeout);
	if (nRet)
	{
		return -99;
	}
	nLen = socket_tcp_ssl(&socket_info,ReceivedDataLengthMax,pReceivedData);
	if(nLen > 0)
	{
		OsLog(LOG_DEBUG,"Dspread: recv[%d]",nLen);
		OsLog(LOG_DEBUG,"Dspread: %s",pReceivedData);
	}
	return nLen;

}

s32 SocketTransmit(pu8 pHost, u32 Port, pu8 pData, u32 DataLength, pu8 pReceivedData, u32 ReceivedDataLengthMax, u32 Timeout )
{
	#warning ("This api doesn't support! ");
}


u32 GetPosid(pu8 pValue)
{
	OsGetTermSn(pValue);
    return strlen(pValue);
}

/*--------------------------------------
|   Function Name:
|       lark_rtc_read
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
u32 lark_rtc_read(pu8     time )
{
	ST_TIME Time;
	OsGetTime(&Time);
	sprintf(time,"%04d",Time.Year);
	sprintf(time+4,"%02d",Time.Month);
	sprintf(time+6,"%02d",Time.Day);
	sprintf(time+8,"%02d",Time.Hour);
	sprintf(time+10,"%02d",Time.Minute);
	sprintf(time+12,"%02d",Time.Second);

    return 14;
}

/*--------------------------------------
|   Function Name:
|       lark_rtc_write
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void lark_rtc_write(pu8     time )
{
	u8 year[6] ={0},m[3]={0},d[3]={0},h[3]={0},mm[3]={0},s[3]={0};
	ST_TIME Time;

	memcpy(year,time,4);
	memcpy(m,time+4,2);
	memcpy(d,time+4+2,2);
	memcpy(h,time+4+2+2,2);
	memcpy(mm,time+4+2+2+2,2);
	memcpy(s,time+4+2+2+2+2,2);
	Time.Year = atoi(year);
	Time.Month = atoi(m);
	Time.Day = atoi(d);
	Time.Hour = atoi(h);
	Time.Minute = atoi(mm);
	Time.Second = atoi(s);

	OsSetTime(&Time);
}

PR_INT32 CardDev_Open(Card_Type_t  TransCardType)
{
    PR_INT32 nRet = PR_NORMAL;

    if(TransCardType & CARD_MAG)
    {
        nRet = OsMsrOpen();
        if(nRet != RET_OK){
            goto EXP;
        }
    }

    if(TransCardType & CARD_IC)
    {
        nRet = OsIccOpen(0);
        if(nRet != RET_OK){
            goto EXP;
        }
    }

    if(TransCardType & CARD_NFC)
    {
        nRet = OsPiccOpen();
        if(nRet != RET_OK){
            goto EXP;
        }
    }

    return nRet;
EXP:
    OsMsrClose();
    OsIccClose(0);
    OsPiccClose();
	return PR_FAILD;
}
void CardDev_Close(PR_INT32 nSwipType){
    OsMsrClose();

	if (nSwipType != INPUT_INSERTIC){
        OsIccClose(0);
    }
		
	if (nSwipType != INPUT_RFCARD){
        OsPiccClose();
    }
}

static bool _CardDataIsNum(PR_INT8 Data){
	if(Data>='0'&&Data<='9')
		return PR_TRUE;
	else
		return PR_FALSE;
}

bool _ParseTrack2(const PR_INT8* theTrack2Data, PR_UINT32 theTrack2DataLength, CardDataInfo* theTrackDataInfo)
{
	PR_UINT32 i=0;
	PR_UINT32 cardNumLength = 0;

	for( ;i<theTrack2DataLength; i++)
	{
		if(_CardDataIsNum(theTrack2Data[i])==PR_FALSE)
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

void DispStartTransaction(Card_Type_t type)
{
    
    char disp[32]={0};
    strcpy(disp,"Pls ");

    TransView_vClearPort();

    if(type & CARD_MAG !=0){
        strcat(disp,"Swip ");
    }
    if(type & CARD_NFC !=0){
        strcat(disp,"NFC ");
    }
    if(type & CARD_IC !=0){
        strcat(disp,"Insert ");
    }
    strcat(disp,"card");
    Disp_vShowStr(1,EM_ALIGN_LEFT, EM_DTYPE_NORMAL,disp);

}

void led_disp_nfc_success_prompt()
{
    OsLed(0,0,0,1);
    OsSleep(30);
    OsLed(0,0,1,1);
    OsSleep(30);
    OsLed(0,1,1,1);
    OsSleep(30);
    OsLed(1,1,1,1);
    OsSleep(30);
    OsLed(0,0,0,0);
}

bool downGradeCheck(bool DowngradeStatus,CardDataInfo *pCardDataInfo)
{
    bool bResult = true;
    u8 serviceCode[8] = {0};
    if(!DowngradeStatus)
    {
        intercept_card_service_code_from_track2(pCardDataInfo->ptTracker2.TeackData,pCardDataInfo->ptTracker2.DataLen,serviceCode);
        if(serviceCode[0] == '2' || serviceCode[0] == '6'){
            OsLog(LOG_DEBUG,"chip card\r\n");
            OsMsrClose();
            OsBeep(1,100);
            disp_chip_card();
            bResult = false;
        }
    }

    return bResult;
}

PR_INT32 _ReadMagCard(CardDataInfo *DataInfo){
    if(OsMsrRead(&DataInfo->ptTracker1,&DataInfo->ptTracker2,&DataInfo->ptTracker3) != 0){
        return PR_FAILD;
    }
    if(DataInfo->ptTracker2.Status != 0 || DataInfo->ptTracker2.DataLen <= 22){
        //BCTC一张错误卡为22个字节LRC等均正确需要判断为错误卡
        return PR_FAILD;
    }
  
    _ParseTrack2((PR_INT8*)DataInfo->ptTracker2.TeackData, DataInfo->ptTracker2.DataLen, DataInfo); 
    return PR_NORMAL;
}

PR_INT32 fun_poll_card(Card_Type_t  TransCardType, PR_INT32 *pRetSwipeType,CardDataInfo *pCardDataInfo,PR_INT32 nTimeoutS,bool DowngradeStatus){

    PR_INT8 szPiccType[8] = {0};
    PR_UINT8 ucATQx[16] = {0};
    PR_INT32 nRet = PR_FAILD;
    unsigned char atr[64] = {0};
    int keyValue = EM_KEY_INVALID;
	ST_TIMER Timer;
	pmessage_body_t pmsg = NULL;

    DSP_Debug();

    if(CardDev_Open(TransCardType) != PR_NORMAL){
        return PR_FAILD;
    }
	DispStartTransaction(TransCardType);
	if (( nRet = OsTimerSet(&Timer, nTimeoutS)))
    {
		OsLog(LOG_INFO,"Dspread:----------fun_wait_events-----ret =%d------nTimeoutS = %d",nRet,nTimeoutS);
        return -1;
    }
fun_poll_card_start:

    set_Lvgl_Kbd_Flag(true);
    Disp_ReleasKey();
    while(1)
    {
        nRet = OsPiccPoll(szPiccType,ucATQx);
        if(nRet == RET_OK){
            OsLog(LOG_INFO,"Dspread: ----------INPUT_RFCARD-----------");
            *pRetSwipeType = INPUT_RFCARD;
            nRet = PR_NORMAL;
            led_disp_nfc_success_prompt();
            goto EXIT;
        }
        nRet = OsIccDetect(0);
        if(nRet == RET_OK){
            OsLog(LOG_INFO,"Dspread: ----------INPUT_INSERTIC-----------");
            if(OsIccInit(ICC_USER_SLOT,0,atr) == 0)
            {
                *pRetSwipeType = INPUT_INSERTIC;
                nRet = PR_NORMAL;
            }
            else
            {

            }
            goto EXIT;
        }
        
        nRet = OsMsrSwiped();
        if(nRet){
            memset(pCardDataInfo,0,sizeof(CardDataInfo));
            if(_ReadMagCard(pCardDataInfo) != PR_NORMAL){
                OsMsrReset();
            }else{
                OsLog(LOG_INFO,"Dspread: ----------SWIPE_CARD OK-----------");
                if(downGradeCheck(DowngradeStatus,pCardDataInfo) == false)
                {
                    goto fun_poll_card_start;
                }
                *pRetSwipeType = INPUT_STRIPE;
                nRet = PR_NORMAL;
                goto EXIT;
            }
        }
        else
        {
            // OsLog(LOG_INFO,"Dspread: ----------OsMsrSwiped ret = %d-----------",nRet);
        }
		if(EM_KEY_CANCEL == Disp_GetKey(10))
		{
            OsLog(LOG_INFO,"Dspread: ----------user cancle-----------");
            nRet = PR_CANCLE;
            goto EXIT;
		}
		else
		{

			if((long)OsTimerCheck(&Timer) <= 0)
			{
                nRet = PR_TIMEOUT;
				OsLog(LOG_INFO,"Dspread: ----------poll card timeout-----------");
				break;
			}
		}
		OsSleep(10);

    }

EXIT:
    if(nRet == PR_NORMAL){
        CardDev_Close(*pRetSwipeType);
    }else{
        //关闭所有读卡设备
        CardDev_Close(0);
    }

    set_Lvgl_Kbd_Flag(false);
    return nRet;
}
u32 StartTrading(unsigned char * pTradingFile)
{
    DSP_Debug();
    int nRetSwipeType = 0;
    u32 nRet  = 0;
    int timeOut;
    Card_Type_t  TransCardType = 0;
    CardDataInfo t_CardDataInfo;
    pu8 pNode = NULL;
    pu8          pTempBuffer = NULL;
	u32          tempBufferLen = 0;
	pu8          pDataBuffer = NULL;
	u32          dataBufferLen = 0;
	bool         DowngradeStatus = true;
    u32           pinLimitMin=0;
    u32           pinLimitMax=0;

    pu8 pTlvTransPool = NULL;
    int tlvTransPoolLen = 512;

    if(pTlvTransPool != NULL)
    {
        free(pTlvTransPool);
        pTlvTransPool = NULL;
    }
    pTlvTransPool = malloc(tlvTransPoolLen);
    memset(pTlvTransPool,0,tlvTransPoolLen);
    tlv_new_(pTlvTransPool,0x7F00,0,NULL);

    pTempBuffer = malloc(512);
	pDataBuffer = malloc(512);

    pNode = tlv_find_(pTradingFile, POS_TAG_TRANS_TIMEOUT);
    if(pNode)
    {
        timeOut = tlv_get_v_(pNode)[0];
        tlv_replace_(pTlvTransPool, POS_TAG_TRANS_TIMEOUT, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        timeOut = 0x3C;
        tlv_replace_(pTlvTransPool, POS_TAG_TRANS_TIMEOUT, 1, (pu8)"\x3C");
    }

        /*amount*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_AMOUNT_AUTH_NUM);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_AMOUNT_AUTH_NUM, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_AMOUNT_AUTH_NUM, 6, (pu8)"\x00\x00\x00\x00\x00\x00");  
    }

        /*amount other*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_AMOUNT_OTHER_NUM);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_AMOUNT_OTHER_NUM, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_AMOUNT_OTHER_NUM, 6, (pu8)"\x00\x00\x00\x00\x00\x00");
    }

        /*trade mode*/
    pNode = tlv_find_(pTradingFile, POS_TAG_TRADE_MODE);
    if(pNode)
    {
		switch(tlv_get_v_(pNode)[0]){
		case TRANS_MODE_ICC:
			TransCardType = CARD_IC;
			break;
		case TRANS_MODE_MAG:
			TransCardType = CARD_MAG;
			break;
		case TRANS_MODE_NFC:
			TransCardType = CARD_NFC;
			break;
		case TRANS_MODE_ICC_MAG:
			TransCardType = CARD_MAG | CARD_IC;
			break;
		case TRANS_MODE_ICC_MAG_DONT_ALLOW_DOWNGRADE:
			TransCardType = CARD_MAG | CARD_IC;
			DowngradeStatus = false;
			break;
		case TRANS_MODE_ICC_MAG_NFC:
			TransCardType = CARD_MAG | CARD_IC | CARD_NFC;
			break;
		case TRANS_MODE_ICC_MAG_NFC_DONT_ALLOW_DOWNGRADE:
			TransCardType = CARD_MAG | CARD_IC | CARD_NFC;
			DowngradeStatus = false;
			break;
        case TRANS_MODE_ICC_MAG_NFC_NOUPKEY:
            TransCardType = CARD_MAG | CARD_IC | CARD_NFC;
            break;
        case TRANS_MODE_ICC_MAG_NFC_NOUPKEY_DONT_ALLOW_DOWNGRADE:
            TransCardType = CARD_MAG | CARD_IC | CARD_NFC;
            DowngradeStatus = false;
            break;
        case TRANS_MODE_ICC_NFC_DONT_USE_UPKEY://仅支持IC/NFC,且禁止使用上翻键盘
            TransCardType = CARD_IC | CARD_NFC;
            DowngradeStatus = false;
            break;
        case TRANS_MODE_ICC_NFC://仅支持IC/NFC,需使用上翻键盘
            TransCardType = CARD_IC | CARD_NFC;
            DowngradeStatus = false;
            break;
        case TRANS_MODE_ICC_NFC_DONT_ALLOW_DOWNGRADE:
            TransCardType = CARD_IC | CARD_NFC;
			DowngradeStatus = false;
            break;
		default:
			TransCardType = CARD_MAG | CARD_IC | CARD_NFC;
			DowngradeStatus = false;
			break;
		}
        tlv_replace_(pTlvTransPool, POS_TAG_TRADE_MODE, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        TransCardType = CARD_MAG | CARD_IC | CARD_NFC;
        tlv_replace_(pTlvTransPool, POS_TAG_TRADE_MODE, 1, (pu8)"\x03");
    }

    /*currency symbol*/
    pNode = tlv_find_(pTradingFile, POS_TAG_CURRENCY_SYMBOL);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, POS_TAG_CURRENCY_SYMBOL, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {

    }

    /*date*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_TRANSACTION_DATE);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_TRANSACTION_DATE, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        lark_rtc_read(pTempBuffer);
        asc_to_bcd(pTempBuffer,14,pDataBuffer);
        tlv_add_child_(pTlvTransPool, TAG_EMV_TRANSACTION_DATE, 3, pDataBuffer + 1);
    }

    /*time*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_TRANSACTION_TIME);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_TRANSACTION_TIME, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        tlv_add_child_(pTlvTransPool, TAG_EMV_TRANSACTION_TIME, 3, pDataBuffer + 4);
    }

    /*rand*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_UNPREDICTABLE_NUMBER);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_UNPREDICTABLE_NUMBER, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        lark_rand(pTempBuffer,4);
        tlv_replace_(pTlvTransPool, TAG_EMV_UNPREDICTABLE_NUMBER, 4, pTempBuffer);
    }

        /*transaction type*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_TRANSACTION_TYPE);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_TRANSACTION_TYPE, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
    else
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_TRANSACTION_TYPE, 1, (pu8)"\x00");
    }

    /*currency code*/
    pNode = tlv_find_(pTradingFile, TAG_EMV_TRANSACTION_CURRENCY_CODE);
    if(pNode)
    {
        tlv_replace_(pTlvTransPool, TAG_EMV_TRANSACTION_CURRENCY_CODE, tlv_get_l_(pNode), tlv_get_v_(pNode));
    }
 

    free(pTempBuffer);
	free(pDataBuffer);

    int ret = -1;
    ret = fun_poll_card(TransCardType, &nRetSwipeType,&t_CardDataInfo,timeOut*1000,DowngradeStatus);
    if(ret != PR_NORMAL)
    {
        CardDev_Close(0);
        if(ret == PR_CANCLE)
            nRet = APP_RC_CANCEL;
        else if(ret == PR_TIMEOUT)
            nRet = APP_RC_TIMEOUT;
        else if(ret == PR_ICC_ERR)
            nRet = APP_RC_FALL_BACK;  
        else
            nRet = APP_RC_TERMINAL;        
        goto EXIT;
    }
    if(nRetSwipeType == INPUT_STRIPE)
    {   
        nRet = App_Mag_Process(&t_CardDataInfo,pTlvTransPool);
    }
    else
    {
        nRet = App_Emv_Process(nRetSwipeType,pTlvTransPool);
    }
    CardDev_Close(0);

EXIT:
    fun_trans_finish(nRet);   

    free(pTlvTransPool);
    pTlvTransPool = NULL;
    return nRet;
}

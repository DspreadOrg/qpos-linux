#include "api_wrapper.h"

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
    OsGetSysVersion(0x01,(char *)pVer);
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
		OsLog(LOG_ERROR,"Dspread: OsWifiConnect ret= %d \n",ret);
        goto EXIT;
    }
	ret = OsWifiCheck(ssid,bssid,&rssi);
	if(ret != 0){
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
	Set_dev_wifi_signal_lvl(lvl);
	OsLog(LOG_DEBUG,"Dspread: wifi signal = %d",lvl);
EXIT:
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
	return   res;
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

/*--------------------------------------
|	Function Name:
|		fun_gprs_get_connect_mode
|	Description:
|	Parameters:
|	Returns:    //Value List: 0  "NO SERVICE", 1 "GSM"  2  4G
+-------------------------------------*/
u32 fun_gprs_get_connect_mode()
{
    int Ret = 0;
    int type = 0;
    Ret =  OsWLGetNetworkType(&type);
    #if 0
    OsLog(LOG_DEBUG,"-----------------OsWLGetNetworkType ret = %d type = %d",Ret,type);
    #endif
    if(Ret == 0)
    {
        return type;
    }

    return 0;
}


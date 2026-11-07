#include "app_thread.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "app_battery_icon.h"
#include "ui_sim.h"
#include "app_setting.h"

u32 g_Event_Id = EVENT_START;

extern ST_WifiApSet *gAp;
extern void CheckBattery(void);

static bool event_thread_flag = false;

static bool dev_init_flag = false;
static bool g_sim_status = false;
static int dev_gprs_connect_status = 0;
static bool wifi_conn_status = false;

bool get_wifi_conn_status()
{
	return wifi_conn_status;
}

void set_wifi_conn_status(bool status)
{
	wifi_conn_status = status;
}

void event_thread_mutex(Switch_t status)
{
    if(status == MUTEX_NO)
    {
        event_thread_flag = true;
    }
    else if(status == MUTEX_OFF)
    {
        event_thread_flag = false;
    }
}

bool event_thread_mutex_status()
{
    return event_thread_flag;
}

/*--------------------------------------
|   Function Name:
|       EventRegister
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/

static int Event_Queue_Id = 0;
void EventRegister(u32 Event_id)
{
	OsSleep(50);
	if(event_thread_mutex_status())
	{
		return;
	}
	event_thread_mutex(MUTEX_NO);
	#ifdef CFG_DBG
	OsLog(LOG_INFO,"Dspread: EventRegister event_id = %d",Event_id);
	#endif
	message_body_t msg;
    msg.msg_id = Event_id;
	if (msgsnd(Event_Queue_Id, &msg, sizeof(message_body_t), 0) == -1) {
		DSP_Debug();
    }
}


void *Event_Thread(void *args)
{
	message_body_t msg;
	int ret = 0;

	key_t key;
	char *file = "/tmp/event_msg";
	FILE *fp = fopen(file, "w");
	if (fp != NULL)
	{
		fclose(fp);
	}

    // 创建一个唯一的 key
    key = ftok("/tmp/event_msg", 'm');
    if (key == -1) {
		return NULL;
    }

	    // 创建消息队列
    Event_Queue_Id = msgget(key, IPC_CREAT | 0666);
    if (Event_Queue_Id == -1) {
		return NULL;
    }
    while(1)
    {
		memset(&msg,0,sizeof(message_body_t));
		ret = -1;
		ret = msgrcv(Event_Queue_Id, &msg, sizeof(message_body_t), 0, 0);
		if (ret != -1)
		{
			EventHandle(msg.msg_id);
			event_thread_mutex(MUTEX_OFF);	
		}
        OsSleep(10);
    }

}

void Gprs_Init(){
    PR_INT32 ret;
    PR_INT8 temp[128] = {0};
    PR_INT32 count = 10;
	int status = 0,simStatus = 0;
	#ifdef CFG_DBG
    OsLog(LOG_ERROR,"Dspread: -------------------Gprs_Init-------------------");
	#endif
    OsWlLock();

	ret = OsWlInit("");
	if(ret != RET_OK){
		#ifdef CFG_DBG
		OsLog(LOG_ERROR,"Dspread: Sim Not Inserted");
		#endif
		return;
	}

	OsSetAirplaneMode(0); //
 	OsWlGetAutoConnectStatus(&status);
	ret = OsWlGetSimStatus(&simStatus);
	if( (ret == RET_OK) && (simStatus == 1))
	{
		#ifdef CFG_DBG
		OsLog(LOG_ERROR,"Dspread: -------------------Sim instert-------------------");
		#endif
	}

	if (status != 1)
		OsWlSetAutoConnectStatus(1); //set auto connect
}

void Wifi_Init()
{
    int ret,rssi,lvl;
	char ip[20] = {0};
    ST_WifiApSet *gAp = NULL;
    gAp = malloc(sizeof(ST_WifiApSet));
    memset(gAp,0,sizeof(ST_WifiApSet));
    
    OsWifiOpen();

    ret = OsGetWifiStatus(gAp->Essid,gAp->Bssid,ip,&rssi);
    if(ret != 0){
		#ifdef CFG_DBG
        OsLog(LOG_ERROR,"OsWifiCheck ret= %d \n",ret);
		#endif
		wifi_conn_status = false;
        Set_dev_wifi_signal_lvl(0); 
    }
    else
    {
        lvl = OsGetWifiSignalStrength();		
		wifi_conn_status = true;	
        Set_dev_wifi_signal_lvl(lvl);
        memset(gAp,0,sizeof(ST_WifiApSet));
    }
    free(gAp);
}

int Dev_nInit()
{
	Wifi_Init();
	Gprs_Init();
	dev_init_flag = true;
	return 0;
}

int GetSimContnectStatus()
{
    int ret = -1;
    ret = OsWlCheck();
    if(ret == 0)
        return 1;
    return 0;
}


void *Signal_Thread(void *args)
{
	bool flag = false;
    int ret,rssi,lvl;
	char ip[20] = {0};
	int time = 0;
	int simStatus = 0;

	while (1)
	{		
		OsSleep(300);
		if (time == 1)
		{
			if(!dev_init_flag)
				continue;
			if(flag)
			{
				flag = false;
				if(g_sim_status)
				{
					ret = OsWlGetSignal();
					if(ret > 0)
					{
						#if 0
						OsLog(LOG_INFO,"Dspread: gprs signal lvl = %d",ret);
						OsLog(LOG_INFO,"Dspread: gprs connect status = %d",OsWlCheck());
						#endif
						Set_dev_gprs_signal_lvl(ret);
					}
					else
					{
						#if 0
						OsLog(LOG_INFO,"Dspread: no gprs signal, ret = %d",ret);
						#endif
						Set_dev_gprs_signal_lvl(0);
					}    
				}
				else
				{
					Set_dev_gprs_signal_lvl(0);
				}     
			}
			else
			{
				ret = OsGetWifiSignalStrength();
				#if 0
				OsLog(LOG_ERROR,"Dspread:  OWifiSignal lvl= %d \n",ret);
				#endif
				if(ret <= 0){
					wifi_conn_status = false; 				
					Set_dev_wifi_signal_lvl(0); 
				}
				else
				{
					wifi_conn_status = true; 
					Set_dev_wifi_signal_lvl(ret);
				}
				flag =true;
			}
			time = 0;
		}
		time++;
	}
}

void thread_init()
{
	static pthread_t event_tid;
	static pthread_t signal_tid;
	
	if(pthread_create(&event_tid,NULL,Event_Thread,NULL) != 0)
        printf("can't create thread\n");

    if(pthread_create(&signal_tid,NULL,Signal_Thread,NULL) != 0)
        printf("can't create thread\n");
}
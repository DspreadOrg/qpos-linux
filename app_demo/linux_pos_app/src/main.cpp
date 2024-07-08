#include "app_includes.h"
#include <signal.h>
#include "proj_sdk.h"

#include "ntp.h"
#include "signature.h"
#include "printer.h"
#include "app_emv.h"


#ifdef __cplusplus
extern "C"
{
#endif
#include "app_ota.h"

#ifdef __cplusplus
}
#endif

static char g_merName[24]={0};

void sigHandle(int sig)
{
    switch (sig)
    {
        case SIGPIPE:
            OsLog(LOG_ERROR, "*** catch signal : SIGPIPE, value = %d\n", sig);
            break;
        default:
            OsLog(LOG_ERROR, "*** catch signal, value = %d\n", sig);
            exit(0);
            break;
    }
}
void signalHandle(){
    signal(SIGPIPE, sigHandle);	//忽略信号
    signal(SIGINT, sigHandle);  //Ctrl + C
    signal(SIGTERM, sigHandle); //kill发出的软件终止
    signal(SIGBUS, sigHandle);  //总线错误
    signal(SIGSEGV, sigHandle); //非法内存访问
    signal(SIGFPE, sigHandle);  //数学相关的异常，如被0除，浮点溢出等
    signal(SIGABRT, sigHandle); //由调用abort函数产生，进程非正常退出
}

extern "C" int ymframwork_init(int argc, char *argv[]);
extern "C" int ymframwork_finish(int ret);


int updateMername(){
    TermInfo * terminfo =NULL;
    terminfo = (TermInfo *)malloc(sizeof(TermInfo));
    memset(terminfo,0,sizeof(TermInfo));
    // db_getTermInfo(terminfo);
    dbg_printf("terminfo->MerName = %s",terminfo->MerName);
    if(strlen(terminfo->MerName) > 0){
        memset(g_merName,0,sizeof(g_merName));
        strcpy(g_merName,terminfo->MerName);
    }

    free(terminfo);

    return 0;
}
PR_INT32 InitTermKey(){

    unsigned char ipek[] = {"\x3F\x22\x16\xD8\x29\x7B\xCE\x9C\x3F\x22\x16\xD8\x29\x7B\xCE\x9C"};
    unsigned char ksn1[] = "\x00\x00\x00\x00\x02\xDD\xDD\xE0\x00\x00";
	
    return 0;
}
void About(){    
    PR_INT32 ret;
    PR_INT8 temp[64] = {0};
    PR_INT8 imei[32] = {0};
    PR_INT8 mcuVer[32] = {0};
    PR_INT8 sysVer[32] = {0};
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"About");

    OsGetTermSn(temp);
    OsWlGetImei(imei);
    OsGetSysVer(0x01,sysVer);
    OsGetSysVer(0x03,mcuVer);
    TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"SN:%s",temp);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"IMEI:%s",imei);
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"MCU Ver:%s",mcuVer);
    TransView_vShowLineEx(4,EM_ALIGN_LEFT,(char*)"SYS Ver:%s",sysVer);
    KB_nWaitKeyMS(10*1000);
}
int inputText(char *disp,unsigned char *result){
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Set APN");
    TransView_vShowLine(1,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"Plz input %s",disp);

    TransView_vShowLine(5,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"If is null,plz press [enter]");

    return Disp_nUserInputView(0,97,319,32,0,32,DPORT_IME_EN|DPORT_IME_NUM|DPORT_IME_SYMBOL,32,result,15*1000,0);
}
void setSimAPN(){
    // OsSetApnParams();
    unsigned char apn[32]={0};
    unsigned char user[32]={0};
    unsigned char password[32]={0};
    int ret = 0;

    if(inputText("apn",apn) <0){
        return ;
    }
    if(inputText("user",user) <0){
        return ;
    }
    if(inputText("password",password) <0){
        return ;
    }

    ret = OsSetApnParams((char*)apn,(char*)user,(char*)password);
    
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Set APN");
    if(ret == 0){
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Set Success");
    }else{
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Set Fail!!!");
    }
    KB_nWaitKeyMS(10*1000);
}
void showSimInfo(){
    TransView_vClearAllScreen();
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"Getting Info...");
    char imei[32+1]={0};
    char iccid[32+1]={0};
    char ip[16+1]={0};
    const char *stu=NULL;

    int cardIn=0;
    
    OsWlGetImei(imei);
    OsWlGetIccid(iccid);
    OsWlGetNetworkIp(ip);
    OsWlGetSimStatus(&cardIn);
    stu = cardIn ? "INSERT":"NO";
    TransView_vClearAllScreen();
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"STU  :%s",stu);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"IMEI :%s",imei);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"ICCID:%s",iccid);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_LEFT,(char*)"IP   :%s",ip);
    KB_nWaitKeyMS_Ex(15*1000);
}
int WirelessMenu(){
    PR_INT8 *pListItem[] = { (char *)"Sim Open",(char *)"Sim Close",(char *)"Apn Set",(char *)"Sim info"};
    PR_INT32 nNumOfItems = sizeof(pListItem) / sizeof(PR_INT8 *);
    PR_INT32 nRet;
    PR_INT32 nGetIndex=0;
    
    while(1)
	{
        TransView_vClearAllScreen();
        nRet = TransView_nShowTableViewEx((char*)"Gprs Setting",(PR_INT8**)pListItem, nNumOfItems, 60,  &nGetIndex);

        if(nRet == PR_NORMAL){
            switch(nGetIndex){
                case 0:
                    nRet =OsWlInit("");
                    if(nRet == ERR_DEV_NOT_OPEN || nRet == ERR_DEV_NOT_EXIST){
                        Gprs_Init();
                    }
                    break;
                case 1:
                    OsWlUnLock();
                    break;
                case 2:
                    setSimAPN();
                    break;
                case 3:
                    showSimInfo();
                    break;
            }
        }else if(nRet == RET_DRPOT_CANCEL || nRet == RET_DPORT_TIMEOUT){
            break;
        }
    }
    return PR_NORMAL;
}
int TransEntery_nDoTransInDefaultEntry()
{
    PR_INT8 *pListItem[] = { (char *)"Trans",(char *)"Print",(char *)"Wireless",(char *)"WIFI",(char *)"About",(char *)"InitKey",(char *)"Tms"};
    PR_INT32 nNumOfItems = sizeof(pListItem) / sizeof(PR_INT8 *);
    PR_INT32 nRet;
    PR_INT32 nGetIndex=0;

    Disp_vClearLine(0);
    // updateMername();
    while(1)
	{
        TransView_vClearAllScreen();
        nRet = TransView_nShowTableViewEx((char*)"MAIN MENU",(PR_INT8**)pListItem, nNumOfItems, 60,  &nGetIndex);
     
        if(nRet == PR_NORMAL){
            switch(nGetIndex){
                case 0:
                    Trans_Payment();
                    break;
                case 1:
                    Print_Order();
                    break;
                case 2: 
                    WirelessMenu();
                    break;
                case 3:
                    wifiSet();
                    break;
                case 4:
                    About();
                    break;
                case 5:
                    InitTermKey();
                    break;
                case 6:
                    Firmware_OTA_Check();
                    break;
            }
        }else if(nRet == RET_DRPOT_CANCEL || nRet == RET_DPORT_TIMEOUT){
            break;
        }
    }
    return PR_NORMAL;
}
extern void kernelIns_init();

int App_Init(){
    char szAppId[] = "linux_pos_app"; 
    OsLogSetTag(szAppId);  //set logcat tag
    signalHandle();
    Disp_vInit(); //UI  init
    DB_bInit(szAppId); //database init

    kernelIns_init();

    thread_init();

    Disp_vSetStatusBarStatus(1);
    OsSetSysSleepStatus(0); //暂时设置不允许休眠
    
    Dev_nInit();
    
    OsSleep(1000);
    set_KernelInsLib_mode(1);
    // Ntp_TimeSync_Proc();
    return 0;
}

extern void Tms_Disp_Callback_Register(void (*Tms_disp)(u32, u32, u32, void*));
int App_Main(){
    int nRet;
	Tms_Disp_Callback_Register(TmsDispCallback);
	while(1)
	{
		//idle
		TransView_vShowLogoView();
		//check key
		nRet = KB_nWaitKeyMS_Ex(500);
		if(nRet == EM_KEY_ENTER){
            TransEntery_nDoTransInDefaultEntry();
        }
        else if (nRet == EM_KEY_CANCEL)
        {         
            if (RET_DPORT_OK == TransView_nDoMessageBox((char *)"Warning", BOX_STYLE_OK | BOX_STYLE_CANCEL, VIEW_TIMEOUT_FOREVER, (char *)"Exit App?"))
            {
                return PR_NORMAL;
            }
        }
	}

	return nRet;
}
int main(int argc, char *argv[])
{
    PR_INT32 ret;

    ymframwork_init(argc, argv);//system api

    App_Init();
    App_Main();

    ymframwork_finish(ret);//system api
    return 0;
}


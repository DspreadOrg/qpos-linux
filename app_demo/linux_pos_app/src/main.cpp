#include "appinc.h"
#include <signal.h>
#include "ota.h"
void sigHandle(int sig)
{
    switch (sig) {
    case SIGINT:
		OsLog(LOG_ERROR,"*** catch signal : SIGINT, value = %d\n", sig);
        break;
    case SIGTERM:
		OsLog(LOG_ERROR,"*** catch signal : SIGTERM, value = %d\n", sig);
        break;
    case SIGBUS:
		OsLog(LOG_ERROR,"*** catch signal : SIGBUS, value = %d\n", sig);
        break;
    case SIGSEGV:
		OsLog(LOG_ERROR,"*** catch signal : SIGSEGV, value = %d\n", sig);
        break;
    case SIGFPE:
		OsLog(LOG_ERROR,"*** catch signal : SIGFPE, value = %d\n", sig);
        break;
    case SIGABRT:
		OsLog(LOG_ERROR,"*** catch signal : SIGABRT, value = %d\n", sig);
        break;
    default:
		OsLog(LOG_ERROR,"*** catch unknown signal, value = %d\n", sig);
        break;
    }
    exit(0);
}


void sigSleepHandle(int sig)
{
	OsLog(LOG_ERROR,"*** catch signal : SIGINT, value = %d\n", sig);  
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

void DisplayInit(){
    Disp_vInit(); //UI  init
    Disp_vSetSysVectorFont(SYS_DEFAULT_FONT_FILE_PATH);
    TransView_vClearAllScreen();
	TransView_vShowLine(2, EM_DTYPE_NORMAL,EM_ALIGN_CENTER, (char*)"App Init");
}

void WirelessInit(){
    PR_INT32 ret;
    PR_INT8 temp[128] = {0};
    PR_INT32 count = 10;
    PR_INT32 autoConnect = 0;
    PR_INT32 airplaneMode = 0;
    PR_INT8 szApn[64] = {0};
    PR_INT8 szApnName[64] = {0};
    PR_INT8 szApnPwd[64] = {0};

    OsWlLock();
    ret = OsWlInit((char*)"");
    OsLog(LOG_ERROR,"============OsWlInit = %d",ret);
    OsWlGetAutoConnectStatus(&autoConnect);
    OsGetAirplaneMode(&airplaneMode);
    if(ret == ERR_WL_NOSIM ){
        OsLog(LOG_ERROR,"Sim Not Inserted\n");
        TransView_vShowLine(2, EM_DTYPE_NORMAL,EM_ALIGN_CENTER, (char*)"Sim Not Inserted");
        KB_nWaitKeyMS(3*1000);
    }else {
        if(airplaneMode == 1){
            OsLog(LOG_ERROR,"=======Airplane Mode Open\n");
            TransView_vShowLine(2, EM_DTYPE_NORMAL,EM_ALIGN_CENTER, (char*)"Airplane Mode Open");
            KB_nWaitKeyMS(3*1000);
        }else if(autoConnect != 1 && Is_Valid_Network()!= RET_OK){
            OsLog(LOG_ERROR,"=======Network Connecting\n");
            TransView_vShowLine(2, EM_DTYPE_NORMAL,EM_ALIGN_CENTER, (char*)"Network Connecting");
            OsGetApnParams(szApn,szApnName,szApnPwd);
            OsWlLogin(szApn,szApnName,szApnPwd,0,15*1000,0,0);
        }else{
            OsLog(LOG_ERROR,"=====Wait For The Connection\n");
            TransView_vShowLine(2, EM_DTYPE_NORMAL,EM_ALIGN_CENTER, (char*)"Wait For The Connection");
            count = 10;
            while(count > 0){
                ret = Is_Valid_Network();
                OsLog(LOG_ERROR,"OsWlCheck = %d",ret);
                if(ret != RET_OK){
                    OsSleep(1000);
                    count --;
                    continue;
                }else{
                    break;
                }
            }
        }
    }
}

void Pinpad_Init(){
    PR_UINT8 param[8] = {0};
    PR_INT32 ret;
    unsigned char tpk[] = {"\x7A""\xF9""\x0F""\x71""\xFE""\xC0""\x8D""\x2E""\x5F""\x34""\x23""\xD3""\xBC""\x6C""\x94""\x34"};
    unsigned char tpkCkv[] = {"\xE1""\x8D""\xE2""\x5E""\xCB""\xCF""\x15""\x91"};
    
    unsigned char tdk[] = {"\x5F""\x34""\x23""\xD3""\xBC""\x6C""\x94""\x34""\x48""\x49""\x16""\x0C""\x0F""\x59""\xAB""\x79"};
    unsigned char tdkCkv[] = {"\x6B""\x94""\x18""\x19""\x1F""\xDA""\x83""\xC6"};

    unsigned char tak[] = {"\x5D""\xEB""\x51""\x54""\xAA""\xCA""\xBB""\xAF""\x5D""\xEB""\x51""\x54""\xAA""\xCA""\xBB""\xAF"};
    unsigned char takCkv[] = {"\x34""\x5E""\xF2""\x50""\x64""\x17""\x13""\x97"};


    //ret = MANAGE_GetSysParam(PARAM_ISFIRST_RUNNINT, param, sizeof(param));
    //if(ret == PR_FAILD || (ret == PR_NORMAL && memcmp(param,"1",1) != 0)){
        ret = Business_WriteKey(PED_TLK,0,PED_TMK,1,(PR_UINT8 *)YL_MK,16,NULL,0,NULL);
        if(ret != PR_NORMAL){
            OsLog(LOG_ERROR,"MK write fail");
        }else{
            OsLog(LOG_ERROR,"MK write success");
        }

        ret = Business_WriteKey(PED_TMK,1,PED_TPK,1,tpk,16,tpkCkv,8,NULL);
        if(ret != PR_NORMAL){
            OsLog(LOG_ERROR,"TPK write fail");
        }else{
            OsLog(LOG_ERROR,"TPK write success");
        }

        ret = Business_WriteKey(PED_TMK,1,PED_TDK,1,tdk,16,tdkCkv,8,NULL);
        if(ret != PR_NORMAL){
            OsLog(LOG_ERROR,"TDK write fail");
        }else{
            OsLog(LOG_ERROR,"TDK write success");
        }

        ret = Business_WriteKey(PED_TMK,1,PED_TAK,1,tak,8,takCkv,8,NULL);
        if(ret != PR_NORMAL){
            OsLog(LOG_ERROR,"TAK write fail");
        }else{
            OsLog(LOG_ERROR,"TAK write success");
            MANAGE_SetSysParam(PARAM_ISFIRST_RUNNINT, (char*)"1", 1);
        }
    //}
}

void SetDefaultIccParamForTest(){
	EmvL2_SetDefaultAidAndCapk();
}

int App_nInitialization()
{
    char szAppId[] = "linux_pos_app";
    OsLogSetTag(szAppId);  //set logcat tag
    signalHandle();
	DisplayInit();   //UI init
    DB_bInit(szAppId); //database init
    EmvL2_Init();   //EMV init
    larktms_init(); // TMS init
    //TEST write Ic card params and capk
    SetDefaultIccParamForTest();
    //TEST write cup mK、tpk tdk  tak
    //Pinpad_Init();
    //END TEST

    WirelessInit();


    
    Ntp_TimeSync_Proc();
	return 0;
}

int Func_Card_Test()
{
    PR_INT8 *pListItem[] = { (char *)"Magnetic",(char *)"IC Card",(char *)"Rfid Card"};
    PR_INT32 nNumOfItems = sizeof(pListItem) / sizeof(PR_INT8 *);
    PR_INT32 nRet;
    PR_INT32 nGetIndex;

    while(1)
	{
        TransView_vClearAllScreen();
	    nRet = TransView_nShowTableViewEx((char*)"MAIN MENU",(PR_INT8**)pListItem, nNumOfItems, 60,  &nGetIndex);
        if(nRet == PR_NORMAL){
            switch(nGetIndex){
                case 0:
                     MagcardTest();
                    break;
                case 1:
                    IccTest();
                    break;
                case 2:
                    RfidTest();
                    break;
            }
        }else if(nRet == RET_DRPOT_CANCEL || nRet == RET_DPORT_TIMEOUT){
            break;
        }
    }
    return PR_NORMAL;
}


int Func_Other_Test()
{
    PR_INT8 *pListItem[] = { (char *)"OTA TEST"};
    PR_INT32 nNumOfItems = sizeof(pListItem) / sizeof(PR_INT8 *);
    PR_INT32 nRet;
    PR_INT32 nGetIndex;

    while(1)
	{
        TransView_vClearAllScreen();
	    nRet = TransView_nShowTableViewEx((char*)"MAIN MENU",(PR_INT8**)pListItem, nNumOfItems, 60,  &nGetIndex);
        if(nRet == PR_NORMAL){
            switch(nGetIndex){
                case 0:
                    Func_OTA_Test();
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
    PR_INT8 *pListItem[] = { (char *)"Sale",(char *)"Sign in",(char *)"Card Test",(char *)"Print",(char *)"Wireless",(char *)"WIFI",(char *)"LED",(char *)"About",(char *)"Other" };
    PR_INT32 nNumOfItems = sizeof(pListItem) / sizeof(PR_INT8 *);
    PR_INT32 nRet;
    PR_INT32 nGetIndex;

    while(1)
	{
        TransView_vClearAllScreen();
	    nRet = TransView_nShowTableViewEx((char*)"MAIN MENU",(PR_INT8**)pListItem, nNumOfItems, 60,  &nGetIndex);
        if(nRet == PR_NORMAL){
            switch(nGetIndex){
                case 0:
                    CousumeTransProc();
                    break;
                case 1:
                    SignIn_Unionpay(); 
                    break;
                case 2:
                    Func_Card_Test();
                    break;
                case 3:
                    PrintTest(1);
                    break;
                case 4:
                    wirelessTest();
                    break;
                case 5:
                    wifiTest();
                    break;
                case 6:
                    LedTest();
                    break;
                case 7:
                    AboutTest();
                    break;
                case 8:
                    Func_Other_Test();    
                    break;
            }
        }else if(nRet == RET_DRPOT_CANCEL || nRet == RET_DPORT_TIMEOUT){
            break;
        }
    }
    return PR_NORMAL;
}


int Idle_nEntery()
{   
    int nRet;

    //SignIn_Unionpay();
	while(1)
	{
		//idle
		TransView_vShowLogoView();
        //ota msg
        ShowUpdateMsgBox();
		//check key
		nRet = KB_nWaitKeyMS_Ex(500);
		if(nRet == EM_KEY_ENTER){
            TransEntery_nDoTransInDefaultEntry();
        }
        else if (nRet == EM_KEY_CANCEL)
        {         
            // if (KB_nWaitKeyMS(300) != EM_KEY_0)
            //     continue;
         
            // if (KB_nWaitKeyMS(300) != EM_KEY_8)
            //     continue;
         
            // if (KB_nWaitKeyMS(300) != EM_KEY_3)
            //     continue;
            if (RET_DPORT_OK == TransView_nDoMessageBox((char *)"Warning", BOX_STYLE_OK | BOX_STYLE_CANCEL, VIEW_TIMEOUT_FOREVER, (char *)"Exit App?"))
            {
                return PR_NORMAL;
            }
        }
	}

	return nRet;
}

extern "C" int ymframwork_init(int argc, char *argv[]);
extern "C" int ymframwork_finish(int ret);
int main(int argc, char *argv[])
{
    PR_INT32 ret;
    ymframwork_init(argc, argv);//system api
    App_nInitialization();
    Idle_nEntery();
    ymframwork_finish(ret);//system api
    return 0;
}


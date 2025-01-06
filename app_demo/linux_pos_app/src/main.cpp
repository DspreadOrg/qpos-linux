#include "app_includes.h"
#include <signal.h>

#include "proj_sdk.h"


static pthread_t lvgl_tick_tid;
static pthread_t lvgl_tid;

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


void lvgl_disp_pause(int runstate, void *par)
{
    disp_disable_update();
}
void lvgl_disp_resume(int runstate, void *par)
{
    disp_enable_update();
}

int main(int argc, char *argv[])
{
    PR_INT32 ret;
    ymframwork_init(argc, argv);//system api
      Disp_vRegisterPauseCallBack(lvgl_disp_pause);
    Disp_vRegisterResumeCallBack(lvgl_disp_resume);
    Disp_vInit(); //UI  init
    Disp_vSetStatusBarStatus(0);
    OsSetSysSleepStatus(1); //暂时设置不允许休眠

    char szAppId[] = "linux_pos_app";
    OsLogSetTag(szAppId);  //set logcat tag
    DB_bInit(szAppId); //database init
    signalHandle();

    static s_Trans_CallbackFunc_t  transCbk;
    transCbk.dispmsg_cb = emvPorcessDisplay;
    transCbk.emv_selectapp_cb = emvSelectMultiApp;
    transCbk.maskPinDisp_cb = emvPinDisp;
    transCbk.onlineprocess_cb = emvOnlineProcess;
    transCbk.traderesult_cb = emvTransResult;
    transCbk.getcardinfo_cb = NULL;

    emv_trans_cb_init(&transCbk);
    thread_init();
    if (pthread_create(&lvgl_tick_tid, NULL, tick_thread, NULL) != 0)
        printf("can't create thread\n");
        
    appStart();

    ymframwork_finish(ret);//system api
    return 0;
}


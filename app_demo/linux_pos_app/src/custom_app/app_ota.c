#include "app_ota.h"
#include "tms_cfg.h"

static char sg_disp_init=0;
void DrawDownloadPercent(int starty,int height,int dispPercent){
    if(sg_disp_init==0){
        POINT_T lineStart;
        POINT_T lineEnd;

        lineStart.X = 0;
        lineStart.Y=  starty;
        
        lineEnd.X = 319;
        lineEnd.Y=  starty;
        
        Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);

        lineStart.Y=  starty+height;
        lineEnd.Y=  starty+height;

        Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);

        lineStart.X = 319;
        lineStart.Y=  starty;
        lineEnd.X = 319;
        lineEnd.Y=  starty+height;
        Disp_vDrawLineAt(lineStart, lineEnd, EM_LINETYPE_SOLID ,EM_COLOR_BLACK);
        sg_disp_init=1;
    }    
    Disp_vDrawRectWithColor(0,starty,320*dispPercent/100,height,0x00FF00);

}
void TmsDispCallback(u32 id,u32 offset,u32 Length,void *pMsg)
{
    u32 percent ;
    TRACE(DBG_TRACE_LVL,"Dspread: %s:id=%d\r\n",__FUNCTION__,id);
    switch(id)
    {
        case OTA_SYNC_SERVER:
            break;
        case OTA_PROCESSING:
            percent = offset*100/Length;
            TransView_vClearLine(2);
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Downloading...");            
            DrawDownloadPercent(150,20,percent);            
            break;
        case OTA_START_DOWNLOAD:
            sg_disp_init=0;
            TransView_vClearAllScreen();
            TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Down app");
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz wait");            
            break;
        case OTA_UPDATE_FINISH:
            
            break;
        case OTA_FIRMWARE_VERIFY:
            TransView_vClearAllScreen();
            TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"In Verify");
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz wait...");            
            break;
        case OTA_FIRMWARE_VERIFY_FAIL:
            TransView_vClearAllScreen();
            TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Verify Fail!");
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Plz check the firmware!!!");            
            break;
        case OTA_OTHER:
            break;
        case OTA_SYNC_ERROR_MSG:
            break;
        case OTA_SYNC_PARAM_DATA:
            break;
        case OTA_NO_AVAILABLE_FIRMWARE:
            break;
        default:
            break;
    }
}
void dispOtaCheckResult() 
{
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"OTA");
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"New App needs to be updated.");
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Update it now?");

    TransView_vShowLine(5,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"[Enter]Down[Cancel]Return");
    
    int key =0;
    while(1){
        int key = KB_nWaitKeyMS_Ex(60*1000);
        if(key == EM_KEY_ENTER){
            Firmware_OTA();
            return ;
        }
        else if (key == EM_KEY_CANCEL){         
            return ;
        }
    }
}

void Firmware_OTA_Check()
{
    set_user_appver(APP_VERSION);
 
    Rc_t result = RC_FAIL;
    TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"OTA");
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"OTA checking...");

    result = tms_client_check(TMS_FW_HEART_CUSTOM_URL);
    if(result == RC_DOWN){
        dispOtaCheckResult();
    }
    else if(result == RC_SUCCESS){
        TransView_vClearAllScreen();
        TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"OTA");
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"No available firmware!");
    }
    else if(result == RC_NET_ERR){
        TransView_vClearAllScreen();
        TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"OTA");
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"OTA Check Fail.");
    }else{
        TransView_vClearAllScreen();
        TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"OTA");
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"No available firmware.");
    }

    KB_nWaitKeyMS(5*10000);
}
void Firmware_OTA()
{
    Rc_t result = RC_FAIL;
    result = tms_client(TMS_FW_HEART_CUSTOM_URL, true);
    if(result == RC_FAIL)
    { 
        TransView_vClearLine(2);
        TransView_vClearLine(3);
        Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"no available firmware!");
    }
    else if(result == RC_NET_ERR)
    {
        TransView_vClearLine(2);
        TransView_vClearLine(3);
        Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Net error! Plz check.");
    }
    else if(result == RC_DOWN)
    {
        TransView_vClearLine(2);
        TransView_vClearLine(3);
        Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"firmware verdify fail!");        
    }
    else if(result == RC_FAIL)
    {
        TransView_vClearLine(2);
        TransView_vClearLine(3);
        Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Ota update fail!");
    }else{
        TransView_vClearLine(2);
        TransView_vClearLine(3);
        Disp_vShowStr(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"No Available Firmware");
    }

    KB_nWaitKeyMS(5*10000);
}

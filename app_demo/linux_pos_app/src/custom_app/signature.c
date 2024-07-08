#include "appinc.h"
#include "signature.h"


int getSignData(PR_UINT8* elesign,PR_INT32 elceSignLen){
    //todo
    return 0;
}
int checkSignValidate(PR_UINT8* elesign,PR_INT32 elceSignLen){
    int i =0;
    int cnt =0;
    int percent = 20;
    int limit = 300*140/8*percent;
    while(i<elceSignLen){
        if(elesign[i] != 0x00){
           cnt += 1; 
        }
        if(cnt > limit){
            return 0;
        }
        i++;
    }
    return 1;
}
int Sign_Get(char* amt,char* date,char*pan){

    PR_INT32 ret;
    PR_INT32 nElecSignRet;
    PR_UINT8 temp[256];
    PR_INT8 FilePath[128] = {0};
	PR_INT8 AppPath[128] = {0};

    PR_UINT8 elceSignBuf[7*1024] ={0};
    PR_INT32 elceSignLen = 0;    
    PR_INT32 elceSignTimeOut = 60;

    //判断CVM TODO


RESIGN:
    Disp_vClearPort();
    Disp_vShowStr(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"ElecSign");

    nElecSignRet = Disp_nElecSign(elceSignBuf,&elceSignLen,NULL,elceSignTimeOut,10,60,300,140,1);

    dbg_printf("nElecSignRet=%d",nElecSignRet);
    dbg_printf("elceSignLen=%d",elceSignLen);
    if(nElecSignRet != 0){
        Disp_vClearPort();
        Disp_vShowStr(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"ElecSign");
        Disp_vShowStr(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"ElecSign Failed");
        KB_nWaitKeyMS(3*1000);

        return -1;
    }
    
    if(checkSignValidate(elceSignBuf,elceSignLen)){
        memset(elceSignBuf,0,sizeof(elceSignBuf));
        elceSignLen = 0;
        goto RESIGN;
    }

    Disp_vClearPort();
    Disp_vShowStr(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"ElecSign");
    Disp_vShowStr(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"ElecSign Success!");
    KB_nWaitKeyMS(3*1000);
    return 0;
}
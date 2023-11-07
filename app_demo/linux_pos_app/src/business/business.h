#pragma once

#include "typedefine.h"
#include "emvdeviceimpl.h"

#ifdef __cplusplus
extern "C"
{
#endif

void MagcardTest();

void IccTest();

void RfidTest();

void PrintTest();

int ppp_connect_test();

void wirelessTest();

void wifiTest();

PR_INT32 CousumeTransProc();

PR_INT32 EmvL2_Init();

PR_INT32 EmvL2_Proc(EmvTransParams_t emvTransParams);

void EmvL2_SetDefaultAidAndCapk();

PR_INT32  PubDownIcParams();

PR_INT32  PubDownPubilcKey();

PR_INT32 SignIn_Unionpay ();



#ifdef __cplusplus
}
#endif
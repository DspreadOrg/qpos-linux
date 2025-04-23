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

void PrintTest(int nCount);

int ppp_connect_test();

void wirelessTest();

void wifiTest();

void LedTest();

void AboutTest();

PR_INT32 CousumeTransProc();

PR_INT32 EmvL2_Init();

PR_INT32 EmvL2_Proc(EmvTransParams_t emvTransParams);

void EmvL2_SetDefaultAidAndCapk();

PR_INT32  PubDownIcParams();

PR_INT32  PubDownPubilcKey();

PR_INT32 SignIn_Unionpay();

void Freelist(int nItemCount, char **list);
char** ConvertList(int nItemCount,ST_WifiApInfo * Aps,char* pWifiName);
PR_INT32 Func_OTA_Test(void);
int cat1_tcp_connect(int *socket_fd, char *ip, int port);
int cat1_tcp_disconnect(int socket_fd);
int cat1_tcp_send_msg(int socket_fd, char *sendmsg, ssize_t sendlen, int timeout);
int cat1_tcp_recv_msg(int socket_fd, char *recvmsg, ssize_t requestlen, int timeout);
#ifdef __cplusplus
}
#endif
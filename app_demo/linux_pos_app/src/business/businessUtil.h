#pragma once
#include "typedefine.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PR_ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))

#define PARAM_ISFIRST_RUNNINT    (char*)"is_first_running" 
#define PARAM_TRADE_NUM          (char*)"trace_num" 
#define PARAM_BATCH_NUM          (char*)"batch_num" 
#define PARAM_EMV_TSC          (char*)"emv_tsc" 

PR_INT32 MANAGE_GetSysParam(PR_INT8 *thePosParamKey, void* theBuffer, PR_UINT32 theBufferLength);

PR_INT32 MANAGE_SetSysParam(PR_INT8 *thePosParamKey, void* theData, PR_UINT32 theDataLength);

PR_INT32 Business_GetTradeNum(PR_INT8 * szTrace,PR_INT32 nInBufLen);

PR_INT32 Business_GetBatchNum(PR_INT8 * szTrace,PR_INT32 nInBufLen);

PR_INT32 Business_GetTradeTsc(PR_INT8 * pTsc,PR_INT32 nInBufLen);

PR_INT32 Business_GetTermSn(PR_INT8 * szParam,PR_INT32 nInBufLen);

PR_INT32 Business_WriteKey(PR_UINT8 SrcKeyType,PR_UINT8 SrcKeyIdx,PR_UINT8 DstKeyType,PR_UINT8 DstKeyIdx,PR_UINT8 *pKey,PR_INT32 keyLen,PR_UINT8 *pKcv,PR_INT32 kcvLen,PR_UINT8 *pKsn);

PR_INT32 Business_CalcMac(PR_INT32 nKeyIdx, PR_UINT8 *pDataIn, int nDataInLen, PR_UINT8 *pMac);

void Business_getAppPath(PR_INT8* pPath,PR_INT32 nBufLen);

PR_INT32 Is_Valid_Network();

void PrintOrder(int nCount);

#ifdef __cplusplus
}
#endif
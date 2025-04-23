#include "appinc.h"

// Parameter record table
#define DB_PARAM "ParamTbl"
#define PARAM_MAX_LEN 128

typedef struct _ParamRecord
{
	PR_INT8 ID[32];
    PR_INT32 nParamLen;
	PR_INT8 Paramdata[PARAM_MAX_LEN];               
}ParamRecord;

#define PARAM_REC_LENGTH sizeof(ParamRecord) 



PR_INT32 MANAGE_GetSysParam(PR_INT8 *thePosParamKey, void* theBuffer, PR_UINT32 theBufferLength)
{
	HANDLE ptTable = PR_NULL;
	int errcode;

	ptTable = DB_hOpen(DB_PARAM,PARAM_REC_LENGTH);
	if(ptTable == PR_NULL)
	{
		OsLog(LOG_ERROR,"DB_PARAM Open Failed");
		return PR_FAILD;
	}
	ParamRecord record;
    memset(&record,0x0,sizeof(ParamRecord));
	if(sizeof(ParamRecord) != DB_nReadRecByKey(ptTable, thePosParamKey, sizeof(ParamRecord), (PR_INT8 *)&record))
	{
		errcode = DB_nGetLastError(ptTable);
		OsLog(LOG_ERROR,"Param ID:%s Errcode:%d",thePosParamKey,errcode);
		DB_vClose(ptTable);
		if(errcode == RET_INTERFACE_NOT_REC){
			return PR_NOT_REC;
		}
		return PR_FAILD;
	}
	if(theBufferLength < record.nParamLen)
	{
		OsLog(LOG_ERROR,"Param ID:%s The Buffer Length Small ",thePosParamKey);
		DB_vClose(ptTable);
		return PR_FAILD;
	}
	memcpy(theBuffer,record.Paramdata,record.nParamLen);
	DB_vClose(ptTable);

	return PR_NORMAL;
}

PR_INT32 MANAGE_SetSysParam(PR_INT8 *thePosParamKey, void* theData, PR_UINT32 theDataLength)
{
	HANDLE ptTable = PR_NULL;
	int errcode;

    ptTable = DB_hOpen(DB_PARAM,PARAM_REC_LENGTH);
	if(ptTable == PR_NULL)
	{
		OsLog(LOG_ERROR,"DB_PARAM Open Failed");
		return PR_FAILD;
	}

	ParamRecord record;
    memset(&record,0x0,sizeof(ParamRecord));
	if(0 >= DB_nReadRecByKey(ptTable, thePosParamKey, sizeof(ParamRecord), (PR_INT8 *)&record))
	{
		errcode = DB_nGetLastError(ptTable);
		if(errcode != RET_INTERFACE_NOT_REC){
			OsLog(LOG_ERROR,"Param ID:%s Errcode:%d",thePosParamKey,DB_nGetLastError(ptTable));
			DB_vClose(ptTable);
			return PR_FAILD;
		}
// Append
		memset(&record,0x0,sizeof(ParamRecord));
		memcpy(record.ID,thePosParamKey, strlen(thePosParamKey));
		memcpy(record.Paramdata,theData, theDataLength);
		record.nParamLen = theDataLength;

		PR_UINT32 nWritenLen = DB_nAppendRecByKey(ptTable, thePosParamKey,(PR_INT8 *)&record, sizeof(ParamRecord));
		if(nWritenLen <= 0)
		{
			OsLog(LOG_ERROR,"Param ID:%s Errcode:%d",thePosParamKey,DB_nGetLastError(ptTable));
			DB_vClose(ptTable);
			return PR_FAILD;
		}
		DB_vClose(ptTable);
		return PR_NORMAL;
	}
// Replace
	if(theDataLength > PARAM_MAX_LEN)
	{
		DB_vClose(ptTable);
		OsLog(LOG_ERROR,"Param ID:%s The DataLength > Max Length",thePosParamKey);
		return PR_FAILD;
	}
	memset(record.Paramdata,0x0,sizeof(record.Paramdata));
	memcpy(record.Paramdata,theData, theDataLength);
	record.nParamLen = theDataLength;
	if(0 >= DB_nReplaceRecByKey(ptTable, thePosParamKey, sizeof(record),(PR_INT8 *)&record))
	{
		OsLog(LOG_ERROR,"Param ID:%s Errcode:%d",thePosParamKey,DB_nGetLastError(ptTable));
		DB_vClose(ptTable);
		return PR_FAILD;
	}
	DB_vClose(ptTable);

	return PR_NORMAL;
}

PR_INT32 Business_GetTradeNum(PR_INT8 * szTrace,PR_INT32 nInBufLen)
{
	PR_INT32 nWaterNo;
    PR_INT32 nWaterNoNew;
    PR_INT32 ret = EXCEPTION;
	PR_UINT8 szBuffer[8];
    
    if(nInBufLen < 6){
         return PR_FAILD;
    }
    BUFCLR(szBuffer);
    if (PR_NORMAL != MANAGE_GetSysParam(PARAM_TRADE_NUM, szBuffer,sizeof(szBuffer)))
    {
// No flow number exists
        nWaterNo = 1;
    }else{
		nWaterNo = atoi((PR_INT8 *)szBuffer);
		if (nWaterNo > 999999 || nWaterNo == 0)
		{
			nWaterNo = 1;
		}
	}
    
    nWaterNoNew = nWaterNo+1;
    if (nWaterNoNew > 999999)
    {
        nWaterNoNew = 1;
    }
    BUFCLR(szBuffer);
    sprintf((char *)szBuffer, "%06d",nWaterNoNew);
    if(PR_NORMAL != MANAGE_SetSysParam(PARAM_TRADE_NUM, szBuffer,strlen((char*)szBuffer))){
        dbg_printf("nWaterNoNew %d write err!",nWaterNoNew);
        return PR_FAILD;
    }
	dbg_printf("cur  nWaterNo %d",nWaterNo);
	sprintf(szTrace, "%06d", nWaterNo);
	return PR_NORMAL;
}


PR_INT32 Business_GetBatchNum(PR_INT8 * szTrace,PR_INT32 nInBufLen)
{
	PR_INT32 nWaterNo;
    PR_INT32 ret = EXCEPTION;
	PR_UINT8 szBuffer[8] = {0};
    
    if(nInBufLen < 6){
         return PR_FAILD;
    }
    BUFCLR(szBuffer);
    if (PR_NORMAL != MANAGE_GetSysParam(PARAM_BATCH_NUM, szBuffer,sizeof(szBuffer)))
    {
        nWaterNo = 1;
		BUFCLR(szBuffer);
		sprintf((char *)szBuffer, "%06d",nWaterNo);
		if(PR_NORMAL != MANAGE_SetSysParam(PARAM_BATCH_NUM, szBuffer,strlen((char*)szBuffer))){
			dbg_printf("nWaterNo %d write err!",nWaterNo);
			return PR_FAILD;
		}
    }else{
		nWaterNo = atoi((PR_INT8 *)szBuffer);
		if (nWaterNo > 999999 || nWaterNo == 0)
		{
			nWaterNo = 1;
			BUFCLR(szBuffer);
			sprintf((char *)szBuffer, "%06d",nWaterNo);
			if(PR_NORMAL != MANAGE_SetSysParam(PARAM_BATCH_NUM, szBuffer,strlen((char*)szBuffer))){
				dbg_printf("nWaterNo %d write err!",nWaterNo);
				return PR_FAILD;
			}
		}
	}
	dbg_printf("cur batch %d",nWaterNo);
	sprintf(szTrace, "%06d", nWaterNo);
	return PR_NORMAL;
}

PR_INT32 Business_GetTradeTsc(PR_INT8 * pTsc,PR_INT32 nInBufLen)
{
	PR_INT32 nWaterNo;
    PR_INT32 nWaterNoNew;
    PR_INT32 ret = EXCEPTION;
	PR_UINT8 szBuffer[8];
    
    if(nInBufLen < 4){
         return PR_FAILD;
    }
    BUFCLR(szBuffer);
    if (PR_NORMAL != MANAGE_GetSysParam(PARAM_EMV_TSC, szBuffer,sizeof(szBuffer)))
    {
// tsc does not exist
        nWaterNo = 1;
    }else{
		nWaterNo = atoi((PR_INT8 *)szBuffer);
		if (nWaterNo > 99999999 || nWaterNo == 0)
		{
			nWaterNo = 1;
		}
	}
    
    nWaterNoNew = nWaterNo+1;
    if (nWaterNoNew > 99999999)
    {
        nWaterNoNew = 1;
    }
    BUFCLR(szBuffer);
    sprintf((char *)szBuffer, "%08d",nWaterNoNew);
    if(PR_NORMAL != MANAGE_SetSysParam(PARAM_EMV_TSC, szBuffer,strlen((char*)szBuffer))){
        dbg_printf("nWaterNoNew %d write err!",nWaterNoNew);
        return PR_FAILD;
    }
	dbg_printf("cur TSC%d",nWaterNo);
	BUFCLR(szBuffer);
	sprintf((char*)szBuffer, "%08d", nWaterNo);
	PR_nUtilAsc2Bcd(8,(char*)szBuffer,EM_ALIGN_RIGHT,4,(PR_INT8*)pTsc);
	return PR_NORMAL;
}

PR_INT32 Business_GetTermSn(PR_INT8 * szParam,PR_INT32 nInBufLen){
    memcpy(szParam,(char*)TERM_SN,15);
    return PR_NORMAL;
}

PR_INT32 Business_WriteKey(PR_UINT8 SrcKeyType,PR_UINT8 SrcKeyIdx,PR_UINT8 DstKeyType,PR_UINT8 DstKeyIdx,PR_UINT8 *pKey,PR_INT32 keyLen,PR_UINT8 *pKcv,PR_INT32 kcvLen,PR_UINT8 *pKsn){
    PR_UINT8 tmep[2048] = {0};
    PR_INT32 offset = 0;
	PR_INT32 nRet = -1;

	nRet = OsPedOpen();
    if(nRet != RET_OK){
        return PR_FAILD;
    }
    memset(tmep,0x0,sizeof(tmep));
    tmep[offset] = 0x03;
    offset += 1;
// SrcKeyType
    tmep[offset] = SrcKeyType;
    offset += 1;
// SrcKeyIdx
    tmep[offset] = SrcKeyIdx;
    offset += 1;
// DstKeyIdx：
    tmep[offset] = DstKeyIdx;
    offset += 1;
// 7 bytes Reserved domain, random number
    offset += 7;
// DstKeyType
    tmep[offset] = DstKeyType;
    offset += 1;
// DstKeyLen
    tmep[offset] = (uchar)keyLen;
    offset += 1;
// Dstkeyvalue
    memcpy(tmep+offset,pKey,keyLen);
    offset += 24;
// KCVMODE
    if(kcvLen <= 0){
        tmep[offset] = 0x0;
    }else{
        tmep[offset] = 0x1;
    }
    offset += 1;
// KCVDATA
    offset += 128;
// Kcv
    if(kcvLen > 0){
        memcpy(tmep+offset,pKcv,kcvLen);
    }
    offset += 8;
// Fill in random numbers
    if(pKsn != NULL){
        memcpy(tmep+offset,pKsn,10);
    }
    offset += 10;
    
	nRet = OsPedWriteKey(tmep);
    if(nRet != RET_OK){
        nRet =  PR_FAILD;
    }else{
		nRet =  PR_NORMAL;
	}
	OsPedClose();
	return nRet;
}

PR_INT32 Business_CalcMac(PR_INT32 nKeyIdx, PR_UINT8 *pDataIn, int nDataInLen, PR_UINT8 *pMac){
	PR_INT32 nRet;
// Mac calculation
	if(pDataIn == NULL || nDataInLen <= 0 || pMac == NULL){
		return PR_FAILD;
	}
	nRet = OsPedOpen();
    if(nRet != RET_OK){
        return PR_FAILD;
    }
    nRet = OsPedGetMac(1, pDataIn, nDataInLen, pMac, 0x03);
    if(nRet != RET_OK){
       	nRet =  PR_FAILD;
    }else{
		nRet =  PR_NORMAL;
	}
	OsPedClose();
	return nRet;
}
// Get the app installation directory
void Business_getAppPath(PR_INT8* pPath,PR_INT32 nBufLen){
	getcwd(pPath, nBufLen);
}

PR_INT32 Is_Valid_Network(){
	PR_INT8 ssid[256] = {0};
    PR_INT8 bssid[20] = {0};
    PR_INT32 rssi = 0;
	PR_INT32 ret;

	OsWlLock();
    if(OsWlCheck() == RET_OK){
		return PR_NORMAL;
	}
    OsWifiOpen();
	ret = OsWifiCheck(ssid,bssid,&rssi);
    if(ret == 0 && strlen(ssid) > 0){
        return PR_NORMAL;
    }

	return PR_FAILD;
}
   
void PrintOrder(int nCount)
{
    PR_INT32 ret;
    PR_UINT8 temp[256];
    PR_INT8 FilePath[128] = {0};
	PR_INT8 AppPath[128] = {0};
    
    if(nCount <= 0){
        return;
    }
    ret = OsPrnOpen(PRN_REAL,NULL);
    if(ret != RET_OK){
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Open Device Failed");
        return;
    }
    OsPrnSetGray(1);
    OsPrnSetSpace(0,4);
    ret = OsPrnCheck();
    if( ret != RET_OK){
        TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,ret == ERR_PRN_BUSY?(char*)"Printer Busy":
                    ret == ERR_PRN_PAPEROUT?(char*)"Printer Paper Out":
                    ret == ERR_PRN_OVERHEAT?(char*)"Printer Overheat":
                    ret == ERR_PRN_OVERVOLTAGE?(char*)"Printer Overvoltage ":
					ret == ERR_BATTERY_VOLTAGE_TOO_LOW?(char*)"Printer Voltage Too Low":(char*)"Printer Exception");
        goto exit;
    }
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Printing");
    for(int i = 0;i< nCount;i++){
        OsPrnReset();
        Business_getAppPath(AppPath,sizeof(AppPath));
        sprintf(FilePath,"%s/res/yl.bmp",AppPath);
        OsLog(LOG_WARN,"bmp path： %s",FilePath);
        OsPrnPutImage((unsigned char *)FilePath);
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_RIGHT);
        OsPrnPrintf((char *)"MERCHANT COPY");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"================================================");
        OsPrnPrintf((char *)" ");
        OsPrnPrintf((char *)"MERCHANT NAME");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"START");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"MERCHANT NO",0,0);
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"1234567890");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"TERMINAL NO                OPERATOR NO");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"33333333                01");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"CARD NUMBER");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"6229 0** **** 6101       CUP");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"ISS NO                     ACQ NO");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"ICBC          19992900");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"TRANS TYPE");
        OsPrnSetPrintParams(24,3,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"SALE");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"EXP DATE");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"2015/02 ");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"BATCH NO                     VUCHER NO");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"000001              000016");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"AUTH NO ");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_RIGHT);
        OsPrnPrintf((char *)"867543234321");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"DATE/TIME");
        OsPrnSetPrintParams(24,1,1,ALIGN_TYPE_RIGHT);
        OsPrnPrintf((char *)"2023/08/23 11:22:33");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"AMOUNT");
        OsPrnSetPrintParams(24,3,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"RMB 10.00");
        OsPrnSetPrintParams(16,1,1,ALIGN_TYPE_LEFT);
        OsPrnPrintf((char *)"REFERENCE ");
        OsPrnPrintf((char *)"================================================");
        OsPrnPrintf((char *)"CARDHOLDER SIGNATURE ");
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)"================================================");
        OsPrnPrintf((char *)"I ACKNOWLEDGE SATISFACTORY RECEIPT OF FELATIUE GOODS/SERVICES");
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        OsPrnPrintf((char *)" ");// 5line
        if((ret = OsPrnStart()) != RET_OK){
            TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,ret == ERR_PRN_BUSY?(char*)"Printer Busy":
                        ret == ERR_PRN_PAPEROUT?(char*)"Printer Paper Out":
                        ret == ERR_PRN_OVERHEAT?(char*)"Printer Overheat":
                        ret == ERR_PRN_OVERVOLTAGE?(char*)"Printer Overvoltage ":
                        ret == ERR_BATTERY_VOLTAGE_TOO_LOW?(char*)"Printer Voltage Too Low":(char*)"Printer Exception");
            goto exit;
        }
    }
    OsPrnFeed(48);
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Print Complete");
exit:
    OsPrnClose();
}
   
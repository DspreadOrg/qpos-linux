#include    "appinc.h"


typedef struct __signin_para
{
	char szCustId[16];          /* 商户号                           */
	char szTermId[9];           /* 终端号                           */
	char szTrace[7];            /* 流水号                           */
	char szErrcode[3];			/*平台返回的错误码*/
	char szErrInfo[1024];		//平台返回的错误码的对应错误信息
	char cSetErrorFlag;			//已经设置本地错误码标志
	int nLocalErrcode;			//本地错误码
}SIGNIN_PARA;

/*********************************************************************************
* 功能描述:
		银联卡签到8583打包
* 输入参数:	signin_unionpay_para：签到信息结构体
			szPackStr:打包缓存
*			nPackSize：缓存空间大小
*			nRetLen：返回数据长度
* 输出参数：
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int sign_in_unionpay_pack_8583(SIGNIN_PARA * signin_unionpay_para, char * szPackStr, int nPackSize, int *nRetLen)
{
	ISODataStru m_iso;  
	int nLength;
	char szBuffer[100],szTerminalNo[9],szCustNo[16],szSeqNo[65];
	uchar szBatch[7];
	uint nLen;
	if (szPackStr == NULL || nRetLen == NULL)
	{
		return -1;
	}
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
	//0域
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay,&m_iso, MSG_ID, "0800", 4));
	//11域
	BUFCLR(szBuffer);
	ASSERT_NORMAL(Business_GetTradeNum(szBuffer,sizeof(szBuffer)-1));
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay,&m_iso, TRACE_NO, szBuffer, 6));
	sprintf(signin_unionpay_para->szTrace,"%6.6s",szBuffer);
	//41终端号
	BUFCLR(szTerminalNo);
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay,&m_iso, POS_ID, YL_TERM, 8));
	sprintf(signin_unionpay_para->szTermId,"%8.8s",YL_TERM);
	//42商户号
	BUFCLR(szCustNo);
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay,&m_iso, CUSTOM_ID, YL_MCH, 15));
	sprintf(signin_unionpay_para->szCustId,"%15.15s",YL_MCH);
	//60.1交易类型		60.2交易批次号	60.3网管交易类型
	BUFCLR(szBatch);
	ASSERT_NORMAL(Business_GetBatchNum(szBatch,sizeof(szBatch) - 1));
	BUFCLR(szBuffer);
	sprintf(szBuffer, "00%6.6s003",szBatch);//不支持磁道加密、3DES
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay,&m_iso, 60, szBuffer, 11));
	//62域
	BUFCLR(szBuffer);
	BUFCLR(szSeqNo);
	memcpy(szSeqNo, "3062", 4);
	nLength = sizeof(szSeqNo) - 5;
    Business_GetTermSn(szSeqNo+4,nLength);
	nLen = strlen(szSeqNo);
	memcpy(szBuffer, "Sequence No", 11);
	sprintf(szBuffer + 11, "%02d%s", nLen, szSeqNo);
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay,&m_iso, 62, szBuffer,strlen(szBuffer)));
	//63域操作员
	ASSERT_NORMAL(PubSetBit(Iso8583Def_union_pay, &m_iso, 63 , "01 ", 3));

	nLength = nPackSize;
	ASSERT_NORMAL(PubIsoToStr(Iso8583Def_union_pay, szPackStr, &m_iso, &nLength));
	*nRetLen = nLength;
	dbg_printfWHex((uchar *)szPackStr, *nRetLen, "8583 data");
	return 0;
}

/*********************************************************************************
* 功能描述:
		银联卡签到报文串包校验
* 输入参数:
* 输出参数：
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int pub_sign_in_check_pack(SIGNIN_PARA * signin_unionpay_para, ISODataStru *m_iso)
{
	if (signin_unionpay_para == NULL || m_iso == NULL)
	{
		return -1;
	}
	char szRetTerminal[9], szRetCustom[16], szRetTrace[7];
	int  nLen;

	//校验终端号
	BUFCLR(szRetTerminal);
	nLen = sizeof(szRetTerminal);
	ASSERT_NORMAL(PubGetBit(m_iso, POS_ID, szRetTerminal, &nLen));
	if (nLen)
	{
		if (PubStrcmp(szRetTerminal, signin_unionpay_para->szTermId))
		{
			dbg_printf("Term No Err");
			return -1;
		}
	}
	//校验商户号
	BUFCLR(szRetCustom);
	nLen = sizeof(szRetCustom);
	ASSERT_NORMAL(PubGetBit(m_iso, CUSTOM_ID, szRetCustom, &nLen));
	if (nLen)
	{
		if (PubStrcmp(szRetCustom, signin_unionpay_para->szCustId))
		{
			dbg_printf("Mch No Err");
			return -1;
		}
	}
	//校验流水号
	BUFCLR(szRetTrace);
	nLen = sizeof(szRetTrace);
	ASSERT_NORMAL(PubGetBit(m_iso, TRACE_NO, szRetTrace, &nLen));
	if (nLen)
	{
		if (PubStrcmp(szRetTrace, signin_unionpay_para->szTrace))
		{
			dbg_printf("Trace No Err");
			return -1;
		}
	}
    return 0;
}
/*********************************************************************************
* 功能描述:
		银联卡签到报文应答解析
* 输入参数:	nCustNo:当前商户编号
			signin_unionpay_para：签到参数
			szReadBuf：后台返回的数据
			nReadLen：返回的数据长度
* 输出参数：
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int pub_sign_in_dis_ret_msg(SIGNIN_PARA * signin_para, char * szReadBuf, int nReadLen)
{	
	int ret;
	char szErrInfo[128] = {0};

	if (signin_para == NULL || szReadBuf == NULL || nReadLen == 0)
	{
		return -1;
	}
	char szErrCode[3], szBuffer[512],szTmpBuffer[30];
	uchar szWorkKey[25], szMacKey[25], szWorkKeyChk[5], szMacKeyChk[5], szTrackKey[25], szTrackKeyChk[5], 
		szMasterKey[25], szEmpty[9], szBatch[7], szTime[15];
	int nLen, nWorkKeyLen, nMacKeyLen, nTrackKeyLen, nMasterKeyLen;
	ISODataStru m_iso;
	int i = 0;
	ASSERT_NORMAL(PubDelAllBit(&m_iso));
	ASSERT_NORMAL(PubStrToIso_separate(szReadBuf + PACK_WHEAD_LEN + 2, nReadLen - PACK_WHEAD_LEN - 2, &m_iso, NULL));
	//检查是否串包
	ASSERT_NORMAL(pub_sign_in_check_pack(signin_para, &m_iso));
	//判断错误码
	BUFCLR(szErrCode);
	nLen = sizeof(szErrCode);
	ASSERT_NORMAL(PubGetBit(&m_iso, RET_CODE, szErrCode, &nLen));
	if (nLen != 2)
	{
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Unpacking Exception");
		return -1;
	}
	memcpy(signin_para->szErrcode, szErrCode, 2);
    TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Response Code:%s",szErrCode);
	if (memcmp(signin_para->szErrcode,"00", 2))
	{
		i = 0;
		while ((err_mess[i][0] != '\0') && memcmp(signin_para->szErrcode, err_mess[i], 2))
			i++;
		if (err_mess[i][0] == '\0') /* Unknow Error */
		{
			sprintf(signin_para->szErrInfo, "%s Resp Exception", signin_para->szErrcode);
		}
		else
		{
			PubStrcpy(signin_para->szErrInfo, err_mess[i]);
		}
		return -1;
	}
	//校验密钥
	BUFCLR(szBuffer);
	nLen = sizeof(szBuffer);
	ASSERT_NORMAL(PubGetBit(&m_iso, 62, szBuffer, &nLen));
	if ((nLen != 40) && (nLen != 60) &&(nLen != 24))
	{
		return -1;
	}
	BUFCLR(szWorkKey);
	BUFCLR(szWorkKeyChk);
	BUFCLR(szMacKey);
	BUFCLR(szMacKeyChk);
	BUFCLR(szTrackKey);
	BUFCLR(szTrackKeyChk);

	if (nLen == 24)
	{
		memcpy(szWorkKey, szBuffer, 8);
		memcpy(szWorkKeyChk, szBuffer + 8, 4);
		memcpy(szMacKey, szBuffer + 12, 8);
		memcpy(szMacKeyChk, szBuffer + 20, 4);
		nWorkKeyLen = 8;
		nMacKeyLen = 8;
		nTrackKeyLen = 0;
	}
	else if (nLen == 40)
	{
		memcpy(szWorkKey, szBuffer, 16);
		memcpy(szWorkKeyChk, szBuffer + 16, 4);
		memcpy(szMacKey, szBuffer + 20, 8);// mackey 银联预留了16位,目前只用了8位
		memcpy(szMacKeyChk, szBuffer + 36, 4);
		nWorkKeyLen = 16;
		nMacKeyLen = 8;
		nTrackKeyLen = 0;
	}
	else//nLen == 60
	{
		memcpy(szWorkKey, szBuffer, 16);
		memcpy(szWorkKeyChk, szBuffer + 16, 4);
		memcpy(szMacKey, szBuffer + 20, 8); // mackey 银联预留了16位,目前只用了8位
		memcpy(szMacKeyChk, szBuffer + 36, 4);
		memcpy(szTrackKey, szBuffer + 40, 16);
		memcpy(szTrackKeyChk, szBuffer + 56, 4);
		nWorkKeyLen = 16;
		nMacKeyLen = 8;
		nTrackKeyLen = 16;
	}
	dbg_printfWHex(szWorkKey, nWorkKeyLen,"work key pin");
	dbg_printfWHex(szMacKey, nMacKeyLen,"mac key");
	dbg_printfWHex(szTrackKey, nTrackKeyLen, "track key");

	if (nWorkKeyLen)
	{
		ret = Business_WriteKey(PED_TMK,1,PED_TPK,1,szWorkKey,nWorkKeyLen,szWorkKeyChk,4,NULL);
		if (ret != PR_NORMAL)
		{
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"TPK Write Failed");
			return -1;
		}
	}
	if (nMacKeyLen)
	{
		ret = Business_WriteKey(PED_TMK,1,PED_TAK,1,szMacKey,nMacKeyLen,szMacKeyChk,4,NULL);
		if (ret != PR_NORMAL)
		{
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"TAK Write Failed");
			return -1;
		}
	}
	if (nTrackKeyLen)
	{
		ret = Business_WriteKey(PED_TMK,1,PED_TDK,1,szTrackKey,nTrackKeyLen,szTrackKeyChk,4,NULL);
		if (ret != PR_NORMAL)
		{
			TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"TDK Write Failed");
			return -1;
		}
	}
	//获取批次号
	BUFCLR(szBuffer);
	nLen = sizeof(szBuffer);
	PubGetBit(&m_iso, 60, szBuffer, &nLen); //iso1
	if (nLen > 0)
	{
		BUFCLR(szBatch);
		memcpy(szBatch, szBuffer + 2, 6);
		ASSERT_NORMAL(MANAGE_SetSysParam(PARAM_BATCH_NUM,szBatch,strlen(szBatch)));
	}
	
	//同步日期时间
	ST_TIME SysTime;
	BUFCLR(szTmpBuffer);
	nLen = sizeof(szTmpBuffer);
	ASSERT_NORMAL(PubGetBit(&m_iso, TRADE_DATE, szTmpBuffer, &nLen));
	dbg_printfWHex(szTmpBuffer,nLen,"Date");
	SysTime.Year = 2023;
	BUFCLR(szTime);
	memcpy(szTime,szTmpBuffer,2);
	SysTime.Month = atoi(szTime);
	BUFCLR(szTime);
	memcpy(szTime,szTmpBuffer+2,2);
	SysTime.Day = atoi(szTime);
	
	BUFCLR(szTmpBuffer);
	nLen = sizeof(szTmpBuffer);
	ASSERT_NORMAL(PubGetBit(&m_iso, TRADE_TIME, szTmpBuffer, &nLen));
	dbg_printfWHex(szTmpBuffer,nLen,"Time");
	BUFCLR(szTime);
	memcpy(szTime,szTmpBuffer,2);
	SysTime.Hour = atoi(szTime);
	BUFCLR(szTime);
	memcpy(szTime,szTmpBuffer+2,2);
	SysTime.Minute = atoi(szTime);
	BUFCLR(szTime);
	memcpy(szTime,szTmpBuffer+4,2);
	SysTime.Second = atoi(szTime);
	OsSetTime(&SysTime);
	return 0;
}
/*********************************************************************************
* 功能描述:
		银联卡签到处理
* 输入参数:dis_flag:前台显示还是后台显示的标志 0 前台显示，1 后台显示
* 输出参数：
* 返回值：0 ：成功
		其他：失败
*********************************************************************************/
PR_INT32 SignIn_Unionpay ()
{
	int nLen;
	char szSigninReadBuf[READBUFF_SIZE + 1] = {0};
	char szSigninSendBuf[READBUFF_SIZE + 1] = {0};
	char szBuffer[READBUFF_SIZE + 1] = {0};
	int nSigninSendLen;
	int nSigninReadLen;
	SIGNIN_PARA signin_unionpay_para;
	int nRet;

	TransView_vClearAllScreen();
    TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,(char*)"Sign In");
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Signing In");
	//清除错误标志
	ZERO_STRUCT(signin_unionpay_para);
	BUFCLR(szBuffer);
	nRet = sign_in_unionpay_pack_8583(&signin_unionpay_para, szBuffer, sizeof(szBuffer) -1, &nLen);
	if (nRet){
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Packing Exception");
		goto FAIL;
	}
	BUFCLR(szSigninSendBuf);
	nRet = pub_pack_header(szBuffer, nLen, szSigninSendBuf, &nSigninSendLen);
	if (nRet)
	{
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"8583 Packing Exception");
		goto FAIL;
	}
#ifndef POS_OFFLINE
	BUFCLR(szSigninReadBuf);
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Connect To Server");
	nRet = network_normal_commu(szSigninSendBuf, nSigninSendLen, szSigninReadBuf, &nSigninReadLen);
	if (nRet)
	{
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Communication Failed");
		goto FAIL;
	}
	nRet = pub_sign_in_dis_ret_msg(&signin_unionpay_para, szSigninReadBuf,nSigninReadLen);
	if (nRet)
	{
		goto FAIL;
	}
	//判断是否已经下载过IC卡参数和公钥
	int aidCount = Emv_GetAidTotalNum();
	if (aidCount <= 3)   //默认写入的测试的IC卡参数只有3条，银联平台有4条
	{
		nRet = PubDownIcParams();
		if (nRet)
		{
			goto FAIL;
		}
	}
	int capkCount = Emv_GetCapkTotalNum();
	if (capkCount <= 1)  //默认写入的测试的IC卡公钥只有1条，银联平台有17条
	{
		nRet = PubDownPubilcKey();
		if (nRet)
		{
			goto FAIL;
		}
	}
	TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sign In Success");
	KB_nWaitKeyMS(5*1000);
	return 0;
#else
	TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sign In Success");
	KB_nWaitKeyMS(5*1000);
	return 0;
#endif
FAIL:
    TransView_vShowLine(3,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Sign In Failed");
	KB_nWaitKeyMS(5*1000);
	return -1;
}

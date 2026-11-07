#include "appinc.h"

int gethostbyname_success[200 + 1];


char *err_mess[] = {
	"00交易成功",
	"01请持卡人与发卡银行联系",
	"02交易失败, 请联系发卡行",
	"03无效商户",
	"04此卡被没收",
	"05持卡人认证失败",
	"06交易失败, 请联系发卡行",
	"07没收卡, 请联系收单行",
	"09交易失败, 请重试",
	"11成功, VIP客户",
	"12无效交易",
	"13无效金额",
	"14无效卡号",
	"15此卡无对应发卡方",
	"19交易失败, 请联系发卡行",
	"20交易失败, 请联系发卡行",
	"21该卡未初始化或睡眠卡",
	"22操作有误, 或超出交易允许天数",
	"23交易失败, 请联系发卡行",
	"25没有原始交易, 请联系发卡方",
	"30请重试",
	"31此卡不能受理",
	"33过期卡, 请联系发卡行",
	"34作弊卡, 吞卡",
	"35没收卡, 请联系收单行",
	"36此卡有误, 请换卡重试",
	"37没收卡, 请联系收单行",
	"38密码错误次数超限, 请与发卡方联系",
	"39交易失败, 请联系发卡行",
	"40发卡方不支持的交易类型",
	"41挂失卡, 请没收",
	"42交易失败, 请联系发卡方",
	"43被窃卡, 请没收",
	"44交易失败, 请联系发卡行",
	"45禁止降级",
	"51可用余额不足",
	"52交易失败, 请联系发卡行",
	"53交易失败, 请联系发卡行",
	"54该卡已过期",
	"55密码错",
	"56交易失败, 请联系发卡行",
	"57不允许此卡交易",
	"58发卡方不允许该卡在本终端进行此交易",
	"59卡片校验错",
	"60交易失败, 请联系发卡行",
	"61交易金额超限",
	"62受限制的卡",
	"63交易失败, 请联系发卡行",
	"64交易金额与原交易不匹配",
	"65超出消费次数限制",
	"66交易失败, 请联系收单行或银联",
	"67没收卡",
	"68交易超时, 请重试",
	"75密码错误次数超限",
	"77请向网络中心签到",
	"79 POS终端重传脱机数据",
	"90系统日切, 请稍后重试",
	"91发卡方状态不正常, 请稍后重试",
	"92发卡方线路异常, 请稍后重试",
	"93交易失败, 请联系发卡行",
	"94拒绝,重复交易,请稍后重试",
	"95交易失败, 请稍后重试",
	"96拒绝,交换中心异常,请稍后重试",
	"97终端未登记",
	"98发卡方超时",
	"99PIN格式错,请重新签到",
	"A0MAC校验错,请重新签到",
	"A1转账货币不一致",
	"A2交易成功,请向发卡行确认",
	"A3账户不正确",
	"A4交易成功,请向发卡行确认",
	"A5交易成功,请向发卡行确认",
	"A6交易成功,请向发卡行确认",
	"A7拒绝,交换中心异常,请稍后重试",
	"",
};



/*********************************************************************************
* 功能描述:
		银联卡签到8583打包报文头
* 输入参数:nCustNo：商户编号
			szPackStr：8583报文
			nPackLen：8583报文长度

* 输出参数：szRetBuf：打包后的数据
			nRetLen：返回数据长度
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int pub_pack_header(char * szPackStr, int nPackLen, char * szRetBuf, int *nRetLen)
{
	char szSendStr[READBUFF_SIZE + 1], szTemp[100];
	uchar szTpdu[11], szHeader[13];
	int nLen = 0;
	uint uiLen = 0;
	if (szPackStr == NULL || szRetBuf == NULL || nRetLen == NULL)
	{
		return -1;
	}
	BUFCLR(szSendStr);
	//长度
	szSendStr[nLen++]=(char)((nPackLen + PACK_WHEAD_LEN)/256);
	szSendStr[nLen++]=(char)((nPackLen + PACK_WHEAD_LEN)%256);
	//TPDU
	BUFCLR(szTpdu);
	c_asc2bcd((uchar *)szSendStr + nLen, YL_TPDU, 10, 0);
	nLen += 5;
	//报文头
	BUFCLR(szHeader);
	//应用类别 + 软件总版本号 + 状态状态 + 处理要求
	memcpy(szHeader, "613200", 6); //IC卡金融支付类应用/银联POS统一版规范版本/正常交易状态
	//软件版本号
	if (strlen(SOFTWARE_VERSION) < 4)
	{
		memcpy(szHeader + 6, "320001", 6);
	}
	else
	{
		memcpy(szHeader + 6, "32", 2);
		BUFCLR(szTemp);
		strcpy(szTemp, SOFTWARE_VERSION);
		memcpy(szHeader + 8, szTemp + strlen(szTemp) - 4, 4);
	}
	c_asc2bcd((uchar *)szSendStr + nLen, szHeader, 12, 0);
	nLen += 6;
	//8583报文
	memcpy(szSendStr + nLen, szPackStr, nPackLen);
	nLen += nPackLen;

	memcpy(szRetBuf, szSendStr, nLen);
	*nRetLen = nLen;
	dbg_printfWHex(szRetBuf, nLen, "8583 data");
	return 0;
}

/*********************************************************************************
* 功能描述:
		分解报文头
* 输入参数:pcStr:报文数据
* 输出参数：
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int Pub_DivPacketHead(char *pcStr)
{
	char szBuffer[20], szHead[7];
	char flag;
	
	memset(szBuffer, 0x00, sizeof(szBuffer));
	memset(szHead, 0x00, sizeof(szHead));

	memcpy(szHead, pcStr + PACK_WHEAD_LEN - 6, 6);
	c_bcd2asc((uchar *)szBuffer, (uchar *)szHead, 12, 0);
	/*
	* 终端状态                             1N
	* 正常交易状态:                        0
	* 测试交易状态                         1
	*/
	//正常状态
	dbg_printfWHex(szBuffer, 12, "head");
	if (memcmp(szBuffer + 2 + 2, "0", 1))
	{
		dbg_printf("Term Status Err!");
		return -1;
	}
	/*
	* 处理要求                             1N
	* 无处理要求                           0
	* 下传终端参数                         1
	* 上传终端状态信息                     2
	* 重新签到                             3
	*/
	flag = szBuffer[5] - '0';
	dbg_printf("ProcFlag:%d",flag);
	// gnProcFlag[nCustNo] = flag;

	return NORMAL;
}
/*********************************************************************************
* 功能描述:
		银联卡签到通讯交互
* 输入参数:dis_flag：前台、后台显示标识
			nCustNo：商户编号
			szSendBuf：发送的数据内容
			nSendLen：发送的数据长度
* 输出参数：szReadBuf：接收的数据内容
			nReadLen：接收数据长度
* 返回值：	0：成功
			-1:参数错
			-2：通讯错
			-3：接收数据异常
*    	 其他：失败
*********************************************************************************/
int network_normal_commu(char * szSendBuf, int nSendLen,char * szReadBuf, int * nReadLen)
{
	socket_parameters_t socket_info;
	int nLen,nRet;
	char szBuffer[256];
	if (szSendBuf == NULL || nSendLen == 0 || szReadBuf == NULL || nReadLen == NULL)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}
	OsWlLock();
    nRet = OsWlCheck();
	if(nRet == ERR_WL_NOSIM ){
        OsLog(LOG_ERROR,"Sim Not Insert\n");
		return -2;
    }else if(nRet != RET_OK){
        if(OsWlLogin(NULL,NULL,NULL,0,20*1000,0,0) != RET_OK){
			OsLog(LOG_ERROR,"Connect Failed\n");
			return -2;
		}
    }
	//初始化通讯参数
	ZERO_STRUCT(socket_info);
	nRet = network_init_socket_para(&socket_info, szSendBuf, nSendLen);
	if (nRet)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}
	// dbg_printfWHex(socket_info.sendbuf, socket_info.sendlen,"IP:%s,PORT:%d,连接超时:%d,接收超时:%d,ssl标志:%d,https标识:%d,发送数据",
	// 	socket_info.ip,socket_info.port,socket_info.connect_time, socket_info.recv_time, socket_info.ssl_flag,socket_info.union_https_flag);
	//通讯
	nLen = network_protocol_socket_tcp (&socket_info, READBUFF_SIZE - 1,szReadBuf);
	if (nLen <= 0)
	{
		return nLen;
	}
	dbg_printfWHex(szReadBuf, nLen, "Recv Data");
	//接收数据解析
	if (nLen < PACK_WHEAD_LEN + 2)
	{
		dbg_printf("Len Err");
		return err_sock_errno(SOCK_ERR_STEP_RECV,0);
	}
	if(((szReadBuf[0]<<8)|szReadBuf[1]) != nLen -2)
	{
		dbg_printf("Heard Len Err");
		return err_sock_errno(SOCK_ERR_STEP_RECV,0);
	}
	//报文头解析
	nRet = Pub_DivPacketHead(szReadBuf + 2);
	if (nRet)
	{
		dbg_printf("Len Err");
		return err_sock_errno(SOCK_ERR_STEP_PARSE,0);
	}
	*nReadLen = nLen;
	return 0;
}

int network_init_socket_para(socket_parameters_t *socket_info,char * szSendBuf, int nSendLen)
{
	uint uiLen;
	uchar szUrl[256];
	uchar szUrl_bak[300];
	int nRet;
	if (socket_info == NULL){
		return -1;
	}
	//商户编号
	socket_info->custno = 0;
	
	//公网直连银联 需要SSL
	socket_info->ssl_flag = 1;	
	memcpy(socket_info->ip,(char*)YL_IP,strlen((char*)YL_IP));
	socket_info->port = YL_PORT;
	//连接超时时间
	socket_info->connect_time = 30;
	//接收超时时间
	socket_info->recv_time = 30;
	//直连银联公网标识
	socket_info->union_https_flag = 1;
	//打印
	socket_info->is_show_title = 1;
	//发送数据
	if (szSendBuf != NULL)
	{
		socket_info->sendlen = nSendLen;
		socket_info->sendbuf = szSendBuf;
	}
	//设置默认socket_id值
	socket_info->socket_id = -1;
	return 0;
}


/*****************************************************
*函数说明:预连接处理
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int network_protocol_socket_tcp_pre_connect (socket_parameters_t *parameters)
{
	return protocol_socket_tcp_pre_connect(parameters);
}
/*****************************************************
*函数说明:连接服务器处理，包含处理预连接
*入口参数:parameters： 通讯参数
	pre_connect_flag：预连接标识
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int network_protocol_socket_tcp_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag)
{
	int nRet;
	uchar szBuffer[1024];
	uint uiLen;

	nRet = protocol_socket_tcp_follow_connect(parameters,pre_connect_flag);
	return nRet;
}

/*****************************************************
*函数说明:发送数据处理
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int network_protocol_socket_tcp_write (socket_parameters_t *parameters)
{
	return protocol_socket_tcp_write(parameters);
}
/*****************************************************
*函数说明:接收数据处理
*入口参数:parameters： 通讯参数
		recvlen:最大接收缓存
*出口参数:outbuf：接收到的数据
*返回值:
		>0:成功
		<=0：失败
*****************************************************/
int network_protocol_socket_tcp_read (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	return protocol_socket_tcp_read(parameters, recvlen, outbuf);
}
/*****************************************************
*函数说明:无线通信挂断处理
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		>0:成功
		<=0：失败
*****************************************************/
int network_protocol_socket_tcp_dialoff (socket_parameters_t *parameters)
{
	return protocol_socket_tcp_dialoff(parameters);
}

/* 
 * function: network_protocol_socket_tcp
 * description:   
 * author:
 * IN:  http_parameters_t *parameters->访问参数
 *      recvlen: 接收最大长度 
 * OUT: outbuf: 数据内容
 * return: >0 -  实际接收长度
 *         <= 0 - fail
 */
int network_protocol_socket_tcp (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int nRet;
	uchar szBuffer[1024];
	uint uiLen;
    nRet = protocol_socket_tcp(parameters, recvlen,outbuf);
	return nRet;
}		/* -----  end of function network_protocol_socket_tcp  ----- */


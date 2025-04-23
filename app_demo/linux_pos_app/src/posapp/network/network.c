#include "appinc.h"

int gethostbyname_success[200 + 1];


char *err_mess[] = {
	"00Successful trade",
	"01Please ask the cardholder to contact the issuing bank",
	"02Transaction failed, please contact the issuing bank",
	"03Invalid merchant",
	"04This card has been confiscated",
	"05Cardholder authentication failed",
	"06Transaction failed, please contact the issuing bank",
	"07Confiscate card, please contact the acquiring bank",
	"09Transaction failed, please try again",
	"11success, VIP customers",
	"12Invalid transaction",
	"13Invalid amount",
	"14Invalid card number",
	"15This card does not have a corresponding issuer",
	"19Transaction failed, please contact the issuing bank",
	"20Transaction failed, please contact the issuing bank",
	"21The card has not been initialized or is sleeping",
	"22Incorrect operation or exceeding the allowed trading days",
	"23Transaction failed, please contact the issuing bank",
	"25No original transaction, please contact the issuer",
	"30Please try again",
	"31This card cannot be accepted",
	"33Expired card, please contact the issuing bank",
	"34Cheating card, card retention",
	"35Confiscate card, please contact the acquiring bank",
	"36This card is incorrect. Please replace the card and try again",
	"37Confiscate card, please contact the acquiring bank",
	"38The number of password errors has exceeded the limit. Please contact the issuing party",
	"39Transaction failed, please contact the issuing bank",
	"40Transaction types not supported by the issuing party",
	"41Report lost card, please confiscate it",
	"42Transaction failed, please contact the issuer",
	"43Stolen card, please confiscate it",
	"44Transaction failed, please contact the issuing bank",
	"45Prohibition of downgrade",
	"51Insufficient available balance",
	"52Transaction failed, please contact the issuing bank",
	"53Transaction failed, please contact the issuing bank",
	"54The card has expired",
	"55Password Error",
	"56Transaction failed, please contact the issuing bank",
	"57This card transaction is not allowed",
	"58The issuing party does not allow the card to conduct this transaction on this terminal",
	"59Card verification error",
	"60Transaction failed, please contact the issuing bank",
	"61Transaction amount exceeds the limit",
	"62Restricted card",
	"63Transaction failed, please contact the issuing bank",
	"64The transaction amount does not match the original transaction",
	"65Exceeding the limit of consumption times",
	"66Transaction failed, please contact the acquiring bank or UnionPay",
	"67Confiscate card",
	"68Transaction timeout, please try again",
	"75Password error count exceeds limit",
	"77Please sign in to the network center",
	"79POS terminal retransmits offline data",
	"90System daily shutdown, please try again later",
	"91The issuer's status is abnormal, please try again later",
	"92The issuing party's line is abnormal, please try again later",
	"93Transaction failed, please contact the issuing bank",
	"94Reject, duplicate transaction, please try again later",
	"95Transaction failed, please try again later",
	"96Rejected, exchange center exception, please try again later",
	"97Terminal not registered",
	"98Card issuer timeout",
	"99PIN format error, please check in again",
	"A0MAC verification error, please check in again",
	"A1Inconsistent transfer currency",
	"A2Transaction successful, please confirm with the issuing bank",
	"A3Account incorrect",
	"A4Transaction successful, please confirm with the issuing bank",
	"A5Transaction successful, please confirm with the issuing bank",
	"A6Transaction successful, please confirm with the issuing bank",
	"A7Rejected, exchange center exception, please try again later",
	"",
};



/*
 * Function description:
 * UnionPay card sign-in 8583 package packet header
 * Enter parameter: nCustNo: Merchant number
 * szPackStr: 8583 message
 * nPackLen: 8583 message length
 * 
 * Output parameters: szRetBuf: Packed data
 * nRetLen: Returns the data length
 * Return value: 0: Success
 * Others: Failed
 */
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
// length
	szSendStr[nLen++]=(char)((nPackLen + PACK_WHEAD_LEN)/256);
	szSendStr[nLen++]=(char)((nPackLen + PACK_WHEAD_LEN)%256);
// TPDU
	BUFCLR(szTpdu);
	c_asc2bcd((uchar *)szSendStr + nLen, YL_TPDU, 10, 0);
	nLen += 5;
// Message header
	BUFCLR(szHeader);
// Application Category + Software Total Version Number + Status Status + Processing Requirements
	memcpy(szHeader, "613200", 6); //IC card financial payment applications/UnionPay POS unified version specification version/normal transaction status
// Software version number
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
// 8583 message
	memcpy(szSendStr + nLen, szPackStr, nPackLen);
	nLen += nPackLen;

	memcpy(szRetBuf, szSendStr, nLen);
	*nRetLen = nLen;
	dbg_printfWHex(szRetBuf, nLen, "8583 data");
	return 0;
}

/*
 * Function description:
 * Decompose the message header
 * Input parameters:pcStr: message data
 * Output parameters:
 * Return value: 0: Success
 * Others: Failed
 */
int Pub_DivPacketHead(char *pcStr)
{
	char szBuffer[20], szHead[7];
	char flag;
	
	memset(szBuffer, 0x00, sizeof(szBuffer));
	memset(szHead, 0x00, sizeof(szHead));

	memcpy(szHead, pcStr + PACK_WHEAD_LEN - 6, 6);
	c_bcd2asc((uchar *)szBuffer, (uchar *)szHead, 12, 0);
/*
 * Terminal status 1N
 * Normal trading status: 0
 * Test transaction status 1
 */
// Normal state
	dbg_printfWHex(szBuffer, 12, "head");
	if (memcmp(szBuffer + 2 + 2, "0", 1))
	{
		dbg_printf("Term Status Err!");
		return -1;
	}
/*
 * Processing requirements 1N
 * No processing requirements 0
 * Download terminal parameters 1
 * Upload terminal status information 2
 * Re-check in 3
 */
	flag = szBuffer[5] - '0';
	dbg_printf("ProcFlag:%d",flag);
// gnProcFlag[nCustNo] = flag;

	return NORMAL;
}
/*
 * Function description:
 * UnionPay card sign-in communication interaction
 * Input parameters: dis_flag: front-end and back-end display identifier
 * nCustNo: Merchant Number
 * szSendBuf: The data content sent
 * nSendLen: The length of the data sent
 * Output parameters: szReadBuf: Received data content
 * nReadLen: Receive data length
 * Return value: 0: Success
 * -1: The parameters are wrong
 * -2: Communication error
 * -3: Receive data exception
 * Others: Failed
 */
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
// Initialize communication parameters
	ZERO_STRUCT(socket_info);
	nRet = network_init_socket_para(&socket_info, szSendBuf, nSendLen);
	if (nRet)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}
// dbg_printfWHex(socket_info.sendbuf, socket_info.sendlen,"IP:%s,PORT:%d,Connection timeout:%d,Receive timeout:%d,Ssl flag:%d,https flag:%d,Send data",
// socket_info.ip,socket_info.port,socket_info.connect_time, socket_info.recv_time, socket_info.ssl_flag,socket_info.union_https_flag);
// communication
	nLen = network_protocol_socket_tcp (&socket_info, READBUFF_SIZE - 1,szReadBuf);
	if (nLen <= 0)
	{
		return nLen;
	}
	dbg_printfWHex(szReadBuf, nLen, "Recv Data");
// Receive data analysis
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
// Header analysis
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
// Merchant number
	socket_info->custno = 0;
	
// Direct connection to UnionPay on the public network requires SSL
	socket_info->ssl_flag = 1;	
	memcpy(socket_info->ip,(char*)YL_IP,strlen((char*)YL_IP));
	socket_info->port = YL_PORT;
// Connection timeout
	socket_info->connect_time = 30;
// Receive timeout
	socket_info->recv_time = 30;
// Directly connected to UnionPay public network logo
	socket_info->union_https_flag = 1;
// Print
	socket_info->is_show_title = 1;
// Send data
	if (szSendBuf != NULL)
	{
		socket_info->sendlen = nSendLen;
		socket_info->sendbuf = szSendBuf;
	}
// Set the default socket_id value
	socket_info->socket_id = -1;
	return 0;
}


/*
 * Function description: Pre-connection processing
 * Entry parameters: parameters: communication parameters
 * Export parameters:
 * Return value:
 * 0: Success
 * Others: Failed
 */
int network_protocol_socket_tcp_pre_connect (socket_parameters_t *parameters)
{
	return protocol_socket_tcp_pre_connect(parameters);
}
/*
 * Function description: Connecting to the server process, including processing pre-connection
 * Entry parameters: parameters: communication parameters
 * pre_connect_flag: pre-connection identifier
 * Export parameters:
 * Return value:
 * 0: Success
 * Others: Failed
 */
int network_protocol_socket_tcp_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag)
{
	int nRet;
	uchar szBuffer[1024];
	uint uiLen;

	nRet = protocol_socket_tcp_follow_connect(parameters,pre_connect_flag);
	return nRet;
}

/*
 * Function description: Send data processing
 * Entry parameters: parameters: communication parameters
 * Export parameters:
 * Return value:
 * 0: Success
 * Others: Failed
 */
int network_protocol_socket_tcp_write (socket_parameters_t *parameters)
{
	return protocol_socket_tcp_write(parameters);
}
/*
 * Function description: Receive data processing
 * Entry parameters: parameters: communication parameters
 * recvlen: Maximum receive cache
 * Exit parameters: outbuf: received data
 * Return value:
 * >0:Success
 * <=0: Failed
 */
int network_protocol_socket_tcp_read (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	return protocol_socket_tcp_read(parameters, recvlen, outbuf);
}
/*
 * Function description: wireless communication hang-up processing
 * Entry parameters: parameters: communication parameters
 * Export parameters:
 * Return value:
 * >0:Success
 * <=0: Failed
 */
int network_protocol_socket_tcp_dialoff (socket_parameters_t *parameters)
{
	return protocol_socket_tcp_dialoff(parameters);
}

/*
 * function: network_protocol_socket_tcp
 * Description:
 * author:
 * IN: http_parameters_t *parameters->Access Parameters
 * recvlen: Receive maximum length
 * OUT: outbuf: Data content
 * return: >0 - Actual receiving length
 * <= 0 - fail
 */
int network_protocol_socket_tcp (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int nRet;
	uchar szBuffer[1024];
	uint uiLen;
    nRet = protocol_socket_tcp(parameters, recvlen,outbuf);
	return nRet;
}		/* -----  end of function network_protocol_socket_tcp  ----- */


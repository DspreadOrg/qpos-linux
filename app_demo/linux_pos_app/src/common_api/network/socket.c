#include "appinc.h"

typedef struct{
	uchar len[2];
	uchar product_sn[20];
	uchar hardware_sn[38];
	uchar cert_crypt[24];
	uchar random[16];
	uchar crc;
}_mb_hardware_crypt_;

int is_string_valid_ip (char *ip)
{
   	int parts[4];  
	int num_parts = 0;  
	char* token = strtok(strdup(ip), ".");  
	while (token != NULL) {  
		parts[num_parts++] = atoi(token);  
		token = strtok(NULL, ".");  
	}  
	if (num_parts != 4) {  
		return EXCEPTION;  
	}  
	for (int i = 0; i < 4; i++) {  
		if (parts[i] < 0 || parts[i] > 255) {  
			return EXCEPTION;  
		}  
	}  
	return NORMAL;  
}		/* -----  end of function is_string_valid_ip   ----- */

/*********************************************************************************
* 功能描述:
		直连银联公网ssl连接http报文头打包
* 输入参数:pData：传入的8583数据
			uiDataLen：8583数据长度
* 输出参数：vPBuffer：打包后的数据
			puiDataLen：打包后的数据长度
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int PubPackHttpHead(unsigned char *pData, unsigned int uiDataLen, unsigned char *vPBuffer, unsigned int *puiDataLen)
{
    unsigned int len;
    char chHttpServerHead[] =
    "POST /mjc/webtrans/VPB_lb HTTP/1.1\r\n"
    "HOST: 145.4.206.244:5000\r\n"
    "User-Agent: Donjin Http 0.1\r\n"
    "Cache-Control: no-cache\r\n"
    "Content-Type:x-ISO-TPDU/x-auth\r\n"
    "Accept: */*\r\n"
    "Content-Length: %d\r\n"
    "\r\n";

    if (pData == NULL || vPBuffer == NULL || puiDataLen == NULL)
    {
        return EXCEPTION;
    }

    sprintf((char *)vPBuffer, chHttpServerHead, uiDataLen);
    len = strlen((char *)vPBuffer);

    memcpy(&(vPBuffer[len]), pData, uiDataLen);
    len += uiDataLen;

    *puiDataLen = len;

    return 0;
}
/*********************************************************************************
* 功能描述:
		直连银联公网ssl连接http报文头截取
* 输入参数:pData：传入的https接收到的数据
			uiDataLen：传入的数据长度
* 输出参数：pData：截取的数据指针位置
			puiDataLen：返回的数据长度
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int PubCheckHttpHead(void *vPBuffer, unsigned int uiDataLen, unsigned char **pData, unsigned int *puiDataLen)
{
	unsigned char *pTmp;
	unsigned int len;

	if (vPBuffer == NULL || uiDataLen == 0 || pData == NULL || puiDataLen == NULL)
	{
		OsLog(LOG_DEBUG,"param FAIL");
		goto FAIL;
	}
	pTmp = (unsigned char *)vPBuffer;
	len = uiDataLen;

	while (len)
	{
		if (*pTmp == '\r' && *(pTmp + 1) == '\n' && *(pTmp + 2) == '\r' && *(pTmp + 3) == '\n')
		{
			pTmp += 4;
			*puiDataLen = uiDataLen - (pTmp - (unsigned char *)vPBuffer);
			//            dbg_printf("pTmp %x, vPBuffer %x %d", pTmp, vPBuffer, *puiDataLen);
			*pData = pTmp;
			goto SUCCESS;
		}
		pTmp++;
		len--;
	}

FAIL:
	return -1;
SUCCESS:
	return 0;
}
/* 
 * function:socket_conversion_parameters  
 * description:   
 * author:  
 * return:
 */
int socket_conversion_parameters (socket_parameters_t *parameters, struct sockaddr_in *addr)
{
    struct hostent *hptr = NULL;
	char **pptr = NULL;
	char szIP[20];
	char szUrl_bak[300];
    // if ( parameters == NULL || parameters->sendbuf == NULL \
    //      || parameters->sendlen == 0 || parameters->port == 0 ) {
    //     return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    // }
    if ( is_string_valid_ip(parameters->ip) == NORMAL) {
        addr->sin_addr.s_addr = inet_addr(parameters->ip);
    }else{
        /* 非IP格式,可能为域名,解析 */
        hptr = gethostbyname(parameters->ip);
        if ( hptr == NULL ) {
            OsLog(LOG_DEBUG,"gethostbyname host : %s ",parameters->ip);
            return err_sock_errno(SOCK_ERR_STEP_GETHOSTBYNAME,0);
        }
		pptr = hptr->h_addr_list;
		if (*pptr == NULL)
		{
			return err_sock_errno(SOCK_ERR_STEP_GETHOSTBYNAME,0);
		}
        addr->sin_addr = *((struct in_addr *)hptr->h_addr);
		BUFCLR(szIP);
		//解析成功，保存域名对应的ip
		if(inet_ntop(AF_INET,(void *)&(addr->sin_addr), szIP, 16) != NULL)
		{
			OsLog(LOG_DEBUG,"dn %s，ip：%s",parameters->ip, szIP);
			//database_config_set_string(parameters->custno, parameters->ip, (uchar *)szIP);
		}
		else
		{
			OsLog(LOG_DEBUG,"ip err！");
		}
		pptr++;
		if (*pptr != NULL)
		{
			BUFCLR(szIP);
			//解析成功，保存对应的ip
			if(inet_ntop(AF_INET,*pptr, szIP, 16) != NULL)
			{
				BUFCLR(szUrl_bak);
				sprintf(szUrl_bak, "%s_bak_ip", parameters->ip);
				OsLog(LOG_DEBUG,"dn%s，ip2：%s",parameters->ip, szIP);
				//database_config_set_string(parameters->custno, szUrl_bak, (uchar *)szIP);
			}
		}
    }
    addr->sin_family = AF_INET;
    addr->sin_port = htons(parameters->port);
    return 0;
}		/* -----  end of function socket_conversion_parameters   ----- */

/*********************************************************************************
* 功能描述:
		通信参数判断和域名解析-仅连接
* 输入参数:
* 输出参数：
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int socket_conversion_parameters_connect (socket_parameters_t *parameters, struct sockaddr_in *addr)
{
    struct hostent *hptr;
	char **pptr = NULL;
	char szIP[20];
	char szUrl_bak[300];
    if ( parameters == NULL || parameters->port == 0 ) {
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
    if ( is_string_valid_ip(parameters->ip) == NORMAL) {
        addr->sin_addr.s_addr = inet_addr(parameters->ip);
    }else{
        /* 非IP格式,可能为域名,解析 */
        hptr = gethostbyname(parameters->ip);
        if ( hptr == NULL ) {
            OsLog(LOG_DEBUG,"gethostbyname host : %s ",parameters->ip);
            return err_sock_errno(SOCK_ERR_STEP_GETHOSTBYNAME,0);
        }
		pptr = hptr->h_addr_list;
		if (*pptr == NULL)
		{
			return err_sock_errno(SOCK_ERR_STEP_GETHOSTBYNAME,0);
		}
        addr->sin_addr = *((struct in_addr *)hptr->h_addr);
		BUFCLR(szIP);
		//解析成功，保存域名对应的ip
		if(inet_ntop(AF_INET,(void *)&(addr->sin_addr), szIP, 16) != NULL)
		{
			OsLog(LOG_DEBUG,"dn%s，ip：%s",parameters->ip, szIP);
			//database_config_set_string(parameters->custno, parameters->ip, (uchar *)szIP);
		}
		else
		{
			OsLog(LOG_DEBUG,"ip err！");
		}
		pptr++;
		if (*pptr != NULL)
		{
			BUFCLR(szIP);
			//解析成功，保存对应的ip
			if(inet_ntop(AF_INET,*pptr, szIP, 16) != NULL)
			{
				BUFCLR(szUrl_bak);
				sprintf(szUrl_bak, "%s_bak_ip", parameters->ip);
				OsLog(LOG_DEBUG,"dn%s，ip2：%s",parameters->ip, szIP);
				//database_config_set_string(parameters->custno, szUrl_bak, (uchar *)szIP);
			}
		}
	}
    addr->sin_family = AF_INET;
    addr->sin_port = htons(parameters->port);
    return 0;
}		/* -----  end of function socket_conversion_parameters   ----- */

/*********************************************************************************
* 功能描述:
		域名解析返回ip
* 输入参数:indata:域名
* 输出参数：outdata：ip地址,最少17个缓存
* 返回值：	0：成功
*    	 其他：失败
*********************************************************************************/
int socket_gethostbyname (int nCustNo, char * indata, char * outdata)
{
    struct hostent *hptr = NULL;
	char szIP[20];
	char szUrl_bak[300];
	char **pptr = NULL;
	struct sockaddr_in addr;
    if (indata == NULL ) {
        return EXCEPTION;
    }
    if ( is_string_valid_ip(indata) == NORMAL) 
	{
		strcpy(outdata, indata);
		return NORMAL;
    }
	else
	{
        /* 非IP格式,可能为域名,解析 */
        hptr = gethostbyname(indata);
        if ( hptr == NULL ) 
		{
            OsLog(LOG_DEBUG,"gethostbyname host : %s failed",indata);
            return EXCEPTION;
        }
		pptr = hptr->h_addr_list;
		if (*pptr == NULL)
		{
			return EXCEPTION;
		}
        addr.sin_addr = *((struct in_addr *)hptr->h_addr);
		BUFCLR(szIP);
		//解析成功，保存域名对应的ip
		if(inet_ntop(AF_INET,(void *)&(addr.sin_addr), szIP, 16) != NULL)
		{
			OsLog(LOG_DEBUG,"dn%s，ip：%s",indata, szIP);
			// strcpy(outdata, szIP);
			// database_config_set_string(nCustNo, indata, (uchar *)szIP);
		}
		else
		{
			OsLog(LOG_DEBUG,"ip err！");
			return EXCEPTION;
		}
		pptr++;
		if (*pptr != NULL)
		{
			BUFCLR(szIP);
			//解析成功，保存对应的ip
			if(inet_ntop(AF_INET,*pptr, szIP, 16) != NULL)
			{
				BUFCLR(szUrl_bak);
				sprintf(szUrl_bak, "%s_bak_ip", indata);
				OsLog(LOG_DEBUG,"dn%s，ip2：%s",indata, szIP);
				//database_config_set_string(nCustNo, szUrl_bak, (uchar *)szIP);
			}
		}
		return NORMAL;
    }
}		/* -----  end of function socket_conversion_parameters   ----- */
/* 
 * function:socket_set_noblock  
 * description:   
 * author: 
 * return:
 */
int socket_set_noblock (int s,int noblock)
{
    int value = 0;
    value = fcntl(s ,F_GETFL,0);
    if ( noblock ) {
         fcntl(s ,F_SETFL,O_NONBLOCK|value);
    }else{
         fcntl(s ,F_SETFL,~O_NONBLOCK&value);
    }
    return 0;
}		/* -----  end of function socket_set_noblock   ----- */


/* 
 * function: socket_status_check
 * description:   
 * author:
 * return:
 */
static int socket_status_check(int s , int for_read, struct timeval tv)
{
    fd_set fdset;
    int ret;
    FD_ZERO(&fdset);
    FD_SET(s, &fdset);
    ret = select(s+1, for_read ? &fdset : NULL, for_read ? NULL : &fdset, NULL, &tv); 
    if(ret <= 0){
        OsLog(LOG_DEBUG,"ret: %d- errno:%d errmsg:%s",ret,errno,strerror(errno));
        return err_sock_errno(SOCK_ERR_STEP_SELECT,(int)errno);
    }
    return NORMAL;
}		/* -----  end of function socket_status_check  ----- */


/* 
 * function: socket_connect
 * description:   
 * author: 
 * return:
 */
int socket_connect (int socket, struct sockaddr_in addr,struct timeval tv)
{
    int ret = -1;
    // socklen_t len;
	OsLog(LOG_DEBUG,"socket=%d", socket);
    if ( connect(socket,(struct sockaddr *)&addr, sizeof(struct sockaddr))  == -1 ) {
        // getsockopt(socket,SOL_SOCKET,SO_ERROR,&ret,&len);
        if ( errno!= EINPROGRESS ) {
            OsLog(LOG_DEBUG,"connect error : [errno = %d][err = %d][message=%s]\n",errno,ret,strerror(errno));
            return err_sock_errno(SOCK_ERR_STEP_CONNECT,(int)errno);
        }
	}
	if ( (ret = socket_status_check(socket,0,tv)) != NORMAL ) {
		OsLog(LOG_DEBUG,"connect error : [errno = %d][err = %d][message=%s]\n",errno,ret,strerror(errno));
		return ret;
	}
    return NORMAL;
}		/* -----  end of function socket_connect  ----- */

/* 
 * function: socket_recv
 * description:   
 * author:  
 * return:
 */
int socket_recv(int nCustNo, int socket, char *buf, int len,struct timeval tv)
{
    int recvlen = 0;
    int ret = 0;
    setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));
    if ((ret = recv(socket,buf,len,0)) <= 0 ) {
        /* 指定时间内未收到数据 */
            return err_sock_errno(SOCK_ERR_STEP_RECV,(int)errno);
    }
    recvlen += ret;
    while( recvlen < len ) {
        /* 接收数据不全,继续等待 */
		switch ( nCustNo ) 
		{
			default:	
				tv.tv_sec = 0;
				tv.tv_usec = 50 * 1000;
				break;
		}				/* -----  end switch  ----- */
        setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));
        if ((ret = recv(socket,buf+recvlen,len-recvlen,0)) <= 0 ) {
            break;
        }
        recvlen += ret;
    }
    return recvlen;
}		/* -----  end of function socket_recv  ----- */


/* 
 * function: socket_tcp  
 * description: tcp 连接
 * author:   
 * return:
 */
int socket_tcp (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int iLen,iRet;
	unsigned char *pData = NULL;
    int socket_fd = 0;
    int len = 0;
    int ret = 0;
	uchar szTmp[READBUFF_SIZE + 200] = {0};
	uint nTmplen = 0;
    struct timeval tv;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    if ( recvlen == 0 || outbuf == NULL ) {
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
    if((ret = socket_conversion_parameters(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
        OsLog(LOG_DEBUG,"socket parameters err!");
        return ret;
    }
    if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        OsLog(LOG_DEBUG,"create_socket err!");
        return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
    }
    OsLog(LOG_DEBUG,"socket id:%d",socket_fd);
    socket_set_noblock(socket_fd,1);
    tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
    tv.tv_usec = 0;
    if ( parameters->is_show_title == 0 || parameters->is_show_title == 1) {
        OsLog(LOG_DEBUG,  "connecting...");
    }
    ret = socket_connect(socket_fd,addr,tv);
    if ( ret ) {
        closesocket(socket_fd);
        OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",parameters->ip,parameters->port,strerror(errno));
        return ret;
    }
    OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
    socket_set_noblock(socket_fd,0);
//    dbg_printfWHex((uchar *)parameters->sendbuf,parameters->sendlen,"socket send to:%s:%d",parameters->ip,parameters->port);

	//测试环境http接入，添加http报文头
	if (parameters->normal_http_flag == 1)
	{
		PubPackHttpHead((uchar *)parameters->sendbuf, parameters->sendlen, szTmp, &nTmplen);
		if(send(socket_fd,szTmp,nTmplen,0)<0){
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"send err[%s]",strerror(errno));
			return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
		}
	}
	else //普通发送
	{
		if(send(socket_fd,parameters->sendbuf,parameters->sendlen,0)<0){
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"send err[%s]",strerror(errno));
			return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
		}
	}
    tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
    tv.tv_usec = 0;
    if ( parameters->is_show_title == 0 || parameters->is_show_title == 1) {
        OsLog(LOG_DEBUG, "recv data...");
    }
	if (parameters->normal_http_flag == 1)//普通http接入
	{
		BUFCLR(szTmp);
		len = socket_recv(parameters->custno,socket_fd,(char *)szTmp,READBUFF_SIZE -1,tv);//最大READBUFF_SIZE
	}
	else
	{
		len = socket_recv(parameters->custno,socket_fd,outbuf,recvlen,tv);
	}

    if(len<=0){
        closesocket(socket_fd);
        OsLog(LOG_DEBUG,"recv err[%s]",strerror(errno));
        return err_sock_errno(SOCK_ERR_STEP_RECV,(int)errno);
    }
//    dbg_printfWHex((uchar *)outbuf,len,"socket recv from :%s:%d",parameters->ip,parameters->port);
    closesocket(socket_fd);

	if (parameters->normal_http_flag == 1)//普通http接入，去除http报文头
	{
		iRet = PubCheckHttpHead(szTmp, len, &pData, (uint *)&iLen);
		OsLog(LOG_DEBUG,"PubCheckHttpHead %d", iRet);
		if (iRet == 0)
		{
			len = (iLen > recvlen)?recvlen:iLen;
			memcpy(outbuf, pData, len);
		}
		else
		{
			return err_sock_errno(SOCK_ERR_STEP_PARA,0);
		}
	}

    return len;
}		/* -----  end of function socket_tcp ----- */

int setSslReadTimeout(SSL *ssl,int timeoutMs){
	    
    struct timeval timeout;
    timeout.tv_sec = timeoutMs/1000;
    timeout.tv_usec = (timeoutMs%1000)*1000;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(SSL_get_fd(ssl), &readfds);

    // 使用select检查套接字是否准备好进行读取
    int ret = select(SSL_get_fd(ssl) + 1, &readfds, NULL, NULL, &timeout);
	OsLog(LOG_DEBUG,"setSslReadTimeout ret[%d]",ret);
    
	return ret;
}



int Ssl_recv_msg(SSL *ssl, char * outbuf, ssize_t recvlen, int timeout)
{
	ssize_t len = 0;
	int offset = 0;
	int pending_status;
	if((ssl == NULL) || (outbuf == NULL) || (recvlen == 0)){
		return -1;
	}
	//setSslReadTimeout(ssl,timeout);
	//从服务器接收信息
	offset = 0;
	len = setSslReadTimeout(ssl,timeout);
	if(len > 0){
		while ((len = SSL_read(ssl, outbuf + offset, recvlen -1 - offset)) > 0) {
			OsLog(LOG_DEBUG,"SSL: SSL_read ret[%d]",len);
			offset += len;
			len = setSslReadTimeout(ssl,500);
			if(len <= 0){
				break;
			}
    	}
		OsLog(LOG_DEBUG,"SSL: SSL_read: %s",outbuf);
	}else{
		OsLog(LOG_DEBUG,"SSL: SSL_read Timeout [%d]",len);
       	offset = len;
	}

	return offset;
}


/* 
 * function: socket_tcp_ssl
 * description:   
 * author: 
 * return:
 */
int socket_tcp_ssl (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int iLen,iRet;
	unsigned char *pData = NULL;
	char szTmpBuf[READBUFF_SIZE];
    int ret;
    int socket_fd = 0;
    SSL_CTX *ctx = NULL;
    SSL     *ssl = NULL;
    struct timeval tv;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    if ( recvlen == 0 || outbuf == NULL ) {
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
    if((ret = socket_conversion_parameters(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
        OsLog(LOG_DEBUG,"socket parameters err!");
        return ret;
    }
    if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        OsLog(LOG_DEBUG,"create_socket err!");
        return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
    }
    tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
    tv.tv_usec = 0; 
    setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));

    socket_set_noblock(socket_fd,1);
    tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
    tv.tv_usec = 0; 
    if ( parameters->is_show_title == 1 || parameters->is_show_title == 0) {
        OsLog(LOG_DEBUG,"connecting...");
    }
    ret = socket_connect(socket_fd,addr,tv);
    if ( ret ) {
        closesocket(socket_fd);
        OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",parameters->ip,parameters->port,strerror(errno));
        goto FAIL;
    }
    OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
    socket_set_noblock(socket_fd,0);

    //SSL_rand_poll();
    /// SSL初始化
    SSL_library_init();
#ifdef CFG_SSL_V3
	if ((ctx = SSL_CTX_new(SSLv3_client_method())) == NULL)
#else
	if ((ctx = SSL_CTX_new(SSLv23_client_method())) == NULL)
#endif
    {
        OsLog(LOG_DEBUG,"SSL: ssl_ctx_new NULL");
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_INIT,0);
        goto FAIL;
    }
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        OsLog(LOG_DEBUG,"SSL: ssl_new NULL");
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_INIT,0);
        goto FAIL;
    }

//	const char *servername = "clients-stg.auth.bold.co";
	OsLog(LOG_DEBUG,"recv max len %d",recvlen);
	OsLog(LOG_DEBUG,"SSL: ssl set host name[%s]",parameters->host);
	SSL_set_tlsext_host_name(ssl, parameters->host);

    if ( (ret = SSL_set_fd(ssl,socket_fd)) < 0) {
        OsLog(LOG_DEBUG,"SSL: ssl set fd err[%d][%s]",ret,SSL_state_string_long(ssl));
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_INIT,0);
        goto FAIL;
    }
    if ( (ret = SSL_connect(ssl)) != 1 ) {
        OsLog(LOG_DEBUG,"SSL: ssl connect err[%d][%s]",ret,SSL_state_string_long(ssl));
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_CONNECT,0);
        goto FAIL;
    }

	//银联公网接入，添加http报文头
	if (parameters->union_https_flag == 1)
	{
		uchar szTmp[READBUFF_SIZE + 200] = {0};
		uint nTmplen = 0;
		PubPackHttpHead((uchar *)parameters->sendbuf, parameters->sendlen, szTmp, &nTmplen);
		if ( (ret = SSL_write(ssl,szTmp,nTmplen)) < 0 ) {
			OsLog(LOG_DEBUG,"SSL: ssl send err[%d][%s]",ret,SSL_state_string_long(ssl));
            SSL_shutdown(ssl);
			SSL_free(ssl);
			SSL_CTX_free(ctx);
			closesocket(socket_fd);
			return err_sock_errno(SOCK_ERR_STEP_SSL_SEND,0);
		}
	}
	else //普通ssl发送
	{
		//    dbg_printfWHex((uchar *)parameters->sendbuf,parameters->sendlen,"socket ssl send to:%s:%d",parameters->ip,parameters->port);
		if ( (ret = SSL_write(ssl,parameters->sendbuf,parameters->sendlen)) < 0 ) {
			OsLog(LOG_DEBUG,"SSL: ssl send err[%d][%s]",ret,SSL_state_string_long(ssl));
            SSL_shutdown(ssl);
			SSL_free(ssl);
			SSL_CTX_free(ctx);
			closesocket(socket_fd);
			return err_sock_errno(SOCK_ERR_STEP_SSL_SEND,0);
		}
	}
    if ( parameters->is_show_title == 1 || parameters->is_show_title == 0 ) {
        OsLog(LOG_DEBUG,"recv data...");
    }
	if (parameters->union_https_flag == 1)//银联公网接入
	{
		ret = SSL_read(ssl,szTmpBuf,READBUFF_SIZE -1); //最大READBUFF_SIZE
	}
	else
	{
		ret = Ssl_recv_msg(ssl,outbuf,recvlen,parameters->recv_time);
	}
    if ( ret < 0 ) {
        OsLog(LOG_DEBUG,"SSL: ssl read err[%d]",ret);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        return err_sock_errno(SOCK_ERR_STEP_SSL_RECV,0);
    }
//    dbg_printfWHex((uchar *)outbuf,ret,"socket ssl recv from :%s:%d",parameters->ip,parameters->port);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(socket_fd);
	if (parameters->union_https_flag == 1)//银联公网接入，去除http报文头
	{
		iRet = PubCheckHttpHead(szTmpBuf, ret, &pData, (uint *)&iLen);
		OsLog(LOG_DEBUG,"PubCheckHttpHead %d", iRet);
		if (iRet == 0)
		{
			ret = (iLen > recvlen)?recvlen:iLen;
			memcpy(outbuf, pData, ret);
		}
		else
		{
			return err_sock_errno(SOCK_ERR_STEP_PARA,0);
		}
	}
	return ret;
FAIL:
    return ret;
}		/* -----  end of function socket_tcp_ssl  ----- */



/* 
 * function: protocol_socket_tcp
 * description:   
 * author: 
 * return:
 */
int protocol_socket_tcp (socket_parameters_t *parameters, int recvlen, char *outbuf)
{
    if ( parameters->ssl_flag == 1 ||parameters->union_https_flag == 1) {
        return socket_tcp_ssl(parameters,recvlen,outbuf);
    }else{
        return socket_tcp(parameters,recvlen,outbuf);
    }
}		/* -----  end of function protocol_socket_tcp  ----- */


/* 
 * function: protocol_socket_udp
 * description: 只实现发送，不实现接收，用于心跳使用  
 * author: 
 * return:
 */
int protocol_socket_udp (socket_parameters_t *parameters, int recvlen, char *outbuf)
{
    int socket_fd = 0;
    int ret;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    if ( recvlen == 0 || outbuf == NULL ) {
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
    if((ret = socket_conversion_parameters(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
        OsLog(LOG_DEBUG,"socket parameters err!");
        return ret;
    }
    if((socket_fd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
        OsLog(LOG_DEBUG,"create_socket err!");
        return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
    }
    ret = sendto(socket_fd, parameters->sendbuf, parameters->sendlen, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if ( ret == -1 ) {
        closesocket(socket_fd);
        return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
    }
    closesocket(socket_fd);
    return 0;
}		/* -----  end of function protocol_socket_udp  ----- */



/*****************************************************
*函数说明:预连接
*入口参数:socket： socket id
		  addr：服务器地址
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_connect_pre_connect (int socket, struct sockaddr_in addr)
{
    int ret = -1;
    // socklen_t len;
    if ( connect(socket,(struct sockaddr *)&addr, sizeof(struct sockaddr))  == -1 ) {
        // getsockopt(socket,SOL_SOCKET,SO_ERROR,&ret,&len);
        if ( errno!= EINPROGRESS ) {
            OsLog(LOG_DEBUG,"connect error : [errno = %d][err = %d][message=%s]\n",errno,ret,strerror(errno));
            return err_sock_errno(SOCK_ERR_STEP_CONNECT,(int)errno);
        }
    }
    return NORMAL;
}
/*****************************************************
*函数说明:预连接输入参数判断
*入口参数:parameters： 通讯参数
*出口参数:addr：服务器地址
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_conversion_parameters_pre_connect (socket_parameters_t *parameters, struct sockaddr_in *addr)
{
    if ( parameters == NULL || parameters->port == 0 ) {
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
    if ( is_string_valid_ip(parameters->ip) == NORMAL) {
        addr->sin_addr.s_addr = inet_addr(parameters->ip);
    }else{
		//域名解析会阻塞，所以不放在预拨号里面
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
    addr->sin_family = AF_INET;
    addr->sin_port = htons(parameters->port);
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
int protocol_socket_tcp_pre_connect (socket_parameters_t *parameters)
{
    int ret;
    int socket_fd = 0;
    struct sockaddr_in addr;
	struct timeval tv;
    bzero(&addr,sizeof(addr));

	if((ret = socket_conversion_parameters_pre_connect(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
		OsLog(LOG_DEBUG,"socket parameters err!");
		return ret;
	}
    if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        OsLog(LOG_DEBUG,"create_socket err!");
        return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
    }
    tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
    tv.tv_usec = 0; 
    setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));

    socket_set_noblock(socket_fd,1);
    tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
    tv.tv_usec = 0; 
    ret = socket_connect_pre_connect(socket_fd,addr);
    if ( ret ) {
        closesocket(socket_fd);
        OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",parameters->ip,parameters->port,strerror(errno));
		return ret;
    }
	parameters->socket_id = socket_fd;
	return NORMAL;
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
int socket_tcp_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag)
{
	int ret;
	int socket_fd = 0;
	struct timeval tv;
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));

	if ((*pre_connect_flag) == YES)//如果有预拨号
	{ 
		//判断传入参数
		if ( parameters == NULL) {
				return err_sock_errno(SOCK_ERR_STEP_PARA,0);
		}
		*pre_connect_flag = NO;
		//使用预拨号的socket id
		socket_fd = parameters->socket_id;
		OsLog(LOG_DEBUG,"socket_id:%d",parameters->socket_id);
		if ( parameters->is_show_title == 1 || parameters->is_show_title == 0) {
			OsLog(LOG_DEBUG, "connecting...");
		}
	
		tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
		tv.tv_usec = 0; 
		//检测网络连接是否成功
		if ( (ret = socket_status_check(socket_fd,0,tv)) != NORMAL ) {
			OsLog(LOG_DEBUG,"connect error : [errno = %d][err = %d][message=%s]\n",errno,ret,strerror(errno));
			closesocket(socket_fd);
			goto FAIL;
		}
		OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
	}
	else
	{
		if((ret = socket_conversion_parameters(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
			OsLog(LOG_DEBUG,"socket parameters err!");
			return ret;
		}
		if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
			OsLog(LOG_DEBUG,"create_socket err!");
			return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
		}
		tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
		tv.tv_usec = 0; 
		setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));

		socket_set_noblock(socket_fd,1);
		tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
		tv.tv_usec = 0; 
		if ( parameters->is_show_title == 1 || parameters->is_show_title == 0) {
			OsLog(LOG_DEBUG,"connecting...");
		}
		ret = socket_connect(socket_fd,addr,tv);
		if ( ret ) {
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",parameters->ip,parameters->port,strerror(errno));
			goto FAIL;
		}
		OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
		//socket id赋值给全局变量
		parameters->socket_id = socket_fd;
		OsLog(LOG_DEBUG,"socket_id:%d",parameters->socket_id);
	}

	socket_set_noblock(socket_fd,0);
	return NORMAL;
FAIL:
	return ret;
}
/*****************************************************
*函数说明:socket发送数据处理
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_tcp_write (socket_parameters_t *parameters)
{
	uchar szTmp[READBUFF_SIZE + 200] = {0};
	uint nTmplen = 0;
	if (parameters == NULL || parameters->sendbuf == NULL || parameters->sendlen == 0)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}

	//http网接入，添加http报文头
	if (parameters->normal_http_flag == 1)
	{
		PubPackHttpHead((uchar *)parameters->sendbuf, parameters->sendlen, szTmp, &nTmplen);
		if(send(parameters->socket_id, szTmp, nTmplen,0)<0){
			closesocket(parameters->socket_id);
			OsLog(LOG_DEBUG,"send err[%s]",strerror(errno));
			return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
		}
	}
	else //普通发送
	{
		if(send(parameters->socket_id,parameters->sendbuf,parameters->sendlen,0)<0){
			closesocket(parameters->socket_id);
			OsLog(LOG_DEBUG,"send err[%s]",strerror(errno));
			return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
		}
	}
	return NORMAL;
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
int socket_tcp_read (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int len;
	struct timeval tv;
	int iLen,iRet;
	unsigned char *pData = NULL;
	char szTmpBuf[READBUFF_SIZE];

	if (recvlen == 0 || outbuf == NULL || parameters == NULL)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}

	if ( parameters->is_show_title == 1 || parameters->is_show_title == 0 ) {
		OsLog(LOG_DEBUG, "recv data...");
	}
	tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
	tv.tv_usec = 0; 

	if (parameters->normal_http_flag == 1)//http接入
	{
		BUFCLR(szTmpBuf);
		len = socket_recv(parameters->custno,parameters->socket_id, szTmpBuf, READBUFF_SIZE -1, tv);
	}
	else
	{
		len = socket_recv(parameters->custno,parameters->socket_id,outbuf,recvlen,tv);
	}

	if(len<=0){
		closesocket(parameters->socket_id);
		OsLog(LOG_DEBUG,"recv err[%s]",strerror(errno));
		return err_sock_errno(SOCK_ERR_STEP_RECV,(int)errno);
	}
	closesocket(parameters->socket_id);

	if (parameters->normal_http_flag == 1)//普通http接入，去除http报文头
	{
		iRet = PubCheckHttpHead(szTmpBuf, len, &pData, (uint *)&iLen);
		OsLog(LOG_DEBUG,"PubCheckHttpHead %d", iRet);
		if (iRet == 0)
		{
			len = (iLen > recvlen)?recvlen:iLen;
			memcpy(outbuf, pData, len);
		}
		else
		{
			return err_sock_errno(SOCK_ERR_STEP_PARA,0);
		}
	}
	return len;
}
/*****************************************************
*函数说明:挂断操作
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_tcp_dialoff (socket_parameters_t *parameters)
{
	if (parameters == NULL)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}
	if (parameters->ssl != NULL)
	{
		SSL_free(parameters->ssl);
		parameters->ssl = NULL;
	}
	if (parameters->ctx != NULL)
	{
		SSL_CTX_free(parameters->ctx);
		parameters->ctx = NULL;
	}
	if (parameters->socket_id != -1)
	{
		closesocket(parameters->socket_id);
		parameters->socket_id = -1;
	}
	return NORMAL;
}
/*****************************************************
*函数说明:ssl连接服务器处理，包含处理预连接
*入口参数:parameters： 通讯参数
	pre_connect_flag：预连接标识
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_tcp_ssl_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag)
{
    int ret;
    int socket_fd = 0;
    SSL_CTX *ctx = NULL;
    SSL     *ssl = NULL;
    struct timeval tv;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));

	if ((*pre_connect_flag) == YES)//如果有预拨号
	{ 
		//判断传入参数
		if ( parameters == NULL ||  parameters->port == 0 ) {
				return err_sock_errno(SOCK_ERR_STEP_PARA,0);
		}
		*pre_connect_flag = NO;
		//使用预拨号的socket id
		socket_fd = parameters->socket_id;
		OsLog(LOG_DEBUG,"socket_id:%d",parameters->socket_id);
		if ( parameters->is_show_title == 1 || parameters->is_show_title == 0) {
			OsLog(LOG_DEBUG, "connecting...");
		}
		
		tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
		tv.tv_usec = 0; 
		//检测网络连接是否成功
		if ( (ret = socket_status_check(socket_fd,0,tv)) != NORMAL ) {
			OsLog(LOG_DEBUG,"connect error : [errno = %d][err = %d][message=%s]\n",errno,ret,strerror(errno));
			closesocket(socket_fd);
			ret = err_sock_errno(SOCK_ERR_STEP_CONNECT,(int)errno);
			goto FAIL;
		}
		OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
	}
	else
	{
		if((ret = socket_conversion_parameters_connect(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
			OsLog(LOG_DEBUG,"socket parameters err!");
			return ret;
		}
		if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
			OsLog(LOG_DEBUG,"create_socket err!");
			return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
		}
		tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
		tv.tv_usec = 0; 
		setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));

		socket_set_noblock(socket_fd,1);
		tv.tv_sec = (parameters->connect_time<=0)?SOCKET_CONNECT_TIMEOUT:parameters->connect_time;
		tv.tv_usec = 0; 
		if ( parameters->is_show_title == 1 || parameters->is_show_title == 0) {
			OsLog(LOG_DEBUG,  "connecting...");
		}
	
		ret = socket_connect(socket_fd,addr,tv);
		if ( ret ) {
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",parameters->ip,parameters->port,strerror(errno));
			goto FAIL;
		}
		//socket id赋值给全局变量
		parameters->socket_id = socket_fd;
		OsLog(LOG_DEBUG,"socket_id:%d",parameters->socket_id);
		OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
	}
    
    socket_set_noblock(socket_fd,0);

    // SSL_rand_poll();
    /// SSL初始化
    SSL_library_init();
#ifdef CFG_SSL_V3
	if ((ctx = SSL_CTX_new(SSLv3_client_method())) == NULL)
#else
	if ((ctx = SSL_CTX_new(SSLv23_client_method())) == NULL)
#endif
    {
        OsLog(LOG_DEBUG,"SSL: ssl_ctx_new NULL");
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_INIT,0);
        goto FAIL;
    }

    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        OsLog(LOG_DEBUG,"SSL: ssl_new NULL");
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_INIT,0);
        goto FAIL;
    }
    if ( (ret = SSL_set_fd(ssl,socket_fd)) < 0) {
        OsLog(LOG_DEBUG,"SSL: ssl set fd err[%d]",ret);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_INIT,0);
        goto FAIL;
    }
    if ( (ret = SSL_connect(ssl)) != 1 ) {
        OsLog(LOG_DEBUG,"SSL: ssl connect err[%d][%s]",ret,SSL_state_string_long(ssl));
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(socket_fd);
        ret = err_sock_errno(SOCK_ERR_STEP_SSL_CONNECT,0);
        goto FAIL;
    }

	parameters->ctx = ctx;
	parameters->ssl = ssl;
	return NORMAL;
FAIL:
    return ret;
}

/*****************************************************
*函数说明:ssl发送数据处理
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_tcp_ssl_write (socket_parameters_t *parameters)
{
    int ret;
	uchar szTmp[READBUFF_SIZE + 200] = {0};
	uint nTmplen = 0;
	if (parameters == NULL || parameters->sendbuf == NULL || parameters->sendlen == 0
		||parameters->ctx == NULL || parameters->ssl == NULL)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}
	//银联公网接入，添加http报文头
	if (parameters->union_https_flag == 1)
	{
		PubPackHttpHead((uchar *)parameters->sendbuf, parameters->sendlen, szTmp, &nTmplen);
		if ( (ret = SSL_write(parameters->ssl,szTmp,nTmplen)) < 0 ) {
			OsLog(LOG_DEBUG,"SSL: ssl send err[%d][%s]",ret,SSL_state_string_long(parameters->ssl));
            SSL_shutdown(parameters->ssl);
			SSL_free(parameters->ssl);
			SSL_CTX_free(parameters->ctx);
			closesocket(parameters->socket_id);
			return err_sock_errno(SOCK_ERR_STEP_SSL_SEND,0);
		}
	}
	else //普通ssl发送
	{
		if ( (ret = SSL_write(parameters->ssl,parameters->sendbuf,parameters->sendlen)) < 0 ) {
			OsLog(LOG_DEBUG,"SSL: ssl send err[%d][%s]",ret,SSL_state_string_long(parameters->ssl));
            SSL_shutdown(parameters->ssl);
			SSL_free(parameters->ssl);
			SSL_CTX_free(parameters->ctx);
			closesocket(parameters->socket_id);
			return err_sock_errno(SOCK_ERR_STEP_SSL_SEND,0);
		}
	}
	return NORMAL;
}
/*****************************************************
*函数说明:ssl接收数据处理
*入口参数:parameters： 通讯参数
		recvlen:最大接收缓存
*出口参数:outbuf：接收到的数据
*返回值:
		>0:成功
		<=0：失败
*****************************************************/
int socket_tcp_ssl_read (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int iLen,iRet;
	unsigned char *pData = NULL;
	char szTmpBuf[READBUFF_SIZE];
	int ret;
	struct timeval tv;

	if (recvlen == 0 || outbuf == NULL
		||parameters == NULL||parameters->ctx == NULL || parameters->ssl == NULL)
	{
		return err_sock_errno(SOCK_ERR_STEP_PARA,0);
	}

	if ( parameters->is_show_title == 1 || parameters->is_show_title == 0 ) {
		OsLog(LOG_DEBUG, "recv data...");
	}
	tv.tv_sec = (parameters->recv_time<=0)?SOCKET_RECV_TIMEOUT:parameters->recv_time;
	tv.tv_usec = 0; 
	
	if (parameters->union_https_flag == 1)//银联公网接入
	{
		ret = SSL_read(parameters->ssl,szTmpBuf,READBUFF_SIZE -1); //最大READBUFF_SIZE
	}
	else
	{
		ret = SSL_read(parameters->ssl,outbuf,recvlen);
	}
	if ( ret < 0 ) {
		OsLog(LOG_DEBUG,"SSL: ssl read err[%d]",ret);
        SSL_shutdown(parameters->ssl);
		SSL_free(parameters->ssl);
		SSL_CTX_free(parameters->ctx);
		closesocket(parameters->socket_id);
		return err_sock_errno(SOCK_ERR_STEP_SSL_RECV,0);
	}
    SSL_shutdown(parameters->ssl);
	SSL_free(parameters->ssl);
	SSL_CTX_free(parameters->ctx);
	closesocket(parameters->socket_id);
	if (parameters->union_https_flag == 1)//银联公网接入，去除http报文头
	{
		iRet = PubCheckHttpHead(szTmpBuf, ret, &pData, (uint *)&iLen);
		OsLog(LOG_DEBUG,"PubCheckHttpHead %d", iRet);
		if (iRet == 0)
		{
			ret = (iLen > recvlen)?recvlen:iLen;
			memcpy(outbuf, pData, ret);
		}
		else
		{
			return err_sock_errno(SOCK_ERR_STEP_SSL_RECV,0);
		}
	}
	return ret;
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
int protocol_socket_tcp_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag)
{
    if ( parameters->ssl_flag == 1 ||parameters->union_https_flag == 1) {
        return socket_tcp_ssl_follow_connect(parameters,pre_connect_flag);
    }else{
        return socket_tcp_follow_connect(parameters,pre_connect_flag);
    }
}
/*****************************************************
*函数说明:发送数据处理
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int protocol_socket_tcp_write (socket_parameters_t *parameters)
{
	if ( parameters->ssl_flag == 1 ||parameters->union_https_flag == 1) {
		return socket_tcp_ssl_write(parameters);
	}else{
		return socket_tcp_write(parameters);
	}
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
int protocol_socket_tcp_read (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	if ( parameters->ssl_flag == 1 ||parameters->union_https_flag == 1) {
		return socket_tcp_ssl_read(parameters, recvlen, outbuf);
	}else{
		return socket_tcp_read(parameters, recvlen, outbuf);
	}
}
/*****************************************************
*函数说明:挂断操作
*入口参数:parameters： 通讯参数
*出口参数:
*返回值:
		0:成功
		其他：失败
*****************************************************/
int protocol_socket_tcp_dialoff (socket_parameters_t *parameters)
{
	return socket_tcp_dialoff(parameters);
}

/*****************************************************
*函数说明:tcp长连接
*入口参数:parameters： 通讯参数
		recvlen接收最大缓存
		
*出口参数:outbuf读取到的数据
*返回值:
		0:成功
		其他：失败
*****************************************************/
int socket_tcp_alive (socket_parameters_t *parameters, int recvlen,char *outbuf)
{
	int iLen,iRet;
	unsigned char *pData = NULL;
    int socket_fd = 0;
    int len = 0;
    int ret = 0;
	uchar szTmp[READBUFF_SIZE + 200] = {0};
	uint nTmplen = 0;
    struct timeval tv;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    if ( recvlen == 0 || outbuf == NULL ) {
        return err_sock_errno(SOCK_ERR_STEP_PARA,0);
    }
	if (parameters->socket_id == -1)
	{
		if((ret = socket_conversion_parameters(parameters,(struct sockaddr_in *)&addr))!=NORMAL){
			OsLog(LOG_DEBUG,"socket parameters err!");
			return ret;
		}
		if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
			OsLog(LOG_DEBUG,"create_socket err!");
			return err_sock_errno(SOCK_ERR_STEP_CREATESOCKET,0);
		}
		OsLog(LOG_DEBUG,"socket id:%d",socket_fd);
		socket_set_noblock(socket_fd,1);
		tv.tv_sec = (parameters->connect_time<=0)?0:parameters->connect_time;
		tv.tv_usec = (parameters->connect_time_us<=0)?0:parameters->connect_time_us;
		if ( parameters->is_show_title == 0 || parameters->is_show_title == 1) {
			OsLog(LOG_DEBUG,  "connecting...");
		}
	
		ret = socket_connect(socket_fd,addr,tv);
		if ( ret ) {
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",parameters->ip,parameters->port,strerror(errno));
			return ret;
		}
		OsLog(LOG_DEBUG,"connect :%s:%d success!",parameters->ip,parameters->port);
		socket_set_noblock(socket_fd,0);
		//赋值socket id
		parameters->socket_id = socket_fd;
	}
	else
	{
		socket_fd = parameters->socket_id;
		//dbg_printf("socket id:%d",socket_fd);
	}
 
//    dbg_printfWHex((uchar *)parameters->sendbuf,parameters->sendlen,"socket send to:%s:%d",parameters->ip,parameters->port);

	//测试环境http接入，添加http报文头
	if (parameters->normal_http_flag == 1)
	{
		PubPackHttpHead((uchar *)parameters->sendbuf, parameters->sendlen, szTmp, &nTmplen);
		if(send(socket_fd,szTmp,nTmplen,0)<0){
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"send err[%s]",strerror(errno));
			//赋值socket id
			parameters->socket_id = -1;
			return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
		}
	}
	else //普通发送
	{
		if(send(socket_fd,parameters->sendbuf,parameters->sendlen,0)<0){
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"send err[%s]",strerror(errno));
			//赋值socket id
			parameters->socket_id = -1;
			return err_sock_errno(SOCK_ERR_STEP_SEND,(int)errno);
		}
	}
    tv.tv_sec = (parameters->recv_time<=0)?0:parameters->recv_time;
    tv.tv_usec = (parameters->recv_time_us<=0)?0:parameters->recv_time_us;
    if ( parameters->is_show_title == 0 || parameters->is_show_title == 1) {
        OsLog(LOG_DEBUG,  "recv data...");
    }

	if (parameters->normal_http_flag == 1)//普通http接入
	{
		BUFCLR(szTmp);
		len = socket_recv(parameters->custno,socket_fd,(char *)szTmp,READBUFF_SIZE -1,tv);//最大READBUFF_SIZE
	}
	else
	{
		len = socket_recv(parameters->custno,socket_fd,outbuf,recvlen,tv);
	}
    if(len<=0){
        closesocket(socket_fd);
        OsLog(LOG_DEBUG,"recv err[%s]",strerror(errno));
		//赋值socket id
		parameters->socket_id = -1;
        return err_sock_errno(SOCK_ERR_STEP_RECV,(int)errno);
    }
//    dbg_printfWHex((uchar *)outbuf,len,"socket recv from :%s:%d",parameters->ip,parameters->port);
	if (parameters->normal_http_flag == 1)//普通http接入，去除http报文头
	{
		iRet = PubCheckHttpHead(szTmp, len, &pData, (uint *)&iLen);
		OsLog(LOG_DEBUG,"PubCheckHttpHead %d", iRet);
		if (iRet == 0)
		{
			len = (iLen > recvlen)?recvlen:iLen;
			memcpy(outbuf, pData, len);
		}
		else
		{
			closesocket(socket_fd);
			OsLog(LOG_DEBUG,"recv err[%s]",strerror(errno));
			//赋值socket id
			parameters->socket_id = -1;
			return err_sock_errno(SOCK_ERR_STEP_PARA,0);
		}
	}
	//closesocket(socket_fd);
    return len;
}		/* -----  end of function socket_tcp ----- */


int init_socket_para(socket_parameters_t *socket_info,char *pHost,int port,char * szSendBuf, int nSendLen,int timeout)
{
	uchar ipAddr[32] = {0};
	struct hostent *host;
	int nRet;
    int ipIndex = 0;
    int i;
	if (socket_info == NULL){
		return -1;
	}
	socket_info->custno = 0;
	//SSL flag
	socket_info->ssl_flag = 1;
    #ifdef CFG_DBG	
    OsLog(LOG_DEBUG,"Dspread: pHost = %s",pHost);
    #endif
	memcpy(socket_info->host,pHost,strlen(pHost));
    host = gethostbyname(pHost);
	if(host == NULL) {
		return -1;
	}
	//IP地址
    #ifdef CFG_DBG
	for(i=0; host->h_addr_list[i]; i++){
		OsLog(LOG_INFO,"IP addr %d: %s\n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
    OsLog(LOG_INFO,"Connect IP addr %d: %s\n", ipIndex, inet_ntoa(*(struct in_addr*)host->h_addr_list[ipIndex]));
    #endif    

	memcpy(ipAddr,inet_ntoa(*(struct in_addr*)host->h_addr_list[ipIndex]),strlen(inet_ntoa(*(struct in_addr*)host->h_addr_list[ipIndex])));
	memcpy(socket_info->ip,ipAddr,strlen(ipAddr));
	socket_info->port = port;
	//send timeout
	socket_info->connect_time = timeout;
	//recoive time out
	socket_info->recv_time = timeout;

	socket_info->union_https_flag = 0;
	//打印
	//socket_info->is_show_title = 1;
	//send buf
	if (szSendBuf != NULL)
	{
		socket_info->sendlen = nSendLen;
		socket_info->sendbuf = szSendBuf;
	}
	//default socket id
	socket_info->socket_id = -1;
	return 0;
}

socket_parameters_t g_socket_info;
    int g_socket_fd = 0;
    SSL_CTX *g_ctx = NULL;
    SSL     *g_ssl = NULL;

int set_ssl_cert(SSL_CTX *ctx)
{
	int ret = 0;
	unsigned char *pCaCertPath = NULL;
	unsigned char *pClientPubCertPath = NULL;
	unsigned char *pClientPrivatetPath = NULL;
	unsigned char AppPath[128] = {0};
	// Set up encryption suite
	const char *cipher_list = "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256";
	if (SSL_CTX_set_cipher_list(ctx, cipher_list) != 1) {
		ERR_print_errors_fp(stderr);
		OsLog(LOG_DEBUG,"SSL_CTX_set_cipher_list Failed ");
		return -1;
	}
	do
	{
		Business_getAppPath(AppPath,128);
		pCaCertPath = malloc(128);
		memset(pCaCertPath,0,128);
		sprintf(pCaCertPath,"%s/res/CA.pem",AppPath);
		pClientPubCertPath = malloc(128);
		memset(pClientPubCertPath,0,128);
		sprintf(pClientPubCertPath,"%s/res/client.pem",AppPath);
		pClientPrivatetPath = malloc(128);
		memset(pClientPrivatetPath,0,128);
		sprintf(pClientPrivatetPath,"%s/res/client.key",AppPath);
		
		if (!SSL_CTX_load_verify_locations(ctx, pCaCertPath, NULL)) {
			OsLog(LOG_DEBUG,"SSL_CTX_load_verify_locations Failed from %s",pCaCertPath);
			//SSL_CTX_free(ctx);
			ret = -6;
			break;
		}
		/*Load client certificate*/
		if (SSL_CTX_use_certificate_file(ctx, pClientPubCertPath, SSL_FILETYPE_PEM) != 1) {
			OsLog(LOG_DEBUG,"Failed to load client certificate from %s", pClientPubCertPath);
			//SSL_CTX_free(ctx);
			ret = -6;
			break;
		}
		/*Load client private key*/
		if (SSL_CTX_use_PrivateKey_file(ctx, pClientPrivatetPath, SSL_FILETYPE_PEM) != 1) {
			OsLog(LOG_DEBUG,"Failed to load client private key from %s", pClientPrivatetPath);
			//SSL_CTX_free(ctx);
			ret = -6;
			break;
		}
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
		/* code */
	} while (0);

	free(pCaCertPath);
	free(pClientPrivatetPath);
	free(pClientPubCertPath);
	return ret;
}	

s32 ssl_server_connect(char * pHost,ssize_t port,int timeout,int mode)
{
    Rc_t rc = RC_FAIL;
    int ret ;
    s32 nLen,nRet;
	char szBuffer[256];
	
	//Initialize communication parameters
	ZERO_STRUCT(g_socket_info);
	nRet = init_socket_para(&g_socket_info,pHost,port, NULL, 0,timeout);
	if (nRet)
	{
		return rc;
	}

    struct timeval tv;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));

    if((ret = socket_conversion_parameters(&g_socket_info,(struct sockaddr_in *)&addr))!=NORMAL){
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"socket parameters err!");
        #endif
        return rc;
    }
    if((g_socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"create_socket err!");
        #endif
        rc = RC_NET_ERR;
        return rc;
    }
    tv.tv_sec = 15;//(g_socket_info.recv_time<=0)?SOCKET_RECV_TIMEOUT:g_socket_info.recv_time;
    tv.tv_usec = 0; 
    setsockopt(g_socket_fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));
    //socket_set_noblock(g_socket_fd,1);
    tv.tv_sec = 15;//(g_socket_info.connect_time<=0)?SOCKET_CONNECT_TIMEOUT:g_socket_info.connect_time;
    tv.tv_usec = 0; 
    if ( g_socket_info.is_show_title == 1 || g_socket_info.is_show_title == 0) {
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"ssl connecting...");
        #endif
    }
    ret = socket_connect(g_socket_fd,addr,tv);
    if ( ret ) {
        closesocket(g_socket_fd);
        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"connect :%s:%d err[%s]",g_socket_info.ip,g_socket_info.port,strerror(errno));
        #endif
        rc = RC_NET_ERR; 
        goto FAIL;
    }
    #ifdef CFG_DBG
    OsLog(LOG_DEBUG,"connect :%s:%d success!",g_socket_info.ip,g_socket_info.port);
    #endif
    //socket_set_noblock(g_socket_fd,0);
    //SSL_rand_poll();
    /// SSL initialization
    SSL_library_init();
#ifdef CFG_SSL_V3
	if ((g_ctx = SSL_CTX_new(SSLv3_client_method())) == NULL)
#else
	if ((g_ctx = SSL_CTX_new(SSLv23_client_method())) == NULL)
#endif
    {
        closesocket(g_socket_fd);
        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        rc = RC_FAIL;
        goto FAIL;
    }
    //Set the maximum protocol version to TLSv1.2
    if (!SSL_CTX_set_max_proto_version(g_ctx, TLS1_2_VERSION)) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(g_ctx);
        closesocket(g_socket_fd);

        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        rc = RC_FAIL;
        goto FAIL;
    }

    // Set the minimum protocol version, and if necessary, limit the minimum version to TLSv1.2
    if (!SSL_CTX_set_min_proto_version(g_ctx, TLS1_2_VERSION)) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(g_ctx);
        closesocket(g_socket_fd);

        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        rc = RC_FAIL;
        goto FAIL;
    }
	if(mode == SSL_VERIFY_NONE)
	{
		SSL_CTX_set_verify(g_ctx, SSL_VERIFY_NONE, NULL);
	}
	else
	{
		if(set_ssl_cert(g_ctx) != 0)
		{
			SSL_CTX_free(g_ctx);
			closesocket(g_socket_fd);

			g_ssl = NULL;
			g_ctx = NULL;
			g_socket_fd = 0;
			rc = RC_FAIL;
			goto FAIL;
		}
	}
	
    g_ssl = SSL_new(g_ctx);
    if (g_ssl == NULL) {
        SSL_CTX_free(g_ctx);
        closesocket(g_socket_fd);

        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        rc = RC_FAIL;
        goto FAIL;
    }
#ifdef CFG_DBG
	OsLog(LOG_DEBUG,"SSL: ssl set host name[%s]",g_socket_info.host);
#endif    
	SSL_set_tlsext_host_name(g_ssl, g_socket_info.host);
    if ( (ret = SSL_set_fd(g_ssl,g_socket_fd)) < 0) {
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"SSL: ssl set fd err[%d][%s]",ret,SSL_state_string_long(g_ssl));
        #endif
        SSL_free(g_ssl);
        SSL_CTX_free(g_ctx);
        closesocket(g_socket_fd);

        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        rc = RC_FAIL;
        goto FAIL;
    }
     
    if ( (ret = SSL_connect(g_ssl)) != 1 ) {
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"SSL: ssl connect err[%d][%s]",ret,SSL_state_string_long(g_ssl));
        #endif
        SSL_shutdown(g_ssl);
        SSL_free(g_ssl);
        SSL_CTX_free(g_ctx);
        closesocket(g_socket_fd);

        g_ssl = NULL;
        g_ctx = NULL;
        g_socket_fd = 0;
        rc = RC_FAIL;
        goto FAIL;
    }
	
    rc = RC_SUCCESS;

	// Verify the validity of the certificate
	if(mode != SSL_VERIFY_NONE)
	{
		if ((ret = SSL_get_verify_result(g_ssl)) != X509_V_OK) {
			// verify fail
			OsLog(LOG_DEBUG,"SSL_get_verify_result Failed [%d]",ret);
			SSL_shutdown(g_ssl);
			SSL_free(g_ssl);
			SSL_CTX_free(g_ctx);
			closesocket(g_socket_fd);

			g_ssl = NULL;
			g_ctx = NULL;
			g_socket_fd = 0;
			rc = RC_FAIL;
			goto FAIL;
		}
		else
		{
			OsLog(LOG_DEBUG,"SSL_get_verify_result success [%d]",ret);
		}
	}

FAIL:
	
    return rc;
}


void ssl_server_disconnect()
{
    if(g_ssl)
    {
        SSL_shutdown(g_ssl);
        SSL_free(g_ssl);
    }
    if(g_ctx)
    {
        SSL_CTX_free(g_ctx);
    }
    closesocket(g_socket_fd);

    g_ssl = NULL;
    g_ctx = NULL;
    g_socket_fd = 0;
}

s32 ssl_send_msg(char * pData, ssize_t DataLength, int timeout)
{
	int ret;
    if (pData != NULL)
	{
		g_socket_info.sendlen = DataLength;
		g_socket_info.sendbuf = pData;
	}
	if(g_ssl == NULL)
		DSP_Debug();
    if ( (ret = SSL_write(g_ssl,g_socket_info.sendbuf,g_socket_info.sendlen)) < 0 ) {
        #ifdef CFG_DBG
        OsLog(LOG_DEBUG,"SSL: ssl send err[%d][%s]",ret,SSL_state_string_long(g_ssl));
        #endif
    }

	OsLog(LOG_DEBUG,"SSL: ssl_send_msg ret =%d",ret);
    return ret;
}

s32 ssl_recv_msg(char * outbuf, ssize_t recvlen, int timeout)
{
	ssize_t len = 0;
	int offset = 0;
	int pending_status;
	if((g_ssl == NULL) || (outbuf == NULL) || (recvlen == 0)){
		return -1;
	}
	//setSslReadTimeout(g_ssl,timeout);
	//Receive information from the server
	offset = 0;
	len = setSslReadTimeout(g_ssl,timeout);
	if(len > 0){
		while ((len = SSL_read(g_ssl, outbuf + offset, recvlen -1 - offset)) > 0) {
			OsLog(LOG_DEBUG,"SSL: SSL_read ret[%d]",len);
			offset += len;
			len = setSslReadTimeout(g_ssl,500);
			if(len <= 0){
				break;
			}
    	}
		OsLog(LOG_DEBUG,"SSL: SSL_read: %s",outbuf);
	}else{
		OsLog(LOG_DEBUG,"SSL: SSL_read Timeout [%d]",len);
       	offset = len;
	}

	return offset;
}

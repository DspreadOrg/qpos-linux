#ifndef _NETWORK_H__
#define _NETWORK_H__ 

#ifdef __cplusplus
extern "C"
{
#endif

#define SOCKET_CONNECT_TIMEOUT           60
#define SOCKET_RECV_TIMEOUT              60

typedef enum __http_method {
    HTTP_METHOD_GET  = 0,                       /* GET操作 */
    HTTP_METHOD_POST = 1,                       /* POST操作 */
}http_method;				/* ----------  end of enum http_method  ---------- */


/* http  */
typedef struct __http_parameters_t {
    char url[256];                              /* 完整的URL地址 */
    int  port;                                  /* 端口: 默认get http:80 https:443*/
    int  method;                                /* 访问方法 GET、POST */
    struct curl_slist *chunk;                   /* http 访问头部信息 */
    int  connect_time;                          /* 连接超时时间，默认30s */
    int  recv_time;                             /* 接收超时时间，默认30s */
    char *postdata;                             /* POST数据*/
	int posdata_len;							//POST数据的长度
}http_parameters_t;				/* ----------  end of struct http_parameters_t  ---------- */


typedef struct __socket_parameters_t {
	int  custno;								//当前商户编号
	char standby_flag;							//是否使用备用域名ip标识
    char host[256];
    char ip[256];                               /* IP、域名 */
    int  port;                                  /* 端口 */
    int  ssl_flag;                              /* 是否启动SSL */
	int  union_https_flag;						//直连银联公网标识
	int  normal_http_flag;						//普通http报文头处理
    int  connect_time;                          /* 连接超时时间，单位秒,默认30s */
	int  connect_time_us;                       /* 连接超时时间，单位微秒,默认0s */
    int  recv_time;                             /* 接收超时时间，单位秒,默认30s */
	int  recv_time_us;                          /* 接收超时时间，单位微秒,默认0s */
    int  is_show_title;                         /* 是否显示倒计时 //0前台、1后台、2不显示标识*/
    int  sendlen;
    char *sendbuf;
	int socket_id;								//返回的预拨号的socket id
	SSL_CTX *ctx;								
	SSL *ssl;
}socket_parameters_t;				/* ----------  end of struct socket_parameters_t  ---------- */


typedef struct __ftp_parameters_t {
    char url[256];
    char src_file[128];                         /* 源文件绝对路径 */
    char dst_file[128];                         /* 目标文件路径 */
    char username[64];                          /* 登录账户 */
    char userpwd[64];                           /* 密码  */
}ftp_parameters_t;				/* ----------  end of struct ftp_parameters_t  ---------- */


#define  READBUFF_SIZE                      (4*1024) 

#define NETWORK_DEFAULT_IP_PING             "114.114.114.114"

extern char *err_mess[];

int pub_pack_header(char * szPackStr, int nPackLen, char * szRetBuf, int *nRetLen);
int network_normal_commu(char * szSendBuf, int nSendLen,char * szReadBuf, int * nReadLen);

int network_init_socket_para(socket_parameters_t *socket_info,char * szSendBuf, int nSendLen);
int network_protocol_socket_tcp(socket_parameters_t *parameters, int recvlen, char *outbuf);
int network_protocol_socket_udp(socket_parameters_t *parameters, int recvlen, char *outbuf);
int network_get_err_msg  (int errCode,int buff_size,int *outlen, char *message);
int network_protocol_socket_tcp_pre_connect (socket_parameters_t *parameters);
int network_protocol_socket_tcp_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag);
int network_protocol_socket_tcp_read (socket_parameters_t *parameters, int recvlen,char *outbuf);
int network_protocol_socket_tcp_dialoff (socket_parameters_t *parameters);
int network_protocol_socket_tcp_write (socket_parameters_t *parameters);

#ifdef __cplusplus
}
#endif
#endif

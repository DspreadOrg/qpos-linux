#ifndef _NETWORK_H__
#define _NETWORK_H__ 

#ifdef __cplusplus
extern "C"
{
#endif

#define SOCKET_CONNECT_TIMEOUT           60
#define SOCKET_RECV_TIMEOUT              60

typedef enum __http_method {
    HTTP_METHOD_GET  = 0,                       /* GET method */
    HTTP_METHOD_POST = 1,                       /* POST method */
}http_method;				/* ----------  end of enum http_method  ---------- */


/*
 * http
 */
typedef struct __http_parameters_t {
    char url[256];                              /* Complete URL address */
    int  port;                                  /* Port: default get http:80 https:443*/
    int  method;                                /* access method GET、POST */
    struct curl_slist *chunk;                   /* http Accessing header information */
    int  connect_time;                          /* Connection timeout, default 30 seconds */
    int  recv_time;                             /* Receive timeout, default is 30s */
    char *postdata;                             /* POST data*/
	int posdata_len;							//POST data length
}http_parameters_t;				/* ----------  end of struct http_parameters_t  ---------- */


typedef struct __socket_parameters_t {
	int  custno;								//Current merchant ID
	char standby_flag;							//Do you want to use an alternative domain IP identifier
    char host[256];
    char ip[256];                               /* IP  domain name*/
    int  port;                                  /* Port */
    int  ssl_flag;                              /* start SSL or not */
	int  union_https_flag;						//Directly connect to the UnionPay public network identifier
	int  normal_http_flag;						//Processing of ordinary HTTP message headers
    int  connect_time;                          /* Connection timeout, unit: seconds, default: 30s */
	int  connect_time_us;                       /* Connection timeout, unit: microseconds, default: 0s */
    int  recv_time;                             /* Receive timeout, unit: seconds, default: 30s */
	int  recv_time_us;                          /* Receive timeout, in microseconds, default 0s */
    int  is_show_title;                         /* Whether to display countdown//0 front-end, 1 back-end, 2 do not display identification*/
    int  sendlen;
    char *sendbuf;
	int socket_id;								//Return the socket ID of the pre dialed number
	SSL_CTX *ctx;								
	SSL *ssl;
}socket_parameters_t;				/* ----------  end of struct socket_parameters_t  ---------- */


typedef struct __ftp_parameters_t {
    char url[256];
    char src_file[128];                         /* Absolute path of source file */
    char dst_file[128];                         /* Target file path */
    char username[64];                          /* Login Account */
    char userpwd[64];                           /* Password  */
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

﻿#ifndef _SOCKET_H__
#define _SOCKET_H__  

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define	INVALID_SOCKET			-1
#define closesocket(sock)		close(sock)


int socket_connect (int socket, struct sockaddr_in addr,struct timeval tv);
int socket_recv(int nCustNo, int socket, char *buf, int len,struct timeval tv);

int protocol_socket_tcp(socket_parameters_t *parameters, int recvlen,char *outbuf);
int protocol_socket_udp (socket_parameters_t *parameters, int recvlen, char *outbuf);

int protocol_socket_tcp_pre_connect (socket_parameters_t *parameters);
int protocol_socket_tcp_follow_connect (socket_parameters_t *parameters, char *pre_connect_flag);
int protocol_socket_tcp_write (socket_parameters_t *parameters);
int protocol_socket_tcp_read (socket_parameters_t *parameters, int recvlen,char *outbuf);
int protocol_socket_tcp_dialoff (socket_parameters_t *parameters);
int socket_gethostbyname (int nCustNo, char * indata, char * outdata);
int socket_tcp_alive (socket_parameters_t *parameters, int recvlen,char *outbuf);
int socket_tcp_dialoff (socket_parameters_t *parameters);

extern long ssl_server_connect(char * pHost,ssize_t port,int timeout);
extern void ssl_server_disconnect();
extern long ssl_send_msg(char * pData, ssize_t DataLength, int timeout);
extern long ssl_recv_msg(char * outbuf, ssize_t recvlen, int timeout);
#endif

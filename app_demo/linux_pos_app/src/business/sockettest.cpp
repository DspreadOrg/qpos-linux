#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
#include "appinc.h"

int cat1_tcp_connect(int *socket_fd, char *ip, int port)
{
	int sockfd = -1;
	struct sockaddr_in servaddr;
 
	if(ip == NULL || socket_fd == NULL){
		return -1;
	}

	//初始化服务器套接字地址
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;//IPv4
	servaddr.sin_port = htons(port);//想连接的服务器的端口
	servaddr.sin_addr.s_addr = inet_addr(ip);//服务器的IP地址
 
	OsLog(LOG_INFO,"----socket test: ip: %s, ip addr = 0x%08x----", ip, servaddr.sin_addr.s_addr);

	//创建套接字
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		OsLog(LOG_INFO,"create socket error: %s(error: %d)\n", strerror(errno), errno);
		return -1;
	}

	//向服务器发送连接请求
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		//连接失败
		OsLog(LOG_INFO,"connect socket error: %s(error: %d)\n", strerror(errno), errno);
		return -1;
	}
 
	*socket_fd = sockfd;
	return 0;
}

int cat1_tcp_send_msg(int socket_fd, char * sendmsg, ssize_t sendlen, int timeout)
{
	ssize_t len = 0;
	int offset = 0;
	if((socket_fd == 0) || (sendmsg == NULL) || (sendlen == 0)){
		return -1;
	}

	//向服务器发送信息
	offset = 0;
	while(sendlen){
		if(timeout == 0) return -1;
		len = send(socket_fd, sendmsg + offset, sendlen, MSG_DONTWAIT);
		OsLog(LOG_INFO,"write len = %d, offset = %d, timeout = %d", len, offset, timeout);
		if(len < 0){
			// perror("cat1_tcp_send_msg");
			if(errno == EINTR){
				continue;
			} else if(errno == EAGAIN){
				usleep(1000);
				timeout--;
				continue;
			}
			return -1;
		}

		offset += len;
		sendlen -= len;
	}

	return offset;
}

int cat1_tcp_recv_msg(int socket_fd, char * recvmsg, ssize_t requestlen, int timeout)
{
	ssize_t len = 0;
	int offset = 0;
	if((socket_fd == 0) || (recvmsg == NULL) || (requestlen == 0)){
		return -1;
	}
	
	//从服务器接收信息
	offset = 0;
	while(requestlen){
		if(timeout == 0) return -1;
		len = recv(socket_fd, recvmsg + offset, requestlen, MSG_DONTWAIT);
		// OsLog(LOG_INFO,"read len = %d, offset = %d, timeout = %d", len, offset, timeout);
		if(len < 0){
			// perror("cat1_tcp_recv_msg");
			if(errno == EINTR){
				continue;
			} else if(errno == EAGAIN){
				usleep(1000);
				timeout--;
				continue;
			}
			return -1;
		}
		requestlen -= len;
		offset += len;
 	}

	return offset;
}

int cat1_tcp_disconnect(int socket_fd)
{
	return close(socket_fd);
}

int ppp_connect_test()
{
    int ret = -1;
    int socketfd = -1;
    struct hostent *host;
	char sendbuf[1024] = {"GET / HTTP/1.1\r\nHost: baidu.com\r\n\r\n"};
	char recvbuf[10240] = {0};
    char ipaddr[128] = {0};

    do{
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Connect To Server");
        host = gethostbyname("www.baidu.com");
        if(host == NULL) {
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Domain Name Resolution Failed");
            ret = -1;
            break;
        }
        //IP地址
        for(int i=0; host->h_addr_list[i]; i++){
            OsLog(LOG_INFO,"IP addr %d: %s\n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        }

        ret = cat1_tcp_connect(&socketfd, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]), 80);
        if(ret != 0){
            OsLog(LOG_INFO,"socket connect failed");
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Server Connection Failed");
            ret = -1;
            break;
        }
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Send Data");
        ret = cat1_tcp_send_msg(socketfd, sendbuf, strlen(sendbuf), 1000);
        if(ret <= 0){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Send Data Failed");
            ret = -1;
            break;
        }
		TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Receive Data");
        ret = cat1_tcp_recv_msg(socketfd, recvbuf, sizeof(recvbuf) - 1, 5000);
        if(ret <= 0){
            TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Receive Data Failed");
            ret = -1;
            break;
        }
        OsLog(LOG_INFO,"recv:\r\n%s", recvbuf);
        OsLog(LOG_INFO,"PPP success");
        TransView_vShowLine(2,EM_DTYPE_NORMAL,EM_ALIGN_CENTER,(char*)"Communication Success");
        ret = 0;
    }while (0);

    if(socketfd != -1){
        cat1_tcp_disconnect(socketfd);
    }
    return ret;
}
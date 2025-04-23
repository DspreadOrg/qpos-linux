#include "appinc.h"
#include "ntp.h"

#define NTP_TIMESTAMP_DELTA 2208988800ull

// Structure that defines the 48 byte NTP packet protocol.
typedef struct
{

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
// li.   Two bits.   Leap indicator.
// vn.   Three bits. Version number of the protocol.
// mode. Three bits. Client will pick mode 3 for client.
    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.


int setBeijingTime(time_t ntpTm)
{
    ST_TIME t_systime;
    time_t beijingTime= ntpTm + 8*60*60;  //Beijing time and UTC are 8 hours apart
    struct tm *tm_info = localtime(&beijingTime);  
    t_systime.Year = tm_info->tm_year + 1900;
    t_systime.Month = tm_info->tm_mon + 1;
    t_systime.Day =  tm_info->tm_mday;
    t_systime.Hour = tm_info->tm_hour;
    t_systime.Minute = tm_info->tm_min;
    t_systime.Second = tm_info->tm_sec;

    OsLog(LOG_DEBUG,"Online time synchronization  %d-%d-%d %d:%d:%d \n", t_systime.Year,t_systime.Month,t_systime.Day,t_systime.Hour,t_systime.Minute,t_systime.Second);
    if(OsSetTime(&t_systime) == 0){
        return 0;
    }else{
        return -1;
    }
}

int ntp_sync_time(char *host_name)
{   
    struct timeval tv;
    int sockfd, n,ret = -1; // Socket file descriptor and the n return result from writing/reading from the socket.
    int portno = 123; // NTP UDP port number.
// char* host_name = "us.pool.ntp.org"; // NTP server host-name.
// Create and zero out the packet. All 48 bytes worth.
    ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    memset( &packet, 0, sizeof( ntp_packet ) );

// Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
    *( ( char * ) &packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

// Create a UDP socket, convert the host-name to an IP address, set the port number,
// connect to the server, send the packet, and then read in the return packet.
    struct sockaddr_in serv_addr; // Server address data structure.
    struct hostent *server;      // Server data structure.

    sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
    if ( sockfd < 0 ){
        OsLog(LOG_ERROR,"ERROR opening socket" );
        ret = -1;
        goto EXIT;
    }
    server = gethostbyname( host_name ); // Convert URL to IP.
    if ( server == NULL ){
        OsLog(LOG_ERROR,"ERROR, no such host" );
        ret = -1;
        goto EXIT;
    }
// Zero out the server address structure.
    bzero( ( char* ) &serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
// Copy the server's IP address to the server address structure.
    bcopy( ( char* )server->h_addr, ( char* ) &serv_addr.sin_addr.s_addr, server->h_length );
// Convert the port number integer to network big-endian style and save it to the server address structure.
    serv_addr.sin_port = htons( portno );
// Call up the server using its IP address and port number.
    if ( connect( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr) ) < 0 ){
        OsLog(LOG_ERROR,"ERROR connecting" );
        ret = -1;
        goto EXIT;
    }

// Send it the NTP packet it wants. If n == -1, it failed.
    n = write( sockfd, ( char* ) &packet, sizeof( ntp_packet ) );
    if ( n < 0 ){
        OsLog(LOG_ERROR,"ERROR writing to socket" );
        ret = -1;
        goto EXIT;
    }

// Wait and receive the packet back from the server. If n == -1, it failed.
    tv.tv_sec = 5;
	tv.tv_usec = 0; 
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));
    n = read( sockfd, ( char* ) &packet, sizeof( ntp_packet ) );
    if ( n < 0 ){
        OsLog(LOG_ERROR,"ERROR reading from socket" );
        ret = -1;
        goto EXIT;
    }

// These two fields contain the time-stamp seconds as the packet left the NTP server.
// The number of seconds correspond to the seconds passed since 1900.
// ntohl() converts the bit/byte order from the network's to host's "endianness".
    packet.txTm_s = ntohl( packet.txTm_s ); // Time-stamp seconds.
    packet.txTm_f = ntohl( packet.txTm_f ); // Time-stamp fraction of a second.
// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
// Subtract 70 years worth of seconds from the seconds since 1900.
// This leaves the seconds since the UNIX epoch of 1970.
// (1900)------------------(1970)**************************************(Time Packet Left the Server)
    time_t txTm = ( time_t ) ( packet.txTm_s - NTP_TIMESTAMP_DELTA );
// Print the time we got from the server, accounting for local timezone and conversion from UTC time.
// OsLog(LOG_ERROR,"Time: %s", ctime( ( const time_t* ) &txTm ) );
// Set Beijing Time
    if(setBeijingTime(txTm) == 0){
        OsLog(LOG_ERROR,"Set Systime Success");
        ret = 0;
    }else{
        OsLog(LOG_ERROR,"Set Systime Failed");
        ret = -1;
    }
EXIT:
    if(sockfd >= 0){
        close(sockfd);
        sockfd = -1;
    }
    return ret;
}


const char *ntp_server_list[] = {
    "time.windows.com",                        
    "pool.ntp.org",                         
    "us.pool.ntp.org",                                                                                        
};

void *ntp_sync_time_process (void *argv)
{
    int i = 0;
    int ret = 0;
    long long start = get_sys_tick();

    while (get_sys_tick() - start < 20*1000)
    {   
        if(Is_Valid_Network() != PR_NORMAL){
            OsSleep(1000);
            continue;
        }

        for ( i=0 ; i<PR_ARRAY_SIZE(ntp_server_list) ; i++ ) {
            ret = ntp_sync_time((char *)ntp_server_list[i]);
            if ( ret == 0 ) {
                return NULL;
            }
        }
    }
    return NULL;
}

void Ntp_TimeSync_Proc (void)
{   
    pthread_t tid;
    pthread_create(&tid,NULL,ntp_sync_time_process,NULL);
}	
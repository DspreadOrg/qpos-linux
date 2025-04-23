
#ifndef NTP_H
#define NTP_H

#ifdef __cplusplus
extern "C"
{
#endif

int ntp_sync_time(char *host_name);

void Ntp_TimeSync_Proc (void);

#ifdef __cplusplus
}
#endif
#endif /* NTP_H */

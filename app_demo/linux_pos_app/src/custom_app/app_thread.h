/*
 * app_thread.h
 *  Note:app new thread
 */

#ifndef _APP_THREAD_H_
#define _APP_THREAD_H_

#include "appinc.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _Switch_t
{
	MUTEX_OFF = 0,
	MUTEX_NO = 1
}Switch_t;

typedef struct _queue_message_body_t
{
	long msgType;
	message_body_t msg;
} queue_message_body_t;


void *Event_Thread(void *args);
void *Signal_Thread(void *args);
void set_wifi_conn_status(bool status);
bool get_wifi_conn_status();
u32 fun_wait_events(u32 events,u32 timeout,pvoid pbuffer );

extern void thread_init();
#ifdef __cplusplus
}
#endif
#endif 

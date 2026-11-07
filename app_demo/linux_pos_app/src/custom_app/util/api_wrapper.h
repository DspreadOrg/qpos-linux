/*
 * app_wrapper.h
 *  Note:app new thread
 */

#ifndef _API_WRAPPER_H_
#define _API_WRAPPER_H_

#include "appinc.h"

#ifdef __cplusplus
extern "C"
{
#endif

void Set_dev_gprs_signal_lvl(u32 lvl);
void Set_dev_wifi_signal_lvl(u32 lvl);
u32 Get_dev_wifi_signal_lvl(void);
u32 Get_dev_gprs_signal_lvl(void);

bool Get_dev_UsbConnect_Status(void);
u32 Get_dev_sim_status(void);
u32 fun_gprs_get_connect_mode();

u32 Get_dev_battery_lvl(void);

extern void transPoolSetTag(int tag,pu8 pValue,int Len);
extern void app_trace_value(unsigned char* pbuf, unsigned int cnt);
extern void GuiEventRegister(u32 msg_id);
#ifdef __cplusplus
}
#endif
#endif 
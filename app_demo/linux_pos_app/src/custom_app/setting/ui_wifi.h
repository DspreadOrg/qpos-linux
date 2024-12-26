#ifndef _UI_WIFI_H
#define _UI_WIFI_H

#define      MAX_SIZE_NAME_WIFI          64
#define      MAX_SIZE_PASSWORD_WIFI      30

extern void DispWifiList();
extern void DispWifiSetting();
extern void netWifiAlwaysConnect_cb(lv_timer_t * timer);
extern void netWifiConnect_cb();
extern void searchWifiNetworks();
extern void DispWifiConnected();

#endif
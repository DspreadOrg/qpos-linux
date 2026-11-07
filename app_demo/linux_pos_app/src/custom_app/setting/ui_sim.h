#ifndef _UI_SIM_H
#define _UI_SIM_H

#include "lvgl_porting.h"

extern void SimConnecting_cb(lv_timer_t * timer);
extern char* get_name_operator(char * nameOperator);
extern void DispConnectDisconnectSim();
extern void DispSimConnectionMessage(const char * message, bool OK_icon);
extern void ValidateSim_cb();
extern void DispAfterSimSettings();
extern void DispSimSetting();

#define EDATEL      "732002"
#define CLARO       "732101"
#define CLARO2      "732100"
#define CLARO3      "732102"
#define MOVISTAR    "732001"
#define MOVISTAR2   "732123"
#define MOVISTAR3   "732124"
#define WOW         "732360"
#define TIGO2       "732103"
#define TIGO        "732111"

#endif
#ifndef _UI_MULTIAPP_H
#define _UI_MULTIAPP_H
#include "app_includes.h"

#define ERROR_CODE          (-1) 

extern void selectMultiApp( pu8 tlvData );
extern void DispMultiAppList( void );
extern bool unpackAppData( pu8 tlvData );

#endif
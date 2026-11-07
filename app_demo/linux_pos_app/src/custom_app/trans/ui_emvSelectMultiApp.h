#ifndef _UI_EMVSELECTMULTIAPP_H
#define _UI_EMVSELECTMULTIAPP_H
#include "app_includes.h"

#define ERROR_CODE          (-1) 

void emvSelectMultiApp( pu8 tlvData );
void DispMultiAppList( void );
bool unpackAppData( pu8 tlvData );

#endif
#ifndef _UI_PIN_EMVPINDISP_H
#define _UI_PIN_EMVPINDISP_H
#include "app_includes.h"

typedef enum{
	PinTypeOnline,
	PinTypeOffline,
	PinTypeOfflineLastePin,
    PinTypeUndef
}EPinType;

extern void emvPinDisp(unsigned char *maskPin);

extern void SetPinType( EPinType type );

extern void UpdatePinMask( pvoid param );

extern void DispPinMask( void );
extern void DispPinEntry( void );
#endif
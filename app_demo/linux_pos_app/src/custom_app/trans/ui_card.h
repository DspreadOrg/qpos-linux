#ifndef _UI_CARD_H
#define _UI_CARD_H
#include "app_includes.h"

typedef struct
{
    Card_Type_t  type;
    char         amt[32];
    u32          timeout;
}StartTransaction_t,*pStartTransaction_t;

extern void DispCardsSupported( void );
extern void DispSeePhone();
extern void DispCards(u32 cardsSupported );
#endif
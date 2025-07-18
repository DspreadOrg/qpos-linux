
#include "app_trans.h"
#include "ui_emvSelectMultiApp.h"

pu8 g_pTransBuffer = NULL;
extern ONLINE_STATUS onlineStatus;
extern u32 lCardsSupported;
/*-----------------------------------------------------------------------------
|   Function
+----------------------------------------------------------------------------*/
static bool lFallbackActive = false;
/*----------------------------------------------------------------------------
|   Function Name:
|       PaymentInit
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void PaymentInit()
{
    sale_init();
}

/*----------------------------------------------------------------------------
|   Function Name:
|       TransPoolSet
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void TransPoolSet(u32 tag,T_U8_VIEW tag_value)
{
    set_tlv_view(g_pTransBuffer,tag,tag_value);
}

void TransPoolUpdate(u32 tag,pu8 pValue,u32 Len)
{
    tlv_replace_(g_pTransBuffer,tag,Len,pValue);
}

u32 getDataFromTransPool(u32 tag,pu8 pData)
{
    return get_tlv_value(g_pTransBuffer, tag, pData);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       GetCardData
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void GetCardData(pu8 pTlv)
{
    unsigned int tag;
    unsigned int length;
    unsigned char *pValue=0;
    unsigned char *pChild=0;

    pChild = tlv_get_first_child_(pTlv);
    while(pChild)
    {
        tag	= tlv_get_t_(pChild);
        length = tlv_get_l_(pChild);
        pValue = tlv_get_v_(pChild);

        tlv_replace_(g_pTransBuffer,tag,length,pValue);

        pChild = tlv_get_next_child_(pChild);
    }
}

void Trans_Payment()
{
    if(g_pTransBuffer)
    {
        free(g_pTransBuffer);
        g_pTransBuffer=NULL;
    }

    OsLog(LOG_DEBUG,"Dspread: <%s,%d>--------------",__FUNCTION__,__LINE__);
    g_pTransBuffer=malloc(TRANS_POOL_MAX);
    memset(g_pTransBuffer,0,TRANS_POOL_MAX);
    PaymentInit();
    Enter_Amount();
}

void Start_Payment( void ) {
	GuiEventRegister(LCD_DISP_START_TRANSACTION);
	lCardsSupported = CARD_NFC|CARD_IC|CARD_MAG;
    read_cards_process(lCardsSupported);
}


void startFallback( void ) {
	lFallbackActive = true;
	GuiEventRegister(LCD_DISP_START_TRANSACTION);
	read_cards_process(CARD_MAG);
}
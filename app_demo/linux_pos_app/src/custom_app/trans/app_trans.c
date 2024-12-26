
#include "app_trans.h"
#include "app_pkt.h"
#include "ui_emvSelectMultiApp.h"

pu8 g_pTransBuffer = NULL;
extern Req_Trans_Type glb_trans_type;
extern ONLINE_STATUS onlineStatus;

#ifdef CFG_DBG
static const char *strCallback[] =
{
    "SWIPE_CARD",
    "TRANS_ONLINE_PIN",
    "TRANS_OFFLINE_PIN",
    "TRANS_NFC_ONLINE",
    "TRANS_NFC_APPROVED",
    "TRANS_NFC_DECLINE",
    "TRANS_OFFLINE_LASTE_PIN",
    "TRANS_REVERSAL",
    "TRANS_APP_SELECT",
    "TRANS_ACCOUNT_TYPE",
    "TRANS_ICC_GAC2",
    "TRANS_ICC_ONLINE",
    "TRANS_PROCESS_CUSTOM",
};
#endif

/// Enumerador con el modo de lectura seleccionado para la transacci贸n
typedef enum TradMode {
	/// Modo de lectura solo chip
	READ_MODE_ICC = 0x01,
	/// Modo de lectura solo banda magn茅tica
	READ_MODE_MAG,
	/// Modo de lectura banda magn茅tica, chip y sin contacto
	READ_MODE_ICC_MAG_NFC,
	/// Modo de lectura banda magn茅tica, chip y sin contacto (sin downgrade)
	READ_MODE_ICC_MAG_NFC_DONT_ALLOW_DOWNGRADE,
	/// Modo de lectura solo banda magn茅tica y chip
	READ_MODE_ICC_MAG,
	/// Modo de lectura solo banda magn茅tica y chip (sin downgrade)
	READ_MODE_ICC_MAG_DONT_ALLOW_DOWNGRADE,
	/// Modo de lectura solo sin contacto
	READ_MODE_NFC,
	/// Modo de lectura solo banda magn茅tica y sin contacto (sin tecla arriba)
	READ_MODE_ICC_MAG_NFC_NOUPKEY,
	/// Modo de lectura banda magnetica, chip y sin contacto (sin tecla arriba)
	READ_MODE_ICC_MAG_NFC_NOUPKEY_DONT_ALLOW_DOWNGRADE,
	/// Modo de lectura chip y sin contacto (sin tecla arriba)
	READ_MODE_ICC_NFC_DONT_USE_UPKEY,
	/// Modo de lectura chip y sin contacto
	READ_MODE_ICC_NFC,
	/// Modo de lectura chip y sin contacto (con downgrade)
	READ_MODE_ICC_NFC_DONT_ALLOW_DOWNGRADE
} E_TRADE_MODE;

static TRANS_T Trans_init[]=
{
    {TAG_EMV_TRANSACTION_TYPE,          1,  "\x00"},
    {POS_TAG_TRADE_MODE,                1,  "\x09"},
    {TAG_EMV_TRANSACTION_CURRENCY_CODE, 2,  "\x01\x70"},
    {POS_TAG_TRANSACTION_PASSWORD_MIN,  1,  "\x04"},
    {POS_TAG_TRANSACTION_PASSWORD_MAX,  1,  "\x06"},

    /*Master card refund transaction test case needs to added.
	At the same time, the following content needs to be added under the AID tag in the EMV configuration file
	<7F11>9C0120DF812105FFFFFFFFFFDF83020101<7F11>
	Note:rember to set transaction type to 20
	*/
    {POS_TAG_CUSTOM_REQUEST_HOST,       1,  "\x04"},//support Master card refund transaction test case.
};

static const u32 g_CardTlv[]=
{
    POS_TAG_RES_RESULT,
    POS_TAG_RES_PAN,
    POS_TAG_RES_CARDHOLDER,
    TAG_EMV_AID_TERMINAL,
    POS_TAG_RES_SERVICE_CODE,
    TAG_EMV_AMOUNT_AUTH_NUM,
    TAG_EMV_TRANSACTION_TYPE,
    TAG_EMV_CVM_RESULTS,
    POS_TAG_CLEAR_TRACK2,
    POS_TAG_ASC_PIN,
    POS_TAG_RES_VALID_PERIOD,
    POS_TAG_RES_KN_ONLINE_DATA,
    TAG_EMV_CRYPTOGRAM_INFO_DATA,
    TAG_EMV_TERMINAL_CAPABILITIES
};
/*-----------------------------------------------------------------------------
|   Function
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
|   Function Name:
|       PaymentInit
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void PaymentInit(pu8 pTransBuffer)
{
    u32 i=0;
    tlv_new_(pTransBuffer, POS_TAG_HEAD, 0, NULL);
    for(i=0;i<SIZE_ARRAY(Trans_init);i++)
    {
        tlv_add_child_(pTransBuffer, Trans_init[i].tag,Trans_init[i].len,Trans_init[i].value);
    }

    memset(&onlineStatus, 0x00, sizeof(onlineStatus));
	onlineStatus.Rc = RC_FAIL;
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

/*----------------------------------------------------------------------------
|   Function Name:
|       Get_callback_type
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static Callback_Type_t Get_callback_type(pu8 pTradingFile)
{
    Callback_Type_t result_type=CALLBACK_TRANS_MAG;
    T_U8_VIEW uvResult=get_tlv_view(pTradingFile,POS_TAG_RES_RESULT);
    #ifdef CFG_DBG
    OsLog(LOG_DEBUG,"RES_RESULT=[%02x]\r\n",uvResult.head[0]);
    #endif
    if(UV_OK(uvResult))
    {
        switch(uvResult.head[0])
        {
            case EMV_PROCESS_MAG_OK:
            case EMV_PROCESS_MAG_ACCOUNT_TYPE:
                result_type=CALLBACK_TRANS_MAG;
                break;
            case EMV_PROCESS_NFC_ONLINE:
                result_type=CALLBACK_TRANS_NFC_ONLINE;
                break;
            case EMV_PROCESS_NFC_APPROVED:
                result_type=CALLBACK_TRANS_NFC_APPROVED;
                break;
            case EMV_PROCESS_NFC_DECLINE:
                result_type=CALLBACK_TRANS_NFC_DECLINE;
                break;
            case EMV_PROCESS_ICC_ONLINE:
                result_type=CALLBACK_TRANS_ICC_ONLINE;
                break;
            case EMV_PROCESS_ICC_GAC2:
                result_type=CALLBACK_TRANS_GAC2;
                break;
            case EMV_PROCESS_ICC_APP_SELECT:
                result_type=CALLBACK_TRANS_APP_SELECT;
                break;
            case EMV_PROCESS_REVERSAL:
                result_type=CALLBACK_TRANS_REVERSAL;
                break;
            case EMV_PROCESS_ONLINE_PIN:
                result_type = CALLBACK_TRANS_ONLINE_PIN;
                break;
            case EMV_PROCESS_OFFLINE_PIN:
                result_type = CALLBACK_TRANS_OFFLINE_PIN;
                break;
            case EMV_PROCESS_OFFLINE_LASTE_PIN:
                result_type = CALLBACK_TRANS_OFFLINE_LASTE_PIN;
                break;
            case EMV_PROCESS_CUSTOM:
                result_type = CALLBACK_TRANS_PROCESS_CUSTOM;
                break;
            default:
                return CALLBACK_END;
                break;
        }
    }
    return result_type;

}

void emvOnlineProcess(unsigned char *pTransCallbackBuffer)
{
    Rc_t Rc = RC_FAIL;
    u32 events=0;
    pu8 pNode = NULL;
    Callback_Type_t  result_type=Get_callback_type(pTransCallbackBuffer);

    // pNode = tlv_find_(pTransCallbackBuffer,POS_TAG_HEAD);
    // OsLog(LOG_DEBUG, "%s--pTransCallbackBuffer[%d]:\r\n",__FUNCTION__,tlv_get_l_(pNode));
    // TRACE_VALUE(DBG_TRACE_LVL, tlv_get_v_(pNode), tlv_get_l_(pNode));

    #ifdef CFG_DBG
    OsLog(LOG_DEBUG,"transaction callback type=%s-%d\r\n",strCallback[result_type],result_type);
    #endif
    GetCardData(pTransCallbackBuffer);


    switch(result_type)
    {
        case CALLBACK_TRANS_MAG:
            //POS_TAG_RES_PAN POS_TAG_RES_PIN_KSN POS_TAG_RES_PINBLOCK  POS_TAG_RES_SERVICE_CODE
            pNode = tlv_find_(pTransCallbackBuffer,POS_TAG_RES_PAN);
            GuiEventRegister(LCD_DISP_GO_ONLINE);
            Rc = online_common(g_pTransBuffer);
            break;
        case CALLBACK_TRANS_ICC_ONLINE:
            //If the encryption mode is DUKPT mode, the encrypted data can be obtained by the following methods,}
            GuiEventRegister(LCD_DISP_GO_ONLINE);
            Rc = online_common(g_pTransBuffer); //card transaction test sample code
            tlv_move_(g_pTransBuffer,pTransCallbackBuffer,POS_TAG_RES_EN_ONLINE_RESULT);
            if(Rc==RC_SUCCESS)
            {
                //callback online transaction result to card
                //2nd Gen AC,write response code / issue script back to IC card
                GuiEventRegister(LCD_DISP_TRANS_SUCCESS);
            }
            else
            {

                 GuiEventRegister(LCD_DISP_TRANS_FAIL);
            }
            break;
        case CALLBACK_TRANS_NFC_ONLINE:
            GuiEventRegister(LCD_DISP_GO_ONLINE);
             Rc = online_common(g_pTransBuffer); //card transaction test sample code
            if(Rc==RC_SUCCESS)
            {
                //callback online transaction result to card
                //2nd Gen AC,write response code / issue script back to IC card
                tlv_move_(g_pTransBuffer,pTransCallbackBuffer,POS_TAG_RES_EN_ONLINE_RESULT);
                GuiEventRegister(LCD_DISP_TRANS_SUCCESS);
            }
            else
            {
                 GuiEventRegister(LCD_DISP_TRANS_FAIL);
            }
            break;
        case CALLBACK_TRANS_REVERSAL:
            //reversal process
            break;
        default:
            break;
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
    glb_trans_type = REQ_TRANS_SALE;
    PaymentInit(g_pTransBuffer);
    Enter_Amount();
}

void Start_Payment( void ) {
	StartTrading(g_pTransBuffer);
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       emvPorcessDisplay
 |   Description: SDK call back function to handle display
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void emvPorcessDisplay(pmessage_body_t pmsg){
	DspDebug("Dspread: emvPorcessDisplay dispId = %d",pmsg->wparam);
    switch(pmsg->wparam){
		case LCD_DISP_START_TRANSACTION:
			setTransInitData(pmsg->pparam);
			GuiEventRegister(LCD_DISP_START_TRANSACTION);
			break;

		case LCD_DISP_READING_CARD:
			GuiEventRegister(LCD_DISP_READING_CARD);
			break;

        case LCD_DISP_INPUT_ONLINE_PIN:
			SetPinType( PinTypeOfflineLastePin );
            GuiEventRegister(LCD_DISP_INPUT_ONLINE_PIN);
            break;

        case LCD_DISP_INPUT_OFFLINE_PIN:
			SetPinType( PinTypeOffline );
            GuiEventRegister(LCD_DISP_INPUT_OFFLINE_PIN);
            break;

        case LCD_DISP_INPUT_LAST_OFFLINE_PIN:
			SetPinType( PinTypeOfflineLastePin );
            GuiEventRegister(LCD_DISP_INPUT_LAST_OFFLINE_PIN);
            break;

        case LCD_DISP_PIN_CONFIRM:
			break;

		case LCD_DISP_CHIP_CARD:
			GuiEventRegister(LCD_DISP_CHIP_CARD);
			break;

		case LCD_DISP_NFC_RETAP:
			GuiEventRegister(LCD_DISP_NFC_RETAP);
			break;

		case LCD_DISP_SEE_PHONE_PLS:
			GuiEventRegister(LCD_DISP_SEE_PHONE_PLS);
			break;

			default:
				break;
	}
}

/*----------------------------------------------------------------------------
 |   Function Name:
 |       emvTransResult
 |   Description: SDK call back function to handle display
 |   Parameters:
 |   Returns:
 +---------------------------------------------------------------------------*/
void emvTransResult(App_Rc_Code_t reCode){
	DspDebug("Dspread: emvTransResult reCode = %08x",reCode);
    switch(reCode){

		case APP_RC_TIMEOUT:
			GuiEventRegister(LCD_DISP_TIMEOUT);
			break;

		case APP_RC_COMPLETED:
			GuiEventRegister(LCD_DISP_TRADE_SUCCESS);
			break;

		case APP_RC_TERMINAL:
		case APP_RC_NFC_TERMINAL:
			GuiEventRegister(LCD_DISP_TRANSACTION_TERMINATED);
			break;

		case APP_RC_CANCEL:
			GuiEventRegister(LCD_DISP_CANCEL);
			break;

	    case APP_RC_EMV_APP_BLOCK:
	    	GuiEventRegister(LCD_DISP_EMV_APP_BLOCK);
	    	break;

	    case APP_RC_EMV_DENAIL:
	    	GuiEventRegister(LCD_DISP_DECLINED_DISP);
			break;

	    case APP_RC_FALL_BACK:
	    	GuiEventRegister(LCD_DISP_FALL_BACK);
	        break;

	    case APP_RC_CARD_NOT_SUPPORT:
	    	GuiEventRegister(LCD_DISP_CARD_NOT_SUPPORT);
	    	break;

	    case APP_RC_EMV_TRANS_TRY_ANOTHER_INTERFACE:
	    	GuiEventRegister(LCD_DISP_TRY_ANOTHER_INTERFACE);
	    	break;
		case APP_RC_NFC_TRY_AGAIN:
			GuiEventRegister(LCD_DISP_NFC_TAP_TRY_AGAIN);
			break;
		case APP_RC_NFC_SUCCESS:
			GuiEventRegister( LCD_DISP_TRADE_SUCCESS );
			break;
		case APP_RC_MAG_COMPLETED:
			GuiEventRegister( LCD_DISP_TRADE_SUCCESS );
			break;
		case APP_RC_MAG_TRADE_FAIL:
			GuiEventRegister( LCD_DISP_DECLINED_DISP );
			break;	
		case APP_RC_TRANS_REVERSE:
			//disp trans reversel result
			GuiEventRegister( LCD_DISP_TRADE_SUCCESS ); // just a demo
			break;
		default:
			break;
	}
}

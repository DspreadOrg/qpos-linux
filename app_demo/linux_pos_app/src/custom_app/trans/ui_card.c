#include "ui_card.h"

static int lCardsSupported;
void DispCards( u32 cardsSupported );

void setTransInitData( pvoid param ) {
	const pStartTransaction_t pStartTransactionData = ( pStartTransaction_t )param;

	lCardsSupported = (u32)pStartTransactionData->type;
}

void DispCardsSupported( void ) {
	DispCards( lCardsSupported );
}

static void CancelTransactionCallback( lv_event_t * event ) {
	lv_event_code_t code = lv_event_get_code( event );
	const lv_indev_t * indev = lv_indev_get_act( );
	lv_indev_type_t indev_type = lv_indev_get_type( indev );

    if( indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY && lv_indev_get_key( indev ) == LV_KEY_ESC ) {
		TransCancle( );
	}
}

void DispCards( u32 cardsSupported ) {
    lv_timer_enable( false );
	lv_obj_clean( Main_Panel );

    lv_text_create( Main_Panel, "Chip NFC MAG", &title_style, LV_ALIGN_TOP_MID, 0, -3 );

	lv_obj_t * Icon_btn = lv_imgbtn_create( Main_Panel );
	lv_group_remove_all_objs( s_group_keypad_indev );
    lv_obj_add_event_cb( Icon_btn, CancelTransactionCallback, LV_EVENT_ALL, NULL );
    lv_group_add_obj( s_group_keypad_indev, Icon_btn );

    u8 index = 0;
    
    if( cardsSupported & CARD_NFC ) {
        lv_icon_create( Main_Panel, &lv_Contactless_Icon, LV_ALIGN_CENTER, 50, 10 );
        index ++;
    }

    if( cardsSupported & CARD_IC ) {
        lv_icon_create( Main_Panel, &lv_Chip_Icon, LV_ALIGN_CENTER, -10, 10 );
        index ++;
    }

    if( cardsSupported & CARD_MAG ){
        lv_icon_create( Main_Panel, &lv_Strip_Icon, LV_ALIGN_CENTER, -80, 10 );
        index ++;
    }

	lv_timer_enable( true );
}


static void errorEventCallback( lv_event_t * event ) {
	lv_event_code_t code = lv_event_get_code( event );
	const lv_indev_t * indev = lv_indev_get_act( );
	lv_indev_type_t indev_type = lv_indev_get_type( indev );

	if( indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY ) {
		switch( lv_indev_get_key( indev ) ) {
            case LV_KEY_DOWN:
                DispMenuOptions( );
                break;

            case LV_KEY_ENTER:
                Trans_Payment( );
                break;

            default:
                break;
		}
	}
}

void DispErrorMessage( const char * line1, const char * line2, lv_event_cb_t eventCb ) {
    lv_timer_enable( false );
	lv_obj_clean( Main_Panel );

	lv_obj_t * Icon_btn = lv_imgbtn_create( Main_Panel );
    lv_group_remove_all_objs( s_group_keypad_indev );
    lv_obj_add_event_cb( Icon_btn, eventCb, LV_EVENT_ALL,NULL );
    lv_group_add_obj( s_group_keypad_indev,Icon_btn );

    lv_icon_create( Main_Panel, &lv_Fail_Icon, LV_ALIGN_TOP_MID, 0, 10 );
    lv_text_create( Main_Panel, line1, &mediumMsg_style, LV_ALIGN_CENTER, 0, -15 );
    lv_text_create( Main_Panel, line2, &message_style, LV_ALIGN_CENTER, 0, 35 );
    lv_icon_create( Main_Panel, &lv_Menu_Icon, LV_ALIGN_BOTTOM_LEFT, 0, 0 );

    lv_timer_enable( true );
}

/**
 * Muestra por pantalla el error de tarjeta bloqueada
 */
void DispAppBlock( void ) {
    DispErrorMessage("App Block", "", errorEventCallback );
}

/**
 * Muestra por pantalla el error de tarjeta rechazada
 */
void DispRejectedCard( void ) {
    DispErrorMessage( "Trans Reject", "", errorEventCallback );
}

void DispCardNotSupported( void ) {
    DispErrorMessage( "Card Not Support", "", errorEventCallback );
}

static void seePhoneCallback( lv_event_t* event ) {
	lv_event_code_t code = lv_event_get_code( event );
	const lv_indev_t * indev = lv_indev_get_act( );
	lv_indev_type_t indev_type = lv_indev_get_type( indev );

    if( indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY ) {
        switch( lv_indev_get_key( indev ) ) {
			case LV_KEY_ESC:
				TransKbdEvent( EVENT_KEY_CANCEL );
				break;
            
            case LV_KEY_ENTER:
                TransKbdEvent(EVENT_KEY_CONFIRM );
                break;

            default:
                break;
        }
	}
}


void DispSeePhone( void ) {
    lv_timer_enable( false );
	lv_obj_clean( Main_Panel );    
	
	lv_obj_t * Icon_btn = lv_imgbtn_create( Main_Panel);    
    lv_group_remove_all_objs( s_group_keypad_indev );
    lv_obj_add_event_cb(Icon_btn, seePhoneCallback, LV_EVENT_ALL,NULL );
    lv_group_add_obj( s_group_keypad_indev,Icon_btn );
    
    lv_text_create( Main_Panel, "Payment", &title_style, LV_ALIGN_TOP_MID, 0, 5 );
    lv_text_create( Main_Panel, "Please see phone", &message_style, LV_ALIGN_CENTER, 0, 0 ); 
 
    lv_timer_enable( true );
}

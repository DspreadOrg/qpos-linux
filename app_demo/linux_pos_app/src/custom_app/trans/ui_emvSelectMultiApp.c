#include "ui_emvSelectMultiApp.h"

struct App {
    int counter;
    char listIndex[ 5 ][ 3 ];
    char listName[ 5 ][ 20 ];
};
static struct App LocalMultiApp;
static pu8 LocalInputTlv = NULL;

static int calcPosition( u32 indexSelected ) {

    if( !isdigit( indexSelected ) ) {
        return ERROR_CODE;
    } 
    int result = indexSelected - '1';
    return result;
}

static void UpdateKernelSelected( u8 selected ) {
    u8 pNode[ 32 ] = { 0 };
    tlv_new_( pNode, POS_TAG_HEAD, 0, NULL);

    tlv_add_child_( pNode, KERNEL_TAG_AID_SELECT_RESULT_INDEX, 1, &selected );
    tlv_replace_( LocalInputTlv, POS_TAG_RES_KN_APP_DATA, tlv_get_node_l_( pNode ), pNode);
}

static void selectProcess( int indexSelected ) {

    if( indexSelected < 0 ) {
        get_transaction_data()->emv_multi_app_select_result = -1;
    }
    else {
        get_transaction_data()->emv_multi_app_select_result = indexSelected;
    }
} 

static int unpackSelected(const char * text ) {
    if( !text ) {
        return ERROR_CODE;
    }

    if(!strlen( text ) ) {
        return ERROR_CODE;       
    }
    
    return calcPosition( text[ 0 ] );
}


static void checkTextSelected( const char * text ) {
    selectProcess( unpackSelected( text ) );
}

static int checkKey( u32 keyPressed ) {
    int result = calcPosition( keyPressed );

    if( result < 0 || result >= LocalMultiApp.counter ) {
        return ERROR_CODE;
    }
    
    return result;
}


static void multiAppSelectCb( lv_event_t * event ) {	  
    lv_event_code_t code = lv_event_get_code( event );
    const lv_indev_t * indev = lv_indev_get_act( );
    lv_indev_type_t indev_type = lv_indev_get_type( indev );
	
    lv_obj_t * obj = lv_event_get_target( event );
    lv_obj_t * base = lv_event_get_user_data( event );
    const char * text = lv_list_get_btn_text( base, obj );

    lv_obj_get_child_cnt(base);

    if( indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY ) {
        u32 keyPressed = lv_indev_get_key( indev );

        switch( keyPressed ) {
            case LV_KEY_ESC:
                TransKbdEvent( EVENT_KEY_CANCEL );
                break;

            case LV_KEY_UP:
                lv_group_focus_prev( s_group_keypad_indev );
                break;
            
            case LV_KEY_DOWN:
                lv_group_focus_next( s_group_keypad_indev );
                break;

            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
                selectProcess( checkKey( keyPressed ) );
                break;
            
            case LV_KEY_ENTER:
                checkTextSelected( text );
                break;

            default:
                break;
        }
    }
    else if( indev_type == LV_INDEV_TYPE_POINTER && code == LV_EVENT_CLICKED ) {
        checkTextSelected( text );
    }	
}


void DispMultiAppList( void ) {
    lv_timer_enable( false );
    lv_obj_clean( Main_Panel );
        
    lv_text_create(Main_Panel, "Please select App", &title_style, LV_ALIGN_TOP_MID, 0, -10);

    lv_obj_t * main_list = lv_obj_create( Main_Panel );
    lv_obj_set_size( main_list, 310, 155);
    lv_obj_add_style( main_list, &gStyleNotBorder, 0 );
    lv_obj_align( main_list, LV_ALIGN_TOP_MID, 0, 20 );
    lv_obj_set_flex_flow( main_list, LV_FLEX_FLOW_COLUMN );

    lv_obj_t * btn_list;
    lv_group_remove_all_objs( s_group_keypad_indev );

    char itemNum[ 30 ] = { 0 };
    
    for( int index = 0; index < LocalMultiApp.counter; index ++ ) {

        sprintf( itemNum, "%d. %s", index + 1, LocalMultiApp.listName[ index ] );
        btn_list = lv_list_btn(main_list, itemNum,NULL,NULL);
        lv_obj_add_style(btn_list,&btn_style, LV_STATE_DEFAULT);

        lv_obj_add_event_cb(btn_list, multiAppSelectCb, LV_EVENT_ALL, (pvoid)main_list);
        lv_group_add_obj(s_group_keypad_indev,btn_list);
    }

    lv_timer_enable( true );
}

void unpackAppsName( int counter, AidCandidate_t *pList ) 
{
    int i;
    memset(&LocalMultiApp, 0x00, sizeof( LocalMultiApp ) );
    LocalMultiApp.counter = counter;
    for(i =0;i<counter;i++)
    {
        if(i == 5)
            break;
        memcpy(LocalMultiApp.listName[i],pList[i]._lable,pList[i]._lable_len);
        sprintf(LocalMultiApp.listIndex[i],"%d",i+1);
    }
}



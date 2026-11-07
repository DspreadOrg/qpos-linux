
#include "appinc.h"
/*----------------------------------------------------------------------------
|   Function Name:
|       MenuOptions_cb
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
static void Qrcode_cb(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_indev_t *indev = lv_indev_get_act();
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    uint32_t key = EVENT_KEY_NONE;

    if (indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_KEY) {
        key = lv_indev_get_key(indev);
    }
    else if (indev_type == LV_INDEV_TYPE_POINTER && code == LV_EVENT_CLICKED) {
        /* 触摸点击：用容器的 user_data 或 obj 索引反查按键号 */
        key = lv_get_btn_key(event);          /* 你自己的实现 */
    }
    else if (indev_type == LV_INDEV_TYPE_KEYPAD && code == LV_EVENT_CLICKED) {
        key = LV_KEY_ENTER;                   /* 焦点按回车 */
    }
	OsLog(LOG_DEBUG,"key = %d",key);
    switch(key)
    {	        
        case LV_KEY_DOWN:
        case LV_KEY_ESC:
            DispMenuOptions();
            break;		   
        case LV_KEY_ENTER:
            Qr_Payment();
            break;
        default:				
            break;
    }    
}
void ui_create_qrcode() {
    char szDisplayAmount[16] = {0};
    char szDisplayQrUrl[256] = {0};
    char *paymentAppList[4] = {"Google.png","Apple.png","MasterCard.png","Visa.png"};

    lv_timer_enable(false);
    lv_obj_clean(Main_Panel);
    lv_group_remove_all_objs(s_group_keypad_indev);
    lv_obj_clear_flag(Main_Panel, LV_OBJ_FLAG_SCROLLABLE); // Disable scrolling

    sprintf(szDisplayQrUrl,"this is a demo from Dspread, and the terminal will display \"Transaction Success\" after press enter.\n amount: $100.00");
    lv_obj_t * ui_qrcode_pad = lv_qrcode_create(Main_Panel, 150, lv_color_hex(0x000000), lv_color_hex(0xFFFFFF));
    lv_obj_set_style_border_width(ui_qrcode_pad, 0, 0);
    lv_qrcode_update(ui_qrcode_pad, szDisplayQrUrl, strlen(szDisplayQrUrl));
    lv_obj_align(ui_qrcode_pad, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_t * tip_lable = lv_label_create(Main_Panel);
    lv_label_set_text(tip_lable, "Open payment App");
    lv_obj_align(tip_lable, LV_ALIGN_TOP_MID, 0, 580);

    lv_obj_t *payment_pad = lv_obj_create(Main_Panel);
    lv_obj_set_size(payment_pad, 320, 24);
    lv_obj_align(payment_pad, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_border_width(payment_pad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(payment_pad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(payment_pad, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(payment_pad,LV_FLEX_ALIGN_SPACE_EVENLY, 0, 0);

    for (size_t i = 0; i < sizeof(paymentAppList)/sizeof(char*); i++)
    {
        lv_obj_t * payment_img = lv_img_create(payment_pad);
        ui_lv_img_set_src(payment_img, paymentAppList[i]);
    }

    lv_obj_add_event_cb(payment_pad, Qrcode_cb, LV_EVENT_KEY,     NULL);
    lv_group_add_obj(s_group_keypad_indev, payment_pad);
    lv_timer_enable(true);
}

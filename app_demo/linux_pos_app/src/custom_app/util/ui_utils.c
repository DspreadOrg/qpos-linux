#include "ui_styles.h"
#include "app_global.h"

/*----------------------------------------------------------------------------
|   Function Name:
|       lv_button_create
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
lv_obj_t * lv_button_create(lv_obj_t *btn_list, lv_coord_t w, lv_coord_t h, char *header, const char *text, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs, bool enabled)
{
    style_theme_button = false;
    lv_obj_t *btn = lv_btn_create(btn_list);
	lv_obj_set_size(btn, w, h);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);  
    lv_obj_add_style(btn, &btn_style, LV_STATE_DEFAULT);

    if( memcmp(header, OK_ICON, strlen(OK_ICON)) == 0 ){
        lv_obj_t * imgSmallOKIcon = lv_img_create(btn);
	    lv_img_set_src(imgSmallOKIcon, &lv_Small_OK_Icon);	
        lv_obj_align(imgSmallOKIcon, LV_ALIGN_TOP_LEFT, -14, -8);    
    }
    else{
        lv_obj_t *btn_header = lv_label_create(btn);
        lv_label_set_text(btn_header, header);	
        lv_obj_align(btn_header, LV_ALIGN_OUT_LEFT_TOP, -12, -10);
        lv_obj_add_style(btn_header, &btn_header_style, LV_STATE_DEFAULT);
    }

    lv_obj_t * btn_test = lv_label_create(btn);
    lv_label_set_text(btn_test, text);
    if(enabled)
        lv_obj_add_style(btn_test, &message_style, LV_STATE_DEFAULT);
    else
        lv_obj_add_style(btn_test, &message_disabled_style, LV_STATE_DEFAULT);
    lv_obj_align(btn_test, align, x_ofs, y_ofs);

    return btn;
}

/*----------------------------------------------------------------------------
|   Function Name:
|       lv_btn_list_create
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
lv_obj_t * lv_btn_list_create(lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t * btn_list = lv_obj_create(Main_Panel);
    lv_obj_set_size(btn_list, w, h);
    lv_obj_add_style(btn_list, &btn_list_style, 0);
    lv_obj_align(btn_list, align, x_ofs, y_ofs);
    lv_obj_set_flex_flow(btn_list, LV_FLEX_FLOW_ROW_WRAP);

    return btn_list;
}

/*----------------------------------------------------------------------------
|   Function Name:
|       lv_add_btn
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
lv_obj_t * lv_add_btn(lv_obj_t *btn_list, lv_event_cb_t event_cb, lv_coord_t w, lv_coord_t h, char *header, const char *text, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs, bool enabled)
{
    lv_obj_t * btn;
    btn = lv_button_create(btn_list, w, h, header, text, align, x_ofs, y_ofs, enabled);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_ALL, (pvoid)btn_list);
    lv_group_add_obj(s_group_keypad_indev, btn);

    return btn;
}

/*----------------------------------------------------------------------------
|   Function Name:
|       lv_text_create
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
lv_obj_t * lv_text_create(lv_obj_t *parent, const char *text, lv_style_t *style, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	lv_obj_t * label = lv_label_create(parent);
	lv_label_set_text(label, text);
	lv_obj_add_style(label, style, LV_STATE_DEFAULT);
	lv_obj_align(label, align, x_ofs, y_ofs);

	return label;
}

/*----------------------------------------------------------------------------
|   Function Name:
|       lv_icon_create
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
lv_obj_t * lv_icon_create(lv_obj_t *parent, const void * src, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t * img = lv_img_create( parent );
    lv_img_set_src( img, src );	
    lv_obj_align( img, align, x_ofs, y_ofs );

    return img;
}

/*----------------------------------------------------------------------------
|   Function Name:
|       lv_get_btn_key
|   Description: 
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
uint32_t lv_get_btn_key(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * base=lv_event_get_user_data(e);
    int key = lv_list_get_btn_id(base, obj);
    
    if(key >= 0)
        key = key + '0' + 1;
    else
        key = EVENT_KEY_NONE;
    
    return key;
}    

/**
 * @file lv_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_list.h"
#include "../../../core/lv_disp.h"
#include "../../../widgets/lv_label.h"
#include "../../../widgets/lv_img.h"
#include "../../../widgets/lv_btn.h"
#include "../../themes/default/lv_theme_default.h"

#if LV_USE_LIST

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

const lv_obj_class_t lv_list_class = { .base_class = &lv_obj_class, .width_def =
		(LV_DPI_DEF * 3) / 2, .height_def = LV_DPI_DEF * 2 };

const lv_obj_class_t lv_list_btn_class = { .base_class = &lv_btn_class, };

const lv_obj_class_t lv_list_text_class = { .base_class = &lv_label_class, };

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t* lv_list_create(lv_obj_t *parent) {
	LV_LOG_INFO("begin")
	lv_obj_t *obj = lv_obj_class_create_obj(&lv_list_class, parent);
	lv_obj_class_init_obj(obj);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
	return obj;
}

lv_obj_t* lv_list_add_text(lv_obj_t *list, const char *txt) {
	LV_LOG_INFO("begin")
	lv_obj_t *obj = lv_obj_class_create_obj(&lv_list_text_class, list);
	lv_obj_class_init_obj(obj);
	lv_label_set_text(obj, txt);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(obj, LV_PCT(100));
	return obj;
}

lv_obj_t* lv_list_add_btn(lv_obj_t *list, const char *icon, const char *txt) {
	

	LV_LOG_INFO("begin")
	lv_obj_t *obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
	lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, 290, 48);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

	if (icon) {
		lv_obj_t *img = lv_img_create(obj);
		lv_img_set_src(img, icon);
	}

	if (txt) {
		lv_obj_t *label = lv_label_create(obj);
		lv_obj_set_size(label, 290, 38);
		lv_label_set_text(label, txt);
		lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_flex_grow(label, 1);
	}

	return obj;
}

lv_obj_t* lv_list_btn(lv_obj_t * main_list, char* wifi_disp,char* email,const char *icon){

    lv_obj_t * obj;
	lv_obj_t * label;
	style_theme_button = true;
    obj = lv_btn_create(main_list);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
	lv_obj_align(obj,LV_ALIGN_CENTER, 0,0);
    label = lv_label_create(obj);
	if(email == NULL)
		lv_label_set_text(label,wifi_disp);
	else
    	lv_label_set_text_fmt(label,"%s\n%s",wifi_disp,email);

    lv_obj_set_size(label, LV_PCT(100), LV_SIZE_CONTENT);
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);  
	lv_obj_set_flex_grow(label, 1);

	if (icon) {
		lv_obj_t *img = lv_img_create(obj);
		lv_img_set_src(img, icon);
	}
            
	return obj;
}

const char* lv_list_get_btn_text(lv_obj_t *list, lv_obj_t *btn) {
	LV_UNUSED(list);
	uint32_t i;
	for (i = 0; i < lv_obj_get_child_cnt(btn); i++) {
		lv_obj_t *child = lv_obj_get_child(btn, i);
		if (lv_obj_check_type(child, &lv_label_class)) {
			return lv_label_get_text(child);
		}

	}

	return "";
}

const int lv_list_get_btn_id(lv_obj_t *list, lv_obj_t *btn) {
	LV_UNUSED(list);
	
	int id = lv_obj_get_child_id(btn);
	if (id ==0xFFFFFFFF)
		return -1;
	else 
		return id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LIST*/

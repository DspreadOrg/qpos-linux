#ifndef _UI_UTILS_H
#define _UI_UTILS_H

extern lv_obj_t * lv_button_create(lv_obj_t *btn_list, lv_coord_t w, lv_coord_t h, char *header, const char *text, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs, bool enabled);
extern lv_obj_t * lv_btn_list_create(lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);
extern lv_obj_t * lv_add_btn(lv_obj_t *btn_list, lv_event_cb_t event_cb, lv_coord_t w, lv_coord_t h, char *header, const char *text, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs, bool enabled);
extern lv_obj_t * lv_text_create(lv_obj_t *parent, const char *text, lv_style_t *style, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);
extern lv_obj_t * lv_icon_create(lv_obj_t *parent, const void * src, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);
extern uint32_t lv_get_btn_key(lv_event_t * e);

#endif

#ifndef _LVGL_PORTING_H
#define _LVGL_PORTING_H
#ifdef __cplusplus
extern "C"
{
#endif
/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#define CFG_LVGL_HOR_RES                         (320)
#define CFG_LVGL_VER_RES                         (240)
#define CFG_LVGL_FLASH_MEM_DIV                   (3)
#define CFG_LVGL_MS_PER_TICK                     (5)
//#define CFG_TP_FILTER

void *tick_thread(void *args);

void appStart();

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void);

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void);

void GuiEventRegister(u32 msg_id);
lv_group_t * Get_keypad_Handle();
#ifdef __cplusplus
}
#endif

#endif
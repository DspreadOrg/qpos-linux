#include "app_includes.h"

#include <linux/input.h>
#include <sys/ipc.h>
#include <sys/msg.h>

static lv_disp_draw_buf_t s_lv_disp_draw_buf;
static lv_group_t * group_keypad_indev = NULL;
static uint32_t     s_KeyLast = 0;

int evdev_fd = -1;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

int evdev_key_val;

static lv_color_t s_lv_color[CFG_LVGL_HOR_RES*CFG_LVGL_VER_RES/CFG_LVGL_FLASH_MEM_DIV];


typedef struct key_struct {
    int touch_x;    //Touch point x
    int touch_y;    //Touch point y
    int down;   //Touch status 1: press 0: release-1: move
    int valid;  //Determine whether the data is valid 1: valid 0: invalid
}touch_key, *p_touch_key;

static touch_key last_touch_key = { 0, 0, -1, 0 };
touch_key g_touch_key = { 0, 0, -1, 0 };

#if LV_USE_LOG
/*--------------------------------------
|	Function Name:
|		LvglPortingCallbackLog
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
static void LvglPortingCallbackLog(const char *buf)
{
	//OsLog(LOG_DEBUG, "%s\r\n", buf);

}
#endif

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}


/*--------------------------------------
|	Function Name:
|		LvglPortingCallbackDrawBuf
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
static void LvglPortingCallbackDrawBuf(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    PR_UINT32 uiAreaW = 0;
    PR_UINT32 uiAreaH = 0;
    POINT_T tStartPoint;

    lv_coord_t hres = disp_drv->rotated == 0 ? disp_drv->hor_res : disp_drv->ver_res;
    lv_coord_t vres = disp_drv->rotated == 0 ? disp_drv->ver_res : disp_drv->hor_res;

    if ((area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) || disp_flush_enabled == false)
    {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    tStartPoint.X = area->x1;
    tStartPoint.Y = area->y1;
    uiAreaW = lv_area_get_width(area);
    uiAreaH = lv_area_get_height(area);
    //lark_lcd_write(LCD_MODE_COLOR, area->x1, area->y1, uiAreaW, uiAreaH, (puint)color_p);
    Disp_vShowImage((unsigned  char *)color_p,0, tStartPoint, uiAreaW, uiAreaH);
    lv_disp_flush_ready(disp_drv);
}
typedef enum{
    LV_INDEV_KEYBOARD = 1,
    LV_INDEV_TOUCHPAD = 3,
}LV_INDEV_TYPE;
uint32_t keypad_get_key(int iTimeOut)
{
    int nRet = -1;
    struct input_event event;
    
    nRet = Disp_GetKeyLinux(&event, iTimeOut);
    if (nRet == 0 && event.type == LV_INDEV_KEYBOARD && event.value == 1)
    {
        if (event.code != 330)
        {
            // OsLog(LOG_DEBUG,"Disp_GetKeyAndTouchpadX = %d",nRet);
            // OsLog(LOG_DEBUG,"event.code = %d",event.code);
            // OsLog(LOG_DEBUG,"event.type = %d",event.type);
            // OsLog(LOG_DEBUG,"event.value = %d",event.value);
            return event.code;
        }
        else
        {
            nRet = -1;
        }
    }

    return nRet;
}
/*--------------------------------------
|	Function Name:
|		LvglPortingCallbackKeypad
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
static void LvglPortingCallbackKeypad(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	int keyValue = EM_KEY_INVALID;
    if(get_Lvgl_Kbd_Flag() || disp_flush_enabled == false)
    {
        return;
    }
#if 1
    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key(10);
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
        switch(act_key)
        {
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x30:
                s_KeyLast = act_key;
                break;
            case 0x01: //cancle
                s_KeyLast = LV_KEY_ESC;                
                break;
            case 0x0e: //backspace
                s_KeyLast = LV_KEY_BACKSPACE;
                break;    
            case 0x019c: //up
                s_KeyLast = LV_KEY_UP;
                break;    
            case 0x0197: //down
                s_KeyLast = LV_KEY_DOWN;
                break;
            case 0x1c:  //confirm
                s_KeyLast = LV_KEY_ENTER;
                break;  
            default:
                data->state = LV_INDEV_STATE_REL;
                break;          
        }
       // OsLog(LOG_DEBUG,"Dspread: s_KeyLast = %d",s_KeyLast);
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = s_KeyLast;
#else
	data->state = LV_INDEV_STATE_PR;
	keyValue = Disp_GetKey(10);
    if(keyValue != EM_KEY_INVALID)
        OsLog(LOG_DEBUG,"---------key value = %d",keyValue);	
	switch(keyValue)
	{       
        case EM_KEY_1:
        case EM_KEY_2:
        case EM_KEY_3:
        case EM_KEY_4:
        case EM_KEY_5:
        case EM_KEY_6:
        case EM_KEY_7:
        case EM_KEY_8:
        case EM_KEY_9:
            s_KeyLast = '0' + (keyValue+1);
            break;
        case EM_KEY_0:
            s_KeyLast = '0';
            break;

	    case EM_KEY_CLEAR:
            s_KeyLast = LV_KEY_BACKSPACE;
		    break;
	    case EM_KEY_ENTER:
    		s_KeyLast = LV_KEY_ENTER;
		    break;
	    case EM_KEY_UP:
    		s_KeyLast = LV_KEY_UP;
		break;
	    case EM_KEY_DOWN:
            s_KeyLast = LV_KEY_DOWN;
		    break;
	    case EM_KEY_POWER:
            s_KeyLast = LV_KEY_HOME;
		    break;
	    case EM_KEY_CANCEL:
    		s_KeyLast = LV_KEY_ESC;
		    break;
	    case EM_KEY_MAX:
            break;
        case EM_KEY_F1:
        case EM_KEY_F2:
        case EM_KEY_F3:
        case EM_KEY_F4:
            break;
        default:
            data->state = LV_INDEV_STATE_REL;
            break;
	}

    data->key = s_KeyLast;
    #endif
}


int readTouchKey(p_touch_key ptouch_key)
{
    struct input_event event;

    while (1)
    {
        /*Read touch data circularly*/
        if (Disp_GetTouchPadLinux(&event, 1)!=0) {            
            //OsLog(LOG_ERROR, "==============Disp_GetTouchPadLinux Error=========================");
            return -1;
        }

        #if defined(CFG_DEBUG_TOUCHKEY)
        OsLog(LOG_ERROR, "Disp_GetTouchPadLinux type = %08x, code = %08x, value = %08x", event.type, event.code, event.value);
        // OsLog(LOG_ERROR, "EV_KEY = %d, EV_ABS =%d, EV_SYN = %d", EV_KEY, EV_ABS, EV_SYN);
        // OsLog(LOG_ERROR, "BTN_TOUCH = %d, ABS_X =%d, ABS_Y = %d, SYN_REPORT = %d", BTN_TOUCH, ABS_X, ABS_Y,SYN_REPORT);
        #endif

        switch(event.type) {
            case EV_KEY :   //Key event
                if(event.code = BTN_TOUCH) {
                    ptouch_key->down = event.value;   // Save the value of the key event
                    ptouch_key->valid = 1;
                    switch(ptouch_key->down) {
                        case 1: //Press the button
                            #if defined(CFG_DEBUG_TOUCHKEY)
                            OsLog(LOG_ERROR, "Press touch key %d, %d\n", ptouch_key->touch_x, ptouch_key->touch_y);
                            #endif
                            break;
                            
                        case 0://Release the button
                            #if defined(CFG_DEBUG_TOUCHKEY)
                            OsLog(LOG_ERROR, "Release touch key %d, %d\n", ptouch_key->touch_x, ptouch_key->touch_y);                              
                            #endif
                            break;
                        case -1:
                            #if defined(CFG_DEBUG_TOUCHKEY)
                            OsLog(LOG_ERROR,"Move touchkey %d, %d\n",ptouch_key->touch_x,ptouch_key->touch_y);
                            #endif
                            break;
                    }

                }
                break;
 
            case EV_ABS : //Absolute displacement event
                switch (event.code)
                {
                    case ABS_X: //Absolute displacement X coordinate
                    case 0x35:
                        ptouch_key->touch_x = event.value;    //Save X coordinat
                        break;
                    case ABS_Y: //Absolute displacement Y coordinate
                    case 0x36:
                        ptouch_key->touch_y = event.value;    //Save Y coordinat
                        break;
                }
                break;            
        }
    }

    return 0;
}


/*--------------------------------------
|	Function Name:
|		LvglPortingCallbackReadTp
|	Description:

+-------------------------------------*/
static void LvglPortingCallbackReadTp(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    if(disp_flush_enabled == false)
        return ;
    
    readTouchKey(&g_touch_key);

    #if defined(CFG_DEBUG_TOUCHKEY)
    OsLog(LOG_ERROR, " g_touch_key.down =%d, g_touch_key.touch_x = %08x, g_touch_key.touch_y = %08x", g_touch_key.down, g_touch_key.touch_x,g_touch_key.touch_y);
    #endif

    if (g_touch_key.valid == 0 || g_touch_key.touch_x > CFG_LVGL_HOR_RES || g_touch_key.touch_y > CFG_LVGL_VER_RES)
    {
        return;
    }

    {
        switch (g_touch_key.down)
        {
        case 1:
            data->point.x = g_touch_key.touch_x;
            data->point.y = g_touch_key.touch_y;
            data->state = LV_INDEV_STATE_PR;

            #if defined(CFG_DEBUG_TOUCHKEY)
            OsLog(LOG_ERROR, "Dspread: touch key pressed %d, %d\n", data->point.x, data->point.y);
            #endif
            break;
        case 0:
            data->point.x = g_touch_key.touch_x;
            data->point.y = g_touch_key.touch_y;
            data->state = LV_INDEV_STATE_REL;
            #if defined(CFG_DEBUG_TOUCHKEY)
            OsLog(LOG_ERROR, "Dspread: CFG_DEBUG_TOUCHKEYtouch key released %d, %d\n", data->point.x, data->point.y);
            #endif
            g_touch_key.down = -1;
            g_touch_key.valid = 0;
            g_touch_key.touch_x = 0;
            g_touch_key.touch_y = 0;
            break;
        default:
            break;
        }
    }
    
    return;
}


/*--------------------------------------
|	Function Name:
|		lvgl_init
|	Description:lvgl init
|	Parameters:
|	Returns:
+-------------------------------------*/
void lvgl_init()
{
    #if LV_USE_LOG
	lv_log_register_print_cb(LvglPortingCallbackLog);
    #endif
    lv_init();

    /*lvgl display driver register*/

	lv_disp_draw_buf_init(&s_lv_disp_draw_buf, s_lv_color, NULL, CFG_LVGL_HOR_RES*CFG_LVGL_VER_RES/CFG_LVGL_FLASH_MEM_DIV);

	{
		static lv_disp_drv_t disp_drv;

		lv_disp_drv_init(&disp_drv);
		disp_drv.hor_res = CFG_LVGL_HOR_RES;
		disp_drv.ver_res = CFG_LVGL_VER_RES;
		disp_drv.flush_cb = LvglPortingCallbackDrawBuf;
		disp_drv.draw_buf = &s_lv_disp_draw_buf;
		lv_disp_drv_register(&disp_drv);
	}

#if 1
        /*lvgl indev driver register*/
	{
		static lv_indev_drv_t indev_drv;

        lv_timer_enable(false);
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		indev_drv.read_cb = LvglPortingCallbackReadTp;
		lv_indev_drv_register(&indev_drv);
		lv_timer_enable(true);
	}
#endif
#if 1
	{
        static lv_indev_drv_t indev_drv;
        lv_indev_t *   indev;

        lv_timer_enable(false);

        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_KEYPAD;
        indev_drv.read_cb = LvglPortingCallbackKeypad;
        indev=lv_indev_drv_register(&indev_drv);

        group_keypad_indev = lv_group_create();
	    lv_indev_set_group(indev, group_keypad_indev);
        lv_timer_enable(true);
	}
    #endif
}
lv_group_t * Get_keypad_Handle()
{
    return group_keypad_indev;
}

/*--------------------------------------
|	Function Name:
|		Gui_tick
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
void * tick_thread(void *args)
{
    while(1)
    {
        lv_tick_inc(CFG_LVGL_MS_PER_TICK);
        OsSleep(CFG_LVGL_MS_PER_TICK);
    }    
}

static bool gui_thread_flag = false;
void gui_thread_mutex(Switch_t status)
{
    if(status == MUTEX_NO)
    {
        gui_thread_flag = true;
    }
    else if(status == MUTEX_OFF)
    {
        gui_thread_flag = false;
    }
}

bool gui_thread_mutex_status()
{
    return gui_thread_flag;
}

static int Lvgl_Queue_Id = 0;
void GuiEventRegister(u32 msg_id)
{
    DSP_Info("msg_id = %d",msg_id);
    queue_message_body_t queMsg;
    queMsg.msgType = 1;
    queMsg.msg.msg_id = msg_id;
	if (msgsnd(Lvgl_Queue_Id, &queMsg, sizeof(queue_message_body_t)-sizeof(long), 0) == -1) {
		DSP_Debug();
    }
    OsSleep(5);
}

int lvgl_Queue_Init()
{
	int ret = 0;

	key_t key;
	char *file = "/tmp/lvgl_msg";
	FILE *fp = fopen(file, "w");
	if (fp != NULL)
	{
        fclose(fp);
	}

    // 创建一个唯一的 key
    key = ftok("/tmp/lvgl_msg", 'm');
    if (key == -1) {
        DSP_Debug();
		goto EXIT;
    }

	    // 创建消息队列
    Lvgl_Queue_Id = msgget(key, IPC_CREAT | 0666);
    if (Lvgl_Queue_Id == -1) {
		goto EXIT;
    }
    ret = PR_NORMAL;
 EXIT:
    return ret;      
}

void devInit()
{
    GuiDisplay(LCD_DISP_DEVICE_INIT);
    lv_task_handler();
    Dev_nInit(); 
}

void appStart()
{
    if(lvgl_Queue_Init() != PR_NORMAL)
    {
        DSP_Debug();
        exit(1);
    }

    lvgl_init();
    devInit();
    GuiDisplay(LCD_DISP_WELCOME);

    queue_message_body_t queMsg;
    int ret = 0;
    while(1)
    {
        memset(&queMsg,0,sizeof(queue_message_body_t));
        ret = -1;
		ret = msgrcv(Lvgl_Queue_Id, &queMsg, sizeof(queue_message_body_t)-sizeof(long), 1, IPC_NOWAIT);
		if (ret != -1)
		{
			GuiDisplay(queMsg.msg.msg_id);
		}
        else
        {
            lv_task_handler();
        }
        OsSleep(CFG_LVGL_MS_PER_TICK);
    }
}

void mem_status()
{
    lv_mem_monitor_t   mon;
    lv_mem_monitor(&mon);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem total_size:        %d\r\n", mon.total_size);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem free_cnt:          %d\r\n", mon.free_cnt);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem free_size:         %d\r\n", mon.free_size);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem free_biggest_size: %d\r\n", mon.free_biggest_size);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem used_cnt:          %d\r\n", mon.used_cnt);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem max_used:          %d\r\n", mon.max_used);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem used_pct:          %d\r\n", mon.used_pct);
	OsLog(LOG_DEBUG, "Dspread: lvgl mem frag_pct:          %d\r\n", mon.frag_pct);
}
#ifndef _TMS_CFG_H
#define _TMS_CFG_H

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef enum
{
    OTA_SYNC_SERVER=0,
    OTA_START_DOWNLOAD,
    OTA_PROCESSING,
    OTA_UPDATE_FINISH,
    OTA_OTHER,
    OTA_FIRMWARE_VERIFY,
    OTA_FIRMWARE_VERIFY_FAIL,
    OTA_SYNC_ERROR_MSG,
    OTA_SYNC_PARAM_DATA,
    OTA_NO_AVAILABLE_FIRMWARE,
	OTA_USB_CONNECT
}OTA_STEP;

extern void set_user_appver(char * ver);
extern Rc_t tms_client_check(const char *szUrl);
extern Rc_t tms_client(const char *szUrl, bool isForce);
extern void Tms_Disp_Callback_Register(void (*Tms_disp)(u32, u32, u32, void*));
#endif

#include "task_handle.h"
/*----------------------------------------------------------------------------
|   Function Name:
|       EventHandle
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void EventHandle(u32 Event_id)
{
    switch(Event_id)
    {
        case EVENT_PAYMENT:
            Start_Payment();
            break;

        case EVENT_WIFI_SEARCH:
            Wifi_Search();
            break;

        case EVENT_WIFI_CONNECT:
            Wifi_Connect();
            break;

		case EVENT_OTA_DOWNLOAD:
            Firmware_OTA();
            break;

		case EVENT_OTA_CHECK:
            Firmware_OTA_Check();
            break;

        default:
            break;
    }
}
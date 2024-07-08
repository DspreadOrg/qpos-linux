#include "task_handle.h"
#include "app_trans.h"
#include "app_ota.h"
/*----------------------------------------------------------------------------
|   Function Name:
|       StartTradingOnlineHandler
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
void StartTradingOnlineHandler(pu8 pTradingFile)
{
    TransactionProcess(pTradingFile);
}

/*----------------------------------------------------------------------------
|   Function Name:
|       EventHandle
|   Description:
|   Parameters:
|   Returns:
+---------------------------------------------------------------------------*/
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
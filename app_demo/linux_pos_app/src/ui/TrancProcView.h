/*
 * TrancProcView.h
 * 
 * Created on: Oct 20, 2014
 * Author: rsj
 * Note: Transaction process related interface interface
 */

#ifndef _TranProcView_H_
#define _TranProcView_H_

#include "appinc.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define 	VIEW_TIMEOUT_FOREVER	-1


// Business menu UI public entrance function
// Message Box Definition:
typedef enum tagMessageBoxID
{
	ID_OK     =  0,
	ID_CANCEL =  1,
	ID_TIMEOUT = -1,
}MessageBoxID_T;

typedef enum tagMessageBoxStyle
{
	BOX_STYLE_OK     = EM_STYLE_OK,
	BOX_STYLE_CANCEL = EM_STYLE_CANCEL,
}MessageBoxStyle_T;

typedef enum tagCurrencyIndicate
{
	INDICATE_FOR_RMB,		// Displayed as RMB: Representing the Renminbi
	INDICATE_FOR_DOLLAR,    // Displayed as USD: Representing the US dollar
	INDICATE_FOR_HKD,		// Displayed as: HKD:  Representing Hong Kong dollars
	INDICATE_FOR_MOPD,      // Displayed as: MOPD stands for Macau Patacas
	INDICATE_FOR_MAX_NUMS,
}CurrencyIndicate_T;

#define EMV_OFFLINEPIN_INPUT    1   /*Offline password*/
#define EMV_OFFLINE_ONLY_INPUT  2   /*Last offline password*/
#define EMV_ONLINEPIN_INPUT   3   /*Online password*/


KB_KEYMAP_T KB_nWaitKeyMS(Int32 MS);

KB_KEYMAP_T KB_nWaitKeyMS_Ex(Int32 MS);

// Clear key values
void KB_vReleasKey();

PR_INT32 	TransView_nDoMessageBox(PR_INT8 *szTitle,
									PR_UINT32 theStype,
									PR_INT32 nTimeoutSec,
									PR_INT8 *args,...);
/*
 * @author:
 * @Date: 10-25-2014
 * @Record: create it;
 * 
 * @Function Name: TransView_vShowLine
 * @Function function: Display string data on specified lines
 * @Input parameters: LINE - The line to be displayed, the line number where it is located
 * TYPE - Display method EM_DTYPE_NORMAL, //Ready display
 * EM_DTYPE_REVERT, //Reverse
 * EM_DTYPE_WITH_UNDERLINE, //Underlined display
 * EM_DTYPE_WITH_TOPLINE, //Under the marking to display
 * EM_DTYPE_WITH_RECT // Add frame
 * 
 * PATTERN - Alignment:
 * EM_ALIGN_LEFT // Left Alignment
 * EM_ALIGN_RIGHT // Right aligned
 * EM_ALIGN_CENTER // Align center
 * ... -Variable parameter table
 * @Output parameters:
 * 
 * @Return value:
 * 
 * @Remark:
 * 
 * @Call Example:
 * /// On line 0, the title content is "Title Display" and set to reverse display center
 * TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,"Title Display");
 * // Or call the same effect as above
 * TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,"%s","Title display");
 */

#define  TransView_vShowLine(LINE, TYPE, PATTERN, ...) \
			 Disp_vShowStr((LINE), (TYPE), (PATTERN), ##__VA_ARGS__)

/*
 * @author:
 * @Date: 10-25-2014
 * @Record: create it;
 * 
 * @Function Name: TransView_vShowLineEx
 * @Function function: Display string data on the specified line (can automatically wrap)
 * @Input parameters: LINE - The line to be displayed, the line number where it is located
 * PATTERN - Alignment:
 * EM_ALIGN_LEFT // Left Alignment
 * EM_ALIGN_RIGHT // Right aligned
 * EM_ALIGN_CENTER // Align center
 * ... -Variable parameter table
 * @Output parameters:
 * 
 * @Return value:
 * 
 * @Remark:
 * 
 * @Call Example:
 */

#define  TransView_vShowLineEx(LINE, PATTERN, ...) \
			Disp_vShowStrAtEx((LINE), (PATTERN), ##__VA_ARGS__)

#define  TransView_vShowPersianByLine(LINE, PATTERN, ...) \
			Disp_vShowPersianStr((LINE), (PATTERN), ##__VA_ARGS__)
/*
 * @author:
 * @Date: 10-25-2014
 * @Record: create it;
 * 
 * @Function Name: TransView_vClearAllScreen
 * @Function Function: Clear all user display content on the screen.The status bar and input method display bar are not included
 * @Input parameters: None
 * 
 * @Output parameters:
 * 
 * @Return value:
 * 
 * @Remark:
 * 
 * @Call Example:
 */
#define 	TransView_vClearAllScreen() Disp_vClearPort()
#define     TransView_vClearLine(LINE)	Disp_vClearLine(LINE)
/*
 * @author:
 * @Date: 10-25-2014
 * @Record: create it;
 * 
 * @Function Name: TransView_vClearPort
 * @Function function: Clear the screen display content, but retain the Title line data
 * @Input parameters: None
 * 
 * @Output parameters:
 * 
 * @Return value:
 * 
 * @Remark:
 * 
 * @Call Example:
 */

void  TransView_vClearPort(void);

void TransView_vShowLogoView();


PR_INT32 TransView_nShowTableViewEx(PR_INT8* pszTitle,PR_INT8** pszListItem,PR_INT32 nItemCount,PR_INT32 nTimeoutMs, PR_INT32 *pnIndexOfSelect);

PR_INT32 TransView_nShowListViewEx(PR_INT8* pszTitle,PR_INT8** pszListItem,PR_INT32 nItemCount,PR_INT32 nTimeoutMs, PR_INT32 *pnIndexOfSelect);

PR_INT32 TransView_nNumber_En_Symbol_InputViewEx(PR_INT8*  pszTip,
																	PR_INT32  MaxLength,
																	  PR_INT32  MinLength,
																	  PR_INT32  nLenOfOutputBuffer, // 传入Buffer长度
																	  PR_INT8*  pcOutput);

PR_INT32 TransView_nShowAmountInputView(PR_INT32 nLenOfBuffer, PR_INT8* pszAmountoutput);

PR_INT32 TransView_nShowPinpadView(PR_INT32 KeyIdx,PR_INT8* pszAmount,PR_UINT8 *DataIn, PR_INT8 *ExpPinLen, PR_INT32 Mode, PR_UINT64 TimeoutS, PR_UINT8 *pPinBlock);

PR_INT32 TransView_nGetOfflinePin(PR_INT8* szAmount,
										PR_INT32 nOfflinePinType,
										PR_INT32 nAtLine,
										PR_INT32 nMaxOfPin,
										PR_INT32 nMinOfPin,
										PR_INT32 nLenOfOfflinePinBuffer,
										PR_INT8* pcOfflinePinOutput);


PR_INT32 TransView_nInputPwd(PR_INT8*  pszTip,
																	PR_INT32  MaxLength,
																	  PR_INT32  MinLength,
																	  PR_INT32  nLenOfOutputBuffer, // 传入Buffer长度
																	  PR_INT8*  pcOutput);
#ifdef __cplusplus
}
#endif
#endif 

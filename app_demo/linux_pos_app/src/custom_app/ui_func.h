/*
 * ui_func.h
 *
 *  Created on: Oct 20, 2014
 *  Author: rsj
 *  Note:交易过程相关界面接口
 */

#ifndef _TranProcView_H_
#define _TranProcView_H_

#include "appinc.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define 	VIEW_TIMEOUT_FOREVER	-1


//业务菜单UI公共入口函数
// Message Box 定义:
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
	INDICATE_FOR_RMB,		// 显示为  RMB: 代表人民币
	INDICATE_FOR_DOLLAR,    // 显示为  USD: 代表美元
	INDICATE_FOR_HKD,		// 显示为： HKD: 代表港元
	INDICATE_FOR_MOPD,      // 显示为: MOPD 代表澳门币
	INDICATE_NODISP,		//
	INDICATE_FOR_MAX_NUMS,
}CurrencyIndicate_T;

#define EMV_OFFLINEPIN_INPUT    1   /*脱机密码*/
#define EMV_OFFLINE_ONLY_INPUT  2   /*最后一次脱机密码*/
#define EMV_ONLINEPIN_INPUT   3   /*联机密码*/

#define      MAX_SIZE_NAME_WIFI          64
#define      MAX_SIZE_PASSWORD_WIFI      30

KB_KEYMAP_T KB_nWaitKeyMS(Int32 MS);

KB_KEYMAP_T KB_nWaitKeyMS_Ex(Int32 MS);

//清空键值
void KB_vReleasKey();

PR_INT32 	TransView_nDoMessageBox(PR_INT8 *szTitle,
									PR_UINT32 theStype,
									PR_INT32 nTimeoutSec,
									PR_INT8 *args,...);
/*
 * @author:
 * @Date: 	10-25-2014
 * @Record: create it;
 *
 * @函数名称: TransView_vShowLine
 * @函数功能: 在指定行上显示字符串数据
 * @输入参数:  LINE - 需要显示的行，所在的行号
 *			  TYPE - 显示方式 	EM_DTYPE_NORMAL,			//正显
								EM_DTYPE_REVERT,			//反显
								EM_DTYPE_WITH_UNDERLINE,	//带下划线正显
								EM_DTYPE_WITH_TOPLINE,		//带上划线正显
								EM_DTYPE_WITH_RECT			//加框

			 PATTERN - 对齐方式 ：
			 	 	 	 	 	 	EM_ALIGN_LEFT   // 左对齐
									EM_ALIGN_RIGHT  // 右对齐
									EM_ALIGN_CENTER // 居中对齐
			... -可变参数表
 * @输出参数:
 *
 * @返回值:
 *
 * @备注:
 *
 * @调用示例:
 * 			/// 在 第0行显示标题内容为 “标题显示”，并设置为反显 居中
 * 			TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,"标题显示");
 * 			// 或者这么调用同上效果
 * 			TransView_vShowLine(0, EM_DTYPE_REVERT, EM_ALIGN_CENTER,"%s","标题显示");
 *
 */

#define  TransView_vShowLine(LINE, TYPE, PATTERN, ...) \
			 Disp_vShowStr((LINE), (TYPE), (PATTERN), ##__VA_ARGS__)

/*
 * @author:
 * @Date: 	10-25-2014
 * @Record: create it;
 *
 * @函数名称: TransView_vShowLineEx
 * @函数功能: 在指定行上显示字符串数据 （能够自动换行）
 * @输入参数:  LINE - 需要显示的行，所在的行号
			 PATTERN - 对齐方式 ：
			 	 	 	 	 	 	EM_ALIGN_LEFT   // 左对齐
									EM_ALIGN_RIGHT  // 右对齐
									EM_ALIGN_CENTER // 居中对齐
			... -可变参数表
 * @输出参数:
 *
 * @返回值:
 *
 * @备注:
 *
 * @调用示例:
 *
 */

#define  TransView_vShowLineEx(LINE, PATTERN, ...) \
			Disp_vShowStrAtEx((LINE), (PATTERN), ##__VA_ARGS__)

#define  TransView_vShowPersianByLine(LINE, PATTERN, ...) \
			Disp_vShowPersianStr((LINE), (PATTERN), ##__VA_ARGS__)
/*
 * @author:
 * @Date: 	10-25-2014
 * @Record: create it;
 *
 * @函数名称: TransView_vClearAllScreen
 * @函数功能: 清空屏幕上所有用户显示内容。不包括状态栏和输入法显示栏
 * @输入参数: 无
 *
 * @输出参数:
 *
 * @返回值:
 *
 * @备注:
 *
 * @调用示例:
 *
 */
#define 	TransView_vClearAllScreen() Disp_vClearPort()
#define     TransView_vClearLine(LINE)	Disp_vClearLine(LINE)
/*
 * @author:
 * @Date: 	10-25-2014
 * @Record: create it;
 *
 * @函数名称: TransView_vClearPort
 * @函数功能: 清空屏幕显示内容,但保留 Title行数据
 * @输入参数: 无
 *
 * @输出参数:
 *
 * @返回值:
 *
 * @备注:
 *
 * @调用示例:
 *
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

void  wifiSet();

#ifdef __cplusplus
}
#endif
#endif 

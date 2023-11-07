#ifndef __DISP_H__
#define __DISP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define BOOL	unsigned char
#define INT64   long
#define INT32	int
#define INT16 	short
#define INT8	char

#define UINT64	unsigned  long

#define UINT16	unsigned  short
#define UINT8	unsigned  char


#define FAILD   	(-1)
#define PARAM_ERROR		(-2)
#define UNKNOW_ERROR	(-255) // defined as


#define Bool		BOOL
#define Int64		INT64
#define Int32		INT32
#define	Int16		INT16
#define Int8		INT8

#define Uint64		UINT64
#define	Uint32		unsigned  int
#define Uint16		UINT16
#define Uint8		UINT8

#define True		TRUE
#define False		FALSE
#define Faild       FAILD

#define SHOW_STR_MAX_LINE 	6

#define DEFAULT_LEFT_MARGIN			10 //  默认左边缘留空像素个数
#define DEFAULT_HEIGHT_LINE			26	// 默认行高
#define DEFAULT_LINE_YPOINT(LINE)	(18 + DEFAULT_HEIGHT_LINE*(LINE)) //默认每行Y轴坐标,计算方式，状态栏22，每个字32高，上下间距1个，
#define DEFAULT_PORT_WIDTH			300  // 默认显示区域宽度
#define DEFAULT_PORT_HEIGHT			(DEFAULT_HEIGHT_LINE)   // 默认输入框区域高度

typedef enum KB_KEYMAP_e
{
	EM_KEY_INVALID = -1,
	EM_KEY_1,
	EM_KEY_2,
	EM_KEY_3,
	EM_KEY_4,
	EM_KEY_5,
	EM_KEY_6,
	EM_KEY_7,
	EM_KEY_8,
	EM_KEY_9,
	EM_KEY_0,
	EM_KEY_F1,
	EM_KEY_F2,
	EM_KEY_F3,
	EM_KEY_F4,
	EM_KEY_CLEAR,
	EM_KEY_ENTER,
	EM_KEY_UP,
	EM_KEY_DOWN,
	EM_KEY_POWER,
	EM_KEY_CANCEL,
	EM_KEY_MAX
}KB_KEYMAP_T;

typedef struct
{
    unsigned char Second;
    unsigned char Minute;
    unsigned char Hour;
    unsigned char DayOfMonth;
    unsigned char Month;
    unsigned int Year;
    unsigned char CheckValue[4];
}SetLocalTimeType;

typedef enum DPORT_DTYPE_e
{
	EM_DTYPE_NORMAL,			//正显
	EM_DTYPE_REVERT,			//反显
	EM_DTYPE_WITH_UNDERLINE,	//带下划线正显
	EM_DTYPE_WITH_TOPLINE,		//带上划线正显
	EM_DTYPE_WITH_RECT			//加框
}DPORT_DTYPE_T;

// Align Definition
typedef enum _ALIGN_DEF_T
{
	EM_ALIGN_LEFT 	=0,
	EM_ALIGN_RIGHT  =1,
	EM_ALIGN_CENTER = 2
}ALIGN_DEF_T;

typedef struct POINT_t
{
	Uint32 X;
	Uint32 Y;

}POINT_T;

typedef enum DPORT_STYLE_e
{
	DPORT_DEFAULT_STYLE 				= (1 << 0),	//默认风格	
	DPORT_DISABLE_NUMBERKEY 		= (1 << 1),	// 禁止数字键进行选择
	DPORT_AUTO_INDEX_SEPARATELY 	= (1 << 2),	// 自动添加索引数字，每页独立进行索引
	DPORT_AUTO_INDEX_CONTINUITY	= (1 << 3),	// 自动添加索引数字，所有项连续索引
	DPORT_ENABLE_BYPASS 				= (1 << 4),	// 使能BYPASS功能
	DPORT_LIST_ALWAYS_RETURN_NUMBERKEY = (1 << 5),	//
	DPORT_ENABLE_LANGEN = (1 << 6),	// messagebox显示英文
}DPORT_STYLE_T;


typedef enum DPORT_LINETYPE_e
{
	EM_LINETYPE_SOLID	 	= 0,	//实线
	EM_LINETYPE_DOTTED 	= 1,	//虚线
}DPORT_LINETYPE_T;

typedef enum DPORT_COLOR_e
{
	EM_COLOR_BLACK,
	EM_COLOR_WHITE,
	EM_COLOR_GREY,
	EM_COLOR_RED,
	EM_COLOR_MAX,
}DPORT_COLOR_T;

typedef enum DPORT_IME_TYPE
{
	DPORT_IME_PY = (1<<0),
	DPORT_IME_EN = (1<<1),
	DPORT_IME_NUM =(1<<2),
	DPORT_IME_SYMBOL = (1<<3)
}DPORT_IME_T;

//提示框界面类型
typedef enum DPORT_TIPPAGE_KEY_TYPE_e
{
	EM_STYLE_OK = (1 <<0),	//确认键
	EM_STYLE_CANCEL= (1 <<1)//取消键
}DPORT_TIPPAGE_KEY_TYPE_T;

///返回值 定义
typedef enum DPORT_RET
{
	RET_DPORT_TIMEOUT = -3,
	RET_DRPOT_CANCEL =  -2,	
	RET_DPORT_FAILD  = -1,
	RET_DPORT_NORMAL =  0,
	RET_DPORT_OK     =  RET_DPORT_NORMAL,
	RET_DPORT_BYPASS = RET_DPORT_NORMAL,

}DPORT_RET_T;

typedef struct tagInputPinPara
{
	Int8 szPan[20];
	Int32 nPanLen;
	Int32 nKeyIndex;
	Int32 nPinMode;
	Int8  szHint[64];
	Int32  nMinPinLen;
	Int32  nMaxPinLen;
}InputPinPara;

typedef enum DPORT_FONT_SIZE
{
	DPORT_DEFAULT 	= 0,
	DPORT_FONT_32     	= 1,
	DPORT_FONT_24		= 2,
	DPORT_FONT_22		= 3,
	DPORT_FONT_16		= 4,
	DPORT_FONT_48     	= 5,
	DPORT_FONT_MAX
}DPORT_FONT_SIZE_T;


typedef enum DPORT_LANGEUGE
{
	DPORT_CHINESE	= 0,
	DPORT_ENGLISH  	= 1
}DPORT_LANGEUGE_T;
 

typedef enum DPORT_STYLE
{
	DPORT_LEFT	= 0,
	DPORT_RIGHT,
	DPORT_CENTER
}DPORT_TEXTSTYLE_T;


/*
 * @author:
 * @Date: 	06-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nSetMsgBoxLanguage
 * @函数功能: 设置messgeBox显示语言
 *
 * @输入参数:language 设置语言(DPORT_CHINESE或DPORT_ENGLISH)
 *
 * @输出参数: 无
 *
 * @返回值:无
 * 			
 * @备注:
 *			
 * @调用示例:
 *
 */
void Disp_nSetMsgBoxLanguage(Uint8 language);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_bInit
 * @函数功能: Disp模块初始化
 *
 * @输入参数:无
 *
 * @输出参数: 无
 *
 * @返回值:无
 * 			
 * @备注:
 *			系统初始化时必须调用一次
 * @调用示例:
 *
 */
void Disp_vInit(void);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vClearLine
 * @函数功能: 清空指定行的显示内容
 *
 * @输入参数:nAtLine  指定行号 			 
 *
 * @输出参数: 无
 *
 * @返回值:无
 *
 * @备注:
 *
 * @调用示例:
 *
 */
void Disp_vClearLine(Uint8 nAtLine);


/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vShowStrAt
 * @函数功能: 在指定的行列中显示字符串的内容
 * @输入参数:
 * 			 nAtLine  	指定行的行号
 * 			 nAtColumn	指定列位置(目前未实现， 默认都填写 0)
 * 			 fmt		显示的数据，使用可变参数方式进行输入
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 * @备注:
 * 	行号范围：针对933，大字体下标为:0-5,一共6行,小字体下标为:0-6,一共7行
 * @调用示例:
 * 			DPort_vShowStrAt(1, 0, "%s", "在这里显示");
 *
 */
void Disp_vShowStrAt(Uint8 nAtLine, Uint8 nAtColumn, Int8* fmt, ...);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vShowStrAtEx
 * @函数功能: 在指定的行列中显示字符串的内容,能够自动换行
 * @输入参数:
 * 			 nAtLine  	指定行的行号
 * 			 nPattern	对齐方式  参见ALIGN_DEF_T定义
 * 			 fmt		显示的数据，使用可变参数方式进行输入
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 * @备注:
 * 	行号范围：针对933，大字体下标为:0-5,一共6行,小字体下标为:0-6,一共7行
 * @调用示例:
 *
 */
void Disp_vShowStrAtEx(Uint8 nAtLine, Uint8 nPattern, Int8* fmt, ...);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vShowStr
 * @函数功能: 在指定的行列中显示字符串内容
 * @输入参数:
 * 			 nAtLine  	指定行的位置
 * 			 nType		显示类型  参见DPORT_DTYPE_T定义
 * 			 nPattern	对齐方式  参见ALIGN_DEF_T定义
 * 			 fmt		显示的数据，使用可变参数方式进行输入
 *
 * @输出参数: 无
 *
 * @返回值:无
 * 			
 * @备注:
 * 	行号范围：针对933，大字体下标为:0-5,一共6行,小字体下标为:0-6,一共7行
 * @调用示例:
 *
 */
void Disp_vShowStr(Uint8 nAtLine, Uint8 nType, Uint8 nPattern, Int8* fmt, ...);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vShowImage
 * @函数功能: 在指定像素点位置显示图片
 * @输入参数:
 * 			 pcBmpData			图片数据缓冲
 * 			 LenofBufferSize	pBmpData所指向buffer长度
 * 			 tStartPoint  		指定屏幕显示起始X、Y点位置,起始位置可使用Disp_vGetUserDisplayRange获取
 * 			 uBmpWidth			图片宽
 * 			 uBmpHeight			图片高
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 *
 * @备注:
 *
 * @调用示例:
 * 		
 *
 */
void Disp_vShowImage(Uint8* pcBmpData, Uint32 LenofBufferSize, POINT_T tStartPoint, Uint32 uBmpWidth, Uint32 uBmpHeight);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_bDrawLineAt
 * @函数功能: 在指定点(nBeginX, nBeginY)到(nEndX, nEndY)画一条直线
 * @输入参数:
 * 			 tBegin  	指定直线的起始点X,Y坐标
 * 			 tEnd		指定直线的终点X,Y坐标
 * 			 nType		指定画线类型，实线、虚线(仅支持直线)
 * 			 nColor		指定描绘的颜色
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 *
 * @备注:
 *  参考 POINT_RANGE_T 定义范围
 * 	933 分辨率为 320X240
 *起始、终止位置可通过Disp_vGetUserDisplayRange获取
 * @调用示例:
 *
 */
void Disp_vDrawLineAt(POINT_T tBegin, POINT_T tEnd, DPORT_LINETYPE_T nType, DPORT_COLOR_T nColor);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vClearPort
 * @函数功能: 将应用程序所显示的所有内容清空
 * @输入参数:	 无
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 *
 * @备注:
 *
 * @调用示例:
 *
 */
void Disp_vClearPort(void);

/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vLightOn
 * @函数功能: 打开背光
 * @输入参数:	 无
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 *
 * @备注:
 *
 * @调用示例:
 *
 */
void Disp_vLightOn(void);


/*
 * @author:
 * @Date: 	10-10-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vLightOff
 * @函数功能: 关闭背光
 * @输入参数:	 无
 *
 * @输出参数: 无
 *
 * @返回值:
 * 			无
 *
 * @备注:
 *
 * @调用示例:
 *
 */
void Disp_vLightOff(void);


/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nUserInputView
 * @函数功能: 用户输入框控件
 * @输入参数:
 * 				 X  Y --控件显示的起始位置
 * 				 nWidth  nHeight--控件显示宽高
 * 				 nLimtMin	-----输入最小长度限制
 *				 nLimtMax	-----输入最大长度限制
 *				 nInputMethod --- 输入法类型
 * 				 nOutBufferLen--- 输出缓存长度
 *				 nTimeOut-----超时时间(单位秒)
 *				 nStyle	-----数字键响应以及自动索引功能(预留，填0)
 *
 * @输出参数: pOutBuffer --- 输出缓存
 *
 * @返回值:
 *			成功		--控件输入长度 
 * 			失败	 	-- RET_DPORT_FAILD
 *			超时		 --RET_DPORT_TIMEOUT
 *			用户取消--RET_DRPOT_CANCEL
 *
 * @备注:
 *			单行显示最多字符根据字体大小不同而不同
 * @调用示例:
 *
 */
Int32 Disp_nUserInputView(int X, int Y, int nWidth, int nHeight, 
							Uint32 nLimtMin,
							Uint32 nLimtMax,
							Uint32 nInputMethod,
							Uint32 nOutBufferLen,
							Uint8 *pOutBuffer,
							Int32 nTimeOut,
							Uint32 nStyle);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nUserInputViewEx
 * @函数功能: 用户输入框控件(可设置输入法类型的设置先后顺序)
 * @输入参数:
 * 				 X  Y --控件显示的起始位置
 * 				 nWidth  nHeight--控件显示宽高
 * 				 nLimtMin	-----输入最小长度限制
 *				 nLimtMax	-----输入最大长度限制
 *				 pInputMethod --- 输入法类型
 *				 nLenOfInputMethod	--输入法数量
 * 				 nOutBufferLen--- 输出缓存长度
 *				 nTimeOut-----超时时间(单位秒)
 *				 nStyle	-----数字键响应以及自动索引功能(预留，填0)
 *
 * @输出参数: pOutBuffer --- 输出缓存
 *
 * @返回值:
 *			成功		--控件输入长度 
 * 			失败	 	-- RET_DPORT_FAILD
 *			超时		 --RET_DPORT_TIMEOUT
 *			用户取消--RET_DRPOT_CANCEL
 *
 * @备注:
 *			单行显示最多15个汉字，30个字符；
 *			可通过nShowLineHeight，参数定义控件占用的行数，扩展控件输入的字符个数
 *			例如：最大支持显示45个汉字输入则可以调整参数为
 *			Disp_nUserInputView(1,3,1,45,DPORT_IME_PY,sizeof(auOutBuffer),auOutBuffer);
 * @调用示例:
 *			PR_INT8 auOutBuffer[10+1]={0};
 *			Uint32 InputMethod[] = {DPORT_IME_PY, DPORT_IME_EN};
 *
 *			Disp_nUserInputView(1,2,1,10,InputMethod,sizeof(aucOutBuffer),aucOutBuffer);
 *
 */
Int32 Disp_nUserInputViewEx(int X, int Y, int nWidth, int nHeight, 
							Uint32 nLimtMin,
							Uint32 nLimtMax,
							Uint32* pInputMethod,
							Uint32 nLenOfInputMethod,
							Uint32 nOutBufferLen,
							Uint8 *pOutBuffer,
							Int32 nTimeOut,
							Uint32 nStyle);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nUserInputViewEx
 * @函数功能: 用户输入框控件(可设置输入法类型的设置先后顺序)
 * @输入参数:
 * 				 X  Y --控件显示的起始位置
 * 				 nWidth  nHeight--控件显示宽高
 * 				 nLimtMin	-----输入最小长度限制
 *				 nLimtMax	-----输入最大长度限制
 *				 pInputMethod --- 输入法类型
 *				 nLenOfInputMethod	--输入法数量
 * 				 nOutBufferLen--- 输出缓存长度
 *				 nTimeOut-----超时时间(单位秒)
 *				 nStyle	-----数字键响应以及自动索引功能(预留，填0)
 *
 * @输出参数: pOutBuffer --- 输出缓存
 *
 * @返回值:
 *			成功		--控件输入长度 
 * 			失败	 	-- RET_DPORT_FAILD
 *			超时		 --RET_DPORT_TIMEOUT
 *			用户取消--RET_DRPOT_CANCEL
 *
 * @备注:
 *			单行显示最多15个汉字，30个字符；
 *			可通过nShowLineHeight，参数定义控件占用的行数，扩展控件输入的字符个数
 *			例如：最大支持显示45个汉字输入则可以调整参数为
 *			Disp_nUserInputView(1,3,1,45,DPORT_IME_PY,sizeof(auOutBuffer),auOutBuffer);
 * @调用示例:
 *			PR_INT8 auOutBuffer[10+1]={0};
 *			Uint32 InputMethod[] = {DPORT_IME_PY, DPORT_IME_EN};
 *
 *			Disp_nUserInputView(1,2,1,10,InputMethod,sizeof(aucOutBuffer),aucOutBuffer);
 *
 */
Int32 Disp_nUserInputViewExEx(int X, int Y, int nWidth, int nHeight, 
							Uint32 nLimtMin,
							Uint32 nLimtMax,
							Uint32* pInputMethod,
							Uint32 nLenOfInputMethod,
							Uint32 nOutBufferLen,
							Uint8 *pOutBuffer,
							Int32 nTimeOut,
							Uint32 nStyle, 
							Uint8 *pszInfo);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nMessageBox
 * @函数功能: 提示框控件
 * @输入参数:	 
 			 szTitle 		--标题信息
 			 theStype		--界面风格参考DPORT_TIPPAGE_KEY_TYPE_T
 			 nTimeoutSec  --超时时间(单位秒)
 *			nStyle	-----数字键响应以及自动索引功能(预留，填0)
 			 args		--格式字符串
 *
 * @输出参数: 无
 *
 * @返回值: 
 *			RET_DPORT_OK 	 --- 确认键返回值
 *			RET_DRPOT_CANCEL --- 取消键返回值
 *			RET_DPORT_TIMEOUT --超时
 * @备注:
 *			最大支持 100个汉字，200个字符显示
 * @调用示例:
 *	 	 界面类型:目前只有单确认键和确认取消键
 * 	 	 Disp_nMessageBox("标题信息",EM_STYLE_OK, 5,"%s","交易成功");
 * 	 	 Disp_nMessageBox("标题信息",EM_STYLE_OK|EM_STYLE_CANCEL, 5, "%s","交易成功");
 *
 */
Int32 Disp_nMessageBox(Int8 *szTitle, Int32 theStype, Int32 nTimeoutSec, Uint32 nStyle, Int8 *args,...);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nMessageBoxEx
 * @函数功能: 提示框控件(内容可翻页)
 * @输入参数:	 
 *			X 	Y  	   --提示框坐标 
 *			nWidth  nHeight    --提示框大小
 * 			szTitle --标题信息
 * 			theStype	   --界面风格参考DPORT_TIPPAGE_KEY_TYPE_T
 *			nTimeoutSec	   --超时时间(单位秒)
 *			nStyle	-----数字键响应以及自动索引功能(预留，填0)
 * 			args		   --格式字符串
 *
 * @输出参数: 无
 *
 * @返回值: 按键值
 *			RET_DPORT_OK 	 	--确认键返回值
 *			RET_DRPOT_CANCEL	--取消键返回值
 *			RET_DPORT_TIMEOUT 	--超时
 *
 * @备注:
 *			最大支持 100个汉字，200个字符显示
 * @调用示例:
 *	 	 界面类型:目前只有单确认键和确认取消键
 *		Disp_nMessageBoxEx(5, 27, 310, 200, NULL,MB_OK | MB_CANCEL, 0, 0,  "%s", "内容测试");
 *		Disp_nMessageBoxEx(5, 27, 310, 200, "标题测试",MB_OK, 0,  0,"%s", "内容测试");
 *
 */
int Disp_nMessageBoxEx(int X, int Y, int nWidth, int nHeight, char* szTitle, int theStype, int nTimeoutSec, Uint32 nStyle, char *args,...);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowEditBox
 * @函数功能: 显示字符串控件(内容可翻页)
 * @输入参数:	 
 *			X 	Y  		--提示框坐标 
 *			nWidth  nHeight   	--提示框大小
 * 			nTimeoutSec		--超时时间(单位秒)
 *			nStyle	-----数字键响应以及自动索引功能(预留，填0)
 * 			fmt			--格式字符串
 *
 * @输出参数: 无
 *
 * @返回值: 按键值
 *			RET_DPORT_OK 	 	--- 确认键返回值
 *			RET_DPORT_TIMEOUT 	--- 超时
 *			RET_DRPOT_CANCEL	--用户取消
 *			RET_DPORT_FAILD		--参数错误
 * @备注:
 *			最大支持 100个汉字，200个字符显示
 * @调用示例:
 *
 *	Disp_nShowEditBox(10, 30, 300, 60, 10, 0,"%s", "交易成功");
 *
 */
int Disp_nShowEditBox(int X, int Y, int nWidth, int nHeight, int nTimeoutSec, unsigned int nStyle, char* fmt, ...);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowEditBoxEx
 * @函数功能: 显示字符串控件(内容可翻页)
 * @输入参数:	 
 *			X 	Y  			--提示框坐标 
 *			nWidth  nHeight   	--提示框大小
 * 			nTimeoutSec		--超时时间(单位秒)
 *			nStyle			--数字键响应以及自动索引功能(预留，填0)
 * 			string			--显示字符串
 *
 * @输出参数: 无
 *
 * @返回值: 按键值
 			RET_DPORT_OK 	 --- 确认键返回值
 			RET_DPORT_TIMEOUT 	--- 超时
 			RET_DRPOT_CANCEL	--用户取消
 *
 * @备注:
 *			
 * @调用示例:
 *
 *	Disp_nShowEditBox(10, 30, 300, 60, 10, 0, "交易成功");
 *
 */
int Disp_nShowEditBoxEx(int X, int Y, int nWidth, int nHeight, int nTimeoutSec, unsigned int nStyle, char* string);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowLogoView
 * @函数功能: 显示待机画面
 * @输入参数:	 
 *			 szLogoPath  -- 待机画面图片路径，格式需要为565格式
 * 			 szScreenTip -- 待机画面提示信息内容
 *
 * @输出参数: 无
 *
 * @返回值: 无
 *
 * @备注:
 *
 * @调用示例:
 *
 * 	 	 Disp_vShowLogoView("UnionPay.bmp", "中国银联");
 */
void Disp_vShowLogoView(Int8 *szLogoPath, Int8* pszScreenTip);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowAmountInputView
 * @函数功能: 显示金额输入界面
 * @输入参数:	 
 *				nX  nY 			--金额框坐标
 * 				nWidth  nHeight 	--金额框大小
 *				pszCurrencyFlag	--货币符号字符串
 *				nMinLen			--最小长度限制
 *				nMaxLen			--最大长度限制
 *				nLenOfBuffer		--pszAmount所指向 buffer长度
 *				nTimeoutSec		--超时时间(单位秒)
 *			nStyle	-----数字键响应以及自动索引功能(预留，填0)
 * @输出参数:
 *				pszAmount		--输入金额buffer
 * @返回值:
 * 			成功 	  -- 金额长度
 *			失败	  -- RET_DPORT_FAILD
 *			用户取消 	-- RET_DRPOT_CANCEL
 * @备注:
 *
 * @调用示例:
 *
 */
int 	Disp_nShowAmountInputView(int nX, int nY, int nWidth, int nHeight,
								  char* pszCurrencyFlag, int nMinLen, int nMaxLen, 
								  int nLenOfBuffer,char* pszAmount, int nTimeoutSec,
								  unsigned int nStyle);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowAmountInputViewEx
 * @函数功能: 显示金额输入界面
 * @输入参数:	 
 *				nX  nY 			--金额框坐标
 * 				nWidth  nHeight 	--金额框大小
 *				pszCurrencyFlag	--货币符号字符串
 *				nMinLen			--最小长度限制
 *				nMaxLen			--最大长度限制
 *				nLenOfBuffer		--pszAmount所指向 buffer长度
 *				nTimeoutSec		--超时时间(单位秒)
                pszInfo         --输入小于长度限制时的提示
 *			    nStyle	-----数字键响应以及自动索引功能(预留，填0)
 * @输出参数:
 *				pszAmount		--输入金额buffer
 * @返回值:
 * 			成功 	  -- 金额长度
 *			失败	  -- RET_DPORT_FAILD
 *			用户取消 	-- RET_DRPOT_CANCEL
 * @备注:
 *
 * @调用示例:
 *
 */
int Disp_nShowAmountInputViewEx(int nX, int nY, int nWidth, int nHeight,
								  char* pszCurrencyFlag, int nMinLen, int nMaxLen, 
								  int nLenOfBuffer,char* pszAmount, int nTimeoutSec,
								  unsigned int nStyle, char *pszInfo);


/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowPasswdInputView
 * @函数功能: 显示用户登录密码输入框
 * @输入参数:
 *				nX  nY 			--密码框坐标
 * 				nWidth  nHeight 	--密码框大小 
 				nLenOfBuffer		--pszPasswd所指向buffer长度
 				nTimeoutSec		--超时时间
 *				nStyle	-----数字键响应以及自动索引功能(预留，填0) 				
 * @输出参数: 
 				pszPasswd		--输入密码buffer
 *
 * @返回值:
 * 			成功 			-- 返回输入的密码长度
 			失败			-- RET_DPORT_FAILD  (包括超时)
 *			用户取消 	-- RET_DRPOT_CANCEL
 * @备注:
 *			用户输入字符串最大长度，根据显示框的宽度于每个字符的宽度决定
 * @调用示例:
 *
 */
int 	Disp_nShowPasswdInputView(int nX, int nY, int nWidth, int nHeight, int nMinLen, 
							int nMaxLen,int nLenOfBuffer,char* pszPasswd, 
							int nTimeoutSec, unsigned int nStyle);


/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowPINPADInputView
 * @函数功能: 内置密码键盘输入框
 * @输入参数:
 *			  	X			--输入框起始坐标
 * 				Y 			--输入框起始坐标
 * 				nWidth		--输入框宽度
 * 				nHeight		--输入框高度
 				pszInputPin	--输入PIN属性
 				nLenOfPinBuffer --pszPin所指向buffer长度
 				ntimeoutsec:	超时时间，秒为单位，最长为60秒
 *
 * @输出参数: pszPin	返回的PIN码
 *
 * @返回值:
 * 			成功 		- 固定返回用户输入的字符长度
 			失败			- RET_DPORT_FAILD
 *			用户取消 		-RET_DRPOT_CANCEL
 			超时			-RET_DPORT_TIMEOUT
 * @备注:
 *			用户输入字符串最大长度，根据显示框的宽度于每个字符的宽度决定
 * @调用示例:
 *
 */
int 	Disp_nShowPINPADInputView(int nX,int nY,int nWidth,int nHeight,
		  	  	  	  	  	  	  InputPinPara *pszInputPin,
		  	  	  	  	  	  	  int nLenOfPinBuffer,int ntimeoutsec,char* pszPin);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowIPInputLine
 * @函数功能: 显示IP输入框
 * @输入参数:
 				X		--输入框起始坐标
 				Y 		--输入框起始坐标
 				nWidth		--输入框宽度
 				nHeight		--输入框高度
 				nMinLen		--输入最小长度限制
 				nMaxLen		--输入最大长度限制
 				nLenOfBuffer	--pszIP所指向内存的大小
 				nTimeoutSec	--超时时间
 *				nStyle	-----数字键响应以及自动索引功能(预留，填0) 				
 *
 * @输出参数: pszIP		--返回IP字符串
 *
 * @返回值:
 * 			成功 	- 固定返回用户输入的字符长度
 *			失败	- RET_DPORT_FAILD
 *			用户取消  - RET_DRPOT_CANCEL
 *			超时	-RET_DPORT_TIMEOUT		
 * @备注:
 *
 * @调用示例:
 *
 */

int 	Disp_nShowIPInputLine(int X, int Y, int nWidth, int nHeight, int nMinLen, int nMaxLen, 
								int nLenOfBuffer,char *pszIP, int nTimeoutSec, unsigned int nStyle);

/*
 * @author:
 * @Date: 	10-27-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nGetUserDisplayRange
 * @函数功能: 返回屏幕可显示区域范围( X, Y 轴的起至参数)
 * @输入参数:
 *
 * @输出参数: 
 *				pnStartX		起始X坐标
 *				pnEndX		起始Y坐标
 *				pnStartY		终止X坐标
 *				pnEndY		终止Y坐标
 *
 * @返回值:
 * 			无
 * @备注:
 *
 * @调用示例:
 *
 *	int nStartX, nEndX, nStartY, nEndY;
 * 	Disp_vGetUserDisplayRange(&nStartX, nEndX, &nStartY, &nEndY);
 *
 */
void Disp_vGetUserDisplayRange(int *pnStartX, int *pnEndX, int *pnStartY, int *pnEndY);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowListView
 * @函数功能: 列表显示窗口，用于显示单列表窗口
 *
 * @输入参数:
 * 				iX 	iY 		--列表框坐标
 				uWidth uHeight	--列表大小
 				pszTitle			--列表标题 
 * 				 pszListItem  -- 显示列表
 * 				 nItemCount   -- 显示项目数
 *				nItemsPerPage	--每页显示项目数
 *				nSelectItem		--初始选中项目
 *				nTimeoutSec		--超时时间:单位秒
 *				nStyle	-----数字键响应以及自动索引功能	
 *@输出参数:
 *				pnIndexOfSelect --选择的项目索引(从0开始计)
 *
 * @返回值:
 *			成功:RET_DPORT_NORMAL
 *			超时:RET_DPORT_TIMEOUT
 *			取消:RET_DRPOT_CANCEL
 *
 * @备注: 只支持 英文、数字输入
 *
 * @调用示例:
 *
 */
int Disp_nShowListView(int iX, int iY, unsigned int uWidth, unsigned int uHeight, char* pszTitle, char** pszListItem, int nItemCount, int nItemsPerPage,
									 int nSelectItem, int nTimeoutSec, int* pnIndexOfSelect, unsigned int nStyle);
/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowListViewEx
 * @函数功能: 列表显示窗口，用于显示单列表窗口
 *
 * @输入参数:
 * 				pszTitle			--列表标题 
 * 				 pszListItem  -- 显示列表
 * 				 nItemCount   -- 显示项目数
 *				nItemsPerPage	--每页显示项目数
 *				nSelectItem		--初始选中项目
 *				nTimeoutSec		--超时时间:单位秒
 *				nStyle	-----数字键响应以及自动索引功能	
 *@输出参数:
 *				pnIndexOfSelect --选择的项目索引(从0开始计)
 *
 * @返回值:
 *			成功:RET_DPORT_NORMAL
 *			超时:RET_DPORT_TIMEOUT
 *			取消:RET_DRPOT_CANCEL
 *
 * @备注: 只支持 英文、数字输入
 *
 * @调用示例:
 *
 */
int Disp_nShowListViewEx(char* pszTitle, char** pszListItem, int nItemCount, int nItemsPerPage,
									 int nSelectItem, int nTimeoutSec, int* pnIndexOfSelect, 
									 unsigned int nStyle);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowTableView
 * @函数功能: 双列表显示UI.用于双列显示
 *
 * @输入参数:
  * 				iX 	iY 		--列表框坐标
 				uWidth uHeight	--列表大小
 * 				pszTitle			--列表标题 
 * 				 pszListItem  -- 显示列表
 * 				 nItemCount   -- 显示项目数
 *				nItemsPerPage	--每页显示项目数
 *				nSelectItem		--初始选中项目
 *				nTimeoutSec		--超时时间:单位秒 
 *				nStyle			-----数字键响应以及自动索引功能	
 *@输出参数:
 *				pnIndexOfSelect  --选择的项目索引(从0开始计)
 *
 * @返回值:
  *			成功:RET_DPORT_NORMAL
 *			超时:RET_DPORT_TIMEOUT
 *			取消:RET_DRPOT_CANCEL
 *			失败:RET_DPORT_FAILD
 *
 * @备注: 只支持 英文、数字输入
 *
 * @调用示例:
 *
 */
int Disp_nShowTableView(int iX, int iY, unsigned int uWidth, unsigned int uHeight,
							char* pszTitle, char** pszListItem, int nItemCount, int nItemsPerPage,
							int nSelectItem,int nTimeoutSec, int* pnIndexOfSelect, unsigned int nStyle);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowTableViewEx
 * @函数功能: 双列表显示UI.用于双列显示
 *
 * @输入参数:
 * 				pszTitle			--列表标题 
 * 				 pszListItem  -- 显示列表
 * 				 nItemCount   -- 显示项目数
 *				nItemsPerPage	--每页显示项目数
 *				nSelectItem		--初始选中项目
 *				nTimeoutSec		--超时时间:单位秒 
 *				nStyle			-----数字键响应以及自动索引功能	 
 *@输出参数:
 *				pnIndexOfSelect  --选择的项目索引(从0开始计)
 *
 * @返回值:
 *
 * @备注: 只支持 英文、数字输入
 *
 * @调用示例:
 *
 */

//列表控件
int Disp_nShowTableViewEx(char* pszTitle, char** pszListItem, int nItemCount,
					int nItemsPerPage, int nSelectItem,int nTimeoutSec, 
					int* pnIndexOfSelect, unsigned int nStyle);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vCleanArea
 * @函数功能:清除指定区域
 *
 * @输入参数:
					X   Y 			---清除区域的坐标
					nWidth  nHeight	---清除区域的大小
 *@输出参数:
 *				无
 *
 * @返回值:
 *
 * @备注: 
 *
 * @调用示例:
 *
 */
void Disp_vCleanArea(int X, int Y, int nWidth, int nHeight);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_nShowDateInputView
 * @函数功能:显示日期输入框
 *
 * @输入参数:
 *					nTimeOutSec		---超时时间
 *@输出参数:
 *					pDate			---返回的时间值
 *
 * @返回值:
 *				成功:RET_DPORT_NORMAL
 *				失败:RET_DPORT_FAILD
 *				取消:RET_DRPOT_CANCEL
 *				超时:RET_DPORT_TIMEOUT
 *
 * @备注: 
 *
 * @调用示例:
 *
 */
int Disp_nShowDateInputView(SetLocalTimeType* pDate,  int nTimeOutSec);

/*
 * @author:
 * @Date: 	10-25-2022
 * @Record: create it;
 *
 * @函数名称: Disp_vSetSysFont
 * @函数功能:设置系统字体
 *
 * @输入参数:
 *					FontType		---字体类型
 *@输出参数:	无
 *
 * @返回值:		无
 *				
 * @备注: 
 *
 * @调用示例:
 *
 */
void Disp_vSetSysFont(DPORT_FONT_SIZE_T FontType);

/*
 * @author:
 * @Date: 	2022-08-14
 * @Record: create it;
 *
 * @函数名称: Disp_vDrawTextA
 * @函数功能:在制定坐标显示字符串
 *
 * @输入参数:
 *				x           x坐标
 *              y           y坐标
 *              w           显示宽度
 *              h           显示长度
 *              pstar       显示字符串
 *              len         字符串长度
 *              nStyle      字体显示方式
 *@输出参数:	无
 *
 * @返回值:		无
 *				
 * @备注: 
 *
 * @调用示例:
 *
 */
void Disp_vDrawTextA(int x, int y, int w, int h, const char * pStr, int len, DPORT_TEXTSTYLE_T nStyle);

int Disp_ReleasKey();

int Disp_GetKey(int ms);

#ifdef __cplusplus
}
#endif
#endif



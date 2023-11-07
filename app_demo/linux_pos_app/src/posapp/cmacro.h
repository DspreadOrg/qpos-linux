#ifndef _CMACRO_H__
#define _CMACRO_H__ 

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 *   define   types
 *-----------------------------------------------------------------------------*/
#ifndef uint32_t
#define uint32_t  unsigned int  
#endif

#ifndef uint16_t
#define uint16_t  unsigned short 
#endif

#ifndef uint8_t
#define uint8_t   unsigned char  
#endif

#ifndef uchar
#define uchar   unsigned char  
#endif
/*-----------------------------------------------------------------------------
 *  define 
 *-----------------------------------------------------------------------------*/
#ifndef YES
#define YES       0x01 
#endif

#ifndef NO
#define NO        0x00 
#endif

#ifndef OK
#define OK        0x01 
#endif

#ifndef ERROR
#define ERROR     0x01 
#endif

#ifndef ON
#define ON        0x01 
#endif

#ifndef OFF
#define OFF       0x00 
#endif

#ifndef NORMAL
#define NORMAL    0x00 
#endif

#ifndef EXCEPTION
#define EXCEPTION  0x01 
#endif

#ifndef true
#define true       0x00 
#endif

#ifndef false
#define false      0x01 
#endif

#ifndef yes
#define yes        0x01
#endif

#ifndef no
#define no         0x00 
#endif

#ifndef success
#define success    0x00 
#endif

#ifndef fail
#define fail       0x01 
#endif

/**************************************************
       操作宏定义
***************************************************/
//获取成员MEMBER在结构体TYPE中的偏移
#ifndef FPOS
#define FPOS(TYPE, MEMBER) ((unsigned int)&((TYPE *)0)->MEMBER)
#endif

//获取TYPE结构体的MEMBER成员的大小
#ifndef FSIZE
#define FSIZE(TYPE, MEMBER) (sizeof(((TYPE*)0)->MEMBER))
#endif

#ifndef LEN
#define LEN(x,Type)   (sizeof(x)/sizeof(Type))
#endif

#ifndef DIM
#define DIM(x)    (sizeof(x)/sizeof(x[0]))   
#endif

#ifndef CLRBUF
#define CLRBUF(Buf)     memset(Buf, 0, sizeof(Buf))  
#endif

#ifndef BUFCLR
#define BUFCLR(Buf)      memset(Buf, 0, sizeof(Buf)) 
#endif

#ifndef ZERO_STRUCT
#define ZERO_STRUCT(x)     memset((char *) &(x), 0, sizeof(x))
#endif

#ifndef CLRSTRU
#define CLRSTRU(x)      memset((char *) &(x), 0, sizeof(x)) 
#endif

#ifndef FILLBUF
#define FILLBUF(Buf,x)  memset(Buf, x, sizeof(Buf)) 
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


/**************************************************
       转换宏定义
***************************************************/

#ifndef BCD2BIN
#define BCD2BIN(x)         (((x) & 0x0f) + ((x)>>4)*10)
#endif

#ifndef BIN2BCD
#define BIN2BCD(x)          ((((x)/10)<<4) + (x)%10) 
#endif

#ifndef LHALFB
#define LHALFB(x)     (unsigned char)(x&0x0F)  //获取低半字节
#endif

#ifndef HHALFB
#define HHALFB(x)     (unsigned char)(x>>4)    //获取高半字节
#endif

#ifndef LBYTE
#define LBYTE(x)      (unsigned char)(x)       //获取0~7位
#endif

#ifndef HBYTE
#define HBYTE(x)      (unsigned char)(x>>8)    //获取8~15位
#endif

#ifndef H2BYTE
#define H2BYTE(x)     (unsigned char)(x>>16)   //获取16~23位
#endif

#ifndef H3BYTE
#define H3BYTE(x)     (unsigned char)(x>>24)   //获取24~31位
#endif

#ifndef LHALFW
#define LHALFW(x)     (unsigned short)(x)      //获取低半字
#endif

#ifndef HHALFW
#define HHALFW(x)     (unsigned short)(x>>16)  //获取高半字
#endif
//----------------位操作相关------------------
#ifndef IFBIT
	#define IFBIT(Data,bit)      (Data&(1<<bit))  //
#endif

#ifndef GETBIT
	#define GETBIT(Data,bit)     ((Data&(1<<bit))>>bit)  //
#endif

// 将指定位old移动新的位newbit 如:0010,0000 MOVEBIT(0x20,5,3)=0000,1000
#ifndef MOVEBIT
	#define MOVEBIT(Data,oldbit,newbit)     (((Data&(1<<oldbit))>>oldbit)<<newbit)
#endif
// 将指定位old右移动Offset位 如:0010,0000 MOVERBIT(0x20,5,2)=0000,1000
#ifndef MOVERBIT
	#define MOVERBIT(Data,oldbit,OffsetBit)    ((Data&(1<<oldbit))>>OffsetBit)
#endif
// 将指定位old左移动Offset位 如:0010,0000 MOVELBIT(0x20,5,2)=1000,0000
#ifndef MOVELBIT
	#define MOVELBIT(Data,oldbit,OffsetBit)    ((Data&(1<<oldbit))<<OffsetBit)
#endif

#ifndef SETBIT
	#define SETBIT(Data,bit)     (((Data |(1<<bit)))
#endif

#ifndef CLRBIT
	#define CLRBIT(Data,bit)   ((Data &(~(1<<bit))))
#endif
//----------------字节转换操作相关----------------
//半字节转换 b3<->b0   b2<->b1
#ifndef CONVERTHBYTE
	#define CONVERTHBYTE(x)  (MOVERBIT(x,3,(3-0))|MOVERBIT(x,2,(2-1))|MOVELBIT(x,0,(3-0))|MOVELBIT(x,1,(2-1)))
#endif
//字节转换 b7<->b0   b6<->b1  b5<->b2  b4<->b3
#ifndef CONVERTBYTE
	#define CONVERTBYTE(x)  ((x>>7)|MOVERBIT(x,6,(6-1))|MOVERBIT(x,5,(5-2))|MOVERBIT(x,4,(4-3))|MOVELBIT(x,1,(6-1))|MOVELBIT(x,2,(5-2))|MOVELBIT(x,3,(4-3))|(x<<7))
#endif
//----------------字节合并操作相关----------------
//半字节合并成8位
#ifndef COMBINE8
	#define COMBINE8(High,Low)         ((unsigned char)(High<<4) | (unsigned char)(Low & 0x0F))
#endif
//2字节合并成16位
#ifndef COMBINE16
	#define COMBINE16(High,Low)        ((unsigned short)(High<<8) | (unsigned short)(Low & 0xFF))
#endif
//4字节合并成32位
#ifndef COMBINE32
	#define COMBINE32(Hhigh,Hlow,Lhigh,Llow)  ( (unsigned long)(Hhigh<<24) | (unsigned long)(((unsigned char)Hlow)<<16) | (unsigned long)(((unsigned char)Lhigh)<<8) | (unsigned long)((unsigned char)Llow))
#endif



/**************************************************
       判断返回值宏定义
***************************************************/
#define  assert_ok(x)             {if(x){return EXCEPTION;}}   //非0返回EXCEPTION
#define  assert_null(x)           {if(!x)  return 1;}  //空指针返回
#define  ASSERT(p)

#define  ASSERT_NORMAL(x)              assert_ok(x)
#define  ASSERT_TRANSBOUNDARY(x,y)    {if(x>y){DISPERR(x);return 1;}} /* 越界检测 */
#define  STOP_LOOP()                  {while(1){sleep(60);}}

//定义应用函数返回
enum
{
	//----------------错误返回码---------------------
	ERRVALUE	=	-1,
	CANCELED	=	-2,
	APPTIMEOUT	=	-3,
	BACK		=	-4,
	CHECKCARD	=	-5,
	END			=	-6,             //返回结束标志
	AMTEXCEED	=	-7,
	NOT_EXPECTED=	-8,				//与期望不符（如：期望的卡号信息与读取到的卡号信息不一致）
	NEED_RELOGIN=	-9,				//需要重新签到
	WRITE_CARD_ERR= -10,			//写卡失败
	ERR_CARD	=	-11,			//无效M1管理卡
	NOT_DATA	=	-12,			//卡内无数据
	RETRADE		=	-13,			 //请重刷后 需重新走交易流程
	DISCONNECTED=	-14,			//连接已断开
	PARSE_FAIL	=	-15,			//数据解析失败
	NOTENOUGH_STORAGE=-16,			//数据长度大于缓存
	NOT_RECORD_NEED_SENDUP=-17,		
	NEED_CHECK	=-18,				//需查询支付状态
	BACK_2		=-19,				//请重刷，需要下电，提示用户
	BACK_UNIONPAY	=-20,			//返回尝试银行卡流程
	BACK_3		=-21,				//闪卡重刷
	BLACK_CARD	=-22,				//黑名单已被锁
	BACK_4		=-23,				//计算票价后的特殊返回
	BACK_UNIONPAY_1902	=-24,		//1902返回尝试银行卡流程
	////----------------正常返回码--------------------
	CNTC_BESTPAY=    1,
	M1_CARD,						//M1卡 
	CHANG_AN_TONG_CARD,			//长安一卡通
	QR_BESTPAY,					//POS通支付
	QR_NFCPAY,					//POS通支付
	APPLE_NFCPAY,				//POS通支付
	MANAGE_CARD ,				//新添太古可乐管理卡
	READCARD_MEMBER_CARD,		//会员卡
	QR_CSCANBPAY,				//二维码C扫B支付
	YINS_MEMBER_CARD,			//会员卡
	M1_MANAGE_CARD,				//M1管理卡
	RF_CARD,					//射频卡
	DRIVER_LOGIN,				//司机签到
	SETTING_CARD,				//设置卡
	COIN_RET,					//现金
	TRANSP_CARD_TYPE,			//交通卡
	LUHU_BUS_CARD,				//路虎员工卡
};


#define  EM_NULL			0
#define  PACK_WHEAD_LEN		11 //报文头发送长度
#define  PACK_RHEAD_LEN     11 //报文头接收长度


/*交易类型 nTransType 定义 */
typedef enum enmTransType
{
	PURCHASE = 1,                           // 消费
	INQUERY,                                // 查询余额
	REFUND,                                 // 退款
	POS_VOID,                               // 撤销
	PRE_AUTH,                               // 预授权
	PRE_AUTH_ADD,                           // 追加预授权
	AUTH_VOID,                              // 预授权撤销
	AUTH_DAY_VOID,                          // 预授权当日撤销
	CONFIRM,                                // 预授权完成请求
	CONFIRM_OFFLINE,                        // 预授权完成通知
	CONFIRM_VOID,                           // 预授权完成撤销
	LOGIN,                                  // 签到/12
	LOGOUT,                                 // 签退
	SETTLE,                                 // 结算
	BATCH_UP,                               // 批上送
	SETTLE_2,                               // 二次结算
	ONLINE,                                 // 联机管理交易
	DOWN_PARAM,                             // 下载参数
	UPLOADSTATE,                            //上送POS状态

	EP_PURCHASE,			                //电子钱包消费
	EP_INQUERY,			                    //电子钱包查询余额
	STRIPE,					                //脚本
	ONLINEMANA,	                            //联机管理交易
	EC_QUERY,		     	                //电子现金余额查询
	EC_PURCHASE,			                //电子现金消费
	qPBOC,			                        //非接触IC卡快速支付
#ifdef SERVER_XUNLIAN
	XUNLIAN_DOWNLOAD_MASTER_KEY,					//下载主密钥
#endif
	MAINKEY_DOWN,							//下载主密钥
	GETURL,					//联机获取二维码
	QUERY_TRANSTA,			//查询交易状态
	YS_YXLM,
	YS_YFF,
#if defined DALIAN_MINGZHU_CARD || defined HAERBING_CARD
	CARDNO_QUERY,			//交通卡卡查询
	BALANCE_QUERY,			//交通卡余额查询
#endif

#ifdef GONG_JIAO_LIAONING
	TYPE_QRPAY_LN,				//二维码消费
#endif
#ifdef  CODE_SCANNER_SOCKET
	TYPE_CSS_QR_PAY,			//非接扫码支付(云卓二维码消费)
#endif     /* -----  CODE_SCANNER_SOCKET  ----- */
#ifdef  GONG_JIAO_MANAGE_SYS
	TYPE_DOWN_PARA,							//参数下载
	TYPE_DOWN_WHITE_LIST,					//白名单下载
	TYPE_GET_RESOURFILE_LIST,				//获取资源文件列表
	TYPE_DOWN_RESOURFILE,					//资源文件下载
	TYPE_DRIVER_LOGIN_LOGOUT,				//司机签到、签退
	TYPE_QR_BATCHUP,						//二维码批上送
	TYPE_TRANSP_CARD_BATCHUP,				//交通卡批上送
	TYPE_BANK_BATCHUP,						//银行卡批上送
	TYPE_ODA_READCARD_BATCHUP,				//银行卡oda批上送
	TYPE_ODA_UPLOAD_RET_BATCHUP,			//银行卡oda上送结果批上送
#if (defined CFG_ECASH) && (defined CFG_ECASH_UPLOAD_TO_MAMNAGE_SYS)
	TYPE_ECASH_READCARD_BATCHUP,			//银行卡电子现金交易（脱机扣款成功）批上送
	TYPE_ECASH_UPLOAD_RET_BATCHUP,			//银行卡电子现金交易上送结果批上送
#endif
#endif     /* -----  GONG_JIAO_MANAGE_SYS  ----- */
#ifdef  CFG_YINSHANG_ERP
	HB_TRANDE_TYPE_LOGIN,
	HB_TRANDE_TYPE_PAY,
	HB_TRANDE_TYPE_QUERY,
	HB_TRANDE_TYPE_AUTOVOID,
	HB_TRANDE_TYPE_MEM_LOAD,
#endif     /* -----  GONG_JIAO_MANAGE_SYS  ----- */
#ifdef CFG_QUERY_WHEN_TIMEOUT_POS_TONG
	TYPE_QUERY_STATUS_POSTONG				//B扫C模式POS交易状态查询
#endif

} TRANS_TYPE;

#define FINANCE_PRODUCT_PAYPASSONLY             0
#define FINANCE_PRODUCT_PAYPASS_CARDREADER      1
#define FINANCE_PRODUCT_ALL_FUNCTION            2


#define     MAC_NO                  0                   /* 没有MAC校验                  */
#define     MAC_NORMAL              2                   /* 普通MAC校验,全部异或后再DES  */
#define     MAC_X99                 1                   /* 标准的MAC校验                */
#define     MAC_ECB                 3                   /* 银联的MAC校验                */
#define     MAC_X919                4                   /* MAC校验                */


#define EM_VOIDISO_LEN			sizeof(ISODataStru)


#define EM_SHOW_MASK       (uchar)1
#define EM_SHOW_NOMASK     (uchar)0


#ifndef FALSE
#define FALSE (1 == 0)
#endif

#ifndef TRUE
#define TRUE  (1 == 1)
#endif

//退出返回值
#define EM_EXITMODE_ENTER      (uchar)0x00
#define EM_EXITMODE_TIMEOUT    (uchar)0x8A
#define EM_EXITMODE_CIDRING    (uchar)0xF0
#define EM_EXITMODE_PAY        (uchar)0xF2
#define EM_EXITMODE_QUIT       (uchar)0xF3
#define EM_EXITMODE_HF         (uchar)0xF4
#define EM_EXITMODE_LEFT       (uchar)0xF5
#define EM_EXITMODE_RIGHT      (uchar)0xF6
#define EM_EXITMODE_FUNCTION   (uchar)0xF7
#define EM_EXITMODE_DEL        (uchar)0xF8
#define EM_EXITMODE_HOOKON     (uchar)0xFE
#define EM_EXITMODE_CANCEL     (uchar)0xFF
#define EM_EXITMODE_STAR       EM_EXITMODE_CANCEL
#define EM_EXITMODE_JING       EM_EXITMODE_ENTER


//平台返回处理要求类型
enum
{
	PROCESS_TYPE_NONE						= 0,	//无
	PROCESS_TYPE_DOWNLOAD_PARA				= 1,	//下载磁卡参数
	PROCESS_TYPE_UPLOAD_STATE				= 2,	//上传终端磁条卡状态信息
	PROCESS_TYPE_LONGIN						= 3,	//签到
	PROCESS_TYPE_DOWNLOAD_PUBKEY			= 4,	//下载IC卡公钥
	PROCESS_TYPE_DOWNLOAD_EMVPARA			= 5,	//下载IC卡参数
	PROCESS_TYPE_DOWNLOAD_QPSPARA			= 9,	//下载小额非接参数
	PROCESS_TYPE_DOWNLOAD_QPSPARA_BIN		= 0x0b, //下载小额非接卡bin新增白名单
	PROCESS_TYPE_DOWNLOAD_QPSPARA_BIN_BL	= 0x0c, //下载小额非接卡bin黑名单
	PROCESS_TYPE_UPLOAD_TERM_CONFIG			= 0x0d, //终端配置上送
};

#ifdef __cplusplus
}
#endif
#endif




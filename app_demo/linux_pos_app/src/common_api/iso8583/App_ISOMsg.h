
#ifndef __ISOMSG_H__
#define __ISOMSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MSG_LEN             3000
#define MAX_FIELD_NUM           128
#define MAX_FIELD_LEN           999


/* ISO8583数据域的定义 */
#define    MSG_ID                               0               /* 信息码                   */
#define    PAN                                  2               /* 主帐号                   */
#define    PROC_CODE                            3               /* 处理码                   */
#define    AMOUNT                               4               /* 交易金额                 */
#define    SETTLE_AMOUNT                        5               /* 清算金额                 */
#define    FEE_AMOUNT                           6               /* 小费金额                 */
#define    LOCAL_TIME                           7               /* 终端时间                 */
#define    BATCH_NO                             9               /* 批次号                   */
#define    TRACE_NO                             11              /* 系统跟踪号               */
#define    TRADE_TIME                           12              /* 交易时间                 */
#define    TRADE_DATE                           13              /* 交易日期                 */
#define    EXPIRY                               14              /* 有效期                   */
#define    PRETRADE_DATE                        15              /* 原交易日期               */
#define    LOCATION_FLAG                        16              /* 场所标志                 */
#define    VERSION_NO                           21              /* 版本号                   */
#define    SERVE_MODE                           22              /* 进入方式                 */   

#define    TRACK2                               35              /* 二磁道信息               */
#define    TRACK3                               36              /* 三磁道信息               */
#define    RETR_NUM                             37              /* 检索参考号               */
#define    AUTH_ID                              38              /* 授权号                   */
#define    RET_CODE                             39              /* 响应代码                 */
#define    MACHINE_ID                           40              /* 机具号                   */
#define    POS_ID                               41              /* 终端号                   */
#define    CUSTOM_ID                            42              /* 商户号                   */ 
#define    ADDI_RESPONSE                        44              /* 附加响应代码             */
#define    ADDI_DATA                            48              /* 附加数据                 */    
#define    MONEYID                              49              /* 货币代码                 */
#define    SETTLE_MONEYID                       50              /* 清算货币代码             */
#define    PIN                                  52              /* 个人PIN                  */
#define    ADDI_AMOUNT                          54              /* 附加金额                 */
#define    ADVERTISEMENT                        57              /* 广告信息 / 中奖信息      */
#define    ERROR_INFO                           58              /* 错误信息                 */
#define    MERCHANT_INFO                        59              /* 商户信息                 */
#define    SECRET_KEY                           62              /* 密钥                     */
#define    MAC                                  64              /* MAC                      */


typedef enum ALIGN_TYPE
{
	ALN_LEFT   = 0,
	ALN_RIGHT  = 1,
} ALIGN_TYPE;

/* 长度类型 */
typedef enum LENGTH_TYPE
{
	LEN_FIX	   = 0,
	LEN_LLVAR  = 1,
	LEN_LLLVAR = 2,
} LENGTH_TYPE;


typedef struct ISO_FIELD ISO_FIELD;
typedef struct ISO_FIELD_DEF ISO_FIELD_DEF;

/*
 * 函数功能：对8583域进行打包，并返回打包后的数据和长度
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
 */
typedef int (*FIELD_PACKAGER_PACK) (ISO_FIELD_DEF *pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);

/*
 * 函数功能：将缓冲区里面的数据解包到8583域中
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
 */
typedef int (*FIELD_PACKAGER_UNPACK) (ISO_FIELD_DEF *pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);


/* ISO 8583 数据域定义 */
struct ISO_FIELD_DEF
{
	short                   len;        /* 数据域最大长度 */
	LENGTH_TYPE				lentype;    /* 数据域长度类型: 1--LLVAR型 2--LLLVAR型 */
    ALIGN_TYPE              align;      /* 对齐类型 */
    char                    padchar;    /* 补齐字符 */
	FIELD_PACKAGER_PACK		pack;		/* 打包函数指针 */
	FIELD_PACKAGER_UNPACK	unpack;     /* 解包函数指针 */
};

/* ISO 8583 数据域结构 */
struct ISO_FIELD
{
    short   len;
    short   off;  /* 指向iso->databuf缓冲区的偏移，数据中不含长度类型的几个字节 */
};

/* ISO 8583 报文结构定义 */
typedef struct ISODataStru
{
	ISO_FIELD	fld[129];
	char		bitmap[129];
	unsigned char databuf[MAX_MSG_LEN];
	short       used;
} ISODataStru;



//////////////////////////////////////////
// Inner help functions
int AddPadChar(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata, unsigned char head);

//////////////////////////////////////////
// 以下为变长域长度计算函数
// ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
void FLALengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void FLALengthGet(ISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

// BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节
void FLBLengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void FLBLengthGet(ISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

// 十六进制形式，如长度104，将表示为 0x00, 0x68,共2个字节
void FLHLengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void FLHLengthGet(ISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

// 变长域长度计算
typedef enum __FLLENGTH_MODE_ENUM {
    FLLENGTH_MODE_ASC = 0 ,                     /* ASC */
    FLLENGTH_MODE_BCD,                          /* BCD */
    FLLENGTH_MODE_HEX,                          /* HEX */
}FLLENGTH_MODE_ENUM;				/* ----------  end of enum FLLENGTH_MODE_ENUM  ---------- */

//////////////////////////////////////////
// 打包、解包函数实现
int FPAMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPAMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPBMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPBMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPABitmapPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPABitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPBBitmapPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPBBitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPACharPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPACharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPBCharPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPBCharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPADigitPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPADigitUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPBDigitPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPBDigitUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPBAmountPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPBAmountUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
int FPBBinaryPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
int FPBBinaryUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);

//////////////////////////////////////////
// Iso8583 操作函数
int PubStrToIso(const ISO_FIELD_DEF Iso8583Def[],char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition);
int PubIsoToStr(const ISO_FIELD_DEF Iso8583Def[],char *pcTargetStr, ISODataStru *SourceIsoStru, int *pnLen);
int PubGetBit(ISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen);
int PubSetBit(const ISO_FIELD_DEF Iso8583Def[],ISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen);
int PubDelAllBit(ISODataStru *pIsoStru);
int PubDelOneBit(ISODataStru *pIsoStru, int nPosition);
int PubIsoToStr_separate(char *pcTargetStr, ISODataStru *pSourceIsoStru, int *pnLen);
int PubStrToIso_separate(char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition);
int PubSetBit_separate( ISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen);
int PubStrToIso_special_ums_oda(const ISO_FIELD_DEF Iso8583Def[],char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition);
/* extern Iso8583Def */
extern const ISO_FIELD_DEF Iso8583Def_pos_tong[]; 
extern const ISO_FIELD_DEF Iso8583Def_union_pay[]; 
extern const ISO_FIELD_DEF Iso8583Def_bus_manage[]; 
extern const ISO_FIELD_DEF Iso8583Def_oda[]; 

#ifdef __cplusplus
}
#endif

#endif // __ISOMSG_H__

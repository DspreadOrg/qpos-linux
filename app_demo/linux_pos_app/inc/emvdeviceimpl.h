#ifndef	_EMV_DEVICE_IMPL_H
#define	_EMV_DEVICE_IMPL_H

#ifdef __cplusplus
extern "C"
{
#endif

//EMV参数tag定义
#define EMVTAG_APP_PAN      0x5A
#define EMVTAG_APP_EXPDATA  0x5F24
#define EMVTAG_APP_PAN_SN   0x5F34
#define EMVTAG_TRACK2     0x57
#define EMVTAG_AC       0x9F26
#define EMVTAG_CID        0x9F27
#define EMVTAG_IAD        0x9F10    //Issuer Application Data
#define EMVTAG_RND_NUM      0x9F37    //Random Number
#define EMVTAG_ATC        0x9F36
#define EMVTAG_TVR        0x95
#define EMVTAG_TXN_DATE     0x9A
#define EMVTAG_TXN_TIME			0x9F21
#define EMVTAG_TXN_TYPE     0x9C
#define EMVTAG_AMOUNT     0x9F02
#define EMVTAG_CURRENCY     0x5F2A
#define EMVTAG_AIP        0x82
#define EMVTAG_COUNTRY_CODE   0x9F1A
#define EMVTAG_OTHER_AMOUNT   0x9F03
#define EMVTAG_TERM_CAP     0x9F33
#define EMVTAG_CVM        0x9F34
#define EMVTAG_TERM_TYPE    0x9F35
#define EMVTAG_IFD        0x9F1E
#define EMVTAG_DF       0x84
#define EMVTAG_APP_VER      0x9F09
#define EMVTAG_TXN_SN     0x9F41
#define EMVTAG_CARD_ID      0x9F63
#define EMVTAG_AID        0x4F
#define EMVTAG_SCRIPT_RESULT  0xDF31
#define EMVTAG_ARC        0x8A
#define EMVTAG_ISS_COUNTRY_CODE 0x5F28
#define EMVTAG_EC_AUTH_CODE   0x9F74
#define EMVTAG_EC_BALANCE   0x9F79
#define EMVTAG_TSI        0x9B
#define EMVTAG_APP_LABEL    0x50
#define EMVTAG_APP_NAME     0x9F12
#define EMVTAG_TRACK3         0x58
#define EMVTAG_AFL              0x94
#define EMVTAG_RFID_OFFLINE_BALANCE 0x9F5D
#define EMVTAG_PAN_SFI_RECORDNO 0xBC
#define EMVTAG_TRACK2_RECORDNO 0xBE

/*终端配置参数*/
typedef  struct  _EmvTermConfig_t
{
	unsigned char _type;            /* 9F35(Terminal), n2, 1 */
	unsigned char _cap[3];          /* 9F33(Terminal), b,  3 */
	unsigned char _add_cap[5];      /* 9F40(Terminal), b,  5 */
	unsigned char _ics[8];				/*	ICS	*/
	unsigned char _ifd_serial_num[8];       /* 9F1E(Terminal), an8, 8 bytes */
	unsigned char _term_country_code[2];    /* 9F1A(Terminal), n3, 2 bytes */
	unsigned char _terminal_id[8];          /* 9F1C(Terminal), an8, 8 bytes */
	unsigned char _trans_curr_code[2];      /* 5F2A(Terminal), n3, 2 bytes */
	unsigned char _trans_curr_exp;          /* 5F36(Terminal), n1, 1 bytes */
	unsigned char _merchant_name[20];		/* 9F4E商户名称*/
}EmvTermConfig_t;

/*输出的AID侯选列表结构体*/
typedef struct _AidCandidate_t
{
	unsigned char _aid[16];		/*	AID	应用标识符*/
	unsigned char _aid_len;		/*	length of AID*/
	unsigned char _lable[20];           /* 应用标签
								50(ICC), ans, 1-16 bytes, we prepare 20 for some PBOC2 errors*/
	unsigned char _lable_len;		/*	length of lable*/
	unsigned char _preferred_name[20];  /* 应用优先名9F12(ICC), ans, 1-16 bytes */
	unsigned char _preferred_name_len;	/*	length of preferred name*/
	unsigned char _priority;            /* 应用优先权标识符87(ICC), b, 1 bytes */
	unsigned char  _enable;              /* indicate whether the candidate is enabled 
									0	去除
									1	可用*/
	unsigned char _resv[3];		/*	reserve bytes	*/
	int   _file_offset;         		/* the offset of this AID	in the parameters file */
}AidCandidate_t;

typedef  struct  _EmvOnlineData_t
{
	char iccResponse[3];            /* 39域 */
	unsigned char ackdata[1024];       /* 55域 */
	int ackdatalen;      				/* 55域长度 */
}EmvOnlineData_t;



#define MAX_EC_RECORD_COUNT 10
//交易日期		9A				3
//交易时间		9F21			3
//授权金额		9F02			6
//其它金额		9F03			6
//终端国家代码	9F1A			2
//交易货币代码	5F2A			2
//商户名称		9F4E			20
//交易类型		9C				1
//应用交易计数器（ATC） 9F36	2
typedef struct _EcTxnLog_t
{
	unsigned  char Date[3];
	unsigned  char Time[3];
	unsigned  char AuthAmout[6];
	unsigned  char OtherAmount[6];
	unsigned  char TerminalCountryCode[2];
	unsigned  char CurrencyCode[2];
	unsigned  char MerchantName[20];
	unsigned  char TxnType;
	unsigned  char ATC[2];
}EcTxnLog;

typedef struct  
{
  EcTxnLog Logs[MAX_EC_RECORD_COUNT];
  int Count;
}EcTxnLogs;

typedef struct _EmvCallBack_t
{	
	//多应用选择 返回值>=0 返回选择的AID序列号 -1失败
	int (*EMV_AidSelect)(AidCandidate_t *pList, int listNum);
	//卡号确认 返回值0确认 -1失败
	int (*EMV_ConfirmCardInfo)(char *pan,int len);
	//请求pin cType：1脱机密码 2最后一次脱机密码 3联机密码  返回值-1输入失败  -2未输入BYPASS  -3中止交易和Timeout  >0输入密码长度
	int (*EMV_InputPasswd)(int cType, char *pszPin);
	//身份确认 type 取值0-5,分别为:身份证/军官证/护照/入境证/临时身份证/其他 pcon：证件号；返回值1	执卡人身份确认  0身份确认失败
	int (*EMV_CertConfirm)(unsigned char type, unsigned char *pcon, unsigned char len);
	//联机  result:联机结果-1 联机失败 >=0   联机成功(有报文应答)
	int (*EMV_OnlineProcess)(EmvOnlineData_t* pOnlineData);
}EmvCallBack_t;


typedef	enum
{
	SIMPLE = 0,  //简易流程 只读卡
	PBOC,    
	QPBOC, 
}EmvTransFlow;

typedef enum
{
	CONTACT_ICC = 0,
	CONTACTLESS_ICC
}IccType;

//内核交易类型
typedef enum
{
  EMV_L2_QUERY = 0,
  EMV_L2_SALE,
  EMV_L2_AUTH,
  EMV_L2_EC_CASH_LOAD,
  EMV_L2_EC_DESIGNATED_ACCOUNT_LOAD,
  EMV_L2_EC_UNDESIGNATED_ACCOUNT_LOAD,
  EMV_L2_EC_CASH_LOAD_VOID,
  EMV_L2_EC_OFFLINE_BALANCE,
  EMV_L2_SALE_LOG,
  EMV_L2_EC_LOAD_ONE_BY_ONE_LOG,
  EMV_L2_EC_LOAD_ALL_IN_ONE_LOG,
  EMV_L2_SALE_VOID,
  EMV_L2_SALE_REFUND,
}EMVTransType;

typedef struct _EmvTransParams_t
{
	IccType icc_type;					/*刷卡类型*/
	EMVTransType trans_type;			/*EMV交易类型*/
	unsigned char trans_amount[12+1];	/*交易金额*/
	unsigned char trans_time[7+1];		/*交易时间*/
	EmvTransFlow trans_flow;			/*交易流程*/
	unsigned char ec_support;			/*是否支持电子现金*/
	unsigned char sm_support;			/*是否支持国密*/
	unsigned char force_online_enable;	/*是否强制联机*/
	unsigned char tsc[4+1];				/*IC交易流水号 4字节bcd */
}EmvTransParams_t;

//emv交易错误码
typedef enum
{
    EMV_L2_ERROR_OK = 0,
    EMV_L2_ERROR_NO_AID_MATCHED,
    EMV_L2_ERROR_QPBOC_READ_LAST_RECORD_FAILED,
    EMV_L2_ERROR_APPLLICATION_BLOCK,
    EMV_L2_ERROR_CARD_BLOCK,
	EMV_L2_ERROR_RFID_EXPIREDAID,
	EMV_L2_ERROR_RFID_LASTRECORDMISS,

    EMV_L2_ERROR_PURE_EC_CARD_NOT_SUPPORT_THIS_TXNTYPE,
    EMV_L2_ERROR_PURE_EC_CARD_NOT_ENOUGH_BALANCE,
    EMV_L2_ERROR_CARD_ACCEPT_WHILE_ARPC_AUTH_FAILED,
    EMV_L2_ERROR_USE_ANOTHER_INTERFACE,
    EMV_L2_INTERFACE_ISSUER_ACCEPT_CARD_AAC,
    EMV_L2_INTERFACE_CARD_ACCEPT_BUT_IN_BLACKLIST,
    EMV_L2_INTERFACE_CARD_ACCEPT_BUT_APPLICATION_EXPIRED,
}EMV_L2_Error;

//emv交易返回值
typedef enum
{
  EMV_L2_CONTINUE =  0,
  EMV_L2_TERMINATE = -1,
  EMV_L2_DENY = 2,
  EMV_L2_ACCEPT = 1,
  EMV_L2_FALLBACK = -2,
  EMV_L2_GO_ONLINE = 3,
  EMV_L2_REMOVE_CARD,
  EMV_L2_FALSF_CARD, //闪卡
  EMV_L2_GO_QPS, //走非接快速业务
  EMV_L2_GO_PBOC, //非接交易过程插卡，退出非接流程走插卡流程
  EMV_L2_GO_MAG,//非接交易过程刷卡，退出非接流程走磁条卡流程
}EMV_L2_Return;

int Emv_KernelInit(const EmvTermConfig_t emvTermCfg, EmvCallBack_t t_callbackfun);

EMV_L2_Return Emv_Process(EmvTransParams_t emvTransParams);

unsigned char* Emv_GetCoreData(unsigned int tagname, int *pvallen);

int Emv_SetCoreData(unsigned int tagname, unsigned char *pvalue, int valuelen);

//获取tagname[]里的一系列TLV数据,返回的数据格式为TLV
int Emv_FetchData(unsigned int* tagname, int count, unsigned char* obuf, int olen);

int Emv_ClrCAPKFile();

int	Emv_ClrAIDFile();

int Emv_PARAM_InputCAPKData(unsigned char *ptlvstrin, int tlvlenin);

int Emv_PARAM_InputAIDData(unsigned char *ptlvstrin, int tlvlenin);

int	Emv_GetAidTotalNum();

int	Emv_GetCapkTotalNum();

int Emv_Get_ECbalance(unsigned char *pout, int *outlen);

int Emv_GetEcTxnLog(EcTxnLogs* theLogs);

int Emv_ClearICTxnLog(void);
#ifdef __cplusplus
}
#endif
#endif


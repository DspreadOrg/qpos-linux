/*
 * app_emv.h
 */

#ifndef _APP_EMV_H_
#define _APP_EMV_H_

#include "appinc.h"

#define TAG_CONTACTLESS_REFUND_PROCESS                  0xDF8302

#define PED_PIN_IPEK_INDEX 0x01
#define PED_EMV_IPEK_INDEX 0x02
#define PED_TRK_IPEK_INDEX 0x03


#define			EMVERROR_SUCCESS				0	/*!< Success */
#define			EMVERROR_FUNCNOTSUPPORT			1	/*!< Application not supported */
#define			EMVERROR_CARDBLOCKED			2	/*!< Card is blocked,SW=6A81 */
#define			EMVERROR_PSENOTFIND				3	/*!< no pse path */
#define			EMVERROR_PSEBLOCKED				4	/*!< pse is blocked */
#define         EMVERROR_ICCCOMAND				5	/*!< IC卡命令失败 */
#define			EMVERROR_ICCDATA				6	/*!< IC卡返回数据错误 */
#define			EMVERROR_PARAM					7	/*!< 参数错误 */
#define			EMVERROR_MISSDATA				8	/*!< 数据缺失 */
#define			EMVERROR_RESELECT				9	/*!<  AID重选 内部使用 */
#define			EMVERROR_DATA					10	/*!<  数据错误 */
#define			EMVERROR_DATAAUTH				11	/*!<  认证错误,内部使用*/
#define			EMVERROR_CAPKNOTFOUND			12	/*!<  CAPK公钥缺失 */
#define			EMVERROR_CERTIFICATIONMISS   	13	/*!< 缺失数据证书 */
#define			EMVERROR_CERTIFICATIONLEN		14	/*!<  数据证书长度错误 */
#define			EMVERROR_HASHVALUE				15	/*!< Hash错误 */
#define			EMVERROR_CAPKINDEXNOTFOUND   	16	/*!< CAPK公钥索引错误 */
#define			EMVERROR_CANCEL					17	/*!< 取消 */
#define			EMVERROR_NEEDARQC				18	/*!< 需要联机 */
#define			EMVERROR_EXTERNALAUTH			19	/*!< 外部认证失败 \todo 删除*/
#define			EMVERROR_ISSUREDATA				20	/*!< 发卡行认证错误 */
#define			EMVERROR_OPENCOM				21	/*!< 端口打开失败 */
#define			EMVERROR_SENDDATA				22	/*!< 发送失败 */
#define			EMVERROR_RECEIVEDATA			23	/*!< 接受失败 */
#define			EMVERROR_TAGNOTFIND				24	/*!< TAG缺失 */
#define			EMVERROR_NOSUPPORTAPP			25	/*!< 无支持的应用 */
#define			EMVERROR_IPKREVOKE				26	/*!< 公钥已回收 */
#define			EMVERROR_PARAMUPDATE			27	/*!< 参数更新错误 */
#define			EMVERROR_VELOCITYCHECK			28	/*!< 频率检查失败, \todo 删除 */
#define			EMVERROR_VERIFY					29	/*!< cardholder verification error */
#define			EMVERROR_SAVEFILE				30	/*!< 文件保存失败 */
#define			EMVERROR_READFILE				31	/*!< 文件读取失败 */
#define			EMVERROR_WRITE					32	/*!< \todo 删除 */
#define			EMVERROR_INVALIDSERVICE			33	/*!< 无效服务 */
#define			EMVERROR_TIMEOUT				34	/*!< 超时 */
#define			EMVERROR_SCRIPTLIMIT			35	/*!< 脚本超限,\todo 删除 */
#define			EMVERROR_CAPKINVALID			36	/*!< 公钥无效,\todo 删除 */
#define			EMVERROR_OVERFLOW				37	/*!< 数据溢出 */
#define			EMVERROR_CID					38	/*!< CID异常 */
#define			EMVERROR_DATE					39	/*!< 日期错误 */
#define			EMVERROR_RESPFORMAT				40	/*!< icc response has none mandatory tag or data */
#define			EMVERROR_BYPASS					41	/*!< input is bypassed */
#define			EMVERROR_FALLBACK				42	/*!< fallback */
#define			EMVERROR_AIDBLOCKED				43	/*!< aid is blocked */
#define			EMVERROR_ISSUERREFERAL			44	/*!< issuer reference fail */
#define			EMVERROR_APPBLOCKED				45	/*!< appication is blocked */
#define			EMVERROR_NORECORD				46	/*!< no icc log */
#define			EMVERROR_NOLOGENTRY				47	/*!< no entry of icc log */
#define			EMVERROR_DUPLICATETAG			48	/*!< has dualicate tag */
#define			EMVERROR_GPORSPONSE				49	/*!< gpo return error */
#define			EMVERROR_ICCRESET				50	/*!< IC卡复位失败 */
#define			EMVERROR_DENAIL					51	/*!< decline the transaction */
#define			EMVERROR_FDDAFAIL				52	/*!< fdda auth error */
#define			EMVERROR_LASTRECORD				53	/*!< last record not correctly responsed */
#define 		EMVERROR_NOPINPAD				54	/*!< no pinpad */ //
#define			EMVERROR_NOPIN					55	/*!< 没有密码或用户忽略了密码输入 */
#define			EMVERROR_TIMEOUT_GAC2		    56	/*!< GAC网络超时 */


/***********  CONTACTLESS ERROR ************/
#define			EMVERROR_CHANGEINTERFACE		60	/*!< 使用其他界面 */
#define			EMVERROR_COLLISION				61	/*!< 询卡冲突(非接) */
#define			EMVERROR_TRANSOVER				62	/*!< 交易结束 */
#define			EMVERROR_EXPDATE				63	/*!< card is expired */
#define			EMVERROR_SDA					64	/*!< SDA认证失败 */
#define			EMVERROR_OTHERAPP				65	/*!< 选择其他应用 */
#define			EMVERROR_NOTALLOWEDSERVICE		67	/*!< 服务不允许 */
#define			EMVERROR_BLACKLIST				69	/*!< 卡片已列入黑名单 */
#define			EMVERROR_VERSIONDIFF			70
#define			EMVERROR_TRANSEXCEEDED  		71	/*!< 非接交易超限 */
#define			EMVERROR_PINBLOCKED				72
#define			EMVERROR_TAGLEN					73 // TLV解析错误
#define			EMVERROR_COLLSION				80 /*!< 询卡冲突 */
#define			EMVERROR_CMD_REST				81	/*!<  复位指令 */

#define  DspOsLog(LIB_lv, ...) {OsLog(LOG_ERROR, "Dspread: ");OsLog(LOG_ERROR, ##__VA_ARGS__);} 

typedef enum
{
	APP_RC_SUCCESS = 0,
	APP_RC_FAIL = 1,

	APP_RC_TIMEOUT = 0x10000001,
	APP_RC_RESET,
	APP_RC_CANCEL,
	APP_RC_COMPLETED,
	APP_RC_TERMINAL,
	APP_RC_NFC_TERMINAL,

	APP_RC_MAG_READY,
	APP_RC_ICC_READY,
	APP_RC_NFC_READY,
	APP_RC_FALL_BACK,
	APP_RC_PSAM_READY,

	APP_RC_OTHER,
	APP_RC_RESET2,
	APP_RC_EMV_DENAIL,
	APP_RC_NFC_MULTI_CARD,
	APP_RC_EMV_GAC2_DENAIL,
	APP_RC_EMV_GAC2_NULL,//第二次gac失败与检查网络冲突情况，联网失败不显示cmd timeout
	APP_RC_NFC_NOT_ALLOW,//不允许交易
	APP_RC_EMV_APP_BLOCK,//不允许交易
	APP_RC_EMV_APP_SEE_PHONE,
	APP_RC_EMV_TRANS_TRY_ANOTHER_INTERFACE,
	APP_RC_EMV_TRANS_GPO_NOT_SUPPORT,
	APP_RC_EMV_REVERSAL,//冲正
	APP_RC_EMV_CARD_BLOCK,

	APP_RC_NFC_DOUBLETAP_DENAIL,
	APP_RC_CARD_NOT_SUPPORT,
	APP_RC_NFC_TRY_AGAIN,

	APP_RC_NFC_SUCCESS,
	APP_RC_TRANS_REVERSE,
	APP_RC_MAG_TRADE_FAIL,
	APP_RC_MAG_COMPLETED,

	APP_RC_NUMS,
}App_Rc_Code_t;

typedef	enum
{
	OFFLINEAPPROVED,
	OFFLINEDECLINED,
	ONLINEAPPROVED,
	ONLINEDECLINED,
	UNABLEONLINE_OFFLINEAPPROVED,
	UNABLEONINE_OFFLINEDECLINED
}TRANSRESULT;

typedef enum
{
    TRANS_CARD_MAG=0x01,
    TRANS_CARD_ICC,
    TRANS_CARD_CTLS
}TRANS_CARD_T;

#define INPUT_STRIPE   (1<<0)  //刷卡
#define INPUT_INSERTIC (1<<1)  //插卡
#define INPUT_RFCARD   (1<<2)  //挥卡

typedef struct tagCardDataInfo
{
	ST_MSR_DATA ptTracker1;
	ST_MSR_DATA ptTracker2;
	ST_MSR_DATA ptTracker3;
	PR_INT8 CardSN[3+1];
	PR_INT8 Pan[19+1+1];		//主账号
	PR_INT8 ExpDate[4+1];
    PR_INT8 ServiceCode[3+1];
    PR_INT8 CardCompany[3+1];	//信用卡公司

}CardDataInfo;

typedef	enum
{
	PROCESS_STATUS_INIT = 0,
	KERNEL_PROCESS_STATUS_NFC_RETAP,

}EMV_KERNEL_PROCESS_STATUS;



extern int app_emv_cbk(void* user_data, unsigned char* buf, unsigned int len);

extern u32 App_Emv_Process(int nRetSwipeType,unsigned char *pTradFile);
extern u32 App_Mag_Process(CardDataInfo *pCardDataInfo, unsigned char *pTradFile);

extern bool get_Pin_Input_Flag();
extern void set_Lvgl_Kbd_Flag(bool flag);

extern void set_KernelInsLib_mode(int mode);
#endif 
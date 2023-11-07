#ifndef _ERRNO_H__
#define _ERRNO_H__ 


#define   SOCK_ERR_SUCCESS                   0

typedef enum __SOCKET_ERR_STEP_ENUM {
    SOCK_ERR_STEP_PARA = 1,           
    SOCK_ERR_STEP_GETHOSTBYNAME,
    SOCK_ERR_STEP_CREATESOCKET,
    SOCK_ERR_STEP_CONNECT,  
    SOCK_ERR_STEP_SEND,
    SOCK_ERR_STEP_RECV,
	SOCK_ERR_STEP_PARSE,

    SOCK_ERR_STEP_SSL_INIT = 50,  
    SOCK_ERR_STEP_SSL_CONNECT,  
    SOCK_ERR_STEP_SSL_SEND ,
    SOCK_ERR_STEP_SSL_RECV,
    SOCK_ERR_STEP_SELECT,  

    SOCK_ERR_CURL_PARA = 100,
    SOCK_ERR_CURL_INIT,
    SOCK_ERR_CURL_PERFORM,
    SOCK_ERR_MAX = 999,
}SOCKET_ERR_STEP_ENUM;				/* ----------  end of enum SOCKET_ERR_STEP_ENUM  ---------- */

typedef enum
    {
        MISPOS_E_SUCCESS = 0,                   // 无错误
        MISPOS_E_CHECK,					        //校验密钥错
        MISPOS_E_NEED_SIGNIN,			        //需签到
        MISPOS_E_REFUSE,				        //卡片拒绝
        MISPOS_E_UNKNOW,				        //交易异常
        MISPOS_E_SEND,					        //发送失败
        MISPOS_E_RECV,					        //接收失败
        MISPOS_E_PARSE,					        //接收错误
        MISPOS_E_LACK_BALANCE,                  //余额不足
        MISPOS_E_ISO8583ERR,		            //8583打包错误
        MISPOS_E_CONNECTERR,		            //网络连接错误
        MISPOS_E_READCARD_ERR,			        //读卡失败
        MISPOS_E_PARAM_ERR,				        //参数错
        MISPOS_E_BLACK_CARD,		            //黑名单卡
        MISPOS_E_CARD_TYPE,			            //仅支持信用卡
        MISPOS_E_OFFLINE_AUTH,		            //脱机认证失败
        MISPOS_E_FILE_FREE,                     //存储满
        MISPOS_E_INVALID_CODE,                  //无效码
        MISPOS_E_ERR_CARD,                      //无效卡
        MISPOS_E_WRITE_BIN,	                    //白名单卡bin
        MISPOS_E_AMOUNT_ERROR,                  //金额异常
        MIS_E_QR_CODE_OVERDUE,                  //二维码过期
        MIS_E_QR_CODE_REPEAT,                   //重复扫码
        MISPOS_E_QR_CODE_ERR_SERVICE_ID,        //业务标识不匹配(乘车码的行业使用范围与终端业务标识不匹配)
        MISPOS_E_INVALID_APPAUTH,               //乘车码的应用机构号未授权
        MISPOS_E_APPAUTH_OVERTIME,              //乘车码的应用机构号授权过期
        MISPOS_E_SAVE_RECOCILIATION_DETAIL,     //保存对账记录异常
        MISPOS_E_NO_CA,                         //未下载乘车码公钥
        MISPOS_E_NO_APPAUTH,                    //未下载乘车码收钱应用机构号
        MISPOS_E_CA_OVERTIME,                   //发码机构公钥证书已过期
        MISPOS_E_NOFINDED_CA,                   //未找到匹配的乘车码公钥
        MISPOS_E_SIGNATURE_FAIL,                //乘车码验签失败
        MISPOS_E_VERSION_EXP,                   //乘车码版本异常
        MISPOS_E_NO_BUS_CUSTOMER,               //乘车码商户未配置
        MISPOS_E_MEMSPACE_APPFLY_AIL,           // 内存空间申请失败
        MISPOS_E_NON_INTRP_CARD,	            //非互通城市卡
        MISPOS_E_ERR_CARD_TYPE,	                //不支持的卡类型
        MISPOS_E_APP_DISABLED,	                //应用未启用
        MISPOS_E_NONSUP_DISC,	                //互通城市卡不支持优惠
        MISPOS_E_NOT_IN_ORG_CODE,	            //不在发卡机构白名单中
        MISPOS_E_DATE_ERR,		                //终端时间异常
        MISPOS_E_OUT_APP_VALIDDATE,	            //应用已过有效期
        MISPOS_E_OVER_TIMES,	                //刷卡次数超限
        MISPOS_E_OUT_RESWAP_TIME,	            //请勿连续刷卡
        MISPOS_E_PSAM_ERROR,		            //PSAM卡错误
        MISPOS_E_GPS_ERROR,                     //等待GPS定位，暂停刷卡
		MISPOS_E_DOWNLOAD_IC_PARA_ERR,			//下载IC卡参数失败
		MISPOS_E_ONLY_DRIVER_CARD,				//请刷司机卡
		MISPOS_E_SELECT_FAIL,					//应用选择失败
		MISPOS_E_READ_SETTING_CARD,				//请刷设置卡
		MISPOS_E_ALREADY_STOP_USE,				//已停用
		MISPOS_E_TEMP_STOP_USE,					//暂不能使用
		MISPOS_E_FARE_PARA,						//票价参数异常
		MISPOS_E_BALANCE,						//余额异常
		MISPOS_E_TRADE,							//交易异常
		MISPOS_E_SHAN_CARD,						//发送闪卡请重刷
		MISPOS_E_DISABLE_USE,					//限制使用
		MISPOS_E_LOSS_EFFICACY_CARD,			//卡失效
		MISPOS_E_FOREIGN_CARD,					//非法卡
		MISPOS_E_LOCK_CARD,						//挂失锁卡
		MISPOS_E_UN_RECORD_CARD,				//未登记,无效卡
		MISPOS_E_RETRY,							//请重刷
		MISPOS_E_QING_TOU_BI,					//请投币
        MISPOS_E_UNION,                         //注意别超过256了
    } MISPOS_ERRCODE;

typedef struct __socket_errno_map_t {
    unsigned short step;
    char message[126];
}socket_errno_map_t;				/* ----------  end of struct socket_errno_map_t  ---------- */

int err_sock_errno(unsigned short step, int err);
int err_sock_errno_mis_error_code(int errCode);
#endif

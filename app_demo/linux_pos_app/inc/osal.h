#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <Stdarg.h>

#define TRUE 1
#define FALSE 0

#define RET_OK 0  //成功
#define ERR_INVALID_HANDLE -1000 //非法句柄
#define ERR_TIME_OUT -1001 //超时
#define ERR_APP_NOT_EXIST -1002 //应用不存在
#define ERR_INVALID_PARAM -1003 //非法参数
#define ERR_DEV_NOT_EXIST -1004 //设备不存在
#define ERR_DEV_BUSY -1005 //设备被占用
#define ERR_DEV_NOT_OPEN -1006 //设备没有打开
#define ERR_ACCESS_DENY -1007 //无权访问
#define ERR_FONT_NOT_EXIST -1008 //没有指定的字体
#define ERR_FILE_FORMAT -1009 //文件格式错误
#define ERR_USER_CANCEL -1010 //用户取消
#define ERR_NO_PORT -1011 //无通信口可用
#define ERR_NOT_CONNECT -1012 //未连接
#define ERR_MEM_FAULT -1013 //内存错误
#define ERR_SYS_BAD -1014 //系统配置有问题
#define ERR_SYS_NOT_SUPPORT -1015 //系统没有提供该功能
#define ERR_STR_LEN -1016 //字符串太长
#define ERR_TOO_MANY_HANDLE -1017 //句柄过多
#define ERR_APP_MODE -1018 //模式出错
#define ERR_FILE_NOT_EXIST -1019 //文件不存在
#define ERR_TS_DISABLE -1020 //触摸屏未打开
#define ERR_FONT_CODE -1021 //字符编码错误
#define ERR_VERSION_TOO_LOW -1022 //版本过低
#define ERR_BATTERY_ABSENT -1023 //电池不存在
#define ERR_BATTERY_VOLTAGE_TOO_LOW -1024 //电池电压过低

/************************************4系统功能*********************************/
//4系统功能
#define ERR_FILE_NOT_FOUND -2201 //文件没有找到
#define ERR_VERIFY_SIGN_FAIL -2204 //验证签名失败
#define ERR_NO_SPACE -2205 //系统空间不够
#define ERR_NEED_ADMIN -2207 //需要更高权限
#define ERR_PUK_NOT_EXIST -2208 PUK //不存在
#define ERR_OS_VERSION_TOO_LOW -2209 //系统版本过低

#define FILE_TYPE_APP 1 //应用包
#define FILE_TYPE_APP_PARAM 2 //应用数据文件
#define FILE_TYPE_SYS_LIB 3 //系统动态库文件
#define FILE_TYPE_PUB_KEY 4 //用户公钥文件
#define FILE_TYPE_AUP 5 //应用升级包

typedef enum{
    LOG_DEBUG, /*显示调试信息*/
    LOG_INFO, /*显示提示信息*/
    LOG_WARN, /*显示警告信息*/
    LOG_ERROR, /*显示错误信息*/
} LOG_T;

//ST_TIME 时间结构体
typedef struct{
    int Year; /*年 1970– 2037*/
    int Month; /*月 1 –12*/
    int Day; /*日 1 –31*/
    int Hour; /*时 0 – 23*/
    int Minute; /*分 0 –59*/
    int Second; /*秒 0 –59*/
    int DayOfWeek; /*周 1–周 7（仅读取时间有效）*/
} ST_TIME;

//ST_TIMER 定时器结构体
typedef struct{
    long long Start;
    long long Stop;
    long long TimeLeft;
} ST_TIMER;

//ST_APP_INFO 应用信息结构体
typedef struct{
    char Id[64];
    char Name[64];
    char Bin[64];
    char Artwork[64];
    char Desc[64];
    char Vender[32];
    char Version[32] ;
} ST_APP_INFO;

//ST_OPT_INFO 可选系统组件信息结构体
typedef struct {
    char Name[64];
    char Version[32];
} ST_OPT_INFO;

long long get_sys_tick();

//功能 设置系统当前的日期和时间
int OsSetTime(const ST_TIME *Time);
//获取终端系统日期和时间。
void OsGetTime(ST_TIME *Time);
//非接模块测试接口
int OsYmRfidTest();
//获取安全攻击状态。 0表示正常  非0 攻击状态
int OsGetAntiTamperState(int* AntiTamperState);
//获取设备模式  mode 0 user版本  1 debug版本
int OsGetDevMode(int *mode);
//获取客户应用CID密文
int OsGetEncCID(char *EncrptyCid);
//获取OS固件的CID密文
int OsGetSysFwEncCID(char *EncrptyCid);
//获取PCBA板号
int OsGetYMPcbaNo(char *pcbaNo);
//设置PCBA板号
int OsSetYMPcbaNo(char *pcbaNo);
//创建一个指定时间的定时器。
int OsTimerSet(ST_TIMER *Timer, unsigned long Ms);
//检测指定定时器的剩余值。
unsigned long OsTimerCheck(ST_TIMER *Timer);
//设置当前进程/线程暂停。
void OsSleep(unsigned int Ms);
//设置 LOG 信息标签。
void OsLogSetTag(const char *Tag);
//保存 LOG
int OsLog(LOG_T Prio, const char *fmt, ...);
//获取系统从上电开机到当前运行的累计时间，不包括休眠时间，单位为毫秒。
unsigned long OsGetTickCount(void);
//获取已安装的应用软件的信息。
int OsGetAppInfo(ST_APP_INFO AppInfo[],int InfoCnt);
//获取已安装的可选系统组件的信息。
int OsGetOptInfo(ST_OPT_INFO OptInfo[],int InfoCnt);
//通过appid获取应用信息
int OsGetAppInfoByAppid(char* appid,ST_APP_INFO *pInfo);
/*
功能 设置蜂鸣器鸣叫的频率和持续时间。
参数
    ToneType 【输入】 蜂鸣类型，范围【1，7】
    DurationMs 【输入】持续时间，范围【10，10000】单位：毫秒。如果 DurationMs<10，持续时间设置为 10；如果 DurationMs>10000 ， 持 续 时 间 设 置 为10000。
返回 无
*/
void OsBeep(int ToneType, int DurationMs);
/*
功能 设置按键音的开关和占空比。
参数
    OnOff【输入】 0 关闭按键音，1 打开按键音。
    DutyCycle【输入】按键音占空比，范围【1，99】。如果 DutyCycle <1，或者 DutyCycle >99，保持原占空比不变。
    DutyCycle=50 时，音量最大。
*/
void OsSetKeyTone(int OnOff,int DutyCycle);

/*
功能 设置低电量LED闪烁频率。
参数
    Freq【输入】 1~20HZ。
返回 0成功 其他失败
*/
int OsSetLPLEDFreq(int Freq);
/*
功能 获取低电量LED闪烁频率。
参数
    Freq【输出】 1~20HZ。
返回 0成功 其他失败
*/
int OsGetLPLEDFreq(int *Freq);

typedef void (*RUNAPP_CB)(char *appid, char *str, void *data);
/*
功能 切换到指定的子应用。
参数
    AppId【输入】 需要切换的子应用 ID。
    Argv【输入】 子应用运行的参数列表，可以为 NULL。(暂不使用)
    Data【输入】 自定义数据，将传递给参数 CbOut 和 CbErr回调。(暂不使用)
    CbOut【输入】 标准输出信息回调函数。(暂不使用)
    CbErr【输入】 标准错误信息回调函数。(暂不使用)
返回
    RET_OK 切换成功
    ERR_APP_NOT_EXIST 子应用不存在
    ERR_ACCESS_DENY 访问设备被拒绝
    ERR_APP_MODE 模式错误
    ERR_INVALID_PARAM 非法参数
    ERR_NEED_ADMIN 需要系统主应用权限
*/
int OsRunApp(char *AppId, char **Argv, void *Data, RUNAPP_CB CbOut,RUNAPP_CB CbErr);
/*
功能 切换到指定的子应用。
参数
    AppId【输入】 需要切换的子应用 ID。
    Argc：【输入】应用入口参数个数 参数最大12个;
    Argv:【输入】参数最大12个，长度小于31字节       
返回
    RET_OK 切换成功
    ERR_APP_NOT_EXIST 子应用不存在
    ERR_ACCESS_DENY 访问设备被拒绝
    ERR_APP_MODE 模式错误
    ERR_INVALID_PARAM 非法参数
    ERR_NEED_ADMIN 需要系统主应用权限
*/
int OsRunAppEx(char *AppId, int Argc,char Argv[][32]);
/*
功能 安装或更新应用软件、应用数据、OPT 包、用户公钥和外设固件(FWP包)。
参数
AppId【输入】 应用ID 预留。
FilePath【输入】 待安装的apk文件路径，不能为 NULL。
FileType 
    FILE_TYPE_APP 应用软件安装包，即 AIP 包
    FILE_TYPE_APP_PARAM 应用数据文件
    FILE_TYPE_SYS_LIB 动态库等可选系统组件，即 OPT 包
    FILE_TYPE_PUB_KEY 用户公钥文件
    FILE_TYPE_AUP 应用升级包
    FILE_TYPE_FWP 设备固件包
返回
    RET_OK 成功
    ERR_PUK_NOT_EXIST 指定的用户公钥不存在
    ERR_FILE_NOT_FOUND 文件不存在
    ERR_FILE_FORMAT  文件格式错误
    ERR_INVALID_PARAM 非法参数
    ERR_VERIFY_SIGN_FAIL 签名错误
    ERR_APP_MODE 模式错误
*/
int OsInstallFile(const char *AppId,const char *FilePath,int FileType);

/*
功能 卸载指定的应用或 OPT 包。
参数
    AppId【输入】应用ID。
    FileType 
        FILE_TYPE_APP 应用包(包括应用已经安装的所有文件)
        FILE_TYPE_SYS_LIB OPT 包(包括 OPT包所有文件)
返回
    RET_OK 成功
    ERR_APP_NOT_EXIST 应用或 OPT 包不存在
    ERR_FONT_NOT_EXIST 字库不存在
*/
int OsUninstallFile(const char *AppId, int FileType);

/*
功能 获取当前的系统固件版本。
参数
    Version【输出】 固件版本缓冲区，
    Size 【输入】 Version 缓冲区长度。建议长度为 64 字节。
返回
    RET_OK 成功获取
    ERR_INVALID_PARAM 非法参数
*/
int OsFirmwareGetVersion(char *Version,int Size);

/*
功能 升级系统固件。
参数 FwPath   固件路径，YMG
     FwType 【输入】   固件类型
        FW_TYPE_SE_APP        SE 应用程序
        FW_TYPE_SE_FW_CFG    SE 固件配置
        FW_TYPE_SE_FW         SE 固件
返回
    RET_OK 更新成功
    RR_FILE_NOT_FOUND   文件不存在
    ERR_VERIFY_SIGN_FAIL 签名错误
    ERR_APP_MODE 模式错误
    ERR_SYS_NOT_SUPPORT OS 不兼容
*/

#define FW_TYPE_SE_APP     1 //SE App se_fota_application.YMG
#define FW_TYPE_SE_FW_CFG  2 //SE FW  se_fota_dtb_pro.YMG
#define FW_TYPE_SE_FW      3 //SE FW  se_fota_firmware.YMG

int OsFirmwareUpgrade(const char *FwPath, int FwType);

/*
功能 获取操作系统以及模块固件版本信息。
参数 VerType 版本类别：
    0x01: 系统版本 
    0x02: 中间件版本号
    0x03: 安全CPU App版本号
    0x04: 安全CPU 固件版本
Version【输出】 版本信息缓冲区大小必须不小于 31 字节｡
返回 无
用法 1. 如果 Version[0]等于 0x00，表示相应模块不存在；
     2. 各版本信息的长度小于等于 31 字节，并以“\0”字符结尾。
*/
void OsGetSysVer(int VerType, char *Version);

void OsGetSysVersion(int VerType, char *Version);

//获取无线模块版本号 缓存32字节
int OsGetWirelessModuleVer(char *ver);

int OsGetTermSn(char* Sn);

//设置SN 最大不超过31字节
int OsSetTermSn(char* Sn);

int OsLed(unsigned int red,unsigned int green,unsigned int yellow,unsigned int blue);

int OsGetSysParam(char *key, char *value);

int OsSetSysParam(char *key, char *value);
/*
功能 文件升级函数回调指针。
参数 Result 升级结果  >=0 升级成功  其他失败
     Reboot 是否需要重启  1需要  0不需要
返回 1 sdk处理重启   0 应用处理重启。
*/
typedef  int (*pUpgradeCallback)(int Result, char Reboot);

int OsUpgradeFile(char *pZipPath,pUpgradeCallback callback);

int OsDownLoadAndUpgradeFile();

//获取启动模式  pMode 1 工厂模式
int OsGetSysBootMode(int *pMode);
//获取硬件版本 hw_version[0] ='4',类型A30  ‘7’类型A50 其它未知
void OsGetHwVersion(char *hw_version);
int OsGetFactoryTestStep();
int OsSetFactoryTestStep(int flag);

//获取当前系统生效的DNS 
int OsGetCurDNSAddr(char* dnsaddr,int dnsaddrLen);
// 设置无线或WIFI的DNS， type类型 0 wifi，1 无线
int OsGetDNSAddr(int type,char* dnsaddr,int dnsaddrLen);
//获取设置无线或WIFI的DNS, type类型 0 wifi，1 无线
int OsSetDNSAddr(int type,char* dnsaddr);
//设置首选网络类型，type取值 0:Auto GSM/LTE，1:GSM only，3:LTE only
int OsSetPreferredNetworkType(int type);
//获取首选网络类型  type取值 0:Auto GSM/LTE，1:GSM only，3:LTE only
int OsGetPreferredNetworkType(int *type);

//设置深度休眠后多久无操作关机（单位0.5h） Time取值 （0<Time<=48）
int OsSetSleepShutdownTime(int Time);
//获取深度休眠后多久无操作关机（单位0.5h）
int OsGetSleepShutdownTime(int *Time);

/************************************5加解密*********************************/
//5加解密
#define ERR_DATA_TOO_BIG -2400  //RSA 被加密数据大于模
#define ERR_GEN_RANDOM -2401  //产生随机数失败
#define ERR_GEN_FAIL -2402  //生成 RSA 密钥对失败

#define SHA_TYPE_1    1
#define SHA_TYPE_224  2
#define SHA_TYPE_256  3
#define SHA_TYPE_384  4
#define SHA_TYPE_512  5

/*
产生真随机数｡
参数
Random【输出】 存放随机数的指针
RandomLen 【输
入】 需要读取的随机数的长度(小于等于 4096 字节)
*/
void OsGetRandom(unsigned char *Random,int RandomLen);
/*
计算 SHA(Secure Hash Algorithm)族散列值｡
参数
Mode 算法类型SHA_TYPE_1  SHA_TYPE_224 SHA_TYPE_256 SHA_TYPE_384 SHA_TYPE_512 
Data【输入】 输入数据缓冲区
DataLen 【输入】 输入数据长度
ShaOut【输出】 输出的 SHA 值，存储空间建议大于 64 个字节｡
*/
void OsSHA(int Mode, const void *Data, int DataLen, unsigned char*ShaOut);

/*
DES/TDES 加解密运算｡
参数
Input【输入】 8 字节的输入数据
Output【输出】 8 字节的输出数据
DesKey【输入】 DES/TDES 密钥
KeyLen 【输入】 8、16 或 24(字节)
Mode 1-加密 0-解密
*/
void OsDES(const unsigned char *Input, unsigned char *Output, const unsigned char *DesKey, int KeyLen, int Mode);

/*
AES 加解密运算｡
参数
Input【输入】 16 字节的输入数据
Output【输出】 16 字节的输出数据
AesKey【输入】 密钥
KeyLen 【输入】 16、24 或 32(字节)
Mode 1-加密 0-解密
*/
void OsAES(const unsigned char *Input, unsigned char *Output, const unsigned char *AesKey,int KeyLen, int Mode);

/*
进行 RSA 加解密运算。
参数
Modulus 【输入】 存放 RSA 运算的模指针(即 n=p*q)按高位在前，低位在后的顺序存储｡
ModulusLen 【输入】 模长度(以字节为单位)
Exp【输入】 存放 RSA 运算的指数指针。按高位在前，低位在后的顺序存储。
ExpLen 【输入】 指数长度(以字节为单位)
DataIn【输入】 输入数据缓冲区指针，长度与模长相同
DataOut【输出】 输出数据缓冲区指针，长度与模长相同
Mode 1-加密 0-解密
返回
RET_OK 成功
ERR_INVALID_PARAM 非法参数
ERR_DATA_TOO_BIG ExpLen 大于 ModulusLen
*/
int OsRSA(const unsigned char *Modulus, int ModulusLen,const unsigned char *Exp, int ExpLen, const unsigned char *DataIn, unsigned char *DataOut,int Mode);

//RSA RSA_NO_PADDING 公钥解密，输入数据长度和NLen相等
int OsEmvRSAPublicDecrypt(unsigned char *N, int NLen,unsigned char *E, int ELen, unsigned char *DataIn,unsigned char *DataOut);
/*
生成指定指数和模长的 RSA 公私密钥对。
参数 Modulus 【输出】 密钥的模(按高位在前，低位在后的顺序存储)
PriExp【输出】 私钥指数(按高位在前，低位在后的顺序存储)
ModulusLen 【 输入】模的长度(单位：字节，可以为：64、128、256、512) 
PubExp【输入】公钥指数。只能为：“ \x00\x00\x00\x03 ” 或“\x00\x01\x00\x01”
返回
RET_OK 成功
ERR_INVALID_PARAM 非法参数
ERR_GEN_RANDOM 数据随机失败
ERR_GEN_FAIL 生成失败
*/
int OsRSAKeyGen(unsigned char *Modulus, unsigned char *PriExp, int ModulusLen, const unsigned char * PubExp);

/************************************7LCD*********************************/

/*
功能 设置屏幕亮度。
参数 Brightness 亮度值，取值范围为[0~10]，0：关闭背光。10：最亮。
默认值为 8，其它值无动作。
*/
void OsScrBrightness(int Brightness);

/*
功能 获取 LCD 物理屏幕大小。
参数
    Width【输出】 宽度，单位为象素。
    Height【输出】 高度，单位为象素。
返回 无
*/
void OsScrGetSize(int *Width, int *Height);

/************************************6 PED*********************************/
#define ERR_PED_NO_KEY -3801 //密钥不存在 
#define ERR_PED_KEY_IDX_ERR -3802 //密钥索引错误 
#define ERR_PED_DERIVE_ERR -3803  //写密钥时,密钥层次错误,源密钥层次低于目的密钥的层次 
#define ERR_PED_CHECK_KEY_FAIL -3804 //密钥验证失败 
#define ERR_PED_NO_PIN_INPUT -3805 //无 PIN 输入 
#define ERR_PED_PIN_INPUT_CANCEL -3806 //取消输入 PIN 
#define ERR_PED_WAIT_INTERVAL -3807 //函数调用的间隔时间小于设定的最小时间间隔(计算
#define ERR_PED_KCV_MODE_ERR -3808 //KCV 模式错误 
#define ERR_PED_KEY_TAG_ERR -3809 //密钥标签错误,无法使用该密钥 
#define ERR_PED_KEY_TYPE_ERR -3810 //密钥类型不支持或者指定索引的密钥类型不匹配 
#define ERR_PED_PIN_LEN_ERR -3811 //输入 PIN 的长度与期望长度不符 
#define ERR_PED_DSTKEY_IDX_ERR -3812 //目的密钥索引错误 
#define ERR_PED_SRCKEY_IDX_ERR -3813 //源密钥索引错误 
#define ERR_PED_KEY_LEN_ERR -3814 //密钥长度错误 
#define ERR_PED_INPUT_PIN_TIMEOUT -3815 //输入 PIN 超时 
#define ERR_PED_NO_ICC -3816 //IC 卡不存在 
#define ERR_PED_ICC_INIT_ERR -3817 //IC 卡初始化错误 
#define ERR_PED_GROUP_IDX_ERR -3818 //DUKPT 组索引错误 
#define ERR_PED_TAMPERED -3819 //PED 已锁 
#define ERR_PED_NO_MORE_BUF -3820 //没有空闲的缓冲区 
#define ERR_PED_NORMAL_ERR -3821 //PED 通用错误 
#define ERR_PED_NEED_ADMIN -3822 //当前操作需要高级权限 
#define ERR_PED_DUKPT_KSN_OVERFLOW -3823 //当前 DUKPT 溢出 
#define ERR_PED_KCV_CHECK_FAIL -3824 //KCV 校验失败 
#define ERR_PED_SRCKEY_TYPE_ERR -3825 //写入密钥时,源密钥 ID 的类型和源密钥类型不匹配 
#define ERR_PED_UNSPT_CMD -3826 //命令不支持 
#define ERR_PED_ADMIN_ERR -3827 //获取系统敏感服务失败 
#define ERR_PED_DOWNLOAD_INACTIVE -3828 //未激活 PED 下载功能
#define ERR_PED_KCV_ODD_CHECK_FAIL -3829 //KCV 奇校验失败 
#define ERR_PED_PED_DATA_RW_FAIL -3830 //读取 PED 数据失败 
#define ERR_PED_ICC_CMD_ERR -3831 //IC 卡操作错误(脱机明文､密文 PIN 验证) 
#define ERR_PED_DUKPT_NEED_INC_KSN -3832  //DUKPT KSN 需要先加 1 
#define ERR_PED_DUKPT_NO_KCV -3833 //DUKPT 密钥没有 KCV 
#define ERR_PED_NO_FREE_FLASH -3834 //PED 存储空间不足 
#define ERR_PED_INPUT_CLEAR -3835 //用户按 CLEAR 键退出 PIN输入 
#define ERR_PED_INPUT_BYPASS_BYFUNCTION -3836 //用户按 FN/ATM4 键取消PIN 输入 
#define ERR_PED_NO_PIN_MODE -3837 //未设置 PIN 输入模式 
#define ERR_PED_DATA_MAC_ERR -3838 //数据 MAC 校验错误 
#define ERR_PED_DATA_CRC_ERR -3839 //数据 CRC 校验错误 
#define ERR_PED_KEY_VALUE_INVALID -3840 //工作密钥值不符合要求或者已经存在 

#define PED_TLK 0x01 //Loading Key 
#define PED_TMK 0x02 //Master Key 
#define PED_TPK 0x03 //PIN Key 
#define PED_TAK 0x04 //MAC Key 
#define PED_TDK 0x05 //Data Key 
#define PED_TIK 0x10 //DUKPT Initial Key 
#define PED_TRK 0x07 //MSR Key 
#define PED_TAESK 0x20 //AES Key 
#define PED_SM2_PVT_KEY 0x30 //SM2 Private Key 
#define PED_SM2_PUB_KEY 0x31 //SM2 Public Key 
#define PED_SM4_TMK 0x32 //SM4 Master Key 
#define PED_SM4_TPK 0x33 //SM4 PIN Key 
#define PED_SM4_TAK 0x34 //SM4 MAC Key 
#define PED_SM4_TDK 0x35 //SM4 Data Key 
#define PED_AES_TLK 0x27 //AES Loading Key 
#define PED_AES_TMK 0x28 //AES Master Key 
#define PED_AES_TPK 0x23 //AES PIN Key 
#define PED_AES_TAK 0x24 //AES MAC Key 
#define PED_AES_TDK 0x20 //AES Data Key 
#define PED_AES_TIK 0x51 //AES Initial Key 


#define  PED_ASTERISK_ALIGN_LEFT 0    //左对齐 
#define  PED_ASTERISK_ALIGN_CENTER 2  //居中 
#define  PED_ASTERISK_ALIGN_RIGHT 1    //右对齐 

typedef struct { 
    int ModulusLen; /* 模长(位) */ 
    unsigned char Modulus[512]; /* 模，不足 512 字节左补 0x00 */ 
    int ExponentLen; /* 指数长度(位) */ 
    unsigned char Exponent [512]; /* 指数，不足 512 字节左补 0x00 */ 
    unsigned char KeyInfo[128]; /* RSA 密钥信息 */ 
 } ST_RSA_KEY; 

typedef struct { 
    int ModulusLen; /* PIN 加密公钥模长(位) */ 
    unsigned char Modulus[256]; /* PIN 加密公钥模数 */ 
    unsigned char Exponent [4]; /* PIN 加密公钥指数 */ 
    int IccRandomLen; /* 从 IC 卡取得的随机数的长度 */ 
    unsigned char IccRandom[8]; /* 从 IC 卡取得的随机数 */ 
} ST_RSA_PINKEY; 

/************************************6.4 基本 PED *********************************/
/*
功能 打开 Prolin 的 PED 服务。 
参数 无 
返回 
    RET_OK 成功 
    ERR_DEV_BUSY 设备被占用 
*/
int OsPedOpen (void); 

/*
功能 获取当前的 PED 的版本。 
参数 PedVer【输出】 PED 版本信息缓冲区，6 字节。 
返回 
    RET_OK 成功 
    ERR_DEV_NOT_OPEN PED 设备未打开 
    ERR_INVALID_PARAM 非法参数 
*/
int OsPedGetVer (unsigned char * PedVer); 

/*
功能 擦除 PED 服务保存的所有密钥信息。 
参数 无 
返回 
    RET_OK 成功 
    ERR_DEV_NOT_OPEN 设备未打开 
    其他 请参考 PED 函数返回值列表
*/
int OsPedEraseKeys (void); 

/*
功能 断开与 PED 服务的连接。 
参数 无 
返回 无 
用法 程序退出前需调用该函数关闭设备。 
*/
void OsPedClose (void); 


/************************************6.5 PIN 输入*********************************/

/*
功能 监听并获取当前状态下用户输入的 PIN 按键数量及当次监听与上次监听中间的历史按键序列。 

参数 

KeyCacheCnt【输出】 获取到的当前历史按键值数量。 
KeyCache【输出】  暂未实现
返回 
    >= 0 获取 PIN 输入事件成功，返回值大小表示此时 PIN 输入界面的“*”的个数。 
    ERR_DEV_NOT_OPEN PED 设备未打开 
    ERR_INVALID_PARAM 非法参数 
    其他 请参考 PED 函数返回值列表 
*/
int OsPedPinKeyNotify(int *KeyCacheCnt, unsigned char *KeyCache);

/*
功能 设置pin输入框显示第几行和对齐方式  默认显示第三行 居左。
参数 无
返回 RET_OK 成功
*/
int OsPedSetAsteriskLayout(int line,int align);

/************************************6.6 MK/SK*********************************/
/*
功能 写入一个密钥，包括 TLK，TMK，TWK，SM4_TMK 和 SM4_TWK的写入、发散，并可以选择使用 KCV 验证密钥正确性。
参数 KeyBlock【输入】 详见规范
返回
    0 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_TAMPERED PED 被锁定
    ERR_PED_NO_MORE_BUF 系统内存空间不足
    ERR_PED_NORMAL_ERR PED 通 用 错 误 （ KeyBlock Format 错误）
    ERR_PED_DERIVE_ERR 密钥发散错误
    ERR_PED_KCV_MODE_ERR PED KCV 校验模式错误
    ERR_PED_KCV_CHECK_FAIL PED KCV 校验失败
    其他 请参考 PED 函数返回值列表
*/
int OsPedWriteKey (const unsigned char * KeyBlock);

/*
功能 指定的时限内,扫描键盘上输入 ExpPinLenIn 指定长度的 PIN，并输出由 Mode 指定算法加密生成的 PIN block。
参数
    KeyIdx TPK 的索引号，取值范围为[1-100] 。
    DataIn【输入】
    TimeoutMs 输入 PIN 的超时时间单位：毫秒最大值为 3000000：表示没有超时时间，PED 不做超时控制。
    PinBlock【输出】8 或 16 字节，指向生成的 PIN block。Mode 为 0x10 时，PinBlock 长度为 16 字节。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetPinBlock (int KeyIdx,const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *PinBlock);
 
typedef void (*PIN_STAR_DISPLAY)(int len);//密码键盘显示*号函数指针，len当前密码长度
/*
功能 指定的时限内,扫描键盘上输入 ExpPinLenIn 指定长度的 PIN，并输出由 Mode 指定算法加密生成的 PIN block。
参数
    KeyIdx TPK 的索引号，取值范围为[1-100] 。
    DataIn【输入】
    TimeoutMs 输入 PIN 的超时时间单位：毫秒最大值为 3000000：表示没有超时时间，PED 不做超时控制。
    PinBlock【输出】8 或 16 字节，指向生成的 PIN block。Mode 为 0x10 时，PinBlock 长度为 16 字节。
    pin_star_display输入  *号显示函数指针
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetPinBlockEx (int KeyIdx,const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *PinBlock,PIN_STAR_DISPLAY pin_star_display);

/*
功能 用 KeyIdx 指定的 MAC 密钥对 DataIn 进行 Mode 指定的运算。
参数
    KeyIdx TAK 的索引号，取值范围为[1~100]。
    DataIn【输入】 需进行 MAC 运算的数据，长度小于等于 2032 字节。
    DataInLen MAC 运算的数据长度，当长度不能被 8 字节整除，则自动补“\x00”。
    Mac【输出】 8 字节，MAC 输出。
    Mode 将 DataIn 按 8 字节为单位分块，依次为 BLOCK1，BLOCK2，BLOCK3 等。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetMac(int KeyIdx, const unsigned char *DataIn, int DataInLen, unsigned char *Mac, int Mode);

/*
功能
使用 TDK 对 DataInLen 长度的数据进行 DES/TDES 解密或加密运算，输出明文或密文；一个指定的 TDK，只能用于加密或解密运算。
参数
    KeyIdx TDK 索引号，取值范围为[1~100]。
    InitVector【输入】加解密运算的初始向量，长度为 8 字节。当 Mode=0x02/0x03/0x04/0x05 时,需要用到初始向量，为 NULL 时默认为“\x00\x00\x00\x00\x00\x00\x00\x00”；当 Mode=0x00/0x01 时，不需要初始向量，可以为 NULL。
    DataIn【输入】 指向需要进行运算的数据。
    DataInLen需要运算的数据长度，以字节为单位，应小于等于 1024；当 Mode 为 0x00~0x05 时，必须为 8 的倍数；当 Mode 为 0x06~0x07 时，可以不是 8 的倍数。
    DataOut【输出】 指向运算后的数据。
    Mode  0x00：ECB 解密 0x01：ECB 加密 0x02：CBC 解密 0x03：CBC 加密 0x04：OFB 解密 0x05：OFB 加密 0x06：CFB8 解密 0x07：CFB8 加密
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedDes(int KeyIdx,unsigned char * InitVector, const unsigned char *DataIn, int DataInLen, unsigned char *DataOut, int Mode);

/*
功能
获取密钥的 KCV 值，以供对话双方进行密钥验证：密钥类型非TIK 时，用指定的密钥及算法对一段数据进行加密，并返回数据密文的前 3 字节；密钥类型为 TIK 时，返回与 TIK 一并注入(计算得到)的 8 字节 KCV。
参数
    KeyType 密钥类型
    KeyIdx 密钥的索引号
    KcvModeKCV 校验模式
    KcvDataLen参与计算 KCV 的数据长度，不超过 128 字节，且被 8 整除；密钥类型为 TIK 时可为 0；KCV校验模式为 0x03 或 0x04 时，该数据长度必须能被 16 整除。
    KcvData【输入】 参与计算 KCV 的数据，密钥类型为 TIK 时可为 NULL。
    Kcv【输出】KCV 值，密钥类型为 TIK 和 AESTIK 时，KCV为 8 字节；密钥类型为 AES 时，KCV 为 5 字节，其余为 3 字节。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetKcv(int KeyType, int KeyIdx, int KcvMode, int KcvDataLen, unsigned char *KcvData, unsigned char *Kcv);


/************************************6.7 DUKPT*********************************/
/*
功 能 写入一个 TIK 密钥，并可以选择使用 KCV 验证密钥正确性。
参数 KeyBlock【输入】
返回
    ERR_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedWriteTIK (const unsigned char *KeyBlock);

/*
功能 指定的时限内，扫描键盘上输入的 PIN，并输出使用 DUKPT 的PIN 密钥计算生成的 PIN block。
参数
    GroupIdx DUKPT 组索引号，取值范围为[1~100]。
    DataIn【输入】
    ExpPinLen【输入】
    Mode 选择 PIN block 的格式：
    TimeoutMs 输入 PIN 的超时时间，单位：毫秒。最大值为 3000000：表示没有超时时间，PED 不做超时控制。
    Ksn【输出】 10 字节，指向当前的 KSN。
    PinBlock【输出】 8 字节，指向生成的 PIN block。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetPinDukpt (int GroupIdx, const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *Ksn, unsigned char *PinBlock);
/*
功能 指定的时限内，扫描键盘上输入的 PIN，并输出使用 DUKPT 的PIN 密钥计算生成的 PIN block。
参数
    GroupIdx DUKPT 组索引号，取值范围为[1~100]。
    DataIn【输入】
    ExpPinLen【输入】
    Mode 选择 PIN block 的格式：
    TimeoutMs 输入 PIN 的超时时间，单位：毫秒。最大值为 3000000：表示没有超时时间，PED 不做超时控制。
    Ksn【输出】 10 字节，指向当前的 KSN。
    PinBlock【输出】 8 字节，指向生成的 PIN block。
    pin_star_display输入  *号显示函数指针
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetPinDukptEx (int GroupIdx, const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *Ksn, unsigned char *PinBlock,PIN_STAR_DISPLAY pin_star_display);

/*
功能 使用 DUKPT 密钥计算 MAC 值。
参数
    GroupIdx DUKPT 组索引号，取值范围为[1~100]。
    DataIn【输入】 指向需要计算 MAC 的数据内容。
    DataInLen 数据的长度，小于等于 2032 字节，当不为 8字节整除时自动补“\x00”。
    Mac【输出】 指向得到的 MAC。
    Ksn【输出】 指向当前的 KSN。
    Mode 
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表

*/
int OsPedGetMacDukpt(int GroupIdx, const unsigned char *DataIn, int DataInLen, unsigned char *Mac, unsigned char *Ksn, int Mode);

/*
功能 使用 DUKPT 密钥，对输入数据进行加解密。
参数
    GroupIdx DUKPT 组索引号，取值范围为[1~100]。
    KeyVarType【输入】
    InitVector【输入】加解密运算的初始向量，8 字节可以为 NULL。
    DataInLen 需要加解密的数据长度，小于等于 8192
    DataIn【输入】 指向需要进行运算的数据
    DataOut【输出】 指向运算后的数据
    Ksn【输出】 指向当前的 KSN，10 字节
    Mode 加解密模式
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedDesDukpt(int GroupIdx, int KeyVarType, unsigned char *InitVector, int DataInLen, unsigned char *DataIn, unsigned char *DataOut, unsigned char *Ksn, int Mode);

/*
功能 获取当前的 KSN 值。
参数
    GroupIdx DUKPT 组索引号，取值范围为[1~100]。
    Ksn【输出】 指向当前 KSN，10 字节
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetKsnDukpt(int GroupIdx, unsigned char *Ksn);
/*
功能 增加指定 DUKPT 组的 KSN 值。
参数 GroupIdx DUKPT 组 索引号，取值范围为[1~100]。
返回 RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedIncreaseKsnDukpt (int GroupIdx);

/************************************11打印机*********************************/

#define ALIGN_TYPE_LEFT 0    //居左
#define ALIGN_TYPE_CENTER 1   //居中
#define ALIGN_TYPE_RIGHT 2   //居右


#define ERR_PRN_BUSY -3701 //打印机忙
#define ERR_PRN_PAPEROUT -3702 //打印机缺纸
#define ERR_PRN_WRONG_PACKAGE -3703 //数据包格式错误
#define ERR_PRN_OVERHEAT -3704 //打印机过热
#define ERR_PRN_OUTOFMEMORY -3705 //要打印数据过大,超出缓冲区长度
#define ERR_PRN_OVERVOLTAGE -3706 //电压过高

enum{
    PRN_REAL = 0,   //物理打印设备；
    PRN_BMP,        //虚拟打印设备，打印结果以 BMP 格式保存在本地。
};

/*
功能 打开打印机设备，包括物理和虚拟打印机。
参数
    printertype【输入】
    打印机设备类型   PRN_REAL：物理打印设备  PRN_BMP：虚拟打印设备，打印结果以 BMP 格式保存在本地。
    targetname【输入】打印设备为物理打印机时，该参数必须为NULL；
    打印设备为虚拟打印机时，指向输出到本地的BMP 文件的路径。若指定文件已经存在，则覆盖。
返回
    RET_OK 成功
    ERR_DEV_NOT_EXIST 设备不存在
    ERR_INVALID_PARAM 非法参数
    ERR_DEV_BUSY 设备被占用
    ERR_BATTERY_ABSENT 电池不存在
    ERR_BATTERY_VOLTA
    GE_TOO_LOW 电池电压过低
*/
int OsPrnOpen(unsigned int printertype, const char*targetname );

/*
功能 重置打印参数,清空打印缓存。
参数 无
返回 无
*/
void OsPrnReset(void);
/*
功能 关闭打印机。
参数 无
返回 无
用法 程序退出时需调用该函数关闭设备。
*/
void OsPrnClose(void);


/*
功能 设置打印灰度。
参数 Level 
    Level =0，保留。
    Level =1，缺省黑度，即普通打印单。
    Level =2，保留。
    Level =3，双层热敏打印。
    Level =4，双层热敏打印，比 3 的黑度更高。
    非法值不改变当前设置。
返回 无
    用法 本接口仅对实体打印机有效。
*/
void OsPrnSetGray(int Level);

/*
功能 设置打印间距。
参数
    CharSpace 字间距，单位为点数(对强制的非等宽字体无效，如各种阿拉伯字体、泰文字体)
    LineSpace 行间距，单位为点数
返回 无
用法
1. 在对打印机有效设置后将一直有效，直至下次设置或调用OsPrnReset( )；
2. 打印机的默认字间距为 0；
3. 打印机的默认行间距热敏为 0，针打为 2；
4. 打印机的行间距最大值为 255；
5. 打印机的字间距最大值为 255；
6. 参数值非法不改变当前设置｡
*/
void OsPrnSetSpace(int CharSpace, int LineSpace);

/*
功能 设置字体反显属性，缺省情况下正常打印。
参数 Attr 
    0：正常
    1：反显
返回 无
用法 对实体及虚拟打印机均有效｡对于打印图形反显无效｡
*/
void OsPrnSetReversal(int Attr);

/*
功能 设置左右边距｡
参数
    Left【输入】 左边距，有效范围是[0，100]，默认值为 0。
    Right【输入】 右边距，有效范围是[0，100]，默认值为 0。
返回
    RET_OK 成功
    ERR_INVALID_PARAM 非法参数
用法 如果实体打印机被设置为纵向打印，那么左边距对应页面的上边距，右边距对应页面的下边距
*/
int OsPrnSetIndent (unsigned int Left, unsigned int Right);
/*
功能 查询打印机当前状态，且仅适用于物理打印机。
参数 无
返回
    RET_OK 打印机工作正常
    ERR_PRN_BUSY 打印机忙
    ERR_PRN_PAPEROUT 打印机缺纸
    ERR_PRN_OVERHEAT 打印机过热
*/
int OsPrnCheck(void);


/*
功能 设置打印参数。自定义
参数
FontSize 字体大小 12  16 24
DoubleWidth 倍宽  1~4 
DoubleHeight 倍高  1~4
AlignType 居左 居中  居右
返回 无
*/
void  OsPrnSetPrintParams(int FontSize,int DoubleWidth, int DoubleHeight,int AlignType);
/*
功能 设置打印参数。自定义
参数
FontSize 字体大小 12  16 24
DoubleWidth 倍宽  1~4 
DoubleHeight 倍高  1~4
AlignType 居左 居中  居右
IsPersian 1 波斯语  
返回 无
*/
void OsPrnSetPrintParamsEx(int FontSize,int DoubleWidth, int DoubleHeight,int AlignType,int IsPersian);
/*
功能 在打印缓冲区内，执行走纸若干个点行。
参数 Pixel 
    执行走纸的点行数：
    >0：进纸
    <0：退纸
    =0：无动作
返回 无
用法 对实体及虚拟打印机皆有效。
*/
void OsPrnFeed(int Pixel);

/*
功能 格式化输出字符串到打印缓冲区内。
参数 Str【输入】 要打印的字符串指针
返回 无
用法
    1. 支持变参；
    2. 支持打印串中出现‘\n’【换行】、‘\f’【换页】控制字符；
    3. 如果打印数据包过长，则程序将溢出；
    4. 如果打印字符串超出打印范围，将自动换行后继续打印；
    5. 缓冲区最大为 2048 字节；
    6. 将 str 存入打印缓冲区，在调用 OsPrnStart( )后，按照写入缓冲区的顺序，开始打印缓冲区里的数据。
    */
void OsPrnPrintf(const char *Str, ...);

/*
功能 直接打印排版好的数据。
参数
    data【输入】 要打印的字符串指针。
    len【输入】 要打印的字符串指针的长度。
返回
    RET_OK 打印机工作正常
    ERR_INVALID_PARAM 非法参数
    ERR_DEV_NOT_OPEN 设备打开失败
    ERR_PRN_PAPEROUT 打印机缺纸
    ERR_PRN_OVERHEAT 打印机过热
    ERR_PRN_OVERVOLTAGE 打印机电压过高
    ERR_PRN_BUSY 打印机设备忙
用法
1. 在调用该函数后，打印机将进行打印，直到打印结束后才返回；
2. 在打印结束后，该函数会返回打印状态，因此可不必再查询打印状态；
*/
int OsPrnRawData (const char *data, int len);

/*
功能 输出图形到打印缓冲区内。
参数 Logo【输入】 需打印的 logo 文件路径。
返回 无
用法 图形数据的生成步骤如下：
1. 画 LOGO：可以使用 Windows 中的“开始/程序/附件/画图”工具画图形，图形的存盘要求为“单色、BMP 格式”；
2. 使用本公司提供的“图形转换工具”将该 BMP 图形文件转换成一个诸如 Logo.h 的头文件（可选择多个 BMP 文件，转换后的头文件中将包含与 BMP 相同数量的数组，数组名的定义与 BMP文件名相关）。
*/
void OsPrnPutImage(const unsigned char *Logo);
/*
功能 输出电子签名单色bmp数据到缓存区。
参数 SignData电子签名单色bmp数据
     len 数据长度
返回 无
用法 缓存只打印最后一次设置的电子签名（只缓存一条，数据小于10K
*/
void OsPrnPutElecSignData(unsigned char *SignData,int len);

/*
功能 取电子签名数据到打印缓冲区。
参数 无
返回 无
用法 缓存只打印最后一次设置的电子签名（只缓存一条，数据小于10K
*/
void OsPrnPutElecSignDataByJpg();
/*
功能 启动打印机，将缓冲区里的数据打印出来。
参数 无
返回
    RET_OK 打印机工作正常
    ERR_PRN_BUSY 打印机忙
    ERR_PRN_PAPEROUT 打印机缺纸
    ERR_PRN_WRONG_PACKAGE 数据包格式错误
    ERR_PRN_OVERHEAT 打印机过热
    ERR_PRN_OUTOFMEMORY 打印数据过大
用法
    1. 在调用该函数后，打印机将进行打印，直到打印结束后才返回；
    2. 在打印结束后，该函数会返回打印状态，因此可不必再查询打印状态；
    3. 如果打印机成功完成打印任务后，再次调用该函数，则重打一遍单据。
*/  
int OsPrnStart(void);

/*
功能 清空打印缓冲区
参数 无
返回 无
*/
void OsPrnClrBuf(void);

/*
功能 设置打印机是否预走纸
参数 cmd 【输入】 1：打印不预走纸
                 2：打印有预走纸
返回
    RET_OK 成功
    ERR_INVALID_PARAM 非法参数
用法 1. 函数只影响打印机预走纸功能，默认为有预走纸。2. 只支持热敏打印机。
*/
int OsPrnSetParam(unsigned int cmd);


/************************************13编码*********************************/

/*
功能 字符编码转换｡
参数
    FromCharset 【输入】 原有字符编码
    ToCharset【输入】 目标字符编码
    InBuf 【输入】 原有编码的字符串，以‘\0’为结尾符；unicode编码需要以‘\0\0’为结尾符。
    OutBuf 【输出】 转换后的目标编码字符串
    LenOut 【输入】 OutBuf 数组大小，至少为 InBuf 数组大小的1.5 倍。
返回
    >=0 转换成功，返回转换后的字符串长度
    ERR_INVALID_PARAM 非法参数
*/
int OsCodeConvert (const char *FromCharset, const char *ToCharset,const char *InBuf, char *OutBuf, unsigned int LenOut);

/************************************14磁条阅读器*********************************/
//14磁条阅读器
#define ERR_MSR_FAILED -2701 //操作失败
#define ERR_MSR_HEADERR -2702 //没有找到起始符
#define ERR_MSR_ENDERR -2703 //没有找到结束符
#define ERR_MSR_LRCERR -2704 //LEC检验错误
#define ERR_MSR_PARERR -2705 //磁道某位数据校验错误
#define ERR_MSR_NOT_SWIPED -2706 //没有刷卡
#define ERR_MSR_NO_DATA -2707 //磁条卡没有数据
#define ERR_MSR_END_ZEROERR -2708 //磁条卡数据格式错误
#define ERR_MSR_PED_DECRYPTERR -2709 //PED解密失败
#define ERR_MSR_NO_TRACK_ERR -2710 //没有检测到磁卡上对应的磁道

//ST_MSR_DATA 磁道信息结构体
typedef struct
{
    unsigned char TeackData[256]; /*解码后的位流*/
    int DataLen; /*磁道数据长度*/
    int Status; /*磁道数据状态，等于0时标识读取数据成功；其他值标识读取失败*/
}ST_MSR_DATA;


/*
打开磁条阅读器设备。
参数 无
返回
    RET_OK 成功
    ERR_DEV_NOT_EXIST 设备不存在
    ERR_DEV_BUSY 设备被占用
*/
int OsMsrOpen(void);
/*
关闭磁条读卡器设备。
参数 无
返回 无
用法 程序退出时需调用该函数关闭设备。
*/
void OsMsrClose(void);
/*
软复位磁条卡阅读器，并清除已读取的磁条卡数据。
参数 无
返回 无
*/
void OsMsrReset(void);
/*
检测是否刷卡。
参数 无
返回
    TRUE 已刷
    FALSE 未刷
    ERR_DEV_NOT_OPEN 设备未打开
*/
int OsMsrSwiped(void);
/*
读取磁条卡数据。
参数 Track1【输出】 输出 1 磁道数据
     Track2【输出】 输出 2 磁道数据
     Track3【输出】 输出 3 磁道数据
返回
    RET_OK 成功读取
    ERR_MSR_NOT_SWIPED 未刷卡
    ERR_INVALID_PARAM 非法参数
    ERR_DEV_NOT_OPEN 设备未打开
*/
int OsMsrRead(ST_MSR_DATA *Track1, ST_MSR_DATA *Track2, ST_MSR_DATA *Track3);
/*
读取通用的单面磁条卡或者编码方式为 JIS 的双面磁条卡的数据。
参数
    Track1【输出】 输出 1 磁道数据
    Track2【输出】 输出 2 磁道数据
    Track3【输出】 输出 3 磁道数据
    Track4【输出】 输出 4 磁道数据
返回
    RET_OK 成功读取
    ERR_MSR_NOT_SWIPED 未刷卡
    ERR_INVALID_PARAM 非法参数
    ERR_DEV_NOT_OPEN 设备未打开
*/
int OsMsrReadJIS(ST_MSR_DATA *Track1, ST_MSR_DATA *Track2, ST_MSR_DATA *Track3, ST_MSR_DATA *Track4);



/************************************15IC 卡读卡器*********************************/

#define ERR_SCI_HW_NOCARD -2800 //没有卡片
#define ERR_SCI_HW_STEP -2801//交换数据时没有初始化，热复位时未上电。
#define ERR_SCI_HW_PARITY -2802 //奇偶校验失败
#define ERR_SCI_HW_TIMEOUT -2803 //超时
#define ERR_SCI_TCK -2804 //TCK 错误
#define ERR_SCI_ATR_TS -2810 //ATR TS 错误
#define ERR_SCI_ATR_TA1 -2811 //ATR TA1 错误
#define ERR_SCI_ATR_TD1 -2812 //ATR TD1 错误
#define ERR_SCI_ATR_TA2 -2813 //ATR TA2 错误
#define ERR_SCI_ATR_TB1 -2814 //ATR TB1 错误
#define ERR_SCI_ATR_TB2 -2815 //ATR TB2 错误
#define ERR_SCI_ATR_TC2 -2816 //ATR TC2 错误
#define ERR_SCI_ATR_TD2 -2817 //ATR TD2 错误
#define ERR_SCI_ATR_TA3 -2818 //ATR TA3 错误
#define ERR_SCI_ATR_TB3 -2819 //ATR TB3 错误
#define ERR_SCI_ATR_TC3 -2820 //ATR TC3 错误
#define ERR_SCI_T_ORDER -2821 //协议不是 T0 或 T1
#define ERR_SCI_PPS_PPSS -2830 //PPS 上的 PPSS 错误
#define ERR_SCI_PPS_PPS0 -2831 //PPS 上的 PPS0 错误
#define ERR_SCI_PPS_PCK -2832 //PPS 上的 ATR PCK 错误
#define ERR_SCI_T0_PARAM -2840 //磁道 0 上的传输的数据太长。
#define ERR_SCI_T0_REPEAT -2841 //磁道0上字符重复超过最大限制
#define ERR_SCI_T0_PROB -2842 //T0 发生程序字节错误
#define ERR_SCI_T1_PARAM -2850 //在 T1 中传输的数据太长
#define ERR_SCI_T1_BWT -2851 //T1 BWT 过大
#define ERR_SCI_T1_CWT -2852 //T1 CWT 过大
#define ERR_SCI_T1_BREP -2853 //T1 太多块重复
#define ERR_SCI_T1_LRC -2854 //T1LRC 错误
#define ERR_SCI_T1_NAD -2855 //T1NAD 错误
#define ERR_SCI_T1_LEN -2856 //T1 LEN 错误
#define ERR_SCI_T1_PCB -2857 //T1 PCB 错误
#define ERR_SCI_T1_SRC -2858 //T1SRC 错误
#define ERR_SCI_T1_SRL -2859 //T1SRL 错误
#define ERR_SCI_T1_SRA -2860 //T1SRA 错误
#define ERR_SCI_PARAM -2880 //非法参数

typedef struct
{
    unsigned char Cmd[4]; /*CLA, INS, P1, P2*/
    int LC; /*发送到 IC 卡的 DataIn 有效数据长度 */
    unsigned char DataIn[512]; /*发送到 IC 卡的数据*/
    int LE; /*期望返回数据的长度*/
}ST_APDU_REQ;

typedef struct
{
    int LenOut; /* 实际从 IC 卡返回的数据长度*/
    unsigned char DataOut[512]; /*从 IC 卡返回的数据指针*/
    unsigned char SWA; /*IC 卡状态字 1*/
    unsigned char SWB; /*IC 卡状态字 2*/
}ST_APDU_RSP;

enum{
    ICC_USER_SLOT = 0,   //用户卡
    ICC_SAM1_SLOT, //SAM 卡卡座 1
    ICC_SAM2_SLOT, //SAM 卡卡座 2
    ICC_SAM3_SLOT, //SAM 卡卡座 3
    ICC_SAM4_SLOT, //SAM 卡卡座 4
};

/*
功能 打开 IC 卡读卡设备｡
参数 Slot
    IC 卡通道号：
    ICC_USER_SLOT 用户卡
    ICC_SAM1_SLOT SAM 卡卡座 1
    ICC_SAM2_SLOT SAM 卡卡座 2
    ICC_SAM3_SLOT SAM 卡卡座 3
    ICC_SAM4_SLOT SAM 卡卡座 4
返回
    RET_OK 成功
    ERR_DEV_NOT_EXIST 设备不存在
    ERR_DEV_BUSY 设备被占用
*/
int OsIccOpen(int Slot);

/*
功能 检查指定的卡座是否有卡插入。
参数 Slot
    IC 卡通道号：
    ICC_USER_SLOT 用户卡
    ICC_SAM1_SLOT SAM 卡卡座 1
    ICC_SAM2_SLOT SAM 卡卡座 2
    ICC_SAM3_SLOT SAM 卡卡座 3
    ICC_SAM4_SLOT SAM 卡卡座 4
返回
    RET_OK 有卡插入
    其他 请参考函数返回值列表
*/
int OsIccDetect(int Slot);

/*
功能 初始化 IC 卡设备。
参数
Slot
IC 卡通道号：
ICC_USER_SLOT 用户卡
ICC_SAM1_SLOT SAM 卡卡座 1
ICC_SAM2_SLOT SAM 卡卡座 2
ICC_SAM3_SLOT SAM 卡卡座 3
ICC_SAM4_SLOT SAM 卡卡座 4
Option(位 0~1)卡电压选择：00 - 5V， 01 - 1.8V，10 - 3V
(位 2) 0；
(位 3~4) 0
(位 5)表示支持的规范类型：0 - EMV 规范  1 - ISO7816 规范
(位 6 ~31)保留：Option 缺省设置为 0(即：5V、非 PPS、标准速率和遵循 EMVx)
Atr【输出】
1. 卡片复位应答(Answer To Reset)，卡片最多返回 34 字节的响应数据。
2. 其内容为：复位应答内容长度(1 字节)+复位应答内容
返回
RET_OK 成功
其他 请参考函数返回值列表
*/
int OsIccInit(int Slot,unsigned long Option,unsigned char *Atr);

/*
功能 与 IC 卡进行命令交互。
参数
Slot
    IC 卡通道号：
    ICC_USER_SLOT 用户卡
    ICC_SAM1_SLOT SAM 卡卡座 1
    ICC_SAM2_SLOT SAM 卡卡座 2
    ICC_SAM3_SLOT SAM 卡卡座 3
    ICC_SAM4_SLOT SAM 卡卡座 4
CtrlFlag
1. Bit0 表示 T=0 协议下是否自动发送
GET RESPONSE 指令：
1-是
0-否
2. Bit1~Bit31 保留
ApduReq【输入】 发送给 IC 卡命令数据结构
ApduRsp【输出】 接收从 IC 卡响应的数据结构
返回
    RET_OK 成功
    其他 请参考函数返回值列表
*/
int OsIccExchange(int Slot,int CtrlFlag,const ST_APDU_REQ *ApduReq,ST_APDU_RSP *ApduRsp);

/*
功能 透明传输/接收功能。
参数 Slot
    IC 卡通道号：
    ICC_USER_SLOT 用户卡
    ICC_SAM1_SLOT SAM 卡卡座 1
    ICC_SAM2_SLOT SAM 卡卡座 2
    ICC_SAM3_SLOT SAM 卡卡座 3
    ICC_SAM4_SLOT SAM 卡卡座 4

CtrlFlag 保留
pucTxBuff【输入】 待传送数据缓冲区
iTxLen 【输入】 待传送数据的长度，以字节为单位
pucRxBuff【输出】 接收卡片响应数据的缓冲区
piRxLen【输出】 接收到的卡片数据字节数
返回
0 成功
其它 失败(参见函数返回值列表)
*/
int OsIccTransfer(int Slot,int CtrlFlag, const unsigned char *pucTxBuff,int iTxLen,unsigned char *pucRxBuff,int *piRxLen);
/*
功能 关闭 IC 卡设备。
参数 Slot
    IC 卡通道号：
    ICC_USER_SLOT 用户卡
    ICC_SAM1_SLOT SAM 卡卡座 1
    ICC_SAM2_SLOT SAM 卡卡座 2
    ICC_SAM3_SLOT SAM 卡卡座 3
    ICC_SAM4_SLOT SAM 卡卡座 4
返回
    RET_OK 成功
    其他 请参考函数返回值列表
*/
int OsIccClose(int Slot);


/************************************16射频读卡器*********************************/
#define PCD_ERR_PAR_FLAG -2901 //校验错误
#define PCD_ERR_CRC_FLAG -2902 //CRC 错误
#define PCD_ERR_WTO_FLAG -2903 //超时或无卡
#define PCD_ERR_COLL_FLAG -2904 //多卡冲突
#define PCD_ERR_ECD_FLAG -2905 //帧格式错误
#define PCD_ERR_EMD_FLAG -2906 //干扰
#define PCD_ERR_COM_FLAG -2907 //芯片故障,无法正确通信
#define PCD_ERR_AUT_FLAG -2908 //M1 认证错误
#define PCD_ERR_TRANSMIT_FLAG -2909 //传输错误
#define PCD_ERR_PROTOCOL_FLAG -2910 //协议错误
#define PCD_ERR_PARAMFILE_FLAG -2911 //配置文件不存在
#define PCD_ERR_USER_CANCEL -2912 //用户取消交易
#define PCD_ERR_CARRIER_OBTAIN_FLAG -2913 //载波未获得
#define PCD_ERR_CONFIG_FLAG -2914 //配置寄存器不成功
#define PCD_ERR_RXLEN_EXCEED_FLAG -2915 //卡片返回数据长度超过设定接收长度
#define PCD_ERR_NOT_ALLOWED_FLAG -2951 //参数错误或取值不允许
#define PCD_CHIP_ABNORMAL -2952 //芯片不存在或异常
#define PCD_CHIP_NOT_OPENED -2953 //模块未打开
#define PCD_CHIP_CARDEXIST -2954 //卡片未移开
#define PCD_ERR_NOT_IDLE_FLAG -2955 //卡片没有在 idle 态
#define PCD_ERR_NOT_POLLING_FLAG -2956 //卡片没有进行 POLLING
#define PCD_ERR_NOT_WAKEUP_FLAG -2957 //卡片没有唤醒
#define PCD_ERR_NOT_ACTIVE_FLAG -2958 //卡片未激活
#define PCD_ERR_NOT_SUPPORT -2959 //芯片不支持

typedef struct pcd_user_t{
    unsigned char wait_retry_limit_w; /* S(WTX)响应发送次数写入允许*/
    unsigned int wait_retry_limit_val; /* S(WTX)响应最大重试次数*/
    unsigned char check_cancel_key_w; /*响应取消键写入允许*/
    unsigned char check_cancel_key_val; /* 0 表示不响应取消键，1 表示响应取消键*/
    int (*check_cancel_key_function)(void);/*检测是否按下取消键函数。如果设置 check_cancel_key_w=1 及check_cancel_key_val=1。则在射频卡交易过程中会调用chec_cancel_key_function()。如果check_cancel_key_function()返回 0，表示没有按下取消键，如果返回非 0，表示已按下取消键，会强行退出交易*/
    unsigned char os_picc_transfer_set_w; /*1 表示os_picc_transfer_set_val 值有效，0表示 os_picc_transfer_set_val 值无效*/
    unsigned char os_picc_transfer_set_val; /* OsPiccTransfer 收发设置,Bit0=0，表示发送禁用 CRC,Bit0=1，表示发送使能 CRC,Bit1=0，表示接收禁用 CRC,Bit1=1，表示接收使能 CRC*/
    unsigned char anti_interference_flag; /*寻卡抗干扰功能设置; 1-启用寻卡抗干扰功能，0-不启用寻卡抗干扰功能*/
    unsigned char protocol_layer_fwt_set_w; /*1表示 protocol_layer_fwt_set_val 值有效，0 表示 protocol_layer_fwt_set_val值无效*/
    unsigned int protocol_layer_fwt_set_val; /*设置协议层的帧等待时间 FWT值*/
    unsigned char os_picc_transfer_rxlen_set_w; /*1 表示os_picc_transfer_rxlen_set_val值有效，0 表示os_picc_transfer_rxlen_set_val值无效*/
    unsigned int os_picc_transfer_rxlen_set_val; /*OsPiccTransfer 半双工块传输中设置最大可接收数据长度值*/
    unsigned char os_picc_transfer_direct_transmit_set_w; /* 1 表示使用数据流的方式进行数据传输，即不适用半双工块传输协议；0 表示使用半双工块传输协议进行数据传输*/
    unsigned char configure_technology_type_w; /* 设 置configure_technology_type_val 参数是否可以写入：1--允许，其它值—不允许*/
    unsigned char configure_technology_type_val; /*配置 OsPiccTransfer 发送数据的调制技术类型*/
    unsigned char reserved[34]; /*保留字节，用于将来扩展，sizeof(PCD_USER_ST) = 76*/
}PCD_USER_ST;

/*
功能 PCD 模块上电处理，使模块进入准备工作状态。
参数 无
返回
    0 打开设备成功
    ERR_BATTERY_VOLTAG
    E_TOO_LOW 电池电压过低
    ERR_BATTERY_ABSENT 电池不存在
    其它 打开设备失败(参见函数返回值列表)
*/
int OsPiccOpen(void);

/*
功能 PCD 模块下电处理。
参数 无
返回
    0 成功
    其它 失败(参见函数返回值列表)
*/
int OsPiccClose(void);

/*
功能 载波复位。
参数 无
返回
0 载波复位成功
其它 载波复位失败(参见函数返回值列表)
*/
int OsPiccResetCarrier(void);


/*
功能 寻卡，暂时只包括“A”和“B”卡两种类型的轮寻。
参数
    pcPiccType【输出】
    寻到卡片类型：
    “A” - A 卡
    “B” - B 卡
    pucATQx 【输出】
    寻卡时卡片应答数据：
    A 卡为 2 字节
    B 卡为 12 字节
返回
    0 寻卡成功
    其它 寻卡失败(参见函数返回值列表)
*/
int OsPiccPoll(char*pcPiccType, unsigned char*pucATQx);


/*
功能 透明传输/接收功能。
参数 pucTxBuff【输入】 待传送数据缓冲区
    iTxLen 【输入】 待传送数据的长度，以字节为单位
    pucRxBuff【输出】 接收卡片响应数据的缓冲区
    piRxLen【输出】 接收到的卡片数据字节数
返回
    0 成功
    其它 失败(参见函数返回值列表)
*/
int OsPiccTransfer(const unsigned char*pucTxBuff,int iTxLen, unsigned char*pucRxBuff, int *piRxLen);

/*
功能 按照 EMV 模式进行移卡操作。
参数 无
返回
    0 成功
    其它 失败
*/
int OsPiccRemove (void);

/*
功能 在指定的通道上，向卡片发送 APDU 格式的数据，并接收响应。
参数
    cid 【输入】 用于指定卡片逻辑通道号。
    其取值范围为 0~14，取值均为 0。
    ApduReq 【输入】 发送给 PICC 卡 命 令 数 据 结 构
    ST_APDU_REQ
    ApduRsp 【输出】 从 PICC 卡 返 回 的 数 据 结 构
    ST_APDU_RSP
返回
    0 成功
    其它 失败(参见函数返回值列表)
*/
int OsPiccIsoCommand(int cid, ST_APDU_REQ*ApduReq,ST_APDU_RSP*ApduRsp);

/*
功能 关闭载波。
参数 无
返回
    0 关闭载波成功
    其它 失败(参见函数返回值列表)
用法 对非接触 IC 卡读卡器进行载波关闭操作，射频场内卡片的状态将变为下电状态。
*/
int OsPiccOffCarrier(void);

/************************************17 通讯端口*********************************/
/*
功能 将usb切换成串口。
参数 无
返回
    0 成功
    其它 失败
*/
int OsStartUsbSerial();

/*
功能 获取串口端口号。
参数 SerialPort【输出】，不能为NULL
返回
    0 成功
    其它 失败
*/
int OsQuerySerialPort(char* SerialPort);
/*
功能 将串口切换成usb。
参数 无
返回
    0 成功
    其它 失败
*/
int OsStopUsbSerial();

int OsStartCommModuleUsbSerial();
int OsStopCommModuleUsbSerial();
int OsStartUsbSerialWithUsbcharge();

/************************************21GPRS/CDMA 无线模块*********************************/
#define ERR_NET_IF -3307  //网络接口链路不可用(链路没有建立或没有相应的设备)

#define PPP_LOGINING 1  //PPP 正在登录中
#define PPP_LOGOUTING 2 //PPP 正在退出中
#define WL_CSD_READY 3 //CSD 拨号业务就绪
#define WL_GPRS_CSD_READY 4 //GPRS 和 CSD 拨号业务就绪
#define ERR_WL_POWER_ONING -3501 //无线模块上电中
#define ERR_WL_POWER_OFF -3502 //无线模块没上电
#define ERR_WL_NOT_INIT -3503 //模块没有被初始化
#define ERR_WL_NEEDPIN -3504 //SIM 卡需要 PIN 
#define ERR_WL_RSPERR -3505 //模块响应错误
#define ERR_WL_NORSP -3506 //模块没有响应
#define ERR_WL_NEEDPUK -3507 //SIM 卡需要 PUK 
#define ERR_WL_WRONG_PIN -3508 //SIM 卡的 PIN 错误
#define ERR_WL_NOSIM -3509 //没有插入 SIM 卡
#define ERR_WL_NOREG -3510 //不能注册到网络
#define ERR_WL_AUTO_RST -3511 //模块自动复位
#define ERR_WL_BUF -3512 //无线模块内存错误
#define ERR_WL_GET_SIGNAL -3513 //获取信号中,请等待 3 秒
#define ERR_WL_NOTYPE -3514 //模块不能识别
#define ERR_WL_PPP_ONLINE -3515 //模块 PPP 在线,不能进行休眠
#define ERR_WL_ERR_BUSY -3516 //模块忙
#define ERR_WL_SLEEP_ONING -3517 //无线模块正在进入休眠
#define ERR_WL_SLEEP_FAIL -3518 //无线模块进入休眠时失败
#define ERR_WL_SIM_FAILURE -3519 //无线模块操作 SIM 卡失败

/*
功能 打开无线模块。建立与 Prolin 无线服务的连接，并获取无线设备/模块的使用权限。
参数 无
返回
    RET_OK 打开无线模块成功
    ERR_DEV_BUSY 模块/设备已经被其他应用程序占用
    ERR_DEV_NOT_EXIST 模块/设备不存在
    ERR_BATTERY_VOLTA
    GE_TOO_LOW 电池电压过低
    ERR_BATTERY_ABSENT 终端电池不存在
*/
int OsWlLock(void);

/*
原型 void OsWlUnLock(void);
功能 释放无线设备的使用权，并断开与 Prolin 无线服务的连接。
参数 无
返回 无
*/
void OsWlUnLock(void);

/*
功能 初始化无线设备。
参数 SimPin【输入】
SIM 卡的密码字符串指针，长度小于 50字节。可以为 NULL，表示不需要密码。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备/模块没打开
    ERR_DEV_NOT_EXIST 无线设备/模块不存在
    ERR_NO_PORT 终端物理串口不足
    ERR_WL_NEEDPIN SIM 卡需要 PIN 
    ERR_WL_RSPERR 模块/设备响应错误
    ERR_WL_NORSP 模块没有响应
    ERR_WL_NEEDPUK SIM 卡需要 PUK 
    ERR_WL_WRONG_PIN PIN 错误
    ERR_WL_NOSIM 无 SIM 卡
    ERR_WL_NOTYPE 模块类型无法识别
    ERR_WL_NOREG 无法注册到 GPRS 网络
*/
int OsWlInit(const char *SimPin);

/*
功能 获取无线信号强度。
参数 无
返回
    0~5 信号强度级别：0 表示没信号；5
    表示最强信号。
    ERR_DEV_NOT_EXIST 无线模块不存在
    ERR_WL_RSPERR 模块响应错误
    ERR_WL_POWER_ONING 无线模块处于下电状态
*/
int OsWlGetSignal(void);

/*
功能 查询无线链路状态。
参数 无
返回
    PPP_LOGOUTING 正在断开链路
    PPP_LOGINING 正在创建链路
    RET_OK 创建链路成功
    ERR_DEV_NOT_EXIST 无线模块不存在
    ERR_WL_POWER_ONING 无线模块上电中
    ERR_WL_POWER_OFF 无线模块下电
    ERR_WL_NOT_INIT 模块没有被初始化
    ERR_NET_PASSWD 密码错误
    ERR_NET_LOGOUT 应用主动断开
    ERR_NET_IF 链路不可用，表示链路没有建立或已断开
*/
int OsWlCheck(void);

/*
功能 登录无线网络，建立无线链路。
参数
    APN【输入】
    GPRS 为无线接入点名称(AccessPointName)，
    CDMA 为拨号号码。长度不超过 50 个字符；当为 NULL 时，应用先自己拨号后，协议栈直接进行 PPP 登录。
    Name【输入】用户名，长度不能超过 50 个字节；不允许为 NULL；没有用户名时，用空字符串“”表示。
    Password【输入】 密码，长度不能超过 50 个字节；不允许为NULL；没有密码时，用空字符串“”表示。
    Auth 认证时采用的算法
    TimeOutMs 超时时间，单位为毫秒；取值范围为 0~3600000。小于 0 时，自动设置为 0；大于 3600000 时，自动设置为 3600000。
    KeepAlive 链路检查间隔，单位为毫秒；取值范围为0~3600000；当为 0 时，不启用 KeepAlive 功能；当为 0~10000 时，自动设置为 10000；当为 10000~3600000 时会根据设定的值进行链路检查。
    ReserParam 保留参数，供扩展使用。
返回
    PPP_LOGINING 处理中
    PPP_LOGOUTING 无线模块正在退出登录
    RET_OK 链路建立成功
    ERR_DEV_NOT_EXIST 没有无线模块
    ERR_DEV_NOT_OPEN 没有执行 OsWlLock 成功
    ERR_INVALID_PARAM 非法参数
    ERR_WL_POWER_ONING 无线模块上电中
    ERR_WL_POWER_OFF 无线模块没上电
    ERR_WL_NOT_INIT 没有初始化成功
    ERR_NET_PASSWD 错误密码
    ERR_NET_SERVER_BUSY 服务器忙，通信失败
    ERR_NET_AUTH 无法连接上 RADIUS 服务器
*/
int OsWlLogin(const char *APN, const char *Name, const char *Password, long Auth, int TimeOutMs, int KeepAlive, int ReserParam);

/*
功能 退出无线网络，断开无线链路。
参数 无
返回
    PPP_LOGOUTING 链路断开中
    ERR_DEV_NOT_OPEN 模块/设备没打开
*/
int OsWlLogout(void);

//TODO 新增接口
//获取sim卡状态
int OsWlGetSimStatus(int* simStatus);
//获取运营商
int OsWlGetSimOperator(char* simOperator);
//获取IMEI
int OsWlGetImei(char* Imei);
//获取Imsi
int OsWlGetImsi(char* Imsi);
//获取Iccid
int OsWlGetIccid(char* Iccid);
//获取信号强度
int OsWlGetSignalStrength(void);
//获取模块REG
int OsWlGetModuleREG(int *state, char* lac, char* cid);
//获取IP
int OsWlGetNetworkIp(char* ipaddr);

int OsWlGetQNetworkInfo(char* networkType, char *oper);
// //获取自动连接状态
// int OsGetAutoConnectStatus(int *status);
// //设置自动连接状态
// int OsSetAutoConnectStatus(int onoff);
//获取无线自动连接状态
int OsWlGetAutoConnectStatus(int *status);
//设置自动连接状态
int OsWlSetAutoConnectStatus(int onoff);
//设置飞行模式
int OsSetAirplaneMode(int onoff);
//获取飞行模式状态
int OsGetAirplaneMode(int *onoff);
//设置APN
int OsSetApnParams(char* apn, char* user,char* password);
//获取APN
int OsGetApnParams(char* apn, char* user,char* password);
//获取当前网络类型  type 0：NOSERVICE无网络, 1:GSM 2G, 2:LTE 4G
int OsWLGetNetworkType(int* type);
//打开发送AT
int OsCustATCmdOpen();
//发送AT
int OsSendCustATCmd(char* atCmd, char* resp);
//关闭发送AT
void OsCustATCmdClose();
//模块下电
int OsWLModulePowerDown(void);
//模块上电
int OsWLModulePowerOn(void);
/************************************22 WiFi*********************************/
//函数返回值列表
#define ERR_MODE_NOT_SUPPORT -3350 //模式设置错误
#define ERR_WIFI_POWER_OFF -3351 //模块未上电
#define ERR_NOT_FOUND_AP -3352 //没有找到 AP
#define ERR_AUTH_SEC_MODE -3353 //认证模式或加密模式错误
#define ERR_WIFI_BAD_SIGNAL -3354 //WiFi 信号差
#define ERR_NET_PASSWD -3355 //密码错误
#define RET_CONNECTING 1 //正在连接
#define ERR_EAP_ID -3359 //证书链错误或者证书校验失败

//加密类型列表
#define PARE_CIPHERS_NONE 0x00000000 //无加密
#define PARE_CIPHERS_WEP64 0x00000001 //WEP 40bit 密钥
#define PARE_CIPHERS_WEP128 0x00000002 //WEP 104bit 密钥
#define PARE_CIPHERS_WEPX 0x00000004 //WEP 加密，未知密钥位
#define PARE_CIPHERS_CCMP 0x00000010 //AES 加密方式
#define PARE_CIPHERS_TKIP 0x00000020 //TKIP 加密方式

//认证模式:
enum WIFI_AUTH_MODE{
    AUTH_NONE_OPEN=1,
    AUTH_NONE_WEP,
    AUTH_NONE_WEP_SHARED, /*该模式在扫描时为 AUTH_NONE_WEP */
    AUTH_IEEE8021X,
    AUTH_WPA_PSK,
    AUTH_WPA_EAP,
    AUTH_WPA_WPA2_PSK,
    AUTH_WPA_WPA2_EAP,
    AUTH_WPA2_PSK,
    AUTH_WPA2_EAP
};

//WEP64、WEP128 扩展
typedef struct _WepSecKey{
    char Key[4][40]; /* WEP 密钥数据 */
    int KeyLen; /* WEP 密钥长度 */
    int Idx; /* WEP 密钥索引[0,3] */
} WEP_SEC_KEY;

//WPA/WPA2-PSK 扩展
typedef struct _WpaPskKey{
    char Key[64]; /* PSK-密钥数据 */
    int KeyLen; /* PSK-密钥长度 */
} WPA_PSK_KEY;

//EAP 扩展
typedef struct _WpaEapKey{
    int EapType; /* EAP 类型 */
    char Pwd[132]; /* 密码 */
    char Id[132]; /* 特性 */
    char CaCert[132]; /* CA 证书的路径和文件名 */
    char CliCert[132];/* 客户端证书的路径和文件名*/
    char PriKey[132]; /* 文件路径到客户端的私钥文件 */
    char PriKeyPwd[132]; /* 密码私钥文件*/
} WPA_EAP_KEY;

//扫描热点信息
typedef struct _WifiApInfo
{
    char Essid[33];/* AP 名字 */
    char Bssid[20];/* MAC 地址*/
    int Channel; /* 信道 */
    int Mode; /* 连接方式, 0:Station; 1:IBSS */
    int Rssi; /* 信号值，范围【-99，0】*/
    int AuthMode; /* 认证模式*/
    int SecMode; /* 加密模式, NONE,WEP,TKIP,CCMP */
}ST_WifiApInfo;

//连接热点设置:
typedef struct _WifiApSet{
    char Essid[33]; /* AP 名字，最大支持 32 字节，以’\0’为结尾符*/
    char Bssid[20];/*MAC 地址，以’\0’为结尾符；如果没有相同 ESSID 的 AP，Bssid 可以为’\0’*/
    int Channel; /* 信道，只在 IBSS 模式下有效，0:默认设置*/
    int Mode; /* 连接方式， 0:Station; 1:IBSS */
    int AuthMode; /* 认证模式*/
    int SecMode; /* 加密模式, NONE,WEP,TKIP,CCMP */
    union KEY_UNION{ /* 密钥设置 */
        WEP_SEC_KEY WepKey; /* WEP 模式*/
        WPA_PSK_KEY PskKey; /* wpa,wpa2-psk 模式*/
        WPA_EAP_KEY EapKey; /* wpa,wpa2-eap 模式*/
    } KeyUnion;
}ST_WifiApSet;

//WPS 模式
typedef enum 
{
    WPS_MODE_PBC = 1,    /* 按键方式连接，也称为 PBC 连接*/
    WPS_MODE_PIN_CLIENT, /*使用终端产生的 PIN 码连接*/
    WPS_MODE_PIN_AP      /*使用 AP 端的 PIN 码连接*/
}WPS_MODE;

/*
功能 连接 WiFi 服务并获取使用权。
参数 无
返回
    RET_OK 成功
    ERR_DEV_NOT_EXIST WiFi 模块驱动加载不正常或者模块错误
    ERR_DEV_BUSY WiFi 已经被使用
    ERR_BATTERY_VOLTAG
    E_TOO_LOW 电池电压过低
    ERR_BATTERY_ABSENT 电池不存在
*/
int OsWifiOpen(void);

/*
功能 释放 WiFi 模块的使用权，调用该函数后，WiFi 通信不受影响。
参数 无
返回 无
*/
void OsWifiClose(void);


/*
功能 扫描已存在的网络。
参数 Aps【输出】 扫描热点信息结构体 ST_WifiApInfo，存放扫描到的网络的信息
返回
    >=0 搜索到 AP 的个数
    ERR_MEM_FAULT 内存错误
    ERR_INVALID_PARAM 参数错误
    ERR_WIFI_POWER_OFF WiFi 模块没有上电
    ERR_DEV_NOT_OPEN 未获取 WiFi 设备使用权
用法：Aps用完需要释放
*/
int OsWifiScan (ST_WifiApInfo **Aps);

/*
功能 连接指定的无线网络。
参数
Ap【输入】 连接热点设置结构体 ST_WifiApSet，存放指定的无线网络属性。
TimeOutMs【输入】 超时时间，【单位：毫秒】；取值范围为 0~3600000。
返回
    RET_OK 连接成功
    RET_CONNECTING 正在连接中
    ERR_NOT_CONNECT 连接失败
    ERR_WIFI_BAD_SIGNAL WiFi 信号差
    ERR_NOT_FOUND_AP 没有找到 AP
    ERR_NET_PASSWD 密码错误
    ERR_AUTH_SEC_MODE 认证模式或加密模式错误
    ERR_WIFI_POWER_OFF WiFi 模块没有上电
    ERR_DEV_NOT_OPEN 未获取 WiFi 设备使用权
    ERR_INVALID_PARAM 参数错误
*/
int OsWifiConnect(const ST_WifiApSet *Ap,int TimeOutMs);

/*
功能 断开与当前网络的连接。
参数 无
返回
    RET_OK 已经断开连接
    ERR_DEV_NOT_OPEN 未获取 WiFi 设备使用权
*/
int OsWifiDisconnect(void);

/*
功能 获取终端当前的网络状态。
参数
Essid【输出】 当前已连接网络的 ESSID，不能为 NULL，长度为 33 字节。
Bssid【输出】 当前已连接网络的 BSSID，可以为 NULL，长度为 20 字节。
Rssi【输出】 信号强度，不能为 NULL。取值范围为【-99，0】，0 表示信号最强。
返回
    RET_OK 连接成功
    RET_CONNECTING 正在连接中
    ERR_NOT_CONNECT 没有连接到网络
    ERR_WIFI_BAD_SIGNAL WiFi 信号差
    ERR_NOT_FOUND_AP 没有找到 AP
    ERR_NET_PASSWD 密码错误
    ERR_AUTH_SEC_MODE 认证模式或加密模式错误
    ERR_INVALID_PARAM 参数错误
*/
int OsWifiCheck(char *Essid,char *Bssid,int *Rssi);

/*
功能 向 WPA_Supplicant 后台服务发送命令，并获取返回结果。
参数
    Argv【输入】 WPA_Supplicant 支持的命令，不能为 NULL。
    Argc【输入】 Argv 二维数组存放命令或参数的个数。
    Result【输出】 WPA_Supplicant 返回结果，不能为 NULL，长度大于 2048 字节。
    Len【输入】 Result 数组长度
返回
    RET_OK 发送成功
    ERR_INVALID_PARAM 参数错误
    ERR_WIFI_POWER_OFF WiFi 模块没有上电
    ERR_DEV_NOT_OPEN 未获取 WiFi 设备使用权
*/
int OsWifiCmd (const char *Argv[],int Argc,char *Result,int Len);

//检测wifi状态
int OsGetWifiStatus(char *Essid,char *Bssid,char* Ip,int *Rssi);
//检测wifi信号轻度  0未连接  1~4格
int OsGetWifiSignalStrength();

int OsWifiGetAutoConnectStatus(int *status);

int OsWifiSetAutoConnectStatus(int onoff);

int OsWifiReconnect();

int OsWifiModuleWakeUp();

int OsWifiModuleSleep();
/************************************29电源管理*********************************/
//29电源管理
#define BATTERY_LEVEL_0 0
#define BATTERY_LEVEL_1 1
#define BATTERY_LEVEL_2 2
#define BATTERY_LEVEL_3 3
#define BATTERY_LEVEL_4 4
#define BATTERY_LEVEL_CHARGE 5
#define BATTERY_LEVEL_COMPLETE 6
#define BATTERY_LEVEL_ABSENT 7

//PM_MSG_T pm广播出来的事件
typedef enum{
    PM_MSG_NO_EVEN, /*没有信息*/
    PM_MSG_ENTER_SLEEP, /*机器进入休眠*/
    PM_MSG_EXIT_SLEEP,  /*机器退出休眠*/
    PM_MSG_ENTER_SCREENSAVER,   /*机器进入屏保*/
    PM_MSG_EXIT_SCREENSAVER,    /*机器退出屏保*/
    PM_MSG_ENTER_POWEROFF,  /*机器开始关机*/
    PM_MSG_OWER_ABORMAL,    /*机器供电异常*/
    PM_MSG_VBATTERY_DAMAGE, /*机器电池损坏*/
}PM_MSG_T;

//PM_REQ_T 客户端请求的动作
typedef enum{
    PM_FORBID_SLEEP,    /*阻止机器进入休眠*/
    PM_ALLOW_SLEEP,    /*允许机器进入休眠*/
    PM_FORBID_SCREENASVER,    /*阻止机器进入屏保*/
    PM_ALLOW_SCREENASVER,    /*允许机器进入屏保*/
    PM_FORBID_POWEROFF,    /*阻止机器关机*/
    PM_ALLOW_POWEROFF,    /*允许机器关机*/
}PM_REQ_T;

//POWER_TYPE 供电类型
typedef enum{
    POWER_ADAPTER = 1,    /*适配器供电*/
    POWER_USB,    /*USB外设供电*/
    POWER_BATTERY,    /*电池供电*/
    POWER_WPC,    /*无限底座供电*/
}POWER_TYPE;

typedef enum { 
    WAKEUP_SRC_NONE = 0, /* 没有进行过休眠唤醒，无唤醒源 */ 
    WAKEUP_SRC_AP,/* AP唤醒 */ 
    WAKEUP_SRC_RTC,/* RTC唤醒 */ 
    WAKEUP_SRC_KB,/* 按键唤醒 */ 
} WAKEUP_SOURCE; 

//检测电池电量
int OsCheckBattery(void);
//检测供电类型
int OsCheckPowerSupply(void);
//重启机器
int OsReboot(void);
//关闭终端
int OsPowerOff(void);
//新增接口
//获取电池电量
int OsGetBatCap(void);
//获取电池电压
int OsGetBatVol(void);
//获取充电状态
int OsGetBatChgState(void);

//设置一级休眠时间
int OsSysSleepTime(int Time);

int OsGetSysSleepTime(int *Time);

//设置二级休眠时间
int OsSetSysDeepSleepTime(int Time);

int OsGetSysDeepSleepTime(int *Time);
//1允许休眠  0 不允许休眠
int OsSetSysSleepStatus(int status);

int OsGetSysSleepStatus(int *status);

//0无效 1Ap唤醒 2RTC唤醒 3按键唤醒
int OsWakeupSource(void);

/************************************自定义数据库接口*********************************/
typedef void*		HANDLE; // 通用指针定义
typedef int (*IsMatch)(const void *, int, const void *, int);

typedef enum DBINTERFACE_RET
{
    RET_INTERFACE_NOT_REC = -4,
	RET_INTERFACE_LENGTHERR = -3,
	RET_INTERFACE_PARAMERR = -2,
	RET_INTERFACE_FAILD    = -1,
	RET_INTERFACE_NORMAL   =  0,
}DB_INTERFACE_T;

//typedef PRecordCompareFunc IsMatch;
typedef struct _SearchingHelper
{
	void* 	pKeywords;
	int 	nLenOfKeyWords;
	int	nOffset;
	IsMatch fFilter;
}SearchingHelper_T;

typedef enum _tagSortType
{
	SORT_BY_NOTHNG = 0,	// NOT BE SORTABLE
	SORT_BY_USER_DEFINE,
}SortType;


/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_bInit
 * @函数功能: 数据库初始化
 * @输入参数:
 *			szAppID	传入当前APP ID
 *
 * @输出参数:		无
 * @返回值:
 *			成功：0
 *			失败：F他
 * @备注:
 *		使用数据库接口前，必须先进行数据库的初始化
 * @调用示例:
 */
int DB_bInit(char *szAppID);
/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_vUnInit
 * @函数功能: 数据库注销
 * @备注:
 *		退出应用时调用
 * @调用示例:
 */
void DB_vUnInit(void);
/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_bInitBySysService
 * @函数功能: 数据库初始化
 * @输入参数:
 * 			szDbName	传入数据库名称
 *			szDbName	传入数据库名称
 *
 * @输出参数:		无
 * @返回值:
 *			成功：0
 *			失败：其他
 * @备注:
 *		使用数据库接口前，必须先进行数据库的初始化
 * @调用示例:
 */
int DB_bInitBySysService(char* szDbPath,char *szDbName);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_hOpen
 * @函数功能: 打开指定记录表
 * @输入参数:
 *			szTableName				表名称
 *			nMaxLenOfRec单条记录最大长度		
 * @输出参数:		无
 * @返回值:
 *			成功： 返回表句柄；
 *			失败： NULL；
 * @备注:
 *
 * @调用示例:
 */

HANDLE DB_hOpen(const char* pszName, int nMaxLenOfRec);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_vClose
 * @函数功能: 关闭指定的表
 * @输入参数:
 *				handle	相应表句柄
 *
 * @输出参数:		无
 * @返回值:		无
 *
 * @备注:
 *
 * @调用示例:
 */
void  DB_vClose(HANDLE hTable);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_bClear
 * @函数功能: 清空表(不删除表)
 * @输入参数:
 *			hTable	相应表句柄
 * @输出参数:		无
 * @返回值:
 * 			成功：0
 * 			失败：其他
 * @备注:
 *
 * @调用示例:
 */
int DB_bClear(HANDLE hTable);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_bRemove
 * @函数功能: 删除表
 * @输入参数:
 *				pszName		表名
 *				nAccess(保留，未使用)	表访问权限(EM_ACCESS_PRIVATE – APP 私有， 其他应用无法访问,EM_ACCESS_SHARED — 共享方式访问，其他应用均可以访问)
 * @输出参数:		无
 * @返回值:
 * 			成功：0
 * 			失败：其他
 * @备注:
 *
 * @调用示例:
 */
int DB_bRemove(const char* pszName, int nAccess);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nReadRec
 * @函数功能: 通过索引的方式读取记录
 * @输入参数:
 *			hTable			表句柄
 *			nIndexOfRec		记录索引值(从0开始计)
 *			nLenOfBuffer	pRecordBuf所指向的buffer长度
 * @输出参数:
 *			pRecordBuf		返回记录内容
 * @返回值:
 * 			成功：返回记录实际的长度值
 * 			失败：RET_INTERFACE_FAILD
 * @备注:
 *
 * @调用示例:
 */
int DB_nReadRec(HANDLE hTable, int nIndexOfRec, int nLenOfBuffer, char *pRecordBuf);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nReadRecByKey
 * @函数功能: 通过关键字的方式读取记录  （速度快 推荐使用）
 * @输入参数:
 *			hTable			表句柄
 *			pKey			查询关键字 必须唯一
 *			nLenOfBuffer		pRecordBuf所指向的buffer长度
 * @输出参数:
 *			pRecordBuf		返回记录内容
 * @返回值:
 * 			成功：返回记录实际的长度值
 * 			失败：RET_INTERFACE_FAILD
 * @备注:
 *
 * @调用示例:
 */
int DB_nReadRecByKey(HANDLE hTable, char* pKey, int nLenOfBuffer, char *pRecordBuf);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nAppendRecByKey
 * @函数功能: 添加记录(带索引关键字 推荐使用)
 * @输入参数:
 *			hTable			表句柄
 *			pKey			查询关键字 必须唯一
 *			pRecordBuf		添加记录数据指针
 *			nRecLen			添加记录长度
 * @输出参数:	无
 *
 * @返回值:
 * 			成功：返回记录实际的长度值
 * 			失败：RET_INTERFACE_FAILD
 * @调用示例:
 */
int DB_nAppendRecByKey(HANDLE hTable, char* pKey, char* pRecordBuf, int nRecLen);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nReplaceRec
 * @函数功能: 更新记录
 * @输入参数:
 *			hTable			表句柄
 *			nIndexOfRec		表记录索引(从0开始计)
 *			pRecord			需要更新的数据
 *			nRecordLen		记录长度
 * @输出参数:	无
 *
 * @返回值:
 * 			成功：返回记录实际的长度值
 * 			失败：RET_INTERFACE_FAILD
 * @备注:
 *
 * @调用示例:
 */
int DB_nReplaceRec(HANDLE hTable, int nIndexOfRec, char* pRecord, int nRecordLen);


/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nReplaceRecByKey
 * @函数功能: 更新记录
 * @输入参数:
 *			hTable			表句柄
 *			pKey			查询关键字 必须唯一
 *			pRecord			需要更新的数据
 *			nRecordLen		记录长度
 * @输出参数:	无
 *
 * @返回值:
 * 			成功：返回记录实际的长度值
 * 			失败：RET_INTERFACE_FAILD
 * @备注:
 *
 * @调用示例:
 */
int DB_nReplaceRecByKey(HANDLE hTable, char* pKey,int nRecordLen,char* pRecord);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_bDeleteRec
 * @函数功能: 删除记录
 * @输入参数:
 *			hTable			表句柄
 *			nIndex		表记录索引(从0开始计)
 * @输出参数:	无
 *
 * @返回值:
 * 			成功：0
 * 			失败：其他
 * @备注:
 *
 * @调用示例:
 */
int DB_bDeleteRec(HANDLE hTable, int nIndex);

/*
 * @author:
 * @Date: 	06-03-2015
 * @Record: create it;
 *
 * @函数名称: DB_bDeleteRecByKey
 * @函数功能: 删除记录
 * @输入参数:
 *			hTable		表句柄
 *			pKey		查询关键字 必须唯一
 * @输出参数:	无
 *
 * @返回值:
 * 			成功：0
 * 			失败：其他
 * @备注:
 *
 * @调用示例:
 */
int DB_bDeleteRecByKey(HANDLE hTable, char* pKey);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_bIsExsit
 * @函数功能: 判断表名是否存在
 * @输入参数:
 *			pszTableName		表名
 *			nAccess				访问模式(保留)	
 *
 * @输出参数:	无
 *
 * @返回值:
 * 			存在：0存在
 * 			不存在：其他
 * @备注:
 *
 * @调用示例:
 */
int DB_bIsExsit(const char* pszTableName, int nAccess);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nGetRecordSUM
 * @函数功能: 获得有效记录的数量
 * @输入参数:
 *			hTable		表句柄
 *
 * @输出参数:	无
 *
 * @返回值:
 *			成功：记录总数；(>= 0)
 *			失败：RET_INTERFACE_FAILD
 * @备注:
 *
 * @调用示例:
 */
int DB_nGetRecordSUM(HANDLE hTable);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nGetLastError
 * @函数功能: 获得当前错误信息
 * @输入参数:
 *			 handle 	表句柄
 *
 * @输出参数:	 无
 * @返回值:
 			错误码
 *
 * @备注:
 *
 * @调用示例:
 */
int DB_nGetLastError(HANDLE hTable);


/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @函数名称: DB_nGetFirstRecord
 * @函数功能: 获取第一条记录
 * @输入参数:
 *			hTable			表句柄
 *			nIndexOfRec		索引指针(未使用)
 *			nRecLen			pRecordBuf所指向的buffer长度
 *
 * @输出参数:	pRecordBuf		-获取到的数据
 *
 * @返回值:
 *			成功：读到的实际长度
 *			失败：RET_INTERFACE_FAILD
 * @备注:
 *
 * @调用示例:
 */
int DB_nGetFirstRecord(HANDLE hTable, int *nIndexOfRec, void* pRecordBuf, int nRecLen);


#ifdef __cplusplus
}
#endif




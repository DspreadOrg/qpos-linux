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
    unsigned long Start;
    unsigned long Stop;
    unsigned long TimeLeft;
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

long get_sys_tick();
//功能 设置系统当前的日期和时间
int OsSetTime(const ST_TIME *Time);
//获取终端系统日期和时间。
void OsGetTime(ST_TIME *Time);
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
用法
1. 只有主应用才能切换应用，否则将返回 ERR_NEED_ADMIN；
2. OsRunApp()根据传入的参数 AppId 切换到指定子应用，但不可切换到主应用，如果 AppId 传入“MAINAPP”，将返回ERR_INVALID_PARAM；
3. 子应用的标准输出信息会输出到 CbOut 回调函数，标准错误信息会输出到 CbErr 回调函数，如有多行标准输出或者标准错误信息，则会多次调用回调函数；其回调函数定义为：typedef void (*RUNAPP_CB)(char *appid, char *str, void *data)
*/
int OsRunApp(char *AppId, char **Argv, void *Data, RUNAPP_CB CbOut,RUNAPP_CB CbErr);

/*
功能 获取子应用的退出码。
参数 无
返回 子应用的退出码
用法 调用 OsRunApp()之后，使用该函数可获取子应用的退出码。
*/
int OsGetAppExitCode(void);

/*
功能 设定注册表中指定的键值。
参数
    Key【输入】系统配置名字，长度不大于 31 个字节，以“persist.sys.”、“rt.sys.”或“rt.app.”开头“\0” 结尾。
    Value【输入】 系统配置参数值，长度不大于 64 个字节，以“\0”结尾，不能为 NULL。
返回
    RET_OK 成功
    ERR_INVALID_PARAM 非法参数，任意输入字符串指针为空指针。
    ERR_NEED_ADMIN 需要系统主应用权限
    ERR_SYS_NOT_SUPPORT 系统不支持此配置名
用法
    1. 只能设置以“persist.sys.”、“rt.sys.”和“rt.app.”开头的系统配置名字，例如“persist.sys.app0.pic”｡更多详情，请参考附录 3注册表。
    2. 该函数不仅支持设置附录 3 注册表中列出的关键字，应用同时也可以自定义使用。
    3. “rt.sys.”、“rt.app.”开头的注册表在重启后会失效，如果需要重启后能保持之前的状态，需要在重启后重新设置；
    4. 设置键值后，要延时约 200 毫秒才生效。
*/
int OsRegSetValue(const char *Key, const char *Value);

/*
功能 安装或更新应用软件、应用数据、OPT 包、用户公钥和外设固件(FWP包)。
参数
Name【输入】 指定的安装目的路径名。
FileName【输入】 待安装的文件路径，不能为 NULL。
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
    ERR_FILE_NOT_FOUND FileName 不存在
    ERR_FILE_FORMAT FileName 文件格式错误
    ERR_INVALID_PARAM 非法参数
    ERR_VERIFY_SIGN_FAIL 签名错误
    ERR_APP_MODE 模式错误
用法
    1. 只有当 FileType 为 FILE_TYPE_APP_PARAM 时，Name 为应用名字，比如“MAINAPP”等终端存在的应用名字，如果终端没有此应用名字，则安装失败；为其它类型时 Name 无效;
    2. 当 FileType 为 FILE_TYPE_FWP 时，如果是无线模块的设备固件包，带电池的机型需要至少 2 格电量，以保证固件包的正常安装。
    3. 只有主应用才有权限安装或更新。
    4. 当 FileType 为 FILE_TYPE_FWP 时，返回值有 RET_OK、ERR_FILE_NOT_FOUND 、 ERR_VERIFY_SIGN_FAIL 、ERR_ACCESS_DENY、ERR_NO_SPACE 和 ERR_APP_MODE；其中返回 ERR_APP_MODE 的情况包括 mmap()失败、fork()失败、execl()失败、FWP 包中的 updater 运行返回错误码。
*/
int OsInstallFile(const char *Name,const char *FileName,int FileType);

/*
功能 卸载指定的应用或 OPT 包。
参数
    AppName【输入】当 FileType 为 FILE_TYPE_APP，
    AppName 为需要删除的应用的 ID；当 FileType 为 FILE_TYPE_SYS_LIB，
    AppName 为 OPT 包名字。
    FileType 
        FILE_TYPE_APP 应用包(包括应用已经安装的所有文件)
        FILE_TYPE_SYS_LIB OPT 包(包括 OPT包所有文件)
返回
    RET_OK 成功
    ERR_APP_NOT_EXIST AppName 指定应用或 OPT 包不存在
    ERR_FONT_NOT_EXIST 字库不存在
*/
int OsUninstallFile(const char *AppName, int FileType);

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
参数 FwFileName【输入】 固件文件名，ZIP 格式。
返回
    RET_OK 更新成功
    RR_FILE_NOT_FOUND FwFileName 文件不存在
    ERR_VERIFY_SIGN_FAIL 签名错误
    ERR_APP_MODE 模式错误
    ERR_SYS_NOT_SUPPORT OS 不兼容
*/
int OsFirmwareUpgrade(const char *FwFileName);

/*
功能 验证 FileName 指定的文件的签名是否正确，文件包括签名数据。
参数
    FileName【输入】 待验证签名的文件路径，该文件已包含签名数据。
    PUKType 【输入】
    PUK_TYPE_M厂商公钥，用于厂商发布的固件的签名认证。
    PUK_TYPE_US_PUK用户签名证书公钥，用于对公钥证书的签名认证。
    PUK_TYPE_USER用户公钥，用于对用户应用的签名认证。
返回
    RET_OK 成功
    ERR_VERIFY_SIGN_FAIL 文件签名信息非法
    ERR_FILE_NOT_EXIST 文件不存在
    ERR_DEV_BUSY 其它程序进行验证签名
    ERR_INVALID_PARAM 非法参数
*/
int OsVerifySign(const char *FileName, int PUKType);

/*
功能 使用指定的签名数据验证文件的签名。
参数
    FileName【输入】 待验证签名的文件路径。该文件不包含签名数据，否则会导致验证失败。
    SignData【输入】签名数据，Prolin 允许的签名数据长度为 284字节｡
    PUKType 【输入】
        PUK_TYPE_M 厂商公钥，用于厂商发布的固件的签名认证
        PUK_TYPE_US_PUK用户签名证书公钥，用于对公钥证书的签名认证
        PUK_TYPE_USER用户公钥，用于对用户应用的签名认证
返回
    RET_OK 成功
    ERR_VERIFY_SIGN_FAIL 文件签名信息非法
    ERR_FILE_NOT_EXIST 文件不存在
    ERR_DEV_BUSY 其它程序进行验证签名
    ERR_INVALID_PARAM 非法参数
*/
int OsVerifySignExternal(const char *FileName, const void *SignData, int PUKType);

/*
功能 获取操作系统以及模块固件版本信息。
参数 VerType 版本类别：
    TYPE_OS_VER 操作系统版本
    TYPE_WIRELESS_VER 无线模块固件版本
Version【输出】 版本信息缓冲区大小必须不小于 31 字节｡
返回 无
用法 1. 如果 Version[0]等于 0x00，表示相应模块不存在；
     2. 各版本信息的长度小于等于 31 字节，并以“\0”字符结尾。
*/
void OsGetSysVer(int VerType, char *Version);
/*
功能 检测手机是否连接座机。
参数 无
返回
    RET_OK 手机在座机上
    ERR_BASE_ABSENT 手机不在座机上
    ERR_SYS_NOT_SUPPORT 系统不支持此功能
    ERR_TIME_OUT 超时
    ERR_SYS_BAD 系统错误
*/
int OsOnBase(void);

/*
功能 保存崩溃报告。
参数 sig 信号值
返回 无
*/
void OsSaveCrashReport(int sig);

/*
功能 挂载/安装外部存储系统，如 USB 可移动磁盘。
参数
    Source【输入】要挂载的文件系统，通常是一个设备，该设备必须位于/dev/block/下路径长度不能超过128 字节。
    Target【输入】 要挂载到的文件目录，该目录必须位于/mnt/下；路径长度不能超过 128 字节。
    FileSystemType【输入】 要挂载的文件系统类型，可取值为 vfat。
    MountFlags【输入】 挂载标识，可为以下标识或标识组合：
    Data【输入】 用户自定义信息，可以为 NULL。
返回
    RET_OK 挂载成功
    ERR_INVALID_PARAM 非法参数
    ERR_STR_LEN 字符串长度过长
    ERR_NEED_ADMIN 权限不够
*/
int OsMount(const char *Source,const char *Target,const char *FileSystemType,unsigned long MountFlags,const void *Data);

/*
功能 卸载文件系统｡
参数
    Target【输入】 要卸载的文件系统，必须为/mnt/目录下的路径，路径长度不能超过 128 字节。
    Flags卸载标识，可为以下标识或标识组合：
    MNT_DETACH：lazy umount，执行后挂载点不可访问，当挂载点不忙时才会卸载。
    MNT_EXPIRE：标记挂载点已过期。
    UMOUNT_NOFOLLOW：如果目标是一个符号链接，则不减少引用计数。
返回
    RET_OK 卸载成功
    ERR_INVALID_PARAM 非法参数
    ERR_STR_LEN 字符串长度过长
    ERR_NEED_ADMIN 权限不够
*/
int OsUmount(const char *Target, int Flags);

/*
功能 格式化存储设备为加密文件系统。
参数
    Dev 【输入】 待格式化的设备，该设备必须位于/dev/block/目录下；路径长度不能超过 128 字节。
    Pwd 【输入】 加密文件系统密码，为可见字符串，长度 6~32字节。
    FsType 【输入】 格式化文件系统类型，可取值“vfat”。
返回
    0 成功
    ERR_INVALID_PARAM 非法参数
    ERR_DEV_NOT_EXIST 文件或路径不存在
*/
int OsCryptFormat(const char *Dev, const char *Pwd,const char *FsType);

/*
功能 挂载加密文件系统。
参数
    Dev 【输入】 要挂载的设备，该设备必须位于/dev/block/目录下；路径长度不能超过 128 字节。
    Pwd 【输入】 加密文件系统密码，为可见字符串，长度 6~32字节。
    Target 【输入】 要挂载到的文件目录，该目录必须位于/mnt/目录下；路径长度不能超过 128 字节。
    FsType 【输入】 要挂载的文件系统类型，可取值为“vfat”。
返回
    0 成功
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_NOT_SUPPORT 系统不支持（密码错误或加密格式不支持）
    ERR_DEV_NOT_EXIST 设备不存在
    ERR_ACCESS_DENY 无访问权限
*/
int OsCryptMount(const char *Dev, const char *Pwd,const char *Target, const char *FsType);

/*
功能 卸载加密文件系统。
参数 Target 【输入】 待关闭的加密 SD 卡的挂载目录。
返回
    0 成功
    ERR_INVALID_PARAM 非法参数
    ERR_DEV_BUSY 设备正在使用中
    ERR_ACCESS_DENY 无访问权限
    ERR_FILE_NOT_EXIST 目录不存在
*/
int OsCryptUmount(const char *Target);

/*
功能 控制 Led 灯亮灭及颜色。
参数
Id 【输入】 Led 灯号
Color 【输入】发光颜色，可设置以下值：
    LED_OFF：灭灯
    LED_RED：红色
    LED_GREEN：绿色
    LED_BLUE：蓝色
    LED_YELLOW : 黄色
    LED_CYAN : 青色
    LED_MAGENTA：品红
    LED_WHITE：亮白
dev 【输入】设备型号。当操作远程设备的 led 灯时，需要输入此参数 ( 如读卡器：“IM700”,”IM500”,”IM20”)；当操作本机 led 灯时，此参数输NULL。
返回
    0 成功
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_SYS_NOT_SUPPORT 不支持
*/
int OsLed(int Id, unsigned int Color,const char *dev);

/*
功能 获取终端各模块累计使用信息。
参数
Key【输入】 设备信息类别。
Value【输出】 设备使用信息。
返回
    RET_OK 成功
    ERR_SYS_NOT_SUPPORT 系统不支持
    ERR_INVALID_PARAM 非法参数
*/
int OsTerminalConsumeInfo(const char *Key, int *Value);

/*
功能 获取所有数字 IO 的输入状态。
参数 value【输出】按 bit 表示每个 IO 的状态，0 表示悬空或高电平，1 表示低电平，如果某一路 IO 不可读(只写)，对应 bit 返回 0
返回
    >=0 vaule 中 bit 位为 1 的个数
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_NOT_SUPPORT 系统不支持
*/
int OsDigitalIOGetStat(int *value);
/*
功能 设置指定 IO 的输出状态。
参数
    index【输入】 指定要设置 IO 的序号，如果 IO 不支持设置(只读)，会返回 ERR_SYS_NOT_SUPPORT
    value【输入】 0 对应 IO 输出为高电平 1 对应 IO 输出为低电平
返回
    RET_OK 成功
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_NOT_SUPPORT 系统不支持
*/
int OsDigitalIOSetStat(int index, int value);






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
功能 设置 LCD 对比度。
参数 Contrast 
对比度值，取值范围为[0~7]。0：最低。7：最高。默认值为 4，其它值无动作。
返回 无
*/
void OsScrContrast(int Contrast);

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

/*
功能   不支持
使用 TR31 数据封装格式写入一个密钥，包括 TMK， TWK，AES_TMK 和AES_TWK 和 AES_TIK 
*/
int OsPedInjectKeyBlock(unsigned char *KeyBlock, int KeyBlkLen); 

/************************************6.5 PIN 输入*********************************/
/*
功能 设置两次计算 PIN block 之间的最小间隔时间。 
参数 TpkIntervalMs 
    = 0 使用默认值(30s) 
    <1000 自动设为 1000(1s) 
    >600000 自动设为 600000(10min) 
    =0xffffffff 当前设置不被改变 
返回 
    RET_OK 成功 
    ERR_DEV_NOT_OPEN PED 设备未打开
*/
int OsPedSetInterval (unsigned long TpkIntervalMs); 

/*
功能 校验脱机明文 PIN。
参数
    IccSlot 卡片所在的卡座号，IccSlot=0。
    ExpPinLen【输入】可输入的合法密码长度字符串，0~12 的枚举集合。应用程序把允许的密码长度全部枚举出来，并且用“,”号隔开每个长度，如允许输入 4、6位密码并且允许无密码直接按确认，则该字符串应该设置为“0,4,6”。若枚举0长度则表示可以不输任何数字而直接按确认键返回。
    Mode 0x00，IC 卡命令模式，现支持符合 EMV2000的 IC 卡命令。
    TimeoutMs 输入 PIN 的超时时间，单位：毫秒，最大值为300000。0：表示没有超时时间，PED 不做超时控制。
    IccRsp【输出】 卡片响应的状态码(2 字节：SWA+SWB)
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedVerifyPlainPin (int IccSlot, const char * ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char * IccRsp); 

/*
功能 校验密文 PIN,步骤如下：
    1. 获取明文 PIN；
    2. 用应用提供的 RsaPinKey 对明文 PIN 按照 EMV 规范进行加密；
    3. 用应用提供的卡片命令与卡片通道号，将密文 PIN 直接发送给卡片。
参数
    Iccslot 卡片所在的卡座
    RsaPinKey【输入】 加密所需数据结构 ST_ RSA_PINKEY。
    ExpPinLen【输入】
    Mode 0x00，符合 EMV2000IC 卡的命令。
    TimeoutMs 输入 PIN 的超时时间，单位：毫秒，最大值为 300000。0：表示没有超时时间，PED 不做超时控制。
    IccRsp【输出】 卡片响应的状态码(2 字节：SWA+SWB)
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedVerifyCipherPin (int IccSlot, const ST_RSA_PINKEY * RsaPinKey, const char * ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char * IccRsp);

/*
功能 设定某些功能键的功能。
参数 KeyFlag 
返回
    RET_OK 成功
    ERR_DEV_NOT_O
    PEN 设备未打开
    ERR_INVALID_PA
    RAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedSetFunctionKey (int KeyFlag);

/*
功能 用于 PIN 输入过程中，终止 PIN 输入。
参数 无
返回 RET_OK 成功
*/
int OsPedCancelPinEntry(void);

/*
功能 设置脱机明文/密文校验模式。
参数
    Mode 【输入】校验模式：0-使用内置密码键盘输入 PIN；1-使用外置密码键盘输入 PIN，通过
    PinBlock 参数导入 PIN。
    TpkIdx 【输入】
    TPK 索引：Mode 为 0 时，无意义； Mode 为 1 时，使用该索引的 TPK 对导入的PinBlock进行解密得到PIN明文。
    PinBlock 【输入】
    PIN block： Mode 为 0 时，无意义； Mode 为 1 时，表示 TPK 加密的ISO9564 Format1 格式的 PIN 密文。
    PinBlockLen 【输入】 PIN block 的长度
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedSetOfflinePin(unsigned char Mode,unsigned char TpkIdx,unsigned char *PinBlock, unsigned short PinBlockLen);

/*
功能 用于 PIN 输入过程中，发送确认键结束 PIN 输入。
参数 无
返回 RET_OK 成功
*/
int OsPedEndPinEntry(void);

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
功能 使用指定密钥类型的密钥索引所在的密钥明文与一串数据进行运算(异或等)，得到密钥写入到同一类型密钥区的另一指定索引位置。
参数
    KeyType 密钥类型： PED_TMK PED_TPK  PED_TAK  PED_TDK 
    SrcKeyIdx 源密钥索引号，取值范围为[1~100]。
    DstKeyIdx 目的密钥索引号，取值范围为[1~100]。
    KeyVar【输入】24 字节，需要参与运算的字符串；为与密钥长度相同的字符串，用于和源索引的密钥异或；可扩展｡
返回
    RET_OK 成功
    ERR_DEV_NOT_O
    PEN PED 设备未打开
    ERR_INVALID_PA
    RAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedWriteKeyVar (int KeyType, int SrcKeyIdx, int DstKeyIdx, const unsigned char *KeyVar);
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

/*
功能 用 KeyIdx 指定的 MAC 密钥对 DataIn 进行 Mode 指定的运算。
参数
    KeyIdx TAK 的索引号，取值范围为[1~100]。
    DataIn【输入】 需进行 MAC 运算的数据，长度小于等于 8192 字节。
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

/*
功能 用 SrcKeyIdx 指定的密钥对 DstFromKeyIdx 指定的密钥进行加密或解密，发散新密钥，并将结果保存到 DstToKeyIdx 指定的密钥。
参数
    SrcKeyType 源密钥类型：PED_TLK  PED_TMK  PED_TAK PED_TPK PED_TDK 
    SrcKeyIdx 源密钥的索引号，
    DstKeyType 目的密钥类型： 
    DstFromKeyIdx 目的密钥源索引号
    DstToKeyIdx 目的密钥目的索引号
    Mode 0x00：DES/TDES 解密0x01：DES/TDES 加密
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedDeriveKey(int SrcKeyType, int SrcKeyIdx, int DstKeyType, int DstFromKeyIdx, int DstToKeyIdx, int Mode);


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
功能 使用 DUKPT 密钥计算 MAC 值。
参数
    GroupIdx DUKPT 组索引号，取值范围为[1~100]。
    DataIn【输入】 指向需要计算 MAC 的数据内容。
    DataInLen 数据的长度，小于等于 8192 字节，当不为 8字节整除时自动补“\x00”。
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
/************************************6.8 RSA*********************************/
/*
功能 读取 RSA 公钥。
参数
    RsaKeyIdx RSA Key 的索引号，取值范围为[1~10]。
    RsaKey【输出】 RSA 公钥结构体 ST_RSA_KEY
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedReadRsaKey(int RsaKeyIdx, ST_RSA_KEY *RsaKey);

/*
功能 注入 RSA 密钥到 PED。
参数
    RsaKeyIdx RSA 密钥的索引号，取值范围为[1~10]。
    RsaKey【输入】 指向需要注入 PED 的 RSA 密钥结构体ST_RSA_KEY。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedWriteRsaKey(int RsaKeyIdx, ST_RSA_KEY *RsaKey);

/*
功能 用存储在 PED 的 RSA 密钥进行 RSA 数据运算。
参数
    RsaKeyIdx RSA Key 的索引号，取值范围为[1~10]。
    DataInLen 需要进行运算的数据长度，单位为字节，与RSA 密钥的模长相同。长度值为 64 到 512 之间 8 的倍数。
    DataIn【输入】 指向需要进行运算的数据
    DataOut【输出】 指向运算后的数据
    KeyInfo【输出】 密钥信息
返回
    RET_OK 成功
    ERR_DEV_NOT_
    OPEN 设备未打开
    ERR_INVALID_P
    ARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedRsaRecover(int KeyIdx, int DataInLen, unsigned char *DataIn, unsigned char *DataOut, unsigned char *KeyInfo);
/*
功能 读取 RSA 密钥密文。
参数
    RsaKeyIdx RSA Key 的索引号，取值范围为[1~10]。
    CipherRsaKey【输出】 指向 RSA 密钥密文
返回
    >0 表示 RSA 密钥密文的长度
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedReadCipherRsaKey(int RsaKeyIdx, unsigned char *CipherRsaKey);
/*
功能 写 RSA 密钥密文。
参数
    RsaKeyIdx RSA Key 的索引号，取值范围为[1~10]。
    CipherRsaKeyLen 写入的 RSA 密钥密文数据的字节长度，小于等于 1024
    CipherRsaKey【输入】 指向 RSA 密钥密文
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedWriteCipherRsaKey(int RsaKeyIdx, int CipherRsaKeyLen, unsigned char *CipherRsaKey);

/************************************6.9 AES*********************************/
/*
功能 将一个 AES 密钥的密文或者明文写入到 AES 密钥区域的指定索引的位置，并可以选择使用 KCV 验证密钥正确性。
参数 KeyBlock【输入】
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN PED 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedWriteAesKey(const unsigned char *KeyBlock);

/*
功能 使用 TAESK 或 PED_AES_TDK 密钥对 DataInLen 指定长度的数据进行 AES 加解密运算。
参数
    KeyIdx【输入】 AES_TDK 密钥索引号，取值范围为[1~100]。
    InitVector【输入/输出】
    DataIn【输入】 需要进行运算的数据
    DataInLen【输入】需要进行运算的数据长度，以字节为单位，应小于等于 1024，且为 16 的倍数。当运算模式为 CTR 模式时，数据长度没有限制。
    DataOut【输出】 指向运算后的数据。
    Mode 【输入】 0x00：ECB 解密模式 0x01：ECB 加密模式 0x02：CBC 解密模式 0x03：CBC 加密模式 0x04：OFB 解密模式 0x05：OFB 加密模式 0x06：CTR 解密模式 0x07：CTR 加密模式
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 无效的参数
    其他 参见 PED 函数返回值列表

*/
int OsPedAes(int KeyIdx,unsigned char*InitVector,const unsigned char*DataIn,int DataInLen,unsigned char *DataOut,int Mode);

/*
功能 使用 AES_TAK 对 DataIn 用 Mode 指定的算法进行 MAC 运算，
将 16 字节的 MAC 结果输出到 Mac
参数
    KeyIndex 【输入】 AES_TPK 组索引号，取值范围为[1~100]
    DataIn【输入】 指向需要计算 MAC 的数据
    DataInLen 【输入】 数据的长度，小于等于 8192 字节，当不为16 字节整除时自动补 0x00
    Mac【输出】 指向得到的 MAC。
    Mode 将 DataIn 按 16 字节为单位分块，依次为BLOCK1，BLOCK2，BLOCK3 等。
        0x00：将 BLOCK1 用 MAC 密钥做 AES 加密，加密结果与 BLOCK2 进行逐位异或后再做 AES 加密，依次进行得到 16 字节的加密结果
        0x01：Hypercom Fast Mode，将 BLOCK1 和BLOCK2 进行逐位异或，异或结果与BLOCK3 进行逐位异或，依次进行，最后得到 16 字节的异或结果，将该结果用 AES_TAK 进行 AES 加密运算
        0x03：CMAC 算法
        0x05：HMAC-HASH256 算法
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    其他 请参考 PED 函数返回值列表
*/
int OsPedGetMacAes(unsigned char KeyIndex, unsigned char *DataIn, int DataInLen, unsigned char *Mac, unsigned char Mode);


/************************************6.10 国密算法 API*********************************/
/*
功能 生成一组 SM2 密钥对。
参数
    PvtKey【输出】 指向 SM2 私钥，32 字节
    PubKey【输出】 指向 SM2 公钥，64 字节
    KeyLenBit【输入】 私钥位数，256 位
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
用法 支持的 SM2 私钥位数为 256 位，对应的公钥位数为 512 位。
*/
int OsPedGenSM2Pair(unsigned char *PvtKey, unsigned char *PubKey, int KeyLenBit);
/*
功能 注入 SM2 私钥或公钥到 PED。
参数
    KeyIdx 【输入】 SM2 密钥索引号，取值范围为[1~20]
    KeyType【输入】密钥类型：PED_SM2_PVT_KEY 0x30 私钥PED_SM2_PUB_KEY 0x31 公钥
    KeyValue 【输入】当 KeyType=0x30 时，KeyValue 长度为32 字节 当 KeyType=0x31 时，KeyValue 长度为64 字节
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    其他 参见 PED 函数返回值列表
*/
int OsPedWriteSM2Key(int KeyIdx, int KeyType, unsigned char *KeyValue);

/*
功能 使用 SM2 算法获得签名信息。
参数
    PubKeyIdx【输入】 SM2 公钥索引号，取值范围为[1~20]
    PvtKeyIdx【输入】 SM2 私钥索引号，取值范围为[1~20]
    Uid【输入】 签名者 ID无特殊约定的情况，用户身份的标识 ID长度为 16 字节，其默认值为 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38。
    UidLen【输入】 签名者 ID 长度，小于等于 512 字节。
    Input【输入】 需要签名的数据
    InputLen【输入】 数据长度，小于等于 2048 字节
    Signature【输入】 签名值，64 字节
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_NO_KEY 密钥不存在
    ERR_PED_TAMPERED PED 被锁定
    其他 参见 PED 函数返回值列表
*/
int OsPedSM2Sign(int PubKeyIdx, int PvtKeyIdx,unsigned char *Uid,int UidLen,unsigned char *Input, int InputLen,unsigned char *Signature);
/*
功能 使用 SM2 公钥验证签名。
参数
    PubKeyIdx【输入】 SM2 公钥索引号，取值范围为[1~20]
    Uid【输入】 签名者 ID。无特殊约定的情况，用户身份的标识 ID的长度为 16 字节，其默认值为 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38。
    UidLen【输入】 签名者 ID 长度，小于等于 512 字节
    Input【输入】 需要签名的数据
    InputLen【输入】 数据长度，小于等于 2048 字节
    Signature【输入】 签名值，64 字节
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_VERIFY_SIGN_FAIL 验证签名失败
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_NO_KEY 密钥不存在
    ERR_PED_TAMPERED PED 被锁定
    其他 参见 PED 函数返回值列表
*/
int OsPedSM2Verify(int PubKeyIdx, unsigned char *Uid,int UidLen,unsigned char *Input,int InputLen,const unsigned char *Signature);

/*
功能 使用 SM2 公钥加密数据或私钥解密数据。
参数
    KeyIdx【输入】 SM2 密钥索引号，取值范围为[1~20]
    Input【输入】 待加密或解密的数据
    InputLen【输入】 数据长度，加密操作时小于等于（2048-96）字节，解密操作时小于等于2048 字节。
    Output【输出】 加密或解密后的数据
    OutputLen【输出】 加密或解密后的数据长度，加密后数据长度为原数据长度+96 字节，解密后数据长度为原数据长度-96 字节。
    Mode【输入】 PED_DECRYPT0x00：使用 SM2 私钥解密数据 PED_ENCRYPT0x01：使用 SM2 公钥加密数据
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_NO_KEY 密钥不存在
    ERR_PED_TAMPERED PED 被锁定
    其他 参见 PED 函数返回值列表
*/
int OsPedSM2Recover(int KeyIdx, unsigned char *Input,int InputLen,unsigned char *Output, int *OutputLen,int Mode);

/*
功能 使用 SM3 算法计算杂凑值。
参数
    Input【输入】 输入数据
    InputLen【输入】 输入数据长度
    Output【输出】 杂凑值，32 字节。
    Mode【输入】 支持 0x00，其它值保留。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
*/
int OsPedSM3(unsigned char *Input, int InputLen,unsigned char *Output,int Mode);

/*
功能 使用 SM4 算法加密或解密数据。
参数
    KeyIdx【输入】 PED_SM4_TDK 的索引号，取值范围为[1～100]
    InitVector【输入】 初始向量，16 字节。对于 ECB 模式该参数为 NULL。
    Input【输入】 需要加密或解密的数据
    InputLen【输入】 数据长度小于等于 1024，以字节为单位，且为 16 的倍数
    Output【输出】 加密或解密后的数据，长度为输入长度
    Mode【输入】 PED_SM4_ECB_DECRYPT 0x00:SM4 ECB 解密  PED_SM4_ECB_ENCRYPT 0x01:SM4 ECB 加密     PED_SM4_CBC_DECRYPT 0x02:SM4 CBC 解密     PED_SM4_CBC_ENCRYPT 0x03:SM4 CBC 加密
返回 RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_NO_KEY 密钥不存在
    ERR_PED_TAMPERED PED 被锁定
    ERR_PED_KEY_LEN_ERR 密钥长度错误
    其他 参见 PED 函数返回值列表
*/
int OsPedSM4(int KeyIdx, unsigned char *InitVector, unsigned char *Input, int InputLen,unsigned char *Output,int Mode);

/*
功能 使用 SM4 算法计算 MAC。
参数
    KeyIdx【输入】 PED_SM4_TAK 的密钥索引号，取值范围为[1～100]
    InitVector【输入】 初始向量，16 字节。
    Input【输入】 需要进行 MAC 运算的数据
    InputLen【输入】 数据长度小于等于 1024，以字节为单位，且为 16 的倍数
    MacOut【输出】 MAC 值
    Mode【输入】 0x00：使用 SM4 CBC 算法计算 MAC 值，首先将初始向量与 BLOCK1 进行异或，并用SM4算法使用TAK对异或的结果进行加密，然后获得的密文与 BLOCK2 异或，用SM4 算法使用 TAK 对结果加密，按顺序给出 16 字节的加密结果。MacOut 为 16 字节。0x01：使用 SM4-TAK 密钥对输入数据计算SM3 Hash，计算结果作为 MacOut(32 字节)。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_NO_KEY 密钥不存在
    ERR_PED_TAMPERED PED 被锁定
    ERR_PED_KEY_LEN_ERR 密钥长度错误
    其他 参见 PED 函数返回值列表
*/
int OsPedGetMacSM(int KeyIdx,unsigned char *InitVector, unsigned char *Input,int InputLen, unsigned char *MacOut,int Mode);

/*
功能 指定的时限内，扫描键盘上输入的 PIN 并输出采用 SM4 算法生成的 PIN block 加密数据块。
参数
    KeyIdx【输入】 PED_SM4_TPK 的密钥索引号，取值范围为[1～100]
    ExpPinLenIn【输入】 可输入的合法密码长度字符串，0~12 的枚举集合。应用程序把允许的密码长度全部枚举出来，并且用“,”号隔开每个长度，如允许输入 4、6 位密码，则该字符串应该设置为“4,6”。
    DataIn【输入】 当 Mode=0x00 时，DataIn 指向卡号移位后生成的 16 位主帐号。
    PinBlockOut【输出】 生成的密文 PIN block，16 字节
    Mode【输入】 PIN block 的格式0x00 ISO9564 格式 0
    TimeoutMs【输入】 输入 PIN 的超时时间，单位：毫秒。最大值为 300000ms。0 表示采用默认超时时间30000ms。
返回
    RET_OK 成功
    ERR_DEV_NOT_OPEN 设备未打开
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_BAD 系统错误
    ERR_PED_KEY_IDX_ERR 密钥索引错误
    ERR_PED_KEY_TYPE_ERR 密钥类型错误
    ERR_PED_NO_KEY 密钥不存在
    ERR_PED_TAMPERED PED 被锁定
    ERR_PED_KEY_LEN_ERR 密钥长度错误
    ERR_PED_NO_PIN_INPUT 未输入任何键值
    ERR_PED_PIN_INPUT_CANCEL 取消输入
    ERR_PED_WAIT_INTERVAL 间隔时间太短
    其他 参见 PED 函数返回值列表
*/
int OsPedGetPinBlockSM4(int KeyIdx, const char *ExpPinLenIn, unsigned char *DataIn,unsigned char *PinBlockOut, int Mode, long TimeoutMs);

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
功能 设置虚拟打印机相关参数。
参数
    Width【输入】 宽度
    Height【输入】 高度
返回
    RET_OK 成功
    ERR_INVALID_PA
    RAM 非法参数
    用法 仅对虚拟打印机有效
*/
int OsPrnSetSize (unsigned int Width, unsigned int Height);

/*
功能 设置打印方向。
参数 Mode【输入】 0：横向打印 1：纵向打印。
返回
    RET_OK 成功
    ERR_INVALID_PARAM 非法参数
*/
int OsPrnSetDirection (unsigned char Mode);

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
功能 获取当前任务已打印的点行数。
参数 无
返回 >=0 当前已打印点行个数
用法 用于单据对齐(一般情况下不使用)。对实体及虚拟打印机皆有效。
*/
int OsPrnGetDotLine(void);
/*
功能 选择打印字体。
参数 fontname【输入】 字体(文件)名称
返回
    RET_OK 成功
    ERR_FONT_NOT_EXIST 字体不存在
    ERR_INVALID_PARAM 非法参数
用法 选择系统中不同的字体样式和字体大小进行打印，可通过调用OsEnumFont( )获取系统内置字体文件名称。
*/
int OsPrnSetFont(const char*fontname);

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
功能 设置打印字体大小。
参数
SingleCodeWidth 单内码字体宽度控制(对非等宽输出字体来说，真正每个字符的宽度不一定满足此设置)选值区间为 8~64
SingleCodeHeight 单内码字体高度控制选值区间为 8~64 
MultiCodeWidth 多内码字体宽度控制选值区间为 12~64 
MultiCodeHeight 多内码字体高度控制选值区间为 12~64 
返回 无
用法 第一次调用 OsPrnOpen()后，字体宽度和高度均为默认的宽度和高度，单内码为(12x24)，多内码为(24x24)。对实体及虚拟打印机皆有效。
*/
void OsPrnSelectFontSize(int SingleCodeWidth, int SingleCodeHeight, int MultiCodeWidth, int MultiCodeHeight);

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
参数 Logo【输入】 需打印的 logo 信息的指针，信息长度不能超过20000 个字节。
返回 无
用法 图形数据的生成步骤如下：
1. 画 LOGO：可以使用 Windows 中的“开始/程序/附件/画图”工具画图形，图形的存盘要求为“单色、BMP 格式”；
2. 使用本公司提供的“图形转换工具”将该 BMP 图形文件转换成一个诸如 Logo.h 的头文件（可选择多个 BMP 文件，转换后的头文件中将包含与 BMP 相同数量的数组，数组名的定义与 BMP文件名相关）。
*/
void OsPrnPutImage(const unsigned char *Logo);

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
(位 2)表示对 PPS 协议支持：0 - 不支持，1 – 支持；
(位 3~4)表示上电复位使用速率：00 - 标准速率 9600  10 - 四倍速率 38400
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
功能 按照 ISO14443-4 中指定的半双工通信协议实现透明传输/接收功能。
参数 Slot
    IC 卡通道号：
    ICC_USER_SLOT 用户卡
    ICC_SAM1_SLOT SAM 卡卡座 1
    ICC_SAM2_SLOT SAM 卡卡座 2
    ICC_SAM3_SLOT SAM 卡卡座 3
    ICC_SAM4_SLOT SAM 卡卡座 4

CtrlFlag
1. Bit0 表示 T=0 协议下是否自动发送 GET RESPONSE 指令：1-是0-否
2. Bit1~Bit31 保留
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
#define ERR_BATTERY_VOLTAGE_TOO_LOW -1024 //电池电压过低

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
功能 对卡片进行防冲突和选择。
参数
    pcPiccType【输入】
    寻到卡片类型：
     “A” - A 卡
     “B” - B 卡
    pucUID【输出】
    卡片的唯一标识：
     A 卡可为 4，7 或 10 字节
     B 卡为 4 字节
    ucATQ0【输入】 此参数未使用
    pucSAK【输出】 选卡时卡片应答数据，长度为 1 字节。
    Prolin 应用编程接口编程指南
    百富计算机技术(深圳)有限公司 150
    SAK 为 A 卡的最后一条选择命令回应的数据，
    B 卡忽略此参数
返回
    0 卡片选择成功
    其它 卡片选择失败(参见函数返回值列表)
*/
int OsPiccAntiSel(const char pcPiccType,unsigned char *pucUID, const unsigned char ucATQ0, unsigned char*pucSAK);

/*
功能 对卡片进行激活处理。
参数
    pcPiccType【输入】
    寻到卡片类型：
     “A” - A 卡
     “B” - B 卡
    pucRATS【输出】
    激活卡片的应答数据：
     A 卡时，pucRATS 为响应 ATS 命令回应
    的数据
     B 卡时，pucRATS 为响应 ATTRIB 命令回
    应的数据
返回
    0 卡片激活成功
    其它 卡片激活失败(参见函数返回值列表)
*/
int OsPiccActive(const char pcPiccType, unsigned char *pucRATS);
/*
功能 按照 ISO14443-4 中指定的半双工通信协议实现透明传输/接收功能。
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
功能 验证 Mifare 卡。
参数
    uid【输入】 卡片 ID，长度为 4 个字节。
    blk no 【输入】 块号
    group【输入】 认证密码类型，取值为‘A’或‘B’。
    psw 【输入】 认证密码，长度为 6 个字节。
返回
    0 成功
    其它 失败
*/
int OsMifareAuthority(unsigned char *uid,unsigned char blk_no,unsigned char group, unsigned char *psw);

/*
功能 对 Mifare 卡指定的块进行读/写操作；或对 Mifare 卡指定数据块进行充/减值/备份操作，操作后的值将更新到另一个指定的数据块中。
参数
    ucOpCode【输入】
    ‘r’/‘R’：读操作
    ‘w’/‘W’：写操作
    ‘+’：充值
    ‘-’：减值
    ‘>’：转存/备份操作
    ucSrcBlkNo【输入】 指定访问的块号pucVal【输入/输出】
    1. 若为读操作，pucVal 输出块内容，指向的缓冲区为 16 字节；
    2. 若为写操作，pucVal 输入块内容，指向的缓冲区为 16 字节；
    3. 若为充值或者减值操作，pucVal 为金额数缓冲区首址，指向的缓冲区为 4 字节；
    4. 若为转存操作，pucVal 无实际意义，但传入的指针不能为 NULL。ucDesBlkNo【输入】 指定操作结果最终写入到的块号(读写块时，该值为 NULL)
返回
    0 成功
    其它 失败
*/
int OsMifareOperate (unsigned charucOpCode, unsigned charucSrcBlkNo, unsigned char*pucVal, unsigned charucDesBlkNo );

/*
功能 对 FeliCa 卡进行初始化配置。
参数
    ucSpeed【输入】
    设置与卡片交互的传输速率：
    1:424Kbp
    其它值：212Kbps
    ucModInvert【输入】
    设置 FeliCa 调制方式：
    1：正向调制输出
    其它值：反向调制输出
返回 0 成功
*/
int OsPiccInitFelica(unsigned char ucSpeed,unsigned char ucModInvert);

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
功能 设置用户配置。
参数 pcd_user_config【输入】 用户配置结构体 PCD_USER_ST
返回
    0 成功
其它 失败(参见函数返回值列表)
*/
int OsPiccSetUserConfig(PCD_USER_ST *pcd_user_config) ;

/*
功能 获取用户配置。
参数 pcd_user_config【输出】 用户配置结构体 PCD_USER_ST
返回
    0 成功
    其它 失败(参见函数返回值列表)
*/
int OsPiccGetUserConfig(PCD_USER_ST *pcd_user_config);

/*
功能 寻卡，包括“A”“B”和“V”卡三种类型的轮寻。
*/
int OsPiccApplePoll(char *pcPiccType,unsigned char *pucATQx,unsigned char *pucSendData);

/*
功能 关闭载波。
参数 无
返回
    0 关闭载波成功
    其它 失败(参见函数返回值列表)
用法 对非接触 IC 卡读卡器进行载波关闭操作，射频场内卡片的状态将变为下电状态。
*/
int OsPiccOffCarrier(void);
/*
功能 对 ISO15693 卡进行初始化配置。
参数 ucDataCodeMode【输入】
    设置与卡片交互的传输速率：
     0：26.48 kbits/s
     其它值：暂不支持
返回
    0
    其它
    成功
    失败(参见函数返回值列表)
*/
int OsPiccInitIso15693(unsigned char ucDataCodeMode);

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
功能 初始化无线设备。
参数
    SimPin【输入】SIM 卡的密码字符串指针，长度小于 50 字节。可以为 NULL，表示不需要密码。
    TimeOutMs【输入】超时时间，单位为毫秒；取值范围为【25000，100000】；当小于 25000 时，自动设置为25000；当大于 100000 时，自动设置为100000。
    CmeString【输出】 保留。当前值为 NULL。
    Size【输入】 保留。当前值为 0。
返回
    WL_CSD_READY CSD 拨号业务就绪
    WL_GPRS_CSD_READY
    GPRS 和 CSD 拨号业务就绪
    ERR_DEV_NOT_OPEN 设备/模块没打开
    ERR_DEV_NOT_EXIST 无线设备/模块不存在
    ERR_NO_PORT 终端物理串口不足
    ERR_WL_NEEDPIN SIM 卡需要 PIN 
    ERR_WL_RSPERR 模块/设备响应错误
    ERR_WL_NORSP 模块没有响应
    ERR_WL_NEEDPUK SIM 卡需要 PUK 
    ERR_WL_WRONG_PIN PIN 错误
    ERR_WL_NOSIM 无 SIM 卡
    ERR_WL_NOREG 无法注册网络
    ERR_INVALID_PARAM 非法参数
*/
int OsWlInitEx(const char *SimPin,int TimeOutMs,char* CmeString,int Size);

/*
功能 设置无线设备上/下电状态。
参数 OnOff【输入】
    无线设备状态：
    1：上电状态
    0：下电状态
返回
    RET_OK 操作成功
    ERR_DEV_NOT_EXIST 无线模块不存在
    ERR_DEV_NOT_OPEN 设备/模块没打开
*/
int OsWlSwitchPower(int OnOff);

/*
功能 设置无线设备休眠状态。
参数 OnOff 【输入】
    1 表示休眠
    0 表示唤醒
    其他表示未知错误
返回
    RET_OK 操作成功
    ERR_DEV_NOT_EXIST 无线模块不存在
    ERR_DEV_NOT_OPEN 设备/模块没打开
    ERR_WL_PPP_ONLINE PPP 在线
*/
int OsWlSwitchSleep(int OnOff);

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
    Auth 认证时采用的算法，支持的算法有：
    PPP_ALG_PAP 0x00000001 PAP 认证算法
    PPP_ALG_CHAP 0x00000002 CHAP 认证算法
    PPP_ALG_MSCHAPV1 0x00000004 MSCHAPV1认证算法
    PPP_ALG_MSCHAPV2 0x00000008 MSCHAPV2认证算法
    PPP_ALG_ALL0xff 支持所有的认证算法
    认证算法至少要采用一种，也可以采用多种；采用多种认证算法时，每个算法相加(+)或相或(|)即可，如 PPP_ALG_PAP| PPP_ALG_CHAP；如 果 未 知 何 种 算 法 ， 可 填 入 参 数PPP_ALG_ALL。
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
功能 登录无线网络，建立无线链路(可修改拨号指令)。
参数
    DialNum【输入】 PPP 拨号指令，如果为 NULL，将采用系统默认拨号指令；长度不超过 50 字节。
    APN【输入】GPRS 为无线接入点名称(AccessPointName)，CDMA 为拨号号码；长度不超过 50 个字符；不允许为 NULL。
    Name【输入】 用户名，长度不超过 50 个字节；不允许为NULL；没有用户名时，用空字符串“”表示。
    Password【输入】 密码；长度不超过 50 个字节；不允许为 NULL；没有密码时，用空字符串“”表示。
    Auth 认证时采用的算法，支持的算法有：
        PPP_ALG_PAP 0x00000001 PAP 认证算法
        PPP_ALG_CHAP 0x00000002 CHAP 认证算法
        PPP_ALG_MSCHAPV1 0x00000004 MSCHAPV1认证算法
        PPP_ALG_MSCHAPV2 0x00000008 MSCHAPV2认证算法
        PPP_ALG_ALL0xff 支持所有的认证算法
        认证算法至少要采用一种，也可以采用多种；采用多种认证算法时，每个算法相加(+)或相或(|)即可，如 PPP_ALG_PAP| PPP_ALG_CHAP；如 果 未 知 何 种 算 法 ， 可 填 入 参 数PPP_ALG_ALL。
    TimeOutMs超时时间，单位为毫秒；取值范围为 0～3600000；当小于 0 时，自动设置为 0；当大于3600000 时，自动设置为 3600000。
    KeepAlive 链路检查间隔，单位为毫秒；取值范围为 0~3600000；当为 0 时，不启用KeepAlive 功能；当为 0~10000 时，自动设置为 10000；当为 10000~3600000 时会根据设定的值进行链路检查。
    ReserParam 保留参数，供扩展使用。
返回
    PPP_LOGINING 处理中
    RET_OK 链路建立成功
    ERR_DEV_NOT_O
    PEN 模块/设备没打开
    ERR_INVALID_PARAM 非法参数
    ERR_WL_POWER_ONING 无线模块上电中
    ERR_WL_POWER_OFF 无线模块没上电
    ERR_WL_NOT_INIT没有初始化
    ERR_NET_PASSWD密码错误
    ERR_NET_SERVER_BUSY 服务器忙，通信失败
*/
int OsWlLoginEx(const char *DialNum,const char *APN,const char *Name,const char *Password,long Auth,int TimeOutMs,int KeepAlive,int ReserParam);

/*
功能 退出无线网络，断开无线链路。
参数 无
返回
    PPP_LOGOUTING 链路断开中
    ERR_DEV_NOT_OPEN 模块/设备没打开
*/
int OsWlLogout(void);

/*
功能 选择 SIM 卡。
参数 simno 【输入】
    0 表示选择卡槽 1 中的 SIM 卡
    1 表示选择卡槽 2 中的 SIM 卡
    其它值参数错误
返回
    RET_OK 选择成功
    ERR_DEV_NOT_EXIST 不存在无线模块
    ERR_DEV_NOT_OPEN 模块/设备没打开
    ERR_WL_ERR_BUSY 模块繁忙
    其它非 0 值 参考返回值列表
*/
int OsWlSelSim(int simno);

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
功能 设置 WiFi 模块上电或下电。
参数 Type 
    0：模块硬件进入下电状态
    1：模块硬件进入上电状态
返回
    RET_OK 设置成功
    ERR_INVALID_PARAM 参数错误
    ERR_DEV_NOT_EXIST WiFi 模块驱动加载不正常或者模块错误
    ERR_DEV_NOT_OPEN 未获取 WiFi 设备使用权
*/
int OsWifiSwitchPower (int Type);

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

/*
功能 WPS 方式连接 AP。
参数
    Mode 【输入】 WPS 的连接模式：WPS_MODE。
    WpsPin 【输入/输出】
        1． 若 连 接 模 式 为 WPS_MODE_PBC ，WpsPin 为 NULL；
        2． 若 连 接 模 式 为WPS_MODE_PIN_CLIENT，WpsPin 输出终端产生的 PIN 码，WpsPin 指向的缓冲区为 8 字节；
        3． 若连接模式为 WPS_MODE_PIN_AP，WpsPin 为 AP 的 PIN 码，WpsPin 指向的缓冲区为 8 字节。
返回
    RET_ CONNECTING 正在连接中
    ERR_DEV_NOT_OPEN 打开失败
    ERR_INVALID_PARAM 非法参数
    ERR_SYS_NOT_SUPPORT 系统不支持此 WiFi 模块
    ERR_PIN_FAIL PIN 码错误
*/
int OsWifiWpsConnect (WPS_MODE Mode,char *WpsPin);
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

//检测电池电量
int OsCheckBattery(void);
//检测供电类型
int OsCheckPowerSupply(void);
//使终端进入休眠模式
int OsSysSleep(void);
//设置终端的电源管理模式
int OsSysSleepEx(int Level);
//进入休眠模式，并在指定时间后自动唤醒
int OsSysSleepTime(int Time);
//重启机器
int OsReboot(void);
//关闭终端
int OsPowerOff(void);
//获取PM模发送的消息
int OsPmGetEvent(int TimeoutMs);
//客户端请求机器进入指定的模式
int OsPmRequest(PM_REQ_T ReqType);
//获取使机器从休眠中唤醒的唤醒源
int OsWakeupSource(void);
//检测供电状态是否异常
int OsCheckPowerStatus(int *PowerStatus);
//获取电池管理系统BMS的信息
int OsCheckBMSMode(int *CurrentMode, int *Capacity, int *FullCharge, int *Recharge);



/************************************自定义数据库接口*********************************/
typedef void*		HANDLE; // 通用指针定义
typedef int (*IsMatch)(const void *, int, const void *, int);

typedef enum DBINTERFACE_RET
{
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




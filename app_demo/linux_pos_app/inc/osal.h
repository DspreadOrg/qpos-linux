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

#define RET_OK 0  // Success
#define ERR_INVALID_HANDLE -1000 // Invalid handle
#define ERR_TIME_OUT -1001 // Timeout
#define ERR_APP_NOT_EXIST -1002 // Application does not exist
#define ERR_INVALID_PARAM -1003 // Invalid parameter
#define ERR_DEV_NOT_EXIST -1004 // Device does not exist
#define ERR_DEV_BUSY -1005 // Device busy
#define ERR_DEV_NOT_OPEN -1006 // Device not opened
#define ERR_ACCESS_DENY -1007 // Access denied
#define ERR_FONT_NOT_EXIST -1008 // Target font does not exist
#define ERR_FILE_FORMAT -1009 // File format error
#define ERR_USER_CANCEL -1010 // User cancelled
#define ERR_NO_PORT -1011 // No available communication port
#define ERR_NOT_CONNECT -1012 // Not connected
#define ERR_MEM_FAULT -1013 // Memory fault
#define ERR_SYS_BAD -1014 // System configuration error
#define ERR_SYS_NOT_SUPPORT -1015 // Feature not supported by system
#define ERR_STR_LEN -1016 // String too long
#define ERR_TOO_MANY_HANDLE -1017 // Too many handles
#define ERR_APP_MODE -1018 // Mode error
#define ERR_FILE_NOT_EXIST -1019 // File does not exist
#define ERR_TS_DISABLE -1020 // Touch screen not opened
#define ERR_FONT_CODE -1021 // Character encoding error
#define ERR_VERSION_TOO_LOW -1022 // Version too low
#define ERR_BATTERY_ABSENT -1023 // Battery absent
#define ERR_BATTERY_VOLTAGE_TOO_LOW -1024 // Battery voltage too low

/************************************4 System Functions *********************************/
// 4 System Functions
#define ERR_FILE_NOT_FOUND -2201 // File not found
#define ERR_VERIFY_SIGN_FAIL -2204 // Verify signature failed
#define ERR_NO_SPACE -2205 // Insufficient system space
#define ERR_NEED_ADMIN -2207 // Higher privileges required
#define ERR_PUK_NOT_EXIST -2208 // PUK does not exist
#define ERR_OS_VERSION_TOO_LOW -2209 // OS version too low

#define FILE_TYPE_APP 1 // Application package
#define FILE_TYPE_APP_PARAM 2 // Application data file
#define FILE_TYPE_SYS_LIB 3 // System dynamic library file
#define FILE_TYPE_PUB_KEY 4 // User public key file
#define FILE_TYPE_AUP 5 // Application upgrade package

typedef enum{
    LOG_DEBUG, /* Display debug information */
    LOG_INFO, /* Display prompt/info information */
    LOG_WARN, /* Display warning information */
    LOG_ERROR, /* Display error information */
} LOG_T;

// ST_TIME: Time structure
typedef struct{
    int Year; /* Year (1970 - 2037) */
    int Month; /* Month (1 - 12) */
    int Day; /* Day (1 - 31) */
    int Hour; /* Hour (0 - 23) */
    int Minute; /* Minute (0 - 59) */
    int Second; /* Second (0 - 59) */
    int DayOfWeek; /* Day of week (1 - 7), only valid for reading time */
} ST_TIME;

// ST_TIMER: Timer structure
typedef struct{
    long long Start;
    long long Stop;
    long long TimeLeft;
} ST_TIMER;

// ST_APP_INFO: Application info structure
typedef struct{
    char Id[64];
    char Name[64];
    char Bin[64];
    char Artwork[64];
    char Desc[64];
    char Vender[32];
    char Version[32] ;
} ST_APP_INFO;

// ST_OPT_INFO: Optional system components info structure
typedef struct {
    char Name[64];
    char Version[32];
} ST_OPT_INFO;

// Get current system tick count in milliseconds.
long long get_sys_tick();

// Set the current system date and time.
int OsSetTime(const ST_TIME *Time);
// Get the terminal system date and time.
void OsGetTime(ST_TIME *Time);
// Contactless module test interface.
int OsYmRfidTest();
// Get anti-tamper state. 0 means normal; non-zero means tamper/attack state.
int OsGetAntiTamperState(int* AntiTamperState);
// Get device mode: 0 for user build, 1 for debug build.
int OsGetDevMode(int *mode);
// Get encrypted CID for customer applications.
int OsGetEncCID(char *EncrptyCid);
// Get encrypted CID for OS firmware.
int OsGetSysFwEncCID(char *EncrptyCid);
// Get PCBA board number.
int OsGetYMPcbaNo(char *pcbaNo);
// Set PCBA board number.
int OsSetYMPcbaNo(char *pcbaNo);
// Create a timer with the specified duration.
int OsTimerSet(ST_TIMER *Timer, unsigned long Ms);
// Check the remaining time of the specified timer.
unsigned long OsTimerCheck(ST_TIMER *Timer);
// Suspend the current process/thread for the specified time.
void OsSleep(unsigned int Ms);
// Set the LOG tag.
void OsLogSetTag(const char *Tag);
// Write a LOG entry.
int OsLog(LOG_T Prio, const char *fmt, ...);
// Get cumulative uptime in milliseconds since power-on, excluding sleep time.
unsigned long OsGetTickCount(void);
// Get information about installed applications.
int OsGetAppInfo(ST_APP_INFO AppInfo[],int InfoCnt);
// Get information about installed optional system components.
int OsGetOptInfo(ST_OPT_INFO OptInfo[],int InfoCnt);
// Get application info by AppId.
int OsGetAppInfoByAppid(char* appid,ST_APP_INFO *pInfo);
/*
Function: Set the buzzer tone type and duration.
Parameters:
    ToneType [in] Buzzer tone type, range [1, 7].
    DurationMs [in] Duration in milliseconds, range [10, 10000].
        If DurationMs < 10, it is clamped to 10.
        If DurationMs > 10000, it is clamped to 10000.
Returns: none.
*/
void OsBeep(int ToneType, int DurationMs);
/*
Function: Enable/disable key tone and set duty cycle.
Parameters:
    OnOff [in] 0 to disable key tone, 1 to enable key tone.
    DutyCycle [in] Duty cycle in range [1, 99].
        If DutyCycle is outside [1, 99], the current duty cycle is kept.
        DutyCycle = 50 gives the maximum volume.
*/
void OsSetKeyTone(int OnOff,int DutyCycle);

/*
Function: Set low-battery LED blink frequency.
Parameters:
    Freq [in] Frequency in 1~20 Hz.
Returns: 0 on success, otherwise failure.
*/
int OsSetLPLEDFreq(int Freq);
/*
Function: Get low-battery LED blink frequency.
Parameters:
    Freq [out] Frequency in 1~20 Hz.
Returns: 0 on success, otherwise failure.
*/
int OsGetLPLEDFreq(int *Freq);

typedef void (*RUNAPP_CB)(char *appid, char *str, void *data);
/*
Function: Switch to the specified sub-application.
Parameters:
    AppId [in] Target sub-application ID.
    Argv [in] Argument list for the sub-application. Can be NULL. (Currently unused)
    Data [in] Custom data passed to CbOut/CbErr callbacks. (Currently unused)
    CbOut [in] Standard output callback. (Currently unused)
    CbErr [in] Standard error callback. (Currently unused)
Returns:
    RET_OK on success.
    ERR_APP_NOT_EXIST if the sub-application does not exist.
    ERR_ACCESS_DENY if device access is denied.
    ERR_APP_MODE on mode error.
    ERR_INVALID_PARAM on invalid parameter.
    ERR_NEED_ADMIN if system main-app privilege is required.
*/
int OsRunApp(char *AppId, char **Argv, void *Data, RUNAPP_CB CbOut,RUNAPP_CB CbErr);
/*
Function: Switch to the specified sub-application (extended arguments).
Parameters:
    AppId [in] Target sub-application ID.
    Argc [in] Number of entry arguments, up to 12.
    Argv [in] Up to 12 arguments, each shorter than 31 bytes.
Returns:
    RET_OK on success.
    ERR_APP_NOT_EXIST if the sub-application does not exist.
    ERR_ACCESS_DENY if device access is denied.
    ERR_APP_MODE on mode error.
    ERR_INVALID_PARAM on invalid parameter.
    ERR_NEED_ADMIN if system main-app privilege is required.
*/
int OsRunAppEx(char *AppId, int Argc,char Argv[][32]);
/*
Function: Install or update application package/data, OPT package, user public key, or peripheral firmware.
Parameters:
    AppId [in] Application ID (reserved).
    FilePath [in] Path of the file to install, must not be NULL.
    FileType:
        FILE_TYPE_APP: application package (AIP).
        FILE_TYPE_APP_PARAM: application data file.
        FILE_TYPE_SYS_LIB: optional system component package (OPT), such as shared libraries.
        FILE_TYPE_PUB_KEY: user public key file.
        FILE_TYPE_AUP: application upgrade package.
        FILE_TYPE_FWP: device firmware package.
Returns:
    RET_OK on success.
    ERR_PUK_NOT_EXIST if the specified public key does not exist.
    ERR_FILE_NOT_FOUND if file is not found.
    ERR_FILE_FORMAT on invalid file format.
    ERR_INVALID_PARAM on invalid parameter.
    ERR_VERIFY_SIGN_FAIL on signature verification failure.
    ERR_APP_MODE on mode error.
*/
int OsInstallFile(const char *AppId,const char *FilePath,int FileType);

/*
Function: Uninstall the specified application or OPT package.
Parameters:
    AppId [in] Application ID.
    FileType:
        FILE_TYPE_APP: application package (including all installed files of the app).
        FILE_TYPE_SYS_LIB: OPT package (including all files in the OPT package).
Returns:
    RET_OK on success.
    ERR_APP_NOT_EXIST if app or OPT package does not exist.
    ERR_FONT_NOT_EXIST if font library does not exist.
*/
int OsUninstallFile(const char *AppId, int FileType);

/*
Function: Get current system firmware version.
Parameters:
    Version [out] Firmware version output buffer.
    Size [in] Length of Version buffer. Recommended: 64 bytes.
Returns:
    RET_OK on success.
    ERR_INVALID_PARAM on invalid parameter.
*/
int OsFirmwareGetVersion(char *Version,int Size);

/*
Function: Upgrade system firmware.
Parameters:
    FwPath [in] Firmware package path (YMG).
    FwType [in] Firmware type:
        FW_TYPE_SE_APP: SE application.
        FW_TYPE_SE_FW_CFG: SE firmware configuration.
        FW_TYPE_SE_FW: SE firmware.
Returns:
    RET_OK on successful upgrade.
    RR_FILE_NOT_FOUND if file is not found.
    ERR_VERIFY_SIGN_FAIL on signature verification failure.
    ERR_APP_MODE on mode error.
    ERR_SYS_NOT_SUPPORT if OS is not compatible.
*/

#define FW_TYPE_SE_APP     1 //SE App se_fota_application.YMG
#define FW_TYPE_SE_FW_CFG  2 //SE FW  se_fota_dtb_pro.YMG
#define FW_TYPE_SE_FW      3 //SE FW  se_fota_firmware.YMG

// Upgrade SE firmware package by type.
int OsFirmwareUpgrade(const char *FwPath, int FwType);

/*
Function: Get OS and module firmware version information.
Parameters:
    VerType version category:
        0x01: system version.
        0x02: middleware version.
        0x03: secure CPU app version.
        0x04: secure CPU firmware version.
    Version [out] Output buffer, must be at least 31 bytes.
Returns: none.
Usage:
    1. If Version[0] equals 0x00, the corresponding module does not exist.
    2. Each version string length is <= 31 bytes and ends with '\0'.
*/
void OsGetSysVer(int VerType, char *Version);

// Alias API for retrieving OS/module version information.
void OsGetSysVersion(int VerType, char *Version);

/*
Function: Get the wireless module version string.
Parameters:
    ver [out] Buffer receiving the null-terminated version string. Must not be NULL and should provide at least 32 bytes.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM ver is NULL
    Others: Error code returned by the system parameter service.
*/
int OsGetWirelessModuleVer(char *ver);

/*
Function: Get the terminal serial number.
Parameters:
    Sn [out] Buffer receiving the terminal serial number. Must not be NULL and should provide at least 32 bytes.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Sn is NULL
    ERR_ACCESS_DENY The device information service is unavailable, rejects access, or returns invalid data
*/
int OsGetTermSn(char* Sn);

/*
Function: Set the terminal serial number.
Parameters:
    Sn [in] Null-terminated serial number string. Must not be NULL; the supported maximum length is 31 bytes.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Sn is NULL
    ERR_ACCESS_DENY The system service rejects the serial number or the operation fails
Usage: Serial number length and content validation are performed by the system service.
*/
int OsSetTermSn(char* Sn);

/*
Function: Get the terminal TUSN.
Parameters:
    tusn [out] Buffer receiving the TUSN. Must not be NULL and must be large enough for the returned data.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM tusn is NULL
    ERR_ACCESS_DENY The device information service is unavailable or returns invalid data
*/
int OsGetTermTUSN(char* tusn);

/*
Function: Set the terminal TUSN.
Parameters:
    tusn [in] TUSN string to set. Must not be NULL.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM tusn is NULL
    ERR_ACCESS_DENY The system service rejects the TUSN or the operation fails
*/
int OsSetTermTUSN(char* tusn);

// Control LED status for red/green/yellow/blue indicators.
int OsLed(unsigned int red,unsigned int green,unsigned int yellow,unsigned int blue);

// Get a system parameter by key.
int OsGetSysParam(char *key, char *value);

// Set a system parameter by key.
int OsSetSysParam(char *key, char *value);
/*
Function: Callback type for file upgrade result.
Parameters:
    Result upgrade result. >= 0 means success, otherwise failure.
    Reboot whether reboot is required: 1 required, 0 not required.
Returns:
    1: SDK handles reboot.
    0: Application handles reboot.
*/
typedef  int (*pUpgradeCallback)(int Result, char Reboot);

/*
Function: Upgrade the system from a local package file and report the result through a callback.
Parameters:
    pZipPath [in] Null-terminated path to the upgrade package file. Must not be NULL.
    callback [in] Callback invoked with the upgrade result and reboot requirement. Must not be NULL.
Returns:
    The result returned by the underlying upgrade service.
    -1 Invalid parameter; the callback is not invoked.
Usage:
    1. This function waits for the underlying upgrade command to return, invokes callback, and then returns the same result.
    2. When the callback indicates that the SDK should handle a required reboot by returning a value greater than 0, OsReboot is called.
*/
int OsUpgradeFile(char *pZipPath,pUpgradeCallback callback);

int OsDownLoadAndUpgradeFile();
// Get boot mode. pMode = 1 indicates factory mode.
int OsGetSysBootMode(int *pMode);
// Get hardware version. hw_version[0] = '4' for A30, '7' for A50, others unknown.
void OsGetHwVersion(char *hw_version);
// Get factory test step flag.
int OsGetFactoryTestStep();
// Set factory test step flag.
int OsSetFactoryTestStep(int flag);

/*
Function: Get the DNS server address currently in use by the system.
Parameters:
    dnsaddr [out] Buffer receiving the null-terminated DNS address string. Must not be NULL.
    dnsaddrLen [in] Size of dnsaddr in bytes. Must be at least 8; 16 bytes is recommended for a complete IPv4 address.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Invalid buffer or insufficient buffer size
    Others: Error code returned by the system service.
*/
int OsGetCurDNSAddr(char* dnsaddr,int dnsaddrLen);

/*
Function: Get the configured DNS server address for the specified network type.
Parameters:
    type [in] Network type: 0 for Wi-Fi, 1 for cellular.
    dnsaddr [out] Buffer receiving the null-terminated DNS address string. Must not be NULL.
    dnsaddrLen [in] Size of dnsaddr in bytes. Must be at least 8; 16 bytes is recommended for a complete IPv4 address.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Invalid network type, buffer, or buffer size
    Others: Error code returned by the system service.
*/
int OsGetDNSAddr(int type,char* dnsaddr,int dnsaddrLen);

/*
Function: Set or clear the DNS server address for the specified network type.
Parameters:
    type [in] Network type: 0 for Wi-Fi, 1 for cellular.
    dnsaddr [in] Null-terminated IPv4 address string, 7 to 15 characters. Use "0.0.0.0" to clear the configured DNS address.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Invalid network type, NULL address, or invalid address-string length
    Others: Error code returned by the system service.
Usage: The function validates the string length but delegates address-format validation to the system service.
*/
int OsSetDNSAddr(int type,char* dnsaddr);
// Set preferred network type. 0: Auto GSM/LTE, 1: GSM only, 3: LTE only.
int OsSetPreferredNetworkType(int type);
// Get preferred network type. 0: Auto GSM/LTE, 1: GSM only, 3: LTE only.
int OsGetPreferredNetworkType(int *type);
// Reset touch panel.
int OsResetTP();

int OsGetMisFlag(int *flag);
/************************************5 Cryptography *********************************/
// 5 Cryptography
#define ERR_DATA_TOO_BIG -2400  // RSA encrypted data is larger than the modulus
#define ERR_GEN_RANDOM -2401  // Failed to generate random number
#define ERR_GEN_FAIL -2402  // Failed to generate RSA key pair

#define SHA_TYPE_1    1
#define SHA_TYPE_224  2
#define SHA_TYPE_256  3
#define SHA_TYPE_384  4
#define SHA_TYPE_512  5

/*
Generate true random number.
Parameters
Random [out] Pointer to store the random number.
RandomLen [in] Length of the random number to read (<= 4096 bytes).
*/
void OsGetRandom(unsigned char *Random,int RandomLen);
/*
Calculate SHA (Secure Hash Algorithm) family hash value.
Parameters
Mode Algorithm type: SHA_TYPE_1, SHA_TYPE_224, SHA_TYPE_256, SHA_TYPE_384, SHA_TYPE_512 
Data [in] Input data buffer
DataLen [in] Input data length
ShaOut [out] Output SHA value, buffer size should be larger than 64 bytes.
*/
void OsSHA(int Mode, const void *Data, int DataLen, unsigned char*ShaOut);

/*
DES/TDES encryption/decryption function.
Parameters
Input [in] 8-byte input data.
Output [out] 8-byte output data.
DesKey [in] DES/TDES key.
KeyLen [in] 8, 16, or 24 (bytes).
Mode 1 - Encrypt, 0 - Decrypt
*/
void OsDES(const unsigned char *Input, unsigned char *Output, const unsigned char *DesKey, int KeyLen, int Mode);

/*
AES encryption/decryption function.
Parameters
Input [in] 16-byte input data.
Output [out] 16-byte output data.
AesKey [in] Key
KeyLen [in] 16, 24, or 32 (bytes).
Mode 1 - Encrypt, 0 - Decrypt
*/
void OsAES(const unsigned char *Input, unsigned char *Output, const unsigned char *AesKey,int KeyLen, int Mode);

/*
Perform RSA encryption/decryption function.
Parameters
Modulus [in] Pointer to the modulus n = p * q, stored in big-endian format (most significant byte first).
ModulusLen [in] Modulus length in bytes.
Exp [in] Pointer to the RSA exponent, stored in big-endian format.
ExpLen [in] Exponent length in bytes.
DataIn [in] Input data buffer pointer, length must be equal to ModulusLen.
DataOut [out] Output data buffer pointer, length must be equal to ModulusLen.
Mode 1 - Encrypt, 0 - Decrypt
Returns
RET_OK Success
ERR_INVALID_PARAM Invalid parameter
ERR_DATA_TOO_BIG ExpLen is larger than ModulusLen
*/
int OsRSA(const unsigned char *Modulus, int ModulusLen,const unsigned char *Exp, int ExpLen, const unsigned char *DataIn, unsigned char *DataOut,int Mode);

// RSA RSA_NO_PADDING public key decryption, input data length must be equal to NLen.
int OsEmvRSAPublicDecrypt(unsigned char *N, int NLen,unsigned char *E, int ELen, unsigned char *DataIn,unsigned char *DataOut);
/*
Generate RSA public/private key pair with specified exponent and modulus lengths.
Parameters Modulus [out] Modulus of the key (stored in big-endian format).
PriExp [out] Private exponent (stored in big-endian format).
ModulusLen [in] Modulus length (unit: bytes, can be 64, 128, 256, 512).
PubExp [in] Public exponent. Can only be "\x00\x00\x00\x03" or "\x00\x01\x00\x01".
Returns
RET_OK Success
ERR_INVALID_PARAM Invalid parameter
ERR_GEN_RANDOM Failed to generate random number
ERR_GEN_FAIL Generation failed
*/
int OsRSAKeyGen(unsigned char *Modulus, unsigned char *PriExp, int ModulusLen, const unsigned char * PubExp);


/***************************************6 MediaAudio******************************/
/*
Function: Play an audio file.
Parameters:
    filePath [in] Path to the audio file. Must not be NULL.
Returns:
    0 Success
    -1 filePath is NULL
    Others: Error returned by the media service
*/
int OsPlayAudio(char* filePath);

/*
Function: Stop audio playback.
Parameters: None.
Returns:
    0 Success
    Others: Error returned by the media service
*/
int OsStopPlayAudio();

/*
Function: Get the audio playback status.
Parameters: None.
Returns:
    1 Audio is playing
    Other values Audio is not playing or an error occurred
*/
int OsGetAudioPlayStatus();

/*
Function: Get the current audio volume.
Parameters: None.
Returns:
    Current volume, in the range [0, 100], or an error code
*/
int OsGetAudioVolume();

/*
Function: Set the audio volume.
Parameters:
    volume [in] Volume level in the range [0, 100].
Returns:
    0 Success
    -1 volume is outside the supported range
    Others: Error returned by the media service
*/
int OsSetAudioVolume(int volume);

/************************************7LCD*********************************/

/*
Function: Set screen brightness.
Parameters: Brightness Brightness value, range is [0, 10]. 0: Backlight off, 10: Brightest.
Default value is 8, other values have no effect.
*/
void OsScrBrightness(int Brightness);

/*
Function: Set screen brightness (percentage). For unsupported devices, fall back to the nearest 10 brightness levels.
Parameter: Pct - brightness value, range [0~100]. 0: turn off backlight (if supported by the device); 100: brightest.
Default value is 80; other values result in no action.
*/
void OsScrBrightnessPercent(int Pct);

/*
Function: Get physical LCD screen size.
Parameters:
    Width [out] Width, in pixels.
    Height [out] Height, in pixels.
Returns: None
*/
void OsScrGetSize(int *Width, int *Height);

/************************************6 PED*********************************/
#define ERR_PED_NO_KEY -3801 // Key does not exist
#define ERR_PED_KEY_IDX_ERR -3802 // Key index error
#define ERR_PED_DERIVE_ERR -3803  // Key hierarchy error during write: source key level is lower than destination key level
#define ERR_PED_CHECK_KEY_FAIL -3804 // Key verification failed
#define ERR_PED_NO_PIN_INPUT -3805 // No PIN input
#define ERR_PED_PIN_INPUT_CANCEL -3806 // PIN input cancelled
#define ERR_PED_WAIT_INTERVAL -3807 // Elapsed time since last call is less than the minimum interval
#define ERR_PED_KCV_MODE_ERR -3808 // KCV mode error
#define ERR_PED_KEY_TAG_ERR -3809 // Key tag error, key cannot be used
#define ERR_PED_KEY_TYPE_ERR -3810 // Key type not supported or mismatched for the specified index
#define ERR_PED_PIN_LEN_ERR -3811 // Length of input PIN does not match expected length
#define ERR_PED_DSTKEY_IDX_ERR -3812 // Destination key index error
#define ERR_PED_SRCKEY_IDX_ERR -3813 // Source key index error
#define ERR_PED_KEY_LEN_ERR -3814 // Key length error
#define ERR_PED_INPUT_PIN_TIMEOUT -3815 // Input PIN timeout
#define ERR_PED_NO_ICC -3816 // IC card does not exist
#define ERR_PED_ICC_INIT_ERR -3817 // IC card initialization error
#define ERR_PED_GROUP_IDX_ERR -3818 // DUKPT group index error
#define ERR_PED_TAMPERED -3819 // PED locked (tampered)
#define ERR_PED_NO_MORE_BUF -3820 // No free buffer
#define ERR_PED_NORMAL_ERR -3821 // PED general error
#define ERR_PED_NEED_ADMIN -3822 // Current operation requires advanced privilege
#define ERR_PED_DUKPT_KSN_OVERFLOW -3823 // Current DUKPT overflow
#define ERR_PED_KCV_CHECK_FAIL -3824 // KCV check failed
#define ERR_PED_SRCKEY_TYPE_ERR -3825 // Key ID type does not match source key type during write
#define ERR_PED_UNSPT_CMD -3826 // Command not supported
#define ERR_PED_ADMIN_ERR -3827 // Failed to access sensitive system service
#define ERR_PED_DOWNLOAD_INACTIVE -3828 // PED download feature not activated
#define ERR_PED_KCV_ODD_CHECK_FAIL -3829 // KCV odd parity check failed
#define ERR_PED_PED_DATA_RW_FAIL -3830 // Failed to read PED data
#define ERR_PED_ICC_CMD_ERR -3831 // IC card operation error (offline plaintext/ciphertext PIN verification)
#define ERR_PED_DUKPT_NEED_INC_KSN -3832  // DUKPT KSN must be incremented first
#define ERR_PED_DUKPT_NO_KCV -3833 // DUKPT key has no KCV
#define ERR_PED_NO_FREE_FLASH -3834 // Insufficient PED flash storage space
#define ERR_PED_INPUT_CLEAR -3835 // User pressed CLEAR key to exit PIN input
#define ERR_PED_INPUT_BYPASS_BYFUNCTION -3836 // User pressed FN/ATM4 key to cancel PIN input
#define ERR_PED_NO_PIN_MODE -3837 // PIN input mode not set
#define ERR_PED_DATA_MAC_ERR -3838 // Data MAC check error
#define ERR_PED_DATA_CRC_ERR -3839 // Data CRC check error
#define ERR_PED_KEY_VALUE_INVALID -3840 // Working key value is invalid or already exists

#define PED_TLK 0x01 // Loading Key 
#define PED_TMK 0x02 // Master Key 
#define PED_TPK 0x03 // PIN Key 
#define PED_TAK 0x04 // MAC Key 
#define PED_TDK 0x05 // Data Key 
#define PED_TIK 0x10 // DUKPT Initial Key 
#define PED_TRK 0x07 // MSR Key 
#define PED_TAESK 0x20 // AES Key 
#define PED_SM2_PVT_KEY 0x30 // SM2 Private Key 
#define PED_SM2_PUB_KEY 0x31 // SM2 Public Key 
#define PED_SM4_TMK 0x32 // SM4 Master Key 
#define PED_SM4_TPK 0x33 // SM4 PIN Key 
#define PED_SM4_TAK 0x34 // SM4 MAC Key 
#define PED_SM4_TDK 0x35 // SM4 Data Key 
#define PED_AES_TLK 0x27 // AES Loading Key 
#define PED_AES_TMK 0x28 // AES Master Key 
#define PED_AES_TPK 0x23 // AES PIN Key 
#define PED_AES_TAK 0x24 // AES MAC Key 
#define PED_AES_TDK 0x20 // AES Data Key 
#define PED_AES_TIK 0x51 // AES Initial Key 


#define  PED_ASTERISK_ALIGN_LEFT 0    // Left aligned 
#define  PED_ASTERISK_ALIGN_CENTER 2  // Centered 
#define  PED_ASTERISK_ALIGN_RIGHT 1    // Right aligned 

typedef struct { 
    int ModulusLen; /* Modulus length (bits) */ 
    unsigned char Modulus[512]; /* Modulus, left-padded with 0x00 if less than 512 bytes */ 
    int ExponentLen; /* Exponent length (bits) */ 
    unsigned char Exponent [512]; /* Exponent, left-padded with 0x00 if less than 512 bytes */ 
    unsigned char KeyInfo[128]; /* RSA key info */ 
 } ST_RSA_KEY; 

typedef struct { 
    int ModulusLen; /* PIN encryption public key modulus length (bits) */ 
    unsigned char Modulus[256]; /* PIN encryption public key modulus */ 
    unsigned char Exponent [4]; /* PIN encryption public key exponent */ 
    int IccRandomLen; /* Length of random number obtained from IC card */ 
    unsigned char IccRandom[8]; /* Random number obtained from IC card */ 
} ST_RSA_PINKEY; 

/************************************6.4 Basic PED *********************************/
/*
Function: Open the Prolin PED service.
Parameters: None.
Returns: 
    RET_OK Success 
    ERR_DEV_BUSY Device busy 
*/
int OsPedOpen (void); 

/*
Function: Get the current PED version.
Parameters: PedVer [out] PED version information buffer, 6 bytes. 
Returns: 
    RET_OK Success 
    ERR_DEV_NOT_OPEN PED device not opened 
    ERR_INVALID_PARAM Invalid parameter 
*/
int OsPedGetVer (unsigned char * PedVer); 

/*
Function: Erase all key information saved by the PED service.
Parameters: None.
Returns: 
    RET_OK Success 
    ERR_DEV_NOT_OPEN Device not opened 
    Others: Please refer to the PED function return value list.
*/
int OsPedEraseKeys (void); 

/*
Function: Disconnect from the PED service.
Parameters: None.
Returns: None.
Usage: This function must be called to close the device before the program exits.
*/
void OsPedClose (void); 


/************************************6.5 PIN Input *********************************/

/*
Function: Listen and get the number of PIN key presses entered by the user in the current state, and the historical key sequence between this listening session and the last one.
Parameters: 
    KeyCacheCnt [out] Number of historical keys obtained.
    KeyCache [out] Not implemented yet.
Returns: 
    >= 0 Successfully obtained PIN input event, size of return value represents the number of asterisks currently on the PIN input screen.
    ERR_DEV_NOT_OPEN PED device not opened 
    ERR_INVALID_PARAM Invalid parameter 
    Others: Please refer to the PED function return value list.
*/
int OsPedPinKeyNotify(int *KeyCacheCnt, unsigned char *KeyCache);

/*
Function: Set which line the PIN input box is displayed on and its alignment (default is 3rd line, left-aligned).
Parameters: None.
Returns: RET_OK Success
*/
int OsPedSetAsteriskLayout(int line,int align);

/*
Function: Inject a key into the PED using a TR-31 key block.
Parameters:
    SrcKeyType Source key type, such as PED_TLK or PED_TMK.
    SrcKeyIdx Source key index. When SrcKeyType is PED_TLK, this must be 1;
        otherwise, the valid range is [1, 255].
    DstKeyIdx Destination key index, range [1, 255].
    KeyBlock [in] TR-31 key block containing the key to inject. Supported
        key usages include DUKPT initial, PIN, MAC, data, and master keys.
    KeyBlkLen Length of KeyBlock in bytes; must be greater than 16.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM KeyBlock is NULL or KeyBlkLen is invalid
    ERR_PED_KEY_TYPE_ERR Source or destination key type is unsupported or invalid
    ERR_PED_KEY_IDX_ERR Source or destination key index is invalid
    ERR_PED_NORMAL_ERR The PED rejected the key block or the operation failed
*/
int OsPedInjectKeyBlockByTR_31(unsigned char SrcKeyType,unsigned char SrcKeyIdx,unsigned char DstKeyIdx,unsigned char *KeyBlock, int KeyBlkLen);

/************************************6.6 MK/SK *********************************/
//key System 
#define MKSK_KEY_SYSTEM_DES  0x0 // DES 
#define MKSK_KEY_SYSTEM_SM4  0x03 //SM4
void OsPedMKSKSetKeySystem (int keySystem);
/*
Function: Write a key, including writing and derivation of TLK, TMK, TWK, SM4_TMK, and SM4_TWK. KCV can optionally be used to verify key correctness.
Parameters: KeyBlock [in] Refer to specifications for details.
Returns:
    0 Success
    ERR_DEV_NOT_OPEN PED device not opened
    ERR_INVALID_PARAM Invalid parameter
    ERR_SYS_BAD System error
    ERR_PED_KEY_IDX_ERR Key index error
    ERR_PED_KEY_TYPE_ERR Key type error
    ERR_PED_TAMPERED PED is locked
    ERR_PED_NO_MORE_BUF Insufficient system memory
    ERR_PED_NORMAL_ERR PED general error (KeyBlock format error)
    ERR_PED_DERIVE_ERR Key derivation error
    ERR_PED_KCV_MODE_ERR PED KCV verification mode error
    ERR_PED_KCV_CHECK_FAIL PED KCV verification failed
    Others: Please refer to the PED function return value list.
*/
int OsPedWriteKey (const unsigned char * KeyBlock);

/*
Function: Scan for PIN input of expected length (ExpPinLen) on the keypad within the specified timeout, and output the PIN block encrypted using the algorithm specified by Mode.
Parameters:
    KeyIdx Index of TPK, range [1, 100].
    DataIn [in]
    TimeoutMs Keypad input timeout in milliseconds. Maximum is 3000000: means no timeout (PED will not perform timeout control).
    PinBlock [out] 8 or 16 bytes, points to the generated PIN block. When Mode is 0x10, PinBlock size is 16 bytes.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN PED device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetPinBlock (int KeyIdx,const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *PinBlock);
 
typedef void (*PIN_STAR_DISPLAY)(int len);// PIN pad display asterisk function pointer, len is current PIN length
/*
Function: Scan for PIN input of expected length (ExpPinLen) on the keypad within the specified timeout, and output the PIN block encrypted using the algorithm specified by Mode.
Parameters:
    KeyIdx Index of TPK, range [1, 100].
    DataIn [in]
    TimeoutMs Keypad input timeout in milliseconds. Maximum is 3000000: means no timeout (PED will not perform timeout control).
    PinBlock [out] 8 or 16 bytes, points to the generated PIN block. When Mode is 0x10, PinBlock size is 16 bytes.
    pin_star_display [in] Pointer to the asterisk display function.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN PED device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED return values.
*/
int OsPedGetPinBlockEx (int KeyIdx,const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *PinBlock,PIN_STAR_DISPLAY pin_star_display);

/*
Function: Get an encrypted PIN block using the soft keyboard.The format of PinBlock encrypted using the DES-ECB (3DES-ECB) algorithm,ISO9564 Format 0
Parameters:
    KeyIdx TPK index, range [1, 100].
    DataIn [in] Null-terminated card number used to generate the PIN block.
    minPinLen Minimum permitted PIN length.
    maxPinLen Maximum permitted PIN length.
    TimeoutMs PIN input timeout in milliseconds.
    PinBlock [out] Buffer receiving the PIN block;
    pin_star_display [in] Callback invoked to display the current PIN length.
Returns:
    RET_OK Success
    ERR_PED_NO_PIN_INPUT No PIN was entered
    ERR_PED_INPUT_PIN_TIMEOUT PIN input timed out
    ERR_PED_PIN_INPUT_CANCEL PIN entry was canceled
    Others: PIN entry or PIN block generation failed
*/
int OsPedGetPinBlockBySoftKeyboard(int KeyIdx,const unsigned char *DataIn, int minPinLen,int maxPinLen,unsigned long TimeoutMs, unsigned char *PinBlock,PIN_STAR_DISPLAY pin_star_display);
/*
Function: Calculate MAC on DataIn using the MAC key specified by KeyIdx and the algorithm specified by Mode.
Parameters:
    KeyIdx Index of TAK, range [1, 100].
    DataIn [in] Data to perform MAC calculation, length <= 2032 bytes.
    DataInLen Length of data to perform MAC. If the length is not a multiple of 8, it is automatically padded with "\x00".
    Mac [out] 8-byte buffer to store the generated MAC.
    Mode MAC calculation mode (blocks DataIn into 8-byte chunks: BLOCK1, BLOCK2, BLOCK3, etc.).
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN PED device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetMac(int KeyIdx, const unsigned char *DataIn, int DataInLen, unsigned char *Mac, int Mode);

/*
Function: Use TDK to perform DES/TDES encryption or decryption on DataIn with length DataInLen, producing plaintext or ciphertext. A designated TDK can only be used for either encryption or decryption.
Parameters:
    KeyIdx Index of TDK, range [1, 100].
    InitVector [in] Initialization Vector (IV) for encryption/decryption, 8 bytes. Required for Mode=0x02/0x03/0x04/0x05, default is all zeros if NULL. Not required when Mode=0x00/0x01 (can be NULL).
    DataIn [in] Pointer to the data on which encryption/decryption is performed.
    DataInLen Length of the data in bytes, must be <= 1024. For Mode 0x00 - 0x05, must be a multiple of 8; for CFB8 modes 0x06 - 0x07, does not need to be a multiple of 8.
    DataOut [out] Pointer to the buffer to store processed data.
    Mode Mode settings: 0x00: ECB Decrypt, 0x01: ECB Encrypt, 0x02: CBC Decrypt, 0x03: CBC Encrypt, 0x04: OFB Decrypt, 0x05: OFB Encrypt, 0x06: CFB8 Decrypt, 0x07: CFB8 Encrypt.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN PED device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedDes(int KeyIdx,unsigned char * InitVector, const unsigned char *DataIn, int DataInLen, unsigned char *DataOut, int Mode);

/*
Function: Get the key check value (KCV) for key verification. If KeyType is not TIK, the designated key is used to encrypt dummy data and return the first 3 bytes. If KeyType is TIK, it returns the 8-byte KCV loaded alongside TIK.
Parameters:
    KeyType Key Type.
    KeyIdx Key index.
    KcvMode KCV verification mode.
    KcvDataLen Data length for KCV calculation, <= 128 bytes and multiple of 8. Can be 0 for TIK. For KCV modes 0x03 or 0x04, length must be a multiple of 16.
    KcvData [in] Data for KCV calculation. For TIK, this can be NULL.
    Kcv [out] Output KCV. 8 bytes for TIK and AES-TIK, 5 bytes for AES, 3 bytes for other types.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN PED device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetKcv(int KeyType, int KeyIdx, int KcvMode, int KcvDataLen, unsigned char *KcvData, unsigned char *Kcv);


/************************************6.7 DUKPT*********************************/
/*
Function: Write a TIK key, with the option to verify key correctness using KCV.
Parameters: KeyBlock [in]
Returns:
    ERR_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedWriteTIK (const unsigned char *KeyBlock);

/*
Function: Within the specified timeout, scan the PIN input from the keypad and output the PIN block generated by the DUKPT PIN key.
Parameters:
    GroupIdx DUKPT group index, range [1, 100].
    DataIn [in]
    ExpPinLen [in]
    Mode Select the format of the PIN block.
    TimeoutMs Timeout in milliseconds. Maximum is 3000000: means no timeout (no timeout control).
    Ksn [out] 10 bytes, points to the current KSN.
    PinBlock [out] 8 bytes, points to the generated PIN block.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetPinDukpt (int GroupIdx, const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *Ksn, unsigned char *PinBlock);
/*
Function: Within the specified timeout, scan the PIN input from the keypad and output the PIN block generated by the DUKPT PIN key.
Parameters:
    GroupIdx DUKPT group index, range [1, 100].
    DataIn [in]
    ExpPinLen [in]
    Mode Select the format of the PIN block.
    TimeoutMs Timeout in milliseconds. Maximum is 3000000: means no timeout (no timeout control).
    Ksn [out] 10 bytes, points to the current KSN.
    PinBlock [out] 8 bytes, points to the generated PIN block.
    pin_star_display [in] Pointer to the asterisk display function.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetPinDukptEx (int GroupIdx, const unsigned char *DataIn, const char *ExpPinLen, int Mode, unsigned long TimeoutMs, unsigned char *Ksn, unsigned char *PinBlock,PIN_STAR_DISPLAY pin_star_display);

/*
Function: Get a DUKPT-encrypted PIN block using the soft keyboard.The format of PinBlock encrypted using the DES-ECB (3DES-ECB) algorithm,ISO9564 Format 0
Parameters:
    KeyIdx DUKPT key group index, range [1, 100].
    DataIn [in] Null-terminated card number used to generate the PIN block.
    minPinLen Minimum permitted PIN length.
    maxPinLen Maximum permitted PIN length.
    TimeoutMs PIN input timeout in milliseconds.
    Ksn [out] 10-byte buffer receiving the current key serial number.
    PinBlock [out] 8-byte buffer receiving the PIN block.
    pin_star_display [in] Callback invoked to display the current PIN length.
Returns:
    RET_OK Success
    ERR_PED_NO_PIN_INPUT No PIN was entered
    ERR_PED_INPUT_PIN_TIMEOUT PIN input timed out
    ERR_PED_PIN_INPUT_CANCEL PIN entry was canceled
    Others: PIN entry or PIN block generation failed
*/
int OsPedGetPinBlockDukptBySoftKeyboard(int KeyIdx,const unsigned char *DataIn, int minPinLen,int maxPinLen,unsigned long TimeoutMs, unsigned char *Ksn,unsigned char *PinBlock,PIN_STAR_DISPLAY pin_star_display);
/*
Function: Use the DUKPT key to calculate the MAC value.
Parameters:
    GroupIdx DUKPT group index, range [1, 100].
    DataIn [in] Points to the data content to perform MAC calculation.
    DataInLen Length of data, <= 2032 bytes. If not divisible by 8, automatically padded with "\x00".
    Mac [out] Points to the generated MAC.
    Ksn [out] Points to the current KSN.
    Mode MAC calculation mode.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetMacDukpt(int GroupIdx, const unsigned char *DataIn, int DataInLen, unsigned char *Mac, unsigned char *Ksn, int Mode);

/*
Function: Perform encryption or decryption on input data using the DUKPT key.
Parameters:
    GroupIdx DUKPT group index, range [1, 100].
    KeyVarType [in] 0: MAC key, 1: DES key, 2: PIN key
    InitVector [in] Initialization Vector (IV) for encryption/decryption, 8 bytes. Can be NULL.
    DataInLen Length of data to process, <= 8192
    DataIn [in] Points to the data to be processed.
    DataOut [out] Points to the output buffer.
    Ksn [out] Points to the current KSN, 10 bytes.
    Mode Encryption/decryption mode.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedDesDukpt(int GroupIdx, int KeyVarType, unsigned char *InitVector, int DataInLen, unsigned char *DataIn, unsigned char *DataOut, unsigned char *Ksn, int Mode);

/*
Function: Get the current KSN value.
Parameters:
    GroupIdx DUKPT group index, range [1, 100].
    Ksn [out] Points to current KSN, 10 bytes.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedGetKsnDukpt(int GroupIdx, unsigned char *Ksn);
/*
Function: Increase the KSN value of the specified DUKPT group.
Parameters: GroupIdx DUKPT group index, range [1, 100].
Returns: RET_OK Success
    ERR_DEV_NOT_OPEN Device not opened
    ERR_INVALID_PARAM Invalid parameter
    Others: Please refer to the PED function return value list.
*/
int OsPedIncreaseKsnDukpt (int GroupIdx);


/*
Function: Close the soft keyboard.
Usage: Call this function to stop or dismiss the soft keyboard.
*/
void OsCloseSoftKeyboard();

/************************************11 Printer *********************************/

#define ALIGN_TYPE_LEFT 0    // Left
#define ALIGN_TYPE_CENTER 1   // Center
#define ALIGN_TYPE_RIGHT 2   // Right


#define ERR_PRN_BUSY -3701 // Printer busy
#define ERR_PRN_PAPEROUT -3702 // Printer out of paper
#define ERR_PRN_WRONG_PACKAGE -3703 // Package format error
#define ERR_PRN_OVERHEAT -3704 // Printer overheated
#define ERR_PRN_OUTOFMEMORY -3705 // Printable data too large, exceeding buffer length
#define ERR_PRN_OVERVOLTAGE -3706 // Overvoltage

enum{
    PRN_REAL = 0,   // Physical printing device;
    PRN_BMP,        // Virtual printing device, printing results are saved locally in BMP format.
};

/*
Function: Open the printer device, including both physical and virtual printers.
Parameters:
    printertype [in] Printer device type (PRN_REAL: physical printer, PRN_BMP: virtual printer).
    targetname [in] When printing to physical printer this must be NULL;
                  When printing to virtual printer, points to output BMP file path. If file exists, it will be overwritten.
Returns:
    RET_OK Success
    ERR_DEV_NOT_EXIST Device does not exist
    ERR_INVALID_PARAM Invalid parameter
    ERR_DEV_BUSY Device is busy
    ERR_BATTERY_ABSENT Battery absent
    ERR_BATTERY_VOLTAGE_TOO_LOW Battery voltage too low
*/
int OsPrnOpen(unsigned int printertype, const char*targetname );

/*
Function: Reset printer parameters and clear printing buffer.
Parameters: None.
Returns: None.
*/
void OsPrnReset(void);
/*
Function: Close the printer.
Parameters: None.
Returns: None.
Usage: This function must be called when the program exits to close the device.
*/
void OsPrnClose(void);


/*
Function: Set printer gray scale (density).
Parameters: Level 
    Level = 0: Reserved.
    Level = 1: Default blackness (ordinary ticket printing).
    Level = 2: Reserved.
    Level = 3: Double-layer thermal printing.
    Level = 4: Double-layer thermal printing with higher density than level 3.
    Invalid values will not change the setting.
Returns: None.
Usage: This interface is only valid for physical printers.
*/
void OsPrnSetGray(int Level);

/*
Function: Set Character and Line spacing.
Parameters:
    CharSpace Character spacing in dots (not valid for forced non-constant width fonts, like Arabic or Thai).
    LineSpace Line spacing in dots.
Returns: None.
Usage:
1. Effective globally until the next call to setting or OsPrnReset().
2. Default character spacing is 0.
3. Default line spacing is 0 for thermal, 2 for stylus.
4. Maximum line spacing is 255.
5. Maximum character spacing is 255.
6. Invalid parameters will not change the settings.
*/
void OsPrnSetSpace(int CharSpace, int LineSpace);

/*
Function: Set fonts reverse video (invert) display attribute, default is normal print.
Parameters: Attr 
    0: Normal
    1: Inverted (white text on black)
    2: Add a border
Returns: None.
Usage: Valid for both physical and virtual printers. Ineffective for image printing.
*/
void OsPrnSetReversal(int Attr);

/*
Function: Set left and right indentation margins.
Parameters:
    Left [in] Left margin indentation, valid range [0, 100], default 0.
    Right [in] Right margin indentation, valid range [0, 100], default 0.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Invalid parameter
Usage: If physical printer is set to landscape/vertical printing, left margin maps to page top margin, right margin to page bottom margin.
*/
int OsPrnSetIndent (unsigned int Left, unsigned int Right);
/*
Function: Query the current printer status (applicable to physical printers only).
Parameters: None.
Returns:
    RET_OK Printer is working normally
    ERR_PRN_BUSY Printer is busy
    ERR_PRN_PAPEROUT Printer is out of paper
    ERR_PRN_OVERHEAT Printer is overheated
*/
int OsPrnCheck(void);

/*
Function: Set the font file path used for subsequent text printing.
Parameters:
    fontPath [in] Null-terminated font file path. Passing NULL or an empty string clears the current font path.
Returns:
    RET_OK Success
Usage:
    1. The function stores the path but does not check whether the file exists or whether the font format is valid.
    2. The path must be shorter than 256 bytes, including the terminating null character.
*/
int OsPrnSetFont(const char*fontPath);
/*
Function: Set printing parameters (customized).
Parameters:
FontSize Font size: 12, 16, 24
DoubleWidth Double width factor: 1~4 
DoubleHeight Double height factor: 1~4
AlignType Alignment type: Left, Center, Right
Returns: None.
*/
void  OsPrnSetPrintParams(int FontSize,int DoubleWidth, int DoubleHeight,int AlignType);
/*
Function: Set printing parameters (customized).
Parameters:
FontSize Font size: 12, 16, 24
DoubleWidth Double width factor: 1~4 
DoubleHeight Double height factor: 1~4
AlignType Alignment type: Left, Center, Right
IsPersian 1 for Persian language 
Returns: None.
*/
void OsPrnSetPrintParamsEx(int FontSize,int DoubleWidth, int DoubleHeight,int AlignType,int IsPersian);
/*
Function: Feed paper by a specified number of dot lines in the print buffer.
Parameters: Pixel 
    Number of dot lines to feed:
    >0: Feed forward
    <0: Feed backward (retract paper)
    =0: No action
Returns: None.
Usage: Valid for both physical and virtual printers.
*/
void OsPrnFeed(int Pixel);

/*
Function: Format and output a string to the print buffer.
Parameters: Str [in] Pointer to print string.
Returns: None.
Usage:
    1. Supports variable arguments.
    2. Supports print string control characters like '\n' (newline) and '\f' (form feed).
    3. If the printing package is too long, buffer overflow will occur.
    4. Automatically wraps the line if print text exceeds the layout printing width.
    5. Maximum buffer size is 2048 bytes.
    6. Stores Str into print buffer. Once OsPrnStart() is called, data will be printed sequentially.
*/
void OsPrnPrintf(const char *Str, ...);

/*
Function: Directly print formatted raw data.
Parameters:
    data [in] Pointer to string data to print.
    len [in] Length of data to print.
Returns:
    RET_OK Printer works normally
    ERR_INVALID_PARAM Invalid parameter
    ERR_DEV_NOT_OPEN Device failed to open
    ERR_PRN_PAPEROUT Printer is out of paper
    ERR_PRN_OVERHEAT Printer is overheated
    ERR_PRN_OVERVOLTAGE Printer voltage is too high
    ERR_PRN_BUSY Printer device is busy
Usage:
1. Blocks until printing is complete before returning.
2. Returns printer status directly on completion (no need to query again afterwards).
*/
int OsPrnRawData (const char *data, int len);

/*
Function: Output graphic image to the print buffer.
Parameters: Logo [in] Path to logo image file.
Returns: None.
Usage: Steps to generate graphic data:
1. Draw logo: Use Windows MS Paint or similar tools to draw. Save file as monochrome BMP format.
2. Use conversion tools to convert the BMP file into C header containing arrays (e.g. Logo.h).
*/
void OsPrnPutImage(const unsigned char *Logo);
/*
Function: Output monochrome electronic signature BMP data to print buffer.
Parameters: SignData Monochrome electronic signature BMP data
     len Data length
Returns: None.
Usage: Buffer prints exactly the last-set electronic signature only (exactly 1 record, size < 10KB).
*/
void OsPrnPutElecSignData(unsigned char *SignData,int len);

/*
Function: Load electronic signature data to print buffer.
Parameters: None.
Returns: None.
Usage: Buffer prints exactly the last-set electronic signature only (exactly 1 record, size < 10KB).
*/
void OsPrnPutElecSignDataByJpg();
/*
Function: Start printer to output buffered data.
Parameters: None.
Returns:
    RET_OK Printer is working normally
    ERR_PRN_BUSY Printer is busy
    ERR_PRN_PAPEROUT Printer is out of paper
    ERR_PRN_WRONG_PACKAGE Data package format error
    ERR_PRN_OVERHEAT Printer is overheated
    ERR_PRN_OUTOFMEMORY Print data is too large
Usage:
    1. Blocks until printing is complete before returning.
    2. Returns printer status directly on completion (no need to query again afterwards).
    3. If printing succeeded, calling this again will reprint the ticket block.
*/  
int OsPrnStart(void);

/*
Function: Clear print buffer.
Parameters: None.
Returns: None.
*/
void OsPrnClrBuf(void);

/*
Function: Set printer pre-feed paper behavior.
Parameters: cmd [in] 
                 1: Printing without pre-feed paper
                 2: Printing with pre-feed paper
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Invalid parameter
Usage: 1. This function only affects pre-feed paper feature, which defaults to on/enabled. 2. Thermal printers only.
*/
int OsPrnSetParam(unsigned int cmd);

/*
Function: Enable or disable complex text rendering mode.
Parameters:
    iValue [in] 0 to disable complex rendering; any non-zero value to enable it.
Returns:
    RET_OK Success
Usage:
    1. When enabled, consecutive text without a newline is combined on the same print line. A newline ends the current combined line.
    2. The mode is automatically disabled after OsPrnStart finishes processing the print queue or encounters a printing error.
*/
int OsPrnComplexMode(int iValue);

/************************************13 Code Convert *********************************/

/*
Function: Character encoding conversion.
Parameters:
    FromCharset [in] Source character encoding.
    ToCharset [in] Target character encoding.
    InBuf [in] Input string of source encoding, null-terminated with '\0' (unicode requires double null-termination '\0\0').
    OutBuf [out] Converted target encoding string.
    LenOut [in] Size of OutBuf array, should be at least 1.5 times the size of InBuf.
Returns:
    >=0 Conversion successful, returns the length of converted string
    ERR_INVALID_PARAM Invalid parameter
*/
int OsCodeConvert (const char *FromCharset, const char *ToCharset,const char *InBuf, char *OutBuf, unsigned int LenOut);

/************************************14 Magnetic Strip Reader *********************************/
// 14 Magnetic Strip Reader
#define ERR_MSR_FAILED -2701 // Operation failed
#define ERR_MSR_HEADERR -2702 // Start sentinel not found
#define ERR_MSR_ENDERR -2703 // End sentinel not found
#define ERR_MSR_LRCERR -2704 // LRC check error
#define ERR_MSR_PARERR -2705 // Track parity check error
#define ERR_MSR_NOT_SWIPED -2706 // Card not swiped
#define ERR_MSR_NO_DATA -2707 // Magnetic card contains no data
#define ERR_MSR_END_ZEROERR -2708 // Magnetic card data format error
#define ERR_MSR_PED_DECRYPTERR -2709 // PED decryption failed
#define ERR_MSR_NO_TRACK_ERR -2710 // Target track not detected on magnetic card

// ST_MSR_DATA: Track data structure
typedef struct
{
    unsigned char TeackData[256]; /* Decoded bitstream */
    int DataLen; /* Track data length */
    int Status; /* Track data status. 0 indicates read success; outer values indicate failure. */
}ST_MSR_DATA;


/*
Function: Open the magnetic strip reader device.
Parameters: None.
Returns:
    RET_OK Success
    ERR_DEV_NOT_EXIST Device does not exist
    ERR_DEV_BUSY Device is busy
*/
int OsMsrOpen(void);
/*
Function: Close the magnetic strip reader device.
Parameters: None.
Returns: None.
Usage: This function must be called when the program exits to close the device.
*/
void OsMsrClose(void);
/*
Function: Soft-reset the magnetic strip reader and clear any swiped card data.
Parameters: None.
Returns: None.
*/
void OsMsrReset(void);
/*
Function: Check if a card has been swiped.
Parameters: None.
Returns:
    TRUE Swiped
    FALSE Not swiped
    ERR_DEV_NOT_OPEN Device not opened
*/
int OsMsrSwiped(void);
/*
Function: Read magnetic strip card data.
Parameters: 
     Track1 [out] Output Track 1 data
     Track2 [out] Output Track 2 data
     Track3 [out] Output Track 3 data
Returns:
    RET_OK Successfully read
    ERR_MSR_NOT_SWIPED Card not swiped
    ERR_INVALID_PARAM Invalid parameter
    ERR_DEV_NOT_OPEN Device not opened
*/
int OsMsrRead(ST_MSR_DATA *Track1, ST_MSR_DATA *Track2, ST_MSR_DATA *Track3);



/************************************15 IC Card Reader *********************************/

#define ERR_SCI_HW_NOCARD -2800 // No card inserted
#define ERR_SCI_HW_STEP -2801 // Not initialized during data exchange, or power-off during warm reset
#define ERR_SCI_HW_PARITY -2802 // Parity error
#define ERR_SCI_HW_TIMEOUT -2803 // Timeout
#define ERR_SCI_TCK -2804 // TCK error
#define ERR_SCI_ATR_TS -2810 // ATR TS error
#define ERR_SCI_ATR_TA1 -2811 // ATR TA1 error
#define ERR_SCI_ATR_TD1 -2812 // ATR TD1 error
#define ERR_SCI_ATR_TA2 -2813 // ATR TA2 error
#define ERR_SCI_ATR_TB1 -2814 // ATR TB1 error
#define ERR_SCI_ATR_TB2 -2815 // ATR TB2 error
#define ERR_SCI_ATR_TC2 -2816 // ATR TC2 error
#define ERR_SCI_ATR_TD2 -2817 // ATR TD2 error
#define ERR_SCI_ATR_TA3 -2818 // ATR TA3 error
#define ERR_SCI_ATR_TB3 -2819 // ATR TB3 error
#define ERR_SCI_ATR_TC3 -2820 // ATR TC3 error
#define ERR_SCI_T_ORDER -2821 // Protocol is neither T0 nor T1
#define ERR_SCI_PPS_PPSS -2830 // PPSS error on PPS
#define ERR_SCI_PPS_PPS0 -2831 // PPS0 error on PPS
#define ERR_SCI_PPS_PCK -2832 // ATR PCK error on PPS
#define ERR_SCI_T0_PARAM -2840 // Transmitted data is too long on T0 protocol
#define ERR_SCI_T0_REPEAT -2841 // Character repetition exceeds limit on T0 protocol
#define ERR_SCI_T0_PROB -2842 // Program byte error during T0 protocol
#define ERR_SCI_T1_PARAM -2850 // Transmitted data is too long in T1 protocol
#define ERR_SCI_T1_BWT -2851 // T1 BWT too large
#define ERR_SCI_T1_CWT -2852 // T1 CWT too large
#define ERR_SCI_T1_BREP -2853 // T1 too many block repetitions
#define ERR_SCI_T1_LRC -2854 // T1 LRC error
#define ERR_SCI_T1_NAD -2855 // T1 NAD error
#define ERR_SCI_T1_LEN -2856 // T1 LEN error
#define ERR_SCI_T1_PCB -2857 // T1 PCB error
#define ERR_SCI_T1_SRC -2858 // T1 SRC error
#define ERR_SCI_T1_SRL -2859 // T1 SRL error
#define ERR_SCI_T1_SRA -2860 // T1 SRA error
#define ERR_SCI_PARAM -2880 // Invalid parameter

typedef struct
{
    unsigned char Cmd[4]; /* CLA, INS, P1, P2 */
    int LC; /* Valid length of DataIn sent to IC card */
    unsigned char DataIn[512]; /* Data sent to IC card */
    int LE; /* Expected length of returned data */
}ST_APDU_REQ;

typedef struct
{
    int LenOut; /* Actual length of data returned from IC card */
    unsigned char DataOut[512]; /* Data buffer returned from IC card */
    unsigned char SWA; /* IC card status word 1 (SWA) */
    unsigned char SWB; /* IC card status word 2 (SWB) */
}ST_APDU_RSP;

enum{
    ICC_USER_SLOT = 0,   // User Card
    ICC_SAM1_SLOT, // SAM Slot 1
    ICC_SAM2_SLOT, // SAM Slot 2
    ICC_SAM3_SLOT, // SAM Slot 3
    ICC_SAM4_SLOT, // SAM Slot 4
};

/*
Function: Open IC card reader device.
Parameters: Slot
    IC card slot/channel number:
    ICC_USER_SLOT: User card
    ICC_SAM1_SLOT: SAM Slot 1
    ICC_SAM2_SLOT: SAM Slot 2
    ICC_SAM3_SLOT: SAM Slot 3
    ICC_SAM4_SLOT: SAM Slot 4
Returns:
    RET_OK Success
    ERR_DEV_NOT_EXIST Device does not exist
    ERR_DEV_BUSY Device busy
*/
int OsIccOpen(int Slot);

/*
Function: Check if card is inserted into the designated slot.
Parameters: Slot
    IC card slot/channel number:
    ICC_USER_SLOT: User card
    ICC_SAM1_SLOT: SAM Slot 1
    ICC_SAM2_SLOT: SAM Slot 2
    ICC_SAM3_SLOT: SAM Slot 3
    ICC_SAM4_SLOT: SAM Slot 4
Returns:
    RET_OK Card inserted
    Others: Please refer to the function return value list.
*/
int OsIccDetect(int Slot);

/*
Function: Initialize IC card device.
Parameters:
Slot
IC card slot/channel number:
ICC_USER_SLOT: User card
ICC_SAM1_SLOT: SAM Slot 1
ICC_SAM2_SLOT: SAM Slot 2
ICC_SAM3_SLOT: SAM Slot 3
ICC_SAM4_SLOT: SAM Slot 4
Option (Bits 0-1) Voltage options: 00 - 5V, 01 - 1.8V, 10 - 3V
(Bit 2) 0;
(Bits 3-4) 0;
(Bit 5) Supported specification: 0 - EMV, 1 - ISO7816
(Bits 6-31) Reserved. Option default is 0 (i.e. 5V, non-PPS, standard rate, EMV compliant).
Atr [out]
1. Answer To Reset (ATR), cards return up to 34 bytes of response data.
2. Content structure: response length (1 byte) + ATR contents.
Returns:
RET_OK Success
Others: Please refer to the function return value list.
*/
int OsIccInit(int Slot,unsigned long Option,unsigned char *Atr);

/*
Function: Transmit and receive commands to/from the IC card (APDU exchange).
Parameters:
Slot
    IC card slot/channel number:
    ICC_USER_SLOT: User card
    ICC_SAM1_SLOT: SAM Slot 1
    ICC_SAM2_SLOT: SAM Slot 2
    ICC_SAM3_SLOT: SAM Slot 3
    ICC_SAM4_SLOT: SAM Slot 4
CtrlFlag
1. Bit 0: Whether to automatically send GET RESPONSE under T=0:
1 - Do not auto-retry
0 - Default, auto-retry
2. Bits 1-31: Reserved
ApduReq [in] Command APDU structure sent to the IC card.
ApduRsp [out] Response APDU structure received from the IC card.
Returns:
    RET_OK Success
    Others: Please refer to the function return value list.
*/
int OsIccExchange(int Slot,int CtrlFlag,const ST_APDU_REQ *ApduReq,ST_APDU_RSP *ApduRsp);

/*
Function: Plain transmit/receive data block transceive.
Parameters: Slot
    IC card slot/channel number:
    ICC_USER_SLOT: User card
    ICC_SAM1_SLOT: SAM Slot 1
    ICC_SAM2_SLOT: SAM Slot 2
    ICC_SAM3_SLOT: SAM Slot 3
    ICC_SAM4_SLOT: SAM Slot 4

CtrlFlag Reserved.
pucTxBuff [in] Buffer of data to be transmitted.
iTxLen [in] Length of data to be transmitted, in bytes.
pucRxBuff [out] Buffer to receive the card's response.
piRxLen [out] Number of bytes received.
Returns:
0 Success
Others: Failed (refer to function return value list).
*/
int OsIccTransfer(int Slot,int CtrlFlag, const unsigned char *pucTxBuff,int iTxLen,unsigned char *pucRxBuff,int *piRxLen);
/*
Function: Close IC card device.
Parameters: Slot
    IC card slot/channel number:
    ICC_USER_SLOT: User card
    ICC_SAM1_SLOT: SAM Slot 1
    ICC_SAM2_SLOT: SAM Slot 2
    ICC_SAM3_SLOT: SAM Slot 3
    ICC_SAM4_SLOT: SAM Slot 4
Returns:
    RET_OK Success
    Others: Please refer to the function return value list.
*/
int OsIccClose(int Slot);


/************************************16 RF Card Reader *********************************/
#define PCD_ERR_PAR_FLAG -2901 // Parity error
#define PCD_ERR_CRC_FLAG -2902 // CRC error
#define PCD_ERR_WTO_FLAG -2903 // Timeout or no card
#define PCD_ERR_COLL_FLAG -2904 // Collision / Multiple cards
#define PCD_ERR_ECD_FLAG -2905 // Frame format error
#define PCD_ERR_EMD_FLAG -2906 // Noise / Interference
#define PCD_ERR_COM_FLAG -2907 // Chip fault, cannot communicate
#define PCD_ERR_AUT_FLAG -2908 // M1 Authentication error
#define PCD_ERR_TRANSMIT_FLAG -2909 // Transmission error
#define PCD_ERR_PROTOCOL_FLAG -2910 // Protocol error
#define PCD_ERR_PARAMFILE_FLAG -2911 // Configuration file does not exist
#define PCD_ERR_USER_CANCEL -2912 // User cancelled transaction
#define PCD_ERR_CARRIER_OBTAIN_FLAG -2913 // RF Carrier wave not obtained
#define PCD_ERR_CONFIG_FLAG -2914 // Register configuration failed
#define PCD_ERR_RXLEN_EXCEED_FLAG -2915 // Response length exceeds maximum buffer size
#define PCD_ERR_NOT_ALLOWED_FLAG -2951 // Invalid parameter value
#define PCD_CHIP_ABNORMAL -2952 // Chip does not exist or abnormal
#define PCD_CHIP_NOT_OPENED -2953 // Module not opened
#define PCD_CHIP_CARDEXIST -2954 // Card not removed
#define PCD_ERR_NOT_IDLE_FLAG -2955 // Card not in IDLE state
#define PCD_ERR_NOT_POLLING_FLAG -2956 // Card not in POLLING status
#define PCD_ERR_NOT_WAKEUP_FLAG -2957 // Card not in WAKEUP status
#define PCD_ERR_NOT_ACTIVE_FLAG -2958 // Card not activated
#define PCD_ERR_NOT_SUPPORT -2959 // Chip not supported

typedef struct pcd_user_t{
    unsigned char wait_retry_limit_w; /* Authorization flag to write S(WTX) response retry limits */
    unsigned int wait_retry_limit_val; /* S(WTX) response maximum retry limits value */
    unsigned char check_cancel_key_w; /* Authorization flag to check cancellation key response */
    unsigned char check_cancel_key_val; /* 0: Don't respond to cancel key, 1: Respond to cancel key */
    int (*check_cancel_key_function)(void);/* Cancel key callback detection function. If check_cancel_key_w=1 and check_cancel_key_val=1, this callback is invoked during transactions. Returning 0 means no cancel key pressed; returning non-zero forces transaction exit. */
    unsigned char os_picc_transfer_set_w; /* 1: os_picc_transfer_set_val is valid, 0: invalid */
    unsigned char os_picc_transfer_set_val; /* OsPiccTransfer transceive settings. Bit 0: 0=send disabled CRC, 1=send enabled CRC. Bit 1: 0=receive disabled CRC, 1=receive enabled CRC. */
    unsigned char anti_interference_flag; /* Anti-interference card polling: 1-Enabled, 0-Disabled. */
    unsigned char protocol_layer_fwt_set_w; /* 1: protocol_layer_fwt_set_val is valid, 0: invalid */
    unsigned int protocol_layer_fwt_set_val; /* Set frame waiting time (FWT) value for protocol layer */
    unsigned char os_picc_transfer_rxlen_set_w; /* 1: os_picc_transfer_rxlen_set_val is valid, 0: invalid */
    unsigned int os_picc_transfer_rxlen_set_val; /* Max readable data length inside OsPiccTransfer half-duplex block protocol */
    unsigned char os_picc_transfer_direct_transmit_set_w; /* 1: Stream-based transceive (bypass half-duplex protocol block format), 0: Use standard block format */
    unsigned char configure_technology_type_w; /* Authorization to write technology type modulation parameter: 1-Allow, others-Do not allow */
    unsigned char configure_technology_type_val; /* Config technology type modulation for OsPiccTransfer data transmissions */
    unsigned char reserved[34]; /* Reserved bytes, sizeof(PCD_USER_ST) = 76 */
}PCD_USER_ST;

/*
Function: PCD module power-on, initialising modules to a ready state.
Parameters: None.
Returns:
    0 Open device successful
    ERR_BATTERY_VOLTAGE_TOO_LOW Battery voltage too low
    ERR_BATTERY_ABSENT Battery does not exist
    Others: Open device failure (refer to function return codes).
*/
int OsPiccOpen(void);

/*
Function: PCD module power-down.
Parameters: None.
Returns:
    0 Success
    Others: Failed (refer to return codes list).
*/
int OsPiccClose(void);

/*
Function: RF carrier reset.
Parameters: None.
Returns:
    0 RF carrier reset successful
    Others: RF carrier reset failed (refer to return codes list).
*/
int OsPiccResetCarrier(void);


/*
Function: Poll card with power-on, currently supports "A" and "B" card types.
Parameters:
    pcPiccType [out] Found card type: "A" - A card, "B" - B card.
    pucATQx [out] Answer to Request: 2 bytes for Type A, 12 bytes for Type B.
Returns:
    0 Card found successfully
    Others: Card polling failed (refer to return codes list).
*/
int OsPiccPoll(char*pcPiccType, unsigned char*pucATQx);


/*
Function: Transmit and receive raw data.
Parameters: 
    pucTxBuff [in] Transmit data buffer.
    iTxLen [in] Length of data to transmit, in bytes.
    pucRxBuff [out] Receive response data buffer.
    piRxLen [out] Number of received bytes.
Returns:
    0 Success
    Others: Failed (refer to return codes list).
*/
int OsPiccTransfer(const unsigned char*pucTxBuff,int iTxLen, unsigned char*pucRxBuff, int *piRxLen);

/*
Function: Perform card removal procedure according to EMV standard.
Parameters: None.
Returns:
    0 Success
    Others: Failed
*/
int OsPiccRemove (void);

/*
Function: Transmit APDU formatted data on designated channel and receive response.
Parameters:
    cid [in] Specifies the card logic channel number, range [0, 14], typically always 0.
    ApduReq [in] Command APDU structure sent to the PICC card.
    ApduRsp [out] Response APDU structure received from the PICC card.
Returns:
    0 Success
    Others: Failed (refer to return codes list).
*/
int OsPiccIsoCommand(int cid, ST_APDU_REQ*ApduReq,ST_APDU_RSP*ApduRsp);

/*
Function: Close the RF carrier.
Parameters: None.
Returns:
    0 Close carrier successful
    Others: Failed (refer to return codes list).
Usage: Turn off the RF carrier wave, transitioning any active card in the field to a powered-down status.
*/
int OsPiccOffCarrier(void);

/*
Function: Poll card.
Parameters: 
    pcPiccType [out] Found card type: 'A' - A card, 'B' - B card
    pucATQx [out] Card response data: 2 bytes for Type A, 12 bytes for Type B.
    pATQxLen [out] ATQx response length.
Returns:
    0 Card found successfully
    Others: Failed (refer to return codes list).
*/ 
int OsPiccSearch(char*pcPiccType, unsigned char*pucATQx,int *pATQxLen);

/*
Function: Perform card anti-collision and selection.
Parameters: 
    pcPiccType [in] Polled card type: 'A' - A card, 'B' - B card.
    pucUID [out] Card unique identifier (UID): 4, 7 or 10 bytes for Type A, 4 bytes for Type B.
    pUIDLen [out] UID data length.
    pucSAK [out] SAK response, 1 byte. Ignored for Type B.
Returns:
    0 Card found successfully
    Others: Failed (refer to return codes list).
*/ 
int OsPiccAntiSel(const char pcPiccType,unsigned char *pucUID, int *pUIDLen, unsigned char*pucSAK);

/*
Function: Perform card activation.
Parameters: 
    pcPiccType [in] Polled card type: 'A' - A card, 'B' - B card.
    pucRATS [out] Card activation response data: For Type A, pucRATS is ATS response; For Type B, pucRATS is ATTRIB response.
    pRATSLen [out] Activation response data (ATS/ATTRIB) length.
Returns:
    0 Card found successfully
    Others: Failed (refer to return codes list).
*/ 
int OsPiccActive(const char pcPiccType, unsigned char *pucRATS,int *pRATSLen);

/*
Function: Poll Felica cards.
Parameters:
    pucResp [out] Card Felica response data. Format: 1-byte LEN + n-bytes payload (where LEN = n + 1).
    piRespLen [out] Received response byte length.
Returns:
    0 Card found successfully
    Others: Card polling failed (refer to return codes list).
*/
int OsPiccFelicaPoll(unsigned char*pucResp,int *piRespLen);

/*
Function: Transparent Felica transceive.
Parameters: 
    pucTxBuff [in] Buffer of data to transmit. Format: 1-byte LEN + n-bytes payload (where LEN = n + 1).
    iTxLen [in] Length of data to transmit, in bytes.
    pucRxBuff [out] Buffer to receive card response. Format: 1-byte LEN + n-bytes payload (where LEN = n + 1).
    piRxLen [out] Received response byte length.
Returns:
    0 Success
    Others: Failed (refer to return codes list).
*/
int OsPiccFelicaTransfer(const unsigned char*pucTxBuff,int iTxLen, unsigned char*pucRxBuff, int *piRxLen);


/*
Function: Poll for a contactless card without powering it on.
Parameters: None.
Returns:
    0 Card polling completed successfully
    Others: Card polling failed (refer to the RFID return codes)
*/
int OsPiccPollWithoutPowerOn(void);

/*
Function: Power on the detected contactless card.
Parameters:
    pucATQx [out] Buffer receiving the card's activation response.
    pATQxLen [out] Receives the number of response bytes written to pucATQx.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM An output parameter is NULL
    PCD_ERR_PAR_FLAG Invalid or unavailable response from the RFID service
    PCD_ERR_NOT_ACTIVE_FLAG The card could not be activated
*/
int OsPiccCardPowerOn(unsigned char*pucATQx,int *pATQxLen);

/************************************17 Communication Ports*********************************/
/*
Function: Switch the USB function to serial port mode.
Parameters: None.
Returns:
    0 Success
    Others: Failed
*/
int OsStartUsbSerial();

/*
Function: Query the serial port device name/path.
Parameters: SerialPort [out] Serial port device descriptor, must not be NULL.
Returns:
    0 Success
    Others: Failed
*/
int OsQuerySerialPort(char* SerialPort);
/*
Function: Check if serial port exists within timeout limit.
Parameters: None.
Returns:
    0 Success
    Others: Failed
*/
int OsCheckSerialPortExist(char* SerialPort,int TimeOut_ms);
/*
Function: Restore USB function from serial mode.
Parameters: None.
Returns:
    0 Success
    Others: Failed
*/
int OsStopUsbSerial();

/*
Function: Switch the communication module USB function to serial mode.
Parameters: None.
Returns:
    RET_OK Communication module USB mode switched successfully
    ERR_ACCESS_DENY The system service failed to switch the USB mode
Usage: USB charging is disabled after the communication module enters serial mode.
*/
int OsStartCommModuleUsbSerial();

/*
Function: Restore the communication module from USB serial mode.
Parameters: None.
Returns:
    RET_OK Communication module USB mode restored successfully
    ERR_ACCESS_DENY The system service failed to restore the USB mode
Usage: USB charging is re-enabled after the communication module leaves serial mode.
*/
int OsStopCommModuleUsbSerial();

/*
Function: Enable the terminal USB serial function while keeping USB charging enabled.
Parameters: None.
Returns:
    RET_OK USB serial mode was enabled successfully
    ERR_ACCESS_DENY The system service failed to enable USB serial mode
Usage: Failure of the subsequent charging-control command is not reported by this function.
*/
int OsStartUsbSerialWithUsbcharge();

/************************************21 GPRS/CDMA Wireless Module*********************************/
#define ERR_NET_IF -3307  // Network interface link unavailable (link not established or corresponding device not present)

#define PPP_LOGINING 1  // PPP logging in
#define PPP_LOGOUTING 2 // PPP logging out
#define WL_CSD_READY 3 // CSD dial-up service ready
#define WL_GPRS_CSD_READY 4 // GPRS and CSD dial-up service ready
#define ERR_WL_POWER_ONING -3501 // Wireless module powering up
#define ERR_WL_POWER_OFF -3502 // Wireless module powered off
#define ERR_WL_NOT_INIT -3503 // Wireless module not initialized
#define ERR_WL_NEEDPIN -3504 // SIM card requires PIN
#define ERR_WL_RSPERR -3505 // Wireless module response error
#define ERR_WL_NORSP -3506 // Wireless module no response
#define ERR_WL_NEEDPUK -3507 // SIM card requires PUK
#define ERR_WL_WRONG_PIN -3508 // SIM card PIN error
#define ERR_WL_NOSIM -3509 // No SIM card inserted
#define ERR_WL_NOREG -3510 // Cannot register to network
#define ERR_WL_AUTO_RST -3511 // Wireless module auto-reset
#define ERR_WL_BUF -3512 // Wireless module memory error
#define ERR_WL_GET_SIGNAL -3513 // Obtaining signal, please wait 3 seconds
#define ERR_WL_NOTYPE -3514 // Unrecognized wireless module
#define ERR_WL_PPP_ONLINE -3515 // Module PPP online, cannot enter sleep
#define ERR_WL_ERR_BUSY -3516 // Module busy
#define ERR_WL_SLEEP_ONING -3517 // Wireless module entering sleep
#define ERR_WL_SLEEP_FAIL -3518 // Failed when wireless module entering sleep
#define ERR_WL_SIM_FAILURE -3519 // Wireless module failed to operate SIM card

/*
Function: Open the wireless module. Establish connection with Prolin wireless service and acquire access permissions.
Parameters: None.
Returns:
    RET_OK Open wireless module successfully
    ERR_DEV_BUSY Module/device is already occupied by other applications
    ERR_DEV_NOT_EXIST Module/device does not exist
    ERR_BATTERY_VOLTAGE_TOO_LOW Battery voltage is too low
    ERR_BATTERY_ABSENT Terminal battery does not exist
*/
int OsWlLock(void);

/*
Function: Release access to the wireless module and disconnect from Prolin wireless service.
Parameters: None.
Returns: None.
*/
void OsWlUnLock(void);

/*
Function: Initialize wireless device/module.
Parameters: SimPin [in] SIM card password string pointer, length < 50 bytes. Can be NULL if no password required.
Returns:
    RET_OK Success
    ERR_DEV_NOT_OPEN Device/module not opened
    ERR_DEV_NOT_EXIST Wireless module does not exist
    ERR_NO_PORT Insufficient terminal physical ports
    ERR_WL_NEEDPIN SIM card requires PIN
    ERR_WL_RSPERR Wireless module response error
    ERR_WL_NORSP Wireless module no response
    ERR_WL_NEEDPUK SIM card requires PUK
    ERR_WL_WRONG_PIN PIN error
    ERR_WL_NOSIM No SIM card
    ERR_WL_NOTYPE Unrecognized module type
    ERR_WL_NOREG Cannot register to GPRS network
*/
int OsWlInit(const char *SimPin);

/*
Function: Get wireless signal strength.
Parameters: None.
Returns:
    0~5 Signal strength level: 0 represents no signal; 5 represents strongest signal.
    ERR_DEV_NOT_EXIST Wireless module does not exist
    ERR_WL_RSPERR Wireless module response error
    ERR_WL_POWER_ONING Wireless module in powered-down status
*/
int OsWlGetSignal(void);

/*
Function: Query wireless link status.
Parameters: None.
Returns:
    PPP_LOGOUTING Disconnecting link
    PPP_LOGINING Creating link
    RET_OK Link established successfully
    ERR_DEV_NOT_EXIST Wireless module does not exist
    ERR_WL_POWER_ONING Wireless module powering up
    ERR_WL_POWER_OFF Wireless module powered off
    ERR_WL_NOT_INIT Wireless module not initialized
    ERR_NET_PASSWD Password error
    ERR_NET_LOGOUT Disconnected by application
    ERR_NET_IF Link unavailable (link not established or disconnected)
*/
int OsWlCheck(void);

/*
Function: Log in to wireless network and establish wireless link.
Parameters:
    APN [in] Access Point Name (APN) for GPRS, or dial number for CDMA. Length <= 50 characters. If NULL, the host dials independently and then performs PPP login.
    Name [in] Username, length <= 50 bytes. Must not be NULL; if no username, use empty string "".
    Password [in] Password, length <= 50 bytes. Must not be NULL; if no password, use empty string "".
    Auth Authentication algorithm.
    TimeOutMs Timeout in milliseconds, range [0, 3600000]. < 0 becomes 0, > 3600000 becomes 3600000.
    KeepAlive Link heartbeat check interval in milliseconds, range [0, 3600000]. 0 disables KeepAlive. Values between (0, 10000) are clamped to 10000.
    ReserParam Reserved parameter for future extension.
Returns:
    PPP_LOGINING Processing login
    PPP_LOGOUTING Wireless module is logging out
    RET_OK Link established successfully
    ERR_DEV_NOT_EXIST No wireless module
    ERR_DEV_NOT_OPEN OsWlLock was not called successfully
    ERR_INVALID_PARAM Invalid parameter
    ERR_WL_POWER_ONING Wireless module powering up
    ERR_WL_POWER_OFF Wireless module powered off
    ERR_WL_NOT_INIT Initialization failed
    ERR_NET_PASSWD Password error
    ERR_NET_SERVER_BUSY Server busy, communication failed
    ERR_NET_AUTH Cannot connect to the RADIUS server
*/
int OsWlLogin(const char *APN, const char *Name, const char *Password, long Auth, int TimeOutMs, int KeepAlive, int ReserParam);

/*
Function: Log out from wireless network and disconnect link.
Parameters: None.
Returns:
    PPP_LOGOUTING Link disconnecting
    ERR_DEV_NOT_OPEN Module/device not opened
*/
int OsWlLogout(void);

// Select preferred SIM slot.  0 for SIM1, 1 for SIM2. Default is SIM1 (0).

/*
Function: Select preferred SIM slot.
Parameters: slot[in] 0 for SIM1, 1 for SIM2. Default is SIM1 (0).
Returns:
    RET_OK Successfully selected SIM slot
    Others: Failed
*/
int OsWlSelSimSlot(int slot);

/*
Function: Get currently used SIM slot.
Parameters: None.
Returns:
    0 for SIM1, 1 for SIM2 
    Others: Failed
*/
int OsWlGetSimSlot();

/*
Function: Get SIM card status.
Parameters: 
    simStatus [out] Points to an integer to store the status: 0 for No SIM, 1 for SIM ready.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetSimStatus(int* simStatus);

/*
Function: Get SIM operator name/info.
Parameters: 
    simOperator [out] Buffer to store operator name.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetSimOperator(char* simOperator);

/*
Function: Get IMEI.
Parameters: 
    Imei [out] Buffer to store IMEI string. Default is SIM1 (0).
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetImei(char* Imei);

/*
Function: Get IMEI of specified slot.
Parameters: 
    slot [in] Slot index.
    Imei [out] Buffer to store IMEI string.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetImeiBySlot(int slot,char* Imei);

/*
Function: Get IMSI.
Parameters: 
    Imsi [out] Buffer to store IMSI string. Default is SIM1 (0).
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetImsi(char* Imsi);

/*
Function: Get ICCID.
Parameters: 
    Iccid [out] Buffer to store ICCID string.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetIccid(char* Iccid);

/*
Function: Get signal strength.
Parameters: None.
Returns:
    >0 Signal strength.
    Others: Failed
*/
int OsWlGetSignalStrength(void);

/*
Function: Get wireless module registration status (state, LAC, CID).
Parameters: 
    state [out] Registration state.
    lac [out] Location Area Code buffer.
    cid [out] Cell ID buffer.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetModuleREG(int *state, char* lac, char* cid);

/*
Function: Get network IP address.
Parameters: 
    ipaddr [out] Buffer to store IP address.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetNetworkIp(char* ipaddr);


/*
Function: Get current network type and operator information.
Parameters: 
    networkType [out] Buffer to store network type.
    oper [out] Buffer to store operator name.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetQNetworkInfo(char* networkType, char *oper);
/*
Function: Get the cellular base station information for the current connection.
Parameters:
    mcc [out] Receives the Mobile Country Code.
    mnc [out] Receives the Mobile Network Code.
    lac [out] Receives the Location Area Code.
    cellid [out] Receives the Cell ID.
    idbm [out] Receives the signal strength in dBm.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM An output parameter is NULL
    -1 Failed to retrieve base station information
*/
int OsWlGetDevBaseStation( int *mcc,int *mnc,int *lac,int *cellid,int *idbm);
/*
Function: Get wireless auto connect status.
Parameters: 
    status [out] Points to an integer to store status: 0 for disabled, 1 for enabled.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlGetAutoConnectStatus(int *status);

/*
Function: Set wireless auto connect status.
Parameters: 
    onoff [in] 0 to disable, 1 to enable.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWlSetAutoConnectStatus(int onoff);

/*
Function: Set airplane mode.
Parameters: 
    onoff [in] 0 to disable, 1 to enable.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsSetAirplaneMode(int onoff);

/*
Function: Get airplane mode status.
Parameters: 
    onoff [out] Points to an integer to store status: 0 for disabled, 1 for enabled.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsGetAirplaneMode(int *onoff);

/*
Function: Set APN parameters.
Parameters: 
    apn [in] Access Point Name.
    user [in] APN username.
    password [in] APN password.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsSetApnParams(char* apn, char* user,char* password);

/*
Function: Get APN parameters.
Parameters: 
    apn [out] Buffer to store Access Point Name.
    user [out] Buffer to store APN username.
    password [out] Buffer to store APN password.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsGetApnParams(char* apn, char* user,char* password);

/*
Function: Get current network type.
Parameters: 
    type [out] Points to an integer to store network type: 0 for NOSERVICE, 1 for GSM 2G, 2 for LTE 4G.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWLGetNetworkType(int* type);

/*
Function: Power down the wireless module.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWLModulePowerDown(void);

/*
Function: Power up the wireless module.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWLModulePowerOn(void);
/*
Function: Enable or disable the wireless network connection.
Parameters:
    enable Set to 1 to enable or 0 to disable the connection.
Returns:
    RET_OK Success
    Others: Failed to change the connection state
*/
int OsWlSetEnable(int enable);

/*
Function: Get the wireless network connection state.
Parameters:
    enable [out] Receives 1 if enabled or 0 if disabled.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM enable is NULL
    Others: Failed to retrieve the connection state
*/
int OsWlGetEnable(int *enable);
/************************************22 WiFi*********************************/
// Return codes list
#define ERR_MODE_NOT_SUPPORT -3350 // Mode configuration error
#define ERR_WIFI_POWER_OFF -3351 // Module powered off
#define ERR_NOT_FOUND_AP -3352 // AP not found
#define ERR_AUTH_SEC_MODE -3353 // Invalid authentication or encryption mode
#define ERR_WIFI_BAD_SIGNAL -3354 // WiFi signal too weak
#define ERR_NET_PASSWD -3355 // Password error
#define RET_CONNECTING 1 // Connecting in progress
#define ERR_EAP_ID -3359 // Certificate chain error or verification failure

// WiFi connection error codes
#define ERR_WLAN_ADD_NETWORK -3360
#define ERR_WLAN_SET_SSID -3361
#define ERR_WLAN_SET_PASSWORK -3362
#define ERR_WLAN_ENABLE_NETWORK -3363
#define ERR_WLAN_SAVE_CONFIG -3364
#define ERR_WLAN_SELECT_NETWORK -3365
#define ERR_WLAN_CONNECT_TO_AP -3366	// Physical connection failed, other issues except wrong password
#define ERR_WLAN_AP_DISCONNECT -3367	// Wifi disconnected
#define ERR_WLAN_UDHCPC_GET_IPADDR -3368// Failed to acquire IP address (weak signal or router configured with static IP)

// Encryption types list
#define PARE_CIPHERS_NONE 0x00000000 // No encryption
#define PARE_CIPHERS_WEP64 0x00000001 // WEP 40-bit key
#define PARE_CIPHERS_WEP128 0x00000002 // WEP 104-bit key
#define PARE_CIPHERS_WEPX 0x00000004 // WEP encryption, unknown key length
#define PARE_CIPHERS_CCMP 0x00000010 // AES encryption mode
#define PARE_CIPHERS_TKIP 0x00000020 // TKIP encryption mode

// Authentication mode:
enum WIFI_AUTH_MODE{
    AUTH_NONE_OPEN=1,
    AUTH_NONE_WEP,
    AUTH_NONE_WEP_SHARED, /* Under scan this mode shows as AUTH_NONE_WEP */
    AUTH_IEEE8021X,
    AUTH_WPA_PSK,
    AUTH_WPA_EAP,
    AUTH_WPA_WPA2_PSK,
    AUTH_WPA_WPA2_EAP,
    AUTH_WPA2_PSK,
    AUTH_WPA2_EAP
};

// WEP64, WEP128 extension settings
typedef struct _WepSecKey{
    char Key[4][40]; /* WEP key data */
    int KeyLen; /* WEP key length */
    int Idx; /* WEP key index range [0, 3] */
} WEP_SEC_KEY;

// WPA/WPA2-PSK extension settings
typedef struct _WpaPskKey{
    char Key[64]; /* PSK key data */
    int KeyLen; /* PSK key length */
} WPA_PSK_KEY;

// EAP extension settings
typedef struct _WpaEapKey{
    int EapType; /* EAP type */
    char Pwd[132]; /* Password */
    char Id[132]; /* Identity */
    char CaCert[132]; /* File path to CA certificate */
    char CliCert[132];/* File path to client certificate */
    char PriKey[132]; /* File path to client private key */
    char PriKeyPwd[132]; /* Private key password */
} WPA_EAP_KEY;

// Scanned AP information
typedef struct _WifiApInfo
{
    char Essid[33];/* AP name (SSID) */
    char Bssid[20];/* MAC address */
    int Channel; /* Channel */
    int Mode; /* Connection mode, 0: Station; 1: IBSS */
    int Rssi; /* Signal strength (RSSI), range [-99, 0] */
    int AuthMode; /* Authentication mode */
    int SecMode; /* Encryption mode: NONE, WEP, TKIP, CCMP */
}ST_WifiApInfo;

// AP connection settings:
typedef struct _WifiApSet{
    char Essid[33]; /* AP name (SSID), max 32 bytes, null-terminated */
    char Bssid[20];/* MAC address, null-terminated; if SSID is unique, Bssid can be null */
    int Channel; /* Channel, active only in IBSS mode, 0: default */
    int Mode; /* Connection mode, 0: Station; 1: IBSS */
    int AuthMode; /* Authentication mode */
    int SecMode; /* Encryption mode: NONE, WEP, TKIP, CCMP */
    union KEY_UNION{ /* Key settings */
        WEP_SEC_KEY WepKey; /* WEP mode */
        WPA_PSK_KEY PskKey; /* WPA/WPA2-PSK mode */
        WPA_EAP_KEY EapKey; /* WPA/WPA2-EAP mode */
    } KeyUnion;
}ST_WifiApSet;

// WPS Mode
typedef enum 
{
    WPS_MODE_PBC = 1,    /* Push-button configuration, also known as PBC */
    WPS_MODE_PIN_CLIENT, /* Connect using terminal pin-code (client) */
    WPS_MODE_PIN_AP      /* Connect using AP-side pin-code (AP) */
}WPS_MODE;

/*
Function: Connect WiFi service and obtain access permissions.
Parameters: None.
Returns:
    RET_OK Success
    ERR_DEV_NOT_EXIST WiFi module driver loading issue or device error
    ERR_DEV_BUSY WiFi module is already occupied
    ERR_BATTERY_VOLTAGE_TOO_LOW Battery voltage is too low
    ERR_BATTERY_ABSENT Battery does not exist
*/
int OsWifiOpen(void);

/*
Function: Release WiFi module access. Calling this function does not interrupt active WiFi communications.
Parameters: None.
Returns: None.
*/
void OsWifiClose(void);


/*
Function: Scan available existing wireless networks.
Parameters: Aps [out] A structured pointer to list of scanned networks info.
Returns:
    >=0 Number of found APs.
    ERR_MEM_FAULT Memory failure
    ERR_INVALID_PARAM Parameter error
    ERR_WIFI_POWER_OFF WiFi module powered off
    ERR_DEV_NOT_OPEN Device/module not opened
Usage: Aps resources must be freed after usage.
*/
int OsWifiScan (ST_WifiApInfo **Aps);

/*
Function: Connect to designated wireless network.
Parameters:
    Ap [in] Scanned network parameters and secure keys.
    TimeOutMs [in] Connection timeout in milliseconds, range [0, 3600000].
Returns:
    RET_OK Successfully connected
    RET_CONNECTING Connecting in progress
    ERR_NOT_CONNECT Connection failed
    ERR_WIFI_BAD_SIGNAL WiFi signal too weak
    ERR_NOT_FOUND_AP AP cannot be found
    ERR_NET_PASSWD Password error
    ERR_AUTH_SEC_MODE Incorrect authentication or encryption mode
    ERR_WIFI_POWER_OFF WiFi module powered off
    ERR_DEV_NOT_OPEN Device/module not opened
    ERR_INVALID_PARAM Parameter error
*/
int OsWifiConnect(const ST_WifiApSet *Ap,int TimeOutMs);

/*
Function: Disconnect from current wireless network.
Parameters: None.
Returns:
    RET_OK Successfully disconnected
    ERR_DEV_NOT_OPEN Device/module not opened
*/
int OsWifiDisconnect(void);

/*
Function: Get current terminal network state parameters.
Parameters:
    Essid [out] Current connected SSID, must not be NULL, length is 33 bytes.
    Bssid [out] Current connected BSSID, can be NULL, length is 20 bytes.
    Rssi [out] Signal strength level ranging [-99, 0], must not be NULL.
Returns:
    RET_OK Connection successful
    RET_CONNECTING Connecting in progress
    ERR_NOT_CONNECT Not connected to network
    ERR_WIFI_BAD_SIGNAL WiFi signal too weak
    ERR_NOT_FOUND_AP AP cannot be found
    ERR_NET_PASSWD Password error
    ERR_AUTH_SEC_MODE Incorrect authentication or encryption mode
    ERR_INVALID_PARAM Parameter error
*/
int OsWifiCheck(char *Essid,char *Bssid,int *Rssi);

/*
Function: Send raw config command or script directly to the WPA Supplicant backend daemon and fetch the result.
Parameters:
    Argv [in] Custom supported command arrays, must not be NULL.
    Argc [in] Length of Argv array.
    Result [out] Received raw response string buffer, must not be NULL, length >= 2048 bytes.
    Len [in] Max length of Result buffer.
Returns:
    RET_OK Successfully sent
    ERR_INVALID_PARAM Parameter error
    ERR_WIFI_POWER_OFF WiFi module powered off
    ERR_DEV_NOT_OPEN Device/module not opened
*/
int OsWifiCmd (const char *Argv[],int Argc,char *Result,int Len);

/*
Function: Check current WiFi communication status, SSIDs and IPs.
Parameters: 
    Essid [out] Current connected SSID, length 33 bytes.
    Bssid [out] Current connected BSSID, length 20 bytes.
    Ip [out] Buffer to store IP address string.
    Rssi [out] Selected WiFi AP signal level (RSSI).
Returns:
    RET_OK Success
    Others: Failed
*/
int OsGetWifiStatus(char *Essid,char *Bssid,char* Ip,int *Rssi);

/*
Function: Get WiFi signal strength, range from 0 (disconnected) up to 4 bars.
Parameters: None.
Returns:
    0-4 Signal strength level.
    Others: Failed
*/
int OsGetWifiSignalStrength();

/*
Function: Get Wi-Fi auto-connect status.
Parameters: 
    status [out] Points to an integer to store status: 0 for disabled, 1 for enabled.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWifiGetAutoConnectStatus(int *status);

/*
Function: Set Wi-Fi auto-connect status.
Parameters: 
    onoff [in] 0 to disable, 1 to enable.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWifiSetAutoConnectStatus(int onoff);

/*
Function: Reconnect to Wi-Fi.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWifiReconnect();

/*
Function: Wake up Wi-Fi module.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWifiModuleWakeUp();

/*
Function: Put Wi-Fi module into sleep mode.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsWifiModuleSleep();

/*
Function: Enable or disable Wi-Fi.
Parameters:
    onoff Set to 1 to enable Wi-Fi or 0 to disable it.
Returns:
    RET_OK Success
    Others: Failed to change the Wi-Fi state
*/
int OsWifiSetEnable(int onoff);

/*
Function: Get the Wi-Fi state.
Parameters:
    onoff [out] Receives 1 if Wi-Fi is enabled or 0 if disabled.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM onoff is NULL
    Others: Failed to retrieve the Wi-Fi state
*/
int OsWifiGetEnable(int *onoff);

/************************************29 Power Management*********************************/
// 29 Power Management
#define BATTERY_LEVEL_0 0
#define BATTERY_LEVEL_1 1
#define BATTERY_LEVEL_2 2
#define BATTERY_LEVEL_3 3
#define BATTERY_LEVEL_4 4
#define BATTERY_LEVEL_CHARGE 5
#define BATTERY_LEVEL_COMPLETE 6
#define BATTERY_LEVEL_ABSENT 7

// PM_MSG_T broadcasted power management events
typedef enum{
    PM_MSG_NO_EVEN, /* No information */
    PM_MSG_ENTER_SLEEP, /* Terminal enters sleep mode */
    PM_MSG_EXIT_SLEEP,  /* Terminal exits sleep mode */
    PM_MSG_ENTER_SCREENSAVER,   /* Terminal enters screen saver */
    PM_MSG_EXIT_SCREENSAVER,    /* Terminal exits screen saver */
    PM_MSG_ENTER_POWEROFF,  /* Terminal begins power off */
    PM_MSG_OWER_ABORMAL,    /* Power supply abnormal */
    PM_MSG_VBATTERY_DAMAGE, /* Battery damaged */
}PM_MSG_T;

// PM_REQ_T custom power requests
typedef enum{
    PM_FORBID_SLEEP,    /* Forbid terminal from entering sleep */
    PM_ALLOW_SLEEP,    /* Allow terminal to enter sleep */
    PM_FORBID_SCREENASVER,    /* Forbid screen saver */
    PM_ALLOW_SCREENASVER,    /* Allow screen saver */
    PM_FORBID_POWEROFF,    /* Forbid terminal from powering off */
    PM_ALLOW_POWEROFF,    /* Allow terminal to power off */
}PM_REQ_T;

// POWER_TYPE custom power types
typedef enum{
    POWER_ADAPTER = 1,    /* Adapter charger */
    POWER_USB,    /* USB powered */
    POWER_BATTERY,    /* Battery powered */
    POWER_WPC,    /* Wireless charging base */
}POWER_TYPE;

typedef enum { 
    WAKEUP_SRC_NONE = 0, /* No sleep-wake event yet */ 
    WAKEUP_SRC_AP,/* AP wake up */ 
    WAKEUP_SRC_RTC,/* RTC wake up */ 
    WAKEUP_SRC_KB,/* Keypad wake up */ 
} WAKEUP_SOURCE; 

/*
Function: Check battery remaining level.
Parameters: None.
Returns:
    BATTERY_LEVEL_0 ~ BATTERY_LEVEL_ABSENT.
    Others: Failed
*/
int OsCheckBattery(void);

/*
Function: Check terminal power supply type.
Parameters: None.
Returns:
    POWER_TYPE value
    Others: Failed
*/
int OsCheckPowerSupply(void);

/*
Function: Reboot the terminal.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsReboot(void);

/*
Function: Power off the terminal.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed
*/
int OsPowerOff(void);

/*
Function: Get battery capacity percentage.
Parameters: None.
Returns:
    Battery capacity percentage (0-100)
    Others: Failed
*/
int OsGetBatCap(void);

/*
Function: Get battery voltage.
Parameters: None.
Returns:
    Battery voltage in uV (microvolts)
    Others: Failed
*/
int OsGetBatVol(void);

/*
Function: Get charging state.
Parameters: None.
Returns:
    0: BATTERY_NOT_CHARGING = 0,
    1: BATTERY_CHARGING,
    2: BATTERY_FULL,
    3: BATTERY_USB_NOT_INSERT,
    0xFF: BATTERY_STATE_ERR
*/
int OsGetBatChgState(void);

/************************************Sleep Control*********************************/

/*
Function: Enable or disable the device sleep function (master switch).
Parameters:
    status [in] 1 to allow sleep, 0 to prevent sleep.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Invalid parameter
*/
int OsSetSysSleepStatus(int status);

/*
Function: Get whether the system sleep function is enabled.
Parameters:
    status [out] Receives 1 if sleep is allowed, or 0 if sleep is disabled.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM status is NULL
*/
int OsGetSysSleepStatus(int *status);

/*
Function: Set the inactivity timeout for level-1 (light) sleep.
Parameters:
    Time [in] Timeout in seconds. Set to 0 to disable automatic screen-off;
        otherwise, the supported range is 10 to 3600 seconds.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Time is outside the supported range
*/
int OsSysSleepTime(int Time);

/*
Function: Get the level-1 (light) sleep timeout.
Parameters:
    Time [out] Receives the timeout in seconds.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Time is NULL
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsGetSysSleepTime(int *Time);

/*
Function: Set the inactivity timeout for level-2 (deep) sleep.
Parameters:
    Time [in] Timeout in seconds, from 10 to 300. Set to -1 to disable
        automatic deep sleep.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Time is outside the supported range
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsSetSysDeepSleepTime(int Time);

/*
Function: Get the level-2 (deep) sleep timeout.
Parameters:
    Time [out] Receives the timeout in seconds; -1 indicates automatic deep
        sleep is disabled.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Time is NULL
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsGetSysDeepSleepTime(int *Time);

/*
Function: Configure the system RTC wake-up schedule.
Parameters:
    type [in] Wake-up action: 0 to reboot the device, 1 to wake the device.
    wakeTime [in] Wake-up time string in "yyyy.mm.dd.hh" format.
    cycleTime [in] Recurrence interval in days; 0 means one-time only,
        and a positive value means repeat every n days.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM wakeTime is NULL or a parameter is invalid
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsSysSetWakeUpTime(int type, char *wakeTime, int cycleTime);

/*
Function: Get the configured system RTC wake-up time.
Parameters:
    wakeTime [out] Buffer receiving the wake-up time string.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM wakeTime is NULL
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsSysGetWakeUpTime(char *wakeTime);

/*
Function: Request the system to enter sleep.
Parameters:
    delayTime [in] Delay before entering sleep, in seconds; must be
        non-negative.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM delayTime is negative
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsSysGoToSleep(int delayTime);

/*
Function: Exit system sleep.
Parameters: None.
Returns:
    RET_OK Success
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsSysExitSleep(void);

/*
Function: Set the inactivity timeout before automatic shutdown during deep
    sleep.
Parameters:
    Time [in] Timeout in half-hour units. Set to -1 to disable automatic
        shutdown; otherwise, the value must be greater than 0 and no greater
        than 48.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Time is outside the supported range
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsSetSleepShutdownTime(int Time);

/*
Function: Get the inactivity timeout before automatic shutdown during deep
    sleep.
Parameters:
    Time [out] Receives the timeout in half-hour units; -1 indicates automatic
        shutdown is disabled.
Returns:
    RET_OK Success
    ERR_INVALID_PARAM Time is NULL
    ERR_SYS_NOT_SUPPORT The system service does not support the operation
*/
int OsGetSleepShutdownTime(int *Time);

/*
Function: Get the source that last woke the system from deep sleep.
Parameters: None.
Returns:
    WAKEUP_SRC_NONE No sleep-wake event has occurred
    WAKEUP_SRC_AP The application processor woke the system
    WAKEUP_SRC_RTC The RTC timer woke the system
    WAKEUP_SRC_KB A key press woke the system
*/
int OsWakeupSource(void);

typedef  void (*pShutdownCallback)(int runstate, void *par);
/*
Function: Register a callback to be invoked during system shutdown.
Parameters:
    callback Function to call during shutdown. Must not be NULL.
Returns: None.
*/
void OsRegisterShutdownCallback(pShutdownCallback callback);

/************************************Custom Database Interface*********************************/
typedef void*		HANDLE; // Generic pointer definition
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
 * @Method Name: DB_bInit
 * @Purpose: Database initialization.
 * @Input Parameters:
 *			szAppID	Current APP ID.
 *
 * @Output Parameters: None.
 * @Returns:
 *			Success: 0
 *			Failure: Other values
 * @Notes:
 *		Must initialize database before calling other database APIs.
 * @Example:
 */
int DB_bInit(char *szAppID);
/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_vUnInit
 * @Purpose: Database uninitialization.
 * @Notes:
 *		Call when exiting current application.
 * @Example:
 */
void DB_vUnInit(void);
/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_bInitBySysService
 * @Purpose: Database initialization.
 * @Input Parameters:
 * 			szDbPath	Database folder path
 *			szDbName	Database name
 *
 * @Output Parameters: None.
 * @Returns:
 *			Success: 0
 *			Failure: Other values
 * @Notes:
 *		Must initialize database before calling other database APIs.
 * @Example:
 */
int DB_bInitBySysService(char* szDbPath,char *szDbName);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_hOpen
 * @Purpose: Open specified table.
 * @Input Parameters:
 *			pszName				Table name
 *			nMaxLenOfRec		Maximum length of a single record		
 * @Output Parameters: None.
 * @Returns:
 *			Success: Table handle;
 *			Failure: NULL;
 * @Notes:
 *
 * @Example:
 */

HANDLE DB_hOpen(const char* pszName, int nMaxLenOfRec);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_vClose
 * @Purpose: Close specified table.
 * @Input Parameters:
 *				hTable	Corresponding table handle.
 *
 * @Output Parameters: None.
 * @Returns: None.
 *
 * @Notes:
 *
 * @Example:
 */
void  DB_vClose(HANDLE hTable);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_bClear
 * @Purpose: Clear all table records (does not delete the table).
 * @Input Parameters:
 *			hTable	Corresponding table handle.
 * @Output Parameters: None.
 * @Returns:
 * 			Success: 0
 * 			Failure: Other values
 * @Notes:
 *
 * @Example:
 */
int DB_bClear(HANDLE hTable);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_bRemove
 * @Purpose: Delete specified table.
 * @Input Parameters:
 *				pszName		Table name.
 *				nAccess     (Reserved, unused) Table access rights. (EM_ACCESS_PRIVATE: Private, EM_ACCESS_SHARED: Shared).
 * @Output Parameters: None.
 * @Returns:
 * 			Success: 0
 * 			Failure: Other values
 * @Notes:
 *
 * @Example:
 */
int DB_bRemove(const char* pszName, int nAccess);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nReadRec
 * @Purpose: Read record by index.
 * @Input Parameters:
 *			hTable			Table handle
 *			nIndexOfRec		Record index (0-based)
 *			nLenOfBuffer	Length of buffer pointed to by pRecordBuf
 * @Output Parameters:
 *			pRecordBuf		Decoded record data buffer
 * @Returns:
 * 			Success: Returns actual record length
 * 			Failure: RET_INTERFACE_FAILD
 * @Notes:
 *
 * @Example:
 */
int DB_nReadRec(HANDLE hTable, int nIndexOfRec, int nLenOfBuffer, char *pRecordBuf);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nReadRecByKey
 * @Purpose: Read record by unique key (Recommended for fast query).
 * @Input Parameters:
 *			hTable			Table handle
 *			pKey			Query keyword, must be unique
 *			nLenOfBuffer	Length of buffer pointed to by pRecordBuf
 * @Output Parameters:
 *			pRecordBuf		Decoded record data buffer
 * @Returns:
 * 			Success: Returns actual record length
 * 			Failure: RET_INTERFACE_FAILD
 * @Notes:
 *
 * @Example:
 */
int DB_nReadRecByKey(HANDLE hTable, char* pKey, int nLenOfBuffer, char *pRecordBuf);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nAppendRecByKey
 * @Purpose: Append record with unique key (Recommended).
 * @Input Parameters:
 *			hTable			Table handle
 *			pKey			Query keyword, must be unique
 *			pRecordBuf		Pointer to record content to append
 *			nRecLen			Length of record to append
 * @Output Parameters:	None.
 *
 * @Returns:
 * 			Success: Returns actual record length
 * 			Failure: RET_INTERFACE_FAILD
 * @Example:
 */
int DB_nAppendRecByKey(HANDLE hTable, char* pKey, char* pRecordBuf, int nRecLen);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nReplaceRec
 * @Purpose: Update record.
 * @Input Parameters:
 *			hTable			Table handle
 *			nIndexOfRec		Table record index (0-based)
 *			pRecord			Data to update
 *			nRecordLen		Record length
 * @Output Parameters:	None.
 *
 * @Returns:
 * 			Success: Returns actual record length
 * 			Failure: RET_INTERFACE_FAILD
 * @Notes:
 *
 * @Example:
 */
int DB_nReplaceRec(HANDLE hTable, int nIndexOfRec, char* pRecord, int nRecordLen);


/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nReplaceRecByKey
 * @Purpose: Update record.
 * @Input Parameters:
 *			hTable			Table handle
 *			pKey			Query keyword, must be unique
 *			pRecord			Data to update
 *			nRecordLen		Record length
 * @Output Parameters:	None.
 *
 * @Returns:
 * 			Success: Returns actual record length
 * 			Failure: RET_INTERFACE_FAILD
 * @Notes:
 *
 * @Example:
 */
int DB_nReplaceRecByKey(HANDLE hTable, char* pKey,int nRecordLen,char* pRecord);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_bDeleteRec
 * @Purpose: Delete record.
 * @Input Parameters:
 *			hTable			Table handle
 *			nIndex		    Table record index (0-based)
 * @Output Parameters:	None.
 *
 * @Returns:
 * 			Success: 0
 * 			Failure: Other values
 * @Notes:
 *
 * @Example:
 */
int DB_bDeleteRec(HANDLE hTable, int nIndex);

/*
 * @author: 	06-03-2015
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_bDeleteRecByKey
 * @Purpose: Delete record.
 * @Input Parameters:
 *			hTable		Table handle
 *			pKey		Query keyword, must be unique
 * @Output Parameters:	None.
 *
 * @Returns:
 * 			Success: 0
 * 			Failure: Other values
 * @Notes:
 *
 * @Example:
 */
int DB_bDeleteRecByKey(HANDLE hTable, char* pKey);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_bIsExsit
 * @Purpose: Check if table name exists.
 * @Input Parameters:
 *			pszTableName		Table name
 *			nAccess				Access mode (Reserved)	
 *
 * @Output Parameters:	None.
 *
 * @Returns:
 * 			Exists: 0
 * 			Not exists: Other values
 * @Notes:
 *
 * @Example:
 */
int DB_bIsExsit(const char* pszTableName, int nAccess);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nGetRecordSUM
 * @Purpose: Get number of active records.
 * @Input Parameters:
 *			hTable		Table handle
 *
 * @Output Parameters:	None.
 *
 * @Returns:
 *			Success: Total records count (>= 0);
 *			Failure: RET_INTERFACE_FAILD
 * @Notes:
 *
 * @Example:
 */
int DB_nGetRecordSUM(HANDLE hTable);

/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nGetLastError
 * @Purpose: Get the last error code.
 * @Input Parameters:
 *			 hTable 	Table handle
 *
 * @Output Parameters:	 None.
 * @Returns:
 			Error code
 *
 * @Notes:
 *
 * @Example:
 */
int DB_nGetLastError(HANDLE hTable);


/*
 * @author:
 * @Date: 	
 * @Record: create it;
 *
 * @Method Name: DB_nGetFirstRecord
 * @Purpose: Get the first record of the table.
 * @Input Parameters:
 *			hTable			Table handle
 *			nIndexOfRec		Index pointer (unused)
 *			nRecLen			Length of buffer pointed to by pRecordBuf
 *
 * @Output Parameters:	pRecordBuf		Decoded record data buffer
 *
 * @Returns:
 *			Success: Actual length read
 *			Failure: RET_INTERFACE_FAILD
 * @Notes:
 *
 * @Example:
 */
int DB_nGetFirstRecord(HANDLE hTable, int *nIndexOfRec, void* pRecordBuf, int nRecLen);


/**********************Camera********************/
/*
Function: Open the camera.
Parameters:
    type [in] Camera type or decoder selection. Values 3 and 4 select a
        decoder algorithm; other values use the configured default.
Returns:
    RET_OK Success
    Others: Failed to open the camera
*/
int OsOpenCamera(int type);

/*
Function: Close the camera.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed to close the camera
*/
int OsCloseCamera();

/*
Function: Read the camera scan result.
Parameters:
    pContent [out] Buffer receiving the scan result.
    inLen [in] Size of pContent in bytes; must be greater than 0.
Returns:
    0 Scan result read successfully
    ERR_INVALID_PARAM pContent is NULL or inLen is invalid
    -2 No scan result is available
    -3 Failed to read the scan result
    -4 Camera scanning service error
*/
int OsReadScanResult(char* pContent,int inLen);

/*
Function: Update camera configuration. Reopen the camera for changes to take
    effect.
Parameters:
    key [in] Configuration key.
    value [in] Configuration value.
        "rotate": Preview rotation angle, from 0 to 359 degrees.
        "fullscreen": Enable or disable full-screen preview ("true"/"false";
            "1"/"0" and "on"/"off" are also accepted).
        "roi": Preview region in "x,y,w,h" format; applies only when
            full-screen mode is disabled.
        "algo": Decoder algorithm selection.
Returns:
    RET_OK Success
    -1 key or value is NULL
    ERR_INVALID_PARAM The key/value is invalid
*/
int OsUpdateCameraConfig(const char* key, const char* value);

/*
Function: Open the camera using the advanced camera configuration.
Parameters:
    type [in] Camera type or decoder selection. Values 3 and 4 select a
        decoder algorithm; other values use the configured default.
Returns:
    RET_OK Success
    Others: Failed to open the camera
*/
int OsOpenCameraAdv(int type);
/**
 * @brief Update advanced camera configuration. Reopen the camera for changes
 *        to take effect.
 * @param key [in] Configuration key.
 * @param value [in] Configuration value.
 *        "rotate": Preview rotation angle, from 0 to 359 degrees.
 *        "fullscreen": Enable or disable full-screen preview ("true"/"false";
 *            "1"/"0" and "on"/"off" are also accepted).
 *        "roi": Preview region in "x,y,w,h" format; applies only when
 *            full-screen mode is disabled.
 *        "algo": Decoder algorithm selection.
 *        "crop": Camera image crop region in "x,y,w,h" format.
 * @return RET_OK on success, -1 if key or value is NULL, or an error code
 *         for an invalid key/value.
 */
int OsUpdateCameraAdvConfig(const char* key, const char* value);

/*
Function: Check whether the camera is open.
Parameters: None.
Returns:
    RET_OK The camera is open
    -1 The camera is not open
*/
int OsCameraIsOpen();

/**********************FlashLight********************/
/*
Function: Turn on the camera flash light.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed to turn on the flash light
*/
int OsOpenFlashLight();

/*
Function: Turn off the camera flash light.
Parameters: None.
Returns:
    RET_OK Success
    Others: Failed to turn off the flash light
*/
int OsCloseFlashLight();

/**********************LVGL********************/
/*
Function: Flush a rectangular pixel area to the display.
Parameters:
    wX [in] X coordinate of the area.
    wY [in] Y coordinate of the area.
    wWidth [in] Width of the area in pixels.
    wHeight [in] Height of the area in pixels.
    pucBuffer [in] Pixel buffer containing the area to display.
Returns:
    Positive value Number of bytes sent to the display service
    Negative value Failed to flush the display area
*/
int OsLvglDispFlush(int wX, int wY, int wWidth, int wHeight, unsigned char* pucBuffer);

/**********************GPS********************/
#define OS_GPS_MAX_SV_COUNT 48

typedef enum {
    OS_GPS_STATUS_NONE = 0,
    OS_GPS_STATUS_ENABLED = 1,
    OS_GPS_STATUS_DISABLED = 2,
    OS_GPS_STATUS_RUNNING = 3,
    OS_GPS_STATUS_STOPPED = 4,
} OS_GPS_STATUS;

typedef enum {
    OS_GNSS_TYPE_GPS = 1,
    OS_GNSS_TYPE_GLONASS = 2,
    OS_GNSS_TYPE_BEIDOU = 4,
    OS_GNSS_TYPE_GALILEO = 8,
} OS_GNSS_TYPE;

typedef struct {
    long long timestamp;     // Timestamp (milliseconds)
    unsigned short flags;    // Flags
    double latitude;         // Latitude (degrees)
    double longitude;        // Longitude (degrees)
    double altitude;         // Altitude (meters)
    float speed;             // Speed (meters/second)
    float bearing;           // Bearing (degrees)
    float accuracy;          // Accuracy (meters)
    int numSvFix;            // Number of fixed satellites used in positioning
    int numSvView;           // Number of visible satellites in view
} ST_GPS_LOCATION;

typedef struct {
    int prn;         // Satellite Pseudo-Random Noise (PRN) code
    float snr;       // Signal-to-Noise Ratio (dB-Hz)
    float elevation; // Elevation angle (degrees)
    float azimuth;  // Azimuth angle (degrees)
    int inUse;      // Whether used in positioning
    long long recvTimestampMs;     // System receive timestamp (milliseconds)
    long long gnssUtcTimestampMs;   // GNSS UTC timestamp (milliseconds)

} ST_GPS_SATELLITE_INFO;

/*
Function: Enable GPS service.
Parameters: None.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSEnable(void);

/*
Function: Disable GPS service.
Parameters: None.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSDisable(void);

/*
Function: Check whether GPS is enabled.
Parameters: None.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSIsEnabled(void);

/*
Function: Start GPS positioning.
Parameters: None.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSStart(void);

/*
Function: Stop GPS positioning.
Parameters: None.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSStop(void);
/*
Function: Check whether GPS is running.
Parameters: None.
Returns: 
    0 running
    Others not running
*/ 
int OsGPSIsRunning(void);

/*
Function: Get current GPS location fix.
Parameters: 
    location [out] Pointer to ST_GPS_LOCATION structure to store location data.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSGetLocation(ST_GPS_LOCATION *location);

/*
Function: Get last known GPS location fix.
Parameters: 
    location [out] Pointer to ST_GPS_LOCATION structure to store location data.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSGetLastKnownLocation(ST_GPS_LOCATION *location);
/*
Function: Get satellite visibility and usage status list.
Parameters: 
    svList [out] Pointer to ST_GPS_SATELLITE_INFO array to store satellite information.
    svCount [out] Pointer to an integer to store the number of satellites.
    maxCount [in] Maximum number of satellites to retrieve.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSGetSvStatus(ST_GPS_SATELLITE_INFO *svList, int *svCount, int maxCount);
/*
Function: Get current GNSS constellation type bitmap.
Parameters: None.
Returns: 
    1=GPS
    2=BDS
    3=GPS+BDS
    4=GLONASS
    5=GPS+GLONASS
    6=BDS+GLONASS
    7=GPS+BDS+GLONASS
    Others Failed
*/ 
int OsGPSGetGnssType(void);
// Get GPS module version string.
/*
Function: Get GPS module version string.
Parameters: 
    version [out] Pointer to a character array to store the version string.
    maxLen [in] Maximum length of the version string.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSGetVersion(char *version, int maxLen);
// Get GPS API version.
/*
Function: Get GPS API version.
Parameters: None.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSGetApiVersion(void);

/*
Function: Query GNSS information by info type.
Parameters: 
    infoType [in] Type of GNSS information to query.
        0 = Query firmware version number
        1 = Query hardware model and serial number
        2 = Query work mode of multi-mode receiver
        3 = Query customer number of the product
        5 = Query upgrade code information
    info [out] Pointer to a character array to store the information.
    maxLen [in] Maximum length of the information string.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSQueryGnssInfo(int infoType, char *info, int maxLen);
/*
Function: Send a custom GPS command and receive response.
Parameters: 
    command [in] Pointer to a character array containing the command.
    response [out] Pointer to a character array to store the response.
    maxResponseLen [in] Maximum length of the response string.
    timeoutMs [in] Timeout for the command in milliseconds.
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSSendCustomCommand(const char *command, char *response, int maxResponseLen, int timeoutMs);

/*
Function: Set GPS work mode.
Parameters: 
    mode [in] GPS work mode to set.
    1=GPS
    2=BDS
    3=GPS+BDS
    4=GLONASS
    5=GPS+GLONASS
    6=BDS+GLONASS
    7=GPS+BDS+GLONASS
Returns: 
    0 Success
    Others Failed
*/ 
int OsGPSSetWorkMode(int mode);

/* External display device with touch and display capabilities. */
typedef struct {
    int x;           /* X coordinate */
    int y;           /* Y coordinate */
    int PenDown;     /* Touch state: 1 = pressed, 0 = released */
    unsigned char RFU[8]; /* Reserved for future extensions */
} ST_TOUCH_DATA;

/*
Function: Get touch position information from the external display device.
Parameters:
    pinfo [out]: Pointer to an ST_TOUCH_DATA buffer. Must not be NULL.
Returns:
    RET_OK: Success.
    ERR_INVALID_PARAM: Invalid parameter.
    ERR_DEV_NOT_OPEN: Device is not open.
*/
int OsExtDispGetTouchInfo(ST_TOUCH_DATA* pinfo);

/*
Function: Refresh display content on the external display device.
Parameters:
    sx [in]: Starting X coordinate.
    sy [in]: Starting Y coordinate.
    width [in]: Width of the area to refresh.
    height [in]: Height of the area to refresh.
    buf [in]: Pointer to the display data buffer. Must not be NULL.
Returns:
    RET_OK: Success.
    ERR_INVALID_PARAM: Invalid parameter; the coordinates or dimensions are out of range, or the buffer size is invalid.
    ERR_DEV_NOT_OPEN: Device is not open.
*/
int OsExtDispFlush(int sx, int sy, int width, int height, void* buf);
/*
Function: Get the dimensions of the external display device.
Parameters:
    width [out]: Display width.
    height [out]: Display height.
Returns:
    RET_OK: Success.
    ERR_INVALID_PARAM: Invalid parameter.
    ERR_DEV_NOT_OPEN: Device is not open.
*/
int OsExtDispGetSize(int *width, int *height);

#ifdef __cplusplus
}
#endif


#ifndef __ISOMSG_H__
#define __ISOMSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MSG_LEN             3000
#define MAX_FIELD_NUM           128
#define MAX_FIELD_LEN           999


/*
 * Definition of ISO8583 Data Domain
 */
#define    MSG_ID                               0               /* message id                   */
#define    PAN                                  2               /* pan                   */
#define    PROC_CODE                            3               /* Processing Code                   */
#define    AMOUNT                               4               /* trans amount                 */
#define    SETTLE_AMOUNT                        5               /* Liquidation amount                 */
#define    FEE_AMOUNT                           6               /* Tip amount                 */
#define    LOCAL_TIME                           7               /* terminal time                 */
#define    BATCH_NO                             9               /* Batch number                   */
#define    TRACE_NO                             11              /* Trace number               */
#define    TRADE_TIME                           12              /* Transaction Time                 */
#define    TRADE_DATE                           13              /* Transaction Date                 */
#define    EXPIRY                               14              /* expiration date                   */
#define    PRETRADE_DATE                        15              /* Original transaction date               */
#define    LOCATION_FLAG                        16              /* Place signs                 */
#define    VERSION_NO                           21              /* version number                   */
#define    SERVE_MODE                           22              /* service mode                 */   

#define    TRACK2                               35              /* Two track information               */
#define    TRACK3                               36              /* Three track information               */
#define    RETR_NUM                             37              /* System reference number               */
#define    AUTH_ID                              38              /* Authorization number                   */
#define    RET_CODE                             39              /* Response code                 */
#define    MACHINE_ID                           40              /* Machine number                   */
#define    POS_ID                               41              /* pos id                   */
#define    CUSTOM_ID                            42              /* Merchant ID                   */ 
#define    ADDI_RESPONSE                        44              /* Additional response code             */
#define    ADDI_DATA                            48              /* Additional Data                 */    
#define    MONEYID                              49              /* Currency                 */
#define    SETTLE_MONEYID                       50              /* Clearing currency code             */
#define    PIN                                  52              /* pin block                  */
#define    ADDI_AMOUNT                          54              /* Additional Amount                 */
#define    ADVERTISEMENT                        57              /* Advertising Information/Winning Information     */
#define    ERROR_INFO                           58              /* error message                 */
#define    MERCHANT_INFO                        59              /* Merchant Information                 */
#define    SECRET_KEY                           62              /* key                     */
#define    MAC                                  64              /* MAC                      */


typedef enum ALIGN_TYPE
{
	ALN_LEFT   = 0,
	ALN_RIGHT  = 1,
} ALIGN_TYPE;

/*
 * Length type
 */
typedef enum LENGTH_TYPE
{
	LEN_FIX	   = 0,
	LEN_LLVAR  = 1,
	LEN_LLLVAR = 2,
} LENGTH_TYPE;


typedef struct ISO_FIELD ISO_FIELD;
typedef struct ISO_FIELD_DEF ISO_FIELD_DEF;

/*
 * Function function: Package the 8583 domain and return the packaged data and length
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
typedef int (*FIELD_PACKAGER_PACK) (ISO_FIELD_DEF *pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);

/*
 * Function function: Unpack the data in the buffer into the 8583 domain
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
typedef int (*FIELD_PACKAGER_UNPACK) (ISO_FIELD_DEF *pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);


/*
 * ISO 8583 Data Domain Definition
 */
struct ISO_FIELD_DEF
{
	short                   len;        /* Maximum length of data domain */
	LENGTH_TYPE				lentype;    /* Data field length type: 1--LLVAR  2--LLLVAR  */
    ALIGN_TYPE              align;      /* Alignment type */
    char                    padchar;    /* Fill in characters */
	FIELD_PACKAGER_PACK		pack;		/* Package function pointer */
	FIELD_PACKAGER_UNPACK	unpack;     /* Unpacking function pointer */
};

/*
 * ISO 8583 Data Domain Structure
 */
struct ISO_FIELD
{
    short   len;
    short   off;  /* Offset pointing to iso ->databuf buffer, excluding a few bytes of length type in the data */
};

/*
 * ISO 8583 Message Structure Definition
 */
typedef struct ISODataStru
{
	ISO_FIELD	fld[129];
	char		bitmap[129];
	unsigned char databuf[MAX_MSG_LEN];
	short       used;
} ISODataStru;



// ////////////////////////////////////////
// Inner help functions
int AddPadChar(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata, unsigned char head);

// ////////////////////////////////////////
// The following is the variable length calculation function
// ASC code form, such as length 104, will be represented as 0x31, 0x30, 0x34, a total of 3 bytes. If it is 9, it will be represented as 0x30, 0x39.
void FLALengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void FLALengthGet(ISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

// BCD code form, such as length 104, will be represented as 0x01, 0x04, 2 bytes in total
void FLBLengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void FLBLengthGet(ISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

// Hexadecimal form, such as length 104, will be represented as 0x00, 0x68, 2 bytes in total
void FLHLengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount);
void FLHLengthGet(ISO_FIELD_DEF *pthis, unsigned char *pindata, int *plen, int *pcount);

// Variable length domain length calculation
typedef enum __FLLENGTH_MODE_ENUM {
    FLLENGTH_MODE_ASC = 0 ,                     /* ASC */
    FLLENGTH_MODE_BCD,                          /* BCD */
    FLLENGTH_MODE_HEX,                          /* HEX */
}FLLENGTH_MODE_ENUM;				/* ----------  end of enum FLLENGTH_MODE_ENUM  ---------- */

// ////////////////////////////////////////
// Packaging and unpacking function implementation
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

// ////////////////////////////////////////
// Iso8583 Operation function
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
/*
 * external iso8583def
 */
extern const ISO_FIELD_DEF Iso8583Def_pos_tong[]; 
extern const ISO_FIELD_DEF Iso8583Def_union_pay[]; 
extern const ISO_FIELD_DEF Iso8583Def_bus_manage[]; 
extern const ISO_FIELD_DEF Iso8583Def_oda[]; 

#ifdef __cplusplus
}
#endif

#endif // __ISOMSG_H__

#pragma once

//app data type define
#define PR_BOOL		long
#define PR_INT64    long long
#define PR_INT32	int
#define PR_INT16 	short
#define PR_INT8		char

#define PR_UINT64	unsigned  long long
#define PR_UINT32	unsigned  int
#define PR_UINT16	unsigned  short
#define PR_UINT8	unsigned  char

#define PR_TRUE		(1==1)
#define PR_FALSE	(1==0)

#define PR_Bool		PR_BOOL
#define PR_Int64	PR_INT64
#define PR_Int32	PR_INT32
#define	PR_Int16	PR_INT16
#define PR_Int8		PR_INT8

#define PR_Uint64	PR_UINT64
#define	PR_Uint32	PR_UINT32
#define PR_Uint16	PR_UINT16
#define PR_Uint8	PR_UINT8

#define PR_True		PR_TRUE
#define PR_False	PR_FALSE

#define PR_bool		PR_BOOL
#define PR_int64	PR_INT64
#define PR_int32	PR_INT32
#define	PR_int16	PR_INT16
#define PR_int8		PR_INT8

#define PR_uint64	PR_UINT64
#define	PR_uint32	PR_UINT32
#define PR_uint16	PR_UINT16
#define PR_uint8	PR_UINT8

#define PR_true		PR_TRUE
#define PR_false	PR_FALSE

#define PR_NULL  NULL
#define PR_NORMAL   0 // success
#define PR_FAILD   -1
#define PR_TIMEOUT -2
#define PR_CANCLE  -3
#define PR_BYPASS  -4
#define PR_NOT_REC -5  //Not Rec
#define PR_ICC_ERR -6
#define Atof_ atof
#define Atoi_ atoi
#define Atol_ atol
#define Strcat_  strcat
#define Strncat_ strncat
#define Strchr_ strchr
#define Strcmp_ strcmp
#define Strcpy_ strcpy
#define Strncpy_ strncpy
#define Strcspn_ strcspn
#define Strerror_ strerror
#define Strpbrk_ strpbrk
#define Strrchr_ strrchr
#define Strspn_ strspn
#define Strstr_ strstr
#define Strtok_ strtok
#define Strxfrm_ strxfrm

#define lark_alloc_mem malloc
#define lark_free_mem free
#define mem_calloc malloc
#define mem_free free



#define PIN_KEY_INDEX 9
#define DES_PIN_KEY_INDEX 8
#define DES_CALC_KEY_INDEX 7
#define PARAM_TB_MAXRECORD_LEN  1024+512
#define PARAM_TB_NAME       "DB_TERM_INFO"
#define PARAM_TERM            "DB_TB_TERM"
#define PARAM_TRANS            "DB_TB_TRANS"



typedef struct
{    
    char TID[12];
    char MID[16];
    char MerName[24];
    char MerAddr[41];
    char phone[16];
    char cardAcceptorId[16];
    char token[1024];
    int expiresIn;
    char expDate[10+1];
    char recv[372];//sum= 1024+512
}TermInfo;

typedef struct
{
    char trace_no[6+1];
    char recv[249];//256
}TransInfo;
// typedef struct
// {    
//     char transType;
//     char printState;//打印情况
//     char transState;
//     char retryCnt;

//     char entryMode[4];
//     char TID[12];
//     char MID[16];
//     char MerName[24];
//     char MerAddr[41];
//     char phone[16];    
//     char pan[19+1];
//     char CardSN[3+1];
//     char ExpDate[4+1];
//     char TransAmount[12+1];
//     char CurrencyCode[3+1];
// 	char PinLen;
//     char TransTime[12+1];
//     char TransStan[6+1];//流水号
//     char ksn[12+1];    
//     char tag_ac[12+1];//todo
//     char tag_ran[4+1];
//     char tag_CVM[3+1];
//     char tag_TVR[5+1];
//     char tag_AIP[2+1];
//     char tag_ATC[2+1];    
//     char cardSN[2+1];
//     char resCode[4];
//     char resMsg[128];
    
//     char recv[658];//1024
// }TransInfo;

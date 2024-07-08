/*
********************************************************************************
*
*   File Name:
*       kernel_api.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _KERNEL_API_H
#define _KERNEL_API_H
#include <stdbool.h>
/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _KERNEL_API_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#ifndef NULL
#define NULL        ((void *)0)
#endif

/* Big-Endian Pointer to Long */
#define BE_PtrToLongLong(ptr)           \
	( \
		(((unsigned long long)(*((u8 *)(ptr) + 0))) << 56) | \
		(((unsigned long long)(*((u8 *)(ptr) + 1))) << 48) | \
		(((unsigned long long)(*((u8 *)(ptr) + 2))) << 40) | \
		(((unsigned long long)(*((u8 *)(ptr) + 3))) << 32)| \
		(((unsigned long long)(*((u8 *)(ptr) + 4))) << 24) | \
		(((unsigned long long)(*((u8 *)(ptr) + 5))) << 16) | \
		(((unsigned long long)(*((u8 *)(ptr) + 6))) << 8) | \
		(((unsigned long long)(*((u8 *)(ptr) + 7))) << 0) \
	)

/* Big-Endian Pointer to Long */
#define BE_PtrToLong(ptr)           \
	( \
		(((u32)(*((u8 *)(ptr) + 0))) << 24) | \
		(((u32)(*((u8 *)(ptr) + 1))) << 16) | \
		(((u32)(*((u8 *)(ptr) + 2))) << 8) | \
		(((u32)(*((u8 *)(ptr) + 3))) << 0) \
	)

/* Big-Endian Pointer to Short */
#define BE_PtrToShort(ptr)          \
	( \
		(((u16)(*((u8 *)(ptr) + 0))) << 8) | \
		(((u16)(*((u8 *)(ptr) + 1))) << 0) \
	)

/* Little-Endian Pointer to Long */
#define LE_PtrToLong(ptr)           \
	( \
		(((u32)(*((u8 *)(ptr) + 0))) << 0) | \
		(((u32)(*((u8 *)(ptr) + 1))) << 8) | \
		(((u32)(*((u8 *)(ptr) + 2))) << 16) | \
		(((u32)(*((u8 *)(ptr) + 3))) << 24) \
	)

/* Little-Endian Pointer to Short */
#define LE_PtrToShort(ptr)          \
	( \
		(((u16)(*((u8 *)(ptr) + 0))) << 0) | \
		(((u16)(*((u8 *)(ptr) + 1))) << 8) \
	)

/* Long Split */
#define LONG_HH(val)        ((((u32)(val)) >> 24) & 0xff)
#define LONG_HL(val)        ((((u32)(val)) >> 16) & 0xff)
#define LONG_LH(val)        ((((u32)(val)) >> 8) & 0xff)
#define LONG_LL(val)        (((u32)(val)) & 0xff)
#define LONG_H(val)         ((((u32)(val)) >> 16) & 0xffff)
#define LONG_L(val)         (((u32)(val)) & 0xffff)

/* Short Split */
#define SHORT_H(val)        ((((u16)(val)) >> 8) & 0xff)
#define SHORT_L(val)        (((u16)(val)) & 0xff)

#define BIT(idx)            ((u32)0x01 << (idx))
#define BIT_GET(var, idx)   ((((var) & BIT(idx)) == 0) ? 0 : 1)
#define BIT_SET(var, idx)   ((var) |= BIT(idx))
#define BIT_CLR(var, idx)   ((var) &= ~(BIT(idx)))

/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/

typedef enum _sha_type_t
{
    SHA_1,    // 120 bits
    SHA_128,  //md5  128bits
	SHA_224,
    SHA_256,
    SHA_384,
    SHA_512,
}Sha_Type_t;

typedef enum _des_type_t
{
    DES_ECB_ENCRYPT,
    DES_ECB_DECRYPT,
    DES_CBC_ENCRYPT,
    DES_CBC_DECRYPT,
}Des_Type_t;

typedef enum
{
    AES_ENCRYPT,
    AES_DECRYPT,

    AES_ECB_ENCRYPT,
    AES_ECB_DECRYPT,
    AES_CBC_ENCRYPT,
    AES_CBC_DECRYPT,

}AES_Type_t;

/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef signed long long	s64,   *ps64;
typedef signed long 		s32,   *ps32;
typedef signed short		s16,   *ps16;
typedef signed char 		s8,    *ps8;
typedef signed int			sint,  *psint;

typedef unsigned long long	u64,   *pu64;
typedef unsigned long		u32,   *pu32;
typedef unsigned short		u16,   *pu16;
typedef unsigned char		u8,    *pu8;
typedef unsigned int		uint,  *puint;

typedef void			   *pvoid;
typedef const void		   *pcvoid;

typedef const s64		   *pcs64;
typedef const s32		   *pcs32;
typedef const s16		   *pcs16;
typedef const s8		   *pcs8;
typedef const sint		   *pcsint;
typedef const u64		   *pcu64;
typedef const u32		   *pcu32;
typedef const u16		   *pcu16;
typedef const u8		   *pcu8;
typedef const uint		   *pcuint;
typedef const char 		   *pc8;



typedef u32 (*pCallback_Process_EntryPoint_Fun)(pu8 param,pu32 len);
typedef u32 (*pCallback_Process_EMV)(pu8 param,pu32 len);


//将 rupay,pure 内核中的 baseKernel.h , amex 中的 global.h
//中的通用函数，统一在这里声明
typedef struct ClessSignal_Message
{
	unsigned char	ucSignalType;                /* DET, RA, STOP, CLEAN, TIMEOUT, ACT */
	unsigned char   *MessageData;
	unsigned int	uiMessageDatalen;
}ClessSignal_Message;

typedef struct EMV_Queue
{
	ClessSignal_Message *pElement;
	unsigned char front;
	unsigned char rear;
	unsigned char ucMaxElement;
}EMV_Queue;



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

GLOBAL int Printf_(const char *fmt, ...);
GLOBAL int Sprintf_(char *str, const char *fmt, ...);

GLOBAL int Memcmp_(const void *str1, const void *str2, int n);
GLOBAL void * Memcpy_(void *dest, const void *src, int n);
GLOBAL void * Memset_(void *str, int c, int n);
GLOBAL int Strlen_(const char *str);

GLOBAL u32 tlv_new_(pu8 pTlvData,u32 T,u32 L,pu8 pV);
GLOBAL u32 tlv_get_t_(pu8 pTlvData);
GLOBAL u32 tlv_get_t_len_(pu8 pTlvData);
GLOBAL u32 tlv_get_l_(pu8 pTlvData);
GLOBAL u32 tlv_get_l_len_(pu8 pTlvData);
GLOBAL pu8 tlv_get_v_(pu8 pTlvData);
GLOBAL u32 tlv_get_node_l_(pu8 pTlvData);
GLOBAL u32 tlv_add_child_(pu8 pTlvData,u32 T,u32 L,pu8 pV);
GLOBAL pu8 tlv_find_(pu8 pTlvData,u32 T);
GLOBAL u32 tlv_delete_(pu8 pTlvData,u32 T);
GLOBAL u32 tlv_modify_(pu8 pTlvData,u32 T,u32 L,pu8 pV);
GLOBAL u32 tlv_replace_(pu8 pTlvData, u32 T, u32 L, pu8 pV);
GLOBAL u32 tlv_move_(pu8 pTlvData, pu8 pTlvDstData, u32 T);
GLOBAL bool tlv_is_composite_(pu8 pTlvData);
GLOBAL pu8 tlv_get_first_child_(pu8 pTlvData);
GLOBAL pu8 tlv_get_next_child_(pu8 pTlvData);
GLOBAL u32 tlv_batch_move_(pu8 pTlvData, pu8 pTlvDstData);

GLOBAL bool rsa_decrypt_(pu8 n,u32 nLen,pu8 d,u32 dLen,pu8 iBuf,u32 iLen,pu8 oBuf);
GLOBAL bool rsa_encrypt_(pu8 n,u32 nLen,pu8 e,u32 eLen,pu8 iBuf,u32 iLen,pu8 oBuf);
GLOBAL bool sha_(Sha_Type_t type,pu8 ibuf,u32 ibufLen,pu8 obuf);
GLOBAL bool des_(Des_Type_t desType,pu8 key,u32 keyLen,pu8 fill,pu8 ibuf,u32 ibufLen,pu8 obuf);
GLOBAL bool aes_(AES_Type_t aesType,pu8 key,u32 keyLen,pu8 ibuf,u32 ibufLen,pu8 obuf);



GLOBAL u32 Kernel_load_callback_Routine_EMV(pCallback_Process_EMV pszRecvData,u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_EMV(pu8 tlv_buffer,u32 len);
GLOBAL u32 Kernel_Prepare_AID_List_EMV(pu8 tlv_buffer,u32 len);
GLOBAL u32 Kernel_Start_EMV(pu8 tlv_buffer,pu32 len);
GLOBAL u32 Kernel_EMV_Get_ICCard_TransLog(pu8 tlv_buffer,pu32 len);
GLOBAL u32 Kernel_EMV_Set_Element(pu8 pInTLVData,u32 uiInTLVDataLen);
GLOBAL u32 Kernel_EMV_Get_Element(u32 uiTAG,pu8 pOutData,pu32 pOutDataLen);
GLOBAL u32 Kernel_Customized_Contact_EMV(pu8 buffer);

GLOBAL u32 Contactless_load_callback_Routine(pCallback_Process_EntryPoint_Fun pszRecvData,u32 dwRecvDataLen);
GLOBAL u32 Contactless_init_EntryPoint(pu8 tlv_buffer,u32 len);
GLOBAL u32 Contactless_prepare_EntryPoint_AID_List(pu8 tlv_buffer,u32 len);
GLOBAL u32 Contactless_Start_Kernel(pu8 tlv_buffer,pu32 tlv_buffer_len);
GLOBAL u32 Contactless_Clean_Kernel(pu8 tlv_buffer,pu32 tlv_buffer_len);
GLOBAL u32 Contactless_Kernel_Customized(pvoid buffer);
GLOBAL u32 Contactless_Kernel_Set_Proprietary_Tag(pvoid buffer);


GLOBAL u32 Kernel_load_callback_Routine_PayPass_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_PayPass_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_PayPass_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_PayPass_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_PayPass_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_PayWave_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_PayWave_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_PayWave_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_PayWave_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_PayWave_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_AMEX_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_AMEX_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_AMEX_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_AMEX_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_AMEX_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_JCB_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_JCB_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_JCB_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_JCB_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_JCB_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_Discover_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_Discover_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_Discover_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_Discover_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_Discover_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_qUICS_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_qUICS_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_qUICS_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_qUICS_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_qUICS_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_qPBOC_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_qPBOC_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_qPBOC_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_qPBOC_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_qPBOC_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_Rupay_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_Rupay_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_Rupay_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_Rupay_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_Rupay_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_MirPS_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_MirPS_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_MirPS_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_MirPS_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_MirPS_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_Interac_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_Interac_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_Interac_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_Interac_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_Interac_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_PURE_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_PURE_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_PURE_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_PURE_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_PURE_(pu8 buffer);

GLOBAL u32 Kernel_load_callback_Routine_BANCOMAT_(pCallback_Process_EntryPoint_Fun pszRecvData, u32 dwRecvDataLen);
GLOBAL u32 Kernel_Init_BANCOMAT_(pu8 tlv_buffer, u32 len);
GLOBAL u32 Kernel_Start_BANCOMAT_(pu8 tlv_buffer, pu32 len);
GLOBAL u32 Kernel_Customized_Contactless_BANCOMAT_(pu8 buffer);
GLOBAL u32 Kernel_Set_Proprietary_Tag_BANCOMAT_(pu8 buffer);



GLOBAL void EMV_Queue_init(EMV_Queue *q);
GLOBAL unsigned int EMV_Queue_isFull(EMV_Queue *q);
GLOBAL unsigned int EMV_Queue_isEmpty(EMV_Queue *q);
GLOBAL unsigned int EMV_Queue_Insert_Element(EMV_Queue *q, ClessSignal_Message signMsg);
GLOBAL unsigned int EMV_Queue_Delete_Element(EMV_Queue * q, ClessSignal_Message *signMsg);

GLOBAL unsigned int Asc_To_Hex(const char* src, unsigned char* dest, unsigned int* num_digits);
GLOBAL unsigned int Hex_To_Asc(const unsigned char* lpbInBuf, unsigned int dwInLen, char* lpbOutBuf);
GLOBAL unsigned long Bcd_To_Long(unsigned char *pucSrc, unsigned int uiSrcLen);
GLOBAL unsigned long Hex_To_Long(unsigned char*pucSrc,unsigned int uiSrcLen);
GLOBAL unsigned int Long_to_Hex(unsigned char *pucDest,unsigned int uiLen,unsigned long *pulSrc);
GLOBAL unsigned int Long_To_Bcd(unsigned char *pucDest,unsigned int uiDestLen,unsigned long *pulSrc);



#ifdef __cplusplus
}
#endif
/*
*******************************************************************************
*   End of File
*******************************************************************************
*/

#undef GLOBAL

#endif  /* #ifndef _KERNEL_API_H */


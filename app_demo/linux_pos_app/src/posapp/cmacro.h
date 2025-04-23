#ifndef _CMACRO_H__
#define _CMACRO_H__ 

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * -----------------------------------------------------------------------------
 * define   types
 * -----------------------------------------------------------------------------
 */
#ifndef uint32_t
#define uint32_t  unsigned int  
#endif

#ifndef uint16_t
#define uint16_t  unsigned short 
#endif

#ifndef uint8_t
#define uint8_t   unsigned char  
#endif

#ifndef uchar
#define uchar   unsigned char  
#endif
/*
 * -----------------------------------------------------------------------------
 * define
 * -----------------------------------------------------------------------------
 */
#ifndef YES
#define YES       0x01 
#endif

#ifndef NO
#define NO        0x00 
#endif

#ifndef OK
#define OK        0x01 
#endif

#ifndef ERROR
#define ERROR     0x01 
#endif

#ifndef ON
#define ON        0x01 
#endif

#ifndef OFF
#define OFF       0x00 
#endif

#ifndef NORMAL
#define NORMAL    0x00 
#endif

#ifndef EXCEPTION
#define EXCEPTION  0x01 
#endif

#ifndef true
#define true       0x00 
#endif

#ifndef false
#define false      0x01 
#endif

#ifndef yes
#define yes        0x01
#endif

#ifndef no
#define no         0x00 
#endif

#ifndef success
#define success    0x00 
#endif

#ifndef fail
#define fail       0x01 
#endif

/*
 * Operation macro definition
 */
// Get the offset of member MEMBER in structure TYPE
#ifndef FPOS
#define FPOS(TYPE, MEMBER) ((unsigned int)&((TYPE *)0)->MEMBER)
#endif

// Get the size of the MEMBER member of the TYPE structure
#ifndef FSIZE
#define FSIZE(TYPE, MEMBER) (sizeof(((TYPE*)0)->MEMBER))
#endif

#ifndef LEN
#define LEN(x,Type)   (sizeof(x)/sizeof(Type))
#endif

#ifndef DIM
#define DIM(x)    (sizeof(x)/sizeof(x[0]))   
#endif

#ifndef CLRBUF
#define CLRBUF(Buf)     memset(Buf, 0, sizeof(Buf))  
#endif

#ifndef BUFCLR
#define BUFCLR(Buf)      memset(Buf, 0, sizeof(Buf)) 
#endif

#ifndef ZERO_STRUCT
#define ZERO_STRUCT(x)     memset((char *) &(x), 0, sizeof(x))
#endif

#ifndef CLRSTRU
#define CLRSTRU(x)      memset((char *) &(x), 0, sizeof(x)) 
#endif

#ifndef FILLBUF
#define FILLBUF(Buf,x)  memset(Buf, x, sizeof(Buf)) 
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


/*
 * Convert macro definition
 */

#ifndef BCD2BIN
#define BCD2BIN(x)         (((x) & 0x0f) + ((x)>>4)*10)
#endif

#ifndef BIN2BCD
#define BIN2BCD(x)          ((((x)/10)<<4) + (x)%10) 
#endif

#ifndef LHALFB
#define LHALFB(x)     (unsigned char)(x&0x0F)  //Get the low half byte
#endif

#ifndef HHALFB
#define HHALFB(x)     (unsigned char)(x>>4)    //Get the high half byte
#endif

#ifndef LBYTE
#define LBYTE(x)      (unsigned char)(x)       //Get 0-7 digits
#endif

#ifndef HBYTE
#define HBYTE(x)      (unsigned char)(x>>8)    //Get 8-15 digits
#endif

#ifndef H2BYTE
#define H2BYTE(x)     (unsigned char)(x>>16)   //Obtain 16~23 digits
#endif

#ifndef H3BYTE
#define H3BYTE(x)     (unsigned char)(x>>24)   //Obtain 24~31 digits
#endif

#ifndef LHALFW
#define LHALFW(x)     (unsigned short)(x)      //Get low half letters
#endif

#ifndef HHALFW
#define HHALFW(x)     (unsigned short)(x>>16)  //Obtain high half width characters
#endif
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef IFBIT
	#define IFBIT(Data,bit)      (Data&(1<<bit))  //
#endif

#ifndef GETBIT
	#define GETBIT(Data,bit)     ((Data&(1<<bit))>>bit)  //
#endif

// Move the specified bit old to the new bit newbit, for example: 0010,0000 MOVEBIT(0x20,5,3)=0000,1000
#ifndef MOVEBIT
	#define MOVEBIT(Data,oldbit,newbit)     (((Data&(1<<oldbit))>>oldbit)<<newbit)
#endif
// Move the specified bit old right Offset bit such as: 0010,0000 MOVERBIT(0x20,5,2)=0000,1000
#ifndef MOVERBIT
	#define MOVERBIT(Data,oldbit,OffsetBit)    ((Data&(1<<oldbit))>>OffsetBit)
#endif
// Move the specified bit left of the Offset bit such as: 0010,0000 MOVELBIT(0x20,5,2)=1000,0000
#ifndef MOVELBIT
	#define MOVELBIT(Data,oldbit,OffsetBit)    ((Data&(1<<oldbit))<<OffsetBit)
#endif

#ifndef SETBIT
	#define SETBIT(Data,bit)     (((Data |(1<<bit)))
#endif

#ifndef CLRBIT
	#define CLRBIT(Data,bit)   ((Data &(~(1<<bit))))
#endif
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Nibble conversion b3<->b0 b2<->b1
#ifndef CONVERTHBYTE
	#define CONVERTHBYTE(x)  (MOVERBIT(x,3,(3-0))|MOVERBIT(x,2,(2-1))|MOVELBIT(x,0,(3-0))|MOVELBIT(x,1,(2-1)))
#endif
// Byte conversion b7<->b0 b6<->b1 b5<->b2 b4<->b3
#ifndef CONVERTBYTE
	#define CONVERTBYTE(x)  ((x>>7)|MOVERBIT(x,6,(6-1))|MOVERBIT(x,5,(5-2))|MOVERBIT(x,4,(4-3))|MOVELBIT(x,1,(6-1))|MOVELBIT(x,2,(5-2))|MOVELBIT(x,3,(4-3))|(x<<7))
#endif
// --------------------------------------------------------------------------------------------------------------------------------
// Combine nibble into 8 bits
#ifndef COMBINE8
	#define COMBINE8(High,Low)         ((unsigned char)(High<<4) | (unsigned char)(Low & 0x0F))
#endif
// Merge 2 bytes into 16 bits
#ifndef COMBINE16
	#define COMBINE16(High,Low)        ((unsigned short)(High<<8) | (unsigned short)(Low & 0xFF))
#endif
// 4 bytes merge into 32 bits
#ifndef COMBINE32
	#define COMBINE32(Hhigh,Hlow,Lhigh,Llow)  ( (unsigned long)(Hhigh<<24) | (unsigned long)(((unsigned char)Hlow)<<16) | (unsigned long)(((unsigned char)Lhigh)<<8) | (unsigned long)((unsigned char)Llow))
#endif



/*
 * Determine the return value macro definition
 */
#define  assert_ok(x)             {if(x){return EXCEPTION;}}   //Non zero return EXCEPTION
#define  assert_null(x)           {if(!x)  return 1;}  //Return of null pointer
#define  ASSERT(p)

#define  ASSERT_NORMAL(x)              assert_ok(x)
#define  ASSERT_TRANSBOUNDARY(x,y)    {if(x>y){DISPERR(x);return 1;}} /* Cross border detection */
#define  STOP_LOOP()                  {while(1){sleep(60);}}

// Define application function return
enum
{
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ERRVALUE	=	-1,
	CANCELED	=	-2,
	APPTIMEOUT	=	-3,
	BACK		=	-4,
	CHECKCARD	=	-5,
	END			=	-6,             //Return to End Flag
	AMTEXCEED	=	-7,
	NOT_EXPECTED=	-8,				//Not meeting expectations
	NEED_RELOGIN=	-9,				//Need to sign in again
	WRITE_CARD_ERR= -10,			//Card writing failed
	ERR_CARD	=	-11,			//Invalid M1 management card
	NOT_DATA	=	-12,			//Countless cards in the card
	RETRADE		=	-13,			 //Please refresh and go through the transaction process again
	DISCONNECTED=	-14,			//connection dropped
	PARSE_FAIL	=	-15,			//Data parsing failed
	NOTENOUGH_STORAGE=-16,			//The data length is greater than the cache
	NOT_RECORD_NEED_SENDUP=-17,		
	NEED_CHECK	=-18,				//Need to check payment status
	BACK_2		=-19,				//Please refresh, power off required, prompt user
	BACK_UNIONPAY	=-20,			//Return to try the bank card process
	BACK_3		=-21,				//Flashcard refresh
	BLACK_CARD	=-22,				//The blacklist has been locked
	BACK_4		=-23,				//Special return after calculating ticket price
	BACK_UNIONPAY_1902	=-24,		//1902 Return to attempt bank card process
// //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	CNTC_BESTPAY=    1,
	M1_CARD,						
	CHANG_AN_TONG_CARD,			
	QR_BESTPAY,					
	QR_NFCPAY,					
	APPLE_NFCPAY,				
	MANAGE_CARD ,				
	READCARD_MEMBER_CARD,		
	QR_CSCANBPAY,				
	YINS_MEMBER_CARD,			
	M1_MANAGE_CARD,				
	RF_CARD,					
	DRIVER_LOGIN,				
	SETTING_CARD,				
	COIN_RET,					
	TRANSP_CARD_TYPE,			
	LUHU_BUS_CARD,				
};


#define  EM_NULL			0
#define  PACK_WHEAD_LEN		11 //Message sender length
#define  PACK_RHEAD_LEN     11 //Message header receiving length


/*
 * Transaction type nTransType definition
 */
typedef enum enmTransType
{
	PURCHASE = 1,                           // Sale
	INQUERY,                                // Query balance
	REFUND,                                 // refund
	POS_VOID,                               // voice
	PRE_AUTH,                               // Pre authorization
	PRE_AUTH_ADD,                           // Add Pre authorization
	AUTH_VOID,                              // Pre authorization revocation
	AUTH_DAY_VOID,                          // Revocation of Pre Authorization on the Same Day
	CONFIRM,                                // Pre authorization completion request
	CONFIRM_OFFLINE,                        // Pre authorization completion notification
	CONFIRM_VOID,                           // Pre authorization completed revocation
	LOGIN,                                  // login 
	LOGOUT,                                 // logout
	SETTLE,                                 // settlement
	BATCH_UP,                               // Batch delivery
	SETTLE_2,                               // Secondary settlement
	ONLINE,                                 // Online management of transactions
	DOWN_PARAM,                             // Download parameters
	UPLOADSTATE,                            //Sending POS status

	EP_PURCHASE,			                //
	EP_INQUERY,			                    //
	STRIPE,					                //
	ONLINEMANA,	                            //
	EC_QUERY,		     	                //
	EC_PURCHASE,			                //
	qPBOC,			                        //
	MAINKEY_DOWN,							//Download Master Key
	GETURL,					//Get QR code online
	QUERY_TRANSTA,			//Query transaction status
	YS_YXLM,
	YS_YFF,

} TRANS_TYPE;

#define FINANCE_PRODUCT_PAYPASSONLY             0
#define FINANCE_PRODUCT_PAYPASS_CARDREADER      1
#define FINANCE_PRODUCT_ALL_FUNCTION            2


#define     MAC_NO                  0                   /* No MAC verification                  */
#define     MAC_NORMAL              2                   /* Normal MAC verification, XOR all before DES  */
#define     MAC_X99                 1                   /* Standard MAC verification                */
#define     MAC_ECB                 3                   /* MAC verification of UnionPay                */
#define     MAC_X919                4                   /* MAC verification                */


#define EM_VOIDISO_LEN			sizeof(ISODataStru)


#define EM_SHOW_MASK       (uchar)1
#define EM_SHOW_NOMASK     (uchar)0


#ifndef FALSE
#define FALSE (1 == 0)
#endif

#ifndef TRUE
#define TRUE  (1 == 1)
#endif

// Exit Return Value
#define EM_EXITMODE_ENTER      (uchar)0x00
#define EM_EXITMODE_TIMEOUT    (uchar)0x8A
#define EM_EXITMODE_CIDRING    (uchar)0xF0
#define EM_EXITMODE_PAY        (uchar)0xF2
#define EM_EXITMODE_QUIT       (uchar)0xF3
#define EM_EXITMODE_HF         (uchar)0xF4
#define EM_EXITMODE_LEFT       (uchar)0xF5
#define EM_EXITMODE_RIGHT      (uchar)0xF6
#define EM_EXITMODE_FUNCTION   (uchar)0xF7
#define EM_EXITMODE_DEL        (uchar)0xF8
#define EM_EXITMODE_HOOKON     (uchar)0xFE
#define EM_EXITMODE_CANCEL     (uchar)0xFF
#define EM_EXITMODE_STAR       EM_EXITMODE_CANCEL
#define EM_EXITMODE_JING       EM_EXITMODE_ENTER


// Platform Return Processing Requirement Type
enum
{
	PROCESS_TYPE_NONE						= 0,	//None
	PROCESS_TYPE_DOWNLOAD_PARA				= 1,	//Download magnetic card parameters
	PROCESS_TYPE_UPLOAD_STATE				= 2,	//Upload terminal magnetic stripe card status information
	PROCESS_TYPE_LONGIN						= 3,	//login
	PROCESS_TYPE_DOWNLOAD_PUBKEY			= 4,	//Download IC card public key
	PROCESS_TYPE_DOWNLOAD_EMVPARA			= 5,	//Download IC card parameters
	PROCESS_TYPE_DOWNLOAD_QPSPARA			= 9,	//Download small non-contact parameters
	PROCESS_TYPE_DOWNLOAD_QPSPARA_BIN		= 0x0b, //Download the new whitening list for small non card bin
	PROCESS_TYPE_DOWNLOAD_QPSPARA_BIN_BL	= 0x0c, //Download the small non card bin blacklist
	PROCESS_TYPE_UPLOAD_TERM_CONFIG			= 0x0d, //Terminal configuration upload
};

#ifdef __cplusplus
}
#endif
#endif




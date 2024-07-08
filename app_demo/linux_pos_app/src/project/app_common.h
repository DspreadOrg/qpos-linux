#ifndef _APP_COMMON_H_
#define _APP_COMMON_H_

#include "appinc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	DUKPT_DEC_ECB = 0x00,
	DUKPT_ENC_ECB = 0x01,
    DUKPT_DEC_CBC = 0x02,
	DUKPT_ENC_CBC = 0x03,
} DUKPT_DES_MODE;

extern u32 get_tlv_value(pu8 pool,u32 T,pu8 V );
extern void set_tlv_view(u8 *pTlvBuf, u32 dwTag, T_U8_VIEW uvVal);
extern T_U8_VIEW get_tlv_view(u8 *pTlvBuf, u32 dwTag);
extern int _asc_2_bcd(uchar * ascstr, int asclen, uchar * bcdstr, int align);
extern u32 asc_to_bcd(pu8 src,u32 len,pu8 dest );
extern u32 asc_to_bcd_sample(pu8 src,u32 len,pu8 dest );
extern u32 bcd_to_asc_sample(pu8 src,u32 len,pu8 dest );
extern u32 format_amt_ascn_to_bcd6(pu8 amt,u32 amtlen,pu8 amtOut );

extern int dukpt_encrypt(int index,int DataInLen,unsigned char *DataIn,unsigned char *DataOut,unsigned char *Ksn,int mode);
extern void track_4byte_to_3byte(pu8 data, u32 len,pu8 out );
extern u32 intercept_card_holder_name_from_track1(pcu8 track1dat, u32 track1length,pu8 cardholder );
extern u32 intercept_card_validity_data_from_track2(pcu8 track2dat, u32 track2length,pu8 validitydata );
extern u32 intercept_card_service_code_from_track2(pcu8 track2dat, u32 track2length,pu8 serviceCode );
extern u32 intercept_pan_from_track2(pcu8 track2dat, u32 track2length,pu8 cardNo );
extern u32 intercept_card_mask_from_pan(pcu8 pandat, u32 panlength,u8 mask,pu8 cardMaskNum );
#ifdef __cplusplus
}
#endif
#endif 
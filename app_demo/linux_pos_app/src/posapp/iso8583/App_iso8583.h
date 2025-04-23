
#ifndef __ISOMSG8583_H__
#define __ISOMSG8583_H__

#include "App_ISOMsg.h"
#ifdef __cplusplus
extern "C"
{
#endif

// ////////////////////////////////////////
// Packaging and unpacking function implementation
    int PubFPAMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPAMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPABitmapPack(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPABitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBBitmapPack(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBBitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);

/*
 * Variable length domain length -> ASC
 */
    int PubFPACharPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPACharUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBCharPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBCharUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
	int PubFPADigitPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPADigitUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBDigitPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBDigitUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBAmountPack_ASC(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBAmountUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBBinaryPack_ASC(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBBinaryUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);

/*
 * Variable length domain length -> BCD
 */
    int PubFPACharPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPACharUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBCharPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBCharUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
	int PubFPADigitPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPADigitUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBDigitPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBDigitUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBAmountPack_BCD(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBAmountUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBBinaryPack_BCD(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBBinaryUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);

/*
 * Variable length domain length -> HEX
 */
    int PubFPACharPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPACharUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBCharPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBCharUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
	int PubFPADigitPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPADigitUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBDigitPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBDigitUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBAmountPack_HEX(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBAmountUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);
    int PubFPBBinaryPack_HEX(ISO_FIELD_DEF * pthis,
                         const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata);
    int PubFPBBinaryUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata);


	
#ifdef __cplusplus
}
#endif

#endif                          // __ISOMSG_H__

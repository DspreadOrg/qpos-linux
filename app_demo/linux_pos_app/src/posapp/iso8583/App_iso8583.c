
#include "App_ISOMsg.h"
#include "App_iso8583.h"


int PubFPAMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPAMsgIDPack(pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPAMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPAMsgIDUnpack(pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBMsgIDPack(pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBMsgIDUnpack(pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPABitmapPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPABitmapPack(pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPABitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPABitmapUnpack(pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBBitmapPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBBitmapPack(pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBBitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBBitmapUnpack(pthis, pfieldbuf, pfieldlen, pindata);
}
/*
 * Variable length domain length -> ASC
 */
int PubFPACharPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPACharPack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPACharUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPACharUnpack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBCharPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBCharPack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBCharUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBCharUnpack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPADigitPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPADigitPack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPADigitUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPADigitUnpack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBDigitPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBDigitPack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBDigitUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBDigitUnpack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBAmountPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBAmountPack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBAmountUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBAmountUnpack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBBinaryPack_ASC(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBBinaryPack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBBinaryUnpack_ASC(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBBinaryUnpack(FLLENGTH_MODE_ASC,pthis, pfieldbuf, pfieldlen, pindata);
}
/*
 * Variable length domain length -> BCD
 */
int PubFPACharPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPACharPack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPACharUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPACharUnpack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBCharPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBCharPack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBCharUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBCharUnpack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPADigitPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPADigitPack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPADigitUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPADigitUnpack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBDigitPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBDigitPack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBDigitUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBDigitUnpack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBAmountPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBAmountPack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBAmountUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBAmountUnpack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBBinaryPack_BCD(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBBinaryPack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBBinaryUnpack_BCD(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBBinaryUnpack(FLLENGTH_MODE_BCD,pthis, pfieldbuf, pfieldlen, pindata);
}

/*
 * Variable length domain length -> HEX
 */
int PubFPACharPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPACharPack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPACharUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPACharUnpack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBCharPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBCharPack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBCharUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBCharUnpack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPADigitPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPADigitPack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPADigitUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPADigitUnpack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBDigitPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBDigitPack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBDigitUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBDigitUnpack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBAmountPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBAmountPack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBAmountUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBAmountUnpack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, pfieldlen, pindata);
}

int PubFPBBinaryPack_HEX(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    return FPBBinaryPack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, fieldlen, poutdata);
}

int PubFPBBinaryUnpack_HEX(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    return FPBBinaryUnpack(FLLENGTH_MODE_HEX,pthis, pfieldbuf, pfieldlen, pindata);
}



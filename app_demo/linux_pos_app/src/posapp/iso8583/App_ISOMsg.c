#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "App_ISOMsg.h"
#include "App_iso8583.h"
#include "function.h"

#define  ASSERT(p)

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef isprint
#define isprint(c) ((char)(c) >= 0x20 && (char)(c) < 0x7f)
#endif

#ifndef isdigit
#define isdigit(c) ((char)(c) >= 0x40 && (char)(c) <= 0x49)
#endif

/*
 * Function function: Complement the data in an ISO domain
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be filled
 * len - The length of the 8583 domain data to be filled
 * head - whether there are header characters (the domain of the amount type has C/D flag)
 * Exit parameters: poutdata - a buffer used to receive the completed data
 * Return Value: Completed string length
 */
int AddPadChar(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata, unsigned char head)
{
	int count = 0;
	int len;
	if (pthis->lentype == LEN_FIX)
		len = pthis->len;
	else
		len = MIN(pthis->len, fieldlen);

	len += head;
	fieldlen = MIN(len, fieldlen);

// put C/D char
	if (head == 1)
	{
		poutdata[count++] = pfieldbuf[0];
		--len;
		--fieldlen;
	}

// add left padchar
	if ((pthis->lentype == LEN_FIX) && (pthis->align == ALN_RIGHT))
	{
		memset(poutdata + count, pthis->padchar, len - fieldlen);
		count += len - fieldlen;
	}

	memcpy(poutdata + count, (char *) pfieldbuf + head, fieldlen);
	count += fieldlen;

// add right padchar
	if ((pthis->lentype == LEN_FIX) && (pthis->align == ALN_LEFT))
	{
		memset(poutdata + count, pthis->padchar, len - fieldlen);
		count += len - fieldlen;
	}

	ASSERT(count == (len + head));
	return count;
}

/*
 * Function function: Calculate the length bytes in 8583 packets according to the type of ISO domain (BCD form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * len - the length of the domain data
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * pcount - the number of bytes in length (fixed length is 0, LLVAR type is 1, and LLLVAR type is 2)
 * Return Value: None
 * Description: BCD code form, such as length 104, will be represented as 0x01, 0x04, a total of 2 bytes. If it is 9, it will be represented as 0x09, a total of 1 byte
 */
void FLBLengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount)
{
	unsigned char *wpt = poutdata;
	if (pthis->lentype == LEN_LLVAR)
		(*wpt++) = (unsigned char)(len % 10) + (len / 10) * 16;
	else if (pthis->lentype == LEN_LLLVAR)
	{
		(*wpt++) = (unsigned char)(len / 100);
		(*wpt++) = (unsigned char)((len % 100) / 10) * 16 + (len % 100) % 10;
	}

	*pcount = (wpt - poutdata);
	return;
}

/*
 * Function function: Calculate the length of the domain in the message based on the type of the ISO domain and the length bytes in the 8583 message (BCD form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data pointer containing the domain message
 * Exit parameters: plen - used to receive the calculated length
 * pcount - the number of bytes in length (fixed length is 0, LLVAR type is 1, and LLLVAR type is 2)
 * Return Value: None
 * Description: BCD code form, such as length 104, will be represented as 0x01, 0x04, a total of 2 bytes. If it is 9, it will be represented as 0x09, a total of 1 byte
 */
void FLBLengthGet(ISO_FIELD_DEF * pthis, unsigned char *pindata, int *plen, int *pcount)
{
	unsigned char *rpt = pindata;

	if (pthis->lentype == LEN_FIX)
		*plen = pthis->len;
	else
	{
		*plen = (*rpt) - ((*rpt) >> 4) * 6;
		rpt++;
		if (pthis->lentype == LEN_LLLVAR)
		{
			*plen = (*plen) * 100 + ((*rpt) - ((*rpt) >> 4) * 6);
			rpt++;
		}
		if (*plen > pthis->len)
			*plen = pthis->len;
	}

	*pcount = (rpt - pindata);
	return;
}

/*
 * Function function: Calculate the length bytes in 8583 packets according to the type of ISO domain (ASC form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * len - the length of the domain data
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * pcount - the number of bytes in length (fixed length is 0, LLVAR type is 1, and LLLVAR type is 2)
 * Return Value: None
 * Description: ASC code form, such as length 104, will be represented as 0x31, 0x30, 0x34, a total of 3 bytes. If it is 9, it will be represented as 0x30, 0x39.
 */
void FLALengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount)
{
	unsigned char *wpt = poutdata;
	if (pthis->lentype == LEN_LLVAR)
    {
        (*wpt++) = (unsigned char)(len / 10) + 0x30;
		(*wpt++) = (unsigned char)(len % 10) + 0x30;
    }
	else if (pthis->lentype == LEN_LLLVAR)
	{
		(*wpt++) = (unsigned char)(len / 100) + 0x30;
		(*wpt++) = (unsigned char)((len % 100) / 10)  + 0x30;
        (*wpt++) = (unsigned char)((len % 100) % 10)  + 0x30;
	}

	*pcount = (wpt - poutdata);
	return;
}

/*
 * Function function: Calculate the length of the domain in the message based on the type of the ISO domain and the length bytes in the 8583 message (ASC form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data pointer containing the domain message
 * Exit parameters: plen - used to receive the calculated length
 * pcount - the number of bytes in length (fixed length is 0, LLVAR type is 1, and LLLVAR type is 2)
 * Return Value: None
 * Description: ASC code form, such as length 104, will be represented as 0x31, 0x30, 0x34, a total of 3 bytes. If it is 9, it will be represented as 0x30, 0x39.
 */
void FLALengthGet(ISO_FIELD_DEF * pthis, unsigned char *pindata, int *plen, int *pcount)
{
	unsigned char *rpt = pindata;

	if (pthis->lentype == LEN_FIX)
		*plen = pthis->len;
	else
	{
        *plen = 0;
        if (pthis->lentype == LEN_LLLVAR)
            *plen = ((*rpt++) - 0x30) * 100;
        *plen += ((*rpt++) - 0x30) * 10;
        *plen += (*rpt++) - 0x30;
		if (*plen > pthis->len)
			*plen = pthis->len;
	}

	*pcount = (rpt - pindata);
	return;
}

/*
 * Function function: Calculate the length bytes in 8583 packets according to the type of ISO domain (HEX form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * len - the length of the domain data
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * pcount - the number of bytes in length (fixed length is 0, LLVAR type is 1, and LLLVAR type is 2)
 * Return Value: None
 * Description: HEX code form, such as length 104, will be represented as 0x00, 0x68, 2 bytes in total, if 9, it will be represented as 0x09, 1 byte in total
 */
void FLHLengthSet(ISO_FIELD_DEF *pthis, int len, unsigned char *poutdata, int *pcount)
{
	unsigned char *wpt = poutdata;
	if (pthis->lentype == LEN_LLVAR)
		(*wpt++) = (unsigned char) len;
	else if (pthis->lentype == LEN_LLLVAR)
	{
		(*wpt++) = (unsigned char) (len / 256);
		(*wpt++) = (unsigned char) (len % 256);
	}

	*pcount = (wpt - poutdata);
	return;
}

/*
 * Function function: Calculate the length of the domain in the message based on the type of the ISO domain and the length bytes in the 8583 message (HEX form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data pointer containing the domain message
 * Exit parameters: plen - used to receive the calculated length
 * pcount - the number of bytes in length (fixed length is 0, LLVAR type is 1, and LLLVAR type is 2)
 * Return Value: None
 * Description: HEX code form, such as length 104, will be represented as 0x00, 0x68, 2 bytes in total, if 9, it will be represented as 0x09, 1 byte in total
 */
void FLHLengthGet(ISO_FIELD_DEF * pthis, unsigned char *pindata, int *plen, int *pcount)
{
	unsigned char *rpt = pindata;

	if (pthis->lentype == LEN_FIX)
		*plen = pthis->len;
	else
	{
        if (pthis->lentype == LEN_LLLVAR)
            *plen = (*rpt++) * 256;
        *plen = (*rpt++);
		if (*plen > pthis->len)
			*plen = pthis->len;
	}

	*pcount = (rpt - pindata);
	return;
}

/*
 * Function function: Package data in an ISO domain (MSG_ID domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPAMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len = pthis->len;
	memcpy(poutdata, pfieldbuf, len);
	return len;
}

/*
 * Function function: Unpack the data in an ISO domain (MSG_ID domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPAMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len = pthis->len;
	memcpy(pfieldbuf, pindata, pthis->len);
	*pfieldlen = len;
	return len;
}

/*
 * Function function: Package data in an ISO domain (MSG_ID domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPBMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len = pthis->len >> 1;
	c_asc2bcd(poutdata, (unsigned char*) pfieldbuf, pthis->len, 0);
	return len;
}

/*
 * Function function: Unpack the data in an ISO domain (MSG_ID domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPBMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len = pthis->len >> 1;
	c_bcd2asc(pfieldbuf, pindata, pthis->len, 0);
	*pfieldlen = pthis->len;
	return len;
}

/*
 * Function function: Package data in an ISO domain (BITMAP bitmap domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPBBitmapPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int bitmaplen;
	int i, j;
	unsigned char bitmap[16], bitmask;

	bitmaplen = fieldlen >> 3;
	memset(bitmap, 0, 16);
	for (i = 0; i < bitmaplen; i++)
	{
		bitmask = 0x80;
		for (j = 0; j < 8; j++, bitmask >>= 1)
		{
			if (pfieldbuf[i * 8 + j])
				bitmap[i] |= bitmask;
		}
	}

	memcpy(poutdata, bitmap, bitmaplen);
	return bitmaplen;
}

/*
 * Function function: Unpack the data in an ISO domain (BITMAP bitmap domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPBBitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int bitmaplen;
	int i, j;
	unsigned char bitmask;

	if (pindata[0] & 0x80)
		bitmaplen = 16;
	else
		bitmaplen = 8;

	memset(pfieldbuf, 0, 128);
	for (i = 0; i < bitmaplen; i++)
	{
		bitmask = pindata[i];
		for (j = 0; j < 8; j++, bitmask <<= 1)
			pfieldbuf[(i << 3) + j] = (bitmask & 0x80) ? 1 : 0;
	}

	*pfieldlen = (bitmaplen << 3);
	return bitmaplen;
}

/*
 * Function function: Package data in an ISO domain (BITMAP bitmap domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPABitmapPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
    int bitmaplen;
    unsigned char bitmap[16];

    memset(bitmap, 0, 16);
    bitmaplen = FPBBitmapPack(pthis, pfieldbuf, fieldlen, bitmap);
    c_bcd2asc(poutdata, bitmap, bitmaplen * 2, 0);

    return bitmaplen * 2;
}

/*
 * Function function: Unpack the data in an ISO domain (BITMAP bitmap domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPABitmapUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
    int bitmaplen;
    unsigned char bitmap[16];

    c_asc2bcd(bitmap, pindata, 16, 0);
    if (bitmap[0] & 0x80)
        bitmaplen = 16;
    else
        bitmaplen = 8;
    c_asc2bcd(bitmap, pindata, bitmaplen * 2, 0);
    FPBBitmapUnpack(pthis, pfieldbuf, pfieldlen, bitmap);
    return bitmaplen * 2;
}


/*
 * Function function: Package data in an ISO domain (character type domain ASC code form)
 * Entry parameters: mode: variable length domain length calculation mode
 * pthis - ISO domain definition type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPACharPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len, count;
	unsigned char tmp[MAX_FIELD_LEN];

    len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthSet(pthis, len, poutdata, &count);
        break;
    default:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    }
	memcpy(poutdata + count, (char *) tmp, len);
	return count + len;
}

/*
 * 
 * Function:Unpack data from an ISO domain (in ASC code format for character type fields)
 * Input Paramter:pthis    ―― Type structure pointer defined by ISO domain
 * 				 pindata  ―― The data buffer containing the domain message
 * Output paramter:pfieldbuf―― Unpackaged 8583 domain data buffer
 * 					plen     ―― Return the length of domain data
 * return：The length of data used from pindata during unpacking
 * 
 */
int FPACharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int  len, count;
// debug("8583 package");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }
    memcpy(pfieldbuf, pindata + count, len);
	*pfieldlen = len;
	return count + len;
}

/*
 * Function function: Package data in an ISO domain (character type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPBCharPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{

	int i = 0;
	int count, len;
	unsigned char tmp[MAX_FIELD_LEN];

	for (i = 0; i < fieldlen; i++)
	{
		if (!isprint(pfieldbuf[i]))
		{
			return EXCEPTION;
		}
	}

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthSet(pthis, len, poutdata, &count);
        break;
    default:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    }

// For odd-length, the last half will be compressed by filling characters.
    if ((len & 1) && (pthis->align == ALN_LEFT))
        tmp[len++] = pthis->padchar;

	c_asc2bcd(poutdata + count, tmp, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));

    return count + (++len >> 1);
}

/*
 * Function function: Unpack the data in an ISO domain (character type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPBCharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }
	c_bcd2asc(pfieldbuf, pindata + count, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	*pfieldlen = len;
	for (i = 0; i < *pfieldlen; i++)
	{
		if (!isprint(pfieldbuf[i]))
		{
			*pfieldlen = 0;
			return EXCEPTION;
		}
	}
	return count + (++len >> 1);
}
/*
 * Function function: Unpack the data in an ISO domain (character type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPCCharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }
	len = len * 2;
	c_bcd2asc(pfieldbuf, pindata + count, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	*pfieldlen = len;
	for (i = 0; i < *pfieldlen; i++)
	{
		if (!isprint(pfieldbuf[i]))
		{
			*pfieldlen = 0;
			return EXCEPTION;
		}
	}
	return count + (++len >> 1);
}
/*
 * Function function: Package data in an ISO domain (numerical type domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPADigitPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int i, len, count;
	unsigned char tmp[MAX_FIELD_LEN];

	for (i = 0; i < fieldlen; i++)
	{
		if (!isdigit(pfieldbuf[i]))
			return EXCEPTION;
	}

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthSet(pthis, len, poutdata, &count);
        break;
    default:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    }
	memcpy(poutdata + count, (char *) tmp, len);
	return count + len;
}

/*
 * Function: Unpack data from an ISO domain (in numerical type ASC code format)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPADigitUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }
	memcpy(pfieldbuf, pindata + count, len);
	*pfieldlen = len;

	for (i = 0; i < *pfieldlen; i++)
	{
		if (!isdigit(pfieldbuf[i]))
		{
			*pfieldlen = 0;
			return EXCEPTION;
		}
	}

	return count + len;
}

/*
 * Function function: Package data in an ISO domain (numerical type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPBDigitPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int i, len, count;
	unsigned char tmp[MAX_FIELD_LEN];

	for (i = 0; i < fieldlen; i++)
	{
		if (!isdigit(pfieldbuf[i]))
			return EXCEPTION;
	}

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthSet(pthis, len, poutdata, &count);
        break;
    default:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    }
// For odd-length, the last half will be compressed by filling characters.
    if ((len & 1) && (pthis->align == ALN_LEFT))
        tmp[len++] = pthis->padchar;

	c_asc2bcd(poutdata + count, tmp, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	return count + (++len >> 1);
}

/*
 * Function function: Unpack the data of an ISO domain (numerical type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPBDigitUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }
	c_bcd2asc(pfieldbuf, pindata + count, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	*pfieldlen = len;

	for (i = 0; i < *pfieldlen; i++)
	{
		if (!isdigit(pfieldbuf[i]))
		{
			*pfieldlen = 0;
			return EXCEPTION;
		}
	}

	return count + (++len >> 1);
}

/*
 * Function function: Package data in an ISO domain (amount type domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPBAmountPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len, count;
	unsigned char tmp[MAX_FIELD_LEN];

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 1); // len contain the char 'C/D'
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthSet(pthis, len, poutdata, &count);
        break;
    default:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    }
	poutdata[count++] = tmp[0];
	--len;
	c_asc2bcd(poutdata + count, tmp + 1, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	return count + (++len >> 1);
}

/*
 * Function function: Unpack the data of an ISO domain (Amount type domain ASC code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPBAmountUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len, count;
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }

	pfieldbuf[0] = pindata[count++];

	c_bcd2asc(pfieldbuf + 1, pindata + count, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	*pfieldlen = count + len;
	return count + (++len >> 1);
}

/*
 * Function function: Package data in an ISO domain (binary type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pfieldbuf - 8583 domain data pointer to be packaged
 * len - the 8583 domain data length to be packaged
 * Exit parameters: poutdata - a buffer used to receive calculated data
 * Return Value: Packed data length
 */
int FPBBinaryPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len, count;
	unsigned char tmp[MAX_FIELD_LEN];

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthSet(pthis, len, poutdata, &count);
        break;
    default:
        FLBLengthSet(pthis, len, poutdata, &count);
        break;
    }
	memcpy(poutdata + count, (char *) tmp, len);
	return count + len;
}

/*
 * Function function: Unpack the data of an ISO domain (binary type domain BCD code form)
 * Entry parameters: pthis - ISO domain defined type structure pointer
 * pindata - data buffer containing the domain message
 * Export parameters: pfieldbuf - unpacked 8583 domain data buffer
 * plen - Return the domain data length
 * Return Value: The length of data used from pindata when unpacking
 */
int FPBBinaryUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len, count;
// debug("University Packaging");
    switch(mode)
    {
    case FLLENGTH_MODE_ASC:
        FLALengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_BCD:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    case FLLENGTH_MODE_HEX:
        FLHLengthGet(pthis, pindata, &len, &count);
        break;
    default:
        FLBLengthGet(pthis, pindata, &len, &count);
        break;
    }
	memcpy(pfieldbuf, pindata + count, len);
	*pfieldlen = len;
	return count + len;
}

/*
 * Function function: Clear all member variable values ​​in ISO_data structure
 * Entry parameters: pIsoStru - Source data structure ISO_data structure pointer
 * Export parameters: pIsoStru - the clear ISO_data structure pointer
 * Return Value: NORMAL - Success
 * EXCEPTION - Failed, IsoStru is empty
 */
int PubDelAllBit(ISODataStru *pIsoStru)
{
	int i;

	if (NULL == pIsoStru) {
		return EXCEPTION;
	}

// Initialize the iso fields
    for (i = 0; i <= MAX_FIELD_NUM; i++)
	{
		pIsoStru->bitmap[i] = 0;
		pIsoStru->fld[i].off = 0;
		pIsoStru->fld[i].len = 0;
	}
    pIsoStru->used = 0;
	return NORMAL;
}

/*
 * Function function: Clear the value of the nPosition field in the IsoStru structure, and clear the corresponding flags in the bitmap.
 * Entry parameters:
 * IsoStru - Source data structure ISO_data structure pointer
 * nPosition -- nPosition member in ISO_data structure
 * Export parameters:
 * IsoStru - Cleared ISO_data structure pointer
 * Return Value:
 * NORMAL - Success
 * EXCEPTION - Failed, IsoStru is NULL
 * ― Invalid member number (nPosition <0 or >=256)
 */
int PubDelOneBit(ISODataStru *pIsoStru, int nPosition)
{
    int i;

    if (NULL == pIsoStru) {
        return EXCEPTION;
    }

    if (nPosition < 0 || nPosition >= MAX_FIELD_NUM){
        return EXCEPTION;
    }

    pIsoStru->bitmap[nPosition - 1] = 0;
    if (pIsoStru->fld[nPosition].off != 0 && pIsoStru->fld[nPosition].len != 0)
    {
// move  forward all the field data in iso->databuf
        for (i = 0; i <= MAX_FIELD_NUM; i++)
        {
            if (pIsoStru->fld[i].len > 0 && (pIsoStru->fld[i].off > pIsoStru->fld[nPosition].off))
                pIsoStru->fld[i].off -= pIsoStru->fld[nPosition].len;
        }
        pIsoStru->used -= pIsoStru->fld[nPosition].len;
        for (i = pIsoStru->fld[nPosition].off; i < pIsoStru->used; i++)
            pIsoStru->databuf[i] = pIsoStru->databuf[i + pIsoStru->fld[nPosition].len];
    }
    pIsoStru->fld[nPosition].off = 0;
    pIsoStru->fld[nPosition].len = 0;

    return NORMAL;
}

/*
 * Function function: convert string to ISO structure
 * Entry parameters:
 * pcSourceStr -- The converted source string
 * nStrLen － Data length
 * Export parameters:
 * pTargetIsoStru – The result of the conversion ISO structure
 * pnPosition - Ignored when passed to NULL;
 * Otherwise, the domain number when there is a conversion error is stored (only valid when returning ERRFIELDLEN)
 * In other cases, it is -1
 * Return Value:
 * NORMAL - Conversion was successful
 * EXCEPTION - The conversion failed, pcSourceStr or TargetIsoStru is empty
 * ― An error occurred when unpacking
 */
int PubStrToIso(const ISO_FIELD_DEF Iso8583Def[],char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition)
{
	int i = 0;        // Loop sub
	int unpacked = 0; // The number of bytes in which the 8583 message has been decrypted
	int used = 0;     // The number of bytes occupied by the 8583 message in the current domain
    unsigned int fieldnum = 0;

    if (NULL == pcSourceStr || NULL == pTargetIsoStru)
    {
        return EXCEPTION;
    }

    PubDelAllBit(pTargetIsoStru);
/*
 * Get Iso8583 BITMAP domain length*8 to determine how many domains are there in total
 */
    fieldnum = Iso8583Def[1].len * 8;
    if ( fieldnum != 64 && fieldnum != 128 ) {
        return EXCEPTION;
    }
	for (i = 0; i <= fieldnum; i++)
	{

		if (i >= 2 && 0 == pTargetIsoStru->bitmap[i - 1]) continue;
		pTargetIsoStru->fld[i].off = pTargetIsoStru->used;
        used = Iso8583Def[i].unpack((ISO_FIELD_DEF*)&Iso8583Def[i], pTargetIsoStru->databuf + pTargetIsoStru->fld[i].off, &pTargetIsoStru->fld[i].len, (unsigned char*) pcSourceStr + unpacked);
		if (used <= 0 || ((unpacked + used) > nStrLen) || (pTargetIsoStru->used + pTargetIsoStru->fld[i].len) > MAX_MSG_LEN)
		{
			if (i == 44)   //If there is an error in field 44, ignore it
			{
				unpacked += 26;
				pTargetIsoStru->used += 25;
				continue;
			}
			if (NULL != pnPosition)
				*pnPosition = i;
            pTargetIsoStru->fld[i].off = 0;
            printf("unpack error");
			return EXCEPTION;
		}
		unpacked += used;
        pTargetIsoStru->used += pTargetIsoStru->fld[i].len;

        if (i == 1) // Bitmap for 2 - 128
        {
            memset(pTargetIsoStru->bitmap, 0, sizeof(pTargetIsoStru->bitmap) / sizeof(pTargetIsoStru->bitmap[0]));
            memcpy(pTargetIsoStru->bitmap, pTargetIsoStru->databuf + pTargetIsoStru->fld[i].off, pTargetIsoStru->fld[i].len);
        }
	}

    if (unpacked != nStrLen)
    {
        return EXCEPTION;
    }

    return NORMAL;
}


/*
 * Function function: convert string to ISO structure
 * Entry parameters:
 * pcSourceStr -- The converted source string
 * nStrLen － Data length
 * Export parameters:
 * pTargetIsoStru – The result of the conversion ISO structure
 * pnPosition - Ignored when passed to NULL;
 * Otherwise, the domain number when there is a conversion error is stored (only valid when returning ERRFIELDLEN)
 * In other cases, it is -1
 * Return Value:
 * NORMAL - Conversion was successful
 * EXCEPTION - The conversion failed, pcSourceStr or TargetIsoStru is empty
 * ― An error occurred when unpacking
 */
int PubStrToIso_special_ums_oda(const ISO_FIELD_DEF Iso8583Def[],char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition)
{
	int i = 0;        // Loop sub
	int unpacked = 0; // The number of bytes in which the 8583 message has been decrypted
	int used = 0;     // The number of bytes occupied by the 8583 message in the current domain
    unsigned int fieldnum = 0;

    if (NULL == pcSourceStr || NULL == pTargetIsoStru)
    {
        return EXCEPTION;
    }

    PubDelAllBit(pTargetIsoStru);
/*
 * Get Iso8583 BITMAP domain length*8 to determine how many domains are there in total
 */
    fieldnum = Iso8583Def[1].len * 8;
    if ( fieldnum != 64 && fieldnum != 128 ) {
        return EXCEPTION;
    }
	for (i = 0; i <= fieldnum; i++)
	{

		if (i >= 2 && 0 == pTargetIsoStru->bitmap[i - 1]) continue;
		pTargetIsoStru->fld[i].off = pTargetIsoStru->used;
		if (i == 62)
		{
			used = PubFPBCharUnpack_BCD((ISO_FIELD_DEF*)&Iso8583Def[i], pTargetIsoStru->databuf + pTargetIsoStru->fld[i].off, &pTargetIsoStru->fld[i].len, (unsigned char*) pcSourceStr + unpacked);
		}
		else
		{
			used = Iso8583Def[i].unpack((ISO_FIELD_DEF*)&Iso8583Def[i], pTargetIsoStru->databuf + pTargetIsoStru->fld[i].off, &pTargetIsoStru->fld[i].len, (unsigned char*) pcSourceStr + unpacked);
		}		
		if (used <= 0 || ((unpacked + used) > nStrLen) || (pTargetIsoStru->used + pTargetIsoStru->fld[i].len) > MAX_MSG_LEN)
		{
			if (i == 44)   //If there is an error in field 44, ignore it
			{
				unpacked += 26;
				pTargetIsoStru->used += 25;
				continue;
			}
			if (NULL != pnPosition)
				*pnPosition = i;
            pTargetIsoStru->fld[i].off = 0;
            printf("unpack error");
			return EXCEPTION;
		}
		unpacked += used;
        pTargetIsoStru->used += pTargetIsoStru->fld[i].len;

        if (i == 1) // Bitmap for 2 - 128
        {
            memset(pTargetIsoStru->bitmap, 0, sizeof(pTargetIsoStru->bitmap) / sizeof(pTargetIsoStru->bitmap[0]));
            memcpy(pTargetIsoStru->bitmap, pTargetIsoStru->databuf + pTargetIsoStru->fld[i].off, pTargetIsoStru->fld[i].len);
        }
	}

    if (unpacked != nStrLen)
    {
        return EXCEPTION;
    }

    return NORMAL;
}
/*
 * Function function: Convert ISO structure to unsigned string
 * Entry parameters:
 * pSourceIsoStru – The transformed source data structure
 * pnLen - The space size of the pcTargetStr (including the ending character)
 * Export parameters:
 * pcTargetStr - the converted result string (BCD code)
 * pnLen - Stores the string length after successful conversion.
 * If it fails, the length is -1.
 * Return Value:
 * NORMAL - Conversion was successful
 * EXCEPTION - The conversion failed, pcTargetStr or pSourceIsoStru is empty
 * ― Packaging failed
 */
int PubIsoToStr(const ISO_FIELD_DEF Iso8583Def[],char *pcTargetStr, ISODataStru *pSourceIsoStru, int *pnLen)
{
	int i = 0;
	int packed = 0;
	int used = 0;
    unsigned int fieldnum = 0;
	if (NULL == pcTargetStr || NULL == pSourceIsoStru)
	{
		return EXCEPTION;
	}
/*
 * Get Iso8583 BITMAP domain length*8 to determine how many domains are there in total
 */
    fieldnum = Iso8583Def[1].len * 8;
    if ( fieldnum != 64 && fieldnum!= 128 ) {
        return EXCEPTION;
    }
	if (PubSetBit(Iso8583Def,pSourceIsoStru, 1, pSourceIsoStru->bitmap, fieldnum))
	{
		return EXCEPTION;
	}
	for (i = 0; i <= fieldnum; i++)
	{
        if (i == 0 || i == 1 || pSourceIsoStru->bitmap[i - 1])
        {
            used = Iso8583Def[i].pack((ISO_FIELD_DEF*)&Iso8583Def[i], pSourceIsoStru->databuf + pSourceIsoStru->fld[i].off, pSourceIsoStru->fld[i].len, (unsigned char*) pcTargetStr + packed);
            if (used < 0) {
                if (NULL != pnLen)
                    *pnLen = -1;
                return EXCEPTION;
            }
            packed += used;
        }
    }
	if (NULL != pnLen)
		*pnLen = packed;
	return NORMAL;
}

/*
 * Function function: Extract the specified member in the structure ISO into the string szTargetStr
 * Entry parameters:
 * pSourceIsoStru ― Source data structure ISO_data structure pointer
 * nPosition -- Nth member in ISO_data structure
 * pnLen - Pass the space size (including the ending symbol) to convert the string
 * Export parameters:
 * pcTargetStr -- Target string
 * pnLen - Returns the length of the string when the conversion is successful
 * When the conversion fails and pnLen is non-empty, it is -1
 * Return Value:
 * NORMAL - Success
 * EXCEPTION - Failed, TargetIsoStru or pcSourceStr or pnLen is empty
 * ― An error occurred when calling HexToAsc
 * -- The specified member is invalid
 * -- The incoming pcSourceStr space is not enough
 */
int PubGetBit(ISODataStru *pSourceIsoStru, int nPosition, char *pcTargetStr, int *pnLen)
{
	if (NULL == pSourceIsoStru || NULL == pcTargetStr || NULL == pnLen)
	{
		return EXCEPTION;
	}

	if (nPosition > 0 && 0 == pSourceIsoStru->bitmap[nPosition - 1])
	{
		pcTargetStr[0] = '\0';
		*pnLen = 0;
		return NORMAL;
	}

	if (*pnLen >= pSourceIsoStru->fld[nPosition].len)
	{
		memcpy(pcTargetStr, pSourceIsoStru->databuf + pSourceIsoStru->fld[nPosition].off, pSourceIsoStru->fld[nPosition].len);
		if (*pnLen > pSourceIsoStru->fld[nPosition].len)
			pcTargetStr[pSourceIsoStru->fld[nPosition].len] = '\0';
		*pnLen = pSourceIsoStru->fld[nPosition].len;
		return NORMAL;
	}
	else
	{
		*pnLen = -1;
		return EXCEPTION;
	}
}

/*
 * Function function: fill the string pcSourceStr with the specified length nStrLen into the structure TargetIsoStru's specified member
 * Entry parameters:
 * nPosition -- The Nth member in the specified ISO_data structure
 * pcSourceStr - Package data
 * nStrLen -- length of pcSourceStr
 * Export parameters:
 * pTargetIsoStru – Pointer to the target structure to be filled ISO_data structure
 * Return Value:
 * NORMAL - Set successfully or nStrLen equals 0
 * EXCEPTION - Failed, TargetIsostru or pcSourceStr is NULL
 * -- The specified member is invalid
 * ― An error occurred when calling AscToHex
 * ― The cumulative length of the ISO structure exceeds MAXISOLEN
 */
int PubSetBit(const ISO_FIELD_DEF Iso8583Def[], ISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen)
{
	unsigned char buf[MAX_FIELD_LEN];
    unsigned int fieldnum = 0;

	if (nStrLen == 0)
	{
		return NORMAL;
	}
	printf("%d",nPosition);
    PubDelOneBit(pTargetIsoStru, nPosition);
/*
 * Get Iso8583 BITMAP domain length*8 to determine how many domains are there in total
 */
    fieldnum = Iso8583Def[1].len * 8;
    if ( fieldnum != 64 && fieldnum != 128 ) {
        return EXCEPTION;
    }
/*
 * Return error if greater than maximum domain
 */
	if (nPosition > fieldnum)
	{
		return EXCEPTION;
	}
	else
	{
		if(nPosition > 1)
			pTargetIsoStru->bitmap[nPosition - 1] = 1;
	}
	if (nStrLen > MAX_FIELD_LEN)
		nStrLen = MAX_FIELD_LEN;

    pTargetIsoStru->fld[nPosition].off = pTargetIsoStru->used;
	memcpy(pTargetIsoStru->databuf + pTargetIsoStru->fld[nPosition].off, pcSourceStr, nStrLen);
	pTargetIsoStru->fld[nPosition].len = nStrLen;
    if((0 >= Iso8583Def[nPosition].pack((ISO_FIELD_DEF*)&Iso8583Def[nPosition], pTargetIsoStru->databuf + pTargetIsoStru->fld[nPosition].off, pTargetIsoStru->fld[nPosition].len, buf))
       || (0 >= Iso8583Def[nPosition].unpack((ISO_FIELD_DEF*)&Iso8583Def[nPosition], pTargetIsoStru->databuf + pTargetIsoStru->fld[nPosition].off, &pTargetIsoStru->fld[nPosition].len, buf)) )
    {
        pTargetIsoStru->fld[nPosition].len = 0;
        pTargetIsoStru->fld[nPosition].off = 0;
        return EXCEPTION;
    }
    pTargetIsoStru->used += pTargetIsoStru->fld[nPosition].len;
   	if (nStrLen > MAX_FIELD_LEN)
		nStrLen = MAX_FIELD_LEN;
	return NORMAL;
}
/*
 * Function function: convert string to ISO structure
 * Entry parameters:
 * nCustNo ― Merchant number
 * pcSourceStr -- The converted source string
 * nStrLen － Data length
 * Export parameters:
 * pTargetIsoStru – The result of the conversion ISO structure
 * pnPosition - Ignored when passed to NULL;
 * Otherwise, the domain number when there is a conversion error is stored (only valid when returning ERRFIELDLEN)
 * In other cases, it is -1
 * Return Value:
 * NORMAL - Conversion was successful
 * EXCEPTION - The conversion failed, pcSourceStr or TargetIsoStru is empty
 * ― An error occurred when unpacking
 */
int PubStrToIso_separate(char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition)
{
	return PubStrToIso(Iso8583Def_union_pay, pcSourceStr, nStrLen, pTargetIsoStru, pnPosition);
}
/*
 * Function function: Convert ISO structure to unsigned string
 * Entry parameters:
 * nCustNo ― Merchant number
 * pSourceIsoStru – The transformed source data structure
 * pnLen - The space size of the pcTargetStr (including the ending character)
 * Export parameters:
 * pcTargetStr - the converted result string (BCD code)
 * pnLen - Stores the string length after successful conversion.
 * If it fails, the length is -1.
 * Return Value:
 * NORMAL - Conversion was successful
 * EXCEPTION - The conversion failed, pcTargetStr or pSourceIsoStru is empty
 * ― Packaging failed
 */
int PubIsoToStr_separate(char *pcTargetStr, ISODataStru *pSourceIsoStru, int *pnLen)
{
	return PubIsoToStr(Iso8583Def_union_pay, pcTargetStr, pSourceIsoStru, pnLen);
}
/*
 * Function function: fill the string pcSourceStr with the specified length nStrLen into the structure TargetIsoStru's specified member
 * Entry parameters:
 * nCustNo ― Merchant number
 * nPosition -- The Nth member in the specified ISO_data structure
 * pcSourceStr - Package data
 * nStrLen -- length of pcSourceStr
 * Export parameters:
 * pTargetIsoStru – Pointer to the target structure to be filled ISO_data structure
 * Return Value:
 * NORMAL - Set successfully or nStrLen equals 0
 * EXCEPTION - Failed, TargetIsostru or pcSourceStr is NULL
 * -- The specified member is invalid
 * ― An error occurred when calling AscToHex
 * ― The cumulative length of the ISO structure exceeds MAXISOLEN
 */
int PubSetBit_separate(ISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen)
{
	return PubSetBit(Iso8583Def_union_pay,pTargetIsoStru, nPosition, pcSourceStr, nStrLen);
}



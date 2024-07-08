#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "App_ISOMsg.h"
#include "App_iso8583.h"
#include "cmacro.h"

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
 * 函数功能：为一个ISO域的数据进行补齐操作
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要补齐的8583域数据指针
 *           len      ―― 将要补齐的8583域数据长度
 *           head     ―― 是否有头字符（金额类型的域有C/D标志）
 * 出口参数：poutdata ―― 用于接收补齐后的数据的缓冲区
 * 返 回 值：补齐后的字符串长度
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
 * 函数功能：根据ISO域的类型计算8583报文中的长度字节(BCD形式)
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           len      ―― 域数据的长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节, 如果是9将表示为0x09,共1个字节
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
 * 函数功能：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(BCD形式)
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：BCD码形式，如长度104，将表示为 0x01, 0x04,共2个字节, 如果是9将表示为0x09,共1个字节
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
 * 函数功能：根据ISO域的类型计算8583报文中的长度字节(ASC形式)
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           len      ―― 域数据的长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
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
 * 函数功能：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(ASC形式)
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：ASC码形式，如长度104，将表示为 0x31, 0x30, 0x34,共3个字节, 如果是9将表示为0x30, 0x39两个字节
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
 * 函数功能：根据ISO域的类型计算8583报文中的长度字节(HEX形式)
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           len      ―― 域数据的长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：HEX码形式，如长度104，将表示为 0x00, 0x68,共2个字节, 如果是9将表示为0x09,共1个字节
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
 * 函数功能：根据ISO域的类型和8583报文中的长度字节，计算报文中域的长度(HEX形式)
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含该域报文的数据指针
 * 出口参数：plen     ―― 用于接收计算后的长度
 *           pcount   ―― 长度字节数（定长为0，LLVAR型为1，LLLVAR型为2）
 * 返 回 值：无
 * 说    明：HEX码形式，如长度104，将表示为 0x00, 0x68,共2个字节, 如果是9将表示为0x09,共1个字节
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
 * 函数功能：为一个ISO域的数据进行打包（MSG_ID域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
*/
int FPAMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len = pthis->len;
	memcpy(poutdata, pfieldbuf, len);
	return len;
}

/*
 * 函数功能：为一个ISO域的数据进行解包（MSG_ID域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPAMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len = pthis->len;
	memcpy(pfieldbuf, pindata, pthis->len);
	*pfieldlen = len;
	return len;
}

/*
 * 函数功能：为一个ISO域的数据进行打包（MSG_ID域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
*/
int FPBMsgIDPack(ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len = pthis->len >> 1;
	c_asc2bcd(poutdata, (unsigned char*) pfieldbuf, pthis->len, 0);
	return len;
}

/*
 * 函数功能：为一个ISO域的数据进行解包（MSG_ID域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPBMsgIDUnpack(ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len = pthis->len >> 1;
	c_bcd2asc(pfieldbuf, pindata, pthis->len, 0);
	*pfieldlen = pthis->len;
	return len;
}

/*
 * 函数功能：为一个ISO域的数据进行打包（BITMAP位图域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
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
 * 函数功能：为一个ISO域的数据进行解包（BITMAP位图域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
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
 * 函数功能：为一个ISO域的数据进行打包（BITMAP位图域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
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
 * 函数功能：为一个ISO域的数据进行解包（BITMAP位图域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
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
 * 函数功能：为一个ISO域的数据进行打包（字符类型域ASC码形式）
 * 入口参数：mode: 变长域长度计算模式
 *           pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
*/
int FPACharPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len, count;
	unsigned char tmp[MAX_FIELD_LEN];

    len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行解包（字符类型域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPACharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int  len, count;
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行打包（字符类型域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
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

    // 对于奇数长度的，最后半个也按补齐字符压缩
    if ((len & 1) && (pthis->align == ALN_LEFT))
        tmp[len++] = pthis->padchar;

	c_asc2bcd(poutdata + count, tmp, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));

    return count + (++len >> 1);
}

/*
 * 函数功能：为一个ISO域的数据进行解包（字符类型域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPBCharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
    //debug("银联打包");
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
* 函数功能：为一个ISO域的数据进行解包（字符类型域BCD码形式）
* 入口参数：pthis    ―― ISO域定义的类型结构指针
*           pindata  ―― 含有该域报文的数据缓冲区
* 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
*           plen     ―― 返回域数据长度
* 返 回 值：解包时从pindata中使用的数据长度
*/
int FPCCharUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行打包（数字类型域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
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
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行解包（数字类型域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPADigitUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行打包（数字类型域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
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
    //debug("银联打包");
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
    // 对于奇数长度的，最后半个也按补齐字符压缩
    if ((len & 1) && (pthis->align == ALN_LEFT))
        tmp[len++] = pthis->padchar;

	c_asc2bcd(poutdata + count, tmp, len, (unsigned char) ((pthis->align == ALN_LEFT) ? 0 : 1));
	return count + (++len >> 1);
}

/*
 * 函数功能：为一个ISO域的数据进行解包（数字类型域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPBDigitUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int i, len, count;
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行打包（金额类型域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
*/
int FPBAmountPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len, count;
	unsigned char tmp[MAX_FIELD_LEN];

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 1); // len contain the char 'C/D'
    //	debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行解包（金额类型域ASC码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPBAmountUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len, count;
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行打包（二进制类型域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pfieldbuf―― 将要打包的8583域数据指针
 *           len      ―― 将要打包的8583域数据长度
 * 出口参数：poutdata ―― 用于接收计算后的数据的缓冲区
 * 返 回 值：打包后的数据长度
*/
int FPBBinaryPack(int mode,ISO_FIELD_DEF * pthis, const unsigned char *pfieldbuf, short fieldlen, unsigned char *poutdata)
{
	int len, count;
	unsigned char tmp[MAX_FIELD_LEN];

	len = AddPadChar(pthis, pfieldbuf, fieldlen, tmp, 0);
    //debug("银联打包");
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
 * 函数功能：为一个ISO域的数据进行解包（二进制类型域BCD码形式）
 * 入口参数：pthis    ―― ISO域定义的类型结构指针
 *           pindata  ―― 含有该域报文的数据缓冲区
 * 出口参数：pfieldbuf―― 解包后的8583域数据缓冲区
 *           plen     ―― 返回域数据长度
 * 返 回 值：解包时从pindata中使用的数据长度
*/
int FPBBinaryUnpack(int mode,ISO_FIELD_DEF * pthis, unsigned char *pfieldbuf, short *pfieldlen, unsigned char *pindata)
{
	int len, count;
    //debug("银联打包");
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
 * 函数功能：清空ISO_data结构中所有成员变量值
 * 入口参数：pIsoStru   ―― 源数据结构ISO_data 结构指针
 * 出口参数：pIsoStru   ―― 清空后的ISO_data结构指针
 * 返 回 值：NORMAL     ―― 成功
 *           EXCEPTION  ―― 失败，IsoStru为空
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
* 函数功能：清空IsoStru结构中第nPosition域的值，同时清除位图中的相应标志
* 入口参数：
*          IsoStru         ―― 源数据结构ISO_data 结构指针
*          nPosition       ―― ISO_data结构中的第nPosition个成员
* 出口参数：
*          IsoStru         ―― 清除后的ISO_data结构指针
* 返 回 值：
*          NORMAL          ―― 成功
*          EXCEPTION       ―― 失败，IsoStru为NULL
*                          ―― 无效的成员号（nPosition <0 或者 >=256）
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
 * 函数功能：转换字符串为ISO结构
 * 入口参数：
 *          pcSourceStr     ―― 转换的源字符串
 *          nStrLen         －－ 数据长度
 * 出口参数：
 *          pTargetIsoStru  ―― 转换的结果ISO结构
 *          pnPosition      ―― 传入为NULL时，忽略;
 * 		                         否则存放转换出错时的域编号（仅对返回ERRFIELDLEN时有效）
 *                               其余情况下均为-1
 * 返 回 值：
 *          NORMAL          ―― 转换成功
 *        	EXCEPTION       ―― 转换失败，pcSourceStr或者TargetIsoStru为空
 *         	                ―― 解包出错
 */
int PubStrToIso(const ISO_FIELD_DEF Iso8583Def[],char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition)
{
	int i = 0;        // 循环子
	int unpacked = 0; // 8583报文已经被解开的字节数
	int used = 0;     // 当前域在8583报文占用的字节数
    unsigned int fieldnum = 0;

    if (NULL == pcSourceStr || NULL == pTargetIsoStru)
    {
        return EXCEPTION;
    }

    PubDelAllBit(pTargetIsoStru);
    /* 获取Iso8583 BITMAP域长度*8 判断总共有几个域 */
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
			if (i == 44)   //如果44域出错则忽略
			{
				unpacked += 26;
				pTargetIsoStru->used += 25;
				continue;
			}
			if (NULL != pnPosition)
				*pnPosition = i;
            pTargetIsoStru->fld[i].off = 0;
            printf("报文解析错误");
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
 * 函数功能：转换字符串为ISO结构
 * 入口参数：
 *          pcSourceStr     ―― 转换的源字符串
 *          nStrLen         －－ 数据长度
 * 出口参数：
 *          pTargetIsoStru  ―― 转换的结果ISO结构
 *          pnPosition      ―― 传入为NULL时，忽略;
 * 		                         否则存放转换出错时的域编号（仅对返回ERRFIELDLEN时有效）
 *                               其余情况下均为-1
 * 返 回 值：
 *          NORMAL          ―― 转换成功
 *        	EXCEPTION       ―― 转换失败，pcSourceStr或者TargetIsoStru为空
 *         	                ―― 解包出错
 */
int PubStrToIso_special_ums_oda(const ISO_FIELD_DEF Iso8583Def[],char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition)
{
	int i = 0;        // 循环子
	int unpacked = 0; // 8583报文已经被解开的字节数
	int used = 0;     // 当前域在8583报文占用的字节数
    unsigned int fieldnum = 0;

    if (NULL == pcSourceStr || NULL == pTargetIsoStru)
    {
        return EXCEPTION;
    }

    PubDelAllBit(pTargetIsoStru);
    /* 获取Iso8583 BITMAP域长度*8 判断总共有几个域 */
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
			if (i == 44)   //如果44域出错则忽略
			{
				unpacked += 26;
				pTargetIsoStru->used += 25;
				continue;
			}
			if (NULL != pnPosition)
				*pnPosition = i;
            pTargetIsoStru->fld[i].off = 0;
            printf("报文解析错误");
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
 * 函数功能：将ISO结构转换成无符号字符串
 * 入口参数：
 *          pSourceIsoStru  ―― 转换的源数据结构
 *          pnLen           ―― 传入pcTargetStr的空间大小（包含结束符）
 * 出口参数：
 *          pcTargetStr     ―― 转换的结果字符串(BCD码)
 *          pnLen           ―― 存放转换成功后的字符串长度。
 *                               如果失败，长度为-1。
 * 返 回 值：
 *          NORMAL          ―― 转换成功
 *          EXCEPTION       ―― 转换失败，pcTargetStr或者pSourceIsoStru为空
 *                          ―― 打包失败
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
    /* 获取Iso8583 BITMAP域长度*8 判断总共有几个域 */
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
 * 函数功能：将结构ISO中指定成员提取到字符串szTargetStr中
 * 入口参数：
 *          pSourceIsoStru   ―― 源数据结构ISO_data 结构指针
 * 		    nPosition       ―― ISO_data结构中的第N个成员
 *          pnLen           ―― 传入要转换字串的空间大小（包含结尾符号）
 * 出口参数：
 *          pcTargetStr     ―― 目标字符串
 *          pnLen           ―― 返回转换成功时字符串的长度
 *                               转换失败且pnLen非空时为-1
 * 返 回 值：
 *          NORMAL          ―― 成功
 *          EXCEPTION       ―― 失败,TargetIsoStru或者pcSourceStr或者pnLen为空
 *                          ―― 调用HexToAsc出错
 *                          ―― 指定的成员无效
 *                          ―― 传入的pcSourceStr空间不够
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
 * 函数功能：将字符串pcSourceStr按指定长度nStrLen填充到结构TargetIsoStru中指定成员
 * 入口参数：
 *          nPosition       ―― 指定的ISO_data结构中的第N个成员
 * 		    pcSourceStr     ―― 打包数据
 *          nStrLen         ―― pcSourceStr 的长度
 * 出口参数：
 *          pTargetIsoStru  ―― 待填充的目标结构ISO_data 结构的指针
 * 返 回 值：
 *          NORMAL          ―― 设置成功或者nStrLen 等于0
 *          EXCEPTION       ―― 失败，TargetIsostru或者pcSourceStr为NULL
 *                          ―― 指定的成员无效
 *                          ―― 调用AscToHex出错
 *                          ―― ISO结构累计长度超过MAXISOLEN
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
    /* 获取Iso8583 BITMAP域长度*8 判断总共有几个域 */
    fieldnum = Iso8583Def[1].len * 8;
    if ( fieldnum != 64 && fieldnum != 128 ) {
        return EXCEPTION;
    }
    /* 大于最大域返回错误 */
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
 * 函数功能：转换字符串为ISO结构
 * 入口参数：
 			nCustNo			―― 商户编号
 *          pcSourceStr     ―― 转换的源字符串
 *          nStrLen         －－ 数据长度
 * 出口参数：
 *          pTargetIsoStru  ―― 转换的结果ISO结构
 *          pnPosition      ―― 传入为NULL时，忽略;
 * 		                         否则存放转换出错时的域编号（仅对返回ERRFIELDLEN时有效）
 *                               其余情况下均为-1
 * 返 回 值：
 *          NORMAL          ―― 转换成功
 *        	EXCEPTION       ―― 转换失败，pcSourceStr或者TargetIsoStru为空
 *         	                ―― 解包出错
 */
int PubStrToIso_separate(char *pcSourceStr, int nStrLen, ISODataStru *pTargetIsoStru, int *pnPosition)
{
	return PubStrToIso(Iso8583Def_union_pay, pcSourceStr, nStrLen, pTargetIsoStru, pnPosition);
}
/*
 * 函数功能：将ISO结构转换成无符号字符串
 * 入口参数：
			nCustNo			―― 商户编号
 *          pSourceIsoStru  ―― 转换的源数据结构
 *          pnLen           ―― 传入pcTargetStr的空间大小（包含结束符）
 * 出口参数：
 *          pcTargetStr     ―― 转换的结果字符串(BCD码)
 *          pnLen           ―― 存放转换成功后的字符串长度。
 *                               如果失败，长度为-1。
 * 返 回 值：
 *          NORMAL          ―― 转换成功
 *          EXCEPTION       ―― 转换失败，pcTargetStr或者pSourceIsoStru为空
 *                          ―― 打包失败
 */
int PubIsoToStr_separate(char *pcTargetStr, ISODataStru *pSourceIsoStru, int *pnLen)
{
	return PubIsoToStr(Iso8583Def_union_pay, pcTargetStr, pSourceIsoStru, pnLen);
}
/*
 * 函数功能：将字符串pcSourceStr按指定长度nStrLen填充到结构TargetIsoStru中指定成员
 * 入口参数：
 			nCustNo			―― 商户编号
 *          nPosition       ―― 指定的ISO_data结构中的第N个成员
 * 		    pcSourceStr     ―― 打包数据
 *          nStrLen         ―― pcSourceStr 的长度
 * 出口参数：
 *          pTargetIsoStru  ―― 待填充的目标结构ISO_data 结构的指针
 * 返 回 值：
 *          NORMAL          ―― 设置成功或者nStrLen 等于0
 *          EXCEPTION       ―― 失败，TargetIsostru或者pcSourceStr为NULL
 *                          ―― 指定的成员无效
 *                          ―― 调用AscToHex出错
 *                          ―― ISO结构累计长度超过MAXISOLEN
 */
int PubSetBit_separate(ISODataStru *pTargetIsoStru, int nPosition, char *pcSourceStr, int nStrLen)
{
	return PubSetBit(Iso8583Def_union_pay,pTargetIsoStru, nPosition, pcSourceStr, nStrLen);
}



#include "function.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NORMAL    0x00 
#define EXCEPTION  0x01 

/* 
 * Function: msb_byte4_to_uint32
 * Description:  4 bytes to int (Big-Endian)  
 * Example:     "\x30\x31\x32\x33" -> 0x30313233
 * Author: none @ 10/16/2018 9:01:33 AM 
 * return: 
 */
int msb_byte4_to_uint32 (const unsigned char byte[4])
{
    unsigned int tmp = 0;

	tmp = byte[0];
	tmp <<= 8;
	tmp += byte[1];
	tmp <<= 8;
	tmp += byte[2];
	tmp <<= 8;
	tmp += byte[3];

	return tmp;
}		/* -----  end of function msb_byte4_to_uint32  ----- */


/* 
 * Function: lsb_byte4_to_uint32
 * Description:  4 bytes to int (Little-Endian)  
 * Example:     "\x30\x31\x32\x33" -> 0x33323130
 * Author: none @ 10/16/2018 9:04:44 AM 
 * return: 
 */
int lsb_byte4_to_uint32 (const unsigned char byte[4])
{
    unsigned int tmp = 0;

	tmp = byte[3];
	tmp <<= 8;
	tmp += byte[2];
	tmp <<= 8;
	tmp += byte[1];
	tmp <<= 8;
	tmp += byte[0];

	return tmp;
}		/* -----  end of function lsb_byte4_to_uint32  ----- */


/* 
 * Function: msb_byte2_to_uint16
 * Description:  2 bytes to unsigned short(Big-Endian)  
 * Example:     "\x30\x31" -> 0x3031
 * Author: none @ 10/16/2018 9:13:55 AM 
 * return:
 */
unsigned short msb_byte2_to_uint16 (const unsigned char byte[2])
{
	return ((byte[0]<<8)+byte[1]);
}		/* -----  end of function msb_byte2_to_uint16  ----- */


/* 
 * Function: lsb_byte2_to_uint16
 * Description:  2 bytes to unsigned short(Little-Endian)  
 * Example:     "\x30\x31" -> 0x3130
 * Author: none @ 10/16/2018 9:07:25 AM 
 * return: 
 */
unsigned short lsb_byte2_to_uint16 (const unsigned char byte[2])
{
	return ((byte[1]<<8)+byte[0]);
}		/* -----  end of function lsb_byte2_to_uint16  ----- */


/* 
 * Function: msb_uint32_to_byte4
 * Description: unsigned int to byte4 (Big-Endian)
 * Example:     0x30313233 -> "\x30\x31\x32\x33"
 * Author: none @ 10/16/2018 9:18:07 AM 
 * return:
 */
void msb_uint32_to_byte4 (unsigned int byte4,unsigned char byte[4])
{
    byte[3] = (unsigned char)byte4;
    byte4 >>= 8;
    byte[2] = (unsigned char)byte4;
    byte4 >>= 8;
    byte[1] = (unsigned char)byte4;
    byte4 >>= 8;
    byte[0] = (unsigned char)byte4;
}		/* -----  end of function msb_uint32_to_byte4  ----- */


/* 
 * Function: lsb_uint32_to_byte4
 * Description: unsigned int to byte4 (Little-Endian)
 * Example:     0x30313233 -> "\x33\x32\x31\x30"
 * Author: none @ 10/16/2018 9:20:35 AM 
 * return:
 */
void lsb_uint32_to_byte4 (unsigned int byte4,unsigned char byte[4])
{
    byte[0] = (unsigned char)byte4;
    byte4 >>= 8;
    byte[1] = (unsigned char)byte4;
    byte4 >>= 8;
    byte[2] = (unsigned char)byte4;
    byte4 >>= 8;
    byte[3] = (unsigned char)byte4;
}		/* -----  end of function lsb_uint32_to_byte4  ----- */


/* 
 * Function: msb_uint16_to_byte2
 * Description:  unsigned short to byte2(Big-Endian) 
 * Example:      0x3031 -> "\x30\x31"
 * Author: none @ 10/16/2018 9:22:02 AM 
 * return:
 */
void msb_uint16_to_byte2 (unsigned short byte2,unsigned char byte[2])
{
    byte[1] = (unsigned char)byte2;
    byte2 >>= 8;
    byte[0] = (unsigned char)byte2;
}		/* -----  end of function msb_uint16_to_byte2  ----- */


/* 
 * Function: lsb_uint16_to_byte2
 * Description:  unsigned short to byte2(Little-Endian) 
 * Example:      0x3031 -> "\x31\x30"
 * Author: none @ 10/16/2018 9:24:02 AM 
 * return:
 */
void lsb_uint16_to_byte2 (unsigned short byte2,unsigned char byte[2])
{
    byte[0] = (unsigned char)byte2;
    byte2 >>= 8;
    byte[1] = (unsigned char)byte2;
}		/* -----  end of function lsb_uint16_to_byte2  ----- */



/* 
 * Function: c_asc2hex
 * Description: 2字节ASC字符转1字节16进制 
 * Example:     '3''4' -> 0x34
 * Author: none @ 10/16/2018 9:27:13 AM 
 * return:
 */
int c_asc2hex (unsigned char *in, unsigned char *out)
{
	unsigned char tmp;
    if ( in == NULL || out == NULL ) {
        return 1;
    }
	if (in[0] >= '0' && in[0] <= '9')
	{
		tmp = (unsigned char)((in[0] - '0') << 4);
	}
	else if (in[0] >= 'a' && in[0] <= 'f')
	{
		tmp = (unsigned char)((in[0] - 'a' + 10) << 4);
	}
	else if (in[0] >= 'A' && in[0] <= 'F')
	{
		tmp = (unsigned char)((in[0] - 'A' + 10) << 4);
	}
	else
	{
        return 1;
	}
	if (in[1] >= '0' && in[1] <= '9')
	{
		tmp |= (unsigned char)(in[1] - '0');
	}
	else if (in[1] >= 'a' && in[1] <= 'f')
	{
		tmp |= ((in[1] - 'a') + 10);
	}
	else if (in[1] >= 'A' && in[1] <= 'F')
	{
		tmp |= ((in[1] - 'A') + 10);
	}
	else
	{
        return 1;
	}
    *out = tmp;
	return 0;
}		/* -----  end of function c_asc2hex  ----- */


/* 
 * Function: c_hex2asc
 * Description:  1字节16进制 转2字节ASC字符
 * Example:      0x34 -> '3''4'
 * Author: none @ 10/16/2018 9:31:23 AM 
 * return:
 */
int c_hex2asc (unsigned char in,unsigned char *out)
{
	unsigned char tmp;
    if ( out == NULL ) {
        return 1;
    }
	tmp = (in>>4);
	if (tmp >= 10)
	{
		tmp += ('A'-10);
	}
	else
	{
		tmp += '0';
	}
	out[0] = tmp;
	tmp = (in&0x0F);
	if (tmp >= 10)
	{
		tmp += ('A'-10);
	}
	else
	{
		tmp += '0';
	}
	out[1] = tmp;
    return 0;
}		/* -----  end of function c_hex2asc  ----- */

/* 
 * Function: c_str2hex
 * Description:  字符串转16进制数组
 * Example: "123456" -> 0x12 0x34 0x56
 * Author: none @ 10/16/2018 9:35:28 AM 
 * return:
 */
int c_str2hex (unsigned int inlen,const void *inbuf, void *outbuf)
{
	unsigned int i;
    unsigned char *in=(unsigned char *)inbuf;
    unsigned char *out=(unsigned char *)outbuf;

    if ( inlen%2 || inbuf == NULL || outbuf == NULL ) {
        return 1;
    }
	for (i=0; i<inlen/2; i++)
	{
        if (c_asc2hex(&in[2*i],&out[i])) {
            return 1;
        }
    }
    return 0;
}		/* -----  end of function c_str2hex  ----- */

/* 
 * Function: c_hex2str
 * Description:  16进制转字符串
 * Example: 0x31 0x32 0x33 -> "123"
 * Author: none @ 10/16/2018 9:44:52 AM 
 * return:
 */
int c_hex2str (unsigned int inlen, const void *inbuf,void *outbuf)
{
	unsigned int i;
    if ( inlen == 0 || inbuf == NULL || outbuf == NULL ) {
        return 1;
    }
    unsigned char *in=(unsigned char *)inbuf;
    unsigned char *out=(unsigned char *)outbuf;
	for (i=0; i<inlen; i++)
	{
        c_hex2asc(in[i],&out[2*i]);
    }
	return 0;
}		/* -----  end of function c_hex2str  ----- */



/* 
 * function: c_asc2bcd
 * description:   
 * author: 
 * return:
 */
void c_asc2bcd (uchar* bcd_buf, uchar* ascii_buf, int conv_len, uchar type)
{
	int     cnt;
	char    ch, ch1;

	if (conv_len & 0x01 && type)
		ch1 = 0;
	else
		ch1 = 0x55;

	for (cnt = 0; cnt < conv_len; ascii_buf++, cnt++)
	{
		if (*ascii_buf >= 'a')
			ch = *ascii_buf - 'a' + 10;
		else if (*ascii_buf >= 'A')
			ch = *ascii_buf - 'A' + 10;
		else if (*ascii_buf >= '0')
			ch = *ascii_buf - '0';
		else
			ch = 0;
		if (ch1 == 0x55)
			ch1 = ch;
		else
		{
			*bcd_buf++ = (ch1 << 4) | ch;
			ch1 = 0x55;
		}
	}

	if (ch1 != 0x55)
		*bcd_buf = ch1 << 4;

	return;
}		/* -----  end of function c_asc2bcd  ----- */

/* 
 * function: c_bcd2asc
 * description:  实现bcd码转asc 
 * author: 
 * return:
 */

void c_bcd2asc(uchar* ascii_buf, uchar* bcd_buf, int conv_len, uchar type)
{
	int cnt;

	if (conv_len & 0x01 && type)
	{
		cnt = 1;
		conv_len++;
	}
	else
		cnt = 0;
	for (; cnt < conv_len; cnt++, ascii_buf++)
	{
		*ascii_buf = ((cnt & 0x01) ? (*bcd_buf++ &0x0f) : (*bcd_buf >> 4));
		*ascii_buf += ((*ascii_buf > 9) ? ('A' - 10) : '0');
	}

	return;
}		/* -----  end of function c_bcd2asc  ----- */

/* 
 * Function: c_compress_str
 * Description:  压缩  2byte->1bytes
 * Example:  0x31 0x3A  -> 0x1A
 * Author: none @ 10/16/2018 9:48:59 AM 
 * return: 0-succ  1-fail(小于0x30或大于0x3F)
 */
int c_compress_str (unsigned int inlen,const void *inbuf,void *outbuf)
{
	unsigned int i;
    unsigned char high,low;
    unsigned char *in=(unsigned char *)inbuf;
    unsigned char *out=(unsigned char *)outbuf;
    if ( inlen%2 || inbuf == NULL || outbuf == NULL ) {
        return 1;
    }
    for ( i=0 ; i<inlen ; i++  ) {
        high = in[2*i];
        low = in[2*i+1];
        if ( (high >= 0x30 && high <= 0x3F)
             && (low >= 0x30 && low <= 0x3F)  ) {
              out[i] = (unsigned char)(high<<4) | (unsigned char)(low & 0x0F);
        } else {
            return 1;
        }
    }
    return 0;
}		/* -----  end of function c_compress_str  ----- */


/* 
 * Function: c_decompress_str
 * Description:  解压缩 1byte->2bytes 
 * Example: 0x1A ->  0x31 0x3A
 * Author: none @ 10/16/2018 9:52:32 AM 
 * return:
 */
int c_decompress_str(unsigned int inlen, const void *inbuf,void *outbuf)
{
	unsigned int i;
    if ( inlen == 0 || inbuf == NULL || outbuf == NULL ) {
        return 1;
    }
    unsigned char *in=(unsigned char *)inbuf;
    unsigned char *out=(unsigned char *)outbuf;
    for ( i=0 ; i<inlen ; i++  ) {
        out[2*i] = (in[i]>>4)+'0';
        out[2*i+1] = (in[i]&0x0F)+'0';
    }
    return 0;
}		/* -----  end of function c_decompress_str  ----- */


/* 
 * Function:c_lower2upper  
 * Description:  转大写 
 * Example: "123aBc" -> "123ABC"
 * Author: none @ 10/16/2018 3:38:12 PM 
 * return:
 */
char *c_lower2upper(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
    {
        if ( (*str >= 'a') && (*str <= 'z') ) {
            *str = *str + ('A' - 'a');
        } 
    }
    return orign;
}		/* -----  end of function c_lower2upper   ----- */


/* 
 * Function:c_upper2lower 
 * Description: 转小写 
 * Example: "123aBc" -> "123abc"
 * Author: none @ 10/16/2018 3:39:49 PM 
 * return:
 */
char *c_upper2lower(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
    {
        if ( (*str >= 'A') && (*str <= 'Z') ) {
            *str = *str + ('a' - 'A');
        } 
    }
    return orign;
}		/* -----  end of function  c_upper2lower  ----- */


/* 
 * Function: c_makelrc
 * Description:  calc lrc 
 * Author: none @ 10/16/2018 10:11:07 AM 
 * In: start：初始值
 * return: 1byte lrc
 */
unsigned char c_makelrc(unsigned char start, unsigned int inlen, const void *inbuf)
{
	unsigned int i;
    unsigned char lrc=start;
    unsigned char *in=(unsigned char *)inbuf;
	for (i=0; i<inlen; i++)
	{
		lrc ^= in[i];
	}
	return lrc;
}

/* 
 * Function: c_checklrc
 * Description:  check lrc 
 * Author: none @ 10/16/2018 10:09:27 AM 
 * In: 初始值:  check:需要校验的值
 * return: 0-succ  1-fail
 */
int c_checklrc(uint8_t start, uint32_t inlen, const void *in, uint8_t check)
{
    if ( c_makelrc(start,inlen,in) == check ) {
        return 0;
    } else {
        return 1;
    }
}

/* 
 * Function: c_make4lrc
 * Description:  calc 4lrc 
 * Author: none @ 10/16/2018 10:14:11 AM 
 * In: start:初始值  
 * return: unsigned int lrc
 */
unsigned int c_make4lrc(unsigned int start, unsigned int inlen, const void *inbuf)
{
    unsigned int *in = (unsigned int *)inbuf;
    unsigned int i,lrc=start;
	for (i=0; i<inlen/4; i++)
	{
		lrc ^= *in++;
	}
	return lrc;
}		/* -----  end of function c_make4lrc  ----- */


/* 
 * Function: c_check4lrc
 * Description:  check 4byte lrc 
 * Author: none @ 10/16/2018 10:17:08 AM 
 * In: start:初始值 check:需要比对的值 
 * return: 0-succ 1-fail
 */
int c_check4lrc(unsigned int start, unsigned int inlen, const void *inbuf, unsigned int check)
{
    if ( c_make4lrc(start,inlen,inbuf) == check ) {
        return 0;
    } else {
        return 1;
    }
}		/* -----  end of function c_check4lrc  ----- */


/* 
 * Function: c_buf8xor
 * Description:  inlen长度的数组每8字节异或，长度不足8倍数补0异或 
 * Author: none @ 10/16/2018 10:19:43 AM 
 * return:
 */
int c_buf8xor(unsigned int inlen, const void *inbuf, void *outbuf)
{
	unsigned int i=0;
	unsigned int j,m,n;
	unsigned char ucIBuf[8];
    unsigned char *in=(unsigned char *)inbuf;
	memset(ucIBuf,0,sizeof(ucIBuf));
	m = inlen/8;
	n = inlen%8;
	while(m--)
	{
		for(j=0;j<8;j++)
		{
			ucIBuf[j] ^= in[i+j];
		}
		i += 8;
	}
	if(n)
	{
		for(j=0;j<n;j++)
		{
			ucIBuf[j] ^= in[i+j];
		}
	}
	memcpy((unsigned char *)outbuf,ucIBuf,8);
    return 0;
}		/* -----  end of function c_buf8xor  ----- */

/* 
 * Function: c_bufxor
 * Description:  2个inlen长度的数组进行异或 
 * Author: none @ 10/16/2018 10:21:11 AM 
 * return:
 */
int c_bufxor(unsigned int len, const void *inbuf1, const void *inbuf2, void *outbuf)
{
	unsigned int i;
    unsigned char *str1=(unsigned char *)inbuf1;
    unsigned char *str2=(unsigned char *)inbuf2;
    unsigned char *out=(unsigned char *)outbuf;
	for(i=0;i<len;i++)
	{
		out[i] = str1[i]^str2[i];
	}
    return 0;
}		/* -----  end of function c_bufxor  ----- */


/* 
 * Function: c_make_crc16_ccitt xmodem
 * Description:  crc16 
 * Author: none @ 10/16/2018 10:39:00 AM 
 * return: 0-succ  1-fail
 */
int c_make_crc16_ccitt (unsigned int inlen, unsigned char *inbuf,unsigned char *crc16)
{
	unsigned int crc= 0;
	unsigned char temp;
    unsigned int table[16]={ /* CRC ??????????*/
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
    };
    if ( inbuf == NULL || crc16 == NULL || inlen == 0 ) {
        return 1;
    }
	while (inlen -- != 0)
	{
		temp = ((unsigned char)( crc>>8))>>4;
		crc <<= 4;
		crc ^= table[temp^(*inbuf/16)];
		temp = ((unsigned char)( crc>>8))>>4;
		crc <<= 4;
		crc ^= table[temp^(*inbuf&0x0f)];
		inbuf++;
	}
	crc16[0] = crc/256;
	crc16[1] = crc%256;
    return 0;
}		/* -----  end of function c_make_crc16_ccitt  ----- */

/* 
 * function: c_get_visible_char
 * description:  提取字段前面的可见字符 
 * author: 
 * return:
 */
int c_get_visible_char (int len, char *in, char *out)
{
    int i = 0;
    for (i=0; i<len; i++)
    {
        if (!( (in[i] >= '0' && in[i] <= '9') 
               ||(in[i] >= 'a' && in[i] <= 'z')
               ||(in[i] >= 'A' && in[i] <= 'Z')
             ) ) {
            break;
        }
        out[i] = in[i];
    }
    return i; 
}		/* -----  end of function c_get_visible_char  ----- */


void c_asc2dcb(char *data, int len)
{
	int i;
	char tmp;
	for(i=0; i<len; i++)
	{
		tmp = ((data[i]&0xf0)>>4)*10;
		tmp += (data[i]&0x0f);
		data[i] = tmp;
	}
}
/*********************************************************************************
* 功能描述:
		字符串对比
		* 输入参数:对比的字符串
		* 返回值：	0：相同
		*    	 其他：不同
*********************************************************************************/
int PubStrcmp(const char *s1,const char* s2)
{
	while(*s1   &&   *s1==*s2)
	{
		s1++;
		s2++;
	}
	return *s1 - *s2;
}
/*********************************************************************************
* 功能描述:
		字符串拷贝
* 输入参数:
* 输出参数：
* 返回值：	0：成功
*********************************************************************************/
int PubStrcpy(char *s1,const char* s2)
{
	while(*s2)
	{
		*s1++ = *s2++;
	}
	*s1 = 0x00;
	return 0;
}


/* 
 * function: c_string_to_ip
 * description:  字符串IP转4个char型 
 * author: 
 * return:
 */
int c_string_to_ip (uchar *string, uchar *ip)
{
    int len = 0;
    char buf[4][8];
    int  j = 0;
    int  i = 0;
    int  k = 0;
    if ( string == NULL || ip == NULL || len > 16 ) {
        return -1;
    }
    len = strlen((char *)string);
    memset(buf,0,4*8);
    for ( i=0 ; i<len ; i++ ) {
        if ( string[i] == '.' ) {
            j++;
            k = 0;
            continue;
        }
        if ( j > 3 ) {
            return -1;
        }
        buf[j][k++] = string[i];
    }
    ip[0] = atoi(buf[0])&0xFF;
    ip[1] = atoi(buf[1])&0xFF;
    ip[2] = atoi(buf[2])&0xFF;
    ip[3] = atoi(buf[3])&0xFF;
    return 0;
}		/* -----  end of function c_string_to_ip   ----- */


/*
 * 函数功能：将ASCII字符串转换成十六进制数据，转换后的十六进制数据长度为ASCII
 * 字符串的一半，ASCII长度为奇数的，转换后，右补零。
 * 入口参数：upcAscBuf  ―― 要转换的ASCII字符串
 *           nConvLen   ―― 转换的字符串长度
 *           ucType     ―― 转换后的类型
 * 出口参数：upcHexBuf  ―― 转换后的十六进制数据
 * 返 回 值：NORMAL     ―― 转换成功
 *           EXCEPTION  ―― 失败，pcHexBuf或者pcAscBuf为NULL
 *           ERRSIZE    ―― 不合法的转换范围
 *           ERRCHAR    ―― 存在非法字符
 */
int  PubAscToHex(uchar *upcHexBuf, uchar *upcAscBuf,
                 int nConvLen, uchar ucType)
{
    int nCnt;
    char cChar, cChar1;

    if (upcHexBuf == NULL || upcAscBuf == NULL)
    {
        ;
        return EXCEPTION;
    }

    if (nConvLen < 0)
    {
        ;
        return EXCEPTION;
    }
    else if (nConvLen == 0)
    {
        upcHexBuf[0] = 0;
        return NORMAL;
    }

    if (nConvLen & 0x01 && ucType)
        cChar1 = 0;
    else
        cChar1 = 0x55;

    for (nCnt = 0; nCnt < nConvLen; upcAscBuf++, nCnt++)
    {
        if (*upcAscBuf >= 'a')
            cChar = *upcAscBuf - 'a' + 10;
        else if (*upcAscBuf >= 'A')
            cChar = *upcAscBuf - 'A' + 10;
        else if (*upcAscBuf >= '0')
            cChar = *upcAscBuf - '0';
        else
            cChar = 0;

        if (cChar1 == 0x55)
            cChar1 = cChar;
        else
        {
            *upcHexBuf++ = (cChar1 << 4) | cChar;
            cChar1 = 0x55;
        }
    }

    if (cChar1 != 0x55)
        *upcHexBuf = cChar1 << 4;

    return NORMAL;
}

/*
 * 函数功能：将十六进制数据转换成ASCII字符串。
 * 入口参数：upcHexBuf ―― 要转换的十六进制数据
 *           nConvLen ―― 转换的字符串长度
 *           ucType ―― 要转换的字串类型
 * 出口参数：upcAscBuf ―― 转换后的ASCII字符串
 * 返 回 值：NORMAL ―― 转换成功
 *           EXCEPTION ―― 失败，pcHexBuf或者pcAscBuf为空
 *           ERRSIZE － 错误的转换长度
 */
int  PubHexToAsc(uchar *upcAscBuf, uchar *upcHexBuf,
                 int nConvLen, uchar ucType)
{
    int nCnt;

    if (upcAscBuf == NULL || upcHexBuf == NULL)
    {
        ;
        return EXCEPTION;
    }

    if (nConvLen < 0)
    {
        ;
        return EXCEPTION;
    }
    else if (nConvLen == 0)
    {
        upcAscBuf[0] = 0;
        return NORMAL;
    }

    if (nConvLen & 0x01 && ucType)
    {
        nCnt = 1;
        nConvLen ++;
    }
    else
        nCnt = 0;
    for (; nCnt < nConvLen; nCnt ++, upcAscBuf ++)
    {
        *upcAscBuf = ((nCnt & 0x01) ? (*upcHexBuf++ & 0x0f) : (*upcHexBuf >> 4));
        *upcAscBuf += ((*upcAscBuf > 9) ? ('A' - 10) : '0');
    }

    return NORMAL;
}


/*****************************************************
*函数名:		General_memcpy
*函数说明:		与memcpy用法一样
*入口参数:
*入口参数说明:
*出口参数:
*出口参数说明:
*返回值:	cpy_len : 拷贝的长度
*返回值说明:
*****************************************************/
unsigned short General_memcpy(unsigned char *string1, unsigned char *string2, unsigned short len)
{
	unsigned short cpy_len;

	if(len == 0)
	{
		len = strlen((char *)string2);
	}

	if(!len)
	{
		return 0;
	}
	cpy_len = len;

	while(len--)
	{
		*string1++ = *string2++;
	}
	return cpy_len;
}
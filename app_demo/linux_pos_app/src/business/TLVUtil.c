/*
 * TLVUtil.c
 *
 *  Created on: Oct 23, 2014
 *      Author: yanbin
 */

#include "TLVUtil.h"
#include <stdio.h>
#include <string.h>

static PR_INT32 _CetTLV(unsigned short tag, char *data, PR_INT32 *datalen, char *field55, short field55_len);
static PR_INT32 decode_tlv_recur(PR_INT32 parent, PR_TLV_T *pobj, PR_INT32 objspace, PR_INT32 deflag);
static void 	uint_2_c4(PR_UINT32 num, PR_INT8 * c4);

#define is_constructed(byte)        ((byte) & 0x20)
#define is_primitive(byte)          !is_constructed(byte)
#define has_subsequent(byte)        (((byte) & 0x1F) == 0x1F)
#define another_byte_follow(byte)   ((byte) & 0x80)
#define lenlen_exceed_one(byte)     ((byte) & 0x80)


PR_INT32 PR_nTlvUtilGetValueString(PR_INT8* pszStream,
								   PR_INT32 nLenOfStream ,
								   PR_INT16 nTLVTag,
								   PR_INT32 nLenOfBuffer,
								   PR_INT8* pcValue)
{
	PR_INT32 nLenOfRead = 0;
	if(-1==_CetTLV(nTLVTag, pcValue, &nLenOfRead, pszStream,nLenOfStream))
		return PR_FAILD;

	return nLenOfStream;
}
/**
* @fn int fetch_tlv(int parent, unsigned int tagname, tlv_t *pobj, int level)
* @ingroup	emvcore
* @li 在tlv_t pobj结构体中获取tagname的TLV数据

* @param[in]	int parent 		从这个父结点开始搜索
* @param[in]	unsigned int tagname 	要查找的标签名
* @param[in]	tlv_t *pobj			要搜索的tlv_t结构体
* @param[in]	int level				搜索的层度
				SEARCH_ONLY_SON		只在自己子结点查找
				SEARCH_ALL_DESC		在所有后续结点查找

* @retval	<=0	失败
			>0	成功

* @sa 	无

* @attention
  	@li	<b>\<调用\></b>
  	@li	<b>\<设计\></b>

*/
PR_INT32 PR_nTlvUtilFetch(PR_INT32 parent, PR_INT32 tagname, PR_TLV_T *pobj, PR_INT32 level)
{
	PR_INT32 i = (parent < 0 ? 0 : parent) + 1;
	PR_INT32 end ;

	/* if no objects or parent referred to a primitive element */
	//TLV对象个数有效且输入指针有效
	if (pobj == NULL || pobj[i - 1].childnum < 0)
	{
		return -1;
	}
	//计算扫描次数
	end = pobj[i - 1].childnum + i;
	//执行扫描
	while (i < end)
	{
		//找到则返回偏移地址
		if (pobj[i].tagname == tagname)
			return i;
		if (level == PR_SEARCH_ONLY_SON)
		{
			/*跳过子结点的子结点*/
			if (pobj[i].childnum > 0)
			{
				i += pobj[i].childnum;
			}
		}
		i ++;
	}
	return 0;
}

PR_INT32 PR_nTlvUtilPack(PR_UINT32 nTagName, PR_UINT32 nLength, PR_INT8 *pcValue, PR_INT8* pcOutputBuffer)
{
  PR_INT32 totalLength = 0;
  if (nTagName > 0xFF)
  {
    pcOutputBuffer[totalLength++] = (nTagName >> 8) & 0xff;
    pcOutputBuffer[totalLength++] = nTagName & 0xff;
  }
  else
  {
    pcOutputBuffer[totalLength++] = nTagName;
  }

  pcOutputBuffer[totalLength++] = nLength & 0xff;
  memcpy(pcOutputBuffer+totalLength, pcValue, nLength);
  totalLength += nLength;

  return totalLength;
}


static void uint_2_c4(PR_UINT32 num, PR_INT8 * c4)
{
	*c4 = (num >> 24);
	*(c4 + 1) = ((num >> 16) & 0x00FF);
	*(c4 + 2) = ((num >> 8) & 0x00FF);
	*(c4 + 3) = (num & 0x00FF);
}


PR_INT32 PR_nTlvUtilPackOne(PR_TLV_T *ptTLV, PR_INT32 nCount, PR_INT8* pcOutputBuffer, PR_INT32 nLenOfOutputBuffer)
{
	PR_INT32 i, reallen = 0, len = 0, taglen;
	char * pdata;
	char   tag[4];

	for (i = 0; i < nCount; i++)
	{
		PR_UINT32 t;

		pdata = ptTLV->pvalue;
		len = ptTLV->valuelen;

		t = ptTLV->tagname;
		//转换tagname
		uint_2_c4(t, tag);
		taglen = 0;
		//过滤掉tagname前面的00
		while (tag[taglen] == 0x00)
			taglen ++;
		//判断是否有足够的空间存tagname
		if (reallen + 4 - taglen > nLenOfOutputBuffer)
		{
			return -1;
		}
		//将tagname输出到buf
		memcpy(pcOutputBuffer + reallen, tag + taglen, 4 - taglen);
		//累加实际输出长度
		reallen += (4 - taglen);

		if (len >= 128)
		{
			//若tag数据长度>=128
			if (reallen + 1 > nLenOfOutputBuffer)
			{
				return -1;
			}
			//插入0x81
			pcOutputBuffer[reallen ++] = 0x81;
		}
		//判断是否有足够的空间存len
		if (reallen + 1 > nLenOfOutputBuffer)
		{
			return -1;
		}
		//输出len值
		pcOutputBuffer[reallen ++] = (PR_INT8)len;
		//判断是否有足够的空间存tag数据
		if (reallen + len > nLenOfOutputBuffer)
		{
			return -1;
		}
		//输出tag数据
		memcpy(pcOutputBuffer + reallen, pdata, len);
		//累加实际输出长度
		reallen += len;
	}
	return reallen;
}

/**
* @fn PR_INT32 parse_tlv(PR_INT8 *ptlvstr, PR_INT32 tlvlen, tlv_t *pobj, PR_INT32 objspace, PR_INT32 deflag)
* @ingroup	emvcore
* @li 解析TLV字符串
	pobj[0]做为记录TLV的长度,字符串指针,TLV个数等，不做实际存入解析出的TLV对象

* @param[in]	PR_INT8 *ptlvstr 	传入TLV字符串的首指针
* @param[in]	PR_INT32 tlvlen 			TLV字符串长度
* @param[out]	tlv_t *pobj		存放解析好的TLV数据结构体数组指针
* @param[in]	PR_INT32 objspace		存放的TLV的pobj的最大空间限制
* @param[in]	PR_INT32 deflag			解析标识
				SINGLE_TLVOBJ		TLV字符串为单个结构数据对象
				STRING_TLVOBJ		TLV对象字符串集(不在结构数据对象内)
				DECODE_LEVEL1		只解析出子结点TLV字串

* @retval	=0	成功
		<0	失败

* @sa 			无

* @attention
  	@li	<b>\<调用\></b>
  	@li	<b>\<设计\></b>

*/
PR_INT32 PR_nTlvUtilParser(PR_INT8 *pcTlvStr, PR_INT32 tlvlen, PR_TLV_T *ptObj, PR_INT32 objspace, PR_INT32 deflag)
{
	PR_INT32 ret;

	if (pcTlvStr == NULL || ptObj == NULL || objspace == 0)
	{
		return -1;
	}
	//起始字符有效性判断
	if (*pcTlvStr == 0x00 || *pcTlvStr == 0xFF)
	{
		/* wrong starting */
		return -1;
	}
	//清零BUF
	memset(ptObj, 0x00, sizeof (PR_TLV_T) * objspace);
	//pobj[0]不实际存储TLV对象
	ptObj[0].pvalue = pcTlvStr;		/*保存TLV字符串指针*/
	ptObj[0].valuelen = tlvlen;		/*保存TLV字符串长度*/

	/*调用实际解析函数*/
	//出错返回-1
	if ((ret = decode_tlv_recur(0, ptObj,  objspace, deflag)) < 0)
	{
		return ret;
	}
	//判断标识
	if (deflag & PR_SINGLE_TLVOBJ)
	{
		//若字符串为单个结构数据对象
		if (ptObj[1]._len  != (unsigned PR_INT32)tlvlen)
		{
			return -1;
		}
	}
	return 0;
}


static PR_INT32 _CetTLV(unsigned short tag, char *data, PR_INT32 *datalen, char *field55, short field55_len)
{
	unsigned short i=0,len;
	unsigned short usGTag;

	if(field55_len <= 0 || data == NULL || datalen == NULL || field55 == NULL) return -1;
	*datalen = 0;

	while(i < field55_len)
	{
		if((field55[i] & 0x1f) == 0x1f){//两字节TAG
			usGTag = field55[i]*256+field55[i+1];
			i += 2;
		}else{
			usGTag = field55[i];
			i += 1;
		}
		if(field55[i] & 0x80){//两字节长度
			len = (field55[i] & 0x7F)*256+field55[i+1];
			i+=2;
		}else{
			len = field55[i];
			i+=1;
		}
		if(tag==usGTag)
		{
			memcpy(data, field55+i, len);
			*datalen=len;
			///return -1;
			return 0;
		}
		i = i + len;
	}

	return -1;
}


/**
* @fn static PR_INT32 decode_tlv_recur(PR_INT32 parent, tlv_t *pobj, PR_INT32 objspace, PR_INT32 deflag)
* @ingroup	emvcore
* @li 解析TLV字符串

* @param[in]	PR_INT32 parent 		父结点的序数
* @param[out]	tlv_t *pobj 	存放解析的TLV结构体
* @param[in]	PR_INT32 objspace	存放解析的TLV结构体容量上限
* @param[in]	PR_INT32 deflag		解析标识
				SINGLE_TLVOBJ		a constructed object
				STRING_TLVOBJ		a tlv object string not in a constructed
				DECODE_LEVEL1		只解析出子结点TLV字串

* @retval	=0	成功
		<0	失败

* @sa 	无

* @attention
  	@li	<b>\<调用\></b>
  	@li	<b>\<设计\></b>
*/
static PR_INT32 decode_tlv_recur(PR_INT32 parent, PR_TLV_T *pobj, PR_INT32 objspace, PR_INT32 deflag)
{

	PR_INT32 itrack = 0, i, tmplen;
	PR_INT32 tlvpos = parent + 1;		//跳过0元素
	PR_UINT32 tmptag;
	PR_INT8* ptlvstr  = pobj[parent].pvalue;

	PR_INT32 tlvlen = pobj[parent].valuelen;

	while (itrack < tlvlen)
	{
		/* omit the 00H and FFH between objects */
		//忽略对象间的0x00和0xFF
		if ((*(ptlvstr + itrack) == 0x00) || (*(ptlvstr + itrack) == 0xFF))
		{
			itrack ++;
			continue;
		}
		//判断tlv个数是否超出限定的最大值
		if (tlvpos == objspace)
		{
			return -1;
		}
		//执行!(val & 0x20)
		if (is_primitive(*(ptlvstr + itrack)))
		{
			pobj[tlvpos].childnum = -1; /* without any children */
		}
		else
		{
			pobj[tlvpos].childnum = 0;/* may have, but now zero */
		}

		/* tag name coding */
		tmptag = 0;
		//保存指向tag name的指针
		pobj[tlvpos]._tagptr = ptlvstr + itrack;
		//执行(val & 0x1f)==0x1f
		//表明tag为两字节
		if (has_subsequent(ptlvstr[itrack]))
		{
			i = 0;
			do {
				tmptag = (tmptag | ptlvstr[itrack ++]) << 8 ;
				if ( ++ i == 4 || itrack == tlvlen)
				{
					return -1;
				}
			} while (another_byte_follow(ptlvstr[itrack]));
		}
		//保存tag名
		pobj[tlvpos].tagname = tmptag | ptlvstr[itrack ++];
		//边界判断
		if (itrack == tlvlen)
		{
			return -1;
		}

		/* length field */
		//保存指向len值的指针
		pobj[tlvpos]._lenptr = ptlvstr + itrack;
		tmplen = ptlvstr[itrack] & 0x7F;
		//val & 0x80 == 1?
		if (lenlen_exceed_one(ptlvstr[itrack ++]))
		{
			if (tmplen > 4 || tmplen > tlvlen - itrack)
			{
				return -1;
			}
			i = tmplen;
			tmplen = 0;
			while (i > 0)
			{
				tmplen |= ptlvstr[itrack ++] << ((--i)  << 3);
			}
		}
		//边界判断
		if (tmplen > tlvlen - itrack)
		{
			return -1;
		}
		//保存len值
		pobj[tlvpos].valuelen = tmplen;
		pobj[tlvpos].parent = parent;
		//保存指向数据的指针
		pobj[tlvpos].pvalue = ptlvstr + itrack;
		itrack += tmplen;
		//该条TLV的长度
		pobj[tlvpos]._len = ptlvstr + itrack - pobj[tlvpos]._tagptr;

		if (pobj[tlvpos].childnum == 0 && !(deflag & PR_DECODE_LEVEL1))
		{
			if ((tmplen = decode_tlv_recur(tlvpos, pobj, objspace, deflag)) < 0)
			{
				return tmplen;
			}
			tlvpos += pobj[tlvpos].childnum;
		}
		tlvpos ++;
	}
	//计算TLV的个数
	pobj[parent].childnum = tlvpos - parent - 1;
	return 0;

}



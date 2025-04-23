/*
 * TLVUtil.c
 * 
 * Created on: Oct 23, 2014
 * Author: yanbin
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
/*
 * @fn int fetch_tlv(int parent, unsigned int tagname, tlv_t *pobj, int level)
 * @group emvcore
 * @li Get the TLV data of tagname in tlv_t pobj structure
 * 
 * @param[in] int parent starts searching from this parent node
 * @param[in] unsigned int tagname The tag name to be found
 * @param[in] tlv_t *pobj tlv_t structure to search
 * @param[in] int level Search level
 * SEARCH_ONLY_SON Only search at its own sub-node
 * SEARCH_ALL_DESC Lookup at all subsequent nodes
 * 
 * @retval <=0 Failed
 * >0 Success
 * 
 * @sa None
 * 
 * @attention
 * @li <b>\<Call\></b>
 * @li <b>\<Design\></b>
 */
PR_INT32 PR_nTlvUtilFetch(PR_INT32 parent, PR_INT32 tagname, PR_TLV_T *pobj, PR_INT32 level)
{
	PR_INT32 i = (parent < 0 ? 0 : parent) + 1;
	PR_INT32 end ;

/*
 * if no objects or parent referred to a primitive element
 */
// The number of TLV objects is valid and the input pointer is valid
	if (pobj == NULL || pobj[i - 1].childnum < 0)
	{
		return -1;
	}
// Calculate the number of scans
	end = pobj[i - 1].childnum + i;
// Perform a scan
	while (i < end)
	{
// If it is found, it will return the offset address
		if (pobj[i].tagname == tagname)
			return i;
		if (level == PR_SEARCH_ONLY_SON)
		{
/*
 * Skip the child nodes of the child nodes
 */
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
// Convert tagname
		uint_2_c4(t, tag);
		taglen = 0;
// Filter out 00 in front of tagname
		while (tag[taglen] == 0x00)
			taglen ++;
// Determine whether there is enough space to store tagname
		if (reallen + 4 - taglen > nLenOfOutputBuffer)
		{
			return -1;
		}
// Output tagname to buf
		memcpy(pcOutputBuffer + reallen, tag + taglen, 4 - taglen);
// Accumulate the actual output length
		reallen += (4 - taglen);

		if (len >= 128)
		{
// If the tag data length >=128
			if (reallen + 1 > nLenOfOutputBuffer)
			{
				return -1;
			}
// Insert 0x81
			pcOutputBuffer[reallen ++] = 0x81;
		}
// Determine whether there is enough space to store len
		if (reallen + 1 > nLenOfOutputBuffer)
		{
			return -1;
		}
// Output len ​​value
		pcOutputBuffer[reallen ++] = (PR_INT8)len;
// Determine whether there is enough space to store tag data
		if (reallen + len > nLenOfOutputBuffer)
		{
			return -1;
		}
// Output tag data
		memcpy(pcOutputBuffer + reallen, pdata, len);
// Accumulate the actual output length
		reallen += len;
	}
	return reallen;
}

/*
 * @fn PR_INT32 parse_tlv(PR_INT8 *ptlvstr, PR_INT32 tlvlen, tlv_t *pobj, PR_INT32 objspace, PR_INT32 deflag)
 * @group emvcore
 * @li parsing TLV strings
 * pobj[0] is used as recording the length of TLV, string pointer, number of TLVs, etc., and does not actually store the parsed TLV object.
 * 
 * @param[in] PR_INT8 *ptlvstr The first pointer to pass in TLV string
 * @param[in] PR_INT32 tlvlen TLV string length
 * @param[out] tlv_t *pobj Stores the parsed TLV data structure array pointer
 * @param[in] PR_INT32 objspace The maximum space limit for the pobj of the TLV stored
 * @param[in] PR_INT32 deflag parsing identifier
 * SINGLE_TLVOBJ TLV string is a single structure data object
 * STRING_TLVOBJ TLV object string set (not in the structure data object)
 * DECODE_LEVEL1 parses only the sub-node TLV string
 * 
 * @retval =0 Success
 * <0 Failed
 * 
 * @sa None
 * 
 * @attention
 * @li <b>\<Call\></b>
 * @li <b>\<Design\></b>
 */
PR_INT32 PR_nTlvUtilParser(PR_INT8 *pcTlvStr, PR_INT32 tlvlen, PR_TLV_T *ptObj, PR_INT32 objspace, PR_INT32 deflag)
{
	PR_INT32 ret;

	if (pcTlvStr == NULL || ptObj == NULL || objspace == 0)
	{
		return -1;
	}
// Determination of validity of starting characters
	if (*pcTlvStr == 0x00 || *pcTlvStr == 0xFF)
	{
/*
 * wrong starting
 */
		return -1;
	}
// Clear BUF
	memset(ptObj, 0x00, sizeof (PR_TLV_T) * objspace);
// pobj[0] does not actually store TLV objects
	ptObj[0].pvalue = pcTlvStr;		/*Save TLV string pointer*/
	ptObj[0].valuelen = tlvlen;		/*Save TLV string length*/

/*
 * Calling the actual parsing function
 */
// Error returns -1
	if ((ret = decode_tlv_recur(0, ptObj,  objspace, deflag)) < 0)
	{
		return ret;
	}
// Judgment mark
	if (deflag & PR_SINGLE_TLVOBJ)
	{
// If the string is a single structure data object
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
		if((field55[i] & 0x1f) == 0x1f){//Two byte TAG
			usGTag = field55[i]*256+field55[i+1];
			i += 2;
		}else{
			usGTag = field55[i];
			i += 1;
		}
		if(field55[i] & 0x80){//Two byte length
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
// /return -1;
			return 0;
		}
		i = i + len;
	}

	return -1;
}


/*
 * @fn static PR_INT32 decode_tlv_recur(PR_INT32 parent, tlv_t *pobj, PR_INT32 objspace, PR_INT32 deflag)
 * @group emvcore
 * @li parsing TLV strings
 * 
 * @param[in] PR_INT32 parent ordinal number of parent node
 * @param[out] tlv_t *pobj Stores parsed TLV structure
 * @param[in] PR_INT32 objspace The upper limit of capacity of TLV structure stored and parsed
 * @param[in] PR_INT32 deflag parsing identifier
 * SINGLE_TLVOBJ a constructed object
 * STRING_TLVOBJ a tlv object string not in a constructed
 * DECODE_LEVEL1 parses only the sub-node TLV string
 * 
 * @retval =0 Success
 * <0 Failed
 * 
 * @sa None
 * 
 * @attention
 * @li <b>\<Call\></b>
 * @li <b>\<Design\></b>
 */
static PR_INT32 decode_tlv_recur(PR_INT32 parent, PR_TLV_T *pobj, PR_INT32 objspace, PR_INT32 deflag)
{

	PR_INT32 itrack = 0, i, tmplen;
	PR_INT32 tlvpos = parent + 1;		//Skip element 0
	PR_UINT32 tmptag;
	PR_INT8* ptlvstr  = pobj[parent].pvalue;

	PR_INT32 tlvlen = pobj[parent].valuelen;

	while (itrack < tlvlen)
	{
/*
 * omit the 00H and FFH between objects
 */
// Ignore 0x00 and 0xFF between objects
		if ((*(ptlvstr + itrack) == 0x00) || (*(ptlvstr + itrack) == 0xFF))
		{
			itrack ++;
			continue;
		}
// Determine whether the number of tlv exceeds the maximum limit
		if (tlvpos == objspace)
		{
			return -1;
		}
// Execute!(val & 0x20)
		if (is_primitive(*(ptlvstr + itrack)))
		{
			pobj[tlvpos].childnum = -1; /* without any children */
		}
		else
		{
			pobj[tlvpos].childnum = 0;/* may have, but now zero */
		}

/*
 * tag name coding
 */
		tmptag = 0;
// Save a pointer to the tag name
		pobj[tlvpos]._tagptr = ptlvstr + itrack;
// Execution (val & 0x1f)==0x1f
// Indicates that the tag is two bytes
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
// Save the tag name
		pobj[tlvpos].tagname = tmptag | ptlvstr[itrack ++];
// Boundary judgment
		if (itrack == tlvlen)
		{
			return -1;
		}

/*
 * length field
 */
// Save a pointer to the len value
		pobj[tlvpos]._lenptr = ptlvstr + itrack;
		tmplen = ptlvstr[itrack] & 0x7F;
// val & 0x80 == 1?
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
// Boundary judgment
		if (tmplen > tlvlen - itrack)
		{
			return -1;
		}
// Save len value
		pobj[tlvpos].valuelen = tmplen;
		pobj[tlvpos].parent = parent;
// Save pointers to data
		pobj[tlvpos].pvalue = ptlvstr + itrack;
		itrack += tmplen;
// The length of the TLV
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
// Calculate the number of TLVs
	pobj[parent].childnum = tlvpos - parent - 1;
	return 0;

}



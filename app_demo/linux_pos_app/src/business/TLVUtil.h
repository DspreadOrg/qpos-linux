/*
 * TLVUtil.h
 * 
 * Created on: Oct 23, 2014
 * Author: yanbin
 */

#ifndef TLVUTIL_H_
#define TLVUTIL_H_

#include "typedefine.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct tag_tlv{
/*
 * internal state
 */
	PR_INT8 *	_tagptr;     /* pointer to 'tag' field */
	PR_INT8 *	_lenptr;     /* pointer to 'len' field */
	PR_UINT32   _len;          /* 'outer' length, specified by user TLV total length*/

/*
 * parsed information
 */
	PR_INT32 	parent;				/*	recored the parent position */
	PR_INT32 	childnum;				/*	the num of children */
	PR_UINT32 tagname;		/*	current tagname */
	PR_UINT32 valuelen;		/*	length of value */
	PR_INT8 * pvalue;	/*	pointer of value*/
}PR_TLV_T;

/*
 * deflag defined
 */
#define PR_SINGLE_TLVOBJ       0x01    /* it is a single constructed object (Single template)*/
#define PR_STRING_TLVOBJ       0x02    /* it is a tlv object string not coded in a constructed object */
#define PR_DECODE_LEVEL1       0x10    /* just decode the object in level one */

/*
 * Parameter level category
 */
#define PR_SEARCH_ALL_DESC     0x01    /* search all the descendants */
#define PR_SEARCH_ONLY_SON     0x02    /* search only its sons  */

PR_INT32 PR_nTlvUtilGetValueString(PR_INT8* pszStream,
								   PR_INT32 nLenOfStream ,
								   PR_INT16 nTLVTag,
								   PR_INT32 nLenOfBuffer,
								   PR_INT8* pcValue);

PR_INT32 PR_nTlvUtilFetch(PR_INT32 parent, PR_INT32 tagname, PR_TLV_T *pobj, PR_INT32 level);

PR_INT32 PR_nTlvUtilParser(PR_INT8 *ptlvstr, PR_INT32 tlvlen, PR_TLV_T *pobj, PR_INT32 objspace, PR_INT32 deflag);

PR_INT32 PR_nTlvUtilPack(PR_UINT32 nTagName, PR_UINT32 nLength, PR_INT8 *pcValue, PR_INT8* pcOutputBuffer);

PR_INT32 PR_nTlvUtilPackOne(PR_TLV_T * ptTLV, PR_INT32 count, PR_INT8* obuf, PR_INT32 olen);

#ifdef __cplusplus
}
#endif
#endif /* TLVUTIL_H_ */

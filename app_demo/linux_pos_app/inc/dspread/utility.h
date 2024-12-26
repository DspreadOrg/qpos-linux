/*
 * Utility.h
 *
 *  Created on: Sep 11, 2014
 *  Author: yanbin
 *  Note:
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include "typedefine.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _PADDING_STYE
{
	EM_PADDING_LEFT,
	EM_PADDING_RIGHT,
}PR_PADDING_STYLE;

/*

 * @Record: create it;
 *
 * @函数名称: PR_nUtilBcd2Asc
 * @函数功能: 将BCD字节流转换为ASC字节流，并返回转换后的长度
 * @输入参数: 
 *					nLenOfBcd		BCD字节流长度
 * 	      				pcBCDStreamIn 	BCD字节流数据指针
 * 	      				nAlign     		转换后的ASC码对齐方式, PR_ALIGN_RIGHT - 右对齐, PR_ALIGN_LEFT - 左对齐
 *	     				nLenOfBuffer	    	pcAscStreamOut 指针所指向BUFFER的长度
 * @输出参数:  	pcAscStreamOut	输出ASC数据字节流
 * @返回值:	 
 *					成功：返回转换后的长度(>0)
 * 			 失败：PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */
 PR_INT32 PR_nUtilBcd2Asc(PR_UINT32 nLenOfBcd,
						 PR_INT8*  pcBCDStreamIn,
						 PR_INT32  nAlign,
						 PR_UINT32 nLenOfBuffer,
						 PR_INT8*  pcAscStreamOut);

/*

 * @Record: create it;
 *
 * @函数名称: PR_nUtilAsc2Bcd
 * @函数功能: 将ASC字节流转换为BCD字节流，并返回转换后的长度
 * @输入参数: nLenOfAsc【输入】  		ASC字节流长度
 * 			pcAscStreamIn【输入】 	ASC字节流数据指针
 * 			nAlign【输入】           转换后的ASC码对齐方式, EM_ALIGN_RIGHT - 右对齐, EM_ALIGN_LEFT - 左对齐
 *			nLenOfBuffer【输入】	    pcBcdStreamOut 指针所指向BUFFER的长度
 *
 *
 *
 * @输出参数:  pcBcdStreamOut【输出】	输出BCD数据字节流
 * @返回值:	 成功：返回转换后的长度(>0)
 * 			 失败：PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */

PR_INT32 PR_nUtilAsc2Bcd(PR_UINT32  nLenOfAsc,
						  PR_INT8*  pcAscStreamIn,
						  PR_INT32  nAlign,
						  PR_UINT32 nLenOfBuffer,
						  PR_INT8*  pcBcdStreamOut);

/*

 * @Record: create it;
 *
 * @函数名称: PR_vUtilBinaryXOR
 * @函数功能: 将输入的Data1,Data2异或,将结果返回至result
 * @输入参数: nDataLength【输入】  需要异或数据的长度
 * 			 pcData1【输入】  	需要异或的数据1 的字节流指针
 * 			 pcData2【输入】 		需要异或的数据2 的字节流指针
 *
 *
 *
 * @输出参数: pcResult【输出】	输出异或后的字节流
 * @返回值:	无
 *
 * @备注:
 * @调用示例:
 *
 * */

 void PR_vUtilBinaryXOR(PR_UINT32 nDataLength,
					   PR_UINT8* pcData1,
					   PR_UINT8* pcData2,
					   PR_UINT8* pcResult);

/*

 * @Record: create it;
 *
 * @函数名称: PR_nUtilHex2Asc
 * @函数功能: 将HEX 字节流转换为对应的ASC字节流，并返回转换后的长度
 * @输入参数: nHexDataLength【输入】  输入的HEX字节流的长度
 * 			 pcHexDataIn【输入】  	输入的HEX字节流数据指针
 * 			 nLenOfBuffer【输入】   pcAscDataOut所指向buffer的长度
 *
 * @输出参数: pcAscDataOut【输出】	输出的ASC字节流指针
 * @返回值:	
 * 				成功:转换后的长度
 *				失败:PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */

PR_INT32 PR_nUtilHex2Asc(PR_UINT32 nHexDataLength,
						 PR_UINT8* pcHexDataIn,
						 PR_UINT32 nLenOfBuffer,
						 PR_INT8*  pcAscDataOut);

/*

 * @Record: create it;
 *
 * @函数名称: PR_pcFindFirstNonSpecifiedChar
 * @函数功能: 返回在输入字符中,与参数中所指定字符不同的字符位置
 * @输入参数: nLenOfStr【输入】    	输入的字符长度
 * 			 pcString【输入】  		输入的字符串
 * 			 cChr【输入】   			所指定的字符
 *
 * @输出参数: pnRemainDataLength【输出】保留的字符长度
 * @返回值:
 * 				成功:返回参数中所指定字符的字符位置
 *				失败:PR_NULL
 * @备注:
 * @调用示例:
 *
 * */

PR_INT8* PR_pcFindFirstNonSpecifiedChar(PR_UINT32 nLenOfStr,
										PR_INT8*  pcString,
										PR_INT8   cChr,
										PR_UINT32* pnRemainDataLength);

/*

 * @函数名称: PR_nUtilBuildeString
 * @函数功能: 将字符流数据,拷贝后转换为字符串(以'\0'为结束符的字符数据流)
 * @输入参数: nLenOfStringBuffer【输入】    	字符字节流数据长度
 * 			 pcString【输入】  	  	字符字节流的数据指针
 * 			 nLenOfAscStream【输入】  
 *			pcAscStream 所指向的buffer长度
 *
 * @输出参数: pcAscStream【输出】 转换后的字符串数据
 * @返回值:	成功 - 返回字符串的长度(>0),长度中包含结束字符
 * 			失败 - 返回 PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */
PR_INT32 PR_nUtilBuildeString(PR_UINT32 nLenOfStringBuffer,
							  PR_INT8*  pcString,
							  PR_UINT32 nLenOfAscStream,
							  PR_UINT8* pcAscStream);
/*
 * @函数名称: PR_nUtilMemcpyEx
 * @函数功能: 内存拷贝操作,如果需要拷贝目标buffer长度超过 源数据长度,剩余的空间则自动填充PaddingChar 所指定的字符
 * @输入参数: pcData【输入】    		源数据
 * 			 nDataLength【输入】  	  	源数据的数据长度
 *			cPaddingChar【输入】			填充字符
 * 			 nOutLength【输入】  		 pcBuffer 所指向的buffer长度
 *
 * @输出参数: pcBuffer【输出】 转换后的字符串数据
 * @返回值:	0 
 *
 * @备注:
 * @调用示例:
 *
 * */
PR_INT32 PR_nUtilMemcpyEx(PR_UINT8* 			pcData,
						  PR_UINT32 			nDataLength,
						  PR_PADDING_STYLE 		nStyle,
						  PR_UINT8   			cPaddingChar,
						  PR_UINT8* 			pcBuffer,
						  PR_UINT32 			nOutLength);


/*

 * @函数名称: PR_bUtilIsDigitalStr
 * @函数功能: 判断输入的字符是否为数值字符串
 * @输入参数: pszStrInput【输入】    			源数据
 *
 * @输出参数: 无
 * @返回值:	数值字符   - 返回 PR_TRUE
 * 			其他字符串 - 返回 PR_FALSE
 *
 * @备注:
 * @调用示例:
 *
 * */

PR_BOOL PR_bUtilIsDigitalStr(PR_INT8* pszStrInput);


/*

 * @函数名称: PR_nUtilNumberToAmt
 * @函数功能: 将输入的数值字符串转换为金额格式
 * @输入参数: pszNum【输入】    			数值字符
 * 			 nLenOfAmtBuffer【输入】  	   输出的金额字符串buffer的大小
 *
 * @输出参数: pszAmt【输出】 转换后的金额字符串
 * @返回值:	成功 - 返回字符串的长度(>0),长度中包含结束字符
 * 			失败 - 返回 PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */

PR_INT32 PR_nUtilNumberToAmt(PR_INT8* pszNum, PR_INT32 nLenOfAmtBuffer, PR_INT8 * pszAmt);

/*
 * @函数名称: PR_nUtilAscToLongHex
 * @函数功能: 将输入的ASC 字符转换为 long long 型返回
 * @输入参数: pszAscData【输入】    	输入的ASC字符
 * 			 nAscDataLen【输入】  	输入的ASC字节流的长度
 *
 * @返回值:	成功 - 转化后的long long型数据
 * 			失败 - 返回 PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */

PR_INT64 PR_nUtilAscToLongHex(PR_INT8 *pszAscData, PR_INT32 nAscDataLen);

/*

 * @Record: create it;
 *
 * @函数名称: PR_nUtilAddAsc
 * @函数功能: 将输入的ASC 字符 进行加法运算
 * @输入参数: theNumberOne【输入】  加数
 * 			 theNumberTwo【输入】  被加数
 *
 * @输出参数:
 * 			theResultSum 【输出】 返回运算的结果
 * @返回值:	成功 - 返回字符串的长度(>0),长度中包含结束字符
 * 			失败 - 返回 PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */
/// Asc string
PR_INT32 PR_nUtilAddAsc(PR_INT8* theNumberOne, PR_INT8* theNumberTwo, PR_INT8* theResultSum);


/*
 * @函数名称: PR_nUtilAddAsc
 * @函数功能: 将输入的ASC 字符 进行减法运算
 * @输入参数: theNumberOne【输入】  减数
 * 			 theNumberTwo【输入】  被减数
 *
 * @输出参数:
 * 			theResult 【输出】 返回运算的结果
 * @返回值:	成功 - 返回字符串的长度(>0),长度中包含结束字符
 * 			失败 - 返回 PR_FAILD
 *
 * @备注:
 * @调用示例:
 *
 * */
/// Sub
PR_INT32 PR_nUtilSubAsc(PR_INT8* theNumberOne, PR_INT8* theNumberTwo, PR_INT8* theResult);

/*
 * @函数名称: PR_nUtilCompareAmount
 * @函数功能: 比对输入金额的大小
 * @输入参数: szAmount1【输入】 金额1
 * 			 szAmout2【输入】  金额2
 *
 * @输出参数:
 *
 * @返回值:	
 *
 *	psTheAmount1  >  psTheAmount2    :返回大于0
 *	psTheAmount1  =  psTheAmount2    :返回0
 *	psTheAmount1  <  psTheAmount2    :返回小于0
 *
 * @备注:
 * @调用示例:
 *
 * */
PR_INT32 PR_nUtilCompareAmount(PR_INT8* psTheAmount1, PR_INT8* psTheAmount2);


/*
 * @函数名称: PR_vGetMD5
 * @函数功能: 计算MD5值
 *
 * @输入参数: 
 *					Buf	--需要计算的数据指针
 *					Len	--需要计算的数据长度

 * @输出参数: 	Digest--md5计算结果
 * @返回值:	成功:返回MD5值的长度(固定为16)
 *				失败:PR_FAILD
 * @备注:
 * @调用示例:
 *
 * */
PR_INT32 PR_nGetMD5(void* Buf, PR_UINT32 Len, void* Digest);

/*
 * @函数名称: PR_nGetCRC16
 * @函数功能: 计算CRC16值
 *
 * @输入参数: 
 *					InitValue	--CRC16的初始值
 *					pszStrInput	--计算CRC16数据的指针
 *					nLenOfBuffer	--需要计算CRC16值数据的长度

 * @输出参数: 	无
 * @返回值:	
 *				返回计算结果
 * @备注:
 * @调用示例:
 *
 * */
PR_UINT16 PR_nGetCRC16(PR_INT32 InitValue, const PR_INT8* pszStrInput, PR_INT32 nLenOfBuffer);

/*
 * @函数名称: RngGenByteVector
 * @函数功能: 生成随机数
 *
 * @输入参数: 
 *					Len	随机数据流的长度

 * @输出参数: 	    Ret 随机数据流
 * @返回值:	    成功:PR_TRUE
 *				失败:PR_FAILD
 * @备注:
 * @调用示例:
 *
 * */
PR_Bool  RngGenByteVector(void* Ret, PR_UINT32 Len);

PR_INT32 nBcd2Asc(PR_INT8* bcd, PR_INT32 asclen, PR_INT8* asc, PR_INT32 align);

PR_INT32 nAsc2Bcd(PR_INT8* ascstr, PR_INT32 asclen, PR_UINT8* bcdstr, PR_INT32 align);

PR_INT8 *debug_hex2string (void *hex, PR_INT32 hexlen);

void Business_getAppPath(PR_INT8* pPath,PR_INT32 nBufLen);

KB_KEYMAP_T KB_nWaitKeyMS(Int32 MS);

PR_INT32 Is_Valid_Network();

void app_trace_value(unsigned char* pbuf, unsigned int cnt);
#ifdef __cplusplus
}
#endif
#endif /* UTILITY_H_ */



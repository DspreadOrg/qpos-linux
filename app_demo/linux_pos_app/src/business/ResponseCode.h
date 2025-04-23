/*
 * ResponseCode.h
 * 
 * Created on: 10 17, 2014
 * Author: jzf
 * Note:
 */

#ifndef _RESPONSE_CODE_H_
#define _RESPONSE_CODE_H_

#include "typedefine.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RESPONSE_CODE_LENGTH	2

void GetResponseCodeHint(PR_INT8* theResponseCode, PR_INT8* theHint);

#ifdef __cplusplus
}
#endif
#endif


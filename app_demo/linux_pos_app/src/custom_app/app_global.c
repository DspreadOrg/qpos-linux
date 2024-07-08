#include "app_global.h"

char nameOperator[20];

u32 str2num(char *strAmt, u32 dwLen)
{
    u32 dwAmt = 0, dwFct = 1;
    int i;
    for(i = dwLen - 1; i >= 0; i--)
    {
        if(strAmt[i] >= '0' && strAmt[i] <= '9')
        {
            dwAmt += (strAmt[i] - '0')*dwFct;
            dwFct *= 10;
        }
    }
    return dwAmt;
}

char *uv2str(char *strRet, T_U8_VIEW *uv)
{
	if(strRet && uv)
	{
		memcpy(strRet, uv->head, uv->len);
		strRet[uv->len] = 0;
    }
	return strRet;
}

int uvcmpstr(T_U8_VIEW *uv, char *str)
{

	u32 i;
	if(uv->len != strlen(str)) return -1;
	for(i = 0; i < uv->len; i++)
		if(uv->head[i] != str[i]) return -1;
	return 0;
}

u32 su2num(T_U8_VIEW *uv)
{
    u32 dwAmt = 0, dwFct = 1;
    int i;
    for(i = uv->len - 1; i >= 0; i--)
    {
        if(uv->head[i] >= '0' && uv->head[i] <= '9')
        {
            dwAmt += (uv->head[i] - '0')*dwFct;
            dwFct *= 10;
        }
    }
    return dwAmt;
}

char *str_in_uv(T_U8_VIEW uvSrc, char *sub)
{
	char *bp, *sp;
	u8 *pTail = uvSrc.head + uvSrc.len;
    if(!UV_OK(uvSrc) || !sub) return (char *)uvSrc.head;

    while(uvSrc.head < pTail)
    {
        bp = (char *)uvSrc.head;
        sp = sub;
        do
        {
            if(!*sp) return (char *)uvSrc.head;
        }while(*sp++ == *bp++);
        uvSrc.head++;
    }
    return NULL;
}

u32 uv2asc(u8 *pAsc, T_U8_VIEW uvBin)
{
	u32 i;
	u8 *pDest = pAsc;
	for(i = 0; i < uvBin.len; i++)
		pDest += sprintf((char *)pDest, "%02X", uvBin.head[i]);
	return (pDest - pAsc);
}

u32 c2_2_uint(pu8  c2)
{
	return ((u32)(*(c2)) << 8) + (*(c2 + 1));
}

u32 c4_2_uint(pu8  c4)
{
	return (((u32)(*c4) << 24) + ((u32)(*(c4 + 1)) << 16) +
	        ((u32)(*(c4 + 2)) << 8) + (*(c4 + 3)));
}

void FormatAmount(T_U8_VIEW uvAmt)
{
    pu8 pBuffer = NULL;
    char Tem[32]={0};

    Memcpy_(Tem,uvAmt.head,uvAmt.len);
    pBuffer = malloc(64);
    Memset_(pBuffer, 0x00, 64);
    format_amt_ascn_to_bcd6(Tem, Strlen_((char * )Tem), pBuffer);
    T_U8_VIEW Trans_amt={pBuffer,6};
    TransPoolSet(TAG_EMV_AMOUNT_AUTH_NUM,Trans_amt);
    free(pBuffer);
}
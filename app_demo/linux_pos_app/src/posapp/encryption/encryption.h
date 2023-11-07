#ifndef _ENCRYPTION_H__
#define _ENCRYPTION_H__ 

#include "cmacro.h"

int ansix98(u_char* result, u_char* work_key, char* passwd, char* cardno, char cDESType);

void son(char *cc,char *dd,char *kk);
void LS(char *bits, char *buffer,uchar count);
void setkeystar(uchar bits[64]);
void s_box(char *aa,char *bb);
void F(uchar n,char *ll,char *rr,char *LL,char *RR);
void ip(uchar *text,char *ll,char *rr);
void _ip(uchar *text,char *ll,char *rr);
void expand0(uchar *in,char *out);
void compress0(char *out,uchar *in);
void compress016(char *out,uchar *in);
void encrypt0(uchar *text,uchar *mtext);
void discrypt0(uchar *mtext,uchar *text);
void DES(uchar *key,uchar *text,uchar *mtext);
void _DES(uchar *key,uchar *text,uchar *mtext);

// void TDESEncrypt(uchar *deskey,uint keylen,uint inlen,uchar *src,uchar *dst);
// void TDESDecrypt(uchar *deskey,uint keylen,uint inlen,uchar *src,uchar *dst);

void DES_CBC(uchar *pucKey,int ucLen,uchar *pucData,uchar *pucEncData,uchar *iv);
void _DES_CBC(uchar *pucKey,int ucLen,uchar *pucEncData,uchar *pucData,uchar *iv);
void TDES(uchar *pucKey,uchar ucLen,uchar *pucData,uchar *pucEncData);
void _TDES(uchar *pucKey,uchar ucLen,uchar *pucEncData,uchar *pucData);

#endif

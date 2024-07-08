﻿// #include "appinc.h"

// #define DES_LONG unsigned long
// /*pdes begin*/

// uchar C[17][28],D[17][28],K[17][48];
// static const uchar pc_1_c[28]={
// 	57,49,41,33,25,17,9
// 	,1,58,50,42,34,26,18
// 	,10,2,59,51,43,35,27
// 	,19,11,3,60,52,44,36};

// static const uchar pc_1_d[28]={
// 	63,55,47,39,31,23,15
// 	,7,62,54,46,38,30,22
// 	,14,6,61,53,45,37,29
// 	,21,13,5,28,20,12,4};

// static const uchar pc_2[48]={
// 	14,17,11,24,1,5,
// 	3,28,15,6,21,10,
// 	23,19,12,4,26,8,
// 	16,7,27,20,13,2,
// 	41,52,31,37,47,55,
// 	30,40,51,45,33,48,
// 	44,49,39,56,34,53,
// 	46,42,50,36,29,32};

// static const uchar ls_count[16]={
// 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

// static const uchar ip_tab[64]={
// 58,50,42,34,26,18,10,2,
// 60,52,44,36,28,20,12,4,
// 62,54,46,38,30,22,14,6,
// 64,56,48,40,32,24,16,8,
// 57,49,41,33,25,17,9,1,
// 59,51,43,35,27,19,11,3,
// 61,53,45,37,29,21,13,5,
// 63,55,47,39,31,23,15,7};

// static const uchar _ip_tab[64]={
// 40,8,48,16,56,24,64,32,
// 39,7,47,15,55,23,63,31,
// 38,6,46,14,54,22,62,30,
// 37,5,45,13,53,21,61,29,
// 36,4,44,12,52,20,60,28,
// 35,3,43,11,51,19,59,27,
// 34,2,42,10,50,18,58,26,
// 33,1,41,9,49,17,57,25};

// static const uchar e_r[48]={
// 32,1,2,3,4,5,4,5,6,7,8,9,
// 8,9,10,11,12,13,12,13,14,15,16,17,
// 16,17,18,19,20,21,20,21,22,23,24,25,
// 24,25,26,27,28,29,28,29,30,31,32,1};

// static const uchar P[32]={
// 16,7,20,21,29,12,28,17,
// 1,15,23,26,5,18,31,10,
// 2,8,24,14,32,27,3,9,
// 19,13,30,6,22,11,4,25};

// static const uchar SSS[16][4][16]={
// 	{
// 		{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
// 		{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},/* err on */
// 		{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
// 		{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13},
// 	},
//     {
//         {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
//         {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
//         {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
//         {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9},
//     },
//     {
//         {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
//         {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
//         {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
//         {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12},
//     },
//     {
//         {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
//         {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
//         {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
//         {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}, /* err on */
//     },
//     {
//         {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
//         {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6}, /* err on */
//         {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
//         {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3},
//     },
//     {
//         {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
//         {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
//         {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
//         {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13},
//     },
//     {
//         {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
//         {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
//         {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
//         {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12},
//     },
//     {
//         {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
//         {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
//         {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
//         {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11},
//     },
// };


// void son(char *cc,char *dd,char *kk)
// {
// 	uchar i;
// 	char buffer[56];
// 	for (i=0;i<28;i++)
// 		buffer[i]=*cc++;

// 	for (i=28;i<56;i++)
// 		buffer[i]=*dd++;

// 	for (i=0;i<48;i++)
// 		*kk++=buffer[pc_2[i]-1];
// }

// void LS(char *bits,char *buffer,uchar count)
// {
// 	uchar i;
// 	for (i=0;i<28;i++)
// 	{
// 		buffer[i]=bits[(i+count)%28];
// 	}
// }

// void setkeystar(uchar bits[64])
// {
// 	uchar i,j;

// 	for (i=0;i<28;i++)
// 		C[0][i]=bits[pc_1_c[i]-1];
// 	for (i=0;i<28;i++)
// 		D[0][i]=bits[pc_1_d[i]-1];
// 	for (j=0;j<16;j++)
// 	{
// 		LS((char *)C[j],(char *)C[j+1],ls_count[j]);
// 		LS((char *)D[j],(char *)D[j+1],ls_count[j]);
// 		son((char *)C[j+1],(char *)D[j+1],(char *)K[j+1]);
// 	}
// }

// void s_box(char *aa,char *bb)
// {
// 	uchar i,j,k,m;
// 	uchar y,z;
// 	char ss[8];
// 	m=0;
// 	for (i=0;i<8;i++)
// 	{
// 		j=6*i;
// 		y=aa[j]*2+aa[j+5];
// 		z=aa[j+1]*8+aa[j+2]*4+aa[j+3]*2+aa[j+4];
// 		ss[i]=SSS[i][y][z];
// 		y=0x08;
// 		for (k=0;k<4;k++)
// 		{
// 			bb[m++]=(ss[i]/y)&1;
// 			y/=2;
// 		}

// 	}
// }

// void  F(uchar n,char *ll,char *rr,char *LL,char *RR)
// {
// 	uchar i;
// 	char buffer[64],tmp[64];
// 	for (i=0;i<48;i++)
// 		buffer[i]=rr[e_r[i]-1];
// 	for (i=0;i<48;i++)
// 		buffer[i]=(buffer[i]+K[n][i])&1;

// 	s_box(buffer,tmp);

// 	for (i=0;i<32;i++)
// 		buffer[i]=tmp[P[i]-1];

// 	for (i=0;i<32;i++)
// 		RR[i]=(buffer[i]+ll[i])&1;

// 	for (i=0;i<32;i++)
// 		LL[i]=rr[i];


// }

// void expand0(uchar *in,char *out)
// {
// 	uchar divide;
// 	uchar i,j;

// 	for (i=0;i<8;i++)
// 	{
// 		divide=0x80;
// 		for (j=0;j<8;j++)
// 		{
// 			*out++=(in[i]/divide)&1;
// 			divide/=2;
// 		}
// 	}
// }



// void ip(uchar *text,char *ll,char *rr)
// {
// 	uchar i;
// 	char buffer[64];
// 	/*
// 	char tmp[64];
// 	*/
// 	expand0(text,buffer);

// 	for (i=0;i<32;i++)
// 		ll[i]=buffer[ip_tab[i]-1];

// 	for (i=0;i<32;i++)
// 		rr[i]=buffer[ip_tab[i+32]-1];
// }

// void _ip(uchar *text,char *ll,char *rr)
// {
// 	uchar i;
// 	char tmp[64];
// 	for (i=0;i<32;i++)
// 		tmp[i]=ll[i];
// 	for (i=32;i<64;i++)
// 		tmp[i]=rr[i-32];
// 	for (i=0;i<64;i++)
// 		text[i]=tmp[_ip_tab[i]-1];
// }


// void compress0(char *out,uchar *in)
// 	/*
// 	uchar *in;
// 	char *out;
// 	*/
// {
// 	uchar times;
// 	uchar i,j;

// 	for (i=0;i<8;i++)
// 	{
// 		times=0x80;
// 		in[i]=0;
// 		for (j=0;j<8;j++)
// 		{
// 			in[i]+=(*out++)*times;
// 			times/=2;
// 		}
// 	}
// }
// void compress016(char *out,uchar *in)
// 	/*
// 	uchar *in;
// 	char *out;
// 	*/
// {
// 	uchar times;
// 	uchar i,j;

// 	for (i=0;i<16;i++)
// 	{
// 		times=0x8;
// 		in[i]='0';
// 		for (j=0;j<4;j++)
// 		{
// 			in[i]+=(*out++)*times;
// 			times/=2;
// 		}
// 	}
// }

// void encrypt0(uchar *text,uchar *mtext)
// 	/*
// 	uchar *text,*mtext;
// 	*/
// {
// 	char ll[64],rr[64],LL[64],RR[64];
// 	char tmp[64];
// 	uchar i,j;
// 	ip(text,ll,rr);

// 	for (i=1;i<17;i++)
// 	{
// 		F(i,ll,rr,LL,RR);
// 		for (j=0;j<32;j++)
// 		{
// 			ll[j]=LL[j];
// 			rr[j]=RR[j];
// 		}
// 	}

// 	_ip((uchar *)tmp,rr,ll);

// 	compress0(tmp,mtext);
// }

// void discrypt0(uchar *mtext,uchar *text)
// 	/*
// 	uchar *text,*mtext;
// 	*/
// {
// 	char ll[64],rr[64],LL[64],RR[64];
// 	char tmp[64];
// 	uchar i,j;
// 	ip(mtext,ll,rr);

// 	for (i=16;i>0;i--)
// 	{
// 		F(i,ll,rr,LL,RR);
// 		for (j=0;j<32;j++)
// 		{
// 			ll[j]=LL[j];
// 			rr[j]=RR[j];
// 		}
// 	}

// 	_ip((uchar *)tmp,rr,ll);

// 	compress0(tmp,text);
// }


// void DES(uchar *key,uchar *text,uchar *mtext)
// 	/*
// 	uchar *text,*mtext,*key;
// 	*/
// {
// 	char tmp[64];
// 	expand0(key,tmp);
// 	setkeystar((uchar *)tmp);
// 	encrypt0(text,mtext);
// }
// void _DES(uchar *key,uchar *text,uchar *mtext)
// 	/*
// 	uchar *text,*mtext,*key;
// 	*/
// {
// 	char tmp[64];
// 	expand0(key,tmp);
// 	setkeystar((uchar *)tmp);
// 	discrypt0(text,mtext);
// }

// int A_(char* result, char* passwd, int len, char* cardno);

// int ansix98(u_char* result, u_char* work_key, char* passwd, char* cardno, char cDESType)
// {
// 	int     len = 0;
// 	u_char  password[9];

// 	len = strlen(passwd);
// 	A_((char *)result, passwd, len, cardno);
// 	DES(work_key, result, password);
// 	if (cDESType)
// 	{
// 		_DES(work_key + 8, password, password);
// 		DES(work_key, password, password);
// 	}
// 	memcpy(result, password, 8);
// 	return NORMAL;
// }

// int A_nocard(char *pucResult,char *passwd,int len)
// {
// 	char    passwd0[17], pwd_block[9];
// 	int     i;

// 	PubStrcpy(passwd0, passwd);
// 	for (i = len; i < 16; i++)
// 		passwd0[i] = 'F';
// 	pwd_block[0] = len;
// 	c_asc2bcd((uchar*)pwd_block + 1, (uchar*)passwd0, 14, 0);
// 	memcpy(pucResult,pwd_block,8);
// 	return 0;
// }

// int A_(char* result, char* passwd, int len, char* cardno)
// {
// 	char    passwd0[17], pwd_block[9], pan_block[9];
// 	int     i;

// 	PubStrcpy(passwd0, passwd);
// 	for (i = len; i < 16; i++)
// 		passwd0[i] = 'F';
// 	pwd_block[0] = len;
// 	c_asc2bcd((u_char*)pwd_block + 1, (u_char*)passwd0, 14, 0);

// 	memset(pan_block, 0, 2);
// 	c_asc2bcd((u_char*)pan_block + 2, (u_char*)cardno + 3, 12, 0);

// 	for (i = 0; i < 8; i++)
// 		result[i] = pwd_block[i] ^ pan_block[i];

// 	return NORMAL;
// }

// void TDES(uchar *pucKey,uchar ucLen,uchar *pucData,uchar *pucEncData)
// {
// 	uchar i;

// 	for(i=0;i<ucLen;i+=8)
// 	{
// 		DES(pucKey,pucData+i,pucEncData+i);
// 		_DES(pucKey+8,pucEncData+i,pucEncData+i);
// 		DES(pucKey,pucEncData+i,pucEncData+i);
// 	}
// }


// void _TDES(uchar *pucKey,uchar ucLen,uchar *pucEncData,uchar *pucData)
// {
// 	uchar i;

// 	for(i=0;i<ucLen;i+=8)
// 	{
// 		_DES(pucKey,pucEncData+i,pucData+i);
// 		DES(pucKey+8,pucData+i,pucData+i);
// 		_DES(pucKey,pucData+i,pucData+i);
// 	}
// }

// void DES3(uchar *pucKey, uchar ucLen, uchar *pucData, uchar *pucEncData)
// {
// 	uchar i;

// 	for (i = 0; i< ucLen; i += 8)
// 	{
// 		DES(pucKey, pucData+i, pucEncData+i);
// 		_DES(pucKey+8, pucEncData+i, pucEncData+i);
// 		DES(pucKey+16, pucEncData+i, pucEncData+i);
// 	}
// }

// /*****************************************************
// *函数功能:DES 的CBC算法实现 加密
// *输入参数:pucKey 加密密钥
// 		  ucLen  加密数据长度
// 		  pucData加密数据内容
// 		  iv     初始向量
// *输出参数:pucEncData 加密结果
// *返回值:
// ****************************************************/
// void DES_CBC(uchar *pucKey,int ucLen,uchar *pucData,uchar *pucEncData,uchar *iv)
// {
// 	uchar szBuffer[9] = {0};
// 	int i,j;
	
// 	for (j = 0; j < ucLen; j += 8)
// 	{
// 		if (j == 0)
// 		{
// 			for (i = 0; i < 8; i++)
// 			{
// 				szBuffer[i] = pucData[i]^iv[i];
// 			}
// 		}
// 		else
// 		{
// 			for (i = 0; i < 8; i++)
// 			{
// 				szBuffer[i] = pucData[j + i]^pucEncData[j - 8 + i];
// 			}
// 		}
// 		DES(pucKey, szBuffer, pucEncData + j);
// 	}
// }
// /*****************************************************
// *函数功能:DES 的CBC算法实现 解密
// *输入参数:pucKey 加密密钥
// 		  ucLen  加密数据长度
// 		  pucData加密数据内容
// 		  iv     初始向量
// *输出参数:pucEncData 加密结果
// *返回值:
// ****************************************************/
// void _DES_CBC(uchar *pucKey,int ucLen,uchar *pucEncData,uchar *pucData,uchar *iv)
// {
// 	uchar szBuffer[9] = {0};
// 	int i,j;

// 	for (j = 0; j < ucLen; j += 8)
// 	{
// 		memset(szBuffer,0x0,sizeof(szBuffer));
// 		_DES(pucKey, pucEncData + j,szBuffer);
// 		if (j == 0)
// 		{
// 			for (i = 0; i < 8; i++)
// 			{
// 				pucData[i] = szBuffer[i]^iv[i];
// 			}
// 		}
// 		else
// 		{
// 			for (i = 0; i < 8; i++)
// 			{
// 				pucData[j + i] = szBuffer[i]^pucEncData[j - 8 + i];
// 			}
// 		}
// 	}
// }
// /* End of this file */


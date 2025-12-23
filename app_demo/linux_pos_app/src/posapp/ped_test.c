#include "ped_test.h"
int Ped_Dukpt_Init()
{
	PR_INT32 nRet = -1;
    unsigned char tlk[] = {"\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10"};
    unsigned char ipek[] = {"\x1A\x4D\x67\x2D\xCA\x6C\xB3\x35\x1F\xD1\xB0\x2B\x23\x7A\xF9\xAE"}; //  encrypt by tlk ,use 3des ecb mode 
    unsigned char kcv[] = {"\x08\xD7\xB4\xFB\x62\x9D\x08\x85"};
    unsigned char ksn[] = "\x12\x02\x40\x60\x30\x00\x01\xE0\x00\x01";

    do
    {
        nRet = ped_write_tlk(PED_TLK,0,PED_TLK,1,tlk,16,NULL,0,NULL);
        if(nRet != RET_OK)
            break;

        nRet = ped_write_ipek(PED_TLK,1,PED_TIK,10,ipek,16,kcv,8,ksn);
        OsLog(LOG_DEBUG,"==========10 nRet = %d",nRet);
        nRet = ped_write_ipek(PED_TLK,1,PED_TIK,11,ipek,16,kcv,8,ksn);
        OsLog(LOG_DEBUG,"==========11 nRet = %d",nRet);
        nRet = ped_write_ipek(PED_TLK,1,PED_TIK,12,ipek,16,kcv,8,ksn);
        OsLog(LOG_DEBUG,"==========12 nRet = %d",nRet);
        
        OsLog(LOG_DEBUG,"==========nRet = %d",nRet);
        unsigned char data[32] = {0};
        OsPedGetKsnDukpt(11,ksn);
        nBcd2Asc(ksn,20,data,0);
        OsLog(LOG_DEBUG,"==========ksn = %s",data);
        
        dukpt_test();

    } while (0);
    

	return nRet;
}

PR_INT32 ped_write_tlk(PR_UINT8 SrcKeyType,PR_UINT8 SrcKeyIdx,PR_UINT8 DstKeyType,PR_UINT8 DstKeyIdx,
					PR_UINT8 *pKey,PR_INT32 keyLen,PR_UINT8 *pKcv,PR_INT32 kcvLen,PR_UINT8 *pKsn)
{
    PR_UINT8 tmep[2048] = {0};
    PR_INT32 offset = 0;
	PR_INT32 nRet = -1;

	nRet = OsPedOpen();
    if(nRet != RET_OK){
        OsLog(LOG_DEBUG,"==========OsPedOpen nRet = %d",nRet);
        return PR_FAILD;
    }
    memset(tmep,0x0,sizeof(tmep));
    tmep[offset] = 0x03;
    offset += 1;
    //SrcKeyType
    tmep[offset] = SrcKeyType;
    offset += 1;
    //SrcKeyIdx
    tmep[offset] = SrcKeyIdx;
    offset += 1;
    //DstKeyIdx：
    tmep[offset] = DstKeyIdx;
    offset += 1;
    //7 bytes Reserved field, random number
    offset += 7;
    //DstKeyType
    tmep[offset] = DstKeyType;
    offset += 1;
    //DstKeyLen
    tmep[offset] = (uchar)keyLen;
    offset += 1;
    //DstKeyValue
    memcpy(tmep+offset,pKey,keyLen);
    offset += 24;
    //KcvMode
    if(kcvLen <= 0){
        tmep[offset] = 0x0;
    }else{
        tmep[offset] = 0x1;
    }
    offset += 1;
    //KcvData
    offset += 128;
    //KCV
    if(kcvLen > 0){
        memcpy(tmep+offset,pKcv,kcvLen);
    }
    offset += 8;

    //
    offset += 10;

    nRet = OsPedWriteKey(tmep);
	if(nRet != RET_OK){
        OsLog(LOG_DEBUG,"==========OsPedWriteKey nRet = %d",nRet);
        nRet =  PR_FAILD;
    }else{
		nRet =  PR_NORMAL;
	}
	OsPedClose();
	return nRet;
}

PR_INT32 ped_write_ipek(PR_UINT8 SrcKeyType,PR_UINT8 SrcKeyIdx,PR_UINT8 DstKeyType,PR_UINT8 DstKeyIdx,
					PR_UINT8 *pKey,PR_INT32 keyLen,PR_UINT8 *pKcv,PR_INT32 kcvLen,PR_UINT8 *pKsn)
{
    PR_UINT8 tmep[2048] = {0};
    PR_INT32 offset = 0;
	PR_INT32 nRet = -1;

	nRet = OsPedOpen();
    if(nRet != RET_OK){
        OsLog(LOG_DEBUG,"==========OsPedOpen nRet = %d",nRet);
        return PR_FAILD;
    }
    memset(tmep,0x0,sizeof(tmep));
    tmep[offset] = 0x03;
    offset += 1;
    //SrcKeyType
    tmep[offset] = SrcKeyType;
    offset += 1;
    //SrcKeyIdx
    tmep[offset] = SrcKeyIdx;
    offset += 1;
    //DstKeyIdx：
    tmep[offset] = DstKeyIdx;
    offset += 1;
    //7 bytes Reserved field, random number
    offset += 7;
    //DstKeyType
    tmep[offset] = DstKeyType;
    offset += 1;
    //DstKeyLen
    tmep[offset] = (uchar)keyLen;
    offset += 1;
    //DstKeyValue
    memcpy(tmep+offset,pKey,keyLen);
    offset += 24;
    //KcvMode
    if(kcvLen <= 0){
        tmep[offset] = 0x00;
    }else{
        tmep[offset] = 0x01;
    }
    offset += 1;
    //KcvData
    offset += 128;
    //KCV
    if(kcvLen > 0){
        memcpy(tmep+offset,pKcv,kcvLen);
    }
    offset += 8;
    // Fill in random numbers 
    if(pKsn != NULL){
        memcpy(tmep+offset,pKsn,10);
    }
    offset += 10;
    nRet = OsPedWriteTIK(tmep);
	if(nRet != RET_OK){
        OsLog(LOG_DEBUG,"==========OsPedWriteTIK nRet = %d",nRet);
        nRet =  PR_FAILD;
    }else{
		nRet =  PR_NORMAL;
	}
	OsPedClose();
	return nRet;						
}

void dukpt_test()
{
	PR_INT32 nRet = -1;
	unsigned char outData[512] = {0};
	unsigned char ksn[32] = {0};
	unsigned char temp[64] = {0};
	nRet = OsPedOpen();
    if(nRet != RET_OK){
        return;
    }
	unsigned char data[] = {"\x7A""\xF9""\x0F""\x71""\xFE""\xC0""\x8D""\x2E""\x5F""\x34""\x23""\xD3""\xBC""\x6C""\x94""\x34"};
    nRet = OsPedIncreaseKsnDukpt(12);
	if(nRet == RET_OK)
	{
		nRet = OsPedDesDukpt(12,0x01,NULL,16,data,outData,ksn,0x03);
		if(nRet == RET_OK)
		{
			OsLog(LOG_DEBUG,"==========OsPedDesDukpt ok nRet= %d",nRet);
			nBcd2Asc(outData,32,temp,1);
			OsLog(LOG_DEBUG,"%s",temp);
			memset(temp,0,sizeof(temp));
			nBcd2Asc(ksn,20,temp,1);

			OsLog(LOG_DEBUG,"%s",temp);
		}
		else{
			OsLog(LOG_DEBUG,"==========OsPedDesDukpt error nRet= %d",nRet);
		}
	}
	else
	{
		OsLog(LOG_DEBUG,"==========ksn increase error nRet= %d",nRet);
	}
	OsPedClose();
	return ;
}

int packWriteKeyBuf(uchar SrcKeyType,uchar SrcKeyIdx,uchar DstKeyType,uchar DstKeyIdx,uchar *pKey,int keyLen,uchar *pKcv,int kcvLen,uchar *pKsn,uchar *pOut){
    uchar tmep[2048] = {0};
    int offset = 0;

    memset(tmep,0x0,sizeof(tmep));
    tmep[offset] = 0x03;
    offset += 1;
    //SrcKeyType
    tmep[offset] = SrcKeyType;
    offset += 1;
    //SrcKeyIdx
    tmep[offset] = SrcKeyIdx;
    offset += 1;
    //DstKeyIdx：
    tmep[offset] = DstKeyIdx;
    offset += 1;
    //7 bytes Reserved field, random number
    offset += 7;
    //DstKeyType
    tmep[offset] = DstKeyType;
    offset += 1;
    //DstKeyLen
    tmep[offset] = (uchar)keyLen;
    offset += 1;
    //DstKeyValue
    memcpy(tmep+offset,pKey,keyLen);
    offset += 24;
    //KcvMode
    if(kcvLen <= 0){
        tmep[offset] = 0x0;
    }else{
        tmep[offset] = 0x1;
    }
    offset += 1;
    //KcvData
    offset += 128;
    //KCV
    if(kcvLen > 0){
        memcpy(tmep+offset,pKcv,kcvLen);
    }
    offset += 8;
    // Fill in random numbers 
    if(pKsn != NULL){
        memcpy(tmep+offset,pKsn,10);
    }
    offset += 10;
    memcpy(pOut,tmep,offset);
    dumpBytes("key load ",tmep,offset);
    return offset;
}

void pinpadDupktTest1(){
    int ret;
    uchar tmep[2048] = {0};
    int offset = 0;
    unsigned char tlk[] = {"\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"};

    unsigned char tik[] = {"\xA8""\xD0""\xB1""\xCF""\x0C""\xE5""\x98""\x70""\x08""\x8C""\xAE""\xD6""\x53""\xBC""\xAA""\xA3"};
    unsigned char tikCkv[] = {"\xC5""\x96""\x1E""\x62""\xBF""\xCE""\x71""\x10"};
    // unsigned char initKsn[] = {"\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x01"};

    unsigned char ipek[] = {"\x7A""\xF9""\x0F""\x71""\xFE""\xC0""\x8D""\x2E""\x5F""\x34""\x23""\xD3""\xBC""\x6C""\x94""\x44"};
    unsigned char initKsn[] = "\x02\x02\x31\x11\x00\x00\x04\xE0\x00\x01";
    unsigned char data[] = {"\x7A""\xF9""\x0F""\x71""\xFE""\xC0""\x8D""\x2E""\x5F""\x34""\x23""\xD3""\xBC""\x6C""\x94""\x34"};

    //7AF90F71FEC08D2E5F3423D3BC6C9444
    
    //02023111000004E00001

    unsigned char mac[16] = {0};
    unsigned char enc[32] = {0};
    unsigned char dnc[32] = {0};
    unsigned char PinBlock[32] = {0};
    unsigned char kcvData[8+1] = {0};
    unsigned char kcv[8+1] = {0};
    unsigned char ksn[10+1] = {0};
    unsigned char uzRand[1024] = {0};

    ret = OsPedOpen();
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to open password keyboard \n");
        return;
    }
    ret = OsPedGetVer(tmep);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to get password keyboard version [%d]\n",ret);
        goto EXIT;
    }
    OsLog(LOG_DEBUG,"Password keyboard version %s \n",tmep);

    ret = OsPedEraseKeys();
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to erase keys \n");
        goto EXIT;
    }
  
    memset(tmep,0x0,sizeof(tmep));
    offset = packWriteKeyBuf(PED_TLK,0,PED_TIK,2,ipek,16,NULL,0,initKsn,tmep);
    ret = OsPedWriteTIK(tmep);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to write PED_TIK \n");
        goto EXIT;
    }
    OsLog(LOG_DEBUG,"Successfully wrote PED_TIK 2 \n");  

    memset(tmep,0x0,sizeof(tmep));
    offset = packWriteKeyBuf(PED_TLK,0,PED_TIK,1,ipek,16,NULL,0,initKsn,tmep);
    ret = OsPedWriteTIK(tmep);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to write PED_TIK \n");
        goto EXIT;
    }
    OsLog(LOG_DEBUG,"Successfully wrote PED_TIK 1 \n");  

    memset(tmep,0x0,sizeof(tmep));
    offset = packWriteKeyBuf(PED_TLK,0,PED_TIK,3,ipek,16,NULL,0,initKsn,tmep);
    ret = OsPedWriteTIK(tmep);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to write PED_TIK \n");
        goto EXIT;
    }
    OsLog(LOG_DEBUG,"Successfully wrote PED_TIK 3 \n");  

    ret = OsPedIncreaseKsnDukpt(2);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to increment KSN in DUKPT mode \n");
        goto EXIT;
    }  
    OsLog(LOG_DEBUG,"Successfully incremented KSN in DUKPT mode \n");

    ret = OsPedGetKsnDukpt(2,ksn);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to get KSN in DUKPT mode \n");
        goto EXIT;
    } 
    dumpBytes("KSN after incrementing by 1",ksn,10);

    memset(ksn,0x0,sizeof(ksn));
    ret = OsPedDesDukpt(2,0x01,NULL, 16, data, enc, ksn,0x03);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to encrypt data in DUKPT mode \n");
        goto EXIT;
    }
    dumpBytes("Current KSN",ksn,10);
    dumpBytes("Encrypted data",enc,16);

    memset(ksn,0x0,sizeof(ksn));
    ret = OsPedDesDukpt(2,0x01,NULL, 16, enc, dnc, ksn,0x02);
    if(ret != RET_OK){
        OsLog(LOG_DEBUG,"Failed to decrypt data in DUKPT mode \n");
        goto EXIT;
    }
    dumpBytes("Current KSN",ksn,10);
    dumpBytes("Decrypted data",dnc,16);

    // // MAC calculation
    // memset(ksn,0x0,sizeof(ksn));
    // ret = OsPedGetMacDukpt(1, tik, 16, mac, ksn,0x23);
    // if(ret != RET_OK){
    //     printf("Failed to calculate MAC \n");
    //     goto EXIT;
    // }
    // dumpBytes("Current KSN",ksn,10);
    // dumpBytes("Mac结果",mac,8);

    // memset(ksn,0x0,sizeof(ksn));
    // ret = OsPedGetPinDukpt(1,(unsigned char *)"6228480068726674572", (char *)"0,4,6,12", 0x20, 30*1000, ksn,PinBlock);
    // if(ret != RET_OK){
    //     printf("Failed to get PIN block \n");
    //     goto EXIT;
    // }
    // dumpBytes("Encrypted password",PinBlock,8);
    // dumpBytes("Current KSN",ksn,10);
    // printf("Finished getting PIN block \n");    
EXIT:
    OsLog(LOG_DEBUG,"Closing password keyboard \n");
    OsPedClose();
}
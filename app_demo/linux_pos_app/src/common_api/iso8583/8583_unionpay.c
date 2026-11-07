#include "App_ISOMsg.h"
#include "App_iso8583.h"

const ISO_FIELD_DEF Iso8583Def_union_pay[] = {
	{4, LEN_FIX, ALN_LEFT, '\0', PubFPBMsgIDPack, PubFPBMsgIDUnpack},   /* 0   "MESSAGE TYPE INDICATOR" */
	{8, LEN_FIX, ALN_LEFT, '\0', PubFPBBitmapPack, PubFPBBitmapUnpack}, /* 1   "BIT MAP" */
	{19, LEN_LLVAR, ALN_LEFT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},  /* 2   "PAN - PRIMARY ACCOUNT NUMBER" */
	{6, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 3   "PROCESSING CODE" */
	{12, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 4   "AMOUNT, TRANSACTION" */
	{12, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 5   "AMOUNT, SETTLEMENT" */
	{12, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 6   "AMOUNT, CARDHOLDER BILLING" */
	{10, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 7   "TRANSMISSION DATE AND TIME" */
	{8, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 8   "AMOUNT, CARDHOLDER BILLING FEE" */
	{8, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 9   "CONVERSION RATE, SETTLEMENT" */
	{8, LEN_FIX, ALN_RIGHT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},     /* 10  "CONVERSION RATE, CARDHOLDER BILLING" */
	{6, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 11  "SYSTEM TRACE AUDIT NUMBER" */
	{6, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 12  "TIME, LOCAL TRANSACTION" */
	{4, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 13  "DATE, LOCAL TRANSACTION" */
	{4, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 14  "DATE, EXPIRATION" */
	{4, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 15  "DATE, SETTLEMENT" */
	{4, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 16  "DATE, CONVERSION" */
	{4, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 17  "DATE, CAPTURE" */
	{4, LEN_FIX, ALN_RIGHT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},     /* 18  "MERCHANTS TYPE" */
	{3, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 19  "ACQUIRING INSTITUTION COUNTRY CODE" */
	{3, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 20  "PAN EXTENDED COUNTRY CODE" */
	{3, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 21  "FORWARDING INSTITUTION COUNTRY CODE" */
	{3, LEN_FIX, ALN_LEFT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},     /* 22  "POINT OF SERVICE ENTRY MODE" */
	{3, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},	/* 23  "CARD SEQUENCE NUMBER" */
	{4, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 24  "NETWORK INTERNATIONAL IDENTIFIEER" */
	{2, LEN_FIX, ALN_RIGHT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},     /* 25  "POINT OF SERVICE CONDITION CODE" */
	{2, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},    /* 26  "POINT OF SERVICE PIN CAPTURE CODE" */
	{1, LEN_FIX, ALN_RIGHT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},     /* 27  "AUTHORIZATION IDENTIFICATION RESP LEN" */
	{8, LEN_FIX, ALN_RIGHT, '\0', PubFPBAmountPack_BCD, PubFPBAmountUnpack_BCD},        /* 28  "AMOUNT, TRANSACTION FEE" */
	{8, LEN_FIX, ALN_RIGHT, '\0', PubFPBAmountPack_BCD, PubFPBAmountUnpack_BCD},        /* 29  "AMOUNT, SETTLEMENT FEE" */
	{8, LEN_FIX, ALN_RIGHT, '\0', PubFPBAmountPack_BCD, PubFPBAmountUnpack_BCD},        /* 30  "AMOUNT, TRANSACTION PROCESSING FEE" */
	{8, LEN_FIX, ALN_RIGHT, '\0', PubFPBAmountPack_BCD, PubFPBAmountUnpack_BCD},        /* 31  "AMOUNT, SETTLEMENT PROCESSING FEE" */
	{11, LEN_LLVAR, ALN_LEFT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD}, /* 32  "ACQUIRING INSTITUTION IDENT CODE" */
	{11, LEN_LLVAR, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD}, /* 33  "FORWARDING INSTITUTION IDENT CODE" */
	{28, LEN_LLVAR, ALN_LEFT, '\0', PubFPACharPack_BCD, PubFPACharUnpack_BCD},  /* 34  "PAN EXTENDED" */
	{40, LEN_LLVAR, ALN_LEFT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 35  "TRACK 2 DATA" */
	{108, LEN_LLLVAR, ALN_LEFT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD}, /* 36  "TRACK 3 DATA" */
	{12, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},     /* 37  "RETRIEVAL REFERENCE NUMBER" */
	{6, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 38  "AUTHORIZATION IDENTIFICATION RESPONSE" */
	{2, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 39  "RESPONSE CODE" */
	{99, LEN_LLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 40  "SERVICE RESTRICTION CODE" */
	{8, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 41  "CARD ACCEPTOR TERMINAL IDENTIFICACION" */
	{15, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},     /* 42  "CARD ACCEPTOR IDENTIFICATION CODE"  */
	{40, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},     /* 43  "CARD ACCEPTOR NAME/LOCATION" */
	{99, LEN_LLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},   /* 44  "ADITIONAL RESPONSE DATA" */
	{76, LEN_LLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},   /* 45  "TRACK 1 DATA" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD}, /* 46  "ADITIONAL DATA - ISO" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPBBinaryPack_BCD, PubFPBBinaryUnpack_BCD}, /* 47  "ADITIONAL DATA - NATIONAL" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD}, /* 48  "ADITIONAL DATA - PRIVATE" */
	{3, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 49  "CURRENCY CODE, TRANSACTION" */
	{3, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 50  "CURRENCY CODE, SETTLEMENT" */
	{3, LEN_FIX, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD},      /* 51  "CURRENCY CODE, CARDHOLDER BILLING"    */
	{8, LEN_FIX, ALN_LEFT, '\0', PubFPBBinaryPack_BCD, PubFPBBinaryUnpack_BCD}, /* 52  "PIN DATA"    */
	{16, LEN_FIX, ALN_RIGHT, '\0', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 53  "SECURITY RELATED CONTROL INFORMATION" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD}, /* 54  "ADDITIONAL AMOUNTS" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPBBinaryPack_BCD, PubFPBBinaryUnpack_BCD}, /* 55  "RESERVED ISO" */
	{99, LEN_LLVAR, ALN_LEFT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD},   /* 56  "RESERVED ISO" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD}, /* 57  "RESERVED NATIONAL" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD}, /* 58  "RESERVED NATIONAL" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPACharPack_BCD, PubFPACharUnpack_BCD}, /* 59  "RESERVED NATIONAL" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD}, /* 60  "RESERVED PRIVATE" */
	{999, LEN_LLLVAR, ALN_LEFT, ' ', PubFPBCharPack_BCD, PubFPBCharUnpack_BCD}, /* 61  "RESERVED PRIVATE" */
	{999, LEN_LLLVAR, ALN_LEFT, '\0', PubFPBBinaryPack_BCD, PubFPBBinaryUnpack_BCD},    /* 62  "RESERVED PRIVATE" */
	//BCTC直联检测中POSP测试发现，后台返回63域格式错，故如果63域出错则忽略  2010-5-18 liuxiling
	{999, LEN_LLLVAR, ALN_LEFT, '\0', PubFPACharPack_BCD, PubFPACharUnpack_BCD/*PubFPACharUnpack_BCD*/}, /* 63  "RESERVED PRIVATE" */
	{8, LEN_FIX, ALN_LEFT, '\0', PubFPBBinaryPack_BCD, PubFPBBinaryUnpack_BCD}, /* 64  "MESSAGE AUTHENTICATION CODE FIELD" */
};


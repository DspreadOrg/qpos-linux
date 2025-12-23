
#include "appinc.h"

#define PR_ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))
static char s_ResponseCodeTable[][RESPONSE_CODE_LENGTH] =
{
	{'0', '0'},
	{'0', '1'},
	{'0', '3'},
	{'0', '4'},
	{'0', '5'},
	{'1', '0'},
	{'1', '1'},
	{'1', '2'},
	{'1', '3'},
	{'1', '4'},
	{'1', '5'},
	{'2', '1'},
	{'2', '2'},
	{'2', '5'},
	{'3', '0'},
	{'3', '1'},
	{'3', '4'},
	{'3', '8'},
	{'4', '0'},
	{'4', '1'},
	{'4', '3'},
	{'4', '5'},    
	{'5', '1'},
	{'5', '4'},
	{'5', '5'},
	{'5', '7'},
	{'5', '8'},
	{'5', '9'},
	{'6', '1'},
	{'6', '2'},
	{'6', '4'},
	{'6', '5'},
	{'6', '6'},
	{'6', '8'},
	{'7', '5'},
	{'9', '0'},
	{'9', '1'},
	{'9', '2'},
	{'9', '4'},
	{'9', '6'},
	{'9', '7'},
	{'9', '8'},
	{'9', '9'},
	{'A', '0'},
	{'A', '1'},
	{'A', '2'},
	{'A', '3'},
	{'A', '4'},
	{'A', '5'},
	{'A', '6'},
	{'A', '7'},
};

#define MAX_HINT_STRING_LENGTH	128
static PR_INT8 s_ResopnseCodeHints [][MAX_HINT_STRING_LENGTH]=
{
	"Successful trade",
	"Please ask the cardholder to contact the issuing bank",
	"Invalid merchant",
	"This card is invalid(POS)",
	"Cardholder authentication failed",
	"Transaction successful but partially accepted",
	"Transaction successful, VIP customer",
	"Invalid transaction",
	"Invalid amount",
	"Invalid card number",
	"This card does not have a corresponding issuer",
	"The card has not been initialized or is sleeping",
	"Incorrect operation or exceeding the allowed trading days",
	"No original transaction, please contact the issuing bank",
	"Please try again",
	"This card cannot be accepted",
	"Cheating card, card retention",
	"The number of password errors has exceeded the limit. Please contact the issuing party",
	"Transaction types not supported by the issuing party",
	"Report lost card(POS)",
	"Stolen card(POS)",
	"Please use a chip",    
	"Insufficient available balance",
	"The card has expired",
	"Password Error",
	"This card transaction is not allowed",
	"The issuing party does not allow the card to conduct this transaction on this terminal",
	"Card verification error",
	"Transaction amount exceeds the limit",
	"Restricted card",
	"The transaction amount does not match the original transaction",
	"Exceeding the limit of consumption times",
	"Transaction failed, please contact the acquiring bank or UnionPay",
	"Transaction timeout, please try again",
	"Password error count exceeds limit",
	"System daily shutdown, please try again later",
	"The issuer's status is abnormal, please try again later",
	"The issuing party's line is abnormal, please try again later",
	"Reject, duplicate transaction, please try again later",
	"Rejected, exchange center exception, please try again later",
	"Terminal not registered",
	"Card issuer timeout",
	"PIN format error, please check in again",
	"MAC verification error, please check in again",
	"Inconsistent transfer currency",
	"Transaction successful, please confirm with the fund transfer bank",
	"The funds received into the bank account are incorrect",
	"Transaction successful, please confirm with the receiving bank for the funds",
	"Transaction successful, please confirm with the receiving bank for the funds",
	"Transaction successful, please confirm with the receiving bank for the funds",
	"Security processing failed",
};

void GetResponseCodeHint(PR_INT8* theResponseCode, PR_INT8* theHint)
{
	PR_UINT32 hintIndex;

	for(hintIndex = 0; hintIndex < PR_ARRAY_SIZE(s_ResponseCodeTable); hintIndex++)
	{
		if (0==memcmp(s_ResponseCodeTable[hintIndex], theResponseCode, RESPONSE_CODE_LENGTH))
		{
			break;
		}
	}

	if (hintIndex==PR_ARRAY_SIZE(s_ResponseCodeTable))
	{
// The corresponding response code cannot be found, and the "Unknown Response Code" is directly prompted
		strcpy(theHint, "Unknown response code");
	}
	else
	{
		strcpy(theHint, s_ResopnseCodeHints[hintIndex]);
	}
}


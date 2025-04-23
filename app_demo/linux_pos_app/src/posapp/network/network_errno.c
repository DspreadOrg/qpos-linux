
#include "appinc.h"

const socket_errno_map_t  errno_message_map[] = {
    {SOCK_ERR_STEP_PARA ,         "SOCKET parameter error"   },
    {SOCK_ERR_STEP_GETHOSTBYNAME, "Domain name resolution error"     },
    {SOCK_ERR_STEP_CREATESOCKET , "Create SOCKET error"   },
    {SOCK_ERR_STEP_CONNECT ,      "Failed to connect to server"   },
    {SOCK_ERR_STEP_SEND ,         "Data send failed"     },
    {SOCK_ERR_STEP_RECV ,         "Data receive failed"     },
	{SOCK_ERR_STEP_PARSE ,        "Failed to receive data parsing" },

    {SOCK_ERR_STEP_SSL_INIT,      "SSL init failed"    },
    {SOCK_ERR_STEP_SSL_CONNECT ,  "SSL connect error"      },
    {SOCK_ERR_STEP_SSL_SEND ,     "SSL send failed"      },
    {SOCK_ERR_STEP_SSL_RECV ,     "SSL receive failed"      },

    {SOCK_ERR_CURL_PARA,          "HTTP:Parameter error"      },
    {SOCK_ERR_CURL_INIT,          "HTTP:init error"    },
    {SOCK_ERR_CURL_PERFORM,       "HTTP:"            },
};

/*
 * function: err_sock_errno
 * description: Modify error return code
 * The high nibble is step, the low nibble is errno
 * author:
 * return:
 */
int err_sock_errno (unsigned short step, int err)
{
    int ret = ((step<<16)|(unsigned short)err);
    return -ret;
}		/* -----  end of function err_sock_errno  ----- */

/*
 * Function description:
 * The error code for communication error is converted to the error code for mis
 * Input parameters: errCode: Error code
 * Output parameters:
 * Return value: MISPOS_E_PARAM_ERR://parameter is incorrect
 * MISPOS_E_SEND: Failed to send data
 * MISPOS_E_RECV: Data reception failed
 * MISPOS_E_PARSE: Data parsing failed
 * MISPOS_E_CONNECTERR: Failed to connect to the server, communication error
 */
int err_sock_errno_mis_error_code(int errCode)
{
	unsigned short step = HHALFW(abs(errCode));        /* Error steps */

	switch(step)
	{
// Incorrect parameters
	case SOCK_ERR_STEP_PARA://SOCKET parameter error
	case SOCK_ERR_CURL_PARA://HTTP:parameter error
		return MISPOS_E_PARAM_ERR;
// Failed to send data
	case SOCK_ERR_STEP_SEND:        //send failed
	case SOCK_ERR_STEP_SSL_SEND:	//SSL send failed
		return MISPOS_E_SEND;
// Data reception failed
	case SOCK_ERR_STEP_RECV:		//receov failed
	case SOCK_ERR_STEP_SSL_RECV:	//SSL receive failed
		return MISPOS_E_RECV;
// Data parsing failed
	case SOCK_ERR_STEP_PARSE:		//Failed to receive data parsing
		return MISPOS_E_PARSE;
	default:
// Failed to connect to the server, communication error
		return MISPOS_E_CONNECTERR;
	}
}



#include "appinc.h"

const socket_errno_map_t  errno_message_map[] = {
    {SOCK_ERR_STEP_PARA ,         "SOCKET参数错误"   },
    {SOCK_ERR_STEP_GETHOSTBYNAME, "域名解析错误"     },
    {SOCK_ERR_STEP_CREATESOCKET , "创建SOCKET错误"   },
    {SOCK_ERR_STEP_CONNECT ,      "连接服务器失败"   },
    {SOCK_ERR_STEP_SEND ,         "数据发送失败"     },
    {SOCK_ERR_STEP_RECV ,         "数据接收失败"     },
	{SOCK_ERR_STEP_PARSE ,        "接收数据解析失败" },

    {SOCK_ERR_STEP_SSL_INIT,      "SSL INIT ERROR"    },
    {SOCK_ERR_STEP_SSL_CONNECT ,  "SSL CONNECT ERROR"      },
    {SOCK_ERR_STEP_SSL_SEND ,     "SSL SEND ERROR"      },
    {SOCK_ERR_STEP_SSL_RECV ,     "SSL RECV ERROR"      },

    {SOCK_ERR_CURL_PARA,          "HTTP:PARAM ERROR"      },
    {SOCK_ERR_CURL_INIT,          "HTTP:INIT ERROR"    },
    {SOCK_ERR_CURL_PERFORM,       "HTTP:"            },
};

/* 
 * function: err_sock_errno
 * description:   修改错误返回码
 *                高半字节为step，低半字节为errno
 * author: 
 * return:
 */
int err_sock_errno (unsigned short step, int err)
{
    int ret = ((step<<16)|(unsigned short)err);
    return -ret;
}		/* -----  end of function err_sock_errno  ----- */

/*********************************************************************************
* 功能描述:
		通讯错误的错误码转换成mis的错误码
* 输入参数:errCode：错误码
* 输出参数：
* 返回值：	MISPOS_E_PARAM_ERR：//参数错
			MISPOS_E_SEND:发送数据失败
			MISPOS_E_RECV：数据接收失败
			MISPOS_E_PARSE：数据解析失败
*    	    MISPOS_E_CONNECTERR：连接服务器失败、通讯错
*********************************************************************************/
int err_sock_errno_mis_error_code(int errCode)
{
	unsigned short step = HHALFW(abs(errCode));        /* 错误步骤 */

	switch(step)
	{
		//参数错
	case SOCK_ERR_STEP_PARA://SOCKET参数错误
	case SOCK_ERR_CURL_PARA://HTTP:参数错
		return MISPOS_E_PARAM_ERR;
		//发送数据失败
	case SOCK_ERR_STEP_SEND:        //数据发送失败
	case SOCK_ERR_STEP_SSL_SEND:	//SSL发送失败
		return MISPOS_E_SEND;
		//数据接收失败
	case SOCK_ERR_STEP_RECV:		//数据接收失败
	case SOCK_ERR_STEP_SSL_RECV:	//SSL接收失败
		return MISPOS_E_RECV;
		//数据解析失败
	case SOCK_ERR_STEP_PARSE:		//接收数据解析失败
		return MISPOS_E_PARSE;
	default:
		//连接服务器失败、通讯错
		return MISPOS_E_CONNECTERR;
	}
}


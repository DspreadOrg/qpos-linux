#ifndef _PROJ_CFG_H
#define _PROJ_CFG_H

/*-----------------------------------------------------------------------------
|   DEBUG
+----------------------------------------------------------------------------*/
#if 1
#define CFG_DBG
#endif

#if defined(CFG_DBG)

#define TRANS_DBG
#define DBG_TRACE_LVL            (TRACE_LVL_INFO)

#endif

#if 0
#define CFG_LVGL_ENABLE
#endif


#define SERVER_HOST_URI "/"
#define SERVER_HOST "www.baidu.com"
#define SERVER_PORT 443

#if 0
#define CFG_LOCAL_TEST   //本地测试，发布时必须关闭
#endif

#define APP_VERSION "1.0.0"

#endif /* #ifndef _PROJ_CFG_H */

/// 3023765165   ana.parra@bold.co    1094897992 1126420409 
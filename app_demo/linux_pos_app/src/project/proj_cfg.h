#ifndef _PROJ_CFG_H
#define _PROJ_CFG_H

/*-----------------------------------------------------------------------------
|   DEBUG
+----------------------------------------------------------------------------*/
#if 1
#define CFG_DBG
#endif

#if 0
#define CFG_LOCAL_TEST   //本地测试，发布时必须关闭
#endif

#define APP_VERSION "V1.0.1"

#ifdef CFG_DBG

static inline void getInfo( const char* module, int line, const char* func ){
    OsLog(LOG_DEBUG, "\rDspread: %s | %s | %d | ", module, func,line);
}

#define DSP_Debug()  do{ getInfo(__FILE__, __LINE__, __FUNCTION__);}while(0)
#define DSP_Info(fmt, ...)  do{ getInfo(__FILE__, __LINE__, __FUNCTION__); OsLog(LOG_DEBUG,fmt,##__VA_ARGS__);}while(0)

#else
	#define DSP_Debug() ((void)0)
	#define DSP_Info(fmt, ...)  ((void)0)
#endif

#endif /* #ifndef _PROJ_CFG_H */

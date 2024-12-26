#pragma once

/*常见的头文件*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h> 
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netdb.h>  

#include <openssl/rsa.h>  
#include <openssl/pem.h>  
#include <openssl/err.h> 
#include <openssl/bn.h> 
#include <openssl/ssl.h>

#include "typedefine.h"
#include "Disp.h"
#include "osal.h"

#include "debug.h"
#include "function.h"
#include "cmacro.h"

#include "proj_define.h"
#include "proj_sdk.h"
#include "proj_cfg.h"

//#include "app_db.h"
#include "app_emv.h"
#include "lib_common.h"
//#include "outcome_list.h"
//#include "error_code_list.h"
//#include "emv_kernel.h"
//#include "emv_kernel_ins.h"
//#include "app_emv_disp.h"
//#include "app_common.h"
//#include "app_socket.h"

#include "lvgl.h"
////api
#include "network.h"
#include "network_errno.h"
#include "socket.h"
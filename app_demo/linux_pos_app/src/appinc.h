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
#include "cmacro.h"

#include "proj_define.h"
#include "proj_sdk.h"
#include "proj_cfg.h"


#include "app_emv.h"
#include "tlv_pro.h"
#include "app_common.h"
////api
#include "network.h"
#include "network_errno.h"
#include "socket.h"
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
#include "emvdeviceimpl.h"
#include "TrancProcView.h"
#include "business.h"
#include "utility.h"
#include "emvdeviceimpl.h"

#include "network.h"
#include "socket.h"
#include "network_errno.h"
#include "cmacro.h"
#include "encryption.h"
#include "function.h"
#include "debug.h"
#include "App_iso8583.h"
#include "TLVUtil.h"
#include "businessUtil.h"
#include "ResponseCode.h"
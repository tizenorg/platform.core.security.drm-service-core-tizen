/*
 * Copyright (c) 2000-2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file	drm_intf_tapps.h
 * @brief	This file includes declarations of the drm TAPPS intf APIs.
 */

#ifndef __DRM_INTF_TAPPS_H__
#define __DRM_INTF_TAPPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <glib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include "db-util.h"

#include "drm-tizen-mid.h"
#include <openssl/aes.h>

/* Enable this flag during development time */
#define _TAPPS_DEBUG_ENABLE_

/* IMEI and other VCONF Paths */

#define DTAPPS_DUID_SIZE 32

#if  0
#define DTAPPS_IMEI_PATH VCONFKEY_TELEPHONY_IMEI
#endif

/*************************************************************************************************************/
typedef bool BOOL;

/* Constants */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

/*************************************************************************************************************/
/* DB Details */

/* drm db schema name */
#define DTAPPS_DB_NAME DB_PATH

typedef enum {
   TAPPSDB_TYPE_NONE,
   TAPPSDB_TYPE_INT,
   TAPPSDB_TYPE_DATETIME,
   TAPPSDB_TYPE_CHAR,
   TAPPSDB_TYPE_VARCHAR,
   TAPPSDB_TYPE_BINARY,
   TAPPSDB_TYPE_BLOB,
   TAPPSDB_TYPE_UNKNOWN = 0xFFFFFFFF
} TAPPSDbType;

/* Enable this Flag if CEK is to be stored in DB */
#define DTAPPS_STORE_CEK_IN_DB

/*************************************************************************************************************/

/* Logs and Macros */

#define MID_DRM 200
#define MID_EXCEPTION 100

#define MCATCH_B _M_catch:

#define MTHROW_B   \
{                  \
	goto _M_catch; \
}

#define MTHROW_BL            \
{                            \
	M_LOGLINE("Throw error") \
	goto _M_catch;           \
}


#define MCFG_DISPLAY_ERROR_LINE

#define DRM_LINUX_SYSDEBUG(dbg_lvl, FMT, ARG...) \
{                                                \
	if ((dbg_lvl) == MID_EXCEPTION)              \
		DRM_TAPPS_EXCEPTION(FMT, ##ARG);         \
	else if((dbg_lvl) == MID_DRM)                \
		DRM_TAPPS_LOG(FMT, ##ARG);               \
	else                                         \
		DRM_TAPPS_FRQ_LOG(FMT, ##ARG);           \
}

#ifdef MCFG_DISPLAY_ERROR_LINE
#define M_LOGLINE(log_prefix) DRM_LINUX_SYSDEBUG(MID_EXCEPTION, "(%s)[Statement] "#log_prefix"", __func__)
#else
#define M_LOGLINE(log_prefix)
#endif // MCFG_DISPLAY_ERROR_LINE

#define MTRY_BL(f)                     \
{                                      \
	if ((f) == 0) {                    \
		M_LOGLINE("Try catched("#f")") \
		goto _M_catch;                 \
	}                                  \
}

/*************************************************************************************************************/

/* New and Delete */

#include <new>
#define NEW new (std::nothrow)

#define MDELETE(x) \
{                  \
	if (x) {       \
		delete x;  \
		x=0;       \
	}              \
}

#define MDELETES(x)  \
{                    \
	if (x) {         \
		delete [] x; \
		x=0;         \
	}                \
}

#define MDELETE0 MDELETE

#define MDELETES0 MDELETES

/*************************************************************************************************************/
/* Mutex structure */
typedef pthread_mutex_t dtapps_mutex_t;

#define dtappslockarg  dtapps_mutex_t*

int dtapps_mutex_lock (dtappslockarg dtapps_mutex);
int dtapps_mutex_unlock (dtappslockarg dtapps_mutex);

/*************************************************************************************************************/
/* Memory Operations Wrapper APIs */

#define DTAPPS_MEMSET memset
#define DTAPPS_MEMCPY memcpy
#define DTAPPS_MALLOC malloc
#define DTAPPS_MEMCPY memcpy
#define DTAPPS_MEMCMP memcmp
#define DTAPPS_FREE free

/*************************************************************************************************************/

/* Time Wrapper APIs */

#define DTAPPS_TIME time
#define DTAPPS_MKTIME mktime
#define DTAPPS_GMTIME gmtime
#define DTAPPS_GMTIME_THREAD_SAFE gmtime_r
#define DTAPPS_ASCTIME asctime

/* String Operations Wrapper APIs */
#ifdef _TAPPS_DEBUG_ENABLE_
unsigned int TAPPS_gstrlcat(char* s1,const char* s2,unsigned int destsize,const char* funName, unsigned int lineno);
unsigned int TAPPS_gstrlcpy(char* s1,const char* s2,unsigned int destsize,const char* funName, unsigned int lineno);
unsigned int TAPPS_strlen(const char* s,const char* funName, unsigned int lineno);

#define TAPPS_GSTRLCPY(dest,src,LenDest) TAPPS_gstrlcpy(dest,src,LenDest,__func__,__LINE__)
#define TAPPS_GSTRLCAT(dest,src,LenDest) TAPPS_gstrlcat(dest,src,LenDest,__func__,__LINE__)
#define TAPPS_STRLEN(string) TAPPS_strlen(string,__func__,__LINE__)
#else
#define TAPPS_GSTRLCPY(dest,src,LenDest) g_strlcpy(dest, src, LenDest)
#define TAPPS_GSTRLCAT(dest,src,LenDest) g_strlcat(dest, src, LenDest)
#define TAPPS_STRLEN(string) strlen(string)
#endif

#define DTAPPS_SNPRINTF snprintf
#define DTAPPS_ATOI atoi
#define DTAPPS_STRNCMP strncmp

int TAPPS_strnicmp(const char * s1, const char * s2, unsigned int sz);
/*************************************************************************************************************/
/* SLEEP API */

void dtapps_sleep(unsigned int TimeInSec,unsigned int TimeInMicroSec);

/*************************************************************************************************************/

/*************************************************************************************************************/
/* OPENSSL APIs */
#define DTAPPS_AES_SET_ENCR_KEY AES_set_encrypt_key
#define DTAPPS_AES_SET_DECR_KEY AES_set_decrypt_key
#define DTAPPS_AES_WRAP_KEY AES_wrap_key
#define DTAPPS_AES_UNWRAP_KEY AES_unwrap_key

/*************************************************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __DRM_INTF_TAPPS_H__ */

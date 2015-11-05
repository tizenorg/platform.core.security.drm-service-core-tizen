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
 * @file	DTapps2Rights.h
 * @brief	This file includes declarations and classes relating to Rights.
 */

#ifndef __DTAPPS_RIGHTS_H__
#define __DTAPPS_RIGHTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drm_intf_tapps.h"

#define DTAPPS_NAME_SIZE	512
#define DTAPPS_CEK_SIZE		32
#define DTAPPS_CID_SIZE		1024

typedef enum
{
	DTAPPS_INSTALL,
	DTAPPS_VERIFY,
} DTAPPS_OPERATION;

typedef struct
{
	unsigned char DUID[DTAPPS_DUID_SIZE + 1];
} DTAPPS_CONSTRAINTS;

/*
typedef enum
{
	DTAPPS_LIC_VALID = 1,
	DTAPPS_LIC_NO_LICENCE,
	DTAPPS_LIC_DUID_MISMATCH,
	DTAPPS_LIC_UNKNOWN_ERROR = -1,
	DTAPPS_LIC_DB_ERROR = -998,
} int;
*/

typedef struct
{
    int r_id;
    char name[512];
    char cid[1024];
    char time[64];// enough size
    #ifdef DTAPPS_STORE_CEK_IN_DB
    char cek[128];
    char cek_hash[64];
    #endif
    char constraint_buffer[512];
    char constraint_hash[64];
} DTAPPS_RIGHTS_ROW;

int DTappsValidateConstraints(DTAPPS_CONSTRAINTS* st_const, DTAPPS_OPERATION opr);
int DTappsInstallLicense(const char* declicbuffer);
int DTappsHasValidLicense(const char* szCid);
int DTappsGetROInfo(const char* pszXML, T_RO *t_RO, unsigned char* name);
int DTappsCalcEval(DTAPPS_CONSTRAINTS* st_const);
BOOL DTappsGetCEK(const char* szCid,T_RO* t_RO);

/* Declaration for Table Handling */
BOOL DTapps_RIGHTS_INSTALL(const char* name,
                          const char* time,
                          #ifdef DTAPPS_STORE_CEK_IN_DB
                          const char* cek,
                          const char* cek_hash,
                          #endif
                          const char* constraint_buffer,
                          const char* constraint_hash,
                          const char* cid,
                          BOOL isUpdate);

BOOL DTapps_RIGHTS_SELECT_ONE(const char* cid, DTAPPS_RIGHTS_ROW* row);

#ifdef __cplusplus
}
#endif

#endif /*__DTAPPS_RIGHTS_H__ */

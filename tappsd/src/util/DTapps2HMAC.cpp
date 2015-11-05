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
 * @file	DTapps2HMAC.cpp
 * @brief	This file includes functions relating to HMAC.
 * @author	Harsha Shekar (h.shekar@samsung.com)
 */
#include <dukgen.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "DTapps2HMAC.h"

void DTappsCalHMACSHA1(unsigned char* key,int key_len,unsigned char* msg,size_t msglen,unsigned char *md,unsigned int *md_len)
{
	HMAC(EVP_sha1(),(void*)key,key_len,msg,msglen,md,md_len);
}

int DTappsGetDeviceKey(unsigned char **pDevKey,unsigned int *DevKeyLen)
{
	int				ret = 0;
	char			passwd[30] = {0,};
	unsigned int	LocalKeyLen = 16;

	char* pDuk = GetDeviceUniqueKey(passwd, strlen(passwd), 16);

	if (!pDuk || !pDevKey || !DevKeyLen)
	{
		DRM_TAPPS_EXCEPTION("pDevKey = %p, DevKeyLen = %p",pDevKey,DevKeyLen);
		goto DTApps_ERR;
	}

	DRM_TAPPS_LOG("LocalKeyLen = %u", LocalKeyLen);

	*pDevKey = (unsigned char*)DTAPPS_MALLOC(LocalKeyLen + 1);

	if(NULL == *pDevKey)
	{
		DRM_TAPPS_EXCEPTION("Malloc Failed");

		goto DTApps_ERR;
	}

	DTAPPS_MEMSET(*pDevKey, 0x0, LocalKeyLen + 1);
	DTAPPS_MEMCPY(*pDevKey, pDuk, LocalKeyLen);

	*DevKeyLen = LocalKeyLen;
	ret = 1;

DTApps_ERR:

	if (ret == 1)
		DRM_TAPPS_LOG("SUCCESS:ret=%d", ret);
	else
		DRM_TAPPS_EXCEPTION("FAILED:ret=%d", ret);

	free(pDuk);

	return ret;
}

void DTappsFreeDeviceKey(unsigned char **pDevKey)
{
	if(pDevKey == NULL)
	{
		DRM_TAPPS_EXCEPTION("pDevKey = %p", pDevKey);

		return;
	}

	DTAPPS_FREE(*pDevKey);
	*pDevKey = NULL;
}

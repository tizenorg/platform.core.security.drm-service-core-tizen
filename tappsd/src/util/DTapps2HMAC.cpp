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
#include "DTapps2HMAC.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <SecCryptoSvc.h>

void DTappsCalHMACSHA1(unsigned char* key,int key_len,unsigned char* msg,size_t msglen,unsigned char *md,unsigned int *md_len)
{
	HMAC(EVP_sha1(),(void*)key,key_len,msg,msglen,md,md_len);
}

int DTappsGetDeviceKey(unsigned char **pDevKey, unsigned int *pDevKeyLen)
{
	if (pDevKey == NULL || pDevKeyLen == NULL) {
		DRM_TAPPS_EXCEPTION("Invalid parameters on DTappsGetDeviceKey.");
		return 0;
	}

	constexpr int KeyLen = 16;
	char passwd[30] = {0,};
	unsigned char *key = NULL;
	int ret = cs_derive_key_with_pass(passwd, strlen(passwd), KeyLen, &key);
	if (ret != CS_ERROR_NONE || key == NULL) {
		DRM_TAPPS_EXCEPTION("cs_derive_key_with_pass ret[%d]", ret);
		free(key);
		return 0;
	}

	*pDevKey = key;
	*pDevKeyLen = static_cast<unsigned int>(KeyLen);

	return 1;
}

void DTappsFreeDeviceKey(unsigned char **pDevKey)
{
	if (pDevKey == NULL)
		return;

	DTAPPS_FREE(*pDevKey);
	*pDevKey = NULL;
}

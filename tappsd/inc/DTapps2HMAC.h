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
 * @file	DTapps2HMAC.h
 * @brief	This file includes declarations relating to HMAC.
 */

#ifndef __DTAPPS_HMAC_H__
#define __DTAPPS_HMAC_H__

#ifdef __cplusplus
	 extern "C" {
#endif

#include "drm_intf_tapps.h"

void DTappsCalHMACSHA1(unsigned char* key,int key_len,unsigned char* msg,size_t msglen,unsigned char *md,unsigned int *md_len);
int DTappsGetDeviceKey(unsigned char **pDevKey,unsigned int *DevKeyLen);
void DTappsFreeDeviceKey(unsigned char **pDevKey);

#ifdef __cplusplus
}
#endif

#endif /*__DTAPPS_HMAC_H__ */

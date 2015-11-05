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
 * @file	DTapps2Base64.h
 * @brief	This file includes declarations for Base64 Encode Decode APIs.
 */

#ifndef __DTAPPS_BASE64_H__
#define __DTAPPS_BASE64_H__

#ifdef __cplusplus
	 extern "C" {
#endif

#include "drm_intf_tapps.h"

BOOL DTappsB64Encode(unsigned char* in, int in_size, unsigned char* out, int out_size);
int DTappsB64Decode(unsigned char* in, int in_size, unsigned char* out, int& out_size);

#ifdef __cplusplus
}
#endif


#endif /*__DTAPPS_BASE64_H__ */

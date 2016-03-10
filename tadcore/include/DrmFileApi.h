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

#ifndef __DRM_FILE_API_H__
#define __DRM_FILE_API_H__


#include "DrmFileMgr.h"
#include "TADC_ErrorCode.h"

/*
enum
{
	DRM_SEEK_SET = 0,
	DRM_SEET_CUR,
	DRM_SEEK_END
} DRM_SEEK_TYPE
*/

//typedef int DrmHandler;

int DrmTdcFileOpen(const char* filePath, int* handle);

int DrmTdcFileClose(int* handle);

int DrmTdcFileRead(int* handle, void* pBuf, long long bufLen, long long* pReadLen);

//int DrmTdcFileSeek(int* handle, DRM_SEEK_TYPE type, long long offset);

int DrmTdcFileSeek(int* handle, long long offset, int origin);

int DrmTdcFileTell(int* handle, long long *position);

#endif

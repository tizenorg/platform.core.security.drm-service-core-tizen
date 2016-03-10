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

#include "DrmFileMgr.h"
#include "TADC_ErrorCode.h"
#include "drm_intf_tapps.h"

EXPORT_API
int DrmTdcFileOpen(const char* filePath, int* handle)
{
	int nRet = TADC_SUCCESS;
	int key = 0;

	DrmFileMgr* pDrmFileMgr = NULL;

	if (filePath == NULL || strlen(filePath) < 1)
	{
		DRM_TAPPS_EXCEPTION("Parameter error!");
		return TADC_PARAMETER_ERROR;
	}

	pDrmFileMgr = DrmFileMgr::GetInstance();
	if (pDrmFileMgr == NULL)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetInstance() error!");
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	nRet = pDrmFileMgr->OpenFileHandler(filePath, &key);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::OpenFileHandler() error!");
		return nRet;
	}

	*handle = key;

	return nRet;
}

EXPORT_API
int DrmTdcFileClose(int* handle)
{
	int nRet = TADC_SUCCESS;
	int key = *handle;

	DrmFileMgr *pDrmFileMgr = DrmFileMgr::GetInstance();
	if (pDrmFileMgr == NULL)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetInstance() error!");
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	nRet = pDrmFileMgr->CloseFileHandler(key);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::OpenFileHandler() error!");
		return nRet;
	}

	return nRet;
}

EXPORT_API
int DrmTdcFileRead(int* handle, void* pBuf, long long bufLen, long long* pReadLen)
{
	int nRet = TADC_SUCCESS;
	int key = *handle;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler* pDrmFileHandler = NULL;

	pDrmFileMgr = DrmFileMgr::GetInstance();
	if (pDrmFileMgr == NULL)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetInstance() error!");
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	nRet = pDrmFileMgr->GetFileHandler(key, &pDrmFileHandler);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetFileHandler() error!");
		return nRet;
	}

	nRet = pDrmFileHandler->DrmRead(pBuf, bufLen, pReadLen);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::DrmRead() error!");
		return nRet;
	}
	return nRet;
}

EXPORT_API
int DrmTdcFileTell(int* handle, long long *position)
{
	int nRet = TADC_SUCCESS;
	int key = *handle;
	long long offset = 0;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler* pDrmFileHandler = NULL;

	pDrmFileMgr = DrmFileMgr::GetInstance();
	if (pDrmFileMgr == NULL)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetInstance() error!");
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	nRet = pDrmFileMgr->GetFileHandler(key, &pDrmFileHandler);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetFileHandler() error!");
		return nRet;
	}

	offset = pDrmFileHandler->DrmTell();

	*position = offset;

	return nRet;
}

EXPORT_API
int DrmTdcFileSeek(int* handle, long long offset, int origin)
{
	int nRet = TADC_SUCCESS;
	int key = *handle;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler* pDrmFileHandler = NULL;

	pDrmFileMgr = DrmFileMgr::GetInstance();
	if (pDrmFileMgr == NULL)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetInstance() error!");
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	nRet = pDrmFileMgr->GetFileHandler(key, &pDrmFileHandler);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileMgr::GetFileHandler() error!");
		return nRet;
	}

	nRet = pDrmFileHandler->DrmSeek(offset, origin);
	if (nRet != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmFileHandler::DrmSeek() error!");
		return nRet;
	}

	return nRet;
}

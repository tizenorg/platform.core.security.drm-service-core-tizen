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

#include <time.h>

#include <new>
#include <cstring>

#include "DrmFileMgr.h"
#include "TADC_ErrorCode.h"
#include "drm_intf_tapps.h"

DrmFileMgr *DrmFileMgr::m_pInstance = NULL;

DrmFileMgr::DrmFileMgr(void)
{
}

DrmFileMgr::~DrmFileMgr(void)
{
	for (auto it = m_HandlerMap.begin(); it != m_HandlerMap.end(); ++it)
		if (it->second != NULL)
			delete it->second;
}

DrmFileMgr *DrmFileMgr::GetInstance(void)
{
	if (m_pInstance != NULL)
		return m_pInstance;

	m_pInstance = new (std::nothrow) DrmFileMgr();
	if (m_pInstance == NULL) {
		DRM_TAPPS_EXCEPTION("Creating DrmFileMgr m_pInstance failed.");
		return NULL;
	}

	m_pInstance->Construct();

	DRM_TAPPS_LOG("m_pInstance is successfully created.");

	return m_pInstance;
}

void DrmFileMgr::FreeInstance(void)
{
	if (m_pInstance != NULL) {
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void DrmFileMgr::Construct(void)
{
	srand(time(NULL));
}

int DrmFileMgr::OpenFileHandler(const char *filePath, int *key)
{
	//DRM_TAPPS_LOG("%s) started. key=%d", __func__, *key);

	if (filePath == NULL || strlen(filePath) < 1) {
		DRM_TAPPS_EXCEPTION("Parameters NULL!");
		return TADC_PARAMETER_ERROR;
	}

	auto pDrmFileHandler = new (std::nothrow) DrmFileHandler();
	if (pDrmFileHandler == NULL) {
		DRM_TAPPS_EXCEPTION("DrmFileHandler allocation failed.");
		return TADC_MEMAlOC_ERROR;
	}

	if (pDrmFileHandler->Construct(filePath) != TADC_SUCCESS) {
		delete pDrmFileHandler;
		DRM_TAPPS_EXCEPTION("DrmFileHandler Construct failed.");
		return TADC_MEMAlOC_ERROR;
	}

	unsigned int seed = time(NULL);
	int tmpKey = rand_r(&seed);

	m_HandlerMap.emplace(tmpKey, pDrmFileHandler);

	*key = tmpKey;

	//SECURE_LOGI("FileHandler is successfully created. key=%d, file=%s", *key, filePath);

	return TADC_SUCCESS;
}

int DrmFileMgr::CloseFileHandler(int key)
{
	//DRM_TAPPS_LOG("%s) started. key=%d", __func__, key);

	if (m_HandlerMap.count(key) == 0) {
		DRM_TAPPS_EXCEPTION("DrmFileHandler is not exist. key=%d", key);
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	auto &pHandler = m_HandlerMap[key];
	if (pHandler != NULL)
		delete pHandler;

	m_HandlerMap.erase(key);

	//DRM_TAPPS_LOG("%s) FileHandler is successfully closed. key=%d", __func__, key);

	return TADC_SUCCESS;
}

int DrmFileMgr::GetFileHandler(int key, DrmFileHandler **handler)
{
	//DRM_TAPPS_LOG("%s) started. key=%d", __func__, key);

	if (m_HandlerMap.count(key) == 0) {
		DRM_TAPPS_EXCEPTION("DrmFileHandler is not exist. key=%d", key);
		return TADC_GET_FILE_HANDLER_ERROR;
	}

	*handler = m_HandlerMap[key];

	//DRM_TAPPS_LOG("FileHandler is successfully retrived. key=%d, ret=%x", key, ret);

	return TADC_SUCCESS;
}

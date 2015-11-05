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

#include <map>
#include "DrmFileHandler.h"

typedef std::map<int, DrmFileHandler *> DrmHandlerMap;

class DrmFileMgr {
public:
	static DrmFileMgr *GetInstance();
	static void FreeInstance(void);

	int OpenFileHandler(const char *filePath, int mode, int *key);
	int CloseFileHandler(int key);
	int GetFileHandler(int key, DrmFileHandler **handler);

private:
	DrmFileMgr(void);
	virtual ~DrmFileMgr(void);

	void Construct(void);

	static DrmFileMgr *m_pInstance;
	DrmHandlerMap m_HandlerMap;
};

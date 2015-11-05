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

#include <stdio.h>

#include "DrmTdcSvc.h"

class DrmFileHandler
{

public:
	DrmFileHandler(void);
	~DrmFileHandler(void);

	int	Construct(const char* szDrmFilePath);
	int	DrmSeek(long long offset, int origin);
	long long DrmTell(void);
	int	DrmRead(void* pBuf, long long bufLen, long long* pReadLen);

#ifndef TEST_CODE_ENABLED
private:
#endif
	int	DrmDecryptBlocks(void);
	long long GetCurBlockIndex(void);

	unsigned char *m_pFilePath;
	unsigned char *m_pCID;
	unsigned char *m_pCEK;
	unsigned char *m_pDecBuf;

	int	m_PlaintextStartOffset;
	FILE *m_pFP;

	long m_encryptionLevel;
	long long m_encryptionRange;
	long long m_plaintextSize;
	long long m_OriginEndOffset;
	long long m_OriginCurOffset;
	long long m_DrmCurOffset;
	long long m_DrmEndOffset;
	long long m_blockCnt;
	long long m_curBlockIndex;
	long long m_decReadlen;
	long long m_extraReadlen;
};

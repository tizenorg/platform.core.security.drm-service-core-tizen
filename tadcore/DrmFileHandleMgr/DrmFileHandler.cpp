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

#include <memory>

#include "TADC_Core.h"
#include "TADC_ErrorCode.h"
#include "DTapps2Rights.h"
#include "DrmFileHandler.h"
#include "TADC_IF.h"

#define TDC_DECRYPT_BLOCKSIZE 512

namespace {

using FileHeaderPtr = std::unique_ptr<T_FILE_HEADER, int(*)(T_FILE_HEADER *)>;
using DrmHeaderPtr = std::unique_ptr<T_DRM_HEADER, int(*)(T_DRM_HEADER *)>;
using RoPtr = std::unique_ptr<T_RO, int(*)(T_RO *)>;

using BufPtr = std::unique_ptr<unsigned char>;

inline FileHeaderPtr createFileHeaderPtr(T_FILE_HEADER *ptr)
{
	return FileHeaderPtr(ptr, TADC_MEMFree_FileHeader);
}

inline DrmHeaderPtr createDrmHeaderPtr(T_DRM_HEADER *ptr)
{
	return DrmHeaderPtr(ptr, TADC_MEMFree_DRMHeader);
}

inline RoPtr createRoPtr(T_RO *ptr)
{
	return RoPtr(ptr, TADC_MEMFree_RO);
}

inline BufPtr createBufPtr(unsigned char *ptr)
{
	return BufPtr(ptr);
}

}

DrmFileHandler::DrmFileHandler(void)
	: m_pFilePath(nullptr)
	, m_pCID(nullptr)
	, m_pCEK(nullptr)
	, m_pFP(nullptr)
{
}

DrmFileHandler::~DrmFileHandler(void)
{
	if (m_pFilePath != NULL)
		delete[] m_pFilePath;

	if (m_pCID != NULL)
		delete[] m_pCID;

	if (m_pCEK != NULL)
		delete[] m_pCEK;

	if (m_pDecBuf != NULL)
		delete[] m_pDecBuf;

	if (m_pFP != NULL)
		fclose(m_pFP);
}

int DrmFileHandler::Construct(const char* szDrmFilePath)
{
	T_FILE_HEADER t_FileHeader;
	T_DRM_HEADER t_DRMHeader;
	T_RO t_RO;

	// 1. Check the file is TADC DRM file.
	memset(&t_FileHeader, 0x00, sizeof(t_FileHeader));
	memset(&t_DRMHeader, 0x00, sizeof(T_DRM_HEADER));
	memset(&t_RO, 0x00, sizeof(T_RO));

	DRM_TAPPS_LOG("%s starts", __func__);

	if (szDrmFilePath == nullptr) {
		DRM_TAPPS_EXCEPTION("Parameters NULL!");
		return TADC_PARAMETER_ERROR;
	}

	int ret = TADC_GetDRMHeaderFromFile(szDrmFilePath, &t_FileHeader, &t_DRMHeader);
	if (ret < 0) {
		DRM_TAPPS_EXCEPTION("Error : TADC_GetDRMHeaderFromFile() - %s TADC Error Code - %d", szDrmFilePath, ret);
		return TADC_NOTTADCFILE_ERROR;
	}

	auto fileHeaderPtr = createFileHeaderPtr(&t_FileHeader);
	auto drmHeaderPtr = createDrmHeaderPtr(&t_DRMHeader);

	// 2. Validate license and get the CEK for the DRM file
	bool bRet = DTappsGetCEK((char*)t_DRMHeader.CID, &t_RO);
	if (!bRet) {
		DRM_TAPPS_EXCEPTION("Error : DTappsGetCEK() - %s", t_DRMHeader.CID);
		return TADC_GET_CEK_ERROR;
	}

	auto roPtr = createRoPtr(&t_RO);

	// 3. Setting the member variable
	m_pFilePath = new unsigned char[strlen(szDrmFilePath) + 1];
	memset(m_pFilePath, 0x00, strlen(szDrmFilePath) + 1);
	memcpy(m_pFilePath, szDrmFilePath, strlen(szDrmFilePath));

	m_pCID = new unsigned char[strlen((char*)t_DRMHeader.CID) + 1];
	memset(m_pCID, 0x00, strlen((char*)t_DRMHeader.CID) + 1);
	memcpy(m_pCID, t_DRMHeader.CID, strlen((char*)t_DRMHeader.CID));

	m_pCEK = new unsigned char[CEK_SIZE + 1];
	memset(m_pCEK, 0x00, CEK_SIZE + 1);
	memcpy(m_pCEK, t_RO.t_Content.CEK, CEK_SIZE);

	m_encryptionLevel = t_DRMHeader.EncryptionLevel;
	m_encryptionRange = t_DRMHeader.EncryptionRange;
	m_plaintextSize = t_DRMHeader.PlaintextSize;

	// 4. Open the DRM file and set the filepointer to member variable
	m_pFP = fopen(szDrmFilePath, "rb");
	if (m_pFP == nullptr) {
		DRM_TAPPS_EXCEPTION("Error : fopen() - %s", szDrmFilePath);
		return TADC_FILE_OPEN_ERROR;
	}

	m_PlaintextStartOffset = t_FileHeader.Offset1 + 35 + t_DRMHeader.XmlSize;

	fseek(m_pFP, 0, SEEK_END);
	m_OriginEndOffset = ftell(m_pFP);

	m_plaintextSize = m_OriginEndOffset - m_PlaintextStartOffset;
	if (m_plaintextSize != t_DRMHeader.PlaintextSize) {
		DRM_TAPPS_EXCEPTION("Error : plaintext file size incorrect. real = %ld, header = %ld", m_plaintextSize, t_DRMHeader.PlaintextSize);
		return TADC_FILE_OPEN_ERROR;
	}

	m_DrmEndOffset = m_plaintextSize;

	fseek(m_pFP, m_PlaintextStartOffset, SEEK_SET);
	m_OriginCurOffset = ftell(m_pFP);
	m_DrmCurOffset = 0;

	m_blockCnt = (m_plaintextSize / 512) + ((m_plaintextSize % 512) ? 1 : 0);

	m_curBlockIndex = 0;
	m_decReadlen = 0;

	DRM_TAPPS_LOG("m_pCEK = %s", m_pCEK);
	DRM_TAPPS_LOG("m_pCID = %s", m_pCID);
	DRM_TAPPS_LOG("m_pFilePath = %s", m_pFilePath);

	DRM_TAPPS_LOG("m_encryptionLevel = %ld", m_encryptionLevel);
	DRM_TAPPS_LOG("m_encryptionRange = %ld", m_encryptionRange);
	DRM_TAPPS_LOG("m_plaintextSize = %ld", m_plaintextSize);

	DRM_TAPPS_LOG("m_PlaintextStartOffset = %ld", m_PlaintextStartOffset);
	DRM_TAPPS_LOG("m_OriginEndOffset = %ld", m_OriginEndOffset);

	DRM_TAPPS_LOG("m_OriginCurOffset = %ld", m_OriginCurOffset);
	DRM_TAPPS_LOG("m_DrmCurOffset = %ld", m_DrmCurOffset);
	DRM_TAPPS_LOG("m_DrmEndOffset = %ld", m_DrmEndOffset);

	DRM_TAPPS_LOG("m_blockCnt = %ld", m_blockCnt);
	DRM_TAPPS_LOG("m_curBlockIndex = %ld", m_curBlockIndex);

	DrmDecryptBlocks();

	return TADC_SUCCESS;
}

int DrmFileHandler::DrmSeek(long long offset, int origin)
{
	int nRet = TADC_SUCCESS;
	long long OriginOffset = 0;
	long long DrmOffset = 0;
	long long temp = 0;

	m_decReadlen = 0;

	if (origin == SEEK_SET)
	{
		DrmOffset = offset;
		if (DrmOffset < 0 || DrmOffset > m_plaintextSize)
		{
			DRM_TAPPS_EXCEPTION("Parameter Wrong! Offset can not be minus. offset=%lld, m_OriginCurOffset=%lld, m_plaintextSize=%lld", offset, m_OriginCurOffset, m_plaintextSize);
			return TADC_PARAMETER_ERROR;
		}

		OriginOffset = m_PlaintextStartOffset + DrmOffset;
		if (fseek(m_pFP, OriginOffset, SEEK_SET) != 0)
		{
			DRM_TAPPS_EXCEPTION("fseek failed.");
			return TADC_FILE_READ_ERROR;
		}

		m_OriginCurOffset = OriginOffset;
		m_DrmCurOffset = DrmOffset;
	}
	else if (origin == SEEK_CUR)
	{
		temp = m_OriginCurOffset;

		if (temp == -1)
		{
			DRM_TAPPS_EXCEPTION("GetOriginCurOffset() failed.");
			return TADC_FILE_READ_ERROR;
		}

		OriginOffset = temp + offset;
		DrmOffset = OriginOffset - m_PlaintextStartOffset;

		if (DrmOffset < 0 || DrmOffset > m_plaintextSize)
		{
			DRM_TAPPS_EXCEPTION("Parameter Wrong! Offset can not be minus. offset=%lld, m_OriginCurOffset=%lld, OriginOffset=%lld, DrmOffset=%lld, m_plaintextSize=%lld", offset, temp, OriginOffset, DrmOffset, m_plaintextSize);
			return TADC_PARAMETER_ERROR;
		}

		if (fseek(m_pFP, OriginOffset, SEEK_SET) != 0)
		{
			DRM_TAPPS_EXCEPTION("fseek failed.");
			return TADC_FILE_READ_ERROR;
		}

		m_OriginCurOffset = OriginOffset;
		m_DrmCurOffset = DrmOffset;
	}
	else if (origin == SEEK_END)
	{
		OriginOffset = m_OriginEndOffset + offset;
		if (fseek(m_pFP, OriginOffset, SEEK_SET) != 0)
		{
			DRM_TAPPS_EXCEPTION("fseek failed.");
			return TADC_FILE_READ_ERROR;
		}

		DrmOffset = OriginOffset - m_PlaintextStartOffset;
		if (DrmOffset < 0 || DrmOffset > m_plaintextSize)
		{
			DRM_TAPPS_EXCEPTION("Parameter Wrong! Offset can not be minus. offset=%lld, m_OriginCurOffset=%lld, OriginOffset=%lld, DrmOffset=%lld, m_plaintextSize=%lld", offset, temp, OriginOffset, DrmOffset, m_plaintextSize);
			return TADC_PARAMETER_ERROR;
		}

		m_OriginCurOffset = OriginOffset;
		m_DrmCurOffset = DrmOffset;
	}
	else
	{
		DRM_TAPPS_EXCEPTION("Parameter Wrong!");
		return TADC_PARAMETER_ERROR;
	}

	return nRet;
}

long long DrmFileHandler::DrmTell(void)
{
	return m_DrmCurOffset;
//	return GetDrmCurOffset();
}

int DrmFileHandler::DrmDecryptBlocks(void)
{
	int ret = TADC_SUCCESS;
	const char* packagePath = (const char*)m_pFilePath;
	T_FILE_HEADER t_FileHeader;
	T_DRM_HEADER t_DRMHeader;
	T_DEVICE_INFO t_DeviceInfo;
	T_RO t_RO;
	DrmTdcFileHeader fileHeader;

	// 1. Check the file is TADC DRM file.
	memset(&t_FileHeader, 0x00, sizeof(t_FileHeader));
	memset(&t_DRMHeader, 0x00, sizeof(T_DRM_HEADER));
	memset(&t_DeviceInfo, 0x00, sizeof(T_DEVICE_INFO));
	memset(&t_RO, 0x00, sizeof(T_RO));

	bool bRet = DrmTdcGetFileHeader(packagePath, &fileHeader);
	if (bRet == FALSE) {
		DRM_TAPPS_EXCEPTION("DrmDecryptBlocks Error : DrmTdcGetFileHeader()");
		return TADC_GET_FILEHEADER_ERROR;
	}

	bRet = DTappsGetCEK(fileHeader.cid, &t_RO);
	if (bRet == FALSE) {
		DRM_TAPPS_SECURE_EXCEPTION("DrmDecryptBlocks Error : DTappsGetCEK() packagePath=%s, fileHeader.cid=%s", packagePath, fileHeader.cid);
		return TADC_GET_CEK_ERROR;
	}

	auto roPtr = createRoPtr(&t_RO);

	DRM_TAPPS_SECURE_LOG("fileHeader.cid=%s, t_RO.t_Content.CEK=%s", fileHeader.cid, t_RO.t_Content.CEK);

	if ((ret = TADC_SetDeviceInfo(&t_DeviceInfo) ) == TADC_GETDUID_ERROR) {
		DRM_TAPPS_EXCEPTION("DrmDecryptBlocks Error : TADC_SetDeviceInfo(), TADC Error Code - %d", ret);
		return TADC_FILE_READ_ERROR;
	}

	ret = TADC_GetDRMHeaderFromFile(packagePath, &t_FileHeader, &t_DRMHeader);
	if (ret < 0) {
		DRM_TAPPS_EXCEPTION("DrmDecryptBlocks Error : TADC_GetDRMHeaderFromFile() - %s,  TADC Error Code - %d", packagePath, ret);
		return TADC_NOTTADCFILE_ERROR;
	}

	/* resource to be scoped-free */
	auto fileHeaderPtr = createFileHeaderPtr(&t_FileHeader);

	//Get CEK
	if ((ret = TADC_GetCEK(&t_DeviceInfo, &t_RO, &t_DRMHeader)) < 0) {
		DRM_TAPPS_EXCEPTION("DrmDecryptBlocks Error: TADC_GetCEK, TADC Error Code - %d", ret);
		return TADC_GET_CEK_ERROR;
	}

	/* resource to be scoped-free */
	auto drmHeaderPtr = createDrmHeaderPtr(&t_DRMHeader);

	if (fseek(m_pFP, m_PlaintextStartOffset, SEEK_SET) != 0) {
		DRM_TAPPS_EXCEPTION("DrmDecryptBlocks Error :fseek failed.");
		return TADC_FILE_READ_ERROR;
	}

	auto EncBlockCnt = m_blockCnt;
	if (m_encryptionRange != -1)
		EncBlockCnt = m_encryptionRange;

	m_pDecBuf = new (std::nothrow) unsigned char[(EncBlockCnt * TDC_DECRYPT_BLOCKSIZE) + 1];
	if (m_pDecBuf == nullptr) {
		DRM_TAPPS_EXCEPTION("DrmRead Error : m_pDecBuf Memory allocation failed");
		return TADC_MEMAlOC_ERROR;
	}

	auto ReadLen = fread(m_pDecBuf, 1, EncBlockCnt * TDC_DECRYPT_BLOCKSIZE, m_pFP);

	long long l = 0;
	for (size_t k = 0 ; k < ReadLen ; k += 512) {
		if (l < EncBlockCnt) {
			auto DecLen = ReadLen - k;
			DecLen = ( DecLen > 512) ? 512 : DecLen;

			if ((ret = TADC_DecryptBlock((char*)m_pDecBuf + k, DecLen, &t_DRMHeader)) < 0) {
				DRM_TAPPS_EXCEPTION("DrmDecryptBlocks Error : TADC_DecryptBlock, TADC Error Code - %d", ret);
				return TADC_DECRYPT_PACKAGE_ERROR;
			}
		}
		l += 1;
	}

	return ret;
}

int DrmFileHandler::DrmRead(void* pBuf, long long buflen, long long* pReadLen)
{
	auto EncBlockCnt = m_blockCnt;
	if (m_encryptionRange != -1)
		EncBlockCnt = m_encryptionRange;

	if (m_DrmCurOffset > EncBlockCnt * TDC_DECRYPT_BLOCKSIZE) {
		auto pNewReadBuf = BufPtr(new (std::nothrow) TADC_U8[buflen + 1]);
		if (!pNewReadBuf) {
			DRM_TAPPS_EXCEPTION("DrmRead Error : pNewReadBuf Malloc Fail");
			return TADC_MEMAlOC_ERROR;
		}

		auto ReadLen = fread(pNewReadBuf.get(), 1, buflen, m_pFP);

		TADC_IF_MemCpy(pBuf, pNewReadBuf.get(), ReadLen);

		*pReadLen = ReadLen;
	} else {
		if (buflen > EncBlockCnt * TDC_DECRYPT_BLOCKSIZE - m_DrmCurOffset) {
			auto pTempReadBuf = createBufPtr(new (std::nothrow) TADC_U8[buflen + 1]);
			if (!pTempReadBuf) {
				DRM_TAPPS_EXCEPTION("DrmRead Error : pTempReadBuf Malloc Fail");
				return TADC_MEMAlOC_ERROR;
			}

			m_decReadlen = (EncBlockCnt * TDC_DECRYPT_BLOCKSIZE) - m_DrmCurOffset;
			TADC_IF_MemCpy(pTempReadBuf.get(), reinterpret_cast<char *>(m_pDecBuf) + m_DrmCurOffset, m_decReadlen);

			m_extraReadlen = buflen - m_decReadlen;

			if (fseek(m_pFP, m_decReadlen, SEEK_CUR) != 0) {
				DRM_TAPPS_EXCEPTION("DrmRead Error: fseek failed.");
				return TADC_FILE_READ_ERROR;
			}

			auto pNewReadBuf = BufPtr(new (std::nothrow) TADC_U8[m_extraReadlen + 1]);
			if (!pNewReadBuf) {
				DRM_TAPPS_EXCEPTION("DrmRead Error : pNewReadBuf Malloc Fail");
				return TADC_MEMAlOC_ERROR;
			}

			auto ReadLen = fread(pNewReadBuf.get(), 1, m_extraReadlen, m_pFP);

			TADC_IF_MemCpy((char*)pTempReadBuf.get() + m_decReadlen, pNewReadBuf.get(), ReadLen);
			TADC_IF_MemCpy(pBuf, pTempReadBuf.get(), buflen);
		} else {
			if (m_DrmCurOffset == 0)
				TADC_IF_MemCpy(pBuf, reinterpret_cast<char *>(m_pDecBuf) + m_decReadlen, buflen);
			else
				TADC_IF_MemCpy(pBuf, reinterpret_cast<char *>(m_pDecBuf) + m_DrmCurOffset + m_decReadlen, buflen);
			m_decReadlen = m_decReadlen + buflen;
		}

		*pReadLen = buflen;
	}

	return TADC_SUCCESS;
}

long long DrmFileHandler::GetCurBlockIndex(void)
{
	m_curBlockIndex = (m_DrmCurOffset / 512) + ((m_DrmCurOffset % 512) ? 1 : 0);

	return m_curBlockIndex;
}

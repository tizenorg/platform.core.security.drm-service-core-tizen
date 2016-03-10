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
 * @file    TADC_Core.cpp
 * @brief   This file includes implementations of the Tizen Apps DRM Core APIs.
*/

#include "drm-tizen-error.h"
#include "TADC_Util.h"
#include "TADC_Sub.h"
#include "TADC_IF.h"
#include "TADC_ErrorCode.h"

static BYTE g_baSignatureKey[32] = {
	0x29, 0x2b, 0xf2, 0x29, 0x1f, 0x8b, 0x47, 0x81, 0x95, 0x0a, 0x84, 0xf8, 0x91, 0xda, 0x07, 0xd0,
	0x9c, 0xde, 0x32, 0x3e, 0x9e, 0x46, 0x4a, 0xfc, 0xa4, 0xcc, 0x55, 0x6e, 0xf2, 0x81, 0x61, 0xdb
};

static BYTE g_baAESKey[32] = {
	0xf8, 0x87, 0x0a, 0xc5, 0xd3, 0x6d, 0x44, 0x49, 0x03, 0x9f, 0xbd, 0x1e, 0xa8, 0x2f, 0xf6, 0xc3,
	0xdf, 0x3b, 0x02, 0x13, 0x58, 0x1b, 0x12, 0x30, 0x1c, 0xd7, 0xad, 0xa5, 0x1f, 0x5d, 0x01, 0x33
};

//Error Code
static DWORD g_TADCErrorCode = 0;

int TADC_SetDeviceInfo(T_DEVICE_INFO *t_DeviceInfo)
{
	int nResult = 0;

	TADC_IF_MemSet(t_DeviceInfo, 0, sizeof(T_DEVICE_INFO));

// Mandatory value
	nResult = TADC_IF_GetDUID((CHAR*)t_DeviceInfo->DUID);
	IF_ERROR_RETURN(nResult, TADC_GETDUID_ERROR);

	return nResult;
}

int TADC_MakeRequestLicense(T_DEVICE_INFO *t_DeviceInfo,
							T_FILE_HEADER *t_FileHeader,
							T_DRM_HEADER *t_DRMHeader,
							unsigned char * outBuffer,
							size_t outBufferSize)
{
	int nResult = 0;
	size_t length = 0;

	//Param Check
	IF_TRUE_RETURN(outBuffer == NULL, TADC_PARAMETER_ERROR);

	snprintf(
		(char*)outBuffer,
		outBufferSize,
		"<?xml version='1.0'?>\n<request>\n<DeviceInfo>\n<duid>%s</duid>\n</DeviceInfo>\n",
			t_DeviceInfo->DUID);
	length = TADC_IF_StrLen((char*)outBuffer);
	IF_TRUE_RETURN(length <= 0, TADC_XMLPARSER_ERROR);

	//Null Check
	IF_TRUE_RETURN(t_DRMHeader->SID == NULL, TADC_SID_NULL_ERROR);
	IF_TRUE_RETURN(t_DRMHeader->CID == NULL, TADC_CID_NULL_ERROR);

	snprintf(
		(char*)outBuffer + length,
		outBufferSize - length,
		"<ContentInfo>\n<DRMType>%d</DRMType>\n<sid>%s</sid>\n<cid>%s</cid>\n</ContentInfo>\n</request>",
			t_FileHeader->DRMType,
			t_DRMHeader->SID,
			t_DRMHeader->CID);
	length = TADC_IF_StrLen((char*)outBuffer);
	IF_TRUE_RETURN(length <= 0, TADC_XMLPARSER_ERROR);

	DRM_TAPPS_LOG("TADC_MakeRequestLicense Success!\n");

	return nResult;
}

int TADC_GetROAcqInfo(unsigned char * inBuffer, T_ROACQ_INFO *t_ROAcqInfo)
{
	int nResult = -1;
	int i = 0, j = 0;
	int nSize = 0;
	int length = 0;

	//Check Param buffer
	IF_TRUE_RETURN(inBuffer == NULL, TADC_PARAMETER_ERROR);
	nSize = TADC_IF_StrLen((char*) inBuffer);
	IF_TRUE_RETURN( nSize<=40 || nSize>RESP_MAXSIZE, TADC_PARAMETER_ERROR);

	//Check XML Result Code ( Success result='0' )
	nResult = -1;
	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer+i), "<response result=", 17))
		{
			i += 17;
			if ((!memcmp((char*)(inBuffer+i), "'0'", 3)) || (!memcmp((char*)(inBuffer+i), "\"0\"", 3)))
			{
				nResult = 0;
				break;
			}
			else
			{
				nResult = -1;
			}
		}
	}
	IF_TRUE_RETURN(((i == nSize) || (nResult < 0)), TADC_RESPONSEMESSAGE_ERROR);


	//Init
	TADC_IF_MemSet(t_ROAcqInfo, 0, sizeof(T_ROACQ_INFO));

	//Get RIURL
	length = 0;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer + i), "<riurl>", 7))
		{
			i += 7;
			for (j = i ; j < nSize ; j++)
			{
				if (!TADC_IF_MemCmp((char*)(inBuffer + j), "</riurl>", 8))
				{
					length = j - i;
					IF_TRUE_RETURN(length <= 0, TADC_PARAMETER_ERROR);
					t_ROAcqInfo->ROAcqURL = (TADC_U8*)TADC_IF_Malloc(length + 1);
					IF_TRUE_RETURN(t_ROAcqInfo->ROAcqURL == NULL, TADC_MEMAlOC_ERROR);
					TADC_IF_MemSet(t_ROAcqInfo->ROAcqURL, 0, length + 1);
					TADC_IF_MemCpy(t_ROAcqInfo->ROAcqURL, inBuffer + i, length);
					break;
				}
			}
			break;
		}
	}
	IF_TRUE_RETURN(length <= 0, TADC_RESPONSEMESSAGE_ERROR);

	//Get ReqID
	length = 0;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer + i), "<reqid>", 7))
		{
			i += 7;
			for (j = i ; j < nSize ; j++)
			{
				if (!TADC_IF_MemCmp((char*)(inBuffer + j), "</reqid>", 8))
				{
					length = j - i;
					IF_TRUE_RETURN(length <= 0, TADC_PARAMETER_ERROR);
					t_ROAcqInfo->ReqID = (TADC_U8*)TADC_IF_Malloc(length + 1);
					IF_TRUE_RETURN(t_ROAcqInfo->ReqID == NULL, TADC_MEMAlOC_ERROR);
					TADC_IF_MemSet(t_ROAcqInfo->ReqID, 0, length + 1);
					TADC_IF_MemCpy(t_ROAcqInfo->ReqID, inBuffer+i, length);
					break;
				}
			}
			break;
		}
	}
	IF_TRUE_RETURN(length <= 0, TADC_RESPONSEMESSAGE_ERROR);
	DRM_TAPPS_LOG("TADC_GetROAcqInfo Success!\n");

	return nResult;
}

int TADC_MakeRequestRO(T_ROACQ_INFO *t_ROAcqInfo, unsigned char * outBuffer, size_t outBufferSize, unsigned char * ROVer)
{
	BYTE sha1_tmp[20];
	TADC_U8* ReqTemp;
	LPSTR psz64Parameter = NULL;

	TADC_U8 key[16];
	TADC_U8 iv[16];

	int outlen = 0;
	int nResult = 0;

	size_t StrSize = 0;
	size_t len = 0;
	size_t i = 0;
	long k = 0;
	size_t reqdataset_size = 0;

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_MakeRequestRO  : Start    \n");

	//Check Param Buffer
	IF_TRUE_RETURN(outBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_ROAcqInfo->ReqID == NULL, TADC_PARAMETER_ERROR);

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_MakeRequestRO  : After Check Param Buffer   \n");
	len = TADC_IF_StrLen((char*)t_ROAcqInfo->ReqID);
	IF_TRUE_RETURN( (len <= 0 || len > 512), TADC_PARAMETER_ERROR);

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_MakeRequestRO  : After Check t_ROAcqInfo->ReqID Len   \n");
	nResult = TADC_IF_GetDHKey(&t_ROAcqInfo->t_DHInfo);
	IF_ERROR_RETURN(nResult, TADC_GETDHKEY_ERROR);

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_MakeRequestRO  : After TADC_IF_GetDHKey  \n");
	IF_TRUE_RETURN(t_ROAcqInfo->t_DHInfo.pSize <= 0 || t_ROAcqInfo->t_DHInfo.pSize > DHKey_SIZE, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_ROAcqInfo->t_DHInfo.ASize <= 0 || t_ROAcqInfo->t_DHInfo.ASize > DHKey_SIZE, TADC_PARAMETER_ERROR);

	//If ROVer 2.0 then add <reqversion> element. 2011.03.08
	if (ROVer != NULL && ROVer[0] >= 2)
	{
		snprintf(
			(char*)outBuffer,
			outBufferSize,
			"<?xml version='1.0'?>\n<request>\n<reqversion>%d.%d</reqversion>\n<reqid>%s</reqid>\n<reqdataset>",
				ROVer[0],
				ROVer[1],
				t_ROAcqInfo->ReqID);
	}
	else
	{
		snprintf(
			(char*)outBuffer,
			outBufferSize,
			"<?xml version='1.0'?>\n<request>\n<reqid>%s</reqid>\n<reqdataset>",
				t_ROAcqInfo->ReqID);
	}

	len = TADC_IF_StrLen((char*)outBuffer);
	i += len;

	reqdataset_size = i;

	TADC_IF_StrNCpy((char*)outBuffer+i, "p=", 2);
	i += 2;

	for (k = 0 ; k < t_ROAcqInfo->t_DHInfo.pSize ; k++)
	{
		snprintf(
			(char*)outBuffer + i + (k * 2),
			outBufferSize - i - (k * 2),
			"%02X",
				t_ROAcqInfo->t_DHInfo.p[k]);
	}
	i += (k * 2);

	snprintf(
		(char*)outBuffer + i,
		outBufferSize - i,
		";g=%ld",
			t_ROAcqInfo->t_DHInfo.g);
	i += 4;

	TADC_IF_StrNCpy((char*)outBuffer + i, ";A=", 3);
	i += 3;

	for (k = 0 ; k < t_ROAcqInfo->t_DHInfo.ASize ; k++)
	{
		snprintf(
			(char*)outBuffer + i + (k * 2),
			outBufferSize - i - (k * 2),
			"%02X",
				t_ROAcqInfo->t_DHInfo.A[k]);
	}
	i += (k * 2);

	outBuffer[i] = 0;

	StrSize = i - reqdataset_size;

	//MemAlloc
	ReqTemp = (TADC_U8*)TADC_IF_Malloc(StrSize);
	IF_TRUE_RETURN(ReqTemp == NULL, TADC_MEMAlOC_ERROR);

	TADC_IF_MemSet(ReqTemp, 0, StrSize);
	TADC_IF_MemCpy(ReqTemp, outBuffer+reqdataset_size, StrSize);
	TADC_IF_SHA1(ReqTemp, StrSize, sha1_tmp);

	TADC_IF_MemCpy(key, &g_baAESKey[0], 16);
	TADC_IF_MemCpy(iv, &g_baAESKey[16], 16);
	TADC_IF_AES_CTR(key, 16, iv, 20, (unsigned char*)sha1_tmp, &outlen, (unsigned char*)sha1_tmp);

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_MakeRequestRO  : After TADC_IF_AES_CTR  \n");

	psz64Parameter = Base64Encode(sha1_tmp, sizeof(sha1_tmp));	//MemAlloc
	if (psz64Parameter == NULL)
	{
		TADC_IF_Free(ReqTemp);
		return TADC_MEMAlOC_ERROR;
	}

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_MakeRequestRO  : After Base64Encode  \n");

	TADC_IF_StrNCpy((char*)(outBuffer + i), ";hmac=", 6);
	len = TADC_IF_StrLen((char*)psz64Parameter);
	TADC_IF_MemCpy(outBuffer + i + 6, psz64Parameter, len);
	TADC_IF_StrNCpy((char*)outBuffer + i + 6 + len, "</reqdataset>\n</request>", 24);

	//MemFree
	if (ReqTemp != NULL)
	{
		TADC_IF_Free(ReqTemp);
		ReqTemp = NULL;
	}
	if (psz64Parameter != NULL)
	{
		TADC_IF_Free(psz64Parameter);
		psz64Parameter = NULL;
	}

	len = TADC_IF_StrLen((char*)outBuffer);
	IF_TRUE_RETURN(len <= 0, TADC_REQMAKEHMAC_ERROR);

	DRM_TAPPS_LOG("TADC_MakeRequestRO Success!\n");

	return len;
}

int TADC_GetHashReqID(unsigned char * inBuffer, unsigned char *hashReqID)
{
	int i = 0, j = 0, nSize = 0;
	char tmpbuf[512];
	int length = 0;
	int nResult = 0;

	//Check Param Buffer
	IF_TRUE_RETURN(inBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(hashReqID == NULL, TADC_PARAMETER_ERROR);
	nSize = TADC_IF_StrLen((char*)inBuffer);
	IF_TRUE_RETURN(nSize <= 40 || nSize>RESP_MAXSIZE, TADC_PARAMETER_ERROR);

	//Check XML Result Code ( Success result='0' )
	nResult = -1;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer + i), "<response result=", 17))
		{
			i += 17;
			if ((!memcmp((char*)(inBuffer + i), "'0'", 3)) || (!memcmp((char*)(inBuffer + i), "\"0\"", 3)))
			{
				nResult = 0;
				break;
			}
			else
			{
				nResult = -1;
			}
		}
	}
	IF_TRUE_RETURN(((i == nSize) || (nResult < 0)), TADC_RESPONSEMESSAGE_ERROR);

	//Init
	TADC_IF_MemSet(tmpbuf, 0, sizeof(tmpbuf));

	//Get reqid
	length = 0;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp(inBuffer + i, "reqid=", 6))
		{
			i += 6;
			for (j = i ; j < nSize ; j++)
			{
				if (!TADC_IF_MemCmp(inBuffer + j, ";", 1))
				{
					length = j - i;
					TADC_IF_StrNCpy((char*)tmpbuf, (char*)(inBuffer + i), length);
					tmpbuf[length] = 0;
					break;
				}
			}
			break;
		}
	}
	IF_TRUE_RETURN(length <= 0, TADC_RESPONSEMESSAGE_ERROR);

	if ((length = TADC_IF_StrLen(tmpbuf)) != 40)
	{
		return -1;
	}

	if ((nResult = HEX2BIN((char*)tmpbuf, hashReqID, &length)) < 0)
	{
		return -1;
	}

	if (length != 20)
	{
		return -1;
	}

	return 0;
}

int TADC_GetResponseRO(unsigned char * inBuffer, T_ROACQ_INFO *t_ROAcqInfo, T_RO *t_RO, unsigned char *outBuffer)
{
	int nHMacSize = 28; // Base64 Enc length of SHA1 20byte
	TADC_U8 sha1_tmp[20] = {0, };
	char hmacBuf[512] = {0, };
	BYTE *ReqTemp = NULL;
	LPBYTE pbBuffer = NULL;

	TADC_U8 key[16] = {0, };
	TADC_U8 iv[16] = {0, };

	int outlen = 0;

	char *License = NULL;
	TCHAR tmpDH_B[DHKey_SIZE * 2 + 1] = {0, };

	// RO XML Parser
	CXMLFile XMLParser;
	CPointerArray paChilds;

	//2011.03.08
	char *timeStamp = NULL;
	int retLen = 0;
	int i = 0, j = 0;
	int nSize = 0;
	int length = 0;
	int req_length = 0;
	int nResult = 0;

	char pRoHeader[36] = {'<', '?', 'x', 'm', 'l', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0', '"', '?', '>', 0x0A,
						'<', 'T', 'i', 'z', 'e', 'n', 'L', 'i', 'c', 'e', 'n', 's', 'e', '>'};

	//Check Param Buffer
	IF_TRUE_RETURN(inBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_ROAcqInfo== NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_RO == NULL, TADC_PARAMETER_ERROR);

	nSize = TADC_IF_StrLen((char*)inBuffer);
	IF_TRUE_RETURN( nSize <= 40 || nSize>RESP_MAXSIZE, TADC_PARAMETER_ERROR);

	IF_TRUE_RETURN(t_ROAcqInfo->t_DHInfo.pSize<=0 || t_ROAcqInfo->t_DHInfo.pSize>DHKey_SIZE, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_ROAcqInfo->t_DHInfo.ASize<=0 || t_ROAcqInfo->t_DHInfo.ASize>DHKey_SIZE, TADC_PARAMETER_ERROR);

	//Check XML Result Code ( Success result='0' )
	nResult = -1;
	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer + i), "<response result=", 17))
		{
			i += 17;
			if ((!memcmp((char*)(inBuffer + i), "'0'", 3)) || (!memcmp((char*)(inBuffer + i), "\"0\"", 3)))
			{
				nResult = 0;
				break;
			}
			else
			{
				nResult = -1;
			}
		}
	}
	IF_TRUE_RETURN(((i == nSize) || (nResult < 0)), TADC_RESPONSEMESSAGE_ERROR);

	//Init
	TADC_IF_MemSet(t_RO, 0, sizeof(T_RO));
	TADC_IF_MemSet(tmpDH_B, 0, sizeof(tmpDH_B));
	TADC_IF_MemSet(sha1_tmp, 0, sizeof(sha1_tmp));
	TADC_IF_MemSet(hmacBuf, 0, sizeof(hmacBuf));

	//Check HMAC
	req_length = 0;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer + i), "<responsedata>", 14))
		{
			i += 14;
			for (j = i ; j < nSize ; j++)
			{
				if (!TADC_IF_MemCmp((char*)(inBuffer+j), ";hmac=", 6))
				{
					req_length = j-i;
					IF_TRUE_RETURN(req_length <= 0 || req_length >= RESP_MAXSIZE, TADC_PARAMETER_ERROR);

					ReqTemp = (TADC_U8*)TADC_IF_Malloc(req_length);
					IF_TRUE_RETURN(ReqTemp == NULL, TADC_MEMAlOC_ERROR);

					TADC_IF_MemSet(ReqTemp, 0, req_length);
					TADC_IF_MemCpy(ReqTemp, inBuffer+i, req_length);

					i += req_length;

					TADC_IF_StrNCpy(hmacBuf, (char*)(inBuffer + i + 6), nHMacSize);
					break;
				}
			}
			break;
		}
	}
	IF_TRUE_RETURN(req_length <= 0, TADC_RESPONSEMESSAGE_ERROR);

	pbBuffer = Base64Decode( (LPCSTR)hmacBuf, &length );
	if (pbBuffer == NULL)
	{
		TADC_IF_Free(ReqTemp);
		ReqTemp = NULL;
		return -1;
	}

	TADC_IF_MemCpy(key, &g_baAESKey[0], 16 );
	TADC_IF_MemCpy(iv, &g_baAESKey[16], 16 );
	TADC_IF_AES_CTR(key, 16, iv, 20, (unsigned char*)pbBuffer, &outlen, (unsigned char*)pbBuffer);

	TADC_IF_SHA1(ReqTemp, req_length, sha1_tmp);
	if (ReqTemp != NULL)
	{
		TADC_IF_Free(ReqTemp);
		ReqTemp = NULL;
	}

	if (TADC_IF_MemCmp(sha1_tmp, pbBuffer, sizeof(sha1_tmp)))
	{
		TADC_IF_Free(pbBuffer);
		pbBuffer = NULL;
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_RESPONSESIGNATURE_ERROR);
		return TADC_RESPONSESIGNATURE_ERROR;
	}

	TADC_IF_Free(pbBuffer);
	pbBuffer = NULL;

	req_length = 0;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp(inBuffer + i, "B=", 2))
		{
			i += 2;
			for (j = i ; j < nSize ; j++)
			{
				if (!TADC_IF_MemCmp(inBuffer + j, ";", 1))
				{
					req_length = j - i;
					IF_TRUE_RETURN(req_length <= 0 || req_length >= (int)sizeof(tmpDH_B), TADC_PARAMETER_ERROR);
					TADC_IF_StrNCpy((char*)tmpDH_B, (char*)(inBuffer + i), req_length);
					tmpDH_B[req_length] = 0;
					break;
				}
			}
			break;
		}
	}
	IF_TRUE_RETURN(req_length <= 0, TADC_RESPONSEMESSAGE_ERROR);

	TADC_IF_MemSet(t_ROAcqInfo->t_DHInfo.B, 0, sizeof(t_ROAcqInfo->t_DHInfo.B));
	TADC_IF_MemCpy(t_ROAcqInfo->t_DHInfo.B, tmpDH_B, sizeof(t_ROAcqInfo->t_DHInfo.B));

	if (HEX2BIN((char*)tmpDH_B, t_ROAcqInfo->t_DHInfo.B, (int*)&t_ROAcqInfo->t_DHInfo.BSize) < 0)
	{
		return TADC_GETDHKEY_ERROR;
	}

	if(t_ROAcqInfo->t_DHInfo.BSize != 16 && t_ROAcqInfo->t_DHInfo.BSize != 64)
	{
		return TADC_GETDHKEY_ERROR;
	}

	TADC_IF_MemSet(t_ROAcqInfo->t_DHInfo.K, 0, sizeof(t_ROAcqInfo->t_DHInfo.K));

	nResult = TADC_IF_GetDHKey_K(&t_ROAcqInfo->t_DHInfo);
	IF_ERROR_RETURN(nResult, TADC_GETDHKEY_ERROR);

	req_length = 0;

	for (i = 0 ; i < nSize ; i++)
	{
		if (!TADC_IF_MemCmp((char*)(inBuffer + i), "license=", 8))
		{
			i += 8;
			for (j = i ; j < nSize ; j++)
			{
				if (!TADC_IF_MemCmp((char*)(inBuffer + j), ";hmac=", 6))
				{
					req_length = j - i;
					IF_TRUE_RETURN(req_length <= 0 || req_length >= RESP_MAXSIZE, TADC_PARAMETER_ERROR);

					License = (char*)TADC_IF_Malloc(req_length + 1);
					IF_TRUE_RETURN(License == NULL, TADC_MEMAlOC_ERROR);

					TADC_IF_MemSet(License, 0, req_length + 1);
					TADC_IF_StrNCpy(License, (char*)(inBuffer + i), req_length);
					break;
				}
			}
			break;
		}
	}
	IF_TRUE_RETURN(req_length <= 0, TADC_RESPONSEMESSAGE_ERROR);

	// Get RO License Info
	pbBuffer = Base64Decode((LPCSTR)License, &length);
	if(!pbBuffer)
	{
		TADC_IF_Free(License);
		License = NULL;
		return -1;
	}

	pbBuffer[length] = 0;
	TADC_IF_Free(License);
	License = NULL;

	// Decrypt License
	TADC_IF_MemCpy(key, &t_ROAcqInfo->t_DHInfo.K[0], 16);
	TADC_IF_MemCpy(iv, &t_ROAcqInfo->t_DHInfo.K[16], 16);
	TADC_IF_AES_CTR(key, 16, iv, length, (unsigned char*)pbBuffer, &outlen, (unsigned char*)pbBuffer);

	// Test Code for get the plaintext ro
	/*
	strcat(tmpPath, (char*)t_RO->t_Content.CID);
	strcat(tmpPath, ".ro");
	fd = fopen(tmpPath, "w+b");
	*/
#if 0
	DRM_TAPPS_LOG("fopen started.");
	fd = fopen("/tmp/1.ro", "w+b");
	if (fd == NULL)
	{
		DRM_TAPPS_EXCEPTION("fopen() failed.");
		return -1;
	}
	DRM_TAPPS_LOG("fopen done.");
	fwrite(pbBuffer, 1, strlen((char*)pbBuffer), fd);
	DRM_TAPPS_LOG("fwrite done.");
	fclose(fd);
	DRM_TAPPS_LOG("fclose done.");
	DRM_TAPPS_LOG("Save decrypted RO success!, path = %s", tmpPath);
#endif
	// Test Code Done!

	if(TADC_IF_MemCmp((char*)pbBuffer, pRoHeader, 36))
	{
		TADC_IF_Free(pbBuffer);
		pbBuffer = NULL;
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_LICENSEXMLPARSING_ERROR);
		return TADC_LICENSEXMLPARSING_ERROR;
	}
	TADC_IF_MemCpy(outBuffer, pbBuffer, length);
	TADC_IF_Free(pbBuffer);
	pbBuffer = NULL;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2011.03.08
	// Get timeStamp
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	retLen = length;
	// <timeStamp>
	req_length = 0;

	DRM_TAPPS_LOG("timeStamp parsing start");

	i = FindString(inBuffer, nSize, (unsigned char *)"<timeStamp>", 11);
	if (i < 0) {
		DRM_TAPPS_LOG("No timeStamp. TADC_GetResponseRO Success!");
		return retLen;
	}

	i += 11;
	j = FindString(inBuffer, nSize, (unsigned char *)"</timeStamp>", 12);
	IF_TRUE_RETURN(j < i, TADC_LICENSEXMLPARSING_ERROR);
	req_length = j - i;
	IF_TRUE_RETURN(req_length > ROXML_MAXSIZE, TADC_PARAMETER_ERROR);

	if (req_length <= 0)
		return retLen;

	timeStamp =  (char*)TADC_IF_Malloc(req_length + 1);
	IF_TRUE_RETURN(timeStamp == NULL, TADC_MEMAlOC_ERROR);

	TADC_IF_MemSet(timeStamp, 0, req_length + 1);
	TADC_IF_StrNCpy(timeStamp, (char*)(inBuffer + i), req_length);

	DRM_TAPPS_LOG("timeStamp parsing end.. req_length[%d]", req_length);

	// Get RO License Info
	pbBuffer = Base64Decode((LPCSTR)timeStamp, &length);
	if (!pbBuffer)
	{
		TADC_IF_Free(timeStamp);
		timeStamp = NULL;
		DRM_TAPPS_EXCEPTION("Base64Decode Failed");
		return -1;
	}

	pbBuffer[length] = 0;
	TADC_IF_Free(timeStamp);
	timeStamp = NULL;

	DRM_TAPPS_LOG("Decrypt timeStamp starts");

	// Decrypt timeStamp
	TADC_IF_MemCpy(key, &t_ROAcqInfo->t_DHInfo.K[0], 16);
	TADC_IF_MemCpy(iv, &t_ROAcqInfo->t_DHInfo.K[16], 16);
	TADC_IF_AES_CTR(key, 16, iv, length, (unsigned char*)pbBuffer, &outlen, (unsigned char*)pbBuffer);

	if (length >= (int)sizeof(t_ROAcqInfo->sTimeStamp))
	{
		TADC_IF_Free(pbBuffer);
		pbBuffer = NULL;
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_RESPONSEMESSAGE_ERROR);
		return TADC_RESPONSEMESSAGE_ERROR;
	}

	TADC_IF_MemCpy(t_ROAcqInfo->sTimeStamp, pbBuffer, length);
	t_ROAcqInfo->sTimeStamp[length] = 0;

	TADC_IF_Free(pbBuffer);
	pbBuffer = NULL;

	DRM_TAPPS_LOG("timeStamp found. TADC_GetResponseRO Success!");
	return retLen;
}

int TADC_VerifyROSignature(IN LPBYTE pszXML)
{
	int nResult = 0;
	int length = 0, i = 0, i2 = 0;

	unsigned char* pSigData = NULL;
	unsigned char* pRICert = NULL;
	unsigned char* pRICACert = NULL;

	int inLen = 0, sigLen = 0, certLen = 0, certLen2 = 0;

	unsigned char* p;
	char TempBuf[CERT_MAXSIZE] = {0, };

	//Check Param Buffer
	IF_TRUE_RETURN(pszXML == NULL, TADC_PARAMETER_ERROR);

	//Check Version
	i = FindString(pszXML, TADC_IF_StrLen((char*)pszXML), (LPBYTE)"<version>2.0</version>", 22);
	if (i < 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyROSignature Debug : It is not the TAD-RO2.0 format. \n");
		return 0;
	}

	DRM_TAPPS_LOG("TADC_IF_VerifyROSignature Debug : It is the TAD-RO2.0 format.\n");

	//Get Certificates ( RI Cert )
	TADC_IF_MemSet(TempBuf, 0x00, CERT_MAXSIZE);

	i = FindString(pszXML, TADC_IF_StrLen((char*)pszXML), (LPBYTE)"<CertificateChain>", 18);
	IF_TRUE_RETURN(i < 0, TADC_RO_CERTIFICATE_ERROR);

	p = pszXML + i; // <certificateChain>
	length = TADC_IF_StrLen((char*)pszXML) - i;

	i = FindString(p, length, (LPBYTE)"<Certificate>", 13);
	IF_TRUE_RETURN(i < 0, TADC_RO_CERTIFICATE_ERROR);
	i += 13; // start position of RI Cert

	i2 = FindString(p, length, (LPBYTE)"</Certificate>", 14);
	IF_TRUE_RETURN(i2 < (i + 13), TADC_RO_CERTIFICATE_ERROR);
	certLen = i2 - i; // size of RI Cert

	TADC_IF_StrNCpy(TempBuf, (char*)p+i, certLen);

	pRICert = Base64Decode( (LPCSTR)TempBuf, &certLen );
	IF_TRUE_RETURN(pRICert == NULL, TADC_RO_CERTIFICATE_ERROR);
	TADC_IF_MemSet(TempBuf, 0x00, CERT_MAXSIZE);

	//Get Certificates ( RICA Cert )
	p = p + (i2 + 14); // first </certificate>
	length = length - (i2 + 14);

	i = FindString(p, length, (LPBYTE)"<Certificate>", 13);
	IF_TRUE_RETURN2(i < 0, TADC_IF_Free(pRICert),,,TADC_RO_CERTIFICATE_ERROR);
	i += 13; // start position of RICA Cert

	i2 = FindString(p, length, (LPBYTE)"</Certificate>", 14);
	IF_TRUE_RETURN2(i2 < (i + 13), TADC_IF_Free(pRICert),,,TADC_RO_CERTIFICATE_ERROR);
	certLen2 = i2 - i; // size of RICA Cert

	TADC_IF_StrNCpy(TempBuf, (char*)p + i, certLen2);

	pRICACert = Base64Decode((LPCSTR)TempBuf, &certLen2);
	IF_TRUE_RETURN2(pRICACert == NULL, TADC_IF_Free(pRICert),,,TADC_RO_CERTIFICATE_ERROR);
	TADC_IF_MemSet(TempBuf, 0x00, CERT_MAXSIZE);

	//Get position of indata
	inLen = FindString(pszXML, TADC_IF_StrLen((char*)pszXML), (LPBYTE)"</CertificateChain>", 19);
	IF_TRUE_RETURN2(inLen < 0, TADC_IF_Free(pRICert),TADC_IF_Free(pRICACert),,TADC_RO_CERTIFICATE_ERROR);
	inLen += 19;

	//Get signature value </certificateChain>
	p = pszXML + inLen;
	length = TADC_IF_StrLen((char*)pszXML)-inLen;

	i = FindString(p, length, (LPBYTE)"<SignatureValue>", 16);
	IF_TRUE_RETURN2(i < 0, TADC_IF_Free(pRICert),TADC_IF_Free(pRICACert),,TADC_RO_SIGNATURE_ERROR);
	i += 16; // start position of signature value

	i2 = FindString(p, length, (LPBYTE)"</SignatureValue>", 17);
	IF_TRUE_RETURN2(i2 < (i + 16), TADC_IF_Free(pRICert),TADC_IF_Free(pRICACert),,TADC_RO_SIGNATURE_ERROR);
	sigLen = i2 - i; // size of signature value

	TADC_IF_StrNCpy(TempBuf, (char*)p + i, sigLen);

	pSigData = Base64Decode( (LPCSTR)TempBuf, &sigLen );
	IF_TRUE_RETURN2(pSigData == NULL, TADC_IF_Free(pRICert),TADC_IF_Free(pRICACert),,TADC_RO_SIGNATURE_ERROR);
	TADC_IF_MemSet(TempBuf, 0x00, CERT_MAXSIZE);

	//Verify the certificate chain
	nResult = TADC_IF_VerifyCertChain(pRICACert, certLen2, pRICert, certLen);
	if (nResult == 0)
	{
		nResult = TADC_IF_VerifySignature(pszXML, inLen, pSigData, sigLen, pRICert, certLen);
	}

	TADC_IF_Free(pRICert);
	TADC_IF_Free(pRICACert);
	TADC_IF_Free(pSigData);

	return nResult;
}

int TADC_GetResponseROInfo(IN LPBYTE pszXML,  T_RO *t_RO)
{
	int nResult = 0;
	CXMLFile oXMLFile;
	CXMLElement* pRoot, *pElement;
	CPointerArray paChilds;
	LPCTSTR pszValue;
	LPBYTE pbBuffer = NULL;
	int length = 0;
	TADC_U8 TempVersion[3] = {0, };

	char pRoHeader[36] = {'<', '?', 'x', 'm', 'l', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0', '"', '?', '>', 0x0A,
						'<', 'T', 'i', 'z', 'e', 'n', 'L', 'i', 'c', 'e', 'n', 's', 'e', '>'};

	//Check Param Buffer
	IF_TRUE_RETURN(pszXML == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_RO == NULL, TADC_PARAMETER_ERROR);

	//Init
	TADC_IF_MemSet(t_RO, 0, sizeof(T_RO));
	TADC_IF_MemSet(TempVersion, 0, sizeof(TempVersion));

	if (TADC_IF_MemCmp((char*)pszXML, pRoHeader, 36))
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_XMLPARSER_ERROR);
		return -1;
	}

	// Verify the signature ( 2011.03.08, Joseph Ahn )
	nResult = TADC_VerifyROSignature(pszXML);
	if (nResult != 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_VerifyROSignature Error Code = %x", nResult);
		return -1;
	}

	nResult = oXMLFile.LoadFromStream((LPCTSTR)pszXML);
	if (nResult < 0)
	{
		DRM_TAPPS_EXCEPTION("oXMLFile.LoadFromStream is failed\n");
		nResult = -1;
		goto finish;
	}

	// Set Version
	pRoot = oXMLFile.GetRoot();
	if (pRoot == NULL)
	{
		DRM_TAPPS_EXCEPTION("oXMLFile.GetRoot is failed\n");
		nResult = -1;
		goto finish;
	}

	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("ContentInfo"), _T("cid"), NULL);
	if (nResult != 0)
	{
		DRM_TAPPS_EXCEPTION("pRoot->Find is failed\n");
		nResult = -1;
		goto finish;
	}

	if (paChilds.GetCount() != 1)
	{
		DRM_TAPPS_EXCEPTION("paChilds.GetCount() is failed\n");
		nResult = -1;
		goto finish;
	}

	pElement = (CXMLElement*)paChilds.Get(0);
	pszValue = pElement->GetValue();
	if (pszValue == NULL)
	{
		DRM_TAPPS_EXCEPTION("pElement->GetValue() is failed\n");
		nResult = -1;
		goto finish;
	}

	length = TADC_IF_StrLen((char*)pszValue);
	if (length <= 0 || length > CID_SIZE)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_StrLen is failed(%d) -  %s\n", length, pszValue);
		nResult = -1;
		goto finish;
	}

	t_RO->t_Content.CID = (TADC_U8*)TADC_IF_Malloc(length + 1); //2011.03.08 ( CID_SIZE -> length )
	IF_TRUE_GOTO(t_RO->t_Content.CID == NULL, -2);
	TADC_IF_MemSet(t_RO->t_Content.CID, 0, length + 1); //2011.03.08 ( CID_SIZE -> length )
	TADC_IF_StrNCpy((CHAR*)t_RO->t_Content.CID, pszValue, length);

	// Get KeyValue (CEK)
	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO(pRoot == NULL, ERROR_INVALID_DATA);

	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("KeyInfo"), _T("CipherValue"), NULL);

	IF_TRUE_GOTO(nResult != 0, ERROR_INVALID_DATA);
	IF_TRUE_GOTO(paChilds.GetCount() != 1, ERROR_INVALID_DATA);

	pElement = (CXMLElement*)paChilds.Get(0);
	pszValue = pElement->GetValue();
	IF_TRUE_GOTO(pszValue == NULL, -2);

	pbBuffer = Base64Decode(pszValue, &length);
	if (pbBuffer == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_XMLPARSER_ERROR);
		return -1;
	}

	if (length <= 0 || length > CEK_SIZE)
	{
		TADC_IF_Free(pbBuffer);
		pbBuffer = NULL;
		nResult = -1;
		goto finish;
	}

	t_RO->t_Content.CEK = (TADC_U8*)TADC_IF_Malloc(length + 1); //2011.03.08 ( CEK_SIZE -> length )
	IF_TRUE_GOTO(t_RO->t_Content.CEK == NULL, -2);
	TADC_IF_MemSet(t_RO->t_Content.CEK, 0, length + 1); //2011.03.08 ( CEK_SIZE -> length )
	TADC_IF_MemCpy(t_RO->t_Content.CEK, pbBuffer, length);

	TADC_IF_Free(pbBuffer);
	pbBuffer = NULL;

	// Get individual
	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO(pRoot == NULL, ERROR_INVALID_DATA);

	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("DeviceInfo"), _T("DUID"), NULL);

	IF_TRUE_GOTO(nResult != 0, ERROR_INVALID_DATA);
	IF_TRUE_GOTO(paChilds.GetCount() != 1, ERROR_INVALID_DATA);

	pElement = (CXMLElement*)paChilds.Get(0);
	pszValue = pElement->GetValue();
	IF_TRUE_GOTO(pszValue == NULL, -1);
	length = TADC_IF_StrLen((char*)pszValue);
	if (length <= 0)
	{
		nResult = -1;
		goto finish;
	}

	t_RO->PerFlag |= DUID_RULE;
	t_RO->t_Permission.t_Individual.BindingType |= DUID_RULE;
	t_RO->t_Permission.t_Individual.DUID = (TADC_U8*)TADC_IF_Malloc(length + 1);

	if (t_RO->t_Permission.t_Individual.DUID == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_MEMAlOC_ERROR);
		nResult = -2;
		goto finish;
	}

	TADC_IF_MemSet(t_RO->t_Permission.t_Individual.DUID, 0, length + 1);
	TADC_IF_MemCpy((CHAR*)t_RO->t_Permission.t_Individual.DUID, pszValue, length);

	nResult = 0;

finish:
	pszValue = NULL;

	if (nResult < 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_XMLPARSER_ERROR);
	}

	if (nResult == -2)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_MEMAlOC_ERROR);
	}

	TADC_IF_Free(pbBuffer);

	return nResult;
}

int TADC_GetFileHeader(unsigned char * inBuffer,  T_FILE_HEADER *t_FileHeader)
{
	int i = 0;
	BYTE FixedFileType[8] = {0x00,0x00,0x00,0x14,0x66,0x74,0x79,0x70};
	char tmp[2];

	//Check Param Bufffer
	IF_TRUE_RETURN(inBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_FileHeader == NULL, TADC_PARAMETER_ERROR);

	//Init
	TADC_IF_MemSet(t_FileHeader, 0, sizeof(T_FILE_HEADER));
	TADC_IF_MemSet(tmp, 0, sizeof(tmp));

	if (TADC_IF_MemCmp(inBuffer, FixedFileType, sizeof(FixedFileType)))
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_CONTENTSSTRUCT_ERROR);
		return -1;
	}
	i += 8;

	if (TADC_IF_MemCmp(inBuffer+i, "TADF", 4))
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_CONTENTSSTRUCT_ERROR);
		return -2;
	}
	i += 4;

	t_FileHeader->Version[0] = '1';
	t_FileHeader->Version[1] = '0';
	i += 4;

	if (TADC_IF_MemCmp(inBuffer+i, "TADF", 4))
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_CONTENTSSTRUCT_ERROR);
		return -2;
	}
	i += 4;

	t_FileHeader->DRMType = inBuffer[i];
	i += 1;

	TADC_IF_MemCpy(t_FileHeader->ContentsType, inBuffer+i, 128);
	i += 128;

	TADC_IF_MemCpy(&t_FileHeader->TotalSize, inBuffer+i, 8);
	i += 8;
	t_FileHeader->TotalSize = _hton64(t_FileHeader->TotalSize);

	TADC_IF_MemCpy(&t_FileHeader->Offset1, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset1 = _hton64(t_FileHeader->Offset1);

	TADC_IF_MemCpy(&t_FileHeader->Offset2, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset2 = _hton64(t_FileHeader->Offset2);

	TADC_IF_MemCpy(&t_FileHeader->Offset3, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset3 = _hton64(t_FileHeader->Offset3);

	TADC_IF_MemCpy(&t_FileHeader->Offset4, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset4 = _hton64(t_FileHeader->Offset4);

	TADC_IF_MemCpy(&t_FileHeader->Offset5, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset5 = _hton64(t_FileHeader->Offset5);

	TADC_IF_MemCpy(&t_FileHeader->Offset6, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset6 = _hton64(t_FileHeader->Offset6);

	TADC_IF_MemCpy(&t_FileHeader->Offset7, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset7 = _hton64(t_FileHeader->Offset7);

	TADC_IF_MemCpy(&t_FileHeader->Offset8, inBuffer+i, 8);
	i += 8;
	t_FileHeader->Offset8 = _hton64(t_FileHeader->Offset8);

	DRM_TAPPS_LOG("TADC_GetFileHeader Success!\n");
	return 0;
}

int TADC_GetDRMHeader(unsigned char * inBuffer, T_DRM_HEADER *t_DRMHeader)
{
	int i = 0;

	//Check Param Bufffer
	IF_TRUE_RETURN(inBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_DRMHeader == NULL, TADC_PARAMETER_ERROR);

	//Init
	TADC_IF_MemSet(t_DRMHeader, 0, sizeof(T_DRM_HEADER));

	if (TADC_IF_MemCmp(inBuffer, "TIZEN_DRM", 9))
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_NOTTADCFILE_ERROR);
		return -1;
	}
	i += 9;

	TADC_IF_MemCpy(t_DRMHeader->Version, inBuffer+i, 2);
	i += 2;

	TADC_IF_MemCpy(&t_DRMHeader->XmlSize, inBuffer+i, 4);
	i += 4;

	t_DRMHeader->XmlSize = htonl_(t_DRMHeader->XmlSize);
	DRM_TAPPS_LOG("TADC_GetDRMHeader Success!\n");

	return 0;
}

int TADC_GetDRMHeaderInfo(unsigned char * inBuffer, T_DRM_HEADER *t_DRMHeader)
{
	int nResult = 0;
	CXMLFile oXMLFile;
	CXMLElement* pRoot, *pNode;
	CPointerArray paChilds;
	LPCTSTR pszValue;

	BYTE sha1_tmp[20];
	unsigned char *ReqTemp=NULL;

	TADC_U8 key[16];
	TADC_U8 iv[16];

	int outlen=0;
	long Length=0;

	//Check Param
	IF_TRUE_RETURN(inBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_DRMHeader == NULL, TADC_PARAMETER_ERROR);

	IF_TRUE_RETURN(t_DRMHeader->XmlSize <= 0 || t_DRMHeader->XmlSize > ROXML_MAXSIZE, TADC_PARAMETER_ERROR);

	ReqTemp = (TADC_U8*)TADC_IF_Malloc(t_DRMHeader->XmlSize);
	IF_TRUE_RETURN(ReqTemp == NULL, TADC_MEMAlOC_ERROR);

	TADC_IF_MemSet(ReqTemp, 0, t_DRMHeader->XmlSize);
	TADC_IF_MemSet(sha1_tmp, 0, sizeof(sha1_tmp)  );

	//Get HMAC
	TADC_IF_MemCpy(ReqTemp, inBuffer + 20, t_DRMHeader->XmlSize);
	TADC_IF_SHA1(ReqTemp, t_DRMHeader->XmlSize, sha1_tmp);

	if (ReqTemp != NULL)
	{
		TADC_IF_Free(ReqTemp);
		ReqTemp = NULL;
	}

	TADC_IF_MemCpy(key, &g_baSignatureKey[0], 16);
	TADC_IF_MemCpy(iv, &g_baSignatureKey[16], 16);
	TADC_IF_AES_CTR(key, 16, iv, 20, (unsigned char*)sha1_tmp, &outlen, (unsigned char*)sha1_tmp);

	if (TADC_IF_MemCmp(sha1_tmp, inBuffer, sizeof(sha1_tmp)))
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_CONTENTSHMAC_ERROR);
		return -1;
	}

	//Load XML Header
	nResult = oXMLFile.LoadFromStream((LPCTSTR)(inBuffer + 20));

	if (nResult != 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_CONTENTSXML_ERROR);
		goto finish;
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );

	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("SID"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			Length = TADC_IF_StrLen((char*)pszValue);

			if (Length > 0)
			{
				t_DRMHeader->SID = (TADC_U8*)TADC_IF_Malloc(Length + 1);
				IF_TRUE_RETURN(t_DRMHeader->SID == NULL, TADC_MEMAlOC_ERROR);
				TADC_IF_MemSet(t_DRMHeader->SID, 0, Length + 1);
				TADC_IF_StrNCpy((CHAR*)t_DRMHeader->SID, pszValue, Length);
			}
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );

	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("CID"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			Length = TADC_IF_StrLen((char*)pszValue); //2011.03.08

			if (Length > 0)
			{
				t_DRMHeader->CID = (TADC_U8*)TADC_IF_Malloc(Length + 1); //2011.03.08 ( CID_SIZE -> Length )
				IF_TRUE_RETURN(t_DRMHeader->CID == NULL, TADC_MEMAlOC_ERROR);
				TADC_IF_MemSet(t_DRMHeader->CID, 0, Length + 1); //2011.03.08 ( CID_SIZE -> Length )
				TADC_IF_StrNCpy((CHAR*)t_DRMHeader->CID, pszValue, Length); //2011.03.08 ( CID_SIZE -> Length )
			}
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO(pRoot == NULL, ERROR_INVALID_DATA);
	paChilds.RemoveAll();
	nResult = pRoot->Find(&paChilds, _T("ContentType"), NULL);

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			Length = TADC_IF_StrLen((char*)pszValue);
			TADC_IF_MemSet(t_DRMHeader->ContentsType, 0, 128);
			TADC_IF_StrNCpy((CHAR*)t_DRMHeader->ContentsType, pszValue, Length);
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );
	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("EncryptionMethod"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			t_DRMHeader->EncryptionMethod = TADC_IF_AtoI((char*)pszValue);
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );
	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("EncryptionLevel"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			t_DRMHeader->EncryptionLevel = TADC_IF_AtoI((char*)pszValue);
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );
	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("EncryptionRange"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			t_DRMHeader->EncryptionRange = TADC_IF_AtoI((char*)pszValue);
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );
	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("RIURL"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			Length = TADC_IF_StrLen((char*)pszValue);

			if (Length > 0)
			{
				t_DRMHeader->RIURL = (TADC_U8*)TADC_IF_Malloc(Length+1);
				IF_TRUE_RETURN(t_DRMHeader->RIURL == NULL, TADC_MEMAlOC_ERROR);
				TADC_IF_MemSet(t_DRMHeader->RIURL, 0, Length+1);
				TADC_IF_StrNCpy((CHAR*)t_DRMHeader->RIURL, pszValue, Length);
			}
		}
	}

	// dummy RIURL(DRM Server spec changed)
	if (t_DRMHeader->RIURL == NULL)
	{
		pszValue = "dummy_riurl";
		Length = TADC_IF_StrLen(pszValue);
		t_DRMHeader->RIURL = (TADC_U8*)TADC_IF_Malloc(Length+1);
		IF_TRUE_RETURN(t_DRMHeader->RIURL == NULL, TADC_MEMAlOC_ERROR);
		TADC_IF_MemSet(t_DRMHeader->RIURL, 0, Length+1);
		TADC_IF_StrNCpy((CHAR*)t_DRMHeader->RIURL, pszValue, Length);
	}

	DRM_TAPPS_LOG("t_DRMHeader->RIURL = %s\n", (char*)t_DRMHeader->RIURL);

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );
	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("PlaintextSize"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			t_DRMHeader->PlaintextSize = TADC_IF_AtoI((char*)pszValue);
		}
	}

	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );
	paChilds.RemoveAll();
	nResult = pRoot->Find( &paChilds, _T("Packdate"), NULL );

	if (nResult == 0)
	{
		if (paChilds.GetCount() == 1)
		{
			pNode = (CXMLElement*)paChilds.Get(0);
			pszValue = pNode->GetValue();
			Length = TADC_IF_StrLen((char*)pszValue);

			if (Length > 0)
			{
				t_DRMHeader->Packdate = (TADC_U8*)TADC_IF_Malloc(Length+1);
				IF_TRUE_RETURN(t_DRMHeader->Packdate == NULL, TADC_MEMAlOC_ERROR);
				TADC_IF_MemSet(t_DRMHeader->Packdate, 0, Length+1);
				TADC_IF_StrNCpy((char*)t_DRMHeader->Packdate, pszValue, Length);
			}
		}
	}

finish:
	if (nResult != 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_CONTENTSXML_ERROR);
		STACKTRACE( _T( "CNCG20File::_ParseHeader()" ) );
	}
	DRM_TAPPS_LOG("TADC_GetDRMHeaderInfo Success! \n");
	return nResult;
}

int TADC_GetCEK(T_DEVICE_INFO *t_DeviceInfo, T_RO *t_RODB, T_DRM_HEADER *t_DRMHeader)
{
	int length1 = 0;

	//Check Parameter Buffer
	IF_TRUE_RETURN(t_DeviceInfo == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_DRMHeader == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_DRMHeader->CID == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_RODB == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_RODB->t_Content.CID == NULL, TADC_PARAMETER_ERROR);

	if(!TADC_IF_StrCmp((char*)t_DRMHeader->CID, (char*)t_RODB->t_Content.CID)) //2011.03.08 ( TADC_IF_MemCmp -> TADC_IF_StrCmp )
	{
		if((t_RODB->PerFlag & DUID_RULE) && (t_RODB->t_Permission.t_Individual.BindingType & DUID_RULE))
		{
			IF_TRUE_RETURN(t_RODB->t_Permission.t_Individual.DUID == NULL, TADC_PARAMETER_ERROR);
			length1 = TADC_IF_StrLen((char*)t_DeviceInfo->DUID);
			DRM_TAPPS_LOG("t_DeviceInfo->DUID = %s, t_RODB->t_Permission.t_Individual.DUID is %s, length1 is %d\n", t_DeviceInfo->DUID, t_RODB->t_Permission.t_Individual.DUID, length1);
			/*if(TADC_IF_MemCmp(t_DeviceInfo->DUID, t_RODB->t_Permission.t_Individual.DUID, length1))
			{
				DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_RULE_DUID_ERROR);
				return -1;
			} */
		}
		IF_TRUE_RETURN(t_RODB->t_Content.CEK == NULL, TADC_PARAMETER_ERROR);
		t_DRMHeader->CEK = (TADC_U8*)TADC_IF_Malloc(CEK_SIZE + 1);
		IF_TRUE_RETURN(t_DRMHeader->CEK == NULL, TADC_MEMAlOC_ERROR);
		TADC_IF_MemSet(t_DRMHeader->CEK, 0, CEK_SIZE + 1);
		TADC_IF_MemCpy(t_DRMHeader->CEK, t_RODB->t_Content.CEK, CEK_SIZE);

		DRM_TAPPS_LOG("TADC_GetCEK Success!\n");
		return 0;
	}

	DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp Error Code = %x", TADC_RULE_NOINFOMATION);

	return -1;
}

int TADC_DecryptBlock( char* pbBuffer, int nSize, T_DRM_HEADER *t_DRMHeader)
{
	TADC_U8 key[16] = {0, };
	TADC_U8 iv[16] = {0, };
	int i = 0, nBlocks = 0, nIndex = 0;
	char baToBeEncrypted[32] = {0, };
	int nRemainBytes = 0, nBlockBytes = 0;

	int nEncryptionLevel = 0;
	int nEncryptionMethod = 0;

	char temp[512] = {0, };
	int length = 0;
	int totlength = 0;

	TADC_IF_MemSet(temp, 0, sizeof(temp));

	IF_TRUE_RETURN(pbBuffer == NULL, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_DRMHeader == NULL, TADC_PARAMETER_ERROR);

	IF_TRUE_RETURN(nSize > 512, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(t_DRMHeader->CEK == NULL, TADC_PARAMETER_ERROR);

	nEncryptionLevel = t_DRMHeader->EncryptionLevel;
	nEncryptionMethod = t_DRMHeader->EncryptionMethod;

	IF_TRUE_RETURN(nEncryptionMethod != 1, TADC_PARAMETER_ERROR);
	IF_TRUE_RETURN(nEncryptionLevel < 0 || nEncryptionLevel > 32, TADC_PARAMETER_ERROR);

	TADC_IF_MemSet( baToBeEncrypted, 1, 32 );

	if (nEncryptionLevel > 16)
	{
		nBlocks = 16;
	}
	else
	{
		nBlocks = nEncryptionLevel;
	}

	for (i = 0 ; i < nBlocks ; i++)
	{
		nIndex = 31 - (i * 2);
		baToBeEncrypted[nIndex] = 0;
	}

	for (i = 16 ; i < nEncryptionLevel ; i++)
	{
		nIndex = 30 - ((i - 16) * 2);
		baToBeEncrypted[nIndex] = 0;
	}

	nRemainBytes = nSize;
	nBlockBytes = 16;

	length = 0;
	totlength = 0;

	for (i = 0 ; i < 32 ; i++)
	{
		if (nRemainBytes < 16)
		{
			nBlockBytes = nRemainBytes;
		}

		if (baToBeEncrypted[i] == 1)
		{
			TADC_IF_MemCpy(temp + length, pbBuffer + totlength, nBlockBytes);
			length += nBlockBytes;
		}

		nRemainBytes -= nBlockBytes;
		totlength += nBlockBytes;

		if (nRemainBytes < 1)
		{
			break;
		}
	}

	TADC_IF_MemCpy(key, &t_DRMHeader->CEK[0], 16);
	TADC_IF_MemCpy(iv, &t_DRMHeader->CEK[16], 16);
	TADC_IF_AES_CTR(key, 16, iv, length, (unsigned char*)temp, &length, (unsigned char*)temp);

	nRemainBytes = nSize;
	nBlockBytes = 16;

	length = 0;
	totlength = 0;

	for (i = 0 ; i < 32 ; i++)
	{
		if (nRemainBytes < 16)
		{
			nBlockBytes = nRemainBytes;
		}

		if (baToBeEncrypted[i] == 1)
		{
			TADC_IF_MemCpy(pbBuffer + totlength, temp + length, nBlockBytes);
			length += nBlockBytes;
		}

		nRemainBytes -= nBlockBytes;
		totlength += nBlockBytes;

		if (nRemainBytes < 1)
		{
			break;
		}
	}
	return 0;
}

int TADC_GetDRMHeaderFromFile(const char *pTADCFilepath, T_FILE_HEADER *t_FileHeader, T_DRM_HEADER *t_DRMHeader )
{

	unsigned char tempbuf[512];
	unsigned char *pbuf = NULL;

	int ret=0;
	ULONG readsize=0, ReadLen=0;

	FILE *hFile =  0; //Apps drm file

	//null check
	if (pTADCFilepath == NULL || t_FileHeader == NULL || t_DRMHeader == NULL)
	{
		DRM_TAPPS_EXCEPTION("Error : Parameter Null.");
		return -1;
	}

	//Init
	TADC_IF_MemSet(tempbuf, 0, sizeof(tempbuf));

	//File Open
	if ((hFile = fopen(pTADCFilepath, "rb")) == NULL)
	{
		DRM_TAPPS_EXCEPTION("Error : fopen() error.");
		return -1;
	}

	readsize = 256;

	//FmReadFile(hFile, tempbuf, readsize, &ReadLen);
	ReadLen = fread(tempbuf, 1, readsize, hFile);
	if ((readsize != ReadLen) || (ret=TADC_GetFileHeader(tempbuf, t_FileHeader)) < 0)
	{
		DRM_TAPPS_EXCEPTION("Error : fread() error.");
		fclose(hFile);
		return -1;
	}
	TADC_IF_MemSet(tempbuf, 0, sizeof(tempbuf));

	//Tizen Apps DRM Type Check
	if (t_FileHeader->DRMType & TIZEN_DRM)
	{
		DRM_TAPPS_LOG("It's a TAPPS DCF = %s", pTADCFilepath);
		readsize = 15;
		ReadLen = fread(tempbuf, 1, readsize, hFile);
		if ((readsize != ReadLen) || (ret=TADC_GetDRMHeader(tempbuf, t_DRMHeader)) < 0)
		{
			DRM_TAPPS_EXCEPTION("Error : fread() error.");
			fclose(hFile);
			return -1;
		}

		readsize = 20 + t_DRMHeader->XmlSize;
		if ((pbuf = (unsigned char*)TADC_IF_Malloc( readsize * sizeof(char))) == NULL) 
		{
			DRM_TAPPS_EXCEPTION("Error : fread() error.");
			fclose(hFile);
			return -1;
		}

		ReadLen = fread(pbuf, 1, readsize, hFile);
		if (readsize != ReadLen)
		{
			DRM_TAPPS_EXCEPTION("Error : fread() error.");
			TADC_IF_Free(pbuf);
			fclose(hFile);
			return -1;
		}

		if (TADC_GetDRMHeaderInfo(pbuf, t_DRMHeader) < 0)
		{
			DRM_TAPPS_EXCEPTION("Error : TADC_GetDRMHeaderInfo error.");
			TADC_IF_Free(pbuf);
			fclose(hFile);
			return -1;
		}
		TADC_IF_Free(pbuf);
	}
	else
	{
		DRM_TAPPS_EXCEPTION("Error : It's not a TApps DCF file.");
		fclose(hFile);
		return -1;
	}

	fclose(hFile);
	return 0;
}

DWORD TADC_GetLastError(void)
{
	DWORD dwError = g_TADCErrorCode;
	g_TADCErrorCode = 0;

	return dwError;
}

int TADC_MEMFree_RO(T_RO *t_ro)
{
	IF_TRUE_RETURN(t_ro == NULL, TADC_PARAMETER_ERROR);
	t_ro->PerFlag = 0;

	if (t_ro->t_Content.CID != NULL)
	{
		TADC_IF_Free(t_ro->t_Content.CID);
		t_ro->t_Content.CID = NULL;
	}

	if (t_ro->t_Content.CEK!= NULL)
	{
		TADC_IF_Free(t_ro->t_Content.CEK);
		t_ro->t_Content.CEK = NULL;
	}

	if (t_ro->t_Permission.t_Individual.DUID != NULL)
	{
		TADC_IF_Free(t_ro->t_Permission.t_Individual.DUID);
		t_ro->t_Permission.t_Individual.DUID = NULL;
	}

	return 0;
}

int TADC_MEMFree_FileHeader(T_FILE_HEADER *t_FileHeader)
{
	IF_TRUE_RETURN(t_FileHeader == NULL, TADC_PARAMETER_ERROR);
	TADC_IF_MemSet(t_FileHeader, 0, sizeof(T_FILE_HEADER));

	return 0;
}

int TADC_MEMFree_DRMHeader(T_DRM_HEADER *t_DrmHeader)
{
	IF_TRUE_RETURN(t_DrmHeader == NULL, TADC_PARAMETER_ERROR);
	t_DrmHeader->XmlSize = 0;
	TADC_IF_MemSet(t_DrmHeader->Version, 0, sizeof(t_DrmHeader->Version));

	if (t_DrmHeader->SID != NULL)
	{
		TADC_IF_Free(t_DrmHeader->SID);
		t_DrmHeader->SID = NULL;
	}

	if (t_DrmHeader->CID != NULL)
	{
		TADC_IF_Free(t_DrmHeader->CID);
		t_DrmHeader->CID = NULL;
	}

	TADC_IF_MemSet(t_DrmHeader->ContentsType, 0, sizeof(t_DrmHeader->ContentsType));
	t_DrmHeader->EncryptionRange = 0;

	if (t_DrmHeader->RIURL != NULL)
	{
		TADC_IF_Free(t_DrmHeader->RIURL);
		t_DrmHeader->RIURL = NULL;
	}

	t_DrmHeader->PlaintextSize = 0;
	if (t_DrmHeader->Packdate != NULL)
	{
		TADC_IF_Free(t_DrmHeader->Packdate);
		t_DrmHeader->Packdate = NULL;
	}

	if (t_DrmHeader->CEK != NULL)
	{
		TADC_IF_Free(t_DrmHeader->CEK);
		t_DrmHeader->CEK = NULL;
	}
	return 0;
}

int TADC_MEMFree_ROAcqInfo(T_ROACQ_INFO *t_ROAcqInfo)
{
	IF_TRUE_RETURN(t_ROAcqInfo == NULL, TADC_PARAMETER_ERROR);

	if (t_ROAcqInfo->ROAcqURL != NULL)
	{
		TADC_IF_Free(t_ROAcqInfo->ROAcqURL);
		t_ROAcqInfo->ROAcqURL = NULL;
	}

	if (t_ROAcqInfo->ReqID != NULL)
	{
		TADC_IF_Free(t_ROAcqInfo->ReqID);
		t_ROAcqInfo->ReqID = NULL;
	}

	TADC_IF_MemSet(&t_ROAcqInfo->t_DHInfo, 0, sizeof(T_DH_INFO));
	return 0;
}

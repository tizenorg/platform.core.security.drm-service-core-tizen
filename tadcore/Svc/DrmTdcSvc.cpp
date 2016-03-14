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
 * @file    DrmTdcSvc.cpp
 * @brief   This file is for TADC Testcase temporarily.
 * @author  Sangil Yoon (si83.yoon@samsung.com)
 * @version 1.0
 *
*/

#include "TADC_Core.h"
#include "TADC_IF.h"

#include "TADC_ErrorCode.h"

#include "DrmTdcSvc.h"

#define DHINFO_MAX        100
#define DHINFO_REQIDLEN   20

#define TDC_DECRYPT_IOLEN 1024 * 1024 // 1024 Kbyte

//DH Session Info Structure
typedef struct
{
	BYTE hashReqID[DHINFO_MAX][DHINFO_REQIDLEN]; //LicenseRequest Request ID Hash 20byte
	T_ROACQ_INFO t_ROAcqInfo[DHINFO_MAX];        //DH Info

} DrmTdcDHInfo;


static DrmTdcDHInfo g_DrmTdcDHInfo;
static BOOL g_DrmTdcDHFlag = FALSE;

static char g_sTimeStamp[21]; //2011.03.08, GMT ("CCCC-YY-MMThh:mm:ssZ")

bool DrmTdcGetFileHeader(
	IN const char *pTADCFilepath, //TDC DRM File Path
	IN OUT DrmTdcFileHeader *pFileHeader //File Header Info ( CID, License URL )
)
{
	T_FILE_HEADER t_FileHeader;
	T_DRM_HEADER t_DRMHeader;

	//null check
	if (!pTADCFilepath || !pFileHeader) {
		DRM_TAPPS_EXCEPTION("DrmTdcGetFileHeader Error : Parameter Null");
		return FALSE;
	}

	//Init
	memset(&t_FileHeader, 0x00, sizeof(T_FILE_HEADER));
	memset(&t_DRMHeader, 0x00, sizeof(T_DRM_HEADER));
	memset(pFileHeader, 0x00, sizeof(DrmTdcFileHeader));

	//Get DRM Header Info
	if (TADC_GetDRMHeaderFromFile(pTADCFilepath, &t_FileHeader, &t_DRMHeader) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcGetFileHeader Error : TADC_GetDRMHeaderFromFile (%s)", pTADCFilepath);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		return FALSE;
	}

	//Copy to pFileHeader ( CID, RIURL )
    if( (sizeof(pFileHeader->cid) < strlen((char *)t_DRMHeader.CID) + 1 ) ||
        (sizeof(pFileHeader->riurl) < strlen((char *)t_DRMHeader.RIURL) + 1 )) {
        DRM_TAPPS_EXCEPTION("DrmTdcGetFileHeader Error : TADC_GetDRMHeaderFromFile. Invalid cid or riurl(%s)", pTADCFilepath);
	    TADC_MEMFree_FileHeader(&t_FileHeader);
        TADC_MEMFree_DRMHeader(&t_DRMHeader);
        return FALSE;
    }
	memcpy(pFileHeader->cid, t_DRMHeader.CID, strlen((char *)t_DRMHeader.CID) + 1);
	memcpy(pFileHeader->riurl, t_DRMHeader.RIURL, strlen((char *)t_DRMHeader.RIURL) + 1);

	//Free
	TADC_MEMFree_FileHeader(&t_FileHeader);
	TADC_MEMFree_DRMHeader(&t_DRMHeader);

	return TRUE;
}

bool DrmTdcDecryptPackage(
	IN const char *pTADCFilepath,
	IN const char *pLicenseBuf,
	IN unsigned int licenseBufLen,
	IN const char *pDecryptedFile)
{
	T_FILE_HEADER t_FileHeader;
	T_DRM_HEADER t_DRMHeader;
	T_DEVICE_INFO t_DeviceInfo;
	T_RO t_RO;

	FILE *hFile1 = INVALID_HOBJ; //TDC drm file
	FILE *hFile2 = INVALID_HOBJ; //Decrypted file

	//2011.03.08, init
	memset(&t_FileHeader, 0x00, sizeof(T_FILE_HEADER));
	memset(&t_DRMHeader, 0x00, sizeof(T_DRM_HEADER));
	memset(&t_DeviceInfo, 0x00, sizeof(T_DEVICE_INFO));
	memset(&t_RO, 0x00, sizeof(T_RO));

	//null check
	if (!pTADCFilepath
		|| !pLicenseBuf
		|| !pDecryptedFile
		|| licenseBufLen < 1) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : Parameter Null");
		return FALSE;
	}

	//Set DeviceInfo
	if (TADC_SetDeviceInfo(&t_DeviceInfo) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : TADC_SetDeviceInfo");
		return FALSE;
	}

	auto pReadBuf = (TADC_U8*)TADC_IF_Malloc(TDC_DECRYPT_IOLEN);
	if (!pReadBuf) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : pReadBuf Malloc Fail");
		return FALSE;
	}

	//Get DRM Header Info
	if (TADC_GetDRMHeaderFromFile(pTADCFilepath, &t_FileHeader, &t_DRMHeader) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : TADC_GetDRMHeaderFromFile");
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	// RO XML Parser
	if (TADC_GetResponseROInfo((unsigned char*)pLicenseBuf, &t_RO) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : TADC_GetResponseROInfo");
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_MEMFree_RO(&t_RO);
		TADC_IF_Free(pReadBuf);

		return FALSE;
	}

	//Get CEK
	if (TADC_GetCEK(&t_DeviceInfo, &t_RO, &t_DRMHeader) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : TADC_GetCEK");
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_MEMFree_RO(&t_RO);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	//Decrypt File
	//File Open ( DRM File )
	if ((hFile1 = fopen(pTADCFilepath, "rb")) == INVALID_HOBJ) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : DRM FmOpenFile (%s)", pTADCFilepath);
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_MEMFree_RO(&t_RO);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	//File Create ( Decrypted File )
	if ((hFile2 = fopen(pDecryptedFile, "w+b")) == INVALID_HOBJ) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : Decrypted File FmOpenFile (%s)", pDecryptedFile);
		fclose(hFile1);
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_MEMFree_RO(&t_RO);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	fseek(hFile1, 0, SEEK_END);
	auto size1 = ftell(hFile1);

	auto offset = t_FileHeader.Offset1 + 35 + t_DRMHeader.XmlSize;
	fseek(hFile1, offset, SEEK_SET);

	auto size2 = size1 - offset; //plain file size
	auto BlockCnt = (size2 / 512) + ((size2 % 512) ? 1 : 0);

	auto EncBlockCnt = BlockCnt;
	if (t_DRMHeader.EncryptionRange != -1)
		EncBlockCnt = t_DRMHeader.EncryptionRange;

	long int i = 0;
	while (i < BlockCnt) {
		size_t ReadLen = fread(pReadBuf, 1, TDC_DECRYPT_IOLEN, hFile1);

		if (ReadLen < 1)
			break;

		for (size_t k = 0; k < ReadLen; k += 512) {
			if (i < EncBlockCnt) {
				size_t DecLen = ReadLen - k;
				DecLen = (DecLen > 512) ? 512 : DecLen;

				if (TADC_DecryptBlock((char *)pReadBuf + k, DecLen, &t_DRMHeader) < 0) {
					DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage Error : TADC_DecryptBlock");
					fclose(hFile1);
					fclose(hFile2);
					TADC_MEMFree_FileHeader(&t_FileHeader);
					TADC_MEMFree_DRMHeader(&t_DRMHeader);
					TADC_MEMFree_RO(&t_RO);
					TADC_IF_Free(pReadBuf);
					return FALSE;
				}
			}
			i += 1;
		}
		fwrite(pReadBuf, 1, ReadLen, hFile2);
	}

	fclose(hFile1); //sample drm file
	fclose(hFile2); //plain file

	TADC_MEMFree_FileHeader(&t_FileHeader);
	TADC_MEMFree_DRMHeader(&t_DRMHeader);
	TADC_MEMFree_RO(&t_RO);
	TADC_IF_Free(pReadBuf);

	return TRUE;
}

bool DrmTdcDecryptPackage2(
	IN const char *pTADCFilepath,
	IN T_RO t_RO,
	IN const char *pDecryptedFile)
{
	T_FILE_HEADER t_FileHeader;
	T_DRM_HEADER t_DRMHeader;
	T_DEVICE_INFO t_DeviceInfo;

	FILE *hFile1 = INVALID_HOBJ; //TDC drm file
	FILE *hFile2 = INVALID_HOBJ; //Decrypted file

	//2011.03.08, init
	memset(&t_FileHeader, 0x00, sizeof(T_FILE_HEADER));
	memset(&t_DRMHeader, 0x00, sizeof(T_DRM_HEADER));
	memset(&t_DeviceInfo, 0x00, sizeof(T_DEVICE_INFO));

	if (!pTADCFilepath || !pDecryptedFile) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : Parameter Null");
		return FALSE;
	}

	//Set DeviceInfo
	if (TADC_SetDeviceInfo(&t_DeviceInfo) == TADC_GETDUID_ERROR) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : TADC_SetDeviceInfo");
		return FALSE;
	}

	auto pReadBuf = (TADC_U8*)TADC_IF_Malloc(TDC_DECRYPT_IOLEN);
	if (!pReadBuf) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : pReadBuf Malloc Fail");
		return FALSE;
	}

	//Get DRM Header Info
	if (TADC_GetDRMHeaderFromFile(pTADCFilepath, &t_FileHeader, &t_DRMHeader) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : TADC_GetDRMHeaderFromFile");
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	//Get CEK
	if (TADC_GetCEK(&t_DeviceInfo, &t_RO, &t_DRMHeader) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : TADC_GetCEK");
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	//Decrypt File
	//File Open ( DRM File )
	if ((hFile1 = fopen(pTADCFilepath, "rb")) == INVALID_HOBJ) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : DRM FmOpenFile (%s)", pTADCFilepath);
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	//File Create ( Decrypted File )
	if ((hFile2 = fopen(pDecryptedFile, "w+b")) == INVALID_HOBJ) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : Decrypted File FmOpenFile (%s)", pDecryptedFile);
		fclose(hFile1);
		TADC_MEMFree_FileHeader(&t_FileHeader);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		TADC_IF_Free(pReadBuf);
		return FALSE;
	}

	fseek(hFile1, 0, SEEK_END);
	auto size1 = ftell(hFile1);

	auto offset = t_FileHeader.Offset1 + 35 + t_DRMHeader.XmlSize;
	fseek(hFile1, offset, SEEK_SET );

	auto size2 = size1 - offset; //plain file size
	auto BlockCnt = (size2 / 512) + ((size2 % 512) ? 1 : 0);

	auto EncBlockCnt = BlockCnt;
	if (t_DRMHeader.EncryptionRange != -1)
		EncBlockCnt = t_DRMHeader.EncryptionRange;

	long int i = 0;
	while (i < BlockCnt) {
		auto ReadLen = fread(pReadBuf, 1, TDC_DECRYPT_IOLEN, hFile1);

		if (ReadLen < 1)
			break;

		for (size_t k = 0 ; k < ReadLen ; k += 512) {
			if (i < EncBlockCnt) {
				auto DecLen = ReadLen - k;
				DecLen = ( DecLen > 512) ? 512 : DecLen;

				if (TADC_DecryptBlock((char*)pReadBuf + k, DecLen, &t_DRMHeader) < 0) {
					DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 Error : TADC_DecryptBlock");
					fclose(hFile1);
					fclose(hFile2);
					TADC_MEMFree_FileHeader(&t_FileHeader);
					TADC_MEMFree_DRMHeader(&t_DRMHeader);
					TADC_IF_Free(pReadBuf);
					return FALSE;
				}
			}
			i += 1;
		}
		fwrite(pReadBuf, 1, ReadLen, hFile2);
	}

	fclose(hFile1); //sample drm file
	fclose(hFile2); //plain file

	TADC_MEMFree_FileHeader(&t_FileHeader);
	TADC_MEMFree_DRMHeader(&t_DRMHeader);
	TADC_IF_Free(pReadBuf);

	return TRUE;
}

bool DrmTdcGeneratePurchaseRequest(
	IN const char *pTADCFilepath,        //TDC DRM File Path
	IN OUT char *pReqBuf,                //Purchase Request Data
	IN OUT unsigned int *pReqBufLen,     //IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
	IN OUT char *pLicenseUrl,            //License Acquisition URL Data
	IN OUT unsigned int *pLicenseUrlLen  //IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
)
{
	T_FILE_HEADER t_FileHeader;
	T_DRM_HEADER t_DRMHeader;
	T_DEVICE_INFO t_DeviceInfo;
	char ReqLicBuf[REQU_MAXSIZE] = {0}; //Request buff max size. (2011.03.08)

	//null check
	if (!pTADCFilepath
		|| !pReqBuf
		|| !pLicenseUrl
		|| *pReqBufLen < 512
		|| *pLicenseUrlLen < 512) {
		DRM_TAPPS_EXCEPTION("DrmTdcGeneratePurchaseRequest Error : Parameter Null");
		return FALSE;
	}

	memset(&t_FileHeader, 0x00, sizeof(T_FILE_HEADER));
	memset(&t_DRMHeader, 0x00, sizeof(T_DRM_HEADER));
	memset(&t_DeviceInfo, 0x00, sizeof(T_DEVICE_INFO));

	//Set DeviceInfo
	if (TADC_SetDeviceInfo(&t_DeviceInfo) == TADC_GETDUID_ERROR) {
		DRM_TAPPS_EXCEPTION("DrmTdcGeneratePurchaseRequest Error : TADC_SetDeviceInfo");
		return FALSE;
	}

	//Get DRM Header Info
	if (TADC_GetDRMHeaderFromFile(pTADCFilepath, &t_FileHeader, &t_DRMHeader) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcGeneratePurchaseRequest Error : TADC_GetDRMHeaderFromFile");
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		return FALSE;
	}

	if (TADC_MakeRequestLicense(&t_DeviceInfo, &t_FileHeader, &t_DRMHeader, (unsigned char*)ReqLicBuf, sizeof(ReqLicBuf)) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcGeneratePurchaseRequest Error : TADC_MakeRequestLicense");
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		return FALSE;
	}

	//Copy pReqBuf
	size_t tmpLen = strlen((char *)ReqLicBuf);
	if (*pReqBufLen <= tmpLen) {
		DRM_TAPPS_EXCEPTION("DrmTdcGeneratePurchaseRequest Error : *pReqBufLen(%d) <= %d", *pReqBufLen, tmpLen);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		return FALSE;
	}

	memcpy(pReqBuf, ReqLicBuf, tmpLen + 1);

	//Copy License URL
	tmpLen = strlen((char *)t_DRMHeader.RIURL);
	if (*pLicenseUrlLen <= tmpLen) {
		DRM_TAPPS_EXCEPTION("DrmTdcGeneratePurchaseRequest Error : *pLicenseUrlLen(%d) <= %d", *pLicenseUrlLen, tmpLen);
		TADC_MEMFree_DRMHeader(&t_DRMHeader);
		return FALSE;
	}

	memcpy(pLicenseUrl, t_DRMHeader.RIURL, tmpLen + 1);

	//Free
	TADC_MEMFree_FileHeader(&t_FileHeader);
	TADC_MEMFree_DRMHeader(&t_DRMHeader);

	return TRUE;
}

int DrmTdcGenerateLicenseRequest(
	IN const char *pRespBuf,              //Response Data String of the Purchase Request ( Null terminator string )
	IN unsigned int respBufLen,           //pResBuf Length
	IN OUT char *pReqBuf,                 //License Request Data
	IN OUT unsigned int *pReqBufLen,      //IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
	IN OUT char *pLicenseUrl,             //License Acquisition URL Data
	IN OUT unsigned int *pLicenseUrlLen   //IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
)
{
	T_ROACQ_INFO *pt_ROAcqInfo = NULL;

	char ReqROBuf[REQU_MAXSIZE] = {0}; //Request buff max size. (2011.03.08)
	BYTE sha1_tmp[DHINFO_REQIDLEN] = {0};
	int idx = 0;
	unsigned char ROVer[2]; //2011.03.08

	//null check
	if (!pRespBuf
		|| !pReqBuf
		|| !pLicenseUrl
		|| respBufLen < 1
		|| *pReqBufLen < 512
		||  *pLicenseUrlLen < 512) {
		DRM_TAPPS_EXCEPTION("DrmTdcGenerateLicenseRequest Error : Parameter Null");
		return TADC_PARAMETER_ERROR;
	}

	if (g_DrmTdcDHFlag == FALSE) {
		DrmTdcDHInfoInit();
		g_DrmTdcDHFlag = TRUE;
	}

	for (idx = 0 ; idx < DHINFO_MAX ; idx++) {
		if (!TADC_IF_MemCmp(g_DrmTdcDHInfo.hashReqID[idx], sha1_tmp, DHINFO_REQIDLEN)) {
			pt_ROAcqInfo = &g_DrmTdcDHInfo.t_ROAcqInfo[idx];
			break;
		}
	}

	// Debug
	DRM_TAPPS_LOG("DrmTdcGenerateLicenseRequest ID Idx = %d", idx);

	if (idx == DHINFO_MAX) {
		DRM_TAPPS_EXCEPTION("DrmTdcGenerateLicenseRequest Error : idx == DHINFO_MAX");
		return TADC_DHINFO_MAX_ERROR;
	}

	//Get ReqID
	if (TADC_GetROAcqInfo((unsigned char*)pRespBuf, pt_ROAcqInfo) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcGenerateLicenseRequest Error : TADC_GetROAcqInfo");
		return TADC_GET_ROACQ_INFO_ERROR;
	}

	ROVer[0] = 2;
	ROVer[1] = 0;

	if (TADC_MakeRequestRO(pt_ROAcqInfo, (unsigned char*)ReqROBuf, sizeof(ReqROBuf), ROVer) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcGenerateLicenseRequest Error : TADC_MakeRequestRO");
		return TADC_MAKE_LICENSEREQ_ERROR;
	}

	// Debug
	DRM_TAPPS_LOG("Debug Log == Copy ReqID Hash Successfully Done!");

	//Copy ReqID Hash
	TADC_IF_SHA1(pt_ROAcqInfo -> ReqID, TADC_IF_StrLen((char*)pt_ROAcqInfo -> ReqID), g_DrmTdcDHInfo.hashReqID[idx]);

	//Copy pReqBuf
	size_t tmpLen = strlen((char *)ReqROBuf);
	if (*pReqBufLen <= tmpLen) {
		DRM_TAPPS_EXCEPTION("DrmTdcGenerateLicenseRequest Error : *pReqBufLen(%d) <= %d", *pReqBufLen, tmpLen);
		return TADC_MEMAlOC_ERROR;
	}

	memcpy(pReqBuf, ReqROBuf, tmpLen + 1);

	//Copy License URL
	tmpLen = strlen((char *)pt_ROAcqInfo->ROAcqURL);
	if (*pLicenseUrlLen <= tmpLen) {
		DRM_TAPPS_EXCEPTION("DrmTdcGenerateLicenseRequest Error : *pLicenseUrlLen(%d) <= %d", *pLicenseUrlLen, tmpLen);
		return TADC_MEMAlOC_ERROR;
	}

	memcpy(pLicenseUrl, (char *)pt_ROAcqInfo->ROAcqURL, tmpLen + 1);

	// Debug
	DRM_TAPPS_LOG("Debug Log == DrmTdcGenerateLicenseRequest Successfully Done!");

	return TADC_SUCCESS;
}

int DrmTdcDecryptLicense(
	IN const char *pRespBuf,              //Response Data String of the Rights Request ( Null terminator string )
	IN unsigned int respBufLen,           //pResBuf Length
	IN OUT char *pDecLicenseBuf,          //Decrypted Rights Object
	IN OUT unsigned int *decLicenseBufLen //IN : pDecLicenseBuf Length, OUT : Decrypted Rights Object String Size (  including null terminator )
)
{
	T_ROACQ_INFO *pt_ROAcqInfo = NULL;
	T_RO t_RO;
	int ret = 0;
	int idx = 0;
	BYTE sha1_tmp[20] = {0};

	//null check
	if (!pRespBuf
		|| !pDecLicenseBuf
		|| respBufLen < 1
		|| *decLicenseBufLen < 512) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptLicense Error : Parameter Null  \n");
		return TADC_PARAMETER_ERROR;
	}

	memset(&t_RO, 0x00, sizeof(T_RO));

	if (TADC_GetHashReqID((unsigned char*)pRespBuf, sha1_tmp) < 0) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptLicense Error : TADC_GetHashReqID  \n");
		return TADC_GET_HASHREQID_ERROR;
	}

	for (idx = 0; idx < DHINFO_MAX; idx++) {
		if (!TADC_IF_MemCmp(g_DrmTdcDHInfo.hashReqID[idx], sha1_tmp, 20)) {
			pt_ROAcqInfo = &g_DrmTdcDHInfo.t_ROAcqInfo[idx];
			break;
		}
	}

	if (idx == DHINFO_MAX) {
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptLicense Error : idx == DHINFO_MAX!, Session-key is not same.\n");
		return TADC_DHINFO_MAX_ERROR;
	}

	//Get Decrypted License
	if ((ret = TADC_GetResponseRO((unsigned char*)pRespBuf, pt_ROAcqInfo, &t_RO, (unsigned char*)pDecLicenseBuf)) < 0) {
		DrmTdcDHInfoFree(idx); //2010.02.26
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptLicense Error : TADC_GetResponseRO! Ret[%x]", ret);
		return TADC_GET_RORES_INFO_ERROR;
	}
	*decLicenseBufLen = strlen(pDecLicenseBuf) + 1;

	// Check Signature and Certificate Chain
	if ((ret = TADC_VerifyROSignature((unsigned char*)pDecLicenseBuf))) {
		DRM_TAPPS_EXCEPTION("TADC_VerifyROSignature Error Code = %x", ret);
		return TADC_RO_SIGNATURE_ERROR;
	}

	//2011.03.08, return time stamp
	memcpy(g_sTimeStamp, pt_ROAcqInfo->sTimeStamp, strlen((char *)pt_ROAcqInfo->sTimeStamp) + 1);

	//Free
	TADC_MEMFree_RO(&t_RO);
	DrmTdcDHInfoFree(idx);

	return TADC_SUCCESS;
}

void DrmTdcDHInfoInit(void)
{
	memset(&g_DrmTdcDHInfo, 0x00, sizeof(DrmTdcDHInfo));
}

bool DrmTdcDHInfoFree(int idx)
{
	memset(&g_DrmTdcDHInfo.hashReqID[idx], 0x00, sizeof(g_DrmTdcDHInfo.hashReqID[idx]));

	if (TADC_MEMFree_ROAcqInfo(&g_DrmTdcDHInfo.t_ROAcqInfo[idx]) < 0)
		return FALSE;

	return TRUE;
}

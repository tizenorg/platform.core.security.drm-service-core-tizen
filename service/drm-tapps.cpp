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
 *
 * @file     drm-tapps.cpp
 * @author   Sunggun Jung    <sunggun.jung@samsung.com>
 *
 */

#include "TADC_Core.h"
#include "TADC_IF.h"

#include "DrmTdcSvc.h"
#include "drm-tizen-mid.h"
#include "drm-tizen-apps.h"
#include "drm-tizen-error.h"

#include "DTapps2SqliteDB.h"
#include "DTapps2Base64.h"
#include "DTapps2HMAC.h"
#include "DTapps2Rights.h"

/* Enable this flag to take API level time profiling */
//#define __DRM_TAPPS_API_TIME_PROFILING__

#ifdef __DRM_TAPPS_API_TIME_PROFILING__
#include <sys/time.h>

class DrmTappsProf
{
public:
	DrmTappsProf(const char* function);
	~DrmTappsProf();
	struct timeval TappsStartTv;
	struct timeval TappsStopTv;
	char TappsfunNane[256];
};

DrmTappsProf::DrmTappsProf(const char* function)
{
	memcpy(TappsfunNane, function, strlen(function));
	gettimeofday(&TappsStartTv, NULL);
	DRM_TAPPS_EXCEPTION("[DRM-TIZEN-PERF]START:SEC=%ld, USEC=%ld for [%s]",(long int)(TappsStartTv.tv_sec) , (long int)(TappsStartTv.tv_usec),function);
}

DrmTappsProf::~DrmTappsProf()
{
	gettimeofday(&TappsStopTv, NULL);
	DRM_TAPPS_EXCEPTION("[DRM-TIZEN-PERF] STOP:SEC=%ld, USEC=%ld for [%s]", (long int)(TappsStopTv.tv_sec), (long int)(TappsStopTv.tv_usec),TappsfunNane);
	DRM_TAPPS_EXCEPTION("[DRM-TIZEN-PERF]TOTAL_DIFFF  : USEC=%ld for [%s]", ((long int)(TappsStopTv.tv_sec - TappsStartTv.tv_sec) * (1000000) + (long int)(TappsStopTv.tv_usec - TappsStartTv.tv_usec)), TappsfunNane);
}

#define DRM_TAPPS_API_TIME() DrmTappsProf DrmTappsObj(__func__);
#else
#define DRM_TAPPS_API_TIME()
#endif /* __DRM_TAPPS_API_TIME_PROFILING__ */

#ifdef __cplusplus
extern "C" {
#endif

/* Define EXPORT_API */
#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif

int _drm_tapps_generate_purchase_request(const char *pTADCFilepath, char *pReqBuf, unsigned int *pReqBufLen, char *pLicenseUrl, unsigned int *pLicenseUrlLen);
int _drm_tapps_generate_license_request(const char *pRespBuf, unsigned int respBufLen, char *pReqBuf, unsigned int *pReqBufLen, char *pLicenseUrl, unsigned int *pLicenseUrlLen);
int _drm_tapps_register_license(const char *pRespBuf, unsigned int respBufLen);
int _drm_tapps_decrypt_package(const char *pTADCFilepath, int stadFileLen, const char *pDecryptedFile, int decryptedFileLen);
int _drm_tapps_is_drm_file(const char *pDcfPath, int dcfPathLen);

EXPORT_API int drm_tizen_generate_license_request(
	const char *pRespBuf,        //Response Data String of the Purchase Request ( Null terminator string )
	unsigned int respBufLen,     //pResBuf Length
	char *pReqBuf,               //License Request Data
	unsigned int *pReqBufLen,    //IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
	char *pLicenseUrl,           //License Acquisition URL Data
	unsigned int *pLicenseUrlLen //IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
)
{
	int ret = TADC_SUCCESS;
	DRM_TAPPS_LOG("%s starts", __func__);

	if (pRespBuf == NULL || pReqBuf == NULL || pLicenseUrl == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Error : Parameters NULL.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	if (strlen(pRespBuf) != respBufLen)
	{
		DRM_TAPPS_EXCEPTION("%s) Error : Input value and size weren't equal.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	ret = _drm_tapps_generate_license_request(pRespBuf, respBufLen,	pReqBuf, pReqBufLen, pLicenseUrl, pLicenseUrlLen);
	DRM_TAPPS_LOG("%s result=%x", __func__, ret);

	return ret;
}

int _drm_tapps_generate_license_request(
	const char *pRespBuf,        //Response Data String of the Purchase Request ( Null terminator string )
	unsigned int respBufLen,     //pResBuf Length
	char *pReqBuf,               //License Request Data
	unsigned int *pReqBufLen,    //IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
	char *pLicenseUrl,           //License Acquisition URL Data
	unsigned int *pLicenseUrlLen //IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
)
{
	int ret = TADC_SUCCESS;
	DRM_TAPPS_LOG("%s starts", __func__);

	if (pRespBuf == NULL || pReqBuf == NULL || pLicenseUrl == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Error : Parameters NULL.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	if (strlen(pRespBuf) != respBufLen)
	{
		DRM_TAPPS_EXCEPTION("%s) Error : Input value and size weren't equal.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	ret = DrmTdcGenerateLicenseRequest( pRespBuf, respBufLen, pReqBuf, pReqBufLen, pLicenseUrl, pLicenseUrlLen );
	DRM_TAPPS_LOG("%s result = %x", __func__, ret);

	return ret;
}

EXPORT_API int drm_tizen_register_license
(
	const char *pRespBuf,				//Response Data String of the Rights Request ( Null terminator string )
	unsigned int respBufLen 			//pResBuf Length
)
{
	int ret = TADC_SUCCESS;
	DRM_TAPPS_LOG("%s starts", __func__);

	if (pRespBuf == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Parameters NULL!", __func__);
		return TADC_PARAMETER_ERROR;
	}

	if (strlen(pRespBuf) != respBufLen)
	{
		DRM_TAPPS_EXCEPTION("%s) Input value and size wasn't equal.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	ret = _drm_tapps_register_license(pRespBuf, respBufLen);
	DRM_TAPPS_LOG("%s result=%x", __func__, ret);

	return ret;
}

int _drm_tapps_register_license(const char *pRespBuf, unsigned int respBufLen)
{
	DRM_TAPPS_API_TIME()

	int Ret = TADC_SUCCESS;
	char pDecLicenseBuf[1024*8] = {0, };
	unsigned int decLicenseBufLen = 0;

	DRM_TAPPS_LOG("%s starts", __func__);

	if(pRespBuf == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Parameters NULL!", __func__);
		return TADC_PARAMETER_ERROR;
	}

	if (strlen(pRespBuf) != respBufLen)
	{
		DRM_TAPPS_EXCEPTION("%s) Input value and size weren't equal.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	memset(pDecLicenseBuf, 0x00, sizeof(pDecLicenseBuf));
	decLicenseBufLen = sizeof(pDecLicenseBuf);

	/* Has to be enabled when the response from server is known which will be encrypted!! */
	Ret = DrmTdcDecryptLicense(pRespBuf, respBufLen, pDecLicenseBuf, &decLicenseBufLen);
	if (Ret != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptLicense failed!!! Ret = %x",Ret);
		return Ret;
	}

	Ret = DTappsInstallLicense(pDecLicenseBuf);
	if (Ret != TADC_SUCCESS)
	{
		DRM_TAPPS_EXCEPTION("DTappsInstallLicense failed!!! Ret = %x", Ret);
		goto DTAPPS_EXIT;
	}

	DRM_TAPPS_LOG("%s result = %x", __func__, Ret);

DTAPPS_EXIT:
	return Ret;
}


EXPORT_API int drm_tizen_is_drm_file(const char *pDcfPath, int dcfPathLen)
{
	int ret = TADC_SUCCESS;

	DRM_TAPPS_LOG("%s starts", __func__);
	if (pDcfPath == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Parameters NULL!", __func__);
		return TADC_PARAMETER_ERROR;
	}

	if ((int)strlen(pDcfPath) != dcfPathLen)
	{
		DRM_TAPPS_EXCEPTION("%s) Input value and size wasn't equal.", __func__);
		return TADC_PARAMETER_ERROR;
	}

	DRM_TAPPS_LOG("_drm_tizen_is_drm_file = %s started!", pDcfPath);

	ret = _drm_tapps_is_drm_file(pDcfPath, dcfPathLen);
	DRM_TAPPS_LOG("%s result=%x", __func__, ret);

	return ret;
}

int _drm_tapps_is_drm_file(const char *pDcfPath, int dcfPathLen)
{
	DRM_TAPPS_API_TIME()

	int ret = TADC_SUCCESS;
	bool bRet = FALSE;
	DrmTdcFileHeader fileHeader;

	DRM_TAPPS_LOG("%s starts", __func__);
	if (pDcfPath == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Parameters NULL!", __func__);
		ret = TADC_PARAMETER_ERROR;
		goto finish;
	}

	if ((int)strlen(pDcfPath) != dcfPathLen)
	{
		DRM_TAPPS_EXCEPTION("%s) Input value and size wasn't equal.", __func__);
		ret = TADC_PARAMETER_ERROR;
		goto finish;
	}

	memset(&fileHeader, 0, sizeof(DrmTdcFileHeader));
	bRet = DrmTdcGetFileHeader(pDcfPath, &fileHeader);
	if (bRet == FALSE)
	{
		DRM_TAPPS_EXCEPTION("%s Error : DrmTdcGetFileHeader is failed", __func__);
		ret = TADC_NOTTADCFILE_ERROR;
	}

	DRM_TAPPS_LOG("%s result = %x", __func__, ret);

finish:
	return ret;
}

EXPORT_API int drm_tizen_decrypt_package(
 const char *pTADCFilepath,   /* TDC DRM File Path  */
 int stadFileLen,
 const char *pDecryptedFile,   /* Decrypted File Path */
 int decryptedFileLen
)
{
	int ret = TADC_SUCCESS;
	DRM_TAPPS_LOG("drm_tizen_decrypt_package(%s, %s) started!", pTADCFilepath, pDecryptedFile);

	ret = _drm_tapps_decrypt_package(pTADCFilepath, stadFileLen, pDecryptedFile, decryptedFileLen);
	DRM_TAPPS_LOG("%s result=%x", __func__, ret);

	return ret;
}

int _drm_tapps_decrypt_package
(
 const char *pTADCFilepath,   //TDC DRM File Path
 int stadFileLen,
 const char *pDecryptedFile,   //Decrypted File Path
 int decryptedFileLen
)
{
	DRM_TAPPS_API_TIME()

	bool bRet = true;
	int ret = TADC_SUCCESS;

	T_RO t_RO;
	DrmTdcFileHeader fileHeader;

	memset(&t_RO, 0x00, sizeof(t_RO));
	memset(&fileHeader, 0x00, sizeof(fileHeader));

	DRM_TAPPS_LOG("%s starts", __func__);
	if (pTADCFilepath == NULL || pDecryptedFile == NULL)
	{
		DRM_TAPPS_EXCEPTION("%s) Parameters NULL!", __func__);
		return TADC_PARAMETER_ERROR;
	}

	if (((int)strlen(pTADCFilepath) != stadFileLen ) || ((int)strlen(pDecryptedFile) != decryptedFileLen))
	{
		DRM_TAPPS_EXCEPTION("%s) Input value and size wasn't equal.", __func__);
		return TADC_PARAMETER_ERROR;
	}
	DRM_TAPPS_SECURE_LOG("%s) TADC File Path=%s, Decrypted File Path=%s", __func__, pTADCFilepath, pDecryptedFile);

	bRet = DrmTdcGetFileHeader(pTADCFilepath, &fileHeader);
	if (FALSE == bRet)
	{
		DRM_TAPPS_EXCEPTION("DrmTdcGetFileHeader failed!!");
		ret = TADC_GET_FILEHEADER_ERROR;
		goto TAPPS_END;
	}
	DRM_TAPPS_LOG("pTADCFilepath=%s, fileHeader.cid=%s, fileHeader.riurl=%s", pTADCFilepath, fileHeader.cid, fileHeader.riurl);

	bRet = DTappsGetCEK(fileHeader.cid,&t_RO);
	if(FALSE == bRet)
	{
		DRM_TAPPS_EXCEPTION("DTappsGetCEK failed!! pTADCFilepath=%s, fileHeader.cid=%s", pTADCFilepath, fileHeader.cid);
		ret = TADC_GET_CEK_ERROR;
		goto TAPPS_END;
	}
	DRM_TAPPS_SECURE_LOG("fileHeader.cid=%s, t_RO.t_Content.CEK=%s", fileHeader.cid, t_RO.t_Content.CEK);

	bRet = DrmTdcDecryptPackage2(pTADCFilepath, t_RO, pDecryptedFile);
	if(FALSE == bRet)
	{
		DRM_TAPPS_EXCEPTION("DrmTdcDecryptPackage2 failed!!, pTADCFilepath=%s, pDecryptedFile=%s",pTADCFilepath,pDecryptedFile);
		ret = TADC_DECRYPT_PACKAGE_ERROR;
		goto TAPPS_END;
	}

TAPPS_END:
	if (t_RO.t_Content.CID)
	{
		DTAPPS_FREE(t_RO.t_Content.CID);
	}
	if (t_RO.t_Content.CEK)
	{
		DTAPPS_FREE(t_RO.t_Content.CEK);
	}
	if ((t_RO.PerFlag & DUID_RULE) && (t_RO.t_Permission.t_Individual.DUID))
	{
		DTAPPS_FREE(t_RO.t_Permission.t_Individual.DUID);
	}
	if (bRet == FALSE)
	{
		DRM_TAPPS_EXCEPTION("%s failed!! pTADCFilepath=%s, pDecryptedFile=%s", __func__, pTADCFilepath, pDecryptedFile);
		return ret;
	}
	else
	{
		DRM_TAPPS_LOG("%s Success!!", __func__);
		return ret;
	}
}

EXPORT_API int drm_tizen_generate_purchase_request
(
	const char *pTADCFilepath,		//TDC DRM File Path
	char *pReqBuf, 				//Purchase Request Data
	unsigned int *pReqBufLen,		//IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
	char *pLicenseUrl, 			//License Acquisition URL Data
	unsigned int *pLicenseUrlLen	//IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
)
{
	DRM_TAPPS_LOG("%s started!", __func__);

	return _drm_tapps_generate_purchase_request(pTADCFilepath,
												pReqBuf,
												pReqBufLen,
												pLicenseUrl,
												pLicenseUrlLen);
}

int _drm_tapps_generate_purchase_request
(
	const char *pTADCFilepath,		//TDC DRM File Path
	char *pReqBuf, 				//Purchase Request Data
	unsigned int *pReqBufLen,		//IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
	char *pLicenseUrl, 			//License Acquisition URL Data
	unsigned int *pLicenseUrlLen	//IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
)
{
	DRM_TAPPS_API_TIME()

	bool bRet = FALSE;

	DRM_TAPPS_LOG("%s starts", __func__);
	bRet = DrmTdcGeneratePurchaseRequest(pTADCFilepath, pReqBuf, pReqBufLen, pLicenseUrl, pLicenseUrlLen);
	if (bRet == FALSE)
	{
		return 0;
	}
    else
	{
		return 1;
	}
}

#ifdef __cplusplus
}
#endif

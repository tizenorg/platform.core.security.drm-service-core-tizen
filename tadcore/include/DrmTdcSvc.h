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
 * @file	DrmTdcSvc.h
 * @brief	This file includes definitions of the Tizen Apps DRM functions.
 * @author	Sunggun.jung (sunggun.jung@samsung.com)
 */

#ifndef DRMTDCSVC_H
#define DRMTDCSVC_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "TADC_Core.h"


//TDC DRM File Header Info
typedef struct
{
	char cid[1024];		// CID ( Content ID )
	char riurl[1024];	// RIURL ( Rights Issuer URL )
		
} DrmTdcFileHeader;

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

/**   
 *  An application can get file Header Info ( CID, License URL )
 * 
 * @param[in]		filePath		file path of Tizen Apps DRM contents
 * @param[out]	pFileHeader	Pointer to be stored TAD Header information.
 * @return		This function	returns TRUE on success or FALSE on failure.
 * @remarks     
 * @see	      
 * @since         
 */
bool DrmTdcGetFileHeader
(
	IN const char *pTADCFilepath, 			//TDC DRM File Path
	IN OUT DrmTdcFileHeader *pFileHeader	//File Header Info ( CID, License URL )
);

/**   
 *  An application can request purchase request for getting License.
 * 
 * @param[in]			pTADCFilepath	File path of Tizen Apps DRM contents
 * @param[out]		pReqBuf	Purchase Request Data
 * @param[in][out]	pReqBufLen	IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
 * @param[out]		pLicenseUrl	License Acquisition URL Data
 * @param[in][out]	pLicenseUrlLen	IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
 * @return			This function	returns TRUE on success or FALSE on failure.
 * @remarks     
 * @see				DrmTdcGenerateLicenseRequest
 * @since         
 */
bool DrmTdcGeneratePurchaseRequest
(
	IN const char *pTADCFilepath,		//TDC DRM File Path
	IN OUT char *pReqBuf, 				//Purchase Request Data
	IN OUT unsigned int *pReqBufLen,		//IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
	IN OUT char *pLicenseUrl, 			//License Acquisition URL Data
	IN OUT unsigned int *pLicenseUrlLen	//IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
);

/**   
 *  An application can request generate request for getting License.
 * 
 * @param[in]			pRespBuf			Response Data String of the Purchase Request ( Null terminator string ) 
 * @param[in]			respBufLen		pResBuf Length
 * @param[in][out]	pReqBuf			License Request Data
 * @param[in][out]	pReqBufLen		IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
 * @param[in][out]	pLicenseUrl		License Acquisition URL Data
 * @param[in][out]	pLicenseUrlLen	IN : IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
 * @return			This function	returns TRUE on success or FALSE on failure.
 * @remarks     
 * @see		DrmTdcGeneratePurchaseRequest
 * @since         
 */
int DrmTdcGenerateLicenseRequest
(
	IN const char *pRespBuf, 			//Response Data String of the Purchase Request ( Null terminator string ) 
	IN unsigned int respBufLen,			//pResBuf Length
	IN OUT char *pReqBuf, 				//License Request Data
	IN OUT unsigned int *pReqBufLen,		//IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
	IN OUT char *pLicenseUrl, 			//License Acquisition URL Data
	IN OUT unsigned int *pLicenseUrlLen	//IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
);

/**   
 *  An application can get TAD license which is encrypted.
 * 
 * @param[in]			pRespBuf			Response Data String of the Rights Request ( Null terminator string ) 
 * @param[in]			respBufLen		pResBuf Length
 * @param[in][out]	pDecLicenseBuf	Decrypted Rights Object
 * @param[in][out]	decLicenseBufLen	IN : pDecLicenseBuf Length, OUT : Decrypted Rights Object String Size (  including null terminator )
 * @return		This function	returns TRUE on success or FALSE on failure.
 * @remarks     
 * @see		DrmTdcDecryptPackage
 * @since         
 */
int DrmTdcDecryptLicense
(
	IN const char *pRespBuf, 				//Response Data String of the Rights Request ( Null terminator string ) 
	IN unsigned int respBufLen,				//pResBuf Length
	IN OUT char *pDecLicenseBuf, 			//Decrypted Rights Object
	IN OUT unsigned int *decLicenseBufLen	//IN : pDecLicenseBuf Length, OUT : Decrypted Rights Object String Size (  including null terminator )
);

/**   
 *  An application can get decrypted contents(Apps) which is encrypted.
 * 
 * @param[in]		pTADCFilepath	TDC DRM File Path
 * @param[in]		pLicenseBuf		pResBuf Length
 * @param[in]		licenseBufLen		pDecLicenseBuf Length
 * @param[in]		pDecryptedFile	Decrypted File Path
 * @return		This function	returns TRUE on success or FALSE on failure.
 * @remarks     
 * @see		DrmTdcDecryptLicense
 * @since         
 */
//Decrypt DRM File
bool DrmTdcDecryptPackage
(
	IN const char *pTADCFilepath, 		//TDC DRM File Path
	IN const char *pLicenseBuf, 			//Decrypted Rights Object
	IN unsigned int licenseBufLen, 		//pDecLicenseBuf Length
	IN const char *pDecryptedFile			//Decrypted File Path
);

/**   
 *  An application can get decrypted contents(Apps) which is encrypted.
 * 
 * @param[in]		pTADCFilepath	TDC DRM File Path
 * @param[in]		t_RO			Decrypted RO Info
 * @param[in]		pDecryptedFile	Decrypted File Path
 * @return		This function	returns TRUE on success or FALSE on failure.
 * @remarks     
 * @see		DrmTdcDecryptLicense
 * @since         
 */
//Decrypt DRM File
bool DrmTdcDecryptPackage2
(
	IN const char *pTADCFilepath, 		//TDC DRM File Path
	IN T_RO		  t_RO, 				//RO Info	
	IN const char *pDecryptedFile		//Decrypted File Path
);

/**   
 *  An application can check contents(Apps) which is valid.
 * 
 * @param[in]		szCid	CID ( Content ID )
 * @see		DrmTdcGetFileHeader
 * @since         
 */
//Decrypt DRM File
bool DrmTdcHasValidLicense
(
	IN const char *szCid // CID ( Content ID )
);

//DrmTdcDHInfo Structure Init
void DrmTdcDHInfoInit(void);

//DrmTdcDHInfo Structure Free
bool DrmTdcDHInfoFree(int idx);

#ifdef  __cplusplus
}
#endif

#endif

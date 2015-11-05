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
 * @file     drm-service-apps.h
 * @author   Sunggun Jung    <sunggun.jung@samsung.com>
 * 
 */

#ifndef __DRM_TIZEN_APPS_H__
#define __DRM_TIZEN_APPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "drm-tizen-error.h"


/**
* @internal
* @defgroup DRM-TAPPS drm-tapps
* @{  
*/

/**   
 *  An application can request generate request for getting License.
 * 
 * @param[in]        pRespBuf          Response Data String of the Purchase Request ( Null terminator string )
 * @param[in]        respBufLen        pResBuf Length
 * @param[in][out]   pReqBuf           License Request Data
 * @param[in][out]   pReqBufLen        IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
 * @param[in][out]   pLicenseUrl       License Acquisition URL Data
 * @param[in][out]   pLicenseUrlLen    IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
 * @return           This function returns TADC_SUCCESS(1) on success or an integer value other than 1 on failure.
 * @remarks     
 * @see              drm_tdc_generate_purchase_request
 * @since         
 */
int drm_tizen_generate_license_request(const char *pRespBuf, unsigned int respBufLen, char *pReqBuf, unsigned int *pReqBufLen, char *pLicenseUrl, unsigned int *pLicenseUrlLen);

/**   
 *  An application can get TAD license which is encrypted.
 *
 * @param[in]        pRespBuf          Response Data String of the Rights Request ( Null terminator string )
 * @param[in]        respBufLen        pResBuf Length
 * @return           This function returns TADC_SUCCESS(1) on success or an integer value other than 1 on failure.
 * @remarks     
 * @see
 * @since         
 */
int drm_tizen_register_license(const char *pRespBuf, unsigned int respBufLen);

/**   
 * API for checks whether the app drm file or not
 * 
 * @param[in]        pDcfPath          File path that to be checked whether the app drm file or not
 * @param[in]        dcfPathLen        File path length that to be checked whether the app drm file or not
 * @return           This function returns TADC_SUCCESS(1) on success or an integer value other than 1 on failure.
 * @remarks
 * @see
 * @since         
 */
int drm_tizen_is_drm_file(const char *pDcfPath, int dcfPathLen);

/**   
 *  An application can get decrypted contents(Apps) which is encrypted.
 * 
 * @param[in]        pTADCFilepath     TDC DRM File Path        
 * @param[in]        tadcFileLen       TDC DRM File length    
 * @param[in]        pDecryptedFile    Decrypted File Path
 * @param[in]        decryptedFileLen  Decrypted File length
 * @return           This function returns TADC_SUCCESS(1) on success or an integer value other than 1 on failure.
 * @remarks     
 * @see
 * @since         
 */
//Decrypt DRM File
int drm_tizen_decrypt_package ( const char *pTADCFilepath, int tadcFileLen, const char *pDecryptedFile, int decryptedFileLen );

/**   
 *  An application can request purchase request for getting License.
 * 
 * @param[in]        pTADCFilepath     File path of Tizen Apps DRM contents
 * @param[out]       pReqBuf           Purchase Request Data
 * @param[in][out]   pReqBufLen        IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
 * @param[out]       pLicenseUrl       License Acquisition URL Data
 * @param[in][out]   pLicenseUrlLen    IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
 * @return           This function returns TADC_SUCCESS(1) on success or an integer value other than 1 on failure.
 * @remarks     
 * @see                DrmTdcGenerateLicenseRequest
 * @since         
 */
int drm_tizen_generate_purchase_request ( const char *pTADCFilepath, char *pReqBuf, unsigned int *pReqBufLen, char *pLicenseUrl, unsigned int *pLicenseUrlLen );

/**
*@}
*/ /* Doxygen : addtogroup*/

#ifdef __cplusplus
}
#endif

#endif

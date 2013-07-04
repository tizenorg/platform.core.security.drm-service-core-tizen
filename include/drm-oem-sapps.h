/*
 * drm-service-core-sapps
 *
 * Copyright (c) 2000 - 2010 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact:Sunggun Jung	<sunggun.jung@samsung.com>
 * 
 */

#ifndef __DRM_OEM_SAPPS_H__
#define __DRM_OEM_SAPPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

/* Return values from Samsung Apps DRM APIs */
 #define DRM_SAPPS_SUCCESS 0        /**<success */
 #define DRM_SAPPS_LIC_VALID 1       /**<License is valid */
 #define DRM_SAPPS_LIC_SUSPEND 2       /**<License is valid but for future use */
 
 /* All the failure cases will have a negative value */   
 #define DRM_SAPPS_INVALID_ARG -1000      /**<invalid argument*/
 #define DRM_SAPPS_PARSING_ERROR -999      /**<parsing contents or rights error*/
 #define DRM_SAPPS_DB_ERROR -998        /**<data base error */
 #define DRM_SAPPS_INSUFFICIENT_MEMORY -997      /**<memory error */
 #define DRM_SAPPS_INTERNAL_ERROR -996      /**<processing error */
 #define DRM_SAPPS_FILE_ERROR -995       /**<file error */

 /* License specific errors */
 #define DRM_SAPPS_LIC_EXPIRED_DATETIME -994     /**<Datetime License is expired */
 #define DRM_SAPPS_LIC_EXPIRED_COUNT -993     /**<Count License is expired */
 #define DRM_SAPPS_LIC_NO_LICENCE -992      /**<No License */
 #define DRM_SAPPS_LIC_IMEI_MISMATCH -991     /**<IMEI is mismatching */
 #define DRM_SAPPS_LIC_PNUM_MISMATCH -990     /**<PNUM is mismatching */
 
 #define DRM_SAPPS_UNKNOWN_ERROR -1     /**<dummy error value */

/* License Information */
typedef struct {
	unsigned int IsLicenseValid;  	/**<whether license is valid or not */
	unsigned int OrgCount;   		/**<Original Count */
	unsigned int CurCount;   		/**<Current Count */
	struct tm starttime; 			/**<starttime in the format of struct tm */
	struct tm endtime; 				/**<endtime in the format of struct tm */
	struct tm securetime; 			/**<securetime in the format of struct tm */
} drm_sapps_license_info;

typedef struct _drm_sapps_license_list{
	struct _drm_sapps_license_list*   pnext;
	drm_sapps_license_info    LicenseInfo;
} drm_sapps_license_list;
/**
* @internal
* @defgroup DRM-SAPPS drm-sapps
* @{  
*/

/**   
 *  An application can request generate request for getting License.
 * 
 * @param[in]		pRespBuf			Response Data String of the Purchase Request ( Null terminator string ) 
 * @param[in]		respBufLen		pResBuf Length
 * @param[in][out]	pReqBuf			License Request Data
 * @param[in][out]	pReqBufLen		IN : pReqBuf Length, OUT : Rights Request Data String Size ( including null terminator )
 * @param[in][out]	pLicenseUrl		License Acquisition URL Data
 * @param[in][out]	pLicenseUrlLen	IN : pLicenseUrl Length, OUT : Rights Issuer Server URL Data String Size (  including null terminator )
 * @return						This function	returns 1 on success or 0 on failure.
 * @remarks     
 * @see							drm_sdc_generate_purchase_request
 * @since         
 */
int drm_oem_sapps_generate_license_request(const char *pRespBuf, unsigned int respBufLen, char *pReqBuf, unsigned int *pReqBufLen, char *pLicenseUrl, unsigned int *pLicenseUrlLen);

/**   
 *  An application can get SAD license which is encrypted.
 * 
 * @param[in]		pRespBuf			Response Data String of the Rights Request ( Null terminator string ) 
 * @param[in]		respBufLen		pResBuf Length
 * @return						This function returns 1 on success or 0 on failure.
 * @remarks     
 * @see		
 * @since         
 */
 int drm_oem_sapps_register_license(const char *pRespBuf, unsigned int respBufLen);

/**   
 * API for checks whether the app drm file or not
 * 
 * @param[in]  	pDcfPath	File path that to be checked whether the app drm file or not
 * @param[in]  	dcfPathLen	File path length that to be checked whether the app drm file or not
 * @return  This function can return any of the following codes
 * SADC_NOTSADCFILE_ERROR = 0x0640
 * SADC_PARAMETER_ERROR = 0X0660
 * SADC_FILE_READ_ERROR = 0x0703
 * SADC_MEMAlOC_ERROR = 0x0662 
 * @remarks
 * @remarks
 * @see
 * @since         
 */
 int drm_oem_sapps_is_drm_file(const char *pDcfPath, int dcfPathLen);

/**   
 *  An application can get decrypted contents(Apps) which is encrypted.
 * 
 * @param[in]		pSADCFilepath	SDC DRM File Path		
 * @param[in]		sadcFileLen		SDC DRM File length	
 * @param[in]		pDecryptedFile	Decrypted File Path
 * @param[in]		decryptedFileLen	Decrypted File length
 * @return
 * SADC_SUCCESS = 0
 * SADC_PARAMETER_ERROR = 0X0660
 * SADC_GET_FILEHEADER_ERROR = 0x0703
 * SADC_GET_CEK_ERROR = 0x0704
 * SADC_DECRYPT_PACKAGE_ERROR = 0x0705
 * @remarks     
 * @see		
 * @since         
 */
//Decrypt DRM File
 int drm_oem_sapps_decrypt_package ( const char *pSADCFilepath, int sadcFileLen, const char *pDecryptedFile, int decryptedFileLen );

/**   
 * API for checks whether the app drm file or not
 * 
 * @param[in]  	pDcfPath	File path that to be checked whether the app drm file or not
 * @param[in]  	dcfPathLen	File path length that to be checked whether the app drm file or not
 * @return  This function can return any of the following codes
 * SADC_SUCCESS = 0
 * SADC_NOTSADCFILE_ERROR = 0x0640
 * SADC_PARAMETER_ERROR = 0X0660
 * SADC_FILE_READ_ERROR = 0x0703
 * SADC_MEMAlOC_ERROR = 0x0662 
 * @remarks
 * @remarks
 * @see
 * @since         
 */
 int drm_oem_sapps_is_drm_app(const char *pszCid, int cidLen);

 int drm_oem_sapps_uninstall_license(const char *szCid, int cidLen);

/**   
 *  An application can get file Header Info ( CID, License URL )
 * 
 * @param[in]		pSADCFilepath	file path of Samsung Apps DRM contents 
 * @param[out]	cid				contents id of Samsung Apps DRM 
 * @param[in]		cidLength			Length of CID. 
 * @param[out]	riurl				Right Issuer URL 
 * @param[in]		riurlLength		Length of Right Issuer URL
 * @return		This function returns 1 on success or 0 on failure.
 * @remarks     
 * @see
 * @since
 */
 int drm_oem_sapps_get_file_header( const char *pSADCFilepath, char *cid, unsigned int cidLength, char *riurl, unsigned int riurlLength );

/**   
 *  An application can request purchase request for getting License.
 * 
 * @param[in]		pSADCFilepath	File path of Samsung Apps DRM contents 
 * @param[out]	pReqBuf			Purchase Request Data
 * @param[in][out]	pReqBufLen		IN : pReqBuf Length, OUT : Purchase Request Data String Size ( including null terminator )
 * @param[out]	pLicenseUrl		License Acquisition URL Data
 * @param[in][out]	pLicenseUrlLen	IN : pLicenseUrl Length, OUT : License Server URL Data String Size (  including null terminator )
 * @return						This function returns 1 on success or 0 on failure.
 * @remarks     
 * @see				DrmSdcGenerateLicenseRequest
 * @since         
 */
 int drm_oem_sapps_generate_purchase_request ( const char *pSADCFilepath, char *pReqBuf, unsigned int *pReqBufLen, char *pLicenseUrl, unsigned int *pLicenseUrlLen );

/**   
 *  An application can check contents(Apps) which has a  valid licence.
 * 
 * @param[in]  szCid  Content ID(CID) of the Samsung Apps contents
 * @return	This function can return any of the following codes
 * DRM_SAPPS_SUCCESS = 0		 <success 
 * DRM_SAPPS_LIC_VALID = 1 	 <License is valid 
 * DRM_SAPPS_LIC_SUSPEND = 2	 <License is valid but for future use 
 * DRM_SAPPS_INVALID_ARG = -1000	<invalid argument
 * DRM_SAPPS_PARSING_ERROR = -999		<parsing contents or rights error
 * DRM_SAPPS_DB_ERROR = -998		 <data base error 
 * DRM_SAPPS_INSUFFICIENT_MEMORY = -997	  <memory error 
 * DRM_SAPPS_INTERNAL_ERROR = -996 	<processing error
 * DRM_SAPPS_FILE_ERROR = -995 	  <file error
 * DRM_SAPPS_LIC_EXPIRED_DATETIME = -994	 <Datetime License is expired
 * DRM_SAPPS_LIC_EXPIRED_COUNT = -993	  <Count License is expired 
 * DRM_SAPPS_LIC_NO_LICENCE = -992 	 <No License
 * DRM_SAPPS_LIC_IMEI_MISMATCH = -991	  <IMEI is mismatching 
 * DRM_SAPPS_LIC_PNUM_MISMATCH = -990	  <PNUM is mismatching 
 * DRM_SAPPS_UNKNOWN_ERROR = -1	<dummy error value 
 * @remarks 
 * @see			drm_sdc_get_file_header
 * @since         
 */
 int drm_oem_sapps_has_valid_license ( const char *szCid );

/**   
 *  An application can update license of Samsung Apps contents.
 * 
 * @param[in]  szCid  Content ID(CID) of the Samsung Apps contents
 * @return  This function can return any of the following codes
 * DRM_SAPPS_SUCCESS = 0        <success 
 * DRM_SAPPS_LIC_VALID = 1      <License is valid 
 * DRM_SAPPS_LIC_SUSPEND = 2    <License is valid but for future use 
 * DRM_SAPPS_INVALID_ARG = -1000   <invalid argument
 * DRM_SAPPS_PARSING_ERROR = -999      <parsing contents or rights error
 * DRM_SAPPS_DB_ERROR = -998        <data base error 
 * DRM_SAPPS_INSUFFICIENT_MEMORY = -997      <memory error 
 * DRM_SAPPS_INTERNAL_ERROR = -996     <processing error
 * DRM_SAPPS_FILE_ERROR = -995       <file error
 * DRM_SAPPS_LIC_EXPIRED_DATETIME = -994    <Datetime License is expired
 * DRM_SAPPS_LIC_EXPIRED_COUNT = -993    <Count License is expired 
 * DRM_SAPPS_LIC_NO_LICENCE = -992      <No License
 * DRM_SAPPS_LIC_IMEI_MISMATCH = -991    <IMEI is mismatching 
 * DRM_SAPPS_LIC_PNUM_MISMATCH = -990    <PNUM is mismatching 
 * DRM_SAPPS_UNKNOWN_ERROR = -1    <dummy error value 
 * @remarks  This API must be called before rendering Samsung Apps contents
 * @see   
 * @since         
 */
 int drm_oem_sapps_consume_license( const char *szCid );

/**   
 *  An application can get license information for Samsung Apps content with given CID.
 * 
 * @param[in]  	szCid  					Content ID(CID) of the Samsung Apps contents
 * @param[out] 	LicenseInfoList 			License information list
 * @return  This function can return any of the following codes
 * DRM_SAPPS_SUCCESS = 0        <success 
 * DRM_SAPPS_LIC_VALID = 1      <License is valid 
 * DRM_SAPPS_LIC_SUSPEND = 2    <License is valid but for future use 
 * DRM_SAPPS_INVALID_ARG = -1000   <invalid argument
 * DRM_SAPPS_PARSING_ERROR = -999      <parsing contents or rights error
 * DRM_SAPPS_DB_ERROR = -998        <data base error 
 * DRM_SAPPS_INSUFFICIENT_MEMORY = -997      <memory error 
 * DRM_SAPPS_INTERNAL_ERROR = -996     <processing error
 * DRM_SAPPS_FILE_ERROR = -995       <file error
 * DRM_SAPPS_LIC_EXPIRED_DATETIME = -994    <Datetime License is expired
 * DRM_SAPPS_LIC_EXPIRED_COUNT = -993    <Count License is expired 
 * DRM_SAPPS_LIC_NO_LICENCE = -992      <No License
 * DRM_SAPPS_LIC_IMEI_MISMATCH = -991    <IMEI is mismatching 
 * DRM_SAPPS_LIC_PNUM_MISMATCH = -990    <PNUM is mismatching 
 * DRM_SAPPS_UNKNOWN_ERROR = -1    <dummy error value 
 * @remarks  
 * @see   
 * @since         
 */
 int drm_oem_sapps_get_license_info( const char *szCid ,drm_sapps_license_list **LicenseInfoList );

/**   
 *  An application frees license information list for Samsung Apps contents.
 * 
 * @param[in]  	pLicenseInfoList  					License information list pointer
 * @return  
 * @remarks  This should be called after using LicenseInfoList recevied in drm_oem_sapps_get_license_info.
 * @remarks  Memory is allocated for LicenseInfoList in drm_oem_sapps_get_license_info that need to be freed by calling drm_oem_sapps_free_license_info
 * @see   drm_oem_sapps_get_license_info
 * @since         
 */
 void drm_oem_sapps_free_license_info(drm_sapps_license_list **pLicenseInfoList);
/**
*@}
*/ /* Doxygen : addtogroup*/

#ifdef __cplusplus
}
#endif

#endif

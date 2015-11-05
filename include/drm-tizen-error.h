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

#ifndef __DRM_TIZEN_ERROR_H__
#define __DRM_TIZEN_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif


#define		TADC_SUCCESS					1				// SUCCESS
// TADC Error Message Define
#define		TADC_GETDUID_ERROR				0x0611			// Get IMEI Error
#define		TADC_GETMCC_ERROR				0x0612			// Get MCC Error
#define		TADC_GETMNC_ERROR				0x0613			// Get MNC Error

#define		TADC_REQMAKEHMAC_ERROR			0x0620			// Make Signature Fail in Reauest Message
#define		TADC_RESPONSEMESSAGE_ERROR		0x0621			// Response Message structure Error
#define		TADC_RESPONSESIGNATURE_ERROR	0x0622			// Response Message Signature Error

#define		TADC_GETDHKEY_ERROR				0x0630			// Get DH Key Error
#define		TADC_GETRESPONSLICENSE_ERROR	0x0631			// Get License information Error in Response Message
#define		TADC_LICENSEXMLPARSING_ERROR	0x0632			// License XML parsing Error in Response Message

#define		TADC_NOTTADCFILE_ERROR			0x0640			// This is not TADC Contents
#define		TADC_CONTENTSVERSION_ERROR		0x0641			// TADC Contents Version Error
#define		TADC_CONTENTSHMAC_ERROR			0x0642			// Contents Signature Error
#define		TADC_CONTENTSXML_ERROR			0x0643			// Contents XML Error in Header
#define		TADC_CONTENTSSTRUCT_ERROR		0X0644			// Contents Header Struct Error

#define		TADC_RULE_DUID_ERROR			0x0651			// IMEI Rule check Error
#define		TADC_RULE_MIN_ERROR				0x0652			// MIN Rule check Error
#define		TADC_RULE_NOINFOMATION			0X0653			// NO Information in DB

#define		TADC_PARAMETER_ERROR			0X0660			// Input Parameter Error
#define		TADC_XMLPARSER_ERROR			0X0661			// XML Parser Error
#define		TADC_MEMAlOC_ERROR				0x0662			// Memory Alocation Error

#define		TADC_CID_NULL_ERROR				0X0670			// CID Null Error
#define		TADC_SID_NULL_ERROR				0X0671			// CID Null Error

//2011.03.08
#define		TADC_RO_SIGNATURE_ERROR			0X0680			// RO Signature Fail
#define		TADC_RO_CERTIFICATE_ERROR		0X0681			// RO Certificate Fail

//2013.03.11
#define		TADC_DHINFO_MAX_ERROR			0x0690			// DHINFO Max error
#define		TADC_GET_ROACQ_INFO_ERROR		0x0691			// TADC_GetROAcqInfo Fail
#define		TADC_MAKE_LICENSEREQ_ERROR		0x0692			// SACD_MakeRequestRO Fail
#define		TADC_GET_HASHREQID_ERROR		0x0693			// TADC_GetHashReqID Fail
#define		TADC_GET_RORES_INFO_ERROR		0x0694			// TADC_GetROAcqInfo Fail

#define		TADC_PARSE_TIMECONSTRAINTS_ERROR	0x0695			// DTappsDtTmStr2StrucTm Fail
#define		TADC_LICENSE_INVALID_ERROR		0x0696			// DSaapsValidateConstraints Fail

#define		TADC_GET_DEVICEKEY_ERROR		0x0697			// DTappsGetDeviceKey Fail
#define		TADC_GET_BASE64ENCODE_ERROR		0x0698			// DTappsB64Encode Fail
#define		TADC_SET_AES_ENCRYPTKEY_ERROR	0x0699			// DTAPPS_AES_SET_ENCR_KEY Fail
#define		TADC_SET_AES_WRAPKEY_ERROR		0x0700			// DTAPPS_AES_WRAP_KEY Fail
#define		TADC_DB_INSTALL_ERROR			0x0702			// DTapps_DB_Install Fail
#define		TADC_DB_READ_ERROR				0x0703			// DTapps_DB_Install Fail

#define		TADC_FILE_OPEN_ERROR			0x0704			//
#define		TADC_FILE_READ_ERROR			0x0705			//
#define		TADC_GET_FILEHEADER_ERROR		0x0706			// DrmTdcGetFileHeader Fail
#define		TADC_GET_CEK_ERROR				0x0707			// DSaapsGetCek Fail
#define		TADC_GET_FILE_HANDLER_ERROR		0x0708
#define		TADC_DECRYPT_PACKAGE_ERROR		0x0709			// DrmTdcDecryptPackage2 Fail

#define		TADC_LICENSE_VALID				1
#define		TADC_LICENSE_DB_ERROR			0x802
#define		TADC_LICENSE_NO_LICENSE			0x805
#define		TADC_LICENSE_DUID_MISMATCH		0x806
#define		TADC_LICENSE_UNKNOWN_ERROR		0x807

#ifdef __cplusplus
}
#endif

#endif // __DRM_TIZEN_ERROR_H__


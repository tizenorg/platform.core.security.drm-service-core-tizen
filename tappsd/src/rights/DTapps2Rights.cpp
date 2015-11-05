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
 * @file	DTapps2Rights.cpp
 * @brief	This file includes functions relating to Rights.
 */

#include <string>

#include "TADC_Core.h"
#include "TADC_IF.h"
#include "drm-tizen-error.h"

#include "TADC_Util.h"

#include "DTapps2Rights.h"
#include "DTapps2SqliteDB.h"
#include "DTapps2HMAC.h"
#include "DTapps2Base64.h"
#include "DTapps2Time.h"

/* Validation and Installation of the Constraints Structure */
int DTappsValidateConstraints(DTAPPS_CONSTRAINTS* st_const, DTAPPS_OPERATION opr)
{
	int ret = TADC_LICENSE_VALID;
	char DUID[DTAPPS_DUID_SIZE] = {0, };
	unsigned char zeroduid[DTAPPS_DUID_SIZE] = {0, };
	int retDeviceInfo = 0;

	if (st_const == NULL)
	{
		DRM_TAPPS_EXCEPTION("Invalid Paramter st_const = %p", st_const);
		ret = TADC_PARAMETER_ERROR;
		goto Error_Exit;
	}

	/* Check for DUID */
	if (DTAPPS_MEMCMP(st_const->DUID, zeroduid, DTAPPS_DUID_SIZE))
	{
		DRM_TAPPS_LOG("st_const->DUID = %s", st_const->DUID);
		if(opr == DTAPPS_VERIFY || opr == DTAPPS_INSTALL)
		{
			retDeviceInfo = TADC_IF_GetDUID(DUID);
			if (retDeviceInfo < 0)
			{
				DRM_TAPPS_EXCEPTION("Retrieve for DUID Failed!!! DUID = %s, duid_len = %d", DUID, strlen(DUID));
				return TADC_LICENSE_UNKNOWN_ERROR;
			}
			if (!(DTAPPS_MEMCMP(st_const->DUID, DUID, DTAPPS_DUID_SIZE)))
			{
				DRM_TAPPS_LOG("DUID match!! st_const->DUID = %s, DUID = %s", st_const->DUID, DUID);
				ret = TADC_LICENSE_VALID;
			}
			else
			{
				DRM_TAPPS_EXCEPTION("DUID don't match!!! st_const->DUID = %s, DUID = %s", st_const->DUID, DUID);
				ret = TADC_LICENSE_DUID_MISMATCH;
				goto Error_Exit;
			}
		}
	}
	DRM_TAPPS_LOG("ret = %d", ret);
	return ret;

Error_Exit:
	DRM_TAPPS_EXCEPTION("Validate Constraints failed!!!, ret = %d", ret);
	return ret;
}

int DTappsInstallLicense(const char* declicbuffer)
{
	unsigned char* db_buffer_org = NULL;
	unsigned char db_buffer_enc[512] = {0, };
	int len_enc = 0, check_ret = 0;
	unsigned char *pDevKey = NULL;
	unsigned int DevKeyLen = 0;
	unsigned char hash[32] = {0, };
	unsigned char hash_enc[64] = {0, };
	unsigned int hash_len = 0;
	int check = TADC_LICENSE_VALID;

	BOOL isOk = FALSE;
	int ret = TADC_SUCCESS;

	unsigned char name[DTAPPS_NAME_SIZE] = {0, };
	unsigned char nullCek[CEK_SIZE] = {0, };

	char sql_query[2048] = {0, };
	int len = 0;
	time_t now = 0;
	char time_buffer[21] = {0, };
	T_RO t_RO = {0, };

	DTAPPS_RIGHTS_ROW row;

#ifdef DTAPPS_STORE_CEK_IN_DB
	AES_KEY key = {{0}};
	unsigned char *encr_cek = NULL;
	unsigned int keylen = 0;
	unsigned int encr_cek_len = 0;
	unsigned char b64_encr_cek[128] = {0, };
	unsigned char encr_cek_hash[32] = {0, };
	unsigned char b64_encr_cek_hash[64] = {0, };
	unsigned int encr_cek_hash_len = 0;
#endif

	// Copy necessary fields into structure to be stored in DB
	DTAPPS_CONSTRAINTS st_constraints = {0, };

	// First Parse the decrypted Licence buffer in order to fill the t_RO buffer and use it to store in DB
	check_ret = DTappsGetROInfo(declicbuffer, &t_RO, name);
	if (check_ret < 0)
	{
		DRM_TAPPS_EXCEPTION("DTappsGetROInfo Error Code = %x, check_ret = %d", TADC_LICENSEXMLPARSING_ERROR,check_ret);
		ret = TADC_LICENSEXMLPARSING_ERROR;
		goto Error_Exit;
	}

	/* Copy DUID */
	if (t_RO.t_Permission.t_Individual.DUID != NULL)
	{
		DTAPPS_MEMCPY(st_constraints.DUID, t_RO.t_Permission.t_Individual.DUID, TAPPS_STRLEN((const char*)t_RO.t_Permission.t_Individual.DUID));
	}
	DRM_TAPPS_LOG("st_constraints.DUID = %s", st_constraints.DUID);

	/* Validation of constraints */
	check = DTappsValidateConstraints(&st_constraints, DTAPPS_INSTALL);
	if (check != TADC_LICENSE_VALID)
	{
		DRM_TAPPS_LOG("Constraints obtained are NOT VALID!!!, check = %d",check);
		ret = TADC_LICENSE_INVALID_ERROR;
		goto Error_Exit;
	}

	/* Create the contraints buffer to be put in DB */
	db_buffer_org = (unsigned char*)DTAPPS_MALLOC(sizeof(DTAPPS_CONSTRAINTS));
	if (db_buffer_org == NULL)
	{
		DRM_TAPPS_EXCEPTION("Memory Allocation error , db_buffer_org = %p", db_buffer_org);
		ret = TADC_MEMAlOC_ERROR;
		goto Error_Exit;
	}

	DTAPPS_MEMSET(db_buffer_org, 0x00, sizeof(DTAPPS_CONSTRAINTS));
	DTAPPS_MEMCPY(db_buffer_org, &st_constraints, sizeof(DTAPPS_CONSTRAINTS));

	/* Calculate the hash - HMAC for the constraints buffer */
	if (0 == DTappsGetDeviceKey(&pDevKey, &DevKeyLen))
	{
		DRM_TAPPS_EXCEPTION("Error in DTAppsGetDeviceKey!!!, pDevKey = %p, DevKeyLen = %d", pDevKey, DevKeyLen);
		ret = TADC_GET_DEVICEKEY_ERROR;
		goto Error_Exit;
	}

	DTappsCalHMACSHA1(pDevKey, (int)DevKeyLen, db_buffer_org, sizeof(DTAPPS_CONSTRAINTS), hash, &hash_len);

	/* Encode the buffer and hash using Base64 and then store it into the DB */
	/* Base64 Encode the constraints buffer */
	len_enc = (sizeof(DTAPPS_CONSTRAINTS) + 2) / 3 * 4;

	isOk = DTappsB64Encode(db_buffer_org, sizeof(DTAPPS_CONSTRAINTS), db_buffer_enc, len_enc);
	if (isOk != TRUE)
	{
		DRM_TAPPS_EXCEPTION("BASE64 Encoding failure!!!! ret = %d", isOk);
		ret = TADC_GET_BASE64ENCODE_ERROR;
		goto Error_Exit;
	}

	/* Base64 Encode the hash buffer */
	len_enc = (hash_len + 2) / 3 * 4;

	isOk = DTappsB64Encode(hash, hash_len, hash_enc, len_enc);
	if (isOk != TRUE)
	{
		DRM_TAPPS_EXCEPTION("BASE64 Encoding failure!!!! ret = %d", isOk);
		ret = TADC_GET_BASE64ENCODE_ERROR;
		goto Error_Exit;
	}

	DRM_TAPPS_LOG("CEK installation started!!");
	if (DTAPPS_MEMCMP((void*)t_RO.t_Content.CEK, (void*)nullCek, CEK_SIZE) != 0)
	{
#ifdef DTAPPS_STORE_CEK_IN_DB
		/* Store the CEK in DB */
		/* First encrypt the CEK , calculate the HASH and then convert both into BASE64 Encoded form */
		//ceklen = (unsigned int)TAPPS_STRLEN((const char*) t_RO.t_Content.CEK);
		/* keylen => Should be a multiple of 8 */
		keylen = CEK_SIZE;
		encr_cek_len = keylen + 8;

		if ((check_ret = DTAPPS_AES_SET_ENCR_KEY(pDevKey, 128, &key)) != 0)
		{
			DRM_TAPPS_EXCEPTION("DTAPPS_AES_SET_ENC_KEY failed!!, check_ret = %d", check_ret);
			ret =  TADC_SET_AES_ENCRYPTKEY_ERROR;
			goto Error_Exit;
		}

		encr_cek = (unsigned char*)DTAPPS_MALLOC(encr_cek_len);
		if (NULL == encr_cek)
		{
			DRM_TAPPS_EXCEPTION("Memory Allocation Error!!, encr_cek = %p", encr_cek);
			ret = TADC_MEMAlOC_ERROR;
			goto Error_Exit;
		}

		check_ret = DTAPPS_AES_WRAP_KEY(&key, NULL, encr_cek, t_RO.t_Content.CEK, keylen);
		if (check_ret <= 0)
		{
			DRM_TAPPS_EXCEPTION("DTAPPS_AES_WRAP_KEY failed!!, check_ret = %d", check_ret);
			ret = TADC_SET_AES_WRAPKEY_ERROR;
			goto Error_Exit;
		}
		DRM_TAPPS_LOG("encr_cek = %s", encr_cek);

		/* Calculate the HASH for Encrypted CEK */
		DTappsCalHMACSHA1(pDevKey, (int)DevKeyLen, encr_cek, encr_cek_len, encr_cek_hash, &encr_cek_hash_len);
		DRM_TAPPS_LOG("encr_cek_hash = %s", encr_cek_hash);

		/* Base64 Encode the Encrypted CEK */
		len_enc = (encr_cek_len + 2) / 3 * 4;
		isOk = DTappsB64Encode(encr_cek, encr_cek_len, b64_encr_cek, len_enc);
		if (isOk != TRUE)
		{
			DRM_TAPPS_EXCEPTION("BASE64 Encoding failure!!!! ret = %d", isOk);
			ret = TADC_GET_BASE64ENCODE_ERROR;
			goto Error_Exit;
		}

		/* Base64 Encode the Hash of Encrypted CEK */
		len_enc = (encr_cek_hash_len + 2) / 3 * 4;
		isOk = DTappsB64Encode(encr_cek_hash, encr_cek_hash_len, b64_encr_cek_hash, len_enc);
		if (isOk != TRUE)
		{
			DRM_TAPPS_EXCEPTION("BASE64 Encoding failure!!!! ret = %d", isOk);
			ret = TADC_GET_BASE64ENCODE_ERROR;
			goto Error_Exit;
		}

		DRM_TAPPS_LOG("b64_encr_cek = %s, b64_encr_cek_hash = %s", b64_encr_cek, b64_encr_cek_hash);
#else
		DRM_TAPPS_LOG("CEK in Secure Storage to be implemented!!!");
#endif
	}
	else
	{
		DRM_TAPPS_EXCEPTION("t_RO.t_Content.CEK is null!!");
	}

	/* Calculate the Time of storage */
	now = DTAPPS_TIME(NULL);
	struct tm time_gmt;
    DTAPPS_GMTIME_THREAD_SAFE(&now, &time_gmt);

	/* Format the time to be stored in the DB buffer - DATETIME format */
	len = DTAPPS_SNPRINTF(
			time_buffer,
			sizeof(time_buffer),
			"%04d-%02d-%02dT%02d:%02d:%02dZ",
				time_gmt.tm_year + 1900,
				time_gmt.tm_mon + 1,
				time_gmt.tm_mday,
				time_gmt.tm_hour,
				time_gmt.tm_min,
				time_gmt.tm_sec);

	if ((len == -1) || (len >= (int)sizeof(time_buffer)))
	{
		DRM_TAPPS_EXCEPTION("snprintf failed , len = %d, sizeof(time_buffer) = %d",len,sizeof(time_buffer));
		ret = TADC_MEMAlOC_ERROR;
		goto Error_Exit;
	}
	DRM_TAPPS_LOG("time_buffer = %s", time_buffer);
	DRM_TAPPS_LOG("Constraint_buffer = %s, Constraint_Hash = %s", db_buffer_enc, hash_enc);

	// 2013/04/25 added for db update
	isOk = DTapps_RIGHTS_SELECT_ONE((char*) t_RO.t_Content.CID, &row);

	// if there is no rights in the db, insert into rights to db
	if (isOk != TRUE)
	{
		DRM_TAPPS_LOG("Insert into dtapps2rights started....");
        isOk = DTapps_RIGHTS_INSTALL((char*) name, time_buffer,
									#ifdef DTAPPS_STORE_CEK_IN_DB
									(char*) b64_encr_cek, (char*) b64_encr_cek_hash,
									#endif
									(char*) db_buffer_enc, (char*) hash_enc, (char*) t_RO.t_Content.CID,
									FALSE);
	    if (isOk != TRUE)
		{
			DRM_TAPPS_EXCEPTION("DB Installation failure!!!! ret = %d", isOk);
			ret = TADC_DB_INSTALL_ERROR;
			goto Error_Exit;
		}
		DRM_TAPPS_LOG("DrmTdcInstallLicense Success!!!!!! ret = %d", ret);
	}
	// if there is same cid rights exist, update right to db
	else
	{
		DRM_TAPPS_LOG("Update dtapps2rights started....");
		isOk = DTapps_RIGHTS_INSTALL((char*) name, time_buffer,
									#ifdef DTAPPS_STORE_CEK_IN_DB
									(char*) b64_encr_cek, (char*) b64_encr_cek_hash,
									#endif
									(char*) db_buffer_enc, (char*) hash_enc, (char*) t_RO.t_Content.CID,
									TRUE);
	    if (isOk != TRUE)
		{
			DRM_TAPPS_EXCEPTION("DB Update failure!!!! ret = %d", isOk);
			ret = TADC_DB_INSTALL_ERROR;
			goto Error_Exit;
		}
		DRM_TAPPS_LOG("DrmTdcInstallLicense Success!!!!!! ret = %d", ret);
	}

	// Test Code for get the plaintext ro
	/*{
	    std::string tmpPath((char *)(t_RO.t_Content.CID));
	    tmpPath += ".ro";

		FILE *fd = fopen(tmpPath.c_str(), "w+b");
		if (fd == NULL)
		{
			goto Error_Exit;
		}
		fwrite(declicbuffer, 1, strlen(declicbuffer), fd);
		fclose(fd);
		DRM_TAPPS_LOG("Save decrypted RO success!, path = %s", tmpPath.c_str());
	}*/
	// Test Code Done!

Error_Exit:
	TADC_MEMFree_RO(&t_RO);

	if (db_buffer_org)
	{
		DTAPPS_FREE((unsigned char*)db_buffer_org);
	}
	if (encr_cek)
	{
		DTAPPS_FREE(encr_cek);
	}

	return ret;
}

int DTappsHasValidLicense(const char* szCid)
{
	int len = 0, const_buf_enclen = 0, hash_buf_enclen = 0, const_buf_declen = 0, hash_buf_declen = 0;

	unsigned char *pDevKey = NULL;
	unsigned int DevKeyLen = 0;
	unsigned char const_buffer_dec[512] = {0, };
	unsigned char hash_buffer_dec[64] = {0, };
	unsigned char md[64] = {0, };
	unsigned int md_len = 0;

	DTAPPS_RIGHTS_ROW row;
	bool check = FALSE;

	int check_valid = TADC_LICENSE_VALID;
	DTAPPS_CONSTRAINTS st_constaints = {0, };

	check = DTapps_RIGHTS_SELECT_ONE(szCid, &row);
	if (check != TRUE)
	{
		DRM_TAPPS_EXCEPTION("There is no RO in the DB. cid  = %s", szCid);
		check_valid = TADC_LICENSE_NO_LICENSE;
		goto DTAPPS_END;
	}

	if (0 == DTappsGetDeviceKey(&pDevKey, &DevKeyLen))
	{
		DRM_TAPPS_EXCEPTION("Error in DTAppsGetDeviceKey pDevKey = %s, DevKeyLen = %d", pDevKey, DevKeyLen);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_END;
	}

	/* BASE64 Decode the constraints buffer extracted from the DB */
	const_buf_enclen = TAPPS_STRLEN(row.constraint_buffer);
	const_buf_declen = const_buf_enclen / 4 * 3;
	if (const_buf_enclen <= 0)
	{
		DRM_TAPPS_EXCEPTION("There is no constaints buffer in the DB. cid = %s", szCid);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_END;
	}
	DRM_TAPPS_FRQ_LOG("const_buf_enclen = %d, const_buf_declen = %d", const_buf_enclen, const_buf_declen);

	(void)DTappsB64Decode((unsigned char*)row.constraint_buffer, const_buf_enclen, const_buffer_dec, const_buf_declen);
	if (const_buf_declen > (const_buf_enclen / 4 * 3))
	{
		DRM_TAPPS_EXCEPTION("BASE64 Decode error !!! const_buffer_dec = %s, const_buf_declen = %d, (const_buf_enclen/4*3) = %d", const_buffer_dec, const_buf_declen, (const_buf_enclen / 4 * 3));
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_END;
	}

	/* BASE64 Decode the hash buffer extracted from the DB */
	DTAPPS_MEMSET(hash_buffer_dec, 0x00, sizeof(hash_buffer_dec));
	DTAPPS_MEMSET(md, 0x00, sizeof(md));

	hash_buf_enclen = TAPPS_STRLEN(row.constraint_hash);
	hash_buf_declen = hash_buf_enclen / 4 * 3;
	if (hash_buf_enclen <= 0)
	{
		DRM_TAPPS_EXCEPTION("There is no constaints_hash buffer in the DB. cid = %s", szCid);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_END;
	}
	DRM_TAPPS_FRQ_LOG("hash_buf_enclen = %d, hash_buf_declen = %d", hash_buf_enclen, hash_buf_declen);

	(void)DTappsB64Decode((unsigned char*)row.constraint_hash, hash_buf_enclen, hash_buffer_dec, hash_buf_declen);
	if (hash_buf_declen > (hash_buf_enclen / 4 * 3))
	{
		DRM_TAPPS_EXCEPTION("BASE64 Decode error !!! hash_buffer_dec = %s, hash_buf_declen = %d, (hash_buf_enclen/4*3) = %d", hash_buffer_dec, hash_buf_declen, (hash_buf_enclen / 4 * 3));
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_END;
	}

	/* Calculate the hash using HMAC from the BASE64Decoded Constraint Buffers and check with the BASE64Decoded Hash buffers */
	DTappsCalHMACSHA1(pDevKey, (int)DevKeyLen, const_buffer_dec, const_buf_declen, md, &md_len);
	if (DTAPPS_MEMCMP(md, hash_buffer_dec, hash_buf_declen) == 0)
	{
		DRM_TAPPS_LOG("HASH matching!!! md = %s, hash_buffer_dec = %s, hash_buf_declen = %d", md, hash_buffer_dec, hash_buf_declen);
	}
	else
	{
		DRM_TAPPS_EXCEPTION("HASH not matching!!! md = %s, hash_buffer_dec = %s, hash_buf_declen = %d", md, hash_buffer_dec, hash_buf_declen);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_END;
	}

	/* Validate the constraints buffer */
	DTAPPS_MEMCPY(&st_constaints, const_buffer_dec, sizeof(DTAPPS_CONSTRAINTS));

	check_valid = DTappsValidateConstraints(&st_constaints, DTAPPS_VERIFY);
	if (check_valid != TADC_LICENSE_VALID)
	{
		DRM_TAPPS_LOG("Invalid Constraints for constraints_buffer = %s check_valid = %d", const_buffer_dec, check_valid);
		/* TODO: Implementation of Auto Deletion feature to delete the rights from the DB if the rights are expired!!!*/
		goto DTAPPS_END;
	}
	else
	{
		DRM_TAPPS_LOG("Rights are Valid!!! Found atleast one Valid Rights..... return with Success!! check_valid = %d", check_valid);
	}

DTAPPS_END:
	if (pDevKey)
	{
		DTappsFreeDeviceKey(&pDevKey);
	}
	if (check_valid != TADC_LICENSE_VALID)
	{
		DRM_TAPPS_EXCEPTION("Constraints not Valid!!!! check_valid = %d", check_valid);
		DRM_TAPPS_EXCEPTION("DrmTdcHasValidLicense Failed!!!");
		return check_valid;
	}
	else
	{
		DRM_TAPPS_LOG("Constraints are Valid!! check_valid = %d", check_valid);
		return TADC_LICENSE_VALID;
	}
}

int DTappsGetROInfo(const char* pszXML, T_RO *t_RO, unsigned char* name)
{
	int				nResult = 0;
	CXMLFile		oXMLFile;
	CXMLElement*	pRoot, *pElement;
	CPointerArray	paChilds;
	LPCTSTR			pszValue;
	int				length = 0;
	TADC_U8			TempVersion[3] = {0};
	unsigned char	*pbBuffer = NULL;
	int len_enc = 0, len_dec = 0;

	const char *pRoHeader = "<?xml version=\"1.0\"?>\n<TizenLicense>";

	//Check Param Buffer
	if (pszXML == NULL || t_RO == NULL || name == NULL)
	{
		DRM_TAPPS_EXCEPTION("Parameters NULL!!! pszXML = %p, t_RO = %p", pszXML, t_RO);
		return -1;
	}

	if (memcmp(pszXML, pRoHeader, strlen(pRoHeader))) {
		DRM_TAPPS_EXCEPTION("TADC_IF_MemCmp. pRoHeader[%s] Error Code = %x", pRoHeader, TADC_XMLPARSER_ERROR);
		return -1;
	}

	DTAPPS_MEMSET(t_RO, 0, sizeof(T_RO));

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

	// Get UID -> Name
	pRoot = oXMLFile.GetRoot();
	if (pRoot == NULL)
	{
		DRM_TAPPS_EXCEPTION("oXMLFile.GetRoot is failed\n");
		nResult = -1;
		goto finish;
	}

	paChilds.RemoveAll();
	nResult = pRoot->Find(&paChilds, _T("LicenseInfo"), _T("uid"), NULL);
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

	pElement = (CXMLElement*) paChilds.Get(0);
	pszValue = pElement->GetValue();
	if (pszValue == NULL)
	{
		DRM_TAPPS_EXCEPTION("pElement->GetValue() is failed\n");
		nResult = -2;
		goto finish;
	}

	length = TAPPS_STRLEN(pszValue);
	if (length <= 0 || length > DTAPPS_NAME_SIZE)
	{
		DRM_TAPPS_EXCEPTION("TAPPS_STRLEN failed(%d) -  %s\n", length, pszValue);
		nResult = -1;
		goto finish;
	}

	memcpy(name, pszValue, length + 1);
	DRM_TAPPS_LOG("name = %s", name);

	// Get UID -> CID
	pRoot = oXMLFile.GetRoot();
	if (pRoot == NULL)
	{
		DRM_TAPPS_EXCEPTION("oXMLFile.GetRoot is failed\n");
		nResult = -1;
		goto finish;
	}

	paChilds.RemoveAll();
	nResult = pRoot->Find(&paChilds, _T("ContentInfo"), _T("cid"), NULL);
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
		nResult = -2;
		goto finish;
	}

	length = TAPPS_STRLEN(pszValue);
	if (length <= 0 || length > CID_SIZE)
	{
		DRM_TAPPS_EXCEPTION("TAPPS_STRLEN is failed(%d) -  %s\n", length, pszValue);
		nResult = -1;
		goto finish;
	}

	t_RO->t_Content.CID = (TADC_U8*)DTAPPS_MALLOC(CID_SIZE + 1);
	IF_TRUE_GOTO(t_RO->t_Content.CID == NULL, -2);

	memset(t_RO->t_Content.CID, 0, CID_SIZE + 1);
	memcpy(t_RO->t_Content.CID, pszValue, CID_SIZE + 1);

	// Get KeyValue (CEK)
	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO(pRoot == NULL, ERROR_INVALID_DATA);
	paChilds.RemoveAll();
	nResult = pRoot->Find(&paChilds, _T("KeyInfo"), _T("CipherValue"), NULL);

	IF_TRUE_GOTO( nResult != 0, ERROR_INVALID_DATA );
	IF_TRUE_GOTO( paChilds.GetCount() != 1, ERROR_INVALID_DATA );

	pElement = (CXMLElement*)paChilds.Get(0);
	pszValue = pElement->GetValue();
	IF_TRUE_GOTO(pszValue == NULL, -2);

	len_enc = TAPPS_STRLEN(pszValue);
	len_dec = len_enc / 4 * 3;

	DRM_TAPPS_LOG("CEK raw = [%s], len_enc = [%d], len_dec = [%d]", pszValue, len_enc, len_dec);

#if 0
	pbBuffer = (unsigned char*)DTAPPS_MALLOC(len_dec + 1);
	if (pbBuffer == NULL)
	{
		DRM_TAPPS_EXCEPTION("Memory Allocation Error pbBuffer = %p", pbBuffer);
		nResult = -2;
		goto finish;
	}

	DTAPPS_MEMSET(pbBuffer, 0x00, len_dec + 1);
#endif

	pbBuffer = Base64Decode(pszValue, &len_dec);
	if (pbBuffer == NULL)
	{
		DRM_TAPPS_EXCEPTION("Memory Allocation Error pbBuffer = %p", pbBuffer);
		nResult = -2;
		goto finish;
	}

	if (len_dec > (len_enc / 4 * 3))
	{
		DRM_TAPPS_EXCEPTION("BASE64 Decode error !!! pbBuffer=%s, len_dec=%d, (len_enc/4*3)=%d", pbBuffer, len_dec, (len_enc / 4 * 3));
		nResult = -1;
		goto finish;
	}
	if (len_dec != CEK_SIZE)
	{
		DRM_TAPPS_EXCEPTION("BASE64 Decode error !!! CEK size=%d, len_dec=%d", CEK_SIZE, len_dec);
		nResult = -1;
		goto finish;
	}

	DRM_TAPPS_LOG("Base64Decoded CEK pbBuffer=%s, len_dec=%d, (len_enc/4*3)=%d", pbBuffer, len_dec, (len_enc / 4 * 3));

	t_RO->t_Content.CEK = (TADC_U8*)DTAPPS_MALLOC(CEK_SIZE + 1);
	IF_TRUE_GOTO(t_RO->t_Content.CEK == NULL, -2);
	DTAPPS_MEMSET(t_RO->t_Content.CEK, 0x00, CEK_SIZE + 1);
	DTAPPS_MEMCPY(t_RO->t_Content.CEK, pbBuffer, CEK_SIZE);
	DRM_TAPPS_SECURE_LOG("CEK t_RO->t_Content.CEK = %s", t_RO->t_Content.CEK);

	// Get individual
	pRoot = oXMLFile.GetRoot();
	IF_TRUE_GOTO( pRoot == NULL, ERROR_INVALID_DATA );

	paChilds.RemoveAll();
	nResult = pRoot->Find(&paChilds, _T("DeviceInfo"), _T("DUID"), NULL);

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
	}

	TADC_IF_MemSet(t_RO->t_Permission.t_Individual.DUID, 0, length + 1);
	TADC_IF_MemCpy((CHAR*)t_RO->t_Permission.t_Individual.DUID, pszValue, length);

	nResult = 0;

finish:
	pszValue = NULL;

	if (pbBuffer)
	{
		DTAPPS_FREE(pbBuffer);
	}

	if (nResult < 0)
	{
		DRM_TAPPS_EXCEPTION(" Error Code = %x", TADC_XMLPARSER_ERROR);
	}

	if (nResult == -2)
	{
		DRM_TAPPS_EXCEPTION(" Error Code = %x", TADC_MEMAlOC_ERROR);
	}

	return nResult;
}

BOOL DTappsGetCEK(const char* szCid, T_RO* t_RO)
{
	char sql_query[2048] = {0, };
	int db_buf_enclen = 0, hash_buf_enclen = 0, db_buf_declen = 0, hash_buf_declen = 0;
	DTAPPS_RIGHTS_ROW row;
	BOOL check = FALSE;

	unsigned char const_buffer_dec[512] = {0, };
	unsigned char encr_cek[128] = {0, };

	unsigned char hash_buffer[64] = {0, };
	unsigned char md[64] = {0, };
	unsigned int md_len = 0;

	unsigned char *pDevKey = NULL;
	unsigned int DevKeyLen = 0;

	int check_valid = TADC_LICENSE_VALID;
	DTAPPS_CONSTRAINTS st_constaints = {0, };

	AES_KEY key = {{0}};

	if (NULL == szCid || NULL == t_RO)
	{
		DRM_TAPPS_EXCEPTION("Null Parameters!! szCid = %p, t_RO = %p", szCid, t_RO);
		goto DTAPPS_ERROR;
	}

	check = DTapps_RIGHTS_SELECT_ONE(szCid, &row);
	if (check != TRUE)
	{
		DRM_TAPPS_EXCEPTION("There is no RO in the DB. cid=%s", szCid);
		check_valid = TADC_LICENSE_NO_LICENSE;
		goto DTAPPS_ERROR;
	}

	if (0 == DTappsGetDeviceKey(&pDevKey, &DevKeyLen))
	{
		DRM_TAPPS_EXCEPTION("error in DTappsGetDeviceKey pDevKey = %s, DevKeyLen = %d", pDevKey, DevKeyLen);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_ERROR;
	}
	DRM_TAPPS_LOG("DTappsGetDeviceKey pDevKey = %s, DevKeyLen = %d", pDevKey, DevKeyLen);

	check_valid = TADC_LICENSE_VALID;

	/* BASE64 Decode the constraints buffer extracted from the DB */
	db_buf_enclen = TAPPS_STRLEN(row.constraint_buffer);
	db_buf_declen = db_buf_enclen / 4 * 3;
	if (db_buf_enclen <= 0)
	{
		DRM_TAPPS_EXCEPTION("There is no constaints buffer in the DB. cid = %s", szCid);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_ERROR;
	}

	DRM_TAPPS_FRQ_LOG("db_buf_enclen = %d, db_buf_declen = %d", db_buf_enclen, db_buf_declen);

	(void)DTappsB64Decode((unsigned char*)row.constraint_buffer, db_buf_enclen, const_buffer_dec, db_buf_declen);
	if (db_buf_declen > (db_buf_enclen / 4 * 3))
	{
		DRM_TAPPS_EXCEPTION("BASE64 Decode error! const_buffer_dec=%s, db_buf_declen=%d, (db_buf_enclen/4*3)=%d", const_buffer_dec, db_buf_declen, (db_buf_enclen / 4 * 3));
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_ERROR;
	}

	/* BASE64 Decode the hash buffer extracted from the DB */
	DTAPPS_MEMSET(hash_buffer, 0x00, sizeof(hash_buffer));
	DTAPPS_MEMSET(md, 0x00, sizeof(md));

	hash_buf_enclen = TAPPS_STRLEN(row.constraint_hash);
	hash_buf_declen = hash_buf_enclen / 4 * 3;
	if (hash_buf_enclen <= 0)
	{
		DRM_TAPPS_EXCEPTION("There is no hash buffer in the DB. cid = %s", szCid);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_ERROR;
	}
	DRM_TAPPS_FRQ_LOG("hash_buf_enclen = %d, hash_buf_declen = %d", hash_buf_enclen, hash_buf_declen);

	(void)DTappsB64Decode((unsigned char*)row.constraint_hash, hash_buf_enclen, hash_buffer, hash_buf_declen);
	if (hash_buf_declen > (hash_buf_enclen / 4 * 3))
	{
		DRM_TAPPS_EXCEPTION("BASE64 Decode error! hash_buffer_dec=%s, hash_buf_declen=%d, (hash_buf_enclen/4*3)=%d", hash_buffer, hash_buf_declen, (hash_buf_enclen / 4 * 3));
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_ERROR;
	}

	/* Calculate the hash using HMAC from the BASE64Decoded Constraint Buffers and check with the BASE64Decoded Hash buffers */
	DTappsCalHMACSHA1(pDevKey, (int)DevKeyLen, const_buffer_dec, db_buf_declen, md, &md_len);
	if (DTAPPS_MEMCMP(md, hash_buffer, hash_buf_declen) == 0)
	{
		DRM_TAPPS_LOG("HASH for constraint buffer matching! md=%s, hash_buffer_dec=%s, hash_buf_declen=%d", md, hash_buffer, hash_buf_declen);
	}
	else
	{
		DRM_TAPPS_EXCEPTION("HASH for constraint buffer not matching! md=%s, hash_buffer_dec=%s, hash_buf_declen=%d", md, hash_buffer, hash_buf_declen);
		check_valid = TADC_LICENSE_UNKNOWN_ERROR;
		goto DTAPPS_ERROR;
	}

	/* Validate the constraints buffer */
	DTAPPS_MEMCPY(&st_constaints, const_buffer_dec, sizeof(DTAPPS_CONSTRAINTS));

	check_valid = DTappsValidateConstraints(&st_constaints, DTAPPS_VERIFY);
	if (check_valid != TADC_LICENSE_VALID)
	{
		DRM_TAPPS_LOG("Invalid Constraints for constraints_buffer=%s, check_valid=%d", const_buffer_dec, check_valid);
		/* TODO: Implementation of Auto Deletion feature to delete the rights from the DB if the rights are expired!!!*/
		goto DTAPPS_ERROR;
	}
	else
	{
		DRM_TAPPS_LOG("Rights are Valid! check_valid = %d", check_valid);

		/* Constraints valid - Extract CEK and fill the t_RO structure with CEK, DUID */
		/* BASE64 Decode the CEK buffer extracted from the DB */
		db_buf_enclen = TAPPS_STRLEN(row.cek);
		db_buf_declen = db_buf_enclen / 4 * 3;
		if (db_buf_enclen <= 0)
		{
			DRM_TAPPS_EXCEPTION("There is no CEK in the DB. cid = %s", szCid);
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}
		DRM_TAPPS_FRQ_LOG("db_buf_enclen = %d, db_buf_declen = %d", db_buf_enclen, db_buf_declen);

		int cek_declen = 0;

		(void)DTappsB64Decode((unsigned char*)row.cek, db_buf_enclen, encr_cek, cek_declen);
		db_buf_declen = cek_declen;

		if (cek_declen > (db_buf_enclen / 4 * 3))
		{
			DRM_TAPPS_EXCEPTION("BASE64 Decode error! encr_cek=%s, cek_declen=%d, (db_buf_enclen/4*3)=%d", encr_cek, cek_declen, (db_buf_enclen / 4 * 3));
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}
		DRM_TAPPS_LOG("BASE64 encr_cek=%s, cek_declen=%d, db_buf_declen=%d, (db_buf_enclen/4*3)=%d", encr_cek, cek_declen, db_buf_declen, (db_buf_enclen / 4 * 3));

		/* BASE64 Decode the hash buffer extracted from the DB */
		DTAPPS_MEMSET(hash_buffer, 0x00, sizeof(hash_buffer));
		DTAPPS_MEMSET(md, 0x00, sizeof(md));

		hash_buf_enclen = TAPPS_STRLEN(row.cek_hash);
		hash_buf_declen = hash_buf_enclen / 4 * 3;
		if (db_buf_enclen <= 0)
		{
			DRM_TAPPS_EXCEPTION("There is no CEK_hash buffer in the DB. cid = %s", szCid);
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}
		DRM_TAPPS_FRQ_LOG("hash_buf_enclen = %d, hash_buf_declen = %d", hash_buf_enclen, hash_buf_declen);

		(void)DTappsB64Decode((unsigned char*)row.cek_hash, hash_buf_enclen, hash_buffer, hash_buf_declen);
		if (hash_buf_declen > (hash_buf_enclen / 4 * 3))
		{
			DRM_TAPPS_EXCEPTION("BASE64 Decode error! hash_buffer_dec=%s, hash_buf_declen=%d, (hash_buf_enclen/4*3)=%d", hash_buffer, hash_buf_declen, (hash_buf_enclen/4*3));
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}

		/* Calculate the hash using HMAC from the BASE64Decoded Encrypted CEK Buffer and check with the BASE64Decoded Hash buffer */
		DTappsCalHMACSHA1(pDevKey, (int)DevKeyLen, encr_cek, db_buf_declen, md, &md_len);
		if (DTAPPS_MEMCMP(md, hash_buffer, hash_buf_declen) == 0)
		{
			DRM_TAPPS_LOG("HASH for Encrypted CEK buffer matching! md=%s, hash_buffer_dec=%s, hash_buf_declen=%d", md, hash_buffer, hash_buf_declen);
		}
		else
		{
			DRM_TAPPS_EXCEPTION("HASH for Encrypted CEK buffer not matching! md=%s, hash_buffer_dec=%s, hash_buf_declen=%d", md, hash_buffer, hash_buf_declen);
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}

		/* Allocate memory for CEK */
		t_RO->t_Content.CEK = (unsigned char*)DTAPPS_MALLOC(CEK_SIZE + 1);
		if (NULL == t_RO->t_Content.CEK)
		{
			DRM_TAPPS_EXCEPTION("Memory Allocation Error!! t_RO->t_Content.CEK = %p", t_RO->t_Content.CEK);
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}

		DTAPPS_MEMSET(t_RO->t_Content.CEK, 0x00, CEK_SIZE + 1);
		/* Decrypt CEK and update t_RO structure */
		if (DTAPPS_AES_SET_DECR_KEY(pDevKey, 128, &key))
		{
			DRM_TAPPS_EXCEPTION("AES_set_decrypt_key failed!!");
			check_valid = TADC_LICENSE_UNKNOWN_ERROR;
			goto DTAPPS_ERROR;
		}

		(void)DTAPPS_AES_UNWRAP_KEY(&key, NULL, t_RO->t_Content.CEK, encr_cek, cek_declen);
		DRM_TAPPS_LOG("DTAPPS_AES_UNWRAP_KEY success!!");

		/* Store DUID in t_RO */
		if (st_constaints.DUID[0] != '\0')
		{
			t_RO -> t_Permission.t_Individual.DUID = (unsigned char*)DTAPPS_MALLOC(DUID_SIZE + 1);
			if (NULL == t_RO -> t_Permission.t_Individual.DUID)
			{
				DRM_TAPPS_EXCEPTION("Memory Allocation Error!! t_RO->t_Permission.t_Individual.DUID = %p", t_RO -> t_Permission.t_Individual.DUID);
				check_valid = TADC_LICENSE_UNKNOWN_ERROR;
				goto DTAPPS_ERROR;
			}
			DTAPPS_MEMSET(t_RO -> t_Permission.t_Individual.DUID, 0x00, DUID_SIZE + 1);
			t_RO->PerFlag |= DUID_RULE;
			t_RO->t_Permission.t_Individual.BindingType |= DUID_RULE;
			DTAPPS_MEMCPY(t_RO -> t_Permission.t_Individual.DUID, st_constaints.DUID, DUID_SIZE);
		}
	}

DTAPPS_ERROR:
	if (pDevKey)
	{
		DTappsFreeDeviceKey(&pDevKey);
	}

	if (check_valid != TADC_LICENSE_VALID)
	{
		DRM_TAPPS_EXCEPTION("DTappsGetCEK FAILED!!!, check_valid = %d", check_valid);
		return FALSE;
	}
	else
	{
		/* All fine... Allocate memory for CID and fill the value */
		if ((t_RO != NULL) && (szCid != NULL))
		{
			t_RO -> t_Content.CID = (unsigned char*) DTAPPS_MALLOC(CID_SIZE + 1);
			if (NULL == t_RO -> t_Content.CID)
			{
				DRM_TAPPS_EXCEPTION("Memory Allocation Error!! t_RO->t_Content.CID = %p", t_RO -> t_Content.CID);
				check_valid = TADC_LICENSE_UNKNOWN_ERROR;
				goto DTAPPS_ERROR;
			}
			DTAPPS_MEMSET(t_RO -> t_Content.CID, 0x00, CID_SIZE + 1);
			DTAPPS_MEMCPY(t_RO -> t_Content.CID, szCid, CID_SIZE);
			DRM_TAPPS_LOG("DTappsGetCEK SUCCESS!!!, check_valid = %d", check_valid);
		}

		return TRUE;
	}
}

BOOL DTapps_RIGHTS_INSTALL(const char* name,
                          const char* time,
                          #ifdef DTAPPS_STORE_CEK_IN_DB
                          const char* cek,
                          const char* cek_hash,
                          #endif
                          const char* constraint_buffer,
                          const char* constraint_hash,
                          const char* cid,
                          BOOL isUpdate)
{
    void*         pDb       = NULL;
    const char*   query     = NULL;
    sqlite3_stmt* pstmt     = NULL;
    unsigned int  dIdx      = 0;
    BOOL          ret_value = FALSE;

    DRM_TAPPS_LOG("Open DB......");
    ret_value = DTappsDBOpen(pDb,__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("DB Open Failed!! ret_value = %d",ret_value);
        return FALSE;
    }

    if(isUpdate == FALSE)
    {
        query = "INSERT INTO dtapps2rights (name, time_t, "
                               #ifdef DTAPPS_STORE_CEK_IN_DB
                               " cek, cek_hash, "
                               #endif
                               " constraint_buffer, constraint_hash, cid) "
                               " VALUES (?, ?, "
                               #ifdef DTAPPS_STORE_CEK_IN_DB
                               " ?, ?, "
                               #endif
                               " ?, ?, ?);";
    }
    else
    {
        query = "UPDATE dtapps2rights SET name=?, time_t=?,"
                               #ifdef DTAPPS_STORE_CEK_IN_DB
                               " cek=?, cek_hash=?, "
                               #endif
                               " constraint_buffer=?, constraint_hash=? "
                               " WHERE r_id=?;";
    }

    DRM_TAPPS_LOG("Prepare Statement........");
    pstmt = (sqlite3_stmt*) DTappsStmtPrepare(pDb, query);
    if(pstmt == NULL)
    {
        DRM_TAPPS_EXCEPTION("Prepare Statement failed. query=%s", query);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Binding Parameters........");
    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)name, strlen(name));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(name=%s) failed. ret_value = %d", name, ret_value);
        goto Error_Exit;
    }

    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)time, strlen(time));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(time=%s) failed. ret_value = %d", time, ret_value);
        goto Error_Exit;
    }

    #ifdef DTAPPS_STORE_CEK_IN_DB
    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)cek, strlen(cek));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(cek=%s) failed. ret_value = %d", cek, ret_value);
        goto Error_Exit;
    }

    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)cek_hash, strlen(cek_hash));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(cek_hash=%s) failed. ret_value = %d", cek_hash, ret_value);
        goto Error_Exit;
    }
    #endif

    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)constraint_buffer, strlen(constraint_buffer));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(constraint_buffer=%s) failed. ret_value = %d", constraint_buffer, ret_value);
        goto Error_Exit;
    }

    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)constraint_hash, strlen(constraint_hash));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(constraint_hash=%s) failed. ret_value = %d", constraint_hash, ret_value);
        goto Error_Exit;
    }

    ret_value = DTappsStmtBindParam(pstmt, dIdx++, TAPPSDB_TYPE_VARCHAR, (void *)cid, strlen(cid));
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(cid=%s) failed. ret_value = %d", cid, ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Begin Transaction........");
    ret_value = DTappsDBBeginImmedTrans(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("DB Begin Transaction ret_value = %d",ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Execute SQL to Insert Contents into Table........");
    ret_value = DTappsStmtExecute(pstmt);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Execute SQL Query Failed!! ret_value = %d",ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Commit DB........");
    ret_value = DTappsDBCommit(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Commit DB Failed!! ret_value = %d",ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Release Statement........");
    ret_value = DTappsStmtRelease(pstmt);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Release Statement Failed!! ret_value = %d",ret_value);
        pstmt = NULL;
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Close DB........");
    ret_value = DTappsDBClose(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Install DB Operartion Successful!!!");
    return TRUE;

Error_Exit:
    if(pstmt != NULL)
        DTappsStmtRelease(pstmt);

    ret_value = DTappsDBRollback (__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Rollback DB Failed!! ret_value = %d",ret_value);
    }

    ret_value = DTappsDBClose(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);
    }

    return FALSE;
}

BOOL DTapps_RIGHTS_SELECT_ONE(const char* cid, DTAPPS_RIGHTS_ROW* row)
{
    void*         pDb       = NULL;
    const char*   query     = NULL;
    sqlite3_stmt* pstmt     = NULL;
    unsigned int  dIdx      = 0;
    BOOL          ret_value = FALSE;
    int           db_ret    = -1;
    DTAPPS_RIGHTS_ROW* result = row;

    char *name, *time, *constraint_buffer, *constraint_hash;
    #ifdef DTAPPS_STORE_CEK_IN_DB
    char *cek, *cek_hash;
    #endif

    DRM_TAPPS_LOG("Open DB......");
    ret_value = DTappsDBOpen(pDb,__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("DB Open Failed!! ret_value = %d",ret_value);
        return FALSE;
    }

    query = "SELECT r_id, name, time_t, "
            #ifdef DTAPPS_STORE_CEK_IN_DB
            " cek, cek_hash, "
            #endif
            " constraint_buffer, constraint_hash "
            " FROM dtapps2rights"
            " WHERE cid=? ORDER BY r_id desc limit 1;";


    DRM_TAPPS_LOG("Begin Transaction........");
    ret_value = DTappsDBBeginImmedTrans(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("DB Begin Transaction Failed!! ret_value = %d",ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Prepare Statement........");
    db_ret = sqlite3_prepare((sqlite3*)pDb, query, -1, &pstmt, NULL);
    if(pstmt == NULL)
    {
        DRM_TAPPS_EXCEPTION("Prepare Statement failed. query=%s, error=%s", query, sqlite3_errmsg((sqlite3*)pDb));
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Binding Parameters........");
    db_ret = sqlite3_bind_text(pstmt, ++dIdx, cid, strlen(cid), SQLITE_TRANSIENT);
    if(db_ret != SQLITE_OK)
    {
        DRM_TAPPS_EXCEPTION("Binding Parameter(cid=%s) failed. ret_value = %d, query=%s", cid, db_ret, query);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Execute SQL to Select Contents from Table........");
    db_ret = sqlite3_step(pstmt);
    if(db_ret == SQLITE_OK || db_ret == SQLITE_DONE)
    {
        DRM_TAPPS_FRQ_LOG("Executed SQL Select Query. But no row was returned");
        goto Error_Exit;
    }
    else if(db_ret != SQLITE_ROW)
    {
        DRM_TAPPS_EXCEPTION("Execute SQL Query Failed!! ret_value = %d, query=%s, error=%s",
                            db_ret, query, sqlite3_errmsg((sqlite3*)pDb));
        goto Error_Exit;
    }

    memset(result, 0, sizeof(DTAPPS_RIGHTS_ROW));

    dIdx = 0;
    result->r_id = sqlite3_column_int(pstmt, dIdx++);
    DRM_TAPPS_LOG("....SEELECTED : r_id = %d", result->r_id);

    name = (char*) sqlite3_column_text(pstmt, dIdx++);
    if(name != NULL)
        memcpy(result->name, name, strlen(name));
    DRM_TAPPS_LOG("....SEELECTED : name = %s", result->name);

    time= (char*) sqlite3_column_text(pstmt, dIdx++);
    if(time != NULL)
        memcpy(result->time, time, strlen(time));
    DRM_TAPPS_LOG("....SEELECTED : time = %s", result->time);

    #ifdef DTAPPS_STORE_CEK_IN_DB
    cek= (char*) sqlite3_column_text(pstmt, dIdx++);
    if(cek != NULL)
        memcpy(result->cek, cek, strlen(cek));
    DRM_TAPPS_LOG("....SEELECTED : cek = %s", result->cek);

    cek_hash= (char*) sqlite3_column_text(pstmt, dIdx++);
    if(cek_hash != NULL)
        memcpy(result->cek_hash, cek_hash, strlen(cek_hash));
    DRM_TAPPS_LOG("....SEELECTED : cek_hash = %s", result->cek_hash);
    #endif

    constraint_buffer = (char*) sqlite3_column_text(pstmt, dIdx++);
    if(constraint_buffer != NULL)
        memcpy(result->constraint_buffer, constraint_buffer, strlen(constraint_buffer));
    DRM_TAPPS_LOG("....SEELECTED : constraint_buffer = %s", result->constraint_buffer);

    constraint_hash = (char*) sqlite3_column_text(pstmt, dIdx++);
    if(constraint_hash != NULL)
        memcpy(result->constraint_hash, constraint_hash, strlen(constraint_hash));
    DRM_TAPPS_LOG("....SEELECTED : constraint_hash = %s", result->constraint_hash);

    memcpy(result->cid, cid, strlen(cid));
    DRM_TAPPS_LOG("....SEELECTED : cid = %s", result->cid);

    DRM_TAPPS_LOG("Release Statement........");
    ret_value = DTappsStmtRelease(pstmt);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Release Statement Failed!! ret_value = %d",ret_value);
        pstmt = NULL;
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Close DB........");
    ret_value = DTappsDBClose(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);
        goto Error_Exit;
    }

    DRM_TAPPS_LOG("Select a DB row Operartion Successful!!!");
    return TRUE;

Error_Exit:
    if(pstmt)
        DTappsStmtRelease(pstmt);

    ret_value = DTappsDBClose(__func__);
    if(ret_value != TRUE)
    {
        DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);
    }

    return FALSE;
}

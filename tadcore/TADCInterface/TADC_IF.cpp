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

//#define _SLP_SIMUL
#include "TADC_IF.h"
#include "TADC_Util.h"
#include "TADC_ErrorCode.h"

#include "drm_intf_tapps.h"

#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/dh.h>
#include <openssl/bn.h>

//2011.03.08 to verify signature
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <dirent.h>

#include "DUIDGenerator.h"


int TADC_IF_GetDUID(char *Duid)
{
	if (!Duid) {
		DRM_TAPPS_EXCEPTION("Invalid argument.");
		return TADC_GETDUID_ERROR;
	}

	char *duid = NULL;
	if (get_duid(&duid) < 0 || !duid) {
		DRM_TAPPS_EXCEPTION("Failed to get DUID.");
		return TADC_GETDUID_ERROR;
	}

	DRM_TAPPS_LOG("DUID is [%s]", duid);
	memcpy(Duid, duid, strlen(duid) + 1);

	free(duid);

	return TADC_SUCCESS;
}

int TADC_IF_GetDHKey(T_DH_INFO *t_dhinfo)
{
	DH *pDH = NULL;
	TADC_IF_MemSet(t_dhinfo, 0, sizeof(t_dh_info));

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_IF_GetDHKey  : After  TADC_IF_MemSet(t_dhinfo, 0, sizeof(t_dh_info))");

	//1. dh new
	if ((pDH = DH_new()) == NULL)
	{
		DRM_TAPPS_EXCEPTION("DH_new() error!");
		return -1;
	}

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_IF_GetDHKey  : After DH_new");

	//2. Set the Prime and Generator Value
	BYTE prime64[64] = {
		0xAE, 0xD9, 0x65, 0x3C, 0x86, 0x3E, 0xD9, 0x6F, 0x31, 0x6E,
		0xF6, 0x08, 0x35, 0xD5, 0x01, 0xC1, 0x41, 0x2E, 0xDD, 0x7E,
		0xE9, 0x09, 0x99, 0x73, 0xF3, 0xB3, 0xAB, 0x1F, 0x80, 0x85,
		0x44, 0x22, 0xDA, 0x07, 0x32, 0x18, 0xC1, 0xF8, 0xC4, 0xED,
		0x9F, 0x66, 0x88, 0xCF, 0xD6, 0x18, 0x8B, 0x28, 0x56, 0xA5,
		0xB3, 0x6A, 0x8E, 0xBB, 0xC4, 0x2B, 0x2B, 0x3A, 0x9C, 0x20,
		0x4E, 0xF7, 0x7F, 0xC3 };
	BYTE generator[1] = {DH_GENERATOR_5};

	pDH->p = BN_bin2bn(prime64, 64, NULL);
	pDH->g = BN_bin2bn(generator, 1, NULL);

	/* Set a to run with normal modexp and b to use constant time */
	pDH->flags &= ~DH_FLAG_NO_EXP_CONSTTIME;

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_IF_GetDHKey  : After Set the Prime and Generator Value");

	//3. Generate DH Key
	if (!DH_generate_key(pDH))
	{
		DRM_TAPPS_EXCEPTION("DH_generate_key() error!");
		return -1;
	}

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_IF_GetDHKey  : After DH_generate_key");

	//4. Save DH Infos ( p, g, A, a )
	TADC_IF_MemCpy(t_dhinfo->p, prime64, 64);
	t_dhinfo->pSize = 64;
	t_dhinfo->g = DH_GENERATOR_5;
	t_dhinfo->ASize = BN_bn2bin(pDH->pub_key, t_dhinfo->A);
	t_dhinfo->aSize = BN_bn2bin(pDH->priv_key, t_dhinfo->a);

	//5. DH Free
	DH_free(pDH);

	// Debug
	DRM_TAPPS_LOG("Debug Log == TADC_IF_GetDHKey  : After DH_free");

	return 0;
}

int TADC_IF_GetDHKey_K(T_DH_INFO *t_dhinfo)
{
	DH *pDH = NULL;
	BIGNUM *pPubKey = NULL;

	char tempbuf[DHKey_SIZE + 1];
	int i = 0;

	unsigned char tempG[1];

	TADC_IF_MemSet(tempbuf, 0, sizeof(tempbuf));

	//1. dh new
	if ((pDH = DH_new()) == NULL)
	{
		DRM_TAPPS_EXCEPTION("DH_new() error!");
		return -1;
	}

	//2.Set DH Info to pDH
	pDH->p = BN_bin2bn(t_dhinfo->p, t_dhinfo->pSize, NULL);
	tempG[0] = t_dhinfo->g;
	pDH->g = BN_bin2bn(tempG, 1, NULL);
	pDH->flags &= ~DH_FLAG_NO_EXP_CONSTTIME;
	pDH->pub_key = BN_bin2bn(t_dhinfo->A, t_dhinfo->ASize, NULL);
	pDH->priv_key = BN_bin2bn(t_dhinfo->a, t_dhinfo->aSize, NULL);

	//3. Set Public Key of Server
	pPubKey = BN_bin2bn(t_dhinfo->B, t_dhinfo->BSize, NULL);

	//4. Compute DH Session Key
	if ((i = DH_compute_key((BYTE*)tempbuf, pPubKey, pDH)) < 0)
	{
		DRM_TAPPS_EXCEPTION("DH_compute_key() error! \n");
		return -1;
	}

	for (i = 0 ; i < (t_dhinfo -> BSize / 2) ; i++)
	{
		t_dhinfo->K[i] = tempbuf[i * 2] ^ tempbuf[(i * 2) + 1];
	}

	//5. DH Free
	DH_free(pDH);
	BN_free(pPubKey);

	return 0;
}

/* Only handles 128 bit aes key */
int TADC_IF_AES_CTR(unsigned char *pKey, int ivLen, unsigned char *pIV, int inLen, unsigned char *in, int *pOutLen, unsigned char *out)
{
	AES_KEY stKey;
	UINT num;
	TADC_U8 ecount[16];
	TADC_U8 chain[16];

	AES_set_encrypt_key(pKey, 128, &stKey);

	num = 0;

	TADC_IF_MemSet(ecount, 0, sizeof(ecount));
	TADC_IF_MemSet(chain, 0, sizeof(chain));
	TADC_IF_MemCpy(chain, pIV, ivLen);

	AES_ctr128_encrypt(in, out, inLen, &stKey, chain, ecount, &num);

	*pOutLen = inLen;

	return 0;
}

int TADC_IF_SHA1(unsigned char *in, int inLen, unsigned char *out)
{
	SHA_CTX AlgInfo;

	SHA1_Init(&AlgInfo);
	SHA1_Update(&AlgInfo, in, inLen);
	SHA1_Final(out, &AlgInfo);

	return 0;
}

int TADC_IF_VerifySignature(unsigned char* inData, int inLen,
							unsigned char* sigData, int sigLen,
							unsigned char* cert, int certLen)
{
	unsigned char hashValue[20];
	int iRet = 0;

	X509* pX509 = NULL;
	EVP_PKEY* pKey = NULL;
	RSA* pRsa = NULL;

	//Check parameters
	if (inData == NULL || sigData == NULL || cert == NULL || inLen < 1 || sigLen < 1 || certLen < 1)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifySignature Error : Parameter error!");
		return -1;
	}

	iRet = 0;

	//1. Make Hash value of indata by using SHA1
	TADC_IF_SHA1(inData, inLen, hashValue);

	//2. Get RSA Public Key from cert data ( DER )
	pX509 = d2i_X509(NULL, (const unsigned char**)&cert, certLen);
	if (pX509 == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifySignature Error : Get RSA Public Key from cert data!");
		return -1;
	}

	pKey = X509_get_pubkey(pX509);
	if (pKey == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifySignature Error : X509_get_pubkey!");
		return -1;
	}

	pRsa = EVP_PKEY_get1_RSA(pKey);
	if (pRsa == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifySignature Error : EVP_PKEY_get1_RSA!");
		if (NULL != pKey)
		{
			EVP_PKEY_free(pKey);
		}
		return -1;
	}

	//3. Verify RSA Sign
	iRet = RSA_verify(NID_sha1, hashValue, 20, sigData, sigLen, pRsa);
	if (1 != iRet)
	{
		int err = 0;
		char tmpBuf[120] = {0,};

		while ((err = ERR_get_error()) != 0)
		{
			DRM_TAPPS_EXCEPTION("TADC_IF_VerifySignature Error : RSA_verify error(%s)", ERR_error_string(err, tmpBuf));
		}
		//Error
		//DRM_TAPPS_EXCEPTION("TADC_IF_VerifySignature Error : RSA_verify error(%s)", ERR_error_string(ERR_get_error(), NULL));

		if (NULL != pKey)
		{
			EVP_PKEY_free(pKey);
		}

		return -1;
	}

	//free
	if (NULL != pKey)
	{
		EVP_PKEY_free(pKey);
	}

	return 0;
}

int AddCertUntrustedCerts(STACK_OF(X509)* untrustedCerts, unsigned char* cert, int certLen)
{
	X509* pstX509 = NULL;

	if (untrustedCerts == NULL || cert == NULL || certLen < 1)
	{
		DRM_TAPPS_EXCEPTION("AddCertSTORE Error : Parameter error!");
		return -1;
	}

	pstX509 = d2i_X509(NULL, (const unsigned char **) &cert, certLen);
	if (pstX509 == NULL)
	{
		DRM_TAPPS_EXCEPTION("AddCertSTORE Error : d2i_X509 error!");
		return -1;
	}

	sk_X509_push(untrustedCerts, pstX509);

	return 0;
}

int AddCertSTOREFromFile(X509_STORE* pstStore, const char* filePath)
{
	X509* pstX509 = NULL;
	FILE* file = NULL;
	int ret = 0;

	file = fopen(filePath, "r");
	if(!file)
	{
		DRM_TAPPS_EXCEPTION("AddCertSTOREFromFile Error : Parameter error! Fail to open a cert file.");
		ret = -1;
		goto error;
	}

	pstX509 = PEM_read_X509(file, NULL, NULL, NULL);
	if (pstX509 == NULL)
	{
		DRM_TAPPS_EXCEPTION("AddCertSTORE Error : d2i_X509 error!");
		ret = -1;
		goto error;
	}

	X509_STORE_add_cert(pstStore, pstX509);

error:
	if(file!=NULL)
		fclose(file);
	return ret;
}

int AddCertSTOREFromDir(X509_STORE* pstStore, const char* dirPath)
{
	int ret = 0;

	DIR *dir = NULL;
	struct dirent entry;
	struct dirent *result;
	int error;
	char file_path_buff[512];

	if (pstStore == NULL || dirPath == NULL)
	{
		DRM_TAPPS_EXCEPTION("AddCertSTOREFromDir Error : Parameter error!");
		ret = -1;
		goto error;
	}

	dir = opendir(dirPath);
	if(dir == NULL) {
		DRM_TAPPS_EXCEPTION("AddCertSTOREFromDir Error : cannot open directory!");
		ret = -1;
		goto error;
	}

	for(;;) {
		error = readdir_r(dir, &entry, &result);
		if( error != 0 ) {
			DRM_TAPPS_EXCEPTION("AddCertSTOREFromDir Error : fail to read entries from a directory!");
			ret = -1;
			goto error;
		}
		// readdir_r returns NULL in *result if the end
		// of the directory stream is reached
		if(result == NULL)
			break;

		if(entry.d_type == DT_REG) { // regular file
			memset(file_path_buff, 0, sizeof(file_path_buff));
			snprintf(file_path_buff, sizeof(file_path_buff), "%s/%s", dirPath, entry.d_name);
			if(AddCertSTOREFromFile(pstStore, file_path_buff) == 0) {
				DRM_TAPPS_LOG("Add root cert : file=%s", file_path_buff);
			}else {
				DRM_TAPPS_LOG("Fail to add root cert : file=%s", file_path_buff);
			}
		}
	}

error:
	if(dir!=NULL)
		closedir(dir);
	return ret;
}

int TADC_IF_VerifyCertChain(unsigned char* rica, int ricaLen,
							unsigned char* cert, int certLen)
{
	X509_STORE_CTX* pstStoreCtx = NULL;
	X509_STORE* pstStore = NULL;
	STACK_OF(X509)* untrustedCerts = NULL;

	X509* pstX509 = NULL;

	int iRet = 0;
	int iErrCode = 0;

	//must call this function.
	OpenSSL_add_all_algorithms();

	pstStore = X509_STORE_new();
	if(pstStore == NULL)
	{
		iRet = -1;
		goto error;
	}

	untrustedCerts = sk_X509_new_null();
	if(untrustedCerts == NULL)
	{
		iRet = -1;
		goto error;
	}


	//Add RICA Cert to certchain
	if ((iRet = AddCertUntrustedCerts(untrustedCerts, rica, ricaLen)) != 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyCertChain Error : Add RICA Cert to certchain!");
		iRet = -1;
		goto error;
	}

	//Add Root CA Cert
	if ((iRet = AddCertSTOREFromDir(pstStore, RO_ISSUER_ROOT_CERTS_DIR)) != 0)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyCertChain Error : Add Root CA Cert!");
		iRet = -1;
		goto error;
	}

	//Get Cert
	pstX509 = d2i_X509(NULL, (const unsigned char **)&cert, certLen);

	if (pstX509 == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyCertChain Error : Get Cert d2i_X509 error!");
		iRet = -1;
		goto error;
	}

	X509_STORE_set_flags(pstStore, X509_V_FLAG_CB_ISSUER_CHECK);
	pstStoreCtx = X509_STORE_CTX_new();
	if (pstStoreCtx == NULL)
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyCertChain Error : 509_STORE_CTX_new error!");
		iRet = -1;
		goto error;
	}

	//init
	X509_STORE_CTX_init(pstStoreCtx, pstStore, pstX509, untrustedCerts);

	//Set Flag
	X509_STORE_CTX_set_flags(pstStoreCtx, X509_V_FLAG_CB_ISSUER_CHECK);

	//verify
	iRet = X509_verify_cert(pstStoreCtx);

	//free
error:
	if (pstStore != NULL)
		X509_STORE_free(pstStore);
	if (pstStoreCtx != NULL)
		X509_STORE_CTX_free(pstStoreCtx);
	if (untrustedCerts != NULL)
		sk_X509_free(untrustedCerts);

	if (iRet == 1)
	{
		DRM_TAPPS_LOG("TADC_IF_VerifyCertChain Success! \n");
		return 0;
	}
	else if (iRet == 0)
	{
		iErrCode = X509_STORE_CTX_get_error(pstStoreCtx);
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyCertChain Error : %s \n", X509_verify_cert_error_string(iErrCode));
		return -1;
	}
	else
	{
		DRM_TAPPS_EXCEPTION("TADC_IF_VerifyCertChain Error : 509_verify_cert error! \n");
		return -1;
	}
}

size_t TADC_IF_StrLen(const char *string)
{
	return strlen(string);
}

int TADC_IF_StrCmp(const char *string1, const char *string2)
{
	return strcmp(string1, string2);
}

int TADC_IF_StrNCmp(const char *string1, const char *string2, size_t count)
{
	return strncmp(string1, string2, count);
}

char *TADC_IF_StrNCpy(char *strDestination, const char *strSource, size_t count)
{
	return strncpy(strDestination, strSource, count);
}

unsigned long TADC_IF_StrtOul(const char *nptr, char **endptr, int base)
{
	return strtoul(nptr, endptr, base);
}

int TADC_IF_MemCmp(const void *buf1, const void *buf2, size_t count)
{
	return memcmp(buf1, buf2, count);
}

void TADC_IF_MemCpy(void *dest, const void *src, size_t count)
{
	memcpy(dest, src, count);
}

void TADC_IF_MemSet(void *dest, int c, size_t count)
{
	memset(dest, c, count);
}

void *TADC_IF_Malloc(size_t size)
{
	return malloc(size);
}

void TADC_IF_Free(void *memblock)
{
	if(memblock != NULL)
	{
		free(memblock);
	}
}

int TADC_IF_AtoI(char *str)
{
	return atoi(str);
}

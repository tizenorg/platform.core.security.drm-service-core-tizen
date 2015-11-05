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

#include "drm-tizen-apps.h"
#include "drm-tizen-error.h"
#include "TADC_Core.h"
#include "TADC_IF.h"
#include "DrmFileMgr.h"

#include "DTapps2Rights.h"
#include "drm_testutil.h"
#include <tzplatform_config.h>

#if 1
static int first_key = 0;
static int second_key = 0;
static int third_key = 0;

bool _write_logfile(const char *filename, char *buf, unsigned int len)
{
	FILE *fd = NULL;
	
	if ((fd = fopen(filename,"w+b")) == NULL)
	{
		return false;
	}

	fwrite(buf, 1, len,fd); 
	fclose(fd);

	return true;
}

bool _read_logfile(const char *filename, char *buf, unsigned int *pLen)
{
	FILE *fd = NULL;
	int  nReadLen = 0;
	
	if ((fd = fopen(filename,"r+b")) == NULL)
	{
		return false;
	}

	nReadLen = fread(buf, 1, *pLen,fd); 
	*pLen = nReadLen;

	fclose(fd);

	return true;
}

bool tc01_VerifyRoSignature_Positive_01(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.ro");

	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc01_VerifyRoSignature_Positive_01() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet != 0)
	{
		printf("VerifyROSignature Failed! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc01_VerifyRoSignature_Positive_01() finished! -------- success \n");
	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc01_VerifyRoSignature_Positive_02(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/8SPXfqc6iL-1.0.0.ro");

	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc01_VerifyRoSignature_Positive_02() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet != 0)
	{
		printf("VerifyROSignature Failed! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc01_VerifyRoSignature_Positive_02() finished! -------- success \n");
	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc01_VerifyRoSignature_Positive_03(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/FightGuiIF-1.0.0.ro");

	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc01_VerifyRoSignature_Positive_03() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet != 0)
	{
		printf("VerifyROSignature Failed! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc01_VerifyRoSignature_Positive_03() finished! -------- success \n");
	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc02_VerifyRoSignature_Negative_Cert_01(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo  = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.cert_only_selfsigned.ro");
	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc02_VerifyRoSignature_Negative_Cert_01() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet >= 0)
	{
		printf("VerifyROSignature have to be failed. But success! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc02_VerifyRoSignature_Negative_Cert_01 finished! -------- success \n");
	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc02_VerifyRoSignature_Negative_Cert_02(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo	= tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.cert_chain_invalid.ro");
	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc02_VerifyRoSignature_Negative_Cert_02() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet >= 0)
	{
		printf("VerifyROSignature have to be failed. But success! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc02_VerifyRoSignature_Negative_Cert_02 finished! -------- success \n");
	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc02_VerifyRoSignature_Negative_Cert_03(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo  = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.cert_invalid.ro");
	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc02_VerifyRoSignature_Negative_Cert_03() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet >= 0)
	{
		printf("VerifyROSignature have to be failed. But success! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc02_VerifyRoSignature_Negative_Cert_03 finished! -------- success \n");
	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc03_VerifyRoSignature_Negative_Signature_01(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo   = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/FightGuiIF-1.0.0.signature_invalid.ro");
	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc03_VerifyRoSignature_Negative_Signature_01() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet >= 0)
	{
		printf("VerifyROSignature have to be failed. But success! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc03_VerifyRoSignature_Negative_Signature_01() finished! -------- success \n");

	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc03_VerifyRoSignature_Negative_Signature_02(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo	= tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.signature_invalid.ro");
	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc03_VerifyRoSignature_Negative_Signature_02() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet >= 0)
	{
		printf("VerifyROSignature have to be failed. But success! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc03_VerifyRoSignature_Negative_Signature_01() finished! -------- success \n");

	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc03_VerifyRoSignature_Negative_Signature_03(void)
{
	char Buf[1024*10] = {0, };	
	const char *pRo  = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/8SPXfqc6iL-1.0.0.signature_invalid.ro");
	FILE *fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	int nRet = 0;

	printf("tc03_VerifyRoSignature_Negative_Signature_03() -------- Started! \n");

	fd = fopen(pRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pRo);
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pRo);
		goto CATCH;
	}

	nRet = TADC_VerifyROSignature((unsigned char*) Buf);
	if (nRet >= 0)
	{
		printf("VerifyROSignature have to be failed. But success! : %s, %d\n", pRo, nRet);
		goto CATCH;
	}
	if (fd != NULL)
	{
		fclose(fd);
	}

	printf("tc03_VerifyRoSignature_Negative_Signature_03() finished! -------- success \n");

	return true;

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return false;
}

bool tc04_isDrmFile_Positive_01(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/38EIfBurLJ.tpk");
	int nRet = 0;

	printf("tc04_isDrmFile_Positive_01() -------- Started!\n");

	nRet = drm_tizen_is_drm_file(pDCF, strlen(pDCF));
	if (nRet != TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file failed. Path = %s, Ret = %d\n", pDCF,  nRet);
		printf("%s file is not TADC file!\n", pDCF);
		return false;
	}

	printf("tc04_isDrmFile_Positive_01() finished! -------- success \n");

	return true;
}

bool tc04_isDrmFile_Positive_02(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/8SPXfqc6iL.tpk");
	int nRet = 0;

	printf("tc04_isDrmFile_Positive_02() -------- Started!\n");

	nRet = drm_tizen_is_drm_file(pDCF, strlen(pDCF));
	if (nRet != TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file failed. Path = %s, Ret = %d\n", pDCF,  nRet);
		printf("%s file is not TADC file!\n", pDCF);
		return false;
	}

	printf("tc04_isDrmFile_Positive_02() finished! -------- success \n");

	return true;
}

bool tc04_isDrmFile_Positive_03(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/FightGuiIF.tpk");
	int nRet = 0;

	printf("tc04_isDrmFile_Positive_03() -------- Started!\n");

	nRet = drm_tizen_is_drm_file(pDCF, strlen(pDCF));
	if (nRet != TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file failed. Path = %s, Ret = %d\n", pDCF,  nRet);
		printf("%s file is not TADC file!\n", pDCF);
		return false;
	}

	printf("tc04_isDrmFile_Positive_03() finished! -------- success \n");

	return true;
}

bool tc05_isDrmFile_Negative_01(void)
{
	const char *pApp = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DecryptedApp/38EIfBurLJ_dec.tpk");
	int nRet = 0;

	printf("tc05_isDrmFile_Negative_01() -------- Started! \n");

	nRet = drm_tizen_is_drm_file(pApp, strlen(pApp));
	if(nRet == TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file have to be failed. But Success!  Path = %s, Ret = %d\n", pApp,  nRet);
		return false;
	}

	printf("tc05_isDrmFile_Negative_01 finished! -------- success \n");

	return true;
}

bool tc05_isDrmFile_Negative_02(void)
{
	const char *pApp = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DecryptedApp/8SPXfqc6iL_dec.tpk");
	int nRet = 0;

	printf("tc05_isDrmFile_Negative_02() -------- Started! \n");

	nRet = drm_tizen_is_drm_file(pApp, strlen(pApp));
	if(nRet == TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file have to be failed. But Success!  Path = %s, Ret = %d\n", pApp,  nRet);
		return false;
	}

	printf("tc05_isDrmFile_Negative_02 finished! -------- success \n");

	return true;
}

bool tc05_isDrmFile_Negative_03(void)
{
	const char *pApp = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DecryptedApp/FightGuiIF_dec.tpk");
	int nRet = 0;

	printf("tc05_isDrmFile_Negative_03() -------- Started! \n");

	nRet = drm_tizen_is_drm_file(pApp, strlen(pApp));
	if(nRet == TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file have to be failed. But Success!  Path = %s, Ret = %d\n", pApp,  nRet);
		return false;
	}

	printf("tc05_isDrmFile_Negative_03 finished! -------- success \n");

	return true;
}

bool
tc06_DrmFileHandlerPositive_01(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/38EIfBurLJ.tpk");
	DrmFileHandler* pDcfHandler = NULL;
	unsigned char* pBufCEK = NULL;
	unsigned char pCEK[CEK_SIZE + 1] = {0xB1, 0x03, 0x4F, 0x30, 0xC8, 0x52, 0x45, 0x7E, 0x9D, 0xA2, 0x52, 0x25, 0x2E, 0xA9, 0x9B, 0x2B, 0x25, 0x36, 0xF1, 0x8D, 0x04, 0xD1, 0x4C, 0xE3, 0x96, 0x81, 0xD9, 0x98, 0xBB, 0xD7, 0x7E, 0xCA, 0x00};
	unsigned char pCID[32] = "38EIfBurLJ-1.0.2";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 1630724;
	long long originalEndOffset = 1631570;
	int nRet = 0;

	printf("tc06_DrmFileHandlerPositive_01() -------- Started! \n");

	pBufCEK = (unsigned char*)malloc(CEK_SIZE + 1);
	memset(pBufCEK, 0x00, CEK_SIZE + 1);

	pDcfHandler = new DrmFileHandler();
	if (pDcfHandler == NULL)
	{
		goto CATCH;
	}

	nRet = pDcfHandler->Construct(pDCF);
	if (nRet != 1)
	{
		printf("tc06_DrmFileHandlerPositive_01 - Construct() failed : %s, %x\n", pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_01 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_01 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_01 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc06_DrmFileHandlerPositive_01 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc06_DrmFileHandlerPositive_01 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc06_DrmFileHandlerPositive_01 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc06_DrmFileHandlerPositive_01 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	if (pDcfHandler != NULL)
	{
		delete pDcfHandler;
	}

	if (pBufCEK != NULL)
	{
		free(pBufCEK);
	}

	printf("tc06_DrmFileHandlerPositive_01 finished! -------- success \n");

	return true;

CATCH:
	if (pDcfHandler != NULL)
	{
		delete pDcfHandler;
	}

	if (pBufCEK != NULL)
	{
		free(pBufCEK);
	}

	return false;
}

bool
tc06_DrmFileHandlerPositive_02(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/8SPXfqc6iL.tpk");
	unsigned char* pBufCEK = NULL;
	DrmFileHandler* pDcfHandler = NULL;
	unsigned char pCEK[CEK_SIZE + 1] = {0xBB, 0x87, 0x5D, 0xA8, 0x2C, 0xC4, 0x47, 0x81, 0x90, 0xBA, 0xD9, 0xB0, 0x0C, 0xD2, 0x94, 0xE9, 0x19, 0x0F, 0x24, 0x62, 0x5B, 0x0B, 0x49, 0x7A, 0xAE, 0x8E, 0x1D, 0x88, 0x7F, 0xF9, 0x96, 0xDB, 0x00};
	unsigned char pCID[32] = "8SPXfqc6iL-1.0.0";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 705072;
	long long originalEndOffset = 705914;
	int nRet = 0;

	printf("tc06_DrmFileHandlerPositive_02() -------- Started! \n");

	pBufCEK = (unsigned char*)malloc(CEK_SIZE + 1);
	memset(pBufCEK, 0x00, CEK_SIZE + 1);

	pDcfHandler = new DrmFileHandler();
	if (pDcfHandler == NULL)
	{
		goto CATCH;
	}

	nRet = pDcfHandler->Construct(pDCF);
	if (nRet != 1)
	{
		printf("tc06_DrmFileHandlerPositive_02 - Construct() failed : %s, %x\n", pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_02 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_02 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_02 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc06_DrmFileHandlerPositive_02 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc06_DrmFileHandlerPositive_02 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc06_DrmFileHandlerPositive_02 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc06_DrmFileHandlerPositive_02 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	if (pDcfHandler != NULL)
	{
		delete pDcfHandler;
	}

	if (pBufCEK != NULL)
	{
		free(pBufCEK);
	}

	printf("tc06_DrmFileHandlerPositive_02 finished! -------- success \n");

	return true;

CATCH:
	if (pDcfHandler != NULL)
	{
		delete pDcfHandler;
	}

	if (pBufCEK != NULL)
	{
		free(pBufCEK);
	}

	return false;
}

bool
tc06_DrmFileHandlerPositive_03(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/FightGuiIF.tpk");
	unsigned char* pBufCEK = NULL;
	DrmFileHandler* pDcfHandler = NULL;
	unsigned char pCEK[CEK_SIZE + 1] = {0x34, 0x5A, 0x94, 0x28, 0xC3, 0xF4, 0x44, 0x3F, 0x86, 0x6D, 0xCF, 0xC5, 0x78, 0x1F, 0x23, 0xCE, 0xE9, 0x9B, 0xC4, 0x45, 0xA3, 0x30, 0x47, 0x1E, 0xB4, 0xE0, 0xAF, 0x96, 0x0F, 0xDE, 0xA9, 0xB4, 0x00};
	unsigned char pCID[32] = "FightGuiIF-1.0.0";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 45193789;
	long long originalEndOffset = 45194636;
	int nRet = 0;

	printf("tc06_DrmFileHandlerPositive_03() -------- Started! \n");

	pBufCEK = (unsigned char*)malloc(CEK_SIZE + 1);
	memset(pBufCEK, 0x00, CEK_SIZE + 1);

	pDcfHandler = new DrmFileHandler();
	if (pDcfHandler == NULL)
	{
		goto CATCH;
	}

	nRet = pDcfHandler->Construct(pDCF);
	if (nRet != 1)
	{
		printf("tc06_DrmFileHandlerPositive_03 - Construct() failed : %s, %x\n", pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_03 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_03 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc06_DrmFileHandlerPositive_03 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc06_DrmFileHandlerPositive_03 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc06_DrmFileHandlerPositive_03 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc06_DrmFileHandlerPositive_03 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc06_DrmFileHandlerPositive_03 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	if (pDcfHandler != NULL)
	{
		delete pDcfHandler;
	}

	if (pBufCEK != NULL)
	{
		free(pBufCEK);
	}

	printf("tc06_DrmFileHandlerPositive_03 finished! -------- success \n");

	return true;

CATCH:
	if (pDcfHandler != NULL)
	{
		delete pDcfHandler;
	}

	if (pBufCEK != NULL)
	{
		free(pBufCEK);
	}

	return false;
}

bool tc07_DrmFileMgrPositive_01(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/38EIfBurLJ.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	unsigned char pCEK[CEK_SIZE + 1] = {0xB1, 0x03, 0x4F, 0x30, 0xC8, 0x52, 0x45, 0x7E, 0x9D, 0xA2, 0x52, 0x25, 0x2E, 0xA9, 0x9B, 0x2B, 0x25, 0x36, 0xF1, 0x8D, 0x04, 0xD1, 0x4C, 0xE3, 0x96, 0x81, 0xD9, 0x98, 0xBB, 0xD7, 0x7E, 0xCA, 0x00};

	unsigned char pCID[32] = "38EIfBurLJ-1.0.2";
	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 1630724;
	long long originalEndOffset = 1631570;

	printf("tc07_DrmFileMgrPositive_01() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->OpenFileHandler(pDCF, 1, &first_key);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc07_DrmFileMgrPositive_01 - OpenFileHandler() failed : key = %d, file = %s, %x\n", first_key, pDCF, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->GetFileHandler(first_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc07_DrmFileMgrPositive_01 - GetFileHandler() failed : key = %d, file = %s, %x\n", first_key, pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc07_DrmFileMgrPositive_01 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc07_DrmFileMgrPositive_01 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc07_DrmFileMgrPositive_01 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc07_DrmFileMgrPositive_01 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc07_DrmFileMgrPositive_01 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc07_DrmFileMgrPositive_01 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc07_DrmFileMgrPositive_01 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	printf("tc07_DrmFileMgrPositive_01 finished! -------- success \n");

	return true;

CATCH:

	return false;
}

bool tc07_DrmFileMgrPositive_02(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/8SPXfqc6iL.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	unsigned char pCEK[CEK_SIZE + 1] = {0xBB, 0x87, 0x5D, 0xA8, 0x2C, 0xC4, 0x47, 0x81, 0x90, 0xBA, 0xD9, 0xB0, 0x0C, 0xD2, 0x94, 0xE9, 0x19, 0x0F, 0x24, 0x62, 0x5B, 0x0B, 0x49, 0x7A, 0xAE, 0x8E, 0x1D, 0x88, 0x7F, 0xF9, 0x96, 0xDB, 0x00};
	unsigned char pCID[32] = "8SPXfqc6iL-1.0.0";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 705072;
	long long originalEndOffset = 705914;

	printf("tc07_DrmFileMgrPositive_02() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->OpenFileHandler(pDCF, 1, &second_key);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc07_DrmFileMgrPositive_02 - OpenFileHandler() failed : key = %d, file = %s, %x\n", second_key, pDCF, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->GetFileHandler(second_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc07_DrmFileMgrPositive_02 - GetFileHandler() failed : key = %d, file = %s, %x\n", second_key, pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc07_DrmFileMgrPositive_02 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc07_DrmFileMgrPositive_02 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc07_DrmFileMgrPositive_02 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc07_DrmFileMgrPositive_02 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc07_DrmFileMgrPositive_02 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc07_DrmFileMgrPositive_02 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc07_DrmFileMgrPositive_02 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	printf("tc07_DrmFileMgrPositive_02 finished! -------- success \n");

	return true;

CATCH:

	return false;
}

bool tc07_DrmFileMgrPositive_03(void)
{
	const char *pDCF  = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/FightGuiIF.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	unsigned char pCEK[CEK_SIZE + 1] = {0x34, 0x5A, 0x94, 0x28, 0xC3, 0xF4, 0x44, 0x3F, 0x86, 0x6D, 0xCF, 0xC5, 0x78, 0x1F, 0x23, 0xCE, 0xE9, 0x9B, 0xC4, 0x45, 0xA3, 0x30, 0x47, 0x1E, 0xB4, 0xE0, 0xAF, 0x96, 0x0F, 0xDE, 0xA9, 0xB4, 0x00};

	unsigned char pCID[32] = "FightGuiIF-1.0.0";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 45193789;
	long long originalEndOffset = 45194636;

	printf("tc07_DrmFileMgrPositive_03() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->OpenFileHandler(pDCF, 1, &third_key);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc07_DrmFileMgrPositive_03 - OpenFileHandler() failed : key = %d, file = %s, %x\n", third_key, pDCF, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->GetFileHandler(third_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc07_DrmFileMgrPositive_03 - GetFileHandler() failed : key = %d, file = %s, %x\n", third_key, pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc07_DrmFileMgrPositive_03 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc07_DrmFileMgrPositive_03 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc07_DrmFileMgrPositive_03 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc07_DrmFileMgrPositive_03 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc07_DrmFileMgrPositive_03 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc07_DrmFileMgrPositive_03 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc07_DrmFileMgrPositive_03 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	printf("tc07_DrmFileMgrPositive_03 finished! -------- success \n");

	return true;

CATCH:

	return false;
}

bool tc08_DrmFileMgrPositive_GetFileHandler_01(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/38EIfBurLJ.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	unsigned char pCEK[CEK_SIZE + 1] = {0xB1, 0x03, 0x4F, 0x30, 0xC8, 0x52, 0x45, 0x7E, 0x9D, 0xA2, 0x52, 0x25, 0x2E, 0xA9, 0x9B, 0x2B, 0x25, 0x36, 0xF1, 0x8D, 0x04, 0xD1, 0x4C, 0xE3, 0x96, 0x81, 0xD9, 0x98, 0xBB, 0xD7, 0x7E, 0xCA, 0x00};

	unsigned char pCID[32] = "38EIfBurLJ-1.0.2";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 1630724;
	long long originalEndOffset = 1631570;

	printf("tc08_DrmFileMgrPositive_GetFileHandler_01() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(first_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 failed : key = %d, file = %s, %x\n", first_key, pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	printf("tc08_DrmFileMgrPositive_GetFileHandler_01 finished! -------- success \n");

	return true;

CATCH:

	return false;
}

bool tc08_DrmFileMgrPositive_GetFileHandler_02(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/8SPXfqc6iL.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	unsigned char pCEK[CEK_SIZE + 1] = {0xBB, 0x87, 0x5D, 0xA8, 0x2C, 0xC4, 0x47, 0x81, 0x90, 0xBA, 0xD9, 0xB0, 0x0C, 0xD2, 0x94, 0xE9, 0x19, 0x0F, 0x24, 0x62, 0x5B, 0x0B, 0x49, 0x7A, 0xAE, 0x8E, 0x1D, 0x88, 0x7F, 0xF9, 0x96, 0xDB, 0x00};

	unsigned char pCID[32] = "8SPXfqc6iL-1.0.0";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 705072;
	long long originalEndOffset = 705914;

	printf("tc08_DrmFileMgrPositive_GetFileHandler_02() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(second_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 failed : key = %d, file = %s, %x\n", second_key, pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	printf("tc08_DrmFileMgrPositive_GetFileHandler_02 finished! -------- success \n");

	return true;

CATCH:

	return false;
}

bool tc08_DrmFileMgrPositive_GetFileHandler_03(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/FightGuiIF.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	unsigned char pCEK[CEK_SIZE + 1] = {0x34, 0x5A, 0x94, 0x28, 0xC3, 0xF4, 0x44, 0x3F, 0x86, 0x6D, 0xCF, 0xC5, 0x78, 0x1F, 0x23, 0xCE, 0xE9, 0x9B, 0xC4, 0x45, 0xA3, 0x30, 0x47, 0x1E, 0xB4, 0xE0, 0xAF, 0x96, 0x0F, 0xDE, 0xA9, 0xB4, 0x00};

	unsigned char pCID[32] = "FightGuiIF-1.0.0";

	long encryptionLevel = 17;
	long long encryptionRange = 10;
	long long plainTextSize = 45193789;
	long long originalEndOffset = 45194636;

	printf("tc08_DrmFileMgrPositive_GetFileHandler_03() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(third_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 failed : key = %d, file = %s, %x\n", third_key, pDCF, nRet);
		goto CATCH;
	}

	if (memcmp(pCEK, pDcfHandler->m_pCEK, CEK_SIZE) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - CEK Comparison failed : %s\n", pDCF);
		goto CATCH;
	}

	if (memcmp(pDCF, pDcfHandler->m_pFilePath, strlen((char*)pDcfHandler->m_pFilePath)) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - FilePath Comparison failed : %s\n", pDcfHandler->m_pFilePath);
		goto CATCH;
	}

	if (memcmp(pCID, pDcfHandler->m_pCID, strlen((char*)pDcfHandler->m_pCID)) != 0)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - CID Comparison failed : %s\n", pDcfHandler->m_pCID);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionLevel != encryptionLevel)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - encryptionLevel Comparison failed : origin = %ld, result = %ld\n", encryptionLevel, pDcfHandler->m_encryptionLevel);
		goto CATCH;
	}

	if (pDcfHandler->m_encryptionRange != encryptionRange)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - encryptionRange Comparison failed : origin = %lld, result = %lld\n", encryptionRange, pDcfHandler->m_encryptionRange);
		goto CATCH;
	}

	if (pDcfHandler->m_plaintextSize != plainTextSize)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - plainTextSize Comparison failed : origin = %lld, result = %lld\n", plainTextSize, pDcfHandler->m_plaintextSize);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	printf("tc08_DrmFileMgrPositive_GetFileHandler_03 finished! -------- success \n");

	return true;

CATCH:

	return false;
}

bool tc09_DrmFileMgrPositive_FileAPI_01(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/38EIfBurLJ.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	long long originalEndOffset = 1631570;
	long long DrmCurOffset = 0;
	long long OriginCurOffset = 846;
	long long i = 0;

	printf("tc09_DrmFileMgrPositive_FileAPI_01() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(first_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 failed : key = %d, file = %s, %x\n", first_key, pDCF, nRet);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	if (OriginCurOffset != pDcfHandler->m_OriginCurOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 - originalCurOffset Comparison failed : origin = %lld, result = %lld\n", OriginCurOffset, pDcfHandler->m_OriginCurOffset);
		goto CATCH;
	}

	for (i = 0 ; i <= pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(i, SEEK_SET);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_SET, forward) : key = %d, file = %s, i = %lld, ret = %x\n", first_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 - DrmCurOffset Comparison failed (SEEK_SET, forward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_SET, forward), DrmCurOffset = %lld, RealEndOffset = %lld, %x\n", DrmCurOffset, pDcfHandler->m_DrmEndOffset, nRet);
		goto CATCH;
	}

	for (i = pDcfHandler->m_DrmEndOffset ; i >= 0 ; i--)
	{
		nRet = pDcfHandler->DrmSeek(i, SEEK_SET);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_SET, backward) : key=%d, file=%s, i=%lld, %x\n", first_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 - DrmCurOffset Comparison failed (SEEK_SET, backward) : origin = %lld, result = %lld, i=%lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_SET, backward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i <= pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(-i, SEEK_END);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_END, forward) : key=%d, file=%s, i=%lld, %x\n", first_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 - DrmCurOffset Comparison failed (SEEK_END, forward) : origin = %lld, result = %lld, i = %%lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_END, forward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = pDcfHandler->m_DrmEndOffset ; i >= 0 ; i--)
	{
		nRet = pDcfHandler->DrmSeek(-i, SEEK_END);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_END, backward) : key=%d, file=%s, i=%lld, %x\n", first_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 - DrmCurOffset Comparison failed (SEEK_END, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_END, backward), DrmCurOffset = %lld, RealEndOffset = %lld, %x\n", DrmCurOffset, pDcfHandler->m_DrmEndOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i < pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(-1, SEEK_CUR);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_CUR, forward) : key=%d, file=%s, i=%lld, %x\n", first_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 - DrmCurOffset Comparison failed : origin=%lld, result=%lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_CUR, forward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i < pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(1, SEEK_CUR);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_CUR, backward) : key=%d, file=%s, i=%lld, %x\n", first_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_01 - DrmCurOffset Comparison failed (SEEK_CUR, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 DrmSeek failed (SEEK_CUR, backward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	printf("tc09_DrmFileMgrPositive_FileAPI_01 finished! -------- success \n");

	return true;

CATCH:
	return false;
}

bool tc09_DrmFileMgrPositive_FileAPI_02(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/8SPXfqc6iL.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	long long originalEndOffset = 705914;
	long long DrmCurOffset = 0;
	long long OriginCurOffset = 842;
	long long i = 0;

	printf("tc09_DrmFileMgrPositive_FileAPI_02() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(second_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 failed : key = %d, file = %s, %x\n", second_key, pDCF, nRet);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	if (OriginCurOffset != pDcfHandler->m_OriginCurOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 - originalCurOffset Comparison failed : origin = %lld, result = %lld\n", OriginCurOffset, pDcfHandler->m_OriginCurOffset);
		goto CATCH;
	}

	for (i = 0 ; i <= pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(i, SEEK_SET);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_SET, forward) : key = %d, file = %s, i = %lld, ret = %x\n", second_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 - DrmCurOffset Comparison failed (SEEK_SET, forward : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_SET, forward), DrmCurOffset = %lld, RealEndOffset = %lld, %x\n", DrmCurOffset, pDcfHandler->m_DrmEndOffset, nRet);
		goto CATCH;
	}

	for (i = pDcfHandler->m_DrmEndOffset ; i >= 0 ; i--)
	{
		nRet = pDcfHandler->DrmSeek(i, SEEK_SET);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_SET, backward) : key=%d, file=%s, i=%lld, %x\n", second_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 - DrmCurOffset Comparison failed (SEEK_SET, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_SET, backward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i <= pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(-i, SEEK_END);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_END, forward) : key=%d, file=%s, i=%lld, %x\n", second_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 - DrmCurOffset Comparison failed (SEEK_END, forward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_END, forward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = pDcfHandler->m_DrmEndOffset ; i >= 0 ; i--)
	{
		nRet = pDcfHandler->DrmSeek(-i, SEEK_END);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_END, backward) : key=%d, file=%s, i=%lld, %x\n", second_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 - DrmCurOffset Comparison failed (SEEK_END, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_END, backward), DrmCurOffset = %lld, RealEndOffset = %lld, %x\n", DrmCurOffset, pDcfHandler->m_DrmEndOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i < pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(-1, SEEK_CUR);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_CUR, forward) : key=%d, file=%s, i=%lld, %x\n", second_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 - DrmCurOffset Comparison failed : origin=%lld, result=%lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_CUR, forward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i < pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(1, SEEK_CUR);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_CUR, backward) : key=%d, file=%s, i=%lld, %x\n", second_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_02 - DrmCurOffset Comparison failed (SEEK_CUR, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 DrmSeek failed (SEEK_CUR, backward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	printf("tc09_DrmFileMgrPositive_FileAPI_02 finished! -------- success \n");

	return true;

CATCH:
	return false;
}

bool tc09_DrmFileMgrPositive_FileAPI_03(void)
{
	const char *pDCF = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/FightGuiIF.tpk");
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	long long originalEndOffset = 45194636;
	long long DrmCurOffset = 0;
	long long OriginCurOffset = 847;
	long long i = 0;

	printf("tc09_DrmFileMgrPositive_FileAPI_03() -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(third_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 failed : key = %d, file = %s, %x\n", third_key, pDCF, nRet);
		goto CATCH;
	}

	if (pDcfHandler->m_OriginEndOffset != originalEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 - originalEndOffset Comparison failed : origin = %lld, result = %lld\n", originalEndOffset, pDcfHandler->m_OriginEndOffset);
		goto CATCH;
	}

	if (OriginCurOffset != pDcfHandler->m_OriginCurOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 - originalCurOffset Comparison failed : origin = %lld, result = %lld\n", OriginCurOffset, pDcfHandler->m_OriginCurOffset);
		goto CATCH;
	}

	for (i = 0 ; i <= pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(i, SEEK_SET);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_SET, forward) : key = %d, file = %s, i = %lld, ret = %x\n", third_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 - DrmCurOffset Comparison failed (SEEK_SET, forward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_SET, forward), DrmCurOffset = %lld, RealEndOffset = %lld, %x\n", DrmCurOffset, pDcfHandler->m_DrmEndOffset, nRet);
		goto CATCH;
	}

	for (i = pDcfHandler->m_DrmEndOffset ; i >= 0 ; i--)
	{
		nRet = pDcfHandler->DrmSeek(i, SEEK_SET);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_SET, backward) : key=%d, file=%s, i=%lld, %x\n", third_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 - DrmCurOffset Comparison failed (SEEK_SET, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_SET, backward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i <= pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(-i, SEEK_END);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_END, forward) : key=%d, file=%s, i=%lld, %x\n", third_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 - DrmCurOffset Comparison failed (SEEK_END, forward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_END, forward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = pDcfHandler->m_DrmEndOffset ; i >= 0 ; i--)
	{
		nRet = pDcfHandler->DrmSeek(-i, SEEK_END);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_END, backward) : key=%d, file=%s, i=%lld, %x\n", third_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 - DrmCurOffset Comparison failed (SEEK_END, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_END, backward), DrmCurOffset = %lld, RealEndOffset = %lld, %x\n", DrmCurOffset, pDcfHandler->m_DrmEndOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i < pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(-1, SEEK_CUR);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_CUR, forward) : key=%d, file=%s, i=%lld, %x\n", third_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 - DrmCurOffset Comparison failed : origin=%lld, result=%lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset);
			goto CATCH;
		}
	}
	if (DrmCurOffset != 0)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_CUR, forward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	for (i = 0 ; i < pDcfHandler->m_DrmEndOffset ; i++)
	{
		nRet = pDcfHandler->DrmSeek(1, SEEK_CUR);
		if (nRet != TADC_SUCCESS)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_CUR, backward) : key=%d, file=%s, i=%lld, %x\n", third_key, pDCF, i, nRet);
			goto CATCH;
		}

		DrmCurOffset = pDcfHandler->DrmTell();
		if (DrmCurOffset != pDcfHandler->m_DrmCurOffset)
		{
			printf("tc09_DrmFileMgrPositive_FileAPI_03 - DrmCurOffset Comparison failed (SEEK_CUR, backward) : origin = %lld, result = %lld, i = %lld\n", DrmCurOffset, pDcfHandler->m_DrmCurOffset, i);
			goto CATCH;
		}
	}
	if (DrmCurOffset != pDcfHandler->m_DrmEndOffset)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 DrmSeek failed (SEEK_CUR, backward), DrmCurOffset = %lld, %x\n", DrmCurOffset, nRet);
		goto CATCH;
	}

	printf("tc09_DrmFileMgrPositive_FileAPI_03 finished! -------- success \n");

	return true;

CATCH:
	return false;
}

bool tc10_DrmFileMgrPositive_CloseFileHandler_01(void)
{
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	printf("tc10_DrmFileMgrPositive_CloseFileHandler_01 -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(first_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_01 failed : key = %d, ret=%x\n", first_key, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->CloseFileHandler(first_key);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_01 failed : key = %d, ret=%x\n", first_key, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->GetFileHandler(first_key, &pDcfHandler);
	if (nRet == TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_01 failed : key = %d\n", first_key);
		goto CATCH;
	}

	printf("tc10_DrmFileMgrPositive_CloseFileHandler_01 finished! -------- success \n");

	return true;

CATCH:
	return false;
}

bool tc10_DrmFileMgrPositive_CloseFileHandler_02(void)
{
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	printf("tc10_DrmFileMgrPositive_CloseFileHandler_02 -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(second_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_02 failed : key = %d, ret=%x\n", second_key, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->CloseFileHandler(second_key);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_02 failed : key = %d, ret=%x\n", second_key, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->GetFileHandler(second_key, &pDcfHandler);
	if (nRet == TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_02 failed : key = %d\n", second_key);
		goto CATCH;
	}

	printf("tc10_DrmFileMgrPositive_CloseFileHandler_02 finished! -------- success \n");

	return true;

CATCH:
	return false;
}

bool tc10_DrmFileMgrPositive_CloseFileHandler_03(void)
{
	int nRet = TADC_SUCCESS;

	DrmFileMgr* pDrmFileMgr = NULL;
	DrmFileHandler *pDcfHandler = NULL;

	printf("tc10_DrmFileMgrPositive_CloseFileHandler_03 -------- Started! \n");

	pDrmFileMgr = DrmFileMgr::GetInstance();

	nRet = pDrmFileMgr->GetFileHandler(third_key, &pDcfHandler);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_03 failed : key = %d, ret=%x\n", third_key, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->CloseFileHandler(third_key);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_03 failed : key = %d, ret=%x\n", third_key, nRet);
		goto CATCH;
	}

	nRet = pDrmFileMgr->GetFileHandler(third_key, &pDcfHandler);
	if (nRet == TADC_SUCCESS)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_03 failed : key = %d\n", third_key);
		goto CATCH;
	}

	printf("tc10_DrmFileMgrPositive_CloseFileHandler_03 finished! -------- success \n");

	return true;

CATCH:
	return false;
}

bool tc11_GetDeviceId_01(void)
{
	unsigned char DUID[33] = {0,};
	int nRet = 0;

	printf("tc11_GetDeviceID_01() -------- Started! \n");

	nRet = TADC_IF_GetDUID((char*)DUID);
	if (nRet !=TADC_SUCCESS)
	{
		printf("tc11_GetDeviceID_01() failed.");
		return false;
	}

	printf("DUID=%s\n", (char*)DUID);
	printf("tc11_GetDeviceID_01() finished! -------- success \n");

	return true;
}

bool tc12_TADC_SetDeviceInfo_01(void)
{
	int	nRet = TADC_SUCCESS;
	T_DEVICE_INFO	t_DeviceInfo;

	printf("tc12_TADC_SetDeviceInfo_01() -------- Started! \n");

	nRet = TADC_SetDeviceInfo(&t_DeviceInfo);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc12_TADC_SetDeviceInfo_01() failed.\n");
		return false;
	}
	printf("DUID=%s\n", (char*)t_DeviceInfo.DUID);
	printf("tc12_TADC_SetDeviceInfo_01() finished! -------- success \n");
	
	return true;
}

bool
tc13_DTappsInstallLicense_01(void)
{
	int nRet = TADC_SUCCESS;
	char Buf[1024*10] = {0, };	
	const char *pFirstRo = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.ro");
	FILE* fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	bool res = true;

	printf("tc13_DTappsInstallLicense_01() -------- Started! \n");

	fd = fopen(pFirstRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pFirstRo);
		res = false;
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		res = false;
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pFirstRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pFirstRo);
		res = false;
		goto CATCH;
	}

	nRet = DTappsInstallLicense(Buf);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc13_DTappsInstallLicense_01() failed. nRet=%d\n", nRet);
		res = false;
		goto CATCH;
	}

	printf("tc13_DTappsInstallLicense_01() finished! -------- success \n");

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return res;
}

bool
tc13_DTappsInstallLicense_02(void)
{
	int	nRet = TADC_SUCCESS;
	char Buf[1024*10] = {0, };	
	const char *pFirstRo = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/8SPXfqc6iL-1.0.0.ro");
	FILE* fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	bool res = true;

	printf("tc13_DTappsInstallLicense_02() -------- Started! \n");

	fd = fopen(pFirstRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pFirstRo);
		res = false;
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		res = false;
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pFirstRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pFirstRo);
		res = false;
		goto CATCH;
	}

	nRet = DTappsInstallLicense(Buf);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc13_DTappsInstallLicense_02() failed. nRet=%d\n", nRet);
		res = false;
		goto CATCH;
	}

	printf("tc13_DTappsInstallLicense_02() finished! -------- success \n");

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return res;
}

bool
tc13_DTappsInstallLicense_03(void)
{
	int	nRet = TADC_SUCCESS;
	char Buf[1024*10] = {0, };	
	const char *pFirstRo = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/FightGuiIF-1.0.0.ro");
	FILE* fd = NULL;
	int	nReadLen = 0;
	long lSize = 0;
	bool res = true;

	printf("tc13_DTappsInstallLicense_03() -------- Started! \n");

	fd = fopen(pFirstRo, "rb");
	if (fd == NULL)
	{
		printf("File is not exist! : %s\n", pFirstRo);
		res = false;
		goto CATCH;
	}

	fseek(fd, 0, SEEK_END);
	lSize = ftell(fd);
	if (lSize < 0)
	{
		printf("fseek() and ftell() failed.");
		res = false;
		goto CATCH;
	}
	rewind(fd);

	memset(Buf, 0x00, sizeof(Buf));
	nReadLen = fread(Buf, 1, lSize, fd);
	if (nReadLen >= sizeof(Buf))
	{
		printf("Buffer error! : %s\n", pFirstRo);
		goto CATCH;
	}

	if (nReadLen != lSize)
	{
		printf("File read error! : %s\n", pFirstRo);
		res = false;
		goto CATCH;
	}

	nRet = DTappsInstallLicense(Buf);
	if (nRet != TADC_SUCCESS)
	{
		printf("tc13_DTappsInstallLicense_03() failed. nRet=%d\n", nRet);
		res = false;
		goto CATCH;
	}

	printf("tc13_DTappsInstallLicense_03() finished! -------- success \n");

CATCH:
	if (fd != NULL)
	{
		fclose(fd);
	}
	return res;
}

bool
tc_local_01_drm_tizen_generate_license_request_p(void)
{
    bool res = true;
    int ret_value = DRMTEST_SUCCESS;

    char *purchase_response = NULL;
    char *req_id = NULL;
    unsigned int  req_buff_len = 1024*5;
    char req_buff[req_buff_len] = {0, };
    unsigned int  url_buff_len = 1024;
    char url_buff[url_buff_len] = {0, };

    printf("[%s] test started\n", __func__);
    ret_value = generate_purchase_response(&purchase_response, &req_id);
    if(ret_value != DRMTEST_SUCCESS) {
        res = false;
        goto CATCH;
    }
    printf("...purchase_response:\n%s\n", purchase_response);

    ret_value = drm_tizen_generate_license_request(purchase_response, strlen(purchase_response),
                                             req_buff, &req_buff_len, url_buff, &url_buff_len);
    if(ret_value != TADC_SUCCESS) {
        printf("...drm_tizen_generate_license_request failed. ret=%d\n", ret_value);
        res = false;
        goto CATCH;
    }
    printf("...license_request_url:%s\n", url_buff);
    printf("...license_request:\n%s\n", req_buff);

    printf("[%s] test ended: result=%d\n", __func__, ret_value);

CATCH:
    if(purchase_response != NULL)
        free(purchase_response);
    if(req_id != NULL)
        free(req_id);

    return res;
}

bool
tc_local_01_drm_tizen_full_test_p(void)
{
    bool res = true;
    int ret_value = DRMTEST_SUCCESS;

    char *purchase_response = NULL;
    char *req_id = NULL;
    unsigned int  req_buff_len = 1024*5;
    char req_buff[req_buff_len] = {0, };
    unsigned int  url_buff_len = 1024;
    char url_buff[url_buff_len] = {0, };

    char* ro_request_buff = NULL;
    char* ro_response_buff = NULL;
    char* dh_key_p = NULL;
    char* dh_key_g = NULL;
    char* dh_key_a = NULL;

    unsigned char duid[33] = {0,};
    const char* cid = "38EIfBurLJ-1.0.2";
    const char* ro_template_path = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/RO/38EIfBurLJ-1.0.2.ro_template");

    const char* encrypted_file_path = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DCF/38EIfBurLJ.tpk");
    const char* plaintext_file_path = tzplatform_mkpath(TZ_SYS_DATA,"drm_test/DecryptedApp/38EIfBurLJ_dec.tpk");
    const char* decrypted_file_path = "/tmp/38EIfBurLJ.tpk";

    int identical = DRMTEST_NOTIDENTICAL;

    printf("[%s] test started\n", __func__);

    if(ret_value != DRMTEST_SUCCESS) {
        res = false;
        goto CATCH;
    }

    ret_value = generate_purchase_response(&purchase_response, &req_id);
    if(ret_value != DRMTEST_SUCCESS) {
        res = false;
        goto CATCH;
    }
    //printf("...purchase_response:\n%s\n", purchase_response);

    ret_value = drm_tizen_generate_license_request(purchase_response, strlen(purchase_response),
                                             req_buff, &req_buff_len, url_buff, &url_buff_len);
    if(ret_value != TADC_SUCCESS) {
        printf("...drm_tizen_generate_license_request failed. ret=%d\n", ret_value);
        res = false;
        goto CATCH;
    }
    printf("...license_request_url:%s\n", url_buff);
    printf("...license_request:\n%s\n", req_buff);

    ret_value = get_dh_key_from_ro_request(req_buff, &dh_key_p, &dh_key_g, &dh_key_a);
    if(ret_value != DRMTEST_SUCCESS) {
        res = false;
        goto CATCH;
    }
    //printf("...DH key: p=%s, g=%s, a=%s\n", dh_key_p, dh_key_g, dh_key_a);

    ret_value = TADC_IF_GetDUID((char*)duid);
    if (ret_value != TADC_SUCCESS) {
        return false;
    }
    printf("duid=%s\n", (char*)duid);

    ret_value = generate_right_object_response(dh_key_p, dh_key_g, dh_key_a, req_id,
                                               cid, ro_template_path, (const char*)duid, &ro_response_buff);
    if(ret_value != DRMTEST_SUCCESS) {
        res = false;
        goto CATCH;
    }
    printf("...right_object_response:\n%s\n", ro_response_buff);

    ret_value = drm_tizen_register_license(ro_response_buff, strlen(ro_response_buff));
    if(ret_value != TADC_SUCCESS) {
        printf("...drm_tizen_register_license failed. ret=%d\n", ret_value);
        res = false;
        goto CATCH;
    }
    printf("...drm_tizen_register_license: succeeded\n");

    ret_value = drm_tizen_decrypt_package(encrypted_file_path, strlen(encrypted_file_path),
                                          decrypted_file_path, strlen(decrypted_file_path));
    if(ret_value != TADC_SUCCESS) {
        printf("...drm_tizen_decrypt_package failed. ret=%d\n", ret_value);
        res = false;
        goto CATCH;
    }
    printf("...drm_tizen_decrypt_package: succeeded\n");

    ret_value = is_identical_files(plaintext_file_path, decrypted_file_path, &identical);
    if(ret_value != DRMTEST_SUCCESS) {
        printf("...is_identical_files failed. ret=%d\n", ret_value);
        res = false;
        goto CATCH;
    }
    if(identical != DRMTEST_IDENTICAL) {
        printf("...the decrypted file is not same with a original plaintext file .\n");
        res = false;
        goto CATCH;
    }
    printf("...drm_tizen_decrypt_package: succeeded : the decrypted file is with a original plaintext file.\n");

CATCH:
    if(purchase_response != NULL)
        free(purchase_response);
    if(req_id != NULL)
        free(req_id);
    if(ro_request_buff != NULL)
        free(ro_request_buff);
    if(ro_response_buff != NULL)
        free(ro_response_buff);

    return res;
}


/*
bool tc15_DrmTdcDecryptPackge_01(void)
{
	int	nRet = TADC_SUCCESS;

}
*/
#endif

int main(int argc, char* argv[])
{
//	return 0;
#if 1
	printf(" ---------- Test Tizen DRM v2.0.1 APIs   ---  Start ....    \n");

	int				bRet = TADC_SUCCESS;
	
	char 			ReqBuf[1024*5] = {0, };
	unsigned int 	ReqBufLen = 0;
	
	char 			RespBuf[1024*10] = {0, };	
	unsigned int	RespBufLen = 0;
	
//	char 			DecLicenseBuf[1024*10] = {0, };	
//	unsigned int 	DecLicenseBufLen = 0;
	
	char 			LicenseUrl[1024] = {0, };
	unsigned int	LicenseUrlLen = 0;
	
	//2011.03.08
	//DrmTdcFileHeader	TDCFileHeader;
//	char	cid[1024] = {0, };
//	char	riurl[1024] = {0, };
	
	char	testFileName[256] = {0, };
	char	tempBuf[256] = {0, };

	bool	isOk = false;

	//----------------------------------------------------------------------------------------------------
	// 0. Static TC running
	//----------------------------------------------------------------------------------------------------

	printf("\n0. Static TC  Start  ----------------------------------------\n");
/*
	isOk = tc01_VerifyRoSignature_Positive_01();
	if (isOk != true)
	{
		printf("tc01_VerifyRoSignature_Positive_01 failed!\n");
	}

	isOk = tc01_VerifyRoSignature_Positive_02();
	if (isOk != true)
	{
		printf("tc01_VerifyRoSignature_Positive_02 failed!\n");
	}

	isOk = tc01_VerifyRoSignature_Positive_03();
	if (isOk != true)
	{
		printf("tc01_VerifyRoSignature_Positive_03 failed!\n");
	}

	isOk = tc02_VerifyRoSignature_Negative_Cert_01();
	if (isOk != true)
	{
		printf("tc02_VerifyRoSignature_Negative_Cert_01 failed!\n");
	}

	isOk = tc02_VerifyRoSignature_Negative_Cert_02();
	if (isOk != true)
	{
		printf("tc02_VerifyRoSignature_Negative_Cert_02 failed!\n");
	}

	isOk = tc02_VerifyRoSignature_Negative_Cert_03();
	if (isOk != true)
	{
		printf("tc02_VerifyRoSignature_Negative_Cert_03 failed!\n");
	}

	isOk = tc03_VerifyRoSignature_Negative_Signature_01();
	if (isOk != true)
	{
		printf("tc03_VerifyRoSignature_Negative_Signature_01 failed!\n");
	}

	isOk = tc03_VerifyRoSignature_Negative_Signature_02();
	if (isOk != true)
	{
		printf("tc03_VerifyRoSignature_Negative_Signature_02 failed!\n");
	}

	isOk = tc03_VerifyRoSignature_Negative_Signature_03();
	if (isOk != true)
	{
		printf("tc03_VerifyRoSignature_Negative_Signature_03 failed!\n");
	}

	isOk = tc04_isDrmFile_Positive_01();
	if (isOk != true)
	{
		printf("tc04_isDrmFile_Positive_01 failed!\n");
	}

	isOk = tc04_isDrmFile_Positive_02();
	if (isOk != true)
	{
		printf("tc04_isDrmFile_Positive_02 failed!\n");
	}

	isOk = tc04_isDrmFile_Positive_03();
	if (isOk != true)
	{
		printf("tc04_isDrmFile_Positive_03 failed!\n");
	}

	isOk = tc05_isDrmFile_Negative_01();
	if (isOk != true)
	{
		printf("tc05_isDrmFile_Negative_01 failed!\n");
	}

	isOk = tc05_isDrmFile_Negative_02();
	if (isOk != true)
	{
		printf("tc05_isDrmFile_Negative_02 failed!\n");
	}

	isOk = tc05_isDrmFile_Negative_03();
	if (isOk != true)
	{
		printf("tc05_isDrmFile_Negative_03 failed!\n");
	}

	isOk = tc13_DTappsInstallLicense_01();
	if (isOk != true)
	{
		printf("tc13_DTappsInstallLicense_01 failed!\n");
	}

	isOk = tc13_DTappsInstallLicense_02();
	if (isOk != true)
	{
		printf("tc13_DTappsInstallLicense_02 failed!\n");
	}

	isOk = tc13_DTappsInstallLicense_03();
	if (isOk != true)
	{
		printf("tc13_DTappsInstallLicense_03 failed!\n");
	}

	isOk = tc06_DrmFileHandlerPositive_01();
	if (isOk != true)
	{
		printf("tc06_DrmFileHandlerPositive_01 failed!\n");
	}

	isOk = tc06_DrmFileHandlerPositive_02();
	if (isOk != true)
	{
		printf("tc06_DrmFileHandlerPositive_02 failed!\n");
	}

	isOk = tc06_DrmFileHandlerPositive_03();
	if (isOk != true)
	{
		printf("tc06_DrmFileHandlerPositive_03 failed!\n");
	}

	isOk = tc07_DrmFileMgrPositive_01();
	if (isOk != true)
	{
		printf("tc07_DrmFileMgrPositive_01 failed!\b");
	}

	isOk = tc07_DrmFileMgrPositive_02();
	if (isOk != true)
	{
		printf("tc07_DrmFileMgrPositive_02 failed!\n");
	}

	isOk = tc07_DrmFileMgrPositive_03();
	if (isOk != true)
	{
		printf("tc07_DrmFileMgrPositive_03 failed!\n");
	}

	isOk = tc08_DrmFileMgrPositive_GetFileHandler_01();
	if (isOk != true)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_01 failed!\n");
	}

	isOk = tc08_DrmFileMgrPositive_GetFileHandler_02();
	if (isOk != true)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_02 failed!\n");
	}

	isOk = tc08_DrmFileMgrPositive_GetFileHandler_03();
	if (isOk != true)
	{
		printf("tc08_DrmFileMgrPositive_GetFileHandler_03 failed!\n");
	}

	isOk = tc09_DrmFileMgrPositive_FileAPI_01();
	if (isOk != true)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_01 failed!\n");
	}

	isOk = tc09_DrmFileMgrPositive_FileAPI_02();
	if (isOk != true)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_02 failed!\n");
	}

	isOk = tc09_DrmFileMgrPositive_FileAPI_03();
	if (isOk != true)
	{
		printf("tc09_DrmFileMgrPositive_FileAPI_03 failed!\n");
	}

	isOk = tc10_DrmFileMgrPositive_CloseFileHandler_01();
	if (isOk != true)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_01 failed!\n");
	}

	isOk = tc10_DrmFileMgrPositive_CloseFileHandler_02();
	if (isOk != true)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_02 failed!\n");
	}

	isOk = tc10_DrmFileMgrPositive_CloseFileHandler_03();
	if (isOk != true)
	{
		printf("tc10_DrmFileMgrPositive_CloseFileHandler_03 failed!\n");
	}

	isOk = tc11_GetDeviceId_01();
	if (isOk != true)
	{
		printf("tc11_GetDeviceId_01 failed!\n");
	}

	isOk = tc12_TADC_SetDeviceInfo_01();
	if (isOk != true)
	{
		printf("tc12_TADC_SetDeviceInfo_01 failed!\n");
	}

	isOk = tc11_GetDeviceId_01();
	if (isOk != true)
	{
		printf("tc11_GetDeviceId_01 failed!\n");
	}
*/
    isOk = tc_local_01_drm_tizen_full_test_p();
    if (isOk != true)
    {
        printf("tc_local_01_drm_tizen_full_test_p failed!\n");
    }

	//----------------------------------------------------------------------------------------------------
	// 1. Check is drm file
	//----------------------------------------------------------------------------------------------------

	printf("===============================================================================\n");
	printf ("Enter test file name  -->  ");

	isOk = scanf("%s", testFileName); 
	if (isOk < 1)
	{
		printf ("Input value wrong! scanf() failed!!");
		return 0;
	}
	printf("\n1. drm_tizen_is_drm_file  Start  ----------------------------------------\n");

	bRet = drm_tizen_is_drm_file(testFileName, strlen(testFileName));
	if (bRet != TADC_SUCCESS)
	{
		printf("drm_tizen_is_drm_file Error! ret = %d\n", bRet);
		return 0;
	}
	printf("%s file is TADC file!\n", testFileName);

	//----------------------------------------------------------------------------------------------------
	// 2. Make PurchaseRequest Data 
	//----------------------------------------------------------------------------------------------------
	printf("\n2. Make PurchaseRequest Data  Start  ----------------------------------------\n");
	
	memset(ReqBuf, 0x00, sizeof(ReqBuf));
	memset(LicenseUrl, 0x00, sizeof(LicenseUrl));
	ReqBufLen = sizeof(ReqBuf);
	LicenseUrlLen = sizeof(LicenseUrl);
	
	bRet = drm_tizen_generate_purchase_request(testFileName, ReqBuf, &ReqBufLen, LicenseUrl, &LicenseUrlLen);
	if (bRet == false)
	{
		printf(" drm_tizen_generate_purchase_request Error!  \n");
		return 0;
	}

	printf("drm_tizen_generate_purchase_request - ReqBufLen : %d,  License Url : %s   \n", ReqBufLen, LicenseUrl);	
	if ((bRet = _write_logfile("Request_1.dat", ReqBuf, ReqBufLen-1)) == false)
	{
		printf(" drm_tizen_generate_purchase_request _write_logfile Error!  \n");
		return 0;
	}

	//break.... HTTP Request & Reaponse Processing...
	printf ("Enter any key after create Response_1.dat file. -->  ");
	scanf ("%s", tempBuf); 

	memset(RespBuf, 0x00, sizeof(RespBuf));	
	RespBufLen = sizeof(RespBuf);
	if ((bRet = _read_logfile("Response_1.dat", RespBuf, &RespBufLen)) == false)
	{
		printf(" drm_tizen_generate_purchase_request _read_logfile Error!  \n");
		return 0;
	}

	//----------------------------------------------------------------------------------------------------
	// 3. Make License Request Data  
	//----------------------------------------------------------------------------------------------------
	printf("\n3. Make License Request Data  Start  ----------------------------------------\n");
	
	memset(ReqBuf, 0x00, sizeof(ReqBuf));
	memset(LicenseUrl, 0x00, sizeof(LicenseUrl));
	ReqBufLen = sizeof(ReqBuf);
	LicenseUrlLen = sizeof(LicenseUrl);	
	
	bRet = drm_tizen_generate_license_request(RespBuf,  RespBufLen,	ReqBuf, &ReqBufLen, LicenseUrl, &LicenseUrlLen);
	if (bRet != TADC_SUCCESS)
	{
		printf(" drm_tizen_generate_license_request Error!  \n");
		return 0;
	}

	printf("drm_tizen_generate_license_request - ReqBufLen : %d,  RO Url : %s   \n", ReqBufLen, LicenseUrl);
	if ((bRet = _write_logfile("Request_2.dat", ReqBuf, ReqBufLen - 1)) == false)
	{
		printf(" drm_tizen_generate_license_request _write_logfile Error!  \n");
		return 0;
	}

	//break.... HTTP Request & Reaponse Processing...
	printf ("Enter any key after create Response_2.dat file. -->  ");
	scanf ("%s", tempBuf); 

	memset(RespBuf, 0x00, sizeof(RespBuf));	
	RespBufLen = sizeof(RespBuf);
	if ((bRet = _read_logfile("Response_2.dat", RespBuf, &RespBufLen)) == false)
	{
		printf(" drm_tizen_generate_license_request _read_logfile Error! \n");
		return 0;
	}

	//----------------------------------------------------------------------------------------------------
	// 4. Decrypt DRM License
	//----------------------------------------------------------------------------------------------------
	printf("\n4. Decrypt DRM License Start  --------------------------------------------\n");

	bRet = drm_tizen_register_license(RespBuf, RespBufLen);
	if (bRet != TADC_SUCCESS)
	{
		printf(" drm_tizen_register_license  Error!  \n");
		return 0;
	}

	//---------------------------------------------------------------------------------------------------
	// 5. Decrypt DRM File
	//----------------------------------------------------------------------------------------------------
	printf("\n5. Decrypt DRM File Start  -----------------------------------------\n");

	sprintf(tempBuf, "%s.dec", testFileName);
	bRet = drm_tizen_decrypt_package(testFileName, strlen(testFileName), tempBuf, strlen(tempBuf));

	if (bRet != TADC_SUCCESS)
	{
		printf(" drm_tizen_decrypt_package  Error!  \n");
		return 0;
	}

	printf("\n --------------Test Tizen Apps DRM APIs   ---  End ---------------\n");

#endif
}

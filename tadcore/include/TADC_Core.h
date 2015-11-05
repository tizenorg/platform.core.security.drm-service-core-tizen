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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "CPointerArray.h"
#include "CXMLAttribute.h"
#include "CXMLElement.h"
#include "CXMLFile.h"

#define		STACKTRACE(pszFunction)		// RETAILMSG(1, pszFunction);
#undef ERRORMSG
#define		ERRORMSG	//-1
#define		IF_ERROR_RETURN( error , errcode)	if(error < 0){DRM_TAPPS_EXCEPTION("TADC Debug Error Code = %x", errcode); return error;}
#define		IF_TRUE_RETURN( error , errcode)	if(error){DRM_TAPPS_EXCEPTION("TADC Debug Error Code = %x", errcode); return -1;}

//2011.03.08
#define		IF_TRUE_RETURN2( error , do1, do2, do3, errcode)	if(error){ do1; do2; do3; DRM_TAPPS_EXCEPTION("TADC Debug Error Code = %x", errcode); return -1;}

#ifndef TADC_SWAP32
#define TADC_SWAP32(x) 	((((TADC_U32)(x) & 0x000000ffL) << 24) |	(((TADC_U32)(x) & 0x0000ff00L) << 8) | (((TADC_U32)(x) & 0x00ff0000L) >> 8) | (((TADC_U32)(x) & 0xff000000L) >> 24))
#endif

#define	TADC_U8		unsigned char
#define	TADC_U16	unsigned short
#define	TADC_U32	unsigned long
#define	DWORD		unsigned long
#define	LPBYTE		unsigned char*
#define	TCHAR		unsigned char

#define BYTE		unsigned char
#define UINT		unsigned int
#define ULONG		unsigned long
#define INT32		long
#define INT64		long long
#define TRUE		1
#define FALSE		0


#define		REQU_MAXSIZE	1024*5	//2011.03.08 (HTTP Request Buffer Max Size)
#define		RESP_MAXSIZE	1024*10	//2011.03.08 (HTTP Response Buffer Max Size)

#define		ROXML_MAXSIZE	4096*2	//2011.03.08
#define		CERT_MAXSIZE	4096	//2011.03.08

#define		SID_SIZE	4
#define		CID_SIZE	1024		//2011.03.08 ( 32 -> 1024 )
#define		CEK_SIZE	32          // multiplication of 8
#define		GMT_SIZE	21

// Core Value Size define
#define		DUID_SIZE		32
#define		DEVICEKEY_SIZE	16
#define		ROID_SIZE		32
#define		DHKey_SIZE		128		//Max 1024 bit

// DRM Type : Tizen DRM DRM - 1
#define		TIZEN_DRM		1

// RO Permission Type
#define		DUID_RULE		1

typedef char CHAR;
typedef bool BOOL;

typedef struct	t_file_header
{
	unsigned char	Version[2];
	unsigned char	DRMType;
	unsigned char	ContentsType[128];
	long long		TotalSize;
	long long		Offset1;
	long long		Offset2;
	long long		Offset3;
	long long		Offset4;
	long long		Offset5;
	long long		Offset6;
	long long		Offset7;
	long long		Offset8;
}T_FILE_HEADER;

typedef struct	t_drm_header
{
	long			XmlSize;
	unsigned char	Version[2];
	unsigned char	*SID;
	unsigned char	*CID;
	unsigned char	ContentsType[128];
	long			EncryptionMethod;
	long			EncryptionLevel;
	long long		EncryptionRange;
	unsigned char	*RIURL;
	long long		PlaintextSize;
	unsigned char	*Packdate;
	unsigned char	*CEK;
} T_DRM_HEADER;

typedef struct	t_decice_info
{
	unsigned char	DUID[DUID_SIZE + 1];
} T_DEVICE_INFO;

typedef struct	t_dh_info
{
	unsigned char	p[DHKey_SIZE+1];				// DH prime binary
	long			pSize;
	long			g;							// DH primitive root. ( 2 or 5 )
	unsigned char	a[DHKey_SIZE+1];				// DH Client private  binary
	long			aSize;
	unsigned char	A[DHKey_SIZE+1];				// DH Client public binary (ga mod p)
	long			ASize;
	unsigned char	B[DHKey_SIZE+1];				// DH Server public binary (gb mod p)
	long			BSize;
	unsigned char	K[32+1];						// DH Session Key
} T_DH_INFO;

typedef struct t_ro_content
{
	unsigned char	*CID;
	unsigned char	*CEK;
} T_RO_CONTENT;

typedef struct	t_roacq_info
{
	unsigned char*	ROAcqURL;
	unsigned char*	ReqID;
	T_DH_INFO		t_DHInfo;
	unsigned char   sTimeStamp[21];
} T_ROACQ_INFO;

typedef struct	t_individual
{
	unsigned char	BindingType;
	unsigned char*	DUID;
} T_INDIVIDUAL;

typedef struct t_ro_permission
{
	T_INDIVIDUAL t_Individual;
} T_RO_PERMISSION;


typedef struct	t_ro
{
	short	int		PerFlag;
	T_RO_CONTENT	t_Content;
	T_RO_PERMISSION t_Permission;
} T_RO;

int	TADC_SetDeviceInfo(T_DEVICE_INFO *t_DeviceInfo);
int	TADC_GetFileHeader(unsigned char *inBuffer, T_FILE_HEADER *t_FileHeader);
int	TADC_GetDRMHeader(unsigned char *inBuffer, T_DRM_HEADER *t_DRMHeader);
int	TADC_GetDRMHeaderInfo(unsigned char * inBuffer, T_DRM_HEADER *t_DRMHeader);
int	TADC_GetDRMHeaderFromFile(const char *pTADCFilepath, T_FILE_HEADER *t_FileHeader, T_DRM_HEADER *t_DRMHeader);
int	TADC_MakeRequestLicense(T_DEVICE_INFO *t_DeviceInfo, T_FILE_HEADER *t_FileHeader, T_DRM_HEADER *t_DRMHeader, unsigned char * outBuffer, size_t outBufferSize);
int	TADC_GetROAcqInfo(unsigned char *inBuffer, T_ROACQ_INFO *t_ROAcqInfo);
int	TADC_MakeRequestRO(T_ROACQ_INFO *t_ROAcqInfo, unsigned char *outBuffer, size_t outBufferSize, unsigned char *ROVer=NULL); //2011.03.08
int	TADC_GetResponseRO(unsigned char *inBuffer, T_ROACQ_INFO *t_ROAcqInfo, T_RO *t_RO, unsigned char *outBuffer);
int TADC_GetResponseROInfo(LPBYTE pszXML,  T_RO *t_RO);
int	TADC_GetHashReqID(unsigned char *inBuffer, unsigned char *hashReqID);
int	TADC_GetCEK(T_DEVICE_INFO *t_DeviceInfo, T_RO *t_RODB, T_DRM_HEADER *t_DRMHeader );
int TADC_DecryptBlock(char* pbBuffer, int nSize, T_DRM_HEADER *t_DRMHeader);
int	TADC_VerifyROSignature(IN LPBYTE pszXML);
DWORD TADC_GetLastError(void);
int TADC_MEMFree_RO(T_RO *t_ro);
int TADC_MEMFree_FileHeader(T_FILE_HEADER *t_FileHeader);
int TADC_MEMFree_DRMHeader(T_DRM_HEADER *t_DrmHeader);
int TADC_MEMFree_ROAcqInfo(T_ROACQ_INFO *t_ROAcqInfo);

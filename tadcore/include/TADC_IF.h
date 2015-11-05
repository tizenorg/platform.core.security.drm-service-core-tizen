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
#include <stdbool.h>

#include "TADC_Sub.h"
#include "drm-tizen-mid.h"

#define RO_ISSUER_ROOT_CERTS_DIR ISSUER_ROOT_CERTS_DIR

int TADC_IF_GetDUID(char *DUID);

int TADC_IF_GetDHKey(T_DH_INFO *t_dhinfo);
int TADC_IF_GetDHKey_K(T_DH_INFO *t_dhinfo);

int TADC_IF_AES_CTR(int keyLen, unsigned char *pKey, int ivLen, unsigned char *pIV, int inLen, unsigned char *in, int *pOutLen, unsigned char *out);
int TADC_IF_SHA1(unsigned char *in, int inLen, unsigned char *out);

size_t TADC_IF_StrLen(const char *string);
int TADC_IF_StrCmp(const char *string1, const char *string2);
int TADC_IF_StrNCmp(const char *string1, const char *string2, size_t count);
char *TADC_IF_StrNCpy(char *strDestination, const char *strSource, size_t count);

int TADC_IF_MemCmp(const void *buf1, const void *buf2, size_t count);
void TADC_IF_MemCpy(void *dest, const void *src, size_t count);
void TADC_IF_MemSet(void *dest, int c, size_t count);

void *TADC_IF_Malloc(size_t size);
void TADC_IF_Free(void *memblock);

int TADC_IF_AtoI(char *str);

void TADC_IF_Debug(const char *str);

int TADC_IF_VerifySignature( unsigned char* inData, int inLen,
							 unsigned char* sigData, int sigLen,
							 unsigned char* cert, int certLen );

int TADC_IF_VerifyCertChain( unsigned char* rica, int ricaLen,
							 unsigned char* cert, int certLen );

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
 * @file	TadcTypes.h
 * @brief	This file includes definitions of constants,
 *			enumerations, and datastructures for the DRM service.
 * @version	1.0
 */

#ifndef __TADC_DRM_TYPES_H__
#define __TADC_DRM_TYPES_H__

#include <stdio.h>
#include <stdarg.h>

#define _T
#define		IF_ERROR_GOTO( error )	if( FAILED( HRESULT_FROM_WIN32( error ) ) ) { nResult = ERRORMSG( error, NULL ); goto finish; }
#define		IF_TRUE_GOTO( cond, error )	if( cond ) { nResult = -1; goto finish; }

#ifndef _SHP_SIMUL
typedef void* LPVOID;
typedef const char* LPCTSTR;
typedef unsigned char* LPBYTE;
typedef char TCHAR;
typedef char* LPTSTR;
typedef LPTSTR LPSTR;
typedef LPCTSTR LPCTSTR;
typedef LPCTSTR LPCSTR;

//typedef HFile HANDLE;
#define ERROR_INVALID_DATA               13L
#define ERROR_INSUFFICIENT_BUFFER        122L    // dderror
//#define CopyMemory AcMemcpy
#define INVALID_HOBJ	NULL
#endif

#endif	//__TADC_DRM_TYPES_H__

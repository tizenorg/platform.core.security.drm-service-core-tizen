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

#ifndef __CPOINTERARRAY_H__
#define __CPOINTERARRAY_H__


#if !defined(AFX_CPOINTERARRAY_H__AB67E4BE_A233_4E3E_B257_9830D90326EE__INCLUDED_)
#define AFX_CPOINTERARRAY_H__AB67E4BE_A233_4E3E_B257_9830D90326EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TadcTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

class CPointerArray
{
protected:
	LPVOID*						m_ppData;
	int							m_nMaxSize;
	int							m_nNumOfData;

public:
	CPointerArray();
	virtual ~CPointerArray();

	int 					Add( LPVOID pData );
	int 					Remove( int nIndex );
	LPVOID 				Get( int nIndex );
	inline int 			GetCount() { return m_nNumOfData; }
	inline void 			RemoveAll() { m_nNumOfData = 0; }
};

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // !defined(AFX_CPOINTERARRAY_H__AB67E4BE_A233_4E3E_B257_9830D90326EE__INCLUDED_)
#endif /* __CPOINTERARRAY_H__ */

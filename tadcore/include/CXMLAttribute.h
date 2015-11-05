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

#include "TadcTypes.h"

#if !defined(AFX_CXMLATTRIBUTE_H__2B925786_3613_47B0_B85D_CAF2053F46DB__INCLUDED_)
#define AFX_CXMLATTRIBUTE_H__2B925786_3613_47B0_B85D_CAF2053F46DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLAttribute
{
protected:
	LPTSTR							m_pszName;
	LPTSTR							m_pszValue;

public:
	CXMLAttribute();
	virtual ~CXMLAttribute();

	int 						SetName( LPCTSTR pszName );
	int 						SetValue( LPCTSTR pszValue );

	inline LPCTSTR 			GetName() { return m_pszName; }
	inline LPCTSTR 			GetValue() { return m_pszValue; }
};

#endif // !defined(AFX_CXMLATTRIBUTE_H__2B925786_3613_47B0_B85D_CAF2053F46DB__INCLUDED_)

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

#if !defined(AFX_CXMLELEMENT_H__B6A6A39B_1980_4A4F_B68B_E87B53A3EE9B__INCLUDED_)
#define AFX_CXMLELEMENT_H__B6A6A39B_1980_4A4F_B68B_E87B53A3EE9B__INCLUDED_

#include "TadcTypes.h"
#include "CXMLAttribute.h"
#include "CPointerArray.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLElement  
{
protected:
	LPTSTR					m_pszName;
	LPTSTR					m_pszValue;
	
	CPointerArray			m_pChilds;
	CPointerArray			m_pAttributes;
	
	int 					_SearchNodes(  CPointerArray* ppaChildNodes,  CXMLElement* pCurrent,  LPCTSTR pszTagName,  va_list args );

public:
	CXMLElement();
	virtual ~CXMLElement();

	int 					SetName(  LPCTSTR pszName );
	int 					SetValue(  LPCTSTR pszValue );
	int 					AddAttribute(  LPCTSTR pszName,  LPCTSTR pszValue );

	inline LPCTSTR 			GetName() { return m_pszName; }
	inline LPCTSTR 			GetValue() { return m_pszValue; }

	int 					AddChild(  CXMLElement* pChild );
	LPCTSTR 				GetAttribute(  LPCTSTR pszName );

	inline int 				GetChildCount() { return m_pChilds.GetCount(); }
	inline int 				GetAttributeCount() { return m_pAttributes.GetCount(); }
	inline CXMLElement* 	GetChild(  int nIndex ) { return (CXMLElement*)m_pChilds.Get( nIndex ); }
	inline CXMLAttribute* 	GetAttribute(  int nIndex ) { return (CXMLAttribute*)m_pAttributes.Get( nIndex ); }

	int						Find(  CPointerArray* pSearchedChild,  LPCTSTR pszChildName, ... );
};

#endif // !defined(AFX_CXMLELEMENT_H__B6A6A39B_1980_4A4F_B68B_E87B53A3EE9B__INCLUDED_)

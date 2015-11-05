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

#if !defined(AFX_CXMLFILE_H__21F76587_B9C8_4407_9C16_186F3D47ADE1__INCLUDED_)
#define AFX_CXMLFILE_H__21F76587_B9C8_4407_9C16_186F3D47ADE1__INCLUDED_

#include "TadcTypes.h"
#include "CXMLElement.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

#define ELEMENT_QUEUE_MAX	2048

class CXMLFile  
{
protected:
	LPCTSTR					m_pszXML;
	CXMLElement*			m_paElementQueue[ ELEMENT_QUEUE_MAX ];
	int						m_nQueueIndex;
	CXMLElement*			m_pRoot;
	
	int 					_Parse();
	int 					_GetElementName(  LPTSTR pszElementName );
	CXMLElement* 			_Pop();
	int 					_Push( CXMLElement* p );
	int 					_GetAttributeNameAndValue(  LPTSTR pszName,  LPTSTR pszValue );

public:
	CXMLFile();
	virtual ~CXMLFile();

	int 					LoadFromStream(  LPCTSTR pszXML );
	int 					LoadFromFile(  LPCTSTR pszFileName );
	inline CXMLElement* 	GetRoot() { return m_pRoot; }
};

#endif // !defined(AFX_CXMLFILE_H__21F76587_B9C8_4407_9C16_186F3D47ADE1__INCLUDED_)

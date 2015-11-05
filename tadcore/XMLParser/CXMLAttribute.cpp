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

#include "CXMLAttribute.h"
#include "TADC_IF.h"
#include "TADC_ErrorCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLAttribute::CXMLAttribute()
	: m_pszName(NULL)
	, m_pszValue(NULL)
{}

CXMLAttribute::~CXMLAttribute()
{
	if (m_pszName)
		delete []m_pszName;

	if (m_pszValue)
		delete []m_pszValue;
}

// [in] pszName : should be null-terminated string
int CXMLAttribute::SetName(LPCTSTR pszName)
{
	int nResult = 0;
	size_t pszNameLen = 0;

	if (m_pszName) {
		delete []m_pszName;
		m_pszName = NULL;
	}

	pszNameLen = strlen(pszName);

	m_pszName = new CHAR[pszNameLen + 1];
	IF_TRUE_GOTO(m_pszName == NULL, ERROR_NOT_ENOUGH_MEMORY);

	memcpy(m_pszName, pszName, pszNameLen + 1);

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLAttribute::SetName() Error!");

	return nResult;
}

// [in] pszValue : should be null-terminated string
int CXMLAttribute::SetValue(LPCTSTR pszValue)
{
	int nResult = 0;
	size_t pszValueLen = 0;

	if (m_pszValue) {
		delete []m_pszValue;
		m_pszValue = NULL;
	}

	pszValueLen = strlen(pszValue);

	m_pszValue = new CHAR[pszValueLen + 1];
	IF_TRUE_GOTO(m_pszName == NULL, ERROR_NOT_ENOUGH_MEMORY);

	memcpy(m_pszValue, pszValue, pszValueLen + 1);

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLAttribute::SetValue() Error!");

	return nResult;
}

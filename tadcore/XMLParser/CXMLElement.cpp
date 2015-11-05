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

#include "CXMLElement.h"

#include "TADC_IF.h"
#include "TADC_ErrorCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLElement::CXMLElement()
	: m_pszName(NULL)
	, m_pszValue(NULL)
{}

CXMLElement::~CXMLElement()
{
	if (m_pszName)
		delete[] m_pszName;

	if (m_pszValue)
		delete[] m_pszValue;

	for (int i = 0; i < m_pAttributes.GetCount(); i++)
		delete (CXMLAttribute *)m_pAttributes.Get(i);

	for (int i = 0; i < m_pChilds.GetCount(); i++)
		delete (CXMLElement *)m_pChilds.Get(i);
}

int CXMLElement::SetName(LPCTSTR pszName)
{
	int nResult = 0;
	size_t pszNameLen = 0;

	IF_TRUE_GOTO(pszName == NULL, TADC_PARAMETER_ERROR);
	pszNameLen = strlen(pszName);

	m_pszName = new CHAR[pszNameLen + 1];
	IF_TRUE_GOTO(m_pszName == NULL, TADC_MEMAlOC_ERROR);

	memcpy(m_pszName, pszName, pszNameLen + 1);

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLElement::SetName() Error!");

	return nResult;
}

int CXMLElement::SetValue(LPCTSTR pszValue)
{
	int nResult = 0;
	size_t pszValueLen = 0;

	IF_TRUE_GOTO(pszValue == NULL, TADC_PARAMETER_ERROR);
	pszValueLen = strlen(pszValue);

	m_pszValue = new CHAR[pszValueLen + 1];
	IF_TRUE_GOTO(m_pszValue == NULL, TADC_MEMAlOC_ERROR);

	memcpy(m_pszValue, pszValue, pszValueLen + 1);

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLElement::SetValue() Error!");
	
	return nResult;
}

int CXMLElement::AddAttribute(LPCTSTR pszName, LPCTSTR pszValue)
{
	int nResult = 0;
	CXMLAttribute *pAttribute = NULL;

	pAttribute = new CXMLAttribute;
	IF_TRUE_GOTO(pAttribute == NULL, TADC_MEMAlOC_ERROR);

	pAttribute->SetName(pszName);
	pAttribute->SetValue(pszValue);

	m_pAttributes.Add(pAttribute);

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLElement::AddAttribute() Error!");

	return nResult;
}

int CXMLElement::AddChild(CXMLElement *pChild)
{
	int nResult = 0;

	m_pChilds.Add(pChild);

	return nResult;
}

int CXMLElement::Find(CPointerArray *pSearchedChild, LPCTSTR pszChildName, ...)
{
	int nResult = 0;;
	va_list args;

	va_start(args, pszChildName);

	nResult = _SearchNodes(pSearchedChild, this, pszChildName, args);

	if (nResult)
		goto finish;

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLElement::Find() Error!");

	va_end(args);
	return nResult;
}

int CXMLElement::_SearchNodes(CPointerArray* ppaChildNodes, CXMLElement* pCurrent, LPCTSTR pszTagName, va_list args)
{
	int nResult = 0;

	CXMLElement *pChild = NULL;
	LPCTSTR pszName = NULL;
	LPCTSTR pszNextName = NULL;

	IF_TRUE_GOTO((!ppaChildNodes || !pCurrent || !pszTagName), TADC_MEMAlOC_ERROR);

	pszNextName = va_arg(args, LPCTSTR);

	for (int i = 0; i < pCurrent->GetChildCount(); i++) {
		pChild = pCurrent->GetChild(i);

		if (pChild)
			pszName = pChild->GetName();

		if (pszName && strcasecmp(pszName, pszTagName) == 0) {
			if (!pszNextName)
				ppaChildNodes->Add(pChild);
			else
				nResult = _SearchNodes(ppaChildNodes, pChild, pszNextName, args);
		}
	}
	
	nResult = 0;

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLElement::_SearchNodes() Error!");

	return nResult;
}

LPCTSTR CXMLElement::GetAttribute(LPCTSTR pszName)
{
	CXMLAttribute *pAttribute;
	LPCTSTR pszValue = NULL;

	for (int i = 0; i < m_pAttributes.GetCount(); i++) {
		pAttribute = (CXMLAttribute *)m_pAttributes.Get(i);

		if (pAttribute && TADC_IF_StrCmp(pAttribute->GetName(), pszName) == 0) {
			pszValue = pAttribute->GetValue();
			break;
		}
	}

	return pszValue;
}

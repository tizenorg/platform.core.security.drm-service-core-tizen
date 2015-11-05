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

#include "CXMLFile.h"
#include "TADC_IF.h"
#include "TADC_ErrorCode.h"

#define _tcsstr strstr
#define _tcschr strchr

#define _MAX_NAME_LENGTH  4096
#define _MAX_VALUE_LENGTH 4096


#define		_SKIP_WHITESPACE()			while(	( *m_pszXML == ' ' ) || \
												( *m_pszXML == '\n' ) || \
												( *m_pszXML == '\r' ) || \
												( *m_pszXML == 0x09 ) ) m_pszXML++

typedef struct _tagESCAPE_CHARSET
{
	LPCTSTR			pszEscape;
	TCHAR			chReplace;
} T_ESCAPE_CHARSET;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLFile::CXMLFile()
{
	m_nQueueIndex = 0;
	m_pRoot = NULL;

	for (int i = 0; i < ELEMENT_QUEUE_MAX; ++i)
		m_paElementQueue[i] = NULL;
	
	m_pszXML = NULL;
}

CXMLFile::~CXMLFile()
{
	if (m_pRoot)
		delete m_pRoot;
}

int CXMLFile::LoadFromStream(LPCTSTR pszXML)
{
	int nResult = 0;

	m_nQueueIndex = 0;

	if (m_pRoot)
		delete m_pRoot;

	m_pRoot = NULL;
	m_pszXML = pszXML;

	nResult = _Parse();

	if (nResult)
		goto finish;

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLFile::LoadFromStream() Error! \n");

	return nResult;
}

int CXMLFile::LoadFromFile(LPCTSTR pszFileName)
{
	int					nResult;
	FILE*				hFile = NULL;
	DWORD				dwFileSize, dwReadBytes;
	LPBYTE				pbBuffer = NULL;
	LPTSTR				pszXML = NULL;

	hFile = fopen(pszFileName, "rb");

	if (!hFile) {
		nResult = -1;
		goto finish;
	}

	fseek(hFile, 0, SEEK_END);
	dwFileSize = ftell(hFile);
	fseek(hFile, 0, SEEK_SET);

	pbBuffer = new BYTE[ dwFileSize + 1 ];
	IF_TRUE_GOTO( pbBuffer == NULL, TADC_MEMAlOC_ERROR );

	dwReadBytes = fread ( pbBuffer, 1, dwFileSize + 1, hFile);

	IF_TRUE_GOTO( dwFileSize != dwReadBytes, TADC_PARAMETER_ERROR );

	pbBuffer[ dwFileSize ] = 0;

	pszXML = new CHAR[ dwFileSize + 256 ];
	IF_TRUE_GOTO( pszXML == NULL, TADC_MEMAlOC_ERROR );

#ifdef _UNICODE
	nResult = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pbBuffer, -1, pszXML, dwFileSize + 256 );
#else
	memcpy(pszXML, pbBuffer, dwFileSize + 1);
#endif

	nResult = LoadFromStream(pszXML);

	if (nResult)
		goto finish;

	nResult = 0;

finish:
	if (hFile)
		fclose(hFile);
	
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLFile::LoadFromFile() Error! \n");

	if (pbBuffer)
		delete []pbBuffer;

	if (pszXML)
		delete []pszXML;

	return nResult;
}

int CXMLFile::_Parse()
{
	int					nResult, i;
	CXMLElement*		pXMLElement = NULL;
	CXMLElement*		pCurrentElement = NULL;
	CXMLElement*		pTemp = NULL;
	TCHAR				szElementName[_MAX_NAME_LENGTH];
	TCHAR				szValue[_MAX_VALUE_LENGTH];
	LPCTSTR				psz;
	size_t				orig_pszXML_len = strlen(m_pszXML);
	size_t				tmp_pszXML_len = orig_pszXML_len;

	_SKIP_WHITESPACE();

	while (*m_pszXML != 0) {
		_SKIP_WHITESPACE();

		if (*m_pszXML == 0)
			break;

		if (*m_pszXML == '<') {
			m_pszXML++;
			
			_SKIP_WHITESPACE();

			if (*m_pszXML == '!') {
				//------------------------------------------------------------------------
				//
				// Comment
				//
				//------------------------------------------------------------------------
			}
			else if (*m_pszXML == '?') {
				//------------------------------------------------------------------------
				//
				// <? ... ?>
				//
				//------------------------------------------------------------------------
				psz = _tcsstr(m_pszXML, _T( "?>" ));
				if (!psz) {
					nResult = -1;
					goto finish;
				}
				
				m_pszXML = psz + 2;
			}
			else if (*m_pszXML == '/') {
				m_pszXML++;
				//------------------------------------------------------------------------
				//
				// End Tag
				//
				//------------------------------------------------------------------------
				nResult = _GetElementName((LPTSTR)szElementName);

				if (nResult != 0)
					goto finish;
				
				//2011.04.27
				if (!pCurrentElement) {
					nResult = -1;
					goto finish;
				}
				
				if (TADC_IF_StrCmp(pCurrentElement->GetName(), (LPTSTR)szElementName) != 0) {
					nResult = -1;
					goto finish;
				}

				tmp_pszXML_len = orig_pszXML_len;
				while (*m_pszXML != '>' && tmp_pszXML_len > 0) {
					tmp_pszXML_len--;
					m_pszXML++;
				}

				m_pszXML++;

				pTemp = _Pop();

				if (pTemp)
					pCurrentElement = pTemp;
				else
					pCurrentElement = m_pRoot;
			}
			else {
				pXMLElement = new CXMLElement;			
				IF_TRUE_GOTO(pXMLElement == NULL, TADC_MEMAlOC_ERROR);
				
				nResult = _GetElementName((LPTSTR)szElementName);
				if (nResult)
					goto finish;

				pXMLElement->SetName((LPTSTR)szElementName);

				if (!m_pRoot)
					m_pRoot = pXMLElement;
				
				if (pCurrentElement) {
					pCurrentElement->AddChild(pXMLElement);
					_Push(pCurrentElement);
				}

				pCurrentElement = pXMLElement;
			}
		}
		else if (*m_pszXML == '/') {
			m_pszXML++;

			if (*m_pszXML != '>') {
				nResult = -1;
				goto finish;
			}

			pCurrentElement = _Pop();
		}
		else if (*m_pszXML == '>') {
			m_pszXML++;
			_SKIP_WHITESPACE();

			i = 0;

			tmp_pszXML_len = orig_pszXML_len;
			while (*m_pszXML != '<' && tmp_pszXML_len > 0) {
				tmp_pszXML_len--;
				szValue[i] = *m_pszXML;
				i++;
				m_pszXML++;
			}

			szValue[i] = 0;
			if (pCurrentElement)
				pCurrentElement->SetValue((LPTSTR)szValue);
		}
		else {
			_SKIP_WHITESPACE();

			nResult = _GetAttributeNameAndValue((LPTSTR)szElementName, (LPTSTR)szValue);

			if (nResult)
				goto finish;

			if (pCurrentElement)
				pCurrentElement->AddAttribute((LPTSTR)szElementName, (LPTSTR)szValue);
		}
	}

	nResult = 0;

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLFile::_Parse() Error!. nResult[%d]", nResult);

	return nResult;
}

int CXMLFile::_GetElementName(LPTSTR pszElementName)
{
	_SKIP_WHITESPACE();

	while ((*m_pszXML != 0 )
		&& (*m_pszXML != ' ')
		&& (*m_pszXML != '>')
		&& (*m_pszXML != '/')) {
		*pszElementName = *m_pszXML;
		m_pszXML++;
		pszElementName++;
	}
	
	*pszElementName = 0;

	return 0;
}

CXMLElement* CXMLFile::_Pop()
{
	if (m_nQueueIndex < 1)
		return NULL;

	return m_paElementQueue[--m_nQueueIndex];
}

int CXMLFile::_Push(IN CXMLElement* p)
{
	if (m_nQueueIndex >= (int)sizeof(m_paElementQueue) / (int)sizeof(m_paElementQueue[0]))
		return ERROR_INSUFFICIENT_BUFFER;

	m_paElementQueue[m_nQueueIndex++] = p;

	return 0;
}

int CXMLFile::_GetAttributeNameAndValue(LPTSTR pszName, LPTSTR pszValue)
{
	int							nResult, nLength, i;
	LPCTSTR						psz;
	TCHAR						chQuotation;
	bool						isFound;
    LPTSTR                      origValueAddr = pszValue;
	static T_ESCAPE_CHARSET		stEscape[] = {	{ _T( "&amp" ), _T( '&' ) },
												{ _T( "&qt" ), _T( '>' ) },
												{ _T( "&lt" ), _T( '<' ) },
												{ _T( "&quot" ), _T( '"' ) },
												{ _T( "&apos" ), _T( '\'' ) } };

	_SKIP_WHITESPACE();

	psz = _tcschr( m_pszXML, '=' );
	if (!psz) {
		nResult = -1;//ERRORMSG( ERROR_SXS_XML_E_BADXMLDECL, NULL );
		goto finish;
	}

	nLength = (int)(psz - m_pszXML);
    if( _MAX_NAME_LENGTH < nLength + 1) {
        nResult = -1;
        goto finish;
    }
	memcpy(pszName, m_pszXML, nLength + 1);

	m_pszXML = psz + 1;

	_SKIP_WHITESPACE();

	if (( *m_pszXML == '\'' ) || ( *m_pszXML == '"' )) {
		chQuotation = *m_pszXML;
		m_pszXML++;
	}
	else {
		chQuotation = ' ';
	}

	while (*m_pszXML != 0 && ((pszValue - origValueAddr)< _MAX_VALUE_LENGTH) ) {
		if (*m_pszXML == '&') {
			isFound = FALSE;
			for (i = 0; i < (int)sizeof(stEscape) / (int)sizeof(stEscape[0]); i++) {
				if (TADC_IF_StrNCmp(m_pszXML, stEscape[i].pszEscape, TADC_IF_StrLen(stEscape[i].pszEscape)) == 0) {
					*pszValue = stEscape[i].chReplace;
					pszValue++;
					m_pszXML += TADC_IF_StrLen(stEscape[i].pszEscape);
					isFound = TRUE;

					break;
				}
			}

			if (isFound == FALSE) {
				*pszValue = *m_pszXML;
				pszValue++;
				m_pszXML++;
			}
		}
		else if (*m_pszXML == chQuotation) {
			m_pszXML++;
			break;
		}
		else if (*m_pszXML == '/' || *m_pszXML == '>')
		{
			if (chQuotation == ' ')
				break;

			*pszValue = *m_pszXML;
			pszValue++;
			m_pszXML++;
		}
		else
		{
			*pszValue = *m_pszXML;
			pszValue++;
			m_pszXML++;
		}
	}

	*pszValue = 0;
	nResult = 0;

finish:
	if (nResult)
		DRM_TAPPS_EXCEPTION("CXMLFile::_GetAttributeNameAndValue() Error!");

	return nResult;
}

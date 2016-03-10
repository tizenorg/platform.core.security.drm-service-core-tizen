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

#include "TADC_Core.h"
#include "TADC_Util.h"
#include "TADC_Sub.h"
#include "TADC_IF.h"
#include "TADC_ErrorCode.h"

// -------------------------- Base64 --------------------------------
static CHAR __base64_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static BYTE __reverse_table[256] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static CHAR __base64_pad = '=';

LPSTR Base64Encode( LPBYTE pbData,  int nLength )
{
	int					i = 0, result = 0;
	LPSTR				pszResult = NULL;

	if (nLength <= 0)
	{
		return NULL;
	}
	result = ( ( nLength + 3 - nLength % 3 ) * 4 / 3 + 1 );

	pszResult = (LPSTR)TADC_IF_Malloc( result );

	if (pszResult == NULL)
	{
		return NULL;
	}

	TADC_IF_MemSet(pszResult, 0x00, result);

	i = 0;

	while (nLength > 2)
	{
		/* keep going until we have less than 24 bits */
		pszResult[ i++ ] = __base64_table[ pbData[ 0 ] >> 2 ];
		pszResult[ i++ ] = __base64_table[ ( ( pbData[ 0 ] & 0x03 ) << 4 ) + ( pbData[ 1 ] >> 4 ) ];
		pszResult[ i++ ] = __base64_table[ ( ( pbData[ 1 ] & 0x0f ) << 2 ) + ( pbData[ 2 ] >> 6 ) ];
		pszResult[ i++ ] = __base64_table[ pbData[ 2 ] & 0x3f ];

		pbData += 3;
		nLength -= 3; /* we just handle 3 octets of data */
	}

	/* now deal with the tail end of things */
	if (nLength != 0)
	{
		pszResult[ i++ ] = __base64_table[ pbData[ 0 ] >> 2 ];
		if (nLength == 1)
		{
			pszResult[ i++ ] = __base64_table[ ( pbData[ 0 ] & 0x03 ) << 4 ];
			pszResult[ i++ ] = __base64_pad;
			pszResult[ i++ ] = __base64_pad;
		}
		else
		{
			pszResult[ i++ ] = __base64_table[ ( ( pbData[ 0 ] & 0x03 ) << 4 ) + ( pbData[ 1 ] >> 4 ) ];
			pszResult[ i++ ] = __base64_table[ ( pbData[ 1 ] & 0x0f ) << 2 ];
			pszResult[ i++ ] = __base64_pad;
		}
	}
	pszResult[ i ] = 0;
	return pszResult;
}

LPBYTE Base64Decode(LPCSTR pszString, int* pnLength)
{
	size_t nStrLength = TADC_IF_StrLen(pszString);
	if (nStrLength % 4 != 0)
		return NULL;

	LPBYTE pbResult = (LPBYTE)TADC_IF_Malloc(nStrLength + 1);
	if (pbResult == NULL)
		 return NULL;

	TADC_IF_MemSet(pbResult, 0x00, nStrLength + 1);

	int nOutputLength = 0;

	for (size_t i = 0; i < nStrLength; i += 4) {
		BYTE b1 = __reverse_table[static_cast<BYTE>(pszString[i])];
		BYTE b2 = __reverse_table[static_cast<BYTE>(pszString[i + 1])];
		BYTE b3 = __reverse_table[static_cast<BYTE>(pszString[i + 2])];
		BYTE b4 = __reverse_table[static_cast<BYTE>(pszString[i + 3])];

		pbResult[nOutputLength++] = (b1 << 2) | (b2 >> 4);

		if (pszString[i + 2] == '=') {
			pbResult[nOutputLength] = (b2 & 0x0F) << 4;
		} else {
			pbResult[nOutputLength++] = ((b2 & 0x0F) << 4) | (b3 >> 2);

			if (pszString[i + 3] == '=')
				pbResult[nOutputLength] = (b3 & 0x03) << 6;
			else
				pbResult[nOutputLength++] = ((b3 & 0x03) << 6) | b4;
		}
	}

	*pnLength = nOutputLength;

	return pbResult;
}
// -------------------------- Base64 --------------------------------]]]]

int HEX2BIN(LPCSTR pszHex, LPBYTE baBin, int* pnLength )
{
	CHAR		szTemp[ 3 ];
	CHAR		szHex[ 1024 ];
	int			i = 0, nLength = 0;

	nLength = TADC_IF_StrLen(pszHex);

	if (nLength <= 0)
	{
		return -1;
	}

	if ((nLength % 2) == 0)
	{
		TADC_IF_StrNCpy(szHex, pszHex, nLength);
	}
	else
	{
		szHex[ 0 ] = '0';
		TADC_IF_StrNCpy(&szHex[ 1 ], pszHex, nLength);
		nLength += 1;
	}

	*pnLength = nLength / 2;
	szTemp[ 2 ] = 0;

	for (i = 0 ; i < *pnLength ; i++ )
	{
		szTemp[ 0 ] = szHex[ i * 2 ];
		szTemp[ 1 ] = szHex[ i * 2 + 1 ];
		baBin[ i ] = (BYTE)strtoul( szTemp, NULL, 16 );
	}
	return 0;
}

//Find String (2011.03.08)
//return : start position of find string or error (-1)
int FindString(unsigned char *in, int inLen, unsigned char *find, int findLen)
{
	int i = 0;

	for (i = 0 ; i <= inLen - findLen ; i++)
	{
		if (!TADC_IF_MemCmp(in + i, find, findLen))
		return i;
	}
	return -1;
}

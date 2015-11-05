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
 
static int __reverse_table[] = { 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, 
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, 
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
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
	int			result = 0; 
	LPBYTE		pbResult = NULL; 
	int			nStrLength = 0, i = 0, nOutputLength = 0; 
	BYTE		b1, b2, b3, b4; 
 
	nStrLength = TADC_IF_StrLen(pszString);
	if (nStrLength % 4 != 0) 
	{ 
		result = -1; 
		goto finish; 
	} 
 
	pbResult = (LPBYTE)TADC_IF_Malloc(nStrLength + 1);
	if (pbResult == NULL)
	{
		 return NULL;
	}
	TADC_IF_MemSet(pbResult, 0x00, nStrLength + 1);
	 
	nOutputLength = 0; 

	for (i = 0 ; i < nStrLength ; i += 4) 
	{ 
		b1 = (BYTE)__reverse_table[ pszString[ i ] ]; 
		b2 = (BYTE)__reverse_table[ pszString[ i + 1 ] ]; 
		b3 = (BYTE)__reverse_table[ pszString[ i + 2 ] ]; 
		b4 = (BYTE)__reverse_table[ pszString[ i + 3 ] ]; 
 
		pbResult[ nOutputLength++ ] = (BYTE)( ( b1 << 2 ) | ( b2 >> 4 ) ); 
		 
		if (pszString[ i + 2 ] == '=') 
		{ 
			pbResult[ nOutputLength ] = (BYTE)( ( b2 & 0x0F ) << 4 ); 
		} 
		else 
		{ 
			pbResult[ nOutputLength++ ] = (BYTE)( ( ( b2 & 0x0F ) << 4 ) | ( b3 >> 2 ) ); 

			if (pszString[ i + 3 ] == '=') 
			{ 
				pbResult[ nOutputLength ] = (BYTE)( ( b3 & 0x03 ) << 6 ); 
			} 
			else 
			{ 
				pbResult[ nOutputLength++ ] = (BYTE)( ( ( b3 & 0x03 ) << 6 ) | b4 ); 
			} 
		} 
	} 
	*pnLength = nOutputLength; 
	result = 0; 
 
finish: 
	if (result != 0) 
	{ 
		TADC_IF_Free( pbResult );
	} 
 
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

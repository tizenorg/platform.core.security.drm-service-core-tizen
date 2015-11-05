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

#include "TADC_Sub.h"

unsigned int htons_(unsigned int hostshort)
{
	BYTE	*pBuffer;
	unsigned int	nResult;

	nResult = 0;
	pBuffer = (LPBYTE)&hostshort;
	nResult	= (((pBuffer[0] << 8) & 0xFF00) | (pBuffer[1] & 0x00FF));

	return nResult;
}

DWORD htonl_(DWORD hostlong)
{
	DWORD   nResult	= hostlong >> 16;
	unsigned int	upper	= (unsigned int)nResult & 0x0000FFFF;
	unsigned int	lower	= (unsigned int)hostlong & 0x0000FFFF;

	upper	= htons_(upper);
	lower	= htons_(lower);
    nResult	= 0x10000 * lower + upper;

	return nResult;
}

INT64 _hton64( INT64 n64host )
{
	INT64   result;
	LPBYTE   p;
	short   n64Test;
	int	 n32High, n32Low;

	static int  isLittleEndian = -1;

	if( isLittleEndian == -1 )
	{
		n64Test = 0x01;
		p = (LPBYTE)&n64Test;

		if( p[ 0 ] == 0x01 )
		{
			isLittleEndian = 1;
		}
		else
		{
			isLittleEndian = 0;
		}
	}

	if( isLittleEndian == 1 )
	{
		n32Low = (INT32)( n64host & 0xFFFFFFFF );
		n32High = (INT32)( n64host >> 32 );

		n32Low = htonl_( n32Low );
		n32High = htonl_( n32High );

		result = ( (INT64)n32Low << 32 ) | n32High;
		return result;
	}
	else
	{
		return n64host;
	}
}

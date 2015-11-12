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

/** 
 * @file	DTapps2Base64.cpp
 * @brief	This file includes functions relating to Base64 Encode Decode.
 */

#include "DTapps2Base64.h"

const unsigned char MAP[128] = 
{
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
};

const char CODE[64] = 
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/' 
};

BOOL DTappsB64Encode(unsigned char* in, int in_size, unsigned char* out, int out_size)
{
	int i, j;
	unsigned char a, b, c;
	int len = (in_size + 2) / 3 * 4;
 
	if( out_size < len )
	   return FALSE;
	
	for( i = 0, j = 0; j < len; i += 3, j += 4 )
	{
	   a = b = c = 0;
	   a = *(in+i);
	   *(out+j) = CODE[(a >> 2) & 0x3F];
	   if( in_size - i > 2 )
	   {
		  b = *(in+i+1);
		  c = *(in+i+2);
		  *(out+j+1) = CODE[((a << 4) & 0x30) + ((b >> 4) & 0xf)];
		  *(out+j+2) = CODE[((b << 2) & 0x3c) + ((c >> 6) & 0x3)];
		  *(out+j+3) = CODE[c & 0x3F];
	   }
	   else if( in_size - i  > 1 )
	   {
		  b = *(in+i+1);
		  *(out+j+1) = CODE[((a << 4) & 0x30) + ((b >> 4) & 0xf)];
		  *(out+j+2) = CODE[((b << 2) & 0x3c) + ((c >> 6) & 0x3)];
		  *(out+j+3) = '=';
	   }
	   else
	   {
		  *(out+j+1) = CODE[((a << 4) & 0x30) + ((b >> 4) & 0xf)];
		  *(out+j+2) = '=';
		  *(out+j+3) = '=';
	   }
	}

	return TRUE;
}


int DTappsB64Decode(unsigned char* in, int in_size, unsigned char* out, int& out_size)
{
	int i, j, k;
	unsigned char c[4];
	int read = 0;
 
	for( i=j=k=0; i < in_size; i++ )
	{
	   if( in[i] >= 128 )
		  continue;
	   c[j] = MAP[(unsigned char)in[i]];
	   if( c[j] == 64 )
		  continue;
	   j++;
 
	   switch( j )
	   {
	   case 2:
		  out[k++] = (c[0]<<2) | ((c[1]>>4) & 0x3);
		  break;
 
	   case 3:
		  out[k++] = ((c[1]<<4) & 0xf0) | ((c[2]>>2) & 0xf);
		  break;
 
	   case 4:
		  out[k++] = ((c[2]<<6) & 0xc0) | c[3];
		  read = i;
		  j = 0;
		  break;
	   }
	}
	 
	out_size = k;

	return read+1;
}

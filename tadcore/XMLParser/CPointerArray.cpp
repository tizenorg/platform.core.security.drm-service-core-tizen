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

#include "CPointerArray.h"
#include "TADC_IF.h"
#include "TADC_ErrorCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define			BUFFER_INC_SIZE				128

CPointerArray::CPointerArray()
{
	m_ppData = NULL;
	m_nMaxSize = 0;
	m_nNumOfData = 0;
}

CPointerArray::~CPointerArray()
{
	if( m_ppData != NULL )
	{
		delete[] m_ppData;
	}
}

int CPointerArray::Add( LPVOID pData )
{
	int					nResult;
	int					nNewSize;
	LPVOID*				ppTemp;

	if( m_ppData == NULL )
	{
		m_nMaxSize = BUFFER_INC_SIZE;
		m_ppData = new LPVOID[ BUFFER_INC_SIZE ];

		if(m_ppData == NULL)
		{
			nResult = -1;

			goto finish;
		}
	}

	if( m_nNumOfData >= m_nMaxSize )
	{
		nNewSize = m_nMaxSize += BUFFER_INC_SIZE;
		ppTemp = new LPVOID[ nNewSize ];
		IF_TRUE_GOTO( ppTemp == NULL, -1 );
		
		memcpy( ppTemp, m_ppData, BUFFER_INC_SIZE * sizeof( LPVOID ) );

		delete[] m_ppData;

		m_nMaxSize += BUFFER_INC_SIZE;
		m_ppData = ppTemp;
	}

	m_ppData[ m_nNumOfData ] = pData;
	m_nNumOfData++;

	nResult = 0;

finish:

	if( nResult != 0 )
	{
		DRM_TAPPS_EXCEPTION("CPointerArray::Add() Error! \n");
	}

	return nResult;
}

int CPointerArray::Remove( int nIndex )
{
	int		nResult;
	int		i;

	if( ( nIndex < 0 ) || ( nIndex >= m_nNumOfData ) )
	{
		nResult = -1;//ERRORMSG( ERROR_INVALID_PARAMETER, NULL );

		goto finish;
	}

	for( i = nIndex ; i < m_nNumOfData - 1 ; i++ )
	{
		m_ppData[ i ] = m_ppData[ i + 1 ];
	}
	
	m_nNumOfData--;

	nResult = 0;

finish:

	if( nResult != 0 )
	{
		DRM_TAPPS_EXCEPTION("CPointerArray::Remove() Error! \n");
	}

	return nResult;
}

LPVOID CPointerArray::Get( int nIndex )
{
	if( ( nIndex < 0 ) || ( nIndex >= m_nNumOfData ) )
	{
		return NULL;
	}

	return m_ppData[ nIndex ];
}

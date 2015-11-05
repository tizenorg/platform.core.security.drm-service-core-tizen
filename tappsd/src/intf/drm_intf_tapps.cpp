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
 * @file	drm_intf_tapps.cpp
 * @brief	This file includes definitions of the drm TAPPS intf APIs.
 */

#include "drm_intf_tapps.h"

/* String Operations */
#ifdef _TAPPS_DEBUG_ENABLE_
unsigned int TAPPS_gstrlcat(char* s1,const char* s2,unsigned int destsize,const char* funName, unsigned int lineno)
{
	unsigned int ret_g_strlcat = (unsigned int)g_strlcat(s1, s2, destsize);

	if(ret_g_strlcat >= (unsigned int)destsize)
	{
		DRM_TAPPS_EXCEPTION("Truncation occured during concatenation at [%s:%u] s1=%s s2=%s destsize=%u ret_g_strlcpy=%u \n",funName,lineno,s1,s2,destsize,ret_g_strlcat);
	}
	else
	{
		DRM_TAPPS_FRQ_LOG("Concatenation Successful at [%s:%u] s1=%s s2=%s destsize=%u ret_g_strlcpy=%u \n",funName,lineno,s1,s2,destsize,ret_g_strlcat);
	}

	return ret_g_strlcat;
}

unsigned int TAPPS_gstrlcpy(char* s1,const char* s2,unsigned int destsize,const char* funName, unsigned int lineno)
{
	unsigned int ret_g_strlcpy = (unsigned int)g_strlcpy(s1, s2, destsize);

	if(ret_g_strlcpy >= (unsigned int)destsize)
	{
		DRM_TAPPS_EXCEPTION("Truncation occured during copying at [%s:%u] s1=%s s2=%s destsize=%u ret_g_strlcpy=%u \n",funName,lineno,s1,s2,destsize,ret_g_strlcpy);
	}
	else
	{
		DRM_TAPPS_FRQ_LOG("Copy Successful at [%s:%u] s1=%s s2=%s destsize=%u ret_g_strlcpy=%u \n",funName,lineno,s1,s2,destsize,ret_g_strlcpy);
	}

	return ret_g_strlcpy;
}

unsigned int TAPPS_strlen(const char* s,const char* funName, unsigned int lineno)
{
	DRM_TAPPS_FRQ_LOG("Function = %s, Line number = %u",funName,lineno);

	return strlen(s);
}
#endif

int TAPPS_strnicmp(const char * s1, const char * s2, unsigned int sz)
{
	int tmp1=0, tmp2=0;

	unsigned int idx =0;

	do
	{
		if ( ((tmp1 = (unsigned char)(*(s1++))) >= 'A') && (tmp1 <= 'Z') )
			tmp1 -= ('A' - 'a');

		if ( ((tmp2 = (unsigned char)(*(s2++))) >= 'A') && (tmp2 <= 'Z') )
			tmp2 -= ('A' - 'a');

		idx++;
	} while ( tmp1 && (tmp1== tmp2)  && idx < sz);

	return (tmp1 - tmp2);
}


/* Mutex Handling */

int dtapps_mutex_lock (dtappslockarg dtapps_mutex)
{
	int retval=-1;

	DRM_TAPPS_FRQ_LOG( "%s MUT-LK-ULK Thread [%d] trying to lock the Mutex=0x%x \n",__func__,drmgettid(),dtapps_mutex);

	retval = pthread_mutex_lock(dtapps_mutex);

	if(retval != 0)
	{
		if(retval == EDEADLK)
		{
			DRM_TAPPS_FRQ_LOG("Mutex is already locked by the same thread  \n");
		}
		else
		{
			DRM_TAPPS_EXCEPTION( "Error locking mutex %d  \n", retval);
			perror("\n drm_mutex_lock:Error:");
		}
		retval=-1;
	}
	else
	{
		DRM_TAPPS_FRQ_LOG( "Mutex lock is successful  \n");
	}
	DRM_TAPPS_FRQ_LOG( "%s retval=%d \n",__func__,retval);

	return retval;
}


int dtapps_mutex_unlock (dtappslockarg dtapps_mutex)
{
	int status=-1;

	DRM_TAPPS_FRQ_LOG( "%s MUT-LK-ULK Thread [%d] trying to unlock the Mutex=0x%x \n",__func__,drmgettid(),dtapps_mutex);

	status = pthread_mutex_unlock(dtapps_mutex);

	if(status != 0)
	{
		DRM_TAPPS_EXCEPTION( "Error unlocking mutex:return value=[%d] \n", status);
		perror("\n drm_mutex_unlock:Error:");
		status = -1;
	}
	else
	{
		DRM_TAPPS_FRQ_LOG( "Mutex unlocked is successful :return value=[%d] \n",status);
	}

	DRM_TAPPS_FRQ_LOG( "%s status=%d \n",__func__,status);

	return status;
}

void dtapps_sleep(unsigned int TimeInSec,unsigned int TimeInMicroSec)
{
	struct timeval tv;
	int retval= -1;
	tv.tv_sec = TimeInSec;
	tv.tv_usec = TimeInMicroSec;

	DRM_TAPPS_FRQ_LOG("%s TimeInSec =%u TimeInMicroSec=%u \n",__func__,tv.tv_sec,tv.tv_usec);

	retval = select(0, NULL, NULL, NULL, &tv);

	DRM_TAPPS_FRQ_LOG("%s select retval=%d \n",__func__,retval);
}

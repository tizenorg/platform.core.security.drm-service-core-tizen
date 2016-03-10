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
 * @file	DTapps2Time.cpp
 * @brief	This file includes functions relating to TIME APIs.
 */

#include "DTapps2Time.h"

BOOL DTappsDtTmStr2Sec(unsigned char *time_str,time_t *TotalSec)
{
	char str[32] = {0};
	char short_str[5] = {0};
	struct tm time_fmt;

	memset(&time_fmt, 0x00, sizeof(time_fmt));

	TAPPS_GSTRLCPY(str, (const char*)time_str, sizeof(str));

	DRM_TAPPS_LOG("str = %s",str);

	if(str[4] != '-' || str[7] != '-' || str[10] != 'T' || str[13] != ':' || str[16] != ':')
	{
		DRM_TAPPS_EXCEPTION("Format is incorrect:str=%s", str);

		return FALSE;
	}

	DTAPPS_MEMCPY(short_str, str, 4);
	time_fmt.tm_year = DTAPPS_ATOI(short_str) - 1900;
	DRM_TAPPS_FRQ_LOG("tm_year = %d", time_fmt.tm_year);

	DTAPPS_MEMSET(short_str, 0x0,5);
	DTAPPS_MEMCPY(short_str, str + 5, 2);
	time_fmt.tm_mon = DTAPPS_ATOI(short_str) - 1;
	DRM_TAPPS_FRQ_LOG("tm_mon = %d", time_fmt.tm_mon);

	DTAPPS_MEMCPY(short_str, str + 8, 2);
	time_fmt.tm_mday = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_mday = %d", time_fmt.tm_mday);
	DTAPPS_MEMCPY(short_str, str + 11, 2);
	time_fmt.tm_hour = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_hour = %d", time_fmt.tm_hour);

	DTAPPS_MEMCPY(short_str, str + 14, 2);
	time_fmt.tm_min = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_min = %d", time_fmt.tm_min);

	DTAPPS_MEMCPY(short_str, str + 17, 2);
	time_fmt.tm_sec = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_sec = %d", time_fmt.tm_sec);

	/* Convert into Seconds */
	*TotalSec = DTAPPS_MKTIME(&time_fmt);
	DRM_TAPPS_LOG("TotalSec = %lu", *TotalSec);

	return TRUE;
}

BOOL DTappsDtTmStr2StrucTm(unsigned char *time_str,struct tm *time_fmt)
{
	char str[32] = {0};
	char short_str[5] = {0};

	TAPPS_GSTRLCPY(str, (const char*)time_str, sizeof(str));

	DRM_TAPPS_LOG("str = %s", str);

	if(str[4]!='-' || str[7]!='-' || str[10]!='T' || str[13]!=':' || str[16]!=':')
	{
		DRM_TAPPS_EXCEPTION("Format is incorrect:str=%s", str);

		return FALSE;
	}

	DTAPPS_MEMCPY(short_str, str, 4);
	time_fmt->tm_year = DTAPPS_ATOI(short_str) - 1900;
	DRM_TAPPS_FRQ_LOG("tm_year = %d", time_fmt->tm_year);

	DTAPPS_MEMSET(short_str, 0x0, 5);
	DTAPPS_MEMCPY(short_str, str + 5, 2);
	time_fmt->tm_mon = DTAPPS_ATOI(short_str) - 1;
	DRM_TAPPS_FRQ_LOG("tm_mon = %d", time_fmt->tm_mon);

	DTAPPS_MEMCPY(short_str, str + 8, 2);
	time_fmt->tm_mday = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_mday = %d", time_fmt->tm_mday);

	DTAPPS_MEMCPY(short_str, str + 11, 2);
	time_fmt->tm_hour = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_hour = %d", time_fmt->tm_hour);

	DTAPPS_MEMCPY(short_str, str + 14, 2);
	time_fmt->tm_min = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_min = %d", time_fmt->tm_min);

	DTAPPS_MEMCPY(short_str, str + 17, 2);
	time_fmt->tm_sec = DTAPPS_ATOI(short_str);
	DRM_TAPPS_FRQ_LOG("tm_sec = %d", time_fmt->tm_sec);

	return TRUE;
}

BOOL DTappsGetSecureTime(time_t* seconds)
{
	time_t now = 0, secure_sec = 0;
	int Delta = 0;
	struct tm gm_fmt;

	memset(&gm_fmt, 0x00, sizeof(gm_fmt));

	now = DTAPPS_TIME(NULL);

	/* TODO: Read the Delta from VCONF */

	secure_sec = now + Delta;

	/* Operations to be done in GMT - Convert this to GM Time */
    DTAPPS_GMTIME_THREAD_SAFE(&secure_sec, &gm_fmt);
	*seconds = DTAPPS_MKTIME(&gm_fmt);

	DRM_TAPPS_LOG("now = %d, secure_sec = %d, seconds(SecureTime in GMT) = %d", now, secure_sec, *seconds);

	return TRUE;
}

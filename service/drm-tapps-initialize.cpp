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

#include <drm-tizen-mid.h>

#include "DTapps2SqliteDB.h"

static int __drm_tad_create_db(void)
{
	BOOL db_ret = FALSE;
	int IsBeginSuccessful = 0;

	char Table_Name[16] = "dtapps2rights";
	char* sql_query_table = NULL;
	void *pDb = NULL;

	db_ret = DTappsDBOpen(pDb, __func__);

	if(db_ret != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DTappsDBOpen FAILED");

		return -1;
	}

	db_ret = DTappsDBBeginImmedTrans(__func__);

	if(db_ret != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DTappsDBBeginImmedTrans FAILED");

		goto ErrorExit;
	}

	IsBeginSuccessful = 1;

	sql_query_table = DTappsGetSQLCreateTable(Table_Name);

	if(sql_query_table == NULL)
	{
		DRM_TAPPS_EXCEPTION("DTappsGetSQLCreateTable FAILED");

		goto ErrorExit;
	}

	DRM_TAPPS_FRQ_LOG("sql_query_table = %s", sql_query_table);

	db_ret = DTappsExecuteSQL(pDb, sql_query_table);

	if(db_ret != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DTappsGetSQLCreateTable FAILED");

		goto ErrorExit;
	}

	db_ret = DTappsDBCommit(__func__);

	if(db_ret != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DTappsDBCommit FAILED");

		goto ErrorExit;
	}

	db_ret = DTappsDBClose(__func__);

	if(db_ret != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DTappsDBClose FAILED");

		goto ErrorExit;
	}

	DRM_TAPPS_FRQ_LOG("TAD DB created successfully");

	return 0;

ErrorExit:

	if (1 == IsBeginSuccessful)
	{
		 DTappsDBRollback(__func__);
	}

	db_ret = DTappsDBClose(__func__);
	if(db_ret != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DTappsDBClose FAILED");
	}

	DRM_TAPPS_EXCEPTION("TAD DB creation FAILED");

	return -1;
}

int main(int, char *[])
{
	DRM_TAPPS_FRQ_LOG("=============Start Create TAD DB file ================\n\n ");

	if(0 != __drm_tad_create_db())
	{
		DRM_TAPPS_EXCEPTION("=============Error Creating TAD DB file ================\n\n ");

		return 1;
	}

	DRM_TAPPS_FRQ_LOG("=============Success Created TAD DB file  ================\n\n ");

	return 0;
}

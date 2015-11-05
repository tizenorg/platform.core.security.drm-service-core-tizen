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
 * @file	DTapps2SqlData.cpp
 * @brief	This file contains all the Database tables.
 */

#include "drm_intf_tapps.h"

#define DTAPPS_TABLE_NAME(name) name "\0"
#define DTAPPS_TABLE_CONTINUE        "\0\1"
#define DTAPPS_TABLE_END             "\0",

const char* dtappsCreateTableSQLData[] =
{
	DTAPPS_TABLE_NAME("dtapps2rights")
		"CREATE TABLE dtapps2rights ("
		"r_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name VARCHAR(512) DEFAULT NULL,"
		"cid VARCHAR(1024) DEFAULT NULL,"
		"time_t DATETIME,"
		#ifdef DTAPPS_STORE_CEK_IN_DB
		"cek VARCHAR(128) DEFAULT NULL,"
		"cek_hash VARCHAR(64) DEFAULT NULL,"
		#endif
		"constraint_buffer VARCHAR(512) DEFAULT NULL,"
		"constraint_hash VARCHAR(64) DEFAULT NULL"
	");"

	DTAPPS_TABLE_END
	NULL// empty do not delete this line
};

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
 * @file	DTapps2SqliteDB.h
 * @brief	This file includes declarations and classes relating to DataBase.
 */

#ifndef __DTAPPS_SQLITE_DB_H__
#define __DTAPPS_SQLITE_DB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TADC_Core.h"
#include "drm_intf_tapps.h"

#define DTAPPS_SQL_FREE_TABLE sqlite3_free_table

typedef	struct _tagTAPPSSqliteSelectTable{
	sqlite3*	handle;
	char** 		result;
	int		n_rows;
	int		n_cols;
}TAPPSSqliteSelectTable;

/* Declarations for DB Wrappers */
BOOL DTappsDBOpen(void *&pDb,const char* CallingFun);
BOOL DTappsDBGet(void *& pDBConnection);
BOOL DTappsDBClose(const char* CallingFun);
BOOL DTappsDBBeginImmedTrans (const char* CallingFun);
BOOL DTappsDBCommit(const char* CallingFun);
BOOL DTappsDBRollback (const char* CallingFun);
BOOL DTappsExecuteSQL(void* pDB, const char* query);
BOOL DTappsSQLGetTable(void* pDB, const char* query, TAPPSSqliteSelectTable* select_table);
char* DTappsGetSQLCreateTable(const char* tableName);
void* DTappsStmtPrepare(void* pDB, const char* query);
int DTappsStmtBindParam (
	void* pStmt,
	unsigned int dIdx,
	unsigned int Type,
	void* pParam,
	unsigned int dParamSize );
int DTappsStmtExecute (void* pStmt);
int DTappsStmtRelease (void* pStmt);
BOOL DTapps_DB_Install(const char* sql_query);
BOOL DTapps_Read_DB(const char* sql_query, TAPPSSqliteSelectTable* select_table);

#ifdef __cplusplus
}
#endif

#endif /*__DTAPPS_SQLITE_DB_H__ */

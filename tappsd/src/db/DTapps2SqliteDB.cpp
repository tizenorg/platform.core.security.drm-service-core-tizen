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
 * @file	DTapps2SqliteDB.cpp
 * @brief	This file includes functions relating to DataBase.
 */

#include "DTapps2SqliteDB.h"
#include <map>

/* Define EXPORT_API temporary */
#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif

static dtapps_mutex_t dtapps_sqlite_db_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef long int dtappsThreadID;

 class DtappsDBConnectionInfo
 {
 public:
	 DtappsDBConnectionInfo();
	 int	 countOpenConnection;
	 sqlite3 *pDBConnection;
 };

 using namespace std;
 static std::map<dtappsThreadID, DtappsDBConnectionInfo *> g_dtapps_sqlite_connection_table;

 DtappsDBConnectionInfo::DtappsDBConnectionInfo()
 {
	 countOpenConnection=0;
	 pDBConnection=NULL;
 }

 class TAPPSDbApiLock
 {
 public:
	TAPPSDbApiLock();
	~TAPPSDbApiLock();
 };

 TAPPSDbApiLock::TAPPSDbApiLock()
 {
	 DRM_TAPPS_FRQ_LOG("LOCK by TID = %ld",drmgettid());
	 if (0 != dtapps_mutex_lock (&dtapps_sqlite_db_mutex))
	 {
		 DRM_TAPPS_EXCEPTION("Error while mutex locking.");
	 }
 }

 TAPPSDbApiLock::~TAPPSDbApiLock()
 {
	 DRM_TAPPS_FRQ_LOG("UNLOCK by TID = %ld",drmgettid());
	 if (0 != dtapps_mutex_unlock(&dtapps_sqlite_db_mutex))
	 {
		 DRM_TAPPS_EXCEPTION("Error while mutex unlocking");
	 }
 }

#define __DTAPPS_DB_SQLITE_RETRY__  (10)

#define	DTAPPS_SQLITE3_SQL_BEGIN_IMMEDIATE		"BEGIN IMMEDIATE TRANSACTION"
#define	DTAPPS_SQLITE3_SQL_COMMIT				"COMMIT TRANSACTION"
#define	DTAPPS_SQLITE3_SQL_ROLLBACK				"ROLLBACK TRANSACTION"

/* DataBase Related API Wrappers */
BOOL DTappsDBOpen(void *&pDb, const char* CallingFun)
{
	DRM_TAPPS_LOG("[%s]: started.Calling function = %s",__func__,CallingFun);

	TAPPSDbApiLock Dblock;
	dtappsThreadID id_curr_thread = drmgettid();

	bool IsMemAllocated = false;
	int	result = SQLITE_ERROR;
	unsigned int pairCount = 0;
	sqlite3*	h_db = NULL;
	DtappsDBConnectionInfo *pDBConnectionInfo = NULL;

	DRM_TAPPS_SECURE_LOG("DB-OPEN-CLOSE [%s]Parent Process ID=[%ld]:Process-ID=[%ld]:Thread-ID=[%ld], id_curr_thread=[%ld]",__func__,getppid(),getpid(),drmgettid(), id_curr_thread);

	pairCount = g_dtapps_sqlite_connection_table.count(id_curr_thread);
	DRM_TAPPS_FRQ_LOG("pairCount=[%u]", pairCount);

	if (0 != pairCount)
	{
		DRM_TAPPS_FRQ_LOG("Connection already exists.. pairCount=[%ld]", pairCount);
		pDBConnectionInfo = g_dtapps_sqlite_connection_table[id_curr_thread];
		DRM_TAPPS_FRQ_LOG("pDBConnectionInfo=[0x%x]", pDBConnectionInfo);

		MTRY_BL(NULL != pDBConnectionInfo);
		MTRY_BL(NULL != pDBConnectionInfo->pDBConnection);
		DRM_TAPPS_FRQ_LOG("pDBConnectionInfo->countOpenConnection=[%d], pDBConnectionInfo->pDBConnection=[0x%x]", pDBConnectionInfo->countOpenConnection, pDBConnectionInfo->pDBConnection);

		++(pDBConnectionInfo->countOpenConnection);
		pDb = pDBConnectionInfo->pDBConnection;
	}
	else
	{
		DRM_TAPPS_LOG("no connection exists.. pairCount=[%ld]", pairCount);
		pDBConnectionInfo = NEW DtappsDBConnectionInfo;
		MTRY_BL(NULL != pDBConnectionInfo);
		IsMemAllocated = true;	 // prevent fix

		DRM_TAPPS_FRQ_LOG("Opening DB connection.");

		result = db_util_open(DTAPPS_DB_NAME, &h_db, 0);
		if (result != SQLITE_OK)
		{
			DRM_TAPPS_EXCEPTION("sqlite msg :[%d]%s",result, sqlite3_errmsg(h_db));
			DRM_TAPPS_SECURE_LOG("db name :%s", DTAPPS_DB_NAME);
			MTHROW_BL
		}

		DRM_TAPPS_FRQ_LOG("sqlite3_open() is success. h_db:%x", h_db);

		pDBConnectionInfo->countOpenConnection = 1;
		pDBConnectionInfo->pDBConnection = h_db; h_db = NULL;

		pDb = pDBConnectionInfo->pDBConnection;

		// Insert the node
		DRM_TAPPS_FRQ_LOG("pDBConnectionInfo->countOpenConnection=[%d], pDBConnectionInfo->pDBConnection=[0x%x]", pDBConnectionInfo->countOpenConnection, pDBConnectionInfo->pDBConnection);

		g_dtapps_sqlite_connection_table.insert(pair<dtappsThreadID, DtappsDBConnectionInfo *>(id_curr_thread, pDBConnectionInfo));pDBConnectionInfo = NULL;
	}

	DRM_TAPPS_LOG("This fn finishes successfully.");

	return TRUE;

MCATCH_B

	if (true == IsMemAllocated)
	{
		MDELETE(pDBConnectionInfo);
	}
	DRM_TAPPS_EXCEPTION("This fn fails");

	return FALSE;
}

BOOL DTappsDBGet(void *& pDBConnection)
{
	DRM_TAPPS_LOG("Inside %s", __func__);

	TAPPSDbApiLock Dblock;
	unsigned int pairCount;
	dtappsThreadID id_curr_thread = drmgettid();
	pDBConnection = NULL;

	DRM_TAPPS_LOG("Parent Process ID=[%ld]:Process-ID=[%ld]:Thread-ID=[%ld], id_curr_thread=[%ld]",getppid(),getpid(),drmgettid(), id_curr_thread);

	pairCount = g_dtapps_sqlite_connection_table.count(id_curr_thread);
	MTRY_BL(0 != pairCount);

	DRM_TAPPS_FRQ_LOG("g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection=[%d], g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection=[0x%x]", g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection, g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection);

	pDBConnection = g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection;

	DRM_TAPPS_LOG("pairCount=[%u], g_dtapps_sqlite_connection_table[connectionIterator]=[0x%x]", pairCount, g_dtapps_sqlite_connection_table[id_curr_thread]);
	DRM_TAPPS_LOG(" This fn finishes successfully..pDBConnection=[0x%x]", pDBConnection);
	DRM_TAPPS_FRQ_LOG("pDBConnection=[0x%x]",pDBConnection);

	return TRUE;

MCATCH_B
	DRM_TAPPS_EXCEPTION("This fn fails.. pDBConnection=[0x%x]", pDBConnection);

	return FALSE;
}

BOOL DTappsDBClose(const char* CallingFun)
{
	DRM_TAPPS_LOG("Inside %s Calling function = %s", __func__, CallingFun);

	TAPPSDbApiLock Dblock;
	unsigned int pairCount;
	sqlite3_stmt*	pstmt	= NULL;
	dtappsThreadID id_curr_thread = drmgettid();
	int countConnection;
	sqlite3 *pDBConnection = NULL;
	int result = SQLITE_ERROR;

	DRM_TAPPS_SECURE_LOG("DB-OPEN-CLOSE [%s]Parent Process ID=[%ld]:Process-ID=[%ld]:Thread-ID=[%ld], id_curr_thread=[%ld]",__func__,getppid(),getpid(),drmgettid(), id_curr_thread);

	pairCount = g_dtapps_sqlite_connection_table.count(id_curr_thread);
	MTRY_BL(0 != pairCount);

	DRM_TAPPS_FRQ_LOG("g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection=[%d], g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection=[0x%x] \n", g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection, g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection);

	--(g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection);
	countConnection = g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection;

	DRM_TAPPS_LOG(" countConnection=[%d] ", countConnection);

	if (0 == countConnection)
	{
		DRM_TAPPS_LOG("closing DB connection info ");

		pDBConnection = g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection;

		DRM_TAPPS_LOG("pairCount=[%u], g_sqlite_connection_table[connectionIterator]=[0x%x]", pairCount, g_dtapps_sqlite_connection_table[id_curr_thread]);
		DRM_TAPPS_LOG("erasing map entry..pDBConnection=[0x%x]", pDBConnection);

		g_dtapps_sqlite_connection_table.erase(id_curr_thread);

		DRM_TAPPS_LOG("finalizing all statements..pDBConnection=[0x%x]", pDBConnection);

		while ((pstmt = sqlite3_next_stmt(pDBConnection, pstmt)) != NULL) // prevent fix
		{
			DRM_TAPPS_LOG("finalizing DB statement..pstmt=[0x%x]", pstmt);
			sqlite3_finalize(pstmt);
		}

		DRM_TAPPS_LOG(" Closing DB connection..pDBConnection=[0x%x]", pDBConnection);

		result = db_util_close(pDBConnection);
		if(result != SQLITE_OK)
		{
			DRM_TAPPS_EXCEPTION("SQLite Close fails. errmsg:%s", sqlite3_errmsg(pDBConnection));
			MTHROW_BL
		}
	}

	DRM_TAPPS_LOG("This fn finishes successfully..pDBConnection=[0x%x]", pDBConnection);

	return TRUE;

MCATCH_B

	DRM_TAPPS_EXCEPTION(" This fn fails.. pDBConnection=[0x%x]", pDBConnection);

	return FALSE;
}

BOOL DTappsDBBeginImmedTrans (const char* CallingFun)
{
	DRM_TAPPS_LOG("Inside %s, Calling function = %s", __func__, CallingFun);

	unsigned int pairCount;
	dtappsThreadID id_curr_thread = drmgettid();
	sqlite3* pDBConnection = NULL;

	int count_try_db=0,rc = -1;

	DRM_TAPPS_SECURE_LOG("DB-OPEN-CLOSE-BEG-COM-RB [%s]Parent Process ID=[%ld]:Process-ID=[%ld]:Thread-ID=[%ld], id_curr_thread=[%ld]",__func__,getppid(),getpid(),drmgettid(), id_curr_thread);

	pairCount = g_dtapps_sqlite_connection_table.count(id_curr_thread);
	MTRY_BL(0 != pairCount);

	DRM_TAPPS_FRQ_LOG("g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection=[%d], g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection=[0x%x]", __func__, g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection, g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection);

	pDBConnection = g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection;

	DRM_TAPPS_LOG("pairCount=[%u], g_dtapps_sqlite_connection_table[connectionIterator]=[0x%x]", pairCount, g_dtapps_sqlite_connection_table[id_curr_thread]);
	DRM_TAPPS_LOG("Beginning DB operations..pDBConnection=[0x%x]", pDBConnection);

	DRM_TAPPS_FRQ_LOG("pDBConnection=[0x%x]",pDBConnection);

	do
	{
		DRM_TAPPS_LOG("START BEGIN");
		rc = sqlite3_exec (pDBConnection, DTAPPS_SQLITE3_SQL_BEGIN_IMMEDIATE, NULL, NULL, NULL);

		DRM_TAPPS_FRQ_LOG("START BEGIN rc=%d", rc);
		if (rc != SQLITE_OK)
		{
			DRM_TAPPS_FRQ_LOG("pDBConnection=0x%x rc=%d ErrMsg:%s", pDBConnection, rc, sqlite3_errmsg(pDBConnection));
			if (rc == SQLITE_BUSY)
			{
				dtapps_sleep(2, 0);
				DRM_TAPPS_FRQ_LOG("Tried [%d] times to begin", count_try_db);
				count_try_db++;

				if (count_try_db >= (int)__DTAPPS_DB_SQLITE_RETRY__)
				{
					DRM_TAPPS_EXCEPTION("Error pDBConnection=0x%x rc=%d ErrMsg:%s", pDBConnection, rc, sqlite3_errmsg(pDBConnection));
					MTHROW_BL
				}
			}
			else
			{
				DRM_TAPPS_EXCEPTION("Error pDBConnection=0x%x rc=%d ErrMsg:%s", pDBConnection, rc, sqlite3_errmsg(pDBConnection));
				MTHROW_BL
			}
		}
		else
		{
			DRM_TAPPS_LOG("begin SUCCESS count_try_db=%d", count_try_db);
			break;
		}
	} while(1);

	DRM_TAPPS_LOG("This fn finishes successfully..pDBConnection=[0x%x]", pDBConnection);

	return TRUE;

MCATCH_B
    DRM_TAPPS_EXCEPTION("This fn fails.. pDBConnection=[0x%x]", pDBConnection);
    return FALSE;
}

BOOL DTappsDBCommit(const char* CallingFun)
{
	DRM_TAPPS_LOG("Inside %s, Calling function = %s", __func__,CallingFun);
	unsigned int pairCount;
	dtappsThreadID id_curr_thread = drmgettid();
	sqlite3* pDBConnection = NULL;

	int count_try_db_commit=0,rc = -1;

	DRM_TAPPS_SECURE_LOG("DB-OPEN-CLOSE-BEG-COM-RB [%s]Parent Process ID=[%ld]:Process-ID=[%ld]:Thread-ID=[%ld], id_curr_thread=[%ld]",__func__,getppid(),getpid(),drmgettid(), id_curr_thread);

	pairCount = g_dtapps_sqlite_connection_table.count(id_curr_thread);
	MTRY_BL(1 == pairCount);

	DRM_TAPPS_FRQ_LOG("g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection=[%d], g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection=[0x%x] \n", g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection, g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection);

	pDBConnection = g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection;

	DRM_TAPPS_LOG("pairCount=[%u], g_dtapps_sqlite_connection_table[connectionIterator]=[0x%x]", pairCount, g_dtapps_sqlite_connection_table[id_curr_thread]);
	DRM_TAPPS_LOG("Commiting DB operations..pDBConnection=[0x%x]", pDBConnection);
	DRM_TAPPS_FRQ_LOG("pDBConnection=[0x%x]",pDBConnection);

	do
	{
		DRM_TAPPS_FRQ_LOG("START Commit");
		rc = sqlite3_exec (pDBConnection, DTAPPS_SQLITE3_SQL_COMMIT, NULL, NULL, NULL);
		DRM_TAPPS_FRQ_LOG("START Commit rc=%d", rc);

		if (rc != SQLITE_OK)
		{
			DRM_TAPPS_FRQ_LOG("pDBConnection=0x%x rc=%d ErrMsg:%s", pDBConnection, rc, sqlite3_errmsg(pDBConnection));

			if (rc == SQLITE_BUSY)
			{
				dtapps_sleep(2, 0);
				DRM_TAPPS_FRQ_LOG("Tried [%d] times to Commit", count_try_db_commit);
				count_try_db_commit++;
				if (count_try_db_commit >= (int)__DTAPPS_DB_SQLITE_RETRY__)
				{
					DRM_TAPPS_EXCEPTION("Error pDBConnection=0x%x rc=%d ErrMsg:%s", pDBConnection, rc, sqlite3_errmsg(pDBConnection));
					MTHROW_BL
				}
			}
			else
			{
				DRM_TAPPS_EXCEPTION("Error pDBConnection=0x%x rc=%d ErrMsg:%s", pDBConnection, rc, sqlite3_errmsg(pDBConnection));
				MTHROW_BL
			}
		}
		else
		{
			DRM_TAPPS_FRQ_LOG("Commit SUCCESS count_try_db_commit=%d", count_try_db_commit);
			break;
		}
	} while(1);

	DRM_TAPPS_LOG("This fn finishes successfully..pDBConnection=[0x%x]", pDBConnection);

	return TRUE;

MCATCH_B
    DRM_TAPPS_EXCEPTION("[%s] This fn fails.. pDBConnection=[0x%x]", pDBConnection);

    return FALSE;
}

BOOL DTappsDBRollback (const char* CallingFun)
{
	DRM_TAPPS_LOG("Inside %s, Calling function = %s", __func__,CallingFun);

	unsigned int pairCount;
	dtappsThreadID id_curr_thread = drmgettid();
	sqlite3* pDBConnection = NULL;

	int count_try_db_rollback = 0, rc = -1;

	DRM_TAPPS_SECURE_LOG("DB-OPEN-CLOSE-BEG-COM-RB [%s]Parent Process ID=[%ld]:Process-ID=[%ld]:Thread-ID=[%ld], id_curr_thread=[%ld]",__func__,getppid(),getpid(),drmgettid(), id_curr_thread);

	pairCount = g_dtapps_sqlite_connection_table.count(id_curr_thread);
	MTRY_BL(1 == pairCount);

	DRM_TAPPS_SECURE_LOG("[%s] g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection=[%d], g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection=[0x%x]", __func__, g_dtapps_sqlite_connection_table[id_curr_thread]->countOpenConnection, g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection);

	pDBConnection = g_dtapps_sqlite_connection_table[id_curr_thread]->pDBConnection;

	DRM_TAPPS_LOG("pairCount=[%u], g_sqlite_connection_table[connectionIterator]=[0x%x]", __func__, pairCount, g_dtapps_sqlite_connection_table[id_curr_thread]);
	DRM_TAPPS_LOG("Rollback DB operations..pDBConnection=[0x%x]", pDBConnection);

	DRM_TAPPS_FRQ_LOG("[%s] pDBConnection=[0x%x]",pDBConnection);

	do
	{
		DRM_TAPPS_FRQ_LOG("START Rollback");
		rc = sqlite3_exec (pDBConnection, DTAPPS_SQLITE3_SQL_ROLLBACK, NULL, NULL, NULL);
		DRM_TAPPS_FRQ_LOG("START  Rollback rc=%d",rc);

		if (rc != SQLITE_OK)
		{
			DRM_TAPPS_FRQ_LOG("pDBConnection=0x%x rc=%d ErrMsg:%s",pDBConnection,rc,sqlite3_errmsg(pDBConnection));

			if(rc == SQLITE_BUSY)
			{
				dtapps_sleep(2,0);
				DRM_TAPPS_FRQ_LOG("Tried [%d] times to Rollback",count_try_db_rollback);
				count_try_db_rollback++;
				if(count_try_db_rollback >= (int)__DTAPPS_DB_SQLITE_RETRY__)
				{
					DRM_TAPPS_EXCEPTION("Error pDBConnection=0x%x rc=%d ErrMsg:%s",pDBConnection,rc,sqlite3_errmsg(pDBConnection));
					MTHROW_BL
				}
			}
			else
			{
				DRM_TAPPS_EXCEPTION("Error pDBConnection=0x%x rc=%d ErrMsg:%s",pDBConnection,rc,sqlite3_errmsg(pDBConnection));
				MTHROW_BL
			}
		}
		else
		{
			DRM_TAPPS_FRQ_LOG("Rollback SUCCESS count_try_db_rollback=%d",count_try_db_rollback);
			break;
		}
	} while(1);

	DRM_TAPPS_LOG("This fn finishes successfully..pDBConnection=[0x%x]", pDBConnection);

	return TRUE;

MCATCH_B
    DRM_TAPPS_EXCEPTION("This fn fails.. pDBConnection=[0x%x]", pDBConnection);

    return FALSE;
}


BOOL DTappsExecuteSQL(void* pDB, const char* query)
{
	int count_try_db_exe = 0, rc = 0;
	sqlite3* h_db = (sqlite3*)pDB;

	TAPPSDbApiLock Dblock;

	do
	{
		rc = sqlite3_exec (h_db, query, NULL, NULL, NULL);
		DRM_TAPPS_FRQ_LOG("EXECUTE rc=%d", rc);

		if (rc != SQLITE_OK)
		{
			if (rc == SQLITE_BUSY)
			{
				dtapps_sleep(2, 0);
				DRM_TAPPS_FRQ_LOG("Tried [%d] times to execute",count_try_db_exe);
				count_try_db_exe++;

				if (count_try_db_exe >= (int)__DTAPPS_DB_SQLITE_RETRY__)
				{
					DRM_TAPPS_EXCEPTION("h_db=0x%x rc=%d ErrMsg:%s count_try_db_exe=%d",h_db,rc,sqlite3_errmsg(h_db),count_try_db_exe);
					DRM_TAPPS_EXCEPTION("query=[%s]",query);
					return FALSE;
				}
			}
			else
			{
				DRM_TAPPS_EXCEPTION("h_db=0x%x rc=%d ErrMsg:%s", h_db, rc, sqlite3_errmsg(h_db));
				DRM_TAPPS_EXCEPTION("query=[%s]", query);
				return FALSE;
			}
		}
		else
		{
			DRM_TAPPS_FRQ_LOG("EXECUTE SUCCESS count_try_db_exe=%d", count_try_db_exe);
			break;
		}
	} while(1);

	return TRUE;
}


BOOL DTappsSQLGetTable(void* pDB, const char* query, TAPPSSqliteSelectTable* select_table)
{
	int rc = 0;
	sqlite3* h_db = (sqlite3*)pDB;

	DRM_TAPPS_FRQ_LOG("h_db=0x%x query=%s", h_db,query);
	TAPPSDbApiLock Dblock;
	select_table->handle = h_db;

	DRM_TAPPS_FRQ_LOG("select_table->handle=%x", select_table->handle);

	int count_try_db_select = 0;

	do
	{
		rc = sqlite3_get_table(h_db, query, &select_table->result, &select_table->n_rows, &select_table->n_cols, NULL);

		DRM_TAPPS_FRQ_LOG("SELECT rc=%d", rc);

		if (rc != SQLITE_OK)
		{
			if (rc == SQLITE_BUSY)
			{
				dtapps_sleep(2,0);
				DRM_TAPPS_FRQ_LOG("SELECT  Tried [%d] times to select \n",count_try_db_select);

				count_try_db_select++;

				if(count_try_db_select >= (int)__DTAPPS_DB_SQLITE_RETRY__)
				{
					DRM_TAPPS_EXCEPTION("SELECT  h_db=0x%x rc=%d ErrMsg:%s count_try_db_select=%d \n",h_db,rc,sqlite3_errmsg(h_db),count_try_db_select);
					DRM_TAPPS_EXCEPTION("query=[%s] \n",query);

					return FALSE;
				}
			}
			else
			{
				DRM_TAPPS_EXCEPTION("SELECT h_db=0x%x rc=%d ErrMsg:%s \n",h_db,rc,sqlite3_errmsg(h_db));
				DRM_TAPPS_EXCEPTION("%s query=[%s] \n",query);

				return FALSE;
			}
		}
		else
		{
			DRM_TAPPS_FRQ_LOG("SELECT SUCCESS count_try_db_select=%d \n",count_try_db_select);

			break;
		}
	} while(1);

	DRM_TAPPS_FRQ_LOG("n_rows=%d n_cols=%d \n",select_table->n_rows, select_table->n_cols);

	return TRUE;
}

extern const char* dtappsCreateTableSQLData[];

char* DTappsGetSQLCreateTable(const char* tableName)
{
	int index=0;

	if(!tableName)
	{
		return NULL;
	}

	DRM_TAPPS_FRQ_LOG("tableName = %s",tableName);

	while(dtappsCreateTableSQLData[index])
	{
		if(TAPPS_strnicmp( dtappsCreateTableSQLData[index],tableName,TAPPS_STRLEN(tableName))==0)
		{
			char* sql = (char*)dtappsCreateTableSQLData[index];
			sql = sql + TAPPS_STRLEN(dtappsCreateTableSQLData[index])+1;

			DRM_TAPPS_FRQ_LOG("sql query = %s",sql);

			return sql;
		}
		else
		{
			index++;
		}
	}

	DRM_TAPPS_EXCEPTION("Specified Table Name is not Valid!!!");

	return NULL;
}

void* DTappsStmtPrepare(void* pDB, const char* query)
{
	int rc = 0;
	sqlite3* 	h_db = (sqlite3*)pDB;
	sqlite3_stmt*	stmt = 0;

	DRM_TAPPS_FRQ_LOG("h_db=0x%x query=%s \n",h_db,query);

	TAPPSDbApiLock Dblock;

	if(0 == TAPPS_strnicmp(query, "SELECT", 6))
	{
		DRM_TAPPS_EXCEPTION("Doesn't support 'Select' query h_db=0x%x query=%s \n",h_db,query);

		return NULL;
	}

	rc = sqlite3_prepare( h_db, query, -1, &stmt, NULL);

	DRM_TAPPS_FRQ_LOG("%s: rc=%d query=%s stmt=0x%x \n",__func__,rc,query,stmt);

	if (rc != SQLITE_OK)
	{
		DRM_TAPPS_EXCEPTION("DTappsStmtPrepare: h_db=0x%x err: %s \n",h_db,sqlite3_errmsg(h_db));
		DRM_TAPPS_EXCEPTION("DTappsStmtPrepare: query: %s\n", query);

		return NULL;
	}

	return stmt;
}


int DTappsStmtBindParam (
	void* pStmt,
	unsigned int dIdx,
	unsigned int Type,
	void* pParam,
	unsigned int dParamSize )
{
	int rc = 0;
	sqlite3_stmt*	stmt = (sqlite3_stmt*)pStmt;

	DRM_TAPPS_FRQ_LOG("Enter \n");

	TAPPSDbApiLock Dblock;

	if(stmt == NULL)
	{
		DRM_TAPPS_EXCEPTION("DTappsStmtBindParam: stmt is NULL \n");

		return FALSE;
	}

	/*
	 * Notice:
	 *  The parameter index in SQL starts from 1 in sqlite3.
	 */
	switch(Type)
	{
	case TAPPSDB_TYPE_NONE:
		rc = sqlite3_bind_null( stmt, dIdx+1);

		break;

	case TAPPSDB_TYPE_INT:
		rc = sqlite3_bind_int( stmt, dIdx+1, *(int*)pParam);
		DRM_TAPPS_FRQ_LOG(" rc=%d type=%d pParam=%d \n",rc,Type,*(int*)pParam);

		break;

	case TAPPSDB_TYPE_DATETIME:

	case TAPPSDB_TYPE_CHAR:

	case TAPPSDB_TYPE_VARCHAR:
		rc = sqlite3_bind_text( stmt, dIdx+1, (char*)pParam, dParamSize, SQLITE_TRANSIENT);
		DRM_TAPPS_FRQ_LOG("rc=%d type=%d dParamSize=%d pParam=%s \n",rc,Type,dParamSize,pParam);

		break;

	case TAPPSDB_TYPE_BINARY:

	case TAPPSDB_TYPE_BLOB:
		#if 0
		{
			char* packet64=NULL;
			unsigned int packet64_size=0;
			if(CMStringUtil::GetBase64Encode((unsigned char*)pParam,dParamSize, &packet64) == true)
			{
				packet64_size = MSTRLEN(packet64);
				rc = sqlite3_bind_blob( stmt, dIdx+1, packet64, packet64_size, SQLITE_TRANSIENT);
				DRM_OMA_FRQ_LOG("%s: rc=%d type=%d packet64_size=%d packet64=%s \n",__func__,rc,Type,packet64_size,packet64);
				if(packet64) MDELETES0(packet64);
			}
		}
		#endif
		break;

	case TAPPSDB_TYPE_UNKNOWN:

	default:
		break;
	}//switch

	if(rc != SQLITE_OK)
	{
		DRM_TAPPS_EXCEPTION("DTappsStmtBindParam: errno: %d\n", rc);

		return FALSE;
	}

	return TRUE;
}


int DTappsStmtExecute (void* pStmt)
{
	int CntTryStmtExe=0, rc =-1;
	sqlite3_stmt*	stmt = (sqlite3_stmt*)pStmt;

	DRM_TAPPS_FRQ_LOG("%s:stmt=0x%x \n",__func__,stmt);

	TAPPSDbApiLock Dblock;

	if(stmt == NULL)
	{
		DRM_TAPPS_EXCEPTION("stmt is NULL \n");

		return FALSE;
	}

	do
	{
		rc = sqlite3_step(stmt);

		DRM_TAPPS_FRQ_LOG(" rc=%d \n",rc);

		if ( rc != SQLITE_DONE )
		{
			if(rc == SQLITE_BUSY)
			{
				dtapps_sleep(2,0);
				DRM_TAPPS_FRQ_LOG("Tried [%d] times to execute stmt \n",CntTryStmtExe);

				CntTryStmtExe++;

				if(CntTryStmtExe >= (int)__DTAPPS_DB_SQLITE_RETRY__)
				{
					DRM_TAPPS_EXCEPTION("stmt=0x%x rc=%d CntTryStmtExe=%d \n",stmt,rc,CntTryStmtExe);

					return FALSE;
				}
			}
			else
			{
				DRM_TAPPS_EXCEPTION("stmt=0x%x rc=%d \n",stmt,rc);

				return FALSE;
			}
		}
		else
		{
			DRM_TAPPS_FRQ_LOG("STMT EXECUTE SUCCESS CntTryStmtExe=%d \n",CntTryStmtExe);

			return TRUE;
		}
	} while(1);
}


int DTappsStmtRelease (void* pStmt)
{
	int	rc = 0;
	sqlite3_stmt*	stmt = (sqlite3_stmt*)pStmt;
	DRM_TAPPS_FRQ_LOG("%s:Enter \n",__func__);
	TAPPSDbApiLock Dblock;
	if (!stmt)
	{
		return FALSE;
	}

	rc = sqlite3_finalize (stmt);

	DRM_TAPPS_FRQ_LOG("rc=%d \n",rc);

	if ( rc != SQLITE_OK )
	{
		DRM_TAPPS_EXCEPTION("DTappsStmtRelease() Errmsg ; %d \n", rc);

		return FALSE;
	}

	return TRUE;
}


BOOL DTapps_DB_Install(const char* sql_query)
{
	void *pDb = NULL;
	BOOL ret_value = FALSE;

	if(sql_query == NULL)
	{
		DRM_TAPPS_EXCEPTION("Parameter NULL!!!, sql_query = %p",sql_query);

		return FALSE;
	}

	DRM_TAPPS_LOG("Open DB......");

	ret_value = DTappsDBOpen(pDb,__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DB Open Failed!! ret_value = %d",ret_value);

		return FALSE;
	}

	DRM_TAPPS_LOG("Begin Transaction........");

	ret_value = DTappsDBBeginImmedTrans(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DB Begin Transaction ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Execute SQL to Insert Contents into Table........");

	ret_value = DTappsExecuteSQL(pDb,sql_query);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Execute SQL Query Failed!! ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Commit DB........");

	ret_value = DTappsDBCommit(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Commit DB Failed!! ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Close DB........");

	ret_value = DTappsDBClose(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Install DB Operartion Successful!!!");

	return TRUE;

Error_Exit:

	ret_value = DTappsDBRollback (__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Rollback DB Failed!! ret_value = %d",ret_value);
	}

	ret_value = DTappsDBClose(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);
	}

	return FALSE;
}

BOOL DTapps_Read_DB(const char* sql_query, TAPPSSqliteSelectTable* select_table)
{
	void *pDb = NULL;
	BOOL ret_value = FALSE;

	if(sql_query == NULL || select_table == NULL)
	{
		DRM_TAPPS_EXCEPTION("Parameters NULL, sql_query = %p, select_table = %p",sql_query,select_table);

		return FALSE;
	}

	DRM_TAPPS_LOG("Open DB......");

	ret_value = DTappsDBOpen(pDb,__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DB Open Failed!! ret_value = %d",ret_value);

		return FALSE;
	}

	DRM_TAPPS_LOG("Begin Transaction........");

	ret_value = DTappsDBBeginImmedTrans(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DB Begin Transaction Failed!! ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Get the Result Table........");

	ret_value = DTappsSQLGetTable(pDb,sql_query,select_table);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DB Get Table failed!! ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Close DB........");

	ret_value = DTappsDBClose(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("DB Close failed!! ret_value = %d",ret_value);

		goto Error_Exit;
	}

	DRM_TAPPS_LOG("Reading from DB Successful!!!");

	return TRUE;

Error_Exit:

	ret_value = DTappsDBRollback (__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Rollback DB Failed!! ret_value = %d",ret_value);
	}

	ret_value = DTappsDBClose(__func__);

	if(ret_value != TRUE)
	{
		DRM_TAPPS_EXCEPTION("Close DB Failed!! ret_value = %d",ret_value);
	}

	DRM_TAPPS_EXCEPTION("Reading DB function failed!!!");

	return FALSE;
}

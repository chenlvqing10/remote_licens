/*
 * sqlite_middle.h - Implement sqlite middleware function
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include "ee_queue.h"
#include "file_ops.h"


#include "db_sqlite_middle.h"

#define LOG_TAG "SQLITE_MIDDLE"

#ifdef _WIN32
    #define ALOGD printf
    #define ALOGE printf
#else
    #include <log/log.h>
#endif
#define DB_NAME "/data/user.db"	/* Database name */

static pthread_mutex_t db_mutex_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * db_stmt_get_blob - Result BLOB values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 * @out:
 *
 * Secondary encapsulation of "sqlite3_column_blob"
 *
 * Returns BLOB result.
 */
int db_stmt_get_blob(sqlite3_stmt *stmt, int index, unsigned char *out)
{
	const char *pdata = sqlite3_column_blob(stmt, index);
	int len = sqlite3_column_bytes(stmt, index);
	if (pdata) {
		memcpy(out, pdata, len);
		return len;
	} else
		ALOGE("[%s:%d] error\n", __func__, __LINE__);

	return 0;
}

int db_stmt_get_blob_v2(sqlite3_stmt *stmt, int index, unsigned char *out, int size)
{
	const char *pdata = sqlite3_column_blob(stmt, index);
	int len = sqlite3_column_bytes(stmt, index);
	len = len > size ? size : len;
	if (pdata) {
		memcpy(out, pdata, len);
		return len;
	} else
		ALOGE("[%s:%d] data is empty or error!\n", __func__, __LINE__);

	return 0;
}

/**
 * db_stmt_get_text - Result text values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 * @out:
 *
 * Secondary encapsulation of "sqlite3_column_text"
 *
 * Returns UTF-8 TEXT result.
 */
int db_stmt_get_text(sqlite3_stmt *stmt, int index, char *out)
{
	const unsigned char *pdata = sqlite3_column_text(stmt, index);
	if (pdata) {
		int len = strlen((const char *)pdata);
		strncpy(out, (const char *)pdata, len);
		return len;
	} else
		ALOGE("[%s:%d] error\n", __func__, __LINE__);

	return 0;
}

int db_stmt_get_text_v2(sqlite3_stmt *stmt, int index, char *out, int size)
{
	const unsigned char *pdata = sqlite3_column_text(stmt, index);
	if (pdata) {
		int len = strlen((const char *)pdata);
		len = len > size ? size : len;
		strncpy(out, (const char *)pdata, len);
		return len;
	} else
		ALOGE("[%s:%d] no text or error!\n", __func__, __LINE__);

	return 0;
}
int db_stmt_get_text_v3(sqlite3_stmt *stmt, int index, char **out)
{
	const unsigned char *pdata = sqlite3_column_text(stmt, index);
	if (pdata) {
		int len = strlen((const char *)pdata);
		*out = malloc(len+1);
		memset(*out, 0x00, len+1);
		strncpy(*out, (const char *)pdata, len);
		return len;
	} else
		ALOGE("[%s:%d] no text or error!\n", __func__, __LINE__);

	return 0;
}


/**
 * db_stmt_get_int - Result int values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 *
 * Secondary encapsulation of "sqlite3_column_int"
 *
 * Returns int value.
 */
int db_stmt_get_int(sqlite3_stmt *stmt, int index)
{
	return sqlite3_column_int(stmt, index);
}

/**
 * db_stmt_get_double - Result double values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 *
 * Secondary encapsulation of "sqlite3_column_double"
 *
 * Returns double value.
 */
double db_stmt_get_double(sqlite3_stmt *stmt, int index)
{
	return sqlite3_column_double(stmt, index);
}

/**
 * db_stmt_get_int64 - Result int64 values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 *
 * Secondary encapsulation of "sqlite3_column_double"
 *
 * Returns double value.
 */
long long db_stmt_get_int64(sqlite3_stmt *stmt, int index)
{
	return sqlite3_column_int64(stmt, index);
}

/**
 * db_get_count - Handling non-query operations by a transaction.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @arg	: Out the Number of data.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
static int db_get_count(sqlite3_stmt *stmt, void *arg)
{
	int ret;
	int *count = arg;

	ret = sqlite3_step(stmt);
	if (ret != SQLITE_ROW) {
		return SQLITE_EMPTY;
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
	}

	*count = db_stmt_get_int(stmt, 0);

	return SQLITE_OK;
}

/**
 * db_get_min_punchtime - Handling non-query operations by a transaction.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @arg	: Out the Number of data.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
static int db_get_min_punchtime(sqlite3_stmt *stmt, void *arg)
{
	int ret;
	double *count = arg;

	ret = sqlite3_step(stmt);
	if (ret != SQLITE_ROW) {
		return SQLITE_EMPTY;
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
	}

	*count = db_stmt_get_double(stmt, 0);

	return SQLITE_OK;
}

/**
 * db_query_result_count - Get the number of results of the query.
 * @sql:  A pointer to the SQL string.
 *
 * Get the first row of the first column of the query results.
 *
 * Returns number of data queried.
 */
int db_query_result_count(const char *sql)
{
	int ret, count = 0;

	ALOGD("[%s:%d] sql=%s\n", __func__, __LINE__, sql);

	ret = db_query_by_var_param(sql, db_get_count, &count, NULL);
	if (SQLITE_OK == ret)
		return count;

	return 0;
}

/**
 * db_query_result_count_by_name - Get the number of results of the query.
 * @sql:  A pointer to the SQL string.
 * @name:  A pointer to the name string.
 *
 * Get the first row of the first column of the query results.
 *
 * Returns number of data queried.
 */
int db_query_result_count_by_name(const char *sql, char *name)
{
	int ret, count = 0;

	if (!sql || !name)
		return -1;
	if (strlen(sql) <= 0 || strlen(name) <= 0)
		return -1;

	ALOGD("[%s:%d] sql=%s\n", __func__, __LINE__, sql);

	ret = db_query_by_var_param(sql, db_get_count, &count, "%s", name);
	if (SQLITE_OK == ret)
		return count;

	return 0;
}

/**
 * db_query_result_min_punchtime - Get min punchtime of punch_record.
 * @sql:  A pointer to the SQL string.
 *
 * Get the first row of the first column of the query results.
 *
 * Returns number of data queried.
 */
double db_query_result_min_punchtime(const char *sql)
{
	int ret = 0;
	double count = 0;

	ALOGD("[%s:%d] sql=%s\n", __func__, __LINE__, sql);

	ret = db_query_by_var_param(sql, db_get_min_punchtime, &count, NULL);
	if (SQLITE_OK == ret)
		return count;

	return 0;
}

/**
 * db_stmt_bind_by_string - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_stmt_bind_by_string(sqlite3_stmt *stmt, unsigned int index, void *arg)
{
	int rc;
	char *param = arg;
	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_text(stmt, 1, param,
			strlen(param), NULL);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}

	return SQLITE_OK;
}

/**
 * db_stmt_bind_by_var - Binding a varying number of arguments To Prepared Statements
 * @stmt: A pointer to the [sqlite3_stmt] object
 * @fmt : Format string.
 * @args: A list of arguments
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
static int db_stmt_bind_by_var(sqlite3_stmt *stmt, const char *fmt, va_list args)
{
	int len, npara = 1;
	int ret = SQLITE_OK;

	if (NULL == fmt) {
		return ret;
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
	}

	for (; *fmt; ++fmt) {
		if (*fmt != '%')
			continue;
		++fmt;

		/* get length */
		len = 0;
		while (isdigit((int)(*fmt))) {
			len = len * 10 + (*fmt - '0');
			++fmt;
		}

		switch (*fmt) {
		case 'd':
			ret = sqlite3_bind_int(stmt, npara, va_arg(args, int));
			break;

		case 's':
			{
			char *str = va_arg(args, char *);
			ret = sqlite3_bind_text(stmt, npara, str, strlen(str), NULL);
			}
			break;

		case 'x': case 'X':
			{
			unsigned char *pdata;
			pdata = va_arg(args, unsigned char *);
			ret = sqlite3_bind_blob(stmt, npara, pdata, len, NULL);
			}
			break;

		default:
			ret = SQLITE_ERROR;
			ALOGE("[%s:%d] error\n", __func__, __LINE__);
			break;
		}

		++npara;

		if (ret) {
			return ret;
			ALOGE("[%s:%d] error\n", __func__, __LINE__);
		}
	}

	return ret;
}

/**
 * db_query_by_var_param - Database query operations, with variable parameter
 * @sql    : A pointer to the SQL string.
 * @create : Get data and create objects.
 * @arg    : This parameter is for create function.
 * @fmt    : Format string:
 *                         %s string
 *                         %d int
 *                         %nx Binary data of length N
 * @...    : Variable parameter.
 *
 * Returns number of data queried.
 */
int db_query_by_var_param(const char *sql, int (*create)(sqlite3_stmt *stmt,
			void *arg), void *arg, const char *fmt, ...)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;

	if (NULL == sql) {
		return 0;
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
	}

	ALOGD("[%s:%d] sql=%s\n", __func__, __LINE__, sql);

	pthread_mutex_lock(&db_mutex_lock);

	/* Open an SQLite database */
	int rc = sqlite3_open(DB_NAME, &db);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] open database failed,rc=%d\n", __func__, __LINE__, rc);
		pthread_mutex_unlock(&db_mutex_lock);

		return 0;
	}

	/* Constructors for the [prepared statement] object */
	rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] database prepare fail,rc=%d\n", __func__, __LINE__, rc);
		goto DB_EXEC_FAIL;
	}

	/* */
	if (fmt) {
		va_list args;
		va_start(args, fmt);
		rc = db_stmt_bind_by_var(stmt, fmt, args);
		va_end(args);
		if (rc) {
			ALOGE("[%s:%d] database bind fail,rc=%d\n", __func__, __LINE__,
				rc);
			goto DB_EXEC_FAIL;
		}
	}

	if (create)
		rc = (*create)(stmt, arg);
	else
		rc = (sqlite3_step(stmt), 0);

	sqlite3_finalize(stmt);

	ALOGD("[%s:%d] exec_ok\n", __func__, __LINE__);

	goto DB_EXEC_OK;

DB_EXEC_FAIL:
	ALOGE("[%s:%d] db operator failed,rc=%d\n", __func__, __LINE__, rc);
	rc = 0;

DB_EXEC_OK:
	sqlite3_close(db);
	pthread_mutex_unlock(&db_mutex_lock);

	return rc;
}

/**
 * db_non_query_operator - Handling non-query operations.
 * @sqlstr	: SQL statement(Multiple statements by ; separate)
 * @bind	: Function that binds variable arguments.
 * @param	: The third parameter of the "bind" functon.
 * @bind.index	: SQL statement number.
 *
 * Execute a SQL statement with on return value.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_non_query_operator(const char *sqlstr, int (*bind)(sqlite3_stmt *,
			unsigned int index, void *arg), void *param)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;

	if (NULL == sqlstr) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return SQLITE_ERROR;
	}

	ALOGD("[%s:%d] sqlstr=%s\n", __func__, __LINE__, sqlstr);

	pthread_mutex_lock(&db_mutex_lock);

	int rc = sqlite3_open(DB_NAME, &db);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] open database failed,rc=%d\n", __func__, __LINE__, rc);
		pthread_mutex_unlock(&db_mutex_lock);
		return rc;
	}

	/* When synchronous is FULL (2), the SQLite database engine will use the xSync
	 * method of the VFS to ensure that all content is safely written to the disk
	 * surface prior to continuing. This ensures that an operating system crash or
	 * power failure will not corrupt the database. FULL synchronous is very safe,
	 * but it is also slower. When synchronous is NORMAL (1), the SQLite database
	 * engine will still sync at the most critical moments, but less often than in
	 * FULL mode. There is a very small (though non-zero) chance that a power failure
	 * at just the wrong time could corrupt the database in NORMAL mode. But in
	 * practice, you are more likely to suffer a catastrophic disk failure or some
	 * other unrecoverable hardware fault. With synchronous OFF (0), SQLite continues
	 * without syncing as soon as it has handed data off to the operating system.
	 * If the application running SQLite crashes, the data will be safe, but the
	 * database might become corrupted if the operating system crashes or the
	 * computer loses power before that data has been written to the disk surface.
	 * On the other hand, some operations are as much as 50 or more times faster
	 * with synchronous OFF.
	 */
	rc = sqlite3_exec(db, "PRAGMA synchronous=FULL;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] PRAGMA synchronous=FULL:ret=%d\n", __func__, __LINE__, rc);
		goto DB_BEGIN_FAIL;
	}

	rc = sqlite3_exec(db, "begin transaction;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] begin transaction:ret=%d\n", __func__, __LINE__, rc);
		goto DB_BEGIN_FAIL;
	}

	char sql[DB_SQL_MAX_LEN];
	int index = 0, offset = 0, n = 0;
	while (sqlstr[index] != 0) {
		offset = 0;
		do {
			if (offset >= DB_SQL_MAX_LEN) {
				ALOGE("[%s:%d] sql is too long,(%d)\n", __func__,
					__LINE__, offset);
				rc = SQLITE_ERROR;
				goto DB_EXEC_FAIL;
			}

			if ((sqlstr[index] != ';') && (sqlstr[index] != 0))
				sql[offset++] = sqlstr[index++];
			else {
				sql[offset] = '\0';

				if (sqlstr[index] == ';')
					index++;

				n++;
				break;
			}
		} while (1);

		ALOGD("[%s:%d] sql:%s\n", __func__, __LINE__, sql);

		rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			ALOGE("[%s:%d] rc=%d\n", __func__, __LINE__, rc);
			goto DB_EXEC_FAIL;
		}

		if (bind)
			rc = (*bind)(stmt, n, param);
		else
			rc = sqlite3_step(stmt);

		sqlite3_finalize(stmt);
		if ((rc != SQLITE_OK) && (rc != SQLITE_DONE)) {
			ALOGE("[%s:%d] bind\n", __func__, __LINE__);
			goto DB_EXEC_FAIL;
		}
	}

	rc = sqlite3_exec(db, "commit transaction;", 0, 0, NULL);
	if (rc) {
		ALOGE("[%s:%d] commit transaction:%d\n", __func__, __LINE__, rc);
		goto DB_EXEC_FAIL;
	}

	ALOGD("[%s:%d] exec_ok\n", __func__, __LINE__);

	goto DB_EXEC_OK;

DB_EXEC_FAIL:
	if (sqlite3_exec(db, "rollback transaction;", 0, 0, NULL))
		ALOGE("[%s:%d] rollback transaction error\n", __func__, __LINE__);

DB_BEGIN_FAIL:
	ALOGE("[%s:%d] db operator failed,rc=%d\n", __func__, __LINE__, rc);

DB_EXEC_OK:
	sqlite3_close(db);
	file_system_sync();
	pthread_mutex_unlock(&db_mutex_lock);

	return rc;
}

/**
 * db_non_query_operator_by_string_param - Handling non-query operations.
 * @sqlstr	: SQL statement(Multiple statements by ; separate)
 * @bind	: Function that binds variable arguments.
 * @param	: The third parameter of the "bind" functon.
 * @str		: SQL bind string.
 *
 * Execute a SQL statement with on return value.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_non_query_operator_by_string_param(const char *sqlstr,
	int (*bind)(sqlite3_stmt *, unsigned int index, void *arg, char *str),
	void *param, char *str)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;

	if (NULL == sqlstr) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return SQLITE_ERROR;
	}

	ALOGD("[%s:%d] sqlstr=%s\n", __func__, __LINE__, sqlstr);

	pthread_mutex_lock(&db_mutex_lock);

	int rc = sqlite3_open(DB_NAME, &db);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] open database failed,rc=%d\n", __func__, __LINE__, rc);
		pthread_mutex_unlock(&db_mutex_lock);
		return rc;
	}

	/* When synchronous is FULL (2), the SQLite database engine will use the xSync
	 * method of the VFS to ensure that all content is safely written to the disk
	 * surface prior to continuing. This ensures that an operating system crash or
	 * power failure will not corrupt the database. FULL synchronous is very safe,
	 * but it is also slower. When synchronous is NORMAL (1), the SQLite database
	 * engine will still sync at the most critical moments, but less often than in
	 * FULL mode. There is a very small (though non-zero) chance that a power failure
	 * at just the wrong time could corrupt the database in NORMAL mode. But in
	 * practice, you are more likely to suffer a catastrophic disk failure or some
	 * other unrecoverable hardware fault. With synchronous OFF (0), SQLite continues
	 * without syncing as soon as it has handed data off to the operating system.
	 * If the application running SQLite crashes, the data will be safe, but the
	 * database might become corrupted if the operating system crashes or the
	 * computer loses power before that data has been written to the disk surface.
	 * On the other hand, some operations are as much as 50 or more times faster
	 * with synchronous OFF.
	 */
	rc = sqlite3_exec(db, "PRAGMA synchronous=FULL;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] PRAGMA synchronous=FULL:ret=%d\n", __func__, __LINE__, rc);
		goto DB_BEGIN_FAIL;
	}

	rc = sqlite3_exec(db, "begin transaction;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] begin transaction:ret=%d\n", __func__, __LINE__, rc);
		goto DB_BEGIN_FAIL;
	}

	char sql[DB_SQL_MAX_LEN];
	int index = 0, offset = 0, n = 0;
	while (sqlstr[index] != 0) {
		offset = 0;
		do {
			if (offset >= DB_SQL_MAX_LEN) {
				ALOGE("[%s:%d] sql is too long,(%d)\n", __func__,
					__LINE__, offset);
				rc = SQLITE_ERROR;
				goto DB_EXEC_FAIL;
			}

			if ((sqlstr[index] != ';') && (sqlstr[index] != 0))
				sql[offset++] = sqlstr[index++];
			else {
				sql[offset] = '\0';

				if (sqlstr[index] == ';')
					index++;

				n++;
				break;
			}
		} while (1);

		ALOGD("[%s:%d] sql:%s\n", __func__, __LINE__, sql);

		rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			ALOGE("[%s:%d] rc=%d\n", __func__, __LINE__, rc);
			goto DB_EXEC_FAIL;
		}

		if (bind)
			rc = (*bind)(stmt, n, param, str);
		else
			rc = sqlite3_step(stmt);

		sqlite3_finalize(stmt);
		if ((rc != SQLITE_OK) && (rc != SQLITE_DONE)) {
			ALOGE("[%s:%d] bind\n", __func__, __LINE__);
			goto DB_EXEC_FAIL;
		}
	}

	rc = sqlite3_exec(db, "commit transaction;", 0, 0, NULL);
	if (rc) {
		ALOGE("[%s:%d] commit transaction:%d\n", __func__, __LINE__, rc);
		goto DB_EXEC_FAIL;
	}

	ALOGD("[%s:%d] exec_ok\n", __func__, __LINE__);

	goto DB_EXEC_OK;

DB_EXEC_FAIL:
	if (sqlite3_exec(db, "rollback transaction;", 0, 0, NULL))
		ALOGE("[%s:%d] rollback transaction error\n", __func__, __LINE__);

DB_BEGIN_FAIL:
	ALOGE("[%s:%d] db operator failed,rc=%d\n", __func__, __LINE__, rc);

DB_EXEC_OK:
	sqlite3_close(db);
	file_system_sync();
	pthread_mutex_unlock(&db_mutex_lock);

	return rc;
}

/**
 * db_non_query_by_var_param - Handling non-query operations.
 * @sql	: A pointer to the SQL string.
 * @fmt	: Format string.
 * @...	: Variable parameter.
 *
 * Execute a SQL statement with a variable argument, without a return value.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_non_query_by_var_param(const char *sql, const char *fmt, ...)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;

	if (NULL == sql)
		return SQLITE_ERROR;

	pthread_mutex_lock(&db_mutex_lock);

	int rc = sqlite3_open(DB_NAME, &db);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] open database failed,rc=%d\n", __func__, __LINE__, rc);
		pthread_mutex_unlock(&db_mutex_lock);
		return rc;
	}

	/* When synchronous is FULL (2), the SQLite database engine will use the xSync
	 * method of the VFS to ensure that all content is safely written to the disk
	 * surface prior to continuing. This ensures that an operating system crash or
	 * power failure will not corrupt the database. FULL synchronous is very safe,
	 * but it is also slower. When synchronous is NORMAL (1), the SQLite database
	 * engine will still sync at the most critical moments, but less often than in
	 * FULL mode. There is a very small (though non-zero) chance that a power failure
	 * at just the wrong time could corrupt the database in NORMAL mode. But in
	 * practice, you are more likely to suffer a catastrophic disk failure or some
	 * other unrecoverable hardware fault. With synchronous OFF (0), SQLite continues
	 * without syncing as soon as it has handed data off to the operating system.
	 * If the application running SQLite crashes, the data will be safe, but the
	 * database might become corrupted if the operating system crashes or the
	 * computer loses power before that data has been written to the disk surface.
	 * On the other hand, some operations are as much as 50 or more times faster
	 * with synchronous OFF.
	 */
	rc = sqlite3_exec(db, "PRAGMA synchronous=FULL;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] PRAGMA synchronous=FULL:ret=%d\n", __func__, __LINE__, rc);
		goto DB_EXEC_FAIL;
	}

	ALOGD("[%s:%d] sql:%s\n", __func__, __LINE__, sql);

	rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] prepare\n", __func__, __LINE__);
		goto DB_EXEC_FAIL;
	}

	if (fmt) {
		va_list args;
		va_start(args, fmt);
		rc = db_stmt_bind_by_var(stmt, fmt, args);
		va_end(args);
		if (rc)
			goto DB_EXEC_FAIL;
	}

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if ((rc != SQLITE_OK) && (rc != SQLITE_DONE)) {
		ALOGE("[%s:%d] bind\n", __func__, __LINE__);
		goto DB_EXEC_FAIL;
	}

	rc = 0;

	ALOGD("[%s:%d] exec_ok\n", __func__, __LINE__);

	goto DB_EXEC_OK;

DB_EXEC_FAIL:
	ALOGE("[%s:%d] db operator failed,rc=%d\n", __func__, __LINE__, rc);

DB_EXEC_OK:
	sqlite3_close(db);
	file_system_sync();
	pthread_mutex_unlock(&db_mutex_lock);

	return rc;
}

/**
 * db_non_query_by_transaction - Handling non-query operations by a transaction.
 * @exec_sqls : Function that executes SQL.
 * @arg	: The second parameter of the "exec_sqls" functon..
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_non_query_by_transaction(int (*exec_sqls)(sqlite3 *db, void *arg),
				void *arg)
{
	sqlite3 *db = NULL;

	pthread_mutex_lock(&db_mutex_lock);

	int rc = sqlite3_open(DB_NAME, &db);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] open database failed,rc=%d\n", __func__, __LINE__, rc);
		pthread_mutex_unlock(&db_mutex_lock);
		return rc;
	}

	/* When synchronous is FULL (2), the SQLite database engine will use the xSync
	 * method of the VFS to ensure that all content is safely written to the disk
	 * surface prior to continuing. This ensures that an operating system crash or
	 * power failure will not corrupt the database. FULL synchronous is very safe,
	 * but it is also slower. When synchronous is NORMAL (1), the SQLite database
	 * engine will still sync at the most critical moments, but less often than in
	 * FULL mode. There is a very small (though non-zero) chance that a power failure
	 * at just the wrong time could corrupt the database in NORMAL mode. But in
	 * practice, you are more likely to suffer a catastrophic disk failure or some
	 * other unrecoverable hardware fault. With synchronous OFF (0), SQLite continues
	 * without syncing as soon as it has handed data off to the operating system.
	 * If the application running SQLite crashes, the data will be safe, but the
	 * database might become corrupted if the operating system crashes or the
	 * computer loses power before that data has been written to the disk surface.
	 * On the other hand, some operations are as much as 50 or more times faster
	 * with synchronous OFF.
	 */
	rc = sqlite3_exec(db, "PRAGMA synchronous=FULL;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] PRAGMA synchronous=FULL:ret=%d\n", __func__, __LINE__, rc);
		goto DB_BEGIN_FAIL;
	}

	rc = sqlite3_exec(db, "begin transaction;", 0, 0, NULL);
	if (rc != SQLITE_OK) {
		ALOGE("[%s:%d] begin transaction:%d\n", __func__, __LINE__, rc);
		goto DB_BEGIN_FAIL;
	}

	if (exec_sqls)
		rc = (*exec_sqls)(db, arg);
	else
		rc = SQLITE_ERROR;

	if ((rc != SQLITE_OK) && (rc != SQLITE_DONE)) {
		ALOGE("[%s:%d] prepare,error,%d\n", __func__, __LINE__, rc);
		goto DB_EXEC_FAIL;
	}

	rc = sqlite3_exec(db, "commit transaction;", 0, 0, NULL);
	if (rc) {
		ALOGE("[%s:%d] commit transaction:%d\n", __func__, __LINE__, rc);
		goto DB_EXEC_FAIL;
	}

	ALOGD("[%s:%d] exec_ok\n", __func__, __LINE__);

	goto DB_EXEC_OK;

DB_EXEC_FAIL:
	if (sqlite3_exec(db, "rollback transaction;", 0, 0, NULL))
		ALOGE("[%s:%d] rollback transaction:error\n", __func__, __LINE__);

DB_BEGIN_FAIL:
	ALOGE("[%s:%d] db operator failed,rc=%d\n", __func__, __LINE__, rc);

DB_EXEC_OK:
	sqlite3_close(db);
	file_system_sync();
	pthread_mutex_unlock(&db_mutex_lock);

	return rc;
}

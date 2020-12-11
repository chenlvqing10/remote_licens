/*
 * db_sqlite_middle.h - Interface for the sqlite middleware
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

#ifndef __SQLITE_MIDDLE_H
#define __SQLITE_MIDDLE_H

#include "sqlite3.h"

#define DB_SQL_MAX_LEN 2048

/**
 * db_stmt_get_blob - Result BLOB values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object..
 * @index: Is the index of the column for which information should be returned.
 * @out:
 *
 * Secondary encapsulation of "sqlite3_column_blob"
 *
 * Returns BLOB result.
 */
int db_stmt_get_blob(sqlite3_stmt *stmt, int index, unsigned char *out);
int db_stmt_get_blob_v2(sqlite3_stmt *stmt,
						int index, unsigned char *out, int size);
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
int db_stmt_get_text(sqlite3_stmt *stmt, int index, char *out);
int db_stmt_get_text_v2(sqlite3_stmt *stmt,
						int index, char *out, int size);
int db_stmt_get_text_v3(sqlite3_stmt *stmt, int index, char **out_ptr);
/**
 * db_stmt_get_int - Result int values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 *
 * Secondary encapsulation of "sqlite3_column_int"
 *
 * Returns int value.
 */
int db_stmt_get_int(sqlite3_stmt *stmt, int index);

/**
 * db_stmt_get_double - Result double values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 *
 * Secondary encapsulation of "sqlite3_column_double"
 *
 * Returns double value.
 */
double db_stmt_get_double(sqlite3_stmt *stmt, int index);

/**
 * db_stmt_get_int64 - Result int64 values from a query.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Is the index of the column for which information should be returned.
 *
 * Secondary encapsulation of "sqlite3_column_double"
 *
 * Returns double value.
 */
long long db_stmt_get_int64(sqlite3_stmt *stmt, int index);

/**
 * db_query_result_count - Get the number of results of the query.
 * @sql:  A pointer to the SQL string.
 *
 * Get the first row of the first column of the query results.
 *
 * Returns number of data queried.
 */
int db_query_result_count(const char *sql);

/**
 * db_query_result_count_by_name - Get the number of results of the query.
 * @sql:  A pointer to the SQL string.
 * @name:  A pointer to the name string.
 *
 * Get the first row of the first column of the query results.
 *
 * Returns number of data queried.
 */
int db_query_result_count_by_name(const char *sql, char *name);

/**
 * db_query_result_min_punchtime - Get min punchtime of punch_record.
 * @sql:  A pointer to the SQL string.
 *
 * Get the first row of the first column of the query results.
 *
 * Returns number of data queried.
 */
double db_query_result_min_punchtime(const char *sql);

/**
 * db_stmt_bind_by_string - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_stmt_bind_by_string(sqlite3_stmt *stmt, unsigned int index, void *arg);

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
			void *arg), void *arg, const char *fmt, ...);

/**
 * db_non_query_operator - Handling non-query operations.
 * @sqlstr	: SQL statement(Multiple statements by ; separate)
 * @bind	: Function that binds variable arguments.
 * @param	: The third parameter of the "bind" functon.
 * @bind.index	: SQL statement number.
 *
 * Execute a SQL statement with on return value.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_non_query_operator(const char *sqlstr, int (*bind)(sqlite3_stmt *,
			unsigned int index, void *arg), void *param);

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
	void *param, char *str);

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
int db_non_query_by_var_param(const char *sql, const char *fmt, ...);

/**
 * db_non_query_by_transaction - Handling non-query operations by a transaction.
 * @exec_sqls : Function that executes SQL.
 * @arg	: The second parameter of the "exec_sqls" functon..
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_non_query_by_transaction(int (*exec_sqls)(sqlite3 *db, void *arg),
				void *arg);

#endif /* __SQLITE_MIDDLE_H */

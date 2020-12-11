/*
 * db_persistence.c - Interface code to access the database
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sqlite3.h"
#include "db_persistence.h"

#define LOG_TAG "SQLITE_MIDDLE"
#include <log/log.h>

#define BEGIN		"BEGIN"
#define COMMIT		"COMMIT"
#define ROLLBACK	"ROLLBACK"

/* The error logger callback function might look something like this */
static void error_log_call_back(void *p_arg, int i_err_code, const char *p_msg)
{
	ALOGD("(%d) %s\n", i_err_code, p_msg);
}

/**
 * db_config - Configure the database.
 *
 * Called before the db_open function.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_config(void)
{
	/* The error logging callback is registered at start-time using C-code */
	sqlite3_config(SQLITE_CONFIG_LOG, error_log_call_back, NULL);

	return SQLITE_OK;
}

/**
 * db_open - Opening a new Database connection
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_open(db_t *this)
{
	void *p_data = NULL;
	int ret = -1;
	int sql_rc = SQLITE_ERROR;

	/* The error logging callback is registered at start-time using C-code */
	sqlite3_config(SQLITE_CONFIG_LOG, error_log_call_back, p_data);

	sql_rc = sqlite3_open(this->db_name, &(this->db_handle));
	if (sql_rc != SQLITE_OK) {
		ALOGE("[%s:%d] Can't open database: %s\n", __func__, __LINE__,
			sqlite3_errmsg(this->db_handle));

		sqlite3_close(this->db_handle);
		ret = -1;
		this->is_open = FALSE;

	} else {
		ret = 0;
		this->is_open = TRUE;
	}

	return ret;
}

static int db_close_proc(db_t *this)
{
	sqlite3_stmt *stmt;

	int sql_rc = sqlite3_close(this->db_handle);

	while (SQLITE_BUSY == sql_rc) {
		sql_rc = SQLITE_OK;

		stmt = sqlite3_next_stmt(this->db_handle, NULL);
		if (NULL == stmt)
			break;

		sql_rc = sqlite3_finalize(stmt);
		if (SQLITE_OK == sql_rc)
			sql_rc = sqlite3_close(this->db_handle);
	}

	return sql_rc;
}

/**
 * db_close - Disconnect the database
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_close(db_t *this)
{
	int sql_rc = SQLITE_ERROR;
	int ret = -1;

	if (NULL == this->db_handle)
		return 0;

	sql_rc = db_close_proc(this);
	if (SQLITE_OK == sql_rc) {
		this->db_handle = NULL;
		this->is_open = FALSE;
		ret = 0;
	}

	return ret;
}

/**
 * db_execute - Execute SQL(Update, Delete,Insert,Create)
 * @this: Pointer to a database structure
 * @str_sql: SQL
 *
 * Returns 0 if success, otherwise -1.
 */
int db_execute(db_t *this, const char *str_sql)
{
	int sql_rc;
	char *str_err_msg = NULL;
	int ret;

	sql_rc = sqlite3_exec(this->db_handle, str_sql, NULL, NULL, &str_err_msg);
	if (sql_rc != SQLITE_OK) {
		ALOGE("[%s:%d] SQL error: %s\n", __func__, __LINE__, str_err_msg);

		sqlite3_free(str_err_msg);
		ret = -1;
	} else
		ret = 0;

	return ret;
}

/**
 * db_transaction_begin - begin transaction
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_transaction_begin(db_t *this)
{
	return db_execute(this, BEGIN);
}

/**
 * db_transaction_commit -  Commit transaction
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_transaction_commit(db_t *this)
{
	return db_execute(this, COMMIT);
}

/**
 * db_transaction_rollback -  Rollback transaction
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_transaction_rollback(db_t *this)
{
	return db_execute(this, ROLLBACK);
}

/**
 * db_init -  Constructor
 * @this: Pointer to a database structure
 * @name: Database name
 *
 * Returns 0 if success, otherwise -1.
 */
int _db_init(db_t *this, const char *name)
{
	int ret = -1;

	assert((this != NULL) && (name != NULL));

	memset(this->db_name, 0, DB_NAME_MAX_LENGTH);
	memcpy(this->db_name, name, DB_NAME_MAX_LENGTH);

	this->db_handle = NULL;
	this->is_open = FALSE;

	ret = db_open(this);

	return ret;
}

/**
 * db_deinit -  Destructor
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int _db_deinit(db_t *this)
{
	int ret = -1;

	ret = db_close(this);

	memset(this->db_name, 0, DB_NAME_MAX_LENGTH);

	return ret;
}


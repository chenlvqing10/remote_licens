/*
 * db_persistence.h - Define the interface to access the database
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

#ifndef __DB_PERSISTENCE_H
#define __DB_PERSISTENCE_H

#include <stdio.h>
#include <string.h>

#include "sqlite3.h"

#define DB_NAME_MAX_LENGTH	128
#define TRUE			1
#define FALSE			0


#ifdef _WIN32

#else
    /*typedef int bool;*/
#endif

typedef void(* err_cb)(void*, int, const char*);
typedef struct tag_db_t {
	char db_name[DB_NAME_MAX_LENGTH];		/* Database file name */
	sqlite3 *db_handle;				/* Database handle */
int is_open;
} db_t;

/**
 * db_config - Configure the database.
 *
 * Called before the db_open function.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_config(void);
/**
 * db_custom_config - Configure the database error process func.
 *
 * Called before the db_open function.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_custom_config(err_cb cb);

/**
 * db_open - Opening a new Database connection
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_open(db_t *);

/**
 * db_close - Disconnect the database
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_close(db_t *);

/**
 * db_execute - Execute SQL(Update, Delete,Insert,Create)
 * @this: Pointer to a database structure
 * @str_sql: SQL
 *
 * Returns 0 if success, otherwise -1.
 */
int db_execute(db_t *, const char *str_sql);

/**
 * db_transaction_begin - begin transaction
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_transaction_begin(db_t *);

/**
 * db_transaction_commit -  Commit transaction
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_transaction_commit(db_t *);

/**
 * db_transaction_rollback -  Rollback transaction
 * @this: Pointer to a database structure
 *
 * Returns 0 if success, otherwise -1.
 */
int db_transaction_rollback(db_t *);

#endif /* __DB_PERSISTENCE_H */

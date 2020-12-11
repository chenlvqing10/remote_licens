/*
 * external_import_pic.c - Interface for import external images
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
#include <stdlib.h>
#include <assert.h>

#include "db_sqlite_middle.h"
#include "db_external_import_pic.h"

#define LOG_TAG "API_DB"
#include <log/log.h>
#include "db.h"

/**
 * external_import_pic_free_queue - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns none.
 */
static void external_import_pic_free_queue(ee_queue_t *h)
{
	ee_queue_t *head = h, *pos, *n;
	external_import_pic_t *p = NULL;

	assert(h != NULL);

	ee_queue_for_each_safe(pos, n, head) {
		p = ee_queue_data(pos, external_import_pic_t, queue);
		free(p);
	}

	ee_queue_init(head);
}

/**
 * external_import_pic_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int external_import_pic_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
	void *arg)
{
	int rc;
	external_import_pic_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_text(stmt, 1, param->pic_name, strlen(param->pic_name), NULL);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		return rc;

	return SQLITE_OK;
}

/**
 * external_import_pic_create_queue - Database query operations, with variable parameter
 * @stmt : .
 * @head : .
 *
 * Returns number of data queried.
 */
static int external_import_pic_create_queue(sqlite3_stmt *stmt, void *head)
{
	int count = 0;
	ee_queue_t *h = head;
	ee_queue_init(h);
	external_import_pic_t *u;

	assert((stmt != NULL) && (head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		u = calloc(sizeof(external_import_pic_t), 1);
		if (!u) {
			ALOGE("[%s:%d] create external_import_pic_t faile\n", __func__,
				__LINE__);

			goto CALLOC_FAIL;
		}

		u->id = db_stmt_get_int(stmt, 0);
		db_stmt_get_text(stmt, 1, u->pic_name);

		ee_queue_insert_tail(h, &(u->queue));

		count++;
	}

	return count;

CALLOC_FAIL:
	external_import_pic_free_queue(h);

	return 0;
}

/**
 * external_import_pic_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void external_import_pic_print(ee_queue_t *head)
{
	external_import_pic_t *q = NULL;

	assert(head != NULL);

	ee_queue_foreach(q, head, external_import_pic_t, queue) {
		ALOGD("[%s:%d] id=%d pic_name=%s\n", __func__, __LINE__, q->id,
			q->pic_name);
	}
}

/**
 * db_external_import_pic_insert - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_insert(external_import_pic_t *p_info)
{
	char str_sql[] = "INSERT INTO external_import_pic(pic_name) VALUES(?);";

	assert(p_info != NULL);

	return dbname_non_query_operator(db_get_face_recg_db_name(), str_sql,
		external_import_pic_bind_stmt, p_info);
}

/**
 * db_external_import_pic_delete - Delete an pic.
 * @user_name: A pointer to username string.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_delete(char *pic_name)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(pic_name != NULL);

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql, "DELETE FROM external_import_pic WHERE pic_name='%s';",
		pic_name);

	return dbname_non_query_operator(db_get_face_recg_db_name(), str_sql, NULL, NULL);
}

/**
 * db_external_import_pic_update - Update pic by name.
 * @user_name: A pointer to username string.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_update(char *pic_name,
	external_import_pic_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert((pic_name != NULL) && (p_info != NULL));

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql,
		"UPDATE external_import_pic SET pic_name=? WHERE pic_name=\'%s\';",
		pic_name
		);

	return dbname_non_query_operator(db_get_face_recg_db_name(), str_sql, external_import_pic_bind_stmt,
		p_info);
}

/**
 * db_external_import_pic_query_begin - Query pic.
 * @user_name: A pointer to username string.
 * @head: A pointer to the head of the queue.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_query_begin(ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(head != NULL);

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql,
		sizeof(str_sql),
		"SELECT * FROM external_import_pic ORDER BY id;");

	return dbname_query_by_var_param(db_get_face_recg_db_name(), str_sql,
		external_import_pic_create_queue, head, NULL);
}

/**
 * db_external_import_pic_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_external_import_pic_query_end(ee_queue_t *head)
{
	assert(head != NULL);

	external_import_pic_free_queue(head);

	return 0;
}


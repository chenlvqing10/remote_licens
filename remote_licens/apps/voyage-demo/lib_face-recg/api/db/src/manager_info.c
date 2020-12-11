/*
 * manager_info.h - Interface for management administrator
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
#include "ee_queue.h"
#include "db_manager_info.h"
#include "db_config.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * manager_info_free_queue - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns none.
 */
static void manager_info_free_queue(ee_queue_t *h)
{
	ee_queue_t *head = h, *pos, *n;
	manager_info_t *p = NULL;

	assert(h != NULL);

	ee_queue_for_each_safe(pos, n, head) {
		p = ee_queue_data(pos, manager_info_t, queue);
		free(p);
	}

	ee_queue_init(head);
}

/**
 * manager_info_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int manager_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index, void *arg)
{
	int rc;
	manager_info_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_text(stmt, 1, param->user_name, strlen(param->user_name),
				NULL);
	sqlite3_bind_text(stmt, 2, param->user_password,
				strlen(param->user_password), NULL);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		return rc;

	return SQLITE_OK;
}

/**
 * manager_info_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void manager_info_print(ee_queue_t *head)
{
	manager_info_t *q = NULL;

	assert(head != NULL);

	ee_queue_foreach(q, head, manager_info_t, queue) {
		ALOGD("[%s:%d] userid:%d user_name:%s user_password:%s\n", __func__,
			__LINE__, q->id, q->user_name, q->user_password);
	}
}

/**
 * manager_info_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_insert(manager_info_t *p_info)
{
	int ret = 0;
	assert(p_info != NULL);

	ret += db_config_set_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_USERNAME,
		p_info->user_name);
	ret += db_config_set_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_PASSWORD,
		p_info->user_password);

	return ret;
}

/**
 * db_manager_info_delete - Delete an administrator user by name.
 * @user_name: A pointer to username string.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_delete(char *user_name)
{
	int ret = 0;

	assert(user_name != NULL);

	ret += db_config_set_string(DBCONF_MANAGERINFO,
			DBCONF_MANAGERINFO_USERNAME,
			"");
	ret += db_config_set_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_PASSWORD,
		"");

	return ret;
}

/**
 * db_manager_info_update - Update administrator information by name.
 * @user_name: A pointer to username string.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_update(char *user_name, manager_info_t *p_info)
{
	int ret = 0;

	assert((user_name != NULL) && (p_info != NULL));

	ret += db_config_set_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_USERNAME,
		p_info->user_name);
	ret += db_config_set_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_PASSWORD,
		p_info->user_password);

	return ret;
}

/**
 * db_manager_info_query_begin - Query administrator information.
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, Query all administrator information.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_query_begin(char *user_name, ee_queue_t *head)
{

	assert(head != NULL);

	ee_queue_t *h = head;
	ee_queue_init(h);
	manager_info_t *u;

	u = calloc(sizeof(manager_info_t), 1);
	db_config_get_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_USERNAME,
		u->user_name,
		sizeof(u->user_name),
		DBCONF_MANAGERINFO_USERNAME_VALUE);
	db_config_get_string(DBCONF_MANAGERINFO,
		DBCONF_MANAGERINFO_PASSWORD,
		u->user_password,
		sizeof(u->user_password),
		DBCONF_MANAGERINFO_PASSWORD_VALUE);
	ee_queue_insert_tail(h, &(u->queue));

	return 1;
}

/**
 * db_manager_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_manager_info_query_end(ee_queue_t *h)
{
	assert(h != NULL);

	manager_info_free_queue(h);

	return 0;
}


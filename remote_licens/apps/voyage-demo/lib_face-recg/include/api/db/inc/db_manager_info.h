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

#ifndef __MANAGER_INFO_H
#define __MANAGER_INFO_H

#include "ee_queue.h"

typedef struct tag_manager_info {
	/* id. */
	int id;

	/* The person ID of the manager */
	int fk_per_id;

	/* Administrator name.
	 *
	 * The default value is "root".
	 * for security, this string is SHA256 encoded and then stored in
	 *   the database
	 */
	char user_name[32];

	/* Administrator password.
	 * The default value is "12345678"
	 * for security, this string is SHA256 encoded and then stored in
	 *   the database
	 */
	char user_password[32];

	/* Bidirectional queue */
	ee_queue_t queue;
} manager_info_t;

/**
 * db_manager_info_update - Update administrator information by name.
 * @user_name: A pointer to username string.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_update(char *user_name, manager_info_t *p_info);

/**
 * manager_info_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_insert(manager_info_t *p_info);

/**
 * db_manager_info_delete - Delete an administrator user.
 * @user_name: A pointer to username string.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_delete(char *user_name);

/**
 * db_manager_info_query_begin - Query administrator information.
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, Query all administrator information.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_manager_info_query_begin(char *user_name, ee_queue_t *head);

/**
 * db_manager_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_manager_info_query_end(ee_queue_t *h);

/**
 * manager_info_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void manager_info_print(ee_queue_t *head);

#endif /* __MANAGER_INFO_H */


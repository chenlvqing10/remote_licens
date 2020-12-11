/*
 * db_person_info.h - Interface for management personnel registration
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

#ifndef __DB_PERSON_INFO_H
#define __DB_PERSON_INFO_H

#include "ee_queue.h"
#include "common_person_info.h"

/**
 * person_info_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void person_info_print(ee_queue_t *head);

/**
 * db_person_info_update - Update user information by name.
 * @user_name: A pointer to username string.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_person_info_update(char *user_name, person_info_t *p_info);

/**
 * db_person_info_insert - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_person_info_insert(person_info_t *p_info);

/**
 * db_person_info_delete - Delete an user by name.
 * @user_name: A pointer to username string.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_person_info_delete(char *user_name);

/**
 * db_person_info_delete_all - Delete all users .
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_person_info_delete_all();

/**
 * db_person_info_query_by_id - Query personnel information by person id.
 * @person_id: The person id to be queried.
 * @p_info: Out, Pointer to 'person_info_t'.
 *
 * Returns number of data queried.
 */
int db_person_info_query_by_id(unsigned int person_id, person_info_t *p_info);

/**
 * db_person_info_query_by_employee_number - Find person information by
 *	employee number.
 * @person_id: The person id to be queried.
 * @p_info: Out, Pointer to 'person_info_t'.
 *
 * Returns number of data queried.
 */
int db_person_info_query_by_employee_number(char *employee_number,
	person_info_t *p_info);

/**
 * db_person_info_count - Get the number of users.
 *
 * Returns number of users.
 */
int db_person_info_count();

/**
 * db_person_info_query_begin - Query user information by name or starting id
 * and count.
 * @user_name: A pointer to username string.
 * @start_id: The start id of the query.
 * @count: The number of data to be queried starting from start_id.
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, Query all user information.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_person_info_query_begin(char *user_name, unsigned int start_id,
			unsigned int count, ee_queue_t *head);

/**
 * db_person_info_query_begin_by_name - Query user information by name
 * and count.
 * @user_name: A pointer to username string.
 * @head: A pointer to the head of the queue.
 *
 * Returns number of data queried.
 */
int db_person_info_query_begin_by_name(char *user_name, ee_queue_t *head);

int db_person_info_query_max_id();

/**
 * db_manager_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_person_info_query_end(ee_queue_t *head);

#endif /* __DB_PERSON_INFO_H */


/*
 * punch_record.h - Interface for management punch record
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

#ifndef __PUNCH_RECORD_H
#define __PUNCH_RECORD_H

#include "ee_queue.h"
#include "common_record_info.h"

/**
 * punch_record_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void punch_record_print(ee_queue_t *head);

/**
 * db_punch_record_update - Update user information by name.
 * @id: Records pointed to by ID.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_update(unsigned int id, punch_record_t *p_info);

/**
 * db_punch_record_insert - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_insert(punch_record_t *p_info);

 /**
 * db_punch_record_delete_by_id - Delete punch record by record id.
 * @id: The punch record id to be queried.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_delete_by_id(int id);

/**
 * db_punch_record_delete - Delete an user by name.
 * @user_name: A pointer to username string.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_delete(char *user_name);

/**
 * db_punch_record_delete_all - Delete all punch record.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_delete_all();

/**
 * db_punch_record_count - Get the number of punch record.
 * @user_name: A pointer to username string.
 * @start_time: Record the start time of punching(ms).
 * @end_time: Record the end time of punching(ms).
 *
 * Returns number of punch record.
 */
int db_punch_record_count(char *user_name, int punch_pass, double start_time,
			double end_time);

/**
 * db_punch_record_count_all - Get the number of all punch record.
 *
 * Returns number of punch record.
 */
int db_punch_record_count_all();

/**
 * db_punch_record_query_begin - Query punch record information
 * @user_name: A pointer to username string.
 * @punch_pass Punch pass flag
 * @start_time: Record the start time of punching(ms).
 * @end_time: Record the end time of punching(ms).
 * @start_id: The start id of the query.
 * @count: The number of data to be queried starting from start_id.
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, query the punch records of all users.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_query_begin(char *user_name, int punch_pass,
			double start_time, double end_time, unsigned int start_id,
			unsigned int count, ee_queue_t *head);

/**
 * db_punch_record_query_begin_by_less_timestamp - Query punch record information by
 * less time.
 * @time: Record before the time of punching(ms).
 * @head: A pointer to the head of the queue.
  *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_query_begin_by_less_timestamp(double time,
			ee_queue_t *head);

/**
 * db_punch_record_query_begin_by_between_timestamp - Query punch record information
 * by less time.
 * @start_time: Record start the time of punching(ms).
 * @end_time: Record end the time of punching(ms).
 * @head: A pointer to the head of the queue.
  *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_query_begin_by_between_timestamp(
	double start_time, double end_time, ee_queue_t *head);

/**
 * db_punch_record_query_min_punch_time - Query min punch time value.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
double db_punch_record_query_min_punch_time();

/**
 * db_punch_record_full_info_query_begin - Query punch record information
 * @user_name: A pointer to username string.
 * @punch_pass Punch pass flag
 * @start_time: Record the start time of punching(ms).
 * @end_time: Record the end time of punching(ms).
 * @start_id: The start id of the query.
 * @count: The number of data to be queried starting from start_id.
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, query the punch records of all users.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_full_info_query_begin(char *user_name, int punch_pass,
			double start_time,
			double end_time, unsigned int start_id,
			unsigned int count, ee_queue_t *head);

/**
 * db_punch_record_full_info_query_begin_by_name - Query punch record information
 * @user_name: A pointer to username string.
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, query none punch records.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_full_info_query_begin_by_name(char *user_name,
			ee_queue_t *head);

/**
 * db_punch_record_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_query_end(ee_queue_t *head);

/**
 * db_punch_record_full_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_full_info_query_end(ee_queue_t *head);

/**
 * db_punch_record_full_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_full_info_query_end_for_cloud(ee_queue_t *head);

/**
 * db_punch_record_full_info_query_cloud_begin_for_cloud - Query punch
 * record information
 * @count: count of query record
 * @head: A pointer to the head of the queue.
 *
 * If user_name is an empty string, query the punch records of all users.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_full_info_query_cloud_begin_for_cloud(int count,
		ee_queue_t *head);

#endif /* __PUNCH_RECORD_H */


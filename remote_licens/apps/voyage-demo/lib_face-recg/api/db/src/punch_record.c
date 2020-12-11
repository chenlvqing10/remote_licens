/*
 * punch_record.c - Interface for management punch record
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
#include <time.h>

#include "db_sqlite_middle.h"
#include "ee_queue.h"
#include "db_punch_record.h"
#include "record_info.h"
#include "common_person_info.h"
#include "file_ops.h"
#include "time_conv.h"
#include "db.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * punch_record_free_queue - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns none.
 */
static void punch_record_free_queue(ee_queue_t *h)
{
	ee_queue_t *head = h, *pos, *n;
	punch_record_t *p = NULL;

	assert(h != NULL);

	ee_queue_for_each_safe(pos, n, head) {
		p = ee_queue_data(pos, punch_record_t, queue);
		free(p);
	}

	ee_queue_init(head);
}

/**
 * punch_record_free_queue - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns none.
 */
static void punch_record_full_info_free_queue(ee_queue_t *h)
{
	ee_queue_t *head = h, *pos, *n;
	full_punch_rec_info_t *p = NULL;

	assert(h != NULL);

	ee_queue_for_each_safe(pos, n, head) {
		p = ee_queue_data(pos, full_punch_rec_info_t, queue);
		free(p);
	}

	ee_queue_init(head);
}

/**
 * punch_record_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int punch_record_bind_stmt(sqlite3_stmt *stmt, unsigned int index, void *arg)
{
	int rc;
	punch_record_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_int(stmt, 1, param->fk_per_id);
	sqlite3_bind_double(stmt, 2, param->match_score);
	sqlite3_bind_text(stmt, 3, param->address,
			strlen(param->address), NULL);
	sqlite3_bind_double(stmt, 4, param->punch_time);
	sqlite3_bind_text(stmt, 5, param->punch_live_large_pic_name,
				strlen(param->punch_live_large_pic_name), NULL);
	sqlite3_bind_text(stmt, 6, param->punch_live_small_pic_name,
			strlen(param->punch_live_small_pic_name), NULL);
	sqlite3_bind_int(stmt, 7, param->punch_pass);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}

	return SQLITE_OK;
}

/**
 * punch_record_create_queue - Database query operations, with variable parameter
 * @stmt : A pointer to the [sqlite3_stmt] object.
 * @head : A pointer to the head of the queue.
 *
 * Returns number of data queried.
 */
static int punch_record_create_queue(sqlite3_stmt *stmt, void *head)
{
	ee_queue_t *h = head;
	ee_queue_init(h);
	punch_record_t *u;
	int count = 0;

	assert((stmt != NULL) && (head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		u = calloc(sizeof(punch_record_t), 1);
		if (!u) {
			ALOGE("[%s:%d] create punch_record_t faile\n", __func__,
				__LINE__);
			goto CALLOC_FAIL;
		}

		u->id = db_stmt_get_int(stmt, 0);
		u->fk_per_id = db_stmt_get_int(stmt, 1);
		u->match_score = db_stmt_get_double(stmt, 2);
		db_stmt_get_text(stmt, 3, u->address);
		u->punch_time = db_stmt_get_double(stmt, 4);
		db_stmt_get_text(stmt, 5, u->punch_live_large_pic_name);
		db_stmt_get_text(stmt, 6, u->punch_live_small_pic_name);
		u->punch_pass = db_stmt_get_int(stmt, 7);

		ee_queue_insert_tail(h, &(u->queue));

		count++;
	}

	return count;

CALLOC_FAIL:
	punch_record_free_queue(h);

	return 0;
}

/**
 * punch_record_full_info_create_queue - Database query operations,
 * with variable parameter
 * @stmt : A pointer to the [sqlite3_stmt] object.
 * @head : A pointer to the head of the queue.
 *
 * Returns number of data queried.
 */
static int punch_record_full_info_create_queue(sqlite3_stmt *stmt,
	void *head)
{
	ee_queue_t *h = head;
	ee_queue_init(h);
	full_punch_rec_info_t *u;
	int count = 0;

	assert((stmt != NULL) && (head != NULL));
	while (SQLITE_ROW == sqlite3_step(stmt)) {
		u = calloc(sizeof(full_punch_rec_info_t), 1);
		if (!u) {
			ALOGE("[%s:%d] create full_punch_rec_info_t faile\n", __func__,
				__LINE__);
			goto CALLOC_FAIL;
		}

		db_stmt_get_text(stmt, 0, u->employee_number);
		db_stmt_get_text(stmt, 1, u->name);
		db_stmt_get_text(stmt, 2, u->gender);
		db_stmt_get_text(stmt, 3, u->id_card_number);
		db_stmt_get_text(stmt, 4, u->department_name);
		db_stmt_get_text(stmt, 5, u->regist_photo_name);
		u->match_score = db_stmt_get_double(stmt, 6);
		db_stmt_get_text(stmt, 7, u->address);
		u->punch_time = db_stmt_get_double(stmt, 8);
		db_stmt_get_text(stmt, 9, u->punch_live_large_pic_name);
		db_stmt_get_text(stmt, 10, u->punch_live_small_pic_name);
		u->punch_pass = db_stmt_get_int(stmt, 11);

		ee_queue_insert_tail(h, &(u->queue));

		count++;
	}

	ALOGD("[%s:%d]count=%d\n", __func__, __LINE__, count);
	return count;

CALLOC_FAIL:
	punch_record_full_info_free_queue(h);
	ALOGD("[%s:%d]CALLOC_FAIL count=%d\n", __func__, __LINE__, count);
	return 0;
}

/**
 * punch_record_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void punch_record_print(ee_queue_t *head)
{
	punch_record_t *q = NULL;

	assert(head != NULL);

	ee_queue_foreach(q, head, punch_record_t, queue) {
		ALOGD("id:%d match_score:%f fk_per_id:%d\n",
			q->id, q->match_score, q->fk_per_id);
	}
}

/**
 * db_punch_record_insert - Combine statements and execute it.
 * @p_info: A pointer to the [punch_record_t] object.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_insert(punch_record_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"INSERT INTO punch_record("
			"fk_per_id,"
			"match_score,"
			"address,"
			"punch_time,"
			"punch_live_large_pic_name,"
			"punch_live_small_pic_name,"
			"punch_pass) "
			"VALUES(?,?,?,?,?,?,?);");

	return dbname_non_query_operator(db_get_record_db_name(), str_sql,
			punch_record_bind_stmt,
			p_info);
}

 /**
 * db_punch_record_delete_by_id - Delete punch record by record id.
 * @id: The punch record id to be queried.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_delete_by_id(int id)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"DELETE FROM punch_record WHERE id=\'%d\';", id);

	return dbname_non_query_operator(db_get_record_db_name(), str_sql, NULL, NULL);
}

/**
 * db_punch_record_delete - Delete an user by name.
 * @user_name: A pointer to username string.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_delete(char *user_name)
{
	int ret = -1;
	int sql_run_ret = SQLITE_ERROR;
	char str_sql[DB_SQL_MAX_LEN];
	char alias[] = "DbB";

	assert(user_name != NULL);

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql,
		"DELETE FROM punch_record WHERE fk_per_id IN("
		"SELECT %s.person_info.id FROM %s.person_info WHERE "
		"%s.person_info.name=?);", alias, alias, alias);

	sql_run_ret = dbunion_non_query_operator(db_get_record_db_name(), db_get_face_db_name(), alias,
		str_sql, db_stmt_bind_by_string, user_name);

	ret = (sql_run_ret == SQLITE_OK) ? 0 : -1;

	return ret;
}

/**
 * db_punch_record_delete_all - Delete all punch record.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_delete_all()
{
	int ret = -1;
	int sql_run_ret = SQLITE_ERROR;
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql, "delete from punch_record;");

	sql_run_ret = dbname_non_query_operator(db_get_record_db_name(), str_sql, NULL, NULL);

	ret = (sql_run_ret == SQLITE_OK) ? 0 : -1;

	return ret;
}

/**
 * db_punch_record_update - Update user information by name.
 * @id: Records pointed to by ID.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_update(unsigned int id, punch_record_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql, "UPDATE punch_record SET "
		"fk_per_id=?,"
		"match_score=?,"
		"address=?,"
		"punch_time=?,"
		"punch_live_large_pic_name=?,"
		"punch_live_small_pic_name=?,"
		"punch_pass=? "
		"WHERE id=%d;",
		id);

	return dbname_non_query_operator(db_get_record_db_name(), str_sql,
		punch_record_bind_stmt, p_info);
}

/**
 * db_punch_record_count - Get the number of punch record.
 * @user_name: A pointer to username string.
 * @punch_pass Punch pass flag
 * @start_time: Record the start time of punching.
 * @end_time: Record the end time of punching.
 *
 * Returns number of punch record.
 */
int db_punch_record_count(char *user_name, int punch_pass, double start_time,
			double end_time)
{
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN];
	char alias[] = "DbB";

	assert(user_name != NULL);

	memset(str_sql, 0, sizeof(str_sql));

	if ((user_name != NULL) && (strlen(user_name) > 0)) {
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT COUNT(*) FROM punch_record WHERE "
			"punch_time >= %f AND "
			"punch_time <= %f AND "
			"punch_pass = %d AND "
			"fk_per_id IN(SELECT %s.person_info.id FROM %s.person_info "
			"WHERE %s.person_info.name=?);",
			start_time,
			end_time,
			punch_pass,
			alias, alias, alias);
		dbunion_query_by_var_param(db_get_record_db_name(), db_get_face_db_name(), alias, str_sql,
			db_get_count, &count, "%s", user_name);

	} else {
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT COUNT(*) FROM punch_record WHERE "
			"punch_time >= %f AND "
			"punch_time <= %f AND "
			"punch_pass = %d;",
			start_time,
			end_time,
			punch_pass);
		dbname_query_by_var_param(db_get_record_db_name(), str_sql, db_get_count,
			&count, NULL);
	}

	return count;
}

/**
 * db_punch_record_count_all - Get the number of all punch record.
 *
 * Returns number of punch record.
 */
int db_punch_record_count_all()
{
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql,
		sizeof(str_sql),
		"SELECT COUNT(*) FROM punch_record;");

	dbname_query_by_var_param(db_get_record_db_name(), str_sql, db_get_count, &count, NULL);

	return count;
}

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
 * Returns number of data queried.
 */
int db_punch_record_query_begin(char *user_name, int punch_pass,
			double start_time, double end_time, unsigned int start_id,
			unsigned int count, ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];
	char alias[] = "DbB";

	assert((user_name != NULL) && (head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	if ((user_name != NULL) && (strlen(user_name) > 0)) {
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT * FROM punch_record WHERE "
			"punch_time >= %f AND "
			"punch_time <= %f AND "
			"punch_pass = %d AND "
			"fk_per_id IN(SELECT id FROM person_info "
			"WHERE name=?) AND "
			"id >= %d "
			"ORDER BY id DESC LIMIT %d;",
			start_time,
			end_time,
			punch_pass,
			start_id,
			count);

		return dbunion_query_by_var_param(db_get_face_db_name(),
				db_get_record_db_name(), alias, str_sql,
				punch_record_create_queue, head, "%s", user_name);
	} else {
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT * FROM punch_record WHERE "
			"punch_time >= %f AND "
			"punch_time <= %f AND "
			"punch_pass = %d AND "
			"id >= %d "
			"ORDER BY id DESC LIMIT %d;",
			start_time,
			end_time,
			punch_pass,
			start_id,
			count);
		return dbname_query_by_var_param(db_get_record_db_name(), str_sql,
			punch_record_create_queue, head, NULL);
	}

}

/**
 * db_punch_record_query_begin_by_less_timestamp - Query punch record information
 * by less time.
 * @time: Record before the time of punching(ms).
 * @head: A pointer to the head of the queue.
  *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_punch_record_query_begin_by_less_timestamp(double time,
	ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert((head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	if (time > 0)
		snprintf(str_sql,
			sizeof(str_sql),
				"SELECT * FROM punch_record WHERE "
				"punch_time < %.0f;", time);

	return dbname_query_by_var_param(db_get_record_db_name(), str_sql,
		punch_record_create_queue, head, NULL);
}

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
	double start_time, double end_time, ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert((head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	if (time > 0)
		snprintf(str_sql,
			sizeof(str_sql),
				"SELECT * FROM punch_record WHERE "
				"punch_time >= %.0f and punch_time <= %.0f;",
				start_time, end_time);

	return dbname_query_by_var_param(db_get_record_db_name(), str_sql,
		punch_record_create_queue, head, NULL);
}

/**
 * db_punch_record_query_min_punch_time - Query min punch time value.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
double db_punch_record_query_min_punch_time()
{
	char str_sql[DB_SQL_MAX_LEN];
	double time_val = 0;
	char buf[128] = {0};

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql,
		sizeof(str_sql),
		"SELECT min(punch_time) from punch_record;");

	ALOGD("[%s:%d] sql=%s\n", __func__, __LINE__, str_sql);
	dbname_query_by_var_param(db_get_record_db_name(), str_sql, db_get_min_punchtime,
		&time_val, NULL);

	time_second_to_str(time_val/1000, buf, sizeof(buf));
	ALOGD("[%s:%d]time_val = %lf, %s\n", __func__, __LINE__, time_val, buf);

	return time_val;
}

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
			double start_time, double end_time, unsigned int start_id,
			unsigned int count, ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];
	char alias[] = "DbB";

	assert((user_name != NULL) && (head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	if ((user_name != NULL) && (strlen(user_name) > 0)) {
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT t2.employee_number, t2.name, t2.gender, "
			"t2.id_card_number, t2.department_name, t2.regist_photo_name, "
			"t1.match_score, t1.address, t1.punch_time, "
			"t1.punch_live_large_pic_name, t1.punch_live_small_pic_name, "
			"t1.punch_pass from "
			"("
				"SELECT * FROM %s.punch_record WHERE "
				"%s.punch_record.punch_time >= %f AND "
				"%s.punch_record.punch_time <= %f AND "
				"%s.punch_record.punch_pass = %d AND "
				"%s.punch_record.fk_per_id IN(SELECT id FROM person_info "
				"WHERE name=?) AND "
				"id >= %d "
				"ORDER BY id DESC LIMIT %d"
			") t1, "
			"person_info t2 where t1.fk_per_id=t2.id ;",
			alias,
			alias, start_time,
			alias, end_time,
			alias, punch_pass,
			alias, start_id,
			count);
		return dbunion_query_by_var_param(db_get_face_db_name(), db_get_record_db_name(), alias,
			str_sql,
			punch_record_full_info_create_queue, head, "%s", user_name);
	} else {
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT t2.employee_number, t2.name, t2.gender, "
			"t2.id_card_number, t2.department_name, t2.regist_photo_name, "
			"t1.match_score, t1.address, t1.punch_time, "
			"t1.punch_live_large_pic_name, t1.punch_live_small_pic_name, "
			"t1.punch_pass from "
			"("
				"SELECT * FROM %s.punch_record WHERE "
				"%s.punch_record.punch_time >= %f AND "
				"%s.punch_record.punch_time <= %f AND "
				"%s.punch_record.punch_pass = %d AND "
				"id >= %d "
				"ORDER BY id DESC LIMIT %d"
			") t1, "
			"person_info t2 where t1.fk_per_id=t2.id ;",
			alias,
			alias, start_time,
			alias, end_time,
			alias, punch_pass,
			start_id,
			count);
		return dbname_query_by_var_param(db_get_record_db_name(), str_sql,
			punch_record_full_info_create_queue, head, NULL);
	}

}

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
	ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];
	char alias[] = "DbB";

	assert((user_name != NULL) && (head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	if ((user_name != NULL) && (strlen(user_name) > 0))
		snprintf(str_sql,
			sizeof(str_sql),
			"SELECT t2.employee_number, t2.name, t2.gender, "
			"t2.id_card_number, t2.department_name, t2.regist_photo_name,"
			"t1.match_score, t1.address, t1.punch_time, "
			"t1.punch_live_large_pic_name, t1.punch_live_small_pic_name, "
			"t1.punch_pass from "
			"("
				"SELECT * FROM %s.punch_record WHERE "
				"%s.punch_record.fk_per_id IN(SELECT id FROM person_info "
				"WHERE name=?) "
				"ORDER BY %s.punch_record.id DESC"
			") t1, "
			"person_info t2 where t1.fk_per_id=t2.id;",
			alias,
			alias,
			alias);

	return dbunion_query_by_var_param(db_get_face_db_name(), db_get_record_db_name(), alias,
			str_sql,
			punch_record_full_info_create_queue, head, "%s", user_name);

}

/**
 * db_punch_record_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_query_end(ee_queue_t *head)
{
	assert(head != NULL);

	punch_record_free_queue(head);

	return 0;
}

/**
 * db_punch_record_full_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_full_info_query_end(ee_queue_t *head)
{
	assert(head != NULL);

	punch_record_full_info_free_queue(head);

	return 0;
}

/**
 * punch_record_full_info_free_queue_for_cloud - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns none.
 */
static void punch_record_full_info_free_queue_for_cloud(ee_queue_t *h)
{
	ee_queue_t *head = h, *pos, *n;
	full_punch_rec_info_t_of_upload *p = NULL;

	assert(h != NULL);

	ee_queue_for_each_safe(pos, n, head) {
		p = ee_queue_data(pos, full_punch_rec_info_t_of_upload, queue);
		free(p);
	}

	ee_queue_init(head);
}

/**
 * db_punch_record_full_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_punch_record_full_info_query_end_for_cloud(ee_queue_t *head)
{
	assert(head != NULL);

	punch_record_full_info_free_queue_for_cloud(head);

	return 0;
}

/**
 * punch_record_full_info_create_queue_for_cloud - Database query operations,
 * with variable parameter
 * @stmt : A pointer to the [sqlite3_stmt] object.
 * @head : A pointer to the head of the queue.
 *
 * Returns number of data queried.
 */
static int punch_record_full_info_create_queue_for_cloud(sqlite3_stmt *stmt,
		void *head)
{
	ee_queue_t *h = head;
	ee_queue_init(h);
	full_punch_rec_info_t_of_upload *u;
	int count = 0;

	assert((stmt != NULL) && (head != NULL));
	while (SQLITE_ROW == sqlite3_step(stmt)) {
		u = calloc(sizeof(full_punch_rec_info_t), 1);
		if (!u) {
			ALOGE("[%s:%d] %s\n",
				__func__, __LINE__,
				"create full_punch_rec_info_t_of_upload faile");
			goto CALLOC_FAIL;
		}

		u->id = db_stmt_get_int(stmt, 0);
		db_stmt_get_text(stmt, 1, u->employee_number);
		db_stmt_get_text(stmt, 2, u->name);
		db_stmt_get_text(stmt, 3, u->gender);
		db_stmt_get_text(stmt, 4, u->nationa);
		u->mobile = db_stmt_get_int(stmt, 5);
		u->access_right = db_stmt_get_int(stmt, 6);
		db_stmt_get_text(stmt, 7, u->access_card_number);
		u->temporary_access_start_time = db_stmt_get_double(stmt, 8);
		u->temporary_access_end_time = db_stmt_get_double(stmt, 9);
		db_stmt_get_text(stmt, 10, u->id_card_number);
		db_stmt_get_text(stmt, 11, u->department_name);
		u->match_score = db_stmt_get_double(stmt, 12);
		db_stmt_get_text(stmt, 13, u->address);
		u->punch_time = db_stmt_get_double(stmt, 14);
		db_stmt_get_text(stmt, 15, u->punch_live_large_pic_name);
		db_stmt_get_text(stmt, 16, u->punch_live_small_pic_name);
		u->punch_pass = db_stmt_get_int(stmt, 17);

		ee_queue_insert_tail(h, &(u->queue));

		count++;
	}

	ALOGD("[%s:%d]count=%d\n", __func__, __LINE__, count);
	return count;

CALLOC_FAIL:
	punch_record_full_info_free_queue(h);
	ALOGD("[%s:%d]CALLOC_FAIL count=%d\n", __func__, __LINE__, count);
	return 0;
}

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
		ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];
	char alias[] = "DbB";

	assert(head != NULL);

	ee_queue_init(head);

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql,
		sizeof(str_sql),
		"SELECT t1.id, t2.employee_number, t2.name, t2.gender, "
		"t2.nationa, t2.mobile, t2.access_right, t2.access_card_number, "
		"t2.temporary_access_start_time, t2.temporary_access_end_time, "
		"t2.id_card_number, t2.department_name, "
		"t1.match_score, t1.address, t1.punch_time, "
		"t1.punch_live_large_pic_name, t1.punch_live_small_pic_name, "
		"t1.punch_pass from "
		"("
			"SELECT * FROM %s.punch_record WHERE "
			"id >= 0 "
			"ORDER BY id DESC LIMIT %d"
		") t1, "
		"person_info t2 where t1.fk_per_id=t2.id;",
		alias, count);

	return dbunion_query_by_var_param(db_get_face_db_name(),
			db_get_record_db_name(), alias,
			str_sql,
			punch_record_full_info_create_queue_for_cloud,
			head, NULL, NULL);
}


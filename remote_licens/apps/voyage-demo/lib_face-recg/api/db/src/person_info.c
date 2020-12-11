/*
 * person_info.c - Interface for management personnel registration
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
#include "db_person_info.h"
#include "db_punch_record.h"
#include "record_info.h"
#include "file_ops.h"
#include "db_config.h"
#include "db.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * person_info_free_queue - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns none.
 */
static void person_info_free_queue(ee_queue_t *h)
{
	ee_queue_t *head = h, *pos, *n;
	person_info_t *p = NULL;

	assert(h != NULL);

	ee_queue_for_each_safe(pos, n, head) {
		p = ee_queue_data(pos, person_info_t, queue);
		free(p);
	}

	ee_queue_init(head);
}

/**
 * person_info_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int person_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index, void *arg)
{
	int rc;
	person_info_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_text(stmt, 1, param->employee_number,
			strlen(param->employee_number), NULL);
	sqlite3_bind_text(stmt, 2, param->name,
			strlen(param->name), NULL);
	sqlite3_bind_text(stmt, 3, param->gender,
			strlen(param->gender), NULL);
	sqlite3_bind_text(stmt, 4, param->nationa,
			strlen(param->nationa), NULL);
	sqlite3_bind_text(stmt, 5, param->id_card_number,
			strlen(param->id_card_number), NULL);

	sqlite3_bind_text(stmt, 6, param->mobile,
			strlen(param->mobile), NULL);
	sqlite3_bind_text(stmt, 7, param->department_name,
			strlen(param->department_name), NULL);
	sqlite3_bind_int(stmt, 8, param->access_right);
	sqlite3_bind_double(stmt, 9, param->temporary_access_start_time);
	sqlite3_bind_double(stmt, 10, param->temporary_access_end_time);

	sqlite3_bind_int(stmt, 11, param->temporary_access_times);
	sqlite3_bind_text(stmt, 12, param->access_card_number,
			strlen(param->access_card_number), NULL);
	sqlite3_bind_text(stmt, 13, param->remarks,
			strlen(param->remarks), NULL);
	sqlite3_bind_text(stmt, 14, param->regist_photo_name,
			strlen(param->regist_photo_name), NULL);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}

	return SQLITE_OK;
}

/**
 * person_feature_info_bind_stmt - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int person_feature_info_bind_stmt(sqlite3_stmt *stmt,
	unsigned int index, void *arg)
{
	int rc;
	person_info_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_blob(stmt, 1, param->face_features,
			FACE_FEATURES_DATA_LENGTH, NULL);
	sqlite3_bind_text(stmt, 2, param->face_key_points,
				strlen(param->face_key_points), NULL);
	sqlite3_bind_double(stmt, 3, param->face_norm[0]);
	sqlite3_bind_double(stmt, 4, param->face_norm[1]);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}

	return SQLITE_OK;
}

/**
 * person_info_bind_stmt_by_string - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int person_info_bind_stmt_by_string(sqlite3_stmt *stmt, unsigned int index,
		void *arg, char *str)
{
	int rc;
	person_info_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_text(stmt, 1, param->employee_number,
			strlen(param->employee_number), NULL);
	sqlite3_bind_text(stmt, 2, param->name,
			strlen(param->name), NULL);
	sqlite3_bind_text(stmt, 3, param->gender,
			strlen(param->gender), NULL);
	sqlite3_bind_text(stmt, 4, param->nationa,
			strlen(param->nationa), NULL);
	sqlite3_bind_text(stmt, 5, param->id_card_number,
			strlen(param->id_card_number), NULL);

	sqlite3_bind_text(stmt, 6, param->mobile,
			strlen(param->mobile), NULL);
	sqlite3_bind_text(stmt, 7, param->department_name,
			strlen(param->department_name), NULL);
	sqlite3_bind_int(stmt, 8, param->access_right);
	sqlite3_bind_double(stmt, 9, param->temporary_access_start_time);
	sqlite3_bind_double(stmt, 10, param->temporary_access_end_time);

	sqlite3_bind_int(stmt, 11, param->temporary_access_times);
	sqlite3_bind_text(stmt, 12, param->access_card_number,
			strlen(param->access_card_number), NULL);
	sqlite3_bind_text(stmt, 13, param->remarks,
			strlen(param->remarks), NULL);
	sqlite3_bind_text(stmt, 14, param->regist_photo_name,
			strlen(param->regist_photo_name), NULL);

	sqlite3_bind_text(stmt, 15, str, strlen(str), NULL);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}

	return SQLITE_OK;
}

/**
 * person_feature_info_bind_stmt_by_string - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: Not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int person_feature_info_bind_stmt_by_string(sqlite3_stmt *stmt,
	unsigned int index, void *arg, char *str)
{
	int rc;
	person_info_t *param = arg;

	assert((stmt != NULL) && (arg != NULL));

	sqlite3_bind_blob(stmt, 1, param->face_features,
			FACE_FEATURES_DATA_LENGTH, NULL);
	sqlite3_bind_text(stmt, 2, param->face_key_points,
				strlen(param->face_key_points), NULL);
	sqlite3_bind_double(stmt, 3, param->face_norm[0]);
	sqlite3_bind_double(stmt, 4, param->face_norm[1]);
	sqlite3_bind_text(stmt, 5, str, strlen(str), NULL);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}

	return SQLITE_OK;
}

static int person_info_create_queue(sqlite3_stmt *stmt, void *head)
{
	int count = 0;
	ee_queue_t *h = head;
	ee_queue_init(h);
	person_info_t *u;

	assert((stmt != NULL) && (head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		u = calloc(sizeof(person_info_t), 1);
		if (!u) {
			ALOGD("[%s:%d] create peson_info_t faile\n", __func__, __LINE__);
			goto CALLOC_FAIL;
		}

		u->id = db_stmt_get_int(stmt, 0);
		db_stmt_get_text(stmt, 1, u->employee_number);
		db_stmt_get_text(stmt, 2, u->name);
		db_stmt_get_text(stmt, 3, u->gender);
		db_stmt_get_text(stmt, 4, u->nationa);
		db_stmt_get_text(stmt, 5, u->id_card_number);

		db_stmt_get_text(stmt, 6, u->mobile);
		db_stmt_get_text(stmt, 7, u->department_name);
		u->access_right = db_stmt_get_int(stmt, 8);
		u->temporary_access_start_time = db_stmt_get_double(stmt, 9);
		u->temporary_access_end_time = db_stmt_get_double(stmt, 10);

		u->temporary_access_times = db_stmt_get_int(stmt, 11);
		db_stmt_get_text(stmt, 12, u->access_card_number);
		db_stmt_get_text(stmt, 13, u->remarks);
		db_stmt_get_text(stmt, 14, u->regist_photo_name);
		db_stmt_get_blob(stmt, 15, u->face_features);
		db_stmt_get_text(stmt, 16, u->face_key_points);
		u->face_norm[0] = db_stmt_get_double(stmt, 17);
		u->face_norm[1] = db_stmt_get_double(stmt, 18);

		ee_queue_insert_tail(h, &(u->queue));

		count++;
	}

	return count;

CALLOC_FAIL:
	person_info_free_queue(h);

	return 0;
}

/**
 * person_info_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void person_info_print(ee_queue_t *head)
{
	person_info_t *q = NULL;

	assert(head != NULL);

	ee_queue_foreach(q, head, person_info_t, queue) {
		ALOGD("userid:%d employee_number:%s name:%s\n",
			q->id, q->employee_number, q->name);
	}
}

/**
 * db_person_info_insert - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_person_info_insert(person_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];
	char str_sql_part[DB_SQL_MAX_LEN];
	int ret = 0;
	int max_id = 0;

	assert(p_info != NULL);
	ALOGD("[%s:%d]employee_number=%s\n", __func__, __LINE__,
		p_info->employee_number);

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql), "select max(id) from person_info;");
	dbname_query_by_var_param(db_get_face_db_name(), str_sql, db_config_get_count,
		&max_id, NULL);
	max_id += 1;

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql,
		"INSERT INTO person_info("
		"id,"
		"employee_number,"
		"name,"
		"gender,"
		"nationa,"
		"id_card_number,"
		"mobile,"
		"department_name,"
		"access_right,"
		"temporary_access_start_time,"
		"temporary_access_end_time,"
		"temporary_access_times,"
		"access_card_number,"
		"remarks,"
		"regist_photo_name"
		") VALUES(%d,?,?,?,?,?,?,?,?,?,?,?,?,?,?);", max_id);

	memset(str_sql_part, 0, sizeof(str_sql_part));
	sprintf(str_sql_part,
		"INSERT INTO person_feature_info("
		"id,"
		"face_features,"
		"face_key_points,"
		"face_norm,"
		"face_norm_1"
		") VALUES(%d,?,?,?,?);", max_id);

	ret += dbname_non_query_operator(db_get_face_db_name(), str_sql,
		person_info_bind_stmt, p_info);

	ret += dbname_non_query_operator(db_get_face_db_name(), str_sql_part,
		person_feature_info_bind_stmt, p_info);

	return ret;
}

/**
 * db_person_info_delete - Delete an user by name.
 * @user_name: A pointer to username string.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_person_info_delete(char *user_name)
{
	int ret = -1;
	int sql_run_ret = SQLITE_ERROR;
	char str_sql[DB_SQL_MAX_LEN];

	assert(user_name != NULL);

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql,
		"DELETE FROM person_feature_info WHERE id in(select id from "
		"person_info where name=?);");
	sql_run_ret = dbname_non_query_operator(db_get_face_db_name(), str_sql,
		db_stmt_bind_by_string, user_name);

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql, "DELETE FROM person_info WHERE name=?;");
	sql_run_ret = dbname_non_query_operator(db_get_face_db_name(), str_sql,
		db_stmt_bind_by_string, user_name);

	ret = (sql_run_ret == SQLITE_OK) ? 0 : -1;

	return ret;
}

/**
 * db_person_info_delete - Delete an user by name.
 * @user_name: A pointer to username string.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_person_info_delete_all()
{
	int ret = -1;
	int sql_run_ret = SQLITE_ERROR;
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));
	sprintf(str_sql, "delete from person_info;delete from person_feature_info;");

	sql_run_ret = dbname_non_query_operator(db_get_face_db_name(), str_sql, NULL, NULL);

	ret = (sql_run_ret == SQLITE_OK) ? 0 : -1;

	return ret;
}

/**
 * db_person_info_update - Update user information by name.
 * @user_name: A pointer to username string.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_person_info_update(char *user_name, person_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];
	char str_sql_part[DB_SQL_MAX_LEN];
	int ret = 0;

	assert((user_name != NULL) && (p_info != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	sprintf(str_sql,
		"UPDATE person_info SET "
		"employee_number=?,"
		"name=?,"
		"gender=?,"
		"nationa=?,"
		"id_card_number=?,"
		"mobile=?,"
		"department_name=?,"
		"access_right=?,"
		"temporary_access_start_time=?,"
		"temporary_access_end_time=?,"
		"temporary_access_times=?,"
		"access_card_number=?,"
		"remarks=?,"
		"regist_photo_name=?,"
		"WHERE name=?;");

	ret += dbname_non_query_operator_by_string_param(db_get_face_db_name(), str_sql,
		person_info_bind_stmt_by_string, p_info, user_name);

	memset(str_sql_part, 0, sizeof(str_sql_part));
	sprintf(str_sql_part,
		"UPDATE person_feature_info SET "
		"face_features=?,"
		"face_key_points=?,"
		"face_norm=?, "
		"face_norm_1=? "
		"WHERE id in (select id from person_info where name=?);");

	ret += dbname_non_query_operator_by_string_param(db_get_face_db_name(), str_sql_part,
		person_feature_info_bind_stmt_by_string, p_info, user_name);

	return ret;
}

/**
 * db_person_info_count - Get the number of users.
 *
 * Returns number of users.
 */
int db_person_info_count()
{
	int count = 0;
	int ret = 0;

	char str_sql[DB_SQL_MAX_LEN] = "SELECT COUNT(*) FROM person_info;";

	ret = dbname_query_by_var_param(db_get_face_db_name(), str_sql,
		db_get_count, &count, NULL);
	ALOGD("[%s:%d]ret=%d\n", __func__, __LINE__, ret);

	return count;
}

/**
 * person_info_get_from_stmt - Query personnel information by person id
 * @stmt :
 * @p_info : A pointer to cloud_platform_t.
 *
 * Returns number of data queried.
 */
static int person_info_get_from_stmt(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	person_info_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		u->id = db_stmt_get_int(stmt, 0);
		db_stmt_get_text(stmt, 1, u->employee_number);
		db_stmt_get_text(stmt, 2, u->name);
		db_stmt_get_text(stmt, 3, u->gender);
		db_stmt_get_text(stmt, 4, u->nationa);
		db_stmt_get_text(stmt, 5, u->id_card_number);

		db_stmt_get_text(stmt, 6, u->mobile);
		db_stmt_get_text(stmt, 7, u->department_name);
		u->access_right = db_stmt_get_int(stmt, 8);
		u->temporary_access_start_time = db_stmt_get_double(stmt, 9);
		u->temporary_access_end_time = db_stmt_get_double(stmt, 10);

		u->temporary_access_times = db_stmt_get_int(stmt, 11);
		db_stmt_get_text(stmt, 12, u->access_card_number);
		db_stmt_get_text(stmt, 13, u->remarks);
		db_stmt_get_text(stmt, 14, u->regist_photo_name);
		db_stmt_get_blob(stmt, 15, u->face_features);
		db_stmt_get_text(stmt, 16, u->face_key_points);
		u->face_norm[0] = db_stmt_get_double(stmt, 17);
		u->face_norm[1] = db_stmt_get_double(stmt, 18);

		count++;
	}

	return count;
}

/**
 * db_person_info_query_by_id - Query personnel information by person id.
 * @person_id: The person id to be queried.
 * @p_info: Out, Pointer to 'person_info_t'.
 *
 * Returns number of data queried.
 */
int db_person_info_query_by_id(unsigned int person_id, person_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(person_info_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
		"SELECT t1.*,t2.face_features,t2.face_key_points,t2.face_norm "
		"FROM person_info t1 left join person_feature_info t2 "
		"on t1.id=t2.id WHERE t1.id=%d;",
		person_id);

	return dbname_query_by_var_param(db_get_face_db_name(), str_sql,
		person_info_get_from_stmt, p_info, NULL);
}

/**
 * db_person_info_query_by_employee_number - Find person information by
 *	employee number.
 * @employee_number: The employee number to be queried.
 * @p_info: Out, Pointer to 'person_info_t'.
 *
 * Returns number of data queried.
 */
int db_person_info_query_by_employee_number(char *employee_number,
	person_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(person_info_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			"SELECT t1.*,t2.face_features,t2.face_key_points,t2.face_norm "
			"FROM person_info t1 left join person_feature_info t2 "
			"on t1.id=t2.id WHERE t1.employee_number=?;");

	return dbname_query_by_var_param(db_get_face_db_name(), str_sql,
		person_info_get_from_stmt, p_info, "%s", employee_number);
}

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
 * Returns number of data queried.
 */
int db_person_info_query_begin(char *user_name, unsigned int start_id,
			unsigned int count, ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert((user_name != NULL) && (head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	if ((user_name != NULL) && (strlen(user_name) > 0)) {
		snprintf(str_sql, sizeof(str_sql),
			"SELECT t1.*,t2.face_features,t2.face_key_points,t2.face_norm,"
			"t2.face_norm_1 "
			"FROM person_info t1 left join person_feature_info t2 "
			"on t1.id=t2.id WHERE t1.name=? ORDER BY t1.id;");
		return dbname_query_by_var_param(db_get_face_db_name(), str_sql,
			person_info_create_queue, head, "%s", user_name);
	} else {
		snprintf(str_sql, sizeof(str_sql),
			"SELECT t1.*,t2.face_features,t2.face_key_points,t2.face_norm,"
			"t2.face_norm_1 "
			"FROM person_info t1 left join person_feature_info t2 "
			"on t1.id=t2.id WHERE t1.id >= %d ORDER BY t1.id "
			"LIMIT %d;", start_id, count);
		return dbname_query_by_var_param(db_get_face_db_name(), str_sql,
			person_info_create_queue, head, NULL);
	}

}

/**
 * db_person_info_query_begin_by_name - Query user information by name
 * and count.
 * @user_name: A pointer to username string.
 * @head: A pointer to the head of the queue.
 *
 * Returns number of data queried.
 */
int db_person_info_query_begin_by_name(char *user_name, ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert((user_name != NULL) && (head != NULL));

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
		"SELECT t1.*,t2.face_features,t2.face_key_points,t2.face_norm "
		"FROM person_info t1 left join person_feature_info t2 "
		"on t1.id=t2.id WHERE t1.name=? ORDER BY t1.id;");

	return dbname_query_by_var_param(db_get_face_db_name(), str_sql,
		person_info_create_queue, head, "%s", user_name);
}

int db_person_info_query_max_id()
{
	int max_id = 0;
	int ret = 0;
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "select max(id) as currentId from person_info;");

	ret =  dbname_query_by_var_param(db_get_face_db_name(), str_sql,
		db_config_get_count, &max_id, NULL);
	ALOGD("[%s:%d]ret=%d\n", __func__, __LINE__, ret);

	return max_id;
}

/**
 * db_manager_info_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_person_info_query_end(ee_queue_t *head)
{
	assert(head != NULL);

	person_info_free_queue(head);

	return 0;
}


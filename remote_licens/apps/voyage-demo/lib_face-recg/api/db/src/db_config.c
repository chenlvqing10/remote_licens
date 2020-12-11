/*
 * db_config.c
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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
#include "db.h"

#define LOG_TAG "db_config"
#include <log/log.h>

typedef struct key_value {
	char *section;
	char *key;
	char *value;
} key_value;

static int db_config_get_value(sqlite3_stmt *stmt, void *value)
{
	int count = 0;
	if (NULL == stmt || NULL == value)
		return -1;
	while (SQLITE_ROW == sqlite3_step(stmt)) {
		db_stmt_get_text(stmt, 0, (char *)value);
		count++;
	}

	return count;
}

int db_config_get_count(sqlite3_stmt *stmt, void *value)
{
	int count = 0;
	int *val = (int *)value;
	if (NULL == stmt || NULL == value)
		return -1;
	while (SQLITE_ROW == sqlite3_step(stmt)) {
		*val = db_stmt_get_int(stmt, 0);
		count++;
	}

	return SQLITE_OK;
}

int db_config_bind_stmt(sqlite3_stmt *stmt, unsigned int index, void *arg)
{
	int rc;
	key_value *kval = arg;
	sqlite3_bind_text(stmt, 1, kval->section,
			strlen(kval->section), NULL);
	sqlite3_bind_text(stmt, 2, kval->key,
			strlen(kval->key), NULL);
	ALOGD("[%s]111", __func__);
	if (NULL != kval->value)
		if (strlen(kval->value) > 0)
			sqlite3_bind_text(stmt, 3, kval->value,
				strlen(kval->value), NULL);
	ALOGD("[%s]222", __func__);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return rc;
	}
	ALOGD("[%s]333", __func__);
	return rc;
}

int db_config_get_int(char *section, char *key, int default_value)
{
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	int value = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	count = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
		db_config_get_value, str_value, "%s%s", section, key);
	if (1 == count)
		sscanf(str_value, "%d", &value);
	else
		value = default_value;

	ALOGD("[%s:%d]section=%s, key=%s\n", __func__, __LINE__, section, key);
	ALOGD("[%s:%d]count=%d, value=%s\n", __func__, __LINE__, count, str_value);

	return value;
}

long db_config_get_long(char *section, char *key, long default_value)
{
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	long value = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	count = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
		db_config_get_value, str_value, "%s%s", section, key);

	if (1 == count)
		sscanf(str_value, "%ld", &value);
	else
		value = default_value;

	ALOGD("[%s:%d]section=%s, key=%s\n", __func__, __LINE__, section, key);
	ALOGD("[%s:%d]count=%d, value=%s\n", __func__, __LINE__, count, str_value);

	return value;
}

double db_config_get_double(char *section, char *key, double default_value)
{
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	double value = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	count = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
		db_config_get_value, str_value, "%s%s", section, key);

	if (1 == count)
		sscanf(str_value, "%lf", &value);
	else
		value = default_value;

	ALOGD("[%s:%d]section=%s, key=%s\n", __func__, __LINE__, section, key);
	ALOGD("[%s:%d]count=%d, value=%s\n", __func__, __LINE__, count, str_value);

	return value;
}

int db_config_get_string(char *section, char *key, char *value,
		int value_size, char *default_value)
{
	int ret = 0;
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	count = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
			db_config_get_value, str_value, "%s%s", section, key);

	if (1 == count) {
		strncpy(value, str_value, value_size-1);
		ret = 0;
	} else if (0 == count) {
		strncpy(value, default_value, value_size-1);
		ret = 0;
	} else
		ret = -1;

	ALOGD("[%s:%d]section=%s, key=%s\n", __func__, __LINE__, section, key);
	ALOGD("[%s:%d]count=%d, value=%s\n", __func__, __LINE__, count, str_value);

	return ret;
}

int db_config_set_int(char *section, char *key, int value)
{
	int ret = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	key_value kval;
	int max_id = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	ret = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
		db_config_get_value, str_value, "%s%s", section, key);
	if (1 == ret) {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"update config set value='%d' where section=? and key=?;",
			value);

		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		ret = dbname_non_query_operator(db_get_config_db_name(),
			str_sql, db_config_bind_stmt, &kval);
		ALOGD("update dbname_non_query_operator(%s %s %d) ret %d\n",
			section, key, value, ret);
	} else {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql), "select max(id) from config;");
		dbname_query_by_var_param(db_get_config_db_name(), str_sql,
			db_config_get_count, &max_id, NULL);
		max_id += 1;

		ALOGD("[%s]max_id=%d\n", __func__, max_id);
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"insert into config(id,section,key,value) values(%d,?,?,?);",
			max_id);
		memset(str_value, 0, sizeof(str_value));
		snprintf(str_value, sizeof(str_value), "%d", value);
		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		kval.value = str_value;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
		ALOGD("insert dbname_non_query_operator(%s %s %d) ret %d\n",
			section, key, value, ret);
	}
	return ret;
}

int db_config_set_long(char *section, char *key, long value)
{
	int ret = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	key_value kval;
	int max_id = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	ret = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
		db_config_get_value, str_value, "%s%s", section, key);
	if (1 == ret) {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"update config set value='%ld' where section=? and key=?;",
			value);

		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
	} else {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql), "select max(id) from config;");
		dbname_query_by_var_param(db_get_config_db_name(), str_sql,
			db_config_get_count, &max_id, NULL);
		max_id += 1;

		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"insert into config(id,section,key,value) values(%d,?,?,?);",
			max_id);

		memset(str_value, 0, sizeof(str_value));
		snprintf(str_value, sizeof(str_value), "%ld", value);
		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		kval.value = str_value;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
	}

	return ret;
}

int db_config_set_double(char *section, char *key, double value)
{
	int ret = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	key_value kval;
	int max_id = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	ret = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
		db_config_get_value, str_value, "%s%s", section, key);
	if (1 == ret) {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"update config set value='%lf' where section=? and key=?;",
			value);

		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
	} else {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql), "select max(id) from config;");
		dbname_query_by_var_param(db_get_config_db_name(), str_sql,
			db_config_get_count, &max_id, NULL);
		max_id += 1;

		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"insert into config(id,section,key,value) values(%d,?,?,?);",
			max_id);

		memset(str_value, 0, sizeof(str_value));
		snprintf(str_value, sizeof(str_value), "%lf", value);
		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		kval.value = str_value;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
	}

	return ret;
}

int db_config_set_string(char *section, char *key, char *value)
{
	int ret = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};
	key_value kval;
	int max_id = 0;

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	ret = dbname_query_by_var_param(db_get_config_db_name(), str_sql,
			db_config_get_value, str_value, "%s%s", section, key);
	if (1 == ret) {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"update config set value='%s' where section=? and key=?",
			value);

		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
	} else {
		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql), "select max(id) from config;");
		dbname_query_by_var_param(db_get_config_db_name(), str_sql,
			db_config_get_count, &max_id, NULL);
		max_id += 1;

		memset(str_sql, 0, sizeof(str_sql));
		snprintf(str_sql, sizeof(str_sql),
			"insert into config(id,section,key,value) values(%d,?,?,?);",
			max_id);

		memset(str_value, 0, sizeof(str_value));
		snprintf(str_value, sizeof(str_value), "%s", value);
		memset(&kval, 0, sizeof(key_value));
		kval.section = section;
		kval.key = key;
		kval.value = str_value;
		ret = dbname_non_query_operator(db_get_config_db_name(), str_sql,
			db_config_bind_stmt, &kval);
	}

	return ret;
}

int which_db_config_get_string(char *which_db, char *section, char *key,
		char *value, int value_size, char *default_value)
{
	int ret = 0;
	int count = 0;
	char str_sql[DB_SQL_MAX_LEN] = {0};
	char str_value[128] = {0};

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
		"select value from config where section=? and key=?;");

	count = dbname_query_by_var_param(which_db, str_sql, db_config_get_value,
		str_value, "%s%s", section, key);

	if (1 == count) {
		strncpy(value, str_value, value_size-1);
		ret = 0;
	} else if (0 == count) {
		strncpy(value, default_value, value_size-1);
		ret = -1;
	} else
		ret = -1;

	ALOGD("[%s:%d]count=%d, value=%s\n", __func__, __LINE__, count, str_value);

	return ret;
}

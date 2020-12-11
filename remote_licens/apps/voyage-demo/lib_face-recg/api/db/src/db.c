/*
 * db.c - Interface for database
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

#define LOG_TAG		"DB"
#include <log/log.h>

#include "file_ops.h"
#include "db_persistence.h"
#include "db_config.h"

char DB_ORIGIN[128] = { "/etc/face-recg.db" };
char DB_DEST[128] = { "/data/face-recg.db" };
char DB_ORIGIN_FACE[128] = { "/etc/face.db" };
char DB_DEST_FACE[128] = { "/data/face.db" };
char DB_ORIGIN_RECORD[128] = { "/etc/record.db" };
char DB_DEST_RECORD[128] = { "/data/record.db" };
char DB_ORIGIN_CONFIG[128] = { "/etc/config.db" };
char DB_DEST_CONFIG[128] = { "/data/config.db" };

static int db_check(void)
{
	int ret = 0;
	char origin_db_version[64] = {0};
	char dest_db_version[64] = {0};

	ALOGD("db2_check\n");

	if (is_file_exist(DB_DEST_FACE)) {
		ret = file_copy(DB_ORIGIN_FACE, DB_DEST_FACE);
		if (ret)
			ALOGE("[%d]failed to copy %s to %s",
			__LINE__, DB_ORIGIN_FACE, DB_DEST_FACE);
	} else {
		memset(origin_db_version, 0, sizeof(origin_db_version));
		memset(dest_db_version, 0, sizeof(dest_db_version));
		which_db_config_get_string(DB_ORIGIN_FACE, DBCONF_DB, DBCONF_DB_VERSION,
			origin_db_version, sizeof(origin_db_version), "");
		which_db_config_get_string(DB_DEST_FACE, DBCONF_DB, DBCONF_DB_VERSION,
			dest_db_version, sizeof(dest_db_version), "");
		ALOGD("[%s:%d]etc_db_version=%s, data_db_version=%s\n", __func__,
			__LINE__, origin_db_version, dest_db_version);
		if (strncmp(origin_db_version, dest_db_version,
			sizeof(origin_db_version)) != 0) {
			ALOGD("etc_db_version != data_db_version\n");
			ret = file_copy(DB_ORIGIN_FACE, DB_DEST_FACE);
			if (ret) {
				ALOGE("[%d]failed to copy %s to %s",
				__LINE__, DB_ORIGIN_FACE, DB_DEST_FACE);
			}
		}
	}

	if (is_file_exist(DB_DEST_RECORD)) {
		ret = file_copy(DB_ORIGIN_RECORD, DB_DEST_RECORD);
		if (ret)
			ALOGE("failed to copy %s to %s",
			DB_ORIGIN_RECORD, DB_DEST_RECORD);
	} else {
		memset(origin_db_version, 0, sizeof(origin_db_version));
		memset(dest_db_version, 0, sizeof(dest_db_version));
		which_db_config_get_string(DB_ORIGIN_RECORD, DBCONF_DB, DBCONF_DB_VERSION,
			origin_db_version, sizeof(origin_db_version), "");
		which_db_config_get_string(DB_DEST_RECORD, DBCONF_DB, DBCONF_DB_VERSION,
			dest_db_version, sizeof(dest_db_version), "");
		ALOGD("[%s:%d]etc_db_version=%s, data_db_version=%s\n", __func__,
			__LINE__, origin_db_version, dest_db_version);
		if (strncmp(origin_db_version, dest_db_version,
			sizeof(origin_db_version)) != 0) {
			ALOGD("etc_db_version != data_db_version\n");
			ret = file_copy(DB_ORIGIN_RECORD, DB_DEST_RECORD);
			if (ret)
				ALOGE("[%d]failed to copy %s to %s",
				__LINE__, DB_ORIGIN_RECORD, DB_DEST_RECORD);
		}
	}

	if (is_file_exist(DB_DEST_CONFIG)) {
		ret = file_copy(DB_ORIGIN_CONFIG, DB_DEST_CONFIG);
		if (ret)
			ALOGE("failed to copy %s to %s",
			DB_ORIGIN_CONFIG, DB_DEST_CONFIG);
	} else {
		memset(origin_db_version, 0, sizeof(origin_db_version));
		memset(dest_db_version, 0, sizeof(dest_db_version));
		which_db_config_get_string(DB_ORIGIN_CONFIG, DBCONF_DB, DBCONF_DB_VERSION,
			origin_db_version, sizeof(origin_db_version), "");
		which_db_config_get_string(DB_DEST_CONFIG, DBCONF_DB, DBCONF_DB_VERSION,
			dest_db_version, sizeof(dest_db_version), "");
		ALOGD("[%s:%d]etc_db_version=%s, data_db_version=%s\n", __func__,
			__LINE__, origin_db_version, dest_db_version);
		if (strncmp(origin_db_version, dest_db_version,
			sizeof(origin_db_version)) != 0) {
			ALOGD("etc_db_version != data_db_version\n");
			ret = file_copy(DB_ORIGIN_CONFIG, DB_DEST_CONFIG);
			if (ret)
				ALOGE("[%d]failed to copy %s to %s",
				__LINE__, DB_ORIGIN_CONFIG, DB_DEST_CONFIG);
		}
	}

	return ret;
}

int db_init(char *db_common_field)
{
	int ret = 0;

	sprintf(DB_ORIGIN, "/etc/face-recg-%s.db", db_common_field);
	sprintf(DB_DEST, "/data/face-recg-%s.db", db_common_field);
	sprintf(DB_ORIGIN_FACE, "/etc/face-%s.db", db_common_field);
	sprintf(DB_DEST_FACE, "/data/face-%s.db", db_common_field);
	sprintf(DB_ORIGIN_RECORD, "/etc/record-%s.db", db_common_field);
	sprintf(DB_DEST_RECORD, "/data/record-%s.db", db_common_field);
	sprintf(DB_ORIGIN_CONFIG, "/etc/config-%s.db", db_common_field);
	sprintf(DB_DEST_CONFIG, "/data/config-%s.db", db_common_field);

	ret = db_check();
	if (ret) {
		ALOGE("db check failed");
		return ret;
	}

	ret = db_config();
	if (ret) {
		ALOGE("db config failed");
		return ret;
	}

	return 0;
}

char *db_get_face_recg_db_name()
{
	return DB_DEST;
}

char *db_get_face_db_name()
{
	return DB_DEST_FACE;
}

char *db_get_record_db_name()
{
	return DB_DEST_RECORD;
}

char *db_get_config_db_name()
{
	return DB_DEST_CONFIG;
}

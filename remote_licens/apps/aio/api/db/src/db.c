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

#ifdef _WIN32
#define ALOGD printf
#define ALOGE printf
/* //#define ALOGD printf */
#else
#define LOG_TAG "DB"
#include <log/log.h>
#endif
#include "db_persistence.h"
#include "file_ops.h"

#ifdef _WIN32
#define DB_ORIGIN "./etc/face-recg.db"
#define DB_DEST "./data/face-recg.db"
#else
#define DB_ORIGIN "/etc/face-recg.db"
#define DB_DEST "/data/face-recg.db"
#endif
#ifndef _WIN32
static int db_check(void)
{
	int ret;

	ALOGD("db check\n");

	/* whether there is a db in /data */
	if (!is_file_exist(DB_DEST))
		return 0;

	/* copy db from /etc to /data */
	ALOGD("copy %s to %s", DB_ORIGIN, DB_DEST);
	ret = file_copy(DB_ORIGIN, DB_DEST);
	if (ret) {
		ALOGE("failed to copy %s to %s", DB_ORIGIN, DB_DEST);
		return -1;
	}

	return 0;
}
#endif
#define MSG_LEN 128
typedef struct {
	int err_no;
	char err_msg[MSG_LEN];
} error_log_t;
static error_log_t error_log;
static inline void clear_msg()
{
	memset(&error_log, 0x00, sizeof(error_log_t));
}
static void error_custom_call_back(void *p_arg, int i_err_code,
								   const char *p_msg)
{
	ALOGD("[**db error**]:(sqlite_error_no:%d) %s\n", i_err_code, p_msg);
	clear_msg();
	error_log.err_no = i_err_code;
	memcpy(error_log.err_msg, (void *)p_msg, MSG_LEN);
}
int get_err_no() { return error_log.err_no; }
const char *get_err_msg() { return error_log.err_msg; }

int db_init(void)
{
	int ret;
#ifndef _WIN32
	ret = db_check();
	if (ret) {
		ALOGE("db check failed");
		return ret;
	}
#endif
	/*	ret = db_config(); */
	ret = db_custom_config(error_custom_call_back);
	if (ret) {
		ALOGE("db config failed");
		return ret;
	}
	return 0;
}

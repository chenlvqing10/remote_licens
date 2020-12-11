/*
 * test_log_svc.c
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
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define LOG_TAG "test_log_svc"
#include <log/log.h>

#include "db_all.h"
#include "face_svc.h"

#include "test_log_svc.h"

#define TEST_LOG_DATA_SIZE 1024
#define _maxsize_ 32
#define _format2_ "%Y%m%d%H%M%S"

static int test_log_fd = -1;
static char *test_log_file = "/data/log/face_recg_live_record_default.csv";

static char *test_ts2str2(long long ts)
{
	static char str[_maxsize_];
	time_t t = ts / 1000000;
	long usec = ts - (ts / 1000000) * 1000000;
	size_t len = strftime(str, _maxsize_, _format2_, localtime(&t));
	snprintf(&str[len], _maxsize_ - len, "%ld", usec);
	return str;
}

void test_log_open_file(void)
{
	/* file mode 644 */
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

	test_log_fd = open(test_log_file, O_RDWR | O_APPEND);
	if (test_log_fd == -1 && errno == ENOENT)
		test_log_fd = open(test_log_file, O_RDWR | O_CREAT | O_TRUNC, mode);
}

static void _test_log_write(void *buf, size_t size)
{
	if (test_log_fd < 0) {
		test_log_open_file();
	}

	write(test_log_fd, buf, size);
}

void test_log_write(const char *fmt, ...)
{
	int size = 0;
	char log_data[TEST_LOG_DATA_SIZE] = {0};

	va_list ap;
	va_start(ap, fmt);
	size += vsnprintf(log_data + size, TEST_LOG_DATA_SIZE - size, fmt, ap);
	va_end(ap);

	_test_log_write(log_data, size);
}

void test_log_close_file(void)
{
	close(test_log_fd);
}

void record_default_header(void)
{
	test_log_write("\n%s, %s, %s, %s, %s, %s, %s\n",
	"index",
	"timestamp",
	"name_id",
	"name",
	"recg_time",
	"living_time",
	"pic_path");
}

void test_log_record_default(web_socket_checkin_info_t *p_info, char *user_name,
	face_record_t *record)
{
	if (record->type == unmatch_face || record->type == match_face) {
		static int index = 0;
		char *name_id = NULL;
		char *name = NULL;
		long long timestamp = record->time;
		double recg_time = 0;
		double living_time = 0;
		if (record->type == match_frame) {
			name_id = p_info->user_id;
			name = user_name;
		} else {
			name_id = " ";
			name = " ";
		}
		if (record->recg_timestamp > 0)
			recg_time = (record->recg_timestamp - record->add_timestamp);
		if (record->living_timestamp > 0)
			living_time = (record->living_timestamp - record->add_timestamp);
		char *pic_path = record->img.path[record->type];
		if (index == 0)
			record_default_header();

		++index;
		test_log_write("%d, %s, %s, %s, %2.3f, %2.3f, %s\n",
			index, test_ts2str2(timestamp), name_id, name,
			recg_time / 1000000, living_time / 1000000, pic_path);
		ALOGE("%d, %s, %s, %s, %2.3f, %2.3f, %s\n",
			index, test_ts2str2(timestamp), name_id, name,
			recg_time / 1000000, living_time / 1000000, pic_path);
	}
}


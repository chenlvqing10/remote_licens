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
#include <dirent.h>
#include <sys/stat.h>

#include "db_sqlite_middle.h"
#include "ee_queue.h"
#include "db_punch_record.h"
#include "record_info.h"
#include "common_person_info.h"
#include "time_conv.h"
#include "file_ops.h"
#include "storage.h"

#define LOG_TAG "record_auto_delete.c"
#include <log/log.h>

#define MMC_DATA_DISK_PATH "/data"
#define MAX_LIMIT_COUNT 10000
#define MAX_LIMIT_USE_RATIO_PERCENT 90

static int is_deleting_flag;

/**
 * is_deleting_record - return flag of deleting record.
 * @
 *
 * Returns 1 if success, otherwise 0.
 */
int is_deleting_record()
{
	return is_deleting_flag;
}

static void delete_logcat_files()
{
	DIR *dir;
	struct dirent *ptr;

	dir = opendir("/data/log");
	while ((ptr = readdir(dir)) != NULL) {
		char d_name_temp[256];
		memcpy(d_name_temp, ptr->d_name, 256);
		char *filename = strtok(d_name_temp, ".");
		if (filename != NULL) {
			char deleteFileName[512];
			struct stat fileStat;
			sprintf(deleteFileName, "/data/log/%s", ptr->d_name);
			if (stat(deleteFileName, &fileStat) == 0) {
				time_t cursec;
				time_get_current_sec(&cursec);
				if (cursec - fileStat.st_mtime > 3600)
					file_remove(deleteFileName);
			}
		}
	}

	closedir(dir);
	file_system_sync();
}

/**
 * punch_record_auto_del_interval_proc - Delete punch record auto thread proc.
 * @
 *
 * Returns 0 if success, otherwise not 0.
 */
void *punch_record_auto_del_interval_proc(void *arg)
{
	char buf[128] = {0};
	int query_count = 0;
	double t, t_before;
	double min_tval = 0;
	ee_queue_t head;
	punch_record_t *q = NULL;

	int count = 0, ret;
	int percent = 0;
	double t_after = 0;

	is_deleting_flag = 0;

	while (1) {
		is_deleting_flag = 0;

		t = time(NULL);

		memset(buf, 0, sizeof(buf));
		time_second_to_str(t, buf, sizeof(buf));
		ALOGD("[%s:%d]t=%.0lf, %s\n", __func__, __LINE__, t, buf);

		t_before = get_before_n_day_timestamp(&t, 90);
		min_tval = db_punch_record_query_min_punch_time();

		memset(buf, 0, sizeof(buf));
		time_second_to_str(t_before, buf, sizeof(buf));
		ALOGD("[%s:%d]t_before=%.0lf, %s\n", __func__, __LINE__,
			t_before, buf);

		memset(buf, 0, sizeof(buf));
		time_second_to_str(min_tval/1000, buf, sizeof(buf));
		ALOGD("[%s:%d]min_tval=%.0lf, %s\n", __func__, __LINE__,
			min_tval, buf);

		if (min_tval/1000 < t_before) {
			is_deleting_flag = 1;
			query_count = db_punch_record_query_begin_by_less_timestamp(
				t_before*1000, &head);
			ALOGD("[%s:%d]query_count=%d\n", __func__, __LINE__,
				query_count);
			if (query_count > 0) {
				ee_queue_foreach(q, &head, punch_record_t, queue) {
					ret = file_remove(q->punch_live_large_pic_name);
					if (ret)
						ALOGE("%s: failed to remove file(%s)",
						__func__, q->punch_live_large_pic_name);
					ret = file_remove(q->punch_live_small_pic_name);
					if (ret)
						ALOGE("%s: failed to remove file(%s)",
						__func__, q->punch_live_small_pic_name);
					db_punch_record_delete_by_id(q->id);
				}
			}
			db_punch_record_query_end(&head);

			/* Synchronize file system data into physical media */
			file_system_sync();
		}
		is_deleting_flag = 0;

		count = db_punch_record_count_all();
		ALOGD("[%s:%d]all_count=%d\n", __func__, __LINE__, count);
		while (count >= MAX_LIMIT_COUNT) {
			is_deleting_flag = 1;
			ALOGD("[%s:%d]max_count rule:all_count=%d\n", __func__, __LINE__,
				count);
			min_tval = db_punch_record_query_min_punch_time();
			t = min_tval/1000;
			t_after = get_after_n_day_timestamp(&t, 1);
			query_count =
				db_punch_record_query_begin_by_between_timestamp(
				min_tval, t_after*1000, &head);
			ALOGD("[maxcount rule]query_count=%d\n", query_count);
			if (query_count > 0) {
				ee_queue_foreach(q, &head, punch_record_t, queue) {
					ret = file_remove(q->punch_live_large_pic_name);
					if (ret)
						ALOGE("%s: failed to remove file(%s)",
						__func__, q->punch_live_large_pic_name);
					ret = file_remove(q->punch_live_small_pic_name);
					if (ret)
						ALOGE("%s: failed to remove file(%s)",
						__func__, q->punch_live_small_pic_name);
					db_punch_record_delete_by_id(q->id);
				}
			}
			db_punch_record_query_end(&head);
			/* Synchronize file system data into physical media */
			file_system_sync();

			count = db_punch_record_count_all();
		}
		is_deleting_flag = 0;

		percent = get_disk_use_ratio_round_up_percent(MMC_DATA_DISK_PATH);
		ALOGD("[%s:%d]percent=%d, free_percent=%d\n", __func__, __LINE__,
			percent, (100-percent));
		while (percent >= MAX_LIMIT_USE_RATIO_PERCENT) {
			is_deleting_flag = 1;
			ALOGD("[%s:%d]max_percent rule:percent=%d\n", __func__, __LINE__,
				percent);
			min_tval = db_punch_record_query_min_punch_time();
			t = min_tval/1000;
			t_after = get_after_n_day_timestamp(&t, 1);
			query_count =
				db_punch_record_query_begin_by_between_timestamp(
				min_tval, t_after*1000, &head);
			ALOGD("[maxpercent rule]query_count=%d\n", query_count);
			if (query_count > 0) {
				ee_queue_foreach(q, &head, punch_record_t, queue) {
					ret = file_remove(q->punch_live_large_pic_name);
					if (ret)
						ALOGE("%s: failed to remove file(%s)",
						__func__, q->punch_live_large_pic_name);
					ret = file_remove(q->punch_live_small_pic_name);
					if (ret)
						ALOGE("%s: failed to remove file(%s)",
						__func__, q->punch_live_small_pic_name);
					db_punch_record_delete_by_id(q->id);
				}
			}
			db_punch_record_query_end(&head);
			/* Synchronize file system data into physical media */
			file_system_sync();

			delete_logcat_files();

			if (query_count <= 0)
				break;
			percent = get_disk_use_ratio_round_up_percent(MMC_DATA_DISK_PATH);
		}
		is_deleting_flag = 0;
		sleep(3600);
	}

	return 0;
}

/**
 * punch_record_auto_del_proc - Delete punch record auto at intervals.
 * @
 *
 * Returns 0 if success, otherwise not 0.
 */
int punch_record_auto_del_proc()
{
	pthread_t p;

	pthread_create(&p, NULL, punch_record_auto_del_interval_proc, NULL);
	pthread_detach(p);

	return 0;
}
/*
 * api/record_db.c - all functions for punch record.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>

#include "cJSON.h"
#include "config_db.h"

#define LOG_TAG "record_db.c"
#include <log/log.h>

#include "restful_api.h"
#include "base64.h"
#include "person.h"
#include "common_record_info.h"
#include "record_db.h"
#include "file_ops.h"
#include "sys_http.h"
#include "time_conv.h"
#include "db_punch_record.h"
#include "lb_os_http.h"

#include "sqlite3.h"
#include "time.h"

#define tol_start_time "2019/06/08 08:08"
#define tol_end_time "2022/06/08 08:08"

/**
 * total_punch_record: total punch record
 * @person: person info of querying
 * @record: punch info of querying
 * @start_time: start punch time
 * @end_time: end punch time
 *
 * @return 0-successful, other-failed
 */
int total_punch_record(person_t *person, punch_record_t *record,
	double start_time, double end_time)
{
	int total = 0;
	ee_queue_t head_record;

	if (!person || !record)
		return -1;

	ee_queue_init(&head_record);
	// total = db_punch_record_full_info_query_begin_of_upload(head_record);

	return total;
}

/**
 * pack_punch_data: pach punch data
 * @q_person: a point of struc person_info_t
 * @q_record: a point of struc punch_record_t
 * @punch_info: a point of struc punch_record_info
 * @record_total: record total
 *
 * @return 0-successful, other-failed
 */
int pack_punch_data(punch_record_t *punch_record,
	punch_record_info *punch_info, int record_total)
{
	char buf[128];

	/** check para */
	if ((punch_record == NULL) || (punch_info == NULL)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		return -1;
	}

	ALOGD("[%s : %d]\n", __func__, punch_record->id);
	punch_info->id = punch_record->id;
	punch_info->match_score = punch_record->match_score;
	punch_info->record_total = record_total;
	time_second_to_str(punch_record->punch_time / 1000, buf, sizeof(buf));
	strcpy(punch_info->punch_time, buf);
	strcpy(punch_info->address, punch_record->address);
	strcpy(punch_info->punch_live_large_pic_name,
		punch_record->punch_live_large_pic_name);
	strcpy(punch_info->punch_live_small_pic_name,
		punch_record->punch_live_small_pic_name);

	return 0;
}

/**
 * search_punch_record - Query punch record
 * @p_info: a pointer of save punch record
 *
 * @return 0-successful, other-failed
 */
int search_punch_record(query_punch_record_info *p_info,
	punch_record_info *punch_info)
{
	/** init para */
	int ret = -1;
	int i = 0;
	ee_queue_t head;
	punch_record_t *record_info = NULL;
	ee_queue_init(&head);

	/** Check para */
	if ((NULL == p_info) || (NULL == punch_info)) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		goto Exit;
	}
	/** Default time */
	if (p_info->start_punch_time == 0)
		time_str_to_sec(tol_start_time, &p_info->start_punch_time);
	if (p_info->end_punch_time == 0)
		time_str_to_sec(tol_end_time, &p_info->end_punch_time);

	/** Get total record */
	punch_info[0].record_total = db_punch_record_count(p_info->name,
		p_info->punch_pass,
		(p_info->start_punch_time * 1000),
		(p_info->end_punch_time * 1000));
	if (punch_info[0].record_total < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}
	if (p_info->record_count > punch_info[0].record_total)
		p_info->record_count = punch_info[0].record_total;

	ALOGD("[%s : %d : %d]\n", __func__, __LINE__,
			punch_info[0].record_total);

	/** query punch data and pack */
	ret = db_punch_record_query_begin(p_info->name,
		p_info->punch_pass,
		(p_info->start_punch_time * 1000),
		(p_info->end_punch_time * 1000),
		p_info->start_id,
		p_info->record_count,
		&head);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ALOGD("[%s : %d : %d]\n", __func__, __LINE__, ret);

	ee_queue_foreach(record_info, &head, punch_record_t, queue) {
		ALOGD("[---> record -> id %d]", record_info->id);
		pack_punch_data(record_info, &punch_info[i], punch_info[0].record_total);
		i++;
	}

Exit:
	db_punch_record_query_end(&head);
	if (ret >= 0)
		return 0;
	else
		return ret;
}


/**
 * delete_all_punch_record: delete all punch record
 *
 * Returns 0 means success, other means failure.
 */
int delete_all_punch_record()
{
	int ret = 0;
	ee_queue_t head;
	punch_record_t *q = NULL;
	double max_tval;
	double min_tval = 0;
	char get_time[30] = {0};

	max_tval = time(NULL) * 1000.0;
	min_tval = db_punch_record_query_min_punch_time();
	time_sec_to_str(max_tval / 1000, get_time, sizeof(get_time));
	ALOGD("max_tval --> %s", get_time);
	memset(get_time, 0, sizeof(get_time));
	time_sec_to_str(min_tval / 1000, get_time, sizeof(get_time));
	ALOGD("min_tval --> %s", get_time);

	ee_queue_init(&head);
	ret = db_punch_record_query_begin_by_between_timestamp(
			min_tval, max_tval, &head);
	if (ret <= 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}
	ALOGD("ret --> %d", ret);

	ret = 0;
	ee_queue_foreach(q, &head, punch_record_t, queue) {
		ret |= file_remove(q->punch_live_large_pic_name);
		ret |= file_remove(q->punch_live_small_pic_name);
		ret |= db_punch_record_delete_by_id(q->id);
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			goto Exit;
		}
	}

Exit:
	db_punch_record_query_end(&head);

	return ret;
}
/**
 * delete_punch_record: delete punch record by id or employee_numb ...
 * persion: person' information
 *
 * Returns 0 means success, other means failure.
 */
int delete_punch_record(query_punch_record_info *p_info)
{
	int ret = 0, i;
	punch_record_info punch_info[DEL_NUM + 1];

	/** para init */
	p_info->record_count = DEL_NUM;
	memset(punch_info, 0, DEL_NUM*sizeof(punch_record_info));

	/** para check */
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ret = search_punch_record(p_info, punch_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}
	while (punch_info[0].record_total != 0) {
		for (i = 0; i < p_info->record_count; i++) {
			int id = punch_info[i].id;
			ALOGD("---->[delete : %d]\n", id);
			ret = db_punch_record_delete_by_id(id);
			ALOGD("[ret = %d]\n", ret);
			if (ret) {
				ALOGE("[%s : %d]\n", __func__, __LINE__);
				goto Exit;
			}
		}
		memset(punch_info, 0, DEL_NUM*sizeof(punch_record_info));
		ret = search_punch_record(p_info, punch_info);
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			goto Exit;
		}
	}

Exit:
	return ret;
}

/**
 * time_str_to_second - Convert time string to seconds.
 * @str_time A pointer to char time
 * @sec Out, time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_str_to_sec(const char *str_time, double *out_sec)
{
	int ret = 0;
	time_t t = 0;
	struct tm *tmp_time = NULL;

	if (str_time == NULL || out_sec == NULL)
		return -1;

	tmp_time = (struct tm *)malloc(sizeof(struct tm));
	/* Note: must be initialized, otherwise tm_isdst will be an arbitrary value */
	memset(tmp_time, 0, sizeof(struct tm));
	strptime(str_time, "%Y/%m/%d %H:%M:%S", tmp_time);
	t = mktime(tmp_time);
	if (t != 0) {
		*out_sec = (long)(t);
		ret = 0;
	} else {
		ret = -1;
	}

	lb_mem_free(tmp_time);

	return ret;
}

/**
 * time_second_to_str - Convert seconds to time strings
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_sec_to_str(double sec, char *out_str_time, unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	if (out_str_time == NULL)
		return -1;

	t = sec;
	tmp = localtime(&t);
	if (NULL == tmp) {
		printf("localtime error\n");
		return -1;
	}

	s = strftime(out_str_time, max_out_str_len, "%Y/%m/%d %H:%M:%S", tmp);
	if (0 == s) {
		printf("strftime returned 0\n");
		return -1;
	}

	return 0;
}
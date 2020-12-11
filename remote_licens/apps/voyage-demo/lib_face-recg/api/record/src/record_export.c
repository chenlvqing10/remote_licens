/*
 * record_export.c - Export punch record to excel file.
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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "char_conv.h"
#include "ee_queue.h"
#include "db_person_info.h"
#include "db_punch_record.h"
#include "time_conv.h"
#include "excel_file_write.h"
#include "record_export.h"

#include "elang.h"
#include "file_ops.h"
#include "hotplug.h"
#include "storage.h"

#define LOG_TAG "RECORD_EXPORT"
#include <log/log.h>

#define UDISK_DEVICE				"/mnt/usb"

/* U disk will be full threshold(unit: MB) */
#define UDISK_FULL_THRESHOLD			200

/* Check the frequency of memory size */
#define UDISK_MEMORY_CHECK_LOOP			100

/* The excel file name of the punch record */
#define PUNCH_REC_EXPORT_EXCEL_NAME		"/mnt/usb/RECG/record"

/* The excel file name of the punch record export dir */
#define PUNCH_REC_EXPORT_DIR			"/mnt/usb/RECG"

/* The excel file name of the punch record export pic dir*/
#define PUNCH_REC_EXPORT_DIR_PIC		"/mnt/usb/RECG/DCIM"

/* The punch record pictures, relative to the path of the U disk, for user viewing */
#define PUNCH_REC_EXPORT_PIC_RELATIVE_U_DISK_DIR	"/RECG/DCIM"

/* Dir of punch-in pictures on the device */
#define PUNCH_REC_PIC_DEVICE_DIR		"/data/face_recg"

/* Number of punch records in one query */
#define QUERY_PUNCH_REC_COUNT_ONE_TIME		1000

static record_export_ctx_t export_ctx;

/* Set the exit flag of imports */
int record_export_set_proc_exit_flag(int is_exit)
{
	export_ctx.is_proc_exit = is_exit;

	return 0;
}

/* Get the exit flag of imports */
static int record_export_get_proc_exit_flag(int *is_proc_exit)
{
	*is_proc_exit = export_ctx.is_proc_exit;

	return 0;
}

/* set sum status */
static int record_export_set_sum_status(int sum_count)
{
	export_ctx.status.sum_count = sum_count;

	return 0;
}

/* set sum status */
static int record_export_set_current_status(int current_count, int fail_count)
{
	export_ctx.status.current_count = current_count;

	export_ctx.status.fail_count = fail_count;

	return 0;
}

/* Set thread completion status falg */
static int record_export_set_done_status(thread_done_e is_done)
{
	export_ctx.done = is_done;

	return 0;
}

/* Get record export contex */
int record_export_get_ctx(record_export_ctx_t *p_ctx)
{
	assert(p_ctx != NULL);

	memcpy(p_ctx, &export_ctx, sizeof(record_export_ctx_t));

	return 0;
}

/* Set thread result code */
static int record_export_set_result_code(record_export_error_type_e result_code)
{
	export_ctx.result_code = result_code;

	return 0;
}

/* Init record export contex */
int record_export_init_ctx(void)
{
	memset(&export_ctx, 0, sizeof(record_export_ctx_t));
	export_ctx.done = EE_THREAD_NOT_RUN;

	return 0;
}

/**
 * excel_punch_record_write_head - Write punch record head to Excel file.
 * @ws Point to an excel file handle
 * @excel_row Line number in the excel file
 *
 * Returns 0 if success, otherwise not 0.
 */
static int record_export_excel_head_write(lxw_worksheet *ws, int excel_row)
{
	int ret = 0;
	int index = 0;
	lxw_error error;

	assert(ws != NULL);

	/* Employee mumber(E.g SW200) */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_EMPLOYEE_NUMBER"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Name */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_NAME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Gender(E.g "Man/Women/..." */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_GENDER"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* ID card number */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_ID_CARD_NUMBER"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Department name(E.g "Software Lab001") */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_DEPARTMENT_NAME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Face recognition score */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_MATCH_SCORE"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* The address where punched */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_ADDRESS"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Punch time */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_PUNCH_TIME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Punch pass */
	error = worksheet_write_string(ws, excel_row, index++,
			elang_str("TEXT_PUNCH_PASS"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* The name of the large picture of the punch record */
	error = worksheet_write_string(ws, excel_row, index++,
		elang_str("TEXT_PUNCH_LIVE_LARGE_PIC_NAME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* The name of the small picture of the punch record */
	error = worksheet_write_string(ws, excel_row, index++,
		elang_str("TEXT_PUNCH_LIVE_SMALL_PIC_NAME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Show export results */
	error = worksheet_write_string(ws, excel_row, index++,
		elang_str("TEXT_EXPORT_RESULT"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	ret = 0;
	goto EXCEL_WRITE_OK;

EXCEL_WRITE_ERROR:
	ALOGD("[%s:%d] Write excel fail: %d=%s\n", __func__, __LINE__, error,
		lxw_strerror(error));
	ret =  -1;

EXCEL_WRITE_OK:

	return ret;
}

/**
 * record_export_excel_row_write - Write a row of punch record to Excel file.
 * @ws Point to an excel file handle
 * @excel_row Line number in the excel file
 * @p_full_rec_info: Structure for storing punch record information
 * @is_export_ok true: Export successfully
 *               false: Export failed
 *
 * Returns 0 if success, otherwise not 0.
 */
static int record_export_excel_row_write(lxw_worksheet *ws,
				unsigned int excel_row,
				full_punch_rec_info_t *p_rec_info,
				bool is_export_ok
				)
{
	int ret = 0;
	int index = 0;
	lxw_error error;
	char *str_new_path;
	char str_score[64];
	char out_str_time[64];
	long sec_punch_time = 0;
	char str_export_result[64];

	assert((ws != NULL) && (p_rec_info != NULL));

	/* Employee mumber(E.g SW200) */
	error = worksheet_write_string(ws, excel_row, index++,
		p_rec_info->employee_number, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Name */
	error = worksheet_write_string(ws, excel_row, index++, p_rec_info->name, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Gender(E.g "Man/Women/..." */
	error = worksheet_write_string(ws, excel_row, index++, p_rec_info->gender, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* ID card number */
	error = worksheet_write_string(ws, excel_row, index++,
		p_rec_info->id_card_number, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Department name(E.g "Software Lab001") */
	error = worksheet_write_string(ws, excel_row, index++,
		p_rec_info->department_name, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Face recognition score
	 * When exporting records, only 2 decimal digits are reserved, which is
	 * consistent with the UI display.
	 */
	memset(str_score, 0, sizeof(str_score));

	/* Keep 2 decimal digits, not rounded, avoid 0.499 to 0.50 */
	snprintf(str_score, sizeof(str_score), "%.2f",
		((int)((p_rec_info->match_score) * 100) / 100.0));

	error = worksheet_write_string(ws, excel_row, index++, str_score, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* The address where punched */
	error = worksheet_write_string(ws, excel_row, index++, p_rec_info->address, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Punch time
	 *
	 * Note: Change double time to string.
	 */
	memset(out_str_time, 0, sizeof(out_str_time));
	sec_punch_time = p_rec_info->punch_time / TIME_BETWEEN_S_AND_MS_CONVER;
	time_second_to_str(sec_punch_time, out_str_time, sizeof(out_str_time));
	error = worksheet_write_string(ws, excel_row, index++, out_str_time, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Punch pass */
	worksheet_write_number(ws, excel_row, index++, p_rec_info->punch_pass, NULL);

	/* The name of the large picture of the punch record */
	str_new_path = string_replace(p_rec_info->punch_live_large_pic_name,
		PUNCH_REC_PIC_DEVICE_DIR,
		PUNCH_REC_EXPORT_PIC_RELATIVE_U_DISK_DIR);
	if (str_new_path != NULL) {
		error = worksheet_write_string(ws, excel_row, index++, str_new_path,
			NULL);
		free(str_new_path);

		if (error != LXW_NO_ERROR)
			goto EXCEL_WRITE_ERROR;

	} else
		index++;

	/* The name of the small picture of the punch record */
	str_new_path = string_replace(p_rec_info->punch_live_small_pic_name,
			PUNCH_REC_PIC_DEVICE_DIR,
			PUNCH_REC_EXPORT_PIC_RELATIVE_U_DISK_DIR);
	if (str_new_path != NULL) {
		error = worksheet_write_string(ws, excel_row, index++, str_new_path,
			NULL);
		free(str_new_path);
		if (error != LXW_NO_ERROR)
			goto EXCEL_WRITE_ERROR;

	} else
		index++;

	/* Write the export result to excel */
	memset(str_export_result, 0, sizeof(str_export_result));
	if (true == is_export_ok)
		snprintf(str_export_result, sizeof(str_export_result),
			elang_str("TEXT_REC_EXPORT_SUCCESS"));
	else
		snprintf(str_export_result, sizeof(str_export_result),
			elang_str("TEXT_REC_EXPORT_FAIL"));
	error = worksheet_write_string(ws, excel_row, index++, str_export_result, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	ret = 0;
	goto EXCEL_WRITE_OK;

EXCEL_WRITE_ERROR:
	ALOGD("[%s:%d] Write excel fail: %d=%s\n", __func__, __LINE__, error,
		lxw_strerror(error));
	ret =  -1;

EXCEL_WRITE_OK:

	return ret;
}

/**
 * export_pic_file_by_path - export  pic file by path.
 * @path: A pointer to the string.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int export_pic_file_by_path(char *path)
{
	int ret = 0;
	char *dname;
	char dirc[128];
	char *str_new_path;

	if (NULL == path) {
		ALOGE("[%s:%d] path is NULL\n", __func__, __LINE__);
		return -1;
	}

	str_new_path = string_replace(path, PUNCH_REC_PIC_DEVICE_DIR,
		PUNCH_REC_EXPORT_DIR_PIC);
	if (NULL == str_new_path) {
		ALOGE("[%s:%d] string_replace fail: %s\n", __func__, __LINE__, path);
		return -1;
	}

	/* Get the path where the picture file is located (excluding the
	 * file name)
	 */
	memset(dirc, 0, sizeof(dirc));
	snprintf(dirc, sizeof(dirc), "%s", str_new_path);
	dname = dirname(dirc);

	/* Create a folder to store pictures */
	if (is_dir_exist(dname) != 0) {
		ret = mk_mul_dirs(dname);
		if (ret != 0) {
			ALOGE("[%s:%d] Create Dir fail: %s\n", __func__, __LINE__,
				dname);
			free(str_new_path);
			return -1;
		}
	}

	ALOGD("[%s:%d] record export: path=%s, str_new_path=%s\n", __func__, __LINE__,
		path, str_new_path);

	/* Copy the picture to the corresponding folder of the U disk */
	ret = file_copy(path, str_new_path);
	if (ret != 0) {
		ret = -1;
		ALOGE("[%s:%d] cp (%s) to (%s) fail\n", __func__, __LINE__,
			path, str_new_path);
	}

	if (is_file_exist(str_new_path) != 0) {
		ret = -1;
		ALOGE("[%s:%d] file (%s) not exist\n", __func__, __LINE__,
			str_new_path);
	}

	free(str_new_path);

	return ret;
}

/**
 * record_export_proc - The entry function for exporting punch records to Excel files
 * @arg: Thread function parameters
 *
 *
 * Returns 0 if success, otherwise not 0.
 */
void *record_export_proc(void *arg)
{
	int tmp_ret = 0;
	int close_ret = 0;
	int row = 0;
	int ret = 0;
	int count = 0;
	lxw_workbook *wb;
	lxw_worksheet *ws;
	ee_queue_t head;
	int fail_count = 0;
	int is_proc_exit = 0;
	float free_size = 0.0;
	int current_count = 0;
	person_info_t person;
	int excel_write_ret = 0;
	punch_record_t *q = NULL;
	full_punch_rec_info_t info;
	static int copy_count;
	char *user_name = NULL;
	int rec_pass = 0;
	double rec_start_time = 0;
	double rec_end_time = 0;
	char str_time[64];
	bool is_export_ok = false;
	int copy_large_pic_ret = 0;
	int copy_small_pic_ret = 0;
	char write_excel_file_name[128];
	record_export_error_type_e proc_ret = RECORD_EXPORT_OK;
	export_query_condition_t query_condition;

	assert(arg != NULL);

	memset(&query_condition, 0, sizeof(export_query_condition_t));
	memcpy(&query_condition, arg, sizeof(export_query_condition_t));
	user_name = query_condition.user_name;
	rec_pass = query_condition.rec_pass;
	rec_start_time = query_condition.rec_start_time;
	rec_end_time = query_condition.rec_end_time;

	/* Change the thread execution state to the running state */
	record_export_set_done_status(EE_THREAD_RUNNING);

	if (udisk_is_ok() != 0) {
		proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
		goto RUN_END;
	}

	/* Create a RECG folder in the root directory of the U Disk*/
	if (is_dir_exist(PUNCH_REC_EXPORT_DIR) != 0) {
		tmp_ret = mk_mul_dirs(PUNCH_REC_EXPORT_DIR);
		if (tmp_ret != 0) {
			proc_ret = RECORD_EXPORT_WRITE_ERR;
			goto RUN_END;
		}
	}

	/* Step1: Create an excel file that writes a punch record */
	memset(str_time, 0, sizeof(str_time));
	time_second_to_file_name_str(time(NULL), str_time, sizeof(str_time));

	memset(write_excel_file_name, 0, sizeof(write_excel_file_name));
	snprintf(write_excel_file_name, sizeof(write_excel_file_name), "%s_%s.xlsx",
		PUNCH_REC_EXPORT_EXCEL_NAME, str_time);

	tmp_ret = excel_file_write_open(write_excel_file_name,
		elang_str("TEXT_PUNCH_REC_EXPORT_EXCEL_SHEET_TITEL"), &wb, &ws);
	if (tmp_ret != 0) {
		proc_ret = RECORD_EXPORT_WRITE_ERR;
		goto RUN_END;
	}

	/* Step2: Write the title of excel */
	ret = record_export_excel_head_write(ws, row++);

	/* Step3: Check the number of punch records matching the query conditions
	 * from the database
	 */
	count = db_punch_record_count(user_name, rec_pass, rec_start_time, rec_end_time);
	record_export_set_sum_status(count);

	/* Step4: Query the punch record from the database that meets the
	 *  query conditions
	 */
	db_punch_record_query_begin(user_name, rec_pass, rec_start_time, rec_end_time,
		0, count, &head);
	ee_queue_foreach(q, &head, punch_record_t, queue) {
		/* Detecting the available space for the udisk */
		if (0 == (copy_count % UDISK_MEMORY_CHECK_LOOP)) {
			ret = get_disk_free(UDISK_DEVICE, &free_size);
			if (ret != 0) {
				ALOGE("[%s:%d] Get disk status fail %s\n",
						__func__, __LINE__, strerror(errno));
				proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
				break;
			}

			if (free_size < UDISK_FULL_THRESHOLD) {
				proc_ret = RECORD_EXPORT_UDISK_FULL_ERR;
				break;
			}
		}
		/* Judge the import process to end early */
		ret = record_export_get_proc_exit_flag(&is_proc_exit);
		if ((0 == ret) && (1 == is_proc_exit)) {
			proc_ret = RECORD_EXPORT_USER_EXIT_ERR;
			break;
		}

		memset(&info, 0, sizeof(info));

		/* If the rejected record is queried, the corresponding person
		 * information will not be obtained because the person may not be
		 * registered.
		 */
		if (rec_pass != 0) {
			/* Find the person information corresponding to the punch
			 * record
			 */
			db_person_info_query_by_id(q->fk_per_id, &person);

			/* Employee mumber(E.g SW200) */
			snprintf(info.employee_number, sizeof(info.employee_number),
				"%s", person.employee_number);

			/* Name */
			/* When the record is rejected, the name of the person is not
			 * displayed
			 */
			snprintf(info.name, sizeof(info.name), "%s", person.name);

			/* Gender(E.g "Man/Women/..." */
			snprintf(info.gender, sizeof(info.gender), "%s", person.gender);

			/* ID card number */
			snprintf(info.id_card_number, sizeof(info.id_card_number), "%s",
				person.id_card_number);

			/* Department name(E.g "Software Lab001") */
			snprintf(info.department_name, sizeof(info.department_name), "%s",
				person.department_name);

			/* Register photo name.(Including jpg suffix) */
			snprintf(info.regist_photo_name, sizeof(info.regist_photo_name),
				"%s", person.regist_photo_name);
		}

		/* Face recognition score */
		info.match_score = q->match_score;

		/* The address where punched */
		snprintf(info.address, sizeof(info.address), "%s", q->address);

		/* Punch time(ms) */
		info.punch_time = q->punch_time;

		/* The name of the large picture of the punch record */
		snprintf(info.punch_live_large_pic_name,
			sizeof(info.punch_live_large_pic_name), "%s",
			q->punch_live_large_pic_name);

		/*  The name of the small picture of the punch record */
		snprintf(info.punch_live_small_pic_name,
			sizeof(info.punch_live_small_pic_name), "%s",
			q->punch_live_small_pic_name);

		/* Punch pass */
		info.punch_pass = q->punch_pass;

		/* Copy the punched live picture to PUNCH_PIC folder */
		copy_large_pic_ret = export_pic_file_by_path(
			q->punch_live_large_pic_name);
		copy_small_pic_ret = export_pic_file_by_path(
			q->punch_live_small_pic_name);
		if ((copy_large_pic_ret != 0) || (copy_small_pic_ret != 0)) {
			if (udisk_is_ok() != 0) {
				proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
				break;
			}

			fail_count++;

			is_export_ok = false;
		} else {
			is_export_ok = true;
			copy_count++;
		}

		/* Step5: Write the punch record to the excel file */
		excel_write_ret = record_export_excel_row_write(ws, row++,
			&info, is_export_ok);
		if (excel_write_ret != 0) {
			ALOGE("[%s:%d] Write excel file fail\n", __func__, __LINE__);
			break;
		}

		/* Step6: Update current export status */
		current_count++;
		record_export_set_current_status(current_count, fail_count);

		/* Synchronize file system data into physical media */
		file_system_sync();
	}
	db_punch_record_query_end(&head);

	copy_count = 0;

	/* Step6: Close the excel */
	close_ret = excel_file_write_close(wb, write_excel_file_name);
	if ((close_ret != 0) || (excel_write_ret != 0)) {
		proc_ret = RECORD_EXPORT_WRITE_ERR;
		ALOGE("[%s:%d] Close excel file fail\n", __func__, __LINE__);
	}

RUN_END:
	/* Synchronize file system data into physical media */
	file_system_sync();

	/* Change the thread execution state to the end state */
	record_export_set_result_code(proc_ret);
	record_export_set_done_status(EE_THREAD_NOT_RUN);

	return NULL;
}

/**
 * record_export_thread - Use thread to process the record export function
 * in the backgroud.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_export_thread(export_query_condition_t *query_condition)
{
	record_export_init_ctx();

	pthread_create(&(export_ctx.tid), NULL, record_export_proc,
		(void *)query_condition);
	pthread_detach(export_ctx.tid);

	return 0;
}



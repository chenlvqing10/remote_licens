/*
 * checkin_export_svc.c - Export checkin record to excel file.
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
#include <sys/prctl.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "clang.h"
#include "char_conv.h"
#include "time_conv.h"
#include "excel_file_write.h"
#include "checkin_export_svc.h"

#include "file_ops.h"
#include "hotplug.h"
#include "storage.h"

#define LOG_TAG "RECORD_EXPORT"
#include <log/log.h>

#define UDISK_DEVICE "/mnt/usb"

/* U disk will be full threshold */
#define UDISK_FULL_THRESHOLD (0.2)

/* The excel file name of the checkin record */
#define PUNCH_REC_EXPORT_EXCEL_NAME "/mnt/usb/RECG/record"

/* The excel file name of the checkin record export dir */
#define PUNCH_REC_EXPORT_DIR "/mnt/usb/RECG"

/* The excel file name of the checkin record export pic dir*/
#define PUNCH_REC_EXPORT_DIR_PIC "/mnt/usb/RECG/DCIM"

/**
 * The checkin record pictures, relative to
 * the path of the U disk, for user viewing */
#define PUNCH_REC_EXPORT_PIC_RELATIVE_U_DISK_DIR "/RECG/DCIM"

/* Dir of check-in pictures on the device */
#define PUNCH_REC_PIC_DEVICE_DIR "/data/face_recg"

#define ONE_QUERY_REC_COUNT (200)

static record_export_ctx_t export_ctx;

/* get checkin count */
static func_get_checkin_count s_get_checkin_count_func;

/* get checkin record information */
func_get_checkin_rec_info s_get_checkin_rec_info_func;


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

#if 0
/**
 * record_export_csv_head_write - Write checkin record head to CSV file.
 * @fp: a pointer to an csv file handle
 * @excel_row: Line number in the csv file
 *
 * Returns 0 if success, otherwise not 0.
 */
static int record_export_csv_head_write(FILE *fp, int excel_row)
{
	int ret = 0;
	int len;

	assert(fp != NULL);

	/* user id,Name,checkin time,Show export results */
	len = fprintf(fp, "\ufeff%s,\ufeff%s,\ufeff%s,\ufeff%s\r\n",
			clang_str("TEXT_EMPLOYEE_NUMBER"),
			clang_str("TEXT_NAME"),
			clang_str("TEXT_PUNCH_TIME"),
			clang_str("TEXT_EXPORT_RESULT"));

	if (len <= 0)
		ret = -1;

	return ret;
}
#else
/**
 * record_export_excel_head_write - Write checkin record head to Excel file.
 * @ws: a pointer to an excel file handle
 * @excel_row: Line number in the excel file
 *
 * Returns 0 if success, otherwise not 0.
 */
static int record_export_excel_head_write(lxw_worksheet *ws, int excel_row)
{
	int ret = 0;
	int index = 0;
	lxw_error error;

	assert(ws != NULL);

	/* user id */
	error = worksheet_write_string(ws, excel_row, index++,
			clang_str("TEXT_EMPLOYEE_NUMBER"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Name */
	error = worksheet_write_string(ws, excel_row, index++,
			clang_str("TEXT_NAME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* checkin time */
	error = worksheet_write_string(ws, excel_row, index++,
			clang_str("TEXT_PUNCH_TIME"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Show export results */
	error = worksheet_write_string(ws, excel_row, index++,
		clang_str("TEXT_EXPORT_RESULT"), NULL);
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
#endif

#if 0
/**
 * record_export_csv_row_write - Write a row of checkin record to csv file.
 * @fp: Point to an csv file handle
 * @excel_row: Line number in the excel file
 * @p_rec_info: checkin record information
 * @is_export_ok true: Export successfully
 *               false: Export failed
 *
 * Returns 0 if success, otherwise not 0.
 */
static int record_export_csv_row_write(FILE *fp,
				unsigned int excel_row,
				checkin_rec_info_t *p_rec_info,
				bool is_export_ok)
{
	int ret = 0;
	int len = 0;
	char out_str_time[64];
	long sec_punch_time = 0;
	char str_export_result[64];

	assert((fp != NULL) && (p_rec_info != NULL));

	/* checkin time, Note: Change double time to string. */
	memset(out_str_time, 0, sizeof(out_str_time));
	sec_punch_time = p_rec_info->checkin_time;
	time_second_to_str(sec_punch_time, out_str_time, sizeof(out_str_time));

	/* Write the export result to csv */
	memset(str_export_result, 0, sizeof(str_export_result));
	if (true == is_export_ok)
		snprintf(str_export_result, sizeof(str_export_result),
			clang_str("TEXT_REC_EXPORT_SUCCESS"));
	else
		snprintf(str_export_result, sizeof(str_export_result),
			clang_str("TEXT_REC_EXPORT_FAIL"));

	/* user id,Name,checkin time,export result */
	len = fprintf(fp, "\ufeff%s,\ufeff%s,\ufeff%s,\ufeff%s\r\n",
			p_rec_info->user_id,
			p_rec_info->name,
			out_str_time,
			str_export_result);

	if (len <= 0)
		ret = -1;

	return ret;
}
#else
/**
 * record_export_excel_row_write - Write a row of checkin record to Excel file.
 * @ws: Point to an excel file handle
 * @excel_row: Line number in the excel file
 * @p_rec_info: checkin record information
 * @is_export_ok true: Export successfully
 *               false: Export failed
 *
 * Returns 0 if success, otherwise not 0.
 */
static int record_export_excel_row_write(lxw_worksheet *ws,
				unsigned int excel_row,
				checkin_rec_info_t *p_rec_info,
				bool is_export_ok)
{
	int ret = 0;
	int index = 0;
	lxw_error error;
	char out_str_time[64];
	long sec_punch_time = 0;
	char str_export_result[64];

	assert((ws != NULL) && (p_rec_info != NULL));

	/* user id */
	error = worksheet_write_string(ws, excel_row, index++,
		p_rec_info->user_id, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Name */
	error = worksheet_write_string(ws, excel_row, index++, p_rec_info->name, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* checkin time, Note: Change double time to string. */
	memset(out_str_time, 0, sizeof(out_str_time));
	sec_punch_time = p_rec_info->checkin_time;
	time_second_to_str(sec_punch_time, out_str_time, sizeof(out_str_time));
	error = worksheet_write_string(ws, excel_row, index++, out_str_time, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	/* Write the export result to excel */
	memset(str_export_result, 0, sizeof(str_export_result));
	if (true == is_export_ok)
		snprintf(str_export_result, sizeof(str_export_result),
			clang_str("TEXT_REC_EXPORT_SUCCESS"));
	else
		snprintf(str_export_result, sizeof(str_export_result),
			clang_str("TEXT_REC_EXPORT_FAIL"));
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
#endif

/**
 * db_ui_person_info_query_end - free person information.
 * @head: A pointer of person info queue.
 *
 * Returns	0 success, -1: failure.
 */
int record_export_free_queue(ee_queue_t *head)
{
	if (NULL == head)
		return -1;

	ee_queue_t *h = head, *pos, *n;
	checkin_rec_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, checkin_rec_info_t, queue);
		if (p) {
			free(p);
			p = NULL;
		}
	}
	ee_queue_init(head);

	return 0;
}

/**
 * record_export_proc - The entry function for exporting punch records to Excel files
 * @arg: Thread function parameters
 *
 *
 * Returns 0 if success, otherwise not 0.
 */
#if 0
void *record_export_proc(void *arg)
{
	int row = 0;
	int ret = 0;
	int count = 0;
	FILE *fp = NULL;
	ee_queue_t head;
	int close_ret = 0;
	int fail_count = 0;
	int is_proc_exit = 0;
	int current_count = 0;
	int excel_write_ret = 0;
	checkin_rec_info_t *q = NULL;
	char str_time[64];
	bool is_export_ok = false;
	char write_excel_file_name[128];
	record_export_error_type_e proc_ret = RECORD_EXPORT_OK;

	/* Change the thread execution state to the running state */
	record_export_set_done_status(EE_THREAD_RUNNING);

	if (udisk_is_ok() != 0) {
		proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
		goto RUN_END;
	}

	if (get_disk_free_ratio(UDISK_DEVICE) < UDISK_FULL_THRESHOLD) {
		proc_ret = RECORD_EXPORT_UDISK_FULL_ERR;
		goto RUN_END;
	}

	/* Create a RECG folder in the root directory of the U Disk*/
	if (is_dir_exist(PUNCH_REC_EXPORT_DIR) != 0) {
		ret = mk_mul_dirs(PUNCH_REC_EXPORT_DIR);
		if (ret != 0) {
			proc_ret = RECORD_EXPORT_WRITE_ERR;
			goto RUN_END;
		}
	}

	/* Step1: Create an excel file that writes a punch record */
	memset(str_time, 0, sizeof(str_time));
	time_second_to_file_name_str(time(NULL),
		str_time, sizeof(str_time));

	memset(write_excel_file_name, 0, sizeof(write_excel_file_name));
	snprintf(write_excel_file_name, sizeof(write_excel_file_name),
		"%s_%s.csv",
		PUNCH_REC_EXPORT_EXCEL_NAME, str_time);

	fp = fopen(write_excel_file_name, "wt");
	if (fp == NULL) {
		proc_ret = RECORD_EXPORT_WRITE_ERR;
		goto RUN_END;
	}

	/* Step2: Write the title of csv */
	record_export_csv_head_write(fp, row++);

	/* Step3: Check the number of punch records matching the query conditions
	 * from the database
	 */
	if (s_get_checkin_count_func)
		count = s_get_checkin_count_func();

	ALOGD("%s[%d] count %d", __func__, __LINE__, count);
	record_export_set_sum_status(count);

	while ((0 == export_ctx.is_proc_exit) && (current_count < count)) {
		if (udisk_is_ok() != 0) {
			proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
			goto RUN_END;
		}
		ee_queue_init(&head);
		if (s_get_checkin_rec_info_func) {
			ret = s_get_checkin_rec_info_func(current_count,
					ONE_QUERY_REC_COUNT, &head);
		}

		ALOGD("%s[%d] current_count %d", __func__, __LINE__, current_count);
		ee_queue_foreach(q, &head, checkin_rec_info_t, queue) {
			/* Judge the import process to end early */
			ret = record_export_get_proc_exit_flag(&is_proc_exit);
			if ((0 == ret) && export_ctx.is_proc_exit) {
				proc_ret = RECORD_EXPORT_USER_EXIT_ERR;
				break;
			}

			/* Find the person information corresponding to the punch
			 * record
			 */
			is_export_ok = true;
			/*
			ALOGD("%s[%d] userid %s name %s time %d",
				__func__, __LINE__,
				q->user_id, q->name, q->checkin_time);
			*/
			/* Step5: Write the checkin record to the excel file */
			excel_write_ret = record_export_csv_row_write(fp,
					row++, q, is_export_ok);
			if (excel_write_ret != 0) {
				ALOGE("[%s:%d] Write excel file fail\n",
					__func__, __LINE__);
				break;
			}

			/* Step6: Update current export status */
			current_count++;
			record_export_set_current_status(current_count, fail_count);
		}
		record_export_free_queue(&head);
	}

	/* Step6: Close the csv */
	close_ret = fclose(fp);
	if ((close_ret != 0) || (excel_write_ret != 0)) {
		proc_ret = RECORD_EXPORT_WRITE_ERR;
		ALOGE("[%s:%d] Close excel file fail\n",
				__func__, __LINE__);
	}
	ALOGD("%s[%d] excel_file_write_close() ret %d write_ret = %d",
			__func__, __LINE__, close_ret, excel_write_ret);
RUN_END:
	/* Synchronize file system data into physical media */
	file_system_sync();

	/* Change the thread execution state to the end state */
	record_export_set_result_code(proc_ret);
	record_export_set_done_status(EE_THREAD_DONE);

	return NULL;
}
#else
void *record_export_proc(void *arg)
{
	int row = 0;
	int ret = 0;
	int count = 0;
	lxw_workbook *wb;
	lxw_worksheet *ws;
	ee_queue_t head;
	int close_ret = 0;
	int fail_count = 0;
	int is_proc_exit = 0;
	int current_count = 0;
	int excel_write_ret = 0;
	checkin_rec_info_t *q = NULL;
	char str_time[64];
	bool is_export_ok = false;
	char write_excel_file_name[128];
	record_export_error_type_e proc_ret = RECORD_EXPORT_OK;
	prctl(PR_SET_NAME, "export_thread");

	/* Change the thread execution state to the running state */
	record_export_set_done_status(EE_THREAD_RUNNING);

	if (udisk_is_ok() != 0) {
		proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
		goto RUN_END;
	}

	if (get_disk_free_ratio(UDISK_DEVICE) < UDISK_FULL_THRESHOLD) {
		proc_ret = RECORD_EXPORT_UDISK_FULL_ERR;
		goto RUN_END;
	}

	/* Create a RECG folder in the root directory of the U Disk*/
	if (is_dir_exist(PUNCH_REC_EXPORT_DIR) != 0) {
		ret = mk_mul_dirs(PUNCH_REC_EXPORT_DIR);
		if (ret != 0) {
			proc_ret = RECORD_EXPORT_WRITE_ERR;
			goto RUN_END;
		}
	}

	/* Step1: Create an excel file that writes a punch record */
	memset(str_time, 0, sizeof(str_time));
	time_second_to_file_name_str(time(NULL),
		str_time, sizeof(str_time));

	memset(write_excel_file_name, 0, sizeof(write_excel_file_name));
	snprintf(write_excel_file_name, sizeof(write_excel_file_name),
		"%s_%s.xlsx",
		PUNCH_REC_EXPORT_EXCEL_NAME, str_time);

	ret = excel_file_write_open(write_excel_file_name,
		clang_str("TEXT_PUNCH_REC_EXPORT_EXCEL_SHEET_TITEL"), &wb, &ws);
	if (ret != 0) {
		proc_ret = RECORD_EXPORT_WRITE_ERR;
		goto RUN_END;
	}

	/* Step2: Write the title of excel */
	record_export_excel_head_write(ws, row++);

	/* Step3: Check the number of punch records matching the query conditions
	 * from the database
	 */
	if (s_get_checkin_count_func)
		count = s_get_checkin_count_func();

	ALOGD("%s[%d] count %d", __func__, __LINE__, count);
	record_export_set_sum_status(count);

	while ((0 == export_ctx.is_proc_exit) && (current_count < count)) {
		if (udisk_is_insert() != 1) {
			proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
			goto RUN_END;
		}
		ee_queue_init(&head);
		if (s_get_checkin_rec_info_func) {
			ret = s_get_checkin_rec_info_func(current_count,
					ONE_QUERY_REC_COUNT, &head);
		}

		ee_queue_foreach(q, &head, checkin_rec_info_t, queue) {
			/* Judge the import process to end early */
			ret = record_export_get_proc_exit_flag(&is_proc_exit);
			if ((0 == ret) && export_ctx.is_proc_exit) {
				proc_ret = RECORD_EXPORT_USER_EXIT_ERR;
				break;
			}
			if (udisk_is_insert() != 1) {
				proc_ret = RECORD_EXPORT_NO_UDISK_ERR;
				break;
			}

			/* Find the person information corresponding to the punch
			 * record
			 */
			is_export_ok = true;
			/*
			ALOGD("%s[%d] userid %s name %s time %d",
				__func__, __LINE__,
				q->user_id, q->name, q->checkin_time);
			*/
			/* Step5: Write the checkin record to the excel file */
			excel_write_ret = record_export_excel_row_write(ws,
					row++, q, is_export_ok);
			if (excel_write_ret != 0) {
				ALOGE("[%s:%d] Write excel file fail\n",
					__func__, __LINE__);
				break;
			}

			/* Step6: Update current export status */
			current_count++;
			record_export_set_current_status(current_count, fail_count);
		}
		record_export_free_queue(&head);
	}

	/* Step6: Close the excel */
	close_ret = excel_file_write_close(wb, write_excel_file_name);
	if ((close_ret != 0) || (excel_write_ret != 0)) {
		proc_ret = RECORD_EXPORT_WRITE_ERR;
		ALOGE("[%s:%d] Close excel file fail\n",
				__func__, __LINE__);
	}
	ALOGD("%s[%d] excel_file_write_close() ret %d write_ret = %d",
			__func__, __LINE__, close_ret, excel_write_ret);
RUN_END:
	/* Synchronize file system data into physical media */
	file_system_sync();

	/* Change the thread execution state to the end state */
	record_export_set_result_code(proc_ret);
	record_export_set_done_status(EE_THREAD_DONE);

	return NULL;
}
#endif

/**
 * record_export_start_service - start record export service
 * @func_count: a function pointer of func_get_checkin_count
 * @func_rec_info: a function pointer of func_get_checkin_rec_info
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_export_start_service(
		func_get_checkin_count func_count,
		func_get_checkin_rec_info func_rec_info)
{
	s_get_checkin_count_func = func_count;
	s_get_checkin_rec_info_func = func_rec_info;

	record_export_init_ctx();

	pthread_create(&(export_ctx.tid), NULL, record_export_proc, NULL);

	return 0;
}

/**
 * record_export_stop_service - stop record export service
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_export_stop_service(void)
{
	export_ctx.is_proc_exit = 1;
	pthread_join(export_ctx.tid, NULL);

	return 0;
}


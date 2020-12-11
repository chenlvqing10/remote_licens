/*
 * person_import.c - Reading personnel registration information from Excel.
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
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <wchar.h>
#include <errno.h>
#include <memory.h>
#include <dirent.h>
#include <string.h>
#include <sys/prctl.h>
#include <limits.h>

#include "clang.h"
#include "hotplug.h"
#include "person.h"
#include "file_ops.h"
#include "char_conv.h"
#include "time_conv.h"
#include "person_import.h"
#include "excel_file_read.h"
#include "excel_file_write.h"
#include "excel_person_public.h"
#include "face.h"
#include "db_all.h"

#define LOG_TAG "IMPORT"
#include "log/log.h"

#define _EXCEL_PERSON_INFO_DEBUG_
#define _EXCEL_JPEG_DEBUG_

/**
 * Number of columns in each person registration information
 * when registering in bulk
 */
#define PERSON_IMPORT_INFO_COL 14

/* NOTE: Define the static variable g_excel_import_status,
 * modified by __thread, each thread will have a separate
 * copy of g_excel_import_status, they will not
 * interfere with each other
 */
static person_import_ctx_t import_ctx;

/**
 * Get the exit flag of imports
 *
 * return object of 'person_reg_param_t'
 */
static person_reg_param_t *excel_person_info_get_reg_param(void)
{
	return &import_ctx.reg_param;
}

/**
 * Set the exit flag of imports
 * @is_exit: flag of exit
 *
 * return 0
 */
int excel_person_info_set_proc_exit_flag(int is_exit)
{
	import_ctx.is_proc_exit = is_exit;

	return 0;
}

/** Get the exit flag of imports
 * @is_proc_exit: a pointer of flag of exit
 *
 * return 0
 */
static int excel_person_info_get_proc_exit_flag(int *is_proc_exit)
{
	*is_proc_exit = import_ctx.is_proc_exit;

	return 0;
}

/**
 * Set the number of imports
 * @sum_count: total count
 *
 * return 0
 */
static int excel_person_info_set_sum_status(int sum_count)
{
	import_ctx.status.sum_count = sum_count;

	return 0;
}

/* Set up the running state of the thread */
static int excel_person_info_set_current_status(int current_count, int fail_count)
{
	import_ctx.status.current_count = current_count;

	import_ctx.status.fail_count = fail_count;

	return 0;
}

/* Set thread completion status falg */
static int excel_person_info_set_done_status(thread_done_e is_done)
{
	import_ctx.done = is_done;

	return 0;
}

/* Set thread result code */
static int excel_person_info_set_result_code(person_import_error_type_e result_code)
{
	import_ctx.result_code = result_code;

	return 0;
}

int check_name_is_validate(const char *str) {
  int check_sub = 0;
  int len = 0;
  if (NULL == str)
    return -1;

  len = strlen(str);
  if (0 == len)
    return -1;
  else if (len > 30)
    return -2;
  if (strspn(str, "0123456789") == len) {
    return -3;
  }

  for (int i = 0; i < len; i++) {
    if (((*(str + i) >= 'A') && (*(str + i) <= 'Z')) ||  //'A' <= *(str+i) <= 'Z'
      ((*(str + i) >= 'a') && (*(str + i) <= 'z')) ||  //'a' <= *(str+i) <= 'z'
      ((*(str + i) >= '0') && (*(str + i) <= '9')) ||  //'0' <= *(str+i) <= '9'
      *(str + i) >= 0x80 ) {  //chinese
      //is_utf8_string
      if (check_sub == 0) {
        if ((str[i] >> 7) == 0) {  // 0xxx xxxx
          continue;
        } else if ((str[i] & 0xE0) == 0xC0) {  // 110x xxxx
          check_sub = 1;
        } else if ((str[i] & 0xF0) == 0xE0) {  // 1110 xxxx
          check_sub = 2;
        } else if ((str[i] & 0xF8) == 0xF0) {  // 1111 0xxx
          check_sub = 3;
        } else if ((str[i] & 0xFC) == 0xF8) {  // 1111 10xx
          check_sub = 4;
        } else if ((str[i] & 0xFE) == 0xFC) {  // 1111 110x
          check_sub = 5;
        } else {
          return -3;
        }
      } else {
        if ((str[i] & 0xC0) != 0x80) {
          return -3;
        }
        check_sub--;
      }
      continue;
    }
    return -3;
  }

  return 0;
}

int check_id_is_validate(const char *str) {
  int len = 0;
  if (NULL == str)
    return -1;

  len = strlen(str);
  if (0 == len)
    return -1;

  for (int i = 0; i < len; i++) {
    if ((*(str + i) < '0') || (*(str + i) > '9')) {
      return -2;
    }
  }

  if (atoi(str) >= INT_MAX)
    return -3;

  return 0;
}

/**
 * excel_person_info_row_collect - Check the validity of each column of data in each row
 * of excel.
 * @p_row A pointer to 'xlsRow'
 * @col_count Number of columns per row
 * @dir_name The directory where the file is located
 * @p_info A pointer to 'person_info_t'
 *
 * Returns PERSON_INFO_OK if success, otherwise not PERSON_INFO_OK.
 */
static e_person_reg_code excel_person_info_row_collect(xlsRow *p_row, WORD col_count,
		char *dir_name, person_t *p_info)
{
	int ret = 0;
	int is_exist = 0;
	ee_queue_t head;
	xlsCell *cell = NULL;
	char photo_path[256];
	e_person_reg_code code = PERSON_REG_OK;

	assert((p_row != NULL) && (dir_name != NULL) && (p_info != NULL));

	/* Check if the number of columns in Excel is correct */
	if (PERSON_IMPORT_INFO_COL != col_count) {
		ALOGE("Person info data error col_count=%d ok_count=%d\n",
			col_count, PERSON_IMPORT_INFO_COL);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

#ifdef _EXCEL_PERSON_INFO_DEBUG_
	for (int i = 0; i < col_count; i++) {
		cell = &p_row->cells.cell[i];
		xls_showCell(cell);
	}
#endif
	/** Check if the value of each column is valid */

	/* Employee number(E.g SW200)
	 *
	 * Type: char[32]
	 *
	 */
	cell = &p_row->cells.cell[0];
	char employee_number[32] = {0};
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL,
				employee_number,sizeof(employee_number));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_number[%s]",
			__func__, __LINE__, employee_number);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	} else if (check_id_is_validate(employee_number)) {
		ALOGE("%s:%d >> invalid employee_number[%s]",
			__func__, __LINE__, employee_number);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}
	memcpy(p_info->employee_number, employee_number, sizeof(employee_number));

	/* Name: char name[32]
	 *
	 * Type: char[32]
	 *
	 * Not NULL
	 */
	cell = &p_row->cells.cell[1];
	if (check_name_is_validate(cell->str)) {
		ALOGE("%s:%d >> invalid employee_name[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}
	else
		ret = excel_check_str_cell_valid(cell, IS_NOT_NULL, p_info->name,
			sizeof(p_info->name));
		if (ret != 0) {
			ALOGE("%s:%d >> invalid employee_name[%s]",
				__func__, __LINE__, cell->str);
			code = PERSON_INFO_DATA_ERROR;
			goto DATA_ERROR;
		}
	/*
	ret = app_input_str_is_valid(p_info->name);
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}
	*/
	/* Find the database and determine if there is a person with the same
	 * name. if yes, reject to register, prompting an error
	 */
	is_exist = db_ui_local_person_query_search_begin(
		&head, p_info->employee_number, NULL, 0, -1);
	db_ui_local_person_query_end(&head);
	if (is_exist > 0) {
		ALOGE("%s:%d >> employee[%s:%s] had been registered",
			__func__, __LINE__, p_info->employee_number, p_info->name);
		code = PERSON_REGISTRATION_ERROR;
		goto DATA_ERROR;
	}

	/* Gender(E.g "Man/Women/..."
	 *
	 * Type: char[8]
	 *
	 */
	cell = &p_row->cells.cell[2];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->gender,
		sizeof(p_info->gender));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_gender[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Nationa
	 *
	 * Type: char[32]
	 *
	 */
	cell = &p_row->cells.cell[3];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->nationa,
		sizeof(p_info->nationa));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_nationa[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* ID card number
	 *
	 * Type: char[32]
	 *
	 */
	cell = &p_row->cells.cell[4];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->id_card_number,
		sizeof(p_info->id_card_number));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_card[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Telephone number
	 *
	 * Type: char[20]
	 *
	 */
	cell = &p_row->cells.cell[5];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->mobile,
		sizeof(p_info->mobile));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_mobile[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Department name(E.g "Software Lab001")
	 *
	 * Type: char[128]
	 *
	 */
	cell = &p_row->cells.cell[6];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->department_name,
		sizeof(p_info->department_name));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_department[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 *
	 * Type: int
	 *
	 * Not NULL
	 */
	cell = &p_row->cells.cell[7];
	ret = excel_check_int_cell_valid(cell, IS_ALLOW_NULL, &p_info->access_right);
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_access[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* When the permission is temporary access permission mode 1,
	 * Allowed start time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 *
	 * Type: double
	 *
	 */
	cell = &p_row->cells.cell[8];
	ret = excel_check_time_cell_valid(cell, IS_ALLOW_NULL,
		&p_info->temporary_access_start_time);
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_start_time[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* When the permission is temporary access permission mode 1,
	 * Allowed end time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 *
	 * Type: double
	 *
	 */
	cell = &p_row->cells.cell[9];
	ret = excel_check_time_cell_valid(cell, IS_ALLOW_NULL,
		&p_info->temporary_access_end_time);
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_end_time[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Allowed number of passes
	 * When the permission is temporary access permission mode 2
	 *
	 * Type: int
	 *
	 */
	cell = &p_row->cells.cell[10];
	ret = excel_check_int_cell_valid(cell, IS_ALLOW_NULL,
		&p_info->temporary_access_times);
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_access_times[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Access card number.
	 * Effective when the punch mode is "Card reader"
	 *
	 * Type: char[32]
	 *
	 */
	cell = &p_row->cells.cell[11];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->access_card_number,
		sizeof(p_info->access_card_number));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_access_card[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Remarks: Used to fill in the description information
	 *
	 * Type: char[128]
	 *
	 */
	cell = &p_row->cells.cell[12];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, p_info->remarks,
		sizeof(p_info->remarks));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_remarks[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Register photo name.(Including jpg suffix)
	 *
	 * Type: char[64]
	 *
	 * Not NULL
	 */
	cell = &p_row->cells.cell[13];
	ret = excel_check_str_cell_valid(cell, IS_NOT_NULL, p_info->regist_photo_name,
		sizeof(p_info->regist_photo_name));
	if (ret != 0) {
		ALOGE("%s:%d >> invalid employee_photo[%s]",
			__func__, __LINE__, cell->str);
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Check if the image exists */
	memset(photo_path, 0, sizeof(photo_path));
	snprintf(photo_path, sizeof(photo_path), "%s/%s/%s", dir_name,
		PERSON_IMPORT_PIC_FOLDER_NAME, p_info->regist_photo_name);
	if (access(photo_path, F_OK) != 0) {
		ALOGE("%s:%d >> invalid employee_path[%s]",
			__func__, __LINE__, photo_path);
		code = PERSON_INFO_PIC_NO_HAVE;
		goto DATA_ERROR;
	}

DATA_ERROR:
	ALOGD("%s:%d >> return %d", __func__, __LINE__, code);
	return code;
}

/**
 * excel_person_info_row_proc - Processing personnel information per row.
 * @p_person_info A point to 'person_t'
 * @p_out_code Out, Registration failure type
 * @dir_name The directory where the file is located
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_person_info_row_proc(person_t *p_person_info,
		e_person_reg_code *p_out_code, char *p_out_detail_info,
				int detail_info_size, char *dir_name)
{
	int ret = 0;
	img_info_t reg_img;
	e_person_reg_code code = PERSON_REG_OK;

	assert((p_person_info != NULL) && (p_out_code != NULL) && (dir_name != NULL));

	/** Step 1: Call the personnel registration interface to register. */
	memset(&reg_img, 0, sizeof(img_info_t));
	snprintf(reg_img.pic_path, sizeof(reg_img.pic_path), "%s/%s/%s", dir_name,
		PERSON_IMPORT_PIC_FOLDER_NAME, p_person_info->regist_photo_name);
	ret = person_reg_proc(p_person_info, &reg_img, p_out_detail_info,
			detail_info_size, excel_person_info_get_reg_param(),
			&code, 1);
	if (ret != 0) {
		ret = -1;
		goto ROW_PROC_ERROR;
	}

	ret = 0;

ROW_PROC_ERROR:
	*p_out_code = code;
	return ret;
}

/**
 * excel_person_info_head_write - Write person info head to Excel file.
 * @p_write_ws A pointer to 'worksheet'.
 * @p_row A pointer to 'xlsRow'
 * @col_count Number of columns per row
 *
 * When there is a problem with one person's registration information, in order
 * to prompt the user, write this information to another excel.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_person_info_head_write(lxw_worksheet *p_write_ws, uint32_t row_number,
		xlsRow *p_row, WORD col_count)
{
	int i = 0;
	int ret = 0;
	char *cell_str = NULL;
	xlsCell *cell = NULL;
	WORD fail_reason_col = 0;
	lxw_error error;

	assert((p_write_ws != NULL) && (p_row != NULL));

	for (i = 0; i < col_count; i++) {
		cell = &p_row->cells.cell[i];
		cell_str = cell->str;

		if (XLS_BLANK_CELL == cell->id) {
			/* do something with a blank cell */
		} else if (cell_str != NULL) {
			/* cell->str contains a string value */
			if (strlen(cell_str) > 0) {
				error = worksheet_write_string(p_write_ws, row_number, i,
						cell_str, NULL);
				if (error != LXW_NO_ERROR)
					goto EXCEL_WRITE_ERROR;
			}
		}
	}

	/* On the last line, display the title of the cause of the error */
	fail_reason_col = i;
	error = worksheet_write_string(p_write_ws, row_number, fail_reason_col,
		clang_str("TEXT_FAIL_REASON_TITLE"), NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	fail_reason_col = i + 1;
	error = worksheet_write_string(p_write_ws, row_number, fail_reason_col,
		clang_str("TEXT_DETAIL_INFO_TITLE"), NULL);
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
 * excel_person_info_row_write - Write a row of person info to Excel
 * @p_write_ws A pointer to 'worksheet'.
 * @p_row A pointer to 'xlsRow'
 * @col_count Number of columns per row
 * @code Failed error code when importing personnel registration information
 *
 * When there is a problem with one person's registration information, in order
 * to prompt the user, write this information to another excel.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_person_info_row_write(lxw_worksheet *p_write_ws, uint32_t row_number,
		xlsRow *p_row, WORD col_count, char *detail_info, e_person_reg_code code)
{
	int i = 0;
	int ret = 0;
	xlsCell *cell = NULL;
	WORD colume = 0;
	lxw_error error;
	char fail_reason_str[64] = {0};
	char cell_str[EXCEL_CELL_MAX_STR_LEN] = {0};

	assert((p_write_ws != NULL) && (p_row != NULL));

	for (i = 0; i < col_count; i++) {
		cell = &p_row->cells.cell[i];

		memset(cell_str, 0, sizeof(cell_str));
		ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, cell_str,
			sizeof(cell_str));
		if ((0 == ret) && (strlen(cell_str) > 0)) {
			error = worksheet_write_string(p_write_ws, row_number, i,
				cell_str, NULL);
			if (error != LXW_NO_ERROR)
				goto EXCEL_WRITE_ERROR;
		}
	}

	/* Display failure information */
	colume = col_count;
	memset(fail_reason_str, 0, sizeof(fail_reason_str));
	person_reg_fail_reason(code, fail_reason_str, sizeof(fail_reason_str));
	error = worksheet_write_string(p_write_ws, row_number, colume,
		fail_reason_str, NULL);
	if (error != LXW_NO_ERROR)
		goto EXCEL_WRITE_ERROR;

	colume = col_count + 1;
	error = worksheet_write_string(p_write_ws, row_number, colume,
		detail_info, NULL);
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
 * excel_person_info_file_sheet_proc - Parse each sheet of the Excel file.
 * @wb A pointer to 'xlsWorkBook'
 * @dir_name The directory where the file is located
 * @proc_ret A pointer to 'person_import_error_type_e'
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_person_info_file_sheet_proc(xlsWorkSheet *ws, char *dir_name,
	person_import_error_type_e *proc_ret)
{
	int ret = 0;
	int close_ret = -1;
	person_t info;
	int is_proc_exit = 0;
	WORD col_count = 0;
	WORD row_count = 0;
	xlsRow *row = NULL;
	lxw_workbook *p_write_wb;
	lxw_worksheet *p_write_ws;
	int reg_fail_count = 0;
	xls_error_t ret_code = LIBXLS_OK;
	e_person_reg_code code = PERSON_REG_OK;
	char write_excel_file_name[128];
	char str_time[64], detail_info[64];

	assert((ws != NULL) && (dir_name != NULL));

	/* Parse each sheet */
	ret_code = xls_parseWorkSheet(ws);
	if (ret_code != LIBXLS_OK) {
		ALOGE("Error parsing worksheet: %s\n",
			xls_getError(ret_code));
		*proc_ret = PERSON_IMPORT_READ_ERR;
		return -1;
	}

	/* Create an Excel file and write the information when there is a registration
	 * failure
	 */
	memset(str_time, 0, sizeof(str_time));
	time_second_to_file_name_str(time(NULL), str_time, sizeof(str_time));

	memset(write_excel_file_name, 0, sizeof(write_excel_file_name));
	snprintf(write_excel_file_name, sizeof(write_excel_file_name), "%s/%s_%s.xlsx",
		dir_name, EXCEL_REG_FAIL_FILE_NAME, str_time);
	excel_file_write_open(write_excel_file_name,
		clang_str("TEXT_EXCEL_REG_FAIL_SHEET_NAME"),
		&p_write_wb, &p_write_ws);

	/* Processing each row
	 * Note: The first action Title needs to be skipped.
	 */
	row_count = ws->rows.lastrow;
	excel_person_info_set_sum_status(row_count);
	for (int j = 0; j <= row_count; j++) {
		/* Judge the import process to end early */
		ret = excel_person_info_get_proc_exit_flag(&is_proc_exit);
		if ((0 == ret) && (1 == is_proc_exit)) {
			ret = -1;
			*proc_ret = PERSON_IMPORT_USER_EXIT_ERR;
			break;
		}

		if (udisk_is_ok() != 0) {
			ALOGD("%s[%d] udisk_is_ok(%d) is not ok.",
				__func__, __LINE__, j);
			ret = -1;
			*proc_ret = PERSON_IMPORT_USER_EXIT_ERR;
			break;
		}

		row = xls_row(ws, j);

		/* Processing only the first PERSON_IMPORT_INFO_COL columns to avoid
		 * reading redundant columns
		 *
		 * NOTE: If you want to read all columns, use:
		 *	col_count = ws->rows.lastcol;
		 */
		col_count = PERSON_IMPORT_INFO_COL;

		if (0 == j) {
			ret = excel_person_info_head_write(p_write_ws, 0, row, col_count);
			if (ret != 0) {
				*proc_ret = PERSON_IMPORT_WRITE_ERR;
				break;
			}
		} else {
			memset(&info, 0, sizeof(person_t));
			code = excel_person_info_row_collect(row, col_count,
				dir_name, &info);

			memset(detail_info, 0, sizeof(detail_info));

			/* If the personnel information is valid, execute the
			 * registration process
			 */
			if (PERSON_REG_OK == code) {
				ret = excel_person_info_row_proc(&info, &code,
				detail_info, sizeof(detail_info), dir_name);
				if ((ret != 0) && (udisk_is_ok() != 0)) {
					*proc_ret = PERSON_IMPORT_NO_UDISK_ERR;
					break;
				}
			}

			/* When the personnel registration information fails to be
			 * imported, the failed information is written to the
			 * file.
			 */
			if (code != PERSON_REG_OK) {
				reg_fail_count++;
				ret = excel_person_info_row_write(p_write_ws,
						reg_fail_count, row, col_count,
						detail_info, code);
				if (ret != 0) {
					*proc_ret = PERSON_IMPORT_WRITE_ERR;
					break;
				}
			}
		}

		#ifdef _EXCEL_PERSON_INFO_DEBUG_
		ALOGD("\n");
		#endif

		/* Set up the running state of the thread */
		excel_person_info_set_current_status(j, reg_fail_count);
	}

	/* Close the Excel file that records the failure information */
	close_ret = excel_file_write_close(p_write_wb, write_excel_file_name);
	if (close_ret != 0) {
		ret = -1;
		*proc_ret = PERSON_IMPORT_WRITE_ERR;
		ALOGE("[%s:%d] Write excel file fail\n", __func__, __LINE__);
	}

	return ret;
}

/* get status */
int person_import_get_ctx(person_import_ctx_t *p_ctx)
{
	assert(p_ctx != NULL);

	memcpy(p_ctx, &import_ctx, sizeof(person_import_ctx_t));

	return 0;
}

/* Init status */
int person_import_init_ctx(void)
{
	face_recognition_param_t recg_param;

	memset(&recg_param, 0, sizeof(face_recognition_param_t));
	face_get_recg_param(&recg_param);

	memset(&import_ctx, 0, sizeof(person_import_ctx_t));
	import_ctx.done = EE_THREAD_NOT_RUN;
#if 0
	import_ctx.reg_param.check_face_size = 0;
	import_ctx.reg_param.face_max = 512;
	import_ctx.reg_param.face_min = 128;
	import_ctx.reg_param.quality = 0;
	import_ctx.reg_param.threshold = 0.5;
	import_ctx.reg_param.refresh_db_each_time = 0;
	import_ctx.reg_param.jpeg_width_stride = 1280;
	import_ctx.reg_param.jpeg_height_stride = 720;
	import_ctx.reg_param.batch_reg = 1;
#else
	import_ctx.reg_param.threshold = recg_param.identify_threshold_score;
	import_ctx.reg_param.refresh_db_each_time = 0;
	import_ctx.reg_param.batch_reg = 1;
#endif

	return 0;
}

/**
 * person_import_proc - Entry function for bulk registration by reading excel file.
 * @p_reg_excel_path: Excel file for batch import of personnel information.
 *
 * Returns 0 if success, otherwise not 0.
 */
void *person_import_proc(void *arg)
{
	int i = 0;
	int ret = -1;
	int sheet_count = 0;
	xlsWorkBook *wb = NULL;
	xlsWorkSheet *ws = NULL;
	char *dname;
	char dirc[128] = {0};
	char *p_reg_excel_path = NULL;
	person_import_error_type_e proc_ret = PERSON_IMPORT_OK;
	prctl(PR_SET_NAME, "person_import");

	ALOGD("[%s:%d] start thread id =%lx\n", __func__, __LINE__, pthread_self());
	prctl(PR_SET_NAME, "person-import");

	assert(arg != NULL);

	/* Change the thread execution state to the running state */
	excel_person_info_set_done_status(EE_THREAD_RUNNING);

	p_reg_excel_path = (char *)arg;
	ALOGD("p_reg_excel_path=%s\n", p_reg_excel_path);

	/* Get the path where the Excel file is located (excluding the file nama) */
	snprintf(dirc, sizeof(dirc), "%s", p_reg_excel_path);
	dname = dirname(dirc);

	if (udisk_is_ok() != 0) {
		proc_ret = PERSON_IMPORT_NO_UDISK_ERR;
		goto IMPORT_END;
	}

	/* Open the Excel file */
	ret = excel_file_read_open(p_reg_excel_path, EXCEL_CHAR_SET, &wb);
	if (ret != 0) {
		ALOGE("[%s:%d] Can't open %s\n", __func__, __LINE__, p_reg_excel_path);
		proc_ret = PERSON_IMPORT_READ_ERR;
		goto IMPORT_END;
	}

	/* sheets: Get the worksheet and then parse */
	sheet_count = (int)wb->sheets.count;
	#ifdef _EXCEL_PERSON_INFO_DEBUG_
	ALOGD("Count of sheets: %d\n", sheet_count);
	#endif
	/* When excel is greater than 1 sheet, only handler first sheet */
	if (sheet_count > 1)
		sheet_count = 1;

	for (i = 0; i < sheet_count; i++) {
		#ifdef _EXCEL_PERSON_INFO_DEBUG_
		ALOGD("Sheet %i (%s) pos %i\n", i , wb->sheets.sheet[i].name,
			wb->sheets.sheet[i].filepos);
		#endif

		ws = xls_getWorkSheet(wb, i);
		ret = excel_person_info_file_sheet_proc(ws, dname, &proc_ret);
		if (ret != 0)
			ALOGE("Proc person import error: %d\n", proc_ret);

		xls_close_WS(ws);
	}

	/* Close the Excel file */
	excel_file_read_close(wb);

	/* Updating face list after successful registration */
	face_db_update();

IMPORT_END:
	/* Synchronize file system data into physical media */
	file_system_sync();

	/* Change the thread execution state to the end state */
	excel_person_info_set_result_code(proc_ret);
	excel_person_info_set_done_status(EE_THREAD_DONE);
	ALOGD("[%s:%d] end thread id =%lx\n", __func__, __LINE__, pthread_self());

	return NULL;
}

int person_import_thread()
{
	pthread_t tid;
#if 0
	if (import_ctx.done == EE_THREAD_NOT_RUN || import_ctx.done == EE_THREAD_DONE) {
		person_import_init_ctx();
		if (!pthread_create(&tid, NULL, person_import_proc,
				PERSON_IMPORT_EXCEL_FILE_PATH)) {
			import_ctx.done = EE_THREAD_RUNNING;
			pthread_detach(tid);
		}
	}
#endif

	person_import_init_ctx();
	pthread_create(&tid, NULL, person_import_proc,
				PERSON_IMPORT_EXCEL_FILE_PATH);
	pthread_detach(tid);
	return 0;
}


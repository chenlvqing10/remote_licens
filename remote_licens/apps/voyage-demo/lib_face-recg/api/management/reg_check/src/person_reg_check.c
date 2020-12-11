/*
 * person_reg_check.c - Test personnel registration.
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
#include <wchar.h>
#include <errno.h>
#include <memory.h>
#include <dirent.h>

#include "elang.h"
#include "file_ops.h"
#include "person.h"
#include "char_conv.h"
#include "time_conv.h"
#include "sqlite3.h"
#include "db_person_info.h"
#include "excel_file_read.h"
#include "excel_file_write.h"
#include "excel_person_public.h"
#include "person_reg_check.h"
#include "config.h"

#define LOG_TAG "REG_CHECK"
#include "log/log.h"

#define _EXCEL_REG_CHECK_DEBUG_

/* A string of expected results */
#define REG_EXPECTED_RESULT_PASS	"PASS"
#define REG_EXPECTED_RESULT_NOT_PASS	"REFUSE"

/** Number of columns in each person registration information when registering in bulk*/
#define REG_CHECK_INFO_COL		15

static person_reg_check_ctx_t check_ctx;

/* get recg threshold */
static person_recg_param_t *excel_reg_check_get_recg_param(void)
{
	return &check_ctx.recg_param;
}

/* set sum status */
static int excel_reg_check_set_sum_status(int sum_count)
{
	check_ctx.status.sum_count = sum_count;

	return 0;
}

/* set sum status */
static int excel_reg_check_set_current_status(int current_count, int fail_count)
{
	check_ctx.status.current_count = current_count;

	check_ctx.status.fail_count = fail_count;

	return 0;
}

/* Set thread completion status falg */
static int excel_reg_check_set_done_status(thread_done_e is_done)
{
	check_ctx.done = is_done;

	return 0;
}

/**
 * excel_reg_check_head_write - Write person reg result head to Excel file.
 * @p_write_ws A pointer to 'worksheet'.
 * @p_row A pointer to 'xlsRow'
 * @col_count Number of columns per row
 *
 * When there is a problem with one person's registration information, in order
 * to prompt the user, write this information to another excel.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_reg_check_head_write(lxw_worksheet *p_write_ws, uint32_t row_number,
		xlsRow *p_row, WORD col_count)
{
	char *cell_str = NULL;
	xlsCell *cell = NULL;
	WORD fail_reason_col = 0;

	assert((p_write_ws != NULL) && (p_row != NULL));

	for (int i = 0; i < col_count; i++) {
		cell = &p_row->cells.cell[i];
		cell_str = cell->str;

		if (XLS_BLANK_CELL == cell->id) {
			/* do something with a blank cell */
		} else if (cell_str != NULL) {
			/* cell->str contains a string value */
			if (strlen(cell_str) > 0) {
				worksheet_write_string(p_write_ws, row_number, i,
						cell_str, NULL);
			}
		}
	}

	/* Show failure reason title */
	fail_reason_col = col_count;
	worksheet_write_string(p_write_ws, row_number, fail_reason_col,
		elang_str("TEXT_REG_CHECK_RESULT_TITLE"), NULL);

	/* Show detail info title */
	fail_reason_col = col_count + 1;
	worksheet_write_string(p_write_ws, row_number, fail_reason_col,
		elang_str("TEXT_REG_CHECK_DETAIL_TITLE"), NULL);

	/* Show comparison results title */
	fail_reason_col = col_count + 2;
	worksheet_write_string(p_write_ws, row_number, fail_reason_col,
		elang_str("TEXT_REG_CHECK_COMPARE_RESULT_TITLE"), NULL);

	return 0;
}

/**
 * excel_reg_check_row_collect - Check the validity of each column of data in each row
 * of excel.
 * @p_row A pointer to 'xlsRow'
 * @col_count Number of columns per row
 * @dir_name The directory where the file is located
 * @p_info A pointer to 'person_info_t'
 *
 * Returns PERSON_INFO_OK if success, otherwise not PERSON_INFO_OK.
 */
static e_person_reg_code excel_reg_check_row_collect(xlsRow *p_row, WORD col_count,
		char *dir_name, reg_check_info_t *p_info)
{
	int ret = 0;
	xlsCell *cell = NULL;
	char photo_path[256];
	person_t *person_info;
	e_person_reg_code code = PERSON_REG_OK;

	assert((p_row != NULL) && (p_info != NULL));

	person_info = &(p_info->person_reg_info);

	/* Check if the number of columns in Excel is correct */
	if (REG_CHECK_INFO_COL != col_count) {
		ALOGE("Person info data error\n");
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

#ifdef _EXCEL_REG_CHECK_DEBUG_
	for (int i = 0; i < col_count; i++) {
		cell = &p_row->cells.cell[i];
		xls_showCell(cell);
	}
#endif
	/* Check if the value of each column is valid */

	/* Employee mumber(E.g SW200)
	 *
	 * Type: char[32]
	 *
	 * Not NULL
	 */
	cell = &p_row->cells.cell[0];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL,
		person_info->employee_number, sizeof(person_info->employee_number));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Name: char name[32]
	 *
	 * Type: char[32]
	 *
	 * Not NULL
	 */
	cell = &p_row->cells.cell[1];
	ret = excel_check_str_cell_valid(cell, IS_NOT_NULL, person_info->name,
		sizeof(person_info->name));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Gender(E.g "Man/Women/..."
	 *
	 * Type: char[8]
	 *
	 * Not NULL
	 */
	cell = &p_row->cells.cell[2];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, person_info->gender,
		sizeof(person_info->gender));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Nationa
	 *
	 * Type: char[32]
	 *
	 */
	cell = &p_row->cells.cell[3];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, person_info->nationa,
		sizeof(person_info->nationa));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* ID card number
	 *
	 * Type: char[32]
	 *
	 */
	cell = &p_row->cells.cell[4];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, person_info->id_card_number,
		sizeof(person_info->id_card_number));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Telephone number
	 *
	 * Type: char[20]
	 *
	 */
	cell = &p_row->cells.cell[5];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, person_info->mobile,
		sizeof(person_info->mobile));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Department name(E.g "Software Lab001")
	 *
	 * Type: char[128]
	 *
	 */
	cell = &p_row->cells.cell[6];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL,
		person_info->department_name, sizeof(person_info->department_name));
	if (ret != 0) {
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
	ret = excel_check_int_cell_valid(cell, IS_ALLOW_NULL, &person_info->access_right);
	if (ret != 0) {
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
		&person_info->temporary_access_start_time);
	if (ret != 0) {
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
		&person_info->temporary_access_end_time);
	if (ret != 0) {
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
		&person_info->temporary_access_times);
	if (ret != 0) {
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
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL,
		person_info->access_card_number,
		sizeof(person_info->access_card_number));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Remarks: Used to fill in the description information
	 *
	 * Type: char[128]
	 *
	 */
	cell = &p_row->cells.cell[12];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, person_info->remarks,
		sizeof(person_info->remarks));
	if (ret != 0) {
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
	ret = excel_check_str_cell_valid(cell, IS_NOT_NULL,
		person_info->regist_photo_name, sizeof(person_info->regist_photo_name));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

	/* Check if the image exists */
	memset(photo_path, 0, sizeof(photo_path));
	snprintf(photo_path, sizeof(photo_path), "%s/%s/%s", dir_name,
		PERSON_REG_CHECK_PIC_FOLDER_NAME, person_info->regist_photo_name);
	if (access(photo_path, F_OK) != 0) {
		code = PERSON_INFO_PIC_NO_HAVE;
		goto DATA_ERROR;
	}

	cell = &p_row->cells.cell[14];
	ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL,
		p_info->reg_expected_result,
		sizeof(p_info->reg_expected_result));
	if (ret != 0) {
		code = PERSON_INFO_DATA_ERROR;
		goto DATA_ERROR;
	}

DATA_ERROR:
	return code;
}

/**
 * excel_reg_check_row_proc - Processing personnel information per row.
 * @p_person_info A point to 'person_t'
 * @p_out_code Out, Registration failure type
 * @dir_name The directory where the file is located
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_reg_check_row_proc(reg_check_info_t *p_reg_check_info,
		e_person_reg_code *p_out_code, char *detail_info, int detail_info_size,
		char *dir_name)
{
	int ret = 0;
	img_info_t reg_img;
	person_t *person_info;
	e_person_reg_code code = PERSON_REG_OK;

	assert((p_reg_check_info != NULL) && (p_out_code != NULL) && (dir_name != NULL));

	person_info = &(p_reg_check_info->person_reg_info);

	/** Step 1: Call the personnel registration interface to register. */
	memset(&reg_img, 0, sizeof(img_info_t));
	snprintf(reg_img.pic_path, sizeof(reg_img.pic_path), "%s/%s/%s", dir_name,
		PERSON_REG_CHECK_PIC_FOLDER_NAME, person_info->regist_photo_name);
	ret = person_recognition_proc(person_info, &reg_img, &code, detail_info,
				detail_info_size, excel_reg_check_get_recg_param());
	if (ret != 0) {
		ret = -1;
		goto ROW_PROC_ERROR;
	}

ROW_PROC_ERROR:
	*p_out_code = code;
	return ret;
}

/**
 * excel_reg_check_row_write - Write a row of person info to Excel
 * @p_write_ws A pointer to 'worksheet'.
 * @row_number Row number
 * @p_row  pointer to 'xlsRow'
 * @col_count Number of columns per row
 * @result_code Failed error code when importing personnel registration information
 * @compare_code Return code after comparing actual test results with preset results
 *
 * When there is a problem with one person's registration information, in order
 * to prompt the user, write this information to another excel.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_reg_check_row_write(lxw_worksheet *p_write_ws, uint32_t row_number,
		xlsRow *p_row, WORD col_count, e_person_reg_code result_code,
		char *detail_info, e_person_reg_code compare_code)
{
	int ret = -1;
	xlsCell *cell = NULL;
	WORD test_reason_col = 0;
	WORD compare_reason_col = 0;
	char test_reason_str[64] = {0};
	char compare_reason_str[64] = {0};
	char cell_str[EXCEL_CELL_MAX_STR_LEN] = {0};

	assert((p_write_ws != NULL) && (p_row != NULL));

	for (int i = 0; i < col_count; i++) {
		cell = &p_row->cells.cell[i];

		memset(cell_str, 0, sizeof(cell_str));
		ret = excel_check_str_cell_valid(cell, IS_ALLOW_NULL, cell_str,
			sizeof(cell_str));
		if ((0 == ret) && (strlen(cell_str) > 0))
			worksheet_write_string(p_write_ws, row_number, i, cell_str,
				NULL);
	}

	/* Display actual test result string */
	test_reason_col = col_count;
	memset(test_reason_str, 0, sizeof(test_reason_str));
	person_reg_fail_reason(result_code, test_reason_str,
		sizeof(test_reason_str));
	worksheet_write_string(p_write_ws, row_number, test_reason_col,
		test_reason_str, NULL);

	/* Display detail string */
	test_reason_col = col_count + 1;
	worksheet_write_string(p_write_ws, row_number, test_reason_col,
		detail_info, NULL);

	/* Display comparison result string */
	compare_reason_col = col_count + 2;
	memset(compare_reason_str, 0, sizeof(compare_reason_str));
	person_reg_fail_reason(compare_code, compare_reason_str,
		sizeof(compare_reason_str));
	worksheet_write_string(p_write_ws, row_number, compare_reason_col,
		compare_reason_str, NULL);

	return 0;
}

/**
 * excel_reg_check_file_sheet_proc - Parse each sheet of the Excel file.
 * @wb A pointer to 'xlsWorkBook'
 * @dir_name The directory where the file is located
 *
 * Returns 0 if success, otherwise not 0.
 */
static int excel_reg_check_file_sheet_proc(xlsWorkSheet *ws, char *dir_name)
{
	WORD col_count = 0;
	WORD row_count = 0;
	xlsRow *row = NULL;
	lxw_workbook *p_write_wb;
	lxw_worksheet *p_write_ws;
	int reg_fail_count = 0;
	reg_check_info_t info;
	xls_error_t ret_code = LIBXLS_OK;
	e_person_reg_code code = PERSON_REG_OK;
	char detail_info[64];
	char write_excel_file_name[128];
	char str_time[64];
	e_person_reg_code compare_code = PERSON_REG_COMPARE_RESULT_FAIL;

	assert((ws != NULL) && (dir_name != NULL));

	/* Parse each sheet */
	ret_code = xls_parseWorkSheet(ws);
	if (ret_code != LIBXLS_OK) {
		ALOGE("Error parsing worksheet: %s\n",
			xls_getError(ret_code));
		return -1;
	}

	/* Create an Excel file and write the information when there is a registration
	 * failure
	 */
	memset(str_time, 0, sizeof(str_time));
	time_second_to_file_name_str(time(NULL), str_time, sizeof(str_time));

	memset(write_excel_file_name, 0, sizeof(write_excel_file_name));
	snprintf(write_excel_file_name, sizeof(write_excel_file_name), "%s/%s_%s.xlsx",
		dir_name, EXCEL_REG_CHECK_RESULT_FILE_PREFIX, str_time);
	excel_file_write_open(write_excel_file_name,
		elang_str("TEXT_EXCEL_REG_CHECK_RESULT_SHEET_NAME"),
		&p_write_wb, &p_write_ws);

	/* Processing each row
	 * Note: The first action Title needs to be skipped.
	 */
	row_count = ws->rows.lastrow;
	excel_reg_check_set_sum_status(row_count);
	for (int j = 0; j <= row_count; j++) {

		row = xls_row(ws, j);

		/* Processing only the first PERSON_IMPORT_INFO_COL columns to avoid
		 * reading redundant columns
		 *
		 * NOTE: If you want to read all columns, use:
		 *	col_count = ws->rows.lastcol;
		 */
		col_count = REG_CHECK_INFO_COL;

		if (0 == j)
			excel_reg_check_head_write(p_write_ws, 0, row, col_count);
		else {
			memset(&info, 0, sizeof(reg_check_info_t));
			code = excel_reg_check_row_collect(row, col_count, dir_name,
				&info);

			/* If the personnel information is valid, the executive
			 * registration process
			 */
			if (PERSON_REG_OK == code) {
				memset(detail_info, 0, sizeof(detail_info));
				excel_reg_check_row_proc(&info, &code,
					detail_info, sizeof(detail_info), dir_name);
			}

			/* Use the registered result code to compare with the preset
			 * value, and then write the result of the comparison to excel.
			 */
			if (((0 == strcmp(info.reg_expected_result,
				REG_EXPECTED_RESULT_PASS)) && (PERSON_REG_OK == code)) ||
				((0 == strcmp(info.reg_expected_result,
				REG_EXPECTED_RESULT_NOT_PASS)) &&
				(PERSON_REG_REFUSE == code))) {
				compare_code = PERSON_REG_COMPARE_RESULT_OK;
			} else {
				compare_code = PERSON_REG_COMPARE_RESULT_FAIL;
				reg_fail_count++;
			}
			excel_reg_check_row_write(p_write_ws, j, row, col_count, code,
				detail_info, compare_code);
		}
		#ifdef _EXCEL_REG_CHECK_DEBUG_
		ALOGD("\n");
		#endif
		excel_reg_check_set_current_status(j, reg_fail_count);
	}

	/* Close the Excel file that records the failure information */
	excel_file_write_close(p_write_wb, write_excel_file_name);

	/* Close batch import of Excel files */
	xls_close_WS(ws);

	return 0;
}

/* get status */
int person_reg_check_get_ctx(person_reg_check_ctx_t *p_ctx)
{
	assert(p_ctx != NULL);

	memcpy(p_ctx, &check_ctx, sizeof(person_reg_check_ctx_t));

	return 0;
}

/* Init status */
int person_reg_check_init_ctx(void)
{
	memset(&check_ctx, 0, sizeof(person_reg_check_ctx_t));
	check_ctx.done = EE_THREAD_NOT_RUN;
	check_ctx.recg_param.threshold = config_get_double("recognition",
						"recg_threshold", 0.5f);
	check_ctx.recg_param.jpeg_width_stride = config_get_int("recognition",
							"jpeg_w_stride", 800);
	check_ctx.recg_param.jpeg_height_stride = config_get_int("recognition",
							"jpeg_h_stride", 1024);
	return 0;
}

/**
 * person_reg_check_proc - Reading personnel information from Excel.
 * @arg: Excel file for batch import of personnel information.
 *
 * Returns 0 if success, otherwise not 0.
 */
void *person_reg_check_proc(void *arg)
{
	int i = 0;
	int ret = -1;
	DWORD sheet_count = 0;
	xlsWorkBook *wb = NULL;
	xlsWorkSheet *ws = NULL;
	char *dname;
	char dirc[128] = {0};
	char *p_read_excel_path = NULL;

	assert(arg != NULL);

	/* Change the thread execution state to the running state */
	excel_reg_check_set_done_status(EE_THREAD_RUNNING);

	p_read_excel_path = (char *)arg;
	ALOGD("p_read_excel_path=%s\n", p_read_excel_path);

	/* Get the path where the Excel file is located (excluding the file nama) */
	snprintf(dirc, sizeof(dirc), "%s", p_read_excel_path);
	dname = dirname(dirc);

	/* Open the Excel file */
	ret = excel_file_read_open(p_read_excel_path, EXCEL_CHAR_SET, &wb);
	if (ret != 0)
		goto REG_CHECK_END;

	/* sheets: Get the worksheet and then parse */
	sheet_count = wb->sheets.count;
	#ifdef _EXCEL_REG_CHECK_DEBUG_
	ALOGD("Count of sheets: %i\n", sheet_count);
	#endif
	/* When excel is greater than 1 sheet, only handler first sheet */
	if (sheet_count > 1)
		sheet_count = 1;

	ALOGD("sheet_count %d\n", sheet_count);
	for (i = 0; i < (int)sheet_count; i++) {
		#ifdef _EXCEL_REG_CHECK_DEBUG_
		ALOGD("Sheet %i (%s) pos %i\n", i , wb->sheets.sheet[i].name,
			wb->sheets.sheet[i].filepos);
		#endif

		ws = xls_getWorkSheet(wb, i);
		ret = excel_reg_check_file_sheet_proc(ws, dname);
		if (ret != 0)
			ALOGE("Proc person reg check error\n");
	}

	/* Close the Excel file */
	excel_file_read_close(wb);

REG_CHECK_END:
	/* Synchronize file system data into physical media */
	file_system_sync();

	/* Change the thread execution state to the end state */
	excel_reg_check_set_done_status(EE_THREAD_DONE);

	return 0;
}

/**
 * person_reg_check_thread - Use thread to process the person reg check function
 * in the backgroud.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_reg_check_thread()
{
	person_reg_check_init_ctx();

	pthread_create(&(check_ctx.tid), NULL, person_reg_check_proc,
		PERSON_REG_CHECK_FILE_PATH);
	pthread_detach(check_ctx.tid);

	return 0;
}


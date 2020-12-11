/*
 * excel_file_write.c - Operation of writing Excel files.
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

/**
 * excel_file_write_open - Open an Excel file.
 * @p_out_wb: Out, Returns the handle of Excel file
 * @p_out_ws: Out, Returns the handle of Excel sheet
 *
 * Returns 0 if success, otherwise not 0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define LOG_TAG "XLS"
#include "log/log.h"
#include "file_ops.h"
#include "excel_file_write.h"

/**
 * excel_file_write_open - Open an Excel file.
 * @p_out_wb: Out, Returns the handle of Excel file
 * @excel_name: Excel file path
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_create_workbook(lxw_workbook **p_out_wb, char *excel_name)
{
	int ret = 0;
	lxw_workbook *wb;
	lxw_workbook_options options = {
		.constant_memory = LXW_TRUE,
		.tmpdir = "/data",
	};

	assert(p_out_wb != NULL);

	/* Use "Constant Memory Mode"
	 * This optimization works by flushing each row after a subsequent row is written
	 */
	wb = workbook_new_opt(excel_name, &options);

	*p_out_wb = wb;

	return ret;
}

/**
 * excel_file_write_create_sheet - Create a sheet.
 * @p_out_wb: In, the handle of Excel file
 * @p_out_ws: Out, Returns the handle of Excel sheet
 * @str_sheet_name Excel sheet name
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_create_sheet(lxw_workbook *p_wb, lxw_worksheet **p_out_ws,
	char *str_sheet_name)
{
	int ret = 0;
	lxw_worksheet *ws;

	assert((p_wb != NULL) && (p_out_ws != NULL) && (str_sheet_name != NULL));

	ws = workbook_add_worksheet(p_wb, str_sheet_name);

	*p_out_ws = ws;

	return ret;
}

/**
 * excel_punch_record_write - Write punch record to Excel file.
 * @excel_name: Excel file path
 * @sheet_name: Excel sheet name.
 * @p_out_wb: Out, the handle of Excel file
 * @p_out_ws: Out, Returns the handle of Excel sheet
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_open(char *excel_name, char *sheet_name, lxw_workbook **p_out_wb,
	lxw_worksheet **p_out_ws)
{
	int ret = 0;

	assert((excel_name != NULL) && (sheet_name != NULL) && (p_out_wb != NULL)
		&& (p_out_ws != NULL));

	/* Create Excel file */
	ret = excel_file_write_create_workbook(p_out_wb, excel_name);
	if (ret != 0) {
		#ifdef _EXCEL_PUNCH_RECORD_EXPORT_DEBUG_
		ALOGE("create excel file %s fail\n", excel_name);
		#endif

		return -1;
	}

	/* Create excel sheet */
	ret = excel_file_write_create_sheet(*p_out_wb, p_out_ws, sheet_name);
	if (ret != 0) {
		#ifdef _EXCEL_PUNCH_RECORD_EXPORT_DEBUG_
		ALOGE("create excel sheet %s fail\n", sheet_name);
		#endif

		return -1;
	}

	return 0;
}

/**
 * excel_file_write_close - Close an Excel file.
 * @wb: The handle to close the Excel file.
 * @file_name: Excel file name.
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_close(lxw_workbook *wb, char *file_name)
{
	int ret = 0;
	lxw_error error;

	assert((wb != NULL) && (file_name != NULL));

	error = workbook_close(wb);
	if (error != LXW_NO_ERROR) {
		#ifdef _EXCEL_PUNCH_RECORD_EXPORT_DEBUG_
		ALOGE("Close (%s) fail, Error %d = %s\n", file_name, error,
			lxw_strerror(error));
		#endif

		ret =  -1;

	} else {
		/* Detect if the excel file exists(ex: if the UDisk is unplugged) */
		if (-1 == is_file_exist(file_name)) {
			ALOGE("Excel not exists");
			ret = -1;
		}
	}

	return ret;
}


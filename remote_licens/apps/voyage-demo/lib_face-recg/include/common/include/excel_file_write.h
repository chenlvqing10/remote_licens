/*
 * excel_file_write.h - Operation of write Excel files
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

#ifndef _EXCEL_FILE_WRITE_H
#define _EXCEL_FILE_WRITE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "xlsxwriter.h"

#define _EXCEL_PUNCH_RECORD_EXPORT_DEBUG_

/**
 * excel_file_write_open - Open an Excel file.
 * @p_out_wb: Out, Returns the handle of Excel file
 * @excel_name: Excel file path
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_create_workbook(lxw_workbook **p_out_wb, char *excel_name);

/**
 * excel_file_write_create_sheet - Create a sheet.
 * @p_out_wb: In, the handle of Excel file
 * @p_out_ws: Out, Returns the handle of Excel sheet
 * @str_sheet_name Excel sheet name
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_create_sheet(lxw_workbook *p_wb, lxw_worksheet **p_out_ws,
	char *str_sheet_name);

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
	lxw_worksheet **p_out_ws);

/**
 * excel_file_write_close - Close an Excel file.
 * @wb: The handle to close the Excel file.
 * @file_name: Excel file name.
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_write_close(lxw_workbook *wb, char *file_name);

#endif /* _EXCEL_FILE_WRITE_H */



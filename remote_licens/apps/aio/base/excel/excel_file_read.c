/*
 * excel_file_read.c - Operation of reading Excel files
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

#include "excel_file_read.h"

#define LOG_TAG "XLS"
#include "log/log.h"
#include "xls.h"

/**
 * excel_file_change_content_to_str - Converting various types of data to string types.
 * @content: IN, Point to a structure that stores excel unit data.
 * @str: OUT, Point to the buffer used to store the converted string.
 * @str_max_len: IN, The buffer pointed to by the second parameter, the maximum length of
 *	data that can be stored
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_change_content_to_str(cell_content_t *content, char *str,
	unsigned int str_max_len)
{
	int value = 0;
	CONTENTS_TYPE_E	type = CELL_UNKNOWN;

	assert((content != NULL) && (str != NULL));

	type = content->type;
	switch (type) {
	case CELL_BLANK:
		break;

	case CELL_STRING:
		snprintf(str, str_max_len, "%s", content->str);
		break;
	case CELL_UNKNOWN:
		snprintf(str, str_max_len, "%s", content->str);
		break;

	case CELL_ERROR:
		snprintf(str, str_max_len, "%d", content->val.e);
		break;

	case CELL_BOOL:
		if (content->val.b > 0)
			value = 1;
		else
			value = 0;
		snprintf(str, str_max_len, "%d", value);
		break;

	case CELL_FLOAT:
		snprintf(str, str_max_len, "%.0f", content->val.d);
		break;

	case CELL_INTEGER:
		snprintf(str, str_max_len, "%ld", content->val.l);
		break;

	default:
		ALOGE("unknow excel cell type\n");
		break;
	}

	return 0;
}

/**
 * excel_file_read_cell - Reading Data and Types from excel unit.
 * @cell: Point to an excel unit.
 * @content: Point to a structure that stores excel unit data.
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_read_cell(xlsCell *cell, cell_content_t *content)
{
	assert((cell != NULL) && (content != NULL));

	memset(content, 0, sizeof(cell_content_t));

	content->col = cell->col + 1;
	content->row = cell->row + 1;
	snprintf(content->str, sizeof(content->str), "%s", cell->str);

	switch (cell->id) {
	/* Cell Value: Formula */
	case XLS_RECORD_FORMAT:
		if (0 == cell->l) {
			content->type = CELL_FLOAT;
			content->val.d = cell->d;
		} else {
			if (0 == strcmp(cell->str, "bool")) {
				/* its boolean, and test cell->d > 0.0 for true */
				content->type = CELL_BOOL;
				content->val.b = (bool)cell->d;
			} else if (0 == strcmp(cell->str, "error")) {
				/* formula is in error */
				content->type = CELL_ERROR;
				content->val.e = (int32_t)cell->d;
			} else
				/* cell->str is valid as the result of a string formula */
				content->type = CELL_STRING;
		}
		break;

	/* Cell Value: Blank Cell */
	case XLS_RECORD_BLANK:
	case XLS_RECORD_MULBLANK:
		content->type = CELL_BLANK;
		break;

	/* Cell Value: Floating-Point Number/RK Number */
	case XLS_RECORD_RK:
	case XLS_RECORD_NUMBER:
		content->type = CELL_FLOAT;
		content->val.d = cell->d;
		break;

	/* Cell Value: "String Constant/SST" / "String Constant" */
	case XLS_RECORD_LABEL:
	case XLS_RECORD_LABELSST:
		content->type = CELL_STRING;
		 /* possible numeric conversion done for you */
		content->val.l = cell->l;
		break;

	/* cell->str contains a string value */
	default:
		content->type = CELL_UNKNOWN;
		break;
	}

	return 0;
}

/**
 * excel_file_read_open - Open an Excel file.
 * @p_file_path: Excel file path.
 * @p_char_set: The encoding of strings
 * @p_out_wb: Out, Returns the handle to open the Excel file
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_read_open(char *p_file_path, char *p_char_set,
			xlsWorkBook **p_out_wb)
{
	int ret = 0;
	xls_error_t code = LIBXLS_OK;

	assert((p_file_path != NULL) && (p_char_set != NULL) && (p_out_wb != NULL));

	ALOGD("p_file_path=%s, p_char_set=%s\n", p_file_path, p_char_set);

	*p_out_wb = xls_open_file(p_file_path, p_char_set, &code);
	if (NULL == *p_out_wb) {
		ret = -1;
		ALOGE("Error reading file: %s\n", xls_getError(code));
	}

	return ret;
}

/**
 * excel_file_read_close - Close an Excel file.
 * @p_wb: The handle to open the Excel file.
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_read_close(xlsWorkBook *p_wb)
{
	assert(p_wb != NULL);

	xls_close_WB(p_wb);

	return 0;
}


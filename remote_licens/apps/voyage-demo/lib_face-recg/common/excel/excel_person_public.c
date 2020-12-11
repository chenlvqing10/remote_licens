/*
 * excel_person_public.c - Reading personnel registration information from Excel.
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

#include "char_ops.h"
#include "time_conv.h"
#include "excel_file_read.h"
#include "excel_file_write.h"
#include "excel_person_public.h"

#define LOG_TAG "XLS"
#include "log/log.h"

/**
 * excel_check_str_cell_valid - Check if the value of the string type is valid
 * @cell A pointer to 'xlsCell'
 * @null_falg Wehther this item is allowed to be NULL
 * @out_str Pointer to the output string
 * @out_str_max_len The max length of the output string
 *
 * Returns 0 if valid, otherwise not 0.
 */
int excel_check_str_cell_valid(xlsCell *cell, e_conten_null_t null_falg,
		char *out_str, unsigned int out_str_max_len)
{
	int ret = 0;
	cell_content_t content;

	assert((cell != NULL) && (out_str != NULL));

	memset(&content, 0, sizeof(cell_content_t));

	excel_file_read_cell(cell, &content);
	if (CELL_BLANK == content.type) {
		/* do something with a blank cell */
		if (IS_NOT_NULL == null_falg)
			ret = -1;
		else
			ret = 0;
	} else {
		excel_file_change_content_to_str(&content, out_str, out_str_max_len);

		#ifdef _EXCEL_PERSON_INFO_DEBUG_
		ALOGD("%-20s", out_str);
		#endif

		ret = 0;
	}

	return ret;
}

/**
 * excel_check_int_cell_valid - Check if the value of the int type is valid
 * @cell A pointer to 'xlsCell'
 * @null_falg Wehther this item is allowed to be NULL
 * @out_str Pointer to the output string
 * @out_str_max_len The max length of the output string
 *
 * Returns 0 if valid, otherwise not 0.
 */
int excel_check_int_cell_valid(xlsCell *cell, e_conten_null_t null_falg,
		int *out_value)
{
	int ret = 0;
	cell_content_t content;
	char str[EXCEL_CELL_MAX_STR_LEN] = {0};

	assert((cell != NULL) && (out_value != NULL));

	memset(&content, 0, sizeof(cell_content_t));
	excel_file_read_cell(cell, &content);
	if (CELL_BLANK == content.type) {
		/* do something with a blank cell */
		if (IS_NOT_NULL == null_falg)
			ret = -1;
		else {
			ret = 0;
			*out_value = 0;
		}
	} else {
		memset(str, 0, sizeof(str));
		excel_file_change_content_to_str(&content, str, sizeof(str));
		if (strlen(str) > 0) {
			if (1 == is_digit_str(str)) {
				*out_value = atoi(str);
				ret = 0;
			} else
				ret = -1;
			#ifdef _EXCEL_PERSON_INFO_DEBUG_
			ALOGD("%-20d", *out_value);
			#endif
		} else
			ret = 0;
	}

	return ret;
}

/**
 * excel_check_time_cell_valid - Check if the value of the time type is valid
 * @cell A pointer to 'xlsCell'
 * @null_falg Wehther this item is allowed to be NULL
 * @out_str Pointer to the output string
 * @out_str_max_len The max length of the output string
 *
 * Returns 0 if valid, otherwise not 0.
 */
int excel_check_time_cell_valid(xlsCell *cell, e_conten_null_t null_falg,
		long *out_value)
{
	int ret = 0;
	char str[EXCEL_CELL_MAX_STR_LEN] = {0};
	cell_content_t content;

	assert((cell != NULL) && (out_value != NULL));

	memset(&content, 0, sizeof(cell_content_t));
	excel_file_read_cell(cell, &content);
	if (CELL_BLANK == content.type) {
		/* do something with a blank cell */
		if (IS_NOT_NULL == null_falg)
			ret = -1;
		else {
			ret = 0;
			*out_value = 0;
		}
	} else {
		memset(str, 0, sizeof(str));
		excel_file_change_content_to_str(&content, str, sizeof(str));
		if (strlen(str) > 0) {
			if (0 == time_str_to_second(str, out_value))
				ret = 0;
			else
				ret = -1;
		} else
			ret = 0;

		#ifdef _EXCEL_PERSON_INFO_DEBUG_
		ALOGD("%-20ld", *out_value);
		#endif
	}

	return ret;
}


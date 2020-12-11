/*
 * excel_file_read.h - Operation of reading Excel files
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

#ifndef _EXCEL_FILE_READ_H
#define _EXCEL_FILE_READ_H

#include "xls.h"

/* The encoding of strings obtained by 'libxls' Library */
#define EXCEL_CHAR_SET		"UTF-8"

/* The maximum string length in an excel unit */
#define EXCEL_CELL_MAX_STR_LEN	4096

/* The type of Data stored in excel unit */
typedef enum {
	CELL_BLANK = 0,
	CELL_STRING,
	CELL_INTEGER,
	CELL_FLOAT,
	CELL_BOOL,
	CELL_ERROR,
	CELL_UNKNOWN
} CONTENTS_TYPE_E;

typedef struct tag_cell_content {
	CONTENTS_TYPE_E	type;
	uint32_t	col;				/* 1 based */
	uint16_t	row;				/* 1 based */
	char		str[EXCEL_CELL_MAX_STR_LEN];	/* even for numbers these values
							 * are formatted as well as
							 * provided below
							 */
	union cell_val {
		long	l;
		double	d;
		bool	b;
		int32_t e;
	} val;
} cell_content_t;

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
	unsigned int str_max_len);

/**
 * excel_file_read_cell - Reading Data and Types from excel unit.
 * @cell: Point to an excel unit.
 * @content: Point to a structure that stores excel unit data.
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_read_cell(xlsCell *cell, cell_content_t *content);

/**
 * excel_file_read_open - Open an Excel file.
 * @p_file_path: Excel file path.
 * @p_char_set: The encoding of strings
 * @p_out_wb: Out, Returns the handle to open the Excel file
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_read_open(char *p_file_path, char *p_char_set, xlsWorkBook **p_out_wb);

/**
 * excel_file_read_close - Close an Excel file.
 * @p_wb: The handle to open the Excel file.
 *
 * Returns 0 if success, otherwise not 0.
 */
int excel_file_read_close(xlsWorkBook *p_wb);

#endif /* _EXCEL_FILE_READ_H */


/*
 * excel_person_public.h - Reading personnel registration information from Excel.
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

#ifndef _EXCEL_PERSON_PUBLIC_H
#define _EXCEL_PERSON_PUBLIC_H

#include "xls.h"

/** A flag indicating whether the conten is NULL */
typedef enum {
	IS_ALLOW_NULL = 0,		/* Cell conten is Allowed to be NULL */
	IS_NOT_NULL,			/* Cell conten is Not allowed to be NULL */
} e_conten_null_t;

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
		char *out_str, unsigned int out_str_max_len);

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
		int *out_value);

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
		long *out_value);

#endif /* _EXCEL_PERSON_PUBLIC_H */


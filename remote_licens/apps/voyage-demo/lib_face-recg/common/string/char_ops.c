/*
 * char_ops.h - String processing interface.
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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "char_ops.h"

#define LOG_TAG "CHAR_OPS"
#include "log/log.h"

/**
 * is_digit_str - Determine whether a string is a number.
 * @str: IN, Pointer to the input string.
 *
 * Returns 1 if it's numbers, otherwise not 0.
 */
int is_digit_str(char *str)
{
	return (strspn(str, "0123456789") == strlen(str));
}

/**
 * conv_float_to_specify_decimal_str - Converts the number of float types to a string of
 * specified decimal digits.
 * @value: IN, float number.
 * @num_decimal: Numbers of decimal digits to be retained.
 * @str: Convert the number of float types to a string.
 * @str_max_len: Maximum length of string.
 * Returns 0 if success, otherwise not 0.
 */
int conv_float_to_specify_decimal_str(float value, unsigned int num_decimal,
	char *str, unsigned int str_max_len)
{
	char buf[64];
	int len = 0;
	int offset = 0;
	int save_len = 0;
	char *pos = NULL;
	int after_point_len = 0;

	assert(str != NULL);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%f", value);

	/* Find the location of the "." of a string */
	pos  = strstr(buf, ".");
	if (pos != NULL) {
		/* The offset of decimal points in strings */
		offset = (pos - buf);

		/* How many digits are there after the decimal point */
		after_point_len = strlen(buf) - offset - 1;

		save_len = (after_point_len > num_decimal) ? num_decimal :
			after_point_len;

		/* Gets the data for the specified decimal point */
		len = offset + save_len + 1;
		memcpy(str, buf, len);
	} else
		snprintf(str, str_max_len, "%s", buf);

	return 0;
}


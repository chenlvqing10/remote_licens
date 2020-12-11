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

#ifndef _CHAR_OPS_H
#define _CHAR_OPS_H

/**
 * is_digit_str - Determine whether a string is a number.
 * @str: IN, Pointer to the input string.
 *
 * Returns 0 if it's numbers, otherwise not 0.
 */
int is_digit_str(char *str);

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
	char *str, unsigned int str_max_len);

#endif  /* _CHAR_OPS_H */


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



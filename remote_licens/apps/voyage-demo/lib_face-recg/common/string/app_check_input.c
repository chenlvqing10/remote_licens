/*
 * app_check_input.c - Check if the input is valid
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

#include <stdlib.h>
#include<regex.h>

#include "app_check_input.h"

#define LOG_TAG "APP_CHECK_INPUT"
#include "log/log.h"

/**
 * app_input_str_is_valid - Check if the input string is valid
 * @user_input_str User-entered string
 *
 * Returns 0 if it is a valid string, otherwise not 0.
 */
int app_input_str_is_valid(const char *user_input_str)
{
	regex_t regex;
	int reti = 0;
	int regex_init = 0;
	int ret = -1;

	/* Filter string: '<>=!-+*(/)|;$#&@% */
	char pattern[] = "\f|\n|\r|\t|\v|'+|<+|>+|=+|!+|-+|\\++|\\*+|/+|"
			"\\(+|\\)+|\\|+|,+|;+|\\$+|#+|&+|@+|%+";

	regex_init = regcomp(&regex, pattern, REG_EXTENDED);

	if (regex_init) {
		/* Compile regex failed */
		ALOGE("Compile regex failed\n");
		ret = 0;
	} else {
		reti = regexec(&regex, user_input_str, 0, NULL, 0);
		if (REG_NOERROR == reti) {
			/* Success. */
			ret = -1;
		} else {
			/* Didn't find a match (for regexec).  */
			ret = 0;
		}
	}

	regfree(&regex);

	return ret;
}


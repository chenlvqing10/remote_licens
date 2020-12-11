/*
 * app_check_input.h - Check if the input is valid
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

#ifndef _APP_CHECK_INPUT_H
#define _APP_CHECK_INPUT_H

/**
 * app_input_str_is_valid - Check if the input string is valid
 * @user_input_str User-entered string
 *
 * Returns 0 if it is a valid string, otherwise not 0.
 */
int app_input_str_is_valid(const char *user_input_str);

#endif /* _APP_CHECK_INPUT_H */

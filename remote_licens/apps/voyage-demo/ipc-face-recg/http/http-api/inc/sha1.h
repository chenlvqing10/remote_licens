/*
 * sha1.h - sha1
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

#ifndef __SHA1_H__
#define __SHA1_H__

/* get_sha1_str: calcuating string sha1
 * @str: source string
 * @length£ºlength of str
 * @sha1: destination string
 *
 * Return: 0 if success, -1 if failure.
*/
int get_sha1_str(const char* str, long long length, char* sha1);

#endif //__SHA1_H__

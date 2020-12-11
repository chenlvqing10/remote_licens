/*
 * clang.h - Multi-language interface
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#ifndef __ELANG_H__
#define __ELANG_H__

#include "lang.h"

/**
* clang_load - load language file
* @path: file path
*
* return -1 if fail
*/
int clang_load(char *path);

/**
* clang_unload - unload language
*
* return -1 if fail
*/
int clang_unload(void);

/**
* clang_set_language - setting language
* @lang_index: index in lang.h
*
* return -1 if fail
*/
int clang_set_language(unsigned int lang_index);

/**
* clang_get_utf8_string - get utf8 string
* @strIndex: index in lang.h
*
* return -1 if fail, don't free the pointer
*/
const char *clang_get_utf8_string(unsigned int str_index);

/**
* clang_str - get utf8 string
* @json_str: string in json
*
* return -1 if fail, don't free the pointer
*/
char *clang_str(const char *json_str);

void clang_test(void);

#endif /* __ELANG_H__ */


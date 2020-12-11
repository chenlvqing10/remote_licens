/*
 * system_info.h - define some function of system infomation
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

#ifndef _SYSTEM_INFO_H
#define _SYSTEM_INFO_H

typedef struct tag_build_info_t {
	char build_date[64];
	char build_user[64];
	char build_host[64];
	char product_name[64];
} build_info_t;

/**
 * system_info_get_kernel_version - Get the kernel version
 * @kernel_version: OUT, Store kernel version string
 * @max_len: IN, Maximum length of string that can be stored
 *
 * Returns 0 if Get the kernel version successfully, otherwise -1.
 */
int system_info_get_kernel_version(char *kernel_version, int max_len);

/**
 * system_info_get_build_info - Get compilation information
 * @p_build_info: OUT, Store kernel version string
 *
 * Returns 0 if Get the compilation information, otherwise -1.
 */
int system_info_get_build_info(build_info_t *p_build_info);

#endif /* _SYSTEM_INFO_H */

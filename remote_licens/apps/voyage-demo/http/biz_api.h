/*
 * api/biz_api.h - all functions for RESTful.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _BIZ_API_H_
#define _BIZ_API_H_

int recognition_set_param(float thr, int lvflg);
int recognition_get_param(float *thr, int *lvflg);
int display_set_bright_to_device(int bri);
int display_get_bright_from_device(int *bri);
int system_info_get_build_info(char *build_date, char *build_user,
	char *build_host, char *product_name);

#endif


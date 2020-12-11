/*
 * api/web_biz_api.c - all functions for RESTful.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "biz_api.h"

#define LOG_TAG "biz_api"
#include <log/log.h>

int recognition_set_param(float thr, int lvflg)
{
	ALOGD("set recognition param: thr=%f, lvflg=%d\n", thr, lvflg);
	return 0;
}

int recognition_get_param(float *thr, int *lvflg)
{
	*thr = 0.6;
	*lvflg = 1;
	return 0;
}

int display_set_bright_to_device(int bri)
{
	ALOGD("set bright value = %d\n", bri);
	return 0;
}

int display_get_bright_from_device(int *bri)
{
	*bri = 19;
	return 0;
}

int system_info_get_build_info(char *build_date, char *build_user,
	char *build_host, char *product_name)
{
	strcpy(build_date, "YYYYMMDD");
	strcpy(build_user, "lombo");
	strcpy(build_host, "SzSwSrv1.lombo.com");
	strcpy(product_name, "n7v1_voyage");
	return 0;
}

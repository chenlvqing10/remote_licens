/*
 * cloud_platform.c - Interface for the restore factory default function
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "db_sqlite_middle.h"
#include "db_cloud_platform.h"
#include "db_config.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * cloud_platform_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void cloud_platform_print(cloud_platform_t *p_info)
{
	assert(p_info != NULL);

	ALOGD("[%s:%d] ai_server_url:%s attendance_url:%s fota_url:%s\n",
		__func__,
		__LINE__,
		p_info->ai_server_url,
		p_info->attendance_url,
		p_info->fota_url);
}

/**
 * db_cloud_platform_update - Update cloud platform information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_cloud_platform_update(cloud_platform_t *p_info)
{
	int ret = 0;

	assert(p_info != NULL);

	ret += db_config_set_string(DBCONF_CLOUDPLATFORM,
		DBCONF_CLOUDPLATFORM_AISERVERURL, p_info->ai_server_url);
	ret += db_config_set_string(DBCONF_CLOUDPLATFORM,
		DBCONF_CLOUDPLATFORM_ATTENDANCEURL, p_info->attendance_url);
	ret += db_config_set_string(DBCONF_CLOUDPLATFORM,
		DBCONF_CLOUDPLATFORM_FOTAURL, p_info->fota_url);

	return ret;
}

/**
 * db_cloud_platform_query - Query cloud platform information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_cloud_platform_query(cloud_platform_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];
	int ret = 0;

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(cloud_platform_t));
	memset(str_sql, 0, sizeof(str_sql));

	ret += db_config_get_string(DBCONF_CLOUDPLATFORM,
	DBCONF_CLOUDPLATFORM_AISERVERURL, p_info->ai_server_url,
	sizeof(p_info->ai_server_url), DBCONF_CLOUDPLATFORM_AISERVERURL_VALUE);

	ret += db_config_get_string(DBCONF_CLOUDPLATFORM,
	DBCONF_CLOUDPLATFORM_ATTENDANCEURL, p_info->attendance_url,
	sizeof(p_info->attendance_url), DBCONF_CLOUDPLATFORM_ATTENDANCEURL_VALUE);

	ret += db_config_get_string(DBCONF_CLOUDPLATFORM, DBCONF_CLOUDPLATFORM_FOTAURL,
	p_info->fota_url, sizeof(p_info->fota_url),
	DBCONF_CLOUDPLATFORM_FOTAURL_VALUE);

	return 1;
}


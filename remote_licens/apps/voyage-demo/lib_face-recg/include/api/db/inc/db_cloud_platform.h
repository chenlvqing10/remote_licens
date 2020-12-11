/*
 * cloud_platform.h - Interface for the restore factory default function
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

#ifndef __CLOUD_PLATFORM_H
#define __CLOUD_PLATFORM_H

#include "ee_queue.h"

typedef struct tag_cloud_platform {
	/* AI server URL address。Store the face features library in the cloud.*/
	char ai_server_url[256];

	/* Attendance cloud platform URL address。Manage through the cloud
	 * (Visitor, Approval，Attendance, Personnel management,etc)
	 */
	char attendance_url[256];

	/* FOTA upgrade URL address。periodically detect this server */
	char fota_url[256];
} cloud_platform_t;

/**
 * cloud_platform_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void cloud_platform_print(cloud_platform_t *p_info);

/**
 * db_cloud_platform_update - Update cloud platform information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_cloud_platform_update(cloud_platform_t *p_info);

/**
 * db_cloud_platform_query - Query cloud platform information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_cloud_platform_query(cloud_platform_t *p_info);

#endif /* __CLOUD_PLATFORM_H */


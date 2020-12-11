/*
 * device_status_param.h - Interface for setting device status parameter functions
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

#ifndef __DEVICE_STATUS_INFO_H
#define __DEVICE_STATUS_INFO_H

#include "ee_queue.h"

typedef struct tag_device_status_info {
	/* Company Name. Information displayed in the UI main window */
	char company_name[32];

	/* Company Logo file name。 */
	char company_logo[32];

	/* Device installation address */
	char device_address[256];

	/* Multi-language choice
	 * Options:
	 *	0: English
	 *	1: Chinese
	 */
	char language;

	/* Punch mode
	 * Options:
	 *	0: 1:N
	 *	1: 1:1 + Card reader
	 *	2: 1:1 + Resident Identity Card
	 *	3: Card reader
	 *	4: Resident Identity Card
	 */
	char punch_mode;

	/* Debug switch
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char debug_switch;
} device_status_param_t;

/**
 * device_status_info_print - Display information in a queue for debugging
 * purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_status_info_print(device_status_param_t *p_info);

/**
 * db_device_status_param_update - Update device status information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_status_param_update(device_status_param_t *p_info);

/**
 * db_device_status_param_query - Query device status information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_status_param_query(device_status_param_t *p_info);

#endif /* __DEVICE_STATUS_INFO_H */


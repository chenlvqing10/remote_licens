/*
 * device_hw_param.h - Interface for setting device hardware parameter functions
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

#ifndef __DEVICE_HW_PARAM_H
#define __DEVICE_HW_PARAM_H

#include "ee_queue.h"

typedef struct tag_device_hw_param {
	/* Volume. Set the volume of the microphone and speaker(range 0~100) */
	char volume;

	/* Automatically adjust screen brightness control
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char auto_display_brightness;

	/* Set the screen brightness value (range 0~100).
	 * When auto_display_brightness function is enabled, Current setting
	 * no effect.
	 */
	char display_brightness;

	/* Infrared light control:
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 *	2: Automatic
	 */
	char infrared_light_set;

	/* Light sensitive control:
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char light_sensitive;

	/* Human induction control:
	 * Options:
	 *	0: Disabled
	 *	1: Enabled
	 */
	char human_induction;

	/* Automatic screen off function
	 * Options:
	 *	0: Disabled
	 *	1: Enabled
	 */
	int auto_screen_off;

	/* Auto-out timeout */
	int auto_screen_off_timeout;

	/* Screen saver function
	 * Options:
	 *	0: Disabled
	 *	1: Enabled
	 */
	int screen_saver_off;

	/* Screen saver timeout */
	int screen_saver_off_timeout;

	/* Picture count */
	int screen_saver_count;

	/* Each picture show time */
	int screen_saver_time;
} device_hw_param_t;

/**
 * device_hw_param_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_hw_param_print(device_hw_param_t *p_info);

/**
 * db_device_hw_param_update - Update device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_update(device_hw_param_t *p_info);

/**
 * db_device_hw_param_query - Query device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_query(device_hw_param_t *p_info);

#endif /* __DEVICE_HW_PARAM_H */


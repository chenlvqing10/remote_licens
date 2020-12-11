/*
 * hotplug.h - Interface for operating U Disk
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

#ifndef __HOTPLUG_H__
#define __HOTPLUG_H__

typedef enum {
	HOTPLUG_TYPE_SDCARD = 0,
	HOTPLUG_TYPE_UDISK
} hotplug_device_type_e;

typedef enum {
	HOTPLUG_STATE_DISCONNECT = 0,
	HOTPLUG_STATE_CONNECT
} hotplug_device_state_e;

/**
 * udisk_is_insert - Check if the U Disk is insert
 * None
 *
 * Returns 1 if insert, otherwise not mount.
 */
int udisk_is_insert(void);

/**
 * udisk_is_ok - Check if the U Disk is mounted
 * None
 *
 * Returns 0 if mount, otherwise not mount.
 */
int udisk_is_ok(void);

/**
 * sdcard_is_ok - Check if the sdcard is mounted
 * None
 *
 * Returns 0 if mount, otherwise not mount.
 */
int sdcard_is_ok(void);

typedef void (*hotplug_change_cb)(hotplug_device_type_e type,
					hotplug_device_state_e state);

int hotplug_register_change_cb(hotplug_change_cb cb);

int hotplug_mgr_init(void);

#endif /* __HOTPLUG_H__ */

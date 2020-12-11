/*
 * reset_svc.h - resetting service background for monitoring reset key and
 *               provide resetting device api
 *
 *
 * Copyright (C) 2016-2020, LomboTech Co.Ltd.
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

#ifndef __UPDATE_SVC_H
#define __UPDATE_SVC_H

/**
 * update_device_start - upgrade device firmware.
 *
 * Returns 0  if success, other if failure.
 */
int update_device_start(void);

/**
 * update_device_copy_pkg - upgrade device firmware.
 *
 * Returns 0  if success, other if failure.
 */
int update_device_copy_pkg(const char *base_path);

int update_device_cancel(void);

/**
 * find_pkg - find valid firmware for upgrade.
 *
 * Returns 0  if find, -1 if failure, 1 not find
 */
int find_pkg(void);
#endif /* __UPDATE_SVC_H */


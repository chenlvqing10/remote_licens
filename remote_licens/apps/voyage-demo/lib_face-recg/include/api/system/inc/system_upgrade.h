/*
 * system_upgrade.h - Interface to upgrade.
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

#ifndef _SYSTEM_UPGRADE_H
#define _SYSTEM_UPGRADE_H

/**
 * write_upgrade_flag_misc: write upgrade flag in /dev/mmcblk0p4
 * @NULL
  * Returns 0 means success, other means failure.
 */
int write_upgrade_flag_misc();

/**
 * write_upgrade_flag_rtc： write upgrade flag in /dev/lombo-misc
 * @NULL
  * Returns 0 means success, other means failure.
 */
int write_upgrade_flag_rtc();

#endif /* _SYSTEM_UPGRADE_H */

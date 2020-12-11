/*
 * storage.h - Interface for operating U Disk
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

#ifndef __STORAGE_H__
#define __STORAGE_H__


/**
 * get_disk_free_ratio - Get disk free ratio
 * @pdisk: input path of string disk path.
 *
 * Returns free radio of disk  if success, error -1.
 */
float get_disk_free_ratio(char *pdisk);

/**
 * get_disk_use_ratio_round_up_percent  - Get disk use ratio round_up percent
 * @pdisk: input path of string disk path.
 *
 * Returns use radio round up of disk  if success, error -1.
 */
int get_disk_use_ratio_round_up_percent(char *pdisk);


/**
 * get_disk_free - Get the avalible space of EMMC
 * @pdisk: Input path of string disk path.
 * @p_free_size: Return the avalible space(Unit: MB).
 *
 * Returns 0 if success, otherwise if failure.
 */
int get_disk_free(char *pdisk, float *p_free_size);

#endif /* __STORAGE_H__ */


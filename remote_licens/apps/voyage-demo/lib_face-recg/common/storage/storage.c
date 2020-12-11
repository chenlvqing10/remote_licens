/*
 * storage.c - Interface for operating U Disk
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOG_TAG		"STORAGE"
#include "log/log.h"

/**
 * get_disk_free_ratio - Get disk free ratio
 * @pdisk: input path of string disk path.
 *
 * Returns free radio of disk  if success, error -1.
 */
float get_disk_free_ratio(char *pdisk)
{
	struct statfs stat;
	int ret;

	long all_blocks;
	float free_ratio;

	if (NULL == pdisk)
		return -1;
	if (strlen(pdisk) <= 0)
		return -1;

	memset(&stat, 0, sizeof(stat));
	ret = statfs(pdisk, &stat);
	if (0 != ret) {
		ALOGE("[%s]statfs fail.\n", __func__);
		return -1;
	}

	all_blocks = stat.f_blocks - stat.f_bfree + stat.f_bavail;
	free_ratio = (float)stat.f_bavail/all_blocks;

	return free_ratio;
}

/**
 * get_disk_use_ratio_round_up_percent  - Get disk use ratio round_up percent
 * @pdisk: input path of string disk path.
 *
 * Returns use radio round up of disk  if success, error -1.
 */
int get_disk_use_ratio_round_up_percent(char *pdisk)
{
	struct statfs stat;
	int ret;

	long all_blocks;
	float use_ratio;

	int use_percent = 0;

	if (NULL == pdisk)
		return -1;
	if (strlen(pdisk) <= 0)
		return -1;

	memset(&stat, 0, sizeof(stat));
	ret = statfs(pdisk, &stat);
	if (0 != ret) {
		ALOGE("[%s]statfs fail.\n", __func__);
		return -1;
	}

	all_blocks = stat.f_blocks - stat.f_bfree + stat.f_bavail;
	use_ratio = (float)(all_blocks-stat.f_bavail)/all_blocks;

	use_percent = (int)(use_ratio*100+0.5);

	return use_percent;
}

/**
 * get_disk_free - Get the avalible space of EMMC
 * @pdisk: Input path of string disk path.
 * @p_free_size: Return the avalible space(Unit: MB).
 *
 * Returns 0 if success, otherwise if failure.
 */
int get_disk_free(char *pdisk, float *p_free_size)
{
	int ret;
	float temp = 0.0;
	struct statfs stat;

	if (NULL == pdisk)
		return -1;

	if (strlen(pdisk) <= 0)
		return -1;

	memset(&stat, 0, sizeof(stat));
	ret = statfs(pdisk, &stat);
	if (0 != ret) {
		ALOGE("[%s]statfs fail.\n", __func__);
		return -1;
	}

	/* Optimal transfer block size */
	unsigned long long block_size = stat.f_bsize;

	 /* Free blocks in filesystem */
	unsigned long long free_size = block_size * stat.f_bavail;

	/* Converting unit to MB */
	temp = (float)((free_size >> 20));

	/* Keep 1 decimal */
	*p_free_size = roundf(temp);

	return 0;
}


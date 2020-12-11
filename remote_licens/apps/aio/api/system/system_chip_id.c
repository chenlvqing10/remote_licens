/*
 * system_chip_id.c - Get the chip id of the device
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
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "system_chip_id.h"

/* ioctrl for getting chip ID */
#define	BUF_ORDER	0xefe00007
#define MISC_DEVICE	"/dev/lombo-misc"

#define LOG_TAG	"CHIP_ID"
#include <log/log.h>

/**
 * system_get_chip_id - Get the chip id of the device.
 * @p_chip_id: Out, Return device chip id information.
 * @len_max: In, Maximum length to save chip ID.
 * Returns 0 if success, otherwise not 0.
 */
int system_get_chip_id(char *p_chip_id, int len_max)
{
	int i = 0;
	int fd = 0;
	int ret = 0;
	unsigned char id[CHIP_ID_LEN] = {0};

	assert(p_chip_id != NULL);

	/* Open efuse device */
	fd = open(MISC_DEVICE, O_RDWR);
	if (fd < 0) {
		ALOGE("Open %s failed!\n", MISC_DEVICE);
		return -1;
	}
	ALOGD("open %s ok!\n", MISC_DEVICE);

	/* Get chip ID */
	ret = ioctl(fd, BUF_ORDER, id);
	if (ret < 0) {
		ALOGE("ioctl read %s failed!\n", MISC_DEVICE);
		close(fd);
		return -1;
	}

	memset(p_chip_id, 0, len_max);
	memcpy(p_chip_id, id, CHIP_ID_LEN);

	/* For debugging, print chip ID */
	ALOGD("id[8]: ");
	for (i = 0; i < CHIP_ID_LEN; i++)
		ALOGD(" %d ", id[i]);
	ALOGD("\n");

	close(fd);

	return 0;
}


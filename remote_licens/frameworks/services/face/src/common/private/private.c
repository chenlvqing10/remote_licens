/*
 * system_private.c - Get the private data from private partition
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
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/kernel.h>

#define PRIVATE_PARTITION "/dev/mmcblk0p3"

#define LOG_TAG "FACE_SVC_PRIVATE"
#include <log/log.h>

#include "private.h"

/**
 * @brief  face_svc_get_private_data
 * @note   read data from private parttion
 * @param  offset:  offset address
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int face_svc_get_private_data(int offset, char *data, int len)
{
	int fd, count;

	if (!data || offset < 0 || len <= 0)
		return -1;

	/* Open customize partition device */
	fd = open(PRIVATE_PARTITION, O_RDONLY);
	if (fd < 0) {
		ALOGE("Open %s failed!\n", PRIVATE_PARTITION);
		return -1;
	}

	count = lseek(fd, offset, SEEK_SET);
	if (count != offset) {
		ALOGE("seek %d failed!\n", offset);
		close(fd);
		return -1;
	}

	count = read(fd, data, len);
	if (count != len) {
		ALOGE("Failed to read, count = %d, require %d\n", count, len);
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

/**
 * @brief  face_svc_set_private_data
 * @note   write data to private parttion
 * @param  offset:
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int face_svc_set_private_data(int offset, char *data, int len)
{
	int fd, count;

	if (!data || offset < 0 || len <= 0)
		return -1;

	/* Open customize partition device */
	fd = open(PRIVATE_PARTITION, O_WRONLY);
	if (fd < 0) {
		ALOGE("Open %s failed!\n", PRIVATE_PARTITION);
		return -1;
	}

	count = lseek(fd, offset, SEEK_SET);
	if (count != offset) {
		ALOGE("seek %d failed!\n", offset);
		close(fd);
		return -1;
	}

	count = write(fd, data, len);
	if (count != len) {
		ALOGE("Failed to write, count = %d, require %d\n", count, len);
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}


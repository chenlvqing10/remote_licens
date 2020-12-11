/*
 * wiegand_read.c - define some function of operate wiegand
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define LOG_TAG "WG"
#include "log/log.h"

#include "wiegand_read.h"

#define WIEGAND_DEV	"/dev/wiegand"

static int fd;
static int read_enable;

int wg_read_init(void)
{
	fd = open(WIEGAND_DEV, O_RDWR);
	if (fd < 0) {
		ALOGE("open %s failed", WIEGAND_DEV);
		return -1;
	}

	read_enable = 1;

	return 0;
}

int wg_read_block(char *buf)
{
	int ret, count = 0;
	fd_set rd;
	struct timeval tv;

	if (fd < 0)
		return -1;

	FD_ZERO(&rd);
	FD_SET(fd, &rd);
	tv.tv_sec = 0;
	tv.tv_usec = 1000 * 1000;

	while (read_enable) {
		ret = select(fd + 1, &rd, NULL, NULL, &tv);
		if (ret == 0) /* time out */
			continue;
		else if (ret == -1)
			break;
		else {
			count = read(fd, buf, sizeof(buf));
			break;
		}
	}

	if (ret == -1)
		return -1;
	else
		return count;
}

int wg_read_deinit(void)
{
	read_enable = 0;

	close(fd);

	return 0;
}


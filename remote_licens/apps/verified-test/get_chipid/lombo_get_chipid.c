/*
 * lombo_get_chipid.c - main interface of getting chipid testcase.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/stat.h>

#define BUF_ORDER 0xefe00007

int main(void)
{
	int fd = 0;
	int ret = 0;
	unsigned char args[8] = { 0 };

	fd = open("/dev/lombo-misc", O_RDWR);
	if (fd < 0) {
		printf("error:open lombo-misc failed!\n");
		return ret;
	}
	printf("open /dev/lombo-misc!\n");

	ret = ioctl(fd, BUF_ORDER, args);
	if (ret < 0) {
		printf("error:ioctl read lombo-misc failed!\n");
		close(fd);
		return ret;
	}

	printf("buff[8]: ");
	for (unsigned char i = 0; i < 8; i++)
		printf("  %d ", args[i]);

	printf("\n");

	printf("----lombo-misc test ok!----\n");
	close(fd);

	return 0;
}

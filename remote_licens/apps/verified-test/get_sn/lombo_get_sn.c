/*
 * lombo_get_sn.c - main interface of getting sn testcase.
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

#define SECTOR_SIZE	0x200

int main(void)
{
	int fd = 0;
	int ret = 0;
	int i = 0;
	/* used to get the ID */
	unsigned char buf[SECTOR_SIZE] = {0};

	/* open the device */
	fd = open("/dev/mmcblk0p3", O_RDWR);
	if (fd < 0) {
		printf("error:open mmcblk0p3 fadfiled!\n");
		return ret;
	}
	printf("open /dev/mmcblk0p3!\n");

	/* get GUID data */
	read(fd, buf, sizeof(buf));
	/* print GUID */
	printf("GUID: ");
	for (i = 0; i < sizeof(buf); i++) {
		if (0 == buf[i])
			continue;
		if (0 == (i%16))
			printf("\n");
		printf("%x\t", buf[i]);
	}

	printf("\n");
	/* get WIFI data */
	lseek(fd, SECTOR_SIZE, SEEK_SET);
	read(fd, buf, sizeof(buf));
	/* print WIFI */
	printf("WIFI: ");
	for (i = 0; i < sizeof(buf); i++) {
		if (0 == buf[i])
			continue;
		if (0 == (i%16))
			printf("\n");
		printf("%x\t", buf[i]);
	}

	printf("\n");
	close(fd);

	return 0;
}

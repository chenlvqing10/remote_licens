/*
 * system_upgrade.c - Interface to upgrade
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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/stat.h>

/**
 * bcb_info_t: save upgrade information
 */
typedef struct tag_bcb_info {
	unsigned int magic; /* mgic: 0x20545055 */
	unsigned int cmd; /* command: 1 upgrade */
	unsigned int rsv1[6]; /* reserved 1 */
	unsigned int status; /* status: 0: upgrade */
	unsigned int rsv2[7];
} bcb_info_t;

/**
 * write_upgrade_flag_misc: write upgrade flag in /dev/mmcblk0p4
 * @NULL
 * Returns 0 means success, other means failure.
 */
int write_upgrade_flag_misc()
{
	int fd = 0;
	int ret = 0;
	int i = 0;
	unsigned char buf[4096] = {0};
	bcb_info_t bcb_info = {0};

	bcb_info.magic = 0x20545055;
	bcb_info.cmd = 0x1;
	bcb_info.status = 0x0;

	printf("open /dev/mmcblk0p4!\n");
	fd = open("/dev/mmcblk0p4", O_RDWR);
	if (fd < 0) {
		printf("error:open mmcblk0p4 fadfiled!\n");
		return ret;
	}
	printf("write /dev/mmcblk0p4!\n");

	lseek(fd, 0, SEEK_SET);
	write(fd, &bcb_info, sizeof(bcb_info_t));

	printf("read /dev/mmcblk0p4!\n");
	lseek(fd, 0, SEEK_SET);
	read(fd, buf, sizeof(buf));
	printf("mmcblk0p4: ");
	for (i = 0; i < sizeof(buf); i++) {
		if (0xcc == buf[i])
			continue;

		if (0 == (i%16))
			printf("\n");

		printf("%x\t", buf[i]);
	}

	printf("\n");
	close(fd);

	return 0;
}

/**
 * write_upgrade_flag_rtc： write upgrade flag in /dev/lombo-misc
 * @NULL
  * Returns 0 means success, other means failure.
 */
int write_upgrade_flag_rtc()
{
	int fd, ret;
	fd = open("/dev/lombo-misc", O_RDWR);
	if (fd < 0) {
		printf("error:open lombo-misc fadfiled!\n");
		return ret;
	}

	ret = ioctl(fd, 0xecd00001, 1);
	printf("ioctl(fd, 0xecd00001, 1) ret %d\n", ret);

	close(fd);

	return 0;
}


/*
 * ota_start.c - Updating firmware of device
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
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/kernel.h>

#define LOG_TAG		"UPGRADE"
#include <log/log.h>

#include "system_update.h"

#define MISC_PARTITION "/dev/mmcblk0p4"

#define UPGRADE_MAGIC_HEAD 0x20545055
#define CMD_UPGRADE 0x1
#define CMD_IOCTL_UPGRADE 0xecd00001

typedef struct tag_bcb_info {
	unsigned int magic;
	unsigned int cmd;
	unsigned int rsv1[6];
	unsigned int status;
	unsigned int rsv2[7];
} bcb_info_t;

static int write_upgrade_flag_misc()
{
	int fd, count;
	bcb_info_t bcb_info;

	bcb_info.magic = UPGRADE_MAGIC_HEAD;
	bcb_info.cmd = CMD_UPGRADE;
	bcb_info.status = 0x0;

	ALOGD("open %s!\n", MISC_PARTITION);
	fd = open(MISC_PARTITION, O_RDWR);
	if (fd < 0) {
		ALOGE("error:open %s fadfiled!\n", MISC_PARTITION);
		return -1;
	}

	lseek(fd, 0, SEEK_SET);
	count = write(fd, &bcb_info, sizeof(bcb_info_t));
	if (count != sizeof(bcb_info_t)) {
		ALOGE("write failed, count = %d", count);
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int write_upgrade_flag_rtc(void)
{
	int fd, ret;

	fd = open("/dev/lombo-misc", O_RDWR);
	if (fd < 0) {
		ALOGE("error:open lombo-misc fadfiled!\n");
		return -1;
	}

	ret = ioctl(fd, CMD_IOCTL_UPGRADE, 1);
	if (ret) {
		ALOGE("ioctl(fd, 0xecd00001, 1) ret %d\n", ret);
		close(fd);
		return ret;
	}

	close(fd);

	return 0;
}

/**
 * ota_start - upgrade device firmware.
 *
 * Returns 0  if success, other if failure.
 */
int ota_start(void)
{ 
	int ret;

	ret = write_upgrade_flag_misc();
	if (ret)
		return ret;
	ALOGD("reboot cause by OTA");
	/* system_reboot */
	system("reboot -d 1");

	return 0;
}


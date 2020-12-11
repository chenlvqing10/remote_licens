/*
 * fw_upgrade_main.c - main interface of the fw_upgrade testcase.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/stat.h>

#define printferror(error_message)	\
	printf("%s error : %s\n", __func__, error_message)

int main(int argc, char **argv)
{
	typedef struct tag_bcb_info {
		unsigned int magic;
		unsigned int cmd;
		unsigned int rsv1[6];
		unsigned int status;
		unsigned int rsv2[7];
	} bcb_info_t;

	int fd = 0;
	unsigned char buf[4096] = {0};
	bcb_info_t bcb_info = {0};

	/* initialization the struct of bcb_info_t */
	bcb_info.magic = 0x20545055;
	bcb_info.cmd = 0x1;
	bcb_info.status = 0x0;

	/* open this node(misc partition) readable and writable */
	fd = open("/dev/mmcblk0p4", O_RDWR);

	/* read and write position points to the fileheader */
	lseek(fd, 0, SEEK_SET);
	/* write sizeof(bcb_info_t) to the address of bcb_info */
	write(fd, &bcb_info, sizeof(bcb_info_t));

	/* read and write position points to the fileheader */
	lseek(fd, 0, SEEK_SET);
	/* read size of buf to buf */
	read(fd, buf, sizeof(buf));

	/* close this node */
	close(fd);
	system("reboot");
	return 0;
}


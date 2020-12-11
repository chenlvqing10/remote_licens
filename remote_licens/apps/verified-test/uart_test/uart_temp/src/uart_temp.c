/*
 * uart_temp.c
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <log/log.h>

#define UART_DEVICE     "/dev/ttySLB2"
#define READ_BUFFER_SIZE 8192

int hal_uart_temp_buf(char *read_buf)
{
	int count;
	int fd;
	struct termios oldtio, newtio;
	char cmd_buf[16];

	memset(cmd_buf, 0, sizeof(cmd_buf));
	sprintf(cmd_buf, "VCMD=DAT");

	/* Open uart device */
	fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		ALOGE("Open %s failed\n", UART_DEVICE);
		return -1;
	}

	/* Get the parameters of the current operating mode */
	tcgetattr(fd, &oldtio);
	memset(&newtio, 0, sizeof(newtio));

	/* Baud rate=115200, data bit=8 . Enable data reception */
	newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD | CSTOPB;
	newtio.c_iflag = IGNPAR;

	/* Clear input buffer and output buffer */
	tcflush(fd, TCIFLUSH);
	/* Set new operating parameters */
	tcsetattr(fd, TCSANOW, &newtio);

	if (strlen(cmd_buf) == 5) {
		strcat(cmd_buf, "   ");
	} else if (strlen(cmd_buf) == 6) {
		strcat(cmd_buf, "  ");
	} else if (strlen(cmd_buf) == 7) {
		strcat(cmd_buf, " ");
	}

	/* Send data to uart */
	count = write(fd, cmd_buf, 8);
	if (count != 8) {
		ALOGE("send failed\n");
		return -1;
	}

	usleep(400000);

	count = read(fd, read_buf, READ_BUFFER_SIZE);
	if (count <= 0)
		ALOGE("read error!\n");

	/*printf("count:%d, len:%d, recv:%s\n", count, strlen(read_buf), read_buf);*/

	close(fd);
	/* Restore original settings */
	tcsetattr(fd, TCSANOW, &oldtio);

	return 0;
}

float hal_uart_get_max_temp()
{
	char read_temp_buf[READ_BUFFER_SIZE];
	char *token;
	int max_temp = 0;
	float return_temp = 0.0;
	int count = 0;

	memset(read_temp_buf, 0, READ_BUFFER_SIZE);

	hal_uart_temp_buf(read_temp_buf);

	token = strtok(read_temp_buf, " ");

	while(token != NULL){
		if (strspn(token, "0123456789abcdefABCDEF") == strlen(token)){
			char dig_str[16];
			sprintf(dig_str, "0x%s", token);
			int iTemp = strtol(dig_str, NULL, 16);
			if (iTemp > max_temp)
				max_temp = iTemp;

			count++;
		}

		token = strtok(NULL, " ");
	}

	return_temp = max_temp / 10.0;
	ALOGD("%s %d return_temp %f court is %d\n", __func__, __LINE__, return_temp, count);
	return return_temp;
}


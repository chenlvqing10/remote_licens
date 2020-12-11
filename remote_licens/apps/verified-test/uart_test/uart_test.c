/*
 * uart_test.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int fd;

static void *read_thread(void *args)
{
	int count;
	char buf[256];

	while (1) {
		memset(buf, 0, sizeof(buf));
		count = read(fd, buf, 256);
		if (count > 0)
			printf("%s", buf);

	}

	return NULL;
}

void help_info(void)
{
	printf("Usage: uarttest <uartid> <baudrate>\n");
	printf("    uartid: uart id, from 0 start\n");
	printf("    baudrate: uart baud rate\n");
}

int main(int argc, char *argv[])
{
	int ret;
	struct termios oldtio, newtio;
	char uart_name[64];
	char buf[256] = "uart send to pc";
	char newline[3] = {0x0D, 0x0A}; /* description line */
	pthread_t read_thread_id;
	int uart_idx;
	int speed;

	if (argc < 3) {
		help_info();
		return -1;
	}

	uart_idx = atoi(argv[1]);
	speed = atoi(argv[2]);

	printf("target uart id: %d, baud rate: %d\n", uart_idx, speed);

	sprintf(uart_name, "/dev/ttySLB%d", uart_idx);

	/* open uart device file */
	fd = open(uart_name, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		printf("Open %s failed\n", uart_name);
		return -1;
	}
	else
		printf("Open %s successfully\n", uart_name);

	/* Set operating parameters */
	tcgetattr(fd, &oldtio);
	memset(&newtio, 0, sizeof(newtio));

	/* Set the baund rate */
	switch (speed) {
	case 2400:
		newtio.c_cflag = B2400;
		break;
	case 4800:
		newtio.c_cflag = B4800;
		break;
	case 9600:
		newtio.c_cflag = B9600;
		break;
	case 19200:
		newtio.c_cflag = B19200;
		break;
	case 38400:
		newtio.c_cflag = B38400;
		break;
	case 57600:
		newtio.c_cflag = B57600;
		break;
	case 115200:
		newtio.c_cflag = B115200;
		break;
	case 230400:
		newtio.c_cflag = B230400;
		break;
	case 460800:
		newtio.c_cflag = B460800;
		break;
	case 500000:
		newtio.c_cflag = B500000;
		break;
	case 576000:
		newtio.c_cflag = B576000;
		break;
	case 921600:
		newtio.c_cflag = B921600;
		break;
	case 1000000:
		newtio.c_cflag = B1000000;
		break;
	case 1152000:
		newtio.c_cflag = B1152000;
		break;
	case 1500000:
		newtio.c_cflag = B1500000;
		break;
	case 2000000:
		newtio.c_cflag = B2000000;
		break;
	case 2500000:
		newtio.c_cflag = B2500000;
		break;
	case 3000000:
		newtio.c_cflag = B3000000;
		break;
	case 3500000:
		newtio.c_cflag = B3500000;
		break;
	case 4000000:
		newtio.c_cflag = B4000000;
		break;
	default:
		newtio.c_cflag = B115200;
		break;
	}

	/* Data bit = 8, enable data recepation */
	newtio.c_cflag |= CS8 | CLOCAL | CREAD | CSTOPB | CRTSCTS;
	newtio.c_iflag = IGNPAR;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	pthread_create(&read_thread_id, NULL, read_thread, NULL);

	while (1) {
		ret = write(fd, buf, 16);
		ret = write(fd, newline, 3);
		if (ret > 0)
			printf("uart send data successfully\n");
		else {
			printf("uart faile send data\n");
			exit(1);
		}

		usleep(100000);
	}

	close(fd);

	return 0;
}


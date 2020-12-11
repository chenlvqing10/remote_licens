/*
 *test_ir_led.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void user_guide(void)
{
	printf("Press 'test_ir_led -R n' to set value(for ir_led) to n\n");
}

int main(int argc, char *argv[])
{
	int fd, fd_duty, fd_enable, fd_period;
	unsigned int duty, period;
	int ret, n;
	char buf[8] = "00000000";

	if ((argc < 3) || (strcmp(argv[1], "-R") != 0)) {
		user_guide();
		return -1;
	}

	duty = 5000;
	n = atoi(argv[2]);
	if (n == 0) {
		duty = 0;
		period = 0;
	} else if (n > 255) {
		printf("the max value is 255\n");
		return -1;
	} else {
		period = duty * 255 / n;
		sprintf(buf, "%d", period);
	}

	fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
	if (fd < 0) {
		printf("open export failed\n");
		return -1;
	}

	ret = write(fd, "0", strlen("0"));
	if (ret < 0) {
		printf("open pwm0 failed\n");
		return -1;
	} else
		printf("open pwm0 success\n");

	fd_period = open("/sys/class/pwm/pwmchip0/pwm0/period", O_RDWR);
	fd_duty = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_RDWR);
	fd_enable = open("/sys/class/pwm/pwmchip0/pwm0/enable", O_RDWR);
	if ((fd_period < 0) || (fd_duty < 0) || (fd_enable < 0)) {
		printf("open error\n");
		return -1;
	}

	ret = write(fd_enable, "1", strlen("1"));
	if (ret < 0) {
		printf("enable pwm0 failed\n");
		return -1;
	}

	write(fd_duty, "5000", sizeof(duty));
	if (ret < 0) {
		printf("set duty_cycle to 5000ns failed\n");
	}

	ret = write(fd_period, buf, sizeof(buf));
	if (ret < 0) {
		printf("set pwm0 period failed\n");
	}

	printf("set value to %d success.\n", n);
	close(fd);
	fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
	write(fd, "0", strlen("0"));
	close(fd);

	return 0;
}

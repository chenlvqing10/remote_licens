/*
 * gpio_test.c
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define SYSFS_GPIO_DIR		"/sys/class/gpio"
#define MAX_BUF			64

/* export gpio sysfs */
static int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	/* write only open */
	fd = open(SYSFS_GPIO_DIR"/export", O_WRONLY);
	if (fd < 0)
		printf("gpio export error!\n");

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

/* unexport gpio sysfs */
static int gpio_unexport(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	/* write only open */
	fd = open(SYSFS_GPIO_DIR"/unexport", O_WRONLY);
	if (fd < 0)
		printf("gpio unexport error!\n");

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

/* set gpio direction */
static int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/direction", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("gpio set dir error!\n");
		return fd;
	}

	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
	close(fd);

	return 0;
}

/* set gpio value */
static int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("gpio set value error!\n");
		return fd;
	}

	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
	close(fd);

	return 0;
}

/* get gpio value */
static int gpio_get_value(unsigned int gpio, unsigned int *value)
{
	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		printf("gpio get value error!\n");
		return fd;
	}

	read(fd, &ch, 1);

	if (ch != '0')
		*value = 1;
	else
		*value = 0;
	close(fd);

	return 0;
}

int main(int argc, char **argv)
{
	unsigned int value = 0;
	int num[] = {12, 37, 94};  /* A12, B21, SIO6 */
	int num1[] = {13, 36, 95};  /* A13, B20, SIO7 */
	int i;

	printf("gpio test start!\n");
	for (i = 0; i < 3; i++) {
		gpio_export(num[i]);
		gpio_set_dir(num[i], 1);  /* direction is output */
		gpio_set_value(num[i], 1);  /* set output value */
		gpio_get_value(num[i], &value);
		printf("gpio_get_value:%d!\n", value);
	}
	gpio_unexport(12);
	printf("\n");
	for (i = 0; i < 3; i++) {
		gpio_export(num1[i]);
		gpio_set_dir(num1[i], 0);  /* direction is input */
		gpio_get_value(num1[i], &value);  /* the value as input is variable */
		printf("gpio_get_value:%d!\n", value);
	}
	gpio_unexport(13);
	printf("gpio test end!\n");

	return 0;
}


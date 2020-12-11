/*
 * gpio.c - define some function of operate fill light
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
#include <string.h>

#define LOG_TAG "gpio"
#include <log/log.h>

#define GPIO_EXPORT	"/sys/class/gpio/export"
#define GPIO_UNEXPORT	"/sys/class/gpio/unexport"
#define GPIO_DIRECTION	"/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE	"/sys/class/gpio/gpio%d/value"

#define GPIO_DIR_OUT	"out"
#define GPIO_DIR_IN	"in"

int gpio_set_output_level(int gpio, int level)
{
	char str[64];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio direction */
	snprintf(str, sizeof(str), GPIO_DIRECTION, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	fprintf(p, "%s", GPIO_DIR_OUT);
	fclose(p);

	/* set gpio value */
	snprintf(str, sizeof(str), GPIO_VALUE, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (level)
		fprintf(p, "%d", 1);
	else
		fprintf(p, "%d", 0);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	return 0;
}

int gpio_set_direction(int gpio, int direction)
{
	char str[64];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio direction */
	snprintf(str, sizeof(str), GPIO_DIRECTION, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (direction)
		fprintf(p, "%s", GPIO_DIR_IN);
	else
		fprintf(p, "%s", GPIO_DIR_OUT);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	return 0;
}

int gpio_set_level(int gpio, int level)
{
	char str[64];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio value */
	snprintf(str, sizeof(str), GPIO_VALUE, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (level)
		fprintf(p, "%d", 1);
	else
		fprintf(p, "%d", 0);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	return 0;
}


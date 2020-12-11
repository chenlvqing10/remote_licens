/*
 * camera_led_ctrl.c
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

#define LOG_TAG "camera_svc"
#include <log/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "camera_led_ctrl.h"

#define GPIO_EXPORT		"/sys/class/gpio/export"
#define GPIO_UNEXPORT		"/sys/class/gpio/unexport"
#define NIR_LED_GPIO		60
#define NIR_LED_DIRECTION	"/sys/class/gpio/gpio60/direction"
#define NIR_LED_VALUE		"/sys/class/gpio/gpio60/value"

static int nir_led_ctrl(int on)
{
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", NIR_LED_GPIO);
	fclose(p);

	/* set gpio direction */
	p = fopen(NIR_LED_DIRECTION, "w");
	if (!p) {
		ALOGE("open %s failed", NIR_LED_DIRECTION);
		return -1;
	}
	fprintf(p, "%s", "out");
	fclose(p);

	/* set gpio value */
	p = fopen(NIR_LED_VALUE, "w");
	if (!p) {
		ALOGE("open %s failed", NIR_LED_VALUE);
		return -1;
	}
	if (on)
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
	fprintf(p, "%d", NIR_LED_GPIO);
	fclose(p);

	return 0;
}

int nir_led_on(void)
{
	return nir_led_ctrl(1);
}

int nir_led_off(void)
{
	return nir_led_ctrl(0);
}

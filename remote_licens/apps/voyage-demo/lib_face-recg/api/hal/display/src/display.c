/*
 * display.c -  display some function of operate display
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "display.h"
#include "db_device_hw_param.h"

#define LOG_TAG "hal-disp"
#include <log/log.h>

#define BRIGHTNESS_CAN_BE_SEEN	50

/* Device file to adjust screen brightness */
#define SCREEN_BRIGHTNESS_DEVICE \
		"/sys/devices/panel_g02.4/backlight/pwm-backlight/brightness"

/* screen switch */
#define SCREEN_SWITCH_DEVICE \
		"/sys/class/graphics/fb0/blank"

static saver_open_cb saver_open_notify_cb;
static saver_close_cb saver_close_notify_cb;

static int screen_control(int off)
{
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(SCREEN_SWITCH_DEVICE, "w");
	if (!p) {
		ALOGE("open %s failed", SCREEN_SWITCH_DEVICE);
		return -1;
	}
	fprintf(p, "%d", off);
	fclose(p);

	return 0;
}

int display_screen_off(void)
{
	return screen_control(1);
}

int display_screen_on(void)
{
	return screen_control(0);
}

int saver_open_set_cb(saver_open_cb cb)
{
	saver_open_notify_cb = cb;

	return 0;
}

int saver_close_set_cb(saver_close_cb cb)
{
	saver_close_notify_cb = cb;

	return 0;
}

int display_screen_saver_on(void)
{
	int ret = -1;

	if (saver_open_notify_cb)
		ret = saver_open_notify_cb();
	if (ret != 0)
		ALOGE("screen saver notify callback fail");

	return ret;
}

int display_screen_saver_off(void)
{
	int ret = 0;

	if (saver_close_notify_cb)
		saver_close_notify_cb();
	if (ret != 0)
		ALOGE("screen saver notify callback fail");

	return ret;
}

/**
 * display_get_bright_from_device - Get screen brightness from device
 * @brightness: OUT, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int display_get_bright_from_device(int *brightness)
{
	int ret = -1;
	FILE *fp = NULL;
	char buf[64];
	char *p = NULL;

	assert(brightness != NULL);

	fp = fopen(SCREEN_BRIGHTNESS_DEVICE, "r");
	if (fp) {
		memset(buf, 0 , sizeof(buf));
		p = fgets(buf, sizeof(buf), fp);
		if (p) {
			*brightness = atoi(p);
			ret = 0;
		}

		fclose(fp);
	}

	return ret;
}

int screen_change_display_value_to_real(int display_val, int *real_val)
{
	float tmp_val = 0.0;

	assert(real_val != NULL);

	tmp_val = BRIGHTNESS_CAN_BE_SEEN + display_val *
		((float)(BRIGHTNESS_MAX - BRIGHTNESS_CAN_BE_SEEN) /
		(float)BRIGHTNESS_MAX);

	/* Convert float type to int */
	*real_val = (int)tmp_val;

	ALOGD("display_val=%d, tmp_val=%f, real_val=%d\n", display_val, tmp_val,
		*real_val);

	return 0;
}

/**
 * display_set_bright_to_device - Set screen brightness to device
 * @brightness: IN, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int display_set_bright_to_device(int brightness)
{
	int ret = -1;
	FILE *fp = NULL;
	int s = 0;
	char buf[8];
	int real_val = 0;

	screen_change_display_value_to_real(brightness, &real_val);

	fp = fopen(SCREEN_BRIGHTNESS_DEVICE, "w");
	if (fp) {
		memset(buf, 0 , sizeof(buf));
		snprintf(buf, sizeof(buf), "%d", real_val);
		s = fputs((const char *)buf, fp);
		if (s != EOF)
			ret = 0;

		fclose(fp);
	}

	return ret;
}


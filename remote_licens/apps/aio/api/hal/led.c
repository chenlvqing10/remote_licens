/*
 * led.c - Define function for operate led
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define LOG_TAG "system_led"
#include <log/log.h>

#include "led.h"

/* device name of infrared light */
#define IR_DEV_NAME \
	"/sys/class/leds/infrared_light/brightness"

/* device name of white light */
#define LIGHT_DEV_NAME \
	"/sys/class/leds/white_light/brightness"

/*
* led_ctrl - set led status
* @node: device node name
* @level: led status
*
* return 0 if success, otherwise return -1
*/
static int led_ctrl(char *node, LED_STATE state)
{
	FILE *p = NULL;

	if (NULL == node) {
		ALOGE("device node is NULL");
		return -2;
	}
	p = fopen(node, "w");
	if (!p) {
		ALOGE("open %s failed", node);
		return -1;
	}

	if (state == LED_OFF)
		fprintf(p, "%d", 0);
	else
		fprintf(p, "%d", 1);

	fclose(p);

	return 0;
}

/*
* infrared_led_ctrl - set infrared led status
* @state: led status
*
* return 0 if success, otherwise return -1
*/
int infrared_led_ctrl(LED_STATE state)
{
	return led_ctrl(IR_DEV_NAME, state);
}

/*
* white_led_ctrl - set light led status
* @state: led status
*
* return 0 if success, otherwise return -1
*/
int white_led_ctrl(LED_STATE state)
{
	return led_ctrl(LIGHT_DEV_NAME, state);
}


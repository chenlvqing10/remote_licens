/*
 * relay.c - Define function for operate relay
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

#define LOG_TAG "system_relay"
#include <log/log.h>

#include "relay.h"

/* device name of relay */
#define RELAY_DEV_NAME \
	"/sys/class/leds/relay/brightness"

/*
* relay_gpio_ctrl - set relay gpio status
* @node: device node name
* @state: relay status
*
* return 0 if success, otherwise return -1
*/
static int relay_gpio_ctrl(char *node, RELAY_STATE state)
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

	if (state == RELAY_OFF)
		fprintf(p, "%d", 0);
	else
		fprintf(p, "%d", 1);

	fclose(p);

	return 0;
}

/*
* relay_ctrl - set relay status
* @state: relay status
*
* return 0 if success, otherwise return -1
*/
int relay_ctrl(RELAY_STATE state)
{
	return relay_gpio_ctrl(RELAY_DEV_NAME, state);
}


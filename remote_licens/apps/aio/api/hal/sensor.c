/*
 * sensor.c - Define function for operate sensor
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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#define LOG_TAG "system_sensor"
#include <log/log.h>

#include "sensor.h"

#define ILLUMIN_DEV	"/sys/bus/iio/devices/iio:device0/in_illuminance_raw"

#define PROXIMITY_DEV	"/sys/bus/iio/devices/iio:device0/in_proximity_raw"

/**
* sensor_get_proximity - get proximity from sensor
* @proximity: A pointer of proximity value
*
* return 0 if success, otherwise return -1
*/
int sensor_get_proximity(int *proximity)
{
	int ret = -1;
	FILE *fp = NULL;
	char buf[64];
	char *p = NULL;

	assert(proximity != NULL);

	fp = fopen(PROXIMITY_DEV, "r");
	if (fp) {
		memset(buf, 0 , sizeof(buf));
		p = fgets(buf, sizeof(buf), fp);
		if (p) {
			/*todo:convert unit to cm*/
			*proximity = atoi(p);
			ret = 0;
		}

		fclose(fp);
	}

	return ret;
}

/**
* sensor_get_illuminance - get illuminance from sensor
* @illuminance: A pointer of illuminance value
*
* return 0 if success, otherwise return -1
*/
int sensor_get_illuminance(int *illuminance)
{
	int ret = -1;
	FILE *fp = NULL;
	char buf[64];
	char *p = NULL;

	assert(illuminance != NULL);

	fp = fopen(ILLUMIN_DEV, "r");
	if (fp) {
		memset(buf, 0 , sizeof(buf));
		p = fgets(buf, sizeof(buf), fp);
		if (p) {
			*illuminance = atoi(p);
			ret = 0;
		}
		fclose(fp);
	}
	return ret;
}


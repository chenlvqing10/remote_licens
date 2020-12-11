/*
 * lombo_thermal.h - Lombo Thermal Management Implementation
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _LOMBO_THERMAL_H_
#define _LOMBO_THERMAL_H_

#include <linux/thermal.h>

#define COOLING_DEV_MAX 8

struct lombo_trip_point {
	int temp;
	enum thermal_trip_type type;
	char cdev_name[COOLING_DEV_MAX][THERMAL_NAME_LENGTH];
};

struct lombo_thsens_platform_data {
	struct lombo_trip_point trip_points[THERMAL_MAX_TRIPS];
	int num_trips;
};

#endif /* _LOMBO_THERMAL_H_ */

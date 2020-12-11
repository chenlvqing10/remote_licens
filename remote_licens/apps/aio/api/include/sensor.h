/*
 * sensor.h - Define function for operate sensor
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

#ifndef _SENSOR_H
#define _SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
* sensor_get_proximity - get proximity from sensor
* @proximity: A pointer of proximity value
*
* return 0 if success, otherwise return -1
*/
int sensor_get_proximity(int *proximity);

/**
* sensor_get_illuminance - get illuminance from sensor
* @illuminance: A pointer of illuminance value
*
* return 0 if success, otherwise return -1
*/
int sensor_get_illuminance(int *illuminance);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SENSOR_H */


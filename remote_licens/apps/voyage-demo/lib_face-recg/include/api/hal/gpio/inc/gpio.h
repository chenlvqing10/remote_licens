/*
 * gpio.h - export some function of operate fill light
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

#ifndef _GPIO_H
#define _GPIO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
* gpio_set_ouput_level - set output level of gpio(gpio direction will be set to out)
* @gpio gpio number
* @level gpio output level
* return 0 if success, otherwise return -1
*/
int gpio_set_output_level(int gpio, int level);

/*
* gpio_set_direction - set direction of gpio
* @gpio gpio number
* @level gpio direction
* return 0 if success, otherwise return -1
*/
int gpio_set_direction(int gpio, int direction);

/*
* gpio_set_level - set level of gpio
* @gpio gpio number
* @level gpio level
* return 0 if success, otherwise return -1
*/
int gpio_set_level(int gpio, int level);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


/*
 * led.h - Define function for operate led
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

#ifndef _LED_H
#define _LED_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* status of led */
typedef enum _LED_STATE {
	LED_OFF = 0,
	LED_ON,
} LED_STATE;

/*
* infrared_led_ctrl - set infrared led status
* @state: led status
*
* return 0 if success, otherwise return -1
*/
int infrared_led_ctrl(LED_STATE state);

/*
* white_led_ctrl - set light led status
* @state: led status
*
* return 0 if success, otherwise return -1
*/
int white_led_ctrl(LED_STATE state);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _LED_H */


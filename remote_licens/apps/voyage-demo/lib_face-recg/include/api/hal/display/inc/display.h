/*
 * display.h - export some function of operate display
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

#ifndef _DISPLAY_H
#define _DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define BRIGHTNESS_MAX 100
#define BRIGHTNESS_MIN 0

typedef int (*saver_open_cb)(void);
typedef int (*saver_close_cb)(void);

/**
 * screen saver activity open callback function
 * @param p a pointer of callback
 * @return NULL
 */
int saver_open_set_cb(saver_open_cb cb);

/**
 * screen saver activity close callback function
 * @param p a pointer of callback
 * @return NULL
 */
int saver_close_set_cb(saver_close_cb cb);

/**
 * display_screen_on - turn on the screen.
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_on(void);

/**
 * display_screen_off - turn off the screen.
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_off(void);

/**
 * display_screen_saver_on - turn on the screen saver.
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_saver_on(void);

/**
 * display_screen_saver_off - turn off the screen saver.
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_saver_off(void);

/**
 * display_get_bright_from_device - Get screen brightness from device
 * @brightness: OUT, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int display_get_bright_from_device(int *brightness);

/**
 * display_set_bright_to_device - Set screen brightness to device
 * @brightness: IN, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int display_set_bright_to_device(int brightness);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


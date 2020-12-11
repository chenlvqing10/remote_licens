/*
 * screen_svc.h - Interface to adjust screen brightness
 * information from Excel.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#ifndef _SCREEN_SVC_H
#define _SCREEN_SVC_H

#include <pthread.h>
#include "ee_list.h"

typedef int (*saver_open_cb)(void);

typedef int (*saver_close_cb)(void);

typedef struct _auto_screen {
	int enable;
	int delay_count;
	int delay_time;
	int thread_running;
	int countdown_running;
	pthread_t auto_thread;
	pthread_cond_t screen_cond;
	pthread_mutex_t screen_lock;
} auto_screen_t;

typedef struct _screen_saver {
	int saver_enable;
	int saver_delay_count;
	int saver_delay_time;
	int saver_thread_running;
	int saver_countdown_running;
	int saver_count;
	int saver_time;
	int saver_index;
	struct list_head *current_node;
	pthread_t saver_thread;
	pthread_cond_t screen_saver_cond;
	pthread_mutex_t screen_saver_lock;
} screen_saver_t;

typedef struct tag_screen_bright {
	/* Automatically adjust screen brightness control
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char auto_display_brightness;

	/* Set the screen brightness value (range 0~100).
	 * When auto_display_brightness function is enabled, Current setting
	 * no effect.
	 */
	char display_brightness;
} screen_bright_t;

typedef struct tag_screen_off_status {
	/* Automatic screen off function
	 * Options:
	 *	0: Disabled
	 *	1: Enabled
	 */
	int auto_screen_off;

	/* Auto-out timeout */
	int auto_screen_off_timeout;

	/* screen saver function
	 * Options:
	 *	0: Disabled
	 *	1: Enabled
	 */
	int screen_saver_off;

	/* screen saver timeout */
	int screen_saver_off_timeout;
} screen_off_status_t;

/**
 * screen_get_bright - Get screen brightness
 * @st_bright: OUT, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_get_bright(screen_bright_t *st_bright);

/**
 * system_screen_set_bright - Set screen brightness
 * @st_bright: IN, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_set_bright(screen_bright_t bright);

/**
 * screen_set_bright - Init display parameters
 * @p_bright: A pointer of screen_bright_t
 * @p_status: A pointer of screen_off_status_t
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_svc_init(screen_bright_t *p_bright,
						screen_off_status_t *p_status);

/**
 * screen_svc_reset - reset timeout count of screen auto off
 * None
 */
void screen_svc_deinit(void);

/**
 * screen_count_reset - reset timeout count of screen auto off
 * None
 */
void screen_count_reset(void);

/**
 * screen_saver_count_reset - reset timeout count of screen saver off
 * None
 */
void screen_saver_count_reset(void);

/**
 * screen_get_bright - Get screen brightness
 * @st_auto: OUT, screen off status
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_get_auto_off_status(screen_off_status_t *st_auto);

/**
 * screen_set_auto_off_status - Set automatic screen off status
 * @st_bright: IN, screen off status
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_set_auto_off_status(screen_off_status_t *st_auto);

/**
 * screen saver activity open callback function
 * @param p a pointer of callback
 *
 * Returns 0 if success, otherwise -1.
 */
int saver_open_set_cb(saver_open_cb cb);

/**
 * screen saver activity close callback function
 * @param p a pointer of callback
 *
 * Returns 0 if success, otherwise -1.
 */
int saver_close_set_cb(saver_close_cb cb);

/**
 * display screen saver
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_saver_on(void);

/**
 * close screen saver
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_saver_off(void);

/**
 * set auto_screen_off svc timeout
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_set_auto_screen_off_timeout(int auto_screen_off_timeout);

/**
 * set screen_saver_off svc timeout
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_set_screen_saver_off_timeout(int screen_saver_off_timeout);


#endif /* _SCREEN_SVC_H */


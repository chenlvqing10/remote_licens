/*
 * system_screen.c - Interface to adjust screen brightness
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "sqlite3.h"
#include "display.h"
#include "db_device_hw_param.h"
#include "system_screen.h"
#include "config.h"

#define LOG_TAG "hal-screen"
#include <log/log.h>

enum screen_state {
	SCREEN_ON,
	SCREEN_OFF,
	SCREEN_SAVER
};

enum screen_state state;
static auto_screen_t auto_screen;
static screen_saver_t screen_saver;

static void *auto_screen_thread(void *args)
{
	auto_screen_t *screen = (auto_screen_t *)args;

	while (screen->thread_running) {
		while (screen->countdown_running) {
			usleep(1000000);
			pthread_mutex_lock(&screen->screen_lock);
			if (--screen->delay_count == 0) {
				screen->countdown_running = 0;
				if (state == SCREEN_SAVER)
					display_screen_saver_off();
				display_screen_off();
				state = SCREEN_OFF;
			}
			pthread_mutex_unlock(&screen->screen_lock);
		}
		pthread_mutex_lock(&screen->screen_lock);
		pthread_cond_wait(&screen->screen_cond, &screen->screen_lock);
		pthread_mutex_unlock(&screen->screen_lock);
		screen->countdown_running = 1;
	}

	return NULL;
}

static void *screen_saver_thread(void *args)
{
	screen_saver_t *screen = (screen_saver_t *)args;

	while (screen->saver_thread_running) {
		while (screen->saver_countdown_running) {
			usleep(1000000);
			pthread_mutex_lock(&screen->screen_saver_lock);
			if (--screen->saver_delay_count == 0) {
				screen->saver_countdown_running = 0;
				if (state != SCREEN_OFF) {
					display_screen_saver_on();
					state = SCREEN_SAVER;
				}
			}
			pthread_mutex_unlock(&screen->screen_saver_lock);
		}
		pthread_mutex_lock(&screen->screen_saver_lock);
		pthread_cond_wait(&screen->screen_saver_cond,
				&screen->screen_saver_lock);
		pthread_mutex_unlock(&screen->screen_saver_lock);
		screen->saver_countdown_running = 1;
	}

	return NULL;
}

void screen_count_reset(void)
{
	screen_bright_t st_bright;
	auto_screen_t *screen = &auto_screen;

	pthread_mutex_lock(&screen->screen_lock);
	if (state == SCREEN_OFF) {
		display_screen_on();
		system_screen_get_bright(&st_bright);
		system_screen_set_bright(st_bright);
		state = SCREEN_ON;
	}
	if (screen->countdown_running == 0)
		pthread_cond_signal(&screen->screen_cond);
	screen->delay_count = screen->delay_time;
	pthread_mutex_unlock(&screen->screen_lock);
}

void screen_saver_count_reset(void)
{
	screen_bright_t st_bright;
	screen_saver_t *screen = &screen_saver;

	pthread_mutex_lock(&screen->screen_saver_lock);
	if (state == SCREEN_SAVER) {
		display_screen_saver_off();
		system_screen_get_bright(&st_bright);
		system_screen_set_bright(st_bright);
		state = SCREEN_ON;
	}
	if (screen->saver_countdown_running == 0)
		pthread_cond_signal(&screen->screen_saver_cond);
	screen->saver_delay_count = screen->saver_delay_time;
	pthread_mutex_unlock(&screen->screen_saver_lock);
}

static int auto_screen_init(void)
{
	int ret;
	int cout = 0;
	device_hw_param_t info;

	auto_screen_t *screen = &auto_screen;
	memset(screen, 0, sizeof(auto_screen_t));

	pthread_mutex_init(&(screen->screen_lock), NULL);
	pthread_cond_init(&(screen->screen_cond), NULL);

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		screen->delay_time = info.auto_screen_off_timeout;
	}
	screen->delay_count = screen->delay_time;
	state = SCREEN_ON;
	screen->countdown_running = 1;
	screen->thread_running = 1;
	ret = pthread_create(&screen->auto_thread, NULL, auto_screen_thread, screen);
	if (ret) {
		ALOGE("create auto screen thread failed");
		return -1;
	}
	screen->enable = 1;

	return 0;
}

static int screen_saver_init(void)
{
	int ret;
	int cout = 0;
	device_hw_param_t info;

	screen_saver_t *screen = &screen_saver;
	memset(screen, 0, sizeof(screen_saver_t));

	pthread_mutex_init(&(screen->screen_saver_lock), NULL);
	pthread_cond_init(&(screen->screen_saver_cond), NULL);

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		screen->saver_delay_time = info.screen_saver_off_timeout;
	}
	screen->saver_delay_count = screen->saver_delay_time;
	state = SCREEN_ON;
	screen->saver_countdown_running = 1;
	screen->saver_thread_running = 1;
	ret = pthread_create(&screen->saver_thread, NULL,
			screen_saver_thread, screen);
	if (ret) {
		ALOGE("create auto screen thread failed");
		return -1;
	}
	screen->saver_enable = 1;

	return 0;
}

int get_screen_saver_config(screen_saver_t *saver)
{
	int ret = -1;
	int cout = 0;
	device_hw_param_t info;

	assert(saver != NULL);

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		saver->saver_count = info.screen_saver_count;
		saver->saver_time = info.screen_saver_time;
	}

	return ret;
}

void auto_screen_deinit(void)
{
	auto_screen_t *screen = &auto_screen;

	screen->enable = 0;
	screen->thread_running = 0;
	screen->countdown_running = 0;
	pthread_join(screen->auto_thread, NULL);
}

/**
 * system_screen_get_bright - Get screen brightness
 * @st_auto: OUT, screen off status
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_get_auto_off_status(screen_off_status_t *st_auto)
{
	int ret = -1;
	int cout = 0;
	device_hw_param_t info;

	assert(st_auto != NULL);

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		st_auto->auto_screen_off = info.auto_screen_off;
		st_auto->auto_screen_off_timeout = info.auto_screen_off_timeout;
		st_auto->screen_saver_off = info.screen_saver_off;
		st_auto->screen_saver_off_timeout =
			info.screen_saver_off_timeout;
	}

	return ret;
}

/**
 * system_screen_set_auto_off_status - Set automatic screen off status
 * @st_bright: IN, screen off status
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_set_auto_off_status(screen_off_status_t *st_auto)
{
	int ret = -1;
	int cout = 0;
	int sql_ret = SQLITE_OK;
	device_hw_param_t info;

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		info.auto_screen_off = st_auto->auto_screen_off;
		info.auto_screen_off_timeout = st_auto->auto_screen_off_timeout;
		sql_ret = db_device_hw_param_update(&info);
		if (SQLITE_OK == sql_ret)
			ret = 0;
		else {
			ret = -1;
			ALOGE("[%s:%d] Update auto screen off fail\n", __func__,
				__LINE__);
		}
	}

	return ret;
}

/**
 * system_screen_get_bright - Get screen brightness
 * @st_bright: OUT, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_get_bright(screen_bright_t *st_bright)
{
	int ret = -1;
	int cout = 0;
	device_hw_param_t info;

	assert(st_bright != NULL);

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		st_bright->auto_display_brightness = info.auto_display_brightness;
		st_bright->display_brightness = info.display_brightness;
	}

	return ret;
}

/**
 * system_screen_set_bright - Set screen brightness
 * @st_bright: IN, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_set_bright(screen_bright_t st_bright)
{
	int ret = -1;
	int cout = 0;
	int sql_ret = SQLITE_OK;
	device_hw_param_t info;

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		info.auto_display_brightness = st_bright.auto_display_brightness;
		info.display_brightness = st_bright.display_brightness;
		sql_ret = db_device_hw_param_update(&info);
		if (SQLITE_OK == sql_ret) {
			ret = 0;
			if (0 == st_bright.auto_display_brightness)
				display_set_bright_to_device(
					st_bright.display_brightness);
		}
	}

	return ret;
}

/**
 * system_screen_set_bright - Init display parameters
 * None
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_init(void)
{
	int ret;
	screen_bright_t st_bright;
	screen_off_status_t st_auto;

	memset(&st_bright, 0, sizeof(st_bright));

	system_screen_get_bright(&st_bright);
	system_screen_set_bright(st_bright);

	memset(&st_auto, 0, sizeof(st_auto));
	ret = system_screen_get_auto_off_status(&st_auto);
	if ((ret != 0) || ((0 == ret) && (0 == st_auto.auto_screen_off) &&
		(0 == st_auto.screen_saver_off))) {
		ALOGD("Get screen status fail or screen auto is disable");
		return 0;
	}

	if (1 == st_auto.auto_screen_off)
		ret = auto_screen_init();
	if (ret) {
		ALOGE("auto screen init failed");
		return -1;
	}

	if (1 == st_auto.screen_saver_off)
		ret = screen_saver_init();
	if (ret) {
		ALOGE("auto screen init failed");
		return -1;
	}

	return 0;
}


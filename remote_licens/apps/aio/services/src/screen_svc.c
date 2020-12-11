/*
 * screen_svc.c - Interface to adjust screen brightness
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
#include <sys/prctl.h>

#include "db_all.h"
#include "display.h"
#include "screen_svc.h"

#define LOG_TAG "screen"
#include <log/log.h>

/* screen status type */
enum screen_state {
	SCREEN_ON,
	SCREEN_OFF,
	SCREEN_SAVER
};

enum screen_state state;
static screen_bright_t st_bright;
static auto_screen_t auto_screen;
static screen_saver_t screen_saver;
static saver_open_cb saver_open_notify_cb;
static saver_close_cb saver_close_notify_cb;

/**
 * screen saver activity open callback function
 * @param p a pointer of callback
 *
 * Returns 0 if success, otherwise -1.
 */
int saver_open_set_cb(saver_open_cb cb)
{
	saver_open_notify_cb = cb;

	return 0;
}

/**
 * screen saver activity close callback function
 * @param p a pointer of callback
 *
 * Returns 0 if success, otherwise -1.
 */
int saver_close_set_cb(saver_close_cb cb)
{
	saver_close_notify_cb = cb;

	return 0;
}

/**
 * display screen saver
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_saver_on(void)
{
	int ret = -1;

	if (saver_open_notify_cb) {
		ALOGD("saver_open_notify_cb");
		ret = saver_open_notify_cb();
	}
	if (ret != 0)
		ALOGE("screen saver notify callback fail");

	return ret;
}

/**
 * close screen saver
 *
 * Returns 0 if success, otherwise -1.
 */
int display_screen_saver_off(void)
{
	int ret = 0;

	if (saver_close_notify_cb) {
		ALOGD("saver_close_notify_cb");
		ret = saver_close_notify_cb();
	}
	if (ret != 0)
		ALOGE("screen saver notify callback fail");

	return ret;
}

/**
 * system_screen_set_bright - Set screen brightness
 * @st_bright: IN, screen brightness
 *
 * Returns 0 if success, otherwise -1.
 */
int system_screen_set_bright(screen_bright_t bright)
{
	int ret = 0;

	if (bright.auto_display_brightness != st_bright.auto_display_brightness) {
		st_bright.auto_display_brightness = bright.auto_display_brightness;
		st_bright.display_brightness = bright.display_brightness;
		ret = display_set_bright_to_device(bright.display_brightness);
	} else {
		if (0 == bright.auto_display_brightness) {
			ret = display_set_bright_to_device(bright.display_brightness);
			if (bright.display_brightness != st_bright.display_brightness)
				st_bright.display_brightness = bright.display_brightness;
		}
	}

	return ret;
}

/**
 * detect thread for auto open/close screen
 * args: a pointer of detect thread
 *
 * Returns NULL.
 */
static void *auto_screen_thread(void *args)
{
	auto_screen_t *screen = (auto_screen_t *)args;
	prctl(PR_SET_NAME, "auto_screen");

	while (screen->thread_running) {
		while (screen->countdown_running) {
			usleep(1000000);
			pthread_mutex_lock(&screen->screen_lock);
			if (--screen->delay_count == 0) {
				screen->countdown_running = 0;
				display_screen_off();
				/*
				if (state == SCREEN_SAVER)
					display_screen_saver_off();
				*/
				state = SCREEN_OFF;
			}
			pthread_mutex_unlock(&screen->screen_lock);
		}
		pthread_mutex_lock(&screen->screen_lock);
		pthread_cond_wait(&screen->screen_cond, &screen->screen_lock);
		pthread_mutex_unlock(&screen->screen_lock);

		pthread_mutex_lock(&screen->screen_lock);
		if (state == SCREEN_OFF) {
			display_screen_saver_off();
			usleep(300 * 1000);
			display_screen_on();
			system_screen_set_bright(st_bright);
			state = SCREEN_ON;
		}
		pthread_mutex_unlock(&screen->screen_lock);

		screen->countdown_running = 1;
	}

	return NULL;
}

/**
 * detect thread for auto open/close screen saver
 * args: a pointer of detect thread
 *
 * Returns NULL.
 */
static void *screen_saver_thread(void *args)
{
	screen_saver_t *screen = (screen_saver_t *)args;
	prctl(PR_SET_NAME, "screen_saver");

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

		pthread_mutex_lock(&screen->screen_saver_lock);
		if (state == SCREEN_SAVER) {
			display_screen_saver_off();
			system_screen_set_bright(st_bright);
			state = SCREEN_ON;
		}
		pthread_mutex_unlock(&screen->screen_saver_lock);

		screen->saver_countdown_running = 1;
	}

	return NULL;
}

/**
 * reset counter of close screen
 *
 * Returns None.
 */
void screen_count_reset(void)
{
	/* screen_bright_t st_bright; */
	auto_screen_t *screen = &auto_screen;
	pthread_mutex_lock(&screen->screen_lock);
#if 0
	if (state == SCREEN_OFF) {
		display_screen_saver_off();
		display_screen_on();
		system_screen_get_bright(&st_bright);
		system_screen_set_bright(st_bright);
		state = SCREEN_ON;
	}
#endif
	if (screen->countdown_running == 0)
		pthread_cond_signal(&screen->screen_cond);
	screen->delay_count = screen->delay_time;
	pthread_mutex_unlock(&screen->screen_lock);
}

/**
 * reset counter of close screen saver
 *
 * Returns None.
 */
void screen_saver_count_reset(void)
{
	/* screen_bright_t st_bright; */
	screen_saver_t *screen = &screen_saver;
	pthread_mutex_lock(&screen->screen_saver_lock);
#if 0
	if (state == SCREEN_SAVER) {
		display_screen_saver_off();
		system_screen_get_bright(&st_bright);
		system_screen_set_bright(st_bright);
		state = SCREEN_ON;
	}
#endif
	if (screen->saver_countdown_running == 0)
		pthread_cond_signal(&screen->screen_saver_cond);
	screen->saver_delay_count = screen->saver_delay_time;
	pthread_mutex_unlock(&screen->screen_saver_lock);
}

/**
 * auto_screen_init - Initialize screen parameter
 * @p_status: A pointer of screen_off_status_t
 *
 * Returns 0 if success, otherwise -1.
 */
static int auto_screen_init(screen_off_status_t *p_status)
{
	int ret;

	if (NULL == p_status)
		return -1;

	ALOGD("auto_screen_init\n");
	auto_screen_t *screen = &auto_screen;
	memset(screen, 0, sizeof(auto_screen_t));

	pthread_mutex_init(&(screen->screen_lock), NULL);
	pthread_cond_init(&(screen->screen_cond), NULL);

	screen->delay_time = p_status->auto_screen_off_timeout;
	ALOGD("screen->delay_time = %d", screen->delay_time);

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

/**
 * auto_screen_init - Initialize screen saver parameter
 * @p_status: A pointer of screen_off_status_t
 *
 * Returns 0 if success, otherwise -1.
 */
static int screen_saver_init(screen_off_status_t *p_status)
{
	int ret;
	if (NULL == p_status)
		return -1;

	ALOGD("screen_saver_init\n");
	screen_saver_t *screen = &screen_saver;
	memset(screen, 0, sizeof(screen_saver_t));

	pthread_mutex_init(&(screen->screen_saver_lock), NULL);
	pthread_cond_init(&(screen->screen_saver_cond), NULL);

	screen->saver_delay_time = p_status->screen_saver_off_timeout;
	ALOGD("screen->saver_delay_time = %d", screen->saver_delay_time);

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

/**
 * auto_screen_deinit - deinitialize screen parameter
 *
 * Returns 0 if success, otherwise -1.
 */
void auto_screen_deinit(void)
{
	auto_screen_t *screen = &auto_screen;

	screen->enable = 0;
	screen->thread_running = 0;
	screen->countdown_running = 0;
	pthread_join(screen->auto_thread, NULL);
	pthread_mutex_destroy(&screen->screen_lock);
	pthread_cond_destroy(&screen->screen_cond);
}

/**
 * screen_saver_deinit - deinitialize screen saver parameter
 *
 * Returns 0 if success, otherwise -1.
 */
void screen_saver_deinit(void)
{
	screen_saver_t *screen = &screen_saver;
	screen->saver_enable = 0;
	screen->saver_thread_running = 0;
	screen->saver_countdown_running = 0;
	pthread_join(screen->saver_thread, NULL);
	pthread_mutex_destroy(&screen->screen_saver_lock);
	pthread_cond_destroy(&screen->screen_saver_cond);
}

/**
 * screen_set_bright - Init display parameters
 * @p_bright: A pointer of screen_bright_t
 * @p_status: A pointer of screen_off_status_t
 *
 * Returns 0 if success, otherwise -1.
 */
int screen_svc_init(screen_bright_t *p_bright,
						screen_off_status_t *p_status)
{
	int ret;

	if ((NULL == p_bright) || (NULL == p_status))
		return -1;

	memcpy(&st_bright, p_bright, sizeof(screen_bright_t));
	system_screen_set_bright(st_bright);

	if ((0 == p_status->auto_screen_off) &&
			(0 == p_status->screen_saver_off)) {
		ALOGD("Get screen status fail or screen auto is disable");
		return 0;
	}

	system_screen_set_bright(st_bright);
	if (1 == p_status->auto_screen_off) {
		ret = auto_screen_init(p_status);
		if (ret) {
			ALOGE("auto screen init failed");
			return -1;
		}
	}

	if (1 == p_status->screen_saver_off) {
		ret = screen_saver_init(p_status);
		if (ret) {
			ALOGE("auto screen init failed");
			return -1;
		}
	}

	return 0;
}

/**
 * screen_svc_deinit - deinit display parameters
 * None
 */
void screen_svc_deinit(void)
{
	auto_screen_deinit();
	screen_saver_deinit();
}

int screen_set_auto_screen_off_timeout(int auto_screen_off_timeout)
{
	int ret = 0;
	device_hw_param_t info;

	ALOGD("%s:%d >> auto_screen_off_timeout=%d",
		__func__, __LINE__, auto_screen_off_timeout);

	memset(&info, 0, sizeof(device_hw_param_t));

	ret = db_device_hw_param_query(&info);
	if (ret != 0) {
		ALOGE("%s:%d Get param error!",__func__,__LINE__);
		return -1;
	}
	info.auto_screen_off_timeout = auto_screen_off_timeout;

	auto_screen.delay_time = info.auto_screen_off_timeout;
	pthread_mutex_lock(&auto_screen.screen_lock);
	auto_screen.delay_count = auto_screen.delay_time;
	pthread_mutex_unlock(&auto_screen.screen_lock);

	db_device_hw_param_update(&info);
	return 0;
}

int screen_set_screen_saver_off_timeout(
	int screen_saver_off_timeout)
{
	int ret = 0;
	device_hw_param_t info;

	ALOGD("%s:%d >> screen_saver_off_timeout=%d",
		__func__, __LINE__,screen_saver_off_timeout);

	memset(&info, 0, sizeof(device_hw_param_t));

	ret = db_device_hw_param_query(&info);
	if (ret != 0) {
		ALOGE("%s:%d Get param error!",__func__,__LINE__);
		return -1;
	}
	info.auto_screen_saver_timeout = screen_saver_off_timeout;

	screen_saver.saver_delay_time = info.auto_screen_saver_timeout;
	pthread_mutex_lock(&screen_saver.screen_saver_lock);
	screen_saver.saver_delay_count = screen_saver.saver_delay_time;
	pthread_mutex_unlock(&screen_saver.screen_saver_lock);

	db_device_hw_param_update(&info);
	return 0;
}


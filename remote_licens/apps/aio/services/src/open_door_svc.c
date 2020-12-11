/*
 * open_door_svc.c - Interface to open door
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
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "open_door_svc.h"
#include "relay.h"
#include "alarm_svc.h"

#define LOG_TAG "open_door_svc"
#include <log/log.h>

#define DEF_OPEN_DOOR_DELAY_TIME 5

/**
 * information structure of system open door
 */
typedef struct tag_sys_open_door_service {
	int is_stop; /* if not stop service */
	pthread_mutex_t open_door_lock; /* lock of thread */
	pthread_cond_t open_door_cond; /* condition of thread */
	pthread_t open_door_thrd; /* thread id */

	int remain_time; /* remain time of open door */
	int alarm_state; /* door sensor state */

} sys_open_door_service_t;

/* instance of check if not open door */
static sys_open_door_service_t s_open_door_service;

/* open door delay time */
static int s_delay_time = DEF_OPEN_DOOR_DELAY_TIME;

void *open_door_thread(void *param)
{
	sys_open_door_service_t *service =
		(sys_open_door_service_t *)param;
	prctl(PR_SET_NAME, "door_thread");

	if (NULL == service)
		return NULL;

	while (!service->is_stop) {
		pthread_mutex_lock(&service->open_door_lock);
		pthread_cond_wait(&service->open_door_cond,
			&service->open_door_lock);
		pthread_mutex_unlock(&service->open_door_lock);

		relay_ctrl(RELAY_ON);
		ALOGD("[%s:%d] RELAY_ON", __func__, __LINE__);

		while (service->remain_time > 0 && !service->alarm_state) {
			service->remain_time--;
			usleep(1000 * 1000);
		}

		relay_ctrl(RELAY_OFF);
		ALOGD("[%s:%d] RELAY_OFF", __func__, __LINE__);
	}

	return NULL;
}

/**
 * open_door_start_service - start service of open door check
 * if not open door.
 * @delay_time: delay time of open dooor
 *
 * Returns 0 if success, otherwise not 0.
 */
int open_door_start_service(int delay_time)
{
	s_delay_time = delay_time;
	s_open_door_service.alarm_state = get_alarm_state();

	if (pthread_mutex_init(&s_open_door_service.open_door_lock, NULL)) {
		ALOGE("init open_door_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_cond_init(&s_open_door_service.open_door_cond, NULL)) {
		ALOGE("init open_door_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		pthread_mutex_destroy(&s_open_door_service.open_door_lock);
		return -2;
	}

	s_open_door_service.is_stop = 0;
	if (pthread_create(&s_open_door_service.open_door_thrd, NULL,
				open_door_thread, &s_open_door_service)) {
		ALOGE("open_door_thread thread failed, errno: %d (%s)\n",
					errno, strerror(errno));
		pthread_mutex_destroy(&s_open_door_service.open_door_lock);
		pthread_cond_destroy(&s_open_door_service.open_door_cond);
		return -3;
	}

	return 0;
}

/**
 * open_door_stop_service - stop service of open door check
 * if not open door.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int open_door_stop_service(void)
{
	s_open_door_service.is_stop = 1;
	pthread_join(s_open_door_service.open_door_thrd, NULL);

	pthread_mutex_destroy(&s_open_door_service.open_door_lock);
	pthread_cond_destroy(&s_open_door_service.open_door_cond);

	return 0;
}

/**
 * open_door - open door
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int open_door(void)
{
	pthread_mutex_lock(&s_open_door_service.open_door_lock);
	pthread_cond_signal(&s_open_door_service.open_door_cond);
	s_open_door_service.remain_time = s_delay_time;
	pthread_mutex_unlock(&s_open_door_service.open_door_lock);

	return 0;
}

/**
 * alarm_state_change - alarm state change
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarm_state_change(int state)
{
	s_open_door_service.alarm_state = state;

	return 0;
}

/**
 * set_open_door_delay_time - set open door delay time
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_open_door_delay_time(int delay_time)
{
	s_delay_time = delay_time;

	return 0;
}


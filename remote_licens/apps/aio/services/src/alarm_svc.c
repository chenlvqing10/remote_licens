/*
 * alarm_svc.c - define alarm in serivce
 *
 *
 * Copyright (C) 2016-2020, LomboTech Co.Ltd.
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

#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "file_ops.h"
#include "sound_svc.h"
#include "alarm_svc.h"
#include "key.h"
#include "gpio.h"

#define LOG_TAG  "ALRAM_SVC"
#include <log/log.h>

#define ALARM_IN_KEY_CODE 0x101
#define DEF_DOOR_SENSOR_DELAY_TIME 100

#define ALARM_SOUND_PATH  "/usr/share/aio/res/sound/alarm.wav"
#define ALARM_STATE_PATH "/sys/devices/virtual/gpio-alarm/alarm/value"

/**
 * information structure of system open door
 */
typedef struct tag_sys_alarm_service {
	int is_stop; /* if not stop service */
	pthread_mutex_t alarm_lock; /* lock of thread */
	pthread_cond_t alarm_cond; /* condition of thread */
	pthread_t alarm_thrd; /* thread id */

	int alarm_switch; /* alarm switch */
	int delay_time; /* delay time of alarm */
	int remain_time; /* remain time of alarm */
	int alarm_state; /* door sensor state */

	/* Door sensor type
	 * Options:
	 *    0: Normaly close
	 *    1: Normaly open
	 *    2: Not detect
	 */
	int door_sensor_type;
} sys_alarm_service_t;

static alarm_signal_cb s_cb;
static void *s_key_event;

/* instance of check if not open door */
static sys_alarm_service_t s_alarm_service;

/**
 * proc_alarm_event： alarm key process function
 * @code: key code (should be 0x101)
 * @type: key type (should be 1)
 * @value: key value (0/1: press down / press up)
 */
static void proc_alarm_event(int code, int type, int value)
{
	ALOGD("%s[%d] code %d type %d value %d",
		__func__, __LINE__, code, type, value);
	if (code == ALARM_IN_KEY_CODE) {
		if (s_cb)
			s_cb(value);
	}
}

/*
* alarm in svc init
* @cb call back function if reset key has signal
* return 0 if success, otherwise return -1
*/
int alarm_in_svc_init(alarm_signal_cb cb)
{
	s_cb = cb;
	s_key_event = key_input_init(ALARM_IN_KEY_CODE, proc_alarm_event);
	if (NULL == s_key_event) {
		ALOGE("%s[%d] key_input_init() failed", __func__, __LINE__);
		return -1;
	}

	return 0;
}

/*
* alarm in svc deinit
*
* return 0 if success, otherwise return -1
*/
int alarm_in_svc_deinit()
{
	return key_input_deinit(s_key_event);
}

/*
* get_alarm_state
*
* return alarm state, otherwise return -1
*/
int get_alarm_state()
{
	char buf[1];

	file_read(ALARM_STATE_PATH, buf, 1);

	return atoi(buf);
/*
	long state;
	char *tmp;
	char buf[32] = {0};
	FILE *stream;

	stream = popen("devmem 0x04015150 16", "r");
	fread(buf, 1, sizeof(buf) - 1, stream);
	pclose(stream);

	state = strtol(buf, &tmp, 16);
	return (int)(state >> 14 & 1);
*/
}

void *alarm_thread(void *param)
{
	prctl(PR_SET_NAME, "alarm_thread");
	sys_alarm_service_t *service = (sys_alarm_service_t *)param;

	if (NULL == service)
		return NULL;

	while (!service->is_stop) {
		while (service->remain_time > 0 && service->alarm_state == DOOR_STATE_OPEN) {
			service->remain_time--;
			usleep(1000 * 1000);
		}

		while (service->alarm_state == DOOR_STATE_OPEN && service->alarm_switch &&
				(service->door_sensor_type == 0)) {
			sound_play_file(ALARM_SOUND_PATH);
			usleep(3000 * 1000);
		}

		pthread_mutex_lock(&service->alarm_lock);
		pthread_cond_wait(&service->alarm_cond, &service->alarm_lock);
		pthread_mutex_unlock(&service->alarm_lock);
	}

	return NULL;
}

/**
 * alarm_start_service - start service of alarm.
 * @delay_time: delay time of alarm
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarm_start_service(alarm_service_t *p_service)
{
	s_alarm_service.alarm_state = get_alarm_state();
	ALOGD("[%s:%d] alarm initial state: %d",
		__func__, __LINE__, s_alarm_service.alarm_state);
	s_alarm_service.alarm_switch = p_service->alarm_switch;
	s_alarm_service.delay_time = p_service->delay_time;
	s_alarm_service.remain_time = p_service->delay_time;
	s_alarm_service.door_sensor_type = p_service->door_sensor_type;

	if (pthread_mutex_init(&s_alarm_service.alarm_lock, NULL)) {
		ALOGE("init alarm_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_cond_init(&s_alarm_service.alarm_cond, NULL)) {
		ALOGE("init alarm_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		pthread_mutex_destroy(&s_alarm_service.alarm_lock);
		return -2;
	}

	s_alarm_service.is_stop = 0;
	if (pthread_create(&s_alarm_service.alarm_thrd, NULL,
				alarm_thread, &s_alarm_service)) {
		ALOGE("alarm_thread thread failed, errno: %d (%s)\n",
					errno, strerror(errno));
		pthread_mutex_destroy(&s_alarm_service.alarm_lock);
		pthread_cond_destroy(&s_alarm_service.alarm_cond);
		return -3;
	}

	return 0;
}

/**
 * alarm_stop_service - stop service of alarm.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarm_stop_service(void)
{
	s_alarm_service.is_stop = 1;
	pthread_join(s_alarm_service.alarm_thrd, NULL);

	pthread_mutex_destroy(&s_alarm_service.alarm_lock);
	pthread_cond_destroy(&s_alarm_service.alarm_cond);

	return 0;
}

/**
 * set_alarm_delay_time - set alarm delay time.
 * @delay_time: delay time of alarm
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_alarm_delay_time(int delay_time)
{
	s_alarm_service.delay_time = delay_time;

	return 0;
}

/**
 * set_door_sensor_type - set door sensor type.
 * @door_sensor_type: door sensor type
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_door_sensor_type(int door_sensor_type)
{
	s_alarm_service.door_sensor_type = door_sensor_type;

	return 0;
}

/**
 * set_alarm_param - set dalarm param.
 * @p_info: alarm param
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_alarm_switch(int alarm_switch)
{
	s_alarm_service.alarm_switch = alarm_switch;

	return 0;
}

/**
 * alarming - alarming.
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming()
{
	s_alarm_service.alarm_state = DOOR_STATE_OPEN;

	if (s_alarm_service.alarm_switch &&
		(s_alarm_service.door_sensor_type == 0)) {
		pthread_mutex_lock(&s_alarm_service.alarm_lock);
		pthread_cond_signal(&s_alarm_service.alarm_cond);
		s_alarm_service.remain_time = s_alarm_service.delay_time;
		pthread_mutex_unlock(&s_alarm_service.alarm_lock);
	}

	return 0;
}

/**
 * alarming_stop - alarming stop.
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming_stop()
{
	s_alarm_service.alarm_state = DOOR_STATE_CLOSE;

	return 0;
}

/**
 * alarming_test - alarming test.
 * @p_info
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming_test(alarm_service_t *p_info)
{
	if (p_info == NULL)
		return -1;

	p_info->alarm_switch = s_alarm_service.alarm_switch;
	p_info->delay_time = s_alarm_service.delay_time;
	p_info->door_sensor_type = s_alarm_service.door_sensor_type;

	s_alarm_service.alarm_state = DOOR_STATE_OPEN;
	s_alarm_service.alarm_switch = 1;
	s_alarm_service.door_sensor_type = 0;

	pthread_mutex_lock(&s_alarm_service.alarm_lock);
	pthread_cond_signal(&s_alarm_service.alarm_cond);
	s_alarm_service.remain_time = 3;
	pthread_mutex_unlock(&s_alarm_service.alarm_lock);

	return 0;
}

/**
 * alarming_test_stop - alarming test stop.
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming_test_stop(alarm_service_t *p_info)
{
	s_alarm_service.alarm_state = DOOR_STATE_CLOSE;

	s_alarm_service.alarm_switch = p_info->alarm_switch;
	s_alarm_service.delay_time = p_info->delay_time;
	s_alarm_service.door_sensor_type = p_info->door_sensor_type;

	return 0;
}


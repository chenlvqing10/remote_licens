/*
 * alarm_svc.h - define alarm in serivce
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

#ifndef __ALARM_SVC_H
#define __ALARM_SVC_H

#define DOOR_STATE_OPEN 1
#define DOOR_STATE_CLOSE 0

/**
 * information structure of system open door
 */
typedef struct _alarm_service {
	/* alarm switch */
	int alarm_switch;

	/* delay time of alarm */
	int delay_time;

	/* Door sensor type
	 * Options:
	 *    0: Normaly close
	 *    1: Normaly open
	 *    2: Not detect
	 */
	int door_sensor_type;
} alarm_service_t;

typedef void (*alarm_signal_cb)(int);

/*
* alarm in svc init
* @cb call back function if reset key has signal
* return 0 if success, otherwise return -1
*/
int alarm_in_svc_init(alarm_signal_cb cb);

/*
* alarm in svc deinit
*
* return 0 if success, otherwise return -1
*/
int alarm_in_svc_deinit();

/*
* get_alarm_state
*
* return alarm state, otherwise return -1
*/
int get_alarm_state();

/**
 * alarm_start_service - start service of alarm.
 * @delay_time: delay time of alarm
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarm_start_service(alarm_service_t *p_service);

/**
 * alarm_stop_service - stop service of alarm.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarm_stop_service(void);

/**
 * set_alarm_delay_time - set alarm delay time
 * @delay_time: delay time of alarm
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_alarm_delay_time(int delay_time);

/**
 * set_door_sensor_type - set door sensor type.
 * @door_sensor_type: door sensor type
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_door_sensor_type(int door_sensor_type);

/**
 * set_alarm_param - set dalarm param.
 * @p_info: alarm param
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_alarm_switch(int alarm_switch);

/**
 * alarming - alarming.
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming();

/**
 * alarming_stop - alarming stop.
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming_stop();

/**
 * alarming_test - alarming test.
 * @p_info  alarm service param
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming_test(alarm_service_t *p_info);

/**
 * alarming_test_stop - alarming test stop.
 * @p_info  alarm service param
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarming_test_stop(alarm_service_t *p_info);

#endif /* __ALARM_SVC_H */


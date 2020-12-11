/*
 * reset_svc.h - resetting service background for monitoring reset key and
 *               provide resetting device api
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
#include <semaphore.h>
#include <dirent.h>
#include <sys/prctl.h>

#include "reset_svc.h"
#include "gpio.h"
#include "system_factory.h"
#include "key.h"

#define LOG_TAG  "RESET_SVC"
#include <log/log.h>

#define RESET_KEY_CODE 0x100

#define KEY_PRESS_DOWN 0
#define KEY_PRESS_UP 1

#define RESET_HOLD_TIME 3000 /*in milliseconds*/
#define RESET_PROB_TIMES 15

typedef struct tag_reset_deamon {
	pthread_t reset_pid; /* Thread ID */
	pthread_mutex_t mutex; /*lock for check_flag*/
	int key_status; /* key status: 0/1: press down/press up */
	int check_flag; /* check flag: 0/1: check/non-check */
	reset_signal_cb cb; /* callback function */
	void *key_event; /* key event object */
	int is_running; /* flag of thread run: 0/1/2: no run/running/end */
} reset_deamon_t;

static reset_deamon_t reset_deamon;

/**
 * Enable the reset thread to monitor reset key if
 * cb had been called and reset_device_immed() doesn't
 * been called;
 */
void set_reset_flag()
{
	reset_deamon.check_flag = 1;
}

/**
 * Disable the reset thread to monitor reset key if
 * cb had been called and reset_device_immed() doesn't
 * been called;
 */
void clear_reset_flag()
{
	reset_deamon.check_flag = 0;
}

/**
 * reset_thread - check reset key status, if reset key
 * press down continued 3 seconds, call callback funtion
 * to application level.
 */
void *reset_thread()
{
	int cnt = 0;
	int gpio_value = 0;
	prctl(PR_SET_NAME, "reset_thread");
	ALOGD("%s: reset thread\n", __func__);
	while (reset_deamon.is_running) {
		while (reset_deamon.check_flag) {
			gpio_value = reset_deamon.key_status;
			if (gpio_value == KEY_PRESS_DOWN) {
				cnt++;
				if (cnt == RESET_PROB_TIMES) {
					clear_reset_flag();
					ALOGD("%s: ===== reset flag device=====\n",
						 __func__);
					if (reset_deamon.cb != NULL)
						reset_deamon.cb();
					break;
				}
			} else
				cnt = 0;

			usleep(RESET_HOLD_TIME*1000/RESET_PROB_TIMES);
		}

		while (!reset_deamon.check_flag) {
			usleep(500 * 1000);
		}
	}

	reset_deamon.is_running = 2;
	ALOGE("%s: !!! Program should never be executing here!\n", __func__);

	return NULL;
}

/**
 * reset_default_cb - default callback function
 */
void reset_default_cb()
{
	ALOGE("%s: !!!Need valid reset callback func!\n", __func__);

	usleep(10 * 1000 * 1000);
	set_reset_flag();
}

/**
 * set_reset_signal_cb - set callback function
 * @cb: a pointer of callback function
 *
 */
static void set_reset_signal_cb(reset_signal_cb cb)
{
	reset_deamon.cb = cb;
}

/**
 * proc_key_event： reset key process function
 * @code: key code (should be 0x100)
 * @type: key type (should be 1)
 * @value: key value (0/1: press down / press up)
 */
void proc_key_event(int code, int type, int value)
{
	ALOGD("%s[%d] code %d type %d value %d",
		__func__, __LINE__, code, type, value);
	if (code == RESET_KEY_CODE) {
		reset_deamon.key_status = value;
		if (0 == value)
			reset_deamon.check_flag = 1;
		else
			reset_deamon.check_flag = 0;
	}
}

/**
 * reset svc init
 * @cb call back function if reset key has signal
 * return 0 if success, otherwise return -1
 */
int reset_factory_svc_init(reset_signal_cb cb)
{
	ALOGD("%s: system factory service init\n", __func__);
	reset_deamon.key_event = key_input_init(RESET_KEY_CODE, proc_key_event);

	if (cb)
		set_reset_signal_cb(cb);
	else
		set_reset_signal_cb(reset_default_cb);

	/*
	set_reset_flag();
	*/
	clear_reset_flag();
	reset_deamon.key_status = KEY_PRESS_UP;
	reset_deamon.is_running = 1;
	pthread_create(&(reset_deamon.reset_pid), NULL, &reset_thread, NULL);
	ALOGD("%s: pthread_create thread.\n", __func__);
	pthread_detach(reset_deamon.reset_pid);
	ALOGD("%s: pthread_detach thread.\n", __func__);

	return 0;
}

/*
* reset svc deinit
*
* return 0 if success, otherwise return -1
*/
int reset_factory_svc_deinit()
{
	reset_deamon.is_running = 0;
	while (reset_deamon.is_running != 2)
		usleep(10 * 1000);
	key_input_deinit(reset_deamon.key_event);

	return 0;
}

/**
 * reset device immedialy.
 *
 * Returns 0 if success, otherwise -1.
 */
int reset_device_immed()
{
	return system_factory();
}


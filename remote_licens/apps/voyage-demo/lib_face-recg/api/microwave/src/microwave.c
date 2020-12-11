/*
 * microwave.c - define some function of operate microwave device
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
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "db_config.h"

#include "microwave.h"

#define LOG_TAG         "microwave"
#include <log/log.h>

typedef struct tag_microwave_dev {
	bool microwave_running;
	pthread_t pid;
	void (*callback)(int state);
} microwave_dev_t;

static microwave_dev_t m_dev;

static void *lombo_microwave_do(void *arg)
{
	int keys_fd;
	microwave_dev_t *dev = (microwave_dev_t *)arg;
	struct input_event t;

	ALOGD("lombo_microwave_do++\n");
	keys_fd = open(MICROWAVE_DEV_PATH, O_RDONLY);
	if (keys_fd <= 0) {
		ALOGE("open %s device error!\n", MICROWAVE_DEV_PATH);
		return NULL;
	}

	while (dev->microwave_running) {
		if (read(keys_fd, &t, sizeof(t)) == sizeof(t)) {
			if (t.type == EV_MSC && t.code == MSC_RAW) {
				if (t.value == OBJECT_MOVE
					|| t.value == OBJECT_STOP) {
					ALOGD("microwave state:%d", t.value);
					if (dev->callback)
						dev->callback(t.value);
				}
			}
		}
	}

	close(keys_fd);

	ALOGD("lombo_microwave_do--s\n");
	return NULL;
}

int lombo_microwave_start(void)
{
	microwave_dev_t *dev = &m_dev;

	ALOGD("lombo_microwave_start\n");
	if (dev->microwave_running == false)
		dev->microwave_running = true;
	else {
		ALOGE("lombo_microwave is runing, please stop it first");
		return -1;
	}

	pthread_create(&dev->pid, NULL, lombo_microwave_do, (void *)dev);

	return 0;
}

int lombo_microwave_stop(void)
{
	microwave_dev_t *dev = &m_dev;

	dev->microwave_running = false;
	pthread_join(dev->pid, NULL);

	ALOGD("lombo_microwave_stop\n");
	return 0;
}

void event_state_set_cb(ev_state_cb cb)
{
	m_dev.callback = cb;
}

int microwave_init(void)
{
	int ret = 0, microwave_sw;

	microwave_sw = db_config_get_int("device_microwave_param",
		"microwave_sw", 0);
	if (microwave_sw)
		ret = lombo_microwave_start();
	else
		ALOGD("no microwave device");

	return ret;
}


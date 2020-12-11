/*
 * temp_masure.c
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

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <dirent.h>

#define LOG_TAG "temp_masure"
#include <log/log.h>
#include <nna_api.h>

#include "face_svc.h"
#include "face_svc_conf.h"
#include "temp_measure.h"
#include "temp_measure_device_001.h"

temp_svc_t *temp_svc = NULL;

struct temp_measure_device {
	int (*device_init)(void);
	float (*device_get_temp)(void);
	int(*device_deinit)(void);
	int (*device_set_measure_frames)(int);
	int (*device_reset_measure_frame)(void);
};

#if 0
static struct temp_measure_device device = {
	temp_device_init,
	temp_device_get,
	temp_device_deinit
};
#endif

#if 1
static struct temp_measure_device device = {
	temp_device_001_init,
	temp_device_001_get,
	temp_device_001_deinit,
	temp_device_001_set_measure_frames,
	temp_device_001_reset_measure_frame
};
#endif

int temp_measure_reset(int id)
{
	list_head_t *list = NULL;
	temp_measure_t *measure = NULL;
	temp_measure_t *tmp = NULL;

	ALOGD("%s", __func__);

	device.device_reset_measure_frame();

	list = &temp_svc->temp_measure_list;
	pthread_mutex_lock(&temp_svc->temp_list_lock);
	list_for_each_entry(measure, list, node) {
		if (measure->id == id) {
			tmp = measure;
			tmp->cnt = 0;
			ALOGD_IF(face_svc_conf->debug.debug_log, "%s reset measure[%d]",
				__func__, tmp->id);
			break;
		}
	}

	if (!tmp) {
		ALOGD("measure id no yet");
	}
	pthread_mutex_unlock(&temp_svc->temp_list_lock);

	return 0;
}

void temp_measure_update(nna_faces_t *rects)
{
	int i = 0;
	list_head_t *list = NULL;
	temp_measure_t *measure = NULL;
	temp_measure_t *tmp = NULL;
	nna_face_t *face_rect = NULL;

	list = &temp_svc->temp_measure_list;

	/*lock*/
	pthread_mutex_lock(&temp_svc->temp_list_lock);
	/* Cleanup */
	list_for_each_entry_safe(measure, tmp, list, node) {
		for (i = 0; i < rects->num; i++) {
			face_rect = &rects->face_rects[i];
			if (face_rect->id == measure->id)
				break;
		}

		if (i >= rects->num) {
			ALOGD_IF(face_svc_conf->debug.debug_log, "%s delete measure[%d]",
				__func__, measure->id);
			list_del(&measure->node);
			/* free */
			free(measure->temp_value);
			measure->temp_value = NULL;
			/* free */
			free(measure);
			measure = NULL;
		}
	}

	for (i = 0; i < rects->num; i++) {
		face_rect = &rects->face_rects[i];

		tmp = NULL;
		list_for_each_entry(measure, list, node) {
			if (measure->id == face_rect->id) {
				tmp = measure;
				break;
			}
		}

		/*not find same id, new temp */
		if (!tmp) {
			tmp = malloc(sizeof(temp_measure_t));
			if (!tmp) {
				ALOGE("%s malloc temp_measure_t failed, errno: %d (%s)",
					__func__, errno, strerror(errno));
				pthread_mutex_unlock(&temp_svc->temp_list_lock);
				return;
			}
			memset(tmp, 0, sizeof(temp_measure_t));
			tmp->temp_value = malloc(
				sizeof(float) * face_svc_conf->svc.temp_read_times);
			if (!tmp->temp_value) {
				ALOGE("%s malloc float failed, errno: %d (%s)",
					__func__, errno, strerror(errno));
				free(tmp);
				tmp = NULL;
				pthread_mutex_unlock(&temp_svc->temp_list_lock);
				return;
			}
			memset(tmp->temp_value, 0,
				sizeof(float) * face_svc_conf->svc.temp_read_times);

			tmp->id = face_rect->id;

			ALOGD_IF(face_svc_conf->debug.debug_log, "%s add measure[%d]",
				__func__, tmp->id);
			list_add(&tmp->node, list);
		}
	}
	pthread_mutex_unlock(&temp_svc->temp_list_lock);
	return;
}

temp_measure_t *temp_measure_get(int id)
{
	list_head_t *list = NULL;
	temp_measure_t *measure = NULL;
	temp_measure_t *tmp = NULL;

	ALOGD("%s", __func__);

	list = &temp_svc->temp_measure_list;

	/* lock */
	pthread_mutex_lock(&temp_svc->temp_list_lock);
	/* get temp */
	list_for_each_entry(measure, list, node) {
		if (measure->id == id) {
			tmp = measure;
			if (tmp->cnt >= face_svc_conf->svc.temp_read_times) {
				pthread_mutex_unlock(&temp_svc->temp_list_lock);
				return tmp;
			} else {
				pthread_mutex_unlock(&temp_svc->temp_list_lock);
				return NULL;
			}
		}
	}

	/* unlock */
	pthread_mutex_unlock(&temp_svc->temp_list_lock);
	return NULL;
}

int temp_measure_set(temp_measure_t *measure)
{
	temp_measure_t *tmp = NULL;
	list_head_t *list = NULL;

	list = &temp_svc->temp_measure_list;

	pthread_mutex_lock(&temp_svc->temp_list_lock);
	/* update list face rect temp */
	list_for_each_entry(tmp, list, node) {
		if (tmp->cnt < face_svc_conf->svc.temp_read_times) {
			tmp->temp_value[tmp->cnt] = measure->value;
			tmp->value = measure->value;
			tmp->cnt++;
		}
		/* maybe need temperature compensation */
	}
	pthread_mutex_unlock(&temp_svc->temp_list_lock);
	return 0;
}

static void *temp_measure_thread(void *arg)
{
	temp_svc_t *svc = (temp_svc_t *)arg;
	temp_measure_t measure;
	float temp_value;

	prctl(PR_SET_NAME, "temp-measure");

	ALOGD("%s", __func__);

	while (1) {
		pthread_mutex_lock(&svc->temp_measure_lock);
		if (!svc->temp_measure)
			pthread_cond_wait(&svc->temp_measure_cond, &svc->temp_measure_lock);
		pthread_mutex_unlock(&svc->temp_measure_lock);

		/* read face rect temp */
		temp_value = device.device_get_temp();
		if (temp_value > 0.0f) {
			memset(&measure, 0, sizeof(temp_measure_t));
			measure.value = temp_value;
			temp_measure_set(&measure);
		} else if (temp_value < 0.0f) {
			usleep(1000 * 1000);
		}

		/* temp read interval */
		/* usleep(face_svc_conf->svc.temp_read_interval * 1000); */
	}
	return NULL;
}

int temp_measure_start(void)
{
	if (!temp_svc) {
		ALOGE("%s g_svc is null", __func__);
		return -1;
	}

	pthread_mutex_lock(&temp_svc->temp_measure_lock);
	if (temp_svc->temp_measure == 0) {
		ALOGD("%s", __func__);
		temp_svc->temp_measure = 1;
		pthread_cond_signal(&temp_svc->temp_measure_cond);
	}
	pthread_mutex_unlock(&temp_svc->temp_measure_lock);
	return 0;
}

int temp_measure_stop(void)
{
	if (!temp_svc) {
		ALOGE("%s g_svc is null", __func__);
		return -1;
	}

	pthread_mutex_lock(&temp_svc->temp_measure_lock);
	if (temp_svc->temp_measure == 1) {
		ALOGD("%s", __func__);
		temp_svc->temp_measure = 0;
		pthread_cond_signal(&temp_svc->temp_measure_cond);
	}
	pthread_mutex_unlock(&temp_svc->temp_measure_lock);
	return 0;
}

int temp_measure_init(void)
{
    if (NULL == temp_svc){
		ALOGD("%s", __func__);
		temp_svc = (temp_svc_t *)malloc(sizeof(temp_svc_t));
		if (!temp_svc) {
			ALOGE("malloc face_svc_t failed, errno: %d (%s)\n",
				errno, strerror(errno));
			return -1;
		}

		memset(temp_svc, 0, sizeof(temp_svc_t));

		INIT_LIST_HEAD(&temp_svc->temp_measure_list);
		if (pthread_mutex_init(&temp_svc->temp_measure_lock, NULL)) {
			ALOGE("init rgb_frame_lock failed, errno: %d (%s)\n",
				errno, strerror(errno));
			return -1;
		}
		if (pthread_cond_init(&temp_svc->temp_measure_cond, NULL)) {
			ALOGE("init rgb_frame_cond failed, errno: %d (%s)\n",
				errno, strerror(errno));
			return -1;
		}
		if (pthread_mutex_init(&temp_svc->temp_list_lock, NULL)) {
			ALOGE("init rgb_frame_lock failed, errno: %d (%s)\n",
				errno, strerror(errno));
			return -1;
		}

		ALOGV("create face service detect thread");
		if (pthread_create(&temp_svc->temp_measure_thrd, NULL,
				temp_measure_thread, (void *)temp_svc)) {
			ALOGE("create face service detect thread failed, "
				"errno: %d (%s)", errno, strerror(errno));
			return -1;
		}

		if(device.device_init()) {
			ALOGE("device_init failed\n");
			return -1;
		}
	}
	return 0;

}

int temp_measure_deinit(void)
{
	ALOGD("%s", __func__);
	/* deint device */

	/* deinit temp measure */
	pthread_mutex_destroy(&temp_svc->temp_list_lock);
	pthread_mutex_destroy(&temp_svc->temp_measure_lock);
	pthread_cond_destroy(&temp_svc->temp_measure_cond);

	if (temp_svc) {
		free(temp_svc);
		temp_svc = NULL;
	}
	if(device.device_deinit()) {
		ALOGE("device_deinit failed\n");
		return -1;
	}
	return 0;
}


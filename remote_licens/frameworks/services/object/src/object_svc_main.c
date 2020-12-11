/*
 * object_svc_main.c
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <execinfo.h>
#include <sys/time.h>
#include <sys/stat.h>

#define LOG_TAG "object_svc"
#include <log/log.h>

#include "jpeg/jpeg_enc.h"

#include "object_svc.h"
#include "object_svc_main.h"
#include "object_svc_media.h"
#include "ax_nir_dt.h"
#include "object_svc_isp.h"

#include "cut.h"

#define CLIPMAX(v, mx) {		\
	if ((v) > (mx))			\
		(v) = (mx);		\
}

long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

#define STACK_DEPTH_MAX 128
void dump_stack(void)
{
	void *stack_trace[STACK_DEPTH_MAX] = {0};
	char **stack_strings = NULL;
	int stack_depth = 0;
	int i = 0;

	stack_depth = backtrace(stack_trace, STACK_DEPTH_MAX);
	stack_strings = (char **)backtrace_symbols(stack_trace, stack_depth);
	if (NULL == stack_strings) {
		ALOGI(" Memory is not enough while dump stack trace\n");
		exit(EXIT_FAILURE);
	}

	ALOGI(" Stack trace:\n");
	for (i = 0; i < stack_depth; i++) {
		ALOGI(" [%d] %s\n", stack_depth - i, stack_strings[i]);
	}
	ALOGI("\n");
	free(stack_strings);
	stack_strings = NULL;
	/* exit(EXIT_FAILURE); */
}

void free_frame(object_frame_t **pp)
{
	object_frame_t *frame = *pp;

	if (frame) {
		ALOGD_IF(0, "~~~ frame out %p", frame);
		object_media_return_frame(frame);
		frame = NULL;
	}
}

static void *object_det_thread(void *arg)
{
	object_svc_t *svc = (object_svc_t *)arg;
	object_frame_t *frame;
	rgb_det_buf_t *det_buf;
	nna_feature_map_t *map;
	object_rects_t cb_object_rects;
	vgg_dt_in_t dt_in;

	ALOGV("enter %s\n", __func__);

	while (1) {
		pthread_mutex_lock(&svc->rgb_frame_lock);
		if (empty_queue(&svc->rgb_frame_queue))
			pthread_cond_wait(&svc->rgb_frame_cond, &svc->rgb_frame_lock);
		frame = (object_frame_t *)de_queue(&svc->rgb_frame_queue);
		pthread_mutex_unlock(&svc->rgb_frame_lock);

		if (frame == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		det_buf = malloc(sizeof(rgb_det_buf_t));
		if (!det_buf) {
			ALOGE("%s malloc rgb_det_buf_t failed, errno: %d (%s)",
				__func__, errno, strerror(errno));
			goto free_res;
		}

		det_buf->frame = frame;

		map = &det_buf->map;
		map->fmt = YUV420;
		if (frame->type == OBJECT_FRAME_TYPE_RGB)
			map->img_type = NNA_VIDEO;
		else
			map->img_type = NNA_IMAGE;
		map->width = svc->rgb_buffer_width;
		map->height = svc->rgb_buffer_height;
		map->channal = 3;
		map->stride1 = map->width;
		map->stride2 = map->width;
		map->buf_hdl.size = frame->size[0] + frame->size[1];
		map->buf_hdl.phyaddr = (unsigned long)frame->phyaddr[0]
			+ frame->width * (frame->height - map->height) / 2;
		map->buf_hdl.viraddr = (unsigned long)frame->viraddr[0]
			+ frame->width * (frame->height - map->height) / 2;
		map->buf_hdl.uv_phyaddr = (unsigned long)frame->phyaddr[1]
			+ frame->width * (frame->height - map->height) / 4;
		map->buf_hdl.uv_viraddr = (unsigned long)frame->viraddr[1]
			+ frame->width * (frame->height - map->height) / 4;

		dt_in.target_sz.width = map->width;
		dt_in.target_sz.height = map->height;
		dt_in.yuv.y = (void *)map->buf_hdl.phyaddr;
		dt_in.yuv.uv = (void *)map->buf_hdl.uv_phyaddr;
		dt_in.dt_roi.x = svc->recg_x;
		dt_in.dt_roi.y = svc->recg_y;
		dt_in.dt_roi.width = svc->recg_width;
		dt_in.dt_roi.height = svc->recg_height;

		int result[4] = {0};
		svc->vgg_fn->vgg_recv_buffer(svc->vgg_fn, &dt_in,
				(void *) map->buf_hdl.viraddr, result);

		list_head_t *list = &svc->det_result_list;
		det_result_t *tmp;
		pthread_mutex_lock(&svc->det_result_lock);

		for (int i = 0; i < 4; i++) {
			ALOGD("%s %d", __func__, __LINE__);
			tmp = malloc(sizeof(det_result_t));
			if (!tmp) {
				ALOGE("%s malloc tracking_object_t failed, errno: %d (%s)",
						__func__, errno, strerror(errno));
				pthread_mutex_unlock(&svc->det_result_lock);
				return NULL;
			}
			svc->det_results.num++;
			memset(tmp, 0, sizeof(det_result_t));
			sprintf(tmp->label, "%s", &svc->label[result[i]][0]);
			ALOGE("%s %d %s--%d", __func__, __LINE__, tmp->label, result[i]);
			list_add(&tmp->node, list);
		}
		pthread_mutex_unlock(&svc->det_result_lock);

		svc->det_rgb_cb(frame, NULL);
		object_media_return_frame(frame);
		continue;

free_res:
		if (frame->type == OBJECT_FRAME_TYPE_RGB && svc->det_cb) {
			memset(&cb_object_rects, 0, sizeof(object_rects_t));
			svc->det_cb(frame, &cb_object_rects);
			free_frame(&frame);
		} else if (frame->type == OBJECT_FRAME_TYPE_API_DET && svc->det_cb) {
			memset(&cb_object_rects, 0, sizeof(object_rects_t));
			svc->det_cb(frame, &cb_object_rects);
		}

		if (det_buf) {
			free(det_buf);
			det_buf = NULL;
		}
	}

	return NULL;
}


int objects_set_det_result(void *arg, void *cb_object_rects)
{
	object_svc_t *svc = (object_svc_t *) arg;
	object_rects_t *rect = (object_rects_t *) cb_object_rects;
	list_head_t *list = NULL;
	det_result_t *tmp = NULL;

	int i = 0;
	if (!svc)
		return 1;
	if (!rect)
		return 1;
	list = &svc->det_result_list;

	pthread_mutex_lock(&svc->det_result_lock);

	for (i = 0; i < rect->cnt; i++) {
		tmp = malloc(sizeof(det_result_t));
		if (!tmp) {
			ALOGE("%s malloc tracking_object_t failed, errno: %d (%s)", __func__,
					errno, strerror(errno));
			pthread_mutex_unlock(&svc->det_result_lock);
			return 1;
		}
		svc->det_results.num++;
		memset(tmp, 0, sizeof(det_result_t));
		tmp->rect.rect[0] = rect->p_rect[i].rect[0];
		tmp->rect.rect[1] = rect->p_rect[i].rect[1];
		tmp->rect.rect[2] = rect->p_rect[i].rect[2];
		tmp->rect.rect[3] = rect->p_rect[i].rect[3];
		sprintf(tmp->label, "%s", "red");
		list_add(&tmp->node, list);
	}
	pthread_mutex_unlock(&svc->det_result_lock);

	return 0;
}

void *objects_get_det_result(void *arg)
{
	int i = 0;
	object_svc_t *svc = (object_svc_t *) arg;
	det_results_t *results = NULL;
	det_result_t *result = NULL;
	det_result_t *result_tmp = NULL;

	if (svc->det_results.num <= 0)
		return NULL;

	results = malloc(sizeof(det_results_t));
	if (!results) {
		ALOGE("%s malloc tracking_object_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return NULL;
	}
	results->num = svc->det_results.num;
	results->result = malloc(sizeof(det_result_t) * results->num);
	if (!results->result) {
		free(results);
		results = NULL;
		ALOGE("%s malloc tracking_object_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return NULL;
	}

	pthread_mutex_lock(&svc->det_result_lock);
	list_for_each_entry_safe(result, result_tmp, &svc->det_result_list, node) {
		if (NULL != result) {
			memcpy(&results->result[i], result, sizeof(det_result_t));
			list_del(&result->node);
			free(result);
			svc->det_results.num--;
			i++;
		}
	}
	pthread_mutex_unlock(&svc->det_result_lock);

	return results;
}

void load_label(object_svc_t *svc, const char *file_path)
{
	ALOGD("%s, %d", __func__, __LINE__);
	FILE *fin = fopen(file_path, "r");
	char *buf = NULL;
	char *substr = NULL;

	if (fin == NULL) {
		ALOGE("open %s failed\n", file_path);
		return;
	}

	buf = (char *)svc->label;

	while (!feof(fin)) {
		fgets(buf, 128, fin);
		substr = strchr(buf, ',');
		if (substr) {
			buf[substr - buf] = '\0';
		}
		buf[32] = '\0';
		buf += 128;
	}
	fclose(fin);
}

void *object_svc_init(void)
{
	object_svc_t *svc;

	ALOGV("enter %s\n", __func__);
	svc = (object_svc_t *) malloc(sizeof(object_svc_t));
	if (!svc) {
		ALOGE("malloc object_svc_t failed, errno: %d (%s)\n", errno,
				strerror(errno));
		return NULL;
	}
	memset(svc, 0, sizeof(object_svc_t));

	object_svc_load_conf(svc);

	object_buffer_size(OBJECT_CAMERA_RGB, svc->rgb_buffer_width,
			svc->rgb_buffer_height, 1);

	if (svc->object_language == 0) {
		load_label(svc, "/usr/share/ax/object_labels.txt");
	} else {
		load_label(svc, "/usr/share/ax/object_labels_cn.txt");
	}

	TAILQ_INIT(&svc->rgb_frame_queue);
	if (pthread_mutex_init(&svc->rgb_frame_lock, NULL)) {
		ALOGE("init rgb_frame_lock failed, errno: %d (%s)\n", errno,
				strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->rgb_frame_cond, NULL)) {
		ALOGE("init rgb_frame_cond failed, errno: %d (%s)\n", errno,
				strerror(errno));
		return NULL;
	}

	INIT_LIST_HEAD(&svc->det_result_list);
	if (pthread_mutex_init(&svc->det_result_lock, NULL)) {
		ALOGE("init det_result_lock failed, errno: %d (%s)\n", errno,
				strerror(errno));
		return NULL;
	}

	ALOGV("object media init");
	svc->media_hdl = object_media_create();
	if (!svc->media_hdl) {
		ALOGE("media create failed");
		return NULL;
	}

	/* create object service detect thread */
	ALOGV("create object service detect thread");
	if (pthread_create(&svc->object_det_thrd, NULL, object_det_thread, (void *) svc)) {
		ALOGE("create object service detect thread failed, errno: %d (%s)",
				errno, strerror(errno));
		return NULL;
	}

	svc->vgg_fn = vgg_open(svc->rgb_buffer_width, svc->rgb_buffer_height);
	return svc;
}

static int object_svc_rgb_frame_cb(void *p, object_frame_t *frame)
{
	object_svc_t *svc = (object_svc_t *) p;
	int ret;
	static unsigned int idx = 1;

	if (!frame || !p)
		return -1;

	if (svc->dump_fps) {
		static long long start = 0;
		long long end = get_systime_us();
		static int fps = 0;

		fps++;
		if (end - start > 1000000) {
			ALOGI("rgb frame %d fps", fps);
			start = get_systime_us();
			fps = 0;
		}
	}

	if (idx++ % svc->det_step) {
		//ALOGV("%s dropped frame: %d", __func__, idx);
		object_media_return_frame(frame);
		return 0;
	}

	/* for calculating time consuming of each algorithm module */
	frame->type = OBJECT_FRAME_TYPE_RGB;
	frame->det_begin = -1;
	frame->det_end = -1;

	pthread_mutex_lock(&svc->rgb_frame_lock);
	ret = en_queue(&svc->rgb_frame_queue, frame);
	pthread_cond_signal(&svc->rgb_frame_cond);
	pthread_mutex_unlock(&svc->rgb_frame_lock);

	return ret;
}

int object_return_frame(void *handle, object_frame_t *f_frame)
{
	object_svc_t *svc = (object_svc_t *) handle;

	ALOGV("enter %s\n", __func__);
	if (!svc)
		return -1;

	return object_media_return_frame(f_frame);
}

int object_get_camera_config(void *handle, enum object_camera_id camera_id,
		camera_cfg_t *cfg)
{
	object_svc_t *svc = (object_svc_t *) handle;

	ALOGV("enter %s\n", __func__);
	if (!handle)
		return -1;

	return object_media_get_camera_config(svc->media_hdl, camera_id, cfg);
}

int object_svc_start(void *handle)
{
	object_svc_t *svc = (object_svc_t *) handle;
	int ret;

	ALOGV("enter %s\n", __func__);
	ret = object_media_start(svc->media_hdl);
	if (ret) {
		ALOGE("object media start failed");
		return -1;
	}

	ret = object_media_set_cb(svc->media_hdl, OBJECT_CAMERA_RGB, svc,
			object_svc_rgb_frame_cb);
	if (ret) {
		ALOGE("object_media_rgb_set_cb failed");
		return -1;
	}

	return 0;
}

int object_svc_stop(void *handle)
{
	object_svc_t *svc = (object_svc_t *) handle;
	int ret;

	ALOGV("enter %s\n", __func__);
	ret = object_media_stop(svc->media_hdl);
	if (ret) {
		ALOGE("object media stop failed");
		return -1;
	}

	return 0;
}

int object_svc_deinit(void *handle)
{
	object_svc_t *svc = (object_svc_t *) handle;
	int ret;

	ALOGV("enter %s\n", __func__);

	vgg_close(svc->vgg_fn);

	ret = object_media_destroy(svc->media_hdl);
	if (ret)
		ALOGE("object media destroy failed");

	if (svc) {
		free(svc);
		svc = NULL;
	}

	return 0;
}

int object_register_det_cb(void *handle, object_det_cb cb)
{
	object_svc_t *svc = (object_svc_t *) handle;

	ALOGV("enter %s\n", __func__);

	if (!svc) {
		ALOGE("object service not init\n");
		return -1;
	}

	if (!cb) {
		ALOGE("cb cannot be NULL\n");
		return -1;
	}

	svc->det_cb = cb;

	return 0;
}

int object_register_det_rgb_cb(void *handle, object_det_rgb_cb cb)
{
	object_svc_t *svc = (object_svc_t *) handle;

	ALOGV("enter %s\n", __func__);

	if (!svc) {
		ALOGE("object service not init\n");
		return -1;
	}

	if (!cb) {
		ALOGE("cb cannot be NULL\n");
		return -1;
	}

	svc->det_rgb_cb = cb;

	return 0;
}

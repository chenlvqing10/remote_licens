/*
 * object_svc_main.h
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

#ifndef _OBJECT_SVC_MAIN_H_
#define _OBJECT_SVC_MAIN_H_

#include <pthread.h>

#include <nna_api.h>
#include <vgg_api.h>

#include "object_svc.h"
#include "queue.h"
#include "list.h"

#define DEFAULT_CAMERAS_TS_DIFF 66666

typedef struct _object_svc {
	void *media_hdl;

	void *rgb_stream;
	head_t rgb_frame_queue;
	pthread_mutex_t rgb_frame_lock;
	pthread_cond_t rgb_frame_cond;
	pthread_t object_det_thrd;

	pthread_mutex_t rgb_det_lock;

	/* det result */
	det_results_t det_results;
	list_head_t det_result_list;
	pthread_mutex_t det_result_lock;

	object_info_t *object_info;

	/* service configuration */
	int rgb_buffer_width;
	int rgb_buffer_height;
	int recg_x;
	int recg_y;
	int recg_width;
	int recg_height;
	int rgb_x_margin;
	int rgb_y_margin;
	int det_step;
	int object_language;
	vgg_fn_t *vgg_fn;

	object_det_cb det_cb;
	object_det_rgb_cb det_rgb_cb;

	int debug_log;
	int dump_yuv;
	int dump_fps;
	char label[1000][128];
} object_svc_t;

typedef struct _rgb_det_buf {
	object_frame_t *frame;

	nna_feature_map_t map;
} rgb_det_buf_t;

void free_rgb_det_buf(object_svc_t *svc, rgb_det_buf_t **pp);

typedef struct _rgb_recg_buf {
	object_frame_t *frame;

	object_info_t object_info;
} rgb_recg_buf_t;

void object_svc_load_conf(object_svc_t *svc);

void free_frame(object_frame_t **pp);

long long get_systime_us(void);

#endif /* _OBJECT_SVC_MAIN_H_ */

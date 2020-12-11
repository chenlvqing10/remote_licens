/*
 * object_svc.h
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

#ifndef _OBJECT_SVC_H_
#define _OBJECT_SVC_H_

#define FACE_SVC_OK   0
#define FACE_SVC_ERR -1

#include "queue.h"
#include "list.h"

/*
 * init object service
 *
 * return the handle of object service
 */
void *object_svc_init(void);

/*
 * start object service
 *
 * handle: the handle of object service
 */
int object_svc_start(void *handle);

/*
 * stop object service
 *
 * handle: the handle of object service
 */
int object_svc_stop(void *handle);

/*
 * deinit object service
 *
 * handle: the handle of object service
 */
int object_svc_deinit(void *handle);

enum object_frame_type {
	OBJECT_FRAME_TYPE_RGB,
	OBJECT_FRAME_TYPE_NIR,
	OBJECT_FRAME_TYPE_API_DET,
};

enum object_camera_id {
	OBJECT_CAMERA_RGB,
	OBJECT_CAMERA_NIR,
	OBJECT_CAMERA_MAX
};

typedef struct _camera_cfg {
	int width;
	int height;
} camera_cfg_t;

typedef struct _object_frame {
	enum object_frame_type type;
	unsigned char *viraddr[3];
	unsigned long phyaddr[3];
	int size[3];
	int width;
	int height;
	int color_fmt;
	long long time_stamp; /* timestamp from v4l2 */

	long long det_begin;
	long long det_end;

	void *priv;
} object_frame_t;

typedef int (*p_object_img_cb)(object_frame_t *frame);

int object_return_frame(void *handle, object_frame_t *f_frame);
int object_get_camera_config(void *handle, enum object_camera_id camera_id,
						camera_cfg_t *cfg);

typedef struct _object_rect {
	/* object position [left, top, right, bottom] */
	int rect[4];
	/* the object position after track and smooth */
	int rect_smooth[4];
	/* object detect socre */
	int score;
	/* same id when the same person */
	int id;

	float kpts[10];
} object_rect_t;

typedef struct _object_rects {
	/* object rect array */
	object_rect_t *p_rect;
	/* object rect count */
	int cnt;

	int error;
} object_rects_t;

#define OBJECT_FEAT_SIZE 1024
typedef struct _object_feat {
	/* object feature array */
	signed char values[OBJECT_FEAT_SIZE];
	/* similarity */
	float score;
} object_feat_t;

typedef struct _feat {
	signed char values[OBJECT_FEAT_SIZE];
} feat_t;

typedef struct _kpts {
	float kpts[10];
} kpts_t;

typedef struct _feats {
	/* objects key points on register */
	kpts_t *kpts;
	/* object count */
	int cnt;
} feats_t;

typedef struct _object_info {
	/* object rect */
	object_rect_t rect;

	int error;
} object_info_t;

typedef struct _det_result {
	struct list_head node;
	object_rect_t rect;
	char label[32];
} det_result_t;

typedef struct _det_results {
	int num;
	det_result_t *result;
} det_results_t;
int objects_set_det_result(void *arg, void *cb_object_rects);
void *objects_get_det_result(void *arg);
typedef void *(*object_det_rgb_cb)(object_frame_t *, object_rects_t *);
int object_register_det_rgb_cb(void *handle, object_det_rgb_cb cb);
typedef void *(*object_det_cb)(object_frame_t *, object_rects_t *);
typedef void *(*object_recg_cb)(object_frame_t *, object_info_t *);

/*
 * register detect result callback for app
 *
 * cb: void *object_det_cb(object_frame_t *frame, object_rects_t *info)
 */
int object_register_det_cb(void *handle, object_det_cb cb);

#endif /* _OBJECT_SVC_H_ */

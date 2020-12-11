/*
 * camera_svc_media.h
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

#ifndef _CAMERA_SVC_MEDIA_H_
#define _CAMERA_SVC_MEDIA_H_

#include <media_common.h>
#include "video_recorder.h"

typedef struct camera_para {
	int width;
	int height;
} camera_para_t;

typedef struct preview_para {
	int top;
	int left;
	int width;
	int height;
	vdisp_mode_e mode;
	vdisp_rotate_mode_e rot;
} preview_para_t;

typedef struct recorder_para {
	char *source;
	camera_para_t cam_para;
	preview_para_t prev_para;
	enc_para_t enc_para;
	int prev_flag;
	int enc_flag;
} recorder_para_t;

void *get_camera_handle(void);
void release_camera_handle(void *hdl);
int start_camera(void *hdl, char *src, camera_para_t *para);
int stop_camera(void *hdl);
int start_cam_preview(void *hdl, preview_para_t *para);
int stop_cam_preview(void *hdl);
int start_cam_record(void *hdl, enc_para_t *para, const char *filename);
int stop_cam_record(void *hdl);
int camera_take_pic(void *handle, char *file);

#endif /* _CAMERA_SVC_MEDIA_H_ */

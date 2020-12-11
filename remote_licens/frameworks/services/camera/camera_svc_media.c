/*
 * face_svc_media.c
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

#define LOG_TAG "camera_svc"
#include <log/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "camera_svc_media.h"
#include "video_stream.h"
#include "audio_stream.h"

typedef struct _camera_svc {
	void *astream;
	void *vstream;
	void *recorder;
} camera_svc_t;


void *get_camera_handle(void)
{
	camera_svc_t *hdl = malloc(sizeof(camera_svc_t));
	if (hdl == NULL)
		return NULL;

	memset(hdl, 0, sizeof(camera_svc_t));
	hdl->vstream = video_stream_create();
	hdl->astream = audio_stream_creat();
	hdl->recorder = video_rec_creat();
	video_rec_set_video_stream(hdl->recorder, hdl->vstream);
	video_rec_set_audio_stream(hdl->recorder, hdl->astream);

	return hdl;
}

void release_camera_handle(void *hdl)
{
	camera_svc_t *camera_svc_hdl = hdl;
	if (hdl == NULL)
		return;

	if (camera_svc_hdl->astream!= NULL)
		audio_stream_release(camera_svc_hdl->astream);
	if (camera_svc_hdl->vstream != NULL)
		video_stream_release(camera_svc_hdl->vstream);
	if (camera_svc_hdl->recorder != NULL)
		video_rec_release(camera_svc_hdl->recorder);
	free(hdl);
}

int start_camera(void *hdl, char *src, camera_para_t *para)
{
	int ret = 0;
	vsrc_param_t src_para;
	camera_svc_t *camera_svc_hdl = hdl;

	if (hdl == NULL)
		return -1;
	ret = video_stream_set_camera_source(camera_svc_hdl->vstream, src);
	if (ret < 0) {
		ALOGE("LB_REC_SET_VIDEO_SOURCE error\n");
		return -1;
	}
	memset(&src_para, 0, sizeof(vsrc_param_t));
	src_para.height = para->height;
	src_para.width = para->width;
	ret = video_stream_set_src_para(camera_svc_hdl->vstream, &src_para);
	if (ret < 0) {
		ALOGE("LB_REC_SET_PARA error\n");
		return -1;
	}
	ret = video_stream_start_camera(camera_svc_hdl->vstream);
	if (ret < 0) {
		ALOGE("LB_REC_PREPARE error\n");
		return -1;
	}
	return 0;
}

int stop_camera(void *hdl)
{
	int ret;
	camera_svc_t *camera_svc_hdl = hdl;

	if (hdl == NULL)
		return -1;
	ret = video_stream_stop_camera(camera_svc_hdl->vstream);
	return ret;
}

int start_cam_preview(void *hdl, preview_para_t *para)
{
	int ret = 0;
	win_para_t disp_para;
	camera_svc_t *camera_svc_hdl = hdl;

	if (hdl == NULL)
		return -1;
	disp_para.rect.height = para->height;
	disp_para.rect.width = para->width;
	disp_para.rect.x = para->left;
	disp_para.rect.y = para->top;
	disp_para.mode = para->mode;

	ret = video_stream_set_disp_para(camera_svc_hdl->vstream, &disp_para, para->rot);
	if (ret < 0) {
		ALOGE("LB_REC_SET_DISP_PARA error");
		return -1;
	}
	
	ret = video_stream_start_preview(camera_svc_hdl->vstream);
	if (ret < 0) {
		ALOGE("LB_REC_PREVIEW error");
		return -1;
	}

	return 0;
}

int stop_cam_preview(void *hdl)
{
	int ret = 0;
	camera_svc_t *camera_svc_hdl = hdl;

	if (hdl == NULL)
		return -1;
	ret = video_stream_stop_preview(camera_svc_hdl->vstream);
	if (ret < 0) {
		ALOGE("LB_REC_STOP_PREVIEW error");
		return -1;
	}
	return 0;
}

int start_cam_record(void *hdl, enc_para_t *para, const char *filename)
{
	int ret = 0;
	camera_svc_t *camera_svc_hdl = hdl;

	if (hdl == NULL)
		return -1;

	ret = video_rec_set_output_file(camera_svc_hdl->recorder, filename);
	if (ret < 0) {
		ALOGE("LB_REC_SET_OUTPUT_FILE error\n");
		return -1;
	}
	ret = video_rec_set_para(camera_svc_hdl->recorder, para);
	if (ret < 0) {
		ALOGE("LB_REC_SET_ENC_PARA error\n");
		return -1;
	}
	ret = video_rec_start(camera_svc_hdl->recorder);
	if (ret < 0) {
		ALOGE("LB_REC_START error\n");
		return -1;
	}

	return 0;
}

int stop_cam_record(void *hdl)
{
	camera_svc_t *camera_svc_hdl = hdl;

	if (hdl == NULL)
		return -1;

	if (video_rec_stop(camera_svc_hdl->recorder) < 0) {
		ALOGE("LB_REC_STOP error\n");
		return -1;
	}
	return 0;
}


int camera_take_pic(void *hdl, char *file)
{
	int ret;
	camera_svc_t *camera_svc_hdl = hdl;

	ret = video_rec_take_picture(camera_svc_hdl->recorder, file);

	return ret;
}


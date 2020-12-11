/*
 * jpeg_enc_test_nv12.c
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "JPEG_ENC"
#include "log/log.h"

#include "video_stream.h"

#define RGB_CAMERA_SOURCE	"/dev/video3"
#define RGB_CAMERA_SIZE_WIDTH	1920 /* 1408 */
#define RGB_CAMERA_SIZE_HEIGHT	1080 /* 800 */
#define NIR_CAMERA_SOURCE	"/dev/video1"
#define NIR_CAMERA_SIZE_WIDTH	800
#define	NIR_CAMERA_SIZE_HEIGHT	600


#define SCREEN_WIDTH		1280 /* 800 */
#define SCREEN_HEIGHT		720 /* 1280 */


static frame_t *pgframe;

static int take_picture(void *handle, frame_t *frame)
{
	printf("width: %d\n", frame->info.video.width);
	printf("height: %d\n", frame->info.video.height);
	printf("addr: %p - %p - %p\n", frame->info.video.addr[0],
		frame->info.video.addr[1], frame->info.video.addr[2]);
	printf("size: %d - %d - %d\n", frame->info.video.size[0],
		frame->info.video.size[1], frame->info.video.size[2]);
	printf("time_stamp: %lld\n", frame->info.video.time_stamp);
	pgframe = frame;
	return 0;
}
int main(int argc, char *argv[])
{
	void *hdl;
	int ret;
	app_frame_cb_t app_frame_cb;
	win_para_t disp_para;

	/* initialize media framework */
	ret = media_init();
	if (ret) {
		printf("media init failed\n");
		return -1;
	}

	hdl = video_stream_create();
	if (!hdl) {
		printf("stream create failed\n");
		return -1;
	}

	ret = video_stream_set_camera_source(hdl, RGB_CAMERA_SOURCE);
	if (ret) {
		printf("video_stream_set_camera_source failed\n");
		return -1;
	}

	ret = video_stream_set_camera_size(hdl,
				RGB_CAMERA_SIZE_WIDTH, RGB_CAMERA_SIZE_HEIGHT);
	if (ret) {
		printf("video_stream_set_camera_size failed\n");
		return -1;
	}

	ret = video_stream_set_rotate(hdl, VDISP_ROTATE_NONE/* VDISP_FLIP_V_ROT_90 */);
	if (ret) {
		printf("video_stream_set_rotate failed\n");
		return -1;
	}

	ret = video_stream_start_camera(hdl);
	if (ret) {
		printf("video_stream_start_camera failed\n");
		return -1;
	}

	memset(&disp_para, 0, sizeof(disp_para));
	disp_para.rect.x = 0;
	disp_para.rect.y = 0;
	disp_para.rect.width = SCREEN_WIDTH;
	disp_para.rect.height = SCREEN_HEIGHT;
	disp_para.mode = VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO;
	ret = video_stream_set_disp_para(hdl, &disp_para, 0);
	if (ret) {
		printf("video_stream_set_disp_para failed\n");
		return -1;
	}

	ret = video_stream_prepare_preview(hdl);
	if (ret) {
		printf("video_stream_prepare_preview failed\n");
		return -1;
	}

	ret = video_stream_start_preview(hdl);
	if (ret) {
		printf("video_stream_start_preview failed\n");
		return -1;
	}

	{
		memset(&app_frame_cb, 0, sizeof(app_frame_cb_t));
		app_frame_cb.buf_handle = take_picture;
		app_frame_cb.type = VS_IMAGE_FRAME;
	}

	while (1) {
		usleep(1000000);
		printf("video_stream_take_picture\n");
		video_stream_take_picture(hdl, &app_frame_cb);
		usleep(100000);
		video_stream_return_frame(pgframe);
	}
	return 0;
}


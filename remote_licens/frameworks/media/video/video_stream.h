/*
 * media_core.h - Standard functionality for lombo recorder.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __VIDEO_STREAM_H_
#define __VIDEO_STREAM_H_

#include <media_common.h>
#include <vdisp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct vsize {
	int   width;
	int   height;
} vsize_t;

typedef struct win_para {
	vdisp_mode_e mode;
	win_rect_t rect;  /* Window postion and size in the csreen */
	win_rect_t crop;  /* Need display image pos and size in the framebuffer,
			   * default full image */
} win_para_t;

typedef struct vsrc_param {
	int height;
	int width;
	int frame_rate;
} vsrc_param_t;

typedef struct vsrc_res {
	int nbuf_cam;
	int nbuf_rot;
} vstream_buf_cfg_t;

#define video_stream_prepare_preview(hdl) (0)
#define video_stream_release_preview(hdl) (0)

void *video_stream_create(void);
void video_stream_release(void *handle);
int video_stream_start_camera(void *handle);
int video_stream_stop_camera(void *handle);
int video_stream_set_camera_source(void *handle, char *src_name);
int video_stream_set_isp_inf_source(void *handle, char *src_name);
int video_stream_set_rotate(void *handle, vdisp_rotate_mode_e rotation);
int video_stream_set_camera_size(void *handle, int width, int height);
int video_stream_take_picture(void *handle, app_frame_cb_t *cb);
int video_stream_pause(void *handle);
int video_stream_resume(void *handle);
int video_stream_start_preview(void *handle);
int video_stream_stop_preview(void *handle);
int video_stream_set_layer_level(void *handle, video_layer_level_t level);
int video_stream_set_frame_cb(void *handle, app_frame_cb_t *cb);
int video_stream_set_disp_para(void *handle, win_para_t *disp_para, int rotate);
int video_stream_set_scale_mode(void *handle, scale_mode_e mode);
int video_stream_disable_display(void *handle);
int video_stream_set_src_para(void *handle, vsrc_param_t *para);
int video_stream_get_src_para(void *handle, vsrc_param_t *para);
int video_stream_return_frame(frame_t *frame);
int video_stream_set_port_cb(void *handle, app_port_filter_t *cb);
int video_stream_set_res(void *handle, vstream_buf_cfg_t *res);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


/*
 * recorder_private.h - Standard functionality for lombo recorder.
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

#ifndef __RECORDER_PRIVATE_H_
#define __RECORDER_PRIVATE_H_
#include "watermark/watermark.h"
#include "vdisp.h"
#include "audio_stream.h"
#include "video_stream.h"

#define REC_DISPLAY_DEVICE_NAME	"disp"

#define PANO_DEFAULT_BUF_NUM 4
#define ENCODER_DEFAULT_BUF_NUM 8
#define ENCODER_BUF_SIZE (800*1024)

#define LB_FILE_SYSTEM_PRIO 16
#define LB_RECORDER_SINK_PRIO 15
#define LB_RECORDER_MUXER_PRIO 16
#define LB_RECORDER_AUDIO_PRIO 17
#define LB_RECORDER_VSRC_PRIO 17
#define LB_RECORDER_VSPLIT_PRIO 17
#define LB_RECORDER_VIDEO_PRIO 17
#define LB_FILE_CLOSE_PRIO 24


typedef enum rec_state {
	REC_STATE_ERR = 0,
	REC_STATE_INIT,
	REC_STATE_PREPARED,
	REC_STATE_PREVIEW,
	REC_STATE_RECORD
} rec_state_t;

typedef struct __venc_info {
	comp_info_t al_comp;
	video_encoder_t venc_format;
	const char *role;
	port_info_t *vin;
	port_info_t *vout;
	int slice_mode;
	int bitrate;
	int quality;
	int out_width;
	int out_height;
	win_rect_t venc_rect;
	int rotate;
} __venc_info_t;

typedef struct al_muxer_private {
	void *app_data;
	fix_duration_param_t fix_duration_param;
} al_muxer_private_t;

typedef struct mux_info {
	comp_info_t al_comp;
	port_info_t *ain;
	port_info_t *vin;
	output_format_t fmt;
	char *filename;
} mux_info_t;

typedef struct video_recorder {
	pthread_mutex_t lock;
	rec_state_t state;
	port_info_t *src_venc;
	port_info_t *src_pic;
	port_info_t *src_amux;

	void *vstream;
	astream_handle audio;

	__venc_info_t venc_info;
	mux_info_t mux_info;
	s64 aref_time;
	s64 vref_time;

	void *pic_private;
	void *watermark_private;
	app_stream_cb_t stream_cb;
	app_stream_cb_t colli_cb;
	al_muxer_private_t mux_priv;
} video_recorder_t;

watermark_priv_t *watermark_init(void);
void watermark_deinit(void *handle);
int watermark_set_bnp(watermark_priv_t *priv, numeral_input_t *bnp);
numeral_output_t *watermark_set_mark(watermark_priv_t *priv,
				numeral_picture_index_t *wm_config);
int watermark_get_markarray(watermark_priv_t *priv, numeral_output_t **mark_array);
int watermark_put_markarray(watermark_priv_t *priv, numeral_output_t *mark_array);

int take_pic_init(video_recorder_t *video_rec);
void take_pic_deinit(video_recorder_t *video_rec);
int take_pic_start(video_recorder_t *video_rec, char *file);

port_info_t *audio_stream_getport(astream_handle hdl, as_port_type_e type);
int audio_stream_putport(astream_handle hdl, port_info_t *port);
int audio_stream_disable_port(void *hdl, port_info_t *port);
int audio_stream_enable_port(void *hdl, port_info_t *port);
int audio_stream_inc_refcnt(astream_handle hdl);

void *video_stream_get_camera(void *handle);
int video_stream_inc_refcnt(void *handle);
int audio_stream_start(astream_handle hdl);
int audio_stream_stop(astream_handle hdl);
port_info_t *audio_stream_start_enc(astream_handle hdl);
int audio_stream_stop_enc(astream_handle hdl, port_info_t *enc_port);

#endif


/*
 * video_recorder.h - Standard functionality for video recorder.
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

#ifndef __VIDEO_RECORDER_H_
#define __VIDEO_RECORDER_H_

#include "media_common.h"
#include "vdisp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef s64
typedef signed long long s64;
#endif

typedef enum recorder_state {
	RECORDER_STATE_ERR = 0,
	RECORDER_STATE_INIT,
	RECORDER_STATE_PREPARED,
	RECORDER_STATE_RECORD,
} recorder_state_t;

typedef enum OUTPUT_FORMAT {
	REC_OUTPUT_FORMAT_NULL = 0,
	REC_OUTPUT_FORMAT_MP4 = 1,
	REC_OUTPUT_FORMAT_MOV = 2,
	REC_OUTPUT_FORMAT_RAW = 3,
	REC_OUTPUT_FORMAT_TS = 4,
	REC_OUTPUT_FORMAT_AUDIO_AAC = 0x100,
	REC_OUTPUT_FORMAT_AUDIO_PCM,
	REC_OUTPUT_FORMAT_NONE = 0x7fff
} output_format_t;

typedef enum VIDEO_ENCODER {
	VIDEO_ENCODER_INVALID = 0,
	VIDEO_ENCODER_H264,
	VIDEO_ENCODER_MJPEG,
	VIDEO_ENCODER_H265,
	VIDEO_ENCODER_NONE = 0x7fff
} video_encoder_t;

typedef struct rec_param {
	/* audio para */
	int audio_channels;		/* Number of channels (e.g. 2 for stereo) */
	int audio_sample_width;	/* Bit per sample */
	int audio_sample_rate;	/* Sampling rate of the source data.
				   Use 0 for variable or unknown sampling rate. */

	/* video para */
	int source_height;
	int source_width;
	int frame_rate;
	vdisp_rotate_mode_e rotate;

	/* encoder para */
	win_rect_t enc_rect;
	audio_encoder_t aenc_format;
	video_encoder_t venc_format;
	int height;
	int width;
	vdisp_rotate_mode_e enc_rot;
	int bitrate;
	output_format_t file_fmt;
} rec_param_t;

typedef struct fix_duration_param {
	int file_duration;
	int (*cb_get_next_file)(void *hdl, char *next_file);
	int (*cb_file_closed)(void *hdl, char *file_name);
} fix_duration_param_t;

/* watermark definitions */
#define MAX_BLENDING_PICTURES 21

typedef struct {
	unsigned char *data;
	unsigned long phy_addr;
	unsigned int width;
	unsigned int height;
	unsigned int picture_size;
	unsigned int stride;
} numeral_picture_t;

typedef struct {
	numeral_picture_t numeral_picture[MAX_BLENDING_PICTURES];
	unsigned int input_picture_num;
	unsigned int colorspace;
} numeral_input_t;

typedef struct {
	char index_array[64];
	unsigned int total_index_num;
	unsigned int start_x_pos;
	unsigned int start_y_pos;
	unsigned int blending_area_index;
} numeral_picture_index_t;

typedef struct enc_para {
	int width;
	int height;
	int bitrate;
	int slice_mode;
	int rotate;
	output_format_t fmt;
	int quality;
	audio_encoder_t aenc_format;
	video_encoder_t venc_format;
} enc_para_t;

void *video_rec_creat(void);
void video_rec_release(void *hdl);
int video_rec_take_picture(void *hdl, char *file);
int video_rec_wm_set_bnp(void *hdl, numeral_input_t *bnp);
int video_rec_wm_set_mark(void *hdl, numeral_picture_index_t *mk);
int video_rec_start(void *hdl);
int video_rec_stop(void *hdl);
int video_rec_set_output_file(void *hdl, const char *filename);
int video_rec_set_output_format(void *hdl, output_format_t fmt);
int video_rec_set_fix_duration_param(void *hdl,
	void *contex, fix_duration_param_t *param);
int video_rec_set_para(void *hdl, enc_para_t *rec_para);
int video_rec_set_enc_area(void *hdl, win_rect_t *area);
int video_rec_get_para(void *hdl, enc_para_t *rec_para);
int video_rec_get_time(void *hdl, int *time);
int video_rec_get_status(void *hdl, int *status);
int video_rec_reset(void *hdl);
int video_rec_set_video_stream(void *hdl, void *vstream);
int video_rec_set_audio_stream(void *hdl, void *astream);
int video_rec_set_stream_cb(void *hdl, app_stream_cb_t *cb);
int video_rec_collision_enable(void *hdl, long collision_cache_time);
int video_rec_collision_disable(void *hdl);
int video_rec_collision_start(void *hdl, colli_file_cfg_t *cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


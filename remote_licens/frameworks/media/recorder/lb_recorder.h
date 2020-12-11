/*
 * lb_recorder.h - Standard functionality for lombo recorder.
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

#ifndef __LB_RECORDER_H_
#define __LB_RECORDER_H_

#include "media_common.h"
#include "vdisp.h"
#include "video_recorder.h"

typedef enum LB_REC_TYPE {
	LB_REC_INVALID =  0,

	LB_REC_PREPARE,
	LB_REC_PREVIEW,
	LB_REC_STOP_PREVIEW,
	LB_REC_START,
	LB_REC_STOP,
	LB_REC_RESET,
	LB_REC_PAUSE,
	LB_REC_GET_TIME,		/* int * */
	LB_REC_GET_STATUS,		/* recorder_state_t */

	LB_REC_SET_PARA,		/* rec_param_t */
	LB_REC_GET_PARA,		/* rec_param_t */
	LB_REC_SET_SRC_RES,

	/* source */
	LB_REC_SET_VIDEO_SOURCE,	/* ptr: source dev name */
	LB_REC_SET_CAP_RATE,		/* capture frame rate */
	LB_REC_SET_WATERMARK_SOURCE,	/* numeral_input_t */
	LB_REC_SET_WATERMARK_INDEX,	/* numeral_picture_index_t */

	/* encode */
	LB_REC_SET_OUTPUT_FILE,		/* ptr: output filename */
	LB_REC_SET_OUTPUT_FORMAT,	/* output_format_t */
	LB_REC_SET_FIX_DURATION_PARA,	/* fix_duration_param_t */
	LB_REC_SET_ENC_PARA,	/* rec_enc_para_t */

	LB_REC_TAKE_PICTURE,	/* filename */

	/* preview */
	LB_REC_SET_SINK,	/* ptr: display dev name */
	LB_REC_SET_ROTATE,	/* vdisp_rotate_mode_e */
	LB_REC_SET_CB_ROT,
	LB_REC_SET_CB_FRAME,	/* app_frame_cb_t */
	LB_REC_FREE_FRAME,	/* al_frame_t */
	LB_REC_SET_LAYER_LEVEL, /* video_layer_level_t */
	LB_REC_SET_DISP_PARA,	/* win_para_t */
	LB_REC_SET_DISP_SCALE_MODE, /* scale_mode_e */
	LB_REC_SET_STREAM_CB,   /* app_stream_cb_t */
	LB_REC_SET_PORT_FILTER,	/* app_stream_cb_t */

	/* audio */
	LB_REC_SET_AUDIO_SOURCE,	/* ptr: source dev name */
	LB_REC_SET_AUDIO_MUTE,		/* 0:disable mute; 1:mute */

	/* panoramic */
	LB_REC_PANO_CREAT,
	LB_REC_PANO_RELEASE,
	LB_REC_PANO_START,
	LB_REC_PANO_STOP,
	LB_REC_PANO_SET_LAYER_LEVEL, /* video_layer_level_t */
	LB_REC_PANO_CALI_PROCESS,
	LB_REC_PANO_SET_PREVIEW_SIZE,
	LB_REC_PANO_SET_DISP_MODE,
	LB_REC_PANO_SET_CALI_PARA,
	LB_REC_PANO_SET_INIT_PARA,
	LB_REC_PANO_SET_CALI_DATA,
	LB_REC_PANO_GET_CALI_DATA,
} lombo_rec_cmd_t;

static inline const char *media_cmd_as_string(lombo_rec_cmd_t i)
{
	switch (i) {
	case LB_REC_PREPARE:
		return "cmd_prepare";
	case LB_REC_PREVIEW:
		return "cmd_preview";
	case LB_REC_STOP_PREVIEW:
		return "cmd_stop_preview";
	case LB_REC_START:
		return "cmd_start";
	case LB_REC_STOP:
		return "cmd_stop";
	case LB_REC_PAUSE:
		return "cmd_pause";
	case LB_REC_GET_TIME:
		return "cmd_get_time";
	case LB_REC_GET_STATUS:
		return "cmd_get_stat";
	case LB_REC_SET_PARA:
		return "cmd_set_para";
	case LB_REC_GET_PARA:
		return "cmd_get_para";
	case LB_REC_SET_VIDEO_SOURCE:
		return "cmd_set_video_source";
	case LB_REC_SET_CAP_RATE:
		return "cmd_set_cap_rate(frame rate)";
	case LB_REC_SET_OUTPUT_FILE:
		return "cmd_set_outpu_file";
	case LB_REC_SET_CB_FRAME:
		return "cmd_set_cb_frame";
	case LB_REC_SET_LAYER_LEVEL:
		return "cmd_set_layer_level";
	case LB_REC_SET_SINK:
		return "cmd_set_sink";

	default:
		break;
	}
	return "unknown media cmd??";
}

void lb_recorder_release(void *hdl);
void *lb_recorder_creat(void);
int lb_recorder_ctrl(void *hdl, int cmd, void *para);

#endif


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

#ifndef __AUDIO_STREAM_H_
#define __AUDIO_STREAM_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <media_common.h>

typedef void *astream_handle;

typedef enum as_port_type {
	AS_PORT_PCM	= 0,
	AS_PORT_CODEC	= 1,
} as_port_type_e;

astream_handle audio_stream_creat(void);
void audio_stream_release(astream_handle hdl);
int audio_stream_set_coding_type(astream_handle hdl, audio_encoder_t coding);
int audio_stream_set_prio(astream_handle hdl, int prio);
int audio_stream_set_channels(astream_handle hdl, int channels);
int audio_stream_set_aec(astream_handle hdl, int on);
int audio_stream_set_sample_width(astream_handle hdl, int bits);
int audio_stream_set_sample_rate(astream_handle hdl, int rate);
signed long long audio_stream_get_time(astream_handle hdl);
int audio_stream_get_sample_rate(astream_handle hdl);
int audio_stream_get_sample_width(astream_handle hdl);
int audio_stream_get_channels(astream_handle hdl);
audio_encoder_t audio_stream_get_coding_type(astream_handle hdl);
int audio_stream_set_mute(astream_handle hdl, int mute_on);
int audio_stream_start(astream_handle hdl);
int audio_stream_stop(astream_handle hdl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


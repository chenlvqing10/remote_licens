/*
 * adec_common.h - Standard functionality for the audio encodec component.
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

#ifndef __ADEC_COMMON_H__
#define __ADEC_COMMON_H__

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "oscl.h"
#include "decode_com.h"
#include "raw_stream.h"
#include <omx_vendor_lb.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct audio_packet {
	uint8_t *data;
	size_t size;
	int64_t timestamp;
	uint32_t offset;
	int flags;
} audio_packet_t;

typedef struct adec_private {
	int64_t nFrames;
	audio_dec_t paudio;
	raw_buf_manager buf;
	omx_audio_info_t info;
	void *dl_hdl;

	void *extradata;
	int extrasize;
	int adec_opened;
	int fmt_changed;
	int is_err_state;
	int dec_err_cnt;
	int underflow;

	audio_packet_t pkt;
	pthread_mutex_t lock;
	int is_decoding_flag;
} adec_private_t;

#ifndef __linux__
void rm(const char *filename);
#endif

adec_private_t *adec_init(void);
int adec_deinit(adec_private_t *audio_private);
int adec_frame(adec_private_t *audio_private,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf);
int adec_open(adec_private_t *audio_private);
int adec_close(adec_private_t *audio_private);
int adec_flush(adec_private_t *audio_private);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __ADEC_COMMON_H__ */

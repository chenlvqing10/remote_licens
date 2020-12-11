/*
 * muxer_common.h - Standard functionality for the audio encodec component.
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

#ifndef __MUXER_HANDLE_H__
#define __MUXER_HANDLE_H__

#include "lombo_muxer.h"
#include "muxer_component.h"
#include "muxer_private.h"

#define MUXER_NUM 4
#define AUDPTS_TO_TIME(context, pts) \
	(pts * 1000 / context->stream_cfg.para[AUDIO_INDEX].sample_rate)

typedef struct muxer_stream_cfg {
	int nstream;
	int exist[2];
	int is_ready[2];
	AV_CodecParameters para[2];
} muxer_stream_cfg_t;

typedef struct muxer_out_cfg {
	av_muxer_type_e id;
	char *filename;
	external_stream_writer_t *writer;
} muxer_out_cfg_t;

typedef struct _muxer_hdl {
	format_muxer *muxer;
	AVIOFILE fp;
	char *filename;
	int muxer_status;
	OMX_S64 start_pts[2];
	OMX_S64 cur_time[2];
	void *component;
} muxer_hdl_t;

typedef struct muxer_hdl_info {
	int num;
	pthread_mutex_t lock;
	muxer_hdl_t muxer_hdl[MUXER_NUM];
} muxer_hdl_mgr_t;

typedef struct context_buf_mgr {
	int64_t ref_time[2]; /* ref time for inbuffer manager, not for muxing */
	int64_t intime[2];
	int64_t sync_time;
	OMX_BUFFERHEADERTYPE *vbuffer;
	OMX_BUFFERHEADERTYPE *abuffer;
	int rebuilding_flag;
	muxer_vpacket_t combine_packet;
} context_buf_mgr_t;

#define CONTEXT_STOP_MASK 0x1
#define CONTEXT_FILENAME_MASK 0x2

typedef struct MuxerContext {
	pthread_mutex_t context_mutex;
	void *lb_comp;
	muxer_hdl_mgr_t mux_hdl_mgr;
	muxer_stream_cfg_t stream_cfg;

	/* para for avsync */
	int64_t ref_time[2]; /* ref time for inbuffer manager, not for muxing */
	int64_t intime[2];
	int64_t sync_time;
	OMX_BUFFERHEADERTYPE *vbuffer;
	OMX_BUFFERHEADERTYPE *abuffer;
	int rebuilding_flag;
	OMX_BUFFERHEADERTYPE *combine_header;
	muxer_vpacket_t combine_packet;

	/* normal muxer info */
	av_muxer_type_e id;
	int file_total_time;
	char *filename;
	external_stream_writer_t ext_writer;
	muxer_hdl_t *mux_hdl;
	OMX_S64 start_ref;
	int ctrl_flag;

	/* collision/emergency info */
	int cnt;
	void *colli_info;
} MuxerContext;

muxer_hdl_t *start_muxer_handle(muxer_hdl_mgr_t *mgr, muxer_stream_cfg_t *cfg,
	av_muxer_type_e type, char *filename, external_stream_writer_t *writer);
int stop_muxer_handle(muxer_hdl_t *muxer_handle);
int muxer_handle_mux_buffer(muxer_hdl_t *muxer_handle, OMX_BUFFERHEADERTYPE *inbuffer);
OMX_TICKS muxer_handle_get_time(muxer_hdl_t *muxer_handle);
OMX_TICKS muxer_handle_get_startpts(muxer_hdl_t *muxer_handle, int index);
void log_muxer_handle(muxer_hdl_t *hdl);


int mux_context_wait_idle(muxer_hdl_mgr_t *mgr);
MuxerContext *mux_context_init(lb_omx_component_t *component);
int mux_context_deinit(MuxerContext *context);
void mux_context_free_combined_packet(MuxerContext *context);
OMX_ERRORTYPE mux_context_return_buffer(MuxerContext *context,
	OMX_BUFFERHEADERTYPE *buffer);
int mux_context_start_normal(MuxerContext *context);
int mux_context_stop_normal(MuxerContext *context);
void log_muxer_context(MuxerContext *context);

#endif /* __MUXER_HANDLE_H__ */


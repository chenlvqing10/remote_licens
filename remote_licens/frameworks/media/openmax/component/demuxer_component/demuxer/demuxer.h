/*
 * demuxer.h - code for demuxer.
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
#ifndef __DEMUXER_H__
#define __DEMUXER_H__

#include <demuxer_plugin.h>
#include <lombo_dec_common.h>
#define EXSUB_START_STREAM_INDEX 128


typedef enum {
	close_subtile_all = 0,
	close_subtile_one_way,
} subtitle_close_e;
typedef enum {
	sub_status_closed = 0,
	sub_status_pause,
	sub_status_opened,
} subtitle_status_e;


OMX_S32 demuxer_exsub_change_status(void *handle, OMX_S32 stream_idx,
	subtitle_status_e status);
OMX_S32 demuxer_exsub_seek(void *handle, OMX_S64 st, OMX_S32 stream_idx);
OMX_STRING demuxer_exsub_probe(OMX_STRING file_url);
OMX_S32 demuxer_exsub_open(void *handle, OMX_STRING name,
	dmx_buffer_info_t *buf_info, exsub_info_t *si);
OMX_S32 demuxer_exsub_close(void *handle, exsub_info_t *si,
	subtitle_close_e type, OMX_S32 exsub_idx);

void *demuxer_open(file_stream_para_t *para, void *ioctx, int io_type,
	dmx_buffer_info_t *buf_info, dmx_mediainfo_t *mediainfo);
int demuxer_close(void *handle);
int demuxer_return_packet(void *handle, dmx_packet_t *pkt);
int demuxer_read_packet(void *handle, dmx_packet_t *pkt);
int demuxer_seek(void *handle, int64_t timestamp);
int demuxer_ctrl(void *handle, int cmd, void *arg);

#endif /* __DEMUXER_H__ */


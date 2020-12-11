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

#ifndef __MUXER_COMPONENT_H__
#define __MUXER_COMPONENT_H__

typedef struct external_stream_writer {
	void *app_data;
	int (*write)(void *, int, uint8_t *, int, int64_t);
} external_stream_writer_t;

typedef struct colli_muxfile_cfg {
	int restart;	/* A collision is inprogress: 0: return fail, 1: restart */
	int cache_time; /* time (ms) save to cache before collision happened */
	int total_time;	/* total time (ms) of collision file */
	av_muxer_type_e type;
	char *filename;	/* saved file */
	external_stream_writer_t ext_writer;
} colli_muxfile_cfg_t;

typedef struct muxer_sync_data {
	int64_t vtime;
	int64_t atime;
} muxer_sync_data_t;

#endif /* __MUXER_COMPONENT_H__ */


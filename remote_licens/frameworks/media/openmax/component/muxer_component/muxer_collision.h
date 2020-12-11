/*
 * muxer_collision.h - collision api used by muxer.
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

#ifndef __MUXER_COLLISION_H__
#define __MUXER_COLLISION_H__

typedef enum colli_state {
	COLLI_STAT_INIT = 0,
	COLLI_STAT_MUX_CACHE,
	COLLI_STAT_RECORDING,
	COLLI_STAT_INVALID,
} colli_state_e;

void *colli_init(MuxerContext *context, long colli_cache_time);
void colli_deinit(void *colli_info);
int colli_start(void *colli_info, colli_muxfile_cfg_t *colli_para);
int colli_stop(void *colli_info);
int colli_state_changed(void *colli_info);
colli_state_e colli_get_state(void *colli_info);
OMX_ERRORTYPE colli_flush(void *colli_info);
OMX_ERRORTYPE colli_cache_buffer(void *colli_info, OMX_BUFFERHEADERTYPE *buffer);
int colli_start_muxer_handle(void *colli_info, void *muxer_handle);
int colli_stop_muxer_handle(void *colli_info, void *muxer_handle);
int colli_muxer_cache(void *_info);
void colli_print_dbg(void *colli_info);

#endif /* __MUXER_COLLISION_H__ */


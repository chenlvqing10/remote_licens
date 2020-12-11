/*
 * demuxer_component.h - code for demuxer component.
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

#ifndef __DEMUXER_COMPONENT_H__
#define __DEMUXER_COMPONENT_H__

typedef struct dmx_stream_handle {
	void *priv;
	int (*fill_data)(void *hdl, int type, void *data, size_t size, int64_t timestamp);
} dmx_stream_handle_t;

OMX_ERRORTYPE demuxer_component_init(lb_omx_component_t *cmp_handle);

#endif /* __DEMUXER_COMPONENT_H__ */


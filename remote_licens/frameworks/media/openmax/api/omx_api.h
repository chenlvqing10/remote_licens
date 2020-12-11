/*
 * omx_comp_core.h - Standard functionality for lombo omx core.
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

#ifndef ___OMX_COMP_CORE_H__
#define ___OMX_COMP_CORE_H__

#include "OMX_Core.h"
#include "base_component.h"
#include "media_common.h"

#define OPENMAX_SHARED_PORT 4
#if (OPENMAX_SHARED_PORT > 8)
#error "OPENMAX_SHARED_PORT must <= 8"
#endif
#define UNTUNNEL_BUFFER_MAP_SIZE 8

#define APP_BUFFER_PASS_ON 0
#define APP_BUFFER_HANDLED 0xAA
#define APP_BUFFER_ASYNC 0xBB

typedef enum port_cb_event {
	OUTPORT_BEFORE_FILL,
	OUTPORT_FILL_DONE,
	INPORT_BEFORE_EMPTY,
	INPORT_EMPTY_DONE,
	PORT_EVENT_INVALID
} port_cb_event_e;

typedef enum {
	PORT_STATE_INIT	= 0,
	PORT_STATE_TUNNELED,
	PORT_STATE_UNTUN_SETUP,
} port_state_t;

typedef struct app_cb_private {
	void *port;
	union {
		video_frame_info_t video;
		audio_frame_info_t audio;
		subtitle_frame_info_t subtitle;
	} info;
	frame_t *cb_frame;
} app_cb_private_t;

typedef struct port_info {
	void *comp_info;
	port_state_t state;
	int index;
	OMX_DIRTYPE edir;
	OMX_PORTDOMAINTYPE domain;
	struct port_info *connect_port;
	void *tunnel_hdl;

	int nbuffer;
	int buf_size;
	int is_shared_buffer;
	OMX_BUFFERHEADERTYPE **header;
	int nbuffer_hold;
	int *hold_map;
	app_frame_cb_t cb_async;
	app_port_filter_t cb_filter;
	app_cb_private_t *priv_data;
} port_info_t;

typedef struct comp_info {
	OMX_COMPONENTTYPE *cmp_hdl;
	char *name;
	OMX_STATETYPE state;
	sem_t *sem_cmd;
	int num_port;
	port_info_t *port_info;
	pthread_mutex_t state_lock;
	void *priv_data;
	void *dbg_info;
} comp_info_t;

extern OMX_CALLBACKTYPE untunnel_common_callbacks;

#define port_name(port) (((comp_info_t *)(port->comp_info))->name)
int component_init(comp_info_t *comp_info,
	char *name,
	OMX_CALLBACKTYPE *callbacks);
void component_deinit(comp_info_t *comp_info);
OMX_ERRORTYPE component_setstate(comp_info_t *al, OMX_STATETYPE s);
OMX_ERRORTYPE component_sendcom(comp_info_t *al, OMX_COMMANDTYPE cmd,
					   OMX_U32 param, OMX_PTR cmd_data);
OMX_ERRORTYPE component_setparam(comp_info_t *comp_info,
				OMX_U32 param, OMX_PTR param_data);
int component_get_port_index(comp_info_t *comp_info,
			 OMX_DIRTYPE edir,
			 OMX_PORTDOMAINTYPE domain);
int untunnel_setup_cb(port_info_t *port, app_frame_cb_t *cb);
int untunnel_setup_ports(port_info_t *out_port, port_info_t *in_port);
void untunnel_unset_ports(port_info_t *out_port, port_info_t *in_port);
void untunnel_queue_buffers(comp_info_t *al);
OMX_ERRORTYPE untunnel_fill_buffer_done(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_BUFFERHEADERTYPE *buffer);
OMX_ERRORTYPE untunnel_empty_buffer_done(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_BUFFERHEADERTYPE *buffer);
OMX_ERRORTYPE untunnel_event_handler(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_EVENTTYPE event,
	OMX_U32 data1,
	OMX_U32 data2,
	OMX_PTR event_data);

int app_empty_buffer_done(frame_t *frame);
void untunnel_unset_cb(port_info_t *out_port);
int untunnel_set_filter(port_info_t *port, app_port_filter_t *filter);
int init_cb_info(port_info_t *port);
int deinit_cb_info(port_info_t *port);
int app_callback_handle(port_info_t *port, OMX_BUFFERHEADERTYPE *buffer,
			port_cb_event_e event);

#endif


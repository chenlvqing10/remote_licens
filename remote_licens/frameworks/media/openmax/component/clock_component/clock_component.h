/*
 * clock_component.h - Standard functionality for lombo clock component.
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
#ifndef __CLOCK_COMPONENT_H__
#define __CLOCK_COMPONENT_H__

/* Maximum number of clock ports */
#define MAX_CLOCK_PORTS				3
#define CLOCK_PORT_AUDIO			0
#define CLOCK_PORT_VIDEO			1
#define CLOCK_PORT_DEMUXER			2
/* subtitle port number must be continuous*/
#define CLOCK_PORT_SUB				3

OMX_ERRORTYPE clock_component_deinit(OMX_IN OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE clock_component_init(lb_omx_component_t *cmp_handle);

#endif /* __CLOCK_COMPONENT_H__ */


/*
 * oscl_eos_disp.h - display api used by lombo media framework.
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

#ifndef __OSCL_LINUX_DISP_H__
#define __OSCL_LINUX_DISP_H__

#include <sys/ioctl.h>
#include <poll.h>
#include <base_component.h>
#include "oscl_disp_i.h"
#include "../../../hal/include/drm_display.h"

#define DISP_FB_MAX_CNT		32

typedef struct crtc_info {
	OMX_U32 crtc_id;
	OMX_U32 crtc_width;
	OMX_U32 crtc_height;
	OMX_U32 primary_buf_id;
} crtc_info_t;

typedef struct disp_hdl {
	OMX_S8 dgb_flag;
	OMX_S8 stop_flag;

	disp_rect_t win_rect;  /* display window */
	disp_rect_t crop_rect; /* need to display source image */
	crtc_info_t crtc_info;
	disp_fb_map_index_t fb_map_table[DISP_FB_MAX_CNT];
	OMX_S32 drm_fd;
	sem_t *event_sem;
	OMX_S32 event_thread_id;
	pthread_t event_thread;

	OMX_U32 rot_handle;
	OMX_U32 scale_handle;

	OMX_S32 valid_plane;
	struct plane_data video_plane;

	OMX_BOOL disable_layer;
} disp_hdl_t;

#endif


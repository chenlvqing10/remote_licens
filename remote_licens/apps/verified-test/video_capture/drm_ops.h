/*
 * drm_ops.h - camera formation api.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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

#ifndef __DRM_OPS__H__
#define __DRM_OPS__H__

#include "drm_display.h"
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>
#include "formation.h"

#define CAM_BUF_QUEUE_SIZE		3
#define CAM_FMT_NUM_PLANES		1

struct drm_data {
	int drm_fd;
	unsigned int crtc_id;
	unsigned int crtc_width;
	unsigned int crtc_height;
	unsigned int primary_buf_id;
	unsigned int plane_id;
	unsigned int plane_idx;
	int default_primary_buf_id;
	struct framebuffer video_bufs[CAM_BUF_QUEUE_SIZE];
	int video_dbuf_fds[CAM_BUF_QUEUE_SIZE];
	struct modeset_out *modeset_list;
};

int drm_init(struct drm_data *drm_dat);

int drm_exit(struct drm_data *drm_dat, struct camera_data *cam_data);

int drm_plane_to_id(struct drm_data *drm_dat);

void drm_set_plane(struct drm_data *drm_dat,
			struct camera_data *cam_data, int buf_index);

void create_video_dma_bufs(struct drm_data *drm_dat,
				struct camera_data *cam_data,
				void *map, unsigned int size,
				int fd, int count);

void create_video_map_bufs(struct drm_data *drm_dat,
				struct camera_data *cam_data,
				int count);

#endif

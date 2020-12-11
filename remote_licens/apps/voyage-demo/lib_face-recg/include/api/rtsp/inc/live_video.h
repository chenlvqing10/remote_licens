/*
 * live_video.h - Get H264 Real-time video data interface.
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
#ifndef _LIVE_VIDEO_H
#define _LIVE_VIDEO_H

#include "face.h"

#define FIFO_NAME	"/data/H264_fifo"
#define BUFFERSIZE	PIPE_BUF

typedef int		bool;
#define true		1
#define false		0

int live_video_init(void *rgb_hdl);
void live_video_face_cb(faces_t *faces);
int live_video_deinit(void);
void live_video_set_blending_callback(void *handle);
int live_video_start_main_rec();
int live_video_stop_main_rec();

#endif /* _LIVE_VIDEO_H */


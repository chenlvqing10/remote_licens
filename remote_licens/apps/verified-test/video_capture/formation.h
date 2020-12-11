/*
 * formation.h - camera formation api.
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

#ifndef __FORMATION__H__
#define __FORMATION__H__
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <assert.h>
#include <signal.h>
#include <linux/videodev2.h>
#include <lombo_malloc.h>
#include "v4l2_camera.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define DEBUG_PRINT 0
#define DEBUG_SELECT 1

#define V4L2_DEV_NODE "/dev/video"

#define FMT_NUM_PLANES 1

struct fourcc_format {
	char *name;
	unsigned int fourcc;
	unsigned int depth;
	unsigned int colplanes;
};

struct buffer {
	void *start[FMT_NUM_PLANES];
	unsigned int fd[FMT_NUM_PLANES];
	size_t length[FMT_NUM_PLANES];
};

struct camera_data {
	unsigned int vid_idx;
	unsigned int cur_width;
	unsigned int cur_height;
	unsigned int framerate;
	int v4l2_fd;
	int buf_num;
	unsigned int cur_memory;
	unsigned int type;
	unsigned int save_frames;

	struct buffer *buffers;

	struct fourcc_format foc;
};
int set_fourcc(char *str, struct fourcc_format *foc);

int query_capability(struct camera_data *cam_data);

void set_framerate(struct camera_data *cam_data);

int set_format(struct camera_data *cam_data);

int request_buffers(struct camera_data *cam_data);

int query_buffer(struct camera_data *cam_data, struct v4l2_buffer *buf);

int queue_buffer(struct camera_data *cam_data, unsigned int index);

int dequeue_buffer(struct camera_data *cam_data);

int stream_on_camera(struct camera_data *cam_data);

int stream_off_camera(struct camera_data *cam_data);

void init_cap(struct camera_data *cam_data);

#endif

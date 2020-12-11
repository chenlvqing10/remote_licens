/*
 * formation.c - main interface of camera formation.
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

#include "formation.h"
static const struct fourcc_format cap_fourcc[] = {
		{
			.name		= "YUYV",
			.fourcc 	= V4L2_PIX_FMT_YUYV,
			.depth		= 16
		}, {
			.name		= "UYVY",
			.fourcc 	= V4L2_PIX_FMT_UYVY,
			.depth		= 16,
		}, {
			.name		= "VYUY",
			.fourcc 	= V4L2_PIX_FMT_VYUY,
			.depth		= 16,
		}, {
			.name		= "YVYU",
			.fourcc 	= V4L2_PIX_FMT_YVYU,
			.depth		= 16,
		}, {
			.name		= "YUV422P",
			.fourcc 	= V4L2_PIX_FMT_YUV422P,
			.depth		= 12,
			.colplanes	= 3,
		}, {
			.name		= "NV16",
			.fourcc 	= V4L2_PIX_FMT_NV16,
			.depth		= 16,
			.colplanes	= 2,
		}, {
			.name		= "NV61",
			.fourcc 	= V4L2_PIX_FMT_NV61,
			.depth		= 16,
			.colplanes	= 2,
		}, {
			.name		= "YUV420",
			.fourcc 	= V4L2_PIX_FMT_YUV420,
			.depth		= 12,
			.colplanes	= 3,
		}, {
			.name		= "NV12",
			.fourcc 	= V4L2_PIX_FMT_NV12,
			.depth		= 12,
			.colplanes	= 2,
		}, {
			.name		= "NV21",
			.fourcc 	= V4L2_PIX_FMT_NV21,
			.depth		= 12,
			.colplanes	= 2,
		},
};

int set_fourcc(char *str, struct fourcc_format *foc)
{
	int i = 0;
	unsigned int fourcc = 0;

	for (i = 0; i < (sizeof(cap_fourcc) / sizeof(cap_fourcc[0])); i++) {
		fourcc = strcmp(cap_fourcc[i].name, str);
		if (fourcc == 0) {
			*foc = cap_fourcc[i];
			return 0;
		}
	}
	printf("not support %s\n", str);
	return -1;
}

int query_capability(struct camera_data *cam_data)
{
	int v4l2_fd = cam_data->v4l2_fd;
	int ret = 0;
	struct v4l2_capability cap;

	CLEAR(cap);
	ret = v4l2_query_cap(v4l2_fd, &cap);
	if (ret < 0)
		return ret;

	if(0 != (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		cam_data->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		printf("capture mode:plane %d\n", cam_data->type);
	} else if(0 != (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
		cam_data->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		printf("capture mode:mplane %d\n", cam_data->type);
	} else {
		printf("this mode is not supported!\n");
		return -1;
	}

#if DEBUG_PRINT
		printf("query video cap sucess %d!!\n", cap.capabilities);
		printf("driver:%s\nbus_info:%s\n", cap.driver, cap.bus_info);
#endif

	return 0;
}

void set_framerate(struct camera_data *cam_data)
{
	int v4l2_fd = cam_data->v4l2_fd;
	int ret = 0;
	unsigned int sur_fram = 0;
	unsigned int framerate = cam_data->framerate;
	unsigned int set_framerate = framerate * 1000 * 1000;

	if (framerate == 0)
		goto show_fram;

	ret = v4l2_set_framerate(v4l2_fd, set_framerate);
	if (ret < 0)
		printf("set invalid,framerate:%d\n", framerate);

show_fram:
	sur_fram = v4l2_get_cur_framerate(v4l2_fd);
	printf(" surrent framerate:%d\n", sur_fram);
}

int set_format(struct camera_data *cam_data)
{
	int v4l2_fd = cam_data->v4l2_fd;
	unsigned int width = cam_data->cur_width;
	unsigned int height = cam_data->cur_height;

	int ret = 0;
	struct v4l2_format fmt;

	CLEAR (fmt);

	if(cam_data->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = width;
		fmt.fmt.pix.height = height;
		fmt.fmt.pix_mp.field = V4L2_FIELD_ANY;
		fmt.fmt.pix_mp.pixelformat = cam_data->foc.fourcc;
	} else {
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		fmt.fmt.pix_mp.pixelformat = cam_data->foc.fourcc;;
		fmt.fmt.pix_mp.width = width;
		fmt.fmt.pix_mp.height = height;
	}

	ret = v4l2_set_format(v4l2_fd, &fmt);
	if (ret < 0)
		return ret;

	printf(" set format ok, pixelformat=%d,width=%d,height=%d\n",
		fmt.fmt.pix_mp.pixelformat, fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height);

	return 0;
}


int request_buffers(struct camera_data *cam_data)
{
	int v4l2_fd = cam_data->v4l2_fd;
	unsigned int count = cam_data->buf_num;
	enum v4l2_memory memory = cam_data->cur_memory;
	int ret = 0;
	struct v4l2_requestbuffers requsbufs;

	requsbufs.count = count;
	requsbufs.type = cam_data->type;
	requsbufs.memory = memory;

	if (memory == V4L2_MEMORY_DMABUF)
		printf(" memory mode: DMA\n");
	else if (memory == V4L2_MEMORY_MMAP)
		printf(" memory mode: MMAP\n");
	else
		printf(" memory mode: error\n");

#if DEBUG_PRINT
	printf(" VIDIOC_REQBUFS begin.....\n");
#endif

	ret = v4l2_rqbufs_bufs(v4l2_fd, &requsbufs);
	if (ret < 0) {
		printf(" request_buffers failed,memory:%d  type:%d\n",
			memory, cam_data->type);
		return ret;
	}
	return 0;
}

int query_buffer(struct camera_data *cam_data, struct v4l2_buffer *buf)
{
	int v4l2_fd = cam_data->v4l2_fd;
	int ret = 0;

	ret = ioctl(v4l2_fd, VIDIOC_QUERYBUF, buf);
	if (ret < 0) {
		printf("VIDIOC_QBUF for buffer %d failed\n", buf->index);
		return ret;
	}

	return 0;
}

int queue_buffer(struct camera_data *cam_data, unsigned int buf_index)
{
	int v4l2_fd = cam_data->v4l2_fd;
	enum v4l2_memory memory = cam_data->cur_memory;
	int ret = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[FMT_NUM_PLANES];

	CLEAR (buf);
	memset(planes_qbuf, 0 , FMT_NUM_PLANES * sizeof(struct v4l2_plane));

	buf.index = buf_index;
	buf.type = cam_data->type;
	buf.memory = memory;

	if(cam_data->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		buf.m.planes = planes_qbuf;
		buf.length = FMT_NUM_PLANES;
	}

	ret = v4l2_queue_buffer(v4l2_fd, &buf);
	if (ret < 0)
		return ret;

	return ret;
}

int dequeue_buffer(struct camera_data *cam_data)
{
	int v4l2_fd = cam_data->v4l2_fd;
	enum v4l2_memory memory = cam_data->cur_memory;
	int ret;
	int buf_index = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_dqbuf[FMT_NUM_PLANES];

	memset(&buf, 0, sizeof(buf));
	memset(planes_dqbuf, 0, FMT_NUM_PLANES * sizeof(struct v4l2_plane));

	/* dequeue buffer */
	buf.type = cam_data->type;
	buf.memory = memory;

	if(cam_data->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		buf.m.planes = planes_dqbuf;
		buf.length = FMT_NUM_PLANES;
	}

	ret = v4l2_dequeue_buffer(v4l2_fd, &buf, &buf_index);
	if (ret < 0)
		return ret;

	return buf_index;
}

int stream_on_camera(struct camera_data *cam_data)
{
	int r = 0;
	int v4l2_fd = cam_data->v4l2_fd;

	r = v4l2_stream_on(v4l2_fd, &cam_data->type);
	if (r < 0)
		return r;

	return 0;
}

int stream_off_camera(struct camera_data *cam_data)
{
	int r = 0;
	int v4l2_fd = cam_data->v4l2_fd;

	r = v4l2_stream_off(v4l2_fd, &cam_data->type);
	if (r < 0)
		return r;

	return 0;
}

void init_cap(struct camera_data *cam_data)
{
	int ret;
	int i;

	struct camera_data *cam_d = cam_data;

	cam_d->save_frames = 20;

	cam_d->vid_idx = 0;
	cam_d->cur_width = 640;
	cam_d->cur_height = 480;
	cam_d->framerate = 60;
	cam_d->buf_num = 3;
	cam_d->cur_memory = V4L2_MEMORY_MMAP;

	for (i = 0; i < sizeof(cap_fourcc); i++) {
		ret = strcmp(cap_fourcc[i].name, "NV12");
		if (ret == 0) {
			cam_data->foc = cap_fourcc[i];
			break;
		}
	}
}


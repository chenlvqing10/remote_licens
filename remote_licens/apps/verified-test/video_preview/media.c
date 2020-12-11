#include <stdio.h>
#include "drm_display.h"
#include "v4l2_camera.h"
#include "media.h"

#define FOURCC_STR(str)		v4l2_fourcc(str[0], str[1], str[2], str[3])

void init_drm(int *drm_fd)
{
	const char *card = "/dev/dri/card0";

	*drm_fd = drm_open_dev_dumb(card);
}

void uninit_drm(int drm_fd)
{
	close(drm_fd);
}

void queue_buffer(struct cam_vid_pipe *pipe, int index)
{
	int dmafd = pipe->video_dbuf_fds[index];
	int v4l2_fd = pipe->v4l2_fd;
	int j = 0;
	int r = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[CAM_FMT_NUM_PLANES];

	memset(&buf, 0, sizeof(buf));
	memset(planes_qbuf, 0 , CAM_FMT_NUM_PLANES * sizeof(struct v4l2_plane));

	buf.m.planes = planes_qbuf;
	buf.index = index;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_DMABUF;
	buf.length = CAM_FMT_NUM_PLANES;

	/* multiplane dmabuf's special handle */
	for (j = 0; j < CAM_FMT_NUM_PLANES; j++) {
		/*
		 * Because FMT_NUM_PLANES is 1 (NV12) currently,
		 * In case of mplane��TODO
		 * also neet to handle multi dmabuf fd��
		 */
		buf.m.planes[j].m.fd = dmafd;
	}

	/**
	 * According to different buffer format, has different handle,
	 * for example NV12.
	 */
	r = v4l2_queue_buffer(v4l2_fd, &buf);
	ASSERT(r == 0);
}

int dequeue_buffer(struct cam_vid_pipe *pipe)
{
	int r;
	int v4l2_fd = pipe->v4l2_fd;
	int buf_index = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_dqbuf[CAM_FMT_NUM_PLANES];

	memset(&buf, 0, sizeof(buf));
	memset(planes_dqbuf, 0, CAM_FMT_NUM_PLANES * sizeof(struct v4l2_plane));

	/* dequeue buffer */
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_DMABUF;
	buf.m.planes = planes_dqbuf;
	buf.length = CAM_FMT_NUM_PLANES;

	r = v4l2_dequeue_buffer(v4l2_fd, &buf, &buf_index);
	ASSERT(r == 0 || errno == EAGAIN);

	if (r != 0 && errno == EAGAIN)
		return -1;

	return buf_index;
}

void stream_on_camera(struct cam_vid_pipe *pipe)
{
	int r = 0;
	int v4l2_fd = pipe->v4l2_fd;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	r = v4l2_stream_on(v4l2_fd, &type);
	ASSERT(r == 0);
}

void stream_off_camera(struct cam_vid_pipe *pipe)
{
	int r = 0;
	int v4l2_fd = pipe->v4l2_fd;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	r = v4l2_stream_off(v4l2_fd, &type);
	ASSERT(r == 0);
}

void init_capture(struct cam_vid_pipe *pipe, const char *viddev)
{
	int r;

	int fd = v4l2_open_dev(viddev, O_RDWR | O_NONBLOCK);
	ASSERT(fd >= 0);
	pipe->v4l2_fd = fd;

	struct v4l2_format fmt = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
	};

	fmt.fmt.pix.pixelformat = FOURCC_STR(CAM_IMAGE_FORMAT);
	fmt.fmt.pix.width = pipe->input_width;
	fmt.fmt.pix.height = pipe->input_height;

	r = v4l2_set_format(fd, &fmt);
	ASSERT(r == 0);

	r = v4l2_get_format(fd, &fmt);
	ASSERT(r == 0);

	/* save the resolution actually set by the driver */
	pipe->input_width = fmt.fmt.pix.width;
	pipe->input_height = fmt.fmt.pix.height;

	struct v4l2_requestbuffers reqbuf = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
		.memory = V4L2_MEMORY_DMABUF,
		.count = pipe->bufs_num,
	};

	r = v4l2_rqbufs_bufs(fd, &reqbuf);
	ASSERT(r == 0);

	if (reqbuf.count < pipe->bufs_num) {
		printf("%s %d, video node allocated only %u of %u buffers\n",
		__func__, __LINE__, reqbuf.count, pipe->bufs_num);
	}
}

void uninit_capture(struct cam_vid_pipe *pipe)
{
	v4l2_exit_dev(pipe->v4l2_fd);
}

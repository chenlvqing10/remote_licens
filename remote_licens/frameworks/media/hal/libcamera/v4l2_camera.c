#define LOG_TAG		"libcamera"

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <log/log.h>
#include "v4l2_camera.h"


int v4l2_open_dev(const char *node, int flag)
{
	int v4l2_fd = 0;

	/* flag reference to O_RDWR | O_NONBLOCK, etc.. */
	v4l2_fd = open(node, flag);
	if (v4l2_fd < 0) {
		ALOGE("%s %d, failed to open %s\n", __func__, __LINE__, node);
		return v4l2_fd;
	}

	return v4l2_fd;
}

int v4l2_get_control(int fd, unsigned int id, int *value)
{
	struct v4l2_control ctrl;
	int ret;
	int v4l2_fd = fd;

	ctrl.id = id;

	ret = ioctl(v4l2_fd, VIDIOC_G_CTRL, &ctrl);
	if (ret < 0) {
		ALOGE("%s %d, VIDIOC_G_CTRL failed.\n",
			__func__, __LINE__);
		return ret;
	}

	*value = ctrl.value;
	return 0;
}

int v4l2_set_control(int fd, unsigned int id, int *value)
{
	struct v4l2_control ctrl;
	int ret;
	int v4l2_fd = fd;

	ctrl.id = id;
	ctrl.value = *value;

	ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0) {
		ALOGE("%s %d, VIDIOC_S_CTRL failed.\n",
			__func__, __LINE__);
		return ret;
	}

	*value = ctrl.value;

	return 0;
}

int v4l2_query_cap(int fd, struct v4l2_capability *caps)
{
	int ret = 0;
	int v4l2_fd = fd;

	ret = ioctl(v4l2_fd, VIDIOC_QUERYCAP, caps);
	if (ret < 0) {
		ALOGE("%s %d, VIDIOC_QUERYCAP failed.\n",
			__func__, __LINE__);
		return ret;
	}

	return 0;
}

int v4l2_rqbufs_bufs(int fd, struct v4l2_requestbuffers *rqbufs)
{
	int ret = 0;
	int v4l2_fd = fd;

	ret = ioctl(v4l2_fd, VIDIOC_REQBUFS, rqbufs);
	if (ret < 0) {
		ALOGE("%s %d, VIDIOC_REQBUFS failed.\n",
			__func__, __LINE__);
		return ret;
	}

	return 0;
}

int v4l2_get_format(int fd, struct v4l2_format *fmt)
{
	int v4l2_fd = fd;
	int ret = 0;

	ret = ioctl(v4l2_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		ALOGE("%s %d ,VIDIOC_G_FMT failed.\n",
			__func__, __LINE__);
		return ret;
	}

	ALOGI("v4l2_get_format: width = %u, height = %u, 4cc = %.4s\n",
		fmt->fmt.pix_mp.width, fmt->fmt.pix_mp.height,
		(char *)&fmt->fmt.pix_mp.pixelformat);

	return 0;
}


int v4l2_set_format(int fd, struct v4l2_format *fmt)
{
	int v4l2_fd = fd;
	int ret = 0;

	ret = ioctl(v4l2_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		ALOGE("%s %d, VIDIOC_S_FMT %dx%d failed.\n",
			__func__, __LINE__,
			fmt->fmt.pix_mp.width,
			fmt->fmt.pix_mp.height);
		return ret;
	}

	return 0;
}

int v4l2_stream_on(int fd, enum v4l2_buf_type *type)
{
	int ret = 0;
	int v4l2_fd = fd;

	ret = ioctl(v4l2_fd, VIDIOC_STREAMON, type);
	if (ret < 0) {
		ALOGE("%s %d, STREAMON failed\n", __func__, __LINE__);
		return ret;
	}

	return 0;
}

int v4l2_stream_off(int fd, enum v4l2_buf_type *type)
{
	int ret = 0;
	int v4l2_fd = fd;

	ret = ioctl(v4l2_fd, VIDIOC_STREAMOFF, type);
	if (ret < 0) {
		ALOGE("%s %d, STREAMOFF failed\n", __func__, __LINE__);
		return ret;
	}

	return 0;
}

int v4l2_queue_buffer(int fd, struct v4l2_buffer *buf)
{
	int ret = 0;
	int v4l2_fd = fd;

	ret = ioctl(v4l2_fd, VIDIOC_QBUF, buf);
	if (ret < 0) {
		ALOGE("VIDIOC_QBUF for buffer %d failed\n", buf->index);
		return ret;
	}

	return 0;
}

int v4l2_dequeue_buffer(int fd, struct v4l2_buffer *buf, int *buf_index)
{
	int ret = 0;
	int v4l2_fd = fd;

	ret = ioctl(v4l2_fd, VIDIOC_DQBUF, buf);
	if (ret) {
		ALOGE("%s %d, fd:%d, VIDIOC_DQBUF failed %d.\n", __func__,
								  __LINE__,
								  v4l2_fd,
								  ret);
		return ret;
	}

	*buf_index = buf->index;

	return 0;
}

int v4l2_enum_frame(int fd, frame_mode_t *info)
{
	struct v4l2_frmsizeenum fsize;
	struct v4l2_frmivalenum fie;
	int i = 0, j = 0;
	int ret = 0;
	int v4l2_fd = fd;

	for (i = 0; i < MAX_WINS_COUNT; i++) {
		fsize.index = i;
		ret = ioctl(v4l2_fd, VIDIOC_ENUM_FRAMESIZES, &fsize);
		if (ret != 0)
			break;
		fie.width = fsize.discrete.width;
		fie.height = fsize.discrete.height;
		for (j = 0; j < MAX_FRS_COUNT; j++) {
			fie.index = j;
			ret = ioctl(v4l2_fd, VIDIOC_ENUM_FRAMEINTERVALS, &fie);
			if ((ret != 0) || (fie.discrete.numerator == 0))
				break;
			info->format[info->num].width = fie.width;
			info->format[info->num].height = fie.height;
			info->format[info->num].frame_rate =
				fie.discrete.denominator * 1000 / fie.discrete.numerator;
			info->num++;
		}
		if (j > MAX_FRS_COUNT)
			ALOGW("%s %d, fd:%d, too much frame rate config.\n",
				__func__,  __LINE__, v4l2_fd);
	}
	if (i > MAX_WINS_COUNT)
		ALOGW("%s %d, fd:%d, too much resolution config.\n",
			__func__,  __LINE__, v4l2_fd);

	if (i == 0) {
		ALOGE("%s %d, no framesize enumed!\n", __func__,  __LINE__);
		return -1;
	}

	return 0;
}

int v4l2_get_cur_framerate(int fd)
{
	struct v4l2_streamparm parms;
	struct v4l2_fract *tpf;
	int frame_rate;
	int v4l2_fd = fd;
	int ret = 0;

	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(v4l2_fd, VIDIOC_G_PARM, &parms);
	if (ret != 0) {
		ALOGE("%s %d: VIDIOC_G_PARM failed!\n", __func__, __LINE__);
		return ret;
	}

	tpf = &parms.parm.capture.timeperframe;
	if ((tpf->numerator == 0) || (tpf->denominator == 0)) {
		ALOGE("%s %d: ERROR numerator %u denominator %u!\n",
			__func__, __LINE__, tpf->numerator, tpf->denominator);
		return -1;
	}

	frame_rate = (int)tpf->denominator / tpf->numerator;

	return frame_rate;
}

int v4l2_set_framerate(int fd, int frame_rate)
{
	struct v4l2_streamparm parms;
	struct v4l2_captureparm *cp = &parms.parm.capture;
	struct v4l2_fract *tpf = &cp->timeperframe;
	int v4l2_fd = fd;
	int ret = 0;
	unsigned int numerator = 0;
	unsigned int denominator = 0;

	numerator = 1000;
	denominator = frame_rate / 1000;
	memset(&parms, 0 , sizeof(parms));
	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	cp->extendedmode = 0;
	tpf->numerator = numerator;
	tpf->denominator = denominator;

	ret = ioctl(v4l2_fd, VIDIOC_S_PARM, &parms);
	if (ret != 0) {
		ALOGE("%s %d: VIDIOC_S_PARM failed!\n", __func__, __LINE__);
		return ret;
	}

	return 0;
}

int v4l2_exit_dev(int fd)
{
	int v4l2_fd = fd;

	close(v4l2_fd);

	return 0;
}

int v4l2_get_isp_dev(int fd)
{
	int v4l2_fd = fd;
	int isp_fd = -1;
	int id;
	char devname[32];

	if ((ioctl(v4l2_fd, VIDIOC_VISS_GET_ISP_DEV, &id)) < 0) {
		ALOGE("VIDIOC_VISS_GET_ISP_DEV failed!\n");
		return -1;
	} else {
		ALOGD("VIDIOC_VISS_GET_ISP_DEV ok, suddev id %d!\n", id);
	}

	sprintf(devname, "/dev/v4l-subdev%d", id);
	isp_fd = open(devname, 0, 0);
	if (isp_fd < 0) {
		ALOGE("Failed to open subdev device node %s\n", devname);
		return isp_fd;
	}

	return isp_fd;
}

int v4l2_get_sensor_dev(int fd)
{
	int v4l2_fd = fd;
	int sensor_fd = -1;
	int id;
	char devname[32];

	if ((ioctl(v4l2_fd, VIDIOC_VISS_GET_SENSOR_DEV, &id)) < 0) {
		ALOGE("VIDIOC_VISS_GET_SENSOR_DEV failed!\n");
		return -1;
	} else {
		ALOGD("VIDIOC_VISS_GET_SENSOR_DEV ok, suddev id %d!\n", id);
	}

	sprintf(devname, "/dev/v4l-subdev%d", id);
	sensor_fd = open(devname, 0, 0);
	if (sensor_fd < 0) {
		ALOGE("Failed to open subdev device node %s\n", devname);
		return sensor_fd;
	}

	return sensor_fd;
}


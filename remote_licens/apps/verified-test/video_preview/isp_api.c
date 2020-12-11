
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>

#include "isp_api.h"

int set_control(int fd, unsigned int id, int *value)
{
	struct v4l2_control ctrl;
	int ret;

	ctrl.id = id;
	ctrl.value = *value;

	ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0) {
		printf("%s failed!\n", __func__);
		return ret;
	}

	*value = ctrl.value;

	return 0;
}

int get_isp_dev(int fd)
{
	int isp_fd = -1;
	int id;
	char devname[32];

	if ((ioctl(fd, VIDIOC_VISS_GET_ISP_DEV, &id)) < 0) {
		printf("%s failed!\n", __func__);
		return -1;
	} else {
		printf("%s ok, suddev id %d!\n", __func__, id);
	}

	sprintf(devname, "/dev/v4l-subdev%d", id);
	isp_fd = open(devname, 0, 0);
	if (isp_fd < 0) {
		printf("Failed to open subdev device node %s\n", devname);
		return isp_fd;
	}

	return isp_fd;
}

int get_sensor_dev(int fd)
{
	int sensor_fd = -1;
	int id;
	char devname[32];

	if ((ioctl(fd, VIDIOC_VISS_GET_SENSOR_DEV, &id)) < 0) {
		printf("%s failed!\n", __func__);
		return -1;
	} else {
		printf("%s ok, suddev id %d!\n", __func__, id);
	}

	sprintf(devname, "/dev/v4l-subdev%d", id);
	sensor_fd = open(devname, 0, 0);
	if (sensor_fd < 0) {
		printf("Failed to open subdev device node %s\n", devname);
		return sensor_fd;
	}

	return sensor_fd;
}

int get_rgb_sensor_exif(int fd, struct viss_isp_exif *exif)
{
	int ret = 0;

	ret = ioctl(fd, VIDIOC_VISS_ISP_EXIF_REQ, exif);
	if (ret < 0) {
		printf("%s failed!\n", __func__);
		return ret;
	}

	printf("exp_time: %d, iso: %d, brightness: %d,back_score: %d,lum %d %d %d, target %d\n",
		exif->exposure_time, exif->iso_speed, exif->brightness,
		exif->back_score, exif->res[0], exif->res[1], exif->res[2],
		exif->res[4]);

	return ret;
}

int get_nir_sensor_exif(int fd, struct viss_isp_exif *exif)
{
	int ret = 0;

	ret = ioctl(fd, VIDIOC_VISS_ISP_EXIF_REQ, exif);
	if (ret < 0) {
		printf("%s failed!\n", __func__);
		return ret;
	}
	printf("exposure_time: %d, iso_speed: %d, avg_lum %d, target %d\n",
		exif->exposure_time, exif->iso_speed, exif->res[2], exif->res[4]);

	return ret;
}

int set_rgb_sensor_hdr(int fd, unsigned int hdr_mode)
{
	int ret = 0;

	ret = ioctl(fd, VIDIOC_VISS_ISP_SENSOR_HDR_MODE, &hdr_mode);
	if (ret < 0)
		printf("%s failed!\n", __func__);
	return ret;
}

int set_exp_bias(int fd, int exp_bias)
{
	int bias_idx = 0;
	int exp_bias_qmenu[] = { -3, -2, -1, 0, 1, 2, 3};

	for (bias_idx = 0; bias_idx < 7; bias_idx++) {
		if (exp_bias <= exp_bias_qmenu[bias_idx])
			break;
	}
	if (bias_idx == 7)
		bias_idx = 6;

	return set_control(fd, V4L2_CID_AUTO_EXPOSURE_BIAS, &bias_idx);
}

int set_wb_mode(int fd, int wb_mode)
{
	return set_control(fd, V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, &wb_mode);
}

int set_wdr_mode(int fd, int wdr_mode)
{
	return set_control(fd, V4L2_CID_WIDE_DYNAMIC_RANGE, &wdr_mode);
}

int set_ae_para(int fd, struct v4l2_rect *rect, int mode)
{
	if (-1 == set_control(fd, V4L2_CID_EXPOSURE_METERING, &mode)) {
		printf("VIDIOC_S_CRTL V4L2_CID_EXPOSURE_METERING error.\n");
		return -1;
	}

	if (-1 == set_control(fd, V4L2_CID_WIN_POS_X, &rect->left)) {
		printf("VIDIOC_S_CRTL V4L2_CID_WIN_POS_X error.\n");
		return -1;
	}

	if (-1 == set_control(fd, V4L2_CID_WIN_POS_Y, &rect->top)) {
		printf("VIDIOC_S_CRTL V4L2_CID_WIN_POS_Y error.\n");
		return -1;
	}

	if (-1 == set_control(fd, V4L2_CID_WIN_WIDTH, &rect->width)) {
		printf("VIDIOC_S_CRTL V4L2_CID_WIN_WIDTH error.\n");
		return -1;
	}

	if (-1 == set_control(fd, V4L2_CID_WIN_HEIGHT, &rect->height)) {
		printf("VIDIOC_S_CRTL V4L2_CID_WIN_HEIGHT error.\n");
		return -1;
	}
	return 0;
}


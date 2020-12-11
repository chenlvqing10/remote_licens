#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>

#define LOG_TAG "object_svc_isp"
#include <log/log.h>

#include "object_svc.h"
#include "object_svc_isp.h"
#include "v4l2_camera.h"

#define VIDEO_ID 		3
#define SUBDEV_ISP_ID 		4
#define SUBDEV_AR0230_ID 	5
#define SUBDEV_GC2145_ID 	6
#define FACE_ISP_RGB_ROI_TH 	50
#define FACE_ISP_NIR_ROI_TH 	20

#define FACE_ISP_MIN(a, b)  			(((a) < (b)) ? (a) : (b))
#define FACE_ISP_MAX(a, b)  			(((a) > (b)) ? (a) : (b))
#define FACE_ISP_CONSTRAIN(value, lo, hi) (FACE_ISP_MIN(FACE_ISP_MAX((value), (lo)), (hi)))

int rgb_width;
int rgb_height;
int rgb_rotation;
int gc2145_fd = 0;
int ar0230_fd = 0;
int video_fd = 0;
int isp_fd = 0;
unsigned int frame_cnt_isp = 0;
int target_last = -1;
struct viss_isp_ae_para rgb_last_para = {0};
struct viss_isp_ae_para nir_last_para = {0};

const unsigned int sensor_ae_taget[16] = {
	0x40, 0x40, 0x48, 0x50,
	0x50, 0x50, 0x50, 0x50,
	0x50, 0x50, 0x60, 0x68,
	0x6C, 0x6C, 0x70, 0x70
};

void object_buffer_size(int id, int width, int height, int rotation)
{
	if (id == OBJECT_CAMERA_RGB) {
		rgb_width = width;
		rgb_height = height;
		rgb_rotation = rotation;
	}
}

void set_ae_para(int fd, struct v4l2_rect *rect, int mode)
{
	if (-1 == v4l2_set_control(fd, V4L2_CID_EXPOSURE_METERING, &mode))
		ALOGI("VIDIOC_S_CRTL V4L2_CID_EXPOSURE_METERING error.\n");

	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_POS_X, &rect->left))
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_POS_X error.\n");

	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_POS_Y, &rect->top))
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_POS_Y error.\n");

	/* in the toolchain head file, v4l2_rect.width v4l2_rect.height is __u32 */
	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_WIDTH, (int *)&rect->width))
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_WIDTH error.\n");

	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_HEIGHT, (int *)&rect->height))
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_HEIGHT error.\n");
}

int object_ae_init(void)
{
	char name[FILENAME_MAX];
	int video_id = VIDEO_ID;
	int isp_id = SUBDEV_ISP_ID;

	if (0 == gc2145_fd) {
		sprintf(name, "/dev/v4l-subdev%d", SUBDEV_GC2145_ID);
		gc2145_fd = open(name, 0, 0);
		if (gc2145_fd < 0) {
			ALOGE("%s: Failed to open subdev device node %s\n", __func__, name);
			return -1;
		}
	}

	if (0 == ar0230_fd) {
		sprintf(name, "/dev/v4l-subdev%d", SUBDEV_AR0230_ID);
		ar0230_fd = open(name, 0, 0);
		if (ar0230_fd < 0) {
			ALOGE("%s: Failed to open subdev device node %s\n", __func__, name);
			return -1;
		}
	}

	if (0 == isp_fd) {
		sprintf(name, "/dev/v4l-subdev%d", isp_id);
		isp_fd = open(name, 0, 0);
		if (isp_fd < 0) {
			ALOGE("%s: Failed to open subdev device node %s\n", __func__, name);
			return -1;
		}
	}

	if (0 == video_fd) {
		sprintf(name, "/dev/video%d", video_id);
		video_fd = open(name, O_RDWR | O_NONBLOCK);
		if (video_fd < 0) {
			ALOGE("%s %d, failed to open %s\n",
				__func__, __LINE__, name);
			return -1;
		}
	}

	return 0;
}

void object_ae_exit(void)
{
	if (gc2145_fd) {
		close(gc2145_fd);
		gc2145_fd = 0;
	}

	if (ar0230_fd) {
		close(ar0230_fd);
		ar0230_fd = 0;
	}

	if (isp_fd) {
		close(isp_fd);
		isp_fd = 0;
	}

	if (video_fd) {
		close(video_fd);
		video_fd = 0;
	}
}

void object_set_ae_para(int id, viss_isp_rects_t *rects)
{
	int ret = 0, tag_id = 0, i;
	unsigned int target = 0;
	struct viss_isp_ae_para ae_para = { 0 };
	viss_isp_rect_t rect = {10000, 10000, 0, 0};

	object_ae_init();

	frame_cnt_isp++;

	for (i = 0; i < rects->cnt; i++) {
		rect.left = FACE_ISP_MIN(rect.left, rects->rects[i].left);
		rect.top = FACE_ISP_MIN(rect.top, rects->rects[i].top);
		rect.right = FACE_ISP_MAX(rect.right, rects->rects[i].right);
		rect.bottom = FACE_ISP_MAX(rect.bottom, rects->rects[i].bottom);
	}

	if (id == OBJECT_CAMERA_NIR) {
		/* Process NIR object ROI */
		if ((frame_cnt_isp % 4) != 0)
			return;

		if (rects->cnt) {
			/* ALOGD("NIR camera before: %d [%d, %d, %d, %d], mode: %d", id,
				rect->left, rect->top, rect->right, rect->bottom, mode); */
			ae_para.rect.left = rect.left;
			ae_para.rect.top = rect.top;
			ae_para.rect.width = (rect.right - rect.left);
			ae_para.rect.height = (rect.bottom - rect.top);
			ae_para.ae_mode = 1;
		} else {
			ae_para.ae_mode = 0;
		}

		if (abs(nir_last_para.rect.left - ae_para.rect.left) > FACE_ISP_NIR_ROI_TH ||
			abs(nir_last_para.rect.top - ae_para.rect.top) > FACE_ISP_NIR_ROI_TH ||
			abs(nir_last_para.rect.width - ae_para.rect.width) > FACE_ISP_NIR_ROI_TH ||
			abs(nir_last_para.rect.height - ae_para.rect.height) > FACE_ISP_NIR_ROI_TH) {
			ALOGD("NIR camera after: %d [%d, %d, %d, %d], mode: %d", id,
				ae_para.rect.left, ae_para.rect.top, ae_para.rect.width,
				ae_para.rect.height, ae_para.ae_mode);

			ret = ioctl(gc2145_fd, VIDIOC_ISP_SET_AE_METER, &ae_para);
			if (ret < 0) {
				ALOGE("%s %d, VIDIOC_S_CTRL failed %d.\n",
					__func__, __LINE__, ret);
			}
			nir_last_para = ae_para;
		}
	} else if (id == OBJECT_CAMERA_RGB) {
		struct viss_isp_exif exif = { 0 };

		/* Process RGB object ROI */
		if (rects->cnt) {
			/* ALOGD("RGB camera before: %d [%d, %d, %d, %d], mode: %d", id,
				rect->left, rect->top, rect->right, rect->bottom, mode); */
			if (rgb_rotation) {
				ae_para.rect.left = (rect.top + 192) * 1920 / 1408;
				ae_para.rect.top = (800 - rect.right) * 1080 / 800;
				ae_para.rect.width = (rect.bottom - rect.top) * 1920 / 1408;
				ae_para.rect.height = (rect.right - rect.left) * 1080 / 800;
				ae_para.ae_mode = 1;
			} else {
				ae_para.rect.left = rect.left;
				ae_para.rect.top = rect.top;
				ae_para.rect.width = (rect.right - rect.left);
				ae_para.rect.height = (rect.bottom - rect.top);
				ae_para.ae_mode = 1;
			}
		} else {
			ae_para.ae_mode = 0;
		}
		if (abs(rgb_last_para.rect.left - ae_para.rect.left) > FACE_ISP_RGB_ROI_TH ||
			abs(rgb_last_para.rect.top - ae_para.rect.top) > FACE_ISP_RGB_ROI_TH ||
			abs(rgb_last_para.rect.width - ae_para.rect.width) > FACE_ISP_RGB_ROI_TH ||
			abs(rgb_last_para.rect.height - ae_para.rect.height) > FACE_ISP_RGB_ROI_TH) {

			ALOGD("RGB camera after: %d [%d, %d, %d, %d], mode: %d",
				id, ae_para.rect.left, ae_para.rect.top,
				ae_para.rect.width, ae_para.rect.height, ae_para.ae_mode);
			set_ae_para(video_fd, &ae_para.rect, ae_para.ae_mode);
			rgb_last_para = ae_para;
		}

		/* Process EXIF */
		ret = ioctl(isp_fd, VIDIOC_VISS_ISP_EXIF_REQ, &exif);
		if (ret < 0) {
			ALOGE("%s %d, ioctl failed %d.\n",
				__func__, __LINE__, ret);
		}

		tag_id = FACE_ISP_CONSTRAIN((int)exif.brightness / 100, 0, 15);
		target = FACE_ISP_CONSTRAIN(
				sensor_ae_taget[tag_id] + exif.back_score / 3, 20, 160);

		if (target_last != target) {
			ALOGD("exposure_time: %d, iso_speed: %d, brightness:"
				" %d, tag_id: %d, target: %d, back_score: %d, exposure_bias: %d\n",
				exif.exposure_time, exif.iso_speed,
				exif.brightness, tag_id, target, exif.back_score, exif.exposure_bias);
			ret = ioctl(gc2145_fd, VIDIOC_ISP_SET_AE_TARGET, &target);
			if (ret < 0) {
				ALOGE("%s %d, ioctl failed %d, errno:%s[%d].\n",
					__func__, __LINE__, ret, strerror(errno), errno);
			}
			target_last = target;
		}
	}
}

int object_set_hdr_mode(int id, unsigned int hdr_mode)
{
	int ret = 0;

	object_ae_init();

	ret = ioctl(ar0230_fd, VIDIOC_VISS_ISP_SENSOR_HDR_MODE, &hdr_mode);
	if (ret < 0) {
		ALOGE("%s %d, ioctl failed %d, errno:%s[%d].\n",
			__func__, __LINE__, ret, strerror(errno), errno);
	}
	return ret;
}

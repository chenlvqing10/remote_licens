#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <iniparser.h>
#include "v4l2_camera.h"

#define LOG_TAG "face_svc_isp"
#include <log/log.h>

#include "face_svc.h"
#include "face_svc_conf.h"
#include "face_svc_isp.h"

typedef enum {
	ONE_ISP = 1,
	TWO_ISP = 2,
	MAX_ISP = TWO_ISP
} isp_cnt_e;

#define FACE_ISP_RGB_ROI_TH 	50
#define FACE_ISP_NIR_ROI_TH 	20

#define FACE_ISP_MIN(a, b)  			(((a) < (b)) ? (a) : (b))
#define FACE_ISP_MAX(a, b)  			(((a) > (b)) ? (a) : (b))
#define FACE_ISP_CONSTRAIN(value, lo, hi) (FACE_ISP_MIN(FACE_ISP_MAX((value), (lo)), (hi)))

int isp_width;
int isp_height;
int nir_fd = 0;
int rgb_fd = 0;
int video_fd = 0;
int isp_fd = 0;
int target_last = -1;
struct viss_isp_ae_para rgb_last_para = {0};
struct viss_isp_ae_para nir_last_para = {0};

const unsigned int sensor_ae_taget[16] = {
	0x40, 0x40, 0x48, 0x50,
	0x50, 0x50, 0x50, 0x50,
	0x50, 0x50, 0x60, 0x68,
	0x6C, 0x6C, 0x70, 0x70
};

void set_ae_para(int fd, struct v4l2_rect *rect, int mode)
{
	if (-1 == v4l2_set_control(fd, V4L2_CID_EXPOSURE_METERING, &mode)) {
		ALOGI("VIDIOC_S_CRTL V4L2_CID_EXPOSURE_METERING error.\n");
	}

	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_POS_X, &rect->left)) {
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_POS_X error.\n");
	}

	if(-1 == v4l2_set_control(fd, V4L2_CID_WIN_POS_Y, &rect->top)) {
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_POS_Y error.\n");
	}

	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_WIDTH, (int *)&rect->width)) {
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_WIDTH error.\n");
	}

	if (-1 == v4l2_set_control(fd, V4L2_CID_WIN_HEIGHT, (int *)&rect->height)) {
		ALOGI("VIDIOC_S_CRTL V4L2_CID_WIN_HEIGHT error.\n");
	}
}

int face_ae_init(void)
{
	if (face_svc_conf->media.rgb_enable) {
		video_fd = open(face_svc_conf->media.rgb_device, O_RDWR | O_NONBLOCK);
		if (video_fd < 0) {
			ALOGE("%s, failed to open %s\n", __func__,
				face_svc_conf->media.rgb_device);
		} else {
			isp_fd = v4l2_get_isp_dev(video_fd);
			rgb_fd = v4l2_get_sensor_dev(video_fd);
			close(video_fd);
		}
	}

	if (face_svc_conf->media.nir_enable) {
		video_fd = open(face_svc_conf->media.nir_device, O_RDWR | O_NONBLOCK);
		if (video_fd < 0) {
			ALOGE("%s, failed to open %s\n", __func__,
				face_svc_conf->media.nir_device);
		} else {
			if (face_svc_conf->isp.isp_cnt == ONE_ISP) {
				nir_fd = v4l2_get_sensor_dev(video_fd);
			} else if (face_svc_conf->isp.isp_cnt == TWO_ISP) {
				nir_fd = v4l2_get_isp_dev(video_fd);
			} else {
				ALOGE("unknow isp num: %d\n", face_svc_conf->isp.isp_cnt);
			}
			close(video_fd);
		}
	}

	video_fd = 0;

	return 0;
}

void face_ae_exit(void)
{
	if (nir_fd) {
		close(nir_fd);
		nir_fd = 0;
	}

	if (rgb_fd) {
		close(rgb_fd);
		rgb_fd = 0;
	}

	if (isp_fd) {
		close(isp_fd);
		isp_fd = 0;
	}
}

void face_set_ae_para(int id, viss_isp_rects_t *rects)
{
	int ret = 0, tag_id = 0, i;
	unsigned int target = 0;
	struct viss_isp_ae_para ae_para = { 0 };
	viss_isp_rect_t rect = {10000, 10000, 0, 0};

	for (i = 0; i < rects->cnt; i++) {
		rect.left = FACE_ISP_MIN(rect.left, rects->rects[i].left);
		rect.top = FACE_ISP_MIN(rect.top, rects->rects[i].top);
		rect.right = FACE_ISP_MAX(rect.right, rects->rects[i].right);
		rect.bottom = FACE_ISP_MAX(rect.bottom, rects->rects[i].bottom);
	}

	if (id == FACE_CAMERA_NIR) {
		/* Process NIR face ROI */
		if (rects->cnt) {
			ALOGD_IF(0, "NIR camera before: %d [%d, %d, %d, %d], nir_rotation: %d", id,
				rect.left, rect.top, rect.right, rect.bottom, face_svc_conf->media.nir_rotation);
			if (face_svc_conf->media.nir_rotation == 90) {
				ae_para.rect.left = rect.top;
				ae_para.rect.top = (face_svc_conf->media.nir_height - rect.right);
				ae_para.rect.width = (rect.bottom - rect.top);
				ae_para.rect.height = (rect.right - rect.left);
			} else if (face_svc_conf->media.nir_rotation == 270) {
				ae_para.rect.left = (face_svc_conf->media.nir_width - rect.bottom);
				ae_para.rect.top = rect.left;
				ae_para.rect.width = (rect.bottom - rect.top);
				ae_para.rect.height = (rect.right - rect.left);
			} else if (face_svc_conf->media.nir_rotation == 180) {
				ae_para.rect.left = (face_svc_conf->media.nir_width - rect.right);
				ae_para.rect.top = (face_svc_conf->media.nir_height - rect.bottom);
				ae_para.rect.width = (rect.right - rect.left);
				ae_para.rect.height = (rect.bottom - rect.top);
			} else {
				ae_para.rect.left = rect.left;
				ae_para.rect.top = rect.top;
				ae_para.rect.width = (rect.right - rect.left);
				ae_para.rect.height = (rect.bottom - rect.top);
			}
			ae_para.ae_mode = 1;
		} else {
			ae_para.ae_mode = 0;
		}

		if (abs(nir_last_para.rect.left - ae_para.rect.left) > FACE_ISP_NIR_ROI_TH ||
		    abs(nir_last_para.rect.top - ae_para.rect.top) > FACE_ISP_NIR_ROI_TH ||
		    abs(nir_last_para.rect.width - ae_para.rect.width) > FACE_ISP_NIR_ROI_TH ||
		    abs(nir_last_para.rect.height - ae_para.rect.height) > FACE_ISP_NIR_ROI_TH ||
		    nir_last_para.ae_mode != ae_para.ae_mode) {
			ALOGD_IF(0, "NIR camera after: %d [%d, %d, %d, %d], mode: %d", id,
				ae_para.rect.left, ae_para.rect.top, ae_para.rect.width,
				ae_para.rect.height, ae_para.ae_mode);
			if (face_svc_conf->isp.isp_cnt == ONE_ISP) {
				ret = ioctl(nir_fd, VIDIOC_ISP_SET_AE_METER, &ae_para);
				if (ret < 0) {
					ALOGE("%s %d, VIDIOC_S_CTRL failed %d.\n",
						__func__, __LINE__, ret);
				}
			} else if (face_svc_conf->isp.isp_cnt == TWO_ISP) {
				set_ae_para(nir_fd, &ae_para.rect, ae_para.ae_mode);
			} else {
				ALOGE("unknow isp num: %d\n", face_svc_conf->isp.isp_cnt);
			}
			nir_last_para = ae_para;
		}
	} else if (id == FACE_CAMERA_RGB) {
		struct viss_isp_exif exif = { 0 };
		struct viss_isp_sensor_fmt_info info;

		if (isp_width == 0 || isp_height == 0) {
			ret = ioctl(rgb_fd, VIDIOC_VISS_ISP_SENSOR_FMT_INFO, &info);
			if (ret < 0) {
				ALOGE("%s %d, VIDIOC_VISS_ISP_SENSOR_FMT_INFO failed %d.\n",
					__func__, __LINE__, ret);
				isp_width = 1920;
				isp_height = 1080;
			} else {
				isp_width = info.width;
				isp_height = info.height;
			}
		}

		/* Process RGB face ROI */
		if (rects->cnt) {
			ALOGD_IF(0, "RGB camera before: %d [%d, %d, %d, %d], rgb_rotation: %d", id,
				rect.left, rect.top, rect.right, rect.bottom, face_svc_conf->media.rgb_rotation);
			if (face_svc_conf->media.rgb_rotation == 90) {
				ae_para.rect.left = (rect.top) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.top = (face_svc_conf->media.rgb_height - rect.right) * isp_height / face_svc_conf->media.rgb_height;
				ae_para.rect.width = (rect.bottom - rect.top) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.height = (rect.right - rect.left) * isp_height / face_svc_conf->media.rgb_height;
			} else if (face_svc_conf->media.rgb_rotation == 270) {
				ae_para.rect.left = (face_svc_conf->media.rgb_width - rect.bottom) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.top = (rect.left) * isp_height / face_svc_conf->media.rgb_height;
				ae_para.rect.width = (rect.bottom - rect.top) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.height = (rect.right - rect.left) * isp_height / face_svc_conf->media.rgb_height;
			} else if (face_svc_conf->media.rgb_rotation == 180) {
				ae_para.rect.left = (face_svc_conf->media.rgb_width - rect.right) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.top = (face_svc_conf->media.rgb_height - rect.bottom) * isp_height / face_svc_conf->media.rgb_height;
				ae_para.rect.width = (rect.right - rect.left) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.height = (rect.bottom - rect.top) * isp_height / face_svc_conf->media.rgb_height;
			} else {
				ae_para.rect.left = (rect.left) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.top = (rect.top) * isp_height / face_svc_conf->media.rgb_height;
				ae_para.rect.width = (rect.right - rect.left) * isp_width / face_svc_conf->media.rgb_width;
				ae_para.rect.height = (rect.bottom - rect.top) * isp_height / face_svc_conf->media.rgb_height;
			}
			ae_para.ae_mode = 1;
		} else {
			ae_para.ae_mode = 0;
		}
		if (abs(rgb_last_para.rect.left - ae_para.rect.left) > FACE_ISP_RGB_ROI_TH ||
		    abs(rgb_last_para.rect.top - ae_para.rect.top) > FACE_ISP_RGB_ROI_TH ||
		    abs(rgb_last_para.rect.width - ae_para.rect.width) > FACE_ISP_RGB_ROI_TH ||
		    abs(rgb_last_para.rect.height - ae_para.rect.height) > FACE_ISP_RGB_ROI_TH ||
		    rgb_last_para.ae_mode != ae_para.ae_mode) {
			ALOGD_IF(0, "RGB camera after: %d [%d, %d, %d, %d], mode: %d",
				id, ae_para.rect.left, ae_para.rect.top,
				ae_para.rect.width, ae_para.rect.height, ae_para.ae_mode);
			set_ae_para(isp_fd, &ae_para.rect, ae_para.ae_mode);
			rgb_last_para = ae_para;
		}

		if (face_svc_conf->isp.isp_cnt == ONE_ISP) {
			if (nir_fd <= 0)
					return;
				if (rects->cnt) {
					float damp = 0.7;
					get_rgb_sensor_exif(id, &exif);

					tag_id = FACE_ISP_CONSTRAIN((int)exif.brightness / 100, 0, 15);
					if (exif.res[0] > exif.res[1])
						target = sensor_ae_taget[tag_id] - exif.back_score / 3;
					else
						target = sensor_ae_taget[tag_id] + exif.back_score / 3;
					target = FACE_ISP_CONSTRAIN(target, 20, 160);
					target = (1 - damp) * target + damp * target_last;
				} else {
					target = 0x50;
				}

				if (target_last != target) {
					ALOGD_IF(0, "tag_id: %d, target: %d\n", tag_id, target);
					ret = ioctl(nir_fd, VIDIOC_ISP_SET_AE_TARGET, &target);
					if (ret < 0) {
						ALOGE("%s %d, ioctl failed %d, errno:%s[%d].\n",
							__func__, __LINE__, ret, strerror(errno), errno);
					}
					target_last = target;

					memset(&exif, 0, sizeof(exif));
					get_nir_sensor_exif(id, &exif);
				}
		}
	}
}

int set_rgb_sensor_hdr(int id, unsigned int hdr_mode)
{
	int ret = 0;

	ret = ioctl(rgb_fd, VIDIOC_VISS_ISP_SENSOR_HDR_MODE, &hdr_mode);
	if (ret < 0) {
		ALOGE("%s %d, ioctl failed %d, errno:%s[%d].\n",
			__func__, __LINE__, ret, strerror(errno), errno);
	}
	return ret;
}

int set_exp_bias(int id, int exp_bias)
{
	int bias_idx = 0;
	int exp_bias_qmenu[] = { -3, -2, -1, 0, 1, 2, 3};

	for (bias_idx = 0; bias_idx < 7; bias_idx++) {
		if (exp_bias <= exp_bias_qmenu[bias_idx])
			break;
	}
	if (bias_idx == 7)
		bias_idx = 6;

	return v4l2_set_control(isp_fd, V4L2_CID_AUTO_EXPOSURE_BIAS, &bias_idx);
}

int set_wb_mode(int id, int wb_mode)
{
	return v4l2_set_control(isp_fd, V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, &wb_mode);
}

int set_wdr_mode(int id, int wdr_mode)
{
	return v4l2_set_control(isp_fd, V4L2_CID_WIDE_DYNAMIC_RANGE, &wdr_mode);
}

/* saturation -10~10
 * -10: gray,
 * -9~-1: under saturation,
 * 0: norm,
 * 1~10 : over saturation
 */
int set_saturation(int id, int saturation)
{
	return v4l2_set_control(isp_fd, V4L2_CID_SATURATION, &saturation);
}

int get_rgb_sensor_exif(int id, struct viss_isp_exif *exif)
{
	int ret = 0;

	ret = ioctl(isp_fd, VIDIOC_VISS_ISP_EXIF_REQ, exif);
	if (ret < 0) {
		ALOGE("%s %d, ioctl failed %d.\n", __func__, __LINE__, ret);
		return ret;
	}
	ALOGD_IF(0, "exp_time: %d, iso: %d, brightness: %d, back_score: %d, lum %d %d %d, target %d\n",
		exif->exposure_time, exif->iso_speed, exif->brightness, exif->back_score,
		exif->res[0], exif->res[1], exif->res[2], exif->res[4]);

	return ret;
}

int get_nir_sensor_exif(int id, struct viss_isp_exif *exif)
{
	int ret = 0;

	ret = ioctl(nir_fd, VIDIOC_VISS_ISP_EXIF_REQ, exif);
	if (ret < 0) {
		ALOGE("%s %d, ioctl failed %d.\n", __func__, __LINE__, ret);
		return ret;
	}
	ALOGD_IF(0, "exposure_time: %d, iso_speed: %d, avg_lum %d, target %d\n",
		exif->exposure_time, exif->iso_speed, exif->res[2], exif->res[4]);

	return ret;
}


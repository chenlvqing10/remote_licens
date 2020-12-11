#ifndef _FACE_SVC_ISP_H_
#define _FACE_SVC_ISP_H_
#include <linux/types.h>
#include <linux/videodev2.h>

/**
 * struct viss_isp_exif - structure for the EXIF information of VISS
 * @exposure_time: exposure time(s) * 10000
 * @shutter_speed:  exposure time(s) * 10000
 * @aperture: aperture fno2.8 = 280
 * @brightness: LV = [0, 20], 0: star light, 20: sun light
 * @exposure_bias: it calls also EV bias
 * @iso_speed: ISO = gain * 100
 * @flash: status register value of the flash
 * @illu_id: AWB temperature id
 *    0: HOR(2100K)
 *    1: A(2900K)
 *    2: TL84(4000K)
 *    3: CWF(4100K)
 *    4: D50(5000K)
 *    5: D65(6500K)
 *    6: D75(7500K)
 * @back_score: back light score = [0, 100] percent
 * @res: reserved info
 * @res[0]: object luminance
 * @res[1]: back luminance
 * @res[2]: average luminance
 * @res[3]: original luminance
 * @res[4]: final luminance
 */

#define NIR_WINDOW_MIN 68

struct viss_isp_exif {
	__u32 exposure_time;
	__u32 shutter_speed;
	__u32 aperture;
	__u32 brightness;
	__u32 exposure_bias;
	__u32 iso_speed;
	__u32 flash;
	__u32 illu_id;
	__u32 back_score;
	__u32 res[16];
};

struct viss_isp_sensor_fmt_info {
	__u32 pclk;
	__u32 vts;
	__u32 hts;
	__u32 hoffset;
	__u32 voffset;
	__u32 width;
	__u32 height;
	__u32 buswidth;
	__u32 mode;
	__u32 field_sel;
	__u32 yuv_seq;
	__u32 yuv_ds_mode;
	__u32 bayer_mode;
	__u32 hpol;
	__u32 vpol;
	__u32 edge;
};

typedef struct _viss_isp_rect {
	int left;
	int top;
	int right;
	int bottom;
} viss_isp_rect_t;

typedef struct _viss_isp_rects {
	viss_isp_rect_t *rects;
	int cnt;
} viss_isp_rects_t;

struct viss_isp_ae_para {
	struct v4l2_rect rect;
	enum v4l2_exposure_metering ae_mode;
};

#define VIDIOC_VISS_ISP_SENSOR_FMT_INFO \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 19, struct viss_isp_sensor_fmt_info)
#define VIDIOC_ISP_SET_AE_METER \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 20, struct viss_isp_ae_para)
#define VIDIOC_VISS_ISP_EXIF_REQ \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 21, struct viss_isp_exif)
#define VIDIOC_ISP_SET_AE_TARGET \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 22, unsigned int)
#define VIDIOC_VISS_ISP_SENSOR_HDR_MODE \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 25, unsigned int)

#define V4L2_CID_WIN_POS_X	(V4L2_CTRL_CLASS_CAMERA | 0x1005)
#define V4L2_CID_WIN_POS_Y	(V4L2_CTRL_CLASS_CAMERA | 0x1006)
#define V4L2_CID_WIN_WIDTH	(V4L2_CTRL_CLASS_CAMERA | 0x1007)
#define V4L2_CID_WIN_HEIGHT	(V4L2_CTRL_CLASS_CAMERA | 0x1008)

#ifndef O_RDWR
#define O_RDWR		00000002
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK	00004000
#endif


/*
 * id: isp id
 */
void face_set_ae_para(int id, viss_isp_rects_t *rects);
int set_rgb_sensor_hdr(int id, unsigned int hdr_mode);
int get_rgb_sensor_exif(int id, struct viss_isp_exif *exif);
int get_nir_sensor_exif(int id, struct viss_isp_exif *exif);
int set_exp_bias(int id, int exp_bias);
int set_wb_mode(int id, int wb_mode);
int set_wdr_mode(int id, int wdr_mode);
int set_saturation(int id, int saturation);
int face_ae_init(void);
void face_ae_exit(void);

#endif /* _FACE_SVC_ISP_H_ */

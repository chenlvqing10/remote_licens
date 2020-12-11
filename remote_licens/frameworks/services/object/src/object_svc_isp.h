#ifndef _OBJECT_SVC_ISP_H_
#define _OBJECT_SVC_ISP_H_
#include <linux/types.h>
#include <linux/videodev2.h>

/**
 * struct viss_isp_exif - structure for the EXIF information of VISS
 * @exposure_time: exposure time register value
 * @shutter_speed: speed of the shutter register value
 * @aperture: aperture register value
 * @exposure_bias: it calls also EV bias
 * @iso_speed: ISO register value
 * @flash: status register value of the flash
 * @back_score: back light score
 */
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
 * id: OBJECT_CAMERA_RGB, OBJECT_CAMERA_NIR
 */
void object_buffer_size(int id, int width, int height, int rotation);
void object_set_ae_para(int id, viss_isp_rects_t *rects);
int object_set_hdr_mode(int id, unsigned int hdr_mode);
int object_ae_init(void);
void object_ae_exit(void);

#endif /* _OBJECT_SVC_ISP_H_ */

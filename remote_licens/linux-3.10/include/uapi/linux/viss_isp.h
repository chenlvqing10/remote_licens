/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS_ISP driver code for LomboTech
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

#ifndef LOMBO_ISP_USER_H
#define LOMBO_ISP_USER_H

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
 * @res[3]: original target
 * @res[4]: final target
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
	__u32 res[16];
};

enum stat_type {
	BGM_STAT_BUF,
	ISM_STAT_BUF,
	MAX_STAT,
};

/*
 * Private IOCTLs
 */
#define VIDIOC_VISS_ISP_STREAMON_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 1, unsigned long)
#define VIDIOC_VISS_ISP_STREAMOFF_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 2, unsigned long)
#define VIDIOC_VISS_ISP_OPEN_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 3, unsigned long)
#define VIDIOC_VISS_ISP_CLOSE_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 4, unsigned long)
#define VIDIOC_VISS_ISP_IRQ_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 5, unsigned long)
#define VIDIOC_VISS_ISP_SET_FMT_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 6, unsigned long)
#define VIDIOC_VISS_ISP_WRITE_REG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 7, struct viss_isp_reg_data)
#define VIDIOC_VISS_ISP_READ_REG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 8, struct viss_isp_reg_data)
#define VIDIOC_VISS_ISP_WRITE_SEQREG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 9, struct viss_isp_reg_seqdata)
#define VIDIOC_VISS_ISP_READ_SEQREG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 10, struct viss_isp_reg_seqdata)
#define VIDIOC_VISS_ISP_STAT_REQ \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 11, struct viss_isp_stat_data)
#define VIDIOC_VISS_SET_SENSOR \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 12, struct viss_isp_reg_data)
#define VIDIOC_VISS_GET_SENSOR \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 13, struct viss_isp_reg_data)
#define VIDIOC_VISS_GET_SENSOR_INFO \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 14, struct viss_isp_sensor_info)
#define VIDIOC_VISS_ISP_RESET \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 16, unsigned int)
#define VIDIOC_VISS_ISP_RESTART_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 17, unsigned long)
#define VIDIOC_VISS_ISP_EXP_CTRL \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 18, struct viss_isp_exp_gain)
#define VIDIOC_VISS_ISP_SENSOR_FMT_INFO \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 19, struct viss_isp_sensor_fmt_info)
#define VIDIOC_ISP_SET_AE_METER \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 20, struct viss_isp_ae_para)
#define VIDIOC_VISS_ISP_EXIF_REQ \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 21, struct viss_isp_exif)
#define VIDIOC_ISP_SET_AE_TARGET \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 22, unsigned int)
#define VIDIOC_VISS_ISP_REPARSE_ACK \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 23, unsigned long)
#define VIDIOC_VISS_ISP_GET_FRAME \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 24, struct viss_isp_frame_data)
#define VIDIOC_VISS_ISP_SENSOR_HDR_MODE \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 25, unsigned int)

#define VIDIOC_VISS_GET_ISP_DEV \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 26, int)
#define VIDIOC_VISS_GET_SENSOR_DEV \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 27, int)

/*
 * Events
 */

#define V4L2_EVENT_VISS_CLASS		(V4L2_EVENT_PRIVATE_START | 0x100)
#define V4L2_EVENT_ISP_STREAMON		(V4L2_EVENT_VISS_CLASS | 0x1)
#define V4L2_EVENT_ISP_STREAMOFF	(V4L2_EVENT_VISS_CLASS | 0x2)
#define V4L2_EVENT_ISP_OPEN		(V4L2_EVENT_VISS_CLASS | 0x3)
#define V4L2_EVENT_ISP_CLOSE		(V4L2_EVENT_VISS_CLASS | 0x4)
#define V4L2_EVENT_ISP_IRQ		(V4L2_EVENT_VISS_CLASS | 0x5)
#define V4L2_EVENT_ISP_SET_FMT		(V4L2_EVENT_VISS_CLASS | 0x6)
#define V4L2_EVENT_ISP_RESTART		(V4L2_EVENT_VISS_CLASS | 0x7)
#define V4L2_EVENT_ISP_SET_ADJUST	(V4L2_EVENT_VISS_CLASS | 0x8)
#define V4L2_EVENT_ISP_GET_ADJUST	(V4L2_EVENT_VISS_CLASS | 0x9)
#define V4L2_EVENT_ISP_FRAME_COMPLETE	(V4L2_EVENT_VISS_CLASS | 0x10)

#define V4L2_EVENT_ISP_REPARSE		(V4L2_EVENT_VISS_CLASS | 0x20)
#define V4L2_EVENT_ISP_USB_RD		(V4L2_EVENT_VISS_CLASS | 0x21)
#define V4L2_EVENT_ISP_USB_WR		(V4L2_EVENT_VISS_CLASS | 0x22)

#define ISP_PATTERN_RGGB	0
#define ISP_PATTERN_GRBG	1
#define ISP_PATTERN_GBRG	2
#define ISP_PATTERN_BGGR	3

#define ISP_INPUT_8BITS_Z	0
#define ISP_INPUT_8BITS		1
#define ISP_INPUT_10BITS	2
#define ISP_INPUT_10BITS_Z	3
#define ISP_INPUT_12BITS	4

#define ISP_ADJUST_CMD_MANUAL_CONTRAST		(0)
#define ISP_ADJUST_CMD_MANUAL_SATURATION	(1)
#define ISP_ADJUST_CMD_MANUAL_SHARPNESS		(2)
#define ISP_ADJUST_CMD_MANUAL_EXPOSURE		(3)
#define ISP_ADJUST_CMD_MANUAL_BRIGHTNESS	(4)
#define ISP_ADJUST_CMD_MANUAL_HUE		(5)
#define ISP_ADJUST_CMD_WIN_POS_X		(6)
#define ISP_ADJUST_CMD_WIN_POS_Y		(7)
#define ISP_ADJUST_CMD_WIN_WIDTH		(8)
#define ISP_ADJUST_CMD_WIN_HEIGHT		(9)
#define ISP_ADJUST_CMD_EXP_METERING		(10)
#define ISP_ADJUST_CMD_EXP_BIAS			(11)
#define ISP_ADJUST_CMD_AWB_MODE			(12)
#define ISP_ADJUST_CMD_WDR_MODE			(13)

#define V4L2_CID_WIN_POS_X	(V4L2_CTRL_CLASS_CAMERA | 0x1005)
#define V4L2_CID_WIN_POS_Y	(V4L2_CTRL_CLASS_CAMERA | 0x1006)
#define V4L2_CID_WIN_WIDTH	(V4L2_CTRL_CLASS_CAMERA | 0x1007)
#define V4L2_CID_WIN_HEIGHT	(V4L2_CTRL_CLASS_CAMERA | 0x1008)


#define ISP_SET_CMD_FLAG		(1 << 31)
#define ISP_SET_CMD_EXPOSURE_TIME	(ISP_SET_CMD_FLAG | 0x0001)
#define ISP_SET_CMD_SHUTTER_SPEED	(ISP_SET_CMD_FLAG | 0x0002)
#define ISP_SET_CMD_APERTURE		(ISP_SET_CMD_FLAG | 0x0003)
#define ISP_SET_CMD_BRIGHTNESS		(ISP_SET_CMD_FLAG | 0x0004)
#define ISP_SET_CMD_EXPOSURE_BIAS	(ISP_SET_CMD_FLAG | 0x0005)
#define ISP_SET_CMD_ISO_SPEED		(ISP_SET_CMD_FLAG | 0x0006)
#define ISP_SET_CMD_FLASH		(ISP_SET_CMD_FLAG | 0x0007)
#define ISP_SET_CMD_ILLU_ID		(ISP_SET_CMD_FLAG | 0x0008)
#define ISP_SET_CMD_BACK_SCORE		(ISP_SET_CMD_FLAG | 0x0009)
#define ISP_SET_CMD_OBJ_LUM		(ISP_SET_CMD_FLAG | 0x000A)
#define ISP_SET_CMD_BACK_LUM		(ISP_SET_CMD_FLAG | 0x000B)
#define ISP_SET_CMD_AVG_LUM		(ISP_SET_CMD_FLAG | 0x000C)
#define ISP_SET_CMD_ORG_TARGET		(ISP_SET_CMD_FLAG | 0x000D)
#define ISP_SET_CMD_FINAL_TARGET	(ISP_SET_CMD_FLAG | 0x000E)
#define ISP_SET_CMD_AWB_STAT		(ISP_SET_CMD_FLAG | 0x000F)

#define ISP_USB_CMD_GET_IMAGE		(0x01)
#define ISP_USB_CMD_GET_JSON		(0x02)
#define ISP_USB_CMD_GET_SENSOR_INFO	(0x03)
#define ISP_USB_CMD_GET_SENSOR_REG	(0x04)

#define ISP_USB_CMD_SET_SENSOR_REG	(0x40)

#define ISP_USB_CMD_SET_JSON		(0x80)
#define ISP_USB_CMD_SET_RELOAD		(0x81)
#define ISP_USB_CMD_SET_JSON_RELOAD	(0x82)

struct isp_usb_cmd {
	u8 cmd;
	u8 flag;
	u32 addr;
	u32 len;
	u8  res[6];
} __packed;

struct viss_isp_reg_data {
	__u32 reg_add;
	__u32 reg_value;
};

struct viss_isp_reg_seqdata {
	__u32 reg_size;
	__u32 __user *reg_add;
	__u32 __user *reg_value;
};

struct viss_isp_exp_gain {
	__u32 exp_time;
	__u32 exp_gain;
};

struct viss_isp_stat_event_status {
	__u32 frame_number;
	__u16 config_counter;
	__u8 buf_err;
	__u32 isp_status;
};
struct viss_isp_stat_data {
	void __user *buf;
	__u32 buf_size;
	__u32 buf_type;
};

struct viss_isp_frame_data {
	struct timeval ts;
	void __user *frame_buf;
	__u32 frame_size;
	__u16 frame_number;
};

struct viss_v4l2_event_data {
	/*word 0*/
	__u32 main_path_width;
	/*word 1*/
	__u32 main_path_height;
	/*word 2*/
	__u32 sub_path_width;
	/*word 3*/
	__u32 sub_path_height;
	/*word 4*/
	__u32 isp_adjust_cmd;
	/*word 5*/
	__u32 isp_adjust_val;
	/*word 6*/
	__u32 nop6;
	/*word 7*/
	__u32 nop7;
	/*word 8*/
	__u32 nop8;
	/*word 9*/
	__u32 nop9;
	/*word 10*/
	__u32 nop10;
	/*word 11*/
	__u32 nop11;
	/*word 12*/
	__u32 nop12;
	/*word 13*/
	__u32 nop13;
	/*word 14*/
	__u32 nop14;
	/*word 15*/
	__u32 nop15;
};
struct viss_isp_sensor_info {
	char sensor_name[32];
	int width;
	int height;
	int json_len;
	int res[4096];
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

struct viss_isp_ae_para {
	struct v4l2_rect rect;
	enum v4l2_exposure_metering ae_mode;
};
int viss_isp_usb_read(void *cmd, void *data);
int viss_isp_usb_write(void *cmd, void *data);

#endif	/* LOMBO_ISP_USER_H */

/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ov5640 mipi-csi sensor driver code for LomboTech
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

#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/viss_isp.h>

#include <media/lombo_viss.h>

#include <mach/debug.h>
#include "../ccc.h"

#define OV5640_VGA_WIDTH		640
#define OV5640_VGA_HEIGHT		480

#define OV5640_720P_WIDTH		1280
#define OV5640_720P_HEIGHT		720

#define OV5640_1080P_WIDTH		1920
#define OV5640_1080P_HEIGHT		1080

/*
 * 2lane YUV init
 */
static const cc_reg_t init_reg_list[] = {
	{ 0x3103, 0x11 }, /* SCCB system control */
	{ 0x3008, 0x82 }, /* software reset */
	{ CC_REG_DELAY, 5 },
	{ 0x3008, 0x42 }, /* software power down */
	{ 0x3103, 0x03 }, /* SCCB system control */
	{ 0x3017, 0x00 }, /* set Frex, Vsync, Href, PCLK, D[9:6] input */
	{ 0x3018, 0x00 }, /* set d[5:0], GPIO[1:0] input */
	{ 0x3034, 0x18 }, /* MIPI 8-bit mode */
	{ 0x3037, 0x13 }, /* PLL */
	{ 0x3108, 0x01 }, /* system divider */
	{ 0x3630, 0x36 },
	{ 0x3631, 0x0e },
	{ 0x3632, 0xe2 },
	{ 0x3633, 0x12 },
	{ 0x3621, 0xe0 },
	{ 0x3704, 0xa0 },
	{ 0x3703, 0x5a },
	{ 0x3715, 0x78 },
	{ 0x3717, 0x01 },
	{ 0x370b, 0x60 },
	{ 0x3705, 0x1a },
	{ 0x3905, 0x02 },
	{ 0x3906, 0x10 },
	{ 0x3901, 0x0a },
	{ 0x3731, 0x12 },
	{ 0x3600, 0x08 }, /* VCM debug mode */
	{ 0x3601, 0x33 }, /* VCM debug mode */
	{ 0x302d, 0x60 }, /* system control */
	{ 0x3620, 0x52 },
	{ 0x371b, 0x20 },
	{ 0x471c, 0x50 },
	{ 0x3a13, 0x43 }, /* AGC pre-gain, 0x40 = 1x */
	{ 0x3a18, 0x00 }, /* gain ceiling */
	{ 0x3a19, 0xf8 }, /* gain ceiling */
	{ 0x3635, 0x13 },
	{ 0x3636, 0x03 },
	{ 0x3634, 0x40 },
	{ 0x3622, 0x01 },
	/* 50Hz/60Hz */
	{ 0x3c01, 0x34 }, /* 50/ 60Hz */
	{ 0x3c04, 0x28 }, /* threshold for low sum */
	{ 0x3c05, 0x98 }, /* threshold for high sum */
	{ 0x3c06, 0x00 }, /* light meter 1 threshold high */
	{ 0x3c08, 0x00 }, /* light meter 2 threshold high */
	{ 0x3c09, 0x1c }, /* light meter 2 threshold low */
	{ 0x3c0a, 0x9c }, /* sample number high */
	{ 0x3c0b, 0x40 }, /* sample number low */
	/* timing */
	{ 0x3800, 0x00 }, /* HS */
	{ 0x3801, 0x00 }, /* HS */
	{ 0x3802, 0x00 }, /* VS */
	{ 0x3804, 0x0a }, /* HW */
	{ 0x3805, 0x3f }, /* HW */
	{ 0x3810, 0x00 }, /* H offset high */
	{ 0x3811, 0x10 }, /* H offset low */
	{ 0x3812, 0x00 }, /* V offset high */
	{ 0x3708, 0x64 },
	{ 0x3a08, 0x01 }, /* B50 */
	{ 0x4001, 0x02 }, /* BLC start line */
	{ 0x4005, 0x1a }, /* BLC always update */
	{ 0x3000, 0x00 }, /* system reset 0 */
	{ 0x3002, 0x1c }, /* system reset 2 */
	{ 0x3004, 0xff }, /* clock enable 00 */
	{ 0x3006, 0xc3 }, /* clock enable 2 */
	{ 0x300e, 0x45 }, /* MIPI control, 2 lane, MIPI enable */
	{ 0x302e, 0x08 },

	{ 0x4300, 0x30 }, /* YUV 422, YUYV(datasheet wrong£¬is VYUY) */
	{ 0x501f, 0x00 }, /* ISP YUV 422 */

	{ 0x4407, 0x04 }, /* JPEG QS */
	{ 0x440e, 0x00 },

	/* clock lane gating enable 2017-11-28 */
	{ 0x4800, 0x24 },

	{ 0x5000, 0xa7 }, /* ISP control, Lenc on, gamma on,
			   * BPC on, WPC on, CIP on */
	/* AWB */
	{ 0x5180, 0xff },
	{ 0x5181, 0xf2 },
	{ 0x5182, 0x00 },
	{ 0x5183, 0x14 },
	{ 0x5184, 0x25 },
	{ 0x5185, 0x24 },
	{ 0x5186, 0x09 },
	{ 0x5187, 0x09 },
	{ 0x5188, 0x09 },
	{ 0x5189, 0x75 },
	{ 0x518a, 0x54 },
	{ 0x518b, 0xe0 },
	{ 0x518c, 0xb2 },
	{ 0x518d, 0x42 },
	{ 0x518e, 0x3d },
	{ 0x518f, 0x56 },
	{ 0x5190, 0x46 },
	{ 0x5191, 0xf8 },
	{ 0x5192, 0x04 },
	{ 0x5193, 0x70 },
	{ 0x5194, 0xf0 },
	{ 0x5195, 0xf0 },
	{ 0x5196, 0x03 },
	{ 0x5197, 0x01 },
	{ 0x5198, 0x04 },
	{ 0x5199, 0x12 },
	{ 0x519a, 0x04 },
	{ 0x519b, 0x00 },
	{ 0x519c, 0x06 },
	{ 0x519d, 0x82 },
	{ 0x519e, 0x38 },
	/* color matrix */
	{ 0x5381, 0x1e },
	{ 0x5382, 0x5b },
	{ 0x5383, 0x08 },
	{ 0x5384, 0x0a },
	{ 0x5385, 0x7e },
	{ 0x5386, 0x88 },
	{ 0x5387, 0x7c },
	{ 0x5388, 0x6c },
	{ 0x5389, 0x10 },
	{ 0x538a, 0x01 },
	{ 0x538b, 0x98 },
	/* CIP */
	{ 0x5300, 0x08 }, /* sharpen MT th1 */
	{ 0x5301, 0x30 }, /* sharpen MT th2 */
	{ 0x5302, 0x10 }, /* sharpen MT offset 1 */
	{ 0x5303, 0x00 }, /* sharpen MT offset 2 */
	{ 0x5304, 0x08 }, /* DNS threshold 1 */
	{ 0x5305, 0x30 }, /* DNS threshold 2 */
	{ 0x5306, 0x08 }, /* DNS offset 1 */
	{ 0x5307, 0x16 }, /* DNS offset 2 */
	{ 0x5309, 0x08 }, /* sharpen TH th1 */
	{ 0x530a, 0x30 }, /* sharpen TH th2 */
	{ 0x530b, 0x04 }, /* sharpen TH offset 1 */
	{ 0x530c, 0x06 }, /* sharpen Th offset 2 */
	/* gamma */
	{ 0x5480, 0x01 },
	{ 0x5481, 0x08 },
	{ 0x5482, 0x14 },
	{ 0x5483, 0x28 },
	{ 0x5484, 0x51 },
	{ 0x5485, 0x65 },
	{ 0x5486, 0x71 },
	{ 0x5487, 0x7d },
	{ 0x5488, 0x87 },
	{ 0x5489, 0x91 },
	{ 0x548a, 0x9a },
	{ 0x548b, 0xaa },
	{ 0x548c, 0xb8 },
	{ 0x548d, 0xcd },
	{ 0x548e, 0xdd },
	{ 0x548f, 0xea },
	{ 0x5490, 0x1d },
	/* UV adjust */
	{ 0x5580, 0x06 }, /* sat on, contrast on */
	{ 0x5583, 0x40 }, /* sat U */
	{ 0x5584, 0x10 }, /* sat V */
	{ 0x5589, 0x10 }, /* UV adjust th1 */
	{ 0x558a, 0x00 }, /* UV adjust th2[8] */
	{ 0x558b, 0xf8 }, /* UV adjust th2[7:0] */
	{ 0x501d, 0x04 }, /* enable manual offset of contrast */
	/* lens correction */
	{ 0x5800, 0x23 },
	{ 0x5801, 0x14 },
	{ 0x5802, 0x0f },
	{ 0x5803, 0x0f },
	{ 0x5804, 0x12 },
	{ 0x5805, 0x26 },
	{ 0x5806, 0x0c },
	{ 0x5807, 0x08 },
	{ 0x5808, 0x05 },
	{ 0x5809, 0x05 },
	{ 0x580a, 0x08 },
	{ 0x580b, 0x0d },
	{ 0x580c, 0x08 },
	{ 0x580d, 0x03 },
	{ 0x580e, 0x00 },
	{ 0x580f, 0x00 },
	{ 0x5810, 0x03 },
	{ 0x5811, 0x09 },
	{ 0x5812, 0x07 },
	{ 0x5813, 0x03 },
	{ 0x5814, 0x00 },
	{ 0x5815, 0x01 },
	{ 0x5816, 0x03 },
	{ 0x5817, 0x08 },
	{ 0x5818, 0x0d },
	{ 0x5819, 0x08 },
	{ 0x581a, 0x05 },
	{ 0x581b, 0x06 },
	{ 0x581c, 0x08 },
	{ 0x581d, 0x0e },
	{ 0x581e, 0x29 },
	{ 0x581f, 0x17 },
	{ 0x5820, 0x11 },
	{ 0x5821, 0x11 },
	{ 0x5822, 0x15 },
	{ 0x5823, 0x28 },
	{ 0x5824, 0x46 },
	{ 0x5825, 0x26 },
	{ 0x5826, 0x08 },
	{ 0x5827, 0x26 },
	{ 0x5828, 0x64 },
	{ 0x5829, 0x26 },
	{ 0x582a, 0x24 },
	{ 0x582b, 0x22 },
	{ 0x582c, 0x24 },
	{ 0x582d, 0x24 },
	{ 0x582e, 0x06 },
	{ 0x582f, 0x22 },
	{ 0x5830, 0x40 },
	{ 0x5831, 0x42 },
	{ 0x5832, 0x24 },
	{ 0x5833, 0x26 },
	{ 0x5834, 0x24 },
	{ 0x5835, 0x22 },
	{ 0x5836, 0x22 },
	{ 0x5837, 0x26 },
	{ 0x5838, 0x44 },
	{ 0x5839, 0x24 },
	{ 0x583a, 0x26 },
	{ 0x583b, 0x28 },
	{ 0x583c, 0x42 },
	{ 0x583d, 0xce },
	{ 0x5025, 0x00 },
	{ 0x3a0f, 0x30 }, /* stable in high */
	{ 0x3a10, 0x28 }, /* stable in low */
	{ 0x3a1b, 0x30 }, /* stable out high */
	{ 0x3a1e, 0x26 }, /* stable out low */
	{ 0x3a11, 0x60 }, /* fast zone high */
	{ 0x3a1f, 0x14 }, /* fast zone low */
	{ 0x3008, 0x02 }, /* wake up */
};

/*
 * VGA 30fps
 * Input Clock = 24Mhz bit rate 224Mbps
 */
static const cc_reg_t ov5640_yuv_vga[] = {
	{ 0x3035, 0x14 }, /* pll */
	{ 0x3036, 0x38 }, /* pll */
	{ 0x3c07, 0x08 }, /* light meter 1 threshold */
	{ 0x3820, 0x41 }, /* ISP flip off, sensor flip off */
	{ 0x3821, 0x07 }, /* ISP mirror on, sensor mirror on */
	/* timing */
	{ 0x3814, 0x31}, /* X inc */
	{ 0x3815, 0x31}, /* Y inc */

	{ 0x3803, 0x04 }, /* VS */
	{ 0x3806, 0x07 }, /* VH */
	{ 0x3807, 0x9b }, /* VH */
	{ 0x3808, 0x02 }, /* DVPHO */
	{ 0x3809, 0x80 }, /* DVPHO */
	{ 0x380a, 0x01 }, /* DVPVO */
	{ 0x380b, 0xe0 }, /* DVPVO */
	{ 0x380c, 0x07 }, /* HTS */
	{ 0x380d, 0x68 }, /* HTS */
	{ 0x380e, 0x03 }, /* VTS */
	{ 0x380f, 0xd8 }, /* VTS */
	{ 0x3813, 0x06 }, /* V offset */

	{ 0x3618, 0x00 },
	{ 0x3612, 0x29 },
	{ 0x3709, 0x52 },
	{ 0x370c, 0x03 },
	{ 0x3a02, 0x03 }, /* 60Hz max exposure */
	{ 0x3a03, 0xd8 }, /* 60Hz max exposure */
	{ 0x3a09, 0x27 }, /* B50 low */
	{ 0x3a0a, 0x00 }, /* B60 high */
	{ 0x3a0b, 0xf6 }, /* B60 low */
	{ 0x3a0e, 0x03 }, /* B50 max */
	{ 0x3a0d, 0x04 }, /* B60 max */
	{ 0x3a14, 0x03 }, /* 50Hz max exposure */
	{ 0x3a15, 0xd8 }, /* 50Hz max exposure */
	{ 0x4004, 0x02 }, /* BLC line number */
	{ 0x4713, 0x03 }, /* JPEG mode 3 */
	{ 0x460b, 0x35 }, /* debug */
	{ 0x460c, 0x22 }, /* VFIFO, PCLK manual */
	{ 0x4837, 0x44 }, /* MIPI global timing */
	{ 0x3824, 0x02 }, /* PCLK divider */
	{ 0x5001, 0xa3 }, /* SDE on, scale on, UV average off, CMX on, AWB on */
};

/*
 * 720p 60fps
 * Input Clock = 24Mhz bit rate 672Mbps
 */
static const cc_reg_t ov5640_yuv_720p[] = {
	{ 0x3035, 0x21 }, /* 0x11:60fps, 0x21:30fps 0x31:20fps 0x41: 15fps */
	{ 0x3036, 0x54 }, /* pll */
	{ 0x3c07, 0x07 }, /* light meter 1 threshold */
	{ 0x3820, 0x41 }, /* ISP flip off, sensor flip off */
	{ 0x3821, 0x07 }, /* ISP mirror on, sensor mirror on */
	/* timing */
	{ 0x3814, 0x31 }, /* X inc */
	{ 0x3815, 0x31 }, /* Y inc */
	{ 0x3803, 0xfa }, /* VS */
	{ 0x3806, 0x06 }, /* VH */
	{ 0x3807, 0xa9 }, /* VH */
	{ 0x3808, 0x05 }, /* DVPHO */
	{ 0x3809, 0x00 }, /* DVPHO */
	{ 0x380a, 0x02 }, /* DVPVO */
	{ 0x380b, 0xd0 }, /* DVPVO */
	{ 0x380c, 0x07 }, /* HTS */
	{ 0x380d, 0x64 }, /* HTS */
	{ 0x380e, 0x02 }, /* VTS */
	{ 0x380f, 0xe4 }, /* VTS */
	{ 0x3813, 0x04 }, /* V offset */
	{ 0x3618, 0x00 },
	{ 0x3612, 0x29 },
	{ 0x3709, 0x52 },
	{ 0x370c, 0x03 },
	/* banding filter */
	{ 0x3a02, 0x02 }, /* 60Hz max exposure */
	{ 0x3a03, 0xe4 }, /* 60Hz max exposure */
	{ 0x3a09, 0xbc }, /* B50 low */
	{ 0x3a0a, 0x01 }, /* B60 high */
	{ 0x3a0b, 0x72 }, /* B60 low */
	{ 0x3a0e, 0x01 }, /* B50 max */
	{ 0x3a0d, 0x02 }, /* B60 max */
	{ 0x3a14, 0x02 }, /* 50Hz max exposure */
	{ 0x3a15, 0xe4 }, /* 50Hz max exposure */
	{ 0x4004, 0x02 }, /* BLC line number */

#if 1
	/* 0, 60fps; 1, 30fps */
	{ 0x4713, 0x02 }, /* JPEG mode 2 */
	{ 0x460b, 0x37 }, /* 0x37 */
	{ 0x460c, 0x20 }, /* VFIFO, PCLK auto    0x20 */
	{ 0x4837, 0x16 }, /* MIPI global timing */
#else
	{ 0x4713, 0x00 }, /* JPEG mode */
	{ 0x460b, 0x35 },
	{ 0x460c, 0x22 }, /* VFIFO, PCLK manual */
	{ 0x4837, 0x0a }, /* MIPI global timing */
#endif
	{ 0x3824, 0x01 }, /* PCLK divider */

	{ 0x5001, 0x83 }, /* SDE on, scale off, UV average off,
			   * CMX on, AWB on 0x83 */
};

/*
 * 1080p 30fps
 * Input Clock = 24Mhz bit rate 672Mbps
 */
static const cc_reg_t ov5640_yuv_1080p[] = {
	{ 0x3820, 0x40 },
	{ 0x3821, 0x06 },
	/* pll and clock setting */
	{ 0x3034, 0x18 },
	{ 0x3035, 0x11 },  /* 0x11:30fps 0x21:15fps 0x41:7.5fps */
	{ 0x3036, 0x54 },
	{ 0x3037, 0x13 },
	{ 0x3108, 0x01 },
	{ 0x3824, 0x01 },

	{ 0x3814, 0x11 }, /* horizton subsample */
	{ 0x3815, 0x11 }, /* vertical subsample */
	{ 0x3800, 0x01 }, /* x address start high byte */
	{ 0x3801, 0x50 }, /* x address start low byte */
	{ 0x3802, 0x01 }, /* y address start high byte */
	{ 0x3803, 0xb2 }, /* y address start low byte */
	{ 0x3804, 0x08 }, /* x address end high byte */
	{ 0x3805, 0xef }, /* x address end low byte */
	{ 0x3806, 0x05 }, /* y address end high byte */
	{ 0x3807, 0xf1 }, /* y address end low byte */
	{ 0x3808, 0x07 }, /* H size MSB */
	{ 0x3809, 0x80 }, /* H size LSB */
	{ 0x380a, 0x04 }, /* V size MSB */
	{ 0x380b, 0x38 }, /* V size LSB */
	{ 0x380c, 0x09 }, /* HTS MSB */
	{ 0x380d, 0xc4 }, /* HTS LSB */
	{ 0x380e, 0x04 }, /* VTS MSB */
	{ 0x380f, 0x60 }, /* VTS LSB */
	{ 0x3813, 0x04 }, /* isp vertical offset low byte */
	{ 0x3618, 0x04 },
	{ 0x3612, 0x2b },
	{ 0x3709, 0x12 },
	{ 0x370c, 0x00 },

	{ 0x3a02, 0x04 }, /* 60HZ max exposure limit MSB */
	{ 0x3a03, 0x60 }, /* 60HZ max exposure limit LSB */
	{ 0x3a08, 0x01 }, /* 50HZ step MSB */
	{ 0x3a09, 0x50 }, /* 50HZ step LSB */
	{ 0x3a0a, 0x01 }, /* 60HZ step MSB */
	{ 0x3a0b, 0x18 }, /* 60HZ step LSB */
	{ 0x3a0e, 0x03 }, /* 50HZ step max */
	{ 0x3a0d, 0x04 }, /* 60HZ step max */
	{ 0x3a14, 0x04 }, /* 50HZ max exposure limit MSB */
	{ 0x3a15, 0x60 }, /* 50HZ max exposure limit LSB */
	{ 0x4004, 0x06 }, /* BLC line number */

	{ 0x4713, 0x00 }, /* JPEG mode */
	{ 0x460b, 0x35 },
	{ 0x460c, 0x22 }, /* VFIFO, PCLK manual */
	{ 0x4837, 0x0a }, /* MIPI global timing */
	{ 0x3824, 0x01 }, /* PCLK divider */
	{ 0x5001, 0x83 }, /* SDE on, scale off, UV average off,
			   * CMX on, AWB on */
};

static const cc_reg_t ov5640_yuv_vga_30fps[] = {
	{ 0x3035, 0x14 }, /* pll */
	{ 0x3036, 0x38 }, /* pll */
};

static const cc_reg_t ov5640_yuv_vga_60fps[] = {
	{ 0x3035, 0x11 }, /* pll */
	{ 0x3036, 0x4A }, /* pll */
};

static const cc_reg_t ov5640_yuv_vga_10fps[] = {
	{ 0x3035, 0x41 }, /* pll */
	{ 0x3036, 0x4A }, /* pll */
};

static const cc_reg_t ov5640_yuv_720p_30fps[] = {
	{ 0x3035, 0x21 },
	{ 0x4713, 0x02 }, /* JPEG mode 2 */
	{ 0x460b, 0x37 }, /* 0x37 */
	{ 0x460c, 0x20 }, /* VFIFO, PCLK auto    0x20 */
	{ 0x4837, 0x16 }, /* MIPI global timing */
};

static const cc_reg_t ov5640_yuv_720p_60fps[] = {
	{ 0x3035, 0x11 },
	{ 0x4713, 0x00 }, /* JPEG mode */
	{ 0x460b, 0x35 },
	{ 0x460c, 0x22 }, /* VFIFO, PCLK manual */
	{ 0x4837, 0x0a }, /* MIPI global timing */
};

static const cc_reg_t ov5640_yuv_1080p_30fps[] = {
	{ 0x3035, 0x11 },
};

static const cc_reg_t ov5640_yuv_1080p_15fps[] = {
	{ 0x3035, 0x21 },
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 5},
	{CC_PWDN,  CC_UP},
	{CC_DELAY, 10},
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 5},
	{CC_RESET,  CC_UP},
	{CC_DELAY, 10},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_UP},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

static const struct cc_win_size win_sizes[] = {
	{
		.width = OV5640_VGA_WIDTH,
		.height = OV5640_VGA_HEIGHT,
		.regs = ov5640_yuv_vga,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_vga),
	},
	{
		.width = OV5640_720P_WIDTH,
		.height = OV5640_720P_HEIGHT,
		.regs = ov5640_yuv_720p,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_720p),
	},
	{
		.width = OV5640_1080P_WIDTH,
		.height = OV5640_1080P_HEIGHT,
		.regs = ov5640_yuv_1080p,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_1080p),
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = OV5640_VGA_WIDTH,
		.height = OV5640_VGA_HEIGHT,

		.framerate = { 1, 30 },	/* 30 fps */
		.regs = ov5640_yuv_vga_30fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_vga_30fps),
		.mfreq = 224000000,	/* mipi-csi clock */
	},
	{
		.width = OV5640_VGA_WIDTH,
		.height = OV5640_VGA_HEIGHT,

		.framerate = { 1, 60 },	/* 60 fps */
		.regs = ov5640_yuv_vga_60fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_vga_60fps),
		.mfreq = 448000000,	/* mipi-csi clock */
	},
	{
		.width = OV5640_VGA_WIDTH,
		.height = OV5640_VGA_HEIGHT,

		.framerate = { 1, 10 },	/* 10 fps */
		.regs = ov5640_yuv_vga_10fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_vga_10fps),
		.mfreq = 112000000,	/* mipi-csi clock */
	},
	{
		.width = OV5640_720P_WIDTH,
		.height = OV5640_720P_HEIGHT,

		.framerate = { 1, 30 },	/* 30 fps */
		.regs = ov5640_yuv_720p_30fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_720p_30fps),
		.mfreq = 336000000,	/* mipi-csi clock */
	},
	{
		.width = OV5640_720P_WIDTH,
		.height = OV5640_720P_HEIGHT,

		.framerate = { 1, 60 },	/* 60 fps */
		.regs = ov5640_yuv_720p_60fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_720p_60fps),
		.mfreq = 672000000,	/* mipi-csi clock */
	},
	{
		.width = OV5640_1080P_WIDTH,
		.height = OV5640_1080P_HEIGHT,

		.framerate = { 1, 30 },	/* 30 fps */
		.regs = ov5640_yuv_1080p_30fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_1080p_30fps),
		.mfreq = 672000000,	/* mipi-csi clock */
	},
	{
		.width = OV5640_1080P_WIDTH,
		.height = OV5640_1080P_HEIGHT,

		.framerate = { 1, 15 },	/* 15 fps */
		.regs = ov5640_yuv_1080p_15fps,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_1080p_15fps),
		.mfreq = 336000000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = cc_si->read_reg(cc_si, 0x300A, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0x300B, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x5640) {
		PRT_ERR("ID wrong! (0x%04x != 0x5640)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int sensor_init(struct cc_sensor_info *cc_si)
{
	struct timespec ts_s;
	struct timespec ts_f;
	int ret = 0;

	PRT_INFO("\n");

	ret = sensor_detect(cc_si);
	if (ret != 0)
		return ret;

	ktime_get_ts(&ts_s);
	ret = cc_si->write_reg_list(cc_si, init_reg_list,
					ARRAY_SIZE(init_reg_list));
	if (ret != 0) {
		PRT_ERR("write %s init_reg_list failed!\n", cc_si->name);
		return -ENODEV;
	}
	ktime_get_ts(&ts_f);

	PRT_INFO("write %s init_reg_list finish.\n", cc_si->name);
	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

#if 0
	{
	u8 value = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(init_reg_list); i++) {
		if (init_reg_list[i].reg == CC_REG_DELAY)
			continue;

		ret = cc_si->read_reg(cc_si, init_reg_list[i].reg, &value);
		if (ret) {
			PRT_ERR("%s read i %d reg 0x%02x error\n",
				cc_si->name, i, init_reg_list[i].reg);
			return ret;
		}

		if (init_reg_list[i].val != value) {
			PRT_ERR("%s %d 0x%02x (0x%02x) != (0x%02x)\n",
				cc_si->name, i, init_reg_list[i].reg,
				value, init_reg_list[i].val);
		}
	}
	}
#endif

	return 0;

}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;

	if (on) {
		ret = cc_si->write_reg(cc_si, 0x4202, 0x00);
		if (ret) {
			PRT_ERR("%d\n", on);
			return ret;
		}

		ret = cc_si->write_reg(cc_si, 0x4800, 0x24);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->write_reg(cc_si, 0x4202, 0x0f);
		if (ret) {
			PRT_ERR("%d\n", on);
			return ret;
		}

		ret = cc_si->write_reg(cc_si, 0x4800, 0x01);
		if (ret)
			PRT_ERR("%d\n", on);
	}
	return ret;
}

/*
 * ctrl for raw sensor finish
 */


#define DRIVER_NAME "ov5640_mipi"

static int sensor_probe(struct platform_device *pdev)
{
	struct cc_sensor_info *cc_si = NULL;
	int ret = 0;

	PRT_INFO("*********probe begin\n");

	cc_si = devm_kzalloc(&pdev->dev, sizeof(*cc_si), GFP_KERNEL);
	if (cc_si == NULL) {
		PRT_ERR("Can't alloc memory!\n");
		return -ENOMEM;
	}

	cc_si->reg_bits = 16;
	cc_si->val_bits = 8;

	cc_si->dlanes = 2;

	cc_si->name = DRIVER_NAME;

	cc_si->power_up_act = power_up_act;
	cc_si->power_down_act = power_down_act;
	cc_si->npwr_up_acts = ARRAY_SIZE(power_up_act);
	cc_si->npwr_dn_acts = ARRAY_SIZE(power_down_act);

	cc_si->framefmts = frame_formats;
	cc_si->nfrmfmts = ARRAY_SIZE(frame_formats);

	cc_si->win_sizes = win_sizes;
	cc_si->nwsizes = ARRAY_SIZE(win_sizes);

	cc_si->framecfgs = frame_cfgs;
	cc_si->nfrmcfgs = ARRAY_SIZE(frame_cfgs);

	ret = cc_sensor_probe(pdev, cc_si);
	if (ret < 0)
		return ret;

	cc_si->detect = sensor_detect;
	cc_si->sensor_init = sensor_init;
	cc_si->set_stream = sensor_set_stream;
	cc_si->sensor_ioctl = NULL;

	PRT_INFO("probe end\n");
	return 0;
}

static int sensor_remove(struct platform_device *pdev)
{
	struct cc_sensor_info *cc_si = platform_get_drvdata(pdev);

	cc_sensor_remove(pdev);

	devm_kfree(&pdev->dev, cc_si);

	return 0;
}

static const struct of_device_id ov5640_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov5640_mipi_of_match);

static struct platform_driver ov5640_mipi_driver = {
	.driver = {
		.of_match_table	= ov5640_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov5640_mipi_driver);

MODULE_DESCRIPTION("OV5640-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);

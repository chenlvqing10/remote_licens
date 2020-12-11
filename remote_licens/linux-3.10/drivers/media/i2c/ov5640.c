/*
 * Copyright (C) 2005-2006 Micronas USA Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (Version 2) as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>

#include <media/v4l2-subdev.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>

MODULE_DESCRIPTION("OmniVision ov5640 sensor driver");
MODULE_LICENSE("GPL v2");

#include <mach/csp.h>
#include <mach/debug.h>

/* #define OV5640_IMAGE_1080P */
#define OV5640_IMAGE_720P

#define OV5640_MAX_WIDTH		2592
#define OV5640_MAX_HEIGHT		1936
#define OV5640_MIN_WIDTH		32
#define OV5640_MIN_HEIGHT		32

#define OV5640_VGA_WIDTH		640
#define OV5640_VGA_HEIGHT		480

#define OV5640_1080P_WIDTH		1920
#define OV5640_1080P_HEIGHT		1080

#define OV5640_720P_WIDTH		1280
#define OV5640_720P_HEIGHT		720

static struct i2c_client *ov5640_client;

typedef struct ov5640_config {
	struct v4l2_subdev sd;
	struct media_pad pad;
#if 0
	enum v4l2_mbus_type bus_type;

	/* External master clock frequency */
	unsigned long mclk_frequency;
#endif
	/* Protects the struct fields below */
	struct mutex lock;

	struct i2c_client *client;

	/* Exposure row interval in us */
	unsigned int exp_row_interval;

	unsigned short id;

	/* YUYV sequence (pixel format) control register */
	u8 tslb_reg;
	struct v4l2_mbus_framefmt format;

	int reset_gpio;
	int pwdn_gpio;

#if 0
	int streaming;
	int power;
#endif
} ov5640_config_t;

typedef struct sensor_reg {
	u16 reg_add;
	u8 reg_value;
} sensor_reg_t;

static void ov5640_reset_cycle(ov5640_config_t *ov5640);

/*
 * 15fps VGA YUV output
 * 24MHz input clock, 24MHz PCLK
 */
static const sensor_reg_t ov5640_reg_list[] = {
	{ 0x3103, 0x11 }, /* system clock from pad, bit[1] */
	{ 0x3008, 0x82 }, /* software reset, bit[7] */
	/* mdelay(5); */
	{ 0x3008, 0x42 }, /* software power down, bit[6] */
	{ 0x3103, 0x03 }, /* system clock from PLL, bit[1] */
	{ 0x3017, 0xff }, /* FREX, Vsync, HREF, PCLK, D[9:6] output enable */
	{ 0x3018, 0xff }, /* D[5:0], GPIO[1:0] output enable */
	{ 0x3034, 0x18 }, /* MIPI 8-bit */

	/*
	 * PLL root divider, bit[4], PLL pre-divider, bit[3:0],
	 * default is 0x13
	 */
	{ 0x3037, 0x13 },

	/* PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2] */
	{ 0x3108, 0x01 },
	/*
	 * SCLK root divider, bit[1:0]
	 */
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
	{ 0x3600, 0x08 }, /* VCM control */
	{ 0x3601, 0x33 }, /* VCM control */
	{ 0x302d, 0x60 }, /* system control */
	{ 0x3620, 0x52 },
	{ 0x371b, 0x20 },
	{ 0x471c, 0x50 },
	{ 0x3a13, 0x43 }, /* pre-gain = 1.047x */
	{ 0x3a18, 0x00 }, /* gain ceiling */
	{ 0x3a19, 0xf8 }, /* gain ceiling = 15.5x */
	{ 0x3635, 0x13 },
	{ 0x3636, 0x03 },
	{ 0x3634, 0x40 },
	{ 0x3622, 0x01 },

	/* 50/60Hz detection */
	{ 0x3c01, 0x34 }, /* Band auto, bit[7] */
	{ 0x3c04, 0x28 }, /* threshold low sum */
	{ 0x3c05, 0x98 }, /* threshold high sum */
	{ 0x3c06, 0x00 }, /* light meter 1 threshold[15:8] */
	{ 0x3c07, 0x08 }, /* light meter 1 threshold[7:0] */
	{ 0x3c08, 0x00 }, /* light meter 2 threshold[15:8] */
	{ 0x3c09, 0x1c }, /* light meter 2 threshold[7:0] */
	{ 0x3c0a, 0x9c }, /* sample number[15:8] */
	{ 0x3c0b, 0x40 }, /* sample number[7:0] */
	{ 0x3810, 0x00 }, /* Timing Hoffset[11:8] */
	{ 0x3811, 0x10 }, /* Timing Hoffset[7:0] */
	{ 0x3812, 0x00 }, /* Timing Voffset[10:8] */
	{ 0x3708, 0x64 },
	{ 0x4001, 0x02 }, /* BLC start from line 2 */
	{ 0x4005, 0x1a }, /* BLC always update */
	{ 0x3000, 0x00 }, /* enable blocks */
	{ 0x3004, 0xff }, /* enable clocks */
	{ 0x300e, 0x58 }, /* MIPI power down, DVP enable */
	{ 0x302e, 0x00 },
	{ 0x4300, 0x30 }, /* YUV 422, YUYV 2018-3-19  YUYV */
	{ 0x501f, 0x00 }, /* YUV 422 */
	{ 0x440e, 0x00 },
	{ 0x5000, 0xa7 }, /* Lenc on, raw gamma on, BPC on, WPC on, CIP on */

	/* AEC target */
	{ 0x3a0f, 0x30 }, /* stable range in high */
	{ 0x3a10, 0x28 }, /* stable range in low */
	{ 0x3a1b, 0x30 }, /* stable range out high */
	{ 0x3a1e, 0x26 }, /* stable range out low */
	{ 0x3a11, 0x60 }, /* fast zone high */
	{ 0x3a1f, 0x14 }, /* fast zone low */

	/* Lens correction for ? */
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
	{ 0x583d, 0xce }, /* lenc BR offset */

	/* AWB */
	{ 0x5180, 0xff }, /* AWB B block */
	{ 0x5181, 0xf2 }, /* AWB control */
	{ 0x5182, 0x00 }, /* [7:4] max local counter, [3:0] max fast counter */
	{ 0x5183, 0x14 }, /* AWB advanced */
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
	{ 0x5191, 0xf8 }, /* AWB top limit */
	{ 0x5192, 0x04 }, /* AWB bottom limit */
	{ 0x5193, 0x70 }, /* red limit */
	{ 0x5194, 0xf0 }, /* green limit */
	{ 0x5195, 0xf0 }, /* blue limit */
	{ 0x5196, 0x03 }, /* AWB control */
	{ 0x5197, 0x01 }, /* local limit */
	{ 0x5198, 0x04 },
	{ 0x5199, 0x12 },
	{ 0x519a, 0x04 },
	{ 0x519b, 0x00 },
	{ 0x519c, 0x06 },
	{ 0x519d, 0x82 },
	{ 0x519e, 0x38 }, /* AWB control */

	/* Gamma */
	{ 0x5480, 0x01 }, /* Gamma bias plus on, bit[0] */
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

	/* color matrix */
	{ 0x5381, 0x1e }, /* CMX1 for Y */
	{ 0x5382, 0x5b }, /* CMX2 for Y */
	{ 0x5383, 0x08 }, /* CMX3 for Y */
	{ 0x5384, 0x0a }, /* CMX4 for U */
	{ 0x5385, 0x7e }, /* CMX5 for U */
	{ 0x5386, 0x88 }, /* CMX6 for U */
	{ 0x5387, 0x7c }, /* CMX7 for V */
	{ 0x5388, 0x6c }, /* CMX8 for V */
	{ 0x5389, 0x10 }, /* CMX9 for V */
	{ 0x538a, 0x01 }, /* sign[9] */
	{ 0x538b, 0x98 }, /* sign[8:1] */

	/* UV adjsut */
	{ 0x5580, 0x06 }, /* saturation on, bit[1] */
	{ 0x5583, 0x40 },
	{ 0x5584, 0x10 },
	{ 0x5589, 0x10 },
	{ 0x558a, 0x00 },
	{ 0x558b, 0xf8 },
	{ 0x501d, 0x40 }, /* enable manual offset of contrast */

	/* CIP */
	{ 0x5300, 0x08 }, /* CIP sharpen MT threshold 1 */
	{ 0x5301, 0x30 }, /* CIP sharpen MT threshold 2 */
	{ 0x5302, 0x10 }, /* CIP sharpen MT offset 1 */
	{ 0x5303, 0x00 }, /* CIP sharpen MT offset 2 */
	{ 0x5304, 0x08 }, /* CIP DNS threshold 1 */
	{ 0x5305, 0x30 }, /* CIP DNS threshold 2 */
	{ 0x5306, 0x08 }, /* CIP DNS offset 1 */
	{ 0x5307, 0x16 }, /* CIP DNS offset 2 */
	{ 0x5309, 0x08 }, /* CIP sharpen TH threshold 1 */
	{ 0x530a, 0x30 }, /* CIP sharpen TH threshold 2 */
	{ 0x530b, 0x04 }, /* CIP sharpen TH offset 1 */
	{ 0x530c, 0x06 }, /* CIP sharpen TH offset 2 */
	{ 0x5025, 0x00 },
};

/*
 * VGA 30fps
 * YUV VGA 30fps, night mode 5fps
 * Input Clock = 24Mhz, PCLK = 56MHz
 */
static const sensor_reg_t ov5640_yuv_vga_30fps[] = {
#if (defined CONFIG_ARCH_LOMBO_N7V0_FPGA) || \
	(defined CONFIG_ARCH_LOMBO_N7V1_FPGA)
	{ 0x3035, 0x41 }, /* PLL, default is 0x11 */
#else
	{ 0x3035, 0x11 }, /* PLL, default is 0x11 */
#endif
	{ 0x3036, 0x4A }, /* PLL, default is 0x46 */
	{ 0x3c07, 0x08 }, /* light meter 1 threshold [7:0] */
	{ 0x3820, 0x41 }, /* Sensor flip off, ISP flip on */
	{ 0x3821, 0x07 }, /* Sensor mirror on, ISP mirror on, H binning on */
	{ 0x3814, 0x31 }, /* X INC */
	{ 0x3815, 0x31 }, /* Y INC */
	{ 0x3800, 0x00 }, /* HS */
	{ 0x3801, 0x00 }, /* HS */
	{ 0x3802, 0x00 }, /* VS */
	{ 0x3803, 0x04 }, /* VS */
	{ 0x3804, 0x0a }, /* HW (HE) */
	{ 0x3805, 0x3f }, /* HW (HE) */
	{ 0x3806, 0x07 }, /* VH (VE) */
	{ 0x3807, 0x9b }, /* VH (VE) */
#if 1
	{ 0x3808, 0x02 }, /* DVPHO */
	{ 0x3809, 0x80 }, /* DVPHO */
	{ 0x380a, 0x01 }, /* DVPVO */
	{ 0x380b, 0xe0 }, /* DVPVO */
#else
	{ 0x3808, 0x00 }, /* DVPHO */
	{ 0x3809, 0xa0 }, /* DVPHO */
	{ 0x380a, 0x00 }, /* DVPVO */
	{ 0x380b, 0x78 }, /* DVPVO */
#endif
	{ 0x380c, 0x07 }, /* HTS */
	{ 0x380d, 0x68 }, /* HTS */
	{ 0x380e, 0x03 }, /* VTS */
	{ 0x380f, 0xd8 }, /* VTS */
	{ 0x3813, 0x06 }, /* Timing Voffset */

	/* bit[7:6]: output drive capability[00: 1x, 01: 2x, 10: 3x, 11: 4x] */
	{ 0x302c, 0x02 },

	{ 0x3618, 0x00 },
	{ 0x3612, 0x29 },
	{ 0x3709, 0x52 },
	{ 0x370c, 0x03 },
	{ 0x3a02, 0x17 }, /* 60Hz max exposure, night mode 5fps */
	{ 0x3a03, 0x10 }, /* 60Hz max exposure */
	/*
	 * banding filters are calculated automatically in camera driver
	 */
#if 1
	{ 0x3a08, 0x01 }, /* B50 step */
	{ 0x3a09, 0x86 }, /* B50 step */
	{ 0x3a0a, 0x01 }, /* B60 step */
	{ 0x3a0b, 0x45 }, /* B60 step */
	{ 0x3a0e, 0x02 }, /* 50Hz max band */
	{ 0x3a0d, 0x03 }, /* 60Hz max band */
#endif
	{ 0x3a14, 0x17 }, /* 50Hz max exposure, night mode 5fps */
	{ 0x3a15, 0x10 }, /* 50Hz max exposure */
	{ 0x4004, 0x02 }, /* BLC 2 lines */
	{ 0x3002, 0x1c }, /* reset JFIFO, SFIFO, JPEG */
	{ 0x3006, 0xc3 }, /* disable clock of JPEG2x, JPEG */
	{ 0x4713, 0x03 }, /* JPEG mode 3 */
	{ 0x4407, 0x04 }, /* Quantization scale */
	{ 0x460b, 0x35 },
	{ 0x460c, 0x22 },
	{ 0x4837, 0x22 }, /* DVP CLK divider */
	{ 0x3824, 0x02 }, /* DVP CLK divider, default is 0x2 */
	/* SDE on, scale on, UV average off, color matrix on, AWB on */
	{ 0x5001, 0xa3 },
	{ 0x3503, 0x00 }, /* AEC/AGC on */
};

static const sensor_reg_t ov5640_yuv_vga_15fps[] = {
#if !(defined CONFIG_ARCH_LOMBO_N7V0_FPGA) && \
	!(defined CONFIG_ARCH_LOMBO_N7V1_FPGA)
	{ 0x3008, 0x82 },
	{ 0x3103, 0x03 },
	{ 0x3017, 0xff },
	{ 0x3018, 0xff },
	{ 0x3108, 0x01 },
	{ 0x3037, 0x13 },
	{ 0x3630, 0x2e },
	{ 0x3632, 0xe2 },
	{ 0x3633, 0x23 },
	{ 0x3634, 0x44 },
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
	{ 0x3600, 0x08 },
	{ 0x3601, 0x33 },
	{ 0x471c, 0x50 },
	{ 0x3820, 0x41 },
	{ 0x3821, 0x07 },
	{ 0x3814, 0x31 },
	{ 0x3815, 0x31 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x04 },
	{ 0x3804, 0x0a },
	{ 0x3805, 0x3f },
	{ 0x3806, 0x07 },
	{ 0x3807, 0x9b },

	/* set output size */
	{ 0x3808, 0x02 }, /* DVPHO */
	{ 0x3809, 0x80 }, /* DVPHO */
	{ 0x380a, 0x01 }, /* DVPVO */
	{ 0x380b, 0xe0 }, /* DVPVO */

	{ 0x380c, 0x07 },
	{ 0x380d, 0x68 },
	{ 0x380e, 0x03 },
	{ 0x380f, 0xd8 },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x10 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x06 },
	{ 0x3618, 0x00 },
	{ 0x3612, 0x49 },
	{ 0x3708, 0x62 },
	{ 0x3709, 0x52 },
	{ 0x370c, 0x03 },
	{ 0x3a02, 0x03 },
	{ 0x3a03, 0xd8 },
	{ 0x3a08, 0x01 },
	{ 0x3a09, 0x27 },
	{ 0x3a0a, 0x00 },
	{ 0x3a0b, 0xf6 },
	{ 0x3a0e, 0x03 },
	{ 0x3a0d, 0x04 },
	{ 0x3a14, 0x03 },
	{ 0x3a15, 0xd8 },
	{ 0x4001, 0x02 },
	{ 0x4004, 0x02 },
	{ 0x3002, 0x1c },
	{ 0x3006, 0xc3 },
	{ 0x4300, 0x30 },
	{ 0x501f, 0x00 },
	{ 0x4713, 0x03 },
	{ 0x3035, 0x11 },
	{ 0x3036, 0x3c }, /* default 0x46 */
	{ 0x4407, 0x04 },
	{ 0x460b, 0x35 },
	{ 0x460c, 0x22 },
	{ 0x3824, 0x02 },
	{ 0x5000, 0xa7 },
	{ 0x5001, 0xa3 },
	{ 0x5000, 0xa7 },
	{ 0x3622, 0x01 },
	{ 0x3635, 0x1c },
	{ 0x3634, 0x40 },
	{ 0x3c01, 0x34 },
	{ 0x3c00, 0x00 },
	{ 0x3c04, 0x28 },
	{ 0x3c05, 0x98 },
	{ 0x3c06, 0x00 },
	{ 0x3c07, 0x08 },
	{ 0x3c08, 0x00 },
	{ 0x3c09, 0x1c },
	{ 0x300c, 0x22 },
	{ 0x3c0a, 0x9c },
	{ 0x3c0b, 0x40 },
	{ 0x5180, 0xff },
	{ 0x5181, 0xf2 },
	{ 0x5182, 0x00 },
	{ 0x5183, 0x94 },
	{ 0x5184, 0x25 },
	{ 0x5185, 0x24 },
	{ 0x5186, 0x06 },
	{ 0x5187, 0x08 },
	{ 0x5188, 0x08 },
	{ 0x5189, 0x78 },
	{ 0x518a, 0x54 },
	{ 0x518b, 0xb2 },
	{ 0x518c, 0xb2 },
	{ 0x518d, 0x44 },
	{ 0x518e, 0x3d },
	{ 0x518f, 0x58 },
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
	{ 0x5381, 0x1c },
	{ 0x5382, 0x5a },
	{ 0x5383, 0x06 },
	{ 0x5384, 0x20 },
	{ 0x5385, 0x80 },
	{ 0x5386, 0xa0 },
	{ 0x5387, 0xa2 },
	{ 0x5388, 0xa0 },
	{ 0x5389, 0x02 },
	{ 0x538a, 0x01 },
	{ 0x538b, 0x98 },
	{ 0x5300, 0x08 },
	{ 0x5301, 0x30 },
	{ 0x5302, 0x10 },
	{ 0x5303, 0x00 },
	{ 0x5304, 0x08 },
	{ 0x5305, 0x30 },
	{ 0x5306, 0x08 },
	{ 0x5307, 0x16 },
	{ 0x5309, 0x08 },
	{ 0x530a, 0x30 },
	{ 0x530b, 0x04 },
	{ 0x530c, 0x06 },
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
	{ 0x5580, 0x02 },
	{ 0x5583, 0x40 },
	{ 0x5584, 0x10 },
	{ 0x5589, 0x10 },
	{ 0x558a, 0x00 },
	{ 0x558b, 0xf8 },
	{ 0x5800, 0x23 },
	{ 0x5801, 0x15 },
	{ 0x5802, 0x10 },
	{ 0x5803, 0x10 },
	{ 0x5804, 0x15 },
	{ 0x5805, 0x23 },
	{ 0x5806, 0x0c },
	{ 0x5807, 0x08 },
	{ 0x5808, 0x05 },
	{ 0x5809, 0x05 },
	{ 0x580a, 0x08 },
	{ 0x580b, 0x0c },
	{ 0x580c, 0x07 },
	{ 0x580d, 0x03 },
	{ 0x580e, 0x00 },
	{ 0x580f, 0x00 },
	{ 0x5810, 0x03 },
	{ 0x5811, 0x07 },
	{ 0x5812, 0x07 },
	{ 0x5813, 0x03 },
	{ 0x5814, 0x00 },
	{ 0x5815, 0x00 },
	{ 0x5816, 0x03 },
	{ 0x5817, 0x07 },
	{ 0x5818, 0x0b },
	{ 0x5819, 0x08 },
	{ 0x581a, 0x05 },
	{ 0x581b, 0x05 },
	{ 0x581c, 0x07 },
	{ 0x581d, 0x0b },
	{ 0x581e, 0x2a },
	{ 0x581f, 0x16 },
	{ 0x5820, 0x11 },
	{ 0x5821, 0x11 },
	{ 0x5822, 0x15 },
	{ 0x5823, 0x29 },
	{ 0x5824, 0xbf },
	{ 0x5825, 0xaf },
	{ 0x5826, 0x9f },
	{ 0x5827, 0xaf },
	{ 0x5828, 0xdf },
	{ 0x5829, 0x6f },
	{ 0x582a, 0x8e },
	{ 0x582b, 0xab },
	{ 0x582c, 0x9e },
	{ 0x582d, 0x7f },
	{ 0x582e, 0x4f },
	{ 0x582f, 0x89 },
	{ 0x5830, 0x86 },
	{ 0x5831, 0x98 },
	{ 0x5832, 0x6f },
	{ 0x5833, 0x4f },
	{ 0x5834, 0x6e },
	{ 0x5835, 0x7b },
	{ 0x5836, 0x7e },
	{ 0x5837, 0x6f },
	{ 0x5838, 0xde },
	{ 0x5839, 0xbf },
	{ 0x583a, 0x9f },
	{ 0x583b, 0xbf },
	{ 0x583c, 0xec },
	{ 0x5025, 0x00 },
	{ 0x3a0f, 0x30 },
	{ 0x3a10, 0x28 },
	{ 0x3a1b, 0x30 },
	{ 0x3a1e, 0x26 },
	{ 0x3a11, 0x60 },
	{ 0x3a1f, 0x14 },
	{ 0x3a18, 0x00 },
	{ 0x3a19, 0xf8 },
	{ 0x3035, 0x41 },

#ifdef COLOR_BAR
	{ 0x503d, 0x80 },
#endif
#endif
};

/*
 * 1280x720, 30fps
 * input clock 24Mhz, PCLK 42Mhz
 */
static const sensor_reg_t ov5640_yuv_720p[] = {
	{ 0x3035, 0x21 }, /* PLL */
	{ 0x3036, 0x54 }, /* PLL */
	{ 0x3c07, 0x07 }, /* lightmeter 1 threshold[7:0] */
	{ 0x3820, 0x41 }, /* flip */
	{ 0x3821, 0x07 }, /* mirror */
	{ 0x3814, 0x31 }, /* timing X inc */
	{ 0x3815, 0x31 }, /* timing Y inc */
	{ 0x3800, 0x00 }, /* HS */
	{ 0x3801, 0x00 }, /* HS */
	{ 0x3802, 0x00 }, /* VS */
	{ 0x3803, 0xfa }, /* VS */
	{ 0x3804, 0x0a }, /* HW (HE) */
	{ 0x3805, 0x3f }, /* HW (HE) */
	{ 0x3806, 0x06 }, /* VH (VE) */
	{ 0x3807, 0xa9 }, /* VH (VE) */
	{ 0x3808, 0x05 }, /* DVPHO */
	{ 0x3809, 0x00 }, /* DVPHO */
	{ 0x380a, 0x02 }, /* DVPVO */
	{ 0x380b, 0xd0 }, /* DVPVO */
	{ 0x380c, 0x07 }, /* HTS */
	{ 0x380d, 0x64 }, /* HTS */
	{ 0x380e, 0x02 }, /* VTS */
	{ 0x380f, 0xe4 }, /* VTS */
	{ 0x3813, 0x04 }, /* timing V offset */
	{ 0x3618, 0x00 },
	{ 0x3612, 0x29 },
	{ 0x3709, 0x52 },
	{ 0x370c, 0x03 },
	{ 0x3a02, 0x02 }, /* 60Hz max exposure */
	{ 0x3a03, 0xe0 }, /* 60Hz max exposure */
	 /* banding filters are calculated automatically in camera driver */
	{ 0x3a08, 0x00 }, /* B50 step */
	{ 0x3a09, 0xDD }, /* B50 step */
	{ 0x3a0a, 0x00 }, /* B60 step */
	{ 0x3a0b, 0xB8 }, /* B60 step */
	{ 0x3a0e, 0x03 }, /* 50Hz max band */
	{ 0x3a0d, 0x04 }, /* 60Hz max band */

	{ 0x3a14, 0x02 }, /* 50Hz max exposure */
	{ 0x3a15, 0xe0 }, /* 50Hz max exposure */
	{ 0x4004, 0x02 }, /* BLC line number */
	{ 0x3002, 0x1c }, /* reset JFIFO, SFIFO, JPG */
	{ 0x3006, 0xc3 }, /* disable clock of JPEG2x, JPEG */
	{ 0x4713, 0x03 }, /* JPEG mode 3 */
	{ 0x4407, 0x04 }, /* Quantization scale */
	{ 0x460b, 0x37 },
	{ 0x460c, 0x20 },
	{ 0x4837, 0x16 }, /* MIPI global timing */
	{ 0x3824, 0x02 }, /* PCLK manual divider */
	{ 0x5001, 0x83 }, /* SDE on, CMX on, AWB on */
	{ 0x3503, 0x00 }, /* AEC/ AGC on */
};

static const sensor_reg_t ov5640_yuv_1080p[] = {
	{ 0x3820, 0x40 },
	{ 0x3821, 0x06 },
	/* pll and clock setting */
	{ 0x3034, 0x18 },
#if (defined CONFIG_ARCH_LOMBO_N7V0_FPGA) || \
	(defined CONFIG_ARCH_LOMBO_N7V1_FPGA)
	{ 0x3035, 0x41 }, /* 0x11:30fps 0x21:15fps 0x41:7.5fps */
#else
	{ 0x3035, 0x21 }, /* 0x11:30fps 0x21:15fps 0x41:7.5fps */
#endif
	{ 0x3036, 0x54 },
	{ 0x3037, 0x13 },
	{ 0x3108, 0x01 },
	{ 0x3824, 0x01 },
	/* delay 5ms */
	/* timing */
	/* 1920x1080 */
	{ 0x3808, 0x07 }, /* H size MSB */
	{ 0x3809, 0x80 }, /* H size LSB */
	{ 0x380a, 0x04 }, /* V size MSB */
	{ 0x380b, 0x38 }, /* V size LSB */
	{ 0x380c, 0x09 }, /* HTS MSB */
	{ 0x380d, 0xc4 }, /* HTS LSB */
	{ 0x380e, 0x04 }, /* VTS MSB */
	{ 0x380f, 0x60 }, /* VTS LSB */

	/* banding step */
	{ 0x3a08, 0x01 }, /* 50HZ step MSB */
	{ 0x3a09, 0x50 }, /* 50HZ step LSB */
	{ 0x3a0a, 0x01 }, /* 60HZ step MSB */
	{ 0x3a0b, 0x18 }, /* 60HZ step LSB */
	{ 0x3a0e, 0x03 }, /* 50HZ step max */
	{ 0x3a0d, 0x04 }, /* 60HZ step max */

	{ 0x3503, 0x00 }, /* AEC enable */
	{ 0x350c, 0x00 },
	{ 0x350d, 0x00 },
	{ 0x3c07, 0x07 }, /* light meter 1 thereshold */
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
	{ 0x3810, 0x00 }, /* isp hortizontal offset high byte */
	{ 0x3811, 0x10 }, /* isp hortizontal offset low byte */
	{ 0x3812, 0x00 }, /* isp vertical offset high byte */
	{ 0x3813, 0x04 }, /* isp vertical offset low byte */

	{ 0x4002, 0x45 }, /* BLC related */
	{ 0x4005, 0x18 }, /* BLC related */

	{ 0x3618, 0x04 },
	{ 0x3612, 0x2b },
	{ 0x3709, 0x12 },
	{ 0x370c, 0x00 },
	{ 0x3a02, 0x04 }, /* 60HZ max exposure limit MSB */
	{ 0x3a03, 0x60 }, /* 60HZ max exposure limit LSB */
	{ 0x3a14, 0x04 }, /* 50HZ max exposure limit MSB */
	{ 0x3a15, 0x60 }, /* 50HZ max exposure limit LSB */

	{ 0x4004, 0x06 }, /* BLC line number */
	{ 0x3002, 0x1c }, /* reset JFIFO SFIFO JPG */
	{ 0x3006, 0xc3 }, /* enable xx clock */
	{ 0x460b, 0x37 }, /* debug mode */
	{ 0x460c, 0x20 }, /* PCLK Manuale */
	{ 0x4837, 0x16 }, /* PCLK period */
	{ 0x5001, 0x83 }, /* ISP effect */

	{ 0x302c, 0x42 }, /* bit[7:6]: output drive capability
			   * 00: 1x, 01: 2x, 10: 3x, 11: 4x */
	{ 0x3a18, 0x00 },
	{ 0x3a19, 0x80 },

#ifdef COLOR_BAR
	{ 0x503d, 0x80 }, /* color bar */
#endif
};

/*
 * YUV 2592x1944, 3.75fps
 * input clock 24Mhz, PCLK 42Mhz
 */
static const sensor_reg_t ov5640_yuv_5m[] = {
#if (defined CONFIG_ARCH_LOMBO_N7V0_FPGA) || \
	(defined CONFIG_ARCH_LOMBO_N7V1_FPGA)
	{ 0x3035, 0x41 }, /* PLL */
	{ 0x3036, 0x69 }, /* PLL */
	{ 0x3c07, 0x07 }, /* lightm eter 1 threshold[7:0] */
	{ 0x3820, 0x40 }, /* flip */
	{ 0x3821, 0x06 }, /* mirror */
	{ 0x3814, 0x11 }, /* timing X inc */
	{ 0x3815, 0x11 }, /* timing Y inc */
	{ 0x3800, 0x00 }, /* HS */
	{ 0x3801, 0x00 }, /* HS */
	{ 0x3802, 0x00 }, /* VS */
	{ 0x3803, 0x00 }, /* VS */
	{ 0x3804, 0x0a }, /* HW (HE) */
	{ 0x3805, 0x3f }, /* HW (HE) */
	{ 0x3806, 0x07 }, /* VH (VE) */
	{ 0x3807, 0x9f }, /* VH (VE) */
	{ 0x3808, 0x0a }, /* DVPHO */
	{ 0x3809, 0x20 }, /* DVPHO */
	{ 0x380a, 0x07 }, /* DVPVO */
	{ 0x380b, 0x98 }, /* DVPVO */
	{ 0x380c, 0x0b }, /* HTS */
	{ 0x380d, 0x1c }, /* HTS */
	{ 0x380e, 0x07 }, /* VTS */
	{ 0x380f, 0xb0 }, /* VTS */
	{ 0x3813, 0x04 }, /* timing V offset */
	{ 0x3618, 0x04 },
	{ 0x3612, 0x2b },
	{ 0x3709, 0x12 },
	{ 0x370c, 0x00 },
	/* banding filters are calculated automatically in camera driver */
#if 0
	{ 0x3a02, 0x07 }, /* 60Hz max exposure */
	{ 0x3a03, 0xae }, /* 60Hz max exposure */
	{ 0x3a08, 0x01 }, /* B50 step */
	{ 0x3a09, 0x27 }, /* B50 step */
	{ 0x3a0a, 0x00 }, /* B60 step */
	{ 0x3a0b, 0xf6 }, /* B60 step */
	{ 0x3a0e, 0x06 }, /* 50Hz max band */
	{ 0x3a0d, 0x08 }, /* 60Hz max band */
	{ 0x3a14, 0x07 }, /* 50Hz max exposure */
	{ 0x3a15, 0xae }, /* 50Hz max exposure */
#else
	{ 0x3a02, 0x07 }, /* 60Hz max exposure */
	{ 0x3a03, 0xb0 }, /* 60Hz max exposure */
	{ 0x3a09, 0x27 }, /* B50 low */
	{ 0x3a0a, 0x00 }, /* B60 high */
	{ 0x3a0b, 0xf6 }, /* B60 low */
	{ 0x3a0e, 0x06 }, /* B50 max */
	{ 0x3a0d, 0x08 }, /* B60 max */
	{ 0x3a14, 0x07 }, /* 50Hz max exposure */
	{ 0x3a15, 0xb0 }, /* 50Hz max exposure */
#endif
	{ 0x4004, 0x06 }, /* BLC line number */
	{ 0x3002, 0x1c }, /* reset JFIFO, SFIFO, JPG */
	{ 0x3006, 0xc3 }, /* disable clock of JPEG2x, JPEG */
	{ 0x4713, 0x02 }, /* JPEG mode 3 */
	{ 0x4407, 0x0c }, /* Quantization sacle */
	{ 0x460b, 0x37 },
	{ 0x460c, 0x20 },
	{ 0x4837, 0x2c }, /* MIPI global timing */
	{ 0x3824, 0x01 }, /* PCLK manual divider */
	{ 0x5001, 0x83 }, /* SDE on, CMX on, AWB on */
	{ 0x3503, 0x00 }, /* defualt is 0x3 */

	{ 0x302c, 0x42 }, /* bit[7:6]: output drive capability
			   * 00: 1x, 01: 2x, 10: 3x, 11: 4x */
#else
	/* capture 5Mega 7.5fps */
	/* pll and clock setting */
	{ 0x3820, 0x40 },
	{ 0x3821, 0x06 },
	{ 0x3034, 0x18 },
	{ 0x3035, 0x41 },
	{ 0x3036, 0x54 },
	{ 0x3037, 0x13 },
	{ 0x3108, 0x01 },
	{ 0x3824, 0x01 },
	/* timing */
	/* 2592*1936 */
	{ 0x3808, 0x0a }, /* H size MSB */
	{ 0x3809, 0x20 }, /* H size LSB */
	{ 0x380a, 0x07 }, /* V size MSB */
	{ 0x380b, 0x90 }, /* V size LSB */
	{ 0x380c, 0x0b }, /* HTS MSB */
	{ 0x380d, 0x1c }, /* HTS LSB */
	{ 0x380e, 0x07 }, /* VTS MSB */
	{ 0x380f, 0xb0 }, /* LSB */

	/* banding step */
	{ 0x3a08, 0x00 }, /* 50HZ step MSB */
	{ 0x3a09, 0x93 }, /* 50HZ step LSB */
	{ 0x3a0a, 0x00 }, /* 60HZ step MSB */
	{ 0x3a0b, 0x7b }, /* 60HZ step LSB */
	{ 0x3a0e, 0x0d }, /* 50HZ step max */
	{ 0x3a0d, 0x10 }, /* 60HZ step max */

	{ 0x3503, 0x07 }, /* AEC disable */
	{ 0x350c, 0x00 },
	{ 0x350d, 0x00 },
	{ 0x3c07, 0x07 }, /* light meter 1 thereshold */

	{ 0x3814, 0x11 }, /* horizton subsample */
	{ 0x3815, 0x11 }, /* vertical subsample */
	{ 0x3800, 0x00 }, /* x address start high byte */
	{ 0x3801, 0x00 }, /* x address start low byte */
	{ 0x3802, 0x00 }, /* y address start high byte */
	{ 0x3803, 0x00 }, /* y address start low byte */
	{ 0x3804, 0x0a }, /* x address end high byte */
	{ 0x3805, 0x3f }, /* x address end low byte */
	{ 0x3806, 0x07 }, /* y address end high byte */
	{ 0x3807, 0x9f }, /* y address end low byte */
	{ 0x3810, 0x00 }, /* isp hortizontal offset high byte */
	{ 0x3811, 0x10 }, /* isp hortizontal offset low byte */
	{ 0x3812, 0x00 }, /* isp vertical offset high byte */
	{ 0x3813, 0x04 }, /* isp vertical offset low byte */

	{ 0x4002, 0xc5 }, /* BLC related */
	{ 0x4005, 0x1a }, /* BLC related */

	{ 0x3618, 0x04 },
	{ 0x3612, 0x2b },
	{ 0x3709, 0x12 },
	{ 0x370c, 0x00 },
	{ 0x3a02, 0x07 }, /* 60HZ max exposure limit MSB */
	{ 0x3a03, 0xb0 }, /* 60HZ max exposure limit LSB */
	{ 0x3a14, 0x07 }, /* 50HZ max exposure limit MSB */
	{ 0x3a15, 0xb0 }, /* 50HZ max exposure limit LSB */
	{ 0x4004, 0x06 }, /* BLC line number */
	{ 0x4837, 0x2c }, /* PCLK period */
	{ 0x5001, 0xa3 }, /* ISP effect */

	{ 0x302c, 0x42 }, /* bit[7:6]: output drive capability
			   * 00: 1x, 01: 2x, 10: 3x, 11: 4x */
#endif
};

#if 0
struct ov5640_framesize {
	u16 width;
	u16 height;
	u16 max_exp_lines;
	const u8 *regs;
};

static const struct ov5640_framesize ov5640_framesizes[] = {
	{
		.width = VGA_WIDTH,
		.height = VGA_HEIGHT,
		.max_exp_lines = 498,
	},
};
#endif

#if 0
struct ov5640_pixfmt {
	u32 code;
	u32 colorspace;
	/* REG_TSLB value, only bits [3:2] may be set. */
	u8 tslb_reg;
};

static const struct ov5640_pixfmt ov5640_formats[] = {
	{ V4L2_MBUS_FMT_YUYV8_2X8, V4L2_COLORSPACE_JPEG, 0x00 },
	{ V4L2_MBUS_FMT_YVYU8_2X8, V4L2_COLORSPACE_JPEG, 0x04 },
	{ V4L2_MBUS_FMT_UYVY8_2X8, V4L2_COLORSPACE_JPEG, 0x0c },
	{ V4L2_MBUS_FMT_VYUY8_2X8, V4L2_COLORSPACE_JPEG, 0x08 },
};
#endif

/* the supported format of this sensor is MEDIA_BUS_FMT_YUYV8_2X8 */
static const struct v4l2_mbus_framefmt ov5640_formats[] = {
	{
		 .code = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

struct ov5640_framesize {
	int	width;
	int	height;
	int	vts;
	int	framerate;
	const sensor_reg_t *regs; /* Regs to tweak */
};

static struct ov5640_framesize ov5640_framesizes[] = {
	/* VGA 30fps */
	{
		.width		= OV5640_VGA_WIDTH,
		.height		= OV5640_VGA_HEIGHT,
		.vts		= 0x3d8,
		.framerate	= 30,
		.regs		= ov5640_yuv_vga_30fps,
	},
	/* VGA 15fps */
	{
		.width		= OV5640_VGA_WIDTH,
		.height		= OV5640_VGA_HEIGHT,
		.vts		= 0x3d8,
		.framerate	= 15,
		.regs		= ov5640_yuv_vga_15fps,
	},
	/* 1080P */
	{
		.width		= OV5640_1080P_WIDTH,
		.height		= OV5640_1080P_HEIGHT,
		.vts		= 0x0460,
		.framerate	= 30,
		.regs		= ov5640_yuv_1080p,
	},
	/* 2560*1920 */
	{
		.width		= OV5640_MAX_WIDTH,
		.height		= OV5640_MAX_HEIGHT,
		.vts		= 0x07b6,
		.framerate	= 10,
		.regs		= ov5640_yuv_5m,
	},
};

static inline void sensor_msleep(unsigned int msecs)
{
	msleep(msecs);
}

static int reg_read(struct i2c_client *client, u16 reg, u8 *val)
{
	int ret;
	/* We have 16-bit i2c addresses - care for endianess */
	unsigned char data[2] = { reg >> 8, reg & 0xff };

	ret = i2c_master_send(client, data, 2);
	if (ret < 2) {
		PRT_INFO("%s: i2c read error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}

	ret = i2c_master_recv(client, val, 1);
	if (ret < 1) {
		PRT_INFO("%s: i2c read error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}
	return 0;
}

static int reg_write(struct i2c_client *client, u16 reg, u8 val)
{
	int ret;
	unsigned char data[3] = { reg >> 8, reg & 0xff, val };

	ret = i2c_master_send(client, data, 3);
	if (ret < 3) {
		PRT_INFO("%s: i2c write error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}

	return 0;
}

static void ov5640_printk_reg_value(u16 reg_addr)
{
	u8 value;
	int ret = 0;

	ret = reg_read(ov5640_client, reg_addr, &value);
	PRT_INFO("Register 0x%08x value is 0x%08x\n", reg_addr, value);

	return;
}

static int ov5640_check_present(void)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = reg_read(ov5640_client, 0x300a, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = reg_read(ov5640_client, 0x300b, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}

	id = (tmp[0] << 8) | tmp[1];

	if (id != 0x5640) {
		PRT_ERR("ID wrong! (0x%x)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int ov5640_cmos_init(void)
{
	u8 value = 0;
	int i;
	int ret = 0;

	ret = ov5640_check_present();
	if (ret != 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(ov5640_reg_list); i++) {
		ret = reg_write(ov5640_client,
				ov5640_reg_list[i].reg_add,
				ov5640_reg_list[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
		if (ov5640_reg_list[i].reg_add == 0x3008)
			sensor_msleep(5);
	}

	PRT_INFO("write reg list finish.\n");
#ifdef OV5640_IMAGE_1080P
	for (i = 0; i < ARRAY_SIZE(ov5640_yuv_1080p); i++) {
		ret = reg_write(ov5640_client,
				ov5640_yuv_1080p[i].reg_add,
				ov5640_yuv_1080p[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}
#elif defined OV5640_IMAGE_720P
	for (i = 0; i < ARRAY_SIZE(ov5640_yuv_720p); i++) {
		ret = reg_write(ov5640_client,
				ov5640_yuv_720p[i].reg_add,
				ov5640_yuv_720p[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}
#else
	for (i = 0; i < ARRAY_SIZE(ov5640_yuv_vga_30fps); i++) {
		ret = reg_write(ov5640_client,
				ov5640_yuv_vga_30fps[i].reg_add,
				ov5640_yuv_vga_30fps[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}
#endif

#if 1
	for (i = 0; i < ARRAY_SIZE(ov5640_reg_list); i++) {
		ret = reg_read(ov5640_client,
			ov5640_reg_list[i].reg_add, &value);
		if (ov5640_reg_list[i].reg_value != value) {
			PRT_ERR("0x%x: 0x%x != 0x%x\n",
				ov5640_reg_list[i].reg_add, value,
				ov5640_reg_list[i].reg_value);
		}
	}
#ifdef OV5640_IMAGE_1080P
	for (i = 0; i < ARRAY_SIZE(ov5640_yuv_1080p); i++) {
		ret = reg_read(ov5640_client,
			ov5640_yuv_1080p[i].reg_add, &value);
		if (ov5640_yuv_1080p[i].reg_value != value) {
			PRT_ERR("0x%x: 0x%x != 0x%x",
				ov5640_yuv_1080p[i].reg_add, value,
				ov5640_yuv_1080p[i].reg_value);
		}
	}
#elif defined OV5640_IMAGE_720P
	for (i = 0; i < ARRAY_SIZE(ov5640_yuv_720p); i++) {
		ret = reg_read(ov5640_client,
			ov5640_yuv_720p[i].reg_add, &value);
		if (ov5640_yuv_720p[i].reg_value != value) {
			PRT_ERR("0x%x: 0x%x != 0x%x",
				ov5640_yuv_720p[i].reg_add, value,
				ov5640_yuv_720p[i].reg_value);
		}
	}
#else
	for (i = 0; i < ARRAY_SIZE(ov5640_yuv_vga_30fps); i++) {
		ret = reg_read(ov5640_client,
			ov5640_yuv_vga_30fps[i].reg_add, &value);
		if (ov5640_yuv_vga_30fps[i].reg_value != value) {
			PRT_ERR("0x%x: 0x%x != 0x%x",
				ov5640_yuv_vga_30fps[i].reg_add, value,
				ov5640_yuv_vga_30fps[i].reg_value);
		}
	}
#endif

#if 1
	PRT_INFO("OV5640 PLL relevant reg\n");
	ov5640_printk_reg_value(0x3034);
	ov5640_printk_reg_value(0x3035);
	ov5640_printk_reg_value(0x3036);
	ov5640_printk_reg_value(0x3037);
	ov5640_printk_reg_value(0x3108);
	ov5640_printk_reg_value(0x3824);
	ov5640_printk_reg_value(0x460C);
	ov5640_printk_reg_value(0x300E);
	ov5640_printk_reg_value(0x302C);
	PRT_INFO("OV5640 setting done\n");
#endif

#endif
	ret = reg_write(ov5640_client, 0x3008, 0x02);
	if (ret != 0) {
		PRT_ERR("failed\n");
		return ret;
	}

	return 0;

}

static inline ov5640_config_t *to_ov5640(struct v4l2_subdev *sd)
{
	return container_of(sd, ov5640_config_t, sd);
}

static const struct v4l2_mbus_framefmt *find_sensor_format(
					struct v4l2_mbus_framefmt *mf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ov5640_formats); i++)
		if (mf->code == ov5640_formats[i].code)
			return &ov5640_formats[i];

	return &ov5640_formats[0];
}

static void ov5640_try_format(struct v4l2_mbus_framefmt *mf)
{
	const struct v4l2_mbus_framefmt *fmt;

	fmt = find_sensor_format(mf);
	PRT_INFO("fmt->code:0x%x\n", fmt->code);
	PRT_INFO("mf->code:0x%x\n", mf->code);
	mf->code = fmt->code;
	mf->field = V4L2_FIELD_NONE;

	/* try pipeline format时会使用 */
#ifdef OV5640_IMAGE_1080P
	mf->width = OV5640_1080P_WIDTH;
	mf->height = OV5640_1080P_HEIGHT;
#elif defined OV5640_IMAGE_720P
	mf->width = OV5640_720P_WIDTH;
	mf->height = OV5640_720P_HEIGHT;
#else
	mf->width = OV5640_VGA_WIDTH;
	mf->height = OV5640_VGA_HEIGHT;
#endif

#if 0
	v4l_bound_align_image(&mf->width, OV5640_MIN_WIDTH,
		OV5640_MAX_WIDTH, 0,
		&mf->height, OV5640_MIN_HEIGHT,
		OV5640_MAX_HEIGHT, 0, 0);
#endif

}

static int ov5640_set_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	ov5640_config_t *ov5640 = to_ov5640(sd);
	struct v4l2_mbus_framefmt *mf = NULL;

	PRT_DBG("\n");

	ov5640_try_format(&fmt->format);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
	}

	if (mf) {
		mutex_lock(&ov5640->lock);
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
			PRT_DBG("\n");
			*mf = fmt->format;
		}
		mutex_unlock(&ov5640->lock);
	}

	return 0;
}

static int ov5640_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	ov5640_config_t *ov5640 = to_ov5640(sd);
	struct v4l2_mbus_framefmt *mf;

	PRT_DBG("\n");

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, 0);
		fmt->format = *mf;
		return 0;
	}

	/* V4L2_SUBDEV_FORMAT_ACTIVE */
	mutex_lock(&ov5640->lock);
	fmt->format = ov5640->format;
	mutex_unlock(&ov5640->lock);

	return 0;
}

/*
 * V4L2 subdev video and pad level operations
 */
static void ov5640_get_default_format(struct v4l2_mbus_framefmt *mf)
{
	mf->width = ov5640_framesizes[0].width;
	mf->height = ov5640_framesizes[0].height;
	mf->colorspace = ov5640_formats[0].colorspace;
	mf->code = ov5640_formats[0].code;
	mf->field = V4L2_FIELD_NONE;
}

/*
 * V4L2 subdev internal operations
 */

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int ov5642_get_register(struct v4l2_subdev *sd,
				struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;
	u8 val;

	if (reg->reg & ~0xffff)
		return -EINVAL;

	reg->size = 1;

	ret = reg_read(client, reg->reg, &val);
	if (!ret)
		reg->val = (__u64)val;

	return ret;
}

static int ov5642_set_register(struct v4l2_subdev *sd,
					const struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	if (reg->reg & ~0xffff || reg->val & ~0xff)
		return -EINVAL;

	return reg_write(client, reg->reg, reg->val);
}
#endif

static int ov5640_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
#if 0
	struct v4l2_mbus_framefmt *format = v4l2_subdev_get_try_format(fh, 0);
	struct v4l2_rect *crop = v4l2_subdev_get_try_crop(fh, 0);
#endif

	return 0;
}

static int ov5640_registered(struct v4l2_subdev *sd)
{
	ov5640_config_t *ov5640 = to_ov5640(sd);
	struct i2c_client *client = ov5640->client;
	int ret = 0;

	PRT_INFO("\n");

	mutex_lock(&ov5640->lock);

	if ((ov5640->reset_gpio != 0) && (ov5640->pwdn_gpio != 0)) {
		ret = devm_gpio_request(&client->dev, ov5640->reset_gpio,
					"reset_gpio");
		if (ret < 0) {
			PRT_ERR("reset_gpio %d request failed (reset_gpio)\n",
				ov5640->reset_gpio);
			ov5640->reset_gpio = 0;
			ov5640->pwdn_gpio = 0;
			goto unlock;
		}

		ret = devm_gpio_request(&client->dev, ov5640->pwdn_gpio,
					"pwdn_gpio");
		if (ret < 0) {
			PRT_ERR("pwdn_gpio %d request failed (pwdn_gpio)\n",
				ov5640->pwdn_gpio);
			devm_gpio_free(&client->dev, ov5640->reset_gpio);
			ov5640->reset_gpio = 0;
			ov5640->pwdn_gpio = 0;
			goto unlock;
		}
	}

	ov5640_reset_cycle(ov5640);
	ret = ov5640_check_present();
	if (ret != 0) {
		PRT_ERR("sensor ov5640 not present!\n");
		if ((ov5640->reset_gpio != 0) && (ov5640->pwdn_gpio != 0)) {
			devm_gpio_free(&client->dev, ov5640->pwdn_gpio);
			devm_gpio_free(&client->dev, ov5640->reset_gpio);
		}
	} else
		PRT_INFO("sensor ov5640 present\n");

unlock:
	mutex_unlock(&ov5640->lock);

	return ret;
}

static const struct v4l2_subdev_internal_ops ov5640_subdev_internal_ops = {
	.registered = ov5640_registered,
	.open = ov5640_open,
};

static void ov5640_reset_cycle(ov5640_config_t *ov5640)
{
	PRT_INFO("\n");

	if ((ov5640->reset_gpio == 0) || (ov5640->pwdn_gpio == 0))
		return;

	/* reset low */
	gpio_direction_output(ov5640->reset_gpio, 0);
	sensor_msleep(5);
	/* pwdn high */
	gpio_direction_output(ov5640->pwdn_gpio, 1);
	sensor_msleep(10);
	/* pwdn low */
	gpio_direction_output(ov5640->pwdn_gpio, 0);
	sensor_msleep(5);
	/* reset high */
	gpio_direction_output(ov5640->reset_gpio, 1);
	sensor_msleep(10);

}

static int ov5640_set_power(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

	if (on) {
		ov5640_config_t *ov5640 = to_ov5640(sd);

		ov5640_reset_cycle(ov5640);
		ret = ov5640_cmos_init();
	}

	PRT_INFO("on:%d\n", on);
	return ret;
}

static int ov5640_s_stream(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

#if 0
	ov5640_config_t *ov5640 = to_ov5640(sd);

	ov5640_reset_cycle(ov5640);
	ret = ov5640_cmos_init();
#endif

	PRT_INFO("*************************on:%d\n", on);
	return ret;
}

static const struct v4l2_subdev_pad_ops ov5640_pad_ops = {
#if 0
	.enum_mbus_code = ov5640_enum_mbus_code,
	.enum_frame_size = ov5640_enum_frame_sizes,
#endif
	.get_fmt = ov5640_get_fmt,
	.set_fmt = ov5640_set_fmt,
};

/*
 * V4L2 subdev internal operations
 */
static const struct v4l2_subdev_core_ops ov5640_subdev_core_ops = {
	.s_power = ov5640_set_power,
	.log_status = v4l2_ctrl_subdev_log_status,
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops ov5640_video_ops = {
	.s_stream = ov5640_s_stream,
};

#ifdef CONFIG_VIDEO_V4L2_SUBDEV_API

static const struct v4l2_subdev_ops ov5640_subdev_ops = {
	.core = &ov5640_subdev_core_ops,
	.pad = &ov5640_pad_ops,
	.video = &ov5640_video_ops,
};
#endif

/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_ops ov5640_ops;

#define DRIVER_NAME "ov5640"

static int ov5640_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int ret = 0;
	enum of_gpio_flags flags;
	struct i2c_adapter *adapter = client->adapter;
	struct device_node *np = client->dev.of_node;
	struct v4l2_subdev *sd;
	ov5640_config_t *ov5640;

	PRT_INFO("*********probe begin\n");

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	ov5640 = devm_kzalloc(&client->dev, sizeof(*ov5640), GFP_KERNEL);
	if (ov5640 == NULL)
		return -ENOMEM;

	ov5640->reset_gpio = of_get_named_gpio_flags(np, "reset_gpio",
							0, &flags);

	ov5640->pwdn_gpio = of_get_named_gpio_flags(np, "pwdn_gpio",
						0, &flags);

	if (!gpio_is_valid(ov5640->reset_gpio) ||
		!gpio_is_valid(ov5640->pwdn_gpio)) {
		PRT_ERR("get gpio %d(reset) and gpio %d(pwdn) failed\n",
				ov5640->reset_gpio, ov5640->pwdn_gpio);
		ov5640->reset_gpio = 0;
		ov5640->pwdn_gpio = 0;
	}

	mutex_init(&ov5640->lock);

	ov5640->client = client;
	sd = &ov5640->sd;

	v4l2_i2c_subdev_init(sd, client, &ov5640_subdev_ops);
	strlcpy(sd->name, DRIVER_NAME, sizeof(sd->name));

	sd->internal_ops = &ov5640_subdev_internal_ops;

	client->flags = I2C_CLIENT_SCCB;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

#if defined(CONFIG_MEDIA_CONTROLLER)
	ov5640->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &ov5640->pad, 0);
	if (ret < 0)
		return ret;
#endif
	ov5640_get_default_format(&ov5640->format);

	ov5640_client = client;

	PRT_INFO("probe end\n");
	return 0;
}

static int ov5640_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l_info(client, "%s %d -----------------\n",
		__func__, __LINE__);

	v4l2_device_unregister_subdev(sd);

#if defined(CONFIG_MEDIA_CONTROLLER)
	media_entity_cleanup(&sd->entity);
#endif

	return 0;
}

/* sensor devices id table */
static const struct i2c_device_id ov5640_id[] = {
	{ "ov5640", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, ov5640_id);

static struct i2c_driver ov5640_driver = {
	.driver = {
		.name = "ov5640",
	},
	.probe = ov5640_probe,
	.remove = ov5640_remove,
	.id_table = ov5640_id,
};

module_i2c_driver(ov5640_driver);


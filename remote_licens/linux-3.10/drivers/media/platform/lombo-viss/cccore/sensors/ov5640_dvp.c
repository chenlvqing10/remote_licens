/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ov5640 dvp sensor driver code for LomboTech
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

#define OV5640_5M_WIDTH			2592
/* #define OV5640_5M_HEIGHT		1944 */
#define OV5640_5M_HEIGHT		1936

/*
 * 2lane YUV init
 */
static const cc_reg_t init_reg_list[] = {
	{ 0x3103, 0x11 }, /* system clock from pad, bit[1] */
	{ 0x3008, 0x82 }, /* software reset, bit[7] */
	{ CC_REG_DELAY, 5 },
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
	{ 0x3008, 0x02 }, /* wake up */
};

/*
 * VGA 30fps
 * YUV VGA 30fps, night mode 5fps
 * Input Clock = 24Mhz, PCLK = 56MHz
 */
static const cc_reg_t ov5640_yuv_vga[] = {
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

static const cc_reg_t ov5640_yuv_vga_15fps[] = {
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
static const cc_reg_t ov5640_yuv_720p[] = {
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
	{ 0x302c, 0x42 }, /* bit[7:6]: output drive capability
		   * 00: 1x, 01: 2x, 10: 3x, 11: 4x */
	{ 0x5001, 0x83 }, /* SDE on, CMX on, AWB on */
	{ 0x3503, 0x00 }, /* AEC/ AGC on */
};

static const cc_reg_t ov5640_yuv_1080p[] = {
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
	{ CC_REG_DELAY, 5 },
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
static const cc_reg_t ov5640_yuv_5m[] = {
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
	{ 0x3503, 0x00 }, /* default is 0x3 */

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
	{
		.width = OV5640_5M_WIDTH,
		.height = OV5640_5M_HEIGHT,
		.regs = ov5640_yuv_5m,
		.regs_cnt = ARRAY_SIZE(ov5640_yuv_5m),
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = OV5640_VGA_WIDTH,
		.height = OV5640_VGA_HEIGHT,

#if (defined CONFIG_ARCH_LOMBO_N7V0_FPGA) || \
	(defined CONFIG_ARCH_LOMBO_N7V1_FPGA)
		.framerate = { 1, 15 },	/* 15 fps */
#else
		.framerate = { 1, 60 },	/* 60 fps */
#endif
	},
	{
		.width = OV5640_720P_WIDTH,
		.height = OV5640_720P_HEIGHT,

		.framerate = { 1, 30 },	/* 30 fps */
	},
	{
		.width = OV5640_1080P_WIDTH,
		.height = OV5640_1080P_HEIGHT,

		.framerate = { 1, 15 },	/* 15 fps */
	},
	{
		.width = OV5640_5M_WIDTH,
		.height = OV5640_5M_HEIGHT,

		.framerate = { 100, 375 },	/* 3.75 fps */
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
		ret = cc_si->write_reg(cc_si, 0x3019, 0x00);
		if (ret) {
			PRT_ERR("%d\n", on);
			return ret;
		}

		ret = cc_si->write_reg(cc_si, 0x4800, 0x24);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->write_reg(cc_si, 0x3019, 0x70);
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


#define DRIVER_NAME "ov5640_dvp"

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

static const struct of_device_id ov5640_dvp_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov5640_dvp_of_match);

static struct platform_driver ov5640_dvp_driver = {
	.driver = {
		.of_match_table	= ov5640_dvp_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov5640_dvp_driver);

MODULE_DESCRIPTION("OV5640-DVP sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);

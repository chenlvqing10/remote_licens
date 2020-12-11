/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * pr2000k ahd chip driver code for LomboTech
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

/*
 * 4lane init
 */
static const cc_reg_t init_reg_list[] = {
	{0x3013, 0x01},
	/* fps 60 1050Mbps clk 525MHz 4lane */
	/* fps 30 unwork */
	{0x0103, 0x01}, /* software_reset */
	{0x0100, 0x00}, /* software_standby */
	{0x0300, 0x05}, /* PLL settings *//* pll1_pre_div = /4 */
	{0x0302, 0x5A}, /* pll1_multiplier = 175 *//* 1080P60FPS 1050Mbps */

	/* i2c_write_byte(0x0302, 0x78); */
	/* pll1_multiplier = 120 */ /* 1080P30FPS 720Mbps */

	{0x0303, 0x00}, /* pll1_divm = /(1 + 0) */
	{0x0304, 0x03}, /* pll1_div_mipi = /8 */
	{0x030B, 0x02}, /* pll2_pre_div = /2 */
	{0x030D, 0x4E}, /* pll2_r_divp = 78 */
	{0x030E, 0x00}, /* pll2_r_divs = /1 */
	{0x030F, 0x04}, /* pll2_r_divsp = /(1 + 4) */
	/* pll2_pre_div0 = /1, pll2_r_divdac = /(1 + 1) */
	{0x0312, 0x01},
	{0x031E, 0x0C}, /* pll1_no_lat = 1, mipi_bitsel_man = 0 */

	/* PAD OEN2, VSYNC out enable=0x80, disable=0x00 */
	{0x3002, 0x80},
	/* PAD OUT2, VSYNC pulse direction low-to-high = 1 */
	{0x3007, 0x01},
	/* PAD SEL2, VSYNC out value = 0 */
	{0x300D, 0x00},
	/* PAD OUT2, VSYNC out select = 0 */
	{0x3010, 0x00},

	/* Npump clock div = /2, Ppump clock div = /4 */
	{0x3015, 0x0},
	/*
	 * mipi_lane_mode = 1+3, mipi_lvds_sel = 1 = MIPI enable,
	 * r_phy_pd_mipi_man = 0, lane_dis_option = 0
	 */
	{0x3018, 0x72},
	/* Clock switch output = normal, pclk_div = /1 */
	{0x3020, 0x93},
	/*
	 * lvds_mode_o = 0, clock lane disable when pd_mipi = 0,
	 * pd_mipi enable when rst_sync = 1
	 */
	{0x3022, 0x01},
	{0x3031, 0x0A}, /* mipi_bit_sel = 10 */
	{0x3034, 0x00}, /* Unknown */
	/* sclk_div = /1, sclk_pre_div = /1, chip debug = 1 */
	{0x3106, 0x05},
	{0x3305, 0xF1}, /* Unknown */
	{0x3307, 0x04}, /* Unknown */
	{0x3308, 0x00}, /* Unknown */
	{0x3309, 0x28}, /* Unknown */
	{0x330A, 0x00}, /* Unknown */
	{0x330B, 0x20}, /* Unknown */
	{0x330C, 0x00}, /* Unknown */
	{0x330D, 0x00}, /* Unknown */
	{0x330E, 0x00}, /* Unknown */
	{0x330F, 0x40}, /* Unknown */

	{0x3500, 0x00}, /* long exposure = 0x9A20 */
	{0x3501, 0x9A}, /* long exposure = 0x9A20 */
	{0x3502, 0x20}, /* long exposure = 0x9A20 */
	/*
	 * Digital fraction gain delay option = Delay 1 frame,
	 * Gain change delay option = Delay 1 frame,
	 * Gain delay option = Delay 1 frame,
	 * Gain manual as sensor gain = Input gain as real gain format,
	 * Exposure delay option (must be 0 = Delay 1 frame,
	 * Exposure change delay option (must be 0) = Delay 1 frame
	 */
	{0x3503, 0x00},
	{0x3505, 0x80}, /* gain conversation option */
	/*
	 * [10:7] are integer gain, [6:0] are fraction gain. For example:
	 * 0x80 is 1x gain, 0x100 is 2x gain, 0x1C0 is 3.5x gain
	 */
	{0x3508, 0x02}, /* long gain = 0x0200 */
	{0x3509, 0x00}, /* long gain = 0x0200 */
	{0x350C, 0x00}, /* short gain = 0x0080 */
	{0x350D, 0x80}, /* short gain = 0x0080 */
	{0x3510, 0x00}, /* short exposure = 0x000200 */
	{0x3511, 0x02}, /* short exposure = 0x000200 */
	{0x3512, 0x00}, /* short exposure = 0x000200 */
	{0x3600, 0x00}, /* Unknown */
	{0x3601, 0x00}, /* Unknown */
	{0x3602, 0x00}, /* Unknown */
	{0x3603, 0x00}, /* Unknown */
	{0x3604, 0x22}, /* Unknown */
	{0x3605, 0x30}, /* Unknown */
	{0x3606, 0x00}, /* Unknown */
	{0x3607, 0x20}, /* Unknown */
	{0x3608, 0x11}, /* Unknown */
	{0x3609, 0x28}, /* Unknown */
	{0x360A, 0x00}, /* Unknown */
	{0x360B, 0x06}, /* Unknown */
	{0x360C, 0xDC}, /* Unknown */
	{0x360D, 0x40}, /* Unknown */
	{0x360E, 0x0C}, /* Unknown */
	{0x360F, 0x20}, /* Unknown */
	{0x3610, 0x07}, /* Unknown */
	{0x3611, 0x20}, /* Unknown */
	{0x3612, 0x88}, /* Unknown */
	{0x3613, 0x80}, /* Unknown */
	{0x3614, 0x58}, /* Unknown */
	{0x3615, 0x00}, /* Unknown */
	{0x3616, 0x4A}, /* Unknown */
	{0x3617, 0x90}, /* Unknown */
	{0x3618, 0x56}, /* Unknown */
	{0x3619, 0x70}, /* Unknown */
	{0x361A, 0x99}, /* Unknown */
	{0x361B, 0x00}, /* Unknown */
	{0x361C, 0x07}, /* Unknown */
	{0x361D, 0x00}, /* Unknown */
	{0x361E, 0x00}, /* Unknown */
	{0x361F, 0x00}, /* Unknown */
	{0x3633, 0x0C}, /* Unknown */
	{0x3634, 0x0C}, /* Unknown */
	{0x3635, 0x0C}, /* Unknown */
	{0x3636, 0x0C}, /* Unknown */
	{0x3638, 0xFF}, /* Unknown */
	{0x3645, 0x13}, /* Unknown */
	{0x3646, 0x83}, /* Unknown */
	{0x364A, 0x07}, /* Unknown */
	{0x3700, 0x30},/* Unknown */
	{0x3701, 0x18},/* Unknown */
	{0x3702, 0x50},/* Unknown */
	{0x3703, 0x32},/* Unknown */
	{0x3704, 0x28},/* Unknown */
	{0x3705, 0x00},/* Unknown */
	{0x3706, 0x6A},/* Unknown */
	{0x3707, 0x08},/* Unknown */
	{0x3708, 0x48},/* Unknown */
	{0x3709, 0x66},/* Unknown */
	{0x370A, 0x01},/* Unknown */
	{0x370B, 0x6A},/* Unknown */
	{0x370C, 0x07},/* Unknown */
	{0x3712, 0x44},/* Unknown */
	{0x3714, 0x24},/* Unknown */
	{0x3718, 0x14},/* Unknown */
	{0x3719, 0x31},/* Unknown */
	{0x371E, 0x31},/* Unknown */
	{0x371F, 0x7F},/* Unknown */
	{0x3720, 0x0A},/* Unknown */
	{0x3721, 0x0A},/* Unknown */
	{0x3724, 0x0C},/* Unknown */
	{0x3725, 0x02},/* Unknown */
	{0x3726, 0x0C},/* Unknown */
	{0x3728, 0x0A},/* Unknown */
	{0x3729, 0x03},/* Unknown */
	{0x372A, 0x06},/* Unknown */
	{0x372B, 0xA6},/* Unknown */
	{0x372C, 0xA6},/* Unknown */
	{0x372D, 0xA6},/* Unknown */
	{0x372E, 0x0C},/* Unknown */
	{0x372F, 0x20},/* Unknown */
	{0x3730, 0x02},/* Unknown */
	{0x3731, 0x0C},/* Unknown */
	{0x3732, 0x28},/* Unknown */
	{0x3733, 0x10},/* Unknown */
	{0x3734, 0x40},/* Unknown */
	{0x3736, 0x30},/* Unknown */
	{0x373A, 0x0A},/* Unknown */
	{0x373B, 0x0B},/* Unknown */
	{0x373C, 0x14},/* Unknown */
	{0x373E, 0x06},/* Unknown */
	{0x3755, 0x10},/* Unknown */
	{0x3758, 0x00},/* Unknown */
	{0x3759, 0x4C},/* Unknown */
	{0x375A, 0x0C},/* Unknown */
	{0x375B, 0x26},/* Unknown */
	{0x375C, 0x20},/* Unknown */
	{0x375D, 0x04},/* Unknown */
	{0x375E, 0x00},/* Unknown */
	{0x375F, 0x28},/* Unknown */
	{0x3760, 0x00},/* Unknown */
	{0x3761, 0x00},/* Unknown */
	{0x3762, 0x00},/* Unknown */
	{0x3763, 0x00},/* Unknown */
	{0x3766, 0xFF},/* Unknown */
	{0x3768, 0x22},/* Unknown */
	{0x3769, 0x44},/* Unknown */
	{0x376A, 0x44},/* Unknown */
	{0x376B, 0x00},/* Unknown */
	{0x376F, 0x01},/* Unknown */
	{0x3772, 0x46},/* Unknown */
	{0x3773, 0x04},/* Unknown */
	{0x3774, 0x2C},/* Unknown */
	{0x3775, 0x13},/* Unknown */
	{0x3776, 0x08},/* Unknown */
	{0x3777, 0x00},/* Unknown */
	{0x3778, 0x16},/* Unknown */
	{0x37A0, 0x88},/* Unknown */
	{0x37A1, 0x7A},/* Unknown */
	{0x37A2, 0x7A},/* Unknown */
	{0x37A3, 0x00},/* Unknown */
	{0x37A4, 0x00},/* Unknown */
	{0x37A5, 0x00},/* Unknown */
	{0x37A6, 0x00},/* Unknown */
	{0x37A7, 0x88},/* Unknown */
	{0x37A8, 0x98},/* Unknown */
	{0x37A9, 0x98},/* Unknown */
	{0x37AA, 0x88},/* Unknown */
	{0x37AB, 0x5C},/* Unknown */
	{0x37AC, 0x5C},/* Unknown */
	{0x37AD, 0x55},/* Unknown */
	{0x37AE, 0x19},/* Unknown */
	{0x37AF, 0x19},/* Unknown */
	{0x37B0, 0x00},/* Unknown */
	{0x37B1, 0x00},/* Unknown */
	{0x37B2, 0x00},/* Unknown */
	{0x37B3, 0x84},/* Unknown */
	{0x37B4, 0x84},/* Unknown */
	{0x37B5, 0x66},/* Unknown */
	{0x37B6, 0x00},/* Unknown */
	{0x37B7, 0x00},/* Unknown */
	{0x37B8, 0x00},/* Unknown */
	{0x37B9, 0xFF},/* Unknown */

	{0x3800, 0x00},/* h_crop_start high */
	{0x3801, 0x0C},/* h_crop_start low */
	{0x3802, 0x00},/* v_crop_start high */
	{0x3803, 0x0C},/* v_crop_start low */
	{0x3804, 0x0C},/* h_crop_end high */
	{0x3805, 0xD3},/* h_crop_end low */
	{0x3806, 0x09},/* v_crop_end high */
	{0x3807, 0xA3},/* v_crop_end low */
	{0x3808, 0x0C},/* h_output_size high */
	{0x3809, 0xC0},/* h_output_size low */
	{0x380A, 0x09},/* v_output_size high */
	{0x380B, 0x90},/* v_output_size low */
	{0x380C, 0x07},/* horizontal timing size high */
	{0x380D, 0x94},/* horizontal timing size low */
	{0x380E, 0x0A},/* vertical timing size high */
	{0x380F, 0x0D},/* vertical timing size low */
	{0x3810, 0x00},/* h_win offset high */
	{0x3811, 0x04},/* h_win offset low */
	{0x3812, 0x00},/* v_win offset high */
	{0x3813, 0x02},/* v_win offset low */
	{0x3814, 0x01},/* h_odd_inc */
	{0x3815, 0x01},/* h_even_inc */
	{0x3820, 0x00},/* format1 */
	{0x3821, 0x40},/* format2 */
	{0x382A, 0x01},/* v_odd_inc */
	{0x382B, 0x01},/* v_even_inc */

	{0x3830, 0x06},/* Unknown */
	{0x3836, 0x01},/* Unknown */
	{0x3837, 0x18},/* Unknown */
	{0x3841, 0xFF},/* AUTO_SIZE_CTRL */
	{0x3846, 0x48},/* Unknown */

	{0x3D85, 0x14},/* OTP_REG85 */
	{0x3D8C, 0x73},/* OTP_SETTING_STT_ADDRESS */
	{0x3D8D, 0xDE},/* OTP_SETTING_STT_ADDRESS */
	{0x3F08, 0x10},/* PSRAM control register */
	{0x3F0A, 0x80},/* PSRAM control register */

	{0x4000, 0xF1},/* BLC CTRL00 = default */
	{0x4001, 0x00},/* BLC CTRL01 */
	{0x4002, 0x27},/* BLC offset = 0x27 */
	{0x4005, 0x10},/* BLC target = 0x0010 */
	{0x4009, 0x81},/* BLC CTRL09 */
	{0x400B, 0x0C},/* BLC CTRL0B = default */
	{0x400A, 0x01},
	{0x4011, 0x20},/* BLC CTRL11 = 0x20 */
	{0x401B, 0x00},/* Zero line R coeff. = 0x0000 */
	{0x401D, 0x00},/* Zero line T coeff. = 0x0000 */
	{0x401F, 0x00},/* BLC CTRL1F */
	{0x4020, 0x00},/* Anchor left start = 0x0004 */
	{0x4021, 0x04},/* Anchor left start = 0x0004 */
	{0x4022, 0x0C},/* Anchor left end = 0x0C60 */
	{0x4023, 0x60},/* Anchor left end = 0x0C60 */
	{0x4024, 0x0F},/* Anchor right start = 0x0F36 */
	{0x4025, 0x36},/* Anchor right start = 0x0F36 */
	{0x4026, 0x0F},/* Anchor right end = 0x0F37 */
	{0x4027, 0x37},/* Anchor right end = 0x0F37 */
	{0x4028, 0x00},/* Top zero line start = 0 */
	{0x4029, 0x04},/* Top zero line number = 4 */
	{0x402A, 0x04},/* Top black line start = 4 */
	{0x402B, 0x08},/* Top black line number = 8 */
	{0x402C, 0x00},/* Bottom zero start line = 0 */
	{0x402D, 0x02},/* Bottom zero line number = 2 */
	{0x402E, 0x04},/* Bottom black line start = 4 */
	{0x402F, 0x08},/* Bottom black line number = 8 */

	{0x4034, 0x3F},/* Unknown */
	{0x403D, 0x04},/* BLC CTRL3D */
	{0x4300, 0xFF},/* clip_max[11:4] = 0xFFF */
	{0x4301, 0x00},/* clip_min[11:4] = 0 */
	{0x4302, 0x0F},/* clip_min/max[3:0] */
	{0x4307, 0x01},/* Unknown */
	{0x4316, 0x00},/* CTRL16 = default */
	{0x4503, 0x18},/* Unknown */
	{0x4500, 0x38},/* Unknown */
	{0x4600, 0x01},/* Unknown */
	{0x4601, 0x97},/* Unknown */
	/* wkup_dly = Mark1 wakeup delay/2^10 = 0x25 */
	{0x4808, 0x25},
	{0x4816, 0x52}, /* Embedded data type*/
	{0x481F, 0x32}, /* clk_prepare_min = 0x32 */
	{0x4825, 0x3A}, /* lpx_p_min = 0x3A */
	{0x4826, 0x40}, /* hs_prepare_min = 0x40 */
	{0x4837, 0x14}, /* pclk_period = 0x14 */
	{0x4850, 0x10}, /* LANE SEL01 */
	{0x4851, 0x32}, /* LANE SEL02 */

	{0x4B00, 0x2A}, /* Unknown */
	{0x4B0D, 0x00}, /* Unknown */
	{0x4D00, 0x04}, /* TPM_CTRL_REG */
	{0x4D01, 0x18}, /* TPM_CTRL_REG */
	{0x4D02, 0xC3}, /* TPM_CTRL_REG */
	{0x4D03, 0xFF}, /* TPM_CTRL_REG */
	{0x4D04, 0xFF}, /* TPM_CTRL_REG */
	{0x4D05, 0xFF}, /* TPM_CTRL_REG */

	/*
	 * Lens correction (LENC) function enable = 0
	 * Slave sensor AWB Gain function enable = 1
	 * Slave sensor AWB Statistics function enable = 1
	 * Master sensor AWB Gain function enable = 1
	 * Master sensor AWB Statistics function enable = 1
	 * Black DPC function enable = 1
	 * White DPC function enable =1
	 */
	{0x5000, 0x7E},
	{0x5001, 0x01}, /* BLC function enable = 1 */
	/*
	 * Horizontal scale function enable = 0
	 * WBMATCH bypass mode = Select slave sensor's gain
	 * WBMATCH function enable = 0
	 * Master MWB gain support RGBC = 0
	 * OTP_DPC function enable = 1
	 * Manual mode of VarioPixel function enable = 0
	 * Manual enable of VarioPixel function enable = 0
	 * Use VSYNC to latch ISP modules's function enable signals = 0
	 */
	{0x5002, 0x08},
	/*
	 * Bypass all ISP modules after BLC module = 0
	 * DPC_DBC buffer control enable = 1
	 * WBMATCH VSYNC selection = Select master sensor's VSYNC fall
	 * Select master AWB gain to embed line = AWB gain before manual mode
	 * Enable BLC's input flip_i signal = 0
	 */
	{0x5003, 0x20},
	{0x5041, 0x1D}, /* ISP CTRL41 - embedded data=on */
	{0x5046, 0x12}, /* ISP CTRL46 = default */
	/*
	 * Tail enable = 1
	 * Saturate cross cluster enable = 1
	 * Remove cross cluster enable = 1
	 * Enable to remove connected defect pixels in same channel = 1
	 * Enable to remove connected defect pixels in different channel = 1
	 * Smooth enable, use average G for recovery = 1
	 * Black/white sensor mode enable = 0
	 * Manual mode enable = 0
	 */
	{0x5780, 0xFC},
	{0x5784, 0x0C}, /* DPC CTRL04 */
	{0x5787, 0x40}, /* DPC CTRL07 */
	{0x5788, 0x08}, /* DPC CTRL08 */
	{0x578A, 0x02}, /* DPC CTRL0A */
	{0x578B, 0x01}, /* DPC CTRL0B */
	{0x578C, 0x01}, /* DPC CTRL0C */
	{0x578E, 0x02}, /* DPC CTRL0E */
	{0x578F, 0x01}, /* DPC CTRL0F */
	{0x5790, 0x01}, /* DPC CTRL10 */
	{0x5901, 0x00}, /* VAP CTRL01 = default */
	/* WINC CTRL08 = embedded data in 1st line*/
	{0x5A08, 0x00},
	{0x5B00, 0x02}, /* OTP CTRL00 */
	{0x5B01, 0x10}, /* OTP CTRL01 */
	{0x5B02, 0x03}, /* OTP CTRL02 */
	{0x5B03, 0xCF}, /* OTP CTRL03 */
	{0x5B05, 0x6C}, /* OTP CTRL05 = default */
	{0x5E00, 0x00}, /* PRE CTRL00 = default */
	{0x5E01, 0x41}, /* PRE_CTRL01 = default */

	{0x0100, 0x00}, /* software_standby */
	{0x3778, 0x17}, /* Unknown */
	{0x3800, 0x02}, /* h_crop_start high */
	{0x3801, 0x26}, /* h_crop_start low */
	{0x3802, 0x02}, /* v_crop_start high */
	{0x3803, 0x8C}, /* v_crop_start low */
	{0x3804, 0x0A}, /* h_crop_end high */
	{0x3805, 0x9D}, /* h_crop_end low */
	{0x3806, 0x07}, /* v_crop_end high */
	{0x3807, 0x0A}, /* v_crop_end low */
	{0x3808, 0x07}, /* h_output_size high  0x07 modefied by tbl*/
	{0x3809, 0x80}, /* h_output_size low   0x90  */
	{0x380A, 0x04}, /* v_output_size high  0x04  */
	{0x380B, 0x38}, /* v_output_size low   0x48  */
	{0x380C, 0x08}, /* horizontal timing size high */
	{0x380D, 0x24}, /* horizontal timing size low */
	{0x380E, 0x05}, /* vertical timing size high */
	{0x380F, 0xEC}, /* vertical timing size low */
	{0x3814, 0x01}, /* h_odd_inc */
	{0x3815, 0x01}, /* h_even_inc */
	{0x3820, 0x00}, /* format1 */
	{0x3821, 0x40}, /* format2 */
	{0x382A, 0x01}, /* v_odd_inc */
	{0x382B, 0x01}, /* v_even_inc */
	{0x3830, 0x06}, /* Unknown */
	{0x3836, 0x01}, /* Unknown */
	{0x3D85, 0x14}, /* OTP_REG85 */
	{0x3F08, 0x10}, /* PSRAM control register */
	{0x4034, 0x3F}, /* Unknown */
	{0x403D, 0x04}, /* BLC CTRL3D */
	{0x4600, 0x00}, /* Unknown */
	{0x4601, 0xef}, /* Unknown */
	{0x4837, 0x16}, /* pclk_period = 0x16 */

};

static struct cc_power_act power_up_act[] = {
	{CC_PWDN,  CC_DOWN},
	{CC_DELAY, 10},
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 5},
	{CC_RESET, CC_UP},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_UP},
	{CC_DELAY, 5},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_DOWN},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_SGRBG12_1X12,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define FHD_WIDTH		1920
#define FHD_HEIGHT		1080
static const struct cc_win_size win_sizes[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
		.mfreq = 540 * 1000 * 1000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = cc_si->read_reg(cc_si, 0x300B, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0x300C, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x8858) {
		PRT_ERR("ID wrong! (0x%04x != 0x8858)\n", id);
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
			PRT_ERR("%s read i %d reg 0x%04x error\n",
				cc_si->name, i, init_reg_list[i].reg);
			return ret;
		}

		if (init_reg_list[i].val != value) {
			PRT_ERR("%s %d 0x%04x (0x%02x) != (0x%02x)\n",
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
		ret = cc_si->write_reg(cc_si, 0x0100, 0x01);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->write_reg(cc_si, 0x0100, 0x00);
		if (ret)
			PRT_ERR("%d\n", on);
	}

	return ret;
}

/*
 * ctrl for sensor start
 */

static int get_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u8 val = 0;
	int ret = 0;

	/* read */
	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &val);
	reg_data->reg_value = val;
#if 0
	PRT_INFO("addr: 0x%x, val: 0x%02x.\n",
		reg_data->reg_add, reg_data->reg_value);
#endif
	return ret;
}

static int set_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u8 val = reg_data->reg_value;
	u8 read_val;
	int ret;

	/* write */
	ret = cc_si->write_reg(cc_si, reg_data->reg_add, val);
	PRT_DBG("addr=0x%x,val=0x%02x.\n",
		reg_data->reg_add, reg_data->reg_value);

	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &read_val);
	if (read_val != val)
		PRT_DBG("ret=%d,reg_add=0x%x,val=0x%02x,read_val=0x%02x\n",
			ret, reg_data->reg_add, val, read_val);

	return ret;
}

static int viss_get_sensor_fmt_info(struct cc_sensor_info *cc_si,
		struct viss_isp_sensor_fmt_info *sensor_fmt_info)
{
	int ret = 0;

	sensor_fmt_info->pclk = 95 * 1000 * 1000;
	sensor_fmt_info->vts = 1516;
	sensor_fmt_info->hts = 2084;
	sensor_fmt_info->hoffset = 0;
	sensor_fmt_info->voffset = 0;
	sensor_fmt_info->width = 1920;
	sensor_fmt_info->height = 1080;
	sensor_fmt_info->bayer_mode = ISP_PATTERN_GBRG;
	sensor_fmt_info->buswidth = ISP_INPUT_10BITS;

	return ret;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;
	int exp_val, gain_val, frame_length, shutter;
	unsigned char explow = 0, expmid = 0, exphigh = 0;
	unsigned char gainlow = 0, gainhigh = 0;

	exp_val = exp_gain->exp_time;
	gain_val = exp_gain->exp_gain * 8 / 256;

	if (gain_val < 1 * 16)
		gain_val = 16;
	if (gain_val > 64 * 16 - 1)
		gain_val = 64 * 16 - 1;

	if (exp_val > 0xfffff)
		exp_val = 0xfffff;

	gain_val *= 8;

	if (gain_val < 2 * 16 * 8) {
		gainhigh = 0;
		gainlow = gain_val;
	} else if (2 * 16 * 8 <= gain_val &&
		gain_val < 4 * 16 * 8) {
		gainhigh = 1;
		gainlow = gain_val/2-8;
	} else if (4 * 16 * 8 <= gain_val &&
		gain_val < 8 * 16 * 8) {
		gainhigh = 3;
		gainlow = gain_val / 4 - 12;
	} else {
		gainhigh = 7;
		gainlow = gain_val / 8 - 8;
	}

	exphigh = (unsigned char) ((0x0f0000 & exp_val) >> 16);
	expmid  = (unsigned char) ((0x00ff00 & exp_val) >> 8);
	explow  = (unsigned char) ((0x0000ff & exp_val));

	shutter = exp_val / 16;
	if (shutter  > 1516 - 4)
		frame_length = shutter + 4;
	else
		frame_length = 1516;

	ret = cc_si->write_reg(cc_si, 0x3208, 0x00); /* enter group write */

	ret = cc_si->write_reg(cc_si, 0x380f, (frame_length & 0xff));
	ret = cc_si->write_reg(cc_si, 0x380e, (frame_length >> 8));

	ret = cc_si->write_reg(cc_si, 0x3509, gainlow);
	ret = cc_si->write_reg(cc_si, 0x3508, gainhigh);

	ret = cc_si->write_reg(cc_si, 0x3502, explow);
	ret = cc_si->write_reg(cc_si, 0x3501, expmid);
	ret = cc_si->write_reg(cc_si, 0x3500, exphigh);
	ret = cc_si->write_reg(cc_si, 0x3208, 0x10); /* end group write */
	ret = cc_si->write_reg(cc_si, 0x3208, 0xa0); /* init group write */

	return 0;
}

static long sensor_ioctl(struct cc_sensor_info *cc_si, unsigned int cmd,
				void *arg)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_VISS_GET_SENSOR:
		ret = get_reg(cc_si, (struct viss_isp_reg_data *)arg);
		break;
	case VIDIOC_VISS_SET_SENSOR:
		ret = set_reg(cc_si, (struct viss_isp_reg_data *)arg);
		break;
	case VIDIOC_VISS_ISP_EXP_CTRL:
		ret = viss_isp_exp_ctrl(cc_si,
			(struct viss_isp_exp_gain *)arg);
		break;
	case VIDIOC_VISS_ISP_SENSOR_FMT_INFO:
		ret = viss_get_sensor_fmt_info(cc_si,
			(struct viss_isp_sensor_fmt_info *)arg);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

/*
 * ctrl for sensor finish
 */


#define DRIVER_NAME "ov8858_mipi"

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

	cc_si->dlanes = 4;

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
	cc_si->sensor_ioctl = sensor_ioctl;

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

static const struct of_device_id ov8858_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov8858_mipi_of_match);

static struct platform_driver ov8858_mipi_driver = {
	.driver = {
		.of_match_table	= ov8858_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov8858_mipi_driver);

MODULE_DESCRIPTION("OV8858-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


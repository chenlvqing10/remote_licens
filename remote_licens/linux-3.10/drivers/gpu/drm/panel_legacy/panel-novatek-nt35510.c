/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/backlight.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <uapi/linux/media-bus-format.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include "../lombo_legacy/lombo_doss.h"

/*
 * @INTERFACE
 * it can config value below, for reference only.
 * VO_DEV_PRGB, VO_DEV_SRGB_RGB, VO_DEV_SRGB_DUMMY_RGB, VO_DEV_SRGB_RGB_DUMMY,
 * VO_DEV_CPU_18BIT_RGB666,
 * VO_DEV_CPU_16BIT_RGB888, VO_DEV_CPU_16BIT_RGB666, VO_DEV_CPU_16BIT_RGB565,
 * VO_DEV_CPU_9BIT_RGB666,
 * VO_DEV_CPU_8BIT_RGB666, VO_DEV_CPU_8BIT_RGB565, VO_DEV_CPU_8BIT_RGB888,
 * VO_DEV_LVDS, VO_DEV_MIPI_DSI_VIDEO, VO_DEV_MIPI_DSI_CMD,
 * VO_DEV_BT601_24BIT, VO_DEV_BT601_16BIT, VO_DEV_BT601_8BIT,
 * VO_DEV_BT656_8BIT,
 * VO_DEV_BT1120_16BIT,VO_DEV_BT1120_8BIT
 * VO_DEV_TVE
 */
#define INTERFACE	(VO_DEV_CPU_16BIT_RGB565)
#define HAS_VSYNC_CMD	(FALSE)
#define HAS_HSYNC_CMD	(FALSE)
#define CMD_ALIGN	(0)

/*
 * TCON COMMON CONFIGURATION
 *
 * @TCON_FMT
 * it can config value below, for reference only.
 * TCON_FMT_RGB888, TCON_FMT_RGB666, TCON_FMT_RGB565
 * TCON_FMT_YUV444, TCON_FMT_YUV422,
 */
#define TCON_FMT	(TCON_FMT_RGB565)
#define IS_INTERLACE	(FALSE)
#define GAMMA_EN	(FALSE)
#define FRC_EN		(FALSE)
#define CSC_EN		(FALSE)
#define HAS_TE		(FALSE)
#define TE_POL		(TCON_IO_POS)

/* DOSS TOP IO Config */
#define DCS_MDELAY_FLAG (0)
#define PWM_IO		(0)
#define RESET_IO	(1)
#define IO_HIGH		(1)
#define IO_LOW		(0)

struct panel_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;
	const struct display_timing *timings;
	unsigned int num_timings;

	unsigned int bpc;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	/**
	 * @prepare: the time (in milliseconds) that it takes for the panel to
	 *           become ready and start receiving video data
	 * @enable: the time (in milliseconds) that it takes for the panel to
	 *          display the first valid frame after starting to receive
	 *          video data
	 * @disable: the time (in milliseconds) that it takes for the panel to
	 *           turn the display off (no content is visible)
	 * @unprepare: the time (in milliseconds) that it takes for the panel
	 *             to power itself down completely
	 */
	struct {
		unsigned int prepare;
		unsigned int enable;
		unsigned int disable;
		unsigned int unprepare;
	} delay;

	u32 bus_format;
};

struct panel_simple {
	struct drm_panel base;
	bool prepared;
	bool enabled;

	struct device *dev;
	const struct panel_desc *desc;

	struct backlight_device *backlight;
	struct regulator *supply;
	struct i2c_adapter *ddc;

	unsigned enable_gpio;
};

static u32 gamma_tbl[256] = {
/* red   power = 2.2 */
/* green power = 1/2.2 */
/* blue  power = 1.4 */
0x000000, 0x001400, 0x001c00, 0x002100, 0x002600, 0x002a01, 0x002e01, 0x003101,
0x003402, 0x003702, 0x003a02, 0x003d03, 0x003f03, 0x004203, 0x004404, 0x004604,
0x004805, 0x004a05, 0x004c06, 0x004e06, 0x005007, 0x015207, 0x015308, 0x015508,
0x015709, 0x015809, 0x015a0a, 0x015c0a, 0x015d0b, 0x025f0c, 0x02600c, 0x02620d,
0x02630d, 0x02640e, 0x03660f, 0x03670f, 0x036810, 0x036a11, 0x036b11, 0x046c12,
0x046e13, 0x046f13, 0x047014, 0x057115, 0x057215, 0x057416, 0x057517, 0x067617,
0x067718, 0x067819, 0x07791a, 0x077a1a, 0x077c1b, 0x087d1c, 0x087e1c, 0x087f1d,
0x09801e, 0x09811f, 0x098220, 0x0a8320, 0x0a8421, 0x0a8522, 0x0b8623, 0x0b8723,
0x0c8824, 0x0c8925, 0x0c8a26, 0x0d8b27, 0x0d8c28, 0x0e8d28, 0x0e8d29, 0x0f8e2a,
0x0f8f2b, 0x10902c, 0x10912d, 0x11922d, 0x11932e, 0x12942f, 0x129530, 0x139631,
0x139632, 0x149733, 0x149834, 0x159934, 0x169a35, 0x169b36, 0x179b37, 0x179c38,
0x189d39, 0x199e3a, 0x199f3b, 0x1a9f3c, 0x1aa03d, 0x1ba13e, 0x1ca23e, 0x1ca33f,
0x1da340, 0x1ea441, 0x1ea542, 0x1fa643, 0x20a644, 0x21a745, 0x21a846, 0x22a947,
0x23a948, 0x24aa49, 0x24ab4a, 0x25ac4b, 0x26ac4c, 0x27ad4d, 0x27ae4e, 0x28af4f,
0x29af50, 0x2ab051, 0x2bb152, 0x2cb153, 0x2cb254, 0x2db355, 0x2eb456, 0x2fb457,
0x30b558, 0x31b659, 0x32b65a, 0x33b75b, 0x33b85c, 0x34b85d, 0x35b95e, 0x36ba5f,
0x37ba61, 0x38bb62, 0x39bc63, 0x3abc64, 0x3bbd65, 0x3cbe66, 0x3dbe67, 0x3ebf68,
0x3fc069, 0x40c06a, 0x41c16b, 0x42c16c, 0x43c26d, 0x44c36f, 0x46c370, 0x47c471,
0x48c572, 0x49c573, 0x4ac674, 0x4bc675, 0x4cc776, 0x4dc877, 0x4ec879, 0x50c97a,
0x51c97b, 0x52ca7c, 0x53cb7d, 0x54cb7e, 0x56cc7f, 0x57cc81, 0x58cd82, 0x59ce83,
0x5bce84, 0x5ccf85, 0x5dcf86, 0x5ed088, 0x60d189, 0x61d18a, 0x62d28b, 0x64d28c,
0x65d38d, 0x66d38f, 0x68d490, 0x69d591, 0x6ad592, 0x6cd693, 0x6dd695, 0x6ed796,
0x70d797, 0x71d898, 0x73d999, 0x74d99b, 0x75da9c, 0x77da9d, 0x78db9e, 0x7adba0,
0x7bdca1, 0x7ddca2, 0x7edda3, 0x80dda4, 0x81dea6, 0x83dfa7, 0x84dfa8, 0x86e0a9,
0x87e0ab, 0x89e1ac, 0x8be1ad, 0x8ce2ae, 0x8ee2b0, 0x8fe3b1, 0x91e3b2, 0x93e4b3,
0x94e4b5, 0x96e5b6, 0x98e5b7, 0x99e6b9, 0x9be6ba, 0x9de7bb, 0x9ee7bc, 0xa0e8be,
0xa2e8bf, 0xa3e9c0, 0xa5e9c2, 0xa7eac3, 0xa9eac4, 0xaaebc5, 0xacebc7, 0xaeecc8,
0xb0ecc9, 0xb1edcb, 0xb3edcc, 0xb5eecd, 0xb7eecf, 0xb9efd0, 0xbbefd1, 0xbcf0d3,
0xbef0d4, 0xc0f1d5, 0xc2f1d7, 0xc4f2d8, 0xc6f2d9, 0xc8f3db, 0xcaf3dc, 0xccf4dd,
0xcef4df, 0xd0f5e0, 0xd2f5e1, 0xd4f6e3, 0xd6f6e4, 0xd8f7e5, 0xdaf7e7, 0xdcf8e8,
0xdef8e9, 0xe0f9eb, 0xe2f9ec, 0xe4faed, 0xe6faef, 0xe8faf0, 0xeafbf2, 0xecfbf3,
0xeefcf4, 0xf0fcf6, 0xf2fdf7, 0xf5fdf9, 0xf7fefa, 0xf9fefb, 0xfbfffd, 0xfdfffe,
};

static u32 init_code[] = {
	/************* Start Initial Sequence **********/
	/**********LV2 Page 1 enable***********/
	2, 0xF000 ,	0x0055,
	2, 0xF001 ,	0x00AA,
	2, 0xF002 ,	0x0052,
	2, 0xF003 ,	0x0008,
	2, 0xF004 ,	0x0001,

	/*************AVDD Set AVDD 5.2V*************/
	2, 0xB000 , 0x000D,
	2, 0xB001 , 0x000D,
	2, 0xB002 , 0x000D,

	/************AVDD ratio****************/
	2, 0xB600 , 0x0034,
	2, 0xB601 , 0x0034,
	2, 0xB602 , 0x0034,

	/************AVEE  -5.2V****************/
	2, 0xB100 , 0x000D,
	2, 0xB101 , 0x000D,
	2, 0xB102 , 0x000D,

	/***********AVEE ratio*************/
	2, 0xB700 , 0x0034,
	2, 0xB701 , 0x0034,
	2, 0xB702 , 0x0034,

	/***********VCL  -2.5V*************/
	2, 0xB200 , 0x0000,
	2, 0xB201 , 0x0000,
	2, 0xB202 , 0x0000,

	/**************VCL ratio*****************/
	2, 0xB800 , 0x0024,
	2, 0xB801 , 0x0024,
	2, 0xB802 , 0x0024,

	/*************VGH 15V  (Free pump)*********/
	2, 0xBF00 , 0x0001,
	2, 0xB300 , 0x000F,
	2, 0xB301 , 0x000F,
	2, 0xB302 , 0x000F,

	/*************VGH ratio*****************/
	2, 0xB900 , 0x0034,
	2, 0xB901 , 0x0034,
	2, 0xB902 , 0x0034,

	/***************VGL_REG -10V**************/
	2, 0xB500 , 0x0008,
	2, 0xB501 , 0x0008,
	2, 0xB502 , 0x0008,

	2, 0xC200 , 0x0003,

	/*****************VGLX ratio***************/
	2, 0xBA00 , 0x0024,
	2, 0xBA01 , 0x0024,
	2, 0xBA02 , 0x0024,

	/*************VGMP/VGSP 4.5V/0V*************/
	2, 0xBC00 , 0x0000,
	2, 0xBC01 , 0x0078,
	2, 0xBC02 , 0x0000,

	/************VGMN/VGSN -4.5V/0V****************/
	2, 0xBD00 , 0x0000,
	2, 0xBD01 , 0x0078,
	2, 0xBD02 , 0x0000,

	/************VCOM  -1.25V****************/
	2, 0xBE00 , 0x0000,
	2, 0xBE01 , 0x0067,

	/************Gamma Setting******************/
	2, 0xD100 , 0x0000,
	2, 0xD101 , 0x0032,
	2, 0xD102 , 0x0000,
	2, 0xD103 , 0x0033,
	2, 0xD104 , 0x0000,
	2, 0xD105 , 0x0041,
	2, 0xD106 , 0x0000,
	2, 0xD107 , 0x005A,
	2, 0xD108 , 0x0000,
	2, 0xD109 , 0x0076,
	2, 0xD10A , 0x0000,
	2, 0xD10B , 0x00A7,
	2, 0xD10C , 0x0000,
	2, 0xD10D , 0x00CF,
	2, 0xD10E , 0x0001,
	2, 0xD10F , 0x0009,
	2, 0xD110 , 0x0001,
	2, 0xD111 , 0x0036,
	2, 0xD112 , 0x0001,
	2, 0xD113 , 0x0073,
	2, 0xD114 , 0x0001,
	2, 0xD115 , 0x009F,
	2, 0xD116 , 0x0001,
	2, 0xD117 , 0x00DF,
	2, 0xD118 , 0x0002,
	2, 0xD119 , 0x0010,
	2, 0xD11A , 0x0002,
	2, 0xD11B , 0x0011,
	2, 0xD11C , 0x0002,
	2, 0xD11D , 0x003D,
	2, 0xD11E , 0x0002,
	2, 0xD11F , 0x0069,
	2, 0xD120 , 0x0002,
	2, 0xD121 , 0x0081,
	2, 0xD122 , 0x0002,
	2, 0xD123 , 0x009D,
	2, 0xD124 , 0x0002,
	2, 0xD125 , 0x00AD,
	2, 0xD126 , 0x0002,
	2, 0xD127 , 0x00C3,
	2, 0xD128 , 0x0002,
	2, 0xD129 , 0x00D0,
	2, 0xD12A , 0x0002,
	2, 0xD12B , 0x00E2,
	2, 0xD12C , 0x0002,
	2, 0xD12D , 0x00EE,
	2, 0xD12E , 0x0003,
	2, 0xD12F , 0x0001,
	2, 0xD130 , 0x0003,
	2, 0xD131 , 0x0026,
	2, 0xD132 , 0x0003,
	2, 0xD133 , 0x008E,

	2, 0xD200 , 0x0000,
	2, 0xD201 , 0x0032,
	2, 0xD202 , 0x0000,
	2, 0xD203 , 0x0033,
	2, 0xD204 , 0x0000,
	2, 0xD205 , 0x0041,
	2, 0xD206 , 0x0000,
	2, 0xD207 , 0x005A,
	2, 0xD208 , 0x0000,
	2, 0xD209 , 0x0076,
	2, 0xD20A , 0x0000,
	2, 0xD20B , 0x00A7,
	2, 0xD20C , 0x0000,
	2, 0xD20D , 0x00CF,
	2, 0xD20E , 0x0001,
	2, 0xD20F , 0x0009,
	2, 0xD210 , 0x0001,
	2, 0xD211 , 0x0036,
	2, 0xD212 , 0x0001,
	2, 0xD213 , 0x0073,
	2, 0xD214 , 0x0001,
	2, 0xD215 , 0x009F,
	2, 0xD216 , 0x0001,
	2, 0xD217 , 0x00DF,
	2, 0xD218 , 0x0002,
	2, 0xD219 , 0x0010,
	2, 0xD21A , 0x0002,
	2, 0xD21B , 0x0011,
	2, 0xD21C , 0x0002,
	2, 0xD21D , 0x003D,
	2, 0xD21E , 0x0002,
	2, 0xD21F , 0x0069,
	2, 0xD220 , 0x0002,
	2, 0xD221 , 0x0081,
	2, 0xD222 , 0x0002,
	2, 0xD223 , 0x009D,
	2, 0xD224 , 0x0002,
	2, 0xD225 , 0x00AD,
	2, 0xD226 , 0x0002,
	2, 0xD227 , 0x00C3,
	2, 0xD228 , 0x0002,
	2, 0xD229 , 0x00D0,
	2, 0xD22A , 0x0002,
	2, 0xD22B , 0x00E2,
	2, 0xD22C , 0x0002,
	2, 0xD22D , 0x00EE,
	2, 0xD22E , 0x0003,
	2, 0xD22F , 0x0001,
	2, 0xD230 , 0x0003,
	2, 0xD231 , 0x0026,
	2, 0xD232 , 0x0003,
	2, 0xD233 , 0x008E,

	2, 0xD300 , 0x0000,
	2, 0xD301 , 0x0032,
	2, 0xD302 , 0x0000,
	2, 0xD303 , 0x0033,
	2, 0xD304 , 0x0000,
	2, 0xD305 , 0x0041,
	2, 0xD306 , 0x0000,
	2, 0xD307 , 0x005A,
	2, 0xD308 , 0x0000,
	2, 0xD309 , 0x0076,
	2, 0xD30A , 0x0000,
	2, 0xD30B , 0x00A7,
	2, 0xD30C , 0x0000,
	2, 0xD30D , 0x00CF,
	2, 0xD30E , 0x0001,
	2, 0xD30F , 0x0009,
	2, 0xD310 , 0x0001,
	2, 0xD311 , 0x0036,
	2, 0xD312 , 0x0001,
	2, 0xD313 , 0x0073,
	2, 0xD314 , 0x0001,
	2, 0xD315 , 0x009F,
	2, 0xD316 , 0x0001,
	2, 0xD317 , 0x00DF,
	2, 0xD318 , 0x0002,
	2, 0xD319 , 0x0010,
	2, 0xD31A , 0x0002,
	2, 0xD31B , 0x0011,
	2, 0xD31C , 0x0002,
	2, 0xD31D , 0x003D,
	2, 0xD31E , 0x0002,
	2, 0xD31F , 0x0069,
	2, 0xD320 , 0x0002,
	2, 0xD321 , 0x0081,
	2, 0xD322 , 0x0002,
	2, 0xD323 , 0x009D,
	2, 0xD324 , 0x0002,
	2, 0xD325 , 0x00AD,
	2, 0xD326 , 0x0002,
	2, 0xD327 , 0x00C3,
	2, 0xD328 , 0x0002,
	2, 0xD329 , 0x00D0,
	2, 0xD32A , 0x0002,
	2, 0xD32B , 0x00E2,
	2, 0xD32C , 0x0002,
	2, 0xD32D , 0x00EE,
	2, 0xD32E , 0x0003,
	2, 0xD32F , 0x0001,
	2, 0xD330 , 0x0003,
	2, 0xD331 , 0x0026,
	2, 0xD332 , 0x0003,
	2, 0xD333 , 0x008E,

	2, 0xD400 , 0x0000,
	2, 0xD401 , 0x0032,
	2, 0xD402 , 0x0000,
	2, 0xD403 , 0x0033,
	2, 0xD404 , 0x0000,
	2, 0xD405 , 0x0041,
	2, 0xD406 , 0x0000,
	2, 0xD407 , 0x005A,
	2, 0xD408 , 0x0000,
	2, 0xD409 , 0x0076,
	2, 0xD40A , 0x0000,
	2, 0xD40B , 0x00A7,
	2, 0xD40C , 0x0000,
	2, 0xD40D , 0x00CF,
	2, 0xD40E , 0x0001,
	2, 0xD40F , 0x0009,
	2, 0xD410 , 0x0001,
	2, 0xD411 , 0x0036,
	2, 0xD412 , 0x0001,
	2, 0xD413 , 0x0073,
	2, 0xD414 , 0x0001,
	2, 0xD415 , 0x009F,
	2, 0xD416 , 0x0001,
	2, 0xD417 , 0x00DF,
	2, 0xD418 , 0x0002,
	2, 0xD419 , 0x0010,
	2, 0xD41A , 0x0002,
	2, 0xD41B , 0x0011,
	2, 0xD41C , 0x0002,
	2, 0xD41D , 0x003D,
	2, 0xD41E , 0x0002,
	2, 0xD41F , 0x0069,
	2, 0xD420 , 0x0002,
	2, 0xD421 , 0x0081,
	2, 0xD422 , 0x0002,
	2, 0xD423 , 0x009D,
	2, 0xD424 , 0x0002,
	2, 0xD425 , 0x00AD,
	2, 0xD426 , 0x0002,
	2, 0xD427 , 0x00C3,
	2, 0xD428 , 0x0002,
	2, 0xD429 , 0x00D0,
	2, 0xD42A , 0x0002,
	2, 0xD42B , 0x00E2,
	2, 0xD42C , 0x0002,
	2, 0xD42D , 0x00EE,
	2, 0xD42E , 0x0003,
	2, 0xD42F , 0x0001,
	2, 0xD430 , 0x0003,
	2, 0xD431 , 0x0026,
	2, 0xD432 , 0x0003,
	2, 0xD433 , 0x008E,

	2, 0xD500 , 0x0000,
	2, 0xD501 , 0x0032,
	2, 0xD502 , 0x0000,
	2, 0xD503 , 0x0033,
	2, 0xD504 , 0x0000,
	2, 0xD505 , 0x0041,
	2, 0xD506 , 0x0000,
	2, 0xD507 , 0x005A,
	2, 0xD508 , 0x0000,
	2, 0xD509 , 0x0076,
	2, 0xD50A , 0x0000,
	2, 0xD50B , 0x00A7,
	2, 0xD50C , 0x0000,
	2, 0xD50D , 0x00CF,
	2, 0xD50E , 0x0001,
	2, 0xD50F , 0x0009,
	2, 0xD510 , 0x0001,
	2, 0xD511 , 0x0036,
	2, 0xD512 , 0x0001,
	2, 0xD513 , 0x0073,
	2, 0xD514 , 0x0001,
	2, 0xD515 , 0x009F,
	2, 0xD516 , 0x0001,
	2, 0xD517 , 0x00DF,
	2, 0xD518 , 0x0002,
	2, 0xD519 , 0x0010,
	2, 0xD51A , 0x0002,
	2, 0xD51B , 0x0011,
	2, 0xD51C , 0x0002,
	2, 0xD51D , 0x003D,
	2, 0xD51E , 0x0002,
	2, 0xD51F , 0x0069,
	2, 0xD520 , 0x0002,
	2, 0xD521 , 0x0081,
	2, 0xD522 , 0x0002,
	2, 0xD523 , 0x009D,
	2, 0xD524 , 0x0002,
	2, 0xD525 , 0x00AD,
	2, 0xD526 , 0x0002,
	2, 0xD527 , 0x00C3,
	2, 0xD528 , 0x0002,
	2, 0xD529 , 0x00D0,
	2, 0xD52A , 0x0002,
	2, 0xD52B , 0x00E2,
	2, 0xD52C , 0x0002,
	2, 0xD52D , 0x00EE,
	2, 0xD52E , 0x0003,
	2, 0xD52F , 0x0001,
	2, 0xD530 , 0x0003,
	2, 0xD531 , 0x0026,
	2, 0xD532 , 0x0003,
	2, 0xD533 , 0x008E,

	2, 0xD600 , 0x0000,
	2, 0xD601 , 0x0032,
	2, 0xD602 , 0x0000,
	2, 0xD603 , 0x0033,
	2, 0xD604 , 0x0000,
	2, 0xD605 , 0x0041,
	2, 0xD606 , 0x0000,
	2, 0xD607 , 0x005A,
	2, 0xD608 , 0x0000,
	2, 0xD609 , 0x0076,
	2, 0xD60A , 0x0000,
	2, 0xD60B , 0x00A7,
	2, 0xD60C , 0x0000,
	2, 0xD60D , 0x00CF,
	2, 0xD60E , 0x0001,
	2, 0xD60F , 0x0009,
	2, 0xD610 , 0x0001,
	2, 0xD611 , 0x0036,
	2, 0xD612 , 0x0001,
	2, 0xD613 , 0x0073,
	2, 0xD614 , 0x0001,
	2, 0xD615 , 0x009F,
	2, 0xD616 , 0x0001,
	2, 0xD617 , 0x00DF,
	2, 0xD618 , 0x0002,
	2, 0xD619 , 0x0010,
	2, 0xD61A , 0x0002,
	2, 0xD61B , 0x0011,
	2, 0xD61C , 0x0002,
	2, 0xD61D , 0x003D,
	2, 0xD61E , 0x0002,
	2, 0xD61F , 0x0069,
	2, 0xD620 , 0x0002,
	2, 0xD621 , 0x0081,
	2, 0xD622 , 0x0002,
	2, 0xD623 , 0x009D,
	2, 0xD624 , 0x0002,
	2, 0xD625 , 0x00AD,
	2, 0xD626 , 0x0002,
	2, 0xD627 , 0x00C3,
	2, 0xD628 , 0x0002,
	2, 0xD629 , 0x00D0,
	2, 0xD62A , 0x0002,
	2, 0xD62B , 0x00E2,
	2, 0xD62C , 0x0002,
	2, 0xD62D , 0x00EE,
	2, 0xD62E , 0x0003,
	2, 0xD62F , 0x0001,
	2, 0xD630 , 0x0003,
	2, 0xD631 , 0x0026,
	2, 0xD632 , 0x0003,
	2, 0xD633 , 0x008E,

	/**************LV2 Page 0 enable*************/
	2, 0xF000 , 0x0055,
	2, 0xF001 , 0x00AA,
	2, 0xF002 , 0x0052,
	2, 0xF003 , 0x0008,
	2, 0xF004 , 0x0000,

	/*************480x800*********************/
	2, 0xB500 , 0x0050,

	/***************Display control**************/
	2, 0xB100 , 0x00CC,
	2, 0xB101 , 0x0000,

	/***************Source hold time*************/
	2, 0xB600 , 0x0005,

	/**************Gate EQ control***************/
	2, 0xB700 , 0x0070,
	2, 0xB701 , 0x0070,

	/*************Source EQ control (Mode 2)******/
	2, 0xB800 , 0x0001,
	2, 0xB801 , 0x0003,
	2, 0xB802 , 0x0003,
	2, 0xB803 , 0x0003,

	/************Inversion mode  (2-dot)***********/
	2, 0xBC00 , 0x0002,
	2, 0xBC01 , 0x0000,
	2, 0xBC02 , 0x0000,

	/***************Frame rate***************/
	2, 0xBD00 , 0x0001,
	2, 0xBD01 , 0x0084,
	2, 0xBD02 , 0x001C,  /* 0X1C */
	2, 0xBD03 , 0x001C,
	2, 0xBD04 , 0x0000,

	/********Timing control 4H w/ 4-Delayms *******/
	2, 0xC900 , 0x00D0,
	2, 0xC901 , 0x0002,
	2, 0xC902 , 0x0050,
	2, 0xC903 , 0x0050,
	2, 0xC904 , 0x0050,

	2, 0x3600 , 0x0000,
	2, 0x3500 , 0x0000,

	2, 0x3A00 , 0x0005,	/* 70 */

	2, 0X3600 , 0X60,
	2, 0X2A02, 0X03,
	2, 0X2A03, 0X1F,
	2, 0X2B02, 0X01,
	2, 0X2B03, 0XDF,

	1, 0x1100 ,
	DCS_MDELAY_FLAG, 120, /* Delay 120ms */
	1, 0x2900 ,
	DCS_MDELAY_FLAG, 50, /* Delay 50ms */
	1, 0x2C00 ,
};

/* CPU LCD Panel */
static void tcon_dev_config(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	/* device interface, reference to vo_dev_if_t */
	vo_dev->dev_if = INTERFACE;
	/* device specific configuation, reference to vo_if_cfg_t */
	vo_dev->if_cfg.cpu_if.has_vsync_cmd = HAS_VSYNC_CMD;
	vo_dev->if_cfg.cpu_if.has_hsync_cmd = HAS_HSYNC_CMD;
	vo_dev->if_cfg.cpu_if.cmd_align = CMD_ALIGN;

	/* tcon host config */
	vo_dev->tcon_host->timing.is_interlace = IS_INTERLACE;
	/* format, reference to tcon_output_fmt_t */
	vo_dev->tcon_host->format = TCON_FMT;
	/* gamma correction */
	vo_dev->tcon_host->gamma_en = GAMMA_EN;
	vo_dev->tcon_host->gamma_tbl = gamma_tbl;
	/* frame rate control (dither) enable */
	vo_dev->tcon_host->frc_en = FRC_EN;
	/* TODO */
	vo_dev->tcon_host->csc_en = CSC_EN;
	/* TE mode */
	vo_dev->tcon_host->has_te = HAS_TE;
	vo_dev->tcon_host->te_pol = TE_POL;
}

static void doss_set_bl_pwm_io(u32 doss_idx, u32 val)
{
#ifdef CONFIG_ARCH_LOMBO_N7V0_FPGA
	csp_doss_io_set_output(doss_idx, PWM_IO);
	csp_doss_io_set_val(doss_idx, PWM_IO, val);
#else

	u32 rdval;
	u32 reg_addr;

	reg_addr = 0xf5014124;
	rdval = READREG32(reg_addr);
	WRITEREG32(reg_addr, (rdval & 0xfffffff0) | 0x00000002);

	reg_addr = 0xf5014150;
	rdval = READREG32(reg_addr);
	WRITEREG32(reg_addr, (rdval & 0xffffff7f) | (val << 8));
#endif
}

static void doss_set_rest_io(u32 doss_idx, u32 val)
{
#ifdef CONFIG_ARCH_LOMBO_N7V0_FPGA
	csp_doss_io_set_output(doss_idx, RESET_IO);
	csp_doss_io_set_val(doss_idx, RESET_IO, val);
#else

	u32 rdval;
	u32 reg_addr;

	reg_addr = 0xf5014120;
	rdval = READREG32(reg_addr);
	WRITEREG32(reg_addr, (rdval & 0x0fffffff) | 0x20000000);

	reg_addr = 0xf5014150;
	rdval = READREG32(reg_addr);
	WRITEREG32(reg_addr, (rdval & 0xffffff7f) | (val << 7));
#endif
}

static int tcon_cpu_write_reg_array(u32 tcon_index, u32 *reg, u32 len)
{
	u32 i = 0;

	while (i < len) {
		if (DCS_MDELAY_FLAG == reg[i]) {
			mdelay(reg[i + 1]);
			i += reg[i] + 2;
		} else {
			csp_tcon_cpu_write_reg(tcon_index,
					       reg[i + 1],
					       reg + i + 2,
					       reg[i] - 1);
			i += reg[i] + 1;
		}
	}

	return 0;
}

static void tcon_dev_init(vo_device_t *vo_dev)
{
	u32 doss_index = vo_dev->tcon_host->doss_index;
	u32 tcon_index = vo_dev->tcon_host->tcon_index;
	u32 id[16];

	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	/* TODO */
	/* Power up, Init , Back Light En */
	doss_set_rest_io(doss_index, IO_HIGH);
	mdelay(1);
	doss_set_rest_io(doss_index, IO_LOW);
	mdelay(10);
	doss_set_rest_io(doss_index, IO_HIGH);
	mdelay(50);

	/* read id for debug */
	csp_tcon_cpu_read_reg(tcon_index, 0x0400, id, 16);
	DRM_DEBUG_KMS("[%d] tcon dev id[0x0400]:%s\n", __LINE__, (char *)id);
	csp_tcon_cpu_read_reg(tcon_index, 0x0401, id, 16);
	DRM_DEBUG_KMS("[%d] tcon dev id[0x0401]:%s\n", __LINE__, (char *)id);
	csp_tcon_cpu_read_reg(tcon_index, 0x0402, id, 16);
	DRM_DEBUG_KMS("[%d] tcon dev id[0x0402]:%s\n", __LINE__, (char *)id);

	tcon_cpu_write_reg_array(tcon_index, init_code, ARRAY_CNT(init_code));
	/* csp_tcon_cpu_write_gram(tcon_index,
	 *			   gram_data, ARRAY_CNT(gram_data));
	 */

	doss_set_bl_pwm_io(doss_index, IO_LOW);
}

static void tcon_dev_exit(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	/* TODO */
	/* Back Light Dis, Exit, Power down */
}

void tcon_dev_probe(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	vo_dev->dev_ops.dev_init = tcon_dev_init;
	vo_dev->dev_ops.dev_exit = tcon_dev_exit;
	tcon_dev_config(vo_dev);
}

static inline struct panel_simple *to_panel_simple(struct drm_panel *panel)
{
	return container_of(panel, struct panel_simple, base);
}

static int panel_simple_get_fixed_modes(struct panel_simple *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!panel->desc)
		return 0;

	for (i = 0; i < panel->desc->num_timings; i++) {
		const struct display_timing *dt = &panel->desc->timings[i];
		struct videomode vm;

		videomode_from_timing(dt, &vm);
		mode = drm_mode_create(drm);
		if (!mode) {
			dev_err(drm->dev, "failed to add mode %ux%u\n",
				dt->hactive.typ, dt->vactive.typ);
			continue;
		}

		drm_display_mode_from_videomode(&vm, mode);
		drm_mode_set_name(mode);

		drm_mode_probed_add(connector, mode);
		num++;
	}

	for (i = 0; i < panel->desc->num_modes; i++) {
		const struct drm_display_mode *m = &panel->desc->modes[i];

		mode = drm_mode_duplicate(drm, m);
		if (!mode) {
			dev_err(drm->dev, "failed to add mode %ux%u@%u\n",
				m->hdisplay, m->vdisplay, m->vrefresh);
			continue;
		}

		drm_mode_set_name(mode);

		drm_mode_probed_add(connector, mode);
		num++;
	}

	connector->display_info.bpc = panel->desc->bpc;
	connector->display_info.width_mm = panel->desc->size.width;
	connector->display_info.height_mm = panel->desc->size.height;

	/* TODO */
	/* if (panel->desc->bus_format)                                    */
	/*	drm_display_info_set_bus_formats(&connector->display_info, */
	/*				 &panel->desc->bus_format, 1); */

	return num;
}

static int panel_simple_of_get_native_mode(struct panel_simple *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	struct device_node *timings_np;
	int ret;

	timings_np = of_get_child_by_name(panel->dev->of_node,
					  "display-timings");
	if (!timings_np) {
		dev_dbg(panel->dev, "failed to find display-timings node\n");
		return 0;
	}

	of_node_put(timings_np);
	mode = drm_mode_create(drm);
	if (!mode)
		return 0;

	ret = of_get_drm_display_mode(panel->dev->of_node, mode,
				      OF_USE_NATIVE_MODE);
	if (ret) {
		dev_dbg(panel->dev, "failed to find dts display timings\n");
		drm_mode_destroy(drm, mode);
		return 0;
	}

	drm_mode_set_name(mode);
	mode->type |= DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static int panel_simple_disable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (!p->enabled)
		return 0;

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_POWERDOWN;
		backlight_update_status(p->backlight);
	}

	if (p->desc && p->desc->delay.disable)
		msleep(p->desc->delay.disable);

	p->enabled = false;

	return 0;
}

static int panel_simple_unprepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (!p->prepared)
		return 0;

	if (p->enable_gpio)
		gpio_direction_output(p->enable_gpio, 0);

	regulator_disable(p->supply);

	if (p->desc && p->desc->delay.unprepare)
		msleep(p->desc->delay.unprepare);

	p->prepared = false;

	return 0;
}

static int panel_simple_prepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	int err;

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (p->prepared)
		return 0;

	err = regulator_enable(p->supply);
	if (err < 0) {
		dev_err(panel->dev, "failed to enable supply: %d\n", err);
		return err;
	}

	if (p->enable_gpio)
		gpio_direction_output(p->enable_gpio, 1);

	if (p->desc && p->desc->delay.prepare)
		msleep(p->desc->delay.prepare);

	p->prepared = true;

	return 0;
}

static int panel_simple_enable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (p->enabled)
		return 0;

	if (p->desc && p->desc->delay.enable)
		msleep(p->desc->delay.enable);

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(p->backlight);
	}

	p->enabled = true;

	return 0;
}

static int panel_simple_get_modes(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	int num = 0;

	/* add device node plane modes */
	num += panel_simple_of_get_native_mode(p);

	/* add hard-coded panel modes */
	num += panel_simple_get_fixed_modes(p);

	/* probe EDID if a DDC bus is available */
	if (p->ddc) {
		struct edid *edid = drm_get_edid(panel->connector, p->ddc);
		drm_mode_connector_update_edid_property(panel->connector, edid);
		if (edid) {
			num += drm_add_edid_modes(panel->connector, edid);
			kfree(edid);
		}
	}

	return num;
}

static int panel_simple_get_timings(struct drm_panel *panel,
				    unsigned int num_timings,
				    struct display_timing *timings)
{
	struct panel_simple *p = to_panel_simple(panel);
	unsigned int i;

	if (!p->desc)
		return 0;

	if (p->desc->num_timings < num_timings)
		num_timings = p->desc->num_timings;

	if (timings)
		for (i = 0; i < num_timings; i++)
			timings[i] = p->desc->timings[i];

	return p->desc->num_timings;
}

static const struct drm_panel_funcs panel_simple_funcs = {
	.disable = panel_simple_disable,
	.unprepare = panel_simple_unprepare,
	.prepare = panel_simple_prepare,
	.enable = panel_simple_enable,
	.get_modes = panel_simple_get_modes,
	.get_timings = panel_simple_get_timings,
};

static int panel_simple_probe(struct device *dev, const struct panel_desc *desc)
{
/* TODO */
#if 0
	struct device_node *backlight, *ddc;
#endif
	struct panel_simple *panel;
	struct panel_desc *of_desc;
	u32 val;
	int err;

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;

	DRM_DEBUG_KMS("[%d] desc:0x%x\n", __LINE__, (unsigned int)desc);

	if (!desc)
		of_desc = devm_kzalloc(dev, sizeof(*of_desc), GFP_KERNEL);
	else
		of_desc = devm_kmemdup(dev, desc, sizeof(*of_desc), GFP_KERNEL);

	if (!of_property_read_u32(dev->of_node, "bus-format", &val))
		of_desc->bus_format = val;
	if (!of_property_read_u32(dev->of_node, "delay,prepare", &val))
		of_desc->delay.prepare = val;
	if (!of_property_read_u32(dev->of_node, "delay,enable", &val))
		of_desc->delay.enable = val;
	if (!of_property_read_u32(dev->of_node, "delay,disable", &val))
		of_desc->delay.disable = val;
	if (!of_property_read_u32(dev->of_node, "delay,unprepare", &val))
		of_desc->delay.unprepare = val;

	panel->enabled = false;
	panel->prepared = false;
	panel->desc = of_desc;
	panel->dev = dev;

/* TODO */
#if 0
	panel->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(panel->supply))
		return PTR_ERR(panel->supply);

	/* Linux4.4 use case */
	/* panel->enable_gpio = devm_gpiod_get_optional(dev, "enable", 0); */
	err = gpio_request(panel->enable_gpio, "enable-gpios");
	if (err) {
		dev_err(dev, "failed to request GPIO: %d\n", err);
		return err;
	}

	backlight = of_parse_phandle(dev->of_node, "backlight", 0);
	if (backlight) {
		panel->backlight = of_find_backlight_by_node(backlight);
		of_node_put(backlight);

		if (!panel->backlight)
			return -EPROBE_DEFER;
	}

	ddc = of_parse_phandle(dev->of_node, "ddc-i2c-bus", 0);
	if (ddc) {
		panel->ddc = of_find_i2c_adapter_by_node(ddc);
		of_node_put(ddc);

		if (!panel->ddc) {
			err = -EPROBE_DEFER;
			goto free_backlight;
		}
	}
#endif

	drm_panel_init(&panel->base);
	panel->base.dev = dev;
	panel->base.funcs = &panel_simple_funcs;

	err = drm_panel_add(&panel->base);
	if (err < 0)
		goto free_ddc;

	dev_set_drvdata(dev, panel);

	DRM_DEBUG_KMS("[%d] over.\n", __LINE__);

	return 0;

free_ddc:
	if (panel->ddc)
		put_device(&panel->ddc->dev);

/* TODO */
#if 0
free_backlight:
	if (panel->backlight)
		put_device(&panel->backlight->dev);
#endif

	return err;
}

static int panel_simple_remove(struct device *dev)
{
	struct panel_simple *panel = dev_get_drvdata(dev);

	drm_panel_detach(&panel->base);
	drm_panel_remove(&panel->base);

	panel_simple_disable(&panel->base);

	if (panel->ddc)
		put_device(&panel->ddc->dev);

	if (panel->backlight)
		put_device(&panel->backlight->dev);

	return 0;
}

static void panel_simple_shutdown(struct device *dev)
{
	struct panel_simple *panel = dev_get_drvdata(dev);

	panel_simple_disable(&panel->base);
}

/* ax fpga board CPU mode 800x480 LCD test */
static const struct drm_display_mode novatek_nt35510_mode = {
	/* dclk_freq */
	.clock = 27000000,
	/* width */
	.hdisplay = 800,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = 800 + 40,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = 800 + 40 + 10,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = 800 + 40 + 128 + 20,
	/* height */
	.vdisplay = 480,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = 480 + 10,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = 480 + 10 + 2,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = 480 + 15 + 10 + 10,
	.vrefresh = 60,
};

static const struct panel_desc novatek_nt35510 = {
	.modes = &novatek_nt35510_mode,
	.num_modes = 1,
	.bpc = 6,
	.size = {
		.width = 800,
		.height = 480,
	},
};

static const struct of_device_id platform_of_match[] = {
	{
		.compatible = "novatek,nt35510",
		.data = &novatek_nt35510,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, platform_of_match);

static int panel_simple_platform_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;

	id = of_match_node(platform_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return panel_simple_probe(&pdev->dev, id->data);
}

static int panel_simple_platform_remove(struct platform_device *pdev)
{
	return panel_simple_remove(&pdev->dev);
}

static void panel_simple_platform_shutdown(struct platform_device *pdev)
{
	panel_simple_shutdown(&pdev->dev);
}

static struct platform_driver panel_simple_platform_driver = {
	.driver = {
		.name = "panel-simple",
		.of_match_table = platform_of_match,
	},
	.probe = panel_simple_platform_probe,
	.remove = panel_simple_platform_remove,
	.shutdown = panel_simple_platform_shutdown,
};

static int __init panel_simple_init(void)
{
	int err;

	err = platform_driver_register(&panel_simple_platform_driver);
	if (err < 0)
		return err;

	return 0;
}
arch_initcall(panel_simple_init);

static void __exit panel_simple_exit(void)
{
	platform_driver_unregister(&panel_simple_platform_driver);
}
module_exit(panel_simple_exit);

MODULE_AUTHOR("Thierry Reding <treding@nvidia.com>");
MODULE_DESCRIPTION("DRM Driver for Simple Panels");
MODULE_LICENSE("GPL and additional rights");

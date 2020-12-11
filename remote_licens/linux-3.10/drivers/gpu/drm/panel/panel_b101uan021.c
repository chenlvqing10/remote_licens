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

#include "panel-simple.h"
#include "lombo_panel_config.h"

#define B101UAN021_HBP		(80)
#define B101UAN021_HFP		(50)
#define B101UAN021_HSW		(50)
#define B101UAN021_HACT		(1920)
#define B101UAN021_VBP		(6)
#define B101UAN021_VFP		(4)
#define B101UAN021_VSW		(4)
#define B101UAN021_VACT		(1200)
#define B101UAN021_DCLK		(148500000)

#define GAMMA_EN		(false)

static u32 gamma_tbl[256] = {
/* red	 power = 2.2 */
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

static struct lombo_vo_dev_config b101uan021_vo = {
	/* device interface, reference to vo_dev_if_t */
	.dev_if = VO_DEV_LVDS,
	/* device specific configuation, reference to vo_if_cfg_t */
	.lvds_if.is_dual_link = true,
	.lvds_if.is_ctrl_invert = false, /* TODO */
	.lvds_if.mode = TCON_LVDS_VESA,
	.lvds_if.bpp = TCON_COLOR_24BPP,

	/* tcon host config */
	/* timing, reference to tcon_timings_t */
	.timing.is_interlace = 0,
	.timing.field1.vt = B101UAN021_VSW + B101UAN021_VBP
			  + B101UAN021_VACT + B101UAN021_VFP,
	.timing.field1.vact = B101UAN021_VACT,
	.timing.field1.vfp = B101UAN021_VFP,
	.timing.field1.vsw = B101UAN021_VSW,
	.timing.hline.ht = B101UAN021_HSW + B101UAN021_HBP
			 + B101UAN021_HACT + B101UAN021_HFP,
	.timing.hline.hact = B101UAN021_HACT,
	.timing.hline.hfp = B101UAN021_HFP,
	.timing.hline.hsw = B101UAN021_HSW,
	.timing.dclk_freq = B101UAN021_DCLK,
	/* resolution in pixel */
	.timing.width = B101UAN021_HACT,
	.timing.height = B101UAN021_VACT,
	/* format, reference to tcon_output_fmt_t */
	.format = TCON_FMT_RGB888,
	.gamma_en = GAMMA_EN,
	.gamma_tbl = gamma_tbl,
};

static struct drm_display_mode b101uan021_mode = {
	/* dclk_freq */
	.clock = B101UAN021_DCLK / 1000,
	/* width */
	.hdisplay = B101UAN021_HACT,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = B101UAN021_HACT + B101UAN021_HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = B101UAN021_HACT + B101UAN021_HFP + B101UAN021_HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = B101UAN021_HACT + B101UAN021_HFP +
			B101UAN021_HSW + B101UAN021_HBP,
	/* height */
	.vdisplay = B101UAN021_VACT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = B101UAN021_VACT + B101UAN021_VFP,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = B101UAN021_VACT + B101UAN021_VFP + B101UAN021_VSW,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = B101UAN021_VACT + B101UAN021_VFP +
			B101UAN021_VSW + B101UAN021_VBP,
	.vrefresh = 60,

	.private_size = sizeof(b101uan021_vo),
	.private = (int *)&b101uan021_vo,
};

static struct panel_delay b101uan021_delay = {
	.prepare = 100,
	.unprepare = 0,
	.enable = 0,
	.disable = 0,
	.power_on = 50,
	.reset = 100,
};
const struct panel_simple_desc b101uan021_desc = {
	.modes = &b101uan021_mode,
	.num_modes = 1,
	.delay = &b101uan021_delay,
	.size = {
		.width = 150,
		.height = 300,
	},
};

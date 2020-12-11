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

#define WKS70002_HBP		(160)
#define WKS70002_HFP		(100)
#define WKS70002_HSW		(100)
#define WKS70002_HACT		(1024)
#define WKS70002_VBP		(23)
#define WKS70002_VFP		(12)
#define WKS70002_VSW		(12)
#define WKS70002_VACT		(600)
#define WKS70002_DCLK		(54000000)

#define GAMMA_EN		(TRUE)

static u32 gamma_tbl[256] = {
/* red   power = 2.2 */
/* green power = 1/2.2 */
/* blue  power = 1.4 */
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x010101,
0x020202, 0x020202, 0x020202, 0x020202, 0x020202, 0x020202, 0x020202, 0x030303,
0x030303, 0x030303, 0x030303, 0x030303, 0x040404, 0x040404, 0x040404, 0x040404,
0x050505, 0x050505, 0x060606, 0x060606, 0x070707, 0x070707, 0x070707, 0x070707,
0x080808, 0x080808, 0x080808, 0x090909, 0x090909, 0x090909, 0x0A0A0A, 0x0A0A0A,
0x0B0B0B, 0x0B0B0B, 0x0B0B0B, 0x0C0C0C, 0x0C0C0C, 0x0D0D0D, 0x0E0E0E, 0x0E0E0E,
0x0f0f0f, 0x0f0f0f, 0x101010, 0x101010, 0x111111, 0x111111, 0x121212, 0x121212,
0x131313, 0x131313, 0x141414, 0x141414, 0x151515, 0x151515, 0x161616, 0x161616,
0x171717, 0x181818, 0x191919, 0x1A1A1A, 0x1A1A1A, 0x1B1B1B, 0x1B1B1B, 0x1C1C1C,
0x1D1D1D, 0x1D1D1D, 0x1E1E1E, 0x1F1F1F, 0x1F1F1F, 0x202020, 0x212121, 0x212121,
0x222222, 0x232323, 0x232323, 0x242424, 0x252525, 0x262626, 0x272727, 0x282828,
0x292929, 0x292929, 0x2A2A2A, 0x2B2B2B, 0x2C2C2C, 0x2D2D2D, 0x2D2D2D, 0x2E2E2E,
0x2F2F2F, 0x303030, 0x313131, 0x313131, 0x323232, 0x333333, 0x343434, 0x353535,
0x363636, 0x373737, 0x373737, 0x393939, 0x3A3A3A, 0x3B3B3B, 0x3C3C3C, 0x3D3D3D,
0x3E3E3E, 0x3F3F3F, 0x404040, 0x414141, 0x424242, 0x434343, 0x444444, 0x454545,
0x464646, 0x474747, 0x484848, 0x494949, 0x4A4A4A, 0x4B4B4B, 0x4C4C4C, 0x4D4D4D,
0x4E4E4E, 0x4F4F4F, 0x505050, 0x515151, 0x525252, 0x535353, 0x555555, 0x565656,
0x575757, 0x585858, 0x595959, 0x5A5A5A, 0x5B5B5B, 0x5D5D5D, 0x5E5E5E, 0x5F5F5F,
0x606060, 0x616161, 0x636363, 0x646464, 0x656565, 0x666666, 0x676767, 0x696969,
0x6A6A6A, 0x6B6B6B, 0x6C6C6C, 0x6E6E6E, 0x6F6F6F, 0x707070, 0x727272, 0x737373,
0x747474, 0x767676, 0x777777, 0x787878, 0x7A7A7A, 0x7B7B7B, 0x7C7C7C, 0x7E7E7E,
0x7F7F7F, 0x808080, 0x828282, 0x838383, 0x858585, 0x868686, 0x878787, 0x898989,
0x8A8A8A, 0x8C8C8C, 0x8D8D8D, 0x8F8F8F, 0x909090, 0x929292, 0x939393, 0x959595,
0x969696, 0x979797, 0x989898, 0x9A9A9A, 0x9B9B9B, 0x9D9D9D, 0x9E9E9E, 0xA0A0A0,
0xA1A1A1, 0xA3A3A3, 0xA5A5A5, 0xA6A6A6, 0xA8A8A8, 0xA9A9A9, 0xABABAB, 0xADADAD,
0xAEAEAE, 0xAFAFAF, 0xB0B0B0, 0xB2B2B2, 0xB4B4B4, 0xB5B5B5, 0xB7B7B7, 0xB9B9B9,
0xBABABA, 0xBCBCBC, 0xBEBEBE, 0xC0C0C0, 0xC1C1C1, 0xC2C2C2, 0xC4C4C4, 0xC6C6C6,
0xC7C7C7, 0xC9C9C9, 0xCBCBCB, 0xCDCDCD, 0xCECECE, 0xD0D0D0, 0xD2D2D2, 0xD3D3D3,
0xD5D5D5, 0xD6D6D6, 0xD8D8D8, 0xDADADA, 0xDCDCDC, 0xDEDEDE, 0xE0E0E0, 0xE1E1E1,
0xE2E2E2, 0xE4E4E4, 0xE6E6E6, 0xE8E8E8, 0xEAEAEA, 0xECECEC, 0xEEEEEE, 0xF0F0F0,
0xF1F1F1, 0xF3F3F3, 0xF5F5F5, 0xF7F7F7, 0xF9F9F9, 0xFBFBFB, 0xFDFDFD, 0xFFFFFF,
};

static struct lombo_vo_dev_config ek79003a_wks70002_vo = {
	/* device interface, reference to vo_dev_if_t */
	.dev_if = VO_DEV_LVDS,
	/* device specific configuation, reference to vo_if_cfg_t */
	.lvds_if.is_dual_link = false,
	.lvds_if.is_ctrl_invert = false, /* TODO */
	.lvds_if.mode = TCON_LVDS_JEIDA,
	.lvds_if.bpp = TCON_COLOR_24BPP,

	/* tcon host config */
	/* timing, reference to tcon_timings_t */
	.timing.is_interlace = 0,
	.timing.field1.vt = WKS70002_VSW + WKS70002_VBP
			  + WKS70002_VACT + WKS70002_VFP,
	.timing.field1.vact = WKS70002_VACT,
	.timing.field1.vfp = WKS70002_VFP,
	.timing.field1.vsw = WKS70002_VSW,
	.timing.hline.ht = WKS70002_HSW + WKS70002_HBP
			 + WKS70002_HACT + WKS70002_HFP,
	.timing.hline.hact = WKS70002_HACT,
	.timing.hline.hfp = WKS70002_HFP,
	.timing.hline.hsw = WKS70002_HSW,
	.timing.dclk_freq = WKS70002_DCLK,
	/* resolution in pixel */
	.timing.width = WKS70002_HACT,
	.timing.height = WKS70002_VACT,
	/* format, reference to tcon_output_fmt_t */
	.format = TCON_FMT_RGB888,
	.gamma_en = GAMMA_EN,
	.gamma_tbl = gamma_tbl,
};

static struct drm_display_mode ek79003a_wks70002_mode = {
	/* dclk_freq */
	.clock = WKS70002_DCLK / 1000,
	/* width */
	.hdisplay = WKS70002_HACT,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = WKS70002_HACT + WKS70002_HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = WKS70002_HACT + WKS70002_HFP + WKS70002_HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = WKS70002_HACT + WKS70002_HFP + WKS70002_HSW + WKS70002_HBP,
	/* height */
	.vdisplay = WKS70002_VACT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = WKS70002_VACT + WKS70002_VFP,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = WKS70002_VACT + WKS70002_VFP + WKS70002_VSW,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = WKS70002_VACT + WKS70002_VFP + WKS70002_VSW + WKS70002_VBP,
	.vrefresh = 60,

	.private_size = sizeof(ek79003a_wks70002_vo),
	.private = (int *)&ek79003a_wks70002_vo,
};

static struct panel_delay ek79003a_wks70002_delay = {
	.prepare = 100,
	.unprepare = 0,
	.enable = 0,
	.disable = 0,
	.power_on = 50,
	.reset = 100,
};
const struct panel_simple_desc ek79003a_wks70002_desc = {
	.modes = &ek79003a_wks70002_mode,
	.num_modes = 1,
	.delay = &ek79003a_wks70002_delay,
	.size = {
		.width = 150,
		.height = 100,
	},
};


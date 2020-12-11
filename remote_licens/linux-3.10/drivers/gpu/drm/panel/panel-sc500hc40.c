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

#include <drm/drmP.h>
#include <drm/drm_panel.h>
#include "panel-simple.h"
#include "lombo_panel_config.h"

/* timing for tcon host */
#define WIDTH	(720)
#define HEIGHT	(1280)
#define HBP	(22)
#define HFP	(16)
#define HSW	(6)
#define HACT	(WIDTH)
#define VBP_F1	(10)
#define VFP_F1	(14)
#define VSW_F1	(2)
#define VACT_F1	(HEIGHT)
#define VT_F1	(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define VBP_F2	(0)
#define VFP_F2	(0)
#define VSW_F2	(0)
#define VACT_F2	(0)
#define VT_F2	(VBP_F2 + VFP_F2 + VSW_F2 + VACT_F2)
#define DCLK	(59400000)

#define DCS_MDELAY_FLAG (0)

static u8 sc500hc40_init_code[] = {
	2, 0x11, 0x00,
	DCS_MDELAY_FLAG, 10,
	2, 0x11, 0x00,
	DCS_MDELAY_FLAG, 10,
	2, 0x11, 0x00,
	DCS_MDELAY_FLAG, 10,
	4, 0xB0, 0x00, 0x32, 0x05,
	6, 0xB0, 0x0A, 0x00, 0x16, 0x00, 0x2B,
	4, 0xB0, 0x1E, 0xB0, 0x00,
	3, 0xB3, 0x00, 0xBA,
	7, 0xB3, 0x02, 0xDC, 0x00, 0xC4, 0x04, 0x0E,
	3, 0xB3, 0x0F, 0x8F,
	3, 0xB4, 0x02, 0x18,
	4, 0xB8, 0x00, 0x40, 0x70,
	3, 0xB8, 0x12, 0xD0,
	5, 0xB8, 0x17, 0x51, 0x15, 0x20,
	5, 0xB8, 0x25, 0x66, 0xA0, 0x06,
	3, 0xB9, 0x0B, 0x0A,
	4, 0xB9, 0x13, 0x90, 0x90,
	5, 0xBA, 0x02, 0x0A, 0x00, 0x0B,
	15, 0xBB, 0x01, 0xFC, 0xFF, 0xFF, 0xF8, 0x2F, 0xF0,
		0x3F, 0xF1, 0x4F, 0xFF, 0xFF, 0xBE, 0x66,
	15, 0xBB, 0x0F, 0xFC, 0xFF, 0xFF, 0xF8, 0x2F,
		0xF0, 0x3F, 0xF1, 0x4F, 0xFF, 0xFF, 0xBE, 0x66,
	11, 0xBC, 0x0C, 0x17, 0x06, 0x1A, 0x17, 0x06, 0x02, 0x0b, 0x02, 0x04,
	8, 0xBC, 0x26, 0x44, 0x44, 0x33, 0x60, 0x33, 0x40,
	8, 0xBC, 0x32, 0x44, 0x44, 0x33, 0x60, 0x33, 0x40,
	3, 0xBD, 0x04, 0x10,
	4, 0xBD, 0x09, 0x51, 0x10,
	3, 0xBD, 0x18, 0x00,
	4, 0xBD, 0x1D, 0x51, 0x00,
	8, 0xBD, 0x28, 0x0A, 0x10, 0xA1, 0x0A, 0x20, 0xA2,
	8, 0xBD, 0x31, 0x0A, 0x10, 0xA1, 0x0A, 0x20, 0xA2,
	3, 0xC0, 0x01, 0xCC,
	5, 0xC0, 0x05, 0x17, 0x5F, 0x20,
	3, 0xC1, 0x08, 0xFF,
	4, 0xC1, 0x0D, 0x68, 0x68,
	23, 0xC3, 0x00, 0x00, 0x12, 0x20, 0x29, 0x37, 0x41, 0x4F,
		0x59, 0x6A, 0x76, 0x82, 0x8D, 0x98, 0xA3, 0xAB, 0xB5,
		0xC0, 0x4D, 0x57, 0x65, 0x7F,
	23, 0xC3, 0x15, 0x00, 0x12, 0x20, 0x29, 0x37, 0x41, 0x4F,
		0x59, 0x6A, 0x76, 0x82, 0x8D, 0x98, 0xA3, 0xAB, 0xB5,
		0xC0, 0x4D, 0x57, 0x65, 0x7F,
	3, 0xE1, 0x00, 0x01,
	4, 0xE1, 0x0E, 0x01, 0x1F,
	4, 0xE1, 0x12, 0x80, 0xFF,

	3, 0xB1, 0x01, 0x28,
	3, 0xB1, 0x03, 0x1C,
	3, 0xB1, 0x02, 0x1D,
	3, 0xB1, 0x05, 0x54,
	3, 0xC2, 0x00, 0xA0,
	3, 0xB3, 0x04, 0xE6,

	3, 0xE1, 0x11, 0x01,
	3, 0xB2, 0x00, 0x1B,
	3, 0xB3, 0x04, 0xE4,
	3, 0xC1, 0x03, 0x05,
	4, 0xE1, 0x0A, 0x80, 0x00,

	3, 0xB4, 0x02, 0x03,
	3, 0xB4, 0x13, 0x00,
	3, 0xB4, 0x14, 0x02,
	3, 0xB4, 0x15, 0x01,
	3, 0xB4, 0x16, 0x00,
	3, 0xB4, 0x17, 0x02,
	3, 0xB4, 0x18, 0x01,
	3, 0xB4, 0x19, 0x00,
	3, 0xB4, 0x1A, 0x02,
	3, 0xB4, 0x1B, 0x01,
	3, 0xB0, 0x20, 0x00,
	3, 0xC1, 0x02, 0xAA,

	3, 0xEC, 0x02, 0x9F,
	3, 0xEC, 0x06, 0x68,

	3, 0xE2, 0x19, 0x01,
	3, 0xB0, 0x1F, 0x83,
	3, 0xE2, 0x1A, 0x06,
	4, 0xE2, 0x1E, 0x01, 0xC0,
	4, 0xB0, 0x04, 0x01, 0xC0,

	3, 0xEC, 0x00, 0xAA,
	3, 0xEC, 0x09, 0xA5,

	3, 0xC0, 0x03, 0x0F,

	3, 0xB0, 0x1C, 0x18,
	3, 0xB0, 0x1D, 0x16,
	3, 0xB3, 0x0F, 0x9F,

	3, 0xF0, 0x00, 0x67,
	3, 0xF0, 0x02, 0x07,

	1, 0x11,
	DCS_MDELAY_FLAG, 120,
	1, 0x29,
	DCS_MDELAY_FLAG, 10,
};

static ssize_t sc500hc40_mipi_dsi_dcs_write_array(
	struct panel_simple_host *host)
{
	u32 i = 0;
	u32 len = (sizeof(sc500hc40_init_code) /
			sizeof((sc500hc40_init_code)[0]));
	u8 *ini_code = &sc500hc40_init_code[0];

	if (!ini_code)
		return 0;

	DRM_DEBUG_KMS("panel_simple_host: %p\n", host->dsi);
	while (i < len) {
		if (ini_code[i] == DCS_MDELAY_FLAG) {
			mdelay(ini_code[i + 1]);
			i += 2;
		} else {
			mipi_dsi_dcs_write_buffer(host->dsi,
				(const void *)&ini_code[i + 1], ini_code[i]);
			i += ini_code[i] + 1;
		}
	}

	return i;
}

static struct lombo_vo_dev_config sc500hc40_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.index = 0,
		.is_dual = false,
		.is_bta = false,
		.lane_num = 4,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_PULSE,
		.is_bitrate_fixed = false,
		.bit_clk = 999000000,
	},

	.timing.is_interlace = false,
	.timing.field1.vt = VT_F1,
	.timing.field1.vact = VACT_F1,
	.timing.field1.vfp = VFP_F1,
	.timing.field1.vsw = VSW_F1,
	.timing.field2.vt = VT_F2,
	.timing.field2.vact = VACT_F2,
	.timing.field2.vfp = VFP_F2,
	.timing.field2.vsw = VSW_F2,
	.timing.hline.ht = HSW + HBP + HACT + HFP,
	.timing.hline.hact = HACT,
	.timing.hline.hfp = HFP,
	.timing.hline.hsw = HSW,
	.timing.width = WIDTH,
	.timing.height = HEIGHT,
	.timing.dclk_freq = DCLK,
	.format = TCON_FMT_RGB888,
};

/* mipi dsi 480x854 LCD test */
static struct drm_display_mode sc500hc40_mode = {
	/* dclk_freq */
	.clock = DCLK / 1000,
	/* width */
	.hdisplay = WIDTH,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = WIDTH + HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = WIDTH + HFP + HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = WIDTH + HFP + HSW + HBP,
	/* height */
	.vdisplay = HEIGHT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = HEIGHT + VFP_F1,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = HEIGHT + VFP_F1 + VSW_F1,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = HEIGHT + VFP_F1 + VSW_F1 + VBP_F1,
	.vrefresh = 60,

	.private_size = sizeof(sc500hc40_vo),
	.private = (int *)&sc500hc40_vo,
};

static struct panel_delay sc500hc40_delay = {
	.prepare = 120,
	.unprepare = 30,
	.enable = 0,
	.disable = 50,
	.power_on = 0,
	.reset = 50,
};

const struct panel_simple_desc sc500hc40_desc = {
	.modes = &sc500hc40_mode,
	.num_modes = 1,
	.bpc = 6,
	.init_panel = PANEL_SIMPLE_DSI,
	.delay = &sc500hc40_delay,
	.size = {
		.width = 69,
		.height = 123,
	},
	.ops.transfer = sc500hc40_mipi_dsi_dcs_write_array,
};


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
#define WIDTH	(480)
#define HEIGHT	(854)
#define HBP	(10)
#define HFP	(10)
#define HSW	(6)
#define HACT	(WIDTH)
#define VBP_F1	(10)
#define VFP_F1	(18)
#define VSW_F1	(6)
#define VACT_F1	(HEIGHT)
#define VT_F1	(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define VBP_F2	(0)
#define VFP_F2	(0)
#define VSW_F2	(0)
#define VACT_F2	(0)
#define VT_F2	(VBP_F2 + VFP_F2 + VSW_F2 + VACT_F2)
#define DCLK	(27000000)

#define DCS_MDELAY_FLAG (0)

static u8 xjs50fwi4015_init_code[] = {
	3 ,
	0xF0 ,
	0x5A ,
	0x5A ,

	3 ,
	0xF1 ,
	0xA5 ,
	0xA5 ,

	12 ,
	0xB4 ,
	0x03 ,
	0x03 ,
	0x03 ,
	0x06 ,
	0x04 ,
	0x0C ,
	0x0E ,
	0x10 ,
	0x12 ,
	0x08 ,
	0x0A ,

	12 ,
	0xB3 ,
	0x03 ,
	0x03 ,
	0x03 ,
	0x07 ,
	0x05 ,
	0x0D ,
	0x0F ,
	0x11 ,
	0x13 ,
	0x09 ,
	0x0B ,

	13 ,
	0xB0 ,
	0x54 ,
	0x32 ,
	0x23 ,
	0x45 ,
	0x44 ,
	0x44 ,
	0x44 ,
	0x44 ,
	0x60 ,
	0x00 ,
	0x60 ,
	0x1C ,

	9 ,
	0xB1 ,
	0x32 ,
	0x84 ,
	0x02 ,
	0x83 ,
	0x12 ,
	0x00 ,
	0x70 ,
	0x1C ,

	6 ,
	0xB2 ,
	0x73 ,
	0x09 ,
	0x08 ,
	0x89 ,
	0x48 ,

	4 ,
	0xB6 ,
	0x70 ,
	0x70 ,
	0x05 ,

	2 ,
	0xB7 ,
	0x41 ,

	7 ,
	0xC1 ,
	0x10 ,
	0x0E ,
	0x02 ,
	0x14 ,
	0x14 ,
	0x08 ,

	2 ,
	0xC2 ,
	0x82 ,

	3 ,
	0xC3 ,
	0x22 ,
	0x11 ,

	3 ,/* ÍÆºÚ */
	0xD0 ,
	0x07 ,
	0xFF ,

	5 ,/* ESD */
	0xD2 ,
	0x63 ,
	0x0B ,
	0x08 ,
	0x88 ,

	8 ,
	0xC6 ,
	0x08 ,
	0x15 ,
	0xFF ,
	0x10 ,
	0x16 ,
	0x80 ,
	0x60 ,

	2 ,
	0xC7 ,
	0x04 ,

	3 ,
	0xC9 ,
	0x37 ,
	0x93 ,

	39 ,/* 2.0 */
	0xC8 ,
	0x7D ,
	0x69 ,
	0x5A ,
	0x4D ,
	0x49 ,
	0x3A ,
	0x3E ,
	0x2A ,
	0x45 ,
	0x47 ,
	0x48 ,
	0x69 ,
	0x57 ,
	0x5F ,
	0x52 ,
	0x4D ,
	0x40 ,
	0x2C ,
	0x07 ,
	0x7D ,
	0x69 ,
	0x5A ,
	0x4D ,
	0x49 ,
	0x3A ,
	0x3E ,
	0x2A ,
	0x45 ,
	0x47 ,
	0x48 ,
	0x69 ,
	0x57 ,
	0x5F ,
	0x52 ,
	0x4D ,
	0x40 ,
	0x2C ,
	0x07 ,

	2 ,
	0x11 ,
	0x00 ,

	DCS_MDELAY_FLAG, 120,

	2 ,
	0x29 ,
	0x00 ,

	DCS_MDELAY_FLAG, 20,
};

static ssize_t xjs50fwi4015_mipi_dsi_dcs_write_array(
	struct panel_simple_host *host)
{
	u32 i = 0;
	u32 len = (sizeof(xjs50fwi4015_init_code) /
			sizeof((xjs50fwi4015_init_code)[0]));
	u8 *ini_code = &xjs50fwi4015_init_code[0];

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

static struct lombo_vo_dev_config xjs50fwi4015_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.index = 0,
		.is_dual = false,
		.is_bta = false,
		.lane_num = 2,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_PULSE,
		.is_bitrate_fixed = false,
		.bit_clk = 549000000,
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
static struct drm_display_mode xjs50fwi4015_mode = {
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

	.private_size = sizeof(xjs50fwi4015_vo),
	.private = (int *)&xjs50fwi4015_vo,
};

static struct panel_delay xjs50fwi4015_delay = {
	.prepare = 150,
	.unprepare = 30,
	.enable = 0,
	.disable = 50,
	.power_on = 0,
	.reset = 50,
};

const struct panel_simple_desc xjs50fwi4015_desc = {
	.modes = &xjs50fwi4015_mode,
	.num_modes = 1,
	.bpc = 6,
	.init_panel = PANEL_SIMPLE_DSI,
	.delay = &xjs50fwi4015_delay,
	.size = {
		.width = 69,
		.height = 123,
	},
	.ops.transfer = xjs50fwi4015_mipi_dsi_dcs_write_array,
};


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
#define WIDTH	(800)
#define HEIGHT	(1280)
#define HBP	(20)
#define HFP	(20)
#define HSW	(20)
#define HACT	(WIDTH)
#define VBP_F1	(8)
#define VFP_F1	(30)
#define VSW_F1	(4)
#define VACT_F1	(HEIGHT)
#define VT_F1	(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define VBP_F2	(0)
#define VFP_F2	(0)
#define VSW_F2	(0)
#define VACT_F2	(0)
#define VT_F2	(VBP_F2 + VFP_F2 + VSW_F2 + VACT_F2)
#define DCLK	(59400000)

#define DCS_MDELAY_FLAG (0)

static u8 cjd9365_init_code[] = {
	/* Page0 */
	2, 0xE0, 0x00,

	2, 0xE1, 0x93,
	2, 0xE2, 0x65,
	2, 0xE3, 0xF8,
	2, 0x80, 0x03,
	/* Page1 */
	2, 0xE0, 0x01,

	/*Set VCOM */
	2, 0x00, 0x00,
	2, 0x01, 0x6A,
	2, 0x03, 0x00,
	2, 0x04, 0x6C,

	2, 0x0C, 0x74,

	2, 0x17, 0x00,
	2, 0x18, 0xD7,
	2, 0x19, 0x01,
	2, 0x1A, 0x00,
	2, 0x1B, 0xD7,
	2, 0x1C, 0x01,

	2, 0x1F, 0x70,
	2, 0x20, 0x2D,
	2, 0x21, 0x2D,
	2, 0x22, 0x7E,

	2, 0x35, 0x28,

	2, 0x37, 0x19,

	2, 0x38, 0x05,
	2, 0x39, 0x00,
	2, 0x3A, 0x01,
	2, 0x3C, 0x7C,
	2, 0x3D, 0xFF,
	2, 0x3E, 0xFF,
	2, 0x3F, 0x7F,

	2, 0x40, 0x06,
	2, 0x41, 0xA0,
	2, 0x43, 0x1E,
	2, 0x44, 0x0B,
	2, 0x45, 0x28,

	/* power voltage  */

	2, 0x55, 0x01,
	2, 0x57, 0xA9,

	2, 0x59, 0x0A,
	2, 0x5A, 0x2E,
	2, 0x5B, 0x1A,
	2, 0x5C, 0x15,

	/* Gamma  */
	2, 0x5D, 0x7F,
	2, 0x5E, 0x57,
	2, 0x5F, 0x43,
	2, 0x60, 0x36,
	2, 0x61, 0x31,
	2, 0x62, 0x22,
	2, 0x63, 0x27,
	2, 0x64, 0x11,
	2, 0x65, 0x2A,
	2, 0x66, 0x2A,
	2, 0x67, 0x2A,
	2, 0x68, 0x49,
	2, 0x69, 0x38,
	2, 0x6A, 0x40,
	2, 0x6B, 0x32,
	2, 0x6C, 0x2E,
	2, 0x6D, 0x21,
	2, 0x6E, 0x10,
	2, 0x6F, 0x02,
	2, 0x70, 0x7F,
	2, 0x71, 0x57,
	2, 0x72, 0x43,
	2, 0x73, 0x36,
	2, 0x74, 0x31,
	2, 0x75, 0x22,
	2, 0x76, 0x27,
	2, 0x77, 0x11,
	2, 0x78, 0x2A,
	2, 0x79, 0x2A,
	2, 0x7A, 0x2A,
	2, 0x7B, 0x49,
	2, 0x7C, 0x38,
	2, 0x7D, 0x40,
	2, 0x7E, 0x32,
	2, 0x7F, 0x2E,
	2, 0x80, 0x21,
	2, 0x81, 0x10,
	2, 0x82, 0x02,

	/* Page2, for GIP */
	2, 0xE0, 0x02,

	2, 0x00, 0x52,
	2, 0x01, 0x55,
	2, 0x02, 0x55,
	2, 0x03, 0x50,
	2, 0x04, 0x77,
	2, 0x05, 0x57,
	2, 0x06, 0x55,
	2, 0x07, 0x4E,
	2, 0x08, 0x4C,
	2, 0x09, 0x5F,
	2, 0x0A, 0x4A,
	2, 0x0B, 0x48,
	2, 0x0C, 0x55,
	2, 0x0D, 0x46,
	2, 0x0E, 0x44,
	2, 0x0F, 0x40,
	2, 0x10, 0x55,
	2, 0x11, 0x55,
	2, 0x12, 0x55,
	2, 0x13, 0x55,
	2, 0x14, 0x55,
	2, 0x15, 0x55,

	/* GIP_R Pin mapping */
	2, 0x16, 0x53,
	2, 0x17, 0x55,
	2, 0x18, 0x55,
	2, 0x19, 0x51,
	2, 0x1A, 0x77,
	2, 0x1B, 0x57,
	2, 0x1C, 0x55,
	2, 0x1D, 0x4F,
	2, 0x1E, 0x4D,
	2, 0x1F, 0x5F,
	2, 0x20, 0x4B,
	2, 0x21, 0x49,
	2, 0x22, 0x55,
	2, 0x23, 0x47,
	2, 0x24, 0x45,
	2, 0x25, 0x41,
	2, 0x26, 0x55,
	2, 0x27, 0x55,
	2, 0x28, 0x55,
	2, 0x29, 0x55,
	2, 0x2A, 0x55,
	2, 0x2B, 0x55,

	/* GIP_L_GS Pin mapping */
	2, 0x2C, 0x13,
	2, 0x2D, 0x15,
	2, 0x2E, 0x15,
	2, 0x2F, 0x01,
	2, 0x30, 0x37,
	2, 0x31, 0x17,
	2, 0x32, 0x15,
	2, 0x33, 0x0D,
	2, 0x34, 0x0F,
	2, 0x35, 0x15,
	2, 0x36, 0x05,
	2, 0x37, 0x07,
	2, 0x38, 0x15,
	2, 0x39, 0x09,
	2, 0x3A, 0x0B,
	2, 0x3B, 0x11,
	2, 0x3C, 0x15,
	2, 0x3D, 0x15,
	2, 0x3E, 0x15,
	2, 0x3F, 0x15,
	2, 0x40, 0x15,
	2, 0x41, 0x15,

	/* GIP_R_GS Pin mapping  */
	2, 0x42, 0x12,
	2, 0x43, 0x15,
	2, 0x44, 0x15,
	2, 0x45, 0x00,
	2, 0x46, 0x37,
	2, 0x47, 0x17,
	2, 0x48, 0x15,
	2, 0x49, 0x0C,
	2, 0x4A, 0x0E,
	2, 0x4B, 0x15,
	2, 0x4C, 0x04,
	2, 0x4D, 0x06,
	2, 0x4E, 0x15,
	2, 0x4F, 0x08,
	2, 0x50, 0x0A,
	2, 0x51, 0x10,
	2, 0x52, 0x15,
	2, 0x53, 0x15,
	2, 0x54, 0x15,
	2, 0x55, 0x15,
	2, 0x56, 0x15,
	2, 0x57, 0x15,

	/* GIP Timing  */
	2, 0x58, 0x40,
	2, 0x5B, 0x10,
	2, 0x5C, 0x06,
	2, 0x5D, 0x40,
	2, 0x5E, 0x00,
	2, 0x5F, 0x00,
	2, 0x60, 0x40,
	2, 0x61, 0x03,
	2, 0x62, 0x04,
	2, 0x63, 0x6C,
	2, 0x64, 0x6C,
	2, 0x65, 0x75,
	2, 0x66, 0x08,
	2, 0x67, 0xB4,
	2, 0x68, 0x08,
	2, 0x69, 0x6C,
	2, 0x6A, 0x6C,
	2, 0x6B, 0x0C,
	2, 0x6D, 0x00,
	2, 0x6E, 0x00,
	2, 0x6F, 0x88,
	2, 0x75, 0xBB,
	2, 0x76, 0x00,
	2, 0x77, 0x05,
	2, 0x78, 0x2A,
	/* Page4 */
	2, 0xE0, 0x04,
	2, 0x09, 0x11,
	2, 0x0E, 0x48,
	2, 0x2B, 0x2B,
	2, 0x2D, 0x03,
	2, 0x2E, 0x44,

	/* Page5 */
	2, 0xE0, 0x05,
	2, 0x12, 0x72,

	/* Page0 */
	2, 0xE0, 0x00,
	2, 0xE6, 0x02,
	2, 0xE7, 0x0C,

	1, 0x11,
	DCS_MDELAY_FLAG, 120,
	1, 0x29,
	DCS_MDELAY_FLAG, 5,
};

static ssize_t cjd9365_mipi_dsi_dcs_write_array(
	struct panel_simple_host *host)
{
	u32 i = 0;
	u32 len = (sizeof(cjd9365_init_code) /
			sizeof((cjd9365_init_code)[0]));
	u8 *ini_code = &cjd9365_init_code[0];

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

static struct lombo_vo_dev_config cjd9365_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.index = 0,
		.is_dual = false,
		.is_bta = false,
		.lane_num = 4,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_EVENT,
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
static struct drm_display_mode cjd9365_mode = {
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

	.private_size = sizeof(cjd9365_vo),
	.private = (int *)&cjd9365_vo,
};

static struct panel_delay cjd9365_delay = {
	.prepare = 120,
	.unprepare = 30,
	.enable = 0,
	.disable = 50,
	.power_on = 0,
	.reset = 100,
};

const struct panel_simple_desc cjd9365_desc = {
	.modes = &cjd9365_mode,
	.num_modes = 1,
	.bpc = 6,
	.init_panel = PANEL_SIMPLE_DSI,
	.delay = &cjd9365_delay,
	.size = {
		.width = 69,
		.height = 123,
	},
	.ops.transfer = cjd9365_mipi_dsi_dcs_write_array,
};


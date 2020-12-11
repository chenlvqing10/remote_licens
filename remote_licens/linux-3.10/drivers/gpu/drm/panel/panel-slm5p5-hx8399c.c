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
#define WIDTH	(1080)
#define HEIGHT	(1920)
#define HBP	(60)
#define HFP	(16)
#define HSW	(60)
#define HACT	(WIDTH)
#define VBP_F1	(3)
#define VFP_F1	(9)
#define VSW_F1	(4)
#define VACT_F1	(HEIGHT)
#define VT_F1	(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define VBP_F2	(0)
#define VFP_F2	(0)
#define VSW_F2	(0)
#define VACT_F2	(0)
#define VT_F2	(VBP_F2 + VFP_F2 + VSW_F2 + VACT_F2)
#define DCLK	(74250000)

#define DCS_MDELAY_FLAG (0)

static u8 slm5p5_hx8399c_init_code[] = {
	/* source scan sequence */
	0x02,
	0x36, 0x02,
	/* Set EXTC */
	0x04,
	0xB9, 0xFF, 0x83, 0x99,
	/* Set Power */
	0x10,
	0xB1, 0x02, 0x04, 0x6D, 0x8D, 0x01, 0x32, 0x33,
	0x11, 0x11, 0x5A, 0x5F, 0x56, 0x73, 0x02, 0x02,
	/* Set Display */
	0x0C,
	0xB2, 0x00, 0x80, 0x80, 0xAE, 0x05, 0x07, 0x5A, 0x11, 0x10, 0x10, 0x00,
	/* Set CYC */
	0x2D,
	0xB4, 0x00, 0xFF, 0x10, 0x18, 0x04, 0x9A, 0x00, 0x00, 0x06, 0x00, 0x02,
	    0x04, 0x00, 0x24, 0x02,
	0x04, 0x0A, 0x21, 0x03, 0x00, 0x00, 0x02, 0x9F, 0x88, 0x10, 0x18, 0x04,
	    0x9A, 0x00, 0x00, 0x08,
	0x00, 0x02, 0x04, 0x00, 0x24, 0x02, 0x04, 0x0A, 0x00, 0x00, 0x02, 0x9F,
	    0x12,
	/* Set GIP_0 */
	0x22,
	0xD3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x05,
	    0x00, 0x05, 0x00, 0x07,
	0x88, 0x07, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
	    0x01, 0x00, 0x00, 0x00,
	0x05, 0x40,
	/* Set GIP_1 */
	0x21,
	0xD5, 0x20, 0x20, 0x19, 0x19, 0x18, 0x18, 0x01, 0x01, 0x00, 0x00, 0x25,
	    0x25, 0x18, 0x18, 0x18,
	0x18, 0x24, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x2F,
	    0x2F, 0x30, 0x30, 0x31,
	0x31,
	/* Set GIP_2 */
	0x21,
	0xD6, 0x24, 0x24, 0x18, 0x18, 0x19, 0x19, 0x00, 0x00, 0x01, 0x01, 0x25,
	    0x25, 0x18, 0x18, 0x18,
	0x18, 0x20, 0x20, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x2F,
	    0x2F, 0x30, 0x30, 0x31,
	0x31,
	/* Set GIP_3_0 */
	0x11,
	0xD8, 0xAA, 0x8A, 0xAA, 0xAA, 0xAA, 0x8A, 0xAA, 0xAA, 0xAA, 0x8A, 0xAA,
	    0xAA, 0xAA, 0x8A, 0xAA,
	0xAA,
	/* Set Bank */
	0x02,
	0xBD, 0x01,
	/* Set GIP_3_1 */
	0x11,
	0xD8, 0xFF, 0xFC, 0xC0, 0x3F, 0xFF, 0xFC, 0xC0, 0x3F, 0xFF, 0xFC, 0xC0,
	    0x3F, 0xFF, 0xFC, 0xC0,
	0x3F,
	/* Set Bank */
	0x02,
	0xBD, 0x02,
	/* Set GIP_3_2 */
	0x09,
	0xD8, 0xFF, 0xFC, 0xC0, 0x3F, 0xFF, 0xFC, 0xC0, 0x3F,
	/* Set Bank */
	0x02,
	0xBD, 0x00,
	/* Set VCOM */
	0x03,
	0xB6, 0x86, 0x86,
	/* Set Panel */
	0x02,
	0xCC, 0x00,
	/* Set GAMMA */
	0x37,
	0xE0, 0x02, 0x2C, 0x3A, 0x35, 0x6E, 0x76, 0x81, 0x7C, 0x81, 0x89, 0x91,
	    0x94, 0x99, 0x9C, 0xA4,
	0xA6, 0xA9, 0xAF, 0xB1, 0xB8, 0xA9, 0xB6, 0xB9, 0x5F, 0x5A, 0x62, 0x7C,
	    0x02, 0x2C, 0x3A, 0x35,
	0x6E, 0x76, 0x81, 0x7C, 0x81, 0x89, 0x91, 0x94, 0x99, 0x9C, 0xA3, 0xA5,
	    0xA7, 0xAF, 0xB1, 0xB6,
	0xAA, 0xB6, 0xB7, 0x5E, 0x59, 0x62, 0x7C,
	/* Checksum Enable */
	0x02,
	0xD0, 0x39,
	/* Sleep Out */
	2,
	0x11, 0x00,
	/* delay 100ms */
	DCS_MDELAY_FLAG, 100,
	/* Display ON */
	2,
	0x29, 0x00,
	/* delay 100ms */
	DCS_MDELAY_FLAG, 100,
};

static ssize_t slm5p5_hx8399c_mipi_dsi_dcs_write_array(
	struct panel_simple_host *host)
{
	u32 i = 0;
	u32 len = (sizeof(slm5p5_hx8399c_init_code) /
			sizeof((slm5p5_hx8399c_init_code)[0]));
	u8 *ini_code = &slm5p5_hx8399c_init_code[0];

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


static struct lombo_vo_dev_config slm5p5_hx8399c_vo = {
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
		.bit_clk = 849000000,
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

/* mipi dsi 1080x1920 LCD test */
static struct drm_display_mode slm5p5_hx8399c_mode = {
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

	.private_size = sizeof(slm5p5_hx8399c_vo),
	.private = (int *)&slm5p5_hx8399c_vo,
};

static struct panel_delay slm5p5_hx8399c_delay = {
	.prepare = 50,
	.unprepare = 50,
	.enable = 50,
	.disable = 50,
	.power_on = 50,
	.reset = 50,
};

const struct panel_simple_desc slm5p5_hx8399c_desc = {
	.modes = &slm5p5_hx8399c_mode,
	.num_modes = 1,
	.bpc = 6,
	.init_panel = PANEL_SIMPLE_DSI,
	.delay = &slm5p5_hx8399c_delay,
	.size = {
		.width = 69,
		.height = 123,
	},
	.ops.transfer = slm5p5_hx8399c_mipi_dsi_dcs_write_array,
};



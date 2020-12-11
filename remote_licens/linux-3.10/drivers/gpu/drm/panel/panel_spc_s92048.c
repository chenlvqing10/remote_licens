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

#define IS_INTERLACE	(FALSE)

#define WIDTH		(800)
#define HEIGHT		(480)

#define HBP		(88)
#define HFP		(40)
#define HSW		(128)
#define HACT		(WIDTH)
#define VBP_F1		(33)
#define VFP_F1		(10)
#define VSW_F1		(2)
#define VACT_F1		(HEIGHT)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT		(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define DCLK		(33000000) /* 33MHz */

static struct lombo_vo_dev_config spc_s92048_vo = {
	/* device interface, reference to vo_dev_if_t */
	.dev_if = VO_DEV_PRGB,
	/* device specific configuation, reference to vo_if_cfg_t */
	.rgb_if.vsync_pol = TCON_IO_NEG,
	.rgb_if.hsync_pol = TCON_IO_NEG,
	.rgb_if.de_pol = TCON_IO_POS,
	.rgb_if.clk_pol = TCON_IO_FALLING,
	.rgb_if.is_clk_ddr = false,

	/* tcon host config */
	/* timing, reference to tcon_timings_t */
	.timing.is_interlace = false,
	.timing.field1.vt = VT,
	.timing.field1.vact = VACT_F1,
	.timing.field1.vfp = VFP_F1,
	.timing.field1.vsw = VSW_F1,
	.timing.field2.vt = VSW_F2 + VBP_F2 + VACT_F2 + VFP_F2,
	.timing.field2.vact = VACT_F2,
	.timing.field2.vfp = VFP_F2,
	.timing.field2.vsw = VSW_F2,
	.timing.hline.ht = HSW + HBP + HACT + HFP,
	.timing.hline.hact = HACT,
	.timing.hline.hfp = HFP,
	.timing.hline.hsw = HSW,
	.timing.dclk_freq = DCLK,
	/* resolution in pixel */
	.timing.width = WIDTH,
	.timing.height = HEIGHT,
	/* format, reference to tcon_output_fmt_t */
	.format = TCON_FMT_RGB666,
};

static struct drm_display_mode spc_s92048_mode = {
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

	.private_size = sizeof(spc_s92048_vo),
	.private = (int *)&spc_s92048_vo,
};

static struct panel_delay spc_s92048_delay = {
	.prepare = 100,
	.unprepare = 0,
	.enable = 0,
	.disable = 0,
	.power_on = 50,
	.reset = 100,
};

const struct panel_simple_desc simple_spc_s92048_desc = {
	.modes = &spc_s92048_mode,
	.num_modes = 1,
	.delay = &spc_s92048_delay,
	.size = {
		.width = 140,
		.height = 90,
	},
};

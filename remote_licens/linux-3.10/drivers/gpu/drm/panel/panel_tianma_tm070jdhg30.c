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

#define TM070JDHG30_HBP		(5)
#define TM070JDHG30_HFP		(50)
#define TM070JDHG30_HSW		(100)
#define TM070JDHG30_HACT	(1280)
#define TM070JDHG30_VBP		(5)
#define TM070JDHG30_VFP		(9)
#define TM070JDHG30_VSW		(4)
#define TM070JDHG30_VACT	(800)
#define TM070JDHG30_DCLK	(55000000) /* 55MHz */

static struct lombo_vo_dev_config tianma_tm070jdhg30_vo = {
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
	.timing.field1.vt = TM070JDHG30_VSW + TM070JDHG30_VBP
			  + TM070JDHG30_VACT + TM070JDHG30_VFP,
	.timing.field1.vact = TM070JDHG30_VACT,
	.timing.field1.vfp = TM070JDHG30_VFP,
	.timing.field1.vsw = TM070JDHG30_VSW,
	.timing.hline.ht = TM070JDHG30_HSW + TM070JDHG30_HBP
			 + TM070JDHG30_HACT + TM070JDHG30_HFP,
	.timing.hline.hact = TM070JDHG30_HACT,
	.timing.hline.hfp = TM070JDHG30_HFP,
	.timing.hline.hsw = TM070JDHG30_HSW,
	.timing.dclk_freq = TM070JDHG30_DCLK,
	/* resolution in pixel */
	.timing.width = TM070JDHG30_HACT,
	.timing.height = TM070JDHG30_VACT,
	/* format, reference to tcon_output_fmt_t */
	.format = TCON_FMT_RGB888,
};

static struct drm_display_mode tianma_tm070jdhg30_mode = {
	/* dclk_freq */
	.clock = TM070JDHG30_DCLK / 1000,
	/* width */
	.hdisplay = TM070JDHG30_HACT,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = TM070JDHG30_HACT + TM070JDHG30_HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = TM070JDHG30_HACT + TM070JDHG30_HFP + TM070JDHG30_HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = TM070JDHG30_HACT + TM070JDHG30_HFP
		+ TM070JDHG30_HSW + TM070JDHG30_HBP,
	/* height */
	.vdisplay = TM070JDHG30_VACT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = TM070JDHG30_VACT + TM070JDHG30_VFP,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = TM070JDHG30_VACT + TM070JDHG30_VFP + TM070JDHG30_VSW,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = TM070JDHG30_VACT + TM070JDHG30_VFP
		+ TM070JDHG30_VSW + TM070JDHG30_VBP,
	.vrefresh = 50,

	.private_size = sizeof(tianma_tm070jdhg30_vo),
	.private = (int *)&tianma_tm070jdhg30_vo,
};

static struct panel_delay tianma_tm070jdhg30_delay = {
	.prepare = 100,
	.unprepare = 0,
	.enable = 0,
	.disable = 0,
	.power_on = 50,
	.reset = 100,
};

const struct panel_simple_desc tianma_tm070jdhg30_desc = {
	.modes = &tianma_tm070jdhg30_mode,
	.num_modes = 1,
	.delay = &tianma_tm070jdhg30_delay,
	.size = {
		.width = 200,
		.height = 120,
	},
};


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

#ifndef _LOMBO_DOSS_H_
#define _LOMBO_DOSS_H_

#include <linux/types.h>

#include "csp_doss_top.h"
#include "csp_tcon/csp_doss_tcon.h"
#include "csp_tve/csp_doss_tve.h"
#include "csp_dsi/csp_doss_mipi_dsi.h"

enum {
	LOMBO_PANEL_BOOT_SYNC_PREPARE = 0,
	LOMBO_PANEL_BOOT_SYNC_ENABLE,
};

typedef enum vo_dev_if {
	VO_DEV_PRGB			= 0,
	VO_DEV_SRGB_RGB			= 1,
	VO_DEV_SRGB_DUMMY_RGB		= 2,
	VO_DEV_SRGB_RGB_DUMMY		= 3,
	VO_DEV_CPU_18BIT_RGB666		= 4,
	VO_DEV_CPU_16BIT_RGB888		= 5,
	VO_DEV_CPU_16BIT_RGB666		= 6,
	VO_DEV_CPU_16BIT_RGB565		= 7,
	VO_DEV_CPU_9BIT_RGB666		= 8,
	VO_DEV_CPU_8BIT_RGB666		= 9,
	VO_DEV_CPU_8BIT_RGB565		= 10,
	VO_DEV_CPU_8BIT_RGB888		= 11,
	VO_DEV_LVDS			= 12,
	VO_DEV_MIPI_DSI_VIDEO		= 13,
	VO_DEV_MIPI_DSI_CMD		= 14,
	VO_DEV_BT601_24BIT		= 15,
	VO_DEV_BT601_16BIT		= 16,
	VO_DEV_BT601_8BIT		= 17,
	VO_DEV_BT656_8BIT		= 18,
	VO_DEV_BT1120_16BIT		= 19,
	VO_DEV_BT1120_8BIT		= 20,
	VO_DEV_TVE			= 21,
	VO_DEV_TVE_NTSC			= 22,
	VO_DEV_TVE_PAL			= 23,
} vo_dev_if_t;

struct lombo_vo_dev_config {
	vo_dev_if_t dev_if;
	union {
		tcon_rgb_if_t rgb_if;
		tcon_cpu_if_t cpu_if;
		tcon_lvds_if_t lvds_if;
		tcon_mipi_dsi_if_t dsi_if;
		tcon_tve_if_t tve_if;
	};

	/* configs for tcon host */
	tcon_timings_t timing;
	tcon_output_fmt_t format;

	/* option generic func */
	bool is_rb_swap;
	bool csc_en;
	tcon_csc_info_t *csc;
	bool frc_en;
	tcon_frc_mode_t frc_mode;
	bool gamma_en;
	u32 *gamma_tbl;

	/* option TE */
	bool has_te;
	tcon_io_pol_t te_pol;

	/* option IRQ */
	tcon_vbi_trigger_t trig_vbi;
	tcon_trig_src_mode_t tcon_trig_src_mode;

};

struct clk;
struct lombo_doss_clk;
struct drm_encoder;

extern bool lombo_tcon_is_lvds_dual_link(struct device *dev);

struct lombo_doss_clk *lombo_doss_get_clk(
	struct device *dev, struct device *doss_top,
	uint32_t path_id, doss_clk_id_t clk_id);
int lombo_doss_put_clk(struct lombo_doss_clk **p_clk);
struct clk *lombo_doss_clk_get_parent(struct lombo_doss_clk *child);
unsigned long lombo_doss_clk_get_rate(struct lombo_doss_clk *clk);
int lombo_doss_clk_set_rate(struct lombo_doss_clk *clk, unsigned long rate);
unsigned int lombo_doss_clk_get_div(struct lombo_doss_clk *clk);
int lombo_doss_clks_enable(uint32_t clk_num, struct lombo_doss_clk **clks,
	vo_dev_if_t dev_if, uint8_t sw);
int lombo_doss_clks_disable(uint32_t clk_num, struct lombo_doss_clk **clks,
	vo_dev_if_t dev_if);

int lombo_doss_route(struct device *doss_top,
	u32 tcon_id, u32 tcon_src);

#ifdef CONFIG_DRM_LOMBO_MIPI_DSI

extern int lombo_dsi_encoder_get_clk(struct drm_encoder *enc,
	uint32_t num, struct lombo_doss_clk **p_clks);

#else

static inline int lombo_dsi_encoder_get_clk(struct drm_encoder *enc,
	uint32_t num, struct lombo_doss_clk **p_clks)
{
	return 0;
}
#endif

#ifdef CONFIG_DRM_LOMBO_TVE
extern int lombo_tve_encoder_get_clk(struct drm_encoder *enc,
	uint32_t num, struct lombo_doss_clk **p_clks);

#else
static inline int lombo_tve_encoder_get_clk(struct drm_encoder *enc,
	uint32_t num, struct lombo_doss_clk **p_clks)
{
	return 0;
}
#endif

#endif

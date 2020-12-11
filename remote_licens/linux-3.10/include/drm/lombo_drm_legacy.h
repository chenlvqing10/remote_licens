/* lombo_drm.h
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _LOMBO_DRM_LEGACY_H_
#define _LOMBO_DRM_LEGACY_H_

#include <uapi/drm/lombo_drm.h>

/* memory type definitions. */
enum e_drm_lombo_gem_mem_type {
	/* Physically Continuous memory and used as default. */
	LOMBO_BO_CONTIG = 0 << 0,
	/* Physically Non-Continuous memory. */
	LOMBO_BO_NONCONTIG      = 1 << 0,
	/* non-cachable mapping and used as default. */
	LOMBO_BO_NONCACHABLE    = 0 << 1,
	/* cachable mapping. */
	LOMBO_BO_CACHABLE       = 1 << 1,
	/* write-combine mapping. */
	LOMBO_BO_WC             = 1 << 2,
	LOMBO_BO_MASK           = LOMBO_BO_NONCONTIG | LOMBO_BO_CACHABLE
					 | LOMBO_BO_WC
};

struct drm_lombo_gem_create {
	uint64_t size;
	unsigned int flags;
	unsigned int handle;
};

struct drm_lombo_gem_map_off {
	unsigned int handle;
	unsigned int pad;
	uint64_t offset;
};

struct drm_lombo_gem_mmap {
	unsigned int handle;
	unsigned int pad;
	uint64_t size;
	uint64_t mapped;
};

struct drm_lombo_gem_info {
	unsigned int handle;
	unsigned int flags;
	uint64_t size;
};

/**
 * A structure for lcd panel information.
 *
 * @timing: default video mode for initializing
 * @width_mm: physical size of lcd width.
 * @height_mm: physical size of lcd height.
 */
struct lombo_drm_panel_info {
	struct fb_videomode timing;
	u32 width_mm;
	u32 height_mm;
};

enum dpu_sclk_id {
	DPU_SCLK0 = 0,
	DPU_SCLK1,
	DPU_SCLK_NUM,
};

struct dpu_clk;

struct dpu_clk *dpu_get_clk(u32 mod_id);
int dpu_put_clk(struct dpu_clk **clk);
enum dpu_sclk_id dpu_clk_get_parent(struct dpu_clk *child);
int dpu_clk_set_parent(struct dpu_clk *child, enum dpu_sclk_id parent);
unsigned long dpu_clk_get_rate(struct dpu_clk *clk);
int dpu_clk_set_rate(struct dpu_clk *clk, unsigned long rate);
int dpu_clk_prepare_enable(struct dpu_clk *clk);
int dpu_clk_disable_unprepare(struct dpu_clk *clk);

#endif	/* _LOMBO_DRM_LEGACY_H_ */

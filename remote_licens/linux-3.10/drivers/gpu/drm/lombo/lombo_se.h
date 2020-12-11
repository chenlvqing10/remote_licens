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

#ifndef _LOMBO_SE_H_
#define _LOMBO_SE_H_

#include <linux/types.h>

struct list_head;

struct device;
struct drm_device;
struct drm_crtc;
struct drm_framebuffer;

struct drm_lombo_scale_fb;
struct lombo_dpu_clk;
struct lombo_fb_cfg;
struct lombo_se;

/* se input port selection */
enum {
	LOMBO_SE_IN_SEL_DRAM = 0, /* SE_INPUT_DRAM */
	LOMBO_SE_IN_SEL_DC = 1, /* SE_INPUT_DC */
};

enum {
	LOMBO_SE_RUN_MODE_OFFLINE = 0,
	LOMBO_SE_RUN_MODE_ONLINE = 1,
};

/**
 * [input]@in_sel: see LOMBO_SE_IN_SEL_xxx.
 * [input]@in_fmt: valid if in_sel_dc.
 * [input]@se_run_mode.
 * [input&output]@se_out_format may be changed by se.
 * [input]@scn_w: width of dpu out.
 * [input]@scn_h: height of dpu out.
 * [input]@scn_fps: frame per second of dpu out.
 * [input&output]@plane_cfg.
 */
struct lombo_se_plane_cfg {
	void *reqer;

	uint32_t in_sel;
	uint32_t in_fmt;
	uint32_t se_run_mode; /* 1:online; 0:offline */
	uint32_t se_out_format; /* drm fourcc format */

	uint32_t scn_w;
	uint32_t scn_h;
	uint32_t scn_fps;

	struct lombo_fb_cfg *fb_cfg;
};

struct lombo_se_scale_cfg {
	void *reqer;
	uint64_t timeout_ns;

	struct drm_framebuffer *src_fb;
	struct drm_lombo_scale_fb *src_cfg;

	struct drm_framebuffer *dst_fb;
	struct drm_lombo_scale_fb *dst_cfg;
};

#ifdef CONFIG_DRM_LOMBO_SE

struct lombo_se *lombo_se_request(struct drm_device *drm, void *reqer);
void lombo_se_release(struct lombo_se *se, void *reqer, uint32_t do_disable);
uint32_t lombo_se_id(struct lombo_se *se);

/* se online mode apis */
int lombo_se_update_plane(struct lombo_se *se, void *reqer,
		struct lombo_se_plane_cfg *req_cfg);
int lombo_se_disable_plane(struct lombo_se *se, void *reqer);
void lombo_se_commit(struct lombo_se *se, void *reqer);
int lombo_se_page_flip_worker(struct lombo_se *se, void *reqer);

/* se offline mode apis */
int lombo_se_scale_check_fbs(struct lombo_se *se,
	struct lombo_se_scale_cfg *cfg);
int lombo_se_scale_exec(struct lombo_se *se,
	struct lombo_se_scale_cfg *cfg);

#else /* !CONFIG_DRM_LOMBO_SE */

static inline struct lombo_se *lombo_se_request(
	struct drm_device *drm, void *reqer)
{
	return NULL;
}

static inline void lombo_se_release(struct lombo_se *se,
	void *reqer, uint32_t do_disable)
{
}

static inline uint32_t lombo_se_id(struct lombo_se *se)
{
	return -ENODEV;
}

static inline int lombo_se_update_plane(struct lombo_se *se,
	void *reqer, struct lombo_se_plane_cfg *req_cfg)
{
	return -ENODEV;
}

static inline int lombo_se_disable_plane(struct lombo_se *se,
	void *reqer)
{
	return -ENODEV;
}

static inline void lombo_se_commit(struct lombo_se *se, void *reqer)
{
}
static inline int lombo_se_page_flip_worker(
	struct lombo_se *se, void *reqer)
{
	return -ENODEV;
}

/* se offline mode apis */
int lombo_se_scale_check_fbs(struct lombo_se *se,
	struct lombo_se_scale_cfg *cfg)
{
	return -ENODEV;
}

int lombo_se_scale_exec(struct lombo_se *se,
	struct lombo_se_scale_cfg *cfg)
{
	return -ENODEV;
}

#endif /* CONFIG_DRM_LOMBO_SE */

#endif /* #ifndef _LOMBO_SE_H_ */

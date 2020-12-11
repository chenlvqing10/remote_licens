/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _LOMBO_OVE_H_
#define _LOMBO_OVE_H_

struct drm_device;
struct drm_framebuffer;
struct lombo_ove;

enum {
	LOMBO_OVE_OUT_DC = 0,
	LOMBO_OVE_OUT_SE0,
	LOMBO_OVE_OUT_SE1,
	LOMBO_OVE_OUT_SE2,
};

/**
 * [input]@out_sel: ove out port. see LOMBO_OVE_OUT_xxx.
 * [input&output]@out_format: output format of ove.
 * [input]@fb_cfg: setmode config of fb.
 */
struct lombo_ove_fb_cfg {
	void *reqer;

	uint32_t out_sel;
	uint32_t out_fmt; /* drm_fourcc */
	struct lombo_fb_cfg *fb_cfg;
};

#ifdef CONFIG_DRM_LOMBO_OVE

struct lombo_ove *lombo_ove_request(struct lombo_ove_fb_cfg *req_cfg);

/* donot call this func in irq */
void lombo_ove_release(struct lombo_ove *ove,
	void *reqer, uint32_t do_disable);

/* ove online mode apis */
int lombo_ove_update_plane(struct lombo_ove *ove,
		struct lombo_ove_fb_cfg *req_cfg);
int lombo_ove_disable_plane(struct lombo_ove *ove, void *reqer);
void lombo_ove_commit(struct lombo_ove *ove, void *reqer);
int lombo_ove_page_flip_worker(struct lombo_ove *ove, void *reqer);

#else

static inline struct lombo_ove *lombo_ove_request(
	struct lombo_ove_fb_cfg *req_cfg)
{
	return NULL;
}

static inline void lombo_ove_release(struct lombo_ove *ove,
	void *reqer, uint32_t do_disable)
{
}

static inline int lombo_ove_update_plane(struct lombo_ove *ove,
		struct lombo_ove_fb_cfg *req_cfg)
{
	return -ENODEV;
}

static inline int lombo_ove_disable_plane(struct lombo_ove *ove, void *reqer)
{
	return -ENODEV;
}

static inline void lombo_ove_commit(struct lombo_ove *ove, void *reqer)
{
}

static inline int lombo_ove_page_flip_worker(
	struct lombo_ove *ove, void *reqer)
{
	return -ENODEV;
}

#endif /* #ifdef CONFIG_DRM_LOMBO_OVE */

#endif /* #ifndef _LOMBO_OVE_H_ */

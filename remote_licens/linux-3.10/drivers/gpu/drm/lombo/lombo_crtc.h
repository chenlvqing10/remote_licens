/* lombo_drm_crtc.c
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

#ifndef _LOMBO_CRTC_H_
#define _LOMBO_CRTC_H_

struct seq_file;
struct lombo_tcon;
struct lombo_dc;

/* color space format */
enum {
	LOMBO_CSFMT_RGB,
	LOMBO_CSFMT_YUV,
	LOMBO_CSFMT_NUM,
};

struct lombo_cursor {
	uint32_t handle;
	uint32_t width;
	uint32_t height;
	uint32_t x;
	uint32_t y;
	uint32_t format;
	struct drm_gem_cma_object *cma_obj;
};

struct lombo_crtc {
	struct drm_crtc crtc;
	int pfa_cache;

	struct lombo_tcon *tcon;
	struct lombo_dc *dc;

	struct lombo_cursor corsor;

	int color_enhance_enable;
	int color_enhance_brightness;
	int color_enhance_saturation;
	int color_enhance_contrast;
	int gamma_enable;
};

struct lombo_fb_cfg {
	struct drm_framebuffer *fb;

	int crtc_x;
	int crtc_y;
	unsigned int crtc_w;
	unsigned int crtc_h;

	/* src_x/y/w/h are Q16 */
	uint32_t src_x;
	uint32_t src_y;
	uint32_t src_w;
	uint32_t src_h;

	int color_space; /* color encording */
	int color_range;

	/* video sharpness */
	int video_lti;
	int video_peak;
};

static inline struct lombo_crtc *drm_crtc_to_lombo_crtc(struct drm_crtc *crtc)
{
	return container_of(crtc, struct lombo_crtc, crtc);
}

struct lombo_crtc *lombo_crtc_init(struct drm_device *drm,
	struct lombo_dc *dc, struct lombo_tcon *tcon);
int lombo_page_flip_atomic_ioctl(struct drm_device *dev, void *data,
		struct drm_file *file_priv);

void lombo_crtc_handle_vblank_events(struct drm_crtc *crtc);

int lombo_crtc_kms_show(struct seq_file *m, void *data);

#endif /* #ifndef _LOMBO_CRTC_H_ */

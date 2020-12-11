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

#ifndef _LOMBO_FB_ARRAY_
#define _LOMBO_FB_ARRAY_

#include <drm/lombo_drm.h>

struct drm_framebuffer;
struct drm_device;
struct drm_file;
struct drm_mode_fb_cmd2;

/**
 * struct lombo_fb_array_fb: fb of fb_array.
 * @fb: pointer to drm fb.
 * @src: src crop of @fb.
 * @dst: position in fb_array.
 */
struct lombo_fb_array_fb {
	struct drm_framebuffer *fb;
	struct drm_lombo_rect_wh src; /* Q16 values */
	struct drm_lombo_rect_wh dst;
	uint8_t alpha_mode;
	uint8_t alpha_value;
	uint8_t is_premul;
	uint8_t pad;
};

struct drm_framebuffer *lombo_fb_array_create(struct drm_device *dev,
	struct drm_file *file_priv, struct drm_mode_fb_cmd2 *mode_cmd);

int lombo_fb_array_get_src_win(struct drm_framebuffer *fb,
	struct drm_lombo_rect_wh *rect);

uint32_t lombo_fb_array_get_fb_num(struct drm_framebuffer *fb);

struct lombo_fb_array_fb *lombo_fb_array_get_fb(
	struct drm_framebuffer *fb, uint32_t index);

#endif /* #ifndef _LOMBO_FB_ARRAY_ */

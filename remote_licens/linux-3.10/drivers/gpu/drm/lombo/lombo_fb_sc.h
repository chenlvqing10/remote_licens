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

#ifndef _LOMBO_FB_SC_
#define _LOMBO_FB_SC_

struct drm_framebuffer;
struct drm_device;
struct drm_file;
struct drm_mode_fb_cmd2;

struct drm_framebuffer *lombo_fb_sc_create(struct drm_device *dev,
	struct drm_file *file_priv, struct drm_mode_fb_cmd2 *mode_cmd);

uint32_t lombo_fb_sc_get_color(struct drm_framebuffer *fb,
	unsigned int plane);

#endif /* #ifndef _LOMBO_FB_SC_ */

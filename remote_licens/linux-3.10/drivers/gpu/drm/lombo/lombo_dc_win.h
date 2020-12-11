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

#ifndef _LOMBO_DC_WIN_H_
#define _LOMBO_DC_WIN_H_

#include "lombo_ove.h"

struct drm_device;
struct drm_plane;
struct drm_framebuffer;
struct lombo_crtc;
struct lombo_dc;
struct lombo_se;

#define LOMBO_DC_WIN_OVR_FB_NUM 4

/* update flag for atomic pageflip flush */
enum {
	LOMBO_DC_WIN_UPDATE_BIT_WIN = 1,
	LOMBO_DC_WIN_UPDATE_BIT_SE = 1 << 1,
	LOMBO_DC_WIN_UPDATE_BIT_OVE = 1 << 2,

	LOMBO_DC_WIN_UPDATE_FLAG_WIN =
		LOMBO_DC_WIN_UPDATE_BIT_WIN,
	LOMBO_DC_WIN_UPDATE_FLAG_SE_WIN =
		LOMBO_DC_WIN_UPDATE_BIT_WIN |
		LOMBO_DC_WIN_UPDATE_BIT_SE,
	LOMBO_DC_WIN_UPDATE_FLAG_OVE_WIN =
		LOMBO_DC_WIN_UPDATE_BIT_WIN |
		LOMBO_DC_WIN_UPDATE_BIT_OVE,
	LOMBO_DC_WIN_UPDATE_FLAG_OVE_SE_WIN =
		LOMBO_DC_WIN_UPDATE_BIT_WIN |
		LOMBO_DC_WIN_UPDATE_BIT_SE |
		LOMBO_DC_WIN_UPDATE_BIT_OVE,
};

struct lombo_dc_win {
	struct drm_plane base;

	struct lombo_dc *dc;
	struct lombo_se *se;
	struct lombo_se *commit_se;
	struct lombo_ove_fb_cfg ove_fb_cfg;
	struct lombo_ove *ove;
	struct lombo_ove *commit_ove;

	/* used to store plane update para */
	struct drm_crtc *crtc;
	struct drm_framebuffer *fb;
	struct drm_framebuffer *old_fb;
	uint32_t fb_num;
	int crtc_x;
	int crtc_y;
	unsigned int crtc_w;
	unsigned int crtc_h;
	/* src_x/y/w/h are Q16 */
	uint32_t src_x;
	uint32_t src_y;
	uint32_t src_w;
	uint32_t src_h;

	int zpos;

	struct drm_property *alpha_property;
	uint16_t alpha_value;
	uint32_t alpha_mode;

	int color_space; /* color_encoding */
	int color_range;

	int video_lti;
	int video_peak;

	uint32_t update_flag;

	/* opaque rect */
	int use_blk;
	int opaque_num;
	struct drm_lombo_rect_wh opaque[4];
	struct drm_lombo_rect_wh trp; /* transparent */
};

static inline struct lombo_dc_win *to_lombo_dc_win(struct drm_plane *plane)
{
	return container_of(plane, struct lombo_dc_win, base);
}

struct lombo_dc_win *lombo_dc_win_init(struct lombo_dc *dc, int id,
	const uint32_t *formats, uint32_t format_count,
	unsigned long possible_crtcs);
int lombo_dc_win_update_win(struct lombo_dc_win *win);
void lombo_dc_win_commit(struct lombo_dc_win *win);
int lombo_dc_win_page_flip_worker(struct lombo_dc_win *win);

#endif /* #ifndef _LOMBO_DC_WIN_H_ */

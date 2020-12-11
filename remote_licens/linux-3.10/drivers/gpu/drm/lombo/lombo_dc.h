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

#ifndef _LOMBO_DC_H_
#define _LOMBO_DC_H_

#include <linux/types.h>
#include "csp_dpu_dc.h"

#define DC_COLOR_ENHANCE_MAX_VALUE	100

struct device;
struct seq_file;
struct drm_plane;
struct drm_device;
struct drm_display_mode;
struct drm_framebuffer;
struct drm_crtc;

struct lombo_dpu_clk;
struct lombo_tcon;
struct lombo_cursor;
struct lombo_dc_win;
struct drm_lombo_rect_wh;

enum {
	LOMBO_CURSOR_PENDING_ENABLE = 1,
	LOMBO_CURSOR_PENDING_DISABLE,
	LOMBO_CURSOR_PENDING_MOVE,
};

enum {
	LOMBO_BLK_PENDING_DISABLE = 1,
	LOMBO_BLK_PENDING_FB, /* only update fbs */
	LOMBO_BLK_PENDING_ENABLE, /* disabled -> enable */
};

struct lombo_pending_rmfbs {
	struct drm_framebuffer *fb_array[128];
	uint32_t array_size;
	uint32_t fb_head;
	uint32_t fb_num; /* total fb num in array */
	uint32_t rm_num;

	spinlock_t lock;
};

struct lombo_dc_cfg {
	uint32_t win_num;

	/* all wins of dc support same formats */
	uint32_t format_count;
	const uint32_t *formats;

	uint8_t support_yuv; /* dc itself can deal yuv fb */
	uint8_t canot_use_se;
	uint8_t no_csc;

	uint8_t pad;
};

struct lombo_dc {
	struct device *dev;
	struct device *dpu_top;
	struct drm_device *drm;
	struct lombo_tcon *tcon;
	uint32_t id; /* hw id */

	struct lombo_dpu_clk *mod_clk;

	void __iomem *reg;

	/* cfg */
	const struct lombo_dc_cfg *cfg;
	uint32_t hw_cursor_num;

	/* store planes for win, hw-corsor sequently */
	struct drm_plane **planes;

	int fbdev_win_id;

	/* add to drv dc_list */
	struct list_head list;

	uint32_t scn_w;
	uint32_t scn_h;
	uint32_t scn_fmt;
	uint32_t scn_fps;

	dc_enhance_info_t ce; /* color enhance */

	/* for video sharpness */
	int lti;
	int peak;

	struct work_struct page_flip_work;

	struct lombo_pending_rmfbs pending_rmfbs;

	struct lombo_cursor *cursor;
	uint32_t cursor_pending;

	/* block linker for fbdev */
	uint32_t available_block_num;
	dpu_fb_t block_fbs[4]; /* cache blk fbs para */
	dpu_rectangle_t block_dst[4];
	uint32_t blk_pending;
};

static inline int lombo_dc_id(struct lombo_dc *dc)
{
	return dc ? dc->id : -ENODEV;
}

static inline void lombo_dc_set_tcon(struct lombo_dc *dc,
	struct lombo_tcon *tcon)
{
	if (dc)
		dc->tcon = tcon;
}

#ifdef CONFIG_DRM_LOMBO_DC

struct drm_plane **lombo_dc_planes_init(
	struct lombo_dc *dc, uint32_t pipe);

int lombo_dc_mode_fixup(struct lombo_dc *dc,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode);
int lombo_dc_get_scn(struct lombo_dc *dc, int *width, int *height);
int lombo_dc_get_csfmt(struct lombo_dc *dc);
int lombo_dc_set_csfmt(struct lombo_dc *dc, int csfmt);

int lombo_dc_set_dpms(struct lombo_dc *dc, int dmps);
int lombo_dc_set_mode_base(struct lombo_dc *dc,
	struct drm_crtc *crtc, int x, int y,
	struct drm_framebuffer *old_fb);
int lombo_dc_set_mode(struct lombo_dc *dc,
	struct drm_crtc *crtc,
	struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode,
	int x, int y,
	struct drm_framebuffer *old_fb);
int lombo_dc_set_color_enhance(struct lombo_dc *dc,
	uint32_t enhance_enable, uint32_t brightness,
	uint32_t saturation, uint32_t contrast);
int lombo_dc_cursor_set(struct lombo_dc *dc, struct lombo_cursor *cursor);
int lombo_dc_cursor_move(struct lombo_dc *dc, struct lombo_cursor *cursor);

int lombo_dc_update_wins(struct lombo_dc *dc);
int lombo_dc_commit(struct lombo_dc *dc);
void lombo_dc_disable(struct lombo_dc *dc, struct drm_crtc *crtc);

int lombo_dc_kms_show(struct lombo_dc *dc,
	struct seq_file *m, void *data);

int lombo_dc_set_fb_transparent_rect(struct lombo_dc *dc,
	struct drm_framebuffer *fb, struct drm_lombo_rect_wh *rect);
int lombo_dc_get_fb_transparent_rect(struct lombo_dc *dc,
	struct drm_framebuffer *fb, struct drm_lombo_rect_wh *rect);

/* inner func */
int lombo_dc_queue_rmfb(struct lombo_dc *dc, struct drm_framebuffer *fb);
int lombo_dc_convert_drm_format(int format);
int lombo_dc_update_blk_for_win(struct lombo_dc *dc,
	struct lombo_dc_win *win, struct drm_framebuffer *fb);

#else /* !CONFIG_DRM_LOMBO_DC */

static inline struct drm_plane **lombo_dc_planes_init(
	struct lombo_dc *dc, uint32_t pipe)
{
	return ERR_PTR(-ENODEV);
}

static inline int lombo_dc_mode_fixup(struct lombo_dc *dc,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{
	return -ENODEV;
}

static inline int lombo_dc_get_csfmt(struct lombo_dc *dc)
{
	return -ENODEV;
}

static inline int lombo_dc_set_dpms(struct lombo_dc *dc, int dmps)
{
	return -ENODEV;
}

static inline int lombo_dc_set_mode_base(struct lombo_dc *dc,
	struct drm_crtc *crtc, int x, int y,
	struct drm_framebuffer *old_fb)
{
	return -ENODEV;
}

static inline int lombo_dc_set_mode(struct lombo_dc *dc,
	struct drm_crtc *crtc,
	struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode,
	int x, int y,
	struct drm_framebuffer *old_fb)
{
	return -ENODEV;
}

static inline int lombo_dc_set_color_enhance(struct lombo_dc *dc,
	uint32_t enhance_enable, uint32_t brightness,
	uint32_t saturation, uint32_t contrast)

{
	return -ENODEV;
}

static inline int lombo_dc_cursor_set(struct lombo_dc *dc,
	struct lombo_cursor *cursor)
{
	return -ENODEV;
}

static inline int lombo_dc_cursor_move(struct lombo_dc *dc,
	struct lombo_cursor *cursor)
{
	return -ENODEV;
}

static inline int lombo_dc_update_wins(struct lombo_dc *dc)
{
	return -ENODEV;
}

static inline int lombo_dc_commit(struct lombo_dc *dc)
{
	return -ENODEV;
}

static inline void lombo_dc_disable(struct lombo_dc *dc,
	struct drm_crtc *crtc)
{
}

static inline int lombo_dc_kms_show(struct lombo_dc *dc,
	struct seq_file *m, void *data)
{
	return -ENODEV;
}

static inline int lombo_dc_set_fb_transparent_rect(struct lombo_dc *dc,
	struct drm_framebuffer *fb, struct drm_lombo_rect_wh *rect)
{
	return -ENODEV;
}

static inline int lombo_dc_get_fb_transparent_rect(struct lombo_dc *dc,
	struct drm_framebuffer *fb, struct drm_lombo_rect_wh *rect)
{
	return -ENODEV;
}

#endif /* CONFIG_DRM_LOMBO_DC */

#endif /* #ifndef _LOMBO_DC_H_ */

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

#include <linux/module.h>
#include <linux/component.h>
#include <linux/of_platform.h>

#include <drm/drmP.h>
#include <drm/drm_color_mgmt.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_gem_cma_helper.h>

#include <drm/lombo_drm.h>
#include "lombo_drv.h"
#include "lombo_crtc.h"
#include "lombo_dpu.h"
#include "lombo_dc.h"
#include "lombo_dc_win.h"
#include "lombo_fb_sc.h"

#include "csp_dpu_dc.h"

#define LOMBO_VIDEO_MAX_LTI_VAL		31
#define LOMBO_VIDEO_MAX_PEAK_VAL	15

int lombo_dc_convert_drm_format(int format)
{
	switch (format) {
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XRGB8888:
		return DPU_FMT_ARGB8888;
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_XBGR8888:
		return DPU_FMT_ABGR8888;
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_RGBX8888:
		return DPU_FMT_RGBA8888;
	case DRM_FORMAT_BGRA8888:
	case DRM_FORMAT_BGRX8888:
		return DPU_FMT_BGRA8888;
	case DRM_FORMAT_RGB888:
		return DPU_FMT_RGB888;
	case DRM_FORMAT_BGR888:
		return DPU_FMT_BGR888;
	case DRM_FORMAT_RGB565:
		return DPU_FMT_RGB565;
	case DRM_FORMAT_BGR565:
		return DPU_FMT_BGR565;
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
		return DPU_FMT_ARGB1555;
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
		return DPU_FMT_ABGR1555;
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_RGBX5551:
		return DPU_FMT_RGBA5551;
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_BGRX5551:
		return DPU_FMT_BGRA5551;
	case DRM_FORMAT_ARGB4444:
	case DRM_FORMAT_XRGB4444:
		return DPU_FMT_ARGB4444;
	case DRM_FORMAT_ABGR4444:
	case DRM_FORMAT_XBGR4444:
		return DPU_FMT_ABGR4444;
	case DRM_FORMAT_RGBA4444:
	case DRM_FORMAT_RGBX4444:
		return DPU_FMT_RGBA4444;
	case DRM_FORMAT_BGRA4444:
	case DRM_FORMAT_BGRX4444:
		return DPU_FMT_BGRA4444;
	case DRM_FORMAT_C8:
		return DPU_FMT_8BPP;

	case DRM_FORMAT_NV12:
		return DPU_FMT_YUV420SP;
	case DRM_FORMAT_NV21:
		return DPU_FMT_YVU420SP;
	case DRM_FORMAT_NV16:
		return DPU_FMT_YUV422SP;
	case DRM_FORMAT_NV61:
		return DPU_FMT_YVU422SP;
	case DRM_FORMAT_AYUV:
		return DPU_FMT_AYUV;

	default:
		DRM_ERROR("unsupport format[%.4s]\n", (char *)&format);
		return 0;
	}
}

static inline int _dc_suspend(struct lombo_dc *dc)
{
	DRM_DEBUG_KMS("dc%d\n", dc->id);
	return lombo_dpu_clk_disable_unprepare(dc->mod_clk);
}

static inline int _dc_resume(struct lombo_dc *dc)
{
	struct lombo_drv *drv = dc->drm->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	uint8_t sw = 0;

	if (bd && (bd->dc_id == dc->id))
		sw = 1;
	DRM_DEBUG_KMS("dc%d, sw=%d\n", dc->id, sw);
	return lombo_dpu_clk_prepare_enable(dc->mod_clk, sw);
}

static inline void lombo_dc_make_backcolor(
	dpu_color_t *color, uint32_t c)
{
	color->a = (uint8_t)(c >> 24);
	color->r = (uint8_t)(c >> 16);
	color->g = (uint8_t)(c >> 8);
	color->b = (uint8_t)c;
}

int lombo_dc_mode_fixup(struct lombo_dc *dc,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{

	DRM_DEBUG_KMS("scn[%ux%u@%u]\n",
		      mode->hdisplay, mode->vdisplay, mode->vrefresh);

	dc->scn_w = mode->hdisplay;
	dc->scn_h = mode->vdisplay;
	dc->scn_fps = mode->vrefresh;

	dc->scn_fmt = DPU_FMT_RGB888;
	if (dc->cfg->no_csc && dc->cfg->support_yuv)
		dc->scn_fmt = DPU_FMT_YUV444P;

	return 0;
}
EXPORT_SYMBOL(lombo_dc_mode_fixup);

/* get dc output resolution */
int lombo_dc_get_scn(struct lombo_dc *dc, int *width, int *height)
{
	if (width)
		*width = dc->scn_w;
	if (height)
		*height = dc->scn_h;

	return 0;
}
EXPORT_SYMBOL(lombo_dc_get_scn);

int lombo_dc_get_csfmt(struct lombo_dc *dc)
{
	switch (dc->scn_fmt) {
	case DPU_FMT_YUV444P:
		return LOMBO_CSFMT_YUV;
	case DPU_FMT_RGB888:
		return LOMBO_CSFMT_RGB;
	default:
		DRM_INFO("unknow scn fmt %d\n", dc->scn_fmt);
		return LOMBO_CSFMT_RGB;
	}
}
EXPORT_SYMBOL(lombo_dc_get_csfmt);

int lombo_dc_set_csfmt(struct lombo_dc *dc, int csfmt)
{
	if ((dc->cfg->no_csc && dc->cfg->support_yuv)
		&& (csfmt != LOMBO_CSFMT_YUV)) {
		DRM_INFO("dc%d not support yuv out\n", dc->id);
		return -ENOTSUPP;
	}

	switch (csfmt) {
	case LOMBO_CSFMT_RGB:
		dc->scn_fmt = DPU_FMT_RGB888;
		return 0;
	case LOMBO_CSFMT_YUV:
		dc->scn_fmt = DPU_FMT_YUV444P;
		return 0;
	default:
		DRM_INFO("unsupport csfmt %d\n", csfmt);
		return -ENOTSUPP;
	}
}
EXPORT_SYMBOL(lombo_dc_set_csfmt);

int lombo_dc_set_dpms(struct lombo_dc *dc, int dpms)
{
	if (dpms)
		return _dc_resume(dc);
	else
		return _dc_suspend(dc);
}
EXPORT_SYMBOL(lombo_dc_set_dpms);

static int lombo_dc_color_enhance_init(struct lombo_dc *dc)
{
	int ret = 0;
	dc_enhance_info_t *ce = &dc->ce;

	/* set fmt is enough, other parameters is initiated in lombo_dc_bind */
	ce->fmt = dc->scn_fmt;

	ret = csp_dc_set_enhancement(dc->id, ce);
	if (ret)
		DRM_ERROR("csp_dc_set_enhancement err ret[%d]\n", ret);

	return ret;
}

static int lombo_dc_enable(struct lombo_dc *dc,
	struct drm_framebuffer *fb)
{
	dpu_color_t bk_color;

	DRM_DEBUG_KMS("fb=%p\n", fb);

	csp_dc_init(dc->id);

	csp_dc_set_screen_size(dc->id, dc->scn_w, dc->scn_h);

	if (dc->scn_fmt == DPU_FMT_YUV444P)
		lombo_dc_make_backcolor(&bk_color, 0xFF008080);
	else
		lombo_dc_make_backcolor(&bk_color, 0xFF000000);
	csp_dc_set_backcolor(dc->id, &bk_color);

	lombo_dc_color_enhance_init(dc);

	csp_dc_enable(dc->id);

	return 0;
}

static struct drm_plane *lombo_dc_pickup_primary_plane(
	struct lombo_dc *dc, struct drm_framebuffer *fb)
{
	struct drm_mode_config *mode_config = &dc->drm->mode_config;
	struct drm_fb_helper *fb_helper = mode_config->fb_helper;
	uint32_t id = 0;

	if (fb_helper && fb_helper->fb == fb) {
		struct lombo_drv *drv = dc->drm->dev_private;

		DRM_DEBUG_KMS("drv->fb_win_id=%d, dc->fbdev_win_id=%d\n",
			      drv->fb_win_id, dc->fbdev_win_id);
		if ((drv->fb_win_id >= 0) &&
			(drv->fb_win_id < dc->cfg->win_num))
			id = drv->fb_win_id;
		else
			id = dc->fbdev_win_id;
	}

	if (id >= dc->cfg->win_num) {
		DRM_ERROR("bad win id %u\n", id);
		return NULL;
	}
	return dc->planes[id];
}

int lombo_dc_set_mode_base(struct lombo_dc *dc,
	struct drm_crtc *crtc,
	int x, int y, struct drm_framebuffer *old_fb)
{
	struct drm_framebuffer *fb = crtc->fb;
	struct drm_plane *primary;
	uint32_t width, height;
	dpu_color_t color;
	int ret;

	if (!fb || !old_fb) {
		DRM_ERROR("fb=%p, old_fb=%p\n", fb, old_fb);
		return -EINVAL;
	}

	if (fb->modifier) {
		lombo_dc_make_backcolor(&color, lombo_fb_sc_get_color(fb, 0));
		DRM_DEBUG_KMS("color=0x%8x\n", *(uint32_t *)&color);
		csp_dc_set_backcolor(dc->id, &color);

		if (old_fb->modifier)
			return 0;

		primary = lombo_dc_pickup_primary_plane(dc, fb);
		if (!primary)
			return -EINVAL;
		primary->funcs->disable_plane(primary);
		drm_framebuffer_unreference(old_fb);

		return 0;
	}

	primary = lombo_dc_pickup_primary_plane(dc, fb);
	if (!primary)
		return -EINVAL;

	width = crtc->mode.hdisplay;
	height = crtc->mode.vdisplay;
	DRM_DEBUG_KMS("x=%d,y=%d,w=%d,h=%d\n",
		x, y, width, height);
	ret = primary->funcs->update_plane(primary, crtc, fb,
				0, 0, width, height,
				x << 16, y << 16, width << 16, height << 16);
	if (ret) {
		DRM_ERROR("update primary plane failed\n");
		return ret;
	}
	drm_framebuffer_reference(fb);
	primary->fb = fb;
	primary->crtc = crtc;

	if (old_fb->modifier) {
		lombo_dc_make_backcolor(&color, lombo_fb_sc_get_color(fb, 0));
		DRM_DEBUG_KMS("color=0x%8x\n", *(uint32_t *)&color);
		csp_dc_set_backcolor(dc->id, &color);
	} else {
		drm_framebuffer_unreference(old_fb);
	}

	return 0;
}
EXPORT_SYMBOL(lombo_dc_set_mode_base);

int lombo_dc_set_mode(struct lombo_dc *dc,
	struct drm_crtc *crtc,
	struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode,
	int x, int y,
	struct drm_framebuffer *old_fb)
{
	struct lombo_drv *drv = dc->drm->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct drm_framebuffer *fb = crtc->fb;
	struct drm_plane *primary;
	uint32_t width, height;
	int ret;
	uint8_t sw = 0;

	if (!fb || old_fb) {
		DRM_ERROR("fixme: fb=%p, old_fb=%p\n", fb, old_fb);
		return -EINVAL;
	}

	if (bd && (bd->dc_id == dc->id))
		sw = 1;

	if (!sw)
		lombo_dc_enable(dc, fb);

	if (fb->modifier == DRM_FORMAT_MOD_LOMBO_SC) {
		if (!sw) {
			dpu_color_t c;

			lombo_dc_make_backcolor(&c,
				lombo_fb_sc_get_color(fb, 0));
			csp_dc_set_backcolor(dc->id, &c);
		}
		return 0;
	}

	primary = lombo_dc_pickup_primary_plane(dc, fb);
	if (!primary)
		return -EINVAL;

	width = crtc->mode.hdisplay;
	height = crtc->mode.vdisplay;
	DRM_DEBUG_KMS("x=%d,y=%d,w=%d,h=%d\n", x, y, width, height);
	ret = primary->funcs->update_plane(primary, crtc, fb,
				0, 0, width, height,
				x << 16, y << 16, width << 16, height << 16);
	if (!ret) {
		drm_framebuffer_reference(fb);
		primary->fb = fb;
		primary->crtc = crtc;
	}

	if (sw) {
		if (primary != dc->planes[bd->win_id]) {
			bd->boot_plane = dc->planes[bd->win_id];
			DRM_DEBUG("mask boot plane of win%d\n", bd->win_id);
		}
	}

	return ret;
}
EXPORT_SYMBOL(lombo_dc_set_mode);

int lombo_dc_set_color_enhance(struct lombo_dc *dc,
	uint32_t enhance_enable, uint32_t brightness,
	uint32_t saturation, uint32_t contrast)
{
	int ret;
	dc_enhance_info_t *ce = &dc->ce;

	ce->enhance_enable = enhance_enable;
	ce->brightness = brightness;
	ce->saturation = saturation;
	ce->contrast = contrast;

	if (ce->enhance_enable != 0)
		ce->enhance_enable = 1;

	if (ce->brightness > DC_COLOR_ENHANCE_MAX_VALUE)
		ce->brightness = DC_COLOR_ENHANCE_MAX_VALUE;

	if (ce->brightness > DC_COLOR_ENHANCE_MAX_VALUE)
		ce->saturation = DC_COLOR_ENHANCE_MAX_VALUE;

	if (ce->brightness > DC_COLOR_ENHANCE_MAX_VALUE)
		ce->contrast = DC_COLOR_ENHANCE_MAX_VALUE;

	ret = csp_dc_set_enhancement(dc->id, ce);
	if (ret)
		DRM_ERROR("csp_dc_set_enhancement err ret[%d]\n", ret);

	return ret;
}
EXPORT_SYMBOL(lombo_dc_set_color_enhance);

int lombo_dc_cursor_set(struct lombo_dc *dc, struct lombo_cursor *cursor)
{
	DRM_DEBUG_KMS("\n");

	if (cursor->cma_obj) {
		csp_dc_blocklinker_set_format(dc->id,
			lombo_dc_convert_drm_format(cursor->format));
		csp_dc_blocklinker_set_alpha_mode(dc->id,
			DPU_PLANE_PIXEL_ALPHA);
		csp_dc_blocklinker_set_alpha_value(dc->id, 0xFF);
		csp_dc_blocklinker_set_blend_rule(dc->id,
			0, DPU_BLEND_SRC_OVER);
		csp_dc_blocklinker_enable(dc->id);
		dc->cursor = cursor;
		dc->cursor_pending = LOMBO_CURSOR_PENDING_ENABLE;
	} else {
		csp_dc_blocklinker_disable(dc->id);
		dc->cursor_pending = LOMBO_CURSOR_PENDING_DISABLE;
	}

	return 0;
}
EXPORT_SYMBOL(lombo_dc_cursor_set);

int lombo_dc_cursor_move(struct lombo_dc *dc, struct lombo_cursor *cursor)
{
	DRM_DEBUG_KMS("\n");

	dc->cursor = cursor;
	dc->cursor_pending = LOMBO_CURSOR_PENDING_MOVE;

	return 0;
}
EXPORT_SYMBOL(lombo_dc_cursor_move);

void lombo_dc_disable(struct lombo_dc *dc, struct drm_crtc *crtc)
{
	struct drm_framebuffer *fb = crtc->fb;
	struct drm_plane *primary = dc->planes[0];
	int ret = 0;

	DRM_DEBUG_KMS("\n");

	if (fb == primary->fb) {
		DRM_DEBUG_KMS("primary plane disable\n");
		ret = primary->funcs->disable_plane(primary);
		if (ret)
			DRM_ERROR("disable primary plane fail\n");
		drm_framebuffer_unreference(primary->fb);
		primary->fb = NULL;
		primary->crtc = NULL;
	} else if (fb->modifier == DRM_FORMAT_MOD_LOMBO_SC) {
		dpu_color_t c;

		lombo_dc_make_backcolor(&c, 0x0);
		csp_dc_set_backcolor(dc->id, &c);
	}

	csp_dc_disable(dc->id);

	dc->scn_w = 0;
	dc->scn_h = 0;

}
EXPORT_SYMBOL(lombo_dc_disable);

int lombo_dc_update_wins(struct lombo_dc *dc)
{
	int i;

	for (i = 0; i < dc->cfg->win_num; i++)
		lombo_dc_win_update_win(to_lombo_dc_win(dc->planes[i]));

	return 0;
}
EXPORT_SYMBOL(lombo_dc_update_wins);

static inline struct drm_framebuffer *
lombo_dc_pending_rmfbs_dequeue_rmfb(
	struct lombo_pending_rmfbs *fbs)
{
	struct drm_framebuffer *fb = NULL;
	uint32_t fb_num, rm_num;
	unsigned long flags;

	spin_lock_irqsave(&fbs->lock, flags);
	if (fbs->rm_num > 0) {
		fb = fbs->fb_array[fbs->fb_head];
		fbs->fb_array[fbs->fb_head] = NULL;
		fbs->fb_head++;
		fbs->fb_head %= fbs->array_size;
		fbs->fb_num--;
		fbs->rm_num--;
		fb_num = fbs->fb_num;
		rm_num = fbs->rm_num;
	}
	spin_unlock_irqrestore(&fbs->lock, flags);

	if (fb)
		DRM_DEBUG_KMS("After dequeue: fb_num=%d, rm_num=%d\n",
			fb_num, rm_num);
	return fb;
}

static inline int lombo_dc_pending_rmfbs_queue_fb(
	struct lombo_pending_rmfbs *fbs, struct drm_framebuffer *fb)
{
	int ret = 0;
	int id;
	uint32_t fb_num, rm_num;
	unsigned long flags;

	if (!fb)
		return 0;

	spin_lock_irqsave(&fbs->lock, flags);
	if (fbs->fb_num < fbs->array_size) {
		id = (fbs->fb_head + fbs->fb_num) % fbs->array_size;
		fbs->fb_array[id] = fb;
		fbs->fb_num++;
		fb_num = fbs->fb_num;
		rm_num = fbs->rm_num;
		/**
		 * maybe todo more:
		 * find the same fb in pending-rmfb(but not rm-fb);
		 * if found, unreferent it immediatly,
		 * and donot need to queue it.
		 */
	} else
		ret = -EBUSY;
	spin_unlock_irqrestore(&fbs->lock, flags);

	if (ret)
		DRM_ERROR("queue pd-rmfb failed, ret=%d\n", ret);
	else
		DRM_DEBUG_KMS("After queue: fb_num=%d, rm_num=%d\n",
			fb_num, rm_num);
	return ret;

}

/* queue fb into pending_rmfbs. this func is for dc win to call */
int lombo_dc_queue_rmfb(struct lombo_dc *dc, struct drm_framebuffer *fb)
{
	return lombo_dc_pending_rmfbs_queue_fb(&dc->pending_rmfbs, fb);
}

/* modify all fbs from pending_fb to rm_fb */
static inline void lombo_dc_commit_rmfbs(struct lombo_dc *dc)
{
	struct lombo_pending_rmfbs *fbs = &dc->pending_rmfbs;
	unsigned long flags;

	spin_lock_irqsave(&fbs->lock, flags);
	fbs->rm_num = fbs->fb_num;
	spin_unlock_irqrestore(&fbs->lock, flags);
}

static int lombo_dc_commit_cursor(struct lombo_dc *dc)
{
	struct lombo_cursor *cursor = dc->cursor;
	dpu_rectangle_t rect;

	if (dc->cursor_pending == LOMBO_CURSOR_PENDING_ENABLE) {
		dpu_fb_t fb;

		fb.addr[0] = cursor->cma_obj->paddr;
		fb.addr[0] <<= 3;
		fb.format = lombo_dc_convert_drm_format(cursor->format);
		fb.pixel_order = 0;
		fb.width[0] = cursor->width;
		fb.height[0] = cursor->height;
		fb.planes = 1;
		fb.linestride_bit[0] = cursor->width * 32;
		csp_dc_blocklinker_set_buffer(dc->id, 0, &fb);

		rect.x = cursor->x;
		rect.y = cursor->y;
		rect.w = cursor->width;
		rect.h = cursor->height;
		csp_dc_blocklinker_set_dest_rectangle(dc->id, 0, &rect);
		csp_dc_blocklinker_set_next(dc->id, 0, 0);
	} else if (dc->cursor_pending == LOMBO_CURSOR_PENDING_MOVE) {
		rect.x = cursor->x;
		rect.y = cursor->y;
		rect.w = cursor->width;
		rect.h = cursor->height;
		csp_dc_blocklinker_set_dest_rectangle(dc->id, 0, &rect);
	}
	dc->cursor_pending = 0;
	return 0;
}

static int lombo_dc_commit_blk(struct lombo_dc *dc)
{
	if ((dc->blk_pending == LOMBO_BLK_PENDING_ENABLE) ||
		(dc->blk_pending == LOMBO_BLK_PENDING_FB)) {
		int i;
		dpu_fb_t *dpu_fb;
		dpu_rectangle_t *dst_win;

		for (i = 0; i < dc->available_block_num; i++) {
			dpu_fb = &dc->block_fbs[i];
			dst_win = &dc->block_dst[i];
			csp_dc_blocklinker_set_format(dc->id, dpu_fb->format);
			csp_dc_blocklinker_set_buffer(dc->id, i, dpu_fb);
			csp_dc_blocklinker_set_dest_rectangle(dc->id,
							      i, dst_win);
			if (i < dc->available_block_num - 1)
				csp_dc_blocklinker_set_next(dc->id, i, i + 1);
			else
				csp_dc_blocklinker_set_next(dc->id, i, 0);
		}
	}

	if (dc->blk_pending == LOMBO_BLK_PENDING_ENABLE) {
		csp_dc_blocklinker_set_alpha_mode(dc->id,
					DPU_PLANE_PIXEL_ALPHA);
		csp_dc_blocklinker_set_alpha_value(dc->id, 0xFF);
		csp_dc_blocklinker_set_blend_rule(dc->id,
					0, DPU_BLEND_SRC_OVER);
		csp_dc_blocklinker_enable(dc->id);
	} else if (dc->blk_pending == LOMBO_BLK_PENDING_DISABLE) {
		csp_dc_blocklinker_disable(dc->id);
	}

	dc->blk_pending = 0;
	return 0;
}

/**
 * Commit all regs changed of dc and its front-components (se).
 * First update non-db-regs if needed, then auto-load db-regs.
 */
int lombo_dc_commit(struct lombo_dc *dc)
{
	int i;

	lombo_dc_commit_cursor(dc);
	lombo_dc_commit_blk(dc);

	for (i = 0; i < dc->cfg->win_num; i++)
		lombo_dc_win_commit(to_lombo_dc_win(dc->planes[i]));
	csp_dc_load_dbr(dc->id);

	lombo_dc_commit_rmfbs(dc);
	schedule_work(&dc->page_flip_work);

	return 0;
}
EXPORT_SYMBOL(lombo_dc_commit);

static int lombo_dc_create_plane_properties(
	struct lombo_dc *dc, struct drm_device *drm)
{
	struct lombo_drv *drv = drm->dev_private;
	struct drm_prop_enum_list enum_list[DRM_COLOR_ENCODING_MAX];
	int i, len;

	drv->prop_zpos = drm_property_create_range(drm, 0,
		"zpos", 0, dc->cfg->win_num - 1);
	if (!drv->prop_zpos) {
		DRM_ERROR("create zpos prop fail\n");
		return -ENOMEM;
	}

	/* create colorspace enum property */
	len = 0;
	for (i = 0; i < DRM_COLOR_ENCODING_MAX; i++) {
		enum_list[len].type = i;
		enum_list[len].name = (char *)drm_get_color_encoding_name(i);
		len++;
	}
	drv->prop_color_space = drm_property_create_enum(drm, 0,
		"COLOR_ENCODING", enum_list, len);
	if (!drv->prop_color_space) {
		DRM_ERROR("create colorspace prop fail\n");
		return -ENOMEM;
	}

	/* create color range enum property */
	WARN_ON((int)DRM_COLOR_RANGE_MAX > (int)DRM_COLOR_ENCODING_MAX);
	len = 0;
	for (i = 0; i < DRM_COLOR_RANGE_MAX; i++) {
		enum_list[len].type = i;
		enum_list[len].name = (char *)drm_get_color_range_name(i);
		len++;
	}
	drv->prop_color_range = drm_property_create_enum(drm, 0,
		"COLOR_RANGE", enum_list, len);
	if (!drv->prop_color_range) {
		DRM_ERROR("create colorspace prop fail\n");
		return -ENOMEM;
	}

	drv->prop_video_lti = drm_property_create_range(drm, 0,
		"video_lti", 0, LOMBO_VIDEO_MAX_LTI_VAL);
	if (!drv->prop_video_lti) {
		DRM_ERROR("create video lti prop fail\n");
		return -ENOMEM;
	}

	drv->prop_video_peak = drm_property_create_range(drm, 0,
		"video_peak", 0, LOMBO_VIDEO_MAX_PEAK_VAL);
	if (!drv->prop_video_peak) {
		DRM_ERROR("create video peak prop fail\n");
		return -ENOMEM;
	}

	return 0;
}

struct drm_plane **lombo_dc_planes_init(
	struct lombo_dc *dc, uint32_t pipe)
{
	struct drm_device *drm = dc->drm;
	struct drm_plane **planes;
	int ret, i;

	ret = lombo_dc_create_plane_properties(dc, drm);
	if (ret)
		return ERR_PTR(ret);

	planes = kcalloc(dc->cfg->win_num + dc->hw_cursor_num,
			sizeof(*planes), GFP_KERNEL);
	if (!planes) {
		DRM_ERROR("alloc for planes fail\n");
		return ERR_PTR(-ENOMEM);
	}

	for (i = 0; i < dc->cfg->win_num; ++i) {
		struct lombo_dc_win *win;

		win = lombo_dc_win_init(dc, i,
			dc->cfg->formats, dc->cfg->format_count,
			BIT(pipe));
		if (IS_ERR(win)) {
			DRM_ERROR("win %d init fail\n", i);
			return ERR_CAST(win);
		}
		planes[i] = &win->base;
	}
	dc->planes = planes;

	return planes;
}
EXPORT_SYMBOL(lombo_dc_planes_init);

static void lombo_dc_page_flip_worker(struct work_struct *work)
{
	struct lombo_dc *dc = container_of(work, struct lombo_dc,
						page_flip_work);
	struct drm_framebuffer *fb = NULL;
	int i;

	for (i = 0; i < dc->cfg->win_num; i++)
		lombo_dc_win_page_flip_worker(to_lombo_dc_win(dc->planes[i]));

	do {
		fb = lombo_dc_pending_rmfbs_dequeue_rmfb(&dc->pending_rmfbs);
		if (fb)
			drm_framebuffer_unreference(fb);
	} while (fb);
}

static char *lombo_dc_get_fmt_str(dpu_pixel_format_t fmt)
{
	switch (fmt) {
	case DPU_FMT_RGB888:
		return "RG24";
	case DPU_FMT_YUV444P:
		return "YU24";
	default:
		DRM_ERROR("unknow fmt 0x%x\n", fmt);
		return "invl";
	}
}

int lombo_dc_kms_show(struct lombo_dc *dc,
	struct seq_file *m, void *data)
{
	int i;
	struct drm_format_name_buf fmt_buf;
	char *alpha_mode_str[] = {
		"PX", /* pixel alpha mode */
		"PL", /* plane alpha mode */
		"PP", /* planel pixel alpha mode */
	};

	seq_printf(m, "DC(%d): output(%dx%d), outfmt(%s), clk=%luHz\n",
		lombo_dc_id(dc), dc->scn_w, dc->scn_h,
		lombo_dc_get_fmt_str(dc->scn_fmt),
		lombo_dpu_clk_get_real_rate(dc->mod_clk));

	seq_puts(m, "win fmt     W x H    offset_addr[0x]        pitch[0x]       source(x, y, w, h)   screen(x, y, w, h)  a(mode,val)\n");
	seq_puts(m, "---|----|-----------|-----------------|--------------------|--------------------|--------------------|----------\n");
	for (i = 0; i < dc->cfg->win_num; i++) {
		struct lombo_dc_win *win = to_lombo_dc_win(dc->planes[i]);
		struct drm_framebuffer *fb = win->fb;

		if (!fb)
			continue;

		drm_get_format_name(fb->format->format, &fmt_buf);
		fmt_buf.str[4] = 0;
		seq_printf(m, "%d   %s  %4dx%-4d [%02x,%06x,%06x][%6x,%6x,%6x](%4d,%4d,%4d,%4d)(%4d,%4d,%4d,%4d)(%s,0x%02x)\n",
			win->zpos, fmt_buf.str, fb->width, fb->height,
			fb->offsets[0], fb->offsets[1], fb->offsets[2],
			fb->pitches[0], fb->pitches[1], fb->pitches[2],
			win->src_x >> 16, win->src_y >> 16,
			win->src_w >> 16, win->src_h >> 16,
			win->crtc_x, win->crtc_y, win->crtc_w, win->crtc_h,
			(win->alpha_mode >= ARRAY_SIZE(alpha_mode_str)) ?
				"INVALID" : alpha_mode_str[win->alpha_mode],
			win->alpha_value);

		if (fb->offsets[0] & ~0xFF)
			DRM_INFO("win%d: fb-offsets[0]=0x%x\n",
				win->zpos, fb->offsets[0]);
		if (fb->offsets[1] & ~0xFFFFFF)
			DRM_INFO("win%d: fb-offsets[1]=0x%x\n",
				win->zpos, fb->offsets[1]);
		if (fb->offsets[2] & ~0xFFFFFF)
			DRM_INFO("win%d: fb-offsets[2]=0x%x\n",
				win->zpos, fb->offsets[2]);
		if (fb->pitches[0] & ~0xFFFFFF)
			DRM_INFO("win%d: fb-pitches[0]=%d\n",
				win->zpos, fb->pitches[0]);
		if (fb->pitches[1] & ~0xFFFFFF)
			DRM_INFO("win%d: fb-pitches[1]=%d\n",
				win->zpos, fb->pitches[1]);
		if (fb->pitches[2] & ~0xFFFFFF)
			DRM_INFO("win%d: fb-pitches[2]=%d\n",
				win->zpos, fb->pitches[2]);
	}

	return 0;
}
EXPORT_SYMBOL(lombo_dc_kms_show);

static inline int fb_contain_rect(const u32 fb_w, const u32 fb_h,
	const struct drm_lombo_rect_wh *r)
{
	if (r->x < 0 ||
	    r->y < 0 ||
	    r->w == 0 ||
	    r->h == 0 ||
	    r->w > fb_w ||
	    r->x > fb_w - r->w ||
	    r->h > fb_h ||
	    r->y > fb_h - r->h) {
		return 0;
	}
	return 1;
}

static int fb_cut(const u32 fb_w, const u32 fb_h,
	const struct drm_lombo_rect_wh *trp,
	struct drm_lombo_rect_wh *res, u32 *num)
{
	int cnt = 0;

	if (!fb_contain_rect(fb_w, fb_h, trp))
		return -EINVAL;

	if (*num == cnt)
		return 0;
	if (trp->y > 0) {
		res[cnt].x = 0;
		res[cnt].y = 0;
		res[cnt].w = fb_w;
		res[cnt].h = trp->y;
		cnt++;
	}

	if (*num == cnt)
		return 0;
	if (trp->x > 0) {
		res[cnt].x = 0;
		res[cnt].y = trp->y;
		res[cnt].w = trp->x;
		res[cnt].h = trp->h;
		cnt++;
	}

	if (*num == cnt)
		return 0;
	if (trp->x + trp->w < fb_w) {
		res[cnt].x = trp->x + trp->w;
		res[cnt].y = trp->y;
		res[cnt].w = fb_w - trp->x - trp->w;
		res[cnt].h = trp->h;
		cnt++;
	}

	if (*num == cnt)
		return 0;
	if (trp->y + trp->h < fb_h) {
		res[cnt].x = 0;
		res[cnt].y = trp->y + trp->h;
		res[cnt].w = fb_w;
		res[cnt].h = fb_h - trp->y - trp->h;
		cnt++;
	}

	*num = cnt;

	return 0;
}

static struct lombo_dc_win *lombo_dc_find_primary_win(
	struct lombo_dc *dc, struct drm_framebuffer *fb)
{
	struct drm_plane *plane;
	int i;

	for (i = 0; i < dc->cfg->win_num; i++) {
		plane = dc->planes[i];
		if (plane && plane->fb && (plane->fb == fb))
			return to_lombo_dc_win(plane);
	}

	DRM_ERROR("%s: no primary win\n", __func__);
	return NULL;
}

int lombo_dc_set_fb_transparent_rect(struct lombo_dc *dc,
	struct drm_framebuffer *fb, struct drm_lombo_rect_wh *rect)
{
	struct lombo_dc_win *win;
	int ret = 0;
	int num = 0;
	int i;

	if (!dc || !fb)
		return -EINVAL;

	win = lombo_dc_find_primary_win(dc, fb);
	if (!win)
		return -EINVAL;

	num = ARRAY_SIZE(win->opaque);
	ret = fb_cut(fb->width, fb->height, rect, win->opaque, &num);

	if (ret == -EINVAL) {
		win->opaque_num = -1;
		memset(win->opaque, 0, sizeof(win->opaque));
		memset(&win->trp, 0, sizeof(win->trp));
		return 0;
	} else if (ret) {
		DRM_ERROR("fb cut fail, ret=%d\n", ret);
		return ret;
	}

	win->opaque_num = num;
	memcpy(&win->trp, rect, sizeof(win->trp));
	DRM_DEBUG_KMS("win%d, trp[%d,%d,%d,%d] in fb[%d,%d]\n", win->zpos,
		rect->x, rect->y, rect->w, rect->h, fb->width, fb->height);
	for (i = 0; i < num; i++) {
		rect = &win->opaque[i];
		DRM_DEBUG_KMS("opaque(%d)[%d,%d,%d,%d]\n",
			i, rect->x, rect->y, rect->w, rect->h);
	}

	return ret;
}
int lombo_dc_get_fb_transparent_rect(struct lombo_dc *dc,
	struct drm_framebuffer *fb, struct drm_lombo_rect_wh *rect)
{
	struct lombo_dc_win *win;

	if (!dc || !fb)
		return -EINVAL;

	win = lombo_dc_find_primary_win(dc, fb);
	if (!win)
		return -EINVAL;

	memcpy(rect, &win->trp, sizeof(*rect));
	return 0;
}

int lombo_dc_update_blk_for_win(struct lombo_dc *dc,
	struct lombo_dc_win *win, struct drm_framebuffer *fb)
{
	int i;
	dpu_fb_t *dpu_fb;
	dpu_rectangle_t *dst_win;
	struct drm_lombo_rect_wh *rect;
	struct drm_gem_cma_object *cma_obj;
	dma_addr_t paddr;
	unsigned int cpp; /* bytes per pixel */
	unsigned int pitch;

	cpp = fb->format->cpp[0];
	pitch = fb->pitches[0];
	cma_obj = drm_fb_cma_get_gem_obj(fb, 0);
	paddr = cma_obj->paddr + fb->offsets[0] +
		pitch * (win->src_y >> 16) +
		cpp * (win->src_x >> 16);

	DRM_DEBUG_KMS("offset=%d, pitch=%d, cpp=%d, src_x=%d, src_y=%d\n",
		fb->offsets[0], pitch, cpp,
		(win->src_x >> 16), (win->src_y >> 16));
	DRM_DEBUG_KMS("paddr=0x%llx, cma_paddr=0x%llx\n",
		(u64)paddr, (u64)cma_obj->paddr);

	for (i = 0; i < win->opaque_num; i++) {
		rect = &win->opaque[i];
		dpu_fb = &dc->block_fbs[i];
		dst_win = &dc->block_dst[i];

		dpu_fb->format = lombo_dc_convert_drm_format(
							fb->format->format);
		dpu_fb->pixel_order = DPU_PO_NORMAL;
		dpu_fb->linestride_bit[0] = pitch << 3;
		dpu_fb->planes = 1;

		dpu_fb->width[0] = rect->w;
		dpu_fb->height[0] = rect->h;
		dpu_fb->addr[0] = paddr + pitch * rect->y + cpp * rect->x;
		dpu_fb->addr[0] <<= 3;

		dst_win->x = rect->x;
		dst_win->y = rect->y;
		dst_win->w = rect->w;
		dst_win->h = rect->h;
		DRM_DEBUG_KMS("i=%d, rect[%d,%d,%d,%d], fmt=%x, addr=%llx\n",
			i, rect->x, rect->y, rect->w, rect->h,
			dpu_fb->format, dpu_fb->addr[0]);
	}
	dc->available_block_num = i;

	return 0;
}

static int lombo_dc_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = dev->of_node;
	struct drm_device *drm = master_data;
	struct lombo_drv *drv = drm->dev_private;

	struct device_node *top;
	struct platform_device *top_pdev;
	struct lombo_dc *dc;
	struct resource *res;
	uint32_t value;

	struct device_node *np_ce; /* node of color enhance */
	dc_enhance_info_t *ce;

	struct device_node *np_vs; /* node of video sharpness */

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	top = of_parse_phandle(dev->of_node, "top", 0);
	if (!top) {
		DRM_ERROR("failed to parse phandle of top\n");
		return -EINVAL;
	}
	top_pdev = of_find_device_by_node(top);
	if (!top_pdev) {
		DRM_ERROR("failed to find device dpu top\n");
		return -EINVAL;
	}

	dc = kzalloc(sizeof(*dc), GFP_KERNEL);
	if (!dc) {
		dev_err(dev, "alloc for dc failed.\n");
		return -ENOMEM;
	}
	dc->dev = dev;
	dc->dpu_top = &top_pdev->dev;
	dc->drm = drm;

	/* get cfg */
	dc->cfg = of_device_get_match_data(dev);
	if (!dc->cfg) {
		dev_err(dev, "failed to get dc cfg\n");
		return -EINVAL;
	}

	/* parse hardware index */
	if (!of_property_read_u32(np, "hw_id", &value))
		dc->id = value;
	else
		DRM_INFO("get hw_id failed\n");
	DRM_DEBUG_KMS("dc id=%d\n", dc->id);

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	dc->reg = devm_ioremap(dev, res->start,
			resource_size(res));
	if (dc->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_dc_set_register_base(dc->id, (unsigned long)dc->reg,
		(u32)resource_size(res));

	/* init clock */
	dc->mod_clk = lombo_dpu_get_clk(dev, dc->dpu_top, dc->id);
	if (!dc->mod_clk) {
		DRM_ERROR("lombo_dpu_get_clk for dc fail\n");
		return -EINVAL;
	}
	if (!of_property_read_u32(np, "sclk", &value))
		lombo_dpu_clk_set_parent(dc->mod_clk, value);
	else
		lombo_dpu_clk_set_parent(dc->mod_clk, 0);
	if (!of_property_read_u32(np, "clk_rate", &value))
		lombo_dpu_clk_set_rate(dc->mod_clk, value);
	else
		DRM_ERROR("failed to of get dc clk_rate\n");

	if (!of_property_read_u32(np, "fbdev_win_id", &value)) {
		dc->fbdev_win_id = value;
		if ((dc->fbdev_win_id >= dc->cfg->win_num)
			|| (dc->fbdev_win_id < 0))
			dc->fbdev_win_id = 2;
	} else
		dc->fbdev_win_id = 2; /* this is a default value */

	/* color enhance */
	ce = &dc->ce;
	np_ce = of_get_child_by_name(np, "color_enhance");
	if (!IS_ERR_OR_NULL(np_ce)
		&& of_device_is_available(np_ce)) {
		ce->enhance_enable = 1;

		ce->fmt = DPU_FMT_RGB888;
		ce->range = DPU_DATA_FULL_RANGE;
		ce->csstd = DPU_CS_BT601;

		if (!of_property_read_u32(np_ce, "brightness", &value)) {
			if (value > DC_COLOR_ENHANCE_MAX_VALUE) {
				DRM_ERROR("max brightness only support %d\n",
					DC_COLOR_ENHANCE_MAX_VALUE);
				value = DC_COLOR_ENHANCE_MAX_VALUE;
			}
			ce->brightness = value;
		} else
			ce->brightness = DC_COLOR_ENHANCE_MAX_VALUE / 2;

		if (!of_property_read_u32(np_ce, "saturation", &value)) {
			if (value > DC_COLOR_ENHANCE_MAX_VALUE) {
				DRM_ERROR("max saturation only support %d\n",
					DC_COLOR_ENHANCE_MAX_VALUE);
				value = DC_COLOR_ENHANCE_MAX_VALUE;
			}
			ce->saturation = value;
		} else
			ce->saturation = DC_COLOR_ENHANCE_MAX_VALUE / 2;

		if (!of_property_read_u32(np_ce, "contrast", &value)) {
			if (value > DC_COLOR_ENHANCE_MAX_VALUE) {
				DRM_ERROR("max contrast only support %d\n",
					DC_COLOR_ENHANCE_MAX_VALUE);
				value = DC_COLOR_ENHANCE_MAX_VALUE;
			}
			ce->contrast = value;
		} else
			ce->contrast = DC_COLOR_ENHANCE_MAX_VALUE / 2;

		of_node_put(np_ce);
	} else {
		ce->enhance_enable = 0;

		ce->fmt = DPU_FMT_RGB888;
		ce->range = DPU_DATA_FULL_RANGE;
		ce->csstd = DPU_CS_BT601;

		ce->brightness = DC_COLOR_ENHANCE_MAX_VALUE / 2;
		ce->saturation = DC_COLOR_ENHANCE_MAX_VALUE / 2;
		ce->contrast = DC_COLOR_ENHANCE_MAX_VALUE / 2;
	}

	/* video sharpness */
	np_vs = of_get_child_by_name(np, "video_sharpness");
	if (!IS_ERR_OR_NULL(np_vs)
		&& of_device_is_available(np_vs)) {
		if (!of_property_read_u32(np_vs, "lti", &value)) {
			if (value > LOMBO_VIDEO_MAX_LTI_VAL) {
				DRM_ERROR("max lti only support %d\n",
					LOMBO_VIDEO_MAX_LTI_VAL);
				value = LOMBO_VIDEO_MAX_LTI_VAL;
			}
			dc->lti = value;
		} else
			dc->lti = 0;

		if (!of_property_read_u32(np_vs, "peak", &value)) {
			if (value > LOMBO_VIDEO_MAX_PEAK_VAL) {
				DRM_ERROR("max peak only support %d\n",
					LOMBO_VIDEO_MAX_PEAK_VAL);
				value = LOMBO_VIDEO_MAX_PEAK_VAL;
			}
			dc->peak = value;
		} else
			dc->peak = 0;

		of_node_put(np_vs);
	} else {
		dc->lti = 0;
		dc->peak = 0;
	}

	INIT_WORK(&dc->page_flip_work, lombo_dc_page_flip_worker);
	dc->pending_rmfbs.array_size =
		(uint32_t)ARRAY_SIZE(dc->pending_rmfbs.fb_array);
	spin_lock_init(&dc->pending_rmfbs.lock);

	dev_set_drvdata(dev, dc);

	list_add_tail(&dc->list, &drv->dc_list);

	return 0;
}

static void lombo_dc_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_dc *dc = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	list_del(&dc->list);
	lombo_dpu_put_clk(&dc->mod_clk);

	csp_dc_set_register_base(dc->id, 0, 0);
	kfree(dc->planes);
	kfree(dc);
}

static const struct component_ops lombo_dc_ops = {
	.bind	= lombo_dc_bind,
	.unbind	= lombo_dc_unbind,
};

static int lombo_dc_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_dc_ops);
}

static int lombo_dc_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_dc_ops);
	return 0;
}

static const uint32_t dc_win_formats[] = {

	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_RGBA8888,
	DRM_FORMAT_BGRA8888,

	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_RGBX8888,
	DRM_FORMAT_BGRX8888,

	DRM_FORMAT_RGB888,
	DRM_FORMAT_BGR888,

	DRM_FORMAT_RGB565,
	DRM_FORMAT_BGR565,

	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_ABGR1555,
	DRM_FORMAT_RGBA5551,
	DRM_FORMAT_BGRA5551,

	DRM_FORMAT_XRGB1555,
	DRM_FORMAT_XBGR1555,
	DRM_FORMAT_RGBX5551,
	DRM_FORMAT_BGRX5551,

	DRM_FORMAT_ARGB4444,
	DRM_FORMAT_ABGR4444,
	DRM_FORMAT_RGBA4444,
	DRM_FORMAT_BGRA4444,

	DRM_FORMAT_XRGB4444,
	DRM_FORMAT_XBGR4444,
	DRM_FORMAT_RGBX4444,
	DRM_FORMAT_BGRX4444,

	/* support yuv due to se */

	DRM_FORMAT_NV12,
	DRM_FORMAT_NV21,
	DRM_FORMAT_NV16,
	DRM_FORMAT_NV61,

	DRM_FORMAT_YUV420,
	DRM_FORMAT_YVU420,
	DRM_FORMAT_YUV422,
	DRM_FORMAT_YVU422,

	DRM_FORMAT_YUYV,
	DRM_FORMAT_YVYU,
	DRM_FORMAT_UYVY,
	DRM_FORMAT_VYUY,
};

static const uint32_t minidc_win_formats[] = {
	DRM_FORMAT_AYUV,
	DRM_FORMAT_NV12,
	DRM_FORMAT_NV21,
	DRM_FORMAT_NV16,
	DRM_FORMAT_NV61,
};

static const struct lombo_dc_cfg dc_cfg = {
	.win_num = 4,
	.format_count = ARRAY_SIZE(dc_win_formats),
	.formats = dc_win_formats,
	.support_yuv = 0,
	.canot_use_se = 0,
	.no_csc = 0,
};

static const struct lombo_dc_cfg minidc_cfg = {
	.win_num = 1,
	.format_count = ARRAY_SIZE(minidc_win_formats),
	.formats = minidc_win_formats,
	.support_yuv = 1,
	.canot_use_se = 1,
	.no_csc = 1,
};

static const struct of_device_id lombo_dc_of_table[] = {
	{
		.compatible = "lombo,lombo-n7v0-dc-0",
		.data = &dc_cfg,
	},
	{
		.compatible = "lombo,lombo-n7v1-dc-0",
		.data = &dc_cfg,
	},
	{
		.compatible = "lombo,lombo-n8v0-dc-0",
		.data = &dc_cfg,
	},
	{
		.compatible = "lombo,lombo-n8v0-dc-1",
		.data = &minidc_cfg,
	},
	{
		.compatible = "lombo,lombo-n9v0-dc-0",
		.data = &dc_cfg,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_dc_of_table);

static struct platform_driver lombo_dc_platform_driver = {
	.probe		= lombo_dc_probe,
	.remove		= lombo_dc_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-dc",
		.of_match_table	= lombo_dc_of_table,
	},
};
module_platform_driver(lombo_dc_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU DC Driver");
MODULE_LICENSE("GPL");

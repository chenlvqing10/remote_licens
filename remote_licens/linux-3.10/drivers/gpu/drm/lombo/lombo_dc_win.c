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

#include <drm/drmP.h>
#include <drm/drm_color_mgmt.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>

#include <drm/lombo_drm.h>
#include "lombo_drv.h"
#include "lombo_crtc.h"
#include "lombo_dpu.h"
#include "lombo_dc.h"
#include "lombo_dc_win.h"
#include "lombo_ove.h"
#include "lombo_se.h"
#include "lombo_tcon.h"

#include "csp_dpu_dc.h"

#define DRM_BLEND_ALPHA_OPAQUE		0xffff

/**
 * lombo_dc_win_create_alpha_property - create a new alpha property
 * @win: dc win of lombo
 *
 * This function creates a generic, mutable, alpha property and enables support
 * for it in the DRM core. It is attached to @plane.
 *
 * The alpha property will be allowed to be within the bounds of 0
 * (transparent) to 0xffff (opaque).
 *
 * Returns:
 * 0 on success, negative error code on failure.
 */
static int lombo_dc_win_create_alpha_property(struct lombo_dc_win *win)
{
	struct drm_property *prop;

	prop = drm_property_create_range(win->base.dev, 0, "alpha",
					 0, DRM_BLEND_ALPHA_OPAQUE);
	if (!prop)
		return -ENOMEM;

	drm_object_attach_property(&win->base.base,
				   prop, DRM_BLEND_ALPHA_OPAQUE);
	win->alpha_property = prop;
	win->alpha_value = DRM_BLEND_ALPHA_OPAQUE;
	win->alpha_mode = DPU_PLANE_PIXEL_ALPHA;

	return 0;
}

static int lombo_dc_win_update_win_idma(struct lombo_dc_win *win,
		struct lombo_fb_cfg *fb_cfg)
{
	struct lombo_dc *dc = win->dc;
	dpu_fb_t win_fb;
	dpu_rectangle_t rect[3];
	struct drm_framebuffer *fb = fb_cfg->fb;
	const struct drm_format_info *fmt_info = fb->format;
	int i;

	DRM_DEBUG_KMS("win%d:opaque_num=%d\n", win->zpos, win->opaque_num);

	if (win->opaque_num == 0) {
		DRM_DEBUG_KMS("no opaque rect, disable both win&blk\n");
		if (win->use_blk) {
			dc->blk_pending = LOMBO_BLK_PENDING_DISABLE;
			win->use_blk = 0;
		}
		csp_dc_window_set_work_mode(dc->id, win->zpos,
					    DPU_WINDOW_NORMAL_MODE);
		csp_dc_window_set_data_source(dc->id, win->zpos,
					      DC_WINDOW_SRC_IDMA);
		csp_dc_window_set_alpha_mode(dc->id, win->zpos,
					     DPU_PIXEL_ALPHA);
		csp_dc_window_set_alpha_value(dc->id, win->zpos, 0);
		csp_dc_window_hide(dc->id, win->zpos);
		return 0;
	} else if (win->opaque_num > 0) {
		DRM_DEBUG_KMS("update_blk_for_win%d\n", win->zpos);
		if (win->use_blk)
			dc->blk_pending = LOMBO_BLK_PENDING_FB;
		else
			dc->blk_pending = LOMBO_BLK_PENDING_ENABLE;
		win->use_blk = 1;
		lombo_dc_update_blk_for_win(dc, win, fb);

		csp_dc_window_set_work_mode(dc->id, win->zpos,
					    DPU_WINDOW_NORMAL_MODE);
		csp_dc_window_set_data_source(dc->id, win->zpos,
					      DC_WINDOW_SRC_IDMA);
		csp_dc_window_set_alpha_mode(dc->id, win->zpos,
					     DPU_PIXEL_ALPHA);
		csp_dc_window_set_alpha_value(dc->id, win->zpos, 0);
		csp_dc_window_hide(dc->id, win->zpos);
		return 0;
	}

	if (win->use_blk) {
		DRM_DEBUG_KMS("blk pending disable\n");
		dc->blk_pending = LOMBO_BLK_PENDING_DISABLE;
		win->use_blk = 0;
	}

	csp_dc_window_set_data_source(dc->id, win->zpos, DC_WINDOW_SRC_IDMA);
	if (fmt_info->is_yuv)
		csp_dc_window_set_work_mode(dc->id, win->zpos,
					    DPU_WINDOW_YUV_MODE);
	else
		csp_dc_window_set_work_mode(dc->id, win->zpos,
					    DPU_WINDOW_NORMAL_MODE);

	/**
	 * XRGB are not the defined formats to dpu.
	 * the X of XRGB should be regraded as invalid.
	 * so plane_alpha is set for XRGB.
	 */
	if (!fmt_info->has_alpha)
		win->alpha_mode = DPU_PLANE_ALPHA; /* no-px-al */
	else if (win->alpha_value != DRM_BLEND_ALPHA_OPAQUE)
		win->alpha_mode = DPU_PLANE_PIXEL_ALPHA; /* px-al & gb-al */
	else
		win->alpha_mode = DPU_PIXEL_ALPHA; /* px-al & no-gb-al */
	csp_dc_window_set_alpha_mode(dc->id, win->zpos, win->alpha_mode);
	csp_dc_window_set_alpha_value(dc->id, win->zpos, win->alpha_value >> 8);
	csp_dc_window_set_blend_rule(dc->id, win->zpos,
				0, DPU_BLEND_SRC_OVER);

	memset((void *)&win_fb, 0, sizeof(win_fb));
	memset((void *)rect, 0, sizeof(rect));
	win_fb.format = lombo_dc_convert_drm_format(fmt_info->format);
	win_fb.pixel_order = DPU_PO_NORMAL;
	win_fb.planes = fmt_info->num_planes;
	for (i = 0; i < win_fb.planes; i++) {
		uint32_t x, y, w, h;
		struct drm_gem_cma_object *cma_obj;

		if (i > 0) {
			x = (fb_cfg->src_x >> 16) / fmt_info->hsub;
			y = (fb_cfg->src_y >> 16) / fmt_info->vsub;
			w = (fb_cfg->src_w >> 16) / fmt_info->hsub;
			h = (fb_cfg->src_h >> 16) / fmt_info->vsub;
			rect[i].x = fb_cfg->crtc_x / fmt_info->hsub;
			rect[i].y = fb_cfg->crtc_y / fmt_info->vsub;
			rect[i].w = fb_cfg->crtc_w / fmt_info->hsub;
			rect[i].h = fb_cfg->crtc_h / fmt_info->vsub;
		} else {
			x = fb_cfg->src_x >> 16;
			y = fb_cfg->src_y >> 16;
			w = fb_cfg->src_w >> 16;
			h = fb_cfg->src_h >> 16;
			rect[i].x = fb_cfg->crtc_x;
			rect[i].y = fb_cfg->crtc_y;
			rect[i].w = fb_cfg->crtc_w;
			rect[i].h = fb_cfg->crtc_h;
		}

		cma_obj = drm_fb_cma_get_gem_obj(fb, i);

		win_fb.addr[i] = cma_obj->paddr + fb->offsets[i]
			+ fb->pitches[i] * y + x * fmt_info->cpp[i];
		win_fb.addr[i] <<= 3; /* in bit unit */
		win_fb.width[i] = w;
		win_fb.height[i] = h;
		win_fb.linestride_bit[i] = fb->pitches[i] << 3;
	}
	csp_dc_window_set_buffer(dc->id, win->zpos, &win_fb);
	csp_dc_window_set_dest_rectangle(dc->id, win->zpos,
					 win_fb.planes, rect);

	csp_dc_window_show(dc->id, win->zpos);

	return 0;
}

static int lombo_dc_win_update_win_se(struct lombo_dc_win *win,
		struct lombo_fb_cfg *fb_cfg)
{
	struct lombo_dc *dc = win->dc;
	struct lombo_se *se = win->se;
	struct lombo_se_plane_cfg req_cfg;
	const struct drm_format_info *format;
	dpu_fb_t win_fb;
	dpu_rectangle_t rect = { 0 };
	uint32_t se_id;
	int ret;

	req_cfg.fb_cfg = fb_cfg;
	req_cfg.reqer = win;
	req_cfg.in_sel = LOMBO_SE_IN_SEL_DRAM;
	req_cfg.se_out_format = DRM_FORMAT_ARGB8888;
	req_cfg.se_run_mode = LOMBO_SE_RUN_MODE_ONLINE;
	req_cfg.scn_w = dc->scn_w;
	req_cfg.scn_h = dc->scn_h;
	req_cfg.scn_fps = dc->scn_fps;
	ret = lombo_se_update_plane(se, win, &req_cfg);
	if (ret)
		return ret;

	se_id = lombo_se_id(se);
	if (se_id == 0)
		csp_dc_window_set_data_source(dc->id, win->zpos,
					DC_WINDOW_SRC_SCALING0);
	else if (se_id == 1)
		csp_dc_window_set_data_source(dc->id, win->zpos,
					DC_WINDOW_SRC_SCALING1);
	else
		DRM_ERROR("bad se id %u\n", se_id);
	csp_dc_window_set_work_mode(dc->id, win->zpos, DPU_WINDOW_NORMAL_MODE);

	/**
	 * XRGB are not the defined formats to dpu.
	 * the X of XRGB should be regraded as invalid.
	 * so plane_alpha is set for XRGB.
	 */
	format =  drm_format_info(req_cfg.se_out_format);
	if (!format->has_alpha)
		win->alpha_mode = DPU_PLANE_ALPHA; /* no-px-al */
	else if (win->alpha_value != DRM_BLEND_ALPHA_OPAQUE)
		win->alpha_mode = DPU_PLANE_PIXEL_ALPHA; /* px-al & gb-al */
	else
		win->alpha_mode = DPU_PIXEL_ALPHA; /* px-al & no-gb-al */
	csp_dc_window_set_alpha_mode(dc->id, win->zpos, win->alpha_mode);
	csp_dc_window_set_alpha_value(dc->id, win->zpos, win->alpha_value >> 8);
	csp_dc_window_set_blend_rule(dc->id, win->zpos,
				0, DPU_BLEND_SRC_OVER);

	memset((void *)&win_fb, 0, sizeof(win_fb));
	win_fb.format = lombo_dc_convert_drm_format(req_cfg.se_out_format);
	win_fb.pixel_order = DPU_PO_NORMAL;
	win_fb.planes = 1;
	csp_dc_window_set_buffer(dc->id, win->zpos, &win_fb);

	rect.x = fb_cfg->crtc_x;
	rect.y = fb_cfg->crtc_y;
	rect.w = fb_cfg->crtc_w;
	rect.h = fb_cfg->crtc_h;
	csp_dc_window_set_dest_rectangle(dc->id, win->zpos, 1, &rect);

	csp_dc_window_show(dc->id, win->zpos);

	return 0;
}

static int lombo_dc_win_update_win_ove(struct lombo_dc_win *win,
		struct lombo_fb_cfg *fb_cfg)
{
	struct lombo_dc *dc = win->dc;
	struct lombo_ove_fb_cfg *req_cfg;
	const struct drm_format_info *format;
	dpu_fb_t dpu_fb;
	dpu_rectangle_t rect = { 0 };
	int ret = 0;

	req_cfg = &win->ove_fb_cfg;
	ret = lombo_ove_update_plane(win->ove, req_cfg);
	if (ret)
		return ret;

	csp_dc_window_set_data_source(dc->id, win->zpos, DC_WINDOW_SRC_OVE);
	csp_dc_window_set_work_mode(dc->id, win->zpos, DPU_WINDOW_NORMAL_MODE);

	/**
	 * XRGB are not the defined formats to dpu.
	 * the X of XRGB should be regraded as invalid.
	 * so plane_alpha is set for XRGB.
	 */
	format = drm_format_info(req_cfg->out_fmt);
	if (!format->has_alpha)
		win->alpha_mode = DPU_PLANE_ALPHA; /* no-px-al */
	else if (win->alpha_value != DRM_BLEND_ALPHA_OPAQUE)
		win->alpha_mode = DPU_PLANE_PIXEL_ALPHA; /* px-al & gb-al */
	else
		win->alpha_mode = DPU_PIXEL_ALPHA; /* px-al & no-gb-al */
	csp_dc_window_set_alpha_mode(dc->id, win->zpos, win->alpha_mode);
	csp_dc_window_set_alpha_value(dc->id, win->zpos, win->alpha_value >> 8);
	csp_dc_window_set_blend_rule(dc->id, win->zpos,
				0, DPU_BLEND_SRC_OVER);

	memset((void *)&dpu_fb, 0, sizeof(dpu_fb));
	dpu_fb.format = lombo_dc_convert_drm_format(req_cfg->out_fmt);
	dpu_fb.pixel_order = DPU_PO_NORMAL;
	dpu_fb.planes = 1;
	csp_dc_window_set_buffer(dc->id, win->zpos, &dpu_fb);

	rect.x = fb_cfg->crtc_x;
	rect.y = fb_cfg->crtc_y;
	rect.w = fb_cfg->crtc_w;
	rect.h = fb_cfg->crtc_h;
	csp_dc_window_set_dest_rectangle(dc->id, win->zpos, 1, &rect);

	csp_dc_window_show(dc->id, win->zpos);

	return 0;
}

static int lombo_dc_win_update_win_ove_se(struct lombo_dc_win *win,
		struct lombo_fb_cfg *fb_cfg)
{
	struct lombo_dc *dc = win->dc;
	struct lombo_se *se = win->se;
	struct lombo_ove *ove = win->ove;
	struct lombo_se_plane_cfg se_cfg;
	struct lombo_ove_fb_cfg *ove_cfg;
	const struct drm_format_info *format;
	dpu_fb_t win_fb;
	dpu_rectangle_t rect = { 0 };
	uint32_t se_id = lombo_se_id(se);
	int ret;

	ove_cfg = &win->ove_fb_cfg;
	ret = lombo_ove_update_plane(ove, ove_cfg);
	if (ret)
		return ret;

	memset((void *)&se_cfg, 0, sizeof(se_cfg));
	se_cfg.reqer = win;
	se_cfg.in_sel = LOMBO_SE_IN_SEL_DC;
	se_cfg.in_fmt = ove_cfg->out_fmt;
	se_cfg.se_run_mode = LOMBO_SE_RUN_MODE_ONLINE;
	se_cfg.se_out_format = DRM_FORMAT_ARGB8888;
	se_cfg.scn_w = dc->scn_w;
	se_cfg.scn_h = dc->scn_h;
	se_cfg.scn_fps = dc->scn_fps;
	se_cfg.fb_cfg = fb_cfg;
	ret = lombo_se_update_plane(se, win, &se_cfg);
	if (ret)
		return ret;

	csp_dc_window_set_data_source(dc->id, win->zpos,
				DC_WINDOW_SRC_SCALING0 + se_id);
	csp_dc_window_set_work_mode(dc->id, win->zpos, DPU_WINDOW_NORMAL_MODE);

	/**
	 * XRGB are not the defined formats to dpu.
	 * the X of XRGB should be regraded as invalid.
	 * so plane_alpha is set for XRGB.
	 */
	format = drm_format_info(se_cfg.se_out_format);
	if (!format->has_alpha)
		win->alpha_mode = DPU_PLANE_ALPHA; /* no-px-al */
	else if (win->alpha_value != DRM_BLEND_ALPHA_OPAQUE)
		win->alpha_mode = DPU_PLANE_PIXEL_ALPHA; /* px-al & gb-al */
	else
		win->alpha_mode = DPU_PIXEL_ALPHA; /* px-al & no-gb-al */
	csp_dc_window_set_alpha_mode(dc->id, win->zpos, win->alpha_mode);
	csp_dc_window_set_alpha_value(dc->id, win->zpos, win->alpha_value >> 8);
	csp_dc_window_set_blend_rule(dc->id, win->zpos,
				0, DPU_BLEND_SRC_OVER);

	memset((void *)&win_fb, 0, sizeof(win_fb));
	win_fb.format = lombo_dc_convert_drm_format(se_cfg.se_out_format);
	win_fb.pixel_order = DPU_PO_NORMAL;
	win_fb.planes = 1;
	csp_dc_window_set_buffer(dc->id, win->zpos, &win_fb);

	rect.x = fb_cfg->crtc_x;
	rect.y = fb_cfg->crtc_y;
	rect.w = fb_cfg->crtc_w;
	rect.h = fb_cfg->crtc_h;
	csp_dc_window_set_dest_rectangle(dc->id, win->zpos, 1, &rect);

	csp_dc_window_show(dc->id, win->zpos);

	return 0;
}


/* do update win data if need, for atomic pageflip flush */
int lombo_dc_win_update_win(struct lombo_dc_win *win)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(win->crtc);
	struct drm_framebuffer *fb;
	struct lombo_fb_cfg fb_cfg = { .fb = NULL, };
	int need_scale = 0;
	int ret = 0;

	if (!win->update_flag) {
		DRM_DEBUG_KMS("win not update dirty\n");
		return 0;
	}
	DRM_DEBUG_KMS("update_flag=0x%x\n", win->update_flag);

	need_scale = ((win->crtc_w << 16) != win->src_w)
		|| ((win->crtc_h << 16) != win->src_h);
	if (need_scale && win->dc->cfg->canot_use_se) {
		DRM_ERROR("dc%d canot use se to scale\n", win->dc->id);
		ret = -EINVAL;
		goto out;
	}

	fb = win->fb;
	if (!fb) {
		DRM_ERROR("null fb, flag=0x%x\n", win->update_flag);
		ret = -EINVAL;
		goto out;
	}

	fb_cfg.fb = win->fb;
	fb_cfg.crtc_x = win->crtc_x;
	fb_cfg.crtc_y = win->crtc_y;
	fb_cfg.crtc_w = win->crtc_w;
	fb_cfg.crtc_h = win->crtc_h;
	fb_cfg.src_x = win->src_x;
	fb_cfg.src_y = win->src_y;
	fb_cfg.src_w = win->src_w;
	fb_cfg.src_h = win->src_h;
	fb_cfg.color_space = win->color_space;
	fb_cfg.color_range = win->color_range;
	fb_cfg.video_lti = win->video_lti;
	fb_cfg.video_peak = win->video_peak;

	if ((fb->format->is_yuv && !win->dc->cfg->support_yuv) || need_scale) {
		/* need se */
		if (!win->se) {
			win->se = lombo_se_request(win->dc->drm, (void *)win);
			if (!win->se) {
				ret = -EBUSY;
				goto out;
			}
		}
		win->update_flag |= LOMBO_DC_WIN_UPDATE_BIT_SE;
		DRM_DEBUG_KMS("add se to flag 0x%x\n", win->update_flag);
	}

	if (fb->modifier == DRM_FORMAT_MOD_LOMBO_FB_ARRAY) {
		if (!win->ove) {
			win->ove_fb_cfg.reqer = (void *)win;
			if (win->update_flag & LOMBO_DC_WIN_UPDATE_BIT_SE)
				win->ove_fb_cfg.out_sel = LOMBO_OVE_OUT_SE0 +
					lombo_se_id(win->se);
			else
				win->ove_fb_cfg.out_sel = LOMBO_OVE_OUT_DC;
			win->ove_fb_cfg.fb_cfg = &fb_cfg;
			win->ove = lombo_ove_request(&win->ove_fb_cfg);
			if (!win->ove) {
				ret = -EBUSY;
				goto out;
			}
		}
		win->update_flag |= LOMBO_DC_WIN_UPDATE_BIT_OVE;
	}

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	if (ret)
		goto out;

	switch (win->update_flag) {
	case LOMBO_DC_WIN_UPDATE_FLAG_WIN:
		ret = lombo_dc_win_update_win_idma(win, &fb_cfg);
		break;
	case LOMBO_DC_WIN_UPDATE_FLAG_SE_WIN:
		ret = lombo_dc_win_update_win_se(win, &fb_cfg);
		win->commit_se = win->se;
		break;
	case LOMBO_DC_WIN_UPDATE_FLAG_OVE_WIN:
		ret = lombo_dc_win_update_win_ove(win, &fb_cfg);
		win->commit_ove = win->ove;
		break;
	case LOMBO_DC_WIN_UPDATE_FLAG_OVE_SE_WIN:
		ret = lombo_dc_win_update_win_ove_se(win, &fb_cfg);
		win->commit_ove = win->ove;
		win->commit_se = win->se;
		break;
	default:
		DRM_ERROR("invalid update flag 0x%x\n", win->update_flag);
	}

	if (win->se && !(win->update_flag & LOMBO_DC_WIN_UPDATE_BIT_SE)) {
		lombo_se_disable_plane(win->se, win);
		lombo_se_release(win->se, win, 0);
		win->commit_se = win->se;
		win->se = NULL;
	}

	if (win->ove &&	!(win->update_flag & LOMBO_DC_WIN_UPDATE_BIT_OVE)) {
		lombo_ove_disable_plane(win->ove, win);
		lombo_ove_release(win->ove, win, 0);
		win->commit_ove = win->ove;
		win->ove = NULL;
	}

	lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);

out:
	if (!ret) {
		if (win->fb)
			drm_framebuffer_reference(win->fb);
		if (win->old_fb) {
			if (lombo_dc_queue_rmfb(win->dc, win->old_fb))
				drm_framebuffer_unreference(win->old_fb);
			win->old_fb = NULL;
		}
	} else {
		win->fb = win->old_fb;
		win->old_fb = NULL;
		DRM_ERROR("fail. ret=%d\n", ret);
	}

	win->update_flag = 0;

	return ret;
}

static int lombo_dc_win_update_plane(struct drm_plane *plane,
		struct drm_crtc *crtc, struct drm_framebuffer *fb,
		int crtc_x, int crtc_y,
		unsigned int crtc_w, unsigned int crtc_h,
		uint32_t src_x, uint32_t src_y,
		uint32_t src_w, uint32_t src_h)
{
	struct lombo_dc_win *win = to_lombo_dc_win(plane);
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	int ret = 0;

	DRM_DEBUG_KMS("crtc[%d,%d,%d,%d],src[%d,%d,%d,%d],win_zpos=%d\n",
		crtc_x, crtc_y, crtc_w, crtc_h,
		src_x >> 16, src_y >> 16, src_w >> 16, src_h >> 16,
		win->zpos);

	if (!win->old_fb)
		win->old_fb = win->fb;
	else
		DRM_INFO("%s: win old_fb is not null\n", __func__);

	win->crtc = crtc;
	win->fb = fb;
	win->crtc_x = crtc_x;
	win->crtc_y = crtc_y;
	win->crtc_w = crtc_w;
	win->crtc_h = crtc_h;
	win->src_x = src_x;
	win->src_y = src_y;
	win->src_w = src_w;
	win->src_h = src_h;

	/* donot know whether need se now, so init flag_win only */
	win->update_flag = LOMBO_DC_WIN_UPDATE_FLAG_WIN;

	if (ecrtc->pfa_cache)
		return 0;

	ret = lombo_dc_win_update_win(win);
	if (ret)
		DRM_ERROR("failed\n");

	return ret;
}

static int lombo_dc_win_disable_plane(struct drm_plane *plane)
{
	struct lombo_dc_win *win = to_lombo_dc_win(plane);
	struct lombo_dc *dc = win->dc;
	struct lombo_tcon *tcon = dc->tcon;
	struct drm_framebuffer *fb = NULL;
	dpu_rectangle_t rect;
	int ret = 0;

	DRM_DEBUG_KMS("dc%d, win%d\n", dc->id, win->zpos);

	ret = lombo_tcon_protect_dbreg(tcon, dc, true);
	if (ret)
		return ret;

	if (win->se) {
		lombo_se_disable_plane(win->se, win);
		lombo_se_release(win->se, win, 0);
		win->commit_se = win->se;
		win->se = NULL;
	}

	if (win->ove) {
		lombo_ove_disable_plane(win->ove, win);
		lombo_ove_release(win->ove, win, 0);
		win->commit_ove = win->ove;
		win->ove = NULL;
	}

	/* just reset winctl-reg to default values */
	csp_dc_window_set_work_mode(dc->id, win->zpos, DPU_WINDOW_NORMAL_MODE);
	csp_dc_window_set_data_source(dc->id, win->zpos, DC_WINDOW_SRC_IDMA);
	csp_dc_window_set_alpha_mode(dc->id, win->zpos, DPU_PIXEL_ALPHA);
	csp_dc_window_set_alpha_value(dc->id, win->zpos, 0);

	memset((void *)&rect, 0, sizeof(rect));
	csp_dc_window_set_dest_rectangle(dc->id, win->zpos, 1, &rect);
	csp_dc_window_hide(dc->id, win->zpos);

	lombo_tcon_protect_dbreg(tcon, dc, false);

	fb = win->fb;
	if (fb) {
		if (lombo_dc_queue_rmfb(win->dc, fb))
			drm_framebuffer_unreference(fb);
		win->fb = NULL;
	}

	return 0;
}

static void lombo_dc_win_plane_cleanup(struct drm_plane *plane)
{
	struct lombo_dc_win *win = to_lombo_dc_win(plane);

	DRM_DEBUG_KMS("\n");

	drm_plane_cleanup(plane);
	kfree(win);
}

static int lombo_dc_win_set_property(struct drm_plane *plane,
	struct drm_property *prop, uint64_t val)
{
	struct lombo_dc_win *win = to_lombo_dc_win(plane);
	struct lombo_dc *dc = win->dc;
	struct lombo_drv *drv = dc->drm->dev_private;

	DRM_DEBUG_KMS("prop(%s)=%llu\n", prop->name, val);
	if (prop == win->alpha_property) {
		win->alpha_value = (uint16_t)val;
		DRM_INFO("alpha mode=%d, value=0x%x\n",
			 win->alpha_mode, win->alpha_value);
		if (win->alpha_value != DRM_BLEND_ALPHA_OPAQUE) {
			if (win->alpha_mode == DPU_PIXEL_ALPHA)
				win->alpha_mode = DPU_PLANE_PIXEL_ALPHA;
			csp_dc_window_set_alpha_mode(dc->id, win->zpos,
						     win->alpha_mode);
			csp_dc_window_set_alpha_value(dc->id, win->zpos,
						      win->alpha_value >> 8);
		} else {
			if (win->alpha_mode == DPU_PLANE_PIXEL_ALPHA)
				win->alpha_mode = DPU_PIXEL_ALPHA;
			csp_dc_window_set_alpha_mode(dc->id, win->zpos,
						     win->alpha_mode);
			csp_dc_window_set_alpha_value(dc->id, win->zpos,
						      win->alpha_value >> 8);
		}
	} else if (prop == drv->prop_zpos) {
		int zpos = (int)val;

		if ((zpos < 0) || (zpos >= dc->cfg->win_num))
			return -EINVAL;
		win->zpos = zpos;
	} else if (prop == drv->prop_color_space) {
		win->color_space = (int)val;
	} else if (prop == drv->prop_color_range) {
		win->color_range = (int)val;
	} else if (prop == drv->prop_video_lti) {
		win->video_lti = (int)val;
	} else if (prop == drv->prop_video_peak) {
		win->video_peak = (int)val;
	} else {
		DRM_ERROR("unknown prop(%s)\n", prop->name);
		return -EINVAL;
	}

	return 0;
}

static const struct drm_plane_funcs lombo_dc_win_plane_funcs = {
	.update_plane = lombo_dc_win_update_plane,
	.disable_plane = lombo_dc_win_disable_plane,
	.destroy = lombo_dc_win_plane_cleanup,
	.set_property = lombo_dc_win_set_property,
};

struct lombo_dc_win *lombo_dc_win_init(struct lombo_dc *dc, int id,
	const uint32_t *formats, uint32_t format_count,
	unsigned long possible_crtcs)
{
	struct drm_device *drm = dc->drm;
	struct lombo_drv *drv = drm->dev_private;
	struct lombo_dc_win *win;
	int ret;

	if (!formats || (format_count == 0)) {
		DRM_ERROR("bad formats. %p, %d\n", formats, format_count);
		return ERR_PTR(-EINVAL);
	}

	win = kzalloc(sizeof(*win), GFP_KERNEL);
	if (!win)
		return ERR_PTR(-ENOMEM);

	ret = drm_plane_init(drm, &win->base, possible_crtcs,
			&lombo_dc_win_plane_funcs,
			formats, format_count,
			false);
	if (ret) {
		DRM_ERROR("drm_plane_init fail, id=%d\n", id);
		return ERR_PTR(ret);
	}
	win->dc = dc;

	win->zpos = id;
	drm_object_attach_property(&win->base.base,
		drv->prop_zpos, win->zpos);

	win->color_space = DRM_COLOR_YCBCR_BT601;
	drm_object_attach_property(&win->base.base,
		drv->prop_color_space, win->color_space);
	win->color_range = DRM_COLOR_YCBCR_LIMITED_RANGE;
	drm_object_attach_property(&win->base.base,
		drv->prop_color_range, win->color_range);
	win->video_lti = dc->lti;
	drm_object_attach_property(&win->base.base,
		drv->prop_video_lti, win->video_lti);
	win->video_peak = dc->peak;
	drm_object_attach_property(&win->base.base,
		drv->prop_video_peak, win->video_peak);

	if (lombo_dc_win_create_alpha_property(win))
		DRM_ERROR("dc win%d create alpha prop failed\n", win->zpos);

	win->opaque_num = -1;

	return win;
}

void lombo_dc_win_commit(struct lombo_dc_win *win)
{
	if (win->commit_se)
		lombo_se_commit(win->commit_se, win);

	if (win->commit_ove)
		lombo_ove_commit(win->commit_ove, win);
}

int lombo_dc_win_page_flip_worker(struct lombo_dc_win *win)
{
	if (win->commit_se) {
		lombo_se_page_flip_worker(win->commit_se, win);
		win->commit_se = NULL;
	}

	if (win->commit_ove) {
		lombo_ove_page_flip_worker(win->commit_ove, win);
		win->commit_ove = NULL;
	}

	return 0;
}

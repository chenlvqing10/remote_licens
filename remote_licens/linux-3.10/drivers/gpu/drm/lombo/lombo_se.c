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
#include <drm/drm_gem_cma_helper.h>
#include <drm/lombo_drm.h>

#include "lombo_drv.h"
#include "lombo_crtc.h"
#include "lombo_dpu.h"
#include "lombo_se.h"

#include "csp_dpu_se.h"
#include "csp_dpu_se_coef_tab.c"

#define DRIVER_NAME "lombo-se"
#define LOMBO_SE_CLK_RATE_DEF 200000000

enum {
	LOMBO_SE_STATE_IDLE = 0,
	LOMBO_SE_STATE_BUSY,
};

struct lombo_se {
	struct device *dev;
	struct device *dpu_top;
	struct drm_device *drm;
	uint32_t id;

	struct lombo_dpu_clk *mod_clk;
	int support_dynamic_rate;
	unsigned long dynamic_rate;

	void __iomem *reg;
	int irq; /* irq no */

	/* add to drv se_list */
	struct list_head list;

	struct mutex en_mutex;
	int en_cnt;

	void *major_reqer;
	void *minor_reqer;

	void *ch0_xcoef0;
	void *ch0_xcoef1;
	void *ch0_ycoef;
	void *ch1_xcoef0;
	void *ch1_xcoef1;
	void *ch1_ycoef;

	wait_queue_head_t wait;
	uint8_t state;
	uint32_t work_state;
	spinlock_t work_lock;

	uint8_t pending_disable;

	int lti; /* low frequency video sharpness */
	int peak; /* high frequency video sharpness */
};

struct lombo_se_scale_aux_para {
	uint32_t out_width;
	uint32_t in_fmt; /* input pixel format */
};

static inline int _se_suspend(struct lombo_se *se)
{
	DRM_DEBUG_KMS("se id=%d\n", se->id);
	csp_se_disable(se->id);
	csp_se_exit(se->id);
	lombo_dpu_clk_disable_unprepare(se->mod_clk);
	return 0;
}

static inline int _se_resume(struct lombo_se *se)
{
	DRM_DEBUG_KMS("se id=%d\n", se->id);
	if (se->support_dynamic_rate)
		lombo_dpu_clk_set_rate(se->mod_clk, LOMBO_SE_CLK_RATE_DEF);
	lombo_dpu_clk_prepare_enable(se->mod_clk, 0);
	csp_se_init(se->id);
	csp_se_disable(se->id);
	csp_se_enable(se->id);
	return 0;
}

static int lombo_se_set_dpms(struct lombo_se *se, int dpms)
{
	int en_cnt;

	mutex_lock(&se->en_mutex);
	if (dpms) {
		se->en_cnt++;
		if (se->en_cnt == 1)
			_se_resume(se);
	} else {
		se->en_cnt--;
		if (se->en_cnt == 0)
			_se_suspend(se);
	}
	en_cnt = se->en_cnt;
	mutex_unlock(&se->en_mutex);

	DRM_DEBUG_KMS("se_id=%d, dpms=%d, en_cnt=%d\n", se->id, dpms, en_cnt);
	return 0;
}

static enum tag_csc_std lombo_se_get_csc_std(
	enum drm_color_encoding encoding, enum drm_color_range range)
{
	DRM_DEBUG_KMS("color: encoding=%d, range=%d\n", encoding, range);
	if ((encoding == DRM_COLOR_YCBCR_BT601)
		&& (range == DRM_COLOR_YCBCR_LIMITED_RANGE))
		return ITU601L;
	else if ((encoding == DRM_COLOR_YCBCR_BT601)
		&& (range == DRM_COLOR_YCBCR_FULL_RANGE))
		return ITU601F;
	else if ((encoding == DRM_COLOR_YCBCR_BT709)
		&& (range == DRM_COLOR_YCBCR_LIMITED_RANGE))
		return ITU709L;
	else if ((encoding == DRM_COLOR_YCBCR_BT709)
		&& (range == DRM_COLOR_YCBCR_FULL_RANGE))
		return ITU709F;
	else
		return ITU601L;
}

/**
 * @uv_swap:  need swap addr of u/v plane.
 *     or need swap r/b for rgb out format;
 */
static se_img_fmt_t lombo_se_convert_drm_format(
	uint32_t format, bool *_swap)
{
	*_swap = false;

	switch (format) {

	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_XBGR8888:
		*_swap = true;
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XRGB8888:
		return ARGB8888;

	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_RGBX8888:
		*_swap = true;
	case DRM_FORMAT_BGRA8888:
	case DRM_FORMAT_BGRX8888:
		return BGRA8888;

	case DRM_FORMAT_ABGR4444:
	case DRM_FORMAT_XBGR4444:
		*_swap = true;
	case DRM_FORMAT_ARGB4444:
	case DRM_FORMAT_XRGB4444:
		return ARGB4444;

	case DRM_FORMAT_RGBA4444:
	case DRM_FORMAT_RGBX4444:
		*_swap = true;
	case DRM_FORMAT_BGRA4444:
	case DRM_FORMAT_BGRX4444:
		return BGRA4444;

	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
		*_swap = true;
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
		return ARGB1555;

	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_RGBX5551:
		*_swap = true;
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_BGRX5551:
		return BGRA5551;

	case DRM_FORMAT_RGB565:
		return RGB565;
	case DRM_FORMAT_BGR565:
		return BGR565;

	case DRM_FORMAT_UYVY:
		return UYVY;
	case DRM_FORMAT_YUYV:
		return YUYV;
	case DRM_FORMAT_VYUY:
		return VYUY;
	case DRM_FORMAT_YVYU:
		return YVYU;

	case DRM_FORMAT_NV12:
		return YUV420SP;
	case DRM_FORMAT_NV21:
		return YVU420SP;
	case DRM_FORMAT_NV16:
		return YUV422SP;
	case DRM_FORMAT_NV61:
		return YVU422SP;

	case DRM_FORMAT_YVU420:
		*_swap = true;
	case DRM_FORMAT_YUV420:
		return YUV420P;

	case DRM_FORMAT_YVU422:
		*_swap = true;
	case DRM_FORMAT_YUV422:
		return YUV422P;

	case DRM_FORMAT_YVU444:
		*_swap = true;
	case DRM_FORMAT_YUV444:
		return YUV444P;

	default:
		DRM_ERROR("not support format=%4s", (char *)&format);
		return ARGB8888;
	}
}

/**
 * @uv_swap:  need swap addr of u/v plane.
 */
static se_wb_fmt_t lombo_se_wb_convert_drm_format(
	uint32_t format, bool *uv_swap)
{
	*uv_swap = false;

	switch (format) {

	case DRM_FORMAT_YVU420:
		*uv_swap = true;
	case DRM_FORMAT_YUV420:
		return WB_YUV420P;

	case DRM_FORMAT_NV12:
		return WB_YUV420SP;

	case DRM_FORMAT_YVU444:
		*uv_swap = true;
	case DRM_FORMAT_YUV444:
		return WB_YUV444P;

	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XRGB8888:
		return WB_ARGB8888;

	case DRM_FORMAT_AYUV:
		return WB_AYUV;

	case DRM_FORMAT_RGB565:
		return WB_RGB565;

	default:
		DRM_ERROR("not support format=%4s", (char *)&format);
		return ARGB8888;
	}
}

/* Scaling horizal coef calculation */
static void lombo_se_get_horz_scaling_para(u32 ratio,
				   u32 taps,
				   u32 **coef_table,
				   u32 **coef_table1,
				   u32 *shiftbit,
				   u32 *sub)
{
	int i;
	int tab_sel;

	for (i = 0; i < 5; i++) {
		/* from small to big */
		if (ratio <= scaling_coef_tab8_arX[i].ratio)
			break;
	}
	tab_sel = min(i, 4); /* max is 4 */
	DRM_DEBUG_KMS("tab_sel=%d\n", tab_sel);
	/* return para */
	if ((taps == 8) && coef_table) {
		*coef_table = (u32 *)(scaling_coef_tab8_arX[tab_sel].tab0_addr);
		#if 0
		for (i = 0; i < 32; i += 4) {
			DRM_DEBUG_KMS("%u: 0x%8x,0x%8x,0x%8x,0x%8x\n", i,
				(*coef_table)[i], (*coef_table)[i + 1],
				(*coef_table)[i + 2], (*coef_table)[i + 3]);
		}
		#endif
	}
	if ((taps == 8) && coef_table1) {
		*coef_table1 = (u32 *)
			(scaling_coef_tab8_arX[tab_sel].tab1_addr);
		#if 0
		for (i = 0; i < 32; i += 4) {
			DRM_DEBUG_KMS("%u: 0x%8x,0x%8x,0x%8x,0x%8x\n", i,
				(*coef_table1)[i], (*coef_table1)[i + 1],
				(*coef_table1)[i + 2], (*coef_table1)[i + 3]);
		}
		#endif
	}

	/* return para */
	shiftbit[0] = scaling_coef_tab8_arX[tab_sel].shift_ctl;
	sub[0] = scaling_coef_tab8_arX[tab_sel].sub;

}

/* Scaling vertical coef calculation */
static void lombo_se_get_vert_scaling_para(u32 ratio,
				   u32 taps,
				   u32 **coef_table,
				   u32 *shiftbit,
				   u32 *sub)
{
	int i;
	int tab_sel;

	for (i = 0; i < 5; i++) {
		if (taps == 4) {
			if (ratio <= scaling_coef_tab4_arY[i].ratio)
				break;
		} else {
			if (ratio >= scaling_coef_tab2_arY[i].ratio)
				break;
		}
	}
	tab_sel = min(i, 4); /* max is 4 */

	/* return para */
	if ((taps == 4) && coef_table) {
		DRM_DEBUG_KMS("tab4_sel=%d\n", tab_sel);
		*coef_table = (u32 *)(scaling_coef_tab4_arY[tab_sel].tab0_addr);
		*shiftbit = scaling_coef_tab4_arY[tab_sel].shift_ctl;
		*sub = scaling_coef_tab4_arY[tab_sel].sub;
	}
	if ((taps == 2) && coef_table) {
		DRM_DEBUG_KMS("tab2_sel=%d\n", tab_sel);
		*coef_table = (u32 *)(scaling_coef_tab2_arY[tab_sel].tab0_addr);
		*shiftbit = scaling_coef_tab2_arY[tab_sel].shift_ctl;
		*sub = scaling_coef_tab2_arY[tab_sel].sub;
	}

}

static void lombo_se_config_scaling_para(struct lombo_se *se,
	struct lombo_se_scale_aux_para *para,
	uint32_t ratio_horz[2], uint32_t ratio_vert[2])
{
	const uint32_t se_max_width = 2048;
	const struct drm_format_info *fmt_info = NULL;

	uint32_t y_taps[2] = {0};
	uint32_t hsub[2] = {0, 0};
	uint32_t vsub[2] = {0, 0};
	uint32_t shiftbitx[2] = {0, 0};
	uint32_t shiftbity[2] = {0, 0};
	uint32_t linebuffer_mode;

	fmt_info = drm_format_info(para->in_fmt);
	if (!fmt_info) {
		DRM_ERROR("invalid in_fmt=0x%x\n", para->in_fmt);
		return;
	}

	/**
	 * calc taps
	 * select linebuffer mode
	 */
	if (para->out_width <= se_max_width / 3) {
		/* vertical direct scale, YUV for 4 lines, RGB for 4 lines */
		y_taps[0] = 4;
		y_taps[1] = 4;
		linebuffer_mode = 2; /* YUV/RGB, all to 4 lines scale */
	} else if ((para->out_width > se_max_width / 3)
		&& fmt_info->is_yuv
		&& (fmt_info->num_planes > 1)) {
		/* vertical direct scale, Y for 4 lines, UV for 2 lines */
		y_taps[0] = 4;
		y_taps[1] = 2;
		linebuffer_mode = 1; /* Only for non-Interleaved YUV */
	} else {
		/* vertical direct scale, for 2 lines */
		y_taps[0] = 2;
		y_taps[1] = 2;
		linebuffer_mode = 0; /* Only for RGB or Interleaved YUV */
	}
	DRM_DEBUG_KMS("y_taps[%d,%d],lb_mode=%d\n",
		      y_taps[0], y_taps[1], linebuffer_mode);

	/* select different table, x 8taps ch0 */
	lombo_se_get_horz_scaling_para(ratio_horz[0], 8,
			(u32 **)&se->ch0_xcoef0, (u32 **)&se->ch0_xcoef1,
			&shiftbitx[0], &hsub[0]);
	/* y 4taps ch0 */
	lombo_se_get_vert_scaling_para(ratio_vert[0], y_taps[0],
			(u32 **)&se->ch0_ycoef,
			&shiftbity[0], &vsub[0]);

	/* x 8taps ch1/2 */
	lombo_se_get_horz_scaling_para(ratio_horz[1], 8,
			(u32 **)&se->ch1_xcoef0, (u32 **)&se->ch1_xcoef1,
			&shiftbitx[1], &hsub[1]);
	/* y 4/2taps ch1/2 */
	lombo_se_get_vert_scaling_para(ratio_vert[1], y_taps[1],
			(u32 **)&se->ch1_ycoef,
			&shiftbity[1], &vsub[1]);

	csp_se_set_scaling_ratio_x(se->id, ratio_horz);
	csp_se_set_scaling_ratio_y(se->id, ratio_vert);

	/* set line buffer mode */
	csp_se_set_linebuffer_mode(se->id, linebuffer_mode);

	/* set shift control, related to the x/ycoef table. */
	csp_se_set_shift_ctl(se->id, shiftbitx, shiftbity);

}

static inline void lombo_se_set_scaling_coef(struct lombo_se *se)
{
	if (se->ch0_xcoef0) {
		csp_se_set_scaling_xcoef0(se->id, 0, se->ch0_xcoef0);
		se->ch0_xcoef0 = NULL;
	}
	if (se->ch0_xcoef1) {
		csp_se_set_scaling_xcoef1(se->id, 0, se->ch0_xcoef1);
		se->ch0_xcoef1 = NULL;
	}
	if (se->ch0_ycoef) {
		csp_se_set_scaling_ycoef(se->id, 0, se->ch0_ycoef);
		se->ch0_ycoef = NULL;
	}
	if (se->ch1_xcoef0) {
		csp_se_set_scaling_xcoef0(se->id, 1, se->ch1_xcoef0);
		se->ch1_xcoef0 = NULL;
	}
	if (se->ch1_xcoef1) {
		csp_se_set_scaling_xcoef1(se->id, 1, se->ch1_xcoef1);
		se->ch1_xcoef1 = NULL;
	}
	if (se->ch1_ycoef) {
		csp_se_set_scaling_ycoef(se->id, 1, se->ch1_ycoef);
		se->ch1_ycoef = NULL;
	}
}

static int lombo_se_calc_dynamic_rate(struct lombo_se *se,
	const struct lombo_se_plane_cfg *req_cfg)
{
	struct csp_se_calc_rate_info info = { 0 };
	int ret;

	info.scn_w = req_cfg->scn_w;
	info.scn_h = req_cfg->scn_h;
	info.scn_fps = req_cfg->scn_fps ? req_cfg->scn_fps : 60;
	info.in_w = req_cfg->fb_cfg->src_w >> 16;
	info.in_h = req_cfg->fb_cfg->src_h >> 16;
	info.out_w = req_cfg->fb_cfg->crtc_w;
	info.out_h = req_cfg->fb_cfg->crtc_h;
	info.sclk_rate = lombo_dpu_clk_get_parent_rate(se->mod_clk);
	ret = csp_se_calc_dynamic_rate(se->id, &info);
	if (ret || (info.calc_rate == 0)) {
		info.calc_rate = LOMBO_SE_CLK_RATE_DEF;
		DRM_ERROR("calc_rate=%lu or ret=%d\n", info.calc_rate, ret);
		DRM_INFO("scn[%ux%u@%u],se%d:%ux%u->%ux%u,sclk=%lu,rate=%lu\n",
			 info.scn_w, info.scn_h, info.scn_fps, se->id,
			 info.in_w, info.in_h, info.out_w, info.out_h,
			 info.sclk_rate, info.calc_rate);
	}

	info.sclk_rate = lombo_dpu_clk_get_req_rate(se->mod_clk);
	if (info.calc_rate != info.sclk_rate) {
		se->dynamic_rate = info.calc_rate;
		DRM_DEBUG("prepare dynamic rate %lu -> %lu\n",
			  info.sclk_rate, info.calc_rate);
	}

	return 0;
}

int lombo_se_update_plane(struct lombo_se *se, void *reqer,
		struct lombo_se_plane_cfg *req_cfg)
{
	struct drm_framebuffer *fb = NULL;
	const struct drm_format_info *fmt_info = NULL;
	const struct drm_format_info *out_fmt_info = NULL;
	struct drm_gem_cma_object *cma_obj[4] = { 0 };

	struct lombo_se_scale_aux_para scale_aux;
	struct lombo_fb_cfg fb_cfg;
	unsigned long long paddr[4] = { 0 };
	uint32_t src_w[2], src_h[2]; /* Q16 */
	uint32_t src_pitch[4] = { 0, 0, 0, 0 };
	uint32_t ratio_horz[2], ratio_vert[2];
	uint32_t xoffset[2] = { 0, 0 };
	uint32_t yoffset[2] = { 0, 0 };

	uint32_t i;
	bool recalc = false;
	bool _swap = false;

	if (!reqer || (reqer != se->major_reqer)) {
		DRM_ERROR("%p not match major reqer %p\n",
			reqer, se->major_reqer);
		return -EINVAL;
	}
	if (req_cfg->in_sel == LOMBO_SE_IN_SEL_DRAM) {
		if (!req_cfg->fb_cfg->fb) {
			DRM_ERROR("null fb at se in sel dram\n");
			return -EINVAL;
		}
		fb = req_cfg->fb_cfg->fb;
		fmt_info = fb->format;
		req_cfg->in_fmt = fmt_info->format;
	}
	DRM_DEBUG_KMS("se in_sel=%d, in_fmt=0x%x\n",
		req_cfg->in_sel, req_cfg->in_fmt);
	if (!fmt_info)
		fmt_info = drm_format_info(req_cfg->in_fmt);

	/* set se's mode paras */
	csp_se_set_input_sel(se->id, req_cfg->in_sel);
	if (req_cfg->se_run_mode == LOMBO_SE_RUN_MODE_ONLINE)
		csp_se_set_online_mode(se->id);
	else
		csp_se_set_offline_mode(se->id);
	csp_se_set_output_progressive(se->id);
	csp_se_set_update_mode(se->id, SE_UPD_MAN);

	/* calc and fix fb's size */
	memcpy((void *)&fb_cfg, req_cfg->fb_cfg, sizeof(fb_cfg));

	if (fb) {
		for (i = 0; i < fmt_info->num_planes; ++i)
			src_pitch[i] = fb->pitches[i];
		if ((req_cfg->se_run_mode == LOMBO_SE_RUN_MODE_ONLINE) &&
			((fb_cfg.src_h / fb_cfg.crtc_h >> 16) >= 2)) {
			/* double pitch to cut-down height of src */
			DRM_DEBUG_KMS("double pitch\n");
			fb_cfg.src_y >>= 1;
			fb_cfg.src_h >>= 1;
			for (i = 0; i < fmt_info->num_planes; i++)
				src_pitch[i] <<= 1;
		}
	}
	if (se->support_dynamic_rate)
		lombo_se_calc_dynamic_rate(se, req_cfg);

re_calc:
	for (i = 0; i < fmt_info->num_planes; i++) {
		uint32_t src_x, src_y;

		if (i == 0) {
			src_x = fb_cfg.src_x;
			src_y = fb_cfg.src_y;
		} else {
			src_x = fb_cfg.src_x / fmt_info->hsub;
			src_y = fb_cfg.src_y / fmt_info->vsub;
		}

		if (i <= 1) {
			if (i == 0) {
				src_w[i] = fb_cfg.src_w;
				src_h[i] = fb_cfg.src_h;
				src_w[1] = src_w[0];
				src_h[1] = src_h[0];
				switch (req_cfg->in_fmt) {
				case DRM_FORMAT_YUYV:
				case DRM_FORMAT_YVYU:
				case DRM_FORMAT_UYVY:
				case DRM_FORMAT_VYUY:
					src_w[1] >>= 1;
					break;
				default:
					break;
				}
			} else {
				src_w[i] = fb_cfg.src_w;
				if ((src_w[i] >> 16) & 1)
					src_w[i] += (1 << 16);
				src_w[i] /= fmt_info->hsub;
				src_h[i] = fb_cfg.src_h / fmt_info->vsub;
			}
			/* calc ratio only at first time */
			if (!recalc) {
				ratio_horz[i] = src_w[i] / fb_cfg.crtc_w;
				ratio_vert[i] = src_h[i] / fb_cfg.crtc_h;
				if (i == 0) {
					/**
					 * for format which has only one plane,
					 * they also need ratio[1].
					 */
					ratio_horz[1] = ratio_horz[0];
					ratio_vert[1] = ratio_vert[0];
					switch (req_cfg->in_fmt) {
					case DRM_FORMAT_YUYV:
					case DRM_FORMAT_YVYU:
					case DRM_FORMAT_UYVY:
					case DRM_FORMAT_VYUY:
						ratio_horz[1] >>= 1;
						break;
					default:
						break;
					}
				}
			}
		}

		if (fb) {
			cma_obj[i] = drm_fb_cma_get_gem_obj(fb, i);
			paddr[i] = cma_obj[i]->paddr + fb->offsets[i]
				+ src_pitch[i] * (src_y >> 16)
				+ ((src_x >> 16) * fmt_info->cpp[i]);
		}

		DRM_DEBUG_KMS("plane%u:paddr=0x%llx, src_w=%u.%u\n",
			i, paddr[i],
			src_w[i ? 1 : 0] >> 16, src_w[i ? 1 : 0] & 0xFFFF);
	}

	if ((paddr[0] & 0xF) || (paddr[1] & 0xF)) {
		uint32_t del_w = paddr[0] & 0xF;
		uint32_t del_x;
		uint32_t new_crtc_w;

		/* del_w must be diviable to pixel */
		if (del_w == 0)
			del_w = 0x10;
		while (del_w % fmt_info->cpp[0]) {
			DRM_DEBUG_KMS("del_w=0x%x, cpp[0]=%d\n",
				      del_w, fmt_info->cpp[0]);
			del_w += 0x10;
		}

		del_x = del_w / fmt_info->cpp[0];
		fb_cfg.src_x -= (del_x << 16);
		fb_cfg.src_w += (del_x << 16);
		new_crtc_w = fb_cfg.src_w / ratio_horz[0];
		fb_cfg.crtc_x -= (new_crtc_w - fb_cfg.crtc_w);
		fb_cfg.crtc_w = new_crtc_w;

		DRM_DEBUG_KMS("del_w=%u,cpp[0]=%u, paddr=0x%llx\n",
			      del_w, fmt_info->cpp[0], paddr[0]);
		DRM_DEBUG_KMS("src_x=0x%x(%u.%u)\n", fb_cfg.src_x,
			fb_cfg.src_x >> 16, fb_cfg.src_x & 0xFFFF);
		DRM_DEBUG_KMS("src_w=0x%x(%u.%u)\n", fb_cfg.src_w,
			fb_cfg.src_w >> 16, fb_cfg.src_w & 0xFFFF);
		DRM_DEBUG_KMS("crtc_x=%u, new_crtc_w=%u, ratio_horz[0]=0x%x\n",
			fb_cfg.crtc_x, fb_cfg.crtc_w, ratio_horz[0]);
		DRM_DEBUG_KMS("goto re_calc ...");

		recalc = true;
		goto re_calc;
	}

	/* cfg scacling para and coef table */
	scale_aux.out_width = fb_cfg.crtc_w;
	scale_aux.in_fmt = req_cfg->in_fmt;
	lombo_se_config_scaling_para(se, &scale_aux, ratio_horz, ratio_vert);

	/* cfg fb's format crop, addr, coord, phase offset */
	csp_se_set_infmt(se->id,
		lombo_se_convert_drm_format(req_cfg->in_fmt, &_swap));
	src_w[0] >>= 16;
	src_h[0] >>= 16;
	src_w[1] >>= 16;
	src_h[1] >>= 16;
	csp_se_set_insize(se->id, src_w, src_h);
	csp_se_set_xoffset(se->id, xoffset);
	csp_se_set_yoffset(se->id, yoffset);
	csp_se_set_inlstr(se->id, src_pitch);
	if (fmt_info->is_yuv && _swap) {
		unsigned long long tmp = paddr[1];
		paddr[1] = paddr[2];
		paddr[2] = tmp;
	}
	csp_se_set_inbuf_addr(se->id, paddr);

	/* enable se resample func */
	DRM_DEBUG_KMS("rsmp[x,y,w,h]:[%u,%u,%u,%u]->[%u,%u,%u,%u]\n",
		req_cfg->fb_cfg->crtc_x,
		req_cfg->fb_cfg->crtc_y,
		req_cfg->fb_cfg->crtc_w,
		req_cfg->fb_cfg->crtc_h,
		fb_cfg.crtc_x, fb_cfg.crtc_y,
		fb_cfg.crtc_w, fb_cfg.crtc_h);
	csp_se_set_enable_rsmp(se->id); /* TODO: no rsmp if rgb no scale*/
	csp_se_set_rsmp_size(se->id,
		fb_cfg.crtc_w,
		fb_cfg.crtc_h);
	csp_se_set_out_crop_size(se->id,
		req_cfg->fb_cfg->crtc_w,
		req_cfg->fb_cfg->crtc_h);
	csp_se_set_out_crop_offset(se->id,
		req_cfg->fb_cfg->crtc_x - fb_cfg.crtc_x,
		req_cfg->fb_cfg->crtc_y - fb_cfg.crtc_y);

	if (!fmt_info->has_alpha)
		csp_se_enable_alpha(se->id);
	else
		csp_se_disable_alpha(se->id);

	out_fmt_info = drm_format_info(req_cfg->se_out_format);
	if (fmt_info->is_yuv && !out_fmt_info->is_yuv) {
		/* TODO, select different color method */
		/*
		 * if input is yuv or BGR, select csc standard and conversion
		 * type and make sure to convert to RGB format before output
		 */
		csp_se_set_csco_coef(se->id, CSC_YUV2RGB,
			lombo_se_get_csc_std(fb_cfg.color_space,
					     fb_cfg.color_range));
		csp_se_enable_csco(se->id);
	} else if (!fmt_info->is_yuv && out_fmt_info->is_yuv) {
		csp_se_set_csco_coef(se->id, CSC_RGB2YUV,
			lombo_se_get_csc_std(fb_cfg.color_space,
					     fb_cfg.color_range));
		csp_se_enable_csco(se->id);
	} else {
		csp_se_disable_csco(se->id);
	}

	/* update video lti */
	if (fb_cfg.video_lti > 0) {
		csp_se_lti_set_coef(se->id, 0, NULL);
		csp_se_lti_enable(se->id, fb_cfg.video_lti);
		csp_se_ltiv_enable(se->id, fb_cfg.video_lti);
	} else {
		csp_se_lti_disable(se->id);
		csp_se_ltiv_disable(se->id);
	}

	/* update video peak */
	if (fb_cfg.video_peak > 0) {
		csp_se_peaking_set_coef(se->id, 0, NULL);
		csp_se_peaking_enable(se->id, fb_cfg.video_peak);
	} else
		csp_se_peaking_disable(se->id);

	/* change out format for online rgb format */
	if ((req_cfg->se_run_mode == LOMBO_SE_RUN_MODE_ONLINE) &&
		!fmt_info->is_yuv && _swap) {
		DRM_DEBUG_KMS("in_fmt=%4s out to fmt AB24\n",
			(char *)&fmt_info->format);
		req_cfg->se_out_format = DRM_FORMAT_ABGR8888;
	}

	se->pending_disable = false;

	return 0;
}
EXPORT_SYMBOL(lombo_se_update_plane);

int lombo_se_disable_plane(struct lombo_se *se, void *reqer)
{
	if (reqer == se->major_reqer) {
		se->pending_disable = 1;
		return 0;
	}
	return -EINVAL;
}
EXPORT_SYMBOL(lombo_se_disable_plane);

void lombo_se_commit(struct lombo_se *se, void *reqer)
{
	if (se->pending_disable) {
		csp_se_stream_stop(se->id);
		csp_se_update_dbr(se->id);
		return;
	}

	if (!reqer || (se->major_reqer != reqer)) {
		DRM_DEBUG_KMS("reqer not match, [%p->%p]\n",
			reqer, se->major_reqer);
		return;
	}

	csp_se_stream_stop(se->id);
	lombo_se_set_scaling_coef(se);
	csp_se_stream_start(se->id);
	csp_se_update_dbr(se->id);
}
EXPORT_SYMBOL(lombo_se_commit);

int lombo_se_page_flip_worker(struct lombo_se *se, void *reqer)
{
	if (se->pending_disable) {
		se->pending_disable = false;
		lombo_se_set_dpms(se, 0);
	}
	return 0;
}
EXPORT_SYMBOL(lombo_se_page_flip_worker);

int lombo_se_scale_check_fbs(struct lombo_se *se,
	struct lombo_se_scale_cfg *cfg)
{
	struct drm_framebuffer *fb;
	struct drm_lombo_scale_fb *fb_cfg;

	/* check src fb cfg */
	fb = cfg->src_fb;
	fb_cfg = cfg->src_cfg;
	if (fb_cfg->crop_x >= fb->width ||
		fb_cfg->crop_w == 0 ||
		fb_cfg->crop_w > fb->width - fb_cfg->crop_x ||
		fb_cfg->crop_y >= fb->height ||
		fb_cfg->crop_h == 0 ||
		fb_cfg->crop_h > fb->height - fb_cfg->crop_y) {
		DRM_ERROR("invalid src crop[%d,%d,%d,%d], fb[%u,%u]\n",
			fb_cfg->crop_x, fb_cfg->crop_y,
			fb_cfg->crop_w, fb_cfg->crop_h,
			fb->width, fb->height);
		return -ENOSPC;
	}
	/* check dst fb cfg */
	fb = cfg->dst_fb;
	fb_cfg = cfg->dst_cfg;
	if (fb_cfg->crop_x >= fb->width ||
		fb_cfg->crop_w == 0 ||
		fb_cfg->crop_w > fb->width - fb_cfg->crop_x ||
		fb_cfg->crop_y >= fb->height ||
		fb_cfg->crop_h == 0 ||
		fb_cfg->crop_h > fb->height - fb_cfg->crop_y) {
		DRM_ERROR("invalid dst crop[%d,%d,%d,%d], fb[%u,%u]\n",
			fb_cfg->crop_x, fb_cfg->crop_y,
			fb_cfg->crop_w, fb_cfg->crop_h,
			fb->width, fb->height);
		return -ENOSPC;
	}

	return 0;
}
EXPORT_SYMBOL(lombo_se_scale_check_fbs);

static int lombo_se_scale_set_wb(struct lombo_se *se,
	struct drm_framebuffer *fb, struct drm_lombo_scale_fb *fb_cfg)
{
	const struct drm_format_info *fmt = fb->format;
	unsigned long long paddr[4] = { 0 };
	struct drm_gem_cma_object *cma_obj[4] = { 0 };
	uint32_t dst_w[4], dst_h[4];
	bool uv_swap = false;
	uint32_t i;

	for (i = 0; i < fmt->num_planes; i++) {
		uint32_t x, y;

		x = fb_cfg->crop_x;
		y = fb_cfg->crop_y;
		dst_w[i] = fb_cfg->crop_w;
		dst_h[i] = fb_cfg->crop_h;
		if (i > 0) {
			x /= fmt->hsub;
			y /= fmt->vsub;
			dst_w[i] /= fmt->hsub;
			dst_h[i] /= fmt->vsub;
		}
		cma_obj[i] = drm_fb_cma_get_gem_obj(fb, i);
		paddr[i] = cma_obj[i]->paddr + fb->offsets[i] +
			fb->pitches[i] * y + x * fmt->cpp[i];

		DRM_DEBUG("%u:paddr=0x%llx, dst[%d,%d,%d,%d]\n",
			i, paddr[i], x, y, dst_w[i], dst_h[i]);
		DRM_DEBUG("pitch=%d\n", fb->pitches[i]);
	}
	csp_se_set_wb_format(se->id, lombo_se_wb_convert_drm_format(
					fmt->format, &uv_swap));
	csp_se_set_wb_size(se->id, dst_w, dst_h);
	if (uv_swap) {
		unsigned long long tmp = paddr[1];

		paddr[1] = paddr[2];
		paddr[2] = tmp;
	}
	csp_se_set_wb_addr(se->id, paddr);
	csp_se_set_wb_linestride(se->id, fb->pitches);
	csp_se_set_wb_timer(se->id, 0x3FF);

	return 0;
}

int lombo_se_scale_exec(struct lombo_se *se,
	struct lombo_se_scale_cfg *cfg)
{
	struct lombo_fb_cfg fb_cfg = { .fb = NULL, };
	struct lombo_se_plane_cfg req_cfg = { .reqer = NULL, };

	unsigned long flags;
	uint64_t time;
	long timeout;
	int ret;

	if (!cfg || !se) {
		DRM_ERROR("null pointers\n");
		return -EINVAL;
	}
	if (cfg->reqer != se->major_reqer) {
		DRM_ERROR("%s: %p not match major reqer %p\n",
			  __func__, cfg->reqer, se->major_reqer);
		return -EINVAL;
	}

	csp_se_disable(se->id);
	csp_se_enable(se->id);

	fb_cfg.fb = cfg->src_fb;
	fb_cfg.src_x = cfg->src_cfg->crop_x << 16;
	fb_cfg.src_y = cfg->src_cfg->crop_y << 16;
	fb_cfg.src_w = cfg->src_cfg->crop_w << 16;
	fb_cfg.src_h = cfg->src_cfg->crop_h << 16;
	fb_cfg.crtc_x = cfg->dst_cfg->crop_x;
	fb_cfg.crtc_y = cfg->dst_cfg->crop_y;
	fb_cfg.crtc_w = cfg->dst_cfg->crop_w;
	fb_cfg.crtc_h = cfg->dst_cfg->crop_h;
	req_cfg.fb_cfg = &fb_cfg;
	req_cfg.reqer = cfg->reqer;
	req_cfg.in_sel = LOMBO_SE_IN_SEL_DRAM;
	req_cfg.se_run_mode = LOMBO_SE_RUN_MODE_OFFLINE;
	req_cfg.se_out_format = cfg->dst_fb->format->format;
	lombo_se_update_plane(se, req_cfg.reqer, &req_cfg);

	/* set scaling coef */
	lombo_se_set_scaling_coef(se);

	/* se write back */
	lombo_se_scale_set_wb(se, cfg->dst_fb, cfg->dst_cfg);

	/* start scale and se wb */
	se->state = LOMBO_SE_STATE_BUSY;
	csp_se_enable_irq_src(se->id, SE_IRQ_WB_FINISH);
	csp_se_update_dbr(se->id);
	csp_se_start_wb(se->id);
	csp_se_stream_start(se->id);

	/* wait wb finish */
	time = cfg->timeout_ns;
	time += 999; /* make sure time >= 1000 */
	do_div(time, 1000);
	timeout = usecs_to_jiffies((uint32_t)time);
	ret = wait_event_interruptible_timeout(se->wait,
		se->state != LOMBO_SE_STATE_BUSY, timeout);
	DRM_DEBUG("ret=%d, state=%d, work_state=0x%x\n",
		ret, se->state, se->work_state);
	spin_lock_irqsave(&se->work_lock, flags);
	if (se->state != LOMBO_SE_STATE_BUSY) {
		se->state = LOMBO_SE_STATE_IDLE;
		spin_unlock_irqrestore(&se->work_lock, flags);
		csp_se_stream_stop(se->id);
		csp_se_stop_wb(se->id);
		csp_se_disable_irq_src(se->id, SE_IRQ_WB_FINISH);
		ret = 0;
	} else {
		csp_se_stream_stop(se->id);
		csp_se_stop_wb(se->id);
		csp_se_disable_irq_src(se->id, SE_IRQ_WB_FINISH);
		csp_se_clear_irq_status(se->id, SE_IRQ_WB_FINISH);
		se->state = LOMBO_SE_STATE_IDLE;
		spin_unlock_irqrestore(&se->work_lock, flags);
		DRM_DEBUG_KMS("scale timeout\n");
		ret = -ETIMEDOUT;
	}

	return ret;
}
EXPORT_SYMBOL(lombo_se_scale_exec);

struct lombo_se *lombo_se_request(struct drm_device *drm, void *reqer)
{
	struct lombo_drv *drv = drm->dev_private;
	struct lombo_se *tmp, *se = NULL;
	uint32_t req_cnt = 1;

	list_for_each_entry(tmp, &drv->se_list, list) {
		if (tmp->major_reqer == reqer) {
			se = tmp;
			goto out;
		}
	}
	req_cnt++;
	list_for_each_entry(tmp, &drv->se_list, list) {
		if (tmp->major_reqer == NULL) {
			tmp->major_reqer = reqer;
			se = tmp;
			goto out;
		}
	}

	/**
	 * FIXME: Grabing se from major_requirer is allowed at present,
	 *    but this action should be abandoned.
	 */
	DRM_ERROR("try to grab se\n");
	req_cnt++;
	list_for_each_entry(tmp, &drv->se_list, list) {
		if (tmp->minor_reqer == reqer) {
			tmp->minor_reqer = tmp->major_reqer;
			tmp->major_reqer = reqer;
			se = tmp;
			goto out;
		}
	}
	req_cnt++;
	list_for_each_entry(tmp, &drv->se_list, list) {
		if (tmp->minor_reqer == NULL) {
			tmp->minor_reqer = tmp->major_reqer;
			tmp->major_reqer = reqer;
			se = tmp;
			goto out;
		}
	}

out:
	if (se) {
		lombo_se_set_dpms(se, 1);
		DRM_DEBUG_KMS("req_cnt=%d, se %u\n", req_cnt, se->id);
	} else {
		DRM_ERROR("request se fail\n");
	}

	return se;
}
EXPORT_SYMBOL(lombo_se_request);

/* donot call this func in irq */
void lombo_se_release(struct lombo_se *se, void *reqer, uint32_t do_disable)
{
	if (se->major_reqer == reqer) {
		se->major_reqer = NULL;
		if (se->minor_reqer) {
			se->major_reqer = se->minor_reqer;
			se->minor_reqer = NULL;
		}
		if (do_disable)
			lombo_se_set_dpms(se, 0);
	} else if (se->minor_reqer == reqer) {
		se->minor_reqer = NULL;
		lombo_se_set_dpms(se, 0);
	}

}
EXPORT_SYMBOL(lombo_se_release);

uint32_t lombo_se_id(struct lombo_se *se)
{
	return se->id;
}
EXPORT_SYMBOL(lombo_se_id);

static irqreturn_t lombo_se_irq_handler(int irq, void *dev_id)
{
	struct lombo_se *se = dev_id;
	unsigned long flags;

	if (irq != se->irq)
		return IRQ_NONE;

	spin_lock_irqsave(&se->work_lock, flags);
	se->work_state = csp_se_get_irq_status(se->id);
	csp_se_clear_irq_status(se->id, se->work_state);
	se->state = LOMBO_SE_STATE_IDLE;
	wake_up_interruptible(&se->wait);
	spin_unlock_irqrestore(&se->work_lock, flags);

	return IRQ_HANDLED;
}

static void lombo_se_init_scale_coef(void)
{
	scaling_coef_tab8_arX[0].tab0_addr = (ulong)sc_tab_8taps_type0_0;
	scaling_coef_tab8_arX[0].tab1_addr = (ulong)sc_tab_8taps_type0_1;
	scaling_coef_tab8_arX[1].tab0_addr = (ulong)sc_tab_8taps_type1_0;
	scaling_coef_tab8_arX[1].tab1_addr = (ulong)sc_tab_8taps_type1_1;
	scaling_coef_tab8_arX[2].tab0_addr = (ulong)sc_tab_8taps_type1_0;
	scaling_coef_tab8_arX[2].tab1_addr = (ulong)sc_tab_8taps_type1_1;
	scaling_coef_tab8_arX[3].tab0_addr = (ulong)sc_tab_8taps_type3_0;
	scaling_coef_tab8_arX[3].tab1_addr = (ulong)sc_tab_8taps_type3_1;
	scaling_coef_tab8_arX[4].tab0_addr = (ulong)sc_tab_8taps_type4_0;
	scaling_coef_tab8_arX[4].tab1_addr = (ulong)sc_tab_8taps_type4_1;

	scaling_coef_tab4_arY[0].tab0_addr = (ulong)sc_tab_4taps_type0;
	scaling_coef_tab4_arY[1].tab0_addr = (ulong)sc_tab_4taps_type1;
	scaling_coef_tab4_arY[2].tab0_addr = (ulong)sc_tab_4taps_type2;
	scaling_coef_tab4_arY[3].tab0_addr = (ulong)sc_tab_4taps_type3;
	scaling_coef_tab4_arY[4].tab0_addr = (ulong)sc_tab_4taps_type4;

	scaling_coef_tab2_arY[0].tab0_addr = (ulong)sc_tab_2taps_type0;
	scaling_coef_tab2_arY[1].tab0_addr = (ulong)sc_tab_2taps_type1;
	scaling_coef_tab2_arY[2].tab0_addr = (ulong)sc_tab_2taps_type2;
	scaling_coef_tab2_arY[3].tab0_addr = (ulong)sc_tab_2taps_type3;
	scaling_coef_tab2_arY[4].tab0_addr = (ulong)sc_tab_2taps_type4;
}

static int lombo_se_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = dev->of_node;
	struct drm_device *drm = master_data;
	struct lombo_drv *drv = drm->dev_private;

	struct device_node *top;
	struct platform_device *top_pdev;
	struct lombo_se *se;
	struct resource *res;
	uint32_t value;
	int ret;

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

	se = kzalloc(sizeof(*se), GFP_KERNEL);
	if (!se) {
		dev_err(dev, "alloc for se failed\n");
		return -ENOMEM;
	}
	se->dev = dev;
	se->dpu_top = &top_pdev->dev;
	se->drm = drm;

	/* parse hardware index */
	if (!of_property_read_u32(np, "hw_id", &value))
		se->id = value;
	else
		DRM_INFO("get se hw_id failed\n");
	DRM_DEBUG_KMS("se id=%d\n", se->id);

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	se->reg = devm_ioremap(dev, res->start,
			resource_size(res));
	if (se->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_se_set_register_base(se->id, se->reg,
		(u32)resource_size(res));
	lombo_se_init_scale_coef();

	/* parse irq */
	se->irq = -1;
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res) {
		ret = devm_request_irq(dev, res->start,
			lombo_se_irq_handler, 0, "lombo_se", se);
		if (ret)
			dev_err(dev, "request interrupt failed.\n");
		else
			DRM_DEBUG_KMS("se%d irq=%llu\n",
				se->id, (uint64_t)res->start);
		se->irq = res->start;
	}

	/* init clock */
	se->mod_clk = lombo_dpu_get_clk(dev, se->dpu_top,
				DPU_MOD_SE0 + se->id);
	if (!se->mod_clk) {
		DRM_ERROR("lombo_dpu_get_clk for se fail\n");
		return -EINVAL;
	}
	if (!of_property_read_u32(np, "sclk", &value))
		lombo_dpu_clk_set_parent(se->mod_clk, value);
	else
		lombo_dpu_clk_set_parent(se->mod_clk, 0);
	if (of_property_read_u32(np, "clk_rate", &value))
		value = 0;
	if (value == 0) {
		se->support_dynamic_rate = 1;
		value = LOMBO_SE_CLK_RATE_DEF;
		DRM_INFO("se%d support dynamic rate\n", se->id);
	}
	lombo_dpu_clk_set_rate(se->mod_clk, value);

	dev_set_drvdata(dev, se);

	init_waitqueue_head(&se->wait);
	spin_lock_init(&se->work_lock);
	list_add_tail(&se->list, &drv->se_list);

	mutex_init(&se->en_mutex);

	return 0;
}

static void lombo_se_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_se *se = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	list_del(&se->list);
	lombo_dpu_put_clk(&se->mod_clk);
	dev_set_drvdata(dev, NULL);
	csp_se_set_register_base(se->id, NULL, 0);
	kfree(se);
}

static const struct component_ops lombo_se_ops = {
	.bind	= lombo_se_bind,
	.unbind	= lombo_se_unbind,
};

static int lombo_se_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_se_ops);
}

static int lombo_se_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_se_ops);
	return 0;
}

static const struct of_device_id lombo_se_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-se-0" },
	{ .compatible = "lombo,lombo-n7v0-se-1" },
	{ .compatible = "lombo,lombo-n7v1-se-0" },
	{ .compatible = "lombo,lombo-n7v1-se-1" },
	{ .compatible = "lombo,lombo-n8v0-se-0" },
	{ .compatible = "lombo,lombo-n8v0-se-1" },
	{ .compatible = "lombo,lombo-n9v0-se-0" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_se_of_table);

static struct platform_driver lombo_se_platform_driver = {
	.probe		= lombo_se_probe,
	.remove		= lombo_se_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= DRIVER_NAME,
		.of_match_table	= lombo_se_of_table,
	},
};
module_platform_driver(lombo_se_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU SE Driver");
MODULE_LICENSE("GPL");

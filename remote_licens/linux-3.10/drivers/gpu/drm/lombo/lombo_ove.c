/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public Licenove as published by the
 * Free Software Foundation;  either version 2 of the  Licenove, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/component.h>
#include <linux/of_platform.h>
#include <linux/pm_runtime.h>

#include <drm/drmP.h>
#include <drm/lombo_drm.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>

#include "csp_dpu_ove.h"
#include "lombo_crtc.h"
#include "lombo_dpu.h"
#include "lombo_drv.h"
#include "lombo_fb_array.h"
#include "lombo_ove.h"

static LIST_HEAD(ove_list);
static DEFINE_SPINLOCK(ove_lock);

struct lombo_ove {
	struct device *dev;
	struct device *dpu_top;
	struct drm_device *drm;

	/* add to drv ove_list */
	struct list_head list;
	void *major_reqer;
	void *minor_reqer;

	uint32_t id; /* hw id */
	void __iomem *reg;
	struct lombo_dpu_clk *clk;

	spinlock_t en_lock;
	atomic_t en_cnt;

	uint8_t pending_release;
};

static inline int _ove_suspend(struct lombo_ove *ove)
{
	csp_ove_disable(ove->id);
	csp_ove_exit(ove->id);
	return 0;
}

static inline int _ove_resume(struct lombo_ove *ove)
{
	csp_ove_init(ove->id);
	csp_ove_set_output_mode(ove->id, 0, 0);
	csp_ove_enable(ove->id);
	csp_ove_set_backcolor(ove->id, 0x0);
	return 0;
}

static int lombo_ove_set_dpms(struct lombo_ove *ove, int dpms)
{
	int en_cnt;

	if (dpms)
		en_cnt = atomic_inc_return(&ove->en_cnt);
	else
		en_cnt = atomic_dec_return(&ove->en_cnt);

	if (en_cnt == 1) {
		lombo_dpu_clk_prepare_enable(ove->clk, 0);
		spin_lock(&ove->en_lock);
		_ove_resume(ove);
		spin_unlock(&ove->en_lock);
	} else if (en_cnt == 0) {
		spin_lock(&ove->en_lock);
		_ove_suspend(ove);
		spin_unlock(&ove->en_lock);
		lombo_dpu_clk_disable_unprepare(ove->clk);
	} else if (en_cnt < 0) {
		DRM_ERROR("ove%d dpms(%d) invalid en_cnt=%d\n",
			  ove->id, dpms, en_cnt);
	}

	if (en_cnt >= 0)
		DRM_DEBUG_KMS("ove%d: dpms=%d, en_cnt=%d\n",
			      ove->id, dpms, en_cnt);
	return 0;
}

static inline int lombo_ove_check_child_fb(struct lombo_ove *ove,
	struct lombo_fb_array_fb *child_fb)
{
	const struct drm_format_info *format;

	if (!child_fb || !child_fb->fb) {
		DRM_ERROR("null pointer\n");
		return -EINVAL;
	}

	format = child_fb->fb->format;
	/* Check whether this ove supports the fb pixel format. */
	if (format->is_yuv) {
		DRM_ERROR("ove not support yuv fmt 0x%08x\n",
			format->format);
		return -ENOTSUPP;
	}

	return 0;
}

static int lombo_check_fb_cfg(struct lombo_ove *ove,
	struct lombo_ove_fb_cfg *req_cfg)
{
	struct drm_framebuffer *fb;
	struct lombo_fb_array_fb *child_fb[OVE_OVL_NUM];
	uint32_t fb_num, i;
	int ret;

	if (!req_cfg ||
		!req_cfg->reqer ||
		!req_cfg->fb_cfg ||
		!req_cfg->fb_cfg->fb) {
		DRM_ERROR("req cfg invalid\n");
		return -EINVAL;
	}

	fb = req_cfg->fb_cfg->fb;
	if (fb->modifier != DRM_FORMAT_MOD_LOMBO_FB_ARRAY) {
		DRM_ERROR("bad mod=%llu for ove\n", fb->modifier);
		return -EINVAL;
	}

	fb_num = lombo_fb_array_get_fb_num(fb);
	if ((fb_num < 2) || (fb_num > OVE_OVL_NUM)) {
		DRM_ERROR("invalid fb_num=%d not in [2,%d]\n",
			fb_num, OVE_OVL_NUM);
		return -EINVAL;
	}

	for (i = 0; i < fb_num; i++) {
		child_fb[i] = lombo_fb_array_get_fb(fb, i);
		ret = lombo_ove_check_child_fb(ove, child_fb[i]);
		if (ret) {
			DRM_ERROR("check fb%d fail\n", i);
			return ret;
		}
	}

	return 0;
}

struct lombo_ove *lombo_ove_request(struct lombo_ove_fb_cfg *req_cfg)
{
	struct lombo_ove *ove = NULL;
	int found = 0;

	if (lombo_check_fb_cfg(ove, req_cfg))
		return NULL;

	spin_lock(&ove_lock);

	list_for_each_entry(ove, &ove_list, list) {
		/* make sure one reqer ask for one se */
		if (ove->major_reqer == req_cfg->reqer) {
			spin_unlock(&ove_lock);
			return ove;
		}
	}

	list_for_each_entry(ove, &ove_list, list) {
		if (ove->major_reqer == NULL) {
			/* assert minor_reqer is null if major is null */
			ove->major_reqer = req_cfg->reqer;
			found = 1;
			goto out;
		}
	}

	list_for_each_entry(ove, &ove_list, list) {
		if (ove->minor_reqer == req_cfg->reqer) {
			ove->minor_reqer = ove->major_reqer;
			ove->major_reqer = req_cfg->reqer;
			found = 2;
			goto out;
		}
	}

	list_for_each_entry(ove, &ove_list, list) {
		if (ove->minor_reqer == NULL) {
			ove->minor_reqer = ove->major_reqer;
			ove->major_reqer = req_cfg->reqer;
			found = 3;
			goto out;
		}
	}

out:
	spin_unlock(&ove_lock);

	if (!found) {
		DRM_ERROR("no available ove!");
		return ERR_PTR(-EBUSY);
	}

	lombo_ove_set_dpms(ove, 1);
	ove->pending_release = false;
	DRM_DEBUG_KMS("found@%d: request ove%d\n", found, ove->id);

	return ove;
}

/* donot call this func in irq */
void lombo_ove_release(struct lombo_ove *ove,
	void *reqer, uint32_t do_disable)
{
	if (!ove)
		return;
	if ((reqer != ove->major_reqer) && (reqer != ove->minor_reqer))
		return;

	spin_lock(&ove_lock);
	if (ove->major_reqer == reqer) {
		ove->major_reqer = NULL;
		if (ove->minor_reqer) {
			ove->major_reqer = ove->minor_reqer;
			ove->minor_reqer = NULL;
			do_disable = 1;
		} else {
			ove->pending_release = true;
		}
	} else if (ove->minor_reqer == reqer) {
		ove->minor_reqer = NULL;
		do_disable = 1; /* do_disable when major exist */
	}
	spin_unlock(&ove_lock);

	if (do_disable)
		lombo_ove_set_dpms(ove, 0);
}

static int lombo_ove_convert_drm_format(int format)
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

	default:
		DRM_ERROR("unsupport format[%.4s]\n", (char *)&format);
		return 0;
	}
}

static int lombo_ove_update_child_fb(struct lombo_ove *ove,
	struct lombo_fb_array_fb *child_fb, uint32_t id)
{
	struct drm_framebuffer *fb = child_fb->fb;
	const struct drm_format_info *format = fb->format;

	dpu_alpha_mode_t alpha_mode;
	unsigned int alpha_value;
	unsigned int premultiplied;
	dpu_rectangle_t rect;
	dpu_fb_t dpu_fb;
	int i;

	if (!ove || !child_fb) {
		DRM_ERROR("null pointer\n");
		return -EINVAL;
	}

	alpha_mode = child_fb->alpha_mode;
	alpha_value = child_fb->alpha_value;
	premultiplied = child_fb->is_premul;
	if (!format->has_alpha) {
		alpha_mode = DPU_PLANE_ALPHA;
		alpha_value = 0xFF;
	}
	csp_ove_ovl_set_alpha_mode(ove->id, id, alpha_mode, premultiplied);
	csp_ove_ovl_set_alpha_value(ove->id, id, alpha_value);

	rect.x = child_fb->dst.x;
	rect.y = child_fb->dst.y;
	rect.w = child_fb->dst.w;
	rect.h = child_fb->dst.h;
	csp_ove_ovl_set_dest_rectangle(ove->id, id, &rect);
	csp_ove_ovl_set_work_mode(ove->id, id, DPU_WINDOW_NORMAL_MODE);

	memset((void *)&dpu_fb, 0, sizeof(dpu_fb));
	for (i = 0; i < format->num_planes; i++) {
		struct drm_gem_cma_object *cma_obj;

		cma_obj = drm_fb_cma_get_gem_obj(fb, i);
		dpu_fb.addr[i] = cma_obj->paddr + fb->offsets[i]
			+ fb->pitches[i] * (child_fb->src.y >> 16)
			+ ((child_fb->src.x >> 16) * format->cpp[i]);
		dpu_fb.addr[i] <<= 3; /* in bit unit */
		dpu_fb.linestride_bit[i] = fb->pitches[i] << 3;
	}
	dpu_fb.format = lombo_ove_convert_drm_format(format->format);
	if (csp_ove_ovl_set_buffer(ove->id, id, &dpu_fb)) {
		DRM_ERROR("ove ovl=%d set buf fail\n", id);
		return -EINVAL;
	}

	csp_ove_ovl_show(ove->id, id);

	return 0;
}

int lombo_ove_update_plane(struct lombo_ove *ove,
		struct lombo_ove_fb_cfg *req_cfg)
{
	struct lombo_fb_cfg *fb_cfg = req_cfg->fb_cfg;
	struct drm_framebuffer *fb = fb_cfg->fb;
	struct lombo_fb_array_fb *child_fb;
	uint32_t fb_num, i;
	int ret = 0;

	struct drm_lombo_rect_wh ove_win; /* ove out win, Q16 */

	/* get ove out win */
	ret = lombo_fb_array_get_src_win(fb, &ove_win);
	if (ret) {
		DRM_ERROR("fb array get src win fail\n");
		goto out;
	}
	DRM_DEBUG_KMS("ove_win[%d,%d,%d,%d], fb_cfg_src[%d,%d,%d,%d]\n",
		      ove_win.x, ove_win.y, ove_win.w, ove_win.h,
		      fb_cfg->src_x, fb_cfg->src_y,
		      fb_cfg->src_w, fb_cfg->src_h);

	/* return args to caller */
	req_cfg->out_fmt = DRM_FORMAT_ARGB8888;

	/* cfg ove */
	csp_ove_set_window_size(ove->id, ove_win.w >> 16, ove_win.h >> 16);

	/* update fb */
	fb_num = lombo_fb_array_get_fb_num(fb);
	for (i = 0; i < fb_num; i++) {
		child_fb = lombo_fb_array_get_fb(fb, i);
		ret = lombo_ove_update_child_fb(ove, child_fb, i);
		if (ret) {
			DRM_ERROR("update child fb(%d) fail\n", i);
			goto out;
		}
	}
	/* disable no-fb ovl */
	for (; i < OVE_OVL_NUM; i++)
		csp_ove_ovl_hide(ove->id, i);

	csp_ove_set_output_port(ove->id, req_cfg->out_sel);

out:
	return ret;
}

int lombo_ove_disable_plane(struct lombo_ove *ove, void *reqer)
{
	int i;

	DRM_DEBUG_KMS("\n");

	if (!reqer || (reqer != ove->major_reqer)) {
		DRM_ERROR("not match major reqer: %p,%p\n",
			reqer, ove->major_reqer);
		return -EINVAL;
	}

	for (i = 0; i < OVE_OVL_NUM; i++)
		csp_ove_ovl_hide(ove->id, i);

	return 0;
}

void lombo_ove_commit(struct lombo_ove *ove, void *reqer)
{
	csp_ove_load_dbr(ove->id);
}

int lombo_ove_page_flip_worker(struct lombo_ove *ove, void *reqer)
{
	if (ove->pending_release) {
		lombo_ove_set_dpms(ove, 0);
		ove->pending_release = 0;
	}
	return 0;
}

static int lombo_ove_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = dev->of_node;
	struct drm_device *drm = master_data;

	struct device_node *top;
	struct platform_device *top_pdev;
	struct lombo_ove *ove;
	struct resource *res;
	uint32_t value;

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

	ove = kzalloc(sizeof(*ove), GFP_KERNEL);
	if (!ove) {
		dev_err(dev, "alloc for ove failed\n");
		return -ENOMEM;
	}
	ove->dev = dev;
	ove->dpu_top = &top_pdev->dev;
	ove->drm = drm;
	dev_set_drvdata(dev, ove);

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	ove->reg = devm_ioremap(dev, res->start,
			resource_size(res));
	if (ove->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_ove_set_register_base(ove->id,
				(unsigned long)ove->reg,
				(u32)resource_size(res));

	/* init clock */
	ove->clk = lombo_dpu_get_clk(dev, ove->dpu_top, DPU_MOD_DC0);
	if (!ove->clk) {
		DRM_ERROR("lombo_dpu_get_clk for ove fail\n");
		return -EINVAL;
	}
	if (!of_property_read_u32(np, "sclk", &value))
		lombo_dpu_clk_set_parent(ove->clk, value);
	else
		lombo_dpu_clk_set_parent(ove->clk, 0);
	if (!of_property_read_u32(np, "clk_rate", &value))
		lombo_dpu_clk_set_rate(ove->clk, value);
	else
		DRM_ERROR("failed to of get ove clk_rate\n");

	spin_lock_init(&ove->en_lock);
	list_add_tail(&ove->list, &ove_list);

	return 0;
}

static void lombo_ove_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_ove *ove = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	list_del(&ove->list);
	lombo_dpu_put_clk(&ove->clk);
	dev_set_drvdata(dev, NULL);
}

static const struct component_ops lombo_ove_ops = {
	.bind	= lombo_ove_bind,
	.unbind	= lombo_ove_unbind,
};

static int lombo_ove_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_ove_ops);
}

static int lombo_ove_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_ove_ops);
	return 0;
}

static const struct of_device_id lombo_ove_of_table[] = {
	{ .compatible = "lombo,lombo-n8v0-ove-0" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_ove_of_table);

static struct platform_driver lombo_ove_platform_driver = {
	.probe		= lombo_ove_probe,
	.remove		= lombo_ove_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-ove",
		.of_match_table	= lombo_ove_of_table,
	},
};
module_platform_driver(lombo_ove_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU OVE Driver");
MODULE_LICENSE("GPL");

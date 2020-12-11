/**
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/lombo_drm.h>

#include "lombo_crtc.h"
#include "lombo_fb_array.h"

/**
 * @base: drm fb base.
 * @src: min-included-dst rect. Q16.
 * @num_fbs: number of fb in array.
 * @fbs: array of fbs.
 */
struct lombo_fb_array {
	struct drm_framebuffer base;
	struct drm_lombo_rect_wh src;
	uint32_t num_fbs;
	struct lombo_fb_array_fb *fbs;
};

static inline struct lombo_fb_array *to_lombo_fb_array(
	struct drm_framebuffer *fb)
{
	return container_of(fb, struct lombo_fb_array, base);
}

static inline struct lombo_fb_array *to_lombo_fb_array_check(
	struct drm_framebuffer *fb)
{
	if (fb && (fb->modifier == DRM_FORMAT_MOD_LOMBO_FB_ARRAY))
		return container_of(fb, struct lombo_fb_array, base);
	DRM_ERROR("no a lombo fb array\n");
	return NULL;
}

static void lombo_fb_array_destroy(struct drm_framebuffer *fb)
{
	struct lombo_fb_array *fb_array = to_lombo_fb_array(fb);
	int i;

	DRM_DEBUG_KMS("\n");
	for (i = 0; i < fb_array->num_fbs; i++) {
		struct lombo_fb_array_fb *pfb = fb_array->fbs + i;

		if (pfb->fb)
			drm_framebuffer_unreference(pfb->fb);
	}
	drm_framebuffer_cleanup(fb);
	kfree(fb_array);
}

static int lombo_fb_array_create_handle(struct drm_framebuffer *fb,
	struct drm_file *file_priv, unsigned int *handle)
{
	DRM_ERROR("Not support create handle\n");
	return -ENOTSUPP;
}

static const struct drm_framebuffer_funcs lombo_fb_array_funcs = {
	.destroy = lombo_fb_array_destroy,
	.create_handle = lombo_fb_array_create_handle,
};

static struct drm_framebuffer *
lombo_gem_fb_array_alloc(struct drm_device *drm,
	struct drm_mode_fb_cmd2 *mode_cmd,
	struct drm_lombo_fb_array_fbcmd *fbcmd, uint32_t num_fb)
{
	struct lombo_fb_array *fb_array;
	struct lombo_fb_array_fb *pfb, *pfb_end;
	struct drm_lombo_rect_pt rect;
	int ret;

	fb_array = kzalloc(sizeof(*fb_array) + num_fb * sizeof(*pfb),
			   GFP_KERNEL);
	if (!fb_array)
		return ERR_PTR(-ENOMEM);

	drm_helper_mode_fill_fb_struct(drm, &fb_array->base, mode_cmd);

	rect.left = rect.top = UINT_MAX;
	rect.right = rect.bottom = 0;
	fb_array->fbs = (struct lombo_fb_array_fb *)(
			(char *)fb_array + sizeof(*fb_array));
	pfb = fb_array->fbs;
	pfb_end = pfb + num_fb;
	for (; pfb != pfb_end; pfb++) {
		struct drm_framebuffer *fb;
		struct drm_lombo_rect_wh *psrc, *pdst;
		unsigned int fb_width, fb_height;

		fb = drm_framebuffer_lookup(drm, fbcmd->fb_id);
		if (!fb) {
			DRM_ERROR("bad fb_id %d\n", fbcmd->fb_id);
			ret = -ENOENT;
			goto err_out;
		}

		if (fb->format->is_yuv) {
			DRM_ERROR("fb of array is yuv\n");
			ret = -EINVAL;
			goto err_out;
		}

		psrc = &fbcmd->src;
		pdst = &fbcmd->dst;

		fb_width = fb->width << 16;
		fb_height = fb->height << 16;
		if ((psrc->w == 0) ||
			(psrc->w > fb_width) ||
			(psrc->x > fb_width - psrc->w) ||
			(psrc->h == 0) ||
			(psrc->h > fb_height) ||
			(psrc->y > fb_height - psrc->h)) {
			DRM_ERROR("Ivalid src of fb\n");
			ret = -ENOSPC;
			goto err_out;
		}

		if ((psrc->w != (pdst->w << 16)) ||
			(psrc->h != (pdst->h << 16))) {
			DRM_ERROR("src(%d.%x,%d.%x) not equal dst(%d,%d)\n",
				psrc->w >> 16, psrc->h >> 16,
				psrc->w & 0xFFFF, psrc->h & 0xFFFF,
				pdst->w, pdst->h);
			ret = -EINVAL;
			goto err_out;
		}

		fb_width = fb_array->base.width;
		fb_height = fb_array->base.height;
		if ((pdst->w > fb_width) ||
			(pdst->x > fb_width - pdst->w) ||
			(pdst->h > fb_height) ||
			(pdst->y > fb_height - pdst->h)) {
			DRM_ERROR("Ivalid dst\n");
			ret = -ENOSPC;
			goto err_out;
		}

		pfb->fb = fb;
		pfb->src = fbcmd->src;
		pfb->dst = fbcmd->dst;
		pfb->alpha_mode = fbcmd->alpha_mode;
		pfb->alpha_value = fbcmd->alpha_value;
		pfb->is_premul = fbcmd->is_premul;
		DRM_DEBUG_KMS("fb%d[%ux%u]:src[%u,%u,%u,%u],dst[%u,%u,%u,%u]\n",
			fb->base.id, fb->width, fb->height,
			pfb->src.x >> 16, pfb->src.y >> 16,
			pfb->src.w >> 16, pfb->src.h >> 16,
			pfb->dst.x, pfb->dst.y, pfb->dst.w, pfb->dst.h);

		if (pdst->x < rect.left)
			rect.left = pdst->x;
		if (pdst->y < rect.top)
			rect.top = pdst->y;
		if (pdst->x + pdst->w > rect.right)
			rect.right = pdst->x + pdst->w;
		if (pdst->y + pdst->h > rect.bottom)
			rect.bottom = pdst->y + pdst->h;

		fb_array->num_fbs++;
		fbcmd++;
	}
	DRM_DEBUG_KMS("rect[%d,%d,%d,%d]\n",
		      rect.left, rect.top, rect.right, rect.bottom);
	fb_array->src.x = rect.left << 16;
	fb_array->src.y = rect.top << 16;
	fb_array->src.w = (rect.right - rect.left) << 16;
	fb_array->src.h = (rect.bottom - rect.top) << 16;

	ret = drm_framebuffer_init(drm, &fb_array->base, &lombo_fb_array_funcs);
	if (ret) {
		DRM_ERROR("Failed to init fb_array: %d\n", ret);
		goto err_out;
	}

	return &fb_array->base;

err_out:
	for (; pfb >= fb_array->fbs; pfb--) {
		if (pfb->fb)
			drm_framebuffer_unreference(pfb->fb);
	}
	kfree(fb_array);

	return ERR_PTR(ret);
}

struct drm_framebuffer *lombo_fb_array_create(struct drm_device *drm,
	struct drm_file *file_priv, struct drm_mode_fb_cmd2 *mode_cmd)
{
	struct drm_framebuffer *fb = ERR_PTR(-EINVAL);
	const struct drm_format_info *info;
	struct drm_gem_object *obj = NULL;
	struct drm_gem_cma_object *cma_obj;
	struct drm_lombo_fb_array_cmd *cmd;
	struct drm_lombo_fb_array_fbcmd *fbcmd;

	if (!(mode_cmd->flags & DRM_MODE_FB_MODIFIERS) ||
		(mode_cmd->modifier[0] != DRM_FORMAT_MOD_LOMBO_FB_ARRAY))
		return ERR_PTR(-EINVAL);

	info = drm_get_format_info(drm, mode_cmd);
	if (!info) {
		DRM_ERROR("get format_info fail\n");
		return ERR_PTR(-EINVAL);
	}

	/* only support 1 plane at present */
	if (info->num_planes != 1)
		return ERR_PTR(-EINVAL);

	/* only support rgb format at present */
	if (info->is_yuv)
		return ERR_PTR(-EINVAL);

	obj = drm_gem_object_lookup(drm, file_priv,
				    mode_cmd->handles[0]);
	if (!obj) {
		DRM_DEBUG_KMS("Failed to lookup GEM object\n");
		goto err_gem_object_put;
	}
	if (obj->size > PAGE_SIZE) {
		DRM_DEBUG_KMS("too large size for fb array\n");
		goto err_gem_object_put;
	}

	cma_obj = to_drm_gem_cma_obj(obj);
	if (!cma_obj->vaddr) {
		DRM_DEBUG_KMS("need alloc by drm gem cma\n");
		goto err_gem_object_put;
	}

	cmd = (void *)cma_obj->vaddr;
	if (cmd->num_fb <= 1) {
		DRM_DEBUG_KMS("need more fb, num_fb=%d\n", cmd->num_fb);
		goto err_gem_object_put;
	}
	fbcmd = (void *)(cma_obj->vaddr + cmd->data_offset);
	fb = lombo_gem_fb_array_alloc(drm, mode_cmd, fbcmd, cmd->num_fb);

err_gem_object_put:
	drm_gem_object_unreference_unlocked(obj);
	return fb;
}

/* rect that returned is Q16 */
int lombo_fb_array_get_src_win(struct drm_framebuffer *fb,
	struct drm_lombo_rect_wh *rect)
{
	struct lombo_fb_array *fb_array = to_lombo_fb_array_check(fb);

	if (!fb_array)
		return -EINVAL;
	memcpy(rect, &fb_array->src, sizeof(*rect));
	return 0;
}

uint32_t lombo_fb_array_get_fb_num(struct drm_framebuffer *fb)
{
	struct lombo_fb_array *fb_array = to_lombo_fb_array_check(fb);

	if (!fb_array)
		return 0;
	return fb_array->num_fbs;
}

struct lombo_fb_array_fb *lombo_fb_array_get_fb(
	struct drm_framebuffer *fb, uint32_t index)
{
	struct lombo_fb_array *fb_array = to_lombo_fb_array_check(fb);

	if (!fb_array || (index >= fb_array->num_fbs))
		return NULL;

	return fb_array->fbs + index;
}


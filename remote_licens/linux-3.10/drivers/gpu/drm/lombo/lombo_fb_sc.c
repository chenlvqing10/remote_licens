/* lombo_framebuffer.c
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

#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>

struct lombo_fb_sc {
	struct drm_framebuffer fb;
	uint32_t color[4];
};

static inline struct lombo_fb_sc *to_lombo_fb_sc(struct drm_framebuffer *fb)
{
	return container_of(fb, struct lombo_fb_sc, fb);
}

static void lombo_fb_sc_destroy(struct drm_framebuffer *fb)
{
	struct lombo_fb_sc *fb_sc = to_lombo_fb_sc(fb);

	DRM_DEBUG_KMS("\n");
	drm_framebuffer_cleanup(fb);
	kfree(fb_sc);
}

static int lombo_fb_sc_create_handle(struct drm_framebuffer *fb,
	struct drm_file *file_priv, unsigned int *handle)
{
	DRM_ERROR("no handle for lombo_fb_sc\n");
	return -ENODEV;
}

static const struct drm_framebuffer_funcs lombo_fb_sc_funcs = {
	.destroy = lombo_fb_sc_destroy,
	.create_handle = lombo_fb_sc_create_handle,
};

static struct drm_framebuffer *
lombo_gem_fb_sc_alloc(struct drm_device *drm,
	struct drm_mode_fb_cmd2 *mode_cmd,
	struct drm_gem_object **obj, unsigned int num_planes)
{
	struct lombo_fb_sc *fb_sc;
	int ret;
	int i;

	fb_sc = kzalloc(sizeof(*fb_sc), GFP_KERNEL);
	if (!fb_sc)
		return ERR_PTR(-ENOMEM);

	drm_helper_mode_fill_fb_struct(drm, &fb_sc->fb, mode_cmd);

	for (i = 0; i < num_planes; i++) {
		struct drm_gem_cma_object *cma_obj = to_drm_gem_cma_obj(obj[i]);
		uint32_t *p = cma_obj->vaddr;

		fb_sc->color[i] = *p;
		DRM_DEBUG_KMS("color[%d]=%x\n", i, fb_sc->color[i]);
	}

	ret = drm_framebuffer_init(drm, &fb_sc->fb, &lombo_fb_sc_funcs);
	if (ret) {
		DRM_ERROR("Failed to init framebuffer: %d\n", ret);
		kfree(fb_sc);
		return ERR_PTR(ret);
	}

	return &fb_sc->fb;
}

struct drm_framebuffer *lombo_fb_sc_create(struct drm_device *drm,
	struct drm_file *file_priv, struct drm_mode_fb_cmd2 *mode_cmd)
{
	struct drm_gem_object *objs[4];
	struct drm_framebuffer *fb;
	int i, num_planes;

	num_planes = drm_format_num_planes(mode_cmd->pixel_format);
	for (i = 0; i < num_planes; i++) {
		struct drm_gem_cma_object *cma_obj;

		objs[i] = drm_gem_object_lookup(drm, file_priv,
						mode_cmd->handles[i]);
		if (!objs[i]) {
			DRM_DEBUG_KMS("Failed to lookup GEM object\n");
			fb = ERR_PTR(-ENOENT);
			goto err_gem_object_put;
		}

		if (objs[i]->size > PAGE_SIZE) {
			DRM_DEBUG_KMS("too large size for fb sc\n");
			fb = ERR_PTR(-EINVAL);
			i++;
			goto err_gem_object_put;
		}

		cma_obj = to_drm_gem_cma_obj(objs[i]);
		if (!cma_obj->vaddr) {
			DRM_DEBUG_KMS("need alloc by drm gem cma\n");
			fb = ERR_PTR(-EINVAL);
			i++;
			goto err_gem_object_put;
		}
	}

	fb = lombo_gem_fb_sc_alloc(drm, mode_cmd, objs, i);

err_gem_object_put:
	for (i--; i >= 0; i--)
		drm_gem_object_unreference_unlocked(objs[i]);

	return fb;
}

uint32_t lombo_fb_sc_get_color(struct drm_framebuffer *fb, unsigned int plane)
{
	uint32_t color = 0;
	struct lombo_fb_sc *fb_sc = to_lombo_fb_sc(fb);

	if (plane < ARRAY_SIZE(fb_sc->color))
		color = fb_sc->color[plane];

	DRM_DEBUG_KMS("color[%d]=0x%8x\n", plane, color);
	return color;
}

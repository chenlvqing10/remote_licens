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
#include <drm/drm_fb_helper.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>

#include <drm/lombo_drm.h>
#include "lombo_drv.h"
#include "lombo_fb_sc.h"
#include "lombo_fb_array.h"
#include "lombo_crtc.h"
#include "lombo_dc.h"

static struct drm_framebuffer *lombo_fb_create(struct drm_device *drm,
	struct drm_file *file_priv, struct drm_mode_fb_cmd2 *mode_cmd)
{
	if (mode_cmd->flags & DRM_MODE_FB_MODIFIERS) {
		DRM_DEBUG_KMS("modifier=0x%llx\n", mode_cmd->modifier[0]);
		if (mode_cmd->modifier[0] == DRM_FORMAT_MOD_LOMBO_SC)
			return lombo_fb_sc_create(drm, file_priv, mode_cmd);
		else if (mode_cmd->modifier[0] == DRM_FORMAT_MOD_LOMBO_FB_ARRAY)
			return lombo_fb_array_create(drm, file_priv, mode_cmd);
		return ERR_PTR(-EINVAL);
	}

	return drm_fb_cma_create(drm, file_priv, mode_cmd);
}

static void lombo_fb_output_poll_changed(struct drm_device *dev)
{
	DRM_DEBUG_KMS("%d\n", __LINE__);
	/* drm_fb_helper_hotplug_event(dev->fb_helper); */
}

static const struct drm_mode_config_funcs lombo_mode_config_funcs = {
	.fb_create = lombo_fb_create,
	.output_poll_changed = lombo_fb_output_poll_changed,
};

int lombo_fb_kmap(uint64_t phy_addr, uint32_t size, int cache,
	void **map_vaddr, uint32_t *map_offset)
{
	uint64_t addr;
	uint32_t offset;
	void *vaddr;

	int npages;
	struct page **pages, **p, *cur_page;
	pgprot_t pgprot;
	int i;

	offset = (uint32_t)(phy_addr & (PAGE_SIZE - 1));
	addr = phy_addr - offset;
	DRM_DEBUG("offset=0x%x, addr=0x%llx\n", offset, addr);

	size += offset;
	npages = PAGE_ALIGN(size) / PAGE_SIZE;
	pages = vmalloc(npages * sizeof(*pages));
	if (!pages) {
		DRM_ERROR("alloc for %d pages fail\n", npages);
		return -ENOMEM;
	}

	p = pages;
	cur_page = phys_to_page(addr);
	for (i = 0; i < npages; i++)
		*(p++) = cur_page++;

	if (cache)
		pgprot = PAGE_KERNEL;
	else
		pgprot = pgprot_noncached(PAGE_KERNEL);

	vaddr = vmap(pages, npages, VM_MAP, pgprot);
	vfree(pages);

	if (!vaddr) {
		DRM_ERROR("vmap fail\n");
		return -EINVAL;
	}
	*map_vaddr = vaddr;
	*map_offset = offset;

	return 0;
}

void lombo_fb_kunmap(void **map_vaddr)
{
	if (map_vaddr && *map_vaddr) {
		vunmap(*map_vaddr);
		*map_vaddr = NULL;
	}
}

#ifdef CONFIG_DRM_FBDEV_EMULATION
static int lombo_fbdev_copy_boot_fb(struct drm_fb_helper *fb_helper,
	struct lombo_boot_disp *bd)
{
	struct drm_framebuffer *fb;
	const struct drm_format_info *format;
	struct drm_gem_cma_object *cma_obj;
	uint32_t fb_height;

	char *map_vaddr, *psrc, *psrc_end, *pdst;
	uint32_t map_offset;
	uint32_t size;
	uint32_t copy_btyes;
	int ret;

	if (!bd || !bd->phy_addr || !bd->pitch) {
		DRM_DEBUG("no boot fb\n");
		return -EINVAL;
	}
	if (!(fb_helper && fb_helper->fb)) {
		DRM_INFO("%s: fb_helper=%p,fb=%p\n", __func__,
			fb_helper, fb_helper ? fb_helper->fb : NULL);
		return -EINVAL;
	}
	fb = fb_helper->fb;
	#ifdef CONFIG_DRM_FBDEV_OVERALLOC
	fb_height = fb->height * 100 / CONFIG_DRM_FBDEV_OVERALLOC;
	#else
	fb_height = fb->height;
	#endif

	if (bd->fmt != fb->format->format) {
		DRM_INFO("%s: no match pixel format %x %x\n",
			__func__, bd->fmt, fb->format->format);
		return -EINVAL;
	}
	format = fb->format;

	/* kmap boot fb */
	size = bd->pitch * bd->height;
	ret = lombo_fb_kmap(bd->phy_addr, size, 1,
			(void **)&map_vaddr, &map_offset);
	if (ret) {
		DRM_INFO("map boot fb fail, ret=%d\n", ret);
		return ret;
	}

	/* fixup copy region */
	if (bd->copy_w > fb->width) {
		bd->copy_x += (bd->copy_w - fb->width) >> 1;
		bd->copy_w = fb->width;
		DRM_DEBUG("fixup copyregion: x=%d, w=%d\n",
			bd->copy_x, bd->copy_w);
	}
	if (bd->copy_h > fb_height) {
		bd->copy_y += (bd->copy_h - fb_height) >> 1;
		bd->copy_h = fb_height;
		DRM_DEBUG("fixup copyregion: y=%d, h=%d\n",
			bd->copy_y, bd->copy_h);
	}

	psrc = map_vaddr + map_offset;
	psrc += bd->copy_y * bd->pitch + bd->copy_x * format->cpp[0];
	copy_btyes = bd->copy_w * format->cpp[0];

	cma_obj = drm_fb_cma_get_gem_obj(fb, 0);
	pdst = (char *)(cma_obj->vaddr) + fb->offsets[0];
	if (fb_height > bd->copy_h)
		pdst += ((fb_height - bd->copy_h) >> 1) * fb->pitches[0];
	if (fb->width > bd->copy_w)
		pdst += ((fb->width - bd->copy_w) >> 1) * format->cpp[0];

	psrc_end = psrc + bd->copy_h * bd->pitch;
	DRM_DEBUG("map:vaddr=%p,ofs=%u;psrc=%p,_end=%p,cmavaddr=%p,pdst=%p\n",
		map_vaddr, map_offset,
		psrc, psrc_end,
		cma_obj->vaddr, pdst);
	for (; psrc != psrc_end; psrc += bd->pitch) {
		memcpy((void *)pdst, (void *)psrc, copy_btyes);
		pdst += fb->pitches[0];
	}

	lombo_fb_kunmap((void **)&map_vaddr);
	return 0;
}
#endif

static int lombo_fb_disable_boot_fb(struct lombo_boot_disp *bd)
{
	struct drm_plane *plane;
	const struct drm_plane_funcs *funcs;

	if (!bd || !bd->boot_plane)
		return -EINVAL;

	plane = bd->boot_plane;
	funcs = plane->funcs;
	if (funcs && funcs->disable_plane)
		return funcs->disable_plane(plane);
	else
		return -EINVAL;
}

int lombo_framebuffer_init(struct drm_device *drm)
{
	#define PREFERRED_BPP	32
	#define MAX_CONNECTOR	4
	struct lombo_drv *drv = drm->dev_private;
	int ret = 0;

	drm->mode_config.max_width = 4096;
	drm->mode_config.max_height = 4096;

	drm->mode_config.funcs = &lombo_mode_config_funcs;

#ifdef CONFIG_DRM_FBDEV_EMULATION
	drv->fbdev = drm_fbdev_cma_init(drm, PREFERRED_BPP,
			drm->mode_config.num_crtc, MAX_CONNECTOR);
	if (IS_ERR(drv->fbdev)) {
		dev_err(drm->dev, "Failed to init fbdev_cma\n");
		ret = PTR_ERR(drv->fbdev);
	}
	lombo_fbdev_copy_boot_fb(drm->mode_config.fb_helper, drv->boot_disp);
#else
	drv->fbdev = NULL;
#endif
	lombo_fb_disable_boot_fb(drv->boot_disp);

	return ret;
}

void lombo_framebuffer_free(struct drm_device *drm)
{
	struct lombo_drv *drv = drm->dev_private;

	drm->mode_config.funcs = NULL;

#ifdef CONFIG_DRM_FBDEV_EMULATION
	drm->mode_config.fb_helper = NULL;
	drm_fbdev_cma_fini(drv->fbdev);
#endif
	drv->fbdev = NULL;
}

int lombo_fb_set_transparent_rect(struct fb_info *info,
	struct drm_lombo_rect_wh *rect)
{
	struct drm_fb_helper *fb_helper = info->par;
	struct drm_device *dev = fb_helper->dev;
	struct drm_crtc *crtc;
	struct lombo_crtc *ecrtc;
	int ret = 0;

	drm_modeset_lock_all(dev);
	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		if (crtc->fb == fb_helper->fb) {
			ecrtc = drm_crtc_to_lombo_crtc(crtc);
			ret = lombo_dc_set_fb_transparent_rect(ecrtc->dc,
							       fb_helper->fb,
							       rect);
			break; /* only set a rect */
		}
	}
	drm_modeset_unlock_all(dev);

	return ret;
}

int lombo_fb_get_transparent_rect(struct fb_info *info,
	struct drm_lombo_rect_wh *rect)
{
	struct drm_fb_helper *fb_helper = info->par;
	struct drm_device *dev = fb_helper->dev;
	struct drm_crtc *crtc;
	struct lombo_crtc *ecrtc;
	int ret = -ENODATA;

	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		if (crtc->fb == fb_helper->fb) {
			ecrtc = drm_crtc_to_lombo_crtc(crtc);
			ret = lombo_dc_get_fb_transparent_rect(ecrtc->dc,
							       fb_helper->fb,
							       rect);
			break; /* only get a rect */
		}
	}
	return ret;
}


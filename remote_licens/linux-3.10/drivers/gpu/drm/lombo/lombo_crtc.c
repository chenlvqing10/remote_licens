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
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>

#include <drm/lombo_drm.h>
#include "lombo_drv.h"
#include "lombo_crtc.h"
#include "lombo_tcon.h"
#include "lombo_dc.h"

static struct drm_encoder *lombo_crtc_get_encoder(struct drm_crtc *crtc)
{
	struct drm_device *drm = crtc->dev;
	struct drm_encoder *encoder;

	list_for_each_entry(encoder, &drm->mode_config.encoder_list, head)
		if (encoder->crtc == crtc)
			return encoder;

	return NULL;
}

static void lombo_crtc_dpms(struct drm_crtc *crtc, int mode)
{
	DRM_DEBUG_KMS("%d\n", __LINE__);
}

static bool lombo_crtc_mode_fixup(struct drm_crtc *crtc,
		const struct drm_display_mode *mode,
		struct drm_display_mode *adjusted_mode)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	uint32_t dc_out_csfmt;
	uint32_t tcon_out_csfmt;
	int ret;

	DRM_DEBUG_KMS("%d\n", __LINE__);

	lombo_tcon_mode_fixup(ecrtc->tcon, mode, adjusted_mode);
	lombo_dc_mode_fixup(ecrtc->dc, mode, adjusted_mode);

	/* check and fixup csc */
	tcon_out_csfmt = lombo_tcon_get_out_csfmt(ecrtc->tcon);
	dc_out_csfmt = tcon_out_csfmt;
	if (ecrtc->dc)
		dc_out_csfmt = lombo_dc_get_csfmt(ecrtc->dc);
	DRM_DEBUG_KMS("dc_out_csfmt=%d, tcon_out_csfmt=%d\n",
		dc_out_csfmt, tcon_out_csfmt);
	if (dc_out_csfmt != tcon_out_csfmt) {
		ret = lombo_tcon_set_in_csfmt(ecrtc->tcon, dc_out_csfmt);
		if (ret && ret != -EBUSY)
			DRM_INFO("%s: fixme: ret=%d\n", __func__, ret);
	}

	return true;
}

static void lombo_crtc_prepare(struct drm_crtc *crtc)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct drm_encoder *enc;

	DRM_DEBUG_KMS("%d\n", __LINE__);

	lombo_dc_set_dpms(ecrtc->dc, 1);

	enc = lombo_crtc_get_encoder(crtc);
	lombo_tcon_set_state(ecrtc->tcon, enc, 1);
}

static int lombo_crtc_mode_set_base(struct drm_crtc *crtc,
	int x, int y, struct drm_framebuffer *old_fb)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	int ret;

	DRM_DEBUG_KMS("%d\n", __LINE__);

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	if (ret)
		return ret;

	lombo_dc_set_mode_base(ecrtc->dc, crtc, x, y, old_fb);

	lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);

	return 0;
}

static int lombo_crtc_mode_set(struct drm_crtc *crtc,
	struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode, int x, int y,
	struct drm_framebuffer *old_fb)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct drm_encoder *enc;
	int ret;

	DRM_DEBUG_KMS("%d\n", __LINE__);

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	if (ret)
		return ret;

	lombo_dc_set_mode(ecrtc->dc,
		crtc, mode, adjusted_mode,
		x, y, old_fb);

	enc = lombo_crtc_get_encoder(crtc);
	lombo_tcon_set_mode(ecrtc->tcon, enc, mode, adjusted_mode);

	lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);

	return 0;
}

static void lombo_crtc_commit(struct drm_crtc *crtc)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);

	DRM_DEBUG_KMS("%d\n", __LINE__);

	lombo_dc_commit(ecrtc->dc);
	lombo_tcon_commit(ecrtc->tcon);
}

static void lombo_crtc_commit_sw(struct drm_crtc *crtc)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct lombo_drv *drv = crtc->dev->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct drm_crtc_helper_funcs *funcs = crtc->helper_private;

	if (bd && (bd->crtc == crtc)) {
		DRM_DEBUG("skip commit for sw...\n");
		return;
	}

	DRM_DEBUG("switching to do commit\n");
	if (funcs)
		funcs->commit = lombo_crtc_commit;
	lombo_tcon_commit(ecrtc->tcon);
}

static void lombo_crtc_disable(struct drm_crtc *crtc)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct drm_encoder *enc;
	int ret;

	DRM_DEBUG_KMS("%s\n", crtc->fb ? "has fb" : "no fb");

	if (!crtc->fb)
		return;

	enc = lombo_crtc_get_encoder(crtc);
	lombo_tcon_disable(ecrtc->tcon);
	lombo_tcon_set_state(ecrtc->tcon, enc, 0);

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	lombo_dc_disable(ecrtc->dc, crtc);
	lombo_dc_set_dpms(ecrtc->dc, 0);
	if (!ret)
		lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);

	lombo_dc_commit(ecrtc->dc);
	lombo_crtc_handle_vblank_events(crtc);
}

static struct drm_crtc_helper_funcs lombo_crtc_helper_funcs = {
	.dpms		= lombo_crtc_dpms,
	.mode_fixup	= lombo_crtc_mode_fixup,
	.prepare	= lombo_crtc_prepare,
	.mode_set	= lombo_crtc_mode_set,
	.mode_set_base	= lombo_crtc_mode_set_base,
	.commit		= lombo_crtc_commit,
	.disable	= lombo_crtc_disable,
};

static int lombo_crtc_cursor_set(struct drm_crtc *crtc,
	struct drm_file *file_priv, uint32_t handle,
	uint32_t width, uint32_t height)
{
	struct drm_device *drm = crtc->dev;
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct lombo_cursor *cursor = &ecrtc->corsor;
	struct drm_gem_object *obj = NULL;
	int ret;

	DRM_DEBUG_KMS("handle=0x%x->0x%x, width=%d, height=%d\n",
		cursor->handle, handle, width, height);

	if (handle == cursor->handle)
		return 0;

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	if (ret)
		return ret;

	if (cursor->handle) {
		obj = &(cursor->cma_obj->base);
		cursor->handle = 0;
		cursor->cma_obj = NULL;
		lombo_dc_cursor_set(ecrtc->dc, cursor);
	}
	if (handle) {
		obj = drm_gem_object_lookup(drm, file_priv, handle);
		if (!obj) {
			dev_err(drm->dev, "Failed to lookup GEM object\n");
			ret = -ENXIO;
			goto out;
		}
		cursor->cma_obj = to_drm_gem_cma_obj(obj);
		cursor->handle = handle;
		cursor->width = width;
		cursor->height = height;
		cursor->format = DRM_FORMAT_ARGB8888;
		ret = lombo_dc_cursor_set(ecrtc->dc, cursor);
		obj = NULL;
	}

out:
	lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);
	if (obj)
		drm_gem_object_unreference_unlocked(obj);

	return ret;
}

static int lombo_crtc_cursor_move(struct drm_crtc *crtc, int x, int y)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct lombo_cursor *cursor = &ecrtc->corsor;
	int ret;

	if (!cursor->handle || !cursor->cma_obj)
		return -ENXIO;

	DRM_DEBUG_KMS("x=%d,y=%d\n", x, y);

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	if (ret)
		return ret;

	cursor->x = x;
	cursor->y = y;
	ret = lombo_dc_cursor_move(ecrtc->dc, cursor);

	lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);

	return 0;
}

static int lombo_crtc_page_flip(struct drm_crtc *crtc,
		 struct drm_framebuffer *fb,
		 struct drm_pending_vblank_event *event)
{
	struct drm_device *drm = crtc->dev;
	struct lombo_drv *drv = drm->dev_private;
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct drm_framebuffer *old_fb = crtc->fb;
	unsigned long flags;
	int ret = 0;

	DRM_DEBUG_KMS("\n");

	ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
	if (ret)
		return ret;

	crtc->fb = fb;
	ret = lombo_crtc_mode_set_base(crtc, crtc->x, crtc->y, old_fb);
	if (ret) {
		DRM_ERROR("failed to mode set base\n");
		crtc->fb = old_fb;
		goto out;
	}

	if (event) {
		event->pipe = drm_crtc_index(crtc);

		ret = drm_vblank_get(drm, event->pipe);
		if (ret) {
			DRM_ERROR("drm vblank get failed\n");
			goto out;
		}

		event->event.sequence = drm_vblank_count(drm, event->pipe);
		spin_lock_irqsave(&drm->event_lock, flags);
		list_add_tail(&event->base.link, &drv->page_flip_event_list);
		spin_unlock_irqrestore(&drm->event_lock, flags);
	}

out:
	lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);
	return ret;
}

/* handle pageflip events */
void lombo_crtc_handle_vblank_events(struct drm_crtc *crtc)
{
	struct drm_device *drm = crtc->dev;
	struct lombo_drv *drv = drm->dev_private;
	unsigned int pipe = drm_crtc_index(crtc);
	struct drm_pending_vblank_event *e, *t;
	unsigned long flags;

	spin_lock_irqsave(&drm->event_lock, flags);

	list_for_each_entry_safe(e, t, &drv->page_flip_event_list, base.link) {
		if (e->pipe != pipe)
			continue;

		list_del(&e->base.link);
		drm_vblank_put(drm, e->pipe);
		drm_send_vblank_event(drm, -1, e);
	}

	spin_unlock_irqrestore(&drm->event_lock, flags);
}

static int lombo_crtc_set_property(struct drm_crtc *crtc,
	struct drm_property *prop, uint64_t val)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);
	struct lombo_drv *drv = crtc->dev->dev_private;
	int ret;
	bool color_enhance_flag = false;

	DRM_DEBUG_KMS("prop(%s)=%llu\n", prop->name, val);
	if (prop == drv->prop_color_enhance_enable) {
		ecrtc->color_enhance_enable = (int)val;
		color_enhance_flag = true;
	} else if (prop == drv->prop_color_enhance_brightness) {
		ecrtc->color_enhance_brightness = (int)val;
		color_enhance_flag = true;
	} else if (prop == drv->prop_color_enhance_saturation) {
		ecrtc->color_enhance_saturation = (int)val;
		color_enhance_flag = true;
	} else if (prop == drv->prop_color_enhance_contrast) {
		ecrtc->color_enhance_contrast = (int)val;
		color_enhance_flag = true;
	} else if (prop == drv->prop_gamma_enable) {
		ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
		if (ret)
			return ret;

		ecrtc->gamma_enable = (int)val;
		if (val)
			lombo_tcon_set_gamma_state(ecrtc->tcon, true);
		else
			lombo_tcon_set_gamma_state(ecrtc->tcon, false);

		lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);
	} else {
		DRM_ERROR("unknown prop(%s)\n", prop->name);
		return -EINVAL;
	}

	if (color_enhance_flag) {
		ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
		if (ret)
			return ret;

		ret = lombo_dc_set_color_enhance(ecrtc->dc,
			ecrtc->color_enhance_enable,
			ecrtc->color_enhance_brightness,
			ecrtc->color_enhance_saturation,
			ecrtc->color_enhance_contrast);

		lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);
	}

	return ret;
}

/*
 * when we want to update gamma table, user space app should make sure
 * set gamma step logic correct instead of using lombo_tcon_protect_dbreg
 * function because update gamma function may cost a lot time, the correct
 * step for user space app recommend:
 * step1: wait a vblank event and then disable gamma immediately
 * step2: use DRM_IOCTL_MODE_SETGAMMA cmd update gamma
 * step3: wait a vblank event and then enable gamma immediately
 */
static void lombo_crtc_gamma_set(struct drm_crtc *crtc,
	u16 *r, u16 *g, u16 *b, uint32_t start, uint32_t size)
{
	struct lombo_crtc *ecrtc = drm_crtc_to_lombo_crtc(crtc);

	lombo_tcon_gamma_set(ecrtc->tcon, r, g, b, start, size);
}

static void lombo_crtc_destroy(struct drm_crtc *crtc)
{
	DRM_DEBUG_KMS("\n");

	drm_crtc_cleanup(crtc);
	kfree(drm_crtc_to_lombo_crtc(crtc));
}

static const struct drm_crtc_funcs lombo_crtc_funcs = {
	.cursor_set = lombo_crtc_cursor_set,
	.cursor_move = lombo_crtc_cursor_move,
	.set_config = drm_crtc_helper_set_config,
	.page_flip = lombo_crtc_page_flip,
	.destroy = lombo_crtc_destroy,
	.set_property = lombo_crtc_set_property,
	.gamma_set = lombo_crtc_gamma_set,
};

static int lombo_crtc_create_properties(struct drm_crtc *crtc)
{
	struct drm_device *drm = crtc->dev;
	struct lombo_drv *drv = drm->dev_private;

	drv->prop_color_enhance_enable = drm_property_create_range(drm, 0,
		"color_enhance_enable",
		0, 1);
	if (!drv->prop_color_enhance_enable) {
		DRM_ERROR("create color enhance enable prop fail\n");
		return -ENOMEM;
	}

	drv->prop_color_enhance_brightness = drm_property_create_range(drm, 0,
		"color_enhance_brightness",
		0, DC_COLOR_ENHANCE_MAX_VALUE);
	if (!drv->prop_color_enhance_brightness) {
		DRM_ERROR("create color enhance brightness prop fail\n");
		return -ENOMEM;
	}

	drv->prop_color_enhance_saturation = drm_property_create_range(drm, 0,
		"color_enhance_saturation",
		0, DC_COLOR_ENHANCE_MAX_VALUE);
	if (!drv->prop_color_enhance_saturation) {
		DRM_ERROR("create color enhance saturation prop fail\n");
		return -ENOMEM;
	}

	drv->prop_color_enhance_contrast = drm_property_create_range(drm, 0,
		"color_enhance_contrast",
		0, DC_COLOR_ENHANCE_MAX_VALUE);
	if (!drv->prop_color_enhance_contrast) {
		DRM_ERROR("create color enhance contrast prop fail\n");
		return -ENOMEM;
	}

	drv->prop_gamma_enable = drm_property_create_range(drm, 0,
		"gamma_enable",
		0, 1);
	if (!drv->prop_gamma_enable) {
		DRM_ERROR("create gamma enable prop fail\n");
		return -ENOMEM;
	}

	return 0;
}

static int lombo_crtc_properties_init(struct lombo_crtc *ecrtc,
	struct lombo_dc *dc)
{
	struct drm_crtc *crtc = &ecrtc->crtc;
	struct lombo_drv *drv = crtc->dev->dev_private;
	dc_enhance_info_t *ce = &dc->ce;
	int ret;

	ret = lombo_crtc_create_properties(crtc);
	if (ret) {
		DRM_ERROR("create the crtc properties fail, ret=%d\n", ret);
		return ret;
	}

	ecrtc->color_enhance_enable = ce->enhance_enable;
	drm_object_attach_property(&ecrtc->crtc.base,
		drv->prop_color_enhance_enable, ecrtc->color_enhance_enable);

	ecrtc->color_enhance_brightness = ce->brightness;
	drm_object_attach_property(&ecrtc->crtc.base,
		drv->prop_color_enhance_brightness,
		ecrtc->color_enhance_brightness);

	ecrtc->color_enhance_saturation = ce->saturation;
	drm_object_attach_property(&ecrtc->crtc.base,
		drv->prop_color_enhance_saturation,
		ecrtc->color_enhance_saturation);

	ecrtc->color_enhance_contrast = ce->contrast;
	drm_object_attach_property(&ecrtc->crtc.base,
		drv->prop_color_enhance_contrast,
		ecrtc->color_enhance_contrast);

	ecrtc->gamma_enable = 0;
	drm_object_attach_property(&ecrtc->crtc.base,
		drv->prop_gamma_enable,
		ecrtc->gamma_enable);

	return ret;
}

struct lombo_crtc *lombo_crtc_init(struct drm_device *drm,
	struct lombo_dc *dc, struct lombo_tcon *tcon)
{
	struct lombo_drv *drv = drm->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct lombo_crtc *ecrtc;
	struct drm_plane **planes;
	int ret;
	uint32_t pipe = drm->mode_config.num_crtc; /* fixme */

	ecrtc = kzalloc(sizeof(*ecrtc), GFP_KERNEL);
	if (!ecrtc)
		return ERR_PTR(-ENOMEM);
	ecrtc->tcon = tcon;
	ecrtc->dc = dc;
	lombo_dc_set_tcon(dc, tcon);

	planes = lombo_dc_planes_init(dc, pipe);
	if (IS_ERR(planes))
		DRM_ERROR("create the planes fail, ret=%d\n",
			  (int)PTR_ERR(planes));

	ret = drm_crtc_init(drm, &ecrtc->crtc, &lombo_crtc_funcs);
	if (ret) {
		dev_err(drm->dev, "Couldn't init DRM CRTC\n");
		return ERR_PTR(ret);
	}

	lombo_crtc_properties_init(ecrtc, dc);

	/* parse crtc_id of boot_disp */
	if (bd && (bd->tcon_id == lombo_tcon_id(tcon))) {
		lombo_crtc_helper_funcs.commit = lombo_crtc_commit_sw;
		bd->crtc = &ecrtc->crtc;
		DRM_DEBUG("bd tcon_id=%d, crtc_id=%d\n",
			bd->tcon_id, drm_crtc_index(bd->crtc));
	}

	drm_crtc_helper_add(&ecrtc->crtc, &lombo_crtc_helper_funcs);

	return ecrtc;
}

static int lombo_crtc_page_flip_atomic(struct lombo_crtc *ecrtc,
	struct drm_lombo_page_flip_atomic *page_flip)
{
	int ret = -EINVAL;
	uint32_t atomic_flag = page_flip->flag
		& DRM_LOMBO_PAGE_FLIP_FLAG_ATOMIC_MASK;

	switch (atomic_flag) {
	case DRM_LOMBO_PAGE_FLIP_FLAG_ATOMIC_CACHE:
		ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, true);
		ecrtc->pfa_cache = 1;
		break;
	case DRM_LOMBO_PAGE_FLIP_FLAG_ATOMIC_FLUSH:
		ecrtc->pfa_cache = 0;
		lombo_dc_update_wins(ecrtc->dc);
		ret = lombo_tcon_protect_dbreg(ecrtc->tcon, ecrtc->dc, false);
		break;
	default:
		DRM_ERROR("unknown atomic flag 0x%x\n", atomic_flag);
	}

	return ret;
}

int lombo_page_flip_atomic_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv)
{
	struct drm_lombo_page_flip_atomic *page_flip = data;
	struct drm_mode_object *obj;
	struct drm_crtc *crtc;
	struct lombo_crtc *ecrtc;
	uint32_t atomic_flag = page_flip->flag
		& DRM_LOMBO_PAGE_FLIP_FLAG_ATOMIC_MASK;
	int ret;

	DRM_DEBUG_KMS("flag=0x%x\n", page_flip->flag);

	if (!drm_core_check_feature(dev, DRIVER_MODESET))
		return -EINVAL;

	if ((atomic_flag > DRM_LOMBO_PAGE_FLIP_FLAG_ATOMIC_FLUSH)
		|| (atomic_flag == 0)) {
		DRM_ERROR("bad flag(0x%x) for pfa\n", atomic_flag);
		return -EINVAL;
	}

	obj = drm_mode_object_find(dev,
		page_flip->crtc_id, DRM_MODE_OBJECT_CRTC);
	if (!obj)
		return -EINVAL;
	crtc = obj_to_crtc(obj);
	ecrtc = drm_crtc_to_lombo_crtc(crtc);

	mutex_lock(&crtc->mutex);
	ret = lombo_crtc_page_flip_atomic(ecrtc, page_flip);
	mutex_unlock(&crtc->mutex);

	return ret;
}

int lombo_crtc_kms_show(struct seq_file *m, void *data)
{
	struct drm_info_node *node = m->private;
	struct drm_device *drm = node->minor->dev;
	unsigned int index;

	for (index = 0;; index++) {
		struct drm_crtc *crtc = drm_crtc_from_index(drm, index);
		struct lombo_crtc *ecrtc;
		struct lombo_dc *dc;
		struct lombo_tcon *tcon;

		if (!crtc)
			break;
		ecrtc = drm_crtc_to_lombo_crtc(crtc);
		dc = ecrtc->dc;
		tcon = ecrtc->tcon;

		seq_printf(m, "PIPE(%d): CRTC(%d), DC(%d), TCON(%d)\n",
			drm_crtc_index(crtc), crtc->base.id,
			lombo_dc_id(dc), lombo_tcon_id(tcon));
		lombo_tcon_kms_show(tcon, m, data);
		lombo_dc_kms_show(dc, m, data);
	}
	return 0;
}

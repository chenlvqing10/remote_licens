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
#include <drm/drm_crtc_helper.h>
#include <drm/drm_panel.h>

#include "lombo_drv.h"
#include "lombo_doss.h"
#include "lombo_tcon.h"
#include "lombo_vo_manage.h"

struct lombo_rgb {
	struct drm_connector connector;
	struct drm_encoder encoder;
	struct lombo_tcon *tcon;
	struct lombo_vo_host host;

	struct drm_panel *panel;

	bool prepared;
};

static inline struct lombo_rgb *to_lombo_rgb_by_enc(
		struct drm_encoder *enc)
{
	return container_of(enc, struct lombo_rgb, encoder);
}

static inline struct lombo_rgb *to_lombo_rgb_by_con(
		struct drm_connector *con)
{
	return container_of(con, struct lombo_rgb, connector);
}

static void lombo_rgb_encoder_dpms(struct drm_encoder *encoder, int mode)
{
	DRM_DEBUG_KMS("\n");
}

static bool lombo_rgb_encoder_mode_fixup(struct drm_encoder *enc,
		   const struct drm_display_mode *mode,
		   struct drm_display_mode *adjusted_mode)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_enc(enc);
	struct drm_panel *p = rgb->panel;

	DRM_DEBUG_KMS("\n");

	if (p && p->funcs && p->funcs->get_mode_private)
		p->funcs->get_mode_private(p,
			mode, (void **)&(adjusted_mode->private));

	return true;
}

static void lombo_rgb_encoder_prepare(struct drm_encoder *encoder)
{
	DRM_DEBUG_KMS("\n");
}

static void lombo_rgb_encoder_mode_set(struct drm_encoder *enc,
		 struct drm_display_mode *mode,
		 struct drm_display_mode *adjusted_mode)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_enc(enc);
	struct lombo_drv *drv = enc->dev->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct drm_panel *p = rgb->panel;

	DRM_DEBUG_KMS("\n");
	if (bd && bd->crtc && (bd->crtc == enc->crtc)) {
		if (p && p->funcs && p->funcs->boot_sync)
			p->funcs->boot_sync(p, LOMBO_PANEL_BOOT_SYNC_PREPARE);
	} else {
		drm_panel_prepare(p);
	}
}

static void lombo_rgb_encoder_commit(struct drm_encoder *encoder)
{
	DRM_DEBUG_KMS("\n");
}

/* disable encoder when not in use - more explicit than dpms off */
static void lombo_rgb_encoder_disable(struct drm_encoder *enc)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_enc(enc);
	struct drm_panel *p = rgb->panel;

	DRM_DEBUG_KMS("dpms=%d\n", rgb->connector.dpms);

	if (rgb->connector.dpms >= DRM_MODE_DPMS_OFF) {
		rgb->connector.dpms = DRM_MODE_DPMS_OFF;
		return;
	}

	drm_panel_disable(p);
	drm_panel_unprepare(p);
	rgb->connector.dpms = DRM_MODE_DPMS_OFF;
}

static const struct drm_encoder_helper_funcs lombo_rgb_enc_helper_funcs = {
	.dpms = lombo_rgb_encoder_dpms,
	.mode_fixup = lombo_rgb_encoder_mode_fixup,
	.prepare = lombo_rgb_encoder_prepare,
	.mode_set = lombo_rgb_encoder_mode_set,
	.commit = lombo_rgb_encoder_commit,
	.disable = lombo_rgb_encoder_disable,
};

static struct drm_encoder_funcs lombo_rgb_enc_funcs = {
	.destroy = drm_encoder_cleanup,
};

static int lombo_rgb_connector_get_modes(struct drm_connector *con)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_con(con);
	struct drm_panel *p = rgb->panel;

	DRM_DEBUG_KMS("\n");
	return drm_panel_get_modes(p);
}

static int lombo_rgb_connector_mode_valid(struct drm_connector *connector,
		  struct drm_display_mode *mode)
{
	DRM_DEBUG_KMS("\n");
	return MODE_OK;
}

static struct drm_encoder *lombo_rgb_best_encoder(
	struct drm_connector *con)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_con(con);

	DRM_DEBUG_KMS("\n");
	return &rgb->encoder;
}

static struct drm_connector_helper_funcs lombo_rgb_con_helper_funcs = {
	.get_modes = lombo_rgb_connector_get_modes,
	.mode_valid = lombo_rgb_connector_mode_valid,
	.best_encoder = lombo_rgb_best_encoder,
};

static void lombo_rgb_connector_dpms(
	struct drm_connector *con, int mode)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_con(con);
	struct drm_panel *p = rgb->panel;
	int old_dpms = con->dpms;

	con->dpms = mode;
	DRM_DEBUG_KMS("mode=%d,old_dpms=%d\n", mode, old_dpms);

	if (mode < old_dpms) {
		/* from off to on */
		struct lombo_drv *drv = con->dev->dev_private;
		struct lombo_boot_disp *bd = drv->boot_disp;

		if (bd && bd->crtc && (bd->crtc == rgb->encoder.crtc)) {
			if (p && p->funcs && p->funcs->boot_sync)
				p->funcs->boot_sync(p,
					LOMBO_PANEL_BOOT_SYNC_ENABLE);
		} else {
			drm_panel_enable(p);
		}
	} else if (mode > old_dpms) {
		/* from on to off */
		drm_panel_disable(p);
	}
}

static enum drm_connector_status lombo_rgb_connector_detect(
	struct drm_connector *con, bool force)
{
	struct lombo_rgb *rgb = to_lombo_rgb_by_con(con);

	DRM_DEBUG_KMS("\n");

	if (rgb->panel)
		return connector_status_connected;
	else
		return connector_status_disconnected;
}

static struct drm_connector_funcs lombo_rgb_con_funcs = {
	.dpms = lombo_rgb_connector_dpms,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.detect = lombo_rgb_connector_detect,
	.destroy = drm_connector_cleanup,
};

static ssize_t lombo_rgb_vo_host_transfer(
	struct lombo_vo_host *host, const struct lombo_vo_msg *msg)
{
	u32 *p = msg->tx_buf;

	switch (msg->type) {
	case LOMBO_CPU_IF_MSG_WR_REG: {
		csp_tcon_cpu_write_reg(host->host_id, *p,
				p + 1, (msg->tx_len - 1));
		break;
		}
	case LOMBO_CPU_IF_MSG_WR_IDX: {
		csp_tcon_cpu_write_reg_index(host->host_id, *p);
		break;
		}
	default:
		DRM_ERROR("msg->type=0x%x\n", msg->type);
		break;
	}

	return 0;
}

struct lombo_vo_host_ops lombo_rgb_vo_host_ops = {
	.transfer = lombo_rgb_vo_host_transfer,
};

struct lombo_rgb *lombo_rgb_init(struct drm_device *drm,
	struct lombo_tcon *tcon,
	struct device_node *np, const char *phandle_name,
	uint32_t possible_crtcs)
{
	struct drm_panel *panel;
	struct device_node *remote;
	struct drm_encoder *encoder;
	struct drm_connector *connector;
	struct lombo_rgb *rgb;
	int encoder_type = DRM_MODE_ENCODER_NONE;
	int connector_type = 0;
	int ret;

	remote = of_parse_phandle(np, phandle_name, 0);
	if (!remote) {
		DRM_DEBUG("not parse phandle of %s\n", phandle_name);
		return NULL;
	} else {
		panel = of_drm_find_panel(remote);
		if (IS_ERR_OR_NULL(panel)) {
			DRM_INFO("no find %s\n", phandle_name);
			return NULL;
		}
	}

	rgb = kzalloc(sizeof(*rgb), GFP_KERNEL);
	if (!rgb)
		return NULL;
	rgb->tcon = tcon;
	rgb->panel = panel;
	rgb->host.ops = &lombo_rgb_vo_host_ops;
	rgb->host.host_id = lombo_tcon_id(tcon);
	DRM_INFO("host_id %d\n", rgb->host.host_id);

	if (!strcmp(phandle_name, "panel_rgb")
		|| !strcmp(phandle_name, "panel_cpu")) {
		/* do nothing */
	} else if (!strcmp(phandle_name, "panel_lvds")) {
		encoder_type = DRM_MODE_ENCODER_LVDS;
		connector_type = DRM_MODE_CONNECTOR_LVDS;
	} else {
		DRM_INFO("unknown %s\n", phandle_name);
	}

	encoder = &rgb->encoder;
	drm_encoder_helper_add(encoder, &lombo_rgb_enc_helper_funcs);
	ret = drm_encoder_init(drm, encoder,
			&lombo_rgb_enc_funcs,
			encoder_type);
	if (ret) {
		dev_err(drm->dev, "Couldn't initialise the encoder%d\n",
			encoder_type);
		goto err_out;
	}
	encoder->possible_crtcs = possible_crtcs;

	connector = &rgb->connector;
	connector->dpms = DRM_MODE_DPMS_OFF;
	drm_connector_helper_add(connector, &lombo_rgb_con_helper_funcs);
	ret = drm_connector_init(drm, connector,
				&lombo_rgb_con_funcs,
				connector_type);
	if (ret) {
		dev_err(drm->dev, "init connector%d fail\n", connector_type);
		goto err_cleanup_encoder;
	}
	drm_object_property_set_value(&connector->base,
		drm->mode_config.dpms_property, connector->dpms);
	drm_mode_connector_attach_encoder(connector, encoder);

	ret = drm_panel_attach(panel, connector);
	if (ret) {
		dev_err(drm->dev, "Couldn't attach our panel\n");
		goto err_cleanup_connector;
	}
	lombo_vo_dev_panel_attach(panel, &rgb->host);

	return rgb;

err_cleanup_connector:
	drm_connector_cleanup(&rgb->connector);
err_cleanup_encoder:
	drm_encoder_cleanup(&rgb->encoder);
err_out:
	kfree(rgb);
	return NULL;
}

int lombo_rgb_deinit(struct lombo_rgb *rgb)
{
	if (!rgb)
		return -EINVAL;

	if (rgb->panel) {
		drm_panel_detach(rgb->panel);
		lombo_vo_dev_panel_detach(rgb->panel);
	}
	kfree(rgb);

	return 0;
}

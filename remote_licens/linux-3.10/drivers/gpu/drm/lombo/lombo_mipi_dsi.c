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
#include <drm/drm_crtc_helper.h>
#include <drm/drm_panel.h>
#include <drm/drm_mipi_dsi.h>
#include <video/mipi_display.h>

#include "lombo_drv.h"
#include "lombo_doss.h"
#include "csp_dsi/csp_doss_mipi_dsi.h"
#include <mach/common.h>

struct lombo_mipi_dsi {
	struct drm_connector connector;
	struct drm_encoder encoder;
	struct mipi_dsi_host host;

	struct device *dev;
	struct device *doss_top;
	struct drm_device *drm;
	void __iomem *reg;
	uint32_t id;

	struct lombo_doss_clk *cfg_clk;
	struct lombo_doss_clk *ref_clk;

	struct drm_panel *panel;

	bool enable;
};

static inline struct lombo_mipi_dsi *to_lombo_mipi_dsi_by_enc(
	struct drm_encoder *enc)
{
	return container_of(enc, struct lombo_mipi_dsi, encoder);
}

static inline struct lombo_mipi_dsi *to_lombo_mipi_dsi_by_con(
	struct drm_connector *con)
{
	return container_of(con, struct lombo_mipi_dsi, connector);
}

static inline struct lombo_mipi_dsi *to_lombo_mipi_dsi_by_host(
	struct mipi_dsi_host *h)
{
	return container_of(h, struct lombo_mipi_dsi, host);
}

static void lombo_dsi_encoder_prepare(struct drm_encoder *enc)
{
	DRM_DEBUG_KMS("\n");
}

static void lombo_dsi_encoder_dpms(
	struct drm_encoder *enc, int mode)
{
	DRM_DEBUG_KMS("\n");
}

static bool lombo_dsi_encoder_mode_fixup(
	struct drm_encoder *enc,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_enc(enc);
	struct drm_panel *p = dsi->panel;
	struct lombo_vo_dev_config *vo;
	int ret = -EINVAL;
	u64 ref_clk_rate = 0;

	DRM_DEBUG_KMS("\n");

	if (p && p->funcs && p->funcs->get_mode_private)
		ret = p->funcs->get_mode_private(p,
			mode, (void **)&(adjusted_mode->private));
	if (ret) {
		DRM_ERROR("get panel mode private failed\n");
		return false;
	}
	vo = (struct lombo_vo_dev_config *)adjusted_mode->private;

	ref_clk_rate = csp_mipi_dsi_get_ref_clk_rate(
		dsi->id, vo->dsi_if.is_bitrate_fixed, vo->timing.dclk_freq);

	lombo_doss_clk_set_rate(dsi->ref_clk, (unsigned long)ref_clk_rate);

	return true;
}

static void lombo_dsi_encoder_mode_set(
	struct drm_encoder *enc,
	struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_enc(enc);
	struct lombo_drv *drv = dsi->drm->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct lombo_vo_dev_config *vo_dev_cfg =
		(struct lombo_vo_dev_config *)(adjusted_mode->private);
	tcon_mipi_dsi_if_t *dsi_if;
	struct drm_panel *p = dsi->panel;
	uint8_t sw = 0;

	DRM_DEBUG_KMS("vo_dev_cfg=%p, crtc=%p\n",
		vo_dev_cfg, enc->crtc);

	if (!vo_dev_cfg)
		return;

	if (bd && bd->crtc && (bd->crtc == enc->crtc))
		sw = 1;

	dsi_if = &vo_dev_cfg->dsi_if;
	dsi_if->dsi_ref_freq = lombo_doss_clk_get_rate(dsi->ref_clk);
	dsi_if->dsi_ref_div = lombo_doss_clk_get_div(dsi->ref_clk);

	if (!sw) {
		if (csp_mipi_dsi_init(dsi->id, dsi_if))
			DRM_ERROR("csp_mipi_dsi_init fail\n");
		csp_mipi_dsi_cfg(dsi->id, dsi_if);
		drm_panel_prepare(p);
		csp_mipi_dsi_start(dsi->id);
	} else {
		DRM_DEBUG_KMS("do for sw\n");
		if (p && p->funcs && p->funcs->boot_sync)
			p->funcs->boot_sync(p, LOMBO_PANEL_BOOT_SYNC_PREPARE);
	}
	dsi->enable = 1;

}

static void lombo_dsi_encoder_commit(struct drm_encoder *enc)
{
	DRM_DEBUG_KMS("\n");
}

static void lombo_dsi_encoder_disable(struct drm_encoder *enc)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_enc(enc);
	struct drm_panel *p = dsi->panel;

	DRM_DEBUG_KMS("enable=%d\n", dsi->enable);

	if (!dsi->enable)
		return;
	dsi->enable = false;

	drm_panel_disable(p);
	dsi->connector.dpms = DRM_MODE_DPMS_OFF;

	csp_mipi_dsi_stop(dsi->id);
	csp_mipi_dsi_reset(dsi->id);
	drm_panel_unprepare(p);

}

static struct drm_encoder_funcs lombo_dsi_enc_funcs = {
	.destroy = drm_encoder_cleanup,
};

static const struct drm_encoder_helper_funcs lombo_dsi_enc_helper_funcs = {
	.dpms = lombo_dsi_encoder_dpms,
	.mode_fixup = lombo_dsi_encoder_mode_fixup,
	.mode_set = lombo_dsi_encoder_mode_set,
	.prepare = lombo_dsi_encoder_prepare,
	.commit = lombo_dsi_encoder_commit,
	.disable = lombo_dsi_encoder_disable,
};

int lombo_dsi_encoder_get_clk(struct drm_encoder *enc,
	uint32_t num, struct lombo_doss_clk **p_clks)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_enc(enc);

	if (!enc || (num < 2) || !p_clks) {
		DRM_ERROR("null pointer or num=%d\n", num);
		return 0;
	}

	*p_clks++ = dsi->ref_clk;
	*p_clks++ = dsi->cfg_clk;
	return 2;
}
EXPORT_SYMBOL(lombo_dsi_encoder_get_clk);

static enum drm_connector_status lombo_dsi_connector_detect(
	struct drm_connector *con, bool force)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_con(con);

	DRM_DEBUG_KMS("\n");

	if (dsi->panel)
		return connector_status_connected;
	else
		return connector_status_disconnected;
}

static int lombo_dsi_connector_get_modes(
	struct drm_connector *con)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_con(con);
	struct drm_panel *panel = dsi->panel;

	DRM_DEBUG_KMS("\n");
	return drm_panel_get_modes(panel);
}

static int lombo_dsi_connector_mode_valid(
	struct drm_connector *connector,
	struct drm_display_mode *mode)
{
	DRM_DEBUG_KMS("\n");
	return MODE_OK;
}

static struct drm_encoder *lombo_dsi_best_encoder(
	struct drm_connector *con)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_con(con);

	DRM_DEBUG_KMS("encoder=%p\n", &dsi->encoder);
	return &dsi->encoder;
}

static struct drm_connector_helper_funcs lombo_dsi_con_helper_funcs = {
	.get_modes = lombo_dsi_connector_get_modes,
	.mode_valid = lombo_dsi_connector_mode_valid,
	.best_encoder = lombo_dsi_best_encoder,
};

static void lombo_dsi_connector_dpms(
	struct drm_connector *con, int mode)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_con(con);
	struct drm_panel *p = dsi->panel;
	int old_dpms = con->dpms;

	con->dpms = mode;
	DRM_DEBUG_KMS("mode=%d,old_dpms=%d\n", mode, old_dpms);

	if (mode < old_dpms) {
		/* from off to on */
		struct lombo_drv *drv = dsi->drm->dev_private;
		struct lombo_boot_disp *bd = drv->boot_disp;

		if (bd && bd->crtc && (bd->crtc == dsi->encoder.crtc)) {
			if (p && p->funcs && p->funcs->boot_sync)
				p->funcs->boot_sync(p,
					LOMBO_PANEL_BOOT_SYNC_ENABLE);
		} else {
			drm_panel_enable(p);
		}
	} else if (mode > old_dpms) /* from on to off */
		drm_panel_disable(p);
}

static struct drm_connector_funcs lombo_dsi_con_funcs = {
	.dpms = lombo_dsi_connector_dpms,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.detect = lombo_dsi_connector_detect,
	.destroy = drm_connector_cleanup,
};

static ssize_t lombo_mipi_dsi_host_transfer(
	struct mipi_dsi_host *host,
	const struct mipi_dsi_msg *msg)
{
	struct lombo_mipi_dsi *dsi = to_lombo_mipi_dsi_by_host(host);
	u8 *buf = (u8 *)msg->tx_buf;

	switch (msg->type) {
	case MIPI_DSI_DCS_SHORT_WRITE:
		csp_mipi_dsi_dcs_sw_0p(dsi->id, msg->channel, buf[0]);
		break;
	case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
		csp_mipi_dsi_dcs_sw_1p(dsi->id, msg->channel, buf[0], buf[1]);
		break;
	case MIPI_DSI_DCS_LONG_WRITE:
		csp_mipi_dsi_dcs_lw(dsi->id, msg->channel, buf, msg->tx_len);
		break;
	default:
		DRM_ERROR("msg->type=0x%x\n", msg->type);
		break;
	}

	return 0;
}

static struct mipi_dsi_host_ops lombo_mipi_dsi_host_ops = {
	.transfer = lombo_mipi_dsi_host_transfer,
};

static inline void lombo_set_csp_mipi_dsi_version(void)
{
	u32 version = CSP_MIPI_DSI_VERSION_V10;

	#if defined(CONFIG_ARCH_LOMBO_N7V1)

	u32 p1, p2 = 0;

	if (!lombo_func2(&p1, &p2)) {
		if (p1 == 1)
			version = CSP_MIPI_DSI_VERSION_NONE;
	} else {
		DRM_INFO("%s: lombo_func2 failed\n", __func__);
	}

	#elif defined(CONFIG_ARCH_LOMBO_N7V0)

	version = CSP_MIPI_DSI_VERSION_NONE;

	#endif

	csp_mipi_dsi_set_version(version);
}

static int lombo_mipi_dsi_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct device_node *np, *remote;
	struct platform_device *pdev, *doss_top_pdev;
	struct drm_device *drm = master_data;
	struct lombo_mipi_dsi *dsi;
	struct resource *res;
	int ret;
	u32 value = 0;

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	np = dev->of_node;
	pdev = to_platform_device(dev);

	remote = of_parse_phandle(dev->of_node, "top", 0);
	if (!remote) {
		DRM_ERROR("failed to parse phandle of top\n");
		return -EINVAL;
	}
	doss_top_pdev = of_find_device_by_node(remote);
	if (!doss_top_pdev) {
		DRM_ERROR("failed to find device doss top\n");
		return -EINVAL;
	}

	dsi = kzalloc(sizeof(*dsi), GFP_KERNEL);
	if (!dsi) {
		dev_err(dev, "alloc for tcon failed.\n");
		return -ENOMEM;
	}
	dsi->dev = dev;
	dsi->doss_top = &doss_top_pdev->dev;
	dsi->drm = drm;
	/* parse hardware index */
	if (!of_property_read_u32(np, "hw_id", &value))
		dsi->id = value;
	else
		DRM_INFO("get dsi hw_id failed\n");
	DRM_DEBUG_KMS("dsi id=%d\n", dsi->id);

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	dsi->reg = devm_ioremap(dev, res->start,
				resource_size(res));
	if (dsi->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	lombo_set_csp_mipi_dsi_version();
	csp_mipi_dsi_set_register_base(dsi->id, dsi->reg,
		(u32)resource_size(res));

	dsi->cfg_clk = lombo_doss_get_clk(dev,
				&doss_top_pdev->dev,
				dsi->id, DOSS_CLK_MIPI_DSI_CFG);
	dsi->ref_clk = lombo_doss_get_clk(dev,
				&doss_top_pdev->dev,
				dsi->id, DOSS_CLK_MIPI_DSI_REF);
	if (!dsi->cfg_clk || !dsi->ref_clk) {
		DRM_ERROR("doss_get_clk for dsi fail. cfg=%p,ref=%p\n",
			dsi->cfg_clk, dsi->ref_clk);
		return -EINVAL;
	}

	dsi->host.ops = &lombo_mipi_dsi_host_ops;

	remote = of_parse_phandle(dev->of_node, "panel_dsi", 0);
	if (remote) {
		dsi->panel = of_drm_find_panel(remote);
		if (IS_ERR_OR_NULL(dsi->panel)) {
			dsi->panel = NULL;
			DRM_DEBUG("fail find dsi panel\n");
		}
	} else
		DRM_DEBUG("failed to parse phandle of panel_mipi_dsi\n");

	if (dsi->panel) {
		drm_encoder_helper_add(&dsi->encoder,
				&lombo_dsi_enc_helper_funcs);
		ret = drm_encoder_init(drm, &dsi->encoder,
				&lombo_dsi_enc_funcs,
				DRM_MODE_ENCODER_DSI);
		if (ret) {
			DRM_ERROR("dsi encoder init fail\n");
			return ret;
		}
		dsi->encoder.possible_crtcs = BIT(0);

		dsi->connector.dpms = DRM_MODE_DPMS_OFF;
		drm_connector_helper_add(&dsi->connector,
					 &lombo_dsi_con_helper_funcs);
		ret = drm_connector_init(drm, &dsi->connector,
					 &lombo_dsi_con_funcs,
					 DRM_MODE_CONNECTOR_DSI);
		if (ret) {
			DRM_ERROR("dsi connector init fail\n");
			return ret;
		}
		drm_object_property_set_value(&dsi->connector.base,
			drm->mode_config.dpms_property, dsi->connector.dpms);
		drm_mode_connector_attach_encoder(
			&dsi->connector, &dsi->encoder);
		drm_mipi_dsi_panel_attach(dsi->panel, &dsi->host);
		drm_panel_attach(dsi->panel, &dsi->connector);
	}

	dev_set_drvdata(dev, dsi);

	return 0;
}

static void lombo_mipi_dsi_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_mipi_dsi *dsi = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	if (dsi->panel) {
		drm_panel_detach(dsi->panel);
		drm_mipi_dsi_panel_detach(dsi->panel);
	}

	lombo_doss_put_clk(&dsi->ref_clk);
	lombo_doss_put_clk(&dsi->cfg_clk);
	csp_mipi_dsi_set_register_base(dsi->id, NULL, 0);
	kfree(dsi);
}

static const struct component_ops lombo_mipi_dsi_ops = {
	.bind	= lombo_mipi_dsi_bind,
	.unbind	= lombo_mipi_dsi_unbind,
};

static int lombo_mipi_dsi_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_mipi_dsi_ops);
}

static int lombo_mipi_dsi_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_mipi_dsi_ops);
	return 0;
}

static const struct of_device_id lombo_mipi_dsi_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-mipi-dsi" },
	{ .compatible = "lombo,lombo-n7v1-mipi-dsi" },
	{ .compatible = "lombo,lombo-n8v0-mipi-dsi" },
	{ .compatible = "lombo,lombo-n9v0-mipi-dsi" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_mipi_dsi_of_table);

static struct platform_driver lombo_mipi_dsi_platform_driver = {
	.probe		= lombo_mipi_dsi_probe,
	.remove		= lombo_mipi_dsi_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-mipi-dsi",
		.of_match_table	= lombo_mipi_dsi_of_table,
	},
};
module_platform_driver(lombo_mipi_dsi_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DOSS MIPI DSI Driver");
MODULE_LICENSE("GPL");

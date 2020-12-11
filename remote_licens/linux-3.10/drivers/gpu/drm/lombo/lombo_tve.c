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

#include "lombo_drv.h"
#include "lombo_doss.h"
#include "lombo_tcon.h"

#define NTSC_WIDTH		(720)
#define NTSC_HEIGHT		(480)
#define NTSC_HBP		(57)
#define NTSC_HFP		(1)
#define NTSC_HSW		(80)
#define NTSC_HACT		(NTSC_WIDTH)
#define NTSC_VBP_F1		(13)
#define NTSC_VFP_F1		(1)
#define NTSC_VSW_F1		(8)
#define NTSC_VACT_F1		(NTSC_HEIGHT >> 1)
#define NTSC_VBP_F2		(14)
#define NTSC_VFP_F2		(1)
#define NTSC_VSW_F2		(NTSC_VSW_F1)
#define NTSC_VACT_F2		(NTSC_HEIGHT >> 1)
#define NTSC_VT	(NTSC_VBP_F1 + NTSC_VFP_F1 + NTSC_VSW_F1 + NTSC_VACT_F1)
#define NTSC_DCLK		(13500000) /* 13.5MHz */
#define NTSC_DEV_IF		(VO_DEV_TVE_NTSC)
#define NTSC_ENC_MODE		(TVE_CVBS)
#define	NTSC_COLOR_STD		(TVE_NTSC_M)
#define NTSC_IS_PAL		(false)
#define NTSC_VREFRESH		(60)

#define PAL_WIDTH		(720)
#define PAL_HEIGHT		(576)
#define PAL_HBP			(57)
#define PAL_HFP			(1)
#define PAL_HSW			(86)
#define PAL_HACT		(PAL_WIDTH)
#define PAL_VBP_F1		(17)
#define PAL_VFP_F1		(1)
#define PAL_VSW_F1		(6)
#define PAL_VACT_F1		(PAL_HEIGHT >> 1)
#define PAL_VBP_F2		(18)
#define PAL_VFP_F2		(1)
#define PAL_VSW_F2		(PAL_VSW_F1)
#define PAL_VACT_F2		(PAL_HEIGHT >> 1)
#define PAL_VT		(PAL_VBP_F1 + PAL_VFP_F1 + PAL_VSW_F1 + PAL_VACT_F1)
#define PAL_DCLK		(13500000) /* 13.5MHz */
#define PAL_DEV_IF		(VO_DEV_TVE_PAL)
#define PAL_ENC_MODE		(TVE_CVBS)
#define	PAL_COLOR_STD		(TVE_PAL_B)
#define PAL_IS_PAL		(true)
#define PAL_VREFRESH		(50)

#define MODES_NUM		(2)

static struct lombo_vo_dev_config tve_vo_config[MODES_NUM] = {
	{
		/* device interface, reference to vo_dev_if_t */
		.dev_if = VO_DEV_TVE_PAL,
		/* device specific configuation, reference to vo_if_cfg_t */
		.tve_if.index = 0,
		.tve_if.enc_mode = PAL_ENC_MODE,
		.tve_if.color_std = PAL_COLOR_STD,

		/* tcon host config */
		/* timing, reference to tcon_timings_t */
		.timing.is_interlace = true,
		.timing.is_pal = PAL_IS_PAL,
		.timing.field1.vt = PAL_VT,
		.timing.field1.vact = PAL_VACT_F1,
		.timing.field1.vfp = PAL_VFP_F1,
		.timing.field1.vsw = PAL_VSW_F1,
		.timing.field2.vt = PAL_VSW_F2 + PAL_VBP_F2 +
					PAL_VACT_F2 + PAL_VFP_F2,
		.timing.field2.vact = PAL_VACT_F2,
		.timing.field2.vfp = PAL_VFP_F2,
		.timing.field2.vsw = PAL_VSW_F2,
		.timing.hline.ht = PAL_HSW + PAL_HBP + PAL_HACT + PAL_HFP,
		.timing.hline.hact = PAL_HACT,
		.timing.hline.hfp = PAL_HFP,
		.timing.hline.hsw = PAL_HSW,
		.timing.dclk_freq = PAL_DCLK,
		/* resolution in pixel */
		.timing.width = PAL_WIDTH,
		.timing.height = PAL_HEIGHT,
		/* format, reference to tcon_output_fmt_t */
		.format = TCON_FMT_YUV422,
	}, {
		/* device interface, reference to vo_dev_if_t */
		.dev_if = VO_DEV_TVE_NTSC,
		/* device specific configuation, reference to vo_if_cfg_t */
		.tve_if.index = 0,
		.tve_if.enc_mode = NTSC_ENC_MODE,
		.tve_if.color_std = NTSC_COLOR_STD,

		/* tcon host config */
		/* timing, reference to tcon_timings_t */
		.timing.is_interlace = true,
		.timing.is_pal = NTSC_IS_PAL,
		.timing.field1.vt = NTSC_VT,
		.timing.field1.vact = NTSC_VACT_F1,
		.timing.field1.vfp = NTSC_VFP_F1,
		.timing.field1.vsw = NTSC_VSW_F1,
		.timing.field2.vt = NTSC_VSW_F2 + NTSC_VBP_F2 +
					NTSC_VACT_F2 + NTSC_VFP_F2,
		.timing.field2.vact = NTSC_VACT_F2,
		.timing.field2.vfp = NTSC_VFP_F2,
		.timing.field2.vsw = NTSC_VSW_F2,
		.timing.hline.ht = NTSC_HSW + NTSC_HBP + NTSC_HACT + NTSC_HFP,
		.timing.hline.hact = NTSC_HACT,
		.timing.hline.hfp = NTSC_HFP,
		.timing.hline.hsw = NTSC_HSW,
		.timing.dclk_freq = NTSC_DCLK,
		/* resolution in pixel */
		.timing.width = NTSC_WIDTH,
		.timing.height = NTSC_HEIGHT,
		/* format, reference to tcon_output_fmt_t */
		.format = TCON_FMT_YUV422,
	},
};

static struct drm_display_mode tve_mode[MODES_NUM] = {
	{
		/* dclk_freq */
		.clock = PAL_DCLK / 1000,
		/* width */
		.hdisplay = PAL_WIDTH,
		/* hsync_start = hdisplay + hfp */
		.hsync_start = PAL_WIDTH + PAL_HFP,
		/* hsync_end = hdisplay + hfp + hsw */
		.hsync_end = PAL_WIDTH + PAL_HFP + PAL_HSW,
		/* htotal = hdisplay + hfp + hsw + hbp */
		.htotal = PAL_WIDTH + PAL_HFP + PAL_HSW + PAL_HBP,
		/* height */
		.vdisplay = (PAL_HEIGHT >> 1),
		/* vsync_start = vdisplay + vfp */
		.vsync_start = PAL_HEIGHT + PAL_VFP_F1,
		/* vsync_end = vdisplay + vfp + vsw */
		.vsync_end = PAL_HEIGHT + PAL_VFP_F1 + PAL_VSW_F1,
		/* vtotal = vdisplay + vfp + vsw + vbp */
		.vtotal = PAL_HEIGHT + PAL_VFP_F1 + PAL_VSW_F1 + PAL_VBP_F1,
		.vrefresh = PAL_VREFRESH,

		.private_size = sizeof(tve_vo_config[0]),
		.private = (int *)&tve_vo_config[0],
	}, {
		/* dclk_freq */
		.clock = NTSC_DCLK / 1000,
		/* width */
		.hdisplay = NTSC_WIDTH,
		/* hsync_start = hdisplay + hfp */
		.hsync_start = NTSC_WIDTH + NTSC_HFP,
		/* hsync_end = hdisplay + hfp + hsw */
		.hsync_end = NTSC_WIDTH + NTSC_HFP + NTSC_HSW,
		/* htotal = hdisplay + hfp + hsw + hbp */
		.htotal = NTSC_WIDTH + NTSC_HFP + NTSC_HSW + NTSC_HBP,
		/* height */
		.vdisplay = (NTSC_HEIGHT >> 1),
		/* vsync_start = vdisplay + vfp */
		.vsync_start = NTSC_HEIGHT + NTSC_VFP_F1,
		/* vsync_end = vdisplay + vfp + vsw */
		.vsync_end = NTSC_HEIGHT + NTSC_VFP_F1 + NTSC_VSW_F1,
		/* vtotal = vdisplay + vfp + vsw + vbp */
		.vtotal = NTSC_HEIGHT + NTSC_VFP_F1 + NTSC_VSW_F1 + NTSC_VBP_F1,
		.vrefresh = NTSC_VREFRESH,

		.private_size = sizeof(tve_vo_config[1]),
		.private = (int *)&tve_vo_config[1],
	},
};

struct lombo_tve {
	struct drm_connector connector;
	struct drm_encoder encoder;
	struct device *dev;
	struct device *doss_top;
	struct drm_device *drm;
	void __iomem *reg;
	u32 id;
	struct lombo_doss_clk *tve_clk;
	struct drm_display_mode *tve_modes;
	bool prepared;
};

static inline struct lombo_tve *to_lombo_tve_by_enc(
		struct drm_encoder *enc)
{
	return container_of(enc, struct lombo_tve, encoder);
}

static inline struct lombo_tve *to_lombo_tve_by_con(
		struct drm_connector *con)
{
	return container_of(con, struct lombo_tve, connector);
}

static void lombo_tve_encoder_dpms(struct drm_encoder *encoder, int mode)
{
	DRM_DEBUG_KMS("\n");
}

static bool lombo_tve_encoder_mode_fixup(struct drm_encoder *enc,
		   const struct drm_display_mode *mode,
		   struct drm_display_mode *adjusted_mode)
{
	struct lombo_tve *tve = to_lombo_tve_by_enc(enc);
	struct lombo_vo_dev_config *vo = NULL;
	u32 i;
	u64 tve_clk_rate;

	for (i = 0; i < MODES_NUM; i++) {
		const struct drm_display_mode *m = tve->tve_modes + i;
		if (drm_mode_equal_no_clocks(mode, m)) {
			adjusted_mode->private = m->private;
			vo = (struct lombo_vo_dev_config *)m->private;
			break;
		} else
			DRM_DEBUG_KMS("mode->vdisp=%d, tve_mode->disp=%d\n",
				mode->vdisplay, m->vdisplay);
	}
	DRM_DEBUG_KMS("vo_dev_config=%p\n", vo);

	tve_clk_rate = vo->timing.dclk_freq;
	tve_clk_rate *= TVE_TCON_CLK_RATIO;
	lombo_doss_clk_set_rate(tve->tve_clk, (unsigned long)tve_clk_rate);

	return true;
}

static void lombo_tve_encoder_prepare(struct drm_encoder *encoder)
{
	DRM_DEBUG_KMS("\n");
}

static void lombo_tve_encoder_mode_set(struct drm_encoder *enc,
		 struct drm_display_mode *mode,
		 struct drm_display_mode *adjusted_mode)
{
	tcon_tve_if_t *tve_if = NULL;
	struct lombo_tve *tve = to_lombo_tve_by_enc(enc);
	struct lombo_vo_dev_config *vo_dev_cfg =
		(struct lombo_vo_dev_config *)(adjusted_mode->private);

	DRM_DEBUG_KMS("vo_dev_cfg=%p, crtc=%p\n",
		vo_dev_cfg, enc->crtc);

	if (!vo_dev_cfg)
		return;

	tve_if = &vo_dev_cfg->tve_if;
	csp_tve_init(tve->id);
	csp_tve_set_mode(tve->id, tve_if);

}

static void lombo_tve_encoder_commit(struct drm_encoder *encoder)
{
	DRM_DEBUG_KMS("\n");
}

/* disable encoder when not in use - more explicit than dpms off */
static void lombo_tve_encoder_disable(struct drm_encoder *enc)
{
	struct lombo_tve *tve = to_lombo_tve_by_enc(enc);

	DRM_DEBUG_KMS("dpms=%d\n", tve->connector.dpms);

	if (tve->connector.dpms >= DRM_MODE_DPMS_OFF) {
		tve->connector.dpms = DRM_MODE_DPMS_OFF;
		return;
	}

	tve->connector.dpms = DRM_MODE_DPMS_OFF;
}

static const struct drm_encoder_helper_funcs lombo_tve_enc_helper_funcs = {
	.dpms = lombo_tve_encoder_dpms,
	.mode_fixup = lombo_tve_encoder_mode_fixup,
	.prepare = lombo_tve_encoder_prepare,
	.mode_set = lombo_tve_encoder_mode_set,
	.commit = lombo_tve_encoder_commit,
	.disable = lombo_tve_encoder_disable,
};

static struct drm_encoder_funcs lombo_tve_enc_funcs = {
	.destroy = drm_encoder_cleanup,
};

int lombo_tve_encoder_get_clk(struct drm_encoder *enc,
	uint32_t num, struct lombo_doss_clk **p_clks)
{
	struct lombo_tve *tve = to_lombo_tve_by_enc(enc);

	if (!enc || (num < 1) || !p_clks) {
		DRM_ERROR("null pointer or num=%d\n", num);
		return 0;
	}

	*p_clks++ = tve->tve_clk;
	return 1;
}
EXPORT_SYMBOL(lombo_tve_encoder_get_clk);

static int lombo_tve_connector_get_modes(struct drm_connector *con)
{
	struct lombo_tve *tve = to_lombo_tve_by_con(con);
	unsigned int i, num = 0;
	struct drm_device *drm = tve->drm;
	struct drm_display_mode *mode;

	DRM_DEBUG_KMS("\n");
	for (i = 0; i < MODES_NUM; i++) {
		const struct drm_display_mode *m = tve->tve_modes + i;

		mode = drm_mode_duplicate(drm, m);
		if (!mode) {
			DRM_ERROR("failed to add mode %ux%u@%u\n",
				m->hdisplay, m->vdisplay, m->vrefresh);
			continue;
		}

		mode->vdisplay <<= 1;
		drm_mode_set_name(mode);
		mode->vdisplay >>= 1;
		DRM_DEBUG_KMS("%d\n", __LINE__);
		drm_mode_probed_add(con, mode);
		num++;
	}
	con->display_info.width_mm = 300;
	con->display_info.height_mm = 200;

	return num;
}

static int lombo_tve_connector_mode_valid(struct drm_connector *connector,
		  struct drm_display_mode *mode)
{
	DRM_DEBUG_KMS("\n");
	return MODE_OK;
}

static struct drm_encoder *lombo_tve_best_encoder(
	struct drm_connector *con)
{
	struct lombo_tve *tve = to_lombo_tve_by_con(con);

	DRM_DEBUG_KMS("\n");
	return &tve->encoder;
}

static struct drm_connector_helper_funcs lombo_tve_con_helper_funcs = {
	.get_modes = lombo_tve_connector_get_modes,
	.mode_valid = lombo_tve_connector_mode_valid,
	.best_encoder = lombo_tve_best_encoder,
};

static void lombo_tve_connector_dpms(
	struct drm_connector *con, int mode)
{
	int old_dpms = con->dpms;

	con->dpms = mode;
	DRM_DEBUG_KMS("mode=%d,old_dpms=%d\n", mode, old_dpms);

}

static enum drm_connector_status lombo_tve_connector_detect(
	struct drm_connector *con, bool force)
{
	DRM_DEBUG_KMS("\n");

	/* when without hotplug, always return connector_status_connected */
	return connector_status_connected;
}

static struct drm_connector_funcs lombo_tve_con_funcs = {
	.dpms = lombo_tve_connector_dpms,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.detect = lombo_tve_connector_detect,
	.destroy = drm_connector_cleanup,
};

static int lombo_tve_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct device_node *np, *remote;
	struct platform_device *pdev, *doss_top_pdev;
	struct drm_device *drm = master_data;
	struct lombo_tve *tve;
	int ret;
	struct resource *res;
	u32 value;

	np = dev->of_node;
	pdev = to_platform_device(dev);
	tve = kzalloc(sizeof(*tve), GFP_KERNEL);
	if (!tve)
		return -ENOMEM;
	DRM_DEBUG_KMS("\n");

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

	tve->dev = dev;
	tve->doss_top = &doss_top_pdev->dev;
	tve->drm = drm;
	tve->tve_modes = &tve_mode[0];

	/* parse hardware index */
	if (!of_property_read_u32(np, "tve_id", &value))
		tve->id = value;
	else
		DRM_INFO("get tve_id failed\n");
	DRM_DEBUG_KMS("tcon id=%d\n", tve->id);

	tve->tve_clk = lombo_doss_get_clk(dev,
					&doss_top_pdev->dev,
					tve->id, DOSS_CLK_TVE);

	if (!tve->tve_clk) {
		DRM_ERROR("doss_get_clk for tve fail. tve_clk=%p\n",
			tve->tve_clk);
		return -EINVAL;
	}

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}

	tve->reg = devm_ioremap(dev, res->start, resource_size(res));
	if (tve->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_tve_set_register_base(tve->id, (unsigned long)tve->reg,
		(u32)resource_size(res));
	DRM_DEBUG_KMS("tve_reg_base: %p", tve->reg);

	/* init tve encoder / connector */
	drm_encoder_helper_add(&tve->encoder,
				&lombo_tve_enc_helper_funcs);
	ret = drm_encoder_init(drm, &tve->encoder,
			&lombo_tve_enc_funcs,
			DRM_MODE_ENCODER_TVDAC);
	if (ret) {
		dev_err(drm->dev, "Couldn't initialise the encoder%d\n",
			DRM_MODE_ENCODER_TVDAC);
		goto err_out;
	}
	tve->encoder.possible_crtcs = BIT(0);
	tve->connector.dpms = DRM_MODE_DPMS_OFF;

	drm_connector_helper_add(&tve->connector,
				&lombo_tve_con_helper_funcs);
	ret = drm_connector_init(drm, &tve->connector,
				&lombo_tve_con_funcs,
				DRM_MODE_CONNECTOR_TV);
	if (ret) {
		dev_err(drm->dev, "init connector%d fail\n",
			DRM_MODE_CONNECTOR_TV);
		goto err_cleanup_encoder;
	}
	drm_object_property_set_value(&tve->connector.base,
		drm->mode_config.dpms_property, tve->connector.dpms);
	ret = drm_mode_connector_attach_encoder(&tve->connector,
						&tve->encoder);

	if (ret) {
		dev_err(drm->dev, "connector can't attach encoder\n");
		goto err_cleanup_connector;
	}
	dev_set_drvdata(dev, tve);

	return 0;

err_cleanup_connector:
	drm_connector_cleanup(&tve->connector);
err_cleanup_encoder:
	drm_encoder_cleanup(&tve->encoder);
err_out:
	kfree(tve);
	return -EINVAL;
}

static void lombo_tve_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_tve *tve = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	csp_tve_set_register_base(tve->id, (unsigned long)NULL, 0);
	kfree(tve);
}

static const struct component_ops lombo_tve_ops = {
	.bind	= lombo_tve_bind,
	.unbind = lombo_tve_unbind,
};

static int lombo_tve_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_tve_ops);
}

static int lombo_tve_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_tve_ops);
	return 0;
}

static const struct of_device_id lombo_tve_of_table[] = {
	{ .compatible = "lombo,lombo-n7v1-tve" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_tve_of_table);

static struct platform_driver lombo_tve_platform_driver = {
	.probe		= lombo_tve_probe,
	.remove		= lombo_tve_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-tve",
		.of_match_table	= lombo_tve_of_table,
	},
};
module_platform_driver(lombo_tve_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DOSS TVE Driver");
MODULE_LICENSE("GPL");

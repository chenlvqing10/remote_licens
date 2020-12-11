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

#include <linux/backlight.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include "lombo_panel_config.h"

#define WRITEREG32(reg, val)	writel(val, (void __iomem *)reg)
#define READREG32(reg)		readl((void __iomem *)reg)

#define HX8394F_GPIO_ACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)
#define HX8394F_GPIO_INACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)

/* timing for tcon host */
#define HX8394F_HBP	(24)
#define HX8394F_HFP	(12)
#define HX8394F_HSW	(20)
#define HX8394F_HACT (720)
#define HX8394F_VBP	(10)
#define HX8394F_VFP	(10)
#define HX8394F_VSW	(10)
#define HX8394F_VACT (1280)
#define HX8394F_VT	(HX8394F_VBP + HX8394F_VFP + HX8394F_VSW + HX8394F_VACT)
#define HX8394F_DCLK	(74250000)

#define DCS_MDELAY_FLAG (0)

struct panel_hx8394f_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;
	const struct display_timing *timings;
	unsigned int num_timings;

	unsigned int bpc;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	/**
	 * @prepare: the time (in milliseconds) that it takes for the panel to
	 *           become ready and start receiving video data
	 * @enable: the time (in milliseconds) that it takes for the panel to
	 *          display the first valid frame after starting to receive
	 *          video data
	 * @disable: the time (in milliseconds) that it takes for the panel to
	 *           turn the display off (no content is visible)
	 * @unprepare: the time (in milliseconds) that it takes for the panel
	 *             to power itself down completely
	 */
	struct {
		unsigned int prepare;
		unsigned int enable;
		unsigned int disable;
		unsigned int unprepare;
	} delay;

	u32 bus_format;
};

struct panel_hx8394f {
	struct drm_panel base;
	bool prepared;
	bool enabled;

	struct device *dev;
	const struct panel_hx8394f_desc *desc;

	struct backlight_device *backlight;
	struct regulator *supply;

	int enable_gpio;
	enum of_gpio_flags enable_gpio_flag;
	int bl_gpio;
	enum of_gpio_flags bl_gpio_flag;
};

static inline struct panel_hx8394f *to_panel_hx8394f(
	struct drm_panel *panel)
{
	return container_of(panel, struct panel_hx8394f, base);
}

static int panel_hx8394f_disable(struct drm_panel *panel)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (!p->enabled)
		return 0;

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_POWERDOWN;
		backlight_update_status(p->backlight);
	}

	if (p->desc && p->desc->delay.disable)
		msleep(p->desc->delay.disable);

	if (p->bl_gpio)
		gpio_direction_output(p->bl_gpio,
			HX8394F_GPIO_INACTIVE(p->bl_gpio_flag));

	p->enabled = false;

	return 0;
}

static int panel_hx8394f_unprepare(struct drm_panel *panel)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (!p->prepared)
		return 0;

	if (p->enable_gpio)
		gpio_direction_output(p->enable_gpio,
			HX8394F_GPIO_INACTIVE(p->enable_gpio_flag));

	if (p->supply)
		regulator_disable(p->supply);

	if (p->desc && p->desc->delay.unprepare)
		msleep(p->desc->delay.unprepare);

	p->prepared = false;

	return 0;
}

static int hx8394f_mipi_dsi_dcs_write_array(
	struct drm_panel *panel, const u8 *ini_code, size_t len)
{
	u32 i = 0;

	while (i < len) {
		if (ini_code[i] == DCS_MDELAY_FLAG) {
			mdelay(ini_code[i + 1]);
			i += 2;
		} else {
			mipi_dsi_dcs_write_buffer(panel->dsi,
				(const void *)&ini_code[i + 1], ini_code[i]);
			i += ini_code[i] + 1;
		}
	}
	return 0;
}

static u8 hx8394f_init_code[] = {
	/* EXTC */
	4, 0xB9, 0xFF, 0x83, 0x94,
	/* Power */
	11, 0xB1, 0x50, 0x12, 0x72, 0x09, 0x32, 0x54, 0x71, 0x51, 0x30, 0x43,
	/* MIPI */
	7, 0xBA, 0x63, 0x03, 0x68, 0x6B, 0xB2, 0xC0,
	/* Display */
	7, 0xB2, 0x00, 0x80, 0x64, 0x0E, 0x0A, 0x2F,
	/* CYC */
	22, 0xB4, 0x1C, 0x78, 0x1C, 0x78, 0x1C, 0x78, 0x01, 0x0C, 0x86, 0x75,
	    0x00, 0x3F, 0x1C, 0x78,
	0x1C, 0x78, 0x1C, 0x78, 0x01, 0x0C, 0x86,
	/* D3 */
	34, 0xD3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x32, 0x10,
	    0x03, 0x00, 0x03, 0x32,
	0x13, 0xC0, 0x00, 0x00, 0x32, 0x10, 0x08, 0x00, 0x00, 0x37, 0x04, 0x05,
	    0x05, 0x37, 0x05,
	0x05, 0x47, 0x0E, 0x40,
	/* GIP */
	45, 0xD5, 0x18, 0x18, 0x18, 0x18, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
	    0x06, 0x07, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	    0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19, 0x19, 0x20,
	    0x21, 0x22, 0x23,
	/* D6 */
	45, 0xD6, 0x18, 0x18, 0x19, 0x19, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02,
	    0x01, 0x00, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	    0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x18, 0x18, 0x23,
	    0x22, 0x21, 0x20,
	/* Gamma */
	59, 0xE0, 0x00, 0x05, 0x0B, 0x11, 0x12, 0x16, 0x19, 0x17, 0x30, 0x40,
	    0x50, 0x4F, 0x58, 0x69,
	0x6F, 0x71, 0x7F, 0x82, 0x7E, 0x8D, 0x9E, 0x4F, 0x4F, 0x53, 0x58, 0x5D,
	    0x67, 0x74, 0x7F,
	0x00, 0x05, 0x0B, 0x11, 0x12, 0x16, 0x19, 0x17, 0x30, 0x40, 0x50, 0x4F,
	    0x58, 0x69, 0x6F,
	0x71, 0x7F, 0x82, 0x7E, 0x8D, 0x9E, 0x4F, 0x4F, 0x53, 0x58, 0x5D, 0x67,
	    0x74, 0x7F,
	/* Panel */
	2, 0xCC, 0x03,
	/* C0 */
	3, 0xC0, 0x1F, 0x73,
	/* VCOM */
	4, 0xB6, 0x3C, 0x3C, 0x00,
	/* D4h */
	2, 0xD4, 0x02,
	/* BD */
	2, 0xBD, 0x01,
	/* GAS */
	2, 0xB1, 0x60,
	/* BD */
	2, 0xBD, 0x00,
	/* Power Mode */
	8, 0xBF, 0x40, 0x81, 0x50, 0x00, 0x1A, 0xFC, 0x01,
	2, 0x11, 0x00,		/* sleep out */
	DCS_MDELAY_FLAG, 100,	/* delay 100ms */
	2, 0x29, 0x00,		/* display on */
	DCS_MDELAY_FLAG, 100,	/* delay 100ms */
};

static int panel_hx8394f_prepare(struct drm_panel *panel)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);
	int err;

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (p->prepared)
		return 0;
	p->prepared = true;

	if (p->supply) {
		err = regulator_enable(p->supply);
		if (err < 0) {
			dev_err(panel->dev,
				"failed to enable supply: %d\n", err);
		}
	}

	if (p->enable_gpio) {
		gpio_direction_output(p->enable_gpio,
			HX8394F_GPIO_ACTIVE(p->enable_gpio_flag));
		mdelay(100);
		gpio_direction_output(p->enable_gpio,
			HX8394F_GPIO_INACTIVE(p->enable_gpio_flag));
		mdelay(100);
		gpio_direction_output(p->enable_gpio,
			HX8394F_GPIO_ACTIVE(p->enable_gpio_flag));
		mdelay(150);
	}

	if (p->desc && p->desc->delay.prepare)
		msleep(p->desc->delay.prepare);

	hx8394f_mipi_dsi_dcs_write_array(&p->base,
		hx8394f_init_code,
		ARRAY_SIZE(hx8394f_init_code));

	return 0;
}

static int panel_hx8394f_enable(struct drm_panel *panel)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (p->enabled)
		return 0;
	p->enabled = true;

	if (p->desc && p->desc->delay.enable)
		msleep(p->desc->delay.enable);

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(p->backlight);
	}

	if (p->bl_gpio)
		gpio_direction_output(p->bl_gpio,
			HX8394F_GPIO_ACTIVE(p->bl_gpio_flag));

	return 0;
}

static int panel_hx8394f_get_fixed_modes(
	struct panel_hx8394f *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!panel->desc)
		return 0;

	for (i = 0; i < panel->desc->num_modes; i++) {
		const struct drm_display_mode *m = &panel->desc->modes[i];

		mode = drm_mode_duplicate(drm, m);
		if (!mode) {
			DRM_ERROR("failed to add mode %ux%u@%u\n",
				m->hdisplay, m->vdisplay, m->vrefresh);
			continue;
		}

		drm_mode_set_name(mode);

		drm_mode_probed_add(connector, mode);
		num++;
	}

	connector->display_info.bpc = panel->desc->bpc;
	connector->display_info.width_mm = panel->desc->size.width;
	connector->display_info.height_mm = panel->desc->size.height;

	return num;
}

static int panel_hx8394f_get_modes(struct drm_panel *panel)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);
	int num = 0;

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	/* add hard-coded panel modes */
	num += panel_hx8394f_get_fixed_modes(p);

	return num;
}

static int panel_hx8394f_get_timings(struct drm_panel *panel,
				    unsigned int num_timings,
				    struct display_timing *timings)
{
	DRM_DEBUG_KMS("%s,%d\n", __func__, __LINE__);
	return 0;
}

static int panel_hx8394f_get_mode_private(
	struct drm_panel *panel,
	const struct drm_display_mode *mode,
	void **private)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);

	if (!(p && p->desc && p->desc->modes))
		return -EINVAL;

	if (!mode) {
		*private = p->desc->modes[0].private;
		return 0;
	}

	if (drm_mode_equal_no_clocks(mode, &p->desc->modes[0])) {
		DRM_DEBUG_KMS("private=%p\n",
			p->desc->modes[0].private);
		*private = p->desc->modes[0].private;
		return 0;
	}
	return -EINVAL;
}

static int panel_hx8394f_boot_sync(struct drm_panel *panel,
					int sync_step)
{
	struct panel_hx8394f *p = to_panel_hx8394f(panel);

	DRM_INFO("sync_step=%d\n", sync_step);
	switch (sync_step) {
	case LOMBO_PANEL_BOOT_SYNC_PREPARE:
		p->prepared = true;
		break;
	case LOMBO_PANEL_BOOT_SYNC_ENABLE:
		p->enabled = true;
		break;
	default:
		break;
	}

	return 0;
}

static const struct drm_panel_funcs panel_hx8394f_funcs = {
	.disable = panel_hx8394f_disable,
	.unprepare = panel_hx8394f_unprepare,
	.prepare = panel_hx8394f_prepare,
	.enable = panel_hx8394f_enable,
	.get_modes = panel_hx8394f_get_modes,
	.get_timings = panel_hx8394f_get_timings,
	.get_mode_private = panel_hx8394f_get_mode_private,
	.boot_sync = panel_hx8394f_boot_sync,
};

static int panel_hx8394f_probe(struct device *dev,
	const struct panel_hx8394f_desc *desc)
{
	struct device_node *np = dev->of_node;
	struct panel_hx8394f *panel;
	struct panel_hx8394f_desc *of_desc;
	struct mipi_dsi_device *dsi_device;
	enum of_gpio_flags flags;
	u32 val;
	int err;

	dev_dbg(dev, "%s,%d\n", __FILE__, __LINE__);

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;
	dsi_device = devm_kzalloc(dev, sizeof(*dsi_device), GFP_KERNEL);
	if (!dsi_device)
		return -ENOMEM;
	dsi_device->channel = 0;
	panel->base.dsi = dsi_device;

	panel->enable_gpio = of_get_named_gpio_flags(
		np, "enable_gpio", 0, &flags);
	panel->enable_gpio_flag = flags;
	DRM_DEBUG_KMS("enable gpio %d, flags 0x%x\n",
		panel->enable_gpio, flags);
	if (!gpio_is_valid(panel->enable_gpio)) {
		DRM_ERROR("gpio %d invalid\n", panel->enable_gpio);
		panel->enable_gpio = 0;
	} else {
		err = devm_gpio_request(dev,
			panel->enable_gpio, "enable_gpio");
		if (err < 0) {
			DRM_ERROR("gpio %d request failed\n",
				panel->enable_gpio);
			panel->enable_gpio = 0;
		}
	}

	panel->bl_gpio = of_get_named_gpio_flags(
		np, "bl_gpio", 0, &flags);
	panel->bl_gpio_flag = flags;
	DRM_DEBUG_KMS("bl gpio %d, flags 0x%x\n", panel->bl_gpio, flags);
	if (!gpio_is_valid(panel->bl_gpio)) {
		DRM_ERROR("bl gpio %d invalid\n", panel->bl_gpio);
		panel->bl_gpio = 0;
	} else {
		err = devm_gpio_request(dev,
			panel->bl_gpio, "bl_gpio");
		if (err < 0) {
			DRM_ERROR("bl gpio %d request failed\n",
				panel->bl_gpio);
			panel->bl_gpio = 0;
		}
	}

	if (!desc)
		of_desc = kzalloc(sizeof(*of_desc), GFP_KERNEL);
	else
		of_desc = kmemdup(desc, sizeof(*of_desc), GFP_KERNEL);

	if (!of_property_read_u32(dev->of_node, "bus-format", &val))
		of_desc->bus_format = val;
	if (!of_property_read_u32(dev->of_node, "delay,prepare", &val))
		of_desc->delay.prepare = val;
	if (!of_property_read_u32(dev->of_node, "delay,enable", &val))
		of_desc->delay.enable = val;
	if (!of_property_read_u32(dev->of_node, "delay,disable", &val))
		of_desc->delay.disable = val;
	if (!of_property_read_u32(dev->of_node, "delay,unprepare", &val))
		of_desc->delay.unprepare = val;

	panel->enabled = false;
	panel->prepared = false;
	panel->desc = of_desc;
	panel->dev = dev;

	drm_panel_init(&panel->base);
	panel->base.dev = dev;
	panel->base.funcs = &panel_hx8394f_funcs;

	err = drm_panel_add(&panel->base);
	if (err < 0)
		goto err_out;

	dev_set_drvdata(dev, panel);

	DRM_DEBUG_KMS("[%d] over.\n", __LINE__);

	return 0;

err_out:
	return err;
}

static int panel_hx8394f_remove(
	struct platform_device *pdev)
{
	struct panel_hx8394f *panel = dev_get_drvdata(&pdev->dev);

	drm_panel_detach(&panel->base);
	drm_panel_remove(&panel->base);

	panel_hx8394f_disable(&panel->base);

	if (panel->backlight)
		put_device(&panel->backlight->dev);

	kfree(panel->desc);

	return 0;
}

static void panel_hx8394f_shutdown(
	struct platform_device *pdev)
{
	struct panel_hx8394f *panel = dev_get_drvdata(&pdev->dev);

	panel_hx8394f_disable(&panel->base);
}

static struct lombo_vo_dev_config hx8394f_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.is_dual = false,
		.is_bta = false,
		.lane_num = 4,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_EVENT,
		.is_bitrate_fixed = false,
		.bit_clk = 999000000,
	},

	.timing.is_interlace = false,
	.timing.field1.vt = HX8394F_VT,
	.timing.field1.vact = HX8394F_VACT,
	.timing.field1.vfp = HX8394F_VFP,
	.timing.field1.vsw = HX8394F_VSW,
	.timing.hline.ht = HX8394F_HSW + HX8394F_HBP
			 + HX8394F_HACT + HX8394F_HFP,
	.timing.hline.hact = HX8394F_HACT,
	.timing.hline.hfp = HX8394F_HFP,
	.timing.hline.hsw = HX8394F_HSW,
	.timing.width = HX8394F_HACT,
	.timing.height = HX8394F_VACT,
	.timing.dclk_freq = HX8394F_DCLK,
	.format = TCON_FMT_RGB888,
};

/* mipi dsi 1080x1920 LCD test */
static const struct drm_display_mode hx8394f_mode = {
	/* dclk_freq */
	.clock = HX8394F_DCLK / 1000,
	/* width */
	.hdisplay = HX8394F_HACT,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = HX8394F_HACT + HX8394F_HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = HX8394F_HACT + HX8394F_HFP + HX8394F_HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = HX8394F_HACT + HX8394F_HFP + HX8394F_HSW + HX8394F_HBP,
	/* height */
	.vdisplay = HX8394F_VACT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = HX8394F_VACT + HX8394F_VFP,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = HX8394F_VACT + HX8394F_VFP + HX8394F_VSW,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = HX8394F_VACT + HX8394F_VFP + HX8394F_VSW + HX8394F_VBP,
	.vrefresh = 60,

	.private_size = sizeof(hx8394f_vo),
	.private = (int *)&hx8394f_vo,
};

static const struct panel_hx8394f_desc hx8394f_desc = {
	.modes = &hx8394f_mode,
	.num_modes = 1,
	.bpc = 6,
	.size = {
		.width = 60,
		.height = 120,
	},
};

static const struct of_device_id panel_hx8394f_of_match[] = {
	{
		.compatible = "hx8394f",
		.data = &hx8394f_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, panel_hx8394f_of_match);

static int panel_hx8394f_pre_probe(
	struct platform_device *pdev)
{
	const struct of_device_id *id;

	if (!of_device_is_available(pdev->dev.of_node))
		return -ENODEV;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	id = of_match_node(panel_hx8394f_of_match,
				pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return panel_hx8394f_probe(&pdev->dev, id->data);
}

static struct platform_driver panel_hx8394f_platform_driver = {
	.driver = {
		.name = "panel-hx8394f",
		.of_match_table = panel_hx8394f_of_match,
	},
	.probe = panel_hx8394f_pre_probe,
	.remove = panel_hx8394f_remove,
	.shutdown = panel_hx8394f_shutdown,
};

static int __init panel_hx8394f_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(
		&panel_hx8394f_platform_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(panel_hx8394f_init);

static void __exit panel_hx8394f_exit(void)
{
	platform_driver_unregister(&panel_hx8394f_platform_driver);
}
module_exit(panel_hx8394f_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("DRM Driver for Panels hx8394f");
MODULE_LICENSE("GPL and additional rights");

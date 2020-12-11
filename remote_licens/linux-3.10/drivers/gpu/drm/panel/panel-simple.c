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
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_panel.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include "panel-simple.h"
#include "lombo_panel_config.h"
#include "../lombo/lombo_vo_manage.h"

#define WRITEREG32(reg, val)	writel(val, (void __iomem *)reg)
#define READREG32(reg)		readl((void __iomem *)reg)
#define ACTIVE(flag)		(((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)
#define INACTIVE(flag)		(((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)

struct panel_simple {
	struct drm_panel panel;
	struct device *dev;

	const char *label;
	unsigned int width;
	unsigned int height;
	struct videomode video_mode;
	unsigned int bus_format;
	bool data_mirror;

	struct backlight_device *backlight;
	struct regulator *supply;

	int power_gpio;
	enum of_gpio_flags power_gpio_flag;
	int reset_gpio;
	enum of_gpio_flags reset_gpio_flag;

	bool enabled;
	bool prepared;

	const struct panel_simple_desc *desc;
	struct panel_simple_host host;
};

static inline struct panel_simple *to_panel_simple(struct drm_panel *panel)
{
	return container_of(panel, struct panel_simple, panel);
}

static int panel_simple_disable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	if (!p->enabled)
		return 0;

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_POWERDOWN;
		p->backlight->props.state |= BL_CORE_FBBLANK;
		backlight_update_status(p->backlight);
	}

	if (p->desc->delay->disable)
		msleep(p->desc->delay->disable);

	p->enabled = false;

	return 0;
}

static int panel_simple_unprepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	if (!p->prepared)
		return 0;

	if (p->reset_gpio)
		gpio_direction_output(p->reset_gpio,
			INACTIVE(p->reset_gpio_flag));
	if (p->power_gpio) {
		gpio_direction_output(p->power_gpio,
			INACTIVE(p->power_gpio_flag));
	}

	if (p->supply)
		regulator_disable(p->supply);

	if (p->desc->delay->unprepare)
		msleep(p->desc->delay->unprepare);

	p->prepared = false;

	return 0;
}

static int panel_simple_prepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	ssize_t size = 0;

	DRM_DEBUG_KMS("\n");
	if (p->prepared)
		return 0;

	if (p->supply) {
		int err;

		err = regulator_enable(p->supply);
		if (err < 0) {
			dev_err(p->dev, "failed to enable supply: %d\n",
				err);
			return err;
		}
	}
	if (p->power_gpio) {
		DRM_DEBUG_KMS("power gpio\n");
		gpio_direction_output(p->power_gpio,
			ACTIVE(p->power_gpio_flag));
		msleep(p->desc->delay->power_on);
	}

	if (p->reset_gpio) {
		DRM_DEBUG_KMS("reset gpio\n");
		gpio_direction_output(p->reset_gpio,
			ACTIVE(p->reset_gpio_flag));
		msleep(p->desc->delay->reset);
		gpio_direction_output(p->reset_gpio,
			INACTIVE(p->reset_gpio_flag));
		msleep(p->desc->delay->reset);
		gpio_direction_output(p->reset_gpio,
			ACTIVE(p->reset_gpio_flag));
	}

	if (p->desc->delay->prepare)
		msleep(p->desc->delay->prepare);

	if (p->desc->ops.transfer)
		size = p->desc->ops.transfer(&p->host);
	DRM_DEBUG_KMS("transfer bytes %zd", size);
	p->prepared = true;

	return 0;
}

static int panel_simple_enable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	if (p->enabled)
		return 0;

	if (p->backlight) {
		p->backlight->props.state &= ~BL_CORE_FBBLANK;
		p->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(p->backlight);
	}

	if (p->desc->delay->enable)
		msleep(p->desc->delay->enable);

	p->enabled = true;

	return 0;
}

static int panel_simple_get_modes(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	struct drm_connector *connector = panel->connector;
	struct drm_device *drm = panel->drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!p->desc)
		return 0;

	for (i = 0; i < p->desc->num_modes; i++) {
		const struct drm_display_mode *m = &p->desc->modes[i];

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

	connector->display_info.width_mm = p->desc->size.width;
	connector->display_info.height_mm = p->desc->size.height;

	return num;
}

static int panel_simple_get_mode_private(struct drm_panel *panel,
		const struct drm_display_mode *mode, void **private)
{
	struct panel_simple *p = to_panel_simple(panel);

	if (!(p->desc && p->desc->modes))
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

static int panel_simple_boot_sync(struct drm_panel *panel,
					int sync_step)
{
	struct panel_simple *p = to_panel_simple(panel);

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

static const struct drm_panel_funcs panel_simple_funcs = {
	.disable = panel_simple_disable,
	.unprepare = panel_simple_unprepare,
	.prepare = panel_simple_prepare,
	.enable = panel_simple_enable,
	.get_modes = panel_simple_get_modes,
	.get_mode_private = panel_simple_get_mode_private,
	.boot_sync = panel_simple_boot_sync,
};

static const struct of_device_id panel_simple_of_table[] = {
#ifdef CONFIG_DRM_PANEL_SIMPLE
	{
		.compatible = "lombo,panel_simple",
	},
#endif
#ifdef CONFIG_DRM_PANEL_SPC_S92048
	{
		.compatible = "spc,s92048",
		.data = &simple_spc_s92048_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_FJ018WV04
	{
		.compatible = "fj018wv04",
		.data = &simple_fj018wv04_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_TIANMA_TM070JDHG30
	{
		.compatible = "tianma,tm070jdhg30",
		.data = &tianma_tm070jdhg30_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_WKS_WKS70002
	{
		.compatible = "ek79003a,wks70002",
		.data = &ek79003a_wks70002_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_DUAL_LINK_B101UAN021
	{
		.compatible = "b101uan021",
		.data = &b101uan021_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_WKS43178
	{
		.compatible = "wks,wks43178",
		.data = &wks43178_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_SLM5P5_HX8399C
	{
		.compatible = "slm5p5,hx8399c",
		.data = &slm5p5_hx8399c_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_LT8912B
	{
		.compatible = "lt8912b",
		.data = &lt8912b_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_XJS50FWI4015
	{
		.compatible = "fiti,xjs50fwi4015",
		.data = &xjs50fwi4015_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_CJD9365
	{
		.compatible = "fiti,cjd9365",
		.data = &cjd9365_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_AM500V40
	{
		.compatible = "fiti,am500v40",
		.data = &am500v40_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_SC500H40
	{
		.compatible = "fiti,sc500hc40",
		.data = &sc500hc40_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_DPMIPIMODEL
	{
		.compatible = "dp,mipimodel",
		.data = &dpmipi_desc,
	},
#endif

	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, panel_simple_of_table);

static int fill_tcon_rgb_if_info(struct lombo_vo_dev_config *vo,
						struct device_node *np)
{
	int val;
	if (!of_property_read_u32(np, "first_line", &val))
		vo->rgb_if.srgb_if.first_line = val;
	DRM_INFO("first_line: %d\n", vo->rgb_if.srgb_if.first_line);

	if (!of_property_read_u32(np, "second_line", &val))
		vo->rgb_if.srgb_if.second_line = val;
	DRM_INFO("second_line: %d\n", vo->rgb_if.srgb_if.second_line);

	if (!of_property_read_u32(np, "field_pol", &val))
		vo->rgb_if.field_pol = val;
	DRM_INFO("field_pol: %d\n", vo->rgb_if.field_pol);

	if (!of_property_read_u32(np, "vblank_pol", &val))
		vo->rgb_if.vblank_pol = val;
	DRM_INFO("vblank_pol: %d\n", vo->rgb_if.vblank_pol);

	if (!of_property_read_u32(np, "hblank_pol", &val))
		vo->rgb_if.hblank_pol = val;
	DRM_INFO("hblank_pol: %d\n", vo->rgb_if.hblank_pol);

	if (!of_property_read_u32(np, "vsync_pol", &val))
		vo->rgb_if.vsync_pol = val;
	DRM_INFO("vsync_pol: %d\n", vo->rgb_if.vsync_pol);

	if (!of_property_read_u32(np, "hsync_pol", &val))
		vo->rgb_if.hsync_pol = val;
	DRM_INFO("hsync_pol: %d\n", vo->rgb_if.hsync_pol);

	if (!of_property_read_u32(np, "de_pol", &val))
		vo->rgb_if.de_pol = val;
	DRM_INFO("de_pol: %d\n", vo->rgb_if.de_pol);

	if (!of_property_read_u32(np, "clk_pol", &val))
		vo->rgb_if.clk_pol = val;
	DRM_INFO("clk_pol: %d\n", vo->rgb_if.clk_pol);

	vo->rgb_if.is_clk_ddr =	of_property_read_bool(np, "is_clk_ddr");
	DRM_INFO("is_clk_ddr: %d\n", vo->rgb_if.is_clk_ddr);

	return 0;
}

static int fill_tcon_cpu_if_info(struct lombo_vo_dev_config *vo,
						struct device_node *np)
{
	int val;

	vo->cpu_if.has_vsync_cmd = of_property_read_bool(np, "has_vsync_cmd");
	DRM_INFO("has_vsync_cmd: %d\n",	vo->cpu_if.has_vsync_cmd);

	vo->cpu_if.has_hsync_cmd = of_property_read_bool(np, "has_hsync_cmd");
	DRM_INFO("has_hsync_cmd: %d\n",	vo->cpu_if.has_hsync_cmd);

	if (!of_property_read_u32(np, "cmd_align", &val))
		vo->cpu_if.cmd_align = val;
	DRM_INFO("cmd_align: %d\n", vo->cpu_if.cmd_align);

	if (!of_property_read_u32(np, "te_pol", &val))
		vo->cpu_if.te_pol = val;
	DRM_INFO("te_pol: %d\n", vo->cpu_if.te_pol);

	return 0;
}

static int fill_tcon_lvds_if_info(struct lombo_vo_dev_config *vo,
						struct device_node *np)
{
	int val;

	vo->lvds_if.is_dual_link =
		of_property_read_bool(np, "is_dual_link");
	DRM_INFO("is_dual_link: %d\n",
		vo->lvds_if.is_dual_link);

	vo->lvds_if.is_ctrl_invert =
		of_property_read_bool(np, "is_ctrl_invert");
	DRM_INFO("is_ctrl_invert: %d\n",
		vo->lvds_if.is_ctrl_invert);

	if (!of_property_read_u32(np, "mode", &val))
		vo->lvds_if.mode = val;
	DRM_INFO("mode: %d\n", vo->lvds_if.mode);

	if (!of_property_read_u32(np, "bpp", &val))
		vo->lvds_if.bpp = val;
	DRM_INFO("bpp: %d\n", vo->lvds_if.bpp);

	return 0;
}

static int fill_tcon_mipi_dsi_if_info(
		struct lombo_vo_dev_config *vo, struct device_node *np)
{
	return 0;
}

static int fill_vo_dev_info(struct device *dev,
	struct lombo_vo_dev_config *vo,	struct device_node *np)
{
	int val, n;
	u32 *data = NULL;

	/* Get lombo_vo_dev_config requied config */

	if (!of_property_read_u32(np, "vo_dev_if", &val))
		vo->dev_if = val;
	DRM_INFO("vo_dev_if: %d\n", vo->dev_if);

	switch (vo->dev_if) {
	case VO_DEV_PRGB:
	case VO_DEV_SRGB_RGB:
	case VO_DEV_SRGB_DUMMY_RGB:
	case VO_DEV_SRGB_RGB_DUMMY:
		fill_tcon_rgb_if_info(vo, np);
		break;
	case VO_DEV_CPU_18BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB888:
	case VO_DEV_CPU_16BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB565:
	case VO_DEV_CPU_9BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB565:
	case VO_DEV_CPU_8BIT_RGB888:
		fill_tcon_cpu_if_info(vo, np);
		break;
	case VO_DEV_LVDS:
		fill_tcon_lvds_if_info(vo, np);
		break;
	case VO_DEV_MIPI_DSI_VIDEO:
	case VO_DEV_MIPI_DSI_CMD:
		fill_tcon_mipi_dsi_if_info(vo, np);
		break;
	default:
		break;
	}

	vo->timing.is_interlace =
		of_property_read_bool(np, "is_interlace");
	DRM_INFO("is_interlace: %d\n",
		vo->timing.is_interlace);

	n = of_property_count_u32_elems(np, "field1");
	if (n > 0) {
		data = devm_kzalloc(dev, n * sizeof(u32), GFP_KERNEL);
		if (!data)
			return -ENOMEM;
		memset(data, 0, n * sizeof(u32));

		if (!of_property_read_u32_array(np,
			"field1", data, n)) {
			vo->timing.field1.vt = *(data + 0);
			vo->timing.field1.vact = *(data + 1);
			vo->timing.field1.vfp = *(data + 2);
			vo->timing.field1.vsw = *(data + 3);

			DRM_INFO("field1.vt: %d\n", vo->timing.field1.vt);
			DRM_INFO("field1.vact: %d\n", vo->timing.field1.vact);
			DRM_INFO("field1.vfp: %d\n", vo->timing.field1.vfp);
			DRM_INFO("field1.vsw: %d\n", vo->timing.field1.vsw);
		}
		if (!of_property_read_u32_array(np,
			"field2", data, n)) {
			vo->timing.field2.vt = *(data + 0);
			vo->timing.field2.vact = *(data + 1);
			vo->timing.field2.vfp = *(data + 2);
			vo->timing.field2.vsw = *(data + 3);

			DRM_INFO("field2.vt: %d\n", vo->timing.field2.vt);
			DRM_INFO("field2.vact: %d\n", vo->timing.field2.vact);
			DRM_INFO("field2.vfp: %d\n", vo->timing.field2.vfp);
			DRM_INFO("field2.vsw: %d\n", vo->timing.field2.vsw);
		}
		if (!of_property_read_u32_array(np,
			"hline", data, n)) {
			vo->timing.hline.ht = *(data + 0);
			vo->timing.hline.hact = *(data + 1);
			vo->timing.hline.hfp = *(data + 2);
			vo->timing.hline.hsw = *(data + 3);

			DRM_INFO("ht: %d\n", vo->timing.hline.ht);
			DRM_INFO("hact: %d\n", vo->timing.hline.hact);
			DRM_INFO("hfp: %d\n", vo->timing.hline.hfp);
			DRM_INFO("hsw: %d\n", vo->timing.hline.hsw);
		}
	}

	if (!of_property_read_u32(np, "dclk_freq", &val))
		vo->timing.dclk_freq = val;
	DRM_INFO("dclk_freq: %d\n", vo->timing.dclk_freq);

	/* if (!of_property_read_u32(np, "timing_width", &val))
		vo->timing.width = val; */
	vo->timing.width = vo->timing.hline.hact;
	DRM_INFO("timing_width: %d\n", vo->timing.width);

	/* if (!of_property_read_u32(np, "timing_height", &val))
		vo->timing.height = val; */
	vo->timing.height = vo->timing.field1.vact + vo->timing.field2.vact;
	DRM_INFO("timing_height: %d\n", vo->timing.height);

	if (!of_property_read_u32(np, "format", &val))
		vo->format = val;
	DRM_INFO("format: %d\n", vo->format);

	/* TODO: Get lombo_vo_dev_config optional config */

	return 0;
}

static int fill_panel_mode_info(struct panel_simple *p,
	struct lombo_vo_dev_config *vo,	struct device_node *np)
{
	int val;

	p->desc->modes->clock = vo->timing.dclk_freq / 1000;
	p->desc->modes->hdisplay = vo->timing.hline.hact;
	p->desc->modes->hsync_start =
		vo->timing.hline.hact + vo->timing.hline.hfp;
	p->desc->modes->hsync_end = vo->timing.hline.hact +
		vo->timing.hline.hfp + vo->timing.hline.hsw;
	p->desc->modes->htotal = vo->timing.hline.ht;

	p->desc->modes->vdisplay = vo->timing.field1.vact;
	p->desc->modes->vsync_start =
		vo->timing.field1.vact + vo->timing.field1.vfp;
	p->desc->modes->vsync_end = vo->timing.field1.vact +
		vo->timing.field1.vfp + vo->timing.field1.vsw;
	p->desc->modes->vtotal = vo->timing.field1.vt;

	DRM_INFO("clock: %d\n", p->desc->modes->clock);
	DRM_INFO("hdisplay: %d\n", p->desc->modes->hdisplay);
	DRM_INFO("hsync_start: %d\n", p->desc->modes->hsync_start);
	DRM_INFO("hsync_end: %d\n", p->desc->modes->hsync_end);
	DRM_INFO("htotal: %d\n", p->desc->modes->htotal);
	DRM_INFO("vdisplay: %d\n", p->desc->modes->vdisplay);
	DRM_INFO("vsync_start: %d\n", p->desc->modes->vsync_start);
	DRM_INFO("vsync_end: %d\n", p->desc->modes->vsync_end);
	DRM_INFO("vtotal: %d\n", p->desc->modes->vtotal);

	if (!of_property_read_u32(np, "vrefresh", &val))
		p->desc->modes->vrefresh = val;
	DRM_INFO("vrefresh: %d\n", p->desc->modes->vrefresh);

	if (of_property_read_bool(np, "is_preferred_mode"))
		p->desc->modes->type |= DRM_MODE_TYPE_PREFERRED;
	DRM_INFO("is_preferred_mode: %s\n",
		p->desc->modes->type ? "yes" : "no");

	return 0;
}

static int panel_simple_parse_dts_info(struct device *dev,
	struct panel_simple *p,	struct lombo_vo_dev_config *vo,
	struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret, val;

	/* Get GPIOs and backlight controller. */
	p->power_gpio = of_get_named_gpio_flags(np,
					"power_gpio", 0, &flags);
	p->power_gpio_flag = flags;
	if (!gpio_is_valid(p->power_gpio)) {
		dev_err(p->dev, "failed to get %s GPIO\n", "power_gpio");
		p->power_gpio = 0;
	} else {
		DRM_DEBUG_KMS("enable-gpio %d, flags 0x%x\n",
			p->power_gpio, flags);
		ret = devm_gpio_request(p->dev,
				p->power_gpio, "power_gpio");
		if (ret) {
			dev_err(p->dev, "power gpio %d request failed\n",
				p->power_gpio);
			p->power_gpio = 0;
		}
	}
	DRM_INFO("power_gpio: %d\npower_gpio_flag: %d\n",
				p->reset_gpio, p->reset_gpio_flag);

	p->reset_gpio = of_get_named_gpio_flags(np,
					"reset_gpio", 0, &flags);
	p->reset_gpio_flag = flags;
	if (!gpio_is_valid(p->reset_gpio)) {
		dev_err(p->dev, "failed to get %s GPIO\n", "reset_gpio");
		p->reset_gpio = 0;
	} else {
		DRM_DEBUG_KMS("reset-gpio %d, flags 0x%x\n",
			p->reset_gpio, flags);
		ret = devm_gpio_request(p->dev,
				p->reset_gpio, "reset_gpio");
		if (ret) {
			dev_err(p->dev, "enable gpio %d request failed\n",
				p->reset_gpio);
			p->reset_gpio = 0;
		}
	}
	DRM_INFO("reset_gpio: %d\nreset_gpio_flag: %d\n",
				p->reset_gpio, p->reset_gpio_flag);

	/* Get lombo_vo_dev_config config */
	fill_vo_dev_info(dev, vo, np);

	/* Get drm_display_mode config */
	fill_panel_mode_info(p, vo, np);

	if (!of_property_read_u32(np, "delay_enable", &val))
		p->desc->delay->enable = val;
	DRM_INFO("delay_enable: %d\n", p->desc->delay->enable);

	if (!of_property_read_u32(np, "delay_disable", &val))
		p->desc->delay->disable = val;
	DRM_INFO("delay_disable: %d\n", p->desc->delay->disable);

	if (!of_property_read_u32(np, "delay_prepare", &val))
		p->desc->delay->prepare = val;
	DRM_INFO("delay_prepare: %d\n", p->desc->delay->prepare);

	if (!of_property_read_u32(np, "delay_unprepare", &val))
		p->desc->delay->unprepare = val;
	DRM_INFO("delay_unprepare: %d\n", p->desc->delay->unprepare);

	return 0;
}

static int panel_simple_init_panel_desc(struct device *dev,
	const struct of_device_id *id, struct panel_simple *p)
{
	struct panel_simple_desc *pdesc = NULL;

	if (id) {
		if (id->data) {
			p->desc = id->data;
			DRM_INFO("use of match address %p\n", id->data);
		} else {
			pdesc = devm_kzalloc(dev,
				sizeof(struct panel_simple_desc), GFP_KERNEL);
			if (!pdesc)
				return -ENOMEM;
			pdesc->modes = devm_kzalloc(dev,
				sizeof(struct drm_display_mode), GFP_KERNEL);
			if (!pdesc->modes)
				return -ENOMEM;
			pdesc->modes->private = (int *)devm_kzalloc(dev,
				sizeof(struct lombo_vo_dev_config), GFP_KERNEL);
			if (!pdesc->modes->private)
				return -ENOMEM;
			pdesc->modes->private_size =
				sizeof(struct lombo_vo_dev_config);
			pdesc->delay = devm_kzalloc(dev,
				sizeof(struct panel_delay), GFP_KERNEL);
			if (!pdesc->delay)
				return -ENOMEM;
			pdesc->num_modes = 1;
			pdesc->size.width = 150;
			pdesc->size.height = 100;
			p->desc = pdesc;
			DRM_INFO("malloc lombo_simple_desc %p\n", p->desc);
		}
	}

	return 0;
}

static int panel_simple_init_panel_host(struct panel_simple *p)
{
	struct lombo_vo_device *vo;
	struct mipi_dsi_device *dsi_device;

	switch (p->desc->init_panel) {
	case PANEL_SIMPLE_DSI:
		dsi_device = devm_kzalloc(p->dev,
				sizeof(*dsi_device), GFP_KERNEL);
		if (!dsi_device)
			return -ENOMEM;
		dsi_device->channel = 0;
		p->panel.dsi = dsi_device;
		p->host.dsi = dsi_device;
		DRM_DEBUG_KMS("dsi=%p\n", dsi_device);
		break;
	case PANEL_SIMPLE_CPU:
		vo = devm_kzalloc(p->dev, sizeof(*vo), GFP_KERNEL);
		if (!vo)
			return -ENOMEM;
		p->panel.lombo_vo_dev = vo;
		p->host.lombo_vo_dev = vo;
		DRM_DEBUG("cpu_vo=%p\n", vo);
		break;
	case PANEL_SIMPLE_I2C:
		/* to be done */
		if (p->desc->ops.attach)
			p->desc->ops.attach(NULL);
		break;
	case PANEL_SIMPLE_SPI:
		/* to be done */
		if (p->desc->ops.attach)
			p->desc->ops.attach(p->dev);
		break;
	case PANEL_SIMPLE_NONE:
	default:
		break;
	}

	return 0;
}

static int panel_simple_probe(struct platform_device *pdev)
{
	struct panel_simple *p;
	struct device_node *np = pdev->dev.of_node;
	struct device_node *remote;
	struct backlight_device	*back;
	int ret;
	const struct of_device_id *id;
	struct device *dev = &pdev->dev;

	DRM_DEBUG("%d\n", __LINE__);
	if (!of_device_is_available(np))
		return -ENODEV;

	p = devm_kzalloc(&pdev->dev, sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;
	p->dev = &pdev->dev;

	id = of_match_node(panel_simple_of_table, pdev->dev.of_node);
	panel_simple_init_panel_desc(dev, id, p);

	panel_simple_init_panel_host(p);
	panel_simple_parse_dts_info(dev, p,
		(struct lombo_vo_dev_config *)(p->desc->modes->private), np);

	remote = of_parse_phandle(np, "backlight", 0);
	if (!remote)
		DRM_ERROR("failed to parse phandle of backlight\n");

	else {
		back = of_find_backlight_by_node(remote);
		if (!back)
			DRM_ERROR("Error: no find backlight device\n");

		p->backlight = back;
	}

	/* Register the panel. */
	drm_panel_init(&p->panel);
	p->panel.dev = p->dev;
	p->panel.funcs = &panel_simple_funcs;

	ret = drm_panel_add(&p->panel);
	if (ret < 0)
		goto error;

	dev_set_drvdata(p->dev, p);
	return 0;

error:
	put_device(&p->backlight->dev);
	return ret;
}

static int panel_simple_remove(struct platform_device *pdev)
{
	struct panel_simple *p = dev_get_drvdata(&pdev->dev);

	DRM_DEBUG("%d\n", __LINE__);
	drm_panel_detach(&p->panel);
	drm_panel_remove(&p->panel);

	panel_simple_disable(&p->panel);
	panel_simple_unprepare(&p->panel);

	if (p->backlight)
		put_device(&p->backlight->dev);

	return 0;
}

static void panel_simple_shutdown(
	struct platform_device *pdev)
{
	struct panel_simple *p = dev_get_drvdata(&pdev->dev);

	DRM_DEBUG("%d\n", __LINE__);
	panel_simple_disable(&p->panel);
	panel_simple_unprepare(&p->panel);
}

static struct platform_driver panel_simple_driver = {
	.probe		= panel_simple_probe,
	.remove		= panel_simple_remove,
	.shutdown	= panel_simple_shutdown,
	.driver		= {
		.name	= "panel-simple",
		.of_match_table = panel_simple_of_table,
	},
};

static int __init panel_simple_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(&panel_simple_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(panel_simple_init);

static void __exit panel_simple_exit(void)
{
	platform_driver_unregister(&panel_simple_driver);
}
module_exit(panel_simple_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("SIMPLE Panel Driver");
MODULE_LICENSE("GPL");

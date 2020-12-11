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

#include "panel-lvds.h"
#include "lombo_panel_config.h"

#define WRITEREG32(reg, val)           writel(val, (void __iomem *)reg)
#define READREG32(reg)                 readl((void __iomem *)reg)

#define LVDS_GPIO_ACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)
#define LVDS_GPIO_INACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)

struct panel_lvds {
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

	int enable_gpio;
	enum of_gpio_flags enable_gpio_flag;

	int bl_gpio;
	enum of_gpio_flags bl_gpio_flag;

	const struct panel_lvds_desc *desc;
};

static inline struct panel_lvds *to_panel_lvds(struct drm_panel *panel)
{
	return container_of(panel, struct panel_lvds, panel);
}

static int panel_lvds_disable(struct drm_panel *panel)
{
	struct panel_lvds *lvds = to_panel_lvds(panel);

	if (lvds->backlight) {
		lvds->backlight->props.power = FB_BLANK_POWERDOWN;
		lvds->backlight->props.state |= BL_CORE_FBBLANK;
		backlight_update_status(lvds->backlight);
	}

	if (lvds->bl_gpio)
		gpio_direction_output(lvds->bl_gpio,
			LVDS_GPIO_INACTIVE(lvds->bl_gpio_flag));

	return 0;
}

static int panel_lvds_unprepare(struct drm_panel *panel)
{
	struct panel_lvds *lvds = to_panel_lvds(panel);

	if (lvds->enable_gpio)
		gpio_direction_output(lvds->enable_gpio,
			LVDS_GPIO_INACTIVE(lvds->enable_gpio_flag));

	if (lvds->supply)
		regulator_disable(lvds->supply);

	return 0;
}

static int panel_lvds_prepare(struct drm_panel *panel)
{
	struct panel_lvds *lvds = to_panel_lvds(panel);

	DRM_DEBUG_KMS("\n");

	if (lvds->supply) {
		int err;

		err = regulator_enable(lvds->supply);
		if (err < 0) {
			dev_err(lvds->dev, "failed to enable supply: %d\n",
				err);
			return err;
		}
	}

	if (lvds->enable_gpio) {
		DRM_DEBUG_KMS("enable gpio\n");
		gpio_direction_output(lvds->enable_gpio,
			LVDS_GPIO_ACTIVE(lvds->enable_gpio_flag));
		msleep(100);
		gpio_direction_output(lvds->enable_gpio,
			LVDS_GPIO_INACTIVE(lvds->enable_gpio_flag));
		msleep(100);
		gpio_direction_output(lvds->enable_gpio,
			LVDS_GPIO_ACTIVE(lvds->enable_gpio_flag));
		msleep(100);
	}

	return 0;
}

static int panel_lvds_enable(struct drm_panel *panel)
{
	struct panel_lvds *lvds = to_panel_lvds(panel);

	if (lvds->backlight) {
		lvds->backlight->props.state &= ~BL_CORE_FBBLANK;
		lvds->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(lvds->backlight);
	}

	if (lvds->bl_gpio)
		gpio_direction_output(lvds->bl_gpio,
			LVDS_GPIO_ACTIVE(lvds->bl_gpio_flag));

	return 0;
}

static int panel_lvds_get_modes(struct drm_panel *panel)
{
	struct panel_lvds *lvds = to_panel_lvds(panel);
	struct drm_connector *connector = panel->connector;
	struct drm_device *drm = panel->drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!lvds->desc)
		return 0;

	for (i = 0; i < lvds->desc->num_modes; i++) {
		const struct drm_display_mode *m = &lvds->desc->modes[i];

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

	connector->display_info.width_mm = lvds->desc->size.width;
	connector->display_info.height_mm = lvds->desc->size.height;

	return num;
}

static int panel_lvds_get_mode_private(struct drm_panel *panel,
		const struct drm_display_mode *mode, void **private)
{
	struct panel_lvds *lvds = to_panel_lvds(panel);

	if (!(lvds->desc && lvds->desc->modes))
		return -EINVAL;

	if (drm_mode_equal_no_clocks(mode, &lvds->desc->modes[0])) {
		DRM_DEBUG_KMS("private=%p\n",
			lvds->desc->modes[0].private);
		*private = lvds->desc->modes[0].private;
		return 0;
	}
	return -EINVAL;
}

static int panel_lvds_boot_sync(struct drm_panel *panel, int sync_step)
{
	DRM_DEBUG_KMS("sync_step=%d\n", sync_step);
	return 0;
}

static const struct drm_panel_funcs panel_lvds_funcs = {
	.disable = panel_lvds_disable,
	.unprepare = panel_lvds_unprepare,
	.prepare = panel_lvds_prepare,
	.enable = panel_lvds_enable,
	.get_modes = panel_lvds_get_modes,
	.get_mode_private = panel_lvds_get_mode_private,
	.boot_sync = panel_lvds_boot_sync,
};

static const struct of_device_id panel_lvds_of_table[] = {
	{ .compatible = "panel-lvds", },
#ifdef CONFIG_DRM_PANEL_WKS_WKS70002
	{
		.compatible = "ek79003a,wks70002",
		.data = &ek79003a_wks70002_desc,
	},
#endif
#ifdef CONFIG_DRM_PANEL_TIANMA_TM070JDHG30
	{
		.compatible = "tianma,tm070jdhg30",
		.data = &tianma_tm070jdhg30_desc,
	},
#endif
	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, panel_lvds_of_table);

static int panel_lvds_probe(struct platform_device *pdev)
{
	struct panel_lvds *lvds;
	struct device_node *np = pdev->dev.of_node;
	int ret;
	enum of_gpio_flags flags;
	const struct of_device_id *id;

	if (!of_device_is_available(np))
		return -ENODEV;

	lvds = devm_kzalloc(&pdev->dev, sizeof(*lvds), GFP_KERNEL);
	if (!lvds)
		return -ENOMEM;

	lvds->dev = &pdev->dev;

	id = of_match_node(panel_lvds_of_table, pdev->dev.of_node);
	if (id)
		lvds->desc = id->data;

#if 0
	lvds->supply = devm_regulator_get_optional(lvds->dev, "power");
	if (IS_ERR(lvds->supply)) {
		ret = PTR_ERR(lvds->supply);

		if (ret != -ENODEV) {
			if (ret != -EPROBE_DEFER)
				dev_err(lvds->dev, "failed to request regulator: %d\n",
					ret);
			return ret;
		}

		lvds->supply = NULL;
	}
#endif

	/* Get GPIOs and backlight controller. */
	lvds->enable_gpio = of_get_named_gpio_flags(np,
					"enable_gpio", 0, &flags);
	if (!gpio_is_valid(lvds->enable_gpio)) {
		dev_err(lvds->dev, "failed to get %s GPIO\n", "enable_gpio");
		lvds->enable_gpio = 0;
	} else {
		DRM_DEBUG_KMS("enable-gpio %d, flags 0x%x\n",
			lvds->enable_gpio, flags);
		ret = devm_gpio_request(lvds->dev,
				lvds->enable_gpio, "enable_gpio");
		if (ret) {
			dev_err(lvds->dev, "enable gpio %d request failed\n",
				lvds->enable_gpio);
			lvds->enable_gpio = 0;
		}
	}

	lvds->bl_gpio = of_get_named_gpio_flags(np,
					"bl_gpio", 0, &flags);
	lvds->bl_gpio_flag = flags;
	if (!gpio_is_valid(lvds->bl_gpio)) {
		dev_err(lvds->dev, "failed to get %s GPIO\n", "bl_gpio");
		lvds->bl_gpio = 0;
	} else {
		DRM_DEBUG_KMS("bl-gpio %d, flags 0x%x\n",
			lvds->bl_gpio, flags);
		ret = devm_gpio_request(lvds->dev,
				lvds->bl_gpio, "bl_gpio");
		if (ret) {
			dev_err(lvds->dev, "bl gpio %d request failed\n",
				lvds->bl_gpio);
			lvds->bl_gpio = 0;
		}
	}

	np = of_parse_phandle(lvds->dev->of_node, "backlight", 0);
	if (np) {
		lvds->backlight = of_find_backlight_by_node(np);
		of_node_put(np);

		if (!lvds->backlight)
			return -EPROBE_DEFER;
	}

	/*
	 * TODO: Handle all power supplies specified in the DT node in a generic
	 * way for panels that don't care about power supply ordering. LVDS
	 * panels that require a specific power sequence will need a dedicated
	 * driver.
	 */

	/* Register the panel. */
	drm_panel_init(&lvds->panel);
	lvds->panel.dev = lvds->dev;
	lvds->panel.funcs = &panel_lvds_funcs;

	ret = drm_panel_add(&lvds->panel);
	if (ret < 0)
		goto error;

	dev_set_drvdata(lvds->dev, lvds);
	return 0;

error:
	put_device(&lvds->backlight->dev);
	return ret;
}

static int panel_lvds_remove(struct platform_device *pdev)
{
	struct panel_lvds *lvds = dev_get_drvdata(&pdev->dev);

	drm_panel_remove(&lvds->panel);

	panel_lvds_disable(&lvds->panel);

	if (lvds->backlight)
		put_device(&lvds->backlight->dev);

	return 0;
}

static struct platform_driver panel_lvds_driver = {
	.probe		= panel_lvds_probe,
	.remove		= panel_lvds_remove,
	.driver		= {
		.name	= "panel-lvds",
		.of_match_table = panel_lvds_of_table,
	},
};

static int __init panel_lvds_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(&panel_lvds_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(panel_lvds_init);

static void __exit panel_lvds_exit(void)
{
	platform_driver_unregister(&panel_lvds_driver);
}
module_exit(panel_lvds_exit);

MODULE_AUTHOR("Laurent Pinchart <laurent.pinchart@ideasonboard.com>");
MODULE_DESCRIPTION("LVDS Panel Driver");
MODULE_LICENSE("GPL");

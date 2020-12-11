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

#include "lombo_panel_config.h"

#define WRITEREG32(reg, val)           writel(val, (void __iomem *)reg)
#define READREG32(reg)                 readl((void __iomem *)reg)

#define RGB_GPIO_ACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)
#define RGB_GPIO_INACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)

#define IS_INTERLACE	(FALSE)

#define WIDTH		(800)
#define HEIGHT		(480)

#define HBP		(88)
#define HFP		(40)
#define HSW		(128)
#define HACT		(WIDTH)
#define VBP_F1		(33)
#define VFP_F1		(10)
#define VSW_F1		(2)
#define VACT_F1		(HEIGHT)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT		(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define DCLK		(33000000) /* 33MHz */

struct panel_rgb_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;

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

};

struct panel_rgb {
	struct drm_panel panel;
	struct device *dev;

	struct backlight_device *backlight;
	struct regulator *supply;

	int enable_gpio;
	enum of_gpio_flags enable_gpio_flag;

	int bl_gpio;
	enum of_gpio_flags bl_gpio_flag;

	const struct panel_rgb_desc *desc;
};

static inline struct panel_rgb *to_panel_rgb(struct drm_panel *panel)
{
	return container_of(panel, struct panel_rgb, panel);
}

static int panel_rgb_disable(struct drm_panel *panel)
{
	struct panel_rgb *rgb = to_panel_rgb(panel);

	if (rgb->backlight) {
		rgb->backlight->props.power = FB_BLANK_POWERDOWN;
		rgb->backlight->props.state |= BL_CORE_FBBLANK;
		backlight_update_status(rgb->backlight);
	}

	if (rgb->bl_gpio)
		gpio_direction_output(rgb->bl_gpio,
			RGB_GPIO_INACTIVE(rgb->bl_gpio_flag));

	return 0;
}

static int panel_rgb_unprepare(struct drm_panel *panel)
{
	struct panel_rgb *rgb = to_panel_rgb(panel);

	if (rgb->enable_gpio)
		gpio_direction_output(rgb->enable_gpio,
			RGB_GPIO_INACTIVE(rgb->enable_gpio_flag));

	if (rgb->supply)
		regulator_disable(rgb->supply);

	return 0;
}

static int panel_rgb_prepare(struct drm_panel *panel)
{
	struct panel_rgb *rgb = to_panel_rgb(panel);

	if (rgb->supply) {
		int err;

		err = regulator_enable(rgb->supply);
		if (err < 0) {
			dev_err(rgb->dev, "failed to enable supply: %d\n",
				err);
			return err;
		}
	}

	if (rgb->enable_gpio) {
		DRM_DEBUG_KMS("enable gpio\n");
		gpio_direction_output(rgb->enable_gpio,
			RGB_GPIO_ACTIVE(rgb->enable_gpio_flag));
		msleep(100);
		gpio_direction_output(rgb->enable_gpio,
			RGB_GPIO_INACTIVE(rgb->enable_gpio_flag));
		msleep(100);
		gpio_direction_output(rgb->enable_gpio,
			RGB_GPIO_ACTIVE(rgb->enable_gpio_flag));
		msleep(100);
	}

	return 0;
}

static int panel_rgb_enable(struct drm_panel *panel)
{
	struct panel_rgb *rgb = to_panel_rgb(panel);

	if (rgb->backlight) {
		rgb->backlight->props.state &= ~BL_CORE_FBBLANK;
		rgb->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(rgb->backlight);
	}

	if (rgb->bl_gpio)
		gpio_direction_output(rgb->bl_gpio,
			RGB_GPIO_ACTIVE(rgb->bl_gpio_flag));

	return 0;
}

static int panel_rgb_get_modes(struct drm_panel *panel)
{
	struct panel_rgb *rgb = to_panel_rgb(panel);
	struct drm_connector *connector = panel->connector;
	struct drm_device *drm = panel->drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!rgb->desc)
		return 0;

	for (i = 0; i < rgb->desc->num_modes; i++) {
		const struct drm_display_mode *m = &rgb->desc->modes[i];

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

	connector->display_info.width_mm = rgb->desc->size.width;
	connector->display_info.height_mm = rgb->desc->size.height;

	return num;
}

static int panel_rgb_get_mode_private(struct drm_panel *panel,
		const struct drm_display_mode *mode, void **private)
{
	struct panel_rgb *rgb = to_panel_rgb(panel);

	if (!(rgb->desc && rgb->desc->modes))
		return -EINVAL;

	if (drm_mode_equal_no_clocks(mode, &rgb->desc->modes[0])) {
		DRM_DEBUG_KMS("private=%p\n",
			rgb->desc->modes[0].private);
		*private = rgb->desc->modes[0].private;
		return 0;
	}
	return -EINVAL;
}

static int panel_rgb_boot_sync(struct drm_panel *panel, int sync_step)
{
	DRM_DEBUG_KMS("sync_step=%d\n", sync_step);
	return 0;
}

static const struct drm_panel_funcs panel_rgb_funcs = {
	.disable = panel_rgb_disable,
	.unprepare = panel_rgb_unprepare,
	.prepare = panel_rgb_prepare,
	.enable = panel_rgb_enable,
	.get_modes = panel_rgb_get_modes,
	.get_mode_private = panel_rgb_get_mode_private,
	.boot_sync = panel_rgb_boot_sync,
};

static struct lombo_vo_dev_config spc_s92048_vo = {
	/* device interface, reference to vo_dev_if_t */
	.dev_if = VO_DEV_PRGB,
	/* device specific configuation, reference to vo_if_cfg_t */
	.rgb_if.vsync_pol = TCON_IO_NEG,
	.rgb_if.hsync_pol = TCON_IO_NEG,
	.rgb_if.de_pol = TCON_IO_POS,
	.rgb_if.clk_pol = TCON_IO_FALLING,
	.rgb_if.is_clk_ddr = false,

	/* tcon host config */
	/* timing, reference to tcon_timings_t */
	.timing.is_interlace = false,
	.timing.field1.vt = VT,
	.timing.field1.vact = VACT_F1,
	.timing.field1.vfp = VFP_F1,
	.timing.field1.vsw = VSW_F1,
	.timing.field2.vt = VSW_F2 + VBP_F2 + VACT_F2 + VFP_F2,
	.timing.field2.vact = VACT_F2,
	.timing.field2.vfp = VFP_F2,
	.timing.field2.vsw = VSW_F2,
	.timing.hline.ht = HSW + HBP + HACT + HFP,
	.timing.hline.hact = HACT,
	.timing.hline.hfp = HFP,
	.timing.hline.hsw = HSW,
	.timing.dclk_freq = DCLK,
	/* resolution in pixel */
	.timing.width = WIDTH,
	.timing.height = HEIGHT,
	/* format, reference to tcon_output_fmt_t */
	.format = TCON_FMT_RGB666,
};

/* spc_s92048 */
static const struct drm_display_mode spc_s92048_mode = {
	/* dclk_freq */
	.clock = DCLK / 1000,
	/* width */
	.hdisplay = WIDTH,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = WIDTH + HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = WIDTH + HFP + HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = WIDTH + HFP + HSW + HBP,
	/* height */
	.vdisplay = HEIGHT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = HEIGHT + VFP_F1,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = HEIGHT + VFP_F1 + VSW_F1,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = HEIGHT + VFP_F1 + VSW_F1 + VBP_F1,
	.vrefresh = 60,

	.private_size = sizeof(spc_s92048_vo),
	.private = (int *)&spc_s92048_vo,
};

static const struct panel_rgb_desc spc_s92048_desc = {
	.modes = &spc_s92048_mode,
	.num_modes = 1,
	.size = {
		.width = 140,
		.height = 90,
	},
};

static const struct of_device_id panel_rgb_of_table[] = {
	{ .compatible = "panel-rgb", },
	{
		.compatible = "spc,s92048",
		.data = &spc_s92048_desc,
	},
	{ /* Sentinel */ },
};

MODULE_DEVICE_TABLE(of, panel_rgb_of_table);

static int panel_rgb_probe(struct platform_device *pdev)
{
	struct panel_rgb *rgb;
	struct device_node *np = pdev->dev.of_node;
	int ret;
	enum of_gpio_flags flags;
	const struct of_device_id *id;

	if (!of_device_is_available(np))
		return -ENODEV;

	rgb = devm_kzalloc(&pdev->dev, sizeof(*rgb), GFP_KERNEL);
	if (!rgb)
		return -ENOMEM;

	rgb->dev = &pdev->dev;

	id = of_match_node(panel_rgb_of_table, pdev->dev.of_node);
	if (id)
		rgb->desc = id->data;

#if 0
	rgb->supply = devm_regulator_get_optional(rgb->dev, "power");
	if (IS_ERR(rgb->supply)) {
		ret = PTR_ERR(rgb->supply);

		if (ret != -ENODEV) {
			if (ret != -EPROBE_DEFER)
				dev_err(rgb->dev, "failed to request regulator: %d\n",
					ret);
			return ret;
		}

		rgb->supply = NULL;
	}
#endif

	/* Get GPIOs and backlight controller. */
	rgb->enable_gpio = of_get_named_gpio_flags(np,
					"enable_gpio", 0, &flags);
	rgb->enable_gpio_flag = flags;
	if (!gpio_is_valid(rgb->enable_gpio)) {
		dev_err(rgb->dev, "failed to get %s GPIO\n", "enable_gpio");
		rgb->enable_gpio = 0;
	} else {
		DRM_DEBUG_KMS("enable-gpio %d, flags 0x%x\n",
			rgb->enable_gpio, flags);
		ret = devm_gpio_request(rgb->dev,
				rgb->enable_gpio, "enable_gpio");
		if (ret) {
			dev_err(rgb->dev, "enable gpio %d request failed\n",
				rgb->enable_gpio);
			rgb->enable_gpio = 0;
		}
	}

	rgb->bl_gpio = of_get_named_gpio_flags(np,
					"bl_gpio", 0, &flags);
	rgb->bl_gpio_flag = flags;
	if (!gpio_is_valid(rgb->bl_gpio)) {
		dev_err(rgb->dev, "failed to get %s GPIO\n", "bl_gpio");
		rgb->bl_gpio = 0;
	} else {
		DRM_DEBUG_KMS("bl-gpio %d, flags 0x%x\n",
			rgb->bl_gpio, flags);
		ret = devm_gpio_request(rgb->dev,
				rgb->bl_gpio, "bl_gpio");
		if (ret) {
			dev_err(rgb->dev, "bl gpio %d request failed\n",
				rgb->bl_gpio);
			rgb->bl_gpio = 0;
		}
	}

	np = of_parse_phandle(rgb->dev->of_node, "backlight", 0);
	if (np) {
		rgb->backlight = of_find_backlight_by_node(np);
		of_node_put(np);

		if (!rgb->backlight)
			return -EPROBE_DEFER;
	}

	/*
	 * TODO: Handle all power supplies specified in the DT node in a generic
	 * way for panels that don't care about power supply ordering. LVDS
	 * panels that require a specific power sequence will need a dedicated
	 * driver.
	 */

	/* Register the panel. */
	drm_panel_init(&rgb->panel);
	rgb->panel.dev = rgb->dev;
	rgb->panel.funcs = &panel_rgb_funcs;

	ret = drm_panel_add(&rgb->panel);
	if (ret < 0)
		goto error;

	dev_set_drvdata(rgb->dev, rgb);
	return 0;

error:
	put_device(&rgb->backlight->dev);
	return ret;
}

static int panel_rgb_remove(struct platform_device *pdev)
{
	struct panel_rgb *rgb = dev_get_drvdata(&pdev->dev);

	drm_panel_remove(&rgb->panel);

	panel_rgb_disable(&rgb->panel);

	if (rgb->backlight)
		put_device(&rgb->backlight->dev);

	return 0;
}

static struct platform_driver panel_rgb_driver = {
	.probe		= panel_rgb_probe,
	.remove		= panel_rgb_remove,
	.driver		= {
		.name	= "panel-rgb",
		.of_match_table = panel_rgb_of_table,
	},
};

static int __init panel_rgb_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(&panel_rgb_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(panel_rgb_init);

static void __exit panel_rgb_exit(void)
{
	platform_driver_unregister(&panel_rgb_driver);
}
module_exit(panel_rgb_exit);

MODULE_AUTHOR("Laurent Pinchart <laurent.pinchart@ideasonboard.com>");
MODULE_DESCRIPTION("LVDS Panel Driver");
MODULE_LICENSE("GPL");

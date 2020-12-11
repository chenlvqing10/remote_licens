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
#include <linux/pwm.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include "lombo_panel_config.h"

#define WRITEREG32(reg, val)	writel(val, (void __iomem *)reg)
#define READREG32(reg)		readl((void __iomem *)reg)

#define GPIO_ACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)
#define GPIO_INACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)

#define SC500_WIDTH	(480)
#define SC500_HEIGHT	(854)

/* timing for tcon host */
#define SC500_HBP	(10)
#define SC500_HFP	(10)
#define SC500_HSW	(6)
#define SC500_HACT	(SC500_WIDTH)
#define SC500_VBP	(10)
#define SC500_VFP	(14)
#define SC500_VSW	(10)
#define SC500_VACT	(SC500_HEIGHT)
#define SC500_VT	(SC500_VBP + SC500_VFP + SC500_VSW + SC500_VACT)
#define SC500_DCLK	(27000000)

#define DCS_MDELAY_FLAG (0)

/*macro for backlight brightness config*/
#define BRIGHTNESS_MIN_DEF			(0)
#define BRIGHTNESS_MAX_DEF			(100)
#define BRIGHTNESS_DEF				(50)
#define PWM_DUTY_MIN				(0)	/*percentage format*/
#define PWM_DUTY_MAX				(100)	/*percentage format*/

struct panel_brightness_map {
	bool config_pwm;	/*flag for dts already config pwm[]*/
	bool config_brightness;	/*flag for dts already config brightness[]*/
	/*flag for dts already config brightness_dft*/
	bool config_brightness_dft;
	unsigned int brightness_max;
	unsigned int brightness_dft;
	unsigned int pwm_period;
	int n;			/*bl curve break up to n segmentations*/
	/*
	*@config_data[]
	*config_data[0——n-1]:	brightness for each inflection point
	*config_data[n——2n-1]:pwm value for each inflection point
	*/
	unsigned int *config_data;
};

struct panel_sc500fb3101st_desc {
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

struct panel_sc500fb3101st {
	struct drm_panel base;
	bool prepared;
	bool enabled;

	struct device *dev;
	const struct panel_sc500fb3101st_desc *desc;

	struct regulator *supply;

	int enable_gpio;
	enum of_gpio_flags enable_gpio_flag;
	int bl_gpio;
	enum of_gpio_flags bl_gpio_flag;
	struct pwm_device *bl_pwm;
	struct backlight_device *backlight;
	struct panel_brightness_map *brightness_map;
	int ctl_brightness;
};

static inline struct panel_sc500fb3101st *to_panel_sc500fb3101st(
	struct drm_panel *panel)
{
	return container_of(panel, struct panel_sc500fb3101st, base);
}

static int panel_sc500fb3101st_disable(struct drm_panel *panel)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (!p->enabled)
		return 0;

	if (p->backlight) {
		int brightness = p->backlight->props.brightness;

		p->backlight->props.brightness = 0;
		p->backlight->props.power = FB_BLANK_POWERDOWN;
		p->backlight->props.state |= BL_CORE_FBBLANK;
		backlight_update_status(p->backlight);
		p->ctl_brightness = brightness; /* save it */
	}
	if (p->bl_gpio)
		gpio_direction_output(p->bl_gpio,
			GPIO_INACTIVE(p->bl_gpio_flag));

	msleep(110);

	p->enabled = false;

	return 0;
}

static int panel_sc500fb3101st_unprepare(struct drm_panel *panel)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (!p->prepared)
		return 0;

	if (p->enable_gpio)
		gpio_direction_output(p->enable_gpio,
			GPIO_INACTIVE(p->enable_gpio_flag));

	if (p->supply)
		regulator_disable(p->supply);

	msleep(30);

	p->prepared = false;

	return 0;
}

static int sc500fb3101st_mipi_dsi_dcs_write_array(
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

static u8 sc500fb3101st_init_code[] = {
	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x13,

	2, 0xEF, 0x08,

	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x10,

	3, 0xC0, 0xE9, 0x03,

	3, 0xC1, 0x0A, 0x02,

	3, 0xC2, 0x07, 0x08,

	2, 0xCC, 0x10,

	17, 0xB0, 0x00, 0x12, 0x1A, 0x0D, 0x11, 0x07, 0x0C,
	0x0A, 0x09, 0x26, 0x05, 0x11, 0x10, 0x2B, 0x33, 0x1B,

	17, 0xB1, 0x00, 0x12, 0x1A, 0x0D, 0x11, 0x06, 0x0B,
	0x07, 0x08, 0x26, 0x03, 0x11, 0x0F, 0x2B, 0x33, 0x1B,

	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x11,

	2, 0xB0, 0x4D,

	2, 0xB1, 0x1A,

	2, 0xB2, 0x87,

	2, 0xB3, 0x80,

	2, 0xB5, 0x47,

	2, 0xB7, 0x85,

	2, 0xB8, 0x21,

	2, 0xB9, 0x10,

	2, 0xC1, 0x78,

	2, 0xC2, 0x78,

	2, 0xD0, 0x88,

	4, 0xE0, 0x00, 0x00, 0x02,

	12, 0xE1, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00,
	0x00, 0x00, 0x00, 0x20, 0x20,

	14, 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	5, 0xE3, 0x00, 0x00, 0x33, 0x00,

	3, 0xE4, 0x22, 0x00,

	17, 0xE5, 0x04, 0x5C, 0xA0, 0xA0, 0x06, 0x5C, 0xA0,
	0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	5, 0xE6, 0x00, 0x00, 0x33, 0x00,

	3, 0xE7, 0x22, 0x00,

	17, 0xE8, 0x05, 0x5C, 0xA0, 0xA0, 0x07, 0x5C, 0xA0,
	0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	8, 0xEB, 0x02, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00,

	3, 0xEC, 0x00, 0x00,

	17, 0xED, 0xFA, 0x45, 0x0B, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB0, 0x54, 0xAF,

	7, 0xEF, 0x08, 0x08, 0x08, 0x45, 0x3F, 0x54,

	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x13,

	3, 0xE8, 0x00, 0x0E,

	DCS_MDELAY_FLAG, 120,
	1, 0x11,
	DCS_MDELAY_FLAG, 120,

	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x13,

	DCS_MDELAY_FLAG, 10,
	3, 0xE8, 0x00, 0x0C,
	DCS_MDELAY_FLAG, 10,

	3, 0xE8, 0x00, 0x00,

	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x00,

	DCS_MDELAY_FLAG, 120,
	1, 0x29,
	DCS_MDELAY_FLAG, 120,
};

static int panel_sc500fb3101st_prepare(struct drm_panel *panel)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);
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
			GPIO_ACTIVE(p->enable_gpio_flag));
		mdelay(50);
		gpio_direction_output(p->enable_gpio,
			GPIO_INACTIVE(p->enable_gpio_flag));
		mdelay(50);
		gpio_direction_output(p->enable_gpio,
			GPIO_ACTIVE(p->enable_gpio_flag));
		mdelay(50);
	}

	sc500fb3101st_mipi_dsi_dcs_write_array(&p->base,
		sc500fb3101st_init_code,
		ARRAY_SIZE(sc500fb3101st_init_code));
	msleep(150);

	return 0;
}

static int panel_sc500fb3101st_enable(struct drm_panel *panel)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (p->enabled)
		return 0;
	p->enabled = true;

	if (p->desc && p->desc->delay.enable)
		msleep(p->desc->delay.enable);

	if (p->backlight) {
		if (p->backlight->props.brightness == 0)
			p->backlight->props.brightness = p->ctl_brightness;
		p->backlight->props.state &= ~BL_CORE_FBBLANK;
		p->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(p->backlight);
	}

	if (p->bl_gpio)
		gpio_direction_output(p->bl_gpio,
			GPIO_ACTIVE(p->bl_gpio_flag));

	return 0;
}

static int panel_sc500fb3101st_get_fixed_modes(
	struct panel_sc500fb3101st *panel)
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

static int panel_sc500fb3101st_get_modes(struct drm_panel *panel)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);
	int num = 0;

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	/* add hard-coded panel modes */
	num += panel_sc500fb3101st_get_fixed_modes(p);

	return num;
}

static int panel_sc500fb3101st_get_timings(struct drm_panel *panel,
				    unsigned int num_timings,
				    struct display_timing *timings)
{
	DRM_DEBUG_KMS("%s,%d\n", __func__, __LINE__);
	return 0;
}

static int panel_sc500fb3101st_get_mode_private(
	struct drm_panel *panel,
	const struct drm_display_mode *mode,
	void **private)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);

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

static int panel_sc500fb3101st_boot_sync(struct drm_panel *panel,
					int sync_step)
{
	struct panel_sc500fb3101st *p = to_panel_sc500fb3101st(panel);

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

static const struct drm_panel_funcs panel_sc500fb3101st_funcs = {
	.disable = panel_sc500fb3101st_disable,
	.unprepare = panel_sc500fb3101st_unprepare,
	.prepare = panel_sc500fb3101st_prepare,
	.enable = panel_sc500fb3101st_enable,
	.get_modes = panel_sc500fb3101st_get_modes,
	.get_timings = panel_sc500fb3101st_get_timings,
	.get_mode_private = panel_sc500fb3101st_get_mode_private,
	.boot_sync = panel_sc500fb3101st_boot_sync,
};

static int panel_sc500fb3101st_brightness_map(
	struct panel_brightness_map *map, int brightness)
{
	int pwm_duty = 0;
	int pwm_min = PWM_DUTY_MIN;
	int pwm_max = PWM_DUTY_MAX;
	int bri_min = BRIGHTNESS_MIN_DEF;
	int bri_max = BRIGHTNESS_MAX_DEF;

	/*select brightness $ pwm percentage for the inflection*/
	if ((map->config_brightness) && (map->config_pwm)) {
		int i = 0;

		for (i = 1; i < map->n; i++) {
			if (brightness <= *(map->config_data + i)) {
				bri_min = *(map->config_data + i - 1);
				bri_max = *(map->config_data + i);
				pwm_min = *(map->config_data + i - 1 + map->n);
				pwm_max = *(map->config_data + i + map->n);
				break;
			}
		}
	} else if (map->config_brightness) {
		pwm_min = PWM_DUTY_MIN;
		pwm_max = PWM_DUTY_MAX;
		bri_min = *(map->config_data);
		bri_max = *(map->config_data + map->n - 1);
	} else if (map->config_pwm) {
		bri_min = BRIGHTNESS_MIN_DEF;
		bri_max = BRIGHTNESS_MAX_DEF;
		pwm_min = *(map->config_data + map->n);
		pwm_max = *(map->config_data + (2 * map->n - 1));
	} else {
		pwm_min = PWM_DUTY_MIN;
		pwm_max = PWM_DUTY_MAX;
		bri_min = BRIGHTNESS_MIN_DEF;
		bri_max = BRIGHTNESS_MAX_DEF;
	}
	DRM_DEBUG_KMS("bri_min:%d bri_max:%d pwm_min:%d pwm_max:%d\n",
		bri_min, bri_max, pwm_min, pwm_max);

	/*pwm_min & pwm_max change to duty count format from percentage format*/
	pwm_min = map->pwm_period * pwm_min / 100;
	pwm_max = map->pwm_period * pwm_max / 100;

	pwm_duty = (brightness - bri_min) * (pwm_max - pwm_min) /
				(bri_max - bri_min) + pwm_min;

	return pwm_duty;
}

static int panel_sc500fb3101st_backlight_update_status(
	struct backlight_device *bl)
{
	struct panel_sc500fb3101st *panel = bl_get_data(bl);
	struct pwm_device *bl_pwm = panel->bl_pwm;
	int brightness = bl->props.brightness;
	int period_ns = pwm_get_period(bl_pwm);

	if (brightness > 0) {
		int duty_cycle = panel_sc500fb3101st_brightness_map(
			panel->brightness_map, brightness);

		DRM_DEBUG_KMS("pwm_duty_ns:%d\n", duty_cycle);
		panel->ctl_brightness = brightness;

		pwm_config(bl_pwm, duty_cycle, period_ns);
		pwm_enable(bl_pwm);
	} else {
		pwm_config(bl_pwm, 1, period_ns);
	/*	pwm_disable(bl_pwm);	*/
	}
	DRM_DEBUG_KMS("bri_dft:%d, bri:%d\n",
		panel->brightness_map->brightness_dft, bl->props.brightness);

	return 0;
}

static int panel_sc500fb3101st_backlight_get_brightness(
	struct backlight_device *bl)
{
	return bl->props.brightness;
}

static const struct backlight_ops panel_sc500fb3101st_backlight_ops = {
	.update_status	= panel_sc500fb3101st_backlight_update_status,
	.get_brightness	= panel_sc500fb3101st_backlight_get_brightness,
};

static int panel_sc500fb3101st_probe(struct device *dev,
	const struct panel_sc500fb3101st_desc *desc)
{
	struct device_node *np = dev->of_node;
	struct panel_sc500fb3101st *panel;
	struct panel_sc500fb3101st_desc *of_desc;
	struct mipi_dsi_device *dsi_device;
	enum of_gpio_flags flags;
	struct panel_brightness_map *of_brightness;
	u32 val;
	u32 *data = NULL;
	int err, n, i;

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;
	dsi_device = devm_kzalloc(dev, sizeof(*dsi_device), GFP_KERNEL);
	if (!dsi_device)
		return -ENOMEM;
	dsi_device->channel = 0;
	panel->base.dsi = dsi_device;

	of_brightness = devm_kzalloc(dev, sizeof(of_brightness), GFP_KERNEL);
	if (!of_brightness)
		return -ENOMEM;
	memset(of_brightness, 0, sizeof(struct panel_brightness_map));

	n = of_property_count_u32_elems(dev->of_node, "bl_brightness");
	DRM_DEBUG_KMS("[%s] [%d] [%d]\n", __func__, __LINE__, n);
	if (n <= 0) {
		n = of_property_count_u32_elems(dev->of_node, "bl_pwm_duty");
		DRM_DEBUG_KMS("[%s] [%d] [%d]\n", __func__, __LINE__, n);
	}

	if (n > 0) {
		DRM_DEBUG_KMS("alloc mem for bl data len: %d\n", n);
		data = devm_kzalloc(dev, 2 * n * sizeof(u32), GFP_KERNEL);
		if (!data)
			return -ENOSYS;
		memset(data, 0, 2 * n * sizeof(u32));

		of_brightness->n = n;
		if (!of_property_read_u32_array(dev->of_node,
			"bl_brightness", data, n)) {
			of_brightness->config_brightness = true;
			of_brightness->brightness_max = *(data + (n - 1));
		} else {
			of_brightness->config_brightness = false;
			of_brightness->brightness_max = BRIGHTNESS_MAX_DEF;
		}

		if (!of_property_read_u32_array(dev->of_node,
			"bl_pwm_duty", data + n, n))
			of_brightness->config_pwm = true;
		else
			of_brightness->config_pwm = false;
	} else {
		DRM_DEBUG_KMS("not cfg bl param:line %d n=%d\n", __LINE__, n);
		of_brightness->config_brightness = false;
		of_brightness->config_pwm = false;
		of_brightness->brightness_max = BRIGHTNESS_MAX_DEF;
		of_brightness->n = 0;
	}

	if (!of_property_read_u32(dev->of_node, "brightness_dft", &val)) {
		of_brightness->brightness_dft = val;
		of_brightness->config_brightness_dft = true;
	} else {
		of_brightness->brightness_dft = BRIGHTNESS_DEF;
		of_brightness->config_brightness_dft = false;
	}
	of_brightness->config_data = data;
	for (i = 0; i < n; i++)
		DRM_DEBUG_KMS("idx %d: brightness:%d pwm:%d\n",
			i, *(data+i), *(data+n+i));

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

	panel->bl_pwm = of_pwm_get(np, NULL);
	if (IS_ERR_OR_NULL(panel->bl_pwm)) {
		DRM_INFO("no bl pwm\n");
		panel->bl_pwm = NULL;
	} else {
		struct backlight_properties props;

		DRM_INFO("#get pwm ok\n");

		of_brightness->pwm_period = pwm_get_period(panel->bl_pwm);
		memset(&props, 0, sizeof(struct backlight_properties));
		props.type = BACKLIGHT_RAW;
		props.max_brightness = of_brightness->brightness_max;
		props.brightness = of_brightness->brightness_dft;
		panel->backlight = backlight_device_register(
			"pwm-backlight",
			dev, panel,
			&panel_sc500fb3101st_backlight_ops,
			&props);
		if (IS_ERR_OR_NULL(panel->backlight)) {
			DRM_INFO("backlight_device_register failed\n");
			panel->backlight = NULL;
		} else {
			panel->ctl_brightness = of_brightness->brightness_dft;
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
	panel->brightness_map = of_brightness;
	panel->dev = dev;

	drm_panel_init(&panel->base);
	panel->base.dev = dev;
	panel->base.funcs = &panel_sc500fb3101st_funcs;

	err = drm_panel_add(&panel->base);
	if (err < 0)
		goto err_out;

	dev_set_drvdata(dev, panel);

	DRM_DEBUG_KMS("[%d] over.\n", __LINE__);

	return 0;

err_out:
	return err;
}

static int panel_sc500fb3101st_remove(
	struct platform_device *pdev)
{
	struct panel_sc500fb3101st *panel = dev_get_drvdata(&pdev->dev);

	drm_panel_detach(&panel->base);
	drm_panel_remove(&panel->base);

	panel_sc500fb3101st_disable(&panel->base);
	panel_sc500fb3101st_unprepare(&panel->base);

	if (panel->backlight)
		put_device(&panel->backlight->dev);

	kfree(panel->desc);

	return 0;
}

static void panel_sc500fb3101st_shutdown(
	struct platform_device *pdev)
{
	struct panel_sc500fb3101st *panel = dev_get_drvdata(&pdev->dev);

	panel_sc500fb3101st_disable(&panel->base);
	panel_sc500fb3101st_unprepare(&panel->base);
}

static struct lombo_vo_dev_config sc500fb3101st_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.is_dual = false,
		.is_bta = false,
		.lane_num = 2,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_PULSE,
		.is_bitrate_fixed = false,
		.bit_clk = 549000000,
	},

	.timing.is_interlace = false,
	.timing.field1.vt = SC500_VT,
	.timing.field1.vact = SC500_VACT,
	.timing.field1.vfp = SC500_VFP,
	.timing.field1.vsw = SC500_VSW,
	.timing.hline.ht = SC500_HSW + SC500_HBP + SC500_HACT + SC500_HFP,
	.timing.hline.hact = SC500_HACT,
	.timing.hline.hfp = SC500_HFP,
	.timing.hline.hsw = SC500_HSW,
	.timing.width = SC500_HACT,
	.timing.height = SC500_VACT,
	.timing.dclk_freq = SC500_DCLK,
	.format = TCON_FMT_RGB888,
};

/* mipi dsi 1080x1920 LCD test */
static const struct drm_display_mode sc500fb3101st_mode = {
	/* dclk_freq */
	.clock = SC500_DCLK / 1000,
	/* width */
	.hdisplay = SC500_HACT,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = SC500_HACT + SC500_HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = SC500_HACT + SC500_HFP + SC500_HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = SC500_HACT + SC500_HFP + SC500_HSW + SC500_HBP,
	/* height */
	.vdisplay = SC500_VACT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = SC500_VACT + SC500_VFP,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = SC500_VACT + SC500_VFP + SC500_VSW,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = SC500_VACT + SC500_VFP + SC500_VSW + SC500_VBP,
	.vrefresh = 60,

	.private_size = sizeof(sc500fb3101st_vo),
	.private = (int *)&sc500fb3101st_vo,
};

static const struct panel_sc500fb3101st_desc sc500fb3101st_desc = {
	.modes = &sc500fb3101st_mode,
	.num_modes = 1,
	.bpc = 6,
	.size = {
		.width = 60,
		.height = 120,
	},
};

static const struct of_device_id panel_sc500fb3101st_of_match[] = {
	{
		.compatible = "sc500fb3101st",
		.data = &sc500fb3101st_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, panel_sc500fb3101st_of_match);

static int panel_sc500fb3101st_pre_probe(
	struct platform_device *pdev)
{
	const struct of_device_id *id;

	if (!of_device_is_available(pdev->dev.of_node))
		return -ENODEV;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	id = of_match_node(panel_sc500fb3101st_of_match,
				pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return panel_sc500fb3101st_probe(&pdev->dev, id->data);
}

static struct platform_driver panel_sc500fb3101st_platform_driver = {
	.driver = {
		.name = "panel-sc500fb3101st",
		.of_match_table = panel_sc500fb3101st_of_match,
	},
	.probe = panel_sc500fb3101st_pre_probe,
	.remove = panel_sc500fb3101st_remove,
	.shutdown = panel_sc500fb3101st_shutdown,
};

static int __init panel_sc500fb3101st_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(
		&panel_sc500fb3101st_platform_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(panel_sc500fb3101st_init);

static void __exit panel_sc500fb3101st_exit(void)
{
	platform_driver_unregister(&panel_sc500fb3101st_platform_driver);
}
module_exit(panel_sc500fb3101st_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("DRM Driver for Panels sc500fb3101st");
MODULE_LICENSE("GPL and additional rights");

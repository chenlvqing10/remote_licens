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
#include "../lombo/csp_dsi/csp_doss_mipi_dsi.h"

#define WRITEREG32(reg, val)	writel(val, (void __iomem *)reg)
#define READREG32(reg)		readl((void __iomem *)reg)

#define ICN6211_LANE_NUM	(4)

#define ICN6211_WIDTH		(480)
#define ICN6211_HEIGHT		(272)

#define ICN6211_HBP		(18)
#define ICN6211_HFP		(18)
#define ICN6211_HSW		(9)
#define ICN6211_HACT		(ICN6211_WIDTH)

#define ICN6211_VBP_F1		(6)
#define ICN6211_VFP_F1		(6)
#define ICN6211_VSW_F1		(3)
#define ICN6211_VACT_F1		(ICN6211_HEIGHT)

#define ICN6211_VBP_F2		(0)
#define ICN6211_VFP_F2		(0)
#define ICN6211_VSW_F2		(0)
#define ICN6211_VACT_F2		(0)

#define ICN6211_DCLK		(9000000)
#define ICN6211_DCS_MDELAY_FLAG (0)

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

struct panel_icn6211_desc {
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

struct panel_icn6211 {
	struct drm_panel base;
	bool prepared;
	bool enabled;

	struct device *dev;
	const struct panel_icn6211_desc *desc;

	struct regulator *supply;

	int bl_gpio;
	int reset_gpio;
	int power_gpio;
	struct pwm_device *bl_pwm;
	struct backlight_device *backlight;
	struct panel_brightness_map *brightness_map;
	int ctl_brightness;
};

/* only 1 i2c addr */
static struct i2c_client *panel_icn6211_i2c_client[1];

u8 icn6211_init_code[] = {
	3, 0x58, 0x20, 0xE0,
	3, 0x58, 0x21, 0x10,
	3, 0x58, 0x22, 0x11,
	3, 0x58, 0x23, 0x12,
	3, 0x58, 0x24, 0x09,
	3, 0x58, 0x25, 0x12,
	3, 0x58, 0x26, 0x00,
	3, 0x58, 0x27, 0x06,
	3, 0x58, 0x28, 0x03,
	3, 0x58, 0x29, 0x06,
	3, 0x58, 0x34, 0x80,
	3, 0x58, 0x36, 0x12,
	3, 0x58, 0xB5, 0xA0,
	3, 0x58, 0x5C, 0xFF,
	3, 0x58, 0x2A, 0x01,
	3, 0x58, 0x56, 0x92,
	3, 0x58, 0x6B, 0x61,
	3, 0x58, 0x69, 0x16,
	3, 0x58, 0x10, 0x40,
	3, 0x58, 0x11, 0x88,
	3, 0x58, 0xB6, 0x20,
	3, 0x58, 0x51, 0x20,
	3, 0x58, 0x09, 0x10,
};

static struct i2c_client *panel_icn6211_get_i2c_client(
					const unsigned short i2c_addr)
{
	const __u32 num = ARRAY_SIZE(panel_icn6211_i2c_client);
	__u32 i;

	for (i = 0; i < num; i++) {
		struct i2c_client *client = panel_icn6211_i2c_client[i];

		if (client && (client->addr == i2c_addr))
			return client;
	}

	return NULL;
}

s32 panel_icn6211_i2c_write_reg_array(
				u32 tidx, u8 *reg, u32 len)
{
	u32 i = 0;

	while (i < len) {
		if (ICN6211_DCS_MDELAY_FLAG == reg[i]) {
			mdelay(reg[i + 1]);
			i += reg[i] + 2;
		} else {
			char buf[3];
			struct i2c_client *client =
				panel_icn6211_get_i2c_client(reg[i + 1] >> 1);

			if (client) {
				buf[0] = reg[i + 2];	/* reg addr */
				buf[1] = reg[i + 3];	/* reg value */

				if (2 != i2c_master_send(client, buf, 2)) {
					DRM_ERROR("%d\n", __LINE__);
					return -1;
				}
			} else {
				pr_err("%d: null i2c_client @%x\n",
					__LINE__, reg[i + 1] >> 1);
			}
			i += reg[i] + 1;
		}
	}

	return 0;
}

static inline struct panel_icn6211 *to_panel_icn6211(
	struct drm_panel *panel)
{
	return container_of(panel, struct panel_icn6211, base);
}

static int panel_icn6211_disable(struct drm_panel *panel)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);

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
		gpio_direction_output(p->bl_gpio, 0);

	if (p->desc && p->desc->delay.disable)
		msleep(p->desc->delay.disable);

	p->enabled = false;

	return 0;
}

static int panel_icn6211_unprepare(struct drm_panel *panel)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (!p->prepared)
		return 0;

	if (p->reset_gpio)
		gpio_direction_output(p->reset_gpio, 0);

	if (p->power_gpio)
		gpio_direction_output(p->power_gpio, 0);

	if (p->supply)
		regulator_disable(p->supply);

	if (p->desc && p->desc->delay.unprepare)
		msleep(p->desc->delay.unprepare);

	p->prepared = false;

	return 0;
}

static int panel_icn6211_prepare(struct drm_panel *panel)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);
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
			return err;
		}
	}

	if (p->power_gpio) {
		gpio_direction_output(p->power_gpio, 1);
		msleep(100);
	}

	if (p->reset_gpio) {
		gpio_direction_output(p->reset_gpio, 1);
		msleep(50);
		gpio_direction_output(p->reset_gpio, 0);
		msleep(50);
		gpio_direction_output(p->reset_gpio, 1);
		msleep(150);
	}
	if (p->desc && p->desc->delay.prepare)
		msleep(p->desc->delay.prepare);

	return 0;
}

static int panel_icn6211_enable(struct drm_panel *panel)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	msleep(100);
	panel_icn6211_i2c_write_reg_array(3, icn6211_init_code,
					  ARRAY_SIZE(icn6211_init_code));

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
		gpio_direction_output(p->bl_gpio, 1);

	return 0;
}

static int panel_icn6211_get_fixed_modes(
	struct panel_icn6211 *panel)
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

static int panel_icn6211_get_modes(struct drm_panel *panel)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);
	int num = 0;

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	/* add hard-coded panel modes */
	num += panel_icn6211_get_fixed_modes(p);

	return num;
}

static int panel_icn6211_get_timings(struct drm_panel *panel,
				    unsigned int num_timings,
				    struct display_timing *timings)
{
	DRM_DEBUG_KMS("%s,%d\n", __func__, __LINE__);
	return 0;
}

static int panel_icn6211_get_mode_private(
	struct drm_panel *panel,
	const struct drm_display_mode *mode,
	void **private)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);

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

static int panel_icn6211_boot_sync(struct drm_panel *panel,
					int sync_step)
{
	struct panel_icn6211 *p = to_panel_icn6211(panel);

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

static int panel_icn6211_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	__u32 i;

	if (!client) {
		DRM_ERROR("%d: null i2c_client\n", __LINE__);
		return -1;
	}

	panel_icn6211_i2c_client[0] = client;
	for (i = 1; i < ARRAY_SIZE(panel_icn6211_i2c_client); i++) {
		panel_icn6211_i2c_client[i] = i2c_new_dummy(client->adapter,
					client->addr + i);
		if (!panel_icn6211_i2c_client[i]) {
			dev_err(&client->adapter->dev,
				"address 0x%02x unavailable\n",
				client->addr + i);
			return -EADDRINUSE;
		} else {
			dev_info(&client->adapter->dev,
				"i2c_new_dummy 0x%02x 0x%02x ok\n",
				client->addr + i,
				panel_icn6211_i2c_client[i]->addr);
		}
	}

	dev_info(&client->adapter->dev,
		"attached panel icn6211 into i2c adapter successfully\n");
	return 0;
}

static int panel_icn6211_i2c_remove(struct i2c_client *client)
{
	dev_info(&client->adapter->dev,
		"detached panel icn6211 from i2c adapter successfully\n");

	return 0;
}

static const struct i2c_device_id panel_icn6211_i2c_id[] = {
	{ "icn6211_i2c", 0 },
	{ },
};

static struct of_device_id panel_icn6211_i2c_match_types[] = {
	{
		.compatible = "chiphon,icn6211_i2c",
	}, {
		/* end node */
	}
};

struct i2c_driver panel_icn6211_i2c_driver = {
	.driver = {
		.name = "icn6211_i2c",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(panel_icn6211_i2c_match_types),
	},
	.id_table = panel_icn6211_i2c_id,
	.probe = panel_icn6211_i2c_probe,
	.remove = panel_icn6211_i2c_remove,
	.command = NULL,
};

static const struct drm_panel_funcs panel_icn6211_funcs = {
	.disable = panel_icn6211_disable,
	.unprepare = panel_icn6211_unprepare,
	.prepare = panel_icn6211_prepare,
	.enable = panel_icn6211_enable,
	.get_modes = panel_icn6211_get_modes,
	.get_timings = panel_icn6211_get_timings,
	.get_mode_private = panel_icn6211_get_mode_private,
	.boot_sync = panel_icn6211_boot_sync,
};

static int panel_icn6211_brightness_map(
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

static int panel_icn6211_backlight_update_status(
	struct backlight_device *bl)
{
	struct panel_icn6211 *panel = bl_get_data(bl);
	struct pwm_device *bl_pwm = panel->bl_pwm;
	int brightness = bl->props.brightness;
	int period_ns = pwm_get_period(bl_pwm);

	if (brightness > 0) {
		int duty_cycle = panel_icn6211_brightness_map(
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

static int panel_icn6211_backlight_get_brightness(
	struct backlight_device *bl)
{
	return bl->props.brightness;
}

static const struct backlight_ops panel_icn6211_backlight_ops = {
	.update_status	= panel_icn6211_backlight_update_status,
	.get_brightness	= panel_icn6211_backlight_get_brightness,
};

static int panel_icn6211_probe(struct device *dev,
			       const struct panel_icn6211_desc *desc)
{
	struct device_node *np = dev->of_node;
	struct panel_icn6211 *panel;
	struct panel_icn6211_desc *of_desc;
	struct mipi_dsi_device *dsi_device;
	enum of_gpio_flags flags;
	struct panel_brightness_map *of_brightness;
	u32 val;
	u32 *data = NULL;
	int err, n, i;

	dev_dbg(dev, "%s,%d\n", __FILE__, __LINE__);

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;
	dsi_device = devm_kzalloc(dev, sizeof(*dsi_device), GFP_KERNEL);
	if (!dsi_device)
		return -ENOMEM;
	dsi_device->channel = 0;
	panel->base.dsi = dsi_device;

	if (i2c_add_driver(&panel_icn6211_i2c_driver)) {
		DRM_ERROR("failed to register panel i2c driver\n");
		return -ENOENT;
	}

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

	panel->power_gpio = of_get_named_gpio_flags(
		np, "power_gpio", 0, &flags);
	DRM_DEBUG_KMS("power_gpio %d, flags 0x%x\n", panel->power_gpio, flags);
	if (!gpio_is_valid(panel->power_gpio)) {
		DRM_ERROR("power_gpio %d invalid\n", panel->power_gpio);
		panel->power_gpio = 0;
	} else {
		err = devm_gpio_request(dev, panel->power_gpio, "power_gpio");
		if (err < 0) {
			DRM_ERROR("power_gpio %d request failed\n",
				panel->power_gpio);
			panel->power_gpio = 0;
		}
	}

	panel->reset_gpio = of_get_named_gpio_flags(
		np, "reset_gpio", 0, &flags);
	DRM_DEBUG_KMS("reset_gpio %d, flags 0x%x\n", panel->reset_gpio, flags);
	if (!gpio_is_valid(panel->reset_gpio)) {
		DRM_ERROR("reset_gpio %d invalid\n", panel->reset_gpio);
		panel->reset_gpio = 0;
	} else {
		err = devm_gpio_request(dev, panel->reset_gpio, "reset_gpio");
		if (err < 0) {
			DRM_ERROR("reset_gpio %d request failed\n",
				panel->reset_gpio);
			panel->reset_gpio = 0;
		}
	}

	panel->bl_gpio = of_get_named_gpio_flags(
		np, "bl_gpio", 0, &flags);
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
			&panel_icn6211_backlight_ops,
			&props);
		if (IS_ERR_OR_NULL(panel->backlight)) {
			DRM_INFO("backlight_device_register failed\n");
			panel->backlight = NULL;
		} else {
			panel->ctl_brightness = of_brightness->brightness_dft;
		}
	}

	if (!desc)
		of_desc = devm_kzalloc(dev, sizeof(*of_desc), GFP_KERNEL);
	else
		of_desc = devm_kmemdup(dev, desc, sizeof(*of_desc), GFP_KERNEL);

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
	panel->base.funcs = &panel_icn6211_funcs;

	err = drm_panel_add(&panel->base);
	if (err < 0)
		goto err_out;

	dev_set_drvdata(dev, panel);

	DRM_DEBUG_KMS("[%d] over.\n", __LINE__);

	return 0;

err_out:
	return err;
}

static int panel_icn6211_remove(struct platform_device *pdev)
{
	struct panel_icn6211 *panel = dev_get_drvdata(&pdev->dev);

	drm_panel_detach(&panel->base);
	drm_panel_remove(&panel->base);

	panel_icn6211_disable(&panel->base);
	panel_icn6211_unprepare(&panel->base);

	if (panel->backlight)
		put_device(&panel->backlight->dev);

	return 0;
}

static void panel_icn6211_shutdown(struct platform_device *pdev)
{
	struct panel_icn6211 *panel = dev_get_drvdata(&pdev->dev);

	panel_icn6211_disable(&panel->base);
	panel_icn6211_unprepare(&panel->base);
}

static struct lombo_vo_dev_config icn6211_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.index = 0,
		.is_dual = false,
		.is_bta = false,
		.lane_num = 4,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_PULSE,
		.is_bitrate_fixed = false,
		.bit_clk = 999000000,
	},

	.timing.is_interlace = false,
	.timing.field1.vt = ICN6211_VBP_F1 + ICN6211_VFP_F1 +
					ICN6211_VSW_F1 + ICN6211_VACT_F1,
	.timing.field1.vact = ICN6211_VACT_F1,
	.timing.field1.vfp = ICN6211_VFP_F1,
	.timing.field1.vsw = ICN6211_VSW_F1,
	.timing.field2.vt = ICN6211_VBP_F2 + ICN6211_VFP_F2 +
					ICN6211_VSW_F2 + ICN6211_VACT_F2,
	.timing.field2.vact = ICN6211_VACT_F2,
	.timing.field2.vfp = ICN6211_VFP_F2,
	.timing.field2.vsw = ICN6211_VSW_F2,
	.timing.hline.ht = ICN6211_HSW + ICN6211_HBP +
				ICN6211_HACT + ICN6211_HFP,
	.timing.hline.hact = ICN6211_HACT,
	.timing.hline.hfp = ICN6211_HFP,
	.timing.hline.hsw = ICN6211_HSW,
	.timing.width = ICN6211_WIDTH,
	.timing.height = ICN6211_HEIGHT,
	.timing.dclk_freq = ICN6211_DCLK,
	.format = TCON_FMT_RGB888,
};

static const struct drm_display_mode icn6211_mode = {
	/* dclk_freq */
	.clock = ICN6211_DCLK / 1000,
	/* width */
	.hdisplay = ICN6211_WIDTH,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = ICN6211_WIDTH + ICN6211_HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = ICN6211_WIDTH + ICN6211_HFP + ICN6211_HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = ICN6211_WIDTH + ICN6211_HFP + ICN6211_HSW + ICN6211_HBP,
	/* height */
	.vdisplay = ICN6211_HEIGHT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = ICN6211_HEIGHT + ICN6211_VFP_F1,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = ICN6211_HEIGHT + ICN6211_VFP_F1 + ICN6211_VSW_F1,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = ICN6211_HEIGHT + ICN6211_VFP_F1 +
			ICN6211_VSW_F1 + ICN6211_VBP_F1,
	.vrefresh = 60,

	.private_size = sizeof(icn6211_vo),
	.private = (int *)&icn6211_vo,
};

static const struct panel_icn6211_desc icn6211_desc = {
	.modes = &icn6211_mode,
	.num_modes = 1,
	.bpc = 6,
	.size = {
		.width = 69,
		.height = 123,
	},
};

static const struct of_device_id panel_icn6211_of_match[] = {
	{
		.compatible = "chipone,panel_icn6211",
		.data = &icn6211_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, panel_icn6211_of_match);

static int panel_icn6211_pre_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;

	if (!of_device_is_available(pdev->dev.of_node))
		return -ENODEV;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	id = of_match_node(panel_icn6211_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return panel_icn6211_probe(&pdev->dev, id->data);
}

static struct platform_driver panel_icn6211_platform_driver = {
	.driver = {
		.name = "panel-icn6211",
		.of_match_table = panel_icn6211_of_match,
	},
	.probe = panel_icn6211_pre_probe,
	.remove = panel_icn6211_remove,
	.shutdown = panel_icn6211_shutdown,
};

static int __init panel_icn6211_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(&panel_icn6211_platform_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(panel_icn6211_init);

static void __exit panel_icn6211_exit(void)
{
	platform_driver_unregister(&panel_icn6211_platform_driver);
}
module_exit(panel_icn6211_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("DRM Driver for Panels icn6211");
MODULE_LICENSE("GPL and additional rights");

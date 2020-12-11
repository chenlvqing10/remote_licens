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
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <uapi/linux/media-bus-format.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include "../lombo_legacy/lombo_doss.h"

/*
 * @INTERFACE
 * it can config value below, for reference only.
 * VO_DEV_PRGB, VO_DEV_SRGB_RGB, VO_DEV_SRGB_DUMMY_RGB, VO_DEV_SRGB_RGB_DUMMY,
 * VO_DEV_CPU_18BIT_RGB666,
 * VO_DEV_CPU_16BIT_RGB888, VO_DEV_CPU_16BIT_RGB666, VO_DEV_CPU_16BIT_RGB565,
 * VO_DEV_CPU_9BIT_RGB666,
 * VO_DEV_CPU_8BIT_RGB666, VO_DEV_CPU_8BIT_RGB565, VO_DEV_CPU_8BIT_RGB888,
 * VO_DEV_LVDS, VO_DEV_MIPI_DSI_VIDEO, VO_DEV_MIPI_DSI_CMD,
 * VO_DEV_BT601_24BIT, VO_DEV_BT601_16BIT, VO_DEV_BT601_8BIT,
 * VO_DEV_BT656_8BIT,
 * VO_DEV_BT1120_16BIT,VO_DEV_BT1120_8BIT
 * VO_DEV_TVE
 */
#define INTERFACE	(VO_DEV_PRGB)
#define VSYNC_POL	(TCON_IO_NEG)
#define HSYNC_POL	(TCON_IO_NEG)
#define DE_POL		(TCON_IO_POS)
#define CLK_POL		(TCON_IO_FALLING)
#define IS_CLK_DDR	(FALSE)

/*
 * TCON COMMON CONFIGURATION
 *
 * @TCON_FMT
 * it can config value below, for reference only.
 * TCON_FMT_RGB888, TCON_FMT_RGB666, TCON_FMT_RGB565
 * TCON_FMT_YUV444, TCON_FMT_YUV422,
 */
#define TCON_FMT	(TCON_FMT_RGB666)
#define IS_INTERLACE	(FALSE)
#define GAMMA_EN	(FALSE)
#define FRC_EN		(TRUE)
#define CSC_EN		(FALSE)
#define HAS_TE		(FALSE)
#define TE_POL		(TCON_IO_POS)

/* DOSS TOP IO Config */
#define DCS_MDELAY_FLAG (0)
#define PWM_IO		(0)
#define RESET_IO	(1)
#define IO_HIGH		(1)
#define IO_LOW		(0)

struct panel_desc {
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

struct panel_simple {
	struct drm_panel base;
	bool prepared;
	bool enabled;

	struct device *dev;
	const struct panel_desc *desc;

	struct backlight_device *backlight;
	struct regulator *supply;
	struct i2c_adapter *ddc;

	unsigned enable_gpio;
};

static u32 gamma_tbl[256] = {
/* red   power = 2.2 */
/* green power = 1/2.2 */
/* blue  power = 1.4 */
0x000000, 0x001400, 0x001c00, 0x002100, 0x002600, 0x002a01, 0x002e01, 0x003101,
0x003402, 0x003702, 0x003a02, 0x003d03, 0x003f03, 0x004203, 0x004404, 0x004604,
0x004805, 0x004a05, 0x004c06, 0x004e06, 0x005007, 0x015207, 0x015308, 0x015508,
0x015709, 0x015809, 0x015a0a, 0x015c0a, 0x015d0b, 0x025f0c, 0x02600c, 0x02620d,
0x02630d, 0x02640e, 0x03660f, 0x03670f, 0x036810, 0x036a11, 0x036b11, 0x046c12,
0x046e13, 0x046f13, 0x047014, 0x057115, 0x057215, 0x057416, 0x057517, 0x067617,
0x067718, 0x067819, 0x07791a, 0x077a1a, 0x077c1b, 0x087d1c, 0x087e1c, 0x087f1d,
0x09801e, 0x09811f, 0x098220, 0x0a8320, 0x0a8421, 0x0a8522, 0x0b8623, 0x0b8723,
0x0c8824, 0x0c8925, 0x0c8a26, 0x0d8b27, 0x0d8c28, 0x0e8d28, 0x0e8d29, 0x0f8e2a,
0x0f8f2b, 0x10902c, 0x10912d, 0x11922d, 0x11932e, 0x12942f, 0x129530, 0x139631,
0x139632, 0x149733, 0x149834, 0x159934, 0x169a35, 0x169b36, 0x179b37, 0x179c38,
0x189d39, 0x199e3a, 0x199f3b, 0x1a9f3c, 0x1aa03d, 0x1ba13e, 0x1ca23e, 0x1ca33f,
0x1da340, 0x1ea441, 0x1ea542, 0x1fa643, 0x20a644, 0x21a745, 0x21a846, 0x22a947,
0x23a948, 0x24aa49, 0x24ab4a, 0x25ac4b, 0x26ac4c, 0x27ad4d, 0x27ae4e, 0x28af4f,
0x29af50, 0x2ab051, 0x2bb152, 0x2cb153, 0x2cb254, 0x2db355, 0x2eb456, 0x2fb457,
0x30b558, 0x31b659, 0x32b65a, 0x33b75b, 0x33b85c, 0x34b85d, 0x35b95e, 0x36ba5f,
0x37ba61, 0x38bb62, 0x39bc63, 0x3abc64, 0x3bbd65, 0x3cbe66, 0x3dbe67, 0x3ebf68,
0x3fc069, 0x40c06a, 0x41c16b, 0x42c16c, 0x43c26d, 0x44c36f, 0x46c370, 0x47c471,
0x48c572, 0x49c573, 0x4ac674, 0x4bc675, 0x4cc776, 0x4dc877, 0x4ec879, 0x50c97a,
0x51c97b, 0x52ca7c, 0x53cb7d, 0x54cb7e, 0x56cc7f, 0x57cc81, 0x58cd82, 0x59ce83,
0x5bce84, 0x5ccf85, 0x5dcf86, 0x5ed088, 0x60d189, 0x61d18a, 0x62d28b, 0x64d28c,
0x65d38d, 0x66d38f, 0x68d490, 0x69d591, 0x6ad592, 0x6cd693, 0x6dd695, 0x6ed796,
0x70d797, 0x71d898, 0x73d999, 0x74d99b, 0x75da9c, 0x77da9d, 0x78db9e, 0x7adba0,
0x7bdca1, 0x7ddca2, 0x7edda3, 0x80dda4, 0x81dea6, 0x83dfa7, 0x84dfa8, 0x86e0a9,
0x87e0ab, 0x89e1ac, 0x8be1ad, 0x8ce2ae, 0x8ee2b0, 0x8fe3b1, 0x91e3b2, 0x93e4b3,
0x94e4b5, 0x96e5b6, 0x98e5b7, 0x99e6b9, 0x9be6ba, 0x9de7bb, 0x9ee7bc, 0xa0e8be,
0xa2e8bf, 0xa3e9c0, 0xa5e9c2, 0xa7eac3, 0xa9eac4, 0xaaebc5, 0xacebc7, 0xaeecc8,
0xb0ecc9, 0xb1edcb, 0xb3edcc, 0xb5eecd, 0xb7eecf, 0xb9efd0, 0xbbefd1, 0xbcf0d3,
0xbef0d4, 0xc0f1d5, 0xc2f1d7, 0xc4f2d8, 0xc6f2d9, 0xc8f3db, 0xcaf3dc, 0xccf4dd,
0xcef4df, 0xd0f5e0, 0xd2f5e1, 0xd4f6e3, 0xd6f6e4, 0xd8f7e5, 0xdaf7e7, 0xdcf8e8,
0xdef8e9, 0xe0f9eb, 0xe2f9ec, 0xe4faed, 0xe6faef, 0xe8faf0, 0xeafbf2, 0xecfbf3,
0xeefcf4, 0xf0fcf6, 0xf2fdf7, 0xf5fdf9, 0xf7fefa, 0xf9fefb, 0xfbfffd, 0xfdfffe,
};

/* Example: Parallel RGB LCD Panel */
static void tcon_dev_config(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	/* device interface, reference to vo_dev_if_t */
	vo_dev->dev_if = INTERFACE;
	/* device specific configuation, reference to vo_if_cfg_t */
	vo_dev->if_cfg.rgb_if.vsync_pol = VSYNC_POL;
	vo_dev->if_cfg.rgb_if.hsync_pol = HSYNC_POL;
	vo_dev->if_cfg.rgb_if.de_pol = DE_POL;
	vo_dev->if_cfg.rgb_if.clk_pol = CLK_POL;
	vo_dev->if_cfg.rgb_if.is_clk_ddr = IS_CLK_DDR;

	/* tcon host config */
	vo_dev->tcon_host->timing.is_interlace = IS_INTERLACE;
	/* format, reference to tcon_output_fmt_t */
	vo_dev->tcon_host->format = TCON_FMT;
	/* gamma correction */
	vo_dev->tcon_host->gamma_en = GAMMA_EN;
	vo_dev->tcon_host->gamma_tbl = gamma_tbl;
	/* frame rate control (dither) enable */
	vo_dev->tcon_host->frc_en = FRC_EN;
	/* TODO */
	vo_dev->tcon_host->csc_en = CSC_EN;
	/* TE mode */
	vo_dev->tcon_host->has_te = HAS_TE;
	vo_dev->tcon_host->te_pol = TE_POL;
}

#include <mach/debug.h>
static void doss_set_bl_pwm_io(u32 doss_idx, u32 val)
{
#ifdef CONFIG_ARCH_LOMBO_N7V0_FPGA
	csp_doss_io_set_output(doss_idx, PWM_IO);
	csp_doss_io_set_val(doss_idx, PWM_IO, val);
#else

	u32 rdval;
	u32 *reg_addr;

	reg_addr = (u32 *)VA_GPIO_GPIOE_FUNC_R1;
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015124 rdval:0x%x\n", rdval);
	WRITEREG32(reg_addr, (rdval & 0xfffffff0) | 0x00000002);
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015124 rdval:0x%x\n", rdval);

	reg_addr = (u32 *)VA_GPIO_GPIOE_DATA;
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015150 rdval:0x%x\n", rdval);
	WRITEREG32(reg_addr, (rdval & 0xfffffeff) | (val << 8));
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015150 rdval:0x%x\n", rdval);
#endif
}

static void doss_set_rest_io(u32 doss_idx, u32 val)
{
#ifdef CONFIG_ARCH_LOMBO_N7V0_FPGA
	csp_doss_io_set_output(doss_idx, RESET_IO);
	csp_doss_io_set_val(doss_idx, RESET_IO, val);
#else

	u32 rdval;
	u32 *reg_addr;

	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	reg_addr = (u32 *)VA_GPIO_GPIOE_FUNC_R0;
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015120 rdval:0x%x\n", rdval);
	WRITEREG32(reg_addr, (rdval & 0x0fffffff) | 0x20000000);
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015120 rdval:0x%x\n", rdval);

	reg_addr = (u32 *)VA_GPIO_GPIOE_DATA;
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015150 rdval:0x%x\n", rdval);
	WRITEREG32(reg_addr, (rdval & 0xffffff7f) | (val << 7));
	rdval = READREG32(reg_addr);
	DRM_DEBUG_KMS("[%d] rdval:0x%x\n", __LINE__, rdval);
	PRT_INFO("0xf4015150 rdval:0x%x\n", rdval);
#endif
}

static void tcon_dev_init(vo_device_t *vo_dev)
{
	u32 doss_index = vo_dev->tcon_host->doss_index;

	DRM_DEBUG_KMS("[%d]\n", __LINE__);

#if 1
	/* Power up, Init , Back Light En */
	doss_set_rest_io(doss_index, IO_HIGH);
	mdelay(1);
	doss_set_rest_io(doss_index, IO_LOW);
	mdelay(10);
	doss_set_rest_io(doss_index, IO_HIGH);
	mdelay(50);

	doss_set_bl_pwm_io(doss_index, IO_LOW);
#endif
}

static void tcon_dev_exit(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);
	/* TODO */
	/* Back Light Dis, Exit, Power down */
}

void tcon_dev_probe(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	vo_dev->dev_ops.dev_init = tcon_dev_init;
	vo_dev->dev_ops.dev_exit = tcon_dev_exit;
	tcon_dev_config(vo_dev);
}

static inline struct panel_simple *to_panel_simple(struct drm_panel *panel)
{
	return container_of(panel, struct panel_simple, base);
}

static int panel_simple_get_fixed_modes(struct panel_simple *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!panel->desc)
		return 0;

	for (i = 0; i < panel->desc->num_timings; i++) {
		const struct display_timing *dt = &panel->desc->timings[i];
		struct videomode vm;

		videomode_from_timing(dt, &vm);
		mode = drm_mode_create(drm);
		if (!mode) {
			dev_err(drm->dev, "failed to add mode %ux%u\n",
				dt->hactive.typ, dt->vactive.typ);
			continue;
		}

		drm_display_mode_from_videomode(&vm, mode);
		drm_mode_set_name(mode);

		drm_mode_probed_add(connector, mode);
		num++;
	}

	for (i = 0; i < panel->desc->num_modes; i++) {
		const struct drm_display_mode *m = &panel->desc->modes[i];

		mode = drm_mode_duplicate(drm, m);
		if (!mode) {
			dev_err(drm->dev, "failed to add mode %ux%u@%u\n",
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

	/* TODO */
	/* if (panel->desc->bus_format)                                    */
	/*	drm_display_info_set_bus_formats(&connector->display_info, */
	/*				 &panel->desc->bus_format, 1); */

	return num;
}

static int panel_simple_of_get_native_mode(struct panel_simple *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	struct device_node *timings_np;
	int ret;

	timings_np = of_get_child_by_name(panel->dev->of_node,
					  "display-timings");
	if (!timings_np) {
		dev_dbg(panel->dev, "failed to find display-timings node\n");
		return 0;
	}

	of_node_put(timings_np);
	mode = drm_mode_create(drm);
	if (!mode)
		return 0;

	ret = of_get_drm_display_mode(panel->dev->of_node, mode,
				      OF_USE_NATIVE_MODE);
	if (ret) {
		dev_dbg(panel->dev, "failed to find dts display timings\n");
		drm_mode_destroy(drm, mode);
		return 0;
	}

	drm_mode_set_name(mode);
	mode->type |= DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static int panel_simple_disable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (!p->enabled)
		return 0;

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_POWERDOWN;
		backlight_update_status(p->backlight);
	}

	if (p->desc && p->desc->delay.disable)
		msleep(p->desc->delay.disable);

	p->enabled = false;

	return 0;
}

static int panel_simple_unprepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (!p->prepared)
		return 0;

	if (p->enable_gpio)
		gpio_direction_output(p->enable_gpio, 0);

	regulator_disable(p->supply);

	if (p->desc && p->desc->delay.unprepare)
		msleep(p->desc->delay.unprepare);

	p->prepared = false;

	return 0;
}

static int panel_simple_prepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	int err;

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (p->prepared)
		return 0;

	err = regulator_enable(p->supply);
	if (err < 0) {
		dev_err(panel->dev, "failed to enable supply: %d\n", err);
		return err;
	}

	if (p->enable_gpio)
		gpio_direction_output(p->enable_gpio, 1);

	if (p->desc && p->desc->delay.prepare)
		msleep(p->desc->delay.prepare);

	p->prepared = true;

	return 0;
}

static int panel_simple_enable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (p->enabled)
		return 0;

	if (p->desc && p->desc->delay.enable)
		msleep(p->desc->delay.enable);

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(p->backlight);
	}

	p->enabled = true;

	return 0;
}

static int panel_simple_get_modes(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	int num = 0;

	/* add device node plane modes */
	num += panel_simple_of_get_native_mode(p);

	/* add hard-coded panel modes */
	num += panel_simple_get_fixed_modes(p);

	/* probe EDID if a DDC bus is available */
	if (p->ddc) {
		struct edid *edid = drm_get_edid(panel->connector, p->ddc);
		drm_mode_connector_update_edid_property(panel->connector, edid);
		if (edid) {
			num += drm_add_edid_modes(panel->connector, edid);
			kfree(edid);
		}
	}

	return num;
}

static int panel_simple_get_timings(struct drm_panel *panel,
				    unsigned int num_timings,
				    struct display_timing *timings)
{
	struct panel_simple *p = to_panel_simple(panel);
	unsigned int i;

	if (!p->desc)
		return 0;

	if (p->desc->num_timings < num_timings)
		num_timings = p->desc->num_timings;

	if (timings)
		for (i = 0; i < num_timings; i++)
			timings[i] = p->desc->timings[i];

	return p->desc->num_timings;
}

static const struct drm_panel_funcs panel_simple_funcs = {
	.disable = panel_simple_disable,
	.unprepare = panel_simple_unprepare,
	.prepare = panel_simple_prepare,
	.enable = panel_simple_enable,
	.get_modes = panel_simple_get_modes,
	.get_timings = panel_simple_get_timings,
};

static int panel_simple_probe(struct device *dev, const struct panel_desc *desc)
{
/* TODO */
#if 0
	struct device_node *backlight, *ddc;
#endif
	struct panel_simple *panel;
	struct panel_desc *of_desc;
	u32 val;
	int err;

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;

	DRM_DEBUG_KMS("[%d] desc:0x%x\n", __LINE__, (unsigned int)desc);

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
	panel->dev = dev;

/* TODO */
#if 0
	panel->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(panel->supply))
		return PTR_ERR(panel->supply);

	/* Linux4.4 use case */
	/* panel->enable_gpio = devm_gpiod_get_optional(dev, "enable", 0); */
	err = gpio_request(panel->enable_gpio, "enable-gpios");
	if (err) {
		dev_err(dev, "failed to request GPIO: %d\n", err);
		return err;
	}

	backlight = of_parse_phandle(dev->of_node, "backlight", 0);
	if (backlight) {
		panel->backlight = of_find_backlight_by_node(backlight);
		of_node_put(backlight);

		if (!panel->backlight)
			return -EPROBE_DEFER;
	}

	ddc = of_parse_phandle(dev->of_node, "ddc-i2c-bus", 0);
	if (ddc) {
		panel->ddc = of_find_i2c_adapter_by_node(ddc);
		of_node_put(ddc);

		if (!panel->ddc) {
			err = -EPROBE_DEFER;
			goto free_backlight;
		}
	}
#endif

	drm_panel_init(&panel->base);
	panel->base.dev = dev;
	panel->base.funcs = &panel_simple_funcs;

	err = drm_panel_add(&panel->base);
	if (err < 0)
		goto free_ddc;

	dev_set_drvdata(dev, panel);

	DRM_DEBUG_KMS("[%d] over.\n", __LINE__);

	return 0;

free_ddc:
	if (panel->ddc)
		put_device(&panel->ddc->dev);

/* TODO */
#if 0
free_backlight:
	if (panel->backlight)
		put_device(&panel->backlight->dev);
#endif

	return err;
}

static int panel_simple_remove(struct device *dev)
{
	struct panel_simple *panel = dev_get_drvdata(dev);

	drm_panel_detach(&panel->base);
	drm_panel_remove(&panel->base);

	panel_simple_disable(&panel->base);

	if (panel->ddc)
		put_device(&panel->ddc->dev);

	if (panel->backlight)
		put_device(&panel->backlight->dev);

	return 0;
}

static void panel_simple_shutdown(struct device *dev)
{
	struct panel_simple *panel = dev_get_drvdata(dev);

	panel_simple_disable(&panel->base);
}

/* 115 fpga board PRGB 800x480 LCD test */
static const struct drm_display_mode spc_s92048_mode = {
	/* dclk_freq */
	.clock = 33000000,
	/* width */
	.hdisplay = 800,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = 800 + 40,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = 800 + 40 + 128,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = 800 + 40 + 128 + 88,
	/* height */
	.vdisplay = 480,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = 480 + 10,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = 480 + 10 + 2,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = 480 + 10 + 2 + 33,
	.vrefresh = 60,
};

static const struct panel_desc spc_s92048 = {
	.modes = &spc_s92048_mode,
	.num_modes = 1,
	.bpc = 6,
	.size = {
		.width = 800,
		.height = 480,
	},
};

static const struct of_device_id platform_of_match[] = {
	{
		.compatible = "spc,s92048",
		.data = &spc_s92048,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, platform_of_match);

static int panel_simple_platform_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;

	id = of_match_node(platform_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return panel_simple_probe(&pdev->dev, id->data);
}

static int panel_simple_platform_remove(struct platform_device *pdev)
{
	return panel_simple_remove(&pdev->dev);
}

static void panel_simple_platform_shutdown(struct platform_device *pdev)
{
	panel_simple_shutdown(&pdev->dev);
}

static struct platform_driver panel_simple_platform_driver = {
	.driver = {
		.name = "panel-simple",
		.of_match_table = platform_of_match,
	},
	.probe = panel_simple_platform_probe,
	.remove = panel_simple_platform_remove,
	.shutdown = panel_simple_platform_shutdown,
};

static int __init panel_simple_init(void)
{
	int err;

	err = platform_driver_register(&panel_simple_platform_driver);
	if (err < 0)
		return err;

	return 0;
}
arch_initcall(panel_simple_init);

static void __exit panel_simple_exit(void)
{
	platform_driver_unregister(&panel_simple_platform_driver);
}
module_exit(panel_simple_exit);

MODULE_AUTHOR("Thierry Reding <treding@nvidia.com>");
MODULE_DESCRIPTION("DRM Driver for Simple Panels");
MODULE_LICENSE("GPL and additional rights");

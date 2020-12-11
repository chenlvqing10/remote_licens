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
#include <linux/of_platform.h>
#include <linux/spi/spi.h>
#include <drm/drmP.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>

#include "panel-simple.h"
#include "lombo_panel_config.h"

#define IS_INTERLACE	(FALSE)

#define WIDTH		(480)
#define HEIGHT		(272)

#define HBP		(180)
#define HFP		(160)
#define HSW		(180)
#define HACT		(WIDTH)
#define VBP_F1		(30)
#define VFP_F1		(30)
#define VSW_F1		(6)
#define VACT_F1		(HEIGHT)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT		(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define DCLK		(19800000)

#define ACTIVE(flag)		(((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)
#define INACTIVE(flag)		(((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)

struct panel_fj018_spi {
	int spi_data;
	enum of_gpio_flags spi_data_flag;
	int spi_clk;
	enum of_gpio_flags spi_clk_flag;
	int spi_cs;
	enum of_gpio_flags spi_cs_flag;
};
struct panel_fj018_spi spi;

static struct lombo_vo_dev_config fj018wv04_vo = {
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

static struct drm_display_mode fj018wv04_mode = {
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

	.private_size = sizeof(fj018wv04_vo),
	.private = (int *)&fj018wv04_vo,
};

static struct panel_delay fj018wv04_delay = {
	.prepare = 150,
	.unprepare = 0,
	.enable = 50,
	.disable = 0,
	.reset = 50,
};

static inline void hw_init_spi(struct panel_fj018_spi *spi)
{
	gpio_direction_output(spi->spi_cs, 1);
	gpio_direction_output(spi->spi_clk, 1);
	gpio_direction_output(spi->spi_data, 0);
}

static inline void hw_setspi_cs(void)
{
	gpio_direction_output(spi.spi_cs, ACTIVE(spi.spi_cs_flag));
}

static inline void hw_clrspi_cs(void)
{
	gpio_direction_output(spi.spi_cs, INACTIVE(spi.spi_cs_flag));
}

static inline void hw_setspi_clk(void)
{
	gpio_direction_output(spi.spi_clk, ACTIVE(spi.spi_clk_flag));
}

static inline void hw_clrspi_clk(void)
{
	gpio_direction_output(spi.spi_clk, INACTIVE(spi.spi_clk_flag));
}

static inline void hw_setspi_data(void)
{
	gpio_direction_output(spi.spi_data, ACTIVE(spi.spi_data_flag));
}

static inline void hw_clrspi_data(void)
{
	gpio_direction_output(spi.spi_data, INACTIVE(spi.spi_data_flag));
}

static void spi_senddata(unsigned char i)
{
	unsigned char n;

	for (n = 0; n < 8; n++) {
		if (i & 0x80)
			hw_setspi_data();
		else
			hw_clrspi_data();

		i <<= 1;

		hw_clrspi_clk();
		hw_setspi_clk();
	}
}

static void spi_writecomm(unsigned char i)
{
	hw_clrspi_cs();

	hw_clrspi_data();

	hw_clrspi_clk();
	hw_setspi_clk();

	spi_senddata(i);

	hw_setspi_cs();
}

static void spi_writedata(unsigned char i)
{
	hw_clrspi_cs();

	hw_setspi_data();

	hw_clrspi_clk();
	hw_setspi_clk();

	spi_senddata(i);

	hw_setspi_cs();
}

static void panel_fj018_init(void)
{
	spi_writecomm(0x36);
	spi_writedata(0x00);

	spi_writecomm(0x3A);

	spi_writedata(0x66);
	spi_writecomm(0xFF);
	spi_writedata(0x77);
	spi_writedata(0x01);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x10);

	spi_writecomm(0xC0);
	spi_writedata(0x21);
	spi_writedata(0x00);

	spi_writecomm(0xC1);
	spi_writedata(0x0B);
	spi_writedata(0x02);

	spi_writecomm(0xC2);
	spi_writedata(0x37);
	spi_writedata(0x02);

	spi_writecomm(0xC6);
	spi_writedata(0x80);

	spi_writecomm(0xC7);
	spi_writedata(0x00);
	spi_writecomm(0xCC);
	spi_writedata(0x10);

	spi_writecomm(0xCD);
	spi_writedata(0x08);

	spi_writecomm(0xB0);
	spi_writedata(0x00);
	spi_writedata(0x12);
	spi_writedata(0x1F);
	spi_writedata(0x15);
	spi_writedata(0x18);
	spi_writedata(0x0A);
	spi_writedata(0x10);
	spi_writedata(0x0A);
	spi_writedata(0x08);
	spi_writedata(0x23);
	spi_writedata(0x06);
	spi_writedata(0x13);
	spi_writedata(0x10);
	spi_writedata(0x12);
	spi_writedata(0x15);
	spi_writedata(0x13);

	spi_writecomm(0xB1);

	spi_writedata(0x00);
	spi_writedata(0x12);
	spi_writedata(0x1F);
	spi_writedata(0x15);
	spi_writedata(0x18);
	spi_writedata(0x0A);
	spi_writedata(0x10);
	spi_writedata(0x0A);
	spi_writedata(0x08);
	spi_writedata(0x23);
	spi_writedata(0x06);
	spi_writedata(0x13);
	spi_writedata(0x10);
	spi_writedata(0x12);
	spi_writedata(0x15);
	spi_writedata(0x13);

	spi_writecomm(0xFF);
	spi_writedata(0x77);
	spi_writedata(0x01);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x11);

	spi_writecomm(0xB0);
	spi_writedata(0x5D);

	spi_writecomm(0xB1);
	spi_writedata(0x51);

	spi_writecomm(0xB2);
	spi_writedata(0x07);

	spi_writecomm(0xB3);
	spi_writedata(0x80);

	spi_writecomm(0xB5);
	spi_writedata(0x4B);

	spi_writecomm(0xB7);
	spi_writedata(0x85);

	spi_writecomm(0xB8);
	spi_writedata(0x20);

	spi_writecomm(0xBA);
	spi_writedata(0x00);
	spi_writecomm(0xBB);
	spi_writedata(0x00);
	spi_writecomm(0xBC);
	spi_writedata(0x00);

	spi_writecomm(0xC1);
	spi_writedata(0x78);

	spi_writecomm(0xC2);
	spi_writedata(0x78);


	spi_writecomm(0xD0);
	spi_writedata(0x88);

	msleep(110);

	spi_writecomm(0xE0);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x02);

	spi_writecomm(0xE1);
	spi_writedata(0x03);
	spi_writedata(0xE0);
	spi_writedata(0x05);
	spi_writedata(0xE0);
	spi_writedata(0x04);
	spi_writedata(0xE0);
	spi_writedata(0x06);
	spi_writedata(0xE0);
	spi_writedata(0x00);
	spi_writedata(0x22);
	spi_writedata(0x22);

	spi_writecomm(0xE2);
	spi_writedata(0x10);
	spi_writedata(0x10);
	spi_writedata(0x20);
	spi_writedata(0x20);
	spi_writedata(0x1B);
	spi_writedata(0xE0);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x1C);
	spi_writedata(0xE0);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);

	spi_writecomm(0xE3);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x11);
	spi_writedata(0x11);

	spi_writecomm(0xE4);
	spi_writedata(0x22);
	spi_writedata(0x22);

	spi_writecomm(0xE5);
	spi_writedata(0x07);
	spi_writedata(0x19);
	spi_writedata(0x00);
	spi_writedata(0xE0);
	spi_writedata(0x09);
	spi_writedata(0x1B);
	spi_writedata(0x00);
	spi_writedata(0xE0);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);

	spi_writecomm(0xE6);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x11);
	spi_writedata(0x11);

	spi_writecomm(0xE7);
	spi_writedata(0x22);
	spi_writedata(0x22);

	spi_writecomm(0xE8);
	spi_writedata(0x08);
	spi_writedata(0x1A);
	spi_writedata(0x00);
	spi_writedata(0xE0);
	spi_writedata(0x0A);
	spi_writedata(0x1C);
	spi_writedata(0x00);
	spi_writedata(0xE0);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);

	spi_writecomm(0xEB);
	spi_writedata(0x00);
	spi_writedata(0x01);
	spi_writedata(0x40);
	spi_writedata(0x40);
	spi_writedata(0x22);
	spi_writedata(0x44);
	spi_writedata(0x40);

	spi_writecomm(0xEC);
	spi_writedata(0x02);
	spi_writedata(0x01);

	spi_writecomm(0xED);
	spi_writedata(0xFF);
	spi_writedata(0xF1);
	spi_writedata(0x45);
	spi_writedata(0x89);
	spi_writedata(0x2B);
	spi_writedata(0xA0);
	spi_writedata(0xFF);
	spi_writedata(0xFF);
	spi_writedata(0xFF);
	spi_writedata(0xFF);
	spi_writedata(0x0A);
	spi_writedata(0xB2);
	spi_writedata(0x98);
	spi_writedata(0x54);
	spi_writedata(0x1F);
	spi_writedata(0xFF);

	spi_writecomm(0xFF);
	spi_writedata(0x77);
	spi_writedata(0x01);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x13);

	spi_writecomm(0xE5);
	spi_writedata(0xE4);

	spi_writecomm(0xFF);
	spi_writedata(0x77);
	spi_writedata(0x01);
	spi_writedata(0x00);
	spi_writedata(0x00);
	spi_writedata(0x00);

	msleep(120);
	spi_writecomm(0x11);
	spi_writecomm(0x29);
	spi_writecomm(0xD4);
	spi_writedata(0xFA);
	spi_writedata(0x00);
	spi_writecomm(0xB0);
	spi_writecomm(0xFA);
	spi_writecomm(0x00);
}

static ssize_t panel_fj018_spi_wr_init_code(
			struct panel_simple_host *host)
{
	panel_fj018_init();

	return 0;
}

static int panel_fj018_spi_attach(void *param)
{
	struct device *dev = (struct device *)param;

	struct panel_fj018_spi *p = &spi;
	struct device_node *np = dev->of_node;
	int ret;

	enum of_gpio_flags flags;

	DRM_DEBUG("%d\n", __LINE__);
	if (!of_device_is_available(np))
		return -ENODEV;

	p->spi_data = of_get_named_gpio_flags(np, "spi_sda", 0, &flags);
	p->spi_data_flag = flags;
	if (!gpio_is_valid(p->spi_data)) {
		dev_err(dev, "failed to get %s GPIO\n", "spi_data");
		p->spi_data = 0;
	} else {
		DRM_DEBUG_KMS("spi_data %d, flags 0x%x\n",
			p->spi_data, flags);
		ret = devm_gpio_request(dev, p->spi_data, "spi_sda");
		if (ret) {
			dev_err(dev, "spi_data %d request failed\n",
				p->spi_data);
			p->spi_data = 0;
		}
	}

	p->spi_clk = of_get_named_gpio_flags(np, "spi_scl", 0, &flags);
	p->spi_clk_flag = flags;
	if (!gpio_is_valid(p->spi_clk)) {
		dev_err(dev, "failed to get %s GPIO\n", "spi_clk");
		p->spi_clk = 0;
	} else {
		DRM_DEBUG_KMS("spi_scl %d, flags 0x%x\n",
			p->spi_clk, flags);
		ret = devm_gpio_request(dev, p->spi_clk, "spi_scl");
		if (ret) {
			dev_err(dev, "spi_clk %d request failed\n", p->spi_clk);
			p->spi_clk = 0;
		}
	}

	p->spi_cs = of_get_named_gpio_flags(np,	"spi_cs", 0, &flags);
	p->spi_cs_flag = flags;
	if (!gpio_is_valid(p->spi_cs)) {
		dev_err(dev, "failed to get %s GPIO\n", "spi_cs");
		p->spi_cs = 0;
	} else {
		DRM_DEBUG_KMS("spi_cs %d, flags 0x%x\n",
			p->spi_cs, flags);
		ret = devm_gpio_request(dev, p->spi_cs, "spi_cs");
		if (ret) {
			dev_err(dev, "spi_cs %d request failed\n", p->spi_cs);
			p->spi_cs = 0;
		}
	}

	hw_init_spi(&spi);
	return 0;
}

const struct panel_simple_desc simple_fj018wv04_desc = {
	.modes = &fj018wv04_mode,
	.num_modes = 1,
	.delay = &fj018wv04_delay,
	.init_panel = PANEL_SIMPLE_SPI,
	.size = {
		.width = 140,
		.height = 90,
	},
	.ops.attach = panel_fj018_spi_attach,
	.ops.transfer = panel_fj018_spi_wr_init_code,
};

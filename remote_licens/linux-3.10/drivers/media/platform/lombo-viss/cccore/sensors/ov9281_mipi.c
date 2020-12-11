/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ov9281 mipi-csi sensor driver code for LomboTech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>

#include <media/lombo_viss.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>

#include <mach/debug.h>
#include "../ccc.h"

#define OV9281_WIDTH		1280
#define OV9281_HEIGHT		800

/*
 * 2lane YUV init
 */
static const cc_reg_t reg_list_raw10[] = {
	{ 0x0103, 0x01 }, /* software reset */
	{ 0x0302, 0x32 },
	/* { OV5640_MIPI_REG_DELAY_FLAG, 0x5 }, */ /* mdelay(5); */
	{ 0x030d, 0x50 },
	{ 0x030e, 0x02 },
	{ 0x3001, 0x00 },
	{ 0x3004, 0x00 },
	{ 0x3005, 0x00 },
	{ 0x3006, 0x04 },
	{ 0x3011, 0x0a },
	{ 0x3013, 0x18 },
	{ 0x301c, 0xf0 },
	{ 0x3022, 0x01 },
	{ 0x3030, 0x10 },
	{ 0x3039, 0x32 },
	{ 0x303a, 0x00 },
	{ 0x3500, 0x00 },
	{ 0x3501, 0x2a },
	{ 0x3502, 0x90 },
	{ 0x3503, 0x08 },
	{ 0x3505, 0x8c },
	{ 0x3507, 0x03 },
	{ 0x3508, 0x00 },
	{ 0x3509, 0x10 },
	{ 0x3610, 0x80 },
	{ 0x3611, 0xa0 },
	{ 0x3620, 0x6e },
	{ 0x3632, 0x56 },
	{ 0x3633, 0x78 },
	{ 0x3662, 0x05 },
	{ 0x3666, 0x00 },
	{ 0x366f, 0x5a },
	{ 0x3680, 0x84 },
	{ 0x3712, 0x80 },
	{ 0x372d, 0x22 },
	{ 0x3731, 0x80 },
	{ 0x3732, 0x30 },
	{ 0x3778, 0x00 },
	{ 0x377d, 0x22 },
	{ 0x3788, 0x02 },
	{ 0x3789, 0xa4 },
	{ 0x378a, 0x00 },
	{ 0x378b, 0x4a },
	{ 0x3799, 0x20 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x00 },
	{ 0x3804, 0x05 },
	{ 0x3805, 0x0f },
	{ 0x3806, 0x03 },
	{ 0x3807, 0x2f },
	{ 0x3808, 0x05 },
	{ 0x3809, 0x00 },
	{ 0x380a, 0x03 },
	{ 0x380b, 0x20 },
	{ 0x380c, 0x02 },
	{ 0x380d, 0xd8 },
	{ 0x380e, 0x03 },
	{ 0x380f, 0x8e },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x08 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x08 },

	{ 0x3814, 0x11 },
	{ 0x3815, 0x11 },

	{ 0x3820, 0x40 },
	{ 0x3821, 0x00 },

	{ 0x382c, 0x05 },

	{ 0x382d, 0xb0 },
	{ 0x389d, 0x00 },
	{ 0x3881, 0x42 },
	{ 0x3882, 0x01 },
	{ 0x3883, 0x00 },
	{ 0x3885, 0x02 },
	{ 0x38a8, 0x02 },
	{ 0x38a9, 0x80 },
	{ 0x38b1, 0x00 },
	{ 0x38b3, 0x02 },
	{ 0x38c4, 0x00 },
	{ 0x38c5, 0xc0 },
	{ 0x38c6, 0x04 },
	{ 0x38c7, 0x80 },
	{ 0x3920, 0xff },
	{ 0x4003, 0x40 },
	{ 0x4008, 0x04 },
	{ 0x4009, 0x0b },
	{ 0x400c, 0x00 },
	{ 0x400d, 0x07 },
	{ 0x4010, 0x40 },
	{ 0x4043, 0x40 },
	{ 0x4307, 0x30 },
	{ 0x4317, 0x00 },
	{ 0x4501, 0x00 },
	{ 0x4507, 0x00 },
	{ 0x4509, 0x00 },
	{ 0x450a, 0x08 },
	{ 0x4601, 0x04 },
	{ 0x470f, 0x00 },
	{ 0x4f07, 0x00 },
	{ 0x4800, 0x00 },
	{ 0x5000, 0x9f },
	{ 0x5001, 0x00 },
	{ 0x5e00, 0x00 },
	{ 0x5d00, 0x07 },
	{ 0x5d01, 0x00 },
	{ 0x4f00, 0x04 },
	{ 0x4f10, 0x00 },
	{ 0x4f11, 0x98 },
	{ 0x4f12, 0x0f },
	{ 0x4f13, 0xc4 },
	/* { 0x0100, 0x01 }, */

};

static const cc_reg_t reg_list_raw8[] = {
	{ 0x0103, 0x01 },
	{ 0x0302, 0x32 },
	{ 0x030d, 0x50 },
	{ 0x030e, 0x02 },
	{ 0x3001, 0x00 },
	{ 0x3001, 0x00 },
	{ 0x3004, 0x00 },
	{ 0x3005, 0x00 },
	{ 0x3006, 0x04 },
	{ 0x3011, 0x0a },
	{ 0x3013, 0x18 },
	{ 0x301c, 0xf0 },
	{ 0x3022, 0x01 },
	{ 0x3030, 0x10 },
	{ 0x3039, 0x32 },
	{ 0x303a, 0x00 },
	{ 0x3500, 0x01 },
	{ 0x3501, 0x2a },
	{ 0x3502, 0x90 },
	{ 0x3503, 0x08 },
	{ 0x3505, 0x8c },
	{ 0x3507, 0x03 },
	{ 0x3508, 0x00 },
	{ 0x3509, 0x10 },
	{ 0x3610, 0x80 },
	{ 0x3611, 0xa0 },
	{ 0x3620, 0x6e },
	{ 0x3632, 0x56 },
	{ 0x3633, 0x78 },
	{ 0x3662, 0x07 },
	{ 0x3666, 0x00 },
	{ 0x366f, 0x5a },
	{ 0x3680, 0x84 },
	{ 0x3712, 0x80 },
	{ 0x372d, 0x22 },
	{ 0x3731, 0x80 },
	{ 0x3732, 0x30 },
	{ 0x3778, 0x00 },
	{ 0x377d, 0x22 },
	{ 0x3788, 0x02 },
	{ 0x3789, 0xa4 },
	{ 0x378a, 0x00 },
	{ 0x378b, 0x4a },
	{ 0x3799, 0x20 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x00 },
	{ 0x3804, 0x05 },
	{ 0x3805, 0x0f },
	{ 0x3806, 0x03 },
	{ 0x3807, 0x2f },
	{ 0x3808, 0x05 },
	{ 0x3809, 0x00 },
	{ 0x380a, 0x03 },
	{ 0x380b, 0x20 },
	{ 0x380c, 0x02 },
	{ 0x380d, 0xd8 },
	{ 0x380e, 0x03 },
	{ 0x380f, 0x8e },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x08 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x08 },
	{ 0x3814, 0x11 },
	{ 0x3815, 0x11 },
	{ 0x3820, 0x40 },
	{ 0x3821, 0x00 },
	{ 0x382c, 0x05 },
	{ 0x382d, 0xb0 },
	{ 0x389d, 0x00 },
	{ 0x3881, 0x42 },
	{ 0x3882, 0x01 },
	{ 0x3883, 0x00 },
	{ 0x3885, 0x02 },
	{ 0x38a8, 0x02 },
	{ 0x38a9, 0x80 },
	{ 0x38b1, 0x00 },
	{ 0x38b3, 0x02 },
	{ 0x38c4, 0x00 },
	{ 0x38c5, 0xc0 },
	{ 0x38c6, 0x04 },
	{ 0x38c7, 0x80 },
	{ 0x3920, 0xff },
	{ 0x4003, 0x40 },
	{ 0x4008, 0x04 },
	{ 0x4009, 0x0b },
	{ 0x400c, 0x00 },
	{ 0x400d, 0x07 },
	{ 0x4010, 0x40 },
	{ 0x4043, 0x40 },
	{ 0x4307, 0x30 },
	{ 0x4317, 0x00 },
	{ 0x4501, 0x00 },
	{ 0x4507, 0x00 },
	{ 0x4509, 0x00 },
	{ 0x450a, 0x08 },
	{ 0x4601, 0x9f },
	{ 0x470f, 0x00 },
	{ 0x4f07, 0x00 },
	{ 0x4800, 0x00 },
	{ 0x5000, 0x9f },
	{ 0x5001, 0x00 },
	{ 0x5e00, 0x00 },
	{ 0x5d00, 0x07 },
	{ 0x5d01, 0x00 },
	{ 0x4f00, 0x04 },
	{ 0x4f10, 0x00 },
	{ 0x4f11, 0x98 },
	{ 0x4f12, 0x0f },
	{ 0x4f13, 0xc4 },
	/* { 0x0100, 0x01 }, */
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 50},
	{CC_RESET, CC_UP},
	{CC_DELAY, 10},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_SGRBG8_1X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
	{
		.mbus_code = V4L2_MBUS_FMT_SGRBG10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

static const struct cc_win_size win_sizes[] = {
	{
		.width = OV9281_WIDTH,
		.height = OV9281_HEIGHT,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = OV9281_WIDTH,
		.height = OV9281_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
		.mfreq = 800000000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = cc_si->read_reg(cc_si, 0x300A, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0x300B, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x9281) {
		PRT_ERR("ID wrong! (0x%04x != 0x9281)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int sensor_init(struct cc_sensor_info *cc_si)
{
#if 0
	u8 value = 0;
	int i;
#endif
	int ret = 0;

	PRT_INFO("\n");

	ret = sensor_detect(cc_si);
	if (ret != 0)
		return ret;

	ret = cc_si->write_reg_list(cc_si, reg_list_raw8,
				ARRAY_SIZE(reg_list_raw8));
	if (ret != 0) {
		PRT_ERR("write reg_list_raw8 failed!\n");
		return -ENODEV;
	}

	PRT_INFO("write reg_list_raw8 finish.\n");

#if 0
	for (i = 0; i < ARRAY_SIZE(reg_list_raw8); i++) {
		if (reg_list_raw8[i].reg == CC_REG_DELAY)
			continue;

		ret = ccsi->read_reg(cc_si,
				reg_list_raw8[i].reg_add,
				&value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}

		if (reg_list_raw8[i].reg_value != value) {
			PRT_ERR("value(0x%02x) != reg_value(0x%02x)\n",
				value, reg_list_raw8[i].reg_value);
		}
	}
#endif

	return 0;

}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;

	if (on) {
		ret = cc_si->write_reg(cc_si, 0x0100, 0x01);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->write_reg(cc_si, 0x0100, 0x00);
		if (ret)
			PRT_ERR("%d\n", on);
	}

	return ret;
}

#define DRIVER_NAME "ov9281_mipi"

static int sensor_probe(struct platform_device *pdev)
{
	struct cc_sensor_info *cc_si = NULL;
	int ret = 0;

	PRT_INFO("*********probe begin\n");

	cc_si = devm_kzalloc(&pdev->dev, sizeof(*cc_si), GFP_KERNEL);
	if (cc_si == NULL) {
		PRT_ERR("Can't alloc memory!\n");
		return -ENOMEM;
	}

	cc_si->reg_bits = 16;
	cc_si->val_bits = 8;

	cc_si->dlanes = 4;

	cc_si->name = DRIVER_NAME;

	cc_si->power_up_act = power_up_act;
	cc_si->power_down_act = power_down_act;
	cc_si->npwr_up_acts = ARRAY_SIZE(power_up_act);
	cc_si->npwr_dn_acts = ARRAY_SIZE(power_down_act);

	cc_si->framefmts = frame_formats;
	cc_si->nfrmfmts = ARRAY_SIZE(frame_formats);

	cc_si->win_sizes = win_sizes;
	cc_si->nwsizes = ARRAY_SIZE(win_sizes);

	cc_si->framecfgs = frame_cfgs;
	cc_si->nfrmcfgs = ARRAY_SIZE(frame_cfgs);

	ret = cc_sensor_probe(pdev, cc_si);
	if (ret < 0)
		return ret;

	cc_si->detect = sensor_detect;
	cc_si->sensor_init = sensor_init;
	cc_si->set_stream = sensor_set_stream;
	cc_si->sensor_ioctl = NULL;

	PRT_INFO("probe end\n");
	return 0;
}

static int sensor_remove(struct platform_device *pdev)
{
	struct cc_sensor_info *cc_si = platform_get_drvdata(pdev);

	cc_sensor_remove(pdev);

	devm_kfree(&pdev->dev, cc_si);

	return 0;
}

static const struct of_device_id ov9281_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov9281_mipi_of_match);

static struct platform_driver ov9281_mipi_driver = {
	.driver = {
		.of_match_table	= ov9281_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov9281_mipi_driver);

MODULE_DESCRIPTION("OV9281-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


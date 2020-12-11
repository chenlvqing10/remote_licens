/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 *tw2866 driver code for LomboTech
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

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/viss_isp.h>

#include <media/lombo_viss.h>

#include <mach/debug.h>
#include "../ccc.h"

static const cc_reg_t init_reg_list1[] = {/* TW2866_1_CHANNEL */
	{ 0xFA, 0x40 },/* Output Enable and Clock Output */
	{ 0xFB, 0xAF },/* Clock Polarity and interrupt */
	{ 0xFC, 0xFF },/* Enable Video and Audio Detection */
	{ 0x9C, 0x20 },/* OVSEND */
	{ 0x9E, 0x42 },/* NOVID 0x42:no channel ID */
	{ 0xF9, 0x11 },/* Video Miscellaneous Function */
	{ 0xAA, 0x00 },/* Video AGC */
	/* { 0x6A, 0x0F }, *//* Clock Output */
	/* { 0x6B, 0x0F }, *//* Clock Output */
	/* { 0x6C, 0x0F }, *//* Output Enable and Clock Output */
	{ 0x60, 0x15 },/* Clock PLL Control, 27MHz clock input */
	{ 0x61, 0x03 },/* 108MHz Clock Select */
	{ 0xCA, 0x00 },/* Video Channel Output */
	{ 0xCD, 0xE4 },/* 1st Channel Selection */
	{ 0x5B, 0x11 },/* Video Output Pin Drive */
	/* { 0x43, 0x18 }, *//* Video Encoder Clock and Delay control*/
};

static const cc_reg_t init_reg_list2[] = {/* TW2866_2_CHANNEL */
	{ 0xFA, 0x45 },/* Output Enable and Clock Output, 54MHz */
	{ 0xFB, 0xAF },/* Clock Polarity Control */
	{ 0xFC, 0xFF },/* Enable Video and Audio Detection */
	{ 0x9C, 0x20 },/* OVSEND */
	{ 0x9E, 0x52 },/* NOVID, with channel ID */
	{ 0xF9, 0x11 },/* Video Miscellaneous Function */
	{ 0xAA, 0x00 },/* Video AGC */
	{ 0x60, 0x15 },/* Clock PLL Control */
	{ 0x61, 0x03 },/* 108MHz Clock Select */
	{ 0xCA, 0x01 },/* Video Channel Output */
	{ 0xCD, 0xE4 },/* 1st Channel Selection */
	{ 0xCC, 0x39 },/* 2nd Channel Selection */
	{ 0xCB, 0x00 },/* Four Channel CIF Time-multiplexed Format */
	{ 0x5B, 0x11 },/* Video Output Pin Drive */
};

static const cc_reg_t init_reg_list3[] = {/* TW2866_4_CHANNEL */
	{ 0xFA, 0x4A },/* Output Enable and Clock Output */
	{ 0xFB, 0xAF },/* Clock Polarity Control */
	{ 0xFC, 0xFF },/* Enable Video and Audio Detection */
	{ 0x9C, 0x20 },/* OVSEND */
	{ 0x9E, 0x52 },/* NOVID */
	{ 0xF9, 0x11 },/* Video Miscellaneous Function */
	{ 0xAA, 0x00 },/* Video AGC */
	{ 0x60, 0x15 },/* Clock PLL Control */
	{ 0x61, 0x03 },/* 108MHz Clock Select */
	{ 0xCA, 0x02 },/* Video Channel Output */
	{ 0xCD, 0xE4 },/* 1st Channel Selection */
	{ 0xCC, 0x39 },/* 2nd Channel Selection */
	{ 0xCB, 0x00 },/* Four Channel CIF Time-multiplexed Format */
	{ 0x5B, 0x11 },/* Video Output Pin Drive */
};

static const cc_reg_t init_reg_list4[] = {/* TW2866_4_CHANNEL_CIF */
	{ 0xFA, 0x45 },/* Output Enable and Clock Output */
	{ 0xFB, 0xAF },/* Clock Polarity Control */
	{ 0xFC, 0xFF },/* Enable Video and Audio Detection */
	{ 0x9C, 0x20 },/* OVSEND */
	{ 0x9E, 0x52 },/* NOVID */
	{ 0xF9, 0x11 },/* Video Miscellaneous Function */
	{ 0xAA, 0x00 },/* Video AGC */
	{ 0x60, 0x15 },/* Clock PLL Control */
	{ 0x61, 0x03 },/* 54MHz Clock Select */
	{ 0xCA, 0x00 },/* Video Channel Output */
	{ 0xCD, 0xE4 },/* 1st Channel Selection */
	{ 0xCC, 0x39 },/* 2nd Channel Selection */
	{ 0xCB, 0x01 },/* Four Channel CIF Time-multiplexed Format */
	{ 0x5B, 0x11 },/* Video Output Pin Drive */
};

static const cc_reg_t init_reg_list5[] = {/* TW2866_4_CHANNEL_QUAD */
	{ 0xFA, 0xC5 },/* Output Enable and Clock Output */
	{ 0xFB, 0xAF },/* Clock Polarity Control */
	{ 0xFC, 0xFF },/* Enable Video and Audio Detection */
	{ 0x9C, 0x20 },/* OVSEND */
	{ 0x9E, 0x52 },/* NOVID */
	{ 0xF9, 0x11 },/* Video Miscellaneous Function */
	{ 0xAA, 0x00 },/* Video AGC */
	{ 0x60, 0x15 },/* Clock PLL Control */
	{ 0x61, 0x03 },/* 54MHz Clock Select */
	{ 0xCA, 0x00 },/* Video Channel Output */
	{ 0xCD, 0xE4 },/* 1st Channel Selection */
	{ 0xCC, 0x39 },/* 2nd Channel Selection */
	{ 0xCB, 0x01 },/* Four Channel CIF Time-multiplexed Format */
	{ 0x5B, 0x11 },/* Video Output Pin Drive */
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_UP},
	{CC_DELAY, 100},
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 100},
	{CC_RESET, CC_UP},
	{CC_DELAY, 100},

};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_UYVY8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define FHD_WIDTH_SUB		720
#define FHD_HEIGHT_SUB		240

#define FHD_WIDTH_CH4_F		(720 * 2)/* F */
#define FHD_HEIGHT_CH4_F	(240 * 2)

#define FHD_WIDTH_CH4_V		(720)/* v */
#define FHD_HEIGHT_CH4_V	(240 * 4)


static const struct cc_win_size win_sizes[] = {
	{
		.width = FHD_WIDTH_CH4_V,
		.height = FHD_HEIGHT_CH4_V,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = FHD_WIDTH_CH4_V,
		.height = FHD_HEIGHT_CH4_V,
		.sub_width = FHD_WIDTH_SUB,
		.sub_height = FHD_HEIGHT_SUB,

		.framerate = { 1000, 29970 },	/* 30fps */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = cc_si->read_reg(cc_si, 0xff, &tmp[0]);
	tmp[0] = (tmp[0] >> 3) & 0x1f;
	if (ret != 0) {
		PRT_ERR("tmp[0]=%02x\n", tmp[0]);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0xfe, &tmp[1]);
	tmp[1] = (tmp[1] >> 6) & 0x03;
	if (ret != 0) {
		PRT_ERR("tmp[0]=%02x,tmp[1]=%02x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[1] << 8) | tmp[0];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x0019) {
		PRT_ERR("ID wrong! (0x%04x != 0x0019)\n", id);
		return -ENODEV;
	}
	return 0;
}

static int sensor_init(struct cc_sensor_info *cc_si)
{
	struct timespec ts_s;
	struct timespec ts_f;
	int ret = 0;

	ret = sensor_detect(cc_si);
	if (ret != 0)
		return ret;

	ktime_get_ts(&ts_s);
	ret = cc_si->write_reg_list(cc_si, init_reg_list3,
					ARRAY_SIZE(init_reg_list3));
	if (ret != 0) {
		PRT_ERR("write %s init_reg_list1 failed!\n", cc_si->name);
		return -ENODEV;
	}
	ktime_get_ts(&ts_f);
	PRT_INFO("write %s init_reg_list4 finish!\n", cc_si->name);

	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

	return 0;
}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;
	PRT_INFO("*************************stream:%d\n", on);
	return ret;
}

#define DRIVER_NAME "tw2866_dvp"

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

	cc_si->reg_bits = 8;
	cc_si->val_bits = 8;

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

static const struct of_device_id tw2866_dvp_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, tw2866_dvp_of_match);

static struct platform_driver tw2866_dvp_driver = {
	.driver = {
		.of_match_table	= tw2866_dvp_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(tw2866_dvp_driver);

MODULE_DESCRIPTION("TW2866-DVP sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


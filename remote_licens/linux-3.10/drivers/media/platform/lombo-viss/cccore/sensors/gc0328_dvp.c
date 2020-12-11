/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * gc0328 dvp sensor driver code for LomboTech
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


static const cc_reg_t init_reg_list[] = {
	{0xfe , 0x80},
	{0xfe , 0x80},
	{0xfc , 0x16},
	{0xfc , 0x16},
	{0xfc , 0x16},
	{0xfc , 0x16},

	{0xfe , 0x00},
	{0x4f , 0x00},
	{0x42 , 0x00},
	{0x03 , 0x00},
	{0x04 , 0xc0},
	{0x77 , 0x62},
	{0x78 , 0x40},
	{0x79 , 0x4d},

	{0x05 , 0x02},
	{0x06 , 0x2c},
	{0x07 , 0x00},
	{0x08 , 0xb8},

	{0xfe , 0x01},
	{0x29 , 0x00},/* anti-flicker step [11:8] */
	{0x2a , 0x60},/* anti-flicker step [7:0] */

	{0x2b , 0x02},/* exp level 0  14.28fps */
	{0x2c , 0xa0},
	{0x2d , 0x03},/* exp level 1  12.50fps */
	{0x2e , 0x00},
	{0x2f , 0x03},/* exp level 2  10.00fps */
	{0x30 , 0xc0},
	{0x31 , 0x05},/* exp level 3  7.14fps */
	{0x32 , 0x40},
	{0xfe , 0x00},

	{0xfe , 0x01},
	{0x4f , 0x00},
	{0x4c , 0x01},
	{0xfe , 0x00},
	/* AWB */
	{0xfe , 0x01},
	{0x51 , 0x80},
	{0x52 , 0x12},
	{0x53 , 0x80},
	{0x54 , 0x60},
	{0x55 , 0x01},
	{0x56 , 0x06},
	{0x5b , 0x02},
	{0x61 , 0xdc},
	{0x62 , 0xdc},
	{0x7c , 0x71},
	{0x7d , 0x00},
	{0x76 , 0x00},
	{0x79 , 0x20},
	{0x7b , 0x00},
	{0x70 , 0xFF},
	{0x71 , 0x00},
	{0x72 , 0x10},
	{0x73 , 0x40},
	{0x74 , 0x40},
	/* awB */
	{0x50 , 0x00},
	{0xfe , 0x01},
	{0x4f , 0x00},
	{0x4c , 0x01},
	{0x4f , 0x00},
	{0x4f , 0x00},
	{0x4f , 0x00},/* wait clr finish  */
	{0x4d , 0x36},/* 40 */
	{0x4e , 0x02},/* SPL D65 */
	{0x4e , 0x02},/* SPL D65 */
	{0x4d , 0x44},/* 40 */
	{0x4e , 0x02},
	{0x4e , 0x02},
	{0x4e , 0x02},/* SPL D65 */
	{0x4e , 0x02},/* SPL D65 */
	{0x4d , 0x53},/* 50 */
	{0x4e , 0x08},/* SPL CWF */
	{0x4e , 0x08},/* DNP */
	{0x4e , 0x02},/* 04 pink DNP */
	{0x4d , 0x63},/* 60 */
	{0x4e , 0x08},/* TL84 */
	{0x4e , 0x08},/* TL84 */
	{0x4d , 0x73},/* 80 */
	{0x4e , 0x20},/* SPL   A */
	{0x4d , 0x83},/* 80 */
	{0x4e , 0x20},/* SPL   A */
	{0x4f , 0x01},

	{0x50 , 0x88},
	{0xfe , 0x00},/* page0 */
	/* BLK */
	{0x27 , 0x00},
	{0x2a , 0x40},
	{0x2b , 0x40},
	{0x2c , 0x40},
	{0x2d , 0x40},
	/* page  0 */
	{0xfe , 0x00},
	{0x0d , 0x01},
	{0x0e , 0xe8},
	{0x0f , 0x02},
	{0x10 , 0x88},
	{0x09 , 0x00},
	{0x0a , 0x00},
	{0x0b , 0x00},
	{0x0c , 0x00},
	{0x16 , 0x00},
	{0x17 , 0x16},/* v flip */
	{0x18 , 0x0e},
	{0x19 , 0x06},

	{0x1b , 0x48},
	{0x1f , 0xC8},
	{0x20 , 0x01},
	{0x21 , 0x78},
	{0x22 , 0xb0},
	{0x23 , 0x04},/* 0x06 20140519 lanking GC0328C */
	{0x24 , 0x11},
	{0x26 , 0x00},

	{0x50 , 0x01},/* crop mode */
	/* global gain for range  */
	{0x70 , 0x85},
	/* block enable */
	{0x40 , 0x7f},
	{0x41 , 0x26},
	{0x42 , 0xff},
	{0x43 , 0x02},/* CbCr fixed enable */
	{0x45 , 0x00},
	{0x44 , 0x02},
	{0x46 , 0x02},

	{0x4b , 0x01},
	{0x50 , 0x01},

	/* dN & EEINTP */
	{0x7e , 0x0a},
	{0x7f , 0x03},
	{0x80 , 0x27},/* 20140915 */
	{0x81 , 0x15},
	{0x82 , 0x90},
	{0x83 , 0x02},
	{0x84 , 0x23},/* 0x22 20140915 */
	{0x90 , 0x2c},
	{0x92 , 0x02},
	{0x94 , 0x02},
	{0x95 , 0x35},

	/* YCP */
	{0xd1 , 0x32},
	{0xd2 , 0x32},
	{0xdd , 0x18},
	{0xde , 0x32},
	{0xe4 , 0x88},
	{0xe5 , 0x40},
	{0xd7 , 0x0e},

	/* GAMMA */
	/* rgb gamma */
	{0xfe , 0x00},
	{0xbf , 0x10},
	{0xc0 , 0x1c},
	{0xc1 , 0x33},
	{0xc2 , 0x48},
	{0xc3 , 0x5a},
	{0xc4 , 0x6b},
	{0xc5 , 0x7b},
	{0xc6 , 0x95},
	{0xc7 , 0xab},
	{0xc8 , 0xbf},
	{0xc9 , 0xcd},
	{0xca , 0xd9},
	{0xcb , 0xe3},
	{0xcc , 0xeb},
	{0xcd , 0xf7},
	{0xce , 0xfd},
	{0xcf , 0xff},

	/* Y gamma  */
	{0xfe , 0x00},
	{0x63 , 0x00},
	{0x64 , 0x05},
	{0x65 , 0x0c},
	{0x66 , 0x1a},
	{0x67 , 0x29},
	{0x68 , 0x39},
	{0x69 , 0x4b},
	{0x6a , 0x5e},
	{0x6b , 0x82},
	{0x6c , 0xa4},
	{0x6d , 0xc5},
	{0x6e , 0xe5},
	{0x6f , 0xFF},

	/* ASDE */
	{0xfe , 0x01},
	{0x18 , 0x02},
	{0xfe , 0x00},
	{0x98 , 0x00},
	{0x9b , 0x20},
	{0x9c , 0x80},
	{0xa4 , 0x10},
	{0xa8 , 0xB0},
	{0xaa , 0x40},
	{0xa2 , 0x23},
	{0xad , 0x01},

	/* AEC    */
	{0xfe , 0x01},
	{0x9c , 0x02},
	{0x08 , 0xa0},
	{0x09 , 0xe8},

	{0x10 , 0x00},
	{0x11 , 0x11},
	{0x12 , 0x10},
	{0x13 , 0x80},/* lanking +2 */
	{0x15 , 0xfc},
	{0x18 , 0x03},
	{0x21 , 0xc0},
	{0x22 , 0x60},
	{0x23 , 0x30},
	{0x25 , 0x00},
	{0x24 , 0x14},

	/* LSC */
	/* gc0328 Alight lsc reg setting list */
	/* Record date: 2013-04-01 15:59:05 */
	{0xfe , 0x01},
	{0xc0 , 0x10},
	{0xc1 , 0x0c},
	{0xc2 , 0x0a},
	{0xc6 , 0x0e},
	{0xc7 , 0x0b},
	{0xc8 , 0x0a},
	{0xba , 0x26},
	{0xbb , 0x1c},
	{0xbc , 0x1d},
	{0xb4 , 0x23},
	{0xb5 , 0x1c},
	{0xb6 , 0x1a},
	{0xc3 , 0x00},
	{0xc4 , 0x00},
	{0xc5 , 0x00},
	{0xc9 , 0x00},
	{0xca , 0x00},
	{0xcb , 0x00},
	{0xbd , 0x00},
	{0xbe , 0x00},
	{0xbf , 0x00},
	{0xb7 , 0x07},
	{0xb8 , 0x05},
	{0xb9 , 0x05},
	{0xa8 , 0x07},
	{0xa9 , 0x06},
	{0xaa , 0x00},
	{0xab , 0x04},
	{0xac , 0x00},
	{0xad , 0x02},
	{0xae , 0x0d},
	{0xaf , 0x05},
	{0xb0 , 0x00},
	{0xb1 , 0x07},
	{0xb2 , 0x03},
	{0xb3 , 0x00},
	{0xa4 , 0x00},
	{0xa5 , 0x00},
	{0xa6 , 0x00},
	{0xa7 , 0x00},
	{0xa1 , 0x3c},
	{0xa2 , 0x50},
	{0xfe , 0x00},

	/* cct */
	{0xB1 , 0x04},
	{0xB2 , 0xfd},
	{0xB3 , 0xfc},
	{0xB4 , 0xf0},
	{0xB5 , 0x05},
	{0xB6 , 0xf0},

	{CC_REG_DELAY, 200},

	{0xfe , 0x00},
	{0x27 , 0xf7},
	{0x28 , 0x7F},
	{0x29 , 0x20},
	{0x33 , 0x20},
	{0x34 , 0x20},
	{0x35 , 0x20},
	{0x36 , 0x20},
	{0x32 , 0x08},

	{0x47 , 0x00},
	{0x48 , 0x00},

	{0xfe , 0x01},
	{0x79 , 0x00},
	{0x7d , 0x00},
	{0x50 , 0x88},
	{0x5b , 0x0c},
	{0x76 , 0x8f},
	{0x80 , 0x70},
	{0x81 , 0x70},
	{0x82 , 0xb0},
	{0x70 , 0xff},
	{0x71 , 0x00},
	{0x72 , 0x28},
	{0x73 , 0x0b},
	{0x74 , 0x0b},

	{0xfe , 0x00},
	{0x70 , 0x45},
	{0x4f , 0x01},
	{0xf1 , 0x07},

	{0xf2 , 0x01},
};

static struct cc_power_act power_up_act[] = {
	{CC_PWDN, CC_UP},
	{CC_DELAY, 5},
	{CC_PWDN,  CC_DOWN},
	{CC_DELAY, 5},

};

static struct cc_power_act power_down_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 5},
	{CC_PWDN,  CC_UP},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define FHD_WIDTH		640
#define FHD_HEIGHT		480

static const struct cc_win_size win_sizes[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	int ret = 0;
	u8 id;

	ret = cc_si->read_reg(cc_si, 0xf0, &id);
	if (ret != 0) {
		PRT_ERR("id=%x\n", id);
		return -ENODEV;
	}

	if (id != 0x9d) {
		PRT_ERR("ID wrong! (0x%02x != 0x9d)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int sensor_init(struct cc_sensor_info *cc_si)
{
	struct timespec ts_s;
	struct timespec ts_f;
	int ret = 0;

	PRT_INFO("\n");

	ret = sensor_detect(cc_si);
	if (ret != 0)
		return ret;

	ktime_get_ts(&ts_s);
#if 0
	ret = cc_si->write_reg_list(cc_si, init_reg_list,
					ARRAY_SIZE(init_reg_list));
	if (ret != 0) {
		PRT_ERR("write %s init_reg_list failed!\n", cc_si->name);
		return -ENODEV;
	}
#endif
	ktime_get_ts(&ts_f);

	PRT_INFO("write %s init_reg_list finish.\n", cc_si->name);
	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

#if 0
	{
	u8 value = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(init_reg_list); i++) {
		if (init_reg_list[i].reg == CC_REG_DELAY)
			continue;

		ret = cc_si->read_reg(cc_si, init_reg_list[i].reg, &value);
		if (ret) {
			PRT_ERR("%s read i %d reg 0x%02x error\n",
				cc_si->name, i, init_reg_list[i].reg);
			return ret;
		}

		if (init_reg_list[i].val != value) {
			PRT_ERR("%s %d 0x%02x (0x%02x) != (0x%02x)\n",
				cc_si->name, i, init_reg_list[i].reg,
				value, init_reg_list[i].val);
		}
	}
	}
#endif

	return 0;
}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;
	PRT_INFO("*************************on:%d\n", on);

	if (on) {
#if 1
		ret = cc_si->write_reg_list(cc_si, init_reg_list,
						ARRAY_SIZE(init_reg_list));
		if (ret != 0) {
			PRT_ERR("write %s init_reg_list failed!\n",
				cc_si->name);
			return -ENODEV;
		}
#endif
	}

	return ret;
}

/*
 * ctrl for sensor start
 */
static int gc0328_set_ae_target(struct cc_sensor_info *cc_si, int val)
{
	int ret = 0;

	return ret;

	ret += cc_si->write_reg(cc_si, 0xfe, 0x1);
	ret += cc_si->write_reg(cc_si, 0x13, val);

	PRT_DBG("target: 0x%x\n", val);
	return ret;
}

static long gc0328_set_ae_meter(struct cc_sensor_info *cc_si, void *arg)
{
	struct viss_isp_ae_para *ae_para = (struct viss_isp_ae_para *)arg;
	long ret = 0;

	return ret;

	if (ae_para->ae_mode ==	V4L2_EXPOSURE_METERING_CENTER_WEIGHTED) {
		ret += cc_si->write_reg(cc_si, 0xfe, 0x1);
		ret += cc_si->write_reg(cc_si, 0x06, ae_para->rect.left/4);
		ret += cc_si->write_reg(cc_si, 0x07, ae_para->rect.top/4);
		ret += cc_si->write_reg(cc_si, 0x08, ae_para->rect.width/4);
		ret += cc_si->write_reg(cc_si, 0x09, ae_para->rect.height/4);
	} else {
		ret += cc_si->write_reg(cc_si, 0xfe, 0x1);
		ret += cc_si->write_reg(cc_si, 0x06, 0);
		ret += cc_si->write_reg(cc_si, 0x07, 0);
		ret += cc_si->write_reg(cc_si, 0x08, 0xa0);
		ret += cc_si->write_reg(cc_si, 0x09, 0x78);
	}
	return ret;
}

static int gc0328_get_exif(struct cc_sensor_info *cc_si,
			struct viss_isp_exif *exif)
{
	int ret = 0;
	u8 tmp;
	u16 exp_line = 0;

	ret += cc_si->write_reg(cc_si, 0xfe, 0x1);

	ret = cc_si->read_reg(cc_si, 0x14, &tmp);/* average lum */
	exif->res[2] = tmp;

	ret = cc_si->read_reg(cc_si, 0x13, &tmp);/* Y target */
	exif->res[4] = tmp;

	ret += cc_si->write_reg(cc_si, 0xfe, 0x0);

	ret = cc_si->read_reg(cc_si, 0x71, &tmp);/* pre_gain */
	exif->iso_speed = tmp * 100 / 16;

	ret = cc_si->read_reg(cc_si, 0x03, &tmp);/* exposure */
	exp_line = tmp;

	ret = cc_si->read_reg(cc_si, 0x04, &tmp);/* exposure */
	exp_line = (exp_line << 8) | tmp;

	exif->exposure_time = exp_line * 333 / (488+44);/* 10000 / 30fps */

	return ret;
}

/*
 * V4L2 subdev internal operations
 */
static long sensor_ioctl(struct cc_sensor_info *cc_si, unsigned int cmd,
			void *arg)
{
	long ret = 0;

	switch (cmd) {
	case VIDIOC_ISP_SET_AE_METER:
		ret = gc0328_set_ae_meter(cc_si, arg);
		break;
	case VIDIOC_ISP_SET_AE_TARGET:
		gc0328_set_ae_target(cc_si, *(unsigned int *) arg);
		break;
	case VIDIOC_VISS_ISP_EXIF_REQ:
		gc0328_get_exif(cc_si, (struct viss_isp_exif *)arg);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

#define DRIVER_NAME "gc0328_dvp"

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
	cc_si->sensor_ioctl = sensor_ioctl;

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

static const struct of_device_id gc0328_dvp_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, gc0328_dvp_of_match);

static struct platform_driver gc0328_dvp_driver = {
	.driver = {
		.of_match_table	= gc0328_dvp_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(gc0328_dvp_driver);

MODULE_DESCRIPTION("GC0328-DVP sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * gc1054 mipi sensor driver code for LomboTech
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
	/*Actual_window_size=1920*720,MIPI 1lane
	 *Mclk=24Mhz，MIPI_clcok=312Mhz,Frame_rate=30fps
	 *HD=1726,VD=750,row_time=44.25us
	 */

	/* SYS */
	{0xf2, 0x00},
	{0xf6, 0x00},
	{0xfc, 0x04},
	{0xf7, 0x01},
	{0xf8, 0x0c},
	{0xf9, 0x06},
	{0xfa, 0x80},
	{0xfc, 0x0e},
	/* ANALOG & CISCTL */
	{0xfe, 0x00},
	{0x03, 0x02},
	{0x04, 0xa6},
	{0x05, 0x02},/* HB */
	{0x06, 0x07},
	{0x07, 0x00},/* VB */
	{0x08, 0x0a},
	{0x09, 0x00},
	{0x0a, 0x04},/* row start */
	{0x0b, 0x00},
	{0x0c, 0x00},/* col start */
	{0x0d, 0x02},
	{0x0e, 0xd4},/* height 724 */
	{0x0f, 0x05},
	{0x10, 0x08},/* width 1288 */
	{0x17, 0xc2},/* v flip */
	{0x18, 0x02},
	{0x19, 0x08},
	{0x1a, 0x18},
	{0x1d, 0x12},
	{0x1e, 0x50},
	{0x1f, 0x80},
	{0x21, 0x30},
	{0x23, 0xf8},
	{0x25, 0x10},
	{0x28, 0x20},
	{0x34, 0x08},/* data low */
	{0x3c, 0x10},
	{0x3d, 0x0e},
	{0xcc, 0x8e},
	{0xcd, 0x9a},
	{0xcf, 0x70},
	{0xd0, 0xa9},
	{0xd1, 0xc5},
	{0xd2, 0xed},/* data high */
	{0xd8, 0x3c},/* dacin offset */
	{0xd9, 0x7a},
	{0xda, 0x12},
	{0xdb, 0x50},
	{0xde, 0x0c},
	{0xe3, 0x60},
	{0xe4, 0x78},
	{0xfe, 0x01},
	{0xe3, 0x01},
	{0xe6, 0x10},/* ramps offset */
	/* ISP */
	{0xfe, 0x01},
	{0x80, 0x50},
	{0x88, 0x73},
	{0x89, 0x03},
	{0x90, 0x01},
	{0x92, 0x02},/* crop win 2<=y<=4 */
	{0x94, 0x03},/* crop win 2<=x<=5 */
	{0x95, 0x02},/* crop win height */
	{0x96, 0xd0},
	{0x97, 0x05},/* crop win width */
	{0x98, 0x00},
	/* BLK */
	{0xfe, 0x01},
	{0x40, 0x22},
	{0x43, 0x03},
	{0x4e, 0x3c},
	{0x4f, 0x00},
	{0x60, 0x00},
	{0x61, 0x80},
	/* GAIN */
	{0xfe, 0x01},
	{0xb0, 0x48},
	{0xb1, 0x01},
	{0xb2, 0x00},
	{0xb6, 0x00},
	{0xfe, 0x02},
	{0x01, 0x00},
	{0x02, 0x01},
	{0x03, 0x02},
	{0x04, 0x03},
	{0x05, 0x04},
	{0x06, 0x05},
	{0x07, 0x06},
	{0x08, 0x0e},
	{0x09, 0x16},
	{0x0a, 0x1e},
	{0x0b, 0x36},
	{0x0c, 0x3e},
	{0x0d, 0x56},
	{0xfe, 0x02},
	{0xb0, 0x00},/* col_gain[11:8] */
	{0xb1, 0x00},
	{0xb2, 0x00},
	{0xb3, 0x11},
	{0xb4, 0x22},
	{0xb5, 0x54},
	{0xb6, 0xb8},
	{0xb7, 0x60},
	{0xb9, 0x00},/* col_gain[12] */
	{0xba, 0xc0},
	{0xc0, 0x20},/* col_gain[7:0] */
	{0xc1, 0x2d},
	{0xc2, 0x40},
	{0xc3, 0x5b},
	{0xc4, 0x80},
	{0xc5, 0xb5},
	{0xc6, 0x00},
	{0xc7, 0x6a},
	{0xc8, 0x00},
	{0xc9, 0xd4},
	{0xca, 0x00},
	{0xcb, 0xa8},
	{0xcc, 0x00},
	{0xcd, 0x50},
	{0xce, 0x00},
	/* DARKSUN */
	{0xcf, 0xa1},
	{0xfe, 0x02},
	{0x54, 0xf7},
	{0x55, 0xf0},
	{0x56, 0x00},
	{0x57, 0x00},
	{0x58, 0x00},
	{0x5a, 0x04},
	/* DD */
	{0xfe, 0x04},
	{0x81, 0x8a},
	/* MIPI */
	{0xfe, 0x03},
	{0x01, 0x03},
	{0x02, 0x11},
	{0x03, 0x90},
	{0x10, 0x90},
	{0x11, 0x2b},
	{0x12, 0x40},/* lwc 1280*5/4 */
	{0x13, 0x06},
	{0x15, 0x06},
	{0x21, 0x02},
	{0x22, 0x02},
	{0x23, 0x08},
	{0x24, 0x02},
	{0x25, 0x10},
	{0x26, 0x04},
	{0x29, 0x02},
	{0x2a, 0x02},
	{0x2b, 0x04},
	{0xfe, 0x00},
};

static struct cc_power_act power_up_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 10},
	{CC_RESET,  CC_UP},
	{CC_DELAY, 10},
};

static struct cc_power_act power_down_act[] = {
	{CC_PWDN, CC_UP},
	{CC_DELAY, 5},
	{CC_RESET,  CC_DOWN},
	{CC_DELAY, 5},
	{CC_PWDN, CC_UP},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_SGBRG10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define FHD_WIDTH		1280
#define FHD_HEIGHT		720

static const struct cc_win_size win_sizes[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.vts = 750,
		.hts = 1726,
		.pclk = 39 * 1000 * 1000,/* vts*hts*framerate */
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
		.mfreq = 312000000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = cc_si->read_reg(cc_si, 0xf0, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0xf1, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x1054) {
		PRT_ERR("ID wrong! (0x%04x != 0x1054))\n", id);
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


static int get_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u8 val = 0;
	int ret = 0;

	/* read */
	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &val);
	reg_data->reg_value = val;
#if 0
	PRT_INFO("addr: 0x%x, val: 0x%02x.\n",
		reg_data->reg_add, reg_data->reg_value);
#endif
	return ret;
}

static int set_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u8 val = reg_data->reg_value;
	u8 read_val;
	int ret;

	/* write */
	ret = cc_si->write_reg(cc_si, reg_data->reg_add, val);
	PRT_DBG("addr=0x%x,val=0x%02x.\n",
		reg_data->reg_add, reg_data->reg_value);

	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &read_val);
	if (read_val != val)
		PRT_DBG("ret=%d,reg_add=0x%x,val=0x%02x,read_val=0x%02x\n",
			ret, reg_data->reg_add, val, read_val);

	return ret;
}

static int viss_get_sensor_fmt_info(struct cc_sensor_info *cc_si,
		struct viss_isp_sensor_fmt_info *sensor_fmt_info)
{
	int ret = 0;

	sensor_fmt_info->vts = cc_si->cur_wsize->vts;
	sensor_fmt_info->hts = cc_si->cur_wsize->hts;
	sensor_fmt_info->pclk = cc_si->cur_wsize->pclk;
	sensor_fmt_info->hoffset = cc_si->cur_wsize->left;
	sensor_fmt_info->voffset = cc_si->cur_wsize->top;
	sensor_fmt_info->width = cc_si->cur_wsize->width;
	sensor_fmt_info->height = cc_si->cur_wsize->height;
	sensor_fmt_info->bayer_mode = ISP_PATTERN_GBRG;
	sensor_fmt_info->buswidth = ISP_INPUT_10BITS;

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;

	ret = cc_si->write_reg(cc_si, 0xfe, 0);
	ret = cc_si->write_reg(cc_si, 0x03, (shutter_time >> 8) & 0xff);
	ret = cc_si->write_reg(cc_si, 0x04, (shutter_time >> 0) & 0xff);

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, __u32 gain)
{
	int ret = 0;
	u32 again, dgain;

	if (gain < 4*256) {
		again = gain/4;
		dgain = 0x100;
	} else if (gain < 64*256) {
		again = 0xff;
		dgain = gain * 256 / (again * 4);
	} else {
		again = 0xff;
		dgain = 0xfff;
	}

	ret = cc_si->write_reg(cc_si, 0xfe, 1);
	ret = cc_si->write_reg(cc_si, 0xb0, again);

	ret = cc_si->write_reg(cc_si, 0xb1, (dgain >> 8) & 0xff);
	ret = cc_si->write_reg(cc_si, 0xb2, (dgain >> 0) & 0xff);

	return ret;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

	ret = viss_isp_set_shutter(cc_si, exp_gain->exp_time / 16);
	if (ret < 0) {
		PRT_ERR("viss_isp_set_shutter failed %d.\n", ret);
		PRT_ERR("viss_isp_exp_ctrl %u, %u.\n",
			exp_gain->exp_time, exp_gain->exp_gain);
		return -ENODEV;
	}

	ret = viss_isp_set_gain(cc_si, exp_gain->exp_gain);
	if (ret < 0) {
		PRT_ERR("viss_isp_set_gain failed %d.\n", ret);
		PRT_ERR("viss_isp_exp_ctrl %u, %u.\n",
			exp_gain->exp_time, exp_gain->exp_gain);
		return -ENODEV;
	}

	return ret;
}

static long sensor_ioctl(struct cc_sensor_info *cc_si, unsigned int cmd,
				void *arg)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_VISS_GET_SENSOR:
		ret = get_reg(cc_si, (struct viss_isp_reg_data *)arg);
		break;
	case VIDIOC_VISS_SET_SENSOR:
		ret = set_reg(cc_si, (struct viss_isp_reg_data *)arg);
		break;
	case VIDIOC_VISS_ISP_EXP_CTRL:
		ret = viss_isp_exp_ctrl(cc_si,
			(struct viss_isp_exp_gain *)arg);
		break;
	case VIDIOC_VISS_ISP_SENSOR_FMT_INFO:
		ret = viss_get_sensor_fmt_info(cc_si,
			(struct viss_isp_sensor_fmt_info *)arg);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

/*
 * ctrl for sensor finish
 */

#define DRIVER_NAME "gc1054_mipi"

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

	cc_si->dlanes = 1;

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

static const struct of_device_id gc1054_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, gc1054_mipi_of_match);

static struct platform_driver gc1054_mipi_driver = {
	.driver = {
		.of_match_table	= gc1054_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(gc1054_mipi_driver);

MODULE_DESCRIPTION("GC1054-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


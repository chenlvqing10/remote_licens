/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * f37 ahd chip driver code for LomboTech
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

/*
 * 2lane init
 */
static const cc_reg_t init_reg_list[] = {
	{0x12, 0x60},
	{0x48, 0x8A},
	{0x48, 0x0A},
	{0x0E, 0x11},
	{0x0F, 0x14},
	{0x10, 0x36},
	{0x11, 0x80},
	{0x0D, 0xF0},
	{0x5F, 0x41},
	{0x60, 0x20},
	{0x58, 0x12},
	{0x57, 0x60},
	{0x9D, 0x00},
	{0x20, 0x80},
	{0x21, 0x07},
	{0x22, 0x65},
	{0x23, 0x04},
	{0x24, 0xC0},
	{0x25, 0x38},
	{0x26, 0x43},
	{0x27, 0x1A},
	{0x28, 0x15},
	{0x29, 0x07},
	{0x2A, 0x0A},
	{0x2B, 0x17},
	{0x2C, 0x00},
	{0x2D, 0x00},
	{0x2E, 0x14},
	{0x2F, 0x44},
	{0x41, 0xC8},
	{0x42, 0x3B},
	{0x47, 0x42},
	{0x76, 0x60},
	{0x77, 0x09},
	{0x1D, 0x00},
	{0x1E, 0x04},
	{0x6C, 0x40},
	{0x6E, 0x2C},
	{0x70, 0xDC},
	{0x71, 0xD3},
	{0x72, 0xD4},
	{0x73, 0x58},
	{0x74, 0x02},
	{0x78, 0x96},
	{0x89, 0x01},
	{0x6B, 0x20},
	{0x86, 0x40},
	{0x31, 0x0C},
	{0x32, 0x38},
	{0x33, 0x6C},
	{0x34, 0x88},
	{0x35, 0x88},
	{0x3A, 0xAF},
	{0x3B, 0x00},
	{0x3C, 0x57},
	{0x3D, 0x78},
	{0x3E, 0xFF},
	{0x3F, 0xF8},
	{0x40, 0xFF},
	{0x56, 0xB2},
	{0x59, 0xE8},
	{0x5A, 0x04},
	{0x85, 0x70},
	{0x8A, 0x04},
	{0x91, 0x13},
	{0x9B, 0x03},
	{0x9C, 0xE1},
	{0xA9, 0x78},
	{0x5B, 0xB0},
	{0x5C, 0x71},
	{0x5D, 0xF6},
	{0x5E, 0x14},
	{0x62, 0x01},
	{0x63, 0x0F},
	{0x64, 0xC0},
	{0x65, 0x02},
	{0x67, 0x65},
	{0x66, 0x04},
	{0x68, 0x00},
	{0x69, 0x7C},
	{0x6A, 0x12},
	{0x7A, 0x80},
	{0x82, 0x21},
	{0x8F, 0x91},
	{0xAE, 0x30},
	{0x13, 0x81},
	{0x96, 0x04},
	{0x4A, 0x05},
	{0x7E, 0xCD},
	{0x50, 0x02},
	{0x49, 0x10},
	{0xAF, 0x12},
	{0x80, 0x41},
	{0x7B, 0x4A},
	{0x7C, 0x08},
	{0x7F, 0x57},
	{0x90, 0x00},
	{0x8C, 0xFF},
	{0x8D, 0xC7},
	{0x8E, 0x00},
	{0x8B, 0x01},
	{0x0C, 0x00},
	{0x81, 0x74},
	{0x19, 0x20},
	{0x46, 0x00},
	{0x12, 0x20},
	{0x48, 0x8A},
	{0x48, 0x0A},
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN, CC_UP},
	{CC_DELAY, 10},
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 20},
	{CC_RESET, CC_UP},
	{CC_DELAY, 20},
};

static struct cc_power_act power_down_act[] = {
	{CC_PWDN, CC_UP},
	{CC_DELAY, 10},
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_SGRBG10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define FHD_WIDTH		1920
#define FHD_HEIGHT		1080
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
		.mfreq = 648*1000*1000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 id_hi;
	u8 id_low;
	u16 id;
	int ret = 0;

	ret = cc_si->read_reg(cc_si, 0x0A, &id_hi);
	if (ret != 0) {
		PRT_ERR("id_hi =%x\n", id_hi);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0x0B, &id_low);
	if (ret != 0) {
		PRT_ERR("id_low =%x\n", id_low);
		return -ENODEV;
	}

	id = (id_hi << 8) | id_low;
	if (id != 0xf37) {
		PRT_ERR("ID wrong! (0x%x)\n", id);
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
#if 0
	ret = cc_si->write_reg_list(cc_si, init_reg_list,
					ARRAY_SIZE(init_reg_list));
	if (ret != 0) {
		PRT_ERR("write %s init_reg_list failed!\n", cc_si->name);
		return -ENODEV;
	}
#endif
	ktime_get_ts(&ts_f);

	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

	return 0;
}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;

	PRT_INFO("*************************on:%d\n", on);

	ret = cc_si->write_reg_list(cc_si, init_reg_list,
					ARRAY_SIZE(init_reg_list));

	return ret;
}

/*
 * ctrl for sensor start
 */

static int get_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u8 val = 0;
	int ret = 0;

	/* read */
	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &val);
	reg_data->reg_value = val;

	PRT_INFO("addr: 0x%x, val: 0x%02x.\n",
		reg_data->reg_add, reg_data->reg_value);

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

	sensor_fmt_info->pclk = 149*1000*1000;
	sensor_fmt_info->vts = 1125;
	sensor_fmt_info->hts = 4400;
	sensor_fmt_info->hoffset = 0;
	sensor_fmt_info->voffset = 0;
	sensor_fmt_info->width = 1920;
	sensor_fmt_info->height = 1080;
	sensor_fmt_info->bayer_mode = ISP_PATTERN_BGGR;
	sensor_fmt_info->buswidth = ISP_INPUT_10BITS;

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;

	ret = cc_si->write_reg(cc_si, 0x02, (shutter_time >> 8) & 0xff);
	ret = cc_si->write_reg(cc_si, 0x01, (shutter_time >> 0) & 0xff);

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, int gain_val)
{
	u8 a_gain = 0, mult_gain = 0;
	int ret;

	a_gain = gain_val;

	if (a_gain < 32) {
		mult_gain = 0U;
	} else if (a_gain < 64) {
		mult_gain = 0x1;
		a_gain /= 2;
	} else if (a_gain < 128) {
		mult_gain = 0x2;
		a_gain /= 4;
	} else if (a_gain < 256) {
		mult_gain = 0x3;
		a_gain /= 8;
	} else if (a_gain < 512) {
		mult_gain = 0x4;
		a_gain /= 16;
	} else {
		mult_gain = 0x4;
		a_gain = 31;
	}

	ret += cc_si->write_reg(cc_si, 0x00,
				(mult_gain << 4) | (a_gain - 16));

	return 0;
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

	ret = viss_isp_set_gain(cc_si, exp_gain->exp_gain / 16);
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


#define DRIVER_NAME "f37_mipi"

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

	cc_si->dlanes = 2;

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

static const struct of_device_id f37_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, f37_mipi_of_match);

static struct platform_driver f37_mipi_driver = {
	.driver = {
		.of_match_table	= f37_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(f37_mipi_driver);

MODULE_DESCRIPTION("F37-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


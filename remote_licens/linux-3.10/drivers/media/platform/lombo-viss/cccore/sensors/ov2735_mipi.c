/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ov2735 mipi sensor driver code for LomboTech
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

/* MIPI_1920x1080_2lane_30fps_84M */
static const cc_reg_t init_reg_list[] = {
	{ 0xfd, 0x00 },
	/* { 0x20, 0x00 },soft reset */

	{ 0xfd, 0x00 },
	{ 0x2f, 0x10 },
	{ 0x34, 0x00 },
	{ 0x30, 0x15 },
	{ 0x33, 0x01 },
	{ 0x35, 0x20 },

	{ 0xfd, 0x01 },
	{ 0x0d, 0x00 },
	{ 0x30, 0x00 },
	{ 0x03, 0x01 },
	{ 0x04, 0x8f },
	{ 0x01, 0x01 },
	{ 0x09, 0x00 },
	{ 0x0a, 0x20 },
	{ 0x06, 0x0a },
	{ 0x24, 0x10 },
	{ 0x01, 0x01 },
	{ 0xfb, 0x73 },
	{ 0x01, 0x01 },

	{ 0xfd, 0x01 },
	{ 0x1a, 0x6b },
	{ 0x1c, 0xea },
	{ 0x16, 0x0c },
	{ 0x21, 0x00 },
	{ 0x11, 0x63 },
	{ 0x19, 0xc3 },
	{ 0x26, 0xda },
	{ 0x29, 0x01 },
	{ 0x33, 0x6f },
	{ 0x2a, 0xd2 },
	{ 0x2c, 0x40 },
	{ 0xd0, 0x02 },
	{ 0xd1, 0x01 },
	{ 0xd2, 0x20 },
	{ 0xd3, 0x04 },
	{ 0xd4, 0x2a },
	{ 0x50, 0x00 },
	{ 0x51, 0x2c },
	{ 0x52, 0x29 },
	{ 0x53, 0x00 },
	{ 0x55, 0x44 },
	{ 0x58, 0x29 },
	{ 0x5a, 0x00 },
	{ 0x5b, 0x00 },
	{ 0x5d, 0x00 },
	{ 0x64, 0x2f },
	{ 0x66, 0x62 },
	{ 0x68, 0x5b },
	{ 0x75, 0x46 },
	{ 0x76, 0xf0 },
	{ 0x77, 0x4f },
	{ 0x78, 0xef },
	{ 0x72, 0xcf },
	{ 0x73, 0x36 },
	{ 0x7d, 0x0d },
	{ 0x7e, 0x0d },
	{ 0x8a, 0x77 },
	{ 0x8b, 0x77 },

	{ 0xfd, 0x01 },
	{ 0xb1, 0x83 },/* DPHY enable 8b */
	{ 0xb3, 0x0b },/* 0b;09;1d */
	{ 0xb4, 0x14 },/* MIPI PLL enable;14;35;36 */
	{ 0x9d, 0x40 },/* mipi hs dc level 40/03/55 */
	{ 0xa1, 0x05 },/* speed/03 */
	{ 0x94, 0x44 },/* dphy time */
	{ 0x95, 0x33 },/* dphy time */
	{ 0x96, 0x1f },/* dphy time */
	{ 0x98, 0x45 },/* dphy time */
	{ 0x9c, 0x10 },/* dphy time */
	{ 0xb5, 0x70 },/* 30 */

	{ 0x25, 0xe0 },
	{ 0x20, 0x7b },
	{ CC_REG_DELAY, 10 },
	{ 0x8f, 0x88 },
	{ 0x91, 0x40 },
	{ 0xfd, 0x01 },
	{ 0xfd, 0x02 },
	{ 0x5e, 0x03 },
	{ 0xa1, 0x04 },
	{ 0xa3, 0x40 },
	{ 0xa5, 0x02 },
	{ 0xa7, 0xc4 },

	{ 0xfd, 0x01 },
	{ 0x86, 0x78 },
	{ 0x89, 0x78 },
	{ 0x87, 0x6b },
	{ 0x88, 0x6b },
	{ 0xfc, 0xe0 },
	{ 0xfe, 0xe0 },
	{ 0xf0, 0x40 },
	{ 0xf1, 0x40 },
	{ 0xf2, 0x40 },
	{ 0xf3, 0x40 },

	{ 0xfd, 0x02 },/* crop to 1920x1080 */
	{ 0xa0, 0x00 },/* Image vertical start MSB3bits */
	{ 0xa1, 0x08 },/* Image vertical start LSB8bits */
	{ 0xa2, 0x04 },/* image vertical size  MSB8bits */
	{ 0xa3, 0x38 },/* image vertical size  LSB8bits */
	{ 0xa4, 0x00 },
	{ 0xa5, 0x04 },/* H start 8Lsb, keep center */
	{ 0xa6, 0x03 },
	{ 0xa7, 0xc0 },/* Half H size Lsb8bits */

	{ 0xfd, 0x01 },
	{ 0x8e, 0x07 },
	{ 0x8f, 0x80 },/* MIPI column number */
	{ 0x90, 0x04 },/* MIPI row number */
	{ 0x91, 0x38 },

	{ 0xfd, 0x03 },
	{ 0xc0, 0x01 },/* enable transfer OTP BP information */

	{ 0xfd, 0x04 },
	{ 0x22, 0x14 },
	{ 0x23, 0x14 },/* enhance normal and dummy BPC */

	{ 0xfd, 0x01 },
	{ 0x06, 0xe0 },/* insert dummy line , the frame rate is 30.01 */
	{ 0x01, 0x01 },
	{ 0xa0, 0x01 },/* MIPI enable, stream on */
};

static struct cc_power_act power_up_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 10},
	{CC_RESET,  CC_UP},
	{CC_DELAY, 10},
	{CC_RESET,  CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN, CC_UP},
	{CC_DELAY, 10},
};

static struct cc_power_act power_down_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 5},
	{CC_RESET,  CC_UP},
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
		.mfreq = 420000000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = cc_si->read_reg(cc_si, 0x02, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0x03, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x2735) {
		PRT_ERR("ID wrong! (0x%04x != 0x2735)\n", id);
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

	sensor_fmt_info->pclk = 75 * 1000 * 1000;
	sensor_fmt_info->vts = 1125;
	sensor_fmt_info->hts = 2222;
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

	shutter_time = shutter_time / 16;

	if (shutter_time < 4)
		shutter_time = 4;

	ret = cc_si->write_reg(cc_si, 0x04, ((shutter_time >> 0) & 0xff));
	ret = cc_si->write_reg(cc_si, 0x03, ((shutter_time >> 8) & 0xff));

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, __u32 gain)
{
	int ret = 0;

	gain = gain / 16;

	ret = cc_si->write_reg(cc_si, 0x24, gain);

	return ret;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

	ret = cc_si->write_reg(cc_si, 0x01, 0);
	ret = cc_si->write_reg(cc_si, 0xfd, 1);

	ret = viss_isp_set_shutter(cc_si, exp_gain->exp_time);
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
	ret = cc_si->write_reg(cc_si, 0x01, 1);

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

#define DRIVER_NAME "ov2735_mipi"

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

static const struct of_device_id ov2735_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov2735_mipi_of_match);

static struct platform_driver ov2735_mipi_driver = {
	.driver = {
		.of_match_table	= ov2735_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov2735_mipi_driver);

MODULE_DESCRIPTION("OV2735-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


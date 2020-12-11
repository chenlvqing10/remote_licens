/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * os02k10 driver code for LomboTech
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

#define VMAX (0x8CA)

/* os02k10_1080p_linearHCG_30fps_10b_2lane_756Mbps */
static const cc_reg_t init_reg_list[] = {
	{ 0x302a, 0x00 },
	{ 0x0103, 0x01 },
	{ 0x0109, 0x01 },
	{ 0x0104, 0x02 },
	{ 0x0102, 0x00 },
	{ 0x0305, 0x5c },
	{ 0x0306, 0x00 },
	{ 0x0307, 0x00 },
	{ 0x0317, 0x09 },
	{ 0x0323, 0x07 },
	{ 0x0324, 0x01 },
	{ 0x0325, 0xb0 },
	{ 0x0327, 0x07 },
	{ 0x032d, 0x02 },
	{ 0x300f, 0x11 },
	{ 0x3012, 0x21 },
	{ 0x3026, 0x10 },
	{ 0x3027, 0x08 },
	{ 0x302d, 0x24 },
	{ 0x3103, 0x29 },
	{ 0x3106, 0x11 },
	{ 0x3400, 0x00 },
	{ 0x3406, 0x08 },
	{ 0x3408, 0x05 },
	{ 0x340c, 0x05 },
	{ 0x3425, 0x51 },
	{ 0x3426, 0x10 },
	{ 0x3427, 0x14 },
	{ 0x3428, 0x10 },
	{ 0x3429, 0x10 },
	{ 0x342a, 0x10 },
	{ 0x342b, 0x04 },
	{ 0x3504, 0x08 },
	{ 0x3508, 0x01 },
	{ 0x3509, 0x00 },
	{ 0x3544, 0x08 },
	{ 0x3548, 0x01 },
	{ 0x3549, 0x00 },
	{ 0x3584, 0x08 },
	{ 0x3588, 0x01 },
	{ 0x3589, 0x00 },
	{ 0x3601, 0x70 },
	{ 0x3604, 0xe3 },
	{ 0x3605, 0x7f },
	{ 0x3606, 0x00 },
	{ 0x3608, 0xa8 },
	{ 0x360a, 0xd0 },
	{ 0x360b, 0x08 },
	{ 0x360e, 0xc8 },
	{ 0x360f, 0x66 },
	{ 0x3610, 0x81 },
	{ 0x3611, 0x89 },
	{ 0x3612, 0x4e },
	{ 0x3613, 0xbd },
	{ 0x362a, 0x0e },
	{ 0x362b, 0x0e },
	{ 0x362c, 0x0e },
	{ 0x362d, 0x0e },
	{ 0x362e, 0x0c },
	{ 0x362f, 0x1a },
	{ 0x3630, 0x32 },
	{ 0x3631, 0x64 },
	{ 0x3638, 0x00 },
	{ 0x3643, 0x00 },
	{ 0x3644, 0x00 },
	{ 0x3645, 0x00 },
	{ 0x3646, 0x00 },
	{ 0x3647, 0x00 },
	{ 0x3648, 0x00 },
	{ 0x3649, 0x00 },
	{ 0x364a, 0x04 },
	{ 0x364c, 0x0e },
	{ 0x364d, 0x0e },
	{ 0x364e, 0x0e },
	{ 0x364f, 0x0e },
	{ 0x3650, 0xff },
	{ 0x3651, 0xff },
	{ 0x3661, 0x07 },
	{ 0x3662, 0x02 },
	{ 0x3663, 0x20 },
	{ 0x3665, 0x12 },
	{ 0x3667, 0xd4 },
	{ 0x3668, 0x80 },
	{ 0x366f, 0x00 },
	{ 0x3671, 0x08 },
	{ 0x3673, 0x2a },
	{ 0x3681, 0x80 },
	{ 0x3700, 0x26 },
	{ 0x3701, 0x1e },
	{ 0x3702, 0x25 },
	{ 0x3703, 0x28 },
	{ 0x3706, 0x3e },
	{ 0x3707, 0x0a },
	{ 0x3708, 0x36 },
	{ 0x3709, 0x55 },
	{ 0x370a, 0x00 },
	{ 0x370b, 0xa3 },
	{ 0x3714, 0x01 },
	{ 0x371b, 0x16 },
	{ 0x371c, 0x00 },
	{ 0x371d, 0x08 },
	{ 0x3756, 0x9b },
	{ 0x3757, 0x9b },
	{ 0x3762, 0x1d },
	{ 0x376c, 0x00 },
	{ 0x3776, 0x05 },
	{ 0x3777, 0x22 },
	{ 0x3779, 0x60 },
	{ 0x377c, 0x48 },
	{ 0x379c, 0x4d },
	{ 0x3784, 0x06 },
	{ 0x3785, 0x0a },
	{ 0x37d1, 0x3e },
	{ 0x37d2, 0x00 },
	{ 0x37d3, 0xa3 },
	{ 0x37d5, 0x3e },
	{ 0x37d6, 0x00 },
	{ 0x37d7, 0xa3 },
	{ 0x37d8, 0x01 },
	{ 0x37dc, 0x00 },
	{ 0x3790, 0x10 },
	{ 0x3793, 0x04 },
	{ 0x3794, 0x07 },
	{ 0x3796, 0x00 },
	{ 0x3797, 0x02 },
	{ 0x37a1, 0x80 },
	{ 0x37bb, 0x88 },
	{ 0x37be, 0x01 },
	{ 0x37bf, 0x00 },
	{ 0x37c0, 0x01 },
	{ 0x37c7, 0x56 },
	{ 0x37ca, 0x21 },
	{ 0x37cc, 0x13 },
	{ 0x37cd, 0x90 },
	{ 0x37cf, 0x02 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x04 },
	{ 0x3804, 0x07 },
	{ 0x3805, 0x8f },
	{ 0x3806, 0x04 },
	{ 0x3807, 0x43 },
	{ 0x3808, 0x07 },
	{ 0x3809, 0x80 },
	{ 0x380a, 0x04 },
	{ 0x380b, 0x38 },
	{ 0x380c, 0x04 },
	{ 0x380d, 0x38 },
	{ 0x380e, 0x04 },
	{ 0x380f, 0xe2 },
	{ 0x3811, 0x08 },
	{ 0x3813, 0x04 },
	{ 0x3814, 0x01 },
	{ 0x3815, 0x01 },
	{ 0x3816, 0x01 },
	{ 0x3817, 0x01 },
	{ 0x381c, 0x00 },
	{ 0x3820, 0x02 },
	{ 0x3821, 0x00 },
	{ 0x3822, 0x14 },
	{ 0x384c, 0x04 },
	{ 0x384d, 0x38 },
	{ 0x3858, 0x0d },
	{ 0x3865, 0x00 },
	{ 0x3866, 0xc0 },
	{ 0x3867, 0x00 },
	{ 0x3868, 0xc0 },
	{ 0x3900, 0x13 },
	{ 0x3940, 0x13 },
	{ 0x3980, 0x13 },
	{ 0x3c01, 0x11 },
	{ 0x3c05, 0x00 },
	{ 0x3c0f, 0x1c },
	{ 0x3c12, 0x0d },
	{ 0x3c19, 0x01 },
	{ 0x3c21, 0x40 },
	{ 0x3c3b, 0x18 },
	{ 0x3c3d, 0xc9 },
	{ 0x3c55, 0xcb },
	{ 0x3c5d, 0xcf },
	{ 0x3c5e, 0xcf },
	{ 0x3ce0, 0x00 },
	{ 0x3ce1, 0x00 },
	{ 0x3ce2, 0x00 },
	{ 0x3ce3, 0x00 },
	{ 0x3d8c, 0x70 },
	{ 0x3d8d, 0x10 },
	{ 0x4001, 0x2f },
	{ 0x4033, 0x80 },
	{ 0x4008, 0x02 },
	{ 0x4009, 0x11 },
	{ 0x4004, 0x00 },
	{ 0x4005, 0x40 },
	{ 0x400a, 0x01 },
	{ 0x400b, 0x3c },
	{ 0x410f, 0x01 },
	{ 0x402e, 0x00 },
	{ 0x402f, 0x40 },
	{ 0x4030, 0x00 },
	{ 0x4031, 0x40 },
	{ 0x4032, 0x0f },
	{ 0x4050, 0x00 },
	{ 0x4051, 0x07 },
	{ 0x4288, 0xcf },
	{ 0x4289, 0x03 },
	{ 0x428a, 0x46 },
	{ 0x430b, 0x0f },
	{ 0x430c, 0xfc },
	{ 0x430d, 0x00 },
	{ 0x430e, 0x00 },
	{ 0x4500, 0x18 },
	{ 0x4501, 0x18 },
	{ 0x4504, 0x00 },
	{ 0x4507, 0x02 },
	{ 0x4508, 0x1a },
	{ 0x4603, 0x00 },
	{ 0x4640, 0x62 },
	{ 0x4646, 0xaa },
	{ 0x4647, 0x55 },
	{ 0x4648, 0x99 },
	{ 0x4649, 0x66 },
	{ 0x464d, 0x00 },
	{ 0x4654, 0x11 },
	{ 0x4655, 0x22 },
	{ 0x4800, 0x04 },
	{ 0x4810, 0xff },
	{ 0x4811, 0xff },
	{ 0x480e, 0x00 },
	{ 0x4813, 0x00 },
	{ 0x4837, 0x0e },
	{ 0x484b, 0x27 },
	{ 0x4d00, 0x4e },
	{ 0x4d01, 0x0c },
	{ 0x4d09, 0x4f },
	{ 0x5000, 0x1f },
	{ 0x5080, 0x00 },
	{ 0x50c0, 0x00 },
	{ 0x5100, 0x00 },
	{ 0x5200, 0x00 },
	{ 0x5201, 0x70 },
	{ 0x5202, 0x03 },
	{ 0x5203, 0x7f },
	{ 0x5780, 0x53 },
	{ 0x5786, 0x01 },
	{ 0x3501, 0x02 },
	{ 0x0305, 0x3f },
	{ 0x4837, 0x15 },
	{ 0x032f, 0x00 },
	{ 0x032e, 0x01 },
	{ 0x032d, 0x06 },
	{ 0x3106, 0x12 },
	{ 0x380c, 0x06 },
	{ 0x380d, 0x40 },
	{ 0x380e, VMAX>>8 },
	{ 0x380f, VMAX&0xFF },
	{ 0x3501, 0x08 },
	{ 0x3502, 0xC2 },
	/* { 0x0100, 0x01 }, */
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 20},
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 20},
	{CC_PWDN, CC_UP},
	{CC_DELAY, 20},
	{CC_RESET,  CC_UP},
	{CC_DELAY, 5},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_DOWN},
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
		.framerate = { 1, 30 }, /* 30fps */
		.mfreq = 756000000,	/* mipi-csi clock */
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

	if (id != 0x5302) {
		PRT_ERR("ID wrong! (0x%04x != 0x5302)\n", id);
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
			PRT_ERR("%s read i %d reg 0x%04x error\n",
				cc_si->name, i, init_reg_list[i].reg);
			return ret;
		}

		if (init_reg_list[i].val != value) {
			PRT_ERR("%s %d 0x%04x (0x%02x) != (0x%02x)\n",
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
		ret = cc_si->write_reg_list(cc_si, init_reg_list,
				ARRAY_SIZE(init_reg_list));
		if (ret != 0) {
			PRT_ERR("write %s init_reg_list failed!\n",
				cc_si->name);
			return -ENODEV;
		}

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

	sensor_fmt_info->pclk = 108 * 1000 * 1000;
	sensor_fmt_info->vts = 2250;
	sensor_fmt_info->hts = 1600;
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
	u32 vmax;

	shutter_time = shutter_time / 16;

	if (shutter_time < 1)
		shutter_time = 1;

	if (shutter_time > VMAX - 8)
		vmax = shutter_time + 8;
	else
		vmax = VMAX;

	ret = cc_si->write_reg(cc_si, 0x380e, ((vmax >> 8) & 0xff));
	ret = cc_si->write_reg(cc_si, 0x380f, ((vmax >> 0) & 0xff));

	ret = cc_si->write_reg(cc_si, 0x3501, ((shutter_time >> 8) & 0xff));
	ret = cc_si->write_reg(cc_si, 0x3502, ((shutter_time >> 0) & 0xff));

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, __u32 gain)
{
	int ret = 0;
	u32 again, dgain;

	if (gain < 2*256) {
		again = gain / 16;
		dgain = 1024 * gain / (again * 16);
	} else if (gain < 4*256) {
		again = gain / 32 * 2;
		dgain = 1024 * gain / (again * 16);
	} else if (gain < 8*256) {
		again = gain / 64 * 4;
		dgain = 1024 * gain / (again * 16);
	} else if (gain < 16*256) {
		again = gain / 256 * 8;
		dgain = 1024 * gain / (again * 16);
	} else if (gain < 248*256) {
		again = 0xf8;
		dgain = 1024 * gain / (again * 16);
	} else {
		again = 0xf8;
		dgain = 0x3ff;
	}

	ret = cc_si->write_reg(cc_si, 0x3508, ((again >> 4) & 0xf));
	ret = cc_si->write_reg(cc_si, 0x3509, ((again << 4) & 0xff));

	ret = cc_si->write_reg(cc_si, 0x350a, ((dgain >> 10) & 0xf));
	ret = cc_si->write_reg(cc_si, 0x350b, ((dgain >> 2) & 0xff));
	ret = cc_si->write_reg(cc_si, 0x350c, ((dgain << 6) & 0xff));

	return ret;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

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

#define DRIVER_NAME "os02k10_mipi"

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

static const struct of_device_id os02k10_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, os02k10_mipi_of_match);

static struct platform_driver os02k10_mipi_driver = {
	.driver = {
		.of_match_table	= os02k10_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(os02k10_mipi_driver);

MODULE_DESCRIPTION("OS02K10-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


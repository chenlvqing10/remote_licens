/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ov2685 driver code for LomboTech
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

/* #define RAW */

#ifndef RAW
/* yuv_800_600_30fps */
static const cc_reg_t init_reg_list[] = {
	{ 0x0103, 0x01 },
	{ 0x3002, 0x00 },
	{ 0x3016, 0x1c },
	{ 0x3018, 0x84 },
	{ 0x301d, 0xf0 },
	{ 0x3020, 0x00 },
	{ 0x3082, 0x2c },
	{ 0x3083, 0x03 },
	{ 0x3084, 0x07 },
	{ 0x3085, 0x03 },
	{ 0x3086, 0x00 },
	{ 0x3087, 0x00 },
	{ 0x3501, 0x26 },
	{ 0x3502, 0x40 },
	{ 0x3503, 0x03 },
	{ 0x350b, 0x36 },
	{ 0x3600, 0xb4 },
	{ 0x3603, 0x35 },
	{ 0x3604, 0x24 },
	{ 0x3605, 0x00 },
	{ 0x3620, 0x24 },
	{ 0x3621, 0x34 },
	{ 0x3622, 0x03 },
	{ 0x3628, 0x10 },
	{ 0x3705, 0x3c },
	{ 0x370a, 0x23 },
	{ 0x370c, 0x50 },
	{ 0x370d, 0xc0 },
	{ 0x3717, 0x58 },
	{ 0x3718, 0x80 },
	{ 0x3720, 0x00 },
	{ 0x3721, 0x09 },
	{ 0x3722, 0x06 },
	{ 0x3723, 0x59 },
	{ 0x3738, 0x99 },
	{ 0x3781, 0x80 },
	{ 0x3784, 0x0c },
	{ 0x3789, 0x60 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x00 },
	{ 0x3804, 0x06 },
	{ 0x3805, 0x4f },
	{ 0x3806, 0x04 },
	{ 0x3807, 0xbf },
	{ 0x3808, 0x03 },
	{ 0x3809, 0x20 },
	{ 0x380a, 0x02 },
	{ 0x380b, 0x58 },
	{ 0x380c, 0x0d },
	{ 0x380d, 0x58 },
	{ 0x380e, 0x02 },
	{ 0x380f, 0x84 },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x04 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x04 },
	{ 0x3814, 0x31 },
	{ 0x3815, 0x31 },
	{ 0x3819, 0x04 },
	{ 0x3820, 0xc6 },
	{ 0x3821, 0x01 },
	{ 0x3a06, 0x01 },
	{ 0x3a07, 0x82 },
	{ 0x3a08, 0x01 },
	{ 0x3a09, 0x42 },
	{ 0x3a0a, 0x0c },
	{ 0x3a0b, 0x10 },
	{ 0x3a0c, 0x14 },
	{ 0x3a0d, 0x20 },
	{ 0x3a0e, 0x01 },
	{ 0x3a0f, 0x82 },
	{ 0x3a10, 0x02 },
	{ 0x3a11, 0x84 },
	{ 0x4000, 0x81 },
	{ 0x4001, 0x40 },
	{ 0x4008, 0x00 },
	{ 0x4009, 0x03 },
	{ 0x4300, 0x30 },
	{ 0x430e, 0x00 },
	{ 0x4602, 0x02 },
	{ 0x481b, 0x40 },
	{ 0x481f, 0x40 },
	{ 0x4837, 0x1e },
	{ 0x5000, 0xbf },/*disable wb gain*/
	{ 0x5001, 0x05 },
	{ 0x5002, 0x32 },
	{ 0x5003, 0x04 },
	{ 0x5004, 0xfe },/*disable awb*/
	{ 0x5005, 0x12 },
	{ 0x0100, 0x01 },
	{ 0x5180, 0xf4 },
	{ 0x5181, 0x11 },
	{ 0x5182, 0x41 },
	{ 0x5183, 0x42 },
	{ 0x5184, 0x78 },
	{ 0x5185, 0x58 },
	{ 0x5186, 0xb5 },
	{ 0x5187, 0xb2 },
	{ 0x5188, 0x08 },
	{ 0x5189, 0x0e },
	{ 0x518a, 0x0c },
	{ 0x518b, 0x4c },
	{ 0x518c, 0x38 },
	{ 0x518d, 0xf8 },
	{ 0x518e, 0x04 },
	{ 0x518f, 0x7f },
	{ 0x5190, 0x40 },
	{ 0x5191, 0x5f },
	{ 0x5192, 0x40 },
	{ 0x5193, 0xff },
	{ 0x5194, 0x40 },
	{ 0x5195, 0x07 },
	{ 0x5196, 0x04 },
	{ 0x5197, 0x04 },
	{ 0x5198, 0x00 },
	{ 0x5199, 0x05 },
	{ 0x519a, 0xd2 },
	{ 0x519b, 0x04 },
	{ 0x5200, 0x09 },
	{ 0x5201, 0x00 },
	{ 0x5202, 0x06 },
	{ 0x5203, 0x20 },
	{ 0x5204, 0x41 },
	{ 0x5205, 0x16 },
	{ 0x5206, 0x00 },
	{ 0x5207, 0x05 },
	{ 0x520b, 0x30 },
	{ 0x520c, 0x75 },
	{ 0x520d, 0x00 },
	{ 0x520e, 0x30 },
	{ 0x520f, 0x75 },
	{ 0x5210, 0x00 },
	{ 0x5280, 0x14 },
	{ 0x5281, 0x02 },
	{ 0x5282, 0x02 },
	{ 0x5283, 0x04 },
	{ 0x5284, 0x06 },
	{ 0x5285, 0x08 },
	{ 0x5286, 0x0c },
	{ 0x5287, 0x10 },
	{ 0x5300, 0xc5 },
	{ 0x5301, 0xa0 },
	{ 0x5302, 0x06 },
	{ 0x5303, 0x0a },
	{ 0x5304, 0x30 },
	{ 0x5305, 0x60 },
	{ 0x5306, 0x90 },
	{ 0x5307, 0xc0 },
	{ 0x5308, 0x82 },
	{ 0x5309, 0x00 },
	{ 0x530a, 0x26 },
	{ 0x530b, 0x02 },
	{ 0x530c, 0x02 },
	{ 0x530d, 0x00 },
	{ 0x530e, 0x0c },
	{ 0x530f, 0x14 },
	{ 0x5310, 0x1a },
	{ 0x5311, 0x20 },
	{ 0x5312, 0x80 },
	{ 0x5313, 0x4b },
	{ 0x5380, 0x01 },
	{ 0x5381, 0x52 },
	{ 0x5382, 0x00 },
	{ 0x5383, 0x4a },
	{ 0x5384, 0x00 },
	{ 0x5385, 0xb6 },
	{ 0x5386, 0x00 },
	{ 0x5387, 0x8d },
	{ 0x5388, 0x00 },
	{ 0x5389, 0x3a },
	{ 0x538a, 0x00 },
	{ 0x538b, 0xa6 },
	{ 0x538c, 0x00 },
	{ 0x5400, 0x0d },
	{ 0x5401, 0x18 },
	{ 0x5402, 0x31 },
	{ 0x5403, 0x5a },
	{ 0x5404, 0x65 },
	{ 0x5405, 0x6f },
	{ 0x5406, 0x77 },
	{ 0x5407, 0x80 },
	{ 0x5408, 0x87 },
	{ 0x5409, 0x8f },
	{ 0x540a, 0xa2 },
	{ 0x540b, 0xb2 },
	{ 0x540c, 0xcc },
	{ 0x540d, 0xe4 },
	{ 0x540e, 0xf0 },
	{ 0x540f, 0xa0 },
	{ 0x5410, 0x6e },
	{ 0x5411, 0x06 },
	{ 0x5480, 0x19 },
	{ 0x5481, 0x00 },
	{ 0x5482, 0x09 },
	{ 0x5483, 0x12 },
	{ 0x5484, 0x04 },
	{ 0x5485, 0x06 },
	{ 0x5486, 0x08 },
	{ 0x5487, 0x0c },
	{ 0x5488, 0x10 },
	{ 0x5489, 0x18 },
	{ 0x5500, 0x02 },
	{ 0x5501, 0x03 },
	{ 0x5502, 0x04 },
	{ 0x5503, 0x05 },
	{ 0x5504, 0x06 },
	{ 0x5505, 0x08 },
	{ 0x5506, 0x00 },
	{ 0x5600, 0x20 },
	{ 0x5603, 0x40 },
	{ 0x5604, 0x28 },
	{ 0x5609, 0x20 },
	{ 0x560a, 0x60 },
	{ 0x5780, 0x3e },
	{ 0x5781, 0x0f },
	{ 0x5782, 0x04 },
	{ 0x5783, 0x02 },
	{ 0x5784, 0x01 },
	{ 0x5785, 0x01 },
	{ 0x5786, 0x00 },
	{ 0x5787, 0x04 },
	{ 0x5788, 0x02 },
	{ 0x5789, 0x00 },
	{ 0x578a, 0x01 },
	{ 0x578b, 0x02 },
	{ 0x578c, 0x03 },
	{ 0x578d, 0x03 },
	{ 0x578e, 0x08 },
	{ 0x578f, 0x0c },
	{ 0x5790, 0x08 },
	{ 0x5791, 0x04 },
	{ 0x5792, 0x00 },
	{ 0x5793, 0x00 },
	{ 0x5794, 0x03 },
	{ 0x5800, 0x03 },
	{ 0x5801, 0x24 },
	{ 0x5802, 0x02 },
	{ 0x5803, 0x40 },
	{ 0x5804, 0x34 },
	{ 0x5805, 0x05 },
	{ 0x5806, 0x12 },
	{ 0x5807, 0x05 },
	{ 0x5808, 0x03 },
	{ 0x5809, 0x3c },
	{ 0x580a, 0x02 },
	{ 0x580b, 0x40 },
	{ 0x580c, 0x26 },
	{ 0x580d, 0x05 },
	{ 0x580e, 0x52 },
	{ 0x580f, 0x06 },
	{ 0x5810, 0x03 },
	{ 0x5811, 0x28 },
	{ 0x5812, 0x02 },
	{ 0x5813, 0x40 },
	{ 0x5814, 0x24 },
	{ 0x5815, 0x05 },
	{ 0x5816, 0x42 },
	{ 0x5817, 0x06 },
	{ 0x5818, 0x0d },
	{ 0x5819, 0x40 },
	{ 0x581a, 0x04 },
	{ 0x581b, 0x0c },
	{ 0x3a03, 0x4c },
	{ 0x3a04, 0x40 },
	{ 0x3a05, 0x46 },
	{ 0x3503, 0x00 },
};
#else
static const cc_reg_t init_reg_list[] = {
	{0x0103, 0x01},
	{0x0100, 0x00},
	{0x3002, 0x00},
	{0x3016, 0x1c},
	{0x3018, 0x44},
	{0x301d, 0xf0},
	{0x3020, 0x00},
	{0x3082, 0x37},
	{0x3083, 0x03},
	{0x3084, 0x09},
	{0x3085, 0x04},
	{0x3086, 0x00},
	{0x3087, 0x00},
	{0x3501, 0x4e},
	{0x3502, 0xe0},
	{0x3503, 0x07},
	{0x350b, 0x36},
	{0x3600, 0xb4},
	{0x3603, 0x35},
	{0x3604, 0x24},
	{0x3605, 0x00},
	{0x3620, 0x24},
	{0x3621, 0x34},
	{0x3622, 0x03},
	{0x3628, 0x10},
	{0x3705, 0x3c},
	{0x370a, 0x21},
	{0x370c, 0x50},
	{0x370d, 0xc0},
	{0x3717, 0x58},
	{0x3718, 0x80},
	{0x3720, 0x00},
	{0x3721, 0x09},
	{0x3722, 0x06},
	{0x3723, 0x59},
	{0x3738, 0x99},
	{0x3781, 0x80},
	{0x3784, 0x0c},
	{0x3789, 0x60},
	{0x3800, 0x00},
	{0x3801, 0x00},
	{0x3802, 0x00},
	{0x3803, 0x00},
	{0x3804, 0x06},
	{0x3805, 0x4f},
	{0x3806, 0x04},
	{0x3807, 0xbf},
	{0x3808, 0x06},
	{0x3809, 0x40},
	{0x380a, 0x04},
	{0x380b, 0xb0},
	{0x380c, 0x06},
	{0x380d, 0xa4},
	{0x380e, 0x05},
	{0x380f, 0x0e},
	{0x3810, 0x00},
	{0x3811, 0x08},
	{0x3812, 0x00},
	{0x3813, 0x08},
	{0x3814, 0x11},
	{0x3815, 0x11},
	{0x3819, 0x04},
	{0x3820, 0xc4},/* filp */
	{0x3821, 0x00},
	{0x3a06, 0x01},
	{0x3a07, 0x84},
	{0x3a08, 0x01},
	{0x3a09, 0x43},
	{0x3a0a, 0x24},
	{0x3a0b, 0x60},
	{0x3a0c, 0x28},
	{0x3a0d, 0x60},
	{0x3a0e, 0x04},
	{0x3a0f, 0x8c},
	{0x3a10, 0x05},
	{0x3a11, 0x0c},
	{0x4000, 0x81},
	{0x4001, 0x40},
	{0x4008, 0x02},
	{0x4009, 0x09},
	{0x4300, 0x00},
	{0x430e, 0x00},
	{0x4602, 0x02},
	{0x481b, 0x40},
	{0x481f, 0x40},
	{0x4837, 0x18},
	{0x5000, 0x1f},
	{0x5001, 0x05},
	{0x5002, 0x30},
	{0x5003, 0x04},
	{0x5004, 0x00},
	{0x5005, 0x0c},
	{0x5280, 0x15},
	{0x5281, 0x06},
	{0x5282, 0x06},
	{0x5283, 0x08},
	{0x5284, 0x1c},
	{0x5285, 0x1c},
	{0x5286, 0x20},
	{0x5287, 0x10},
};
#endif

static struct cc_power_act power_up_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 20},
	{CC_PWDN, CC_UP},
	{CC_DELAY, 20},
};

static struct cc_power_act power_down_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 20},

};

static const struct cc_frame_format frame_formats[] = {
	{
#ifndef RAW
		.mbus_code = V4L2_MBUS_FMT_YUYV8_2X8,
#else
		.mbus_code = V4L2_MBUS_FMT_SGRBG10_1X10,
#endif
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};
#ifndef RAW
#define FHD_WIDTH		800
#define FHD_HEIGHT		600
#else
#define FHD_WIDTH		1600
#define FHD_HEIGHT		1200
#endif
static const struct cc_win_size win_sizes[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.left = 0,
		.top = 0,
		.pclk = 65994000,
		.vts = 1294,
		.hts = 1700,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,
		.framerate = { 1, 30 }, /* 30fps */
#ifndef RAW
		.mfreq = 270000000,
#else
		.mfreq = 660000000,
#endif
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

	if (id != 0x2685) {
		PRT_ERR("ID wrong! (0x%04x != 0x2685)\n", id);
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
		PRT_INFO("***********stream**************:%d\n", on);
		ret = cc_si->write_reg(cc_si, 0x0100, 0x01);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		PRT_INFO("***********stream**************:%d\n", on);
		ret = cc_si->write_reg(cc_si, 0x0100, 0x00);
		if (ret)
			PRT_ERR("%d\n", on);
	}

	return ret;
}

/*
 * ctrl for sensor start
 */
#ifdef RAW
static int get_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u8 val = 0;
	int ret = 0;

	/* read */
	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &val);
	reg_data->reg_value = val;

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

	sensor_fmt_info->pclk = cc_si->cur_wsize->pclk;
	sensor_fmt_info->vts = cc_si->cur_wsize->vts;
	sensor_fmt_info->hts = cc_si->cur_wsize->hts;
	sensor_fmt_info->hoffset = cc_si->cur_wsize->left;
	sensor_fmt_info->voffset = cc_si->cur_wsize->top;
	sensor_fmt_info->width = cc_si->cur_wsize->width;
	sensor_fmt_info->height = cc_si->cur_wsize->height;
	sensor_fmt_info->bayer_mode = ISP_PATTERN_GRBG;
	sensor_fmt_info->buswidth = ISP_INPUT_8BITS;

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;
	int vmax = cc_si->cur_wsize->vts;

	if (shutter_time / 16 > vmax - 2)
		vmax = shutter_time / 16 + 2;

	ret = cc_si->write_reg(cc_si, 0x380e, (vmax & 0xff00) >> 8);
	ret = cc_si->write_reg(cc_si, 0x380f, (vmax & 0xff) >> 0);

	ret = cc_si->write_reg(cc_si, 0x3500, (shutter_time & 0xf0000) >> 16);
	ret = cc_si->write_reg(cc_si, 0x3501, (shutter_time & 0x0ff00) >> 8);
	ret = cc_si->write_reg(cc_si, 0x3502, (shutter_time & 0x000ff));

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, u32 gain)
{
	int ret = 0;

	gain = gain / 16;

	if (gain >= 0x7ff)
		gain = 0x7ff;

	ret = cc_si->write_reg(cc_si, 0x350b, gain & 0x0ff);
	ret = cc_si->write_reg(cc_si, 0x350a, (gain & 0x700) >> 8);

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
#else
static int ov2685_set_ae_target(struct cc_sensor_info *cc_si, int val)
{
	int ret = 0;

	return ret;

	ret += cc_si->write_reg(cc_si, 0x3a05, val);
	ret += cc_si->write_reg(cc_si, 0x3a03, val + 6);
	ret += cc_si->write_reg(cc_si, 0x3a04, val - 6);

	PRT_DBG("target: 0x%x\n", val);
	return ret;
}

static long ov2685_set_ae_meter(struct cc_sensor_info *cc_si, void *arg)
{
	struct viss_isp_ae_para *pae = (struct viss_isp_ae_para *)arg;
	long ret = 0;

	return ret;

	if (pae->ae_mode == V4L2_EXPOSURE_METERING_CENTER_WEIGHTED) {
		ret += cc_si->write_reg(cc_si, 0x5900, pae->rect.left >> 8);
		ret += cc_si->write_reg(cc_si, 0x5901, pae->rect.left >> 0);
		ret += cc_si->write_reg(cc_si, 0x5902, pae->rect.top >> 8);
		ret += cc_si->write_reg(cc_si, 0x5903, pae->rect.top >> 0);
		ret += cc_si->write_reg(cc_si, 0x5904, pae->rect.width >> 8);
		ret += cc_si->write_reg(cc_si, 0x5905, pae->rect.width >> 0);
		ret += cc_si->write_reg(cc_si, 0x5906, pae->rect.height >> 8);
		ret += cc_si->write_reg(cc_si, 0x5907, pae->rect.height >> 0);
		ret += cc_si->write_reg(cc_si, 0x5910, 0x1);
	} else {
		ret += cc_si->write_reg(cc_si, 0x5910, 0x2);
	}

	return ret;
}

static int ov2685_get_exif(struct cc_sensor_info *cc_si,
			struct viss_isp_exif *exif)
{
	int ret = 0;
	u8 tmp;
	u16 exp_line = 0;
	u16 gain = 0;

	ret = cc_si->read_reg(cc_si, 0x5913, &tmp);/* average lum */
	exif->res[2] = tmp;

	ret = cc_si->read_reg(cc_si, 0x3a05, &tmp);/* Y target */
	exif->res[4] = tmp;

	ret = cc_si->read_reg(cc_si, 0x350a, &tmp);/* gain */
	gain = tmp;
	ret = cc_si->read_reg(cc_si, 0x350b, &tmp);/* gain */
	gain = (gain << 8) | tmp;
	exif->iso_speed = gain * 100 / 16;

	ret = cc_si->read_reg(cc_si, 0x3501, &tmp);/* exposure */
	exp_line = tmp;
	ret = cc_si->read_reg(cc_si, 0x3502, &tmp);/* exposure */
	exp_line = ((exp_line << 8) | tmp) >> 4;

	exif->exposure_time = exp_line * 333 / 644;/* 10000 / 30fps */

	return ret;
}
#endif
/*
 * V4L2 subdev internal operations
 */
static long sensor_ioctl(struct cc_sensor_info *cc_si, unsigned int cmd,
			void *arg)
{
	long ret = 0;

	switch (cmd) {
#ifndef RAW
	case VIDIOC_ISP_SET_AE_METER:
		ret = ov2685_set_ae_meter(cc_si, arg);
		break;
	case VIDIOC_ISP_SET_AE_TARGET:
		ov2685_set_ae_target(cc_si, *(unsigned int *) arg);
		break;
	case VIDIOC_VISS_ISP_EXIF_REQ:
		ov2685_get_exif(cc_si, (struct viss_isp_exif *)arg);
		break;
#else
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
#endif
	default:
		return -EINVAL;
	}
	return ret;
}

/*
 * ctrl for sensor finish
 */

#define DRIVER_NAME "ov2685_mipi"

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
#ifndef RAW
	cc_si->dlanes = 2;
#else
	cc_si->dlanes = 1;
#endif
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

static const struct of_device_id ov2685_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov2685_mipi_of_match);

static struct platform_driver ov2685_mipi_driver = {
	.driver = {
		.of_match_table	= ov2685_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov2685_mipi_driver);

MODULE_DESCRIPTION("OV2685-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


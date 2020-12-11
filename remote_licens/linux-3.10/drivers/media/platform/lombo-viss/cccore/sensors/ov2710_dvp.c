/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ov2710 dvp sensor driver code for LomboTech
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

/* #define LOW_FREQ */

#ifdef LOW_FREQ
static const cc_reg_t init_reg_list[] = {
	{0x3103, 0x93 },
	{0x3008, 0x82 },
	{0x3017, 0x7f },
	{0x3018, 0xfc },
	{0x3706, 0x61 },
	{0x3712, 0x0c },
	{0x3630, 0x6d },
	{0x3801, 0xb4 },
	{0x3621, 0x04 },
	{0x3604, 0x60 },
	{0x3603, 0xa7 },
	{0x3631, 0x26 },
	{0x3600, 0x04 },
	{0x3620, 0x37 },
	{0x3623, 0x00 },
	{0x3702, 0x9e },
	{0x3703, 0x5c },
	{0x3704, 0x40 },
	{0x370d, 0x0f },
	{0x3713, 0x9f },
	{0x3714, 0x4c },
	{0x3710, 0x9e },
	{0x3801, 0xc4 },
	{0x3605, 0x05 },
	{0x3606, 0x3f },
	{0x302d, 0x90 },
	{0x370b, 0x40 },
	{0x3716, 0x31 },
	{0x3707, 0x52 },
	{0x380d, 0x74 },
	{0x5181, 0x20 },
	{0x518f, 0x00 },
	{0x4301, 0xff },
	{0x4303, 0x00 },
	{0x3a00, 0x78 },
	{0x300f, 0x88 },
	{0x3011, 0x28 },
	{0x3a1a, 0x06 },
	{0x3a18, 0x00 },
	{0x3a19, 0xe0 },
	{0x3a13, 0x50 },
	{0x382e, 0x0f },
	{0x381a, 0x1a },
	{0x401d, 0x02 },
	{0x5688, 0x03 },
	{0x5684, 0x07 },
	{0x5685, 0xa0 },
	{0x5686, 0x14 },
	{0x5687, 0x43 },
	{0x3a0f, 0xf0 },
	{0x3a10, 0x38 },
	{0x3a1b, 0xf8 },
	{0x3a1e, 0x30 },
	{0x3a11, 0x90 },
	{0x3a1f, 0x10 },
	{0x3010, 0x10 },
	{0x3012, 0x01 },
	{0x3503, 0x07 },
	{0x3502, 0x00 },
	{0x3501, 0x22 },
	{0x3500, 0x00 },
	{0x350b, 0x00 },
	{0x350a, 0x00 },
	{0x3406, 0x01 },
	{0x3400, 0x04 },
	{0x3401, 0x00 },
	{0x3402, 0x04 },
	{0x3403, 0x00 },
	{0x3404, 0x04 },
	{0x3405, 0x00 },
	{0x4000, 0x05 },
	{0x302c, 0x00 },
	{0x5000, 0xdf },
};

#else
static const cc_reg_t init_reg_list[] = {
	{ 0x3008, 0x42 },
	{ 0x300e, 0x1a },
	{ 0x300f, 0x88 },
	{ 0x3010, 0x00 },
	{ 0x3011, 0x18 },
	{ 0x3012, 0x00 },
	{ 0x3016, 0x00 },
	{ 0x3017, 0x7f },
	{ 0x3018, 0xfc },
	{ 0x3019, 0x00 },
	{ 0x301a, 0x00 },
	{ 0x301b, 0x00 },
	{ 0x301c, 0x00 },
	{ 0x301d, 0x00 },
	{ 0x301e, 0x00 },
	{ 0x302c, 0x02 },
	{ 0x3100, 0x6c },
	{ 0x3103, 0x03 },
	{ 0x3104, 0x01 },
	{ 0x3200, 0x40 },
	{ 0x3201, 0x4a },
	{ 0x3202, 0x54 },
	{ 0x3203, 0x5e },
	{ 0x3212, 0x10 },
	{ 0x3621, 0x04 },
	{ 0x370d, 0x07 },
	{ 0x3800, 0x01 },
	{ 0x3801, 0xc4 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x0a },
	{ 0x3804, 0x07 },
	{ 0x3805, 0x80 },
	{ 0x3806, 0x04 },
	{ 0x3807, 0x38 },
	{ 0x3808, 0x07 },
	{ 0x3809, 0x80 },
	{ 0x380a, 0x04 },
	{ 0x380b, 0x38 },
	{ 0x380c, 0x09 },
	{ 0x380d, 0x74 },
	{ 0x380e, 0x04 },
	{ 0x380f, 0x50 },
	{ 0x3810, 0x10 },
	{ 0x3818, 0x80 },
	{ 0x3500, 0x00 },
	{ 0x3501, 0x33 },
	{ 0x3502, 0x80 },
	{ 0x3503, 0x07 },
	{ 0x350a, 0x00 },
	{ 0x350b, 0x00 },
	{ 0x350c, 0x00 },
	{ 0x350d, 0x00 },
	{ 0x3a00, 0x78 },
	{ 0x3a01, 0x04 },
	{ 0x3a02, 0x02 },
	{ 0x3a03, 0x28 },
	{ 0x3a04, 0x00 },
	{ 0x3a08, 0x14 },
	{ 0x3a09, 0xc0 },
	{ 0x3a0a, 0x11 },
	{ 0x3a0b, 0x40 },
	{ 0x3a0d, 0x04 },
	{ 0x3a0e, 0x03 },
	{ 0x3a0f, 0x40 },
	{ 0x3a10, 0x38 },
	{ 0x3a11, 0x90 },
	{ 0x3a12, 0x00 },
	{ 0x3a13, 0x54 },
	{ 0x3a14, 0x01 },
	{ 0x3a15, 0xf2 },
	{ 0x3a16, 0x00 },
	{ 0x3a17, 0x89 },
	{ 0x3a18, 0x00 },
	{ 0x3a19, 0x7a },
	{ 0x3a1b, 0x48 },
	{ 0x3a1c, 0x06 },
	{ 0x3a1d, 0x18 },
	{ 0x3a1e, 0x30 },
	{ 0x3a1f, 0x10 },
	{ 0x3a20, 0x20 },
	{ 0x3d00, 0x00 },
	{ 0x3d01, 0x00 },
	{ 0x3d02, 0x00 },
	{ 0x3d03, 0x00 },
	{ 0x3d04, 0x00 },
	{ 0x3d05, 0x00 },
	{ 0x3d06, 0x00 },
	{ 0x3d07, 0x00 },
	{ 0x3d08, 0x00 },
	{ 0x3d09, 0x00 },
	{ 0x3d0a, 0x00 },
	{ 0x3d0b, 0x00 },
	{ 0x3d0c, 0x00 },
	{ 0x3d0d, 0x00 },
	{ 0x3d0e, 0x00 },
	{ 0x3d0f, 0x00 },
	{ 0x3d10, 0x00 },
	{ 0x4000, 0x01 },
	{ 0x4001, 0x00 },
	{ 0x4002, 0x00 },
	{ 0x401d, 0x22 },
	{ 0x4201, 0x00 },
	{ 0x4202, 0x00 },
	{ 0x4700, 0x04 },
	{ 0x4704, 0x00 },
	{ 0x4708, 0x01 },
	{ 0x4709, 0x00 },
	{ 0x4800, 0x04 },
	{ 0x4801, 0x03 },
	{ 0x4803, 0x50 },
	{ 0x4804, 0x8d },
	{ 0x4805, 0x10 },
	{ 0x4810, 0xff },
	{ 0x4811, 0xff },
	{ 0x4812, 0x00 },
	{ 0x4813, 0x00 },
	{ 0x4814, 0x2a },
	{ 0x4815, 0x00 },
	{ 0x4818, 0x00 },
	{ 0x4819, 0x96 },
	{ 0x481a, 0x00 },
	{ 0x481b, 0x3c },
	{ 0x481c, 0x01 },
	{ 0x481d, 0x86 },
	{ 0x481e, 0x00 },
	{ 0x481f, 0x3c },
	{ 0x4820, 0x00 },
	{ 0x4821, 0x56 },
	{ 0x4822, 0x00 },
	{ 0x4823, 0x3c },
	{ 0x4824, 0x00 },
	{ 0x4825, 0x32 },
	{ 0x4826, 0x00 },
	{ 0x4827, 0x32 },
	{ 0x4828, 0x00 },
	{ 0x4829, 0x64 },
	{ 0x482a, 0x05 },
	{ 0x482b, 0x04 },
	{ 0x482c, 0x00 },
	{ 0x482d, 0x00 },
	{ 0x482e, 0x34 },
	{ 0x482f, 0x00 },
	{ 0x4830, 0x00 },
	{ 0x4831, 0x04 },
	{ 0x4832, 0x00 },
	{ 0x5000, 0x59 },
	{ 0x5001, 0x4e },
	{ 0x5002, 0xe0 },
	{ 0x5005, 0xdc },
	{ 0x501f, 0x03 },
	{ 0x503d, 0x00 },
	{ 0x3400, 0x04 },
	{ 0x3401, 0x00 },
	{ 0x3402, 0x04 },
	{ 0x3403, 0x00 },
	{ 0x3404, 0x04 },
	{ 0x3405, 0x00 },
	{ 0x3406, 0x00 },
	{ 0x5180, 0x40 },
	{ 0x5181, 0x20 },
	{ 0x5182, 0x04 },
	{ 0x5183, 0x08 },
	{ 0x518c, 0xf0 },
	{ 0x518d, 0xf0 },
	{ 0x518e, 0xf0 },
	{ 0x518f, 0x00 },
	{ 0x5680, 0x00 },
	{ 0x5681, 0x00 },
	{ 0x5682, 0x00 },
	{ 0x5683, 0x00 },
	{ 0x5684, 0x07 },
	{ 0x5685, 0xa0 },
	{ 0x5686, 0x04 },
	{ 0x5687, 0x43 },
	{ 0x5780, 0x7f },
	{ 0x5781, 0x20 },
	{ 0x5782, 0x18 },
	{ 0x5783, 0x08 },
	{ 0x5784, 0x04 },
	{ 0x5785, 0x40 },
	{ 0x5786, 0x18 },
	{ 0x5787, 0x08 },
	{ 0x5788, 0x04 },
	{ 0x5789, 0x08 },
	{ 0x578a, 0x20 },
	{ 0x578b, 0x07 },
	{ 0x578c, 0x00 },
	{ 0x5790, 0x00 },
	{ 0x5791, 0x08 },
	{ 0x5792, 0x00 },
	{ 0x5793, 0x18 },
	{ 0x5794, 0x00 },
	{ 0x5795, 0x80 },
	{ 0x5796, 0x01 },
	{ 0x5797, 0x00 },
	{ 0x5800, 0x03 },
	{ 0x5801, 0x0c },
	{ 0x5802, 0x03 },
	{ 0x5803, 0x06 },
	{ 0x5804, 0x22 },
	{ 0x5805, 0x07 },
	{ 0x5806, 0xc2 },
	{ 0x5807, 0x08 },
	{ 0x5808, 0x03 },
	{ 0x5809, 0x0c },
	{ 0x580a, 0x03 },
	{ 0x580b, 0x06 },
	{ 0x580c, 0x22 },
	{ 0x580d, 0x07 },
	{ 0x580e, 0xc2 },
	{ 0x580f, 0x08 },
	{ 0x5810, 0x03 },
	{ 0x5811, 0x0c },
	{ 0x5812, 0x03 },
	{ 0x5813, 0x06 },
	{ 0x5814, 0x22 },
	{ 0x5815, 0x07 },
	{ 0x5816, 0xc2 },
	{ 0x5817, 0x08 },
	{ 0x5818, 0x04 },
	{ 0x5819, 0x80 },
	{ 0x581a, 0x06 },
	{ 0x581b, 0x0c },
	{ 0x581c, 0x80 },
	{ 0x6000, 0x1f },
	{ 0x6001, 0x01 },
	{ 0x6002, 0x00 },
	{ 0x6003, 0x76 },
	{ 0x6004, 0x42 },
	{ 0x6005, 0x01 },
	{ 0x6006, 0x00 },
	{ 0x6007, 0x76 },
	{ 0x6008, 0x42 },
	{ 0x6009, 0x01 },
	{ 0x600a, 0x00 },
	{ 0x600b, 0x76 },
	{ 0x600c, 0x42 },
	{ 0x600d, 0x01 },
	{ 0x600e, 0x00 },
	{ 0x600f, 0x76 },
	{ 0x6010, 0x42 },
	{ 0x6011, 0x01 },
	{ 0x6012, 0x00 },
	{ 0x6013, 0x76 },
	{ 0x6014, 0x42 },
	{ 0x302d, 0x90 },
	{ 0x3600, 0x04 },
	{ 0x3601, 0x04 },
	{ 0x3602, 0x04 },
	{ 0x3603, 0xa7 },
	{ 0x3604, 0x60 },
	{ 0x3605, 0x05 },
	{ 0x3606, 0x12 },
	{ 0x3620, 0x07 },
	{ 0x3623, 0x40 },
	{ 0x3630, 0x6b },
	{ 0x3631, 0x24 },
	{ 0x3a1a, 0x06 },
	{ 0x3702, 0x9e },
	{ 0x3703, 0x74 },
	{ 0x3704, 0x10 },
	{ 0x3706, 0x61 },
	{ 0x370b, 0x40 },
	{ 0x3710, 0x9e },
	{ 0x3712, 0x0c },
	{ 0x3713, 0x8b },
	{ 0x3714, 0x74 },
	{ 0x3811, 0x06 },
	{ 0x381c, 0x21 },
	{ 0x381d, 0x50 },
	{ 0x381e, 0x01 },
	{ 0x381f, 0x20 },
	{ 0x3820, 0x00 },
	{ 0x3821, 0x00 },
	{ 0x401c, 0x08 },
	{ 0x4301, 0xff },
	{ 0x4303, 0x00 },
	{ 0x5688, 0x03 },
	{ 0x3500, 0x00 },
	{ 0x3501, 0x4c },
	{ 0x3502, 0x0c },
	{ 0x350a, 0x00 },
	{ 0x350b, 0x05 },
};
#endif
static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 5},
	{CC_PWDN,  CC_UP},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_DOWN},
	{CC_DELAY, 5},
	{CC_RESET, CC_UP},
	{CC_DELAY, 10},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_UP},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_SGRBG10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};
#ifdef LOW_FREQ
#define OV2710_1080P_WIDTH		1920
#define OV2710_1080P_HEIGHT		1080
static const struct cc_win_size win_sizes[] = {
	{
		.width = OV2710_1080P_WIDTH,
		.height = OV2710_1080P_HEIGHT,
		.left = 0,
		.top = 0,
		.pclk = 40075200,
		.vts = 1104,
		.hts = 2420,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = OV2710_1080P_WIDTH,
		.height = OV2710_1080P_HEIGHT,

		.framerate = { 1, 15 },	/* 15fps */
	},
};

#else
#define OV2710_720P_WIDTH		1280
#define OV2710_720P_HEIGHT		720
static const struct cc_win_size win_sizes[] = {
	{
		.width = OV2710_720P_WIDTH,
		.height = OV2710_720P_HEIGHT,
		.left = 0,
		.top = 0,
		.pclk = 80150400,
		.vts = 1104,
		.hts = 2420,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = OV2710_720P_WIDTH,
		.height = OV2710_720P_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
	},
};
#endif
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

	if (id != 0x2710) {
		PRT_ERR("ID wrong! (0x%04x != 0x2710)\n", id);
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
	ret = cc_si->write_reg_list(cc_si, init_reg_list,
					ARRAY_SIZE(init_reg_list));
	if (ret != 0) {
		PRT_ERR("write %s init_reg_list failed!\n", cc_si->name);
		return -ENODEV;
	}
	ktime_get_ts(&ts_f);

	PRT_INFO("write %s init_reg_list finish.\n", cc_si->name);
	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

	return 0;
}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;

	if (on) {
		ret = cc_si->write_reg(cc_si, 0x3008, 0x02);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->write_reg(cc_si, 0x3008, 0x42);
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
	sensor_fmt_info->bayer_mode = ISP_PATTERN_BGGR;
	sensor_fmt_info->buswidth = ISP_INPUT_10BITS;

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;

	ret = cc_si->write_reg(cc_si, 0x3500, (shutter_time & 0xf0000) >> 16);
	ret = cc_si->write_reg(cc_si, 0x3501, (shutter_time & 0x0ff00) >> 8);
	ret = cc_si->write_reg(cc_si, 0x3502, (shutter_time & 0x000ff));

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, u32 gain)
{
	u8 gain_multi;
	u8 final_gain;
	int ret = 0;

	if (gain < 2 * 256) {
		gain_multi = 0;
	} else if (gain < 4 * 256) {
		gain_multi = 0x10;
		gain /= 2;
	} else if (gain < 8 * 256) {
		gain_multi = 0x20 + 0x10;
		gain /= 4;
	} else if (gain < 16 * 256) {
		gain_multi = 0x40 + 0x20 + 0x10;
		gain /= 8;
	} else {
		gain_multi = 0x80 + 0x40 + 0x20 + 0x10;
		gain /= 16;
	}

	gain = (gain - 256) / 16;
	if (gain > 0x0F)
		gain = 0x0F;

	final_gain = gain_multi | gain;

	ret = cc_si->write_reg(cc_si, 0x350a, 0);
	ret = cc_si->write_reg(cc_si, 0x350b, final_gain);

	return ret;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

	ret = cc_si->write_reg(cc_si, 0x3212, 0);

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

	ret = cc_si->write_reg(cc_si, 0x3212, 0x10);
	ret = cc_si->write_reg(cc_si, 0x3212, 0xA0);

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


#define DRIVER_NAME "ov2710_dvp"

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

static const struct of_device_id ov2710_dvp_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov2710_dvp_of_match);

static struct platform_driver ov2710_dvp_driver = {
	.driver = {
		.of_match_table	= ov2710_dvp_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ov2710_dvp_driver);

MODULE_DESCRIPTION("OV2710-DVP sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


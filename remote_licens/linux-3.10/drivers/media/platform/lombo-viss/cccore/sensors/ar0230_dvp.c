/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ar0230 dvp sensor driver code for LomboTech
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

#include <linux/of_device.h>

#include <media/lombo_viss.h>

#include <mach/debug.h>
#include "../ccc.h"

static const cc_reg_t ar0230_reg_hdr_list[] = {
	{0x3ED6, 0x2CB3},
	{0x2436, 0x000E},
	{0x320C, 0x0180},
	{0x320E, 0x0300},
	{0x3210, 0x0500},
	{0x3204, 0x0B6D},
	{0x30FE, 0x0080},
	{0x3ED8, 0x7B99},
	{0x3EDC, 0x9BA8},
	{0x3EDA, 0x9B9B},
	{0x3092, 0x006F},
	{0x3EEC, 0x1C04},
	{0x30BA, 0x779C},
	{0x3EF6, 0xA70F},
	{0x3044, 0x0410},
	{0x3ED0, 0xFF44},
	{0x3ED4, 0x031F},
	{0x30FE, 0x0080},
	{0x3EE2, 0x8866},
	{0x3EE4, 0x6623},
	{0x3EE6, 0x2263},
	{0x30E0, 0x4283},
	{0x30F0, 0x1283},
	{0x3064, 0x1802},
	{0x301A, 0x10D8},	/* RESET_REGISTER */
	{0x30B0, 0x0118},	/* DIGITAL_TEST enable low power */
	{0x31AC, 0x100C},
	{0x302A, 0x0008},	/* VT_PIX_CLK_DIV */
	{0x302C, 0x0001},	/* VT_SYS_CLK_DIV */
	{0x302E, 0x0002},	/* PRE_PLL_CLK_DIV */
	{0x3030, 0x002C},	/* PLL_MULTIPLIER */
	{0x3036, 0x000C},	/* OP_PIX_CLK_DIV */
	{0x3038, 0x0001},	/* OP_SYS_CLK_DIV */
	{0x3002, 0x0000},	/* Y_ADDR_START */
	{0x3004, 0x0004},	/* X_ADDR_START */
	{0x3006, 0x0437},	/* Y_ADDR_END */
	{0x3008, 0x0783},	/* X_ADDR_END */
	{0x300A, 0x0465},	/* 1125 vts */
	{0x300C, 0x044C},	/* 1100 hts */
	{0x3012, 0x03F4},	/* 1046 */
	{0x30A2, 0x0001},
	{0x30A6, 0x0001},
	{0x30AE, 0x0001},
	{0x30A8, 0x0001},
	{0x3040, 0x0000},
	{0x31AE, 0x0301},
	{0x3082, 0x0008},
	{0x3176, 0x0080},
	{0x3178, 0x0080},
	{0x317A, 0x0080},
	{0x317C, 0x0080},
	{0x31E0, 0x0200},
	{0x2420, 0x0000},
	{0x2440, 0x0004},
	{0x2442, 0x0080},
	{0x301E, 0x0000},
	{0x2450, 0x0000},
	{0x320A, 0x0080},
	{0x31D0, 0x0000},
	{0x2400, 0x0002},
	{0x2410, 0x0005},
	{0x2412, 0x002D},
	{0x2444, 0xF400},
	{0x2446, 0x0001},
	{0x2438, 0x0010},
	{0x243A, 0x0012},
	{0x243C, 0xFFFF},
	{0x243E, 0x0100},
	{0x3206, 0x0B08},
	{0x3208, 0x1E13},
	{0x3202, 0x0080},
	{0x3200, 0x0002},
	{0x3190, 0x0000},
	{0x318A, 0x0E74},
	{0x318C, 0xC000},
	{0x318E, 0x0200},
	{0x3192, 0x0400},
	{0x3198, 0x183C},
	{0x3060, 0x000B},
	{0x3096, 0x0480},
	{0x3098, 0x0480},
	{0x3206, 0x0B08},
	{0x3208, 0x1E13},
	{0x3202, 0x0080},
	{0x3200, 0x0002},
	{0x3100, 0x0000},
	{0x306E, 0x9210},
	{0x30BA, 0x779C},
	{0x318E, 0x0200},
	{0x301A, 0x10DC},
};

static const cc_reg_t ar0230_reg_linear_list[] = {
	{0x3ED6, 0x2CB3},
	{0x2436, 0x000E},
	{0x320C, 0x0180},
	{0x320E, 0x0300},
	{0x3210, 0x0500},
	{0x3204, 0x0B6D},
	{0x30FE, 0x0080},
	{0x3ED8, 0x7B99},
	{0x3EDC, 0x9BA8},
	{0x3EDA, 0x9B9B},
	{0x3092, 0x006F},
	{0x3EEC, 0x1C04},
	{0x30BA, 0x779C},
	{0x3EF6, 0xA70F},
	{0x3044, 0x0410},
	{0x3ED0, 0xFF44},
	{0x3ED4, 0x031F},
	{0x30FE, 0x0080},
	{0x3EE2, 0x8866},
	{0x3EE4, 0x6623},
	{0x3EE6, 0x2263},
	{0x30E0, 0x4283},
	{0x30F0, 0x1283},
	{0x3064, 0x1802},
	{0x301A, 0x10D8},	/* RESET_REGISTER */
	{0x30B0, 0x0118},	/* DIGITAL_TEST enable low power */
	{0x31AC, 0x100C},
	{0x302A, 0x0008},	/* VT_PIX_CLK_DIV */
	{0x302C, 0x0001},	/* VT_SYS_CLK_DIV */
	{0x302E, 0x0002},	/* PRE_PLL_CLK_DIV */
	{0x3030, 0x002C},	/* PLL_MULTIPLIER */
	{0x3036, 0x000C},	/* OP_PIX_CLK_DIV */
	{0x3038, 0x0001},	/* OP_SYS_CLK_DIV */
	{0x3002, 0x0000},	/* Y_ADDR_START */
	{0x3004, 0x0004},	/* X_ADDR_START */
	{0x3006, 0x0437},	/* Y_ADDR_END */
	{0x3008, 0x0783},	/* X_ADDR_END */
	{0x300A, 0x0465},	/* 1125 vts */
	{0x300C, 0x044C},	/* 1100 hts */
	{0x3012, 0x03F4},	/* 1046 */
	{0x30A2, 0x0001},
	{0x30A6, 0x0001},
	{0x30AE, 0x0001},
	{0x30A8, 0x0001},
	{0x3040, 0x0000},
	{0x31AE, 0x0301},
	{0x3082, 0x0009},
	{0x3176, 0x0080},
	{0x3178, 0x0080},
	{0x317A, 0x0080},
	{0x317C, 0x0080},
	{0x31E0, 0x0200},
	{0x2420, 0x0000},
	{0x2440, 0x0004},
	{0x2442, 0x0080},
	{0x301E, 0x0000},
	{0x2450, 0x0000},
	{0x320A, 0x0080},
	{0x31D0, 0x0001},
	{0x2400, 0x0001},
	{0x2410, 0x0005},
	{0x2412, 0x002D},
	{0x2444, 0xF400},
	{0x2446, 0x0001},
	{0x2438, 0x0010},
	{0x243A, 0x0012},
	{0x243C, 0xFFFF},
	{0x243E, 0x0100},
	{0x3206, 0x0B08},
	{0x3208, 0x1E13},
	{0x3202, 0x0080},
	{0x3200, 0x0002},
	{0x3190, 0x0000},
	{0x318A, 0x0E74},
	{0x318C, 0x0000},
	{0x318E, 0x0200},
	{0x3192, 0x0400},
	{0x3198, 0x183C},
	{0x3060, 0x000B},
	{0x3096, 0x0480},
	{0x3098, 0x0480},
	{0x3206, 0x0B08},
	{0x3208, 0x1E13},
	{0x3202, 0x0080},
	{0x3200, 0x0002},
	{0x3100, 0x0000},
	{0x306E, 0x9210},
	{0x30BA, 0x779C},
	{0x318E, 0x0200},
	{0x301A, 0x10DC},
};

static u16 ar0230_hdr_seq[] = {
	0x4558, 0x729B, 0x4A31, 0x4342, 0x8E03, 0x2A14, 0x4578, 0x7B3D,
	0xFF3D, 0xFF3D, 0xEA2A, 0x043D, 0x102A, 0x052A, 0x1535, 0x2A05,
	0x3D10, 0x4558, 0x2A04, 0x2A14, 0x3DFF, 0x3DFF, 0x3DEA, 0x2A04,
	0x622A, 0x288E, 0x0036, 0x2A08, 0x3D64, 0x7A3D, 0x0444, 0x2C4B,
	0x8F00, 0x430C, 0x2D63, 0x4316, 0x8E03, 0x2AFC, 0x5C1D, 0x5754,
	0x495F, 0x5305, 0x5307, 0x4D2B, 0xF810, 0x164C, 0x0855, 0x562B,
	0xB82B, 0x984E, 0x1129, 0x0429, 0x8429, 0x9460, 0x5C19, 0x5C1B,
	0x4548, 0x4508, 0x4588, 0x29B6, 0x8E01, 0x2AF8, 0x3E02, 0x2AFA,
	0x3F09, 0x5C1B, 0x29B2, 0x3F0C, 0x3E02, 0x3E13, 0x5C13, 0x3F11,
	0x3E0B, 0x5F2B, 0x902A, 0xF22B, 0x803E, 0x043F, 0x0660, 0x29A2,
	0x29A3, 0x5F4D, 0x192A, 0xFA29, 0x8345, 0xA83E, 0x072A, 0xFB3E,
	0x2945, 0x8821, 0x3E08, 0x2AFA, 0x5D29, 0x9288, 0x102B, 0x048B,
	0x1685, 0x8D48, 0x4D4E, 0x2B80, 0x4C0B, 0x603F, 0x282A, 0xF23F,
	0x0F29, 0x8229, 0x8329, 0x435C, 0x155F, 0x4D19, 0x2AFA, 0x4558,
	0x8E00, 0x2A98, 0x3F06, 0x1244, 0x4A04, 0x4316, 0x0543, 0x1658,
	0x4316, 0x5A43, 0x1606, 0x4316, 0x0743, 0x168E, 0x032A, 0x9C45,
	0x787B, 0x3F07, 0x2A9D, 0x3E2E, 0x4558, 0x253E, 0x068E, 0x012A,
	0x988E, 0x0012, 0x444B, 0x0343, 0x2D46, 0x4316, 0xA343, 0x165D,
	0x0D29, 0x4488, 0x102B, 0x0453, 0x0D8B, 0x1685, 0x448E, 0x032A,
	0xFC5C, 0x1D8D, 0x6057, 0x5449, 0x5F53, 0x0553, 0x074D, 0x2BF8,
	0x1016, 0x4C08, 0x5556, 0x2BB8, 0x2B98, 0x4E11, 0x2904, 0x2984,
	0x2994, 0x605C, 0x195C, 0x1B45, 0x4845, 0x0845, 0x8829, 0xB68E,
	0x012A, 0xF83E, 0x022A, 0xFA3F, 0x095C, 0x1B29, 0xB23F, 0x0C3E,
	0x023E, 0x135C, 0x133F, 0x113E, 0x0B5F, 0x2B90, 0x2AF2, 0x2B80,
	0x3E04, 0x3F06, 0x6029, 0xA229, 0xA35F, 0x4D1C, 0x2AFA, 0x2983,
	0x45A8, 0x3E07, 0x2AFB, 0x3E29, 0x4588, 0x243E, 0x082A, 0xFA5D,
	0x2992, 0x8810, 0x2B04, 0x8B16, 0x868D, 0x484D, 0x4E2B, 0x804C,
	0x0B60, 0x3F28, 0x2AF2, 0x3F0F, 0x2982, 0x2983, 0x2943, 0x5C15,
	0x5F4D, 0x1C2A, 0xFA45, 0x588E, 0x002A, 0x983F, 0x064A, 0x739D,
	0x0A43, 0x160B, 0x4316, 0x8E03, 0x2A9C, 0x4578, 0x3F07, 0x2A9D,
	0x3E12, 0x4558, 0x3F04, 0x8E01, 0x2A98, 0x8E00, 0x9176, 0x9C77,
	0x9C46, 0x4416, 0x1690, 0x7A12, 0x444B, 0x4A00, 0x4316, 0x6343,
	0x1608, 0x4316, 0x5043, 0x1665, 0x4316, 0x6643, 0x168E, 0x032A,
	0x9C45, 0x783F, 0x072A, 0x9D5D, 0x0C29, 0x4488, 0x102B, 0x0453,
	0x0D8B, 0x1686, 0x3E1F, 0x4558, 0x283E, 0x068E, 0x012A, 0x988E,
	0x008D, 0x6012, 0x444B, 0x2C2C, 0x2C2C
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_UP},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_DOWN},
	{CC_DELAY, 20},
	{CC_RESET, CC_UP},
	{CC_DELAY, 5},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN,  CC_UP},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_SGRBG12_1X12,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define FHD_WIDTH		1920
#define FHD_HEIGHT		1080
static const struct cc_win_size win_sizes[] = {
	{
		.width = 1728,
		.height = 1080,
		.left = 96,
		.top = 0,
		.pclk = 37125000,
		.vts = 1125,
		.hts = 1100,
	},
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,
		.left = 0,
		.top = 0,
		.pclk = 37125000,
		.vts = 1125,
		.hts = 1100,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = 1728,
		.height = 1080,

		.framerate = { 1, 30 },	/* 30fps */
	},

	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u16 tmp;
	int ret = 0;

	ret = cc_si->read_reg(cc_si, 0x3000, &tmp);
	if (ret != 0) {
		PRT_ERR("tmp=%x\n", tmp);
		return -ENODEV;
	}

	PRT_INFO("id: 0x%04x.\n", tmp);

	if (tmp != 0x0056) {
		PRT_ERR("ID wrong! (0x%04x != 0x0056)\n", tmp);
		return -ENODEV;
	}

	return 0;
}

static int sensor_init(struct cc_sensor_info *cc_si)
{
	struct timespec ts_s;
	struct timespec ts_f;
	const cc_reg_t *regs_list;
	u32 count = 0;
	u16 value = 0;
	int i;
	int ret = 0;

	PRT_DBG("\n");

	ret = sensor_detect(cc_si);
	if (ret != 0)
		return ret;

	if (cc_si->hdr_mode) {
		regs_list = ar0230_reg_hdr_list;
		count = ARRAY_SIZE(ar0230_reg_hdr_list);
	} else {
		regs_list = ar0230_reg_linear_list;
		count = ARRAY_SIZE(ar0230_reg_linear_list);
	}

	ret = cc_si->write_reg(cc_si, 0x301A, 0x0001); /* RESET_REGISTER */
	sensor_msleep(50);
	ret = cc_si->write_reg(cc_si, 0x301A, 0x10D8); /* RESET_REGISTER */

	ktime_get_ts(&ts_s);
	ret = cc_si->write_reg(cc_si, 0x3088, 0x8000);
	for (i = 0; i < ARRAY_SIZE(ar0230_hdr_seq); i++) {
#if 0
		PRT_INFO("write reg 0x3086: 0x%08x.\n", ar0230_hdr_seq[i]);
#endif
		ret = cc_si->write_reg(cc_si, 0x3086, ar0230_hdr_seq[i]);
		if (ret != 0) {
			PRT_ERR("write failed ret: %d.\n", ret);
			return ret;
		}
	}

	ret = cc_si->write_reg_list(cc_si, regs_list, count);
	if (ret != 0) {
		PRT_ERR("write %s regs_list failed!\n",
			cc_si->name);
		return ret;
	}

	PRT_INFO("write %s regs_list finish.\n", cc_si->name);
	ktime_get_ts(&ts_f);

	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

	if (cc_si->vflip && cc_si->mirror) {
		ret = cc_si->write_reg(cc_si, 0x301c, 0x0103);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else if (cc_si->vflip) {
		ret = cc_si->write_reg(cc_si, 0x301c, 0x0102);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else if (cc_si->mirror) {
		ret = cc_si->write_reg(cc_si, 0x301c, 0x0101);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	}

	ret = cc_si->read_reg(cc_si, 0x300a, &value);
	PRT_INFO("ret: %d. 0x300a: 0x%04x.\n", ret, value);
	if (ret != 0)
		return ret;

	ret = cc_si->read_reg(cc_si, 0x300c, &value);
	PRT_INFO("ret: %d. 0x300c: 0x%04x.\n", ret, value);
	if (ret != 0)
		return ret;

	PRT_INFO("finished.\n");

#if 0
	for (i = 0; i < count; i++) {
		if (regs_list[i].reg == CC_REG_DELAY)
			continue;

		ret = cc_si->read_reg(cc_si, regs_list[i].reg, &value);
		if (ret) {
			PRT_ERR("%s read i %d reg 0x%04x error\n",
				cc_si->name, i, regs_list[i].reg);
			return ret;
		}

		if (regs_list[i].val != value) {
			PRT_ERR("%s %d 0x%04x (0x%04x) != (0x%04x)\n",
				cc_si->name, i, regs_list[i].reg,
				value, regs_list[i].val);
		}
	}
#endif

	return 0;

}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	u16 tmp = 0;
	int ret = 0;

	if (on) {
		ret = cc_si->write_reg(cc_si, 0x301A, 0x10DC);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->read_reg(cc_si, 0x301A, &tmp);
		PRT_INFO("0x301A: 0x%04x.\n", tmp);
		tmp = tmp & (u16)(~(1 << 2));
		ret = cc_si->write_reg(cc_si, 0x301A, tmp);
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
	u16 val = 0;
	int ret = 0;

	/* read */
	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &val);
	reg_data->reg_value = val;
#if 0
	PRT_INFO("addr: 0x%x, val: 0x%04x.\n",
		reg_data->reg_add, reg_data->reg_value);
#endif
	return ret;
}

static int set_reg(struct cc_sensor_info *cc_si,
			struct viss_isp_reg_data *reg_data)
{
	u16 val = reg_data->reg_value;
	u16 read_val;
	int ret;

	/* write */
	ret = cc_si->write_reg(cc_si, reg_data->reg_add, val);
	PRT_DBG("addr=0x%04x,val=0x%04x.\n",
		reg_data->reg_add, reg_data->reg_value);

	ret = cc_si->read_reg(cc_si, reg_data->reg_add, &read_val);
	if (read_val != val)
		PRT_DBG("ret=%d,reg_add=0x%04x,val=0x%04x,read_val=0x%04x\n",
			ret, reg_data->reg_add, val, read_val);

	return ret;
}

static int viss_isp_sensor_hdr_mode(struct cc_sensor_info *cc_si,
				unsigned int *mode)
{
	if (*mode != cc_si->hdr_mode) {
		cc_si->hdr_mode = *mode;
		sensor_set_stream(cc_si, 0);
		sensor_msleep(10);
		cc_si->power_down(cc_si);
		sensor_msleep(10);
		cc_si->power_up(cc_si);
		sensor_msleep(10);
		sensor_init(cc_si);
		sensor_set_stream(cc_si, 1);
	}

	return 0;
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
	sensor_fmt_info->buswidth = ISP_INPUT_12BITS;

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;

	shutter_time = shutter_time / 16;
	ret = cc_si->write_reg(cc_si, 0x3012, (u16)shutter_time);

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, float gain)
{
	__u32 a_gain = 0, mult_gain = 0, d_gain = 0;
	int ret = 0;

	if (gain > 10880)
		return 0;

	if (gain > 43) {
		ret += cc_si->write_reg(cc_si, 0x3100, 0x004);
		a_gain = 16 * gain / 43;
	} else {
		ret += cc_si->write_reg(cc_si, 0x3100, 0x0);
		a_gain = gain;
	}

	if (cc_si->hdr_mode) {
		/* hdr and linear mode switch */
		if (a_gain < 16 * 16) {
			ret += cc_si->write_reg(cc_si, 0x3082, 0x08);
			cc_si->hdr_flag = 1;
		} else {
			ret += cc_si->write_reg(cc_si, 0x3082, 0x09);
			a_gain /= 16;
			cc_si->hdr_flag = 0;
		}
	}

	if (a_gain < 32) {
		mult_gain = 0U;
		d_gain = 128;
	} else if (a_gain < 64) {
		mult_gain = 0x1;
		a_gain /= 2;
		d_gain = 128;
	} else if (a_gain < 128) {
		mult_gain = 0x2;
		a_gain /= 4;
		d_gain = 128;
	} else if (a_gain < 256) {
		mult_gain = 0x3;
		a_gain /= 8;
		d_gain = 128;
	} else if (a_gain < 4096) {
		mult_gain = 0x4;
		d_gain = 128 * a_gain / 16 / 16;
		a_gain = 16;
	} else {
		mult_gain = 0x4;
		d_gain = 0x7FF;
		a_gain = 16;
	}

	ret += cc_si->write_reg(cc_si, 0x3060,
				(u16)(mult_gain << 4) | (a_gain - 16));
	ret += cc_si->write_reg(cc_si, 0x305E, (u16)(d_gain));

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
	case VIDIOC_VISS_ISP_SENSOR_HDR_MODE:
		viss_isp_sensor_hdr_mode(cc_si, (unsigned int *)arg);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

/*
 * ctrl for sensor finish
 */


#define DRIVER_NAME "ar0230_dvp"

static const struct of_device_id ar0230_dvp_of_match[] = {
	{
		.compatible = DRIVER_NAME,
		.data = (void *)0,
	},
	{
		.compatible = "ar0230_dvp_s",
		.data = (void *)1,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ar0230_dvp_of_match);

static int sensor_probe(struct platform_device *pdev)
{
	const struct of_device_id *match;
	struct cc_sensor_info *cc_si = NULL;
	int ret = 0;

	PRT_INFO("*********probe begin\n");

	cc_si = devm_kzalloc(&pdev->dev, sizeof(*cc_si), GFP_KERNEL);
	if (cc_si == NULL) {
		PRT_ERR("Can't alloc memory!\n");
		return -ENOMEM;
	}

	cc_si->reg_bits = 16;
	cc_si->val_bits = 16;

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

	match = of_match_device(ar0230_dvp_of_match, &pdev->dev);
	cc_si->priv_data = (u32)match->data;

	if (cc_si->priv_data != 0) {
		int tmp;

		tmp = cc_si->reset_pin;
		cc_si->reset_pin = cc_si->pwdn_pin;
		cc_si->pwdn_pin = tmp;
	}

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

static struct platform_driver ar0230_dvp_driver = {
	.driver = {
		.of_match_table	= ar0230_dvp_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(ar0230_dvp_driver);

MODULE_DESCRIPTION("AR0230-DVP sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


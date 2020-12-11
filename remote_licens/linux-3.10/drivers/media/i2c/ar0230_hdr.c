/*
 * Copyright (C) 2005-2006 Micronas USA Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (Version 2) as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-subdev.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/viss_isp.h>

MODULE_DESCRIPTION("ON ar0230 sensor driver");
MODULE_LICENSE("GPL v2");

#include <mach/debug.h>

#define DRIVER_NAME "ar0230_hdr"
enum SENSOR_ID {
	ID_AR0230,
};

typedef struct tag_ar0230_config {
	struct v4l2_subdev	sd;
	struct media_pad	pad;
	enum v4l2_mbus_type	bus_type;

	/* External master clock frequency */
	unsigned long		mclk_frequency;

	/* Protects the struct fields below */
	struct mutex		lock;

	struct i2c_client	*client;

	/* Exposure row interval in us */
	unsigned int		exp_row_interval;

	unsigned short		id;

	u8			mirror;

	/* YUYV sequence (pixel format) control register */
	u8			tslb_reg;
	struct			v4l2_mbus_framefmt format;

	int			reset_gpio;
	int			pwdn_gpio;
	int			bsel_gpio;

#if 0
	int			streaming;
	int			power;
#endif
} ar0230_config_t;

typedef struct tag_sensor_reg {
	u16	reg_add;
	u16	reg_value;
} sensor_reg_t;

static void ar0230_reset_cycle(ar0230_config_t *ar0230);
static void ar0230_pwdn(ar0230_config_t *ar0230);

ar0230_config_t *g_ar0230;

static const sensor_reg_t ar0230_reg_hdr_list[] = {
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

static const sensor_reg_t ar0230_reg_linear_list[] = {
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

struct ar0230_reg_info {
	u32			list_id;
	const sensor_reg_t	*reg_list;
	u32			list_sz;
	u32			ms_dly;
};

enum ar0230_list_id {
	LIST_AR0230_LINEAR_REGS,
	LIST_AR0230_HDR_REGS,
};

struct ar0230_reg_info ar0230_reg_lists[] = {
	{LIST_AR0230_LINEAR_REGS, ar0230_reg_linear_list,
		ARRAY_SIZE(ar0230_reg_linear_list), 0},
	{LIST_AR0230_HDR_REGS, ar0230_reg_hdr_list,
		ARRAY_SIZE(ar0230_reg_hdr_list), 0},
};

struct ar0230_framesize {
	u16		width;
	u16		height;
	u16		max_exp_lines;
	const u8	*regs;
};

#define FHD_WIDTH		1920
#define FHD_HEIGHT		1080

static const struct ar0230_framesize ar0230_framesizes[] = {
	{
		.width		= FHD_WIDTH,
		.height		= FHD_HEIGHT,
		.max_exp_lines	= 498,
	},
};

/* the supported format of this sensor is MEDIA_BUS_FMT_UYVY8_2X8 */
static const struct v4l2_mbus_framefmt ar0230_formats[] = {
	{
		.code = V4L2_MBUS_FMT_UYVY8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

static int reg_read16(struct i2c_client *client, u16 reg, u16 *val)
{
	struct i2c_msg msg[2];
	u8 buf[2];
	int ret, tmp_val;
	u16 reg_val;

	unsigned char command[2] = { reg >> 8, reg & 0xff};

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 2;
	msg[0].buf   = (u8 *)command;

	msg[1].addr  = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len   = 2;
	msg[1].buf   = buf;

	/*
	 * if return value of this function is < 0,
	 * it mean error.
	 * else, under 16bit is valid data.
	 */
	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret < 0)
		return ret;

	memcpy(&tmp_val, buf, 2);
	reg_val =  ((tmp_val & 0x00ff) << 8) | ((tmp_val & 0xff00) >> 8);
	memcpy(val, &reg_val, 2);

	return 0;
}

static int reg_write16(struct i2c_client *client, u16 reg, u16 val)
{
	struct i2c_msg msg;
	u8 buf[4] = {reg >> 8, reg & 0xff, val >> 8, val & 0xff};
	int ret;

	msg.addr  = client->addr;
	msg.flags = 0;
	msg.len   = 4;
	msg.buf   = buf;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret >= 0)
		ret = 0;

	return ret;

}

u16 ar0230_hdr_seq[] = {
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

static int reg_list_write(struct i2c_client *client,
			enum ar0230_list_id list_id)
{
	const sensor_reg_t *reg_list;
	int i = 0;
	int ret = 0;

	reg_write16(client, 0x301A, 0x0001); /* RESET_REGISTER */
	mdelay(50);
	reg_write16(client, 0x301A, 0x10D8); /* RESET_REGISTER */
	/*
	mdelay(200);
	reg_write16(client, 0x301A, 0x0059);
	mdelay(200);
	*/
	reg_write16(client, 0x3088, 0x8000);
	for (i = 0; i < ARRAY_SIZE(ar0230_hdr_seq); i++) {
		/*PRT_INFO("write reg 0x%08x: 0x%08x.\n",
			0x3086,
			ar0230_hdr_seq[i]);
			*/
		ret = reg_write16(client, 0x3086, ar0230_hdr_seq[i]);
		if (ret != 0) {
			PRT_ERR("write failed ret: %d.\n", ret);
			return ret;
		}
	}

	reg_list = ar0230_reg_lists[list_id].reg_list;
	for (i = 0; i < ar0230_reg_lists[list_id].list_sz; i++) {
		/*
		PRT_INFO("write reg 0x%08x: 0x%08x.\n",
			reg_list[i].reg_add,
			reg_list[i].reg_value);
		*/
		ret = reg_write16(client, reg_list[i].reg_add,
				reg_list[i].reg_value);
		if (ret != 0) {
			PRT_ERR("write failed ret: %d.\n", ret);
			return ret;
		}
#if 0
		if (ar0230_reg_lists[list_id].ms_dly != 0)
			mdelay(ar0230_reg_lists[list_id].ms_dly);
#endif
	}

	return ret;
}

static int ar0230_check_present(ar0230_config_t *ar0230)
{
	struct i2c_client *client = ar0230->client;
	u16 tmp;
	int ret = 0;

	ret = reg_read16(client, 0x3000, &tmp);
	PRT_INFO("0x%x.\n", tmp);

	if (tmp != 0x0056) {
		PRT_ERR("ID wrong! (tmp=0x%x)\n", tmp);
		return -ENODEV;
	}

	return 0;
}

static int cmos_ar0230_init(ar0230_config_t *ar0230)
{
	struct i2c_client *client = ar0230->client;
	int ret = 0;
	u16 value = 0;

	ret = ar0230_check_present(ar0230);
	if (ret != 0) {
		PRT_ERR("sensor ar0230 not present!\n");
		return ret;
	}

	ret = reg_list_write(client, LIST_AR0230_HDR_REGS);
	if (ret != 0)
		return ret;

	if (ar0230->mirror) {
		ret = reg_write16(client, 0x301c, 0x0101);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else {
		ret = reg_write16(client, 0x301c, 0x0102);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	}

	ret = reg_read16(client, 0x300a, &value);
	PRT_INFO("ret: %d. 0x300a: 0x%08x.\n", ret, value);
	if (ret != 0)
		return ret;

	ret = reg_read16(client, 0x300c, &value);
	PRT_INFO("ret: %d. 0x300c: 0x%08x.\n", ret, value);
	if (ret != 0)
		return ret;

	PRT_INFO("finished.\n");

	return 0;
}

static inline ar0230_config_t *to_ar0230(struct v4l2_subdev *sd)
{
	return container_of(sd, ar0230_config_t, sd);
}

static const struct v4l2_mbus_framefmt *find_sensor_format(
	struct v4l2_mbus_framefmt *mf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar0230_formats); i++)
		if (mf->code == ar0230_formats[i].code)
			return &ar0230_formats[i];

	return &ar0230_formats[0];
}

#define ar0230_SENSOR_MAX_WIDTH		1412
#define ar0230_SENSOR_MAX_HEIGHT	1412
#define ar0230_SENSOR_MIN_WIDTH		32
#define ar0230_SENSOR_MIN_HEIGHT	32
static void ar0230_try_format(struct v4l2_mbus_framefmt *mf)
{
	const struct v4l2_mbus_framefmt *fmt;

	fmt = find_sensor_format(mf);
	PRT_INFO("fmt->code:0x%x\n", fmt->code);
	PRT_INFO("mf->code:0x%x\n", mf->code);
	mf->code = fmt->code;
	mf->field = V4L2_FIELD_NONE;

#if 0
	mf->width = 1280;
	mf->height = 720;
#endif

#if 0
	v4l_bound_align_image(&mf->width, ar0230_SENSOR_MIN_WIDTH,
			      ar0230_SENSOR_MAX_WIDTH, 0,
			      &mf->height, ar0230_SENSOR_MIN_HEIGHT,
			      ar0230_SENSOR_MAX_HEIGHT, 0, 0);
#endif

}

static int ar0230_set_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			  struct v4l2_subdev_format *fmt)
{
	ar0230_config_t *ar0230 = to_ar0230(sd);
	struct v4l2_mbus_framefmt *mf = NULL;

	PRT_DBG("\n");

	ar0230_try_format(&fmt->format);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
	}

	if (mf) {
		mutex_lock(&ar0230->lock);
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
			PRT_DBG("\n");
			*mf = fmt->format;
		}
		mutex_unlock(&ar0230->lock);
	}

	return 0;
}

static int ar0230_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			  struct v4l2_subdev_format *fmt)
{
	ar0230_config_t *ar0230 = to_ar0230(sd);
	struct v4l2_mbus_framefmt *mf;

	PRT_DBG("\n");

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, 0);
		fmt->format = *mf;
		return 0;
	}

	/* s3c-camif --> V4L2_SUBDEV_FORMAT_ACTIVE */
	mutex_lock(&ar0230->lock);
	fmt->format = ar0230->format;
	mutex_unlock(&ar0230->lock);

	return 0;
}

/*
 * V4L2 subdev video and pad level operations
 */
static void ar0230_get_default_format(struct v4l2_mbus_framefmt *mf)
{
	mf->width = ar0230_framesizes[0].width;
	mf->height = ar0230_framesizes[0].height;
	mf->colorspace = ar0230_formats[0].colorspace;
	mf->code = ar0230_formats[0].code;
	mf->field = V4L2_FIELD_NONE;
}

static int get_reg(struct v4l2_subdev *sd, struct viss_isp_reg_data *reg_data)
{
	struct i2c_client *client = to_ar0230(sd)->client;
	u16 val = 0;
	int ret = 0;

	/* read */
	ret = reg_read16(client, reg_data->reg_add, &val);
	reg_data->reg_value = val;
#if 0
	PRT_INFO("get_reg add: 0x%x, val: 0x%04x.\n",
		reg_data->reg_add, reg_data->reg_value);
#endif
	return ret;
}

static int set_reg(struct v4l2_subdev *sd,
			struct viss_isp_reg_data *reg_data)
{
	struct i2c_client *client = to_ar0230(sd)->client;
	u16 val = reg_data->reg_value;
	u16 read_val;
	int ret;
	if (reg_data->reg_add == 0x3012)
		val = val/16;

	/* write */
	ret = reg_write16(client, reg_data->reg_add, val);
	PRT_DBG("set_reg add: 0x%x, val: 0x%04x.\n",
		reg_data->reg_add, reg_data->reg_value);


	ret = reg_read16(client, reg_data->reg_add, &read_val);
	if (read_val != val)
		PRT_ERR("reg_add=0x%x,val=0x%02x,read_val=0x%04x\n",
			reg_data->reg_add, val, read_val);
	return ret;
}

static int viss_get_sensor_info(struct v4l2_subdev *sd,
		struct viss_isp_sensor_info *sensor_info)
{
	const char *sensor_name = NULL;
	int ret = 0;

	sensor_name = "ar0230";

	ret = snprintf(sensor_info->sensor_name,
		sizeof(sensor_info->sensor_name), sensor_name);

	return ret;
}

static int viss_get_sensor_fmt_info(struct v4l2_subdev *sd,
		struct viss_isp_sensor_fmt_info *sensor_fmt_info)
{

	int ret = 0;

	sensor_fmt_info->pclk = 37125000;
	sensor_fmt_info->vts = 1125;
	sensor_fmt_info->hts = 1100;
	sensor_fmt_info->input_width = 1920;
	sensor_fmt_info->input_height = 1080;
	sensor_fmt_info->output_widht = 1920;
	sensor_fmt_info->output_height = 1080;
	sensor_fmt_info->bayer_mode = ISP_PATTERN_GRBG;
	sensor_fmt_info->buswidth = ISP_INPUT_12BITS;

	return ret;
}

static int viss_isp_set_shutter(struct v4l2_subdev *sd, __u32 shutter_time)
{
	struct i2c_client *client = to_ar0230(sd)->client;
	int ret = 0;

	shutter_time = shutter_time / 16;

	/* hdr and linear mode switch */
	if (shutter_time < 900) {
		ret += reg_write16(client, 0x3082, 0x08);
		ret += reg_write16(client, 0x3012,
			(u16)shutter_time);
	} else {
		ret += reg_write16(client, 0x3082, 0x09);
		ret += reg_write16(client, 0x3012,
			(u16)shutter_time / 16);
	}

	return ret;
}

static int viss_isp_set_gain(struct v4l2_subdev *sd, __u32 gain)
{
	struct i2c_client *client = to_ar0230(sd)->client;
	int ret = 0;
	__u32 a_gain = 0, mult_gain = 0, d_gain = 0;

	if (gain > 10880)
		return 0;

	if (gain > 43) {
		ret += reg_write16(client, 0x3100, 0x004);
		a_gain = 16 * gain / 43;
	} else {
		ret += reg_write16(client, 0x3100, 0x0);
		a_gain = gain;
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

	ret += reg_write16(client, 0x3060,
				(u16)(mult_gain << 4) | (a_gain - 16));
	ret += reg_write16(client, 0x305E, (u16)(d_gain));

	return ret;
}

static int viss_isp_exp_ctrl(struct v4l2_subdev *sd,
	struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

	ret = viss_isp_set_shutter(sd, exp_gain->exp_time);
	if (ret < 0) {
		PRT_ERR("viss_isp_set_shutter failed %d.\n", ret);
		PRT_ERR("viss_isp_exp_ctrl %u, %u.\n",
			exp_gain->exp_time, exp_gain->exp_gain);
		return -ENODEV;
	}

	ret = viss_isp_set_gain(sd, exp_gain->exp_gain / 16);
	if (ret < 0) {
		PRT_ERR("viss_isp_set_gain failed %d.\n", ret);
		PRT_ERR("viss_isp_exp_ctrl %u, %u.\n",
			exp_gain->exp_time, exp_gain->exp_gain);
		return -ENODEV;
	}

	return ret;
}

#if 0
static int viss_set_mirror(struct v4l2_subdev *sd, unsigned int *en)
{
	int ret = 0;
	struct i2c_client *client = to_ar0230(sd)->client;

	PRT_DBG("viss_set_mirror en: %d.\n", *en);
	if (*en) {
		ret = reg_write16(client, 0x301c, 0x0103);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else {
		ret = reg_write16(client, 0x301c, 0x0100);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	}
	return ret;
}
#endif

/*
 * V4L2 subdev internal operations
 */
static long ar0230_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_VISS_GET_SENSOR:
		ret = get_reg(sd, (struct viss_isp_reg_data *)arg);
		break;
	case VIDIOC_VISS_SET_SENSOR:
		ret = set_reg(sd, (struct viss_isp_reg_data *)arg);
		break;
	case VIDIOC_VISS_GET_SENSOR_INFO:
		ret = viss_get_sensor_info(sd,
			(struct viss_isp_sensor_info *)arg);
		break;
	case VIDIOC_VISS_ISP_EXP_CTRL:
		ret = viss_isp_exp_ctrl(sd,
			(struct viss_isp_exp_gain *)arg);
		break;
	case VIDIOC_VISS_ISP_SENSOR_FMT_INFO:
		ret = viss_get_sensor_fmt_info(sd,
			(struct viss_isp_sensor_fmt_info *)arg);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

static int ar0230_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
#if 0
	struct v4l2_mbus_framefmt *format = v4l2_subdev_get_try_format(fh, 0);
	struct v4l2_rect *crop = v4l2_subdev_get_try_crop(fh, 0);
#endif

	return 0;
}

static int ar0230_registered(struct v4l2_subdev *sd)
{
	ar0230_config_t *ar0230 = to_ar0230(sd);
	struct i2c_client *client = ar0230->client;
	int ret = 0;

	mutex_lock(&ar0230->lock);

	ar0230->id = ID_AR0230;

	if ((ar0230->reset_gpio != 0) && (ar0230->pwdn_gpio != 0)) {
		ret = devm_gpio_request(&client->dev, ar0230->reset_gpio,
					"reset_gpio");
		if (ret < 0) {
			PRT_ERR("reset_gpio %d request failed (reset_gpio)\n",
				ar0230->reset_gpio);
			ar0230->reset_gpio = 0;
			ar0230->pwdn_gpio = 0;
			goto unlock;
		}

		ret = devm_gpio_request(&client->dev, ar0230->pwdn_gpio,
					"pwdn_gpio");
		if (ret < 0) {
			PRT_ERR("pwdn_gpio %d request failed (pwdn_gpio)\n",
				ar0230->pwdn_gpio);
			devm_gpio_free(&client->dev, ar0230->reset_gpio);
			ar0230->reset_gpio = 0;
			ar0230->pwdn_gpio = 0;
			goto unlock;
		}
	}

	ar0230_reset_cycle(ar0230);
	ret = ar0230_check_present(ar0230);
	ar0230_pwdn(ar0230);
	if (ret == 0) {
		PRT_INFO("sensor ar0230 present\n");
		ar0230->id = ID_AR0230;
	}

unlock:
	mutex_unlock(&ar0230->lock);

	return ret;
}

static const struct v4l2_subdev_internal_ops ar0230_subdev_internal_ops = {
	.registered = ar0230_registered,
	.open = ar0230_open,
};

static void ar0230_reset_cycle(ar0230_config_t *ar0230)
{
	if ((ar0230->reset_gpio == 0) || (ar0230->pwdn_gpio == 0))
		return;

#if 0
	/* reset low */
	gpio_direction_output(ar0230->reset_gpio, 1);
	mdelay(10);
	/* pwdn low */
	gpio_direction_output(ar0230->pwdn_gpio, 0);
	mdelay(20);
	/* reset high */
	gpio_direction_output(ar0230->reset_gpio, 1);
#else
	/* reset low */
	gpio_direction_output(ar0230->reset_gpio, 0);
	mdelay(10);
	/* pwdn high */
	gpio_direction_output(ar0230->pwdn_gpio, 1);
	mdelay(10);
	/* pwdn low */
	gpio_direction_output(ar0230->pwdn_gpio, 0);
	mdelay(20);
	/* reset high */
	gpio_direction_output(ar0230->reset_gpio, 1);
	mdelay(5);
#endif
}

static void ar0230_pwdn(ar0230_config_t *ar0230)
{
#if 1
	if ((ar0230->reset_gpio == 0) || (ar0230->pwdn_gpio == 0))
		return;

	/* reset low */
	gpio_direction_output(ar0230->reset_gpio, 0);
	mdelay(10);
	/* pwdn high */
	gpio_direction_output(ar0230->pwdn_gpio, 1);
#endif
}

static int ar0230_set_power(struct v4l2_subdev *sd, int on)
{
	ar0230_config_t *ar0230 = to_ar0230(sd);
	int ret = 0;

	if (on) {
#if 1
		ar0230_reset_cycle(ar0230);
#endif
		ret = cmos_ar0230_init(ar0230);
	} else {
		ar0230_pwdn(ar0230);
	}

	PRT_INFO("on:%d\n", on);
	return ret;
}

static int ar0230_s_stream(struct v4l2_subdev *sd, int on)
{
	int ret = 0;
	u16 tmp = 0;
	ar0230_config_t *ar0230 = to_ar0230(sd);
	struct i2c_client *client = ar0230->client;

	ret = reg_read16(client, 0x3000, &tmp);
	PRT_INFO("0x%x.\n", tmp);

	ret = reg_read16(client, 0x300a, &tmp);
	if (ret != 0)
		return ret;
	PRT_INFO("ret: %d. 0x300a: 0x%08x.\n", ret, tmp);

	ret = reg_read16(client, 0x300c, &tmp);
	if (ret != 0)
		return ret;
	PRT_INFO("ret: %d. 0x300c: 0x%08x.\n", ret, tmp);

	ret = reg_read16(client, 0x301A, &tmp);
	if (ret != 0)
		return ret;
	PRT_INFO("0x%08x.\n", tmp);

	ret = reg_write16(client, 0x301A, 0x10DC);
	if (ret != 0)
		return ret;

	ret = reg_read16(client, 0x301A, &tmp);
	if (ret != 0)
		return ret;
	PRT_INFO("0x%08x.\n", tmp);

	PRT_INFO("*************************on:%d\n", on);
	return ret;
}


static const struct v4l2_subdev_pad_ops ar0230_pad_ops = {
#if 0
	.enum_mbus_code = ar0230_enum_mbus_code,
	.enum_frame_size = ar0230_enum_frame_sizes,
#endif
	.get_fmt = ar0230_get_fmt,
	.set_fmt = ar0230_set_fmt,
};

/* -----------------------------------------------------------------------------
 * V4L2 subdev internal operations
 */
static const struct v4l2_subdev_core_ops ar0230_subdev_core_ops = {
	.s_power = ar0230_set_power,
	.ioctl = ar0230_ioctl,
	.log_status = v4l2_ctrl_subdev_log_status,
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops ar0230_video_ops = {
	.s_stream		= ar0230_s_stream,
};

#ifdef CONFIG_VIDEO_V4L2_SUBDEV_API
/*
 * 这个结构体是作为v4l2子设备的op,
 * 会通过video device的ioctl调用实现。
 */
static const struct v4l2_subdev_ops ar0230_subdev_ops = {
	.core  = &ar0230_subdev_core_ops,
	.pad   = &ar0230_pad_ops,
	.video = &ar0230_video_ops,
};
#endif

/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_ops ar0230_ops;

static u16 g_cur_reg_addr = 0xFFFF;
static ssize_t
ar0230_sysfs_reg_node_store(struct device *dev, struct device_attribute *attr,
		      const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	int addr, value;
	char end;
	int res;

	res = sscanf(buf, "%x %x%c", &addr, &value, &end);
	if (res < 1) {
		PRT_ERR("Can't parse reg address (%s)\n", buf);
		return -EINVAL;
	}

	if ((res > 2) && (end != '\n')) {
		PRT_ERR("Extra parameters (%s)\n", buf);
		return -EINVAL;
	}

	if (addr > 0xFFFF) {
		PRT_ERR("addr is too large (%s)!\n", buf);
		return -EINVAL;
	}

	g_cur_reg_addr = (u16)addr;

	if (res >= 2) {
		if (value > 0xFFFF) {
			PRT_ERR("value is too large (%s)!\n", buf);
			return -EINVAL;
		}

		res = reg_write16(client, g_cur_reg_addr, (u16)value);
		if (res) {
			PRT_ERR("\n");
			return res;
		}
	}

	return count;
}

static ssize_t
ar0230_sysfs_reg_node_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	u16 value;
	int res;

	if (g_cur_reg_addr == 0xFFFF) {
		PRT_ERR("write reg addr to reg_node first!\n");
		return -EINVAL;
	}

	res = reg_read16(client, g_cur_reg_addr, &value);
	if (res != 0) {
		PRT_ERR("read reg (0x%04x) failed!\n", g_cur_reg_addr);
		return res;
	}

	return sprintf(buf, "0x%04x\n", value);
}

static DEVICE_ATTR(reg_node, S_IRUGO | S_IWUSR | S_IWGRP,
			ar0230_sysfs_reg_node_show,
			ar0230_sysfs_reg_node_store);


static ssize_t
ar0230_sysfs_board_version_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	if (g_ar0230 == NULL) {
		PRT_ERR("g_ar0230 is NULL!\n");
		return -ENODEV;
	}

	return sprintf(buf, "%d", g_ar0230->mirror);
}

static DEVICE_ATTR(board_version, S_IRUGO,
			ar0230_sysfs_board_version_show,
			NULL);

/* 该函数会在camif_register_sensor时注册匹配时调用 */
static int ar0230_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int ret = 0;
	enum of_gpio_flags flags;
	struct i2c_adapter *adapter = client->adapter;
	struct device_node *np = client->dev.of_node;
	struct v4l2_subdev *sd;

	ar0230_config_t *ar0230;

	PRT_INFO("*********ar0230 probe begin\n");

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	ar0230 = devm_kzalloc(&client->dev, sizeof(*ar0230), GFP_KERNEL);
	if (ar0230 == NULL)
		return -ENOMEM;

	ar0230->reset_gpio = of_get_named_gpio_flags(np, "reset_gpio",
							0, &flags);

	ar0230->pwdn_gpio = of_get_named_gpio_flags(np, "pwdn_gpio",
							0, &flags);

	if (!gpio_is_valid(ar0230->reset_gpio) ||
		!gpio_is_valid(ar0230->pwdn_gpio)) {
		PRT_ERR("get gpio %d and gpio %d failed\n",
				ar0230->reset_gpio, ar0230->pwdn_gpio);
		ar0230->reset_gpio = 0;
		ar0230->pwdn_gpio = 0;
	}

	PRT_INFO("ar0230->mirror=0\n");
	ar0230->bsel_gpio = of_get_named_gpio_flags(np, "bsel_gpio",
							0, &flags);
	if (!gpio_is_valid(ar0230->bsel_gpio)) {
		ar0230->mirror = 0;
	} else {
		ret = devm_gpio_request(&client->dev, ar0230->bsel_gpio,
					"bsel_gpio");
		if (ret < 0) {
			PRT_WARN("request bsel_gpio %d failed\n",
				ar0230->bsel_gpio);
		} else {
			ret = gpio_direction_input(ar0230->bsel_gpio);
			if (ret < 0) {
				PRT_WARN("set input of bsel_gpio %d failed\n",
					ar0230->bsel_gpio);
			} else {
				ar0230->mirror =
					__gpio_get_value(ar0230->bsel_gpio);
			}

			PRT_INFO("ar0230->mirror=%d\n", ar0230->mirror);
			devm_gpio_free(&client->dev, ar0230->bsel_gpio);
		}

		ret = device_create_file(&client->dev, &dev_attr_board_version);
		if (ret < 0)
			PRT_WARN("Can't create reg_node!\n");
	}

	mutex_init(&ar0230->lock);

	ar0230->client = client;
	sd = &ar0230->sd;

	v4l2_i2c_subdev_init(sd, client, &ar0230_subdev_ops);
	strlcpy(sd->name, DRIVER_NAME, sizeof(sd->name));

	sd->internal_ops = &ar0230_subdev_internal_ops;

	client->flags = I2C_CLIENT_SCCB;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

#if defined(CONFIG_MEDIA_CONTROLLER)
	/* 设置pad的源端entity初始化，主要用于发送内容。*/
	ar0230->pad.flags = MEDIA_PAD_FL_SOURCE;
	/* 设置v4l2 subdevice实体的类型-- sensor */
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	/* 初始化v4l2 subdevice media实体 */
	ret = media_entity_init(&sd->entity, 1, &ar0230->pad, 0);
	if (ret < 0)
		return ret;
#endif
	ar0230_get_default_format(&ar0230->format);

	g_ar0230 = ar0230;

	ret = device_create_file(&client->dev, &dev_attr_reg_node);
	if (ret < 0)
		PRT_WARN("Can't create reg_node!\n");

	PRT_INFO("probe end\n");
	return 0;
}


static int ar0230_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l_info(client, "%s %d -----------------\n",
		__func__, __LINE__);

	v4l2_device_unregister_subdev(sd);

#if defined(CONFIG_MEDIA_CONTROLLER)
	media_entity_cleanup(&sd->entity);
#endif

	return 0;
}


/* 摄像头sensor设备信息表 */
static const struct i2c_device_id ar0230_id[] = {
	{ "ar0230_hdr", 0 },
	{}
};
/*
MODULE_DEVICE_TABLE(i2c, ar0230_id);
*/
/* i2c总线驱动结构体成员 */
static struct i2c_driver ar0230_driver = {
	.driver = {
		.name	= "ar0230_hdr",
	},
	.probe = ar0230_probe,
	.remove = ar0230_remove,
	.id_table = ar0230_id,
};

/* i2c总线ar0230摄像头驱动注册 */
module_i2c_driver(ar0230_driver);


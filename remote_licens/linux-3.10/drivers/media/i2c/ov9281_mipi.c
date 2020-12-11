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
#include <media/lombo_viss.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>

MODULE_DESCRIPTION("OV9281-MIPI sensor driver");
MODULE_LICENSE("GPL v2");

#include <mach/debug.h>

#define OV9281_RAW8

#define OV9281_WIDTH		1280
#define OV9281_HEIGHT		800

static struct i2c_client *ov9281_client;

typedef struct ov9281_config {
	struct v4l2_subdev sd;
	struct media_pad pad;
#if 0
	enum v4l2_mbus_type bus_type;

	/* External master clock frequency */
	unsigned long mclk_frequency;
#endif
	/* Protects the struct fields below */
	struct mutex lock;

	struct i2c_client *client;

	/* Exposure row interval in us */
	unsigned int exp_row_interval;

	unsigned short id;

	/* YUYV sequence (pixel format) control register */
	u8 tslb_reg;
	struct v4l2_mbus_framefmt format;

	int reset_gpio;
	int pwdn_gpio;

#if 0
	int streaming;
	int power;
#endif
} ov9281_config_t;

typedef struct sensor_reg {
	u16 reg_add;
	u8 reg_value;
} sensor_reg_t;

static void ov9281_reset_cycle(ov9281_config_t *ov9281);
static void ov9281_pwdn(ov9281_config_t *ov9281);

#if 1
/*
 * 2lane YUV init
 */
static const struct sensor_reg ov9281_reg_list[] = {
	{ 0x0103, 0x01 }, /* software reset */
	{ 0x0302, 0x32 },
	/* { OV5640_MIPI_REG_DELAY_FLAG, 0x5 }, */ /* mdelay(5); */
	{ 0x030d, 0x50 },
	{ 0x030e, 0x02 },
	{ 0x3001, 0x00 },
	{ 0x3004, 0x00 },
	{ 0x3005, 0x00 },
	{ 0x3006, 0x04 },
	{ 0x3011, 0x0a },
	{ 0x3013, 0x18 },
	{ 0x301c, 0xf0 },
	{ 0x3022, 0x01 },
	{ 0x3030, 0x10 },
	{ 0x3039, 0x32 },
	{ 0x303a, 0x00 },
	{ 0x3500, 0x00 },
	{ 0x3501, 0x2a },
	{ 0x3502, 0x90 },
	{ 0x3503, 0x08 },
	{ 0x3505, 0x8c },
	{ 0x3507, 0x03 },
	{ 0x3508, 0x00 },
	{ 0x3509, 0x10 },
	{ 0x3610, 0x80 },
	{ 0x3611, 0xa0 },
	{ 0x3620, 0x6e },
	{ 0x3632, 0x56 },
	{ 0x3633, 0x78 },
	{ 0x3662, 0x05 },
	{ 0x3666, 0x00 },
	{ 0x366f, 0x5a },
	{ 0x3680, 0x84 },
	{ 0x3712, 0x80 },
	{ 0x372d, 0x22 },
	{ 0x3731, 0x80 },
	{ 0x3732, 0x30 },
	{ 0x3778, 0x00 },
	{ 0x377d, 0x22 },
	{ 0x3788, 0x02 },
	{ 0x3789, 0xa4 },
	{ 0x378a, 0x00 },
	{ 0x378b, 0x4a },
	{ 0x3799, 0x20 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x00 },
	{ 0x3804, 0x05 },
	{ 0x3805, 0x0f },
	{ 0x3806, 0x03 },
	{ 0x3807, 0x2f },
	{ 0x3808, 0x05 },
	{ 0x3809, 0x00 },
	{ 0x380a, 0x03 },
	{ 0x380b, 0x20 },
	{ 0x380c, 0x02 },
	{ 0x380d, 0xd8 },
	{ 0x380e, 0x03 },
	{ 0x380f, 0x8e },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x08 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x08 },

	{ 0x3814, 0x11 },
	{ 0x3815, 0x11 },

	{ 0x3820, 0x40 },
	{ 0x3821, 0x00 },

	{ 0x382c, 0x05 },

	{ 0x382d, 0xb0 },
	{ 0x389d, 0x00 },
	{ 0x3881, 0x42 },
	{ 0x3882, 0x01 },
	{ 0x3883, 0x00 },
	{ 0x3885, 0x02 },
	{ 0x38a8, 0x02 },
	{ 0x38a9, 0x80 },
	{ 0x38b1, 0x00 },
	{ 0x38b3, 0x02 },
	{ 0x38c4, 0x00 },
	{ 0x38c5, 0xc0 },
	{ 0x38c6, 0x04 },
	{ 0x38c7, 0x80 },
	{ 0x3920, 0xff },
	{ 0x4003, 0x40 },
	{ 0x4008, 0x04 },
	{ 0x4009, 0x0b },
	{ 0x400c, 0x00 },
	{ 0x400d, 0x07 },
	{ 0x4010, 0x40 },
	{ 0x4043, 0x40 },
	{ 0x4307, 0x30 },
	{ 0x4317, 0x00 },
	{ 0x4501, 0x00 },
	{ 0x4507, 0x00 },
	{ 0x4509, 0x00 },
	{ 0x450a, 0x08 },
	{ 0x4601, 0x04 },
	{ 0x470f, 0x00 },
	{ 0x4f07, 0x00 },
	{ 0x4800, 0x00 },
	{ 0x5000, 0x9f },
	{ 0x5001, 0x00 },
	{ 0x5e00, 0x00 },
	{ 0x5d00, 0x07 },
	{ 0x5d01, 0x00 },
	{ 0x4f00, 0x04 },
	{ 0x4f10, 0x00 },
	{ 0x4f11, 0x98 },
	{ 0x4f12, 0x0f },
	{ 0x4f13, 0xc4 },
	/* { 0x0100, 0x01 }, */

};
#else
/*
 * 2lane YUV init
 */
static const struct sensor_reg ov9281_reg_list[] = {
	{0x0100, 0x00},
	{0x030a, 0x00},
	{0x0300, 0x03},
	{0x0301, 0x00},
	{0x0302, 0x3c},
	{0x0303, 0x01},
	{0x0304, 0x02},
	{0x0305, 0x03},
	{0x0306, 0x01},
	{0x0314, 0x00},
	{0x030b, 0x03},
	{0x030c, 0x00},
	{0x030d, 0x3c},
	{0x030f, 0x05},
	{0x030e, 0x02},
	{0x0313, 0x02},
	{0x0312, 0x05},
	{0x0100, 0x01},
	{0x3004, 0x03},
	{0x3005, 0xff},
	{0x3006, 0xee},
	{0x3011, 0x00},
	{0x3025, 0x00},
	{0x3026, 0x00},
	{0x3027, 0x00},
	{0x4311, 0x60},
	{0x4312, 0x00},
	{0x4313, 0x10},
	{0x4314, 0x00},
	{0x4315, 0x01},
	{0x4316, 0x00},
	{0x4317, 0x01},
#if 0
	{0x4701, 0x00},
	{0x4702, 0x00},
	{0x4703, 0x02},
	{0x4704, 0x10},
	{0x4705, 0x06},
	{0x4706, 0x00},
	{0x4707, 0x10},
	{0x4708, 0x09},
#else
	{0x470f, 0x00},
	{0x4f07, 0x00},
	{0x4800, 0x00},
#endif
	{0x3800, 0x00},
	{0x3801, 0x00},
	{0x3802, 0x00},
	{0x3803, 0x00},
	{0x3804, 0x05},
	{0x3805, 0x07},
	{0x3806, 0x03},
	{0x3807, 0x27},
	{0x3808, 0x05},
	{0x3809, 0x00},
	{0x380a, 0x03},
	{0x380b, 0x20},
	{0x380c, 0x05},
	{0x380d, 0x15},
	{0x380e, 0x04},
	{0x380f, 0xce},
	{0x3820, 0x00},
	{0x3821, 0x04},
	{0x3406, 0x01},
	{0x3400, 0x0f},
	{0x3401, 0xff},
	{0x3402, 0x0f},
	{0x3403, 0xff},
	{0x3404, 0x0f},
	{0x3405, 0xff},
	{0x3500, 0x00},
	{0x3501, 0x00},
	{0x3502, 0x80},
	{0x3503, 0x00},
	{0x3509, 0xc0},
	{0x3920, 0xff},
	{0x3921, 0x00},
	{0x3922, 0x00},
	{0x3923, 0x00},
	{0x3924, 0x00},
	{0x3925, 0x00},
	{0x3926, 0x00},
	{0x3927, 0x00},
	{0x3928, 0x06},
	{0x5000, 0x9f},
	{0x4000, 0xcf},
	{0x4001, 0x20},
	{0x4002, 0x00},
	{0x4003, 0x10},
	{0x4008, 0x00},
	{0x4009, 0x07},
	{0x400c, 0x00},
	{0x400d, 0x07},
	{0x4010, 0x41},
	{0x4011, 0x7f},
	{0x4e00, 0x01},
	{0x4e01, 0x01},
	{0x4e02, 0x00},
	{0x4e03, 0xff},
	{0x4e04, 0x00},
	{0x4e05, 0xff},
	{0x4e06, 0x00},
	{0x4e07, 0xff},
	{0x4e08, 0x01},
	{0x4e09, 0x0f},
	{0x4e0a, 0xff},
	{0x4e0b, 0xff},
	{0x4e0c, 0xff},
	{0x4e0d, 0xff},
	{0x4e0e, 0x00},
	{0x4e0f, 0x23},
	{0x4e10, 0x9a},
	{0x4e11, 0x02},
	{0x4e12, 0x01},
	{0x5e00, 0x00},
};
#endif

static const struct sensor_reg ov9281_raw8_reg_list[] = {
	{ 0x0103, 0x01 },
	{ 0x0302, 0x32 },
	{ 0x030d, 0x50 },
	{ 0x030e, 0x02 },
	{ 0x3001, 0x00 },
	{ 0x3001, 0x00 },
	{ 0x3004, 0x00 },
	{ 0x3005, 0x00 },
	{ 0x3006, 0x04 },
	{ 0x3011, 0x0a },
	{ 0x3013, 0x18 },
	{ 0x301c, 0xf0 },
	{ 0x3022, 0x01 },
	{ 0x3030, 0x10 },
	{ 0x3039, 0x32 },
	{ 0x303a, 0x00 },
	{ 0x3500, 0x01 },
	{ 0x3501, 0x2a },
	{ 0x3502, 0x90 },
	{ 0x3503, 0x08 },
	{ 0x3505, 0x8c },
	{ 0x3507, 0x03 },
	{ 0x3508, 0x00 },
	{ 0x3509, 0x10 },
	{ 0x3610, 0x80 },
	{ 0x3611, 0xa0 },
	{ 0x3620, 0x6e },
	{ 0x3632, 0x56 },
	{ 0x3633, 0x78 },
	{ 0x3662, 0x07 },
	{ 0x3666, 0x00 },
	{ 0x366f, 0x5a },
	{ 0x3680, 0x84 },
	{ 0x3712, 0x80 },
	{ 0x372d, 0x22 },
	{ 0x3731, 0x80 },
	{ 0x3732, 0x30 },
	{ 0x3778, 0x00 },
	{ 0x377d, 0x22 },
	{ 0x3788, 0x02 },
	{ 0x3789, 0xa4 },
	{ 0x378a, 0x00 },
	{ 0x378b, 0x4a },
	{ 0x3799, 0x20 },
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x00 },
	{ 0x3804, 0x05 },
	{ 0x3805, 0x0f },
	{ 0x3806, 0x03 },
	{ 0x3807, 0x2f },
	{ 0x3808, 0x05 },
	{ 0x3809, 0x00 },
	{ 0x380a, 0x03 },
	{ 0x380b, 0x20 },
	{ 0x380c, 0x02 },
	{ 0x380d, 0xd8 },
	{ 0x380e, 0x03 },
	{ 0x380f, 0x8e },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x08 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x08 },
	{ 0x3814, 0x11 },
	{ 0x3815, 0x11 },
	{ 0x3820, 0x40 },
	{ 0x3821, 0x00 },
	{ 0x382c, 0x05 },
	{ 0x382d, 0xb0 },
	{ 0x389d, 0x00 },
	{ 0x3881, 0x42 },
	{ 0x3882, 0x01 },
	{ 0x3883, 0x00 },
	{ 0x3885, 0x02 },
	{ 0x38a8, 0x02 },
	{ 0x38a9, 0x80 },
	{ 0x38b1, 0x00 },
	{ 0x38b3, 0x02 },
	{ 0x38c4, 0x00 },
	{ 0x38c5, 0xc0 },
	{ 0x38c6, 0x04 },
	{ 0x38c7, 0x80 },
	{ 0x3920, 0xff },
	{ 0x4003, 0x40 },
	{ 0x4008, 0x04 },
	{ 0x4009, 0x0b },
	{ 0x400c, 0x00 },
	{ 0x400d, 0x07 },
	{ 0x4010, 0x40 },
	{ 0x4043, 0x40 },
	{ 0x4307, 0x30 },
	{ 0x4317, 0x00 },
	{ 0x4501, 0x00 },
	{ 0x4507, 0x00 },
	{ 0x4509, 0x00 },
	{ 0x450a, 0x08 },
	{ 0x4601, 0x9f },
	{ 0x470f, 0x00 },
	{ 0x4f07, 0x00 },
	{ 0x4800, 0x00 },
	{ 0x5000, 0x9f },
	{ 0x5001, 0x00 },
	{ 0x5e00, 0x00 },
	{ 0x5d00, 0x07 },
	{ 0x5d01, 0x00 },
	{ 0x4f00, 0x04 },
	{ 0x4f10, 0x00 },
	{ 0x4f11, 0x98 },
	{ 0x4f12, 0x0f },
	{ 0x4f13, 0xc4 },
	/* { 0x0100, 0x01 }, */
};

/* the supported format of this sensor is MEDIA_BUS_FMT_YUYV8_2X8 */
static const struct v4l2_mbus_framefmt ov9281_formats[] = {
	{
#ifdef OV9281_RAW8
		.code = V4L2_MBUS_FMT_SGRBG8_1X8,
#else
		.code = V4L2_MBUS_FMT_SGRBG10_1X10,
#endif
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

struct ov9281_framesize {
	int	width;
	int	height;
	int	vts;
	int	framerate;
	const sensor_reg_t *regs; /* Regs to tweak */
};

static struct ov9281_framesize ov9281_framesizes[] = {
	/* 1600x1200 */
	{
		.width		= OV9281_WIDTH,
		.height		= OV9281_HEIGHT,
		.vts		= 0x02e4,
		.framerate	= 60,
		.regs		= ov9281_reg_list,
	},
};

static inline void sensor_msleep(unsigned int msecs)
{
	msleep(msecs);
}

static int reg_read(struct i2c_client *client, u16 reg, u8 *val)
{
	int ret;
	/* We have 16-bit i2c addresses - care for endianess */
	unsigned char data[2] = { reg >> 8, reg & 0xff };

	ret = i2c_master_send(client, data, 2);
	if (ret < 2) {
		PRT_INFO("%s: i2c read error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}

	ret = i2c_master_recv(client, val, 1);
	if (ret < 1) {
		PRT_INFO("%s: i2c read error, reg: %x\n",
				__func__, reg);
		return ret < 0 ? ret : -EIO;
	}
	return 0;
}

static int reg_write(struct i2c_client *client, u16 reg, u8 val)
{
	int ret;
	unsigned char data[3] = { reg >> 8, reg & 0xff, val };

	ret = i2c_master_send(client, data, 3);
	if (ret < 3) {
		PRT_INFO("%s: i2c write error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}

	return 0;
}

#if 0
static void ov9281_printk_reg_value(u16 reg_addr)
{
	u8 value;
	int ret = 0;

	ret = reg_read(ov9281_client, reg_addr, &value);
	PRT_INFO("Register 0x%08x value is 0x%08x\n", reg_addr, value);

	return;
}
#endif

void ov9281_mipi_transmitter_reg(void)
{
	PRT_INFO("\nOV9281 mipi transmitter reg\n");
	/* ov9281_printk_reg_value(0x4800); */

	return;
}

void ov9281_PLL_relevant_reg(void)
{
	PRT_INFO("\nOV9281 PLL relevant reg\n");
	/* ov9281_printk_reg_value(0x3034); */

	return;
}

static int ov9281_check_present(void)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = reg_read(ov9281_client, 0x300A, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = reg_read(ov9281_client, 0x300B, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x9281) {
		PRT_ERR("ID wrong! (0x%x)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int ov9281_cmos_init(void)
{
	const struct sensor_reg *reg_list = NULL;
	u32 reg_cnt = 0;
	u8 value = 0;
	int i;
	int ret = 0;
	PRT_INFO("\n");

	ret = ov9281_check_present();
	if (ret != 0)
		return ret;

#ifdef OV9281_RAW8
	reg_list = ov9281_raw8_reg_list;
	reg_cnt = ARRAY_SIZE(ov9281_raw8_reg_list);
#else
	reg_list = ov9281_reg_list;
	reg_cnt = ARRAY_SIZE(ov9281_reg_list);
#endif

	for (i = 0; i < reg_cnt; i++) {
		ret = reg_write(ov9281_client,
				reg_list[i].reg_add,
				reg_list[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}

		if (reg_list[i].reg_add == 0x0103)
			sensor_msleep(5);
	}

	PRT_INFO("write ov9281_reg_list finish.\n");

	for (i = 0; i < reg_cnt; i++) {
		ret = reg_read(ov9281_client,
				reg_list[i].reg_add,
				&value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}

		if (reg_list[i].reg_value != value) {
			PRT_ERR("value(0x%02x) != reg_value(0x%02x)\n",
				value, reg_list[i].reg_value);
		}
	}

	return 0;

}

static inline ov9281_config_t *to_ov9281(struct v4l2_subdev *sd)
{
	return container_of(sd, ov9281_config_t, sd);
}

static const struct v4l2_mbus_framefmt *find_sensor_format(
					struct v4l2_mbus_framefmt *mf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ov9281_formats); i++)
		if (mf->code == ov9281_formats[i].code)
			return &ov9281_formats[i];

	return &ov9281_formats[0];
}

static void ov9281_try_format(struct v4l2_mbus_framefmt *mf)
{
	const struct v4l2_mbus_framefmt *fmt;

	fmt = find_sensor_format(mf);
	PRT_INFO("fmt->code:0x%x\n", fmt->code);
	PRT_INFO("mf->code:0x%x\n", mf->code);
	mf->code = fmt->code;
	mf->field = V4L2_FIELD_NONE;

	mf->width = OV9281_WIDTH;
	mf->height = OV9281_HEIGHT;

#if 0
	v4l_bound_align_image(&mf->width,  OV9281_MIN_WIDTH,
		OV9281_MAX_WIDTH, 0,
		&mf->height,  OV9281_MIN_HEIGHT,
		OV9281_MAX_HEIGHT, 0, 0);
#endif
}

static int ov9281_set_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	ov9281_config_t *ov9281 = to_ov9281(sd);
	struct v4l2_mbus_framefmt *mf = NULL;

	PRT_DBG("\n");

	ov9281_try_format(&fmt->format);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
	}

	if (mf) {
		mutex_lock(&ov9281->lock);
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
			PRT_DBG("\n");
			*mf = fmt->format;
		}
		mutex_unlock(&ov9281->lock);
	}

	{
	struct viss_source_info *si = NULL;
	struct mcsi_if_info *mcsi_if = NULL;

	si = v4l2_get_subdev_hostdata(sd);
	if (si == NULL) {
		PRT_ERR("can't get sensor configuration data!\n");
		return -EINVAL;
	}
	mcsi_if = (struct mcsi_if_info *)si->if_info;

	mcsi_if->dlanes = 2;
#ifdef OV9281_RAW8
	mcsi_if->data_type = VISS_MCSI_RAW8;
	mcsi_if->mfreq = 800000000;
#else
	mcsi_if->data_type = VISS_MCSI_RAW10;
	mcsi_if->mfreq = 800000000;
#endif

	PRT_INFO("dlanes=%d,data_type=%d,mfreq=%d\n",
		mcsi_if->dlanes, mcsi_if->data_type, mcsi_if->mfreq);
	}

	return 0;
}

static int ov9281_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	ov9281_config_t *ov9281 = to_ov9281(sd);
	struct v4l2_mbus_framefmt *mf;

	PRT_DBG("\n");

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, 0);
		fmt->format = *mf;
		return 0;
	}

	/* V4L2_SUBDEV_FORMAT_ACTIVE */
	mutex_lock(&ov9281->lock);
	fmt->format = ov9281->format;
	mutex_unlock(&ov9281->lock);

	return 0;
}

/*
 * V4L2 subdev video and pad level operations
 */
static void ov9281_get_default_format(struct v4l2_mbus_framefmt *mf)
{
	mf->width = ov9281_framesizes[0].width;
	mf->height = ov9281_framesizes[0].height;
	mf->colorspace = ov9281_formats[0].colorspace;
	mf->code = ov9281_formats[0].code;
	mf->field = V4L2_FIELD_NONE;
}

/*
 * V4L2 subdev internal operations
 */

static int ov9281_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
#if 0
	struct v4l2_mbus_framefmt *format = v4l2_subdev_get_try_format(fh, 0);
	struct v4l2_rect *crop = v4l2_subdev_get_try_crop(fh, 0);
#endif

	return 0;
}

static int ov9281_registered(struct v4l2_subdev *sd)
{
	ov9281_config_t *ov9281 = to_ov9281(sd);
	struct i2c_client *client = ov9281->client;
	int ret = 0;

	mutex_lock(&ov9281->lock);

	if ((ov9281->reset_gpio != 0) && (ov9281->pwdn_gpio != 0)) {
		ret = devm_gpio_request(&client->dev, ov9281->reset_gpio,
					"reset_gpio");
		if (ret < 0) {
			PRT_ERR("reset_gpio %d request failed (reset_gpio)\n",
				ov9281->reset_gpio);
			ov9281->reset_gpio = 0;
			ov9281->pwdn_gpio = 0;
			goto unlock;
		}

		ret = devm_gpio_request(&client->dev, ov9281->pwdn_gpio,
					"pwdn_gpio");
		if (ret < 0) {
			PRT_ERR("pwdn_gpio %d request failed (pwdn_gpio)\n",
				ov9281->pwdn_gpio);
			devm_gpio_free(&client->dev, ov9281->reset_gpio);
			ov9281->reset_gpio = 0;
			ov9281->pwdn_gpio = 0;
			goto unlock;
		}
	}
#if 0
	mdelay(100);
#endif

	ov9281_reset_cycle(ov9281);
	mdelay(100);
	ret = ov9281_check_present();
	ov9281_pwdn(ov9281);
	if (ret != 0) {
		PRT_ERR("sensor ov9281 not present!\n");
		if ((ov9281->reset_gpio != 0) && (ov9281->pwdn_gpio != 0)) {
			devm_gpio_free(&client->dev, ov9281->pwdn_gpio);
			devm_gpio_free(&client->dev, ov9281->reset_gpio);
			ov9281->reset_gpio = 0;
			ov9281->pwdn_gpio = 0;
		}
	} else
		PRT_INFO("sensor ov9281 present\n");

unlock:
	mutex_unlock(&ov9281->lock);

	return ret;
}

static const struct v4l2_subdev_internal_ops ov9281_subdev_internal_ops = {
	.registered = ov9281_registered,
	.open = ov9281_open,
};

static void ov9281_reset_cycle(ov9281_config_t *ov9281)
{
	PRT_INFO("\n");

	if ((ov9281->reset_gpio == 0) || (ov9281->pwdn_gpio == 0))
		return;

#if 0
	/* reset down */
	gpio_direction_output(ov9281->reset_gpio, 0);
	mdelay(5);

	/* pwdn high */
	gpio_direction_output(ov9281->pwdn_gpio, 1);
	mdelay(5);

	/* pwdn down */
	gpio_direction_output(ov9281->pwdn_gpio, 0);
	mdelay(5);

	/* reset high */
	gpio_direction_output(ov9281->reset_gpio, 1);
	mdelay(5);
#else
	/* reset down */
	gpio_direction_output(ov9281->reset_gpio, 0);
	mdelay(50);

	/* reset high */
	gpio_direction_output(ov9281->reset_gpio, 1);
	mdelay(10);
#endif
}

static void ov9281_pwdn(ov9281_config_t *ov9281)
{
#if 1
	if ((ov9281->reset_gpio == 0) || (ov9281->pwdn_gpio == 0))
		return;

	/* reset low */
	gpio_direction_output(ov9281->reset_gpio, 0);
	mdelay(10);
	/* pwdn high */
	gpio_direction_output(ov9281->pwdn_gpio, 1);
#endif
}

static int ov9281_set_power(struct v4l2_subdev *sd, int on)
{
	ov9281_config_t *ov9281 = to_ov9281(sd);
	int ret = 0;

	if (on) {
#if 1
		ov9281_reset_cycle(ov9281);
#endif
		ret = ov9281_cmos_init();
	} else {
		ov9281_pwdn(ov9281);
	}

	PRT_INFO("on:%d\n", on);
	return ret;
}

static int ov9281_s_stream(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

#if 0
	ov9281_config_t *ov9281 = to_ov9281(sd);

	ov9281_reset_cycle(ov9281);
	ret = ov9281_cmos_init();
#endif

	if (on) {
		ret = reg_write(ov9281_client, 0x0100, 0x01);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	} else {
		ret = reg_write(ov9281_client, 0x0100, 0x00);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}

	PRT_INFO("*************************on:%d\n", on);
	return ret;
}

static const struct v4l2_subdev_pad_ops ov9281_pad_ops = {
#if 0
	.enum_mbus_code = ov9281_enum_mbus_code,
	.enum_frame_size = ov9281_enum_frame_sizes,
#endif
	.get_fmt = ov9281_get_fmt,
	.set_fmt = ov9281_set_fmt,
};

/*
 * V4L2 subdev internal operations
 */
static const struct v4l2_subdev_core_ops ov9281_subdev_core_ops = {
	.s_power = ov9281_set_power,
	.log_status = v4l2_ctrl_subdev_log_status,
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops ov9281_video_ops = {
	.s_stream = ov9281_s_stream,
};

#ifdef CONFIG_VIDEO_V4L2_SUBDEV_API

static const struct v4l2_subdev_ops ov9281_subdev_ops = {
	.core = &ov9281_subdev_core_ops,
	.pad = &ov9281_pad_ops,
	.video = &ov9281_video_ops,
};
#endif

/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_ops ov9281_ops;

#define DRIVER_NAME "ov9281_mipi"

static int ov9281_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int ret = 0;
	enum of_gpio_flags flags;
	struct i2c_adapter *adapter = client->adapter;
	struct device_node *np = client->dev.of_node;
	struct v4l2_subdev *sd;
	ov9281_config_t *ov9281;

	PRT_INFO("*********probe begin\n");

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	ov9281 = devm_kzalloc(&client->dev, sizeof(*ov9281), GFP_KERNEL);
	if (ov9281 == NULL)
		return -ENOMEM;

	ov9281->reset_gpio = of_get_named_gpio_flags(np, "reset_gpio",
							0, &flags);

	ov9281->pwdn_gpio = of_get_named_gpio_flags(np, "pwdn_gpio",
						0, &flags);

	if (!gpio_is_valid(ov9281->reset_gpio) ||
		!gpio_is_valid(ov9281->pwdn_gpio)) {
		PRT_ERR("get gpio %d(reset) and gpio %d(pwdn) failed\n",
				ov9281->reset_gpio, ov9281->pwdn_gpio);
		ov9281->reset_gpio = 0;
		ov9281->pwdn_gpio = 0;
	}

	mutex_init(&ov9281->lock);

	ov9281->client = client;
	sd = &ov9281->sd;

	v4l2_i2c_subdev_init(sd, client, &ov9281_subdev_ops);
	strlcpy(sd->name, DRIVER_NAME, sizeof(sd->name));

	sd->internal_ops = &ov9281_subdev_internal_ops;

	client->flags = I2C_CLIENT_SCCB;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

#if defined(CONFIG_MEDIA_CONTROLLER)
	ov9281->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &ov9281->pad, 0);
	if (ret < 0)
		return ret;
#endif
	ov9281_get_default_format(&ov9281->format);

	ov9281_client = client;

	PRT_INFO("probe end\n");
	return 0;
}

static int ov9281_remove(struct i2c_client *client)
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

/* sensor devices id table */
static const struct i2c_device_id ov9281_mipi_id[] = {
	{ DRIVER_NAME, 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, ov9281_mipi_id);

static const struct of_device_id ov9281_mipi_of_match[] = {
	{
		.compatible = "ov9281_mipi",
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ov9281_mipi_of_match);

static struct i2c_driver ov9281_mipi_driver = {
	.driver = {
		.of_match_table = ov9281_mipi_of_match,
		.name = DRIVER_NAME,
	},
	.probe = ov9281_probe,
	.remove = ov9281_remove,
	.id_table = ov9281_mipi_id,
};

module_i2c_driver(ov9281_mipi_driver);


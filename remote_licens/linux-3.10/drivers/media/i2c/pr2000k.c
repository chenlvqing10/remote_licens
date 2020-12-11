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

MODULE_DESCRIPTION("PixelPlus pr2000k driver");
MODULE_LICENSE("GPL v2");

#include <mach/csp.h>
#include <mach/debug.h>

#define PR2000K_1080P_WIDTH		1920
#define PR2000K_1080P_HEIGHT		1080

enum page_id {
	PAGE0_ID,
	PAGE1_ID,
	PAGE2_ID,
};

static struct i2c_client *pr2000k_client;

typedef struct pr2000k_config {
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

#if 0
	int streaming;
	int power;
#endif
} pr2000k_config_t;

typedef struct tag_ahd_reg {
	u8 page_id;
	u8 reg_add;
	u8 reg_value;
	u8 rsvd;
} ahd_reg_t;

static void pr2000k_reset_cycle(pr2000k_config_t *pr2000k);

static const ahd_reg_t pr2000k_dvp_ahd_1080p[] = {
	/* Page0 sys */
	{ PAGE0_ID, 0xff, 0x00 },

	{ PAGE0_ID, 0x10, 0x83 },

	{ PAGE0_ID, 0x11, 0x07 },
	{ PAGE0_ID, 0x12, 0x00 },
	{ PAGE0_ID, 0x13, 0x00 },
	/* b[1:0] => Select C amera Input. VinP(1), VinN(3), Differ(0) */
	{ PAGE0_ID, 0x14, 0x21 },
	{ PAGE0_ID, 0x15, 0x44 },
	{ PAGE0_ID, 0x16, 0x0d },
	{ PAGE0_ID, 0x40, 0x00 },
	{ PAGE0_ID, 0x47, 0x3a },
	{ PAGE0_ID, 0x4e, 0x3f },
	{ PAGE0_ID, 0x80, 0x56 },
	{ PAGE0_ID, 0x81, 0x0e },
	{ PAGE0_ID, 0x82, 0x0d },
	{ PAGE0_ID, 0x84, 0x30 },
	{ PAGE0_ID, 0x86, 0x20 },
	{ PAGE0_ID, 0x87, 0x00 },
	{ PAGE0_ID, 0x8a, 0x00 },
	{ PAGE0_ID, 0x90, 0x00 },
	{ PAGE0_ID, 0x91, 0x00 },
	{ PAGE0_ID, 0x92, 0x00 },
	{ PAGE0_ID, 0x94, 0xff },
	{ PAGE0_ID, 0x95, 0xff },
	{ PAGE0_ID, 0x96, 0xff },
	{ PAGE0_ID, 0xa0, 0x00 },
	{ PAGE0_ID, 0xa1, 0x20 },
	{ PAGE0_ID, 0xa4, 0x01 },
	{ PAGE0_ID, 0xa5, 0xe3 },
	{ PAGE0_ID, 0xa6, 0x00 },
	{ PAGE0_ID, 0xa7, 0x12 },
	{ PAGE0_ID, 0xa8, 0x00 },
	{ PAGE0_ID, 0xd0, 0x30 },
	{ PAGE0_ID, 0xd1, 0x08 },
	{ PAGE0_ID, 0xd2, 0x21 },
	{ PAGE0_ID, 0xd3, 0x00 },
	{ PAGE0_ID, 0xd8, 0x30 },
	{ PAGE0_ID, 0xd9, 0x08 },
	{ PAGE0_ID, 0xda, 0x21 },
	{ PAGE0_ID, 0xe0, 0x35 },
	{ PAGE0_ID, 0xe1, 0x80 },
	{ PAGE0_ID, 0xe2, 0x18 },
	{ PAGE0_ID, 0xe3, 0x00 },
	{ PAGE0_ID, 0xe4, 0x00 },
	{ PAGE0_ID, 0xea, 0x01 },
	{ PAGE0_ID, 0xeb, 0xff },
	{ PAGE0_ID, 0xf1, 0x44 },
	{ PAGE0_ID, 0xf2, 0x01 },

	/* Page1 vdec */
	{ PAGE1_ID, 0xff, 0x01 },
	{ PAGE1_ID, 0x00, 0xe4 },
	{ PAGE1_ID, 0x01, 0x61 },
	{ PAGE1_ID, 0x02, 0x00 },
	{ PAGE1_ID, 0x03, 0x57 },
	{ PAGE1_ID, 0x04, 0x0c },
	{ PAGE1_ID, 0x05, 0x88 },
	{ PAGE1_ID, 0x06, 0x04 },
	{ PAGE1_ID, 0x07, 0xb2 },
	{ PAGE1_ID, 0x08, 0x44 },
	{ PAGE1_ID, 0x09, 0x34 },
	{ PAGE1_ID, 0x0a, 0x02 },
	{ PAGE1_ID, 0x0b, 0x14 },
	{ PAGE1_ID, 0x0c, 0x04 },
	{ PAGE1_ID, 0x0d, 0x08 },
	{ PAGE1_ID, 0x0e, 0x5e },
	{ PAGE1_ID, 0x0f, 0x5e },
	{ PAGE1_ID, 0x10, 0x26 },
	{ PAGE1_ID, 0x11, 0x00 },
	{ PAGE1_ID, 0x12, 0x87 },

	{ PAGE1_ID, 0x13, 0x24 },
	{ PAGE1_ID, 0x14, 0x80 },
	{ PAGE1_ID, 0x15, 0x2a },

	{ PAGE1_ID, 0x16, 0x38 },
	{ PAGE1_ID, 0x17, 0x00 },
	{ PAGE1_ID, 0x18, 0x80 },
	{ PAGE1_ID, 0x19, 0x48 },
	{ PAGE1_ID, 0x1a, 0x6c },
	{ PAGE1_ID, 0x1b, 0x05 },
	{ PAGE1_ID, 0x1c, 0x61 },
	{ PAGE1_ID, 0x1d, 0x07 },
	{ PAGE1_ID, 0x1e, 0x7e },
	{ PAGE1_ID, 0x1f, 0x80 },
	{ PAGE1_ID, 0x20, 0x80 },
	{ PAGE1_ID, 0x21, 0x80 },
	{ PAGE1_ID, 0x22, 0x90 },
	{ PAGE1_ID, 0x23, 0x80 },
	{ PAGE1_ID, 0x24, 0x80 },
	{ PAGE1_ID, 0x25, 0x80 },
	{ PAGE1_ID, 0x26, 0x84 },
	{ PAGE1_ID, 0x27, 0x82 },
	{ PAGE1_ID, 0x28, 0x00 },
	{ PAGE1_ID, 0x29, 0xff },
	{ PAGE1_ID, 0x2a, 0xff },
	{ PAGE1_ID, 0x2b, 0x00 },
	{ PAGE1_ID, 0x2c, 0x00 },
	{ PAGE1_ID, 0x2d, 0x00 },
	{ PAGE1_ID, 0x2e, 0x00 },
	{ PAGE1_ID, 0x2f, 0x00 },
	{ PAGE1_ID, 0x30, 0x00 },
	{ PAGE1_ID, 0x31, 0x00 },
	{ PAGE1_ID, 0x32, 0xc0 },
	{ PAGE1_ID, 0x33, 0x14 },
	{ PAGE1_ID, 0x34, 0x14 },
	{ PAGE1_ID, 0x35, 0x80 },
	{ PAGE1_ID, 0x36, 0x80 },
	{ PAGE1_ID, 0x37, 0xad },
	{ PAGE1_ID, 0x38, 0x4b },
	{ PAGE1_ID, 0x39, 0x08 },
	{ PAGE1_ID, 0x3a, 0x21 },
	{ PAGE1_ID, 0x3b, 0x02 },
	{ PAGE1_ID, 0x3c, 0x01 },
	{ PAGE1_ID, 0x3d, 0x23 },
	{ PAGE1_ID, 0x3e, 0x05 },
	{ PAGE1_ID, 0x3f, 0xc8 },
	{ PAGE1_ID, 0x40, 0x05 },
	{ PAGE1_ID, 0x41, 0x55 },
	{ PAGE1_ID, 0x42, 0x01 },
	{ PAGE1_ID, 0x43, 0x38 },
	{ PAGE1_ID, 0x44, 0x6a },
	{ PAGE1_ID, 0x45, 0x00 },
	{ PAGE1_ID, 0x46, 0x14 },

	{ PAGE1_ID, 0x47, 0xb0 },
	{ PAGE1_ID, 0x48, 0xdf },

	{ PAGE1_ID, 0x49, 0x00 },
	{ PAGE1_ID, 0x4a, 0x7b },
	{ PAGE1_ID, 0x4b, 0x60 },
	{ PAGE1_ID, 0x4c, 0x00 },
	{ PAGE1_ID, 0x4d, 0x26 },
	{ PAGE1_ID, 0x4e, 0x00 },
	{ PAGE1_ID, 0x4f, 0x24 },
	{ PAGE1_ID, 0x50, 0x01 },
	{ PAGE1_ID, 0x51, 0x28 },
	{ PAGE1_ID, 0x52, 0x40 },
	{ PAGE1_ID, 0x53, 0x0c },
	{ PAGE1_ID, 0x54, 0x0f },
	{ PAGE1_ID, 0x55, 0x8d },
	{ PAGE1_ID, 0x70, 0x06 },
	{ PAGE1_ID, 0x71, 0x08 },
	{ PAGE1_ID, 0x72, 0x0a },
	{ PAGE1_ID, 0x73, 0x0c },
	{ PAGE1_ID, 0x74, 0x0e },
	{ PAGE1_ID, 0x75, 0x10 },
	{ PAGE1_ID, 0x76, 0x12 },
	{ PAGE1_ID, 0x77, 0x14 },
	{ PAGE1_ID, 0x78, 0x06 },
	{ PAGE1_ID, 0x79, 0x08 },
	{ PAGE1_ID, 0x7a, 0x0a },
	{ PAGE1_ID, 0x7b, 0x0c },
	{ PAGE1_ID, 0x7c, 0x0e },
	{ PAGE1_ID, 0x7d, 0x10 },
	{ PAGE1_ID, 0x7e, 0x12 },
	{ PAGE1_ID, 0x7f, 0x14 },
	{ PAGE1_ID, 0x80, 0x00 },
	{ PAGE1_ID, 0x81, 0x09 },
	{ PAGE1_ID, 0x82, 0x00 },
	{ PAGE1_ID, 0x83, 0x07 },
	{ PAGE1_ID, 0x84, 0x00 },
	{ PAGE1_ID, 0x85, 0x17 },
	{ PAGE1_ID, 0x86, 0x03 },
	{ PAGE1_ID, 0x87, 0xe5 },

	{ PAGE1_ID, 0x88, 0x05 },
	{ PAGE1_ID, 0x89, 0x24 },
	{ PAGE1_ID, 0x8a, 0x05 },
	{ PAGE1_ID, 0x8b, 0x24 },
	{ PAGE1_ID, 0x8c, 0x08 },
	{ PAGE1_ID, 0x8d, 0xe8 },
	{ PAGE1_ID, 0x8e, 0x05 },
	{ PAGE1_ID, 0x8f, 0x47 },
	{ PAGE1_ID, 0x90, 0x02 },
	{ PAGE1_ID, 0x91, 0xb4 },

	{ PAGE1_ID, 0x92, 0x73 },
	{ PAGE1_ID, 0x93, 0xe8 },
	{ PAGE1_ID, 0x94, 0x0f },
	{ PAGE1_ID, 0x95, 0x5e },
	{ PAGE1_ID, 0x96, 0x03 },
	{ PAGE1_ID, 0x97, 0xd0 },
	{ PAGE1_ID, 0x98, 0x17 },
	{ PAGE1_ID, 0x99, 0x34 },
	{ PAGE1_ID, 0x9a, 0x13 },
	{ PAGE1_ID, 0x9b, 0x56 },
	{ PAGE1_ID, 0x9c, 0x0b },
	{ PAGE1_ID, 0x9d, 0x9a },
	{ PAGE1_ID, 0x9e, 0x09 },
	{ PAGE1_ID, 0x9f, 0xab },
	{ PAGE1_ID, 0xa0, 0x01 },
	{ PAGE1_ID, 0xa1, 0x74 },
	{ PAGE1_ID, 0xa2, 0x01 },
	{ PAGE1_ID, 0xa3, 0x6b },
	{ PAGE1_ID, 0xa4, 0x00 },
	{ PAGE1_ID, 0xa5, 0xba },
	{ PAGE1_ID, 0xa6, 0x00 },
	{ PAGE1_ID, 0xa7, 0xa3 },
	{ PAGE1_ID, 0xa8, 0x01 },
	{ PAGE1_ID, 0xa9, 0x39 },
	{ PAGE1_ID, 0xaa, 0x01 },
	{ PAGE1_ID, 0xab, 0x39 },
	{ PAGE1_ID, 0xac, 0x00 },
	{ PAGE1_ID, 0xad, 0xc1 },
	{ PAGE1_ID, 0xae, 0x00 },
	{ PAGE1_ID, 0xaf, 0xc1 },

	{ PAGE1_ID, 0xb0, 0x05 },
	{ PAGE1_ID, 0xb1, 0xcc },
	{ PAGE1_ID, 0xb2, 0x09 },
	{ PAGE1_ID, 0xb3, 0x6d },

	{ PAGE1_ID, 0xb4, 0x00 },
	{ PAGE1_ID, 0xb5, 0x17 },
	{ PAGE1_ID, 0xb6, 0x08 },
	{ PAGE1_ID, 0xb7, 0xe8 },
	{ PAGE1_ID, 0xb8, 0xb0 },
	{ PAGE1_ID, 0xb9, 0xce },
	{ PAGE1_ID, 0xba, 0x90 },
	{ PAGE1_ID, 0xbb, 0x00 },
	{ PAGE1_ID, 0xbc, 0x00 },
	{ PAGE1_ID, 0xbd, 0x04 },
	{ PAGE1_ID, 0xbe, 0x07 },
	{ PAGE1_ID, 0xbf, 0x80 },
	{ PAGE1_ID, 0xc0, 0x00 },
	{ PAGE1_ID, 0xc1, 0x20 },
	{ PAGE1_ID, 0xc2, 0x04 },
	{ PAGE1_ID, 0xc3, 0x38 },
	{ PAGE1_ID, 0xff, 0x01 },
	{ PAGE1_ID, 0x54, 0x0e },
	{ PAGE1_ID, 0xff, 0x01 },
	{ PAGE1_ID, 0x54, 0x0f },
};

/* the supported format of this sensor is V4L2_MBUS_FMT_UYVY8_2X8 */
static const struct v4l2_mbus_framefmt pr2000k_formats[] = {
	{
		.code = V4L2_MBUS_FMT_UYVY8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};


struct pr2000k_framesize {
	int	width;
	int	height;
	int	framerate;
};

static struct pr2000k_framesize pr2000k_framesizes[] = {
	/* 1080P */
	{
		.width		= PR2000K_1080P_WIDTH,
		.height		= PR2000K_1080P_HEIGHT,
		.framerate	= 25,
	},
};

static int __reg_read(struct i2c_client *client, u8 reg, u8 *val)
{
	int ret;

	ret = i2c_master_send(client, &reg, 1);
	if (ret < 1) {
		PRT_ERR("%s: i2c read error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}

	ret = i2c_master_recv(client, val, 1);
	if (ret < 1) {
		PRT_ERR("%s: i2c read error, reg: %x\n",
			__func__, reg);
		return ret < 0 ? ret : -EIO;
	}
	return 0;
}

static int __reg_write(struct i2c_client *client, u8 reg, u8 val)
{
	int ret;
	unsigned char data[2] = { reg, val };

	ret = i2c_master_send(client, data, 2);
	if (ret < 2) {
		PRT_ERR("i2c write error, reg: %x, val: %x\n", reg, val);
		return ret < 0 ? ret : -EIO;
	}

	return 0;
}

static int pr2000k_reg_read(struct i2c_client *client, u8 page_id, u8 reg,
				u8 *val)
{
	int ret;

	ret = __reg_write(client, 0xFF, page_id);
	if (ret != 0) {
		PRT_ERR("page_id=%d\n", page_id);
		goto out;
	}

	ret = __reg_read(client, reg, val);
	if (ret != 0)
		PRT_ERR("page_id=%d,reg=0x%02x\n", page_id, reg);

out:
	return ret;
}

static int pr2000k_reg_write(struct i2c_client *client, u8 page_id, u8 reg,
				u8 val)
{
	int ret;

	ret = __reg_write(client, 0xFF, page_id);
	if (ret != 0) {
		PRT_ERR("page_id=%d\n", page_id);
		goto out;
	}

	ret = __reg_write(client, reg, val);
	if (ret != 0)
		PRT_ERR("page_id=%d,reg=0x%02x\n", page_id, reg);

out:
	return ret;
}

static int pr2000k_check_present(void)
{
	u8 tmp[2];
	int ret = 0;

	/* read deviceID Low byte,should be 0x00 */
	ret = pr2000k_reg_read(pr2000k_client, PAGE0_ID, 0xFD, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}

	/* read deviceID High byte,should be 0x20 */
	ret = pr2000k_reg_read(pr2000k_client, PAGE0_ID, 0xFC, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}

	if ((tmp[0] != 0x00) && (tmp[1] != 0x20)) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}

	return 0;
}

static int pr2000k_init(void)
{
	u8 value = 0;
	int i;
	int ret = 0;

	ret = pr2000k_check_present();
	if (ret != 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(pr2000k_dvp_ahd_1080p); i++) {
		ret = pr2000k_reg_write(pr2000k_client,
				pr2000k_dvp_ahd_1080p[i].page_id,
				pr2000k_dvp_ahd_1080p[i].reg_add,
				pr2000k_dvp_ahd_1080p[i].reg_value);
		if (ret) {
			PRT_ERR("i=%d,page_id=%d,reg_add=%x,reg_value=%x\n",
				i, pr2000k_dvp_ahd_1080p[i].page_id,
				pr2000k_dvp_ahd_1080p[i].reg_add,
				pr2000k_dvp_ahd_1080p[i].reg_value);
			return ret;
		}
	}

	PRT_INFO("write reg list finish.\n");

#if 1
	for (i = 0; i < ARRAY_SIZE(pr2000k_dvp_ahd_1080p); i++) {
		ret = pr2000k_reg_read(pr2000k_client,
			pr2000k_dvp_ahd_1080p[i].page_id,
			pr2000k_dvp_ahd_1080p[i].reg_add, &value);
		if (pr2000k_dvp_ahd_1080p[i].reg_value != value) {
			PRT_ERR("%d %d 0x%x: 0x%x != 0x%x\n",
				i, pr2000k_dvp_ahd_1080p[i].page_id,
				pr2000k_dvp_ahd_1080p[i].reg_add, value,
				pr2000k_dvp_ahd_1080p[i].reg_value);
		}
	}
#endif

	return 0;
}

static inline pr2000k_config_t *to_pr2000k(struct v4l2_subdev *sd)
{
	return container_of(sd, pr2000k_config_t, sd);
}

static const struct v4l2_mbus_framefmt *find_sensor_format(
					struct v4l2_mbus_framefmt *mf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pr2000k_formats); i++)
		if (mf->code == pr2000k_formats[i].code)
			return &pr2000k_formats[i];

	return &pr2000k_formats[0];
}

static void pr2000k_try_format(struct v4l2_mbus_framefmt *mf)
{
	const struct v4l2_mbus_framefmt *fmt;

	fmt = find_sensor_format(mf);
	PRT_INFO("fmt->code:0x%x\n", fmt->code);
	PRT_INFO("mf->code:0x%x\n", mf->code);
	mf->code = fmt->code;
	mf->field = V4L2_FIELD_NONE;

	mf->width = PR2000K_1080P_WIDTH;
	mf->height = PR2000K_1080P_HEIGHT;
}

static int pr2000k_set_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	pr2000k_config_t *pr2000k = to_pr2000k(sd);
	struct v4l2_mbus_framefmt *mf = NULL;

	PRT_DBG("\n");

	pr2000k_try_format(&fmt->format);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
	}

	if (mf) {
		mutex_lock(&pr2000k->lock);
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
			PRT_DBG("\n");
			*mf = fmt->format;
		}
		mutex_unlock(&pr2000k->lock);
	}

	return 0;
}

static int pr2000k_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	pr2000k_config_t *pr2000k = to_pr2000k(sd);
	struct v4l2_mbus_framefmt *mf;

	PRT_DBG("\n");

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, 0);
		fmt->format = *mf;
		return 0;
	}

	/* V4L2_SUBDEV_FORMAT_ACTIVE */
	mutex_lock(&pr2000k->lock);
	fmt->format = pr2000k->format;
	mutex_unlock(&pr2000k->lock);

	return 0;
}

/*
 * V4L2 subdev video and pad level operations
 */
static void pr2000k_get_default_format(struct v4l2_mbus_framefmt *mf)
{
	mf->width = pr2000k_framesizes[0].width;
	mf->height = pr2000k_framesizes[0].height;
	mf->colorspace = pr2000k_formats[0].colorspace;
	mf->code = pr2000k_formats[0].code;
	mf->field = V4L2_FIELD_NONE;
}

static int pr2000k_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
#if 0
	struct v4l2_mbus_framefmt *format = v4l2_subdev_get_try_format(fh, 0);
	struct v4l2_rect *crop = v4l2_subdev_get_try_crop(fh, 0);
#endif

	return 0;
}

static int pr2000k_registered(struct v4l2_subdev *sd)
{
	pr2000k_config_t *pr2000k = to_pr2000k(sd);
	struct i2c_client *client = pr2000k->client;
	int ret = 0;

	PRT_INFO("\n");

	mutex_lock(&pr2000k->lock);

	if (pr2000k->reset_gpio != 0) {
		ret = devm_gpio_request(&client->dev, pr2000k->reset_gpio,
					"reset_gpio");
		if (ret < 0) {
			PRT_ERR("reset_gpio %d request failed (reset_gpio)\n",
				pr2000k->reset_gpio);
			pr2000k->reset_gpio = 0;
			return -ENODEV;
		}
	}
	pr2000k_reset_cycle(pr2000k);
	ret = pr2000k_check_present();
	if (ret != 0) {
		PRT_ERR("ahd chip pr2000k not present!\n");
		if (pr2000k->reset_gpio != 0)
			devm_gpio_free(&client->dev, pr2000k->reset_gpio);
	} else
		PRT_INFO("ahd chip pr2000k present\n");
	mutex_unlock(&pr2000k->lock);

	return ret;
}

static const struct v4l2_subdev_internal_ops pr2000k_subdev_internal_ops = {
	.registered = pr2000k_registered,
	.open = pr2000k_open,
};

static void pr2000k_reset_cycle(pr2000k_config_t *pr2000k)
{
	PRT_INFO("\n");

	if (pr2000k->reset_gpio == 0)
		return;

	/* reset high */
	gpio_direction_output(pr2000k->reset_gpio, 1);
	msleep(100);
	/* reset low */
	gpio_direction_output(pr2000k->reset_gpio, 0);
	msleep(100);
	/* reset high */
	gpio_direction_output(pr2000k->reset_gpio, 1);
	msleep(100);
}

static int pr2000k_set_power(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

	if (on) {
		pr2000k_config_t *pr2000k = to_pr2000k(sd);
		pr2000k_reset_cycle(pr2000k);
		ret = pr2000k_init();
	}

	PRT_INFO("on:%d\n", on);
	return ret;
}

static int pr2000k_s_stream(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

#if 0
	pr2000k_config_t *pr2000k = to_pr2000k(sd);

	pr2000k_reset_cycle(pr2000k);

	ret = pr2000k_init();
#endif

	PRT_INFO("*************************on:%d\n", on);
	return ret;
}

static const struct v4l2_subdev_pad_ops pr2000k_pad_ops = {
#if 0
	.enum_mbus_code = pr2000k_enum_mbus_code,
	.enum_frame_size = pr2000k_enum_frame_sizes,
#endif
	.get_fmt = pr2000k_get_fmt,
	.set_fmt = pr2000k_set_fmt,
};

/*
 * V4L2 subdev internal operations
 */
static const struct v4l2_subdev_core_ops pr2000k_subdev_core_ops = {
	.s_power = pr2000k_set_power,
	.log_status = v4l2_ctrl_subdev_log_status,
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops pr2000k_video_ops = {
	.s_stream = pr2000k_s_stream,
};

#ifdef CONFIG_VIDEO_V4L2_SUBDEV_API

static const struct v4l2_subdev_ops pr2000k_subdev_ops = {
	.core = &pr2000k_subdev_core_ops,
	.pad = &pr2000k_pad_ops,
	.video = &pr2000k_video_ops,
};
#endif

/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_ops pr2000k_ops;

#define DRIVER_NAME "pr2000k"

static int pr2000k_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int ret = 0;
	enum of_gpio_flags flags;
	struct i2c_adapter *adapter = client->adapter;
	struct device_node *np = client->dev.of_node;
	struct v4l2_subdev *sd;
	pr2000k_config_t *pr2000k;

	PRT_INFO("*********8probe begin\n");

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	pr2000k = devm_kzalloc(&client->dev, sizeof(*pr2000k), GFP_KERNEL);
	if (pr2000k == NULL)
		return -ENOMEM;

	pr2000k->reset_gpio = of_get_named_gpio_flags(np, "reset_gpio",
							0, &flags);

	if (!gpio_is_valid(pr2000k->reset_gpio)) {
		PRT_ERR("get gpio %d failed(reset_gpio)\n",
				pr2000k->reset_gpio);
		pr2000k->reset_gpio = 0;
	}

	mutex_init(&pr2000k->lock);

	pr2000k->client = client;
	sd = &pr2000k->sd;

	v4l2_i2c_subdev_init(sd, client, &pr2000k_subdev_ops);
	strlcpy(sd->name, DRIVER_NAME, sizeof(sd->name));

	sd->internal_ops = &pr2000k_subdev_internal_ops;

	client->flags = I2C_CLIENT_SCCB;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

#if defined(CONFIG_MEDIA_CONTROLLER)
	pr2000k->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &pr2000k->pad, 0);
	if (ret < 0)
		return ret;
#endif
	pr2000k_get_default_format(&pr2000k->format);

	pr2000k_client = client;

	PRT_INFO("probe end\n");
	return 0;
}

static int pr2000k_remove(struct i2c_client *client)
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
static const struct i2c_device_id pr2000k_id[] = {
	{ "pr2000k", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, pr2000k_id);

static struct i2c_driver pr2000k_driver = {
	.driver = {
		.name = "pr2000k",
	},
	.probe = pr2000k_probe,
	.remove = pr2000k_remove,
	.id_table = pr2000k_id,
};

module_i2c_driver(pr2000k_driver);


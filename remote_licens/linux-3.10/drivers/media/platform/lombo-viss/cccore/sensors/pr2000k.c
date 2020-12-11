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

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/viss_isp.h>

#include <media/lombo_viss.h>

#include <mach/debug.h>
#include "../ccc.h"

enum page_id {
	PAGE0_ID,
	PAGE1_ID,
	PAGE2_ID,
};

typedef struct tag_ahd_reg {
	u8 page_id;
	u8 reg_add;
	u8 reg_value;
	u8 rsvd;
} ahd_reg_t;

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

static struct cc_power_act power_up_act[] = {
	{CC_RESET,  CC_UP},
	{CC_DELAY, 100},
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 100},
	{CC_RESET,  CC_UP},
	{CC_DELAY, 100},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 100},
};

static const struct cc_frame_format frame_formats[] = {
	{
		.mbus_code = V4L2_MBUS_FMT_FIXED,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

#define PR2000K_1080P_WIDTH		1920
#define PR2000K_1080P_HEIGHT		1080

static const struct cc_win_size win_sizes[] = {
	{
		.width = PR2000K_1080P_WIDTH,
		.height = PR2000K_1080P_HEIGHT,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = PR2000K_1080P_WIDTH,
		.height = PR2000K_1080P_HEIGHT,

		.framerate = { 1, 25 },	/* 25 fps */
	},
};

static int pr2000k_reg_read(struct cc_sensor_info *cc_si, u8 page_id,
				u8 reg, u8 *val)
{
	int ret;

	ret = cc_si->write_reg(cc_si, 0xFF, page_id);
	if (ret != 0) {
		PRT_ERR("page_id=%d\n", page_id);
		goto out;
	}

	ret = cc_si->read_reg(cc_si, reg, val);
	if (ret != 0)
		PRT_ERR("page_id=%d,reg=0x%02x\n", page_id, reg);

out:
	return ret;
}

static int pr2000k_reg_write(struct cc_sensor_info *cc_si, u8 page_id,
				u8 reg, u8 val)
{
	int ret;

	ret = cc_si->write_reg(cc_si, 0xFF, page_id);
	if (ret != 0) {
		PRT_ERR("page_id=%d\n", page_id);
		goto out;
	}

	ret = cc_si->write_reg(cc_si, reg, val);
	if (ret != 0)
		PRT_ERR("page_id=%d,reg=0x%02x\n", page_id, reg);

out:
	return ret;
}

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	/* read deviceID Low byte,should be 0x00 */
	ret = pr2000k_reg_read(cc_si, PAGE0_ID, 0xFD, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}

	/* read deviceID High byte,should be 0x20 */
	ret = pr2000k_reg_read(cc_si, PAGE0_ID, 0xFC, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}

	id = (tmp[1] << 8) | tmp[0];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x2000) {
		PRT_ERR("ID wrong! (0x%04x != 0x2000)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int sensor_init(struct cc_sensor_info *cc_si)
{
	struct timespec ts_s;
	struct timespec ts_f;
	int i = 0;
	int ret = 0;

	PRT_INFO("\n");

	ret = sensor_detect(cc_si);
	if (ret != 0)
		return ret;

	ktime_get_ts(&ts_s);
	for (i = 0; i < ARRAY_SIZE(pr2000k_dvp_ahd_1080p); i++) {
		ret = pr2000k_reg_write(cc_si,
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
	ktime_get_ts(&ts_f);

	PRT_INFO("write %s pr2000k_dvp_ahd_1080p list finish.\n", cc_si->name);
	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));


#if 0
	for (i = 0; i < ARRAY_SIZE(pr2000k_dvp_ahd_1080p); i++) {
		ret = pr2000k_reg_read(cc_si,
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

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;

	return ret;
}

/*
 * ctrl for raw sensor finish
 */


#define DRIVER_NAME "pr2000k_ahd"

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
	cc_si->sensor_ioctl = NULL;

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

static const struct of_device_id pr2000k_ahd_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, pr2000k_ahd_of_match);

static struct platform_driver pr2000k_ahd_driver = {
	.driver = {
		.of_match_table	= pr2000k_ahd_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(pr2000k_ahd_driver);

MODULE_DESCRIPTION("PR200K AHD chip driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);

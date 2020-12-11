/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * pr2000k ahd chip driver code for LomboTech
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

#define VMAX (0x465)

/*
 * RAW12 4lane init
 */
static const cc_reg_t init_reg_list[] = {
	{ 0x3000, 0x01 },
	{ 0x3002, 0x00 },
	{ 0x3005, 0x01 },
	{ 0x3007, 0x00 },
	{ 0x3009, 0x02 },
	{ 0x300A, 0xF0 },
	{ 0x3011, 0x02 },
	{ 0x3018, 0x65 },
	{ 0x3019, 0x04 },
	{ 0x301C, 0x30 },
	{ 0x301D, 0x11 },
	{ 0x3046, 0x01 },
	{ 0x304B, 0x0A },
	{ 0x305C, 0x18 },
	{ 0x305D, 0x03 },
	{ 0x305E, 0x20 },
	{ 0x305F, 0x01 },
	{ 0x309E, 0x4A },
	{ 0x309F, 0x4A },
	{ 0x30D2, 0x19 },
	{ 0x30D7, 0x03 },

	{ 0x3129, 0x00 },
	{ 0x313B, 0x61 },
	{ 0x315E, 0x1A },
	{ 0x3164, 0x1A },
	{ 0x317C, 0x00 },
	{ 0x31EC, 0x0E },

	{ 0x3405, 0x20 },
	{ 0x3407, 0x03 },
	{ 0x3414, 0x0A },
	{ 0x3418, 0x49 },
	{ 0x3419, 0x04 },
	{ 0x3441, 0x0C },
	{ 0x3442, 0x0C },
	{ 0x3443, 0x03 },
	{ 0x3444, 0x20 },
	{ 0x3445, 0x25 },
	{ 0x3446, 0x47 },
	{ 0x3447, 0x00 },
	{ 0x3448, 0x1F },
	{ 0x3449, 0x00 },
	{ 0x344A, 0x17 },
	{ 0x344B, 0x00 },
	{ 0x344C, 0x0F },
	{ 0x344D, 0x00 },
	{ 0x344E, 0x17 },
	{ 0x344F, 0x00 },
	{ 0x3450, 0x47 },
	{ 0x3451, 0x00 },
	{ 0x3452, 0x0F },
	{ 0x3453, 0x00 },
	{ 0x3454, 0x0F },
	{ 0x3455, 0x00 },
	{ 0x3472, 0x9C },
	{ 0x3473, 0x07 },
	{ 0x3480, 0x49 },
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_RESET, CC_UP},
	{CC_DELAY, 20},
};

static struct cc_power_act power_down_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
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
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
		.mfreq = 223 * 1000 * 1000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{

	u8 id;
	int ret = 0;

	ret = cc_si->read_reg(cc_si, 0x3008, &id);
	if (ret != 0) {
		PRT_ERR("id=%x\n", id);
		return -ENODEV;
	}

	if (id != 0xA0) {
		PRT_ERR("ID wrong! (0x%x)\n", id);
		return -ENODEV;
	}

	PRT_INFO("id=%x\n", id);

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
	ret = cc_si->write_reg_list(cc_si, init_reg_list,
					ARRAY_SIZE(init_reg_list));
	if (ret != 0) {
		PRT_ERR("write %s init_reg_list failed!\n", cc_si->name);
		return -ENODEV;
	}
	ktime_get_ts(&ts_f);

	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

	if (cc_si->vflip && cc_si->mirror) {
		ret = cc_si->write_reg(cc_si, 0x3007, 0x03);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else if (cc_si->vflip) {
		ret = cc_si->write_reg(cc_si, 0x3007, 0x01);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else if (cc_si->mirror) {
		ret = cc_si->write_reg(cc_si, 0x3007, 0x02);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	}

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
	PRT_INFO("*************************on:%d\n", on);

	if (on) {
		ret = cc_si->write_reg(cc_si, 0x3000, 0x00);
		if (ret)
			PRT_ERR("%d\n", on);
	} else {
		ret = cc_si->write_reg(cc_si, 0x3000, 0x01);
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

	sensor_fmt_info->pclk = 149*1000*1000;
	sensor_fmt_info->vts = 1125;
	sensor_fmt_info->hts = 4400;
	sensor_fmt_info->hoffset = 0;
	sensor_fmt_info->voffset = 0;
	sensor_fmt_info->width = 1920;
	sensor_fmt_info->height = 1080;
	sensor_fmt_info->bayer_mode = ISP_PATTERN_RGGB;
	sensor_fmt_info->buswidth = ISP_INPUT_12BITS;

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;
	u32 vmax, shs;

	if (shutter_time < 2)
		shutter_time = 2;

	if (shutter_time > VMAX - 2)
		vmax = shutter_time + 2;
	else
		vmax = VMAX;
	ret += cc_si->write_reg(cc_si, 0x3018, ((vmax >> 0) & 0xff));
	ret += cc_si->write_reg(cc_si, 0x3019, ((vmax >> 8) & 0xff));

	shs = vmax - (shutter_time + 1);

	ret += cc_si->write_reg(cc_si, 0x3020, ((shs >> 0) & 0xff));
	ret += cc_si->write_reg(cc_si, 0x3021, ((shs >> 8) & 0xff));
	ret += cc_si->write_reg(cc_si, 0x3022, ((shs >> 16) & 0x3));

	return ret;
}

static u32 gain_table[231] = {
	   256,    264,    274,    283,    293,    304,    314,    326,
	   337,    349,    361,    374,    387,    401,    415,    429,
	   444,    460,    476,    493,    510,    528,    547,    566,
	   586,    607,    628,    650,    673,    697,    721,    746,
	   773,    800,    828,    857,    887,    918,    951,    984,
	  1019,   1054,   1092,   1130,   1170,   1211,   1253,   1297,
	  1343,   1390,   1439,   1490,   1542,   1596,   1652,   1710,
	  1771,   1833,   1897,   1964,   2033,   2104,   2178,   2255,
	  2334,   2416,   2501,   2589,   2680,   2774,   2872,   2973,
	  3077,   3185,   3297,   3413,   3533,   3657,   3786,   3919,
	  4057,   4199,   4347,   4500,   4658,   4822,   4991,   5167,
	  5348,   5536,   5731,   5932,   6141,   6356,   6580,   6811,
	  7050,   7298,   7555,   7820,   8095,   8379,   8674,   8979,
	  9294,   9621,   9959,  10309,  10671,  11046,  11435,  11836,
	 12252,  12683,  13129,  13590,  14068,  14562,  15074,  15604,
	 16152,  16720,  17307,  17915,  18545,  19197,  19871,  20570,
	 21293,  22041,  22816,  23617,  24447,  25306,  26196,  27116,
	 28069,  29056,  30077,  31134,  32228,  33361,  34533,  35747,
	 37003,  38303,  39649,  41043,  42485,  43978,  45523,  47123,
	 48779,  50494,  52268,  54105,  56006,  57974,  60012,  62121,
	 64304,  66564,  68903,  71324,  73831,  76425,  79111,  81891,
	 84769,  87748,  90832,  94024,  97328, 100748, 104289, 107954,
	111748, 115675, 119740, 123948, 128303, 132812, 137480, 142311,
	147312, 152489, 157848, 163395, 169137, 175081, 181234, 187603,
	194195, 201020, 208084, 215397, 222966, 230802, 238913, 247309,
	256000, 264996, 274308, 283948, 293927, 304256, 314948, 326016,
	337473, 349333, 361609, 374317, 387471, 401088, 415183, 429773,
	444877, 460510, 476694, 493446, 510787, 528737, 547318, 566552,
	586462, 607071, 628405, 650489, 673348, 697011, 721506
};

static char gain_mode_buf = 0x02;
static int viss_isp_set_gain(struct cc_sensor_info *cc_si, int gain_val)
{
	int ret;
	u8 rdval, db_idx;
	char gain_mode = 0x02;

	ret = cc_si->read_reg(cc_si, 0x3009, &rdval);
	if (ret != 0)
		return ret;

	if (gain_val < 1 * 256)
		gain_val = 256;

	for (db_idx = 0; db_idx < 231; db_idx++) {
		if (gain_val <= gain_table[db_idx])
			break;
	}

	/* 21*0.3db = 6.3db > 2x gain */
	if (db_idx >= 21) {
		gain_mode = rdval | 0x10;
		ret = cc_si->write_reg(cc_si, 0x3014, db_idx - 20);
	} else {
		gain_mode = rdval & 0xef;
		ret = cc_si->write_reg(cc_si, 0x3014, db_idx);
	}

	/*
	usleep_range(14000, 15000);
	ret = cc_si->write_reg(cc_si, 0x3009, gain_mode);
	*/
	gain_mode_buf = gain_mode;
	schedule_delayed_work(&cc_si->sensor_work, msecs_to_jiffies(45));

	return 0;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

	ret = cc_si->write_reg(cc_si, 0x3001, 0x01);

	ret = viss_isp_set_shutter(cc_si, exp_gain->exp_time / 16);
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
	ret = cc_si->write_reg(cc_si, 0x3001, 0x00);

	return ret;
}

static void sensor_gain_switch(struct work_struct *work)
{
	struct cc_sensor_info *cc_si = container_of(to_delayed_work(work),
				struct cc_sensor_info, sensor_work);
	/* HCG and LCG mode switch */
	cc_si->write_reg(cc_si, 0x3009, gain_mode_buf);
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


#define DRIVER_NAME "imx327_mipi"

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

	cc_si->dlanes = 4;

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

	INIT_DELAYED_WORK(&cc_si->sensor_work, sensor_gain_switch);

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

static const struct of_device_id imx327_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, imx327_mipi_of_match);

static struct platform_driver imx327_mipi_driver = {
	.driver = {
		.of_match_table	= imx327_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(imx327_mipi_driver);

MODULE_DESCRIPTION("imx327-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


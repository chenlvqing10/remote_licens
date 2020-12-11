/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * f37 ahd chip driver code for LomboTech
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

/*
 * 2lane init
 */
static const cc_reg_t init_reg_list[] = {
	/* window_size=1928*1080,mipi 2lane, */
	/* mclk=24mhz, mipi_clock=300mhz, frame_rate=30fps,
	 * row_time=29.33us pclk =74.25M
	 */
	/* pixel_line_total=2200,line_frame_total=1125 */
	/* system */
	{0xfe, 0x80},
	{0xfe, 0x80},
	{0xfe, 0x80},
	{0xfe, 0x00},
	{0xf2, 0x00}, /* [1]I2C_open_ena [0]pwd_dn */
	{0xf3, 0x00}, /* 0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io */
	{0xf4, 0x36}, /* [6:4]pll_ldo_set */
	 /* [7]soc_mclk_enable [6]pll_ldo_en
	  * [5:4]cp_clk_sel [3:0]cp_clk_div
	  */
	{0xf5, 0xc0},
	{0xf6, 0x44}, /* [7:3]wpllclk_div [2:0]refmp_div */
	/* [7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode
	 * [3]refmp_enb [1]div2en [0]pllmp_en
	 */
	{0xf7, 0x01},
	{0xf8, 0x32}, /* 38////38//[7:0]pllmp_div */
	/* 82//[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc */
	{0xf9, 0x42},
	{0xfc, 0x8e},
	/*cisctl&analog*/
	{0xfe, 0x00},
	{0x87, 0x18}, /* [6]aec_delay_mode */
	{0xee, 0x30}, /* [5:4]dwen_sramen */
	{0xd0, 0xb7}, /* ramp_en */
	{0x03, 0x04},
	{0x04, 0x10},
	{0x05, 0x04}, /* 05 */
	{0x06, 0x4c}, /* 28//[11:0]hb */
	{0x07, 0x00},
	{0x08, 0xfa},
	{0x09, 0x00},
	{0x0a, 0x02}, /* cisctl row start */
	{0x0b, 0x00},
	{0x0c, 0x02}, /* cisctl col start */
	{0x0d, 0x04},
	{0x0e, 0x40},
	{0x0f, 0x07},
	{0x10, 0x88},
	{0x12, 0xe2}, /* vsync_ahead_mode */
	{0x13, 0x16},
	{0x19, 0x0a}, /* ad_pipe_num */
	{0x21, 0x1c}, /* eqc1fc_eqc2fc_sw */
	{0x28, 0x0a}, /* 16//eqc2_c2clpen_sw */
	{0x29, 0x24}, /* eq_post_width */
	{0x2b, 0x04}, /* c2clpen --eqc2 */
	{0x32, 0xf8}, /* [5]txh_en ->avdd28 */
	{0x37, 0x03}, /* [3:2]eqc2sel=0 */
	{0x39, 0x15}, /* 17 //[3:0]rsgl */
	{0x43, 0x07}, /* vclamp */
	{0x44, 0x40}, /* 0e//post_tx_width */
	{0x46, 0x0b}, /* txh?a?a3.2v */
	{0x4b, 0x20}, /* rst_tx_width */
	{0x4e, 0x08}, /* 12//ramp_t1_width */
	{0x55, 0x20}, /* read_tx_width_pp */
	{0x66, 0x05}, /* 18//stspd_width_r1 */
	{0x67, 0x05}, /* 40//5//stspd_width_r */
	{0x77, 0x01}, /* dacin offset x31 */
	{0x78, 0x00}, /* dacin offset */
	{0x7c, 0x93}, /* [1:0] co1comp */
	{0x8c, 0x12}, /* 12 ramp_t1_ref */
	{0x8d, 0x92},
	{0x90, 0x00},
	{0x41, 0x04},
	{0x42, 0x65},
	{0x9d, 0x10},
	{0xce, 0x7c}, /* 70//78//[4:2]c1isel */
	{0xd2, 0x41}, /* [5:3]c2clamp */
	{0xd3, 0xdc}, /* 0x39[7]=0,0xd3[3]=1 rsgh=vref */
	{0xe6, 0x40}, /* ramps offset */
	/*gain*/
	{0xb6, 0xc0},
	{0xb0, 0x70},
	{0xb1, 0x01},
	{0xb2, 0x00},
	{0xb3, 0x00},
	{0xb4, 0x00},
	{0xb8, 0x01},
	{0xb9, 0x00},
	/*blk*/
	{0x26, 0x30},
	{0xfe, 0x01},
	{0x40, 0x23},
	{0x55, 0x07},
	{0x60, 0x00}, /* [7:0]WB_offset */
	{0xfe, 0x04},
	{0x14, 0x78},
	{0x15, 0x78},
	{0x16, 0x78},
	{0x17, 0x78},
	/*window*/
	{0xfe, 0x01},
	{0x94, 0x01},
	{0x95, 0x04},
	{0x96, 0x38}, /* [10:0]out_height */
	{0x97, 0x07},
	{0x98, 0x80}, /* [11:0]out_width */
	/*ISP*/
	{0xfe, 0x01},
	{0x01, 0x05},
	{0x02, 0x89},
	{0x04, 0x01},
	{0x07, 0xa6},
	{0x08, 0xa9},
	{0x09, 0xa8},
	{0x0a, 0xa7},
	{0x0b, 0xff},
	{0x0c, 0xff},
	{0x0f, 0x00},
	{0x50, 0x1c},
	{0x89, 0x03},
	{0xfe, 0x04},
	{0x28, 0x86},
	{0x29, 0x86},
	{0x2a, 0x86},
	{0x2b, 0x68},
	{0x2c, 0x68},
	{0x2d, 0x68},
	{0x2e, 0x68},
	{0x2f, 0x68},
	{0x30, 0x4f},
	{0x31, 0x68},
	{0x32, 0x67},
	{0x33, 0x66},
	{0x34, 0x66},
	{0x35, 0x66},
	{0x36, 0x66},
	{0x37, 0x66},
	{0x38, 0x62},
	{0x39, 0x62},
	{0x3a, 0x62},
	{0x3b, 0x62},
	{0x3c, 0x62},
	{0x3d, 0x62},
	{0x3e, 0x62},
	{0x3f, 0x62},

	/*DVP & MIPI*/
	{0xfe, 0x01},
	/*
	 * [5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk
	 * [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
	 */
	{0x9a, 0x06},
	{0xfe, 0x00},
	 /* [7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk */
	{0x7b, 0x2a},
	{0x23, 0x2d}, /* [3]rst_rc [2:1]drv_sync [0]pwd_rc */
	{0xfe, 0x03},
	 /* 20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en */
	{0x01, 0x27},
	/* [7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff */
	{0x02, 0x56},
	 /* b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph
	  * [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
	  */
	{0x03, 0x8e},
	{0x12, 0x80},
	{0x13, 0x07}, /* LWC */
	{0x15, 0x12}, /* [1:0]clk_lane_mode */
	{0xfe, 0x00},
	/* 40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3] */
	/* {0x3e, 0x91}, */
};

static struct cc_power_act power_up_act[] = {
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 20},
	{CC_RESET, CC_UP},
	{CC_DELAY, 10},
	{CC_PWDN, CC_UP},
	{CC_DELAY, 20},
};

static struct cc_power_act power_down_act[] = {
	{CC_PWDN, CC_DOWN},
	{CC_DELAY, 10},
	{CC_RESET, CC_DOWN},
	{CC_DELAY, 10},
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
		.width = 1728,
		.height = 1080,
		.left = 96,
		.top = 0,
		.pclk = 74250000,
		.vts = 1125,
		.hts = 2200,
	},

	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,
		.left = 0,
		.top = 0,
		.pclk = 74250000,
		.vts = 1125,
		.hts = 2200,
	},
};

static const struct cc_frame_cfg frame_cfgs[] = {
	{
		.width = 1728,
		.height = 1080,

		.framerate = { 1, 30 }, /* 30fps */
		.mfreq = 445*1000*1000, /* mipi-csi clock */
	},

	{
		.width = FHD_WIDTH,
		.height = FHD_HEIGHT,

		.framerate = { 1, 30 },	/* 30fps */
		.mfreq = 445*1000*1000,	/* mipi-csi clock */
	},
};

static int sensor_detect(struct cc_sensor_info *cc_si)
{
	u8 id_hi;
	u8 id_low;
	u16 id;
	int ret = 0;

	ret = cc_si->read_reg(cc_si, 0xf0, &id_hi);
	if (ret != 0) {
		PRT_ERR("id_hi =%x\n", id_hi);
		return -ENODEV;
	}
	ret = cc_si->read_reg(cc_si, 0xf1, &id_low);
	if (ret != 0) {
		PRT_ERR("id_low =%x\n", id_low);
		return -ENODEV;
	}

	id = (id_hi << 8) | id_low;
	if (id != 0x2053) {
		PRT_ERR("ID wrong! (0x%x)\n", id);
		return -ENODEV;
	}
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

	if (cc_si->vflip && cc_si->mirror) {
		ret = cc_si->write_reg(cc_si, 0x17, 0x83);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else if (cc_si->vflip) {
		ret = cc_si->write_reg(cc_si, 0x17, 0x82);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	} else if (cc_si->mirror) {
		ret = cc_si->write_reg(cc_si, 0x17, 0x81);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	}

	PRT_INFO("timeused %d %d\n", (int)(ts_f.tv_sec - ts_s.tv_sec),
		(int)(ts_f.tv_nsec - ts_s.tv_nsec));

	return 0;
}

static int sensor_set_stream(struct cc_sensor_info *cc_si, int on)
{
	int ret = 0;

	PRT_INFO("*************************on:%d\n", on);

	if (on)
		ret = cc_si->write_reg(cc_si, 0x3e, 0x91);
	else {
		ret = cc_si->write_reg(cc_si, 0x3e, 0x00);
		msleep(50); /* 50ms */
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

	PRT_INFO("addr: 0x%x, val: 0x%02x.\n",
		reg_data->reg_add, reg_data->reg_value);

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
	sensor_fmt_info->bayer_mode = ISP_PATTERN_RGGB;
	sensor_fmt_info->buswidth = ISP_INPUT_10BITS;

	return ret;
}

u32 reg_val_table[29][4] = {
	{0x00, 0x00, 0x01, 0x00},
	{0x00, 0x10, 0x01, 0x0c},
	{0x00, 0x20, 0x01, 0x1b},
	{0x00, 0x30, 0x01, 0x2c},
	{0x00, 0x40, 0x01, 0x3f},
	{0x00, 0x50, 0x02, 0x16},
	{0x00, 0x60, 0x02, 0x35},
	{0x00, 0x70, 0x03, 0x16},
	{0x00, 0x80, 0x04, 0x02},
	{0x00, 0x90, 0x04, 0x31},
	{0x00, 0xa0, 0x05, 0x32},
	{0x00, 0xb0, 0x06, 0x35},
	{0x00, 0xc0, 0x08, 0x04},
	{0x00, 0x5a, 0x09, 0x19},
	{0x00, 0x83, 0x0b, 0x0f},
	{0x00, 0x93, 0x0d, 0x12},
	{0x00, 0x84, 0x10, 0x00},
	{0x00, 0x94, 0x12, 0x3a},
	{0x01, 0x2c, 0x1a, 0x02},
	{0x01, 0x3c, 0x1b, 0x20},
	{0x00, 0x8c, 0x20, 0x0f},
	{0x00, 0x9c, 0x26, 0x07},
	{0x02, 0x64, 0x36, 0x21},
	{0x02, 0x74, 0x37, 0x3a},
	{0x00, 0xc6, 0x3d, 0x02},
	{0x00, 0xdc, 0x3f, 0x3f},
	{0x02, 0x85, 0x3f, 0x3f},
	{0x02, 0x95, 0x3f, 0x3f},
	{0x00, 0xce, 0x3f, 0x3f},
};

u32 analog_multiple[29] = {
	1024,
	1184,
	1424,
	1632,
	2032,
	2352,
	2832,
	3248,
	4160,
	4800,
	5776,
	6640,
	8064,
	9296,
	11552,
	13312,
	16432,
	18912,
	22528,
	25936,
	31840,
	36656,
	45600,
	52512,
	64768,
	82880,
	88000,
	107904,
	113168,
};

static u32 cur_expline = -1;
static u32 cur_again = -1;
static u32 cur_dgain = -1;

static void gc2053_calc_gain(u32 gain, u32 *_again, u32 *_dgain)
{
	u32 decimal;
	u32 analog_index;
	u32 dgain;

	if (gain < 1024)
		gain = 1024;
	if (gain > 127 * 1024)
		gain = 127*1024;

	PRT_DBG("gain = %d ", gain);

	analog_index = 0;
	while (analog_index < 29) {
		if (gain < analog_multiple[analog_index])
			break;
		analog_index++;
	}
	dgain = gain * 1000 / analog_multiple[analog_index - 1];
	decimal = (dgain * 64) / 1000;

	*_dgain = (decimal << 2) & 0xfffc;
	*_again = analog_index - 1;
}

static int gc2053_set_again(struct cc_sensor_info *cc_si, u32 again)
{
	int ret = 0;

	if (cur_again == again)
		return ret;
	cur_again = again;

	ret = cc_si->write_reg(cc_si,	0xb4, reg_val_table[again][0]);
	ret = cc_si->write_reg(cc_si,	0xb3, reg_val_table[again][1]);
	ret = cc_si->write_reg(cc_si,	0xb8, reg_val_table[again][2]);
	ret = cc_si->write_reg(cc_si,	0xb9, reg_val_table[again][3]);

	return ret;
}

static int gc2053_set_dgain(struct cc_sensor_info *cc_si, u32 dgain)
{
	int ret = 0;

	if (cur_dgain == dgain)
		return ret;
	cur_dgain = dgain;

	ret = cc_si->write_reg(cc_si,	0xb1, (dgain >> 8) & 0xff);
	ret = cc_si->write_reg(cc_si,	0xb2, (dgain >> 0) & 0xff);

	return ret;
}

static int viss_isp_set_shutter(struct cc_sensor_info *cc_si,
				__u32 shutter_time)
{
	int ret = 0;

	if (cur_expline == shutter_time)
		return ret;
	cur_expline = shutter_time;

	ret = cc_si->write_reg(cc_si, 0xfe, 00);
	ret = cc_si->write_reg(cc_si, 0x03, (shutter_time >> 8) & 0xff);
	ret = cc_si->write_reg(cc_si, 0x04, (shutter_time >> 0) & 0xff);

	return ret;
}

static int viss_isp_set_gain(struct cc_sensor_info *cc_si, int gain_val)
{
	int ret = 0;
	u32 gain;
	u32 again, dgain;

	gain = (gain_val * 4);
	gc2053_calc_gain(gain, &again, &dgain);
	ret = gc2053_set_again(cc_si, again);
	ret = gc2053_set_dgain(cc_si, dgain);

	return ret;
}

static int viss_isp_exp_ctrl(struct cc_sensor_info *cc_si,
				struct viss_isp_exp_gain *exp_gain)
{
	int ret = 0;

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


#define DRIVER_NAME "gc2053_mipi"

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

static const struct of_device_id gc2053_mipi_of_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, gc2053_mipi_of_match);

static struct platform_driver gc2053_mipi_driver = {
	.driver = {
		.of_match_table	= gc2053_mipi_of_match,
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
};

module_platform_driver(gc2053_mipi_driver);

MODULE_DESCRIPTION("GC2053-MIPI sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);


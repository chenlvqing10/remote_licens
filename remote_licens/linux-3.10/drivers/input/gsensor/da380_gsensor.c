/*
 * da380_gsensor.c - Gsensor module realization
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#ifdef CONFIG_LOMBO_RTC_POWER
#include <mach/rtc_power.h>
#endif

#define DA380_GSENSOR_ADDR		0x27

/* DA380 Register */
#define SOFT_RESET			0x00
#define CHIPID				0x01

#define ACC_X_LSB			0x02
#define ACC_X_MSB			0x03

#define ACC_Y_LSB			0x04
#define ACC_Y_MSB			0x05

#define ACC_Z_LSB			0x06
#define ACC_Z_MSB			0x07

#define MOTION_FLAG			0x09
#define NEWDATA_FLAG			0x0A

#define TAP_ACTIVE_STATUS		0x0B
#define ORIENT_STATUS			0x0C

/*
 Resolution
 bit[3:2] -- 00:14bit, 01:12bit, 10:10bit, 11:8bit
 FS bit[1:0] -- 00:+/-2g, 01:+/-4g, 10:+/-8g, 11:+/-16g
*/
#define RESOLUTION_RANGE		0x0F

#define ODR_AXIS			0x10
#define MODE_BW				0x11
#define SWAP_POLARITY			0x12
#define INT_SET1			0x16
#define INT_SET2			0x17
#define INT_MAP1			0x19
#define INT_MAP2			0x1A
#define INT_CONFIG			0x20
#define INT_LATCH			0x21
#define FREEFALL_DUR			0x22
#define FREEFALL_THS			0x23
#define FREEFALL_HYST			0x24
#define ACTIVE_DUR			0x27
#define ACTIVE_THS			0x28
#define TAP_DUR				0x2A
#define TAP_THS				0x2B
#define ORIENT_HYST			0x2C
#define Z_BLOCK				0x2D
#define SELF_TEST			0x32
#define ENGINEERING_MODE		0x7f

/* gsensor function parameter define */
#define GS_MEASURE_RANGE_2G		1
#define GS_MEASURE_RANGE_4G		2
#define GS_MEASURE_RANGE_8G		3
#define GS_MEASURE_RANGE_16G		4

#define GS_ACC_X			1
#define GS_ACC_Y			2
#define GS_ACC_Z			3

#define GS_MODE_NORMAL			1
#define GS_MODE_LOW_POWER		2
#define GS_MODE_SUSPEND			3

#define GS_MOTION_FLAG_ORIENT		1
#define GS_MOTION_FLAG_S_TAP		2
#define GS_MOTION_FLAG_D_TAP		3
#define GS_MOTION_FLAG_ACTIVE		4
#define GS_MOTION_FLAG_FREEFALL		5

#define GS_AXIS_DISABLE_X		1
#define GS_AXIS_DISABLE_Y		2
#define GS_AXIS_DISABLE_Z		3

#define GS_ORIENT_INT_EN		1
#define GS_S_TAP_INT_EN			2
#define GS_D_TAP_INT_EN			3
#define GS_ACTIVE_INT_EN_Z		4
#define GS_ACTIVE_INT_EN_Y		5
#define GS_ACTIVE_INT_EN_X		6
#define GS_NEW_DATA_INT_EN		7
#define GS_FREEFALL_INT_EN		8

#define GS_INT_ORIENT			1
#define GS_INT_S_TAP			2
#define GS_INT_D_TAP			3
#define GS_INT_ACTIVE			4
#define GS_INT_FREEFALL			5
#define GS_INT_NEW_DATA			6

struct gsensor_i2c {
	struct i2c_client *client;
};

static struct gsensor_i2c *_gs_i2c_dev;

/**
 * gsensor_write - write a 8bit data to gsensor register by i2c
 * @reg: register of gsensor to write
 * @data: the data to write
 *
 * return 0 if success, other if failed
 */
static int gsensor_write(u8 reg, u8 data)
{
	struct i2c_client *client;
	struct i2c_msg msgs;
	u8 buf[2] = {reg, data};

	if (!_gs_i2c_dev) {
		pr_err("gsensor_write error: _gs_i2c_dev == NULL\n");
		return -EINVAL;
	}

	client = _gs_i2c_dev->client;

	msgs.addr  = client->addr;
	msgs.flags = 0;
	msgs.buf   = buf;
	msgs.len   = 2;

	if (i2c_transfer(client->adapter, &msgs, 1) != 1) {
		pr_err("gsensor_write: i2c_transfer error\n");
		return -EIO;
	}

	return 0;
}

/**
 * gsensor_read - read a 8bit data from gsensor register by i2c
 * @reg: register of gsensor to read
 * @redata: the data to store
 *
 * return 0 if success, other if failed
 */
static int gsensor_read(u8 reg, u8 *redata)
{
	struct i2c_client *client;
	struct i2c_msg xfer[2];

	if (!_gs_i2c_dev) {
		pr_err("gsensor_read error: _gs_i2c_dev == NULL\n");
		return -EINVAL;
	}

	client = _gs_i2c_dev->client;

	/* the register to read */
	xfer[0].addr = client->addr;
	xfer[0].flags = 0;
	xfer[0].len = 1;
	xfer[0].buf = &reg;

	/* Read data */
	xfer[1].addr = client->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 1;
	xfer[1].buf = redata;

	if (i2c_transfer(client->adapter, xfer, 2) != 2) {
		pr_err("gsensor_read: i2c_transfer error\n");
		return -EIO;
	}

	return 0;
}

/**
 * gsensor_set_bit - set one bit value for register of gsensor
 * @reg: register of gsensor to set
 * @n: bit position
 * @bv: bit value
 *
 * return return 0 if success, other if failed
 */
static int gsensor_set_bit(u8 reg, u8 n, u8 bv)
{
	u8 val, mask;
	int ret;

	if ((bv != 0x00) && (bv != 0x01)) {
		pr_err("param bv error\n");
		return -EINVAL;
	}

	ret = gsensor_read(reg, &val);
	if (ret != 0) {
		pr_err("gsensor_set_bit error\n");
		return -EIO;
	}

	mask = ~(1u << n);
	val = (val & mask) | (bv << n);

	ret = gsensor_write(reg, val);
	if (ret != 0) {
		pr_err("gsensor_set_bit error\n");
		return -EIO;
	}

	return 0;
}

/**
 * gsensor_get_bit - get one bit value from register of gsensor
 * @reg: register of gsensor
 * @n: bit position
 * @bv: bit value to store
 *
 * return return 0 if success, other if failed
 */
static int gsensor_get_bit(u8 reg, u8 n, u8 *bv)
{
	int ret;
	u8 val, msk;

	if (n > 7) {
		pr_err("param n error\n");
		return -EINVAL;
	}

	ret = gsensor_read(reg, &val);
	if (ret != 0) {
		pr_err("gsensor_get_bit error\n");
		return -EIO;
	}

	msk = (u8)(1u << n);
	*bv = ((u8)(val & msk)) >> n;
	return 0;
}

/* set gsensor measure range: 2g, 4g, 8g, 16g */
void gsensor_set_measure_range(int type)
{
	u8 val;
	int ret;

	/* unused | unused | unused | unused | Resolution[1:0] | FS[1:0] */
	ret = gsensor_read(RESOLUTION_RANGE, &val);
	if (ret != 0) {
		pr_err("gsensor_read error\n");
		return;
	}

	/* clear FS[1:0] */
	val = val & 0xfc;

	/* FS[1:0] -- 00: 2g, 01: 4g, 10: 8g, 11: 16g */
	switch (type) {
	case GS_MEASURE_RANGE_2G:
		val = val | 0x00;
		break;

	case GS_MEASURE_RANGE_4G:
		val = val | 0x01;
		break;

	case GS_MEASURE_RANGE_8G:
		val = val | 0x02;
		break;

	case GS_MEASURE_RANGE_16G:
		val = val | 0x03;
		break;

	default:
	{
		pr_err("set range error, unknown type: %d\n", type);
		return;
	}
		break;
	}

	ret = gsensor_write(RESOLUTION_RANGE, val);
	if (ret != 0)
		pr_err("gsensor_set_measure_range error\n");
}

/* set gsensor power mode */
void gsensor_set_pwr_mode(int type)
{
	u8 val;
	int ret;

	/* pwr_mode [1:0] | unused | low_power_bw [3:0] | unused*/
	ret = gsensor_read(MODE_BW, &val);
	if (ret != 0) {
		pr_err("gsensor_read error\n");
		return;
	}

	/* clear pwr_mode[1:0] */
	val = val & 0x3f;

	/*
	pwr_mode [1:0]
	00:normal mode
	01: low power mode
	1x: suspend mode
	*/
	switch (type) {
	case GS_MODE_NORMAL:
		val = val | 0x00;
		break;

	case GS_MODE_LOW_POWER:
		val = val | 0x40;
		break;

	case GS_MODE_SUSPEND:
		val = val | 0x80;
		break;

	default:
	{
		pr_err("set_pwr_mode error, unknown type: %d\n", type);
		return;
	}
		break;
	}

	ret = gsensor_write(MODE_BW, val);
	if (ret != 0) {
		pr_err("gsensor_set_measure_range error\n");
		return;
	}
}

/* set gsensor axis disable */
void gsensor_set_axis_disable(int type, bool disable)
{
	u8 n, val;
	int ret;

	ret = gsensor_read(ODR_AXIS, &val);
	if (ret != 0) {
		pr_err("gsensor_read error\n");
		return;
	}

	val = disable ? 0x01 : 0x00;

	switch (type) {
	case GS_AXIS_DISABLE_X:
		n = 7;
		break;

	case GS_AXIS_DISABLE_Y:
		n = 6;
		break;

	case GS_AXIS_DISABLE_Z:
		n = 5;
		break;

	default:
	{
		pr_err("set_axis_disable error, unknown type: %d\n", type);
		return;
	}
		break;
	}

	ret = gsensor_set_bit(ODR_AXIS, n, val);
	if (ret != 0)
		pr_err("gsensor_set_axis_disable error\n");
}

/* set gsensor interrupt enable */
void gsensor_set_int_en(int type, bool en)
{
	int ret;
	u8 n, reg, val;

	/* 0: disable the orient interrupt, 1:enable the orient interrupt */
	val = en ? 0x01 : 0x00;

	/*
	INT_SET1 register
	unused | Orient_int_en | S_tap_int_en | d_tap_int_en
	unused | active_int_en_z | active_int_en_y | active_int_en_x

	INT_SET2 register
	unused | unused | unused |New_data_int_en
	Freefall_int_en | unused | unused | unused
	*/
	switch (type) {
	case GS_ORIENT_INT_EN:
		n = 6;
		reg = INT_SET1;
		break;

	case GS_S_TAP_INT_EN:
		n = 5;
		reg = INT_SET1;
		break;

	case GS_D_TAP_INT_EN:
		n = 4;
		reg = INT_SET1;
		break;

	case GS_ACTIVE_INT_EN_Z:
		n = 2;
		reg = INT_SET1;
		break;

	case GS_ACTIVE_INT_EN_Y:
		n = 1;
		reg = INT_SET1;
		break;

	case GS_ACTIVE_INT_EN_X:
		n = 0;
		reg = INT_SET1;
		break;

	case GS_NEW_DATA_INT_EN:
		n = 4;
		reg = INT_SET2;
		break;

	case GS_FREEFALL_INT_EN:
		n = 3;
		reg = INT_SET2;
		break;

	default:
		pr_err("gsensor_set_int_en error, unknown type: %d\n", type);
		return;
	}

	ret = gsensor_set_bit(reg, n, val);
	if (ret != 0)
		pr_err("gsensor_set_int_en error\n");
}

/* set gsensor INT map */
void gsensor_set_int_map(int type, bool mp)
{
	int ret;
	u8 n, reg, val;

	/* 0:doesn't mapping orient interrupt to INT */
	/* 1:mapping orient interrupt to INT */
	val = mp ? 0x01 : 0x00;

	/*
	INT_MAP1
	x | Int_orient | Int_s_tap | Int_d_tap
	x | Int_active | x | Int_freefall

	INT_MAP2
	x | x | x | x | x | x | x | Int_new_data
	*/
	switch (type) {
	case GS_INT_ORIENT:
		n = 6;
		reg = INT_MAP1;
		break;

	case GS_INT_S_TAP:
		n = 5;
		reg = INT_MAP1;
		break;

	case GS_INT_D_TAP:
		n = 4;
		reg = INT_MAP1;
		break;

	case GS_INT_ACTIVE:
		n = 2;
		reg = INT_MAP1;
		break;

	case GS_INT_FREEFALL:
		n = 0;
		reg = INT_MAP1;
		break;

	case GS_INT_NEW_DATA:
		n = 0;
		reg = INT_MAP2;
		break;

	default:
		pr_err("gsensor_set_int_map error, unknown type: %d\n", type);
		return;
	}

	ret = gsensor_set_bit(reg, n, val);
	if (ret != 0)
		pr_err("gsensor_set_int_map error\n");
}

/* get gsensor interrupt flag */
int gsensor_get_motion_flag(int type, u8 *flag)
{
	int ret;
	u8 val, n;

	/*
	unused |Orient_int |S_tap_int |D_tap_int |
	unused |Active_int |unused |Freefall_int
	*/
	ret = gsensor_read(MOTION_FLAG, &val);
	if (ret != 0) {
		pr_err("gsensor_read error\n");
		return -EIO;
	}

	switch (type) {
	case GS_MOTION_FLAG_ORIENT:
		n = 6;
		break;

	case GS_MOTION_FLAG_S_TAP:
		n = 5;
		break;

	case GS_MOTION_FLAG_D_TAP:
		n = 4;
		break;

	case GS_MOTION_FLAG_ACTIVE:
		n = 2;
		break;

	case GS_MOTION_FLAG_FREEFALL:
		n = 0;
		break;

	default:
		pr_err("get_motion_flag error, unknown type: %d\n", type);
		return -EIO;
	}

	ret = gsensor_get_bit(MOTION_FLAG, n, flag);
	if (ret != 0)
		pr_err("gsensor_get_motion_flag error\n");

	return ret;
}

/* get gsensor acc value */
s16 gsensor_get_acc(int type)
{
	s16 acc;
	u8 lsb, msb;

	switch (type) {
	case GS_ACC_X:
		gsensor_read(ACC_X_LSB, &lsb);
		gsensor_read(ACC_X_MSB, &msb);
		break;

	case GS_ACC_Y:
		gsensor_read(ACC_Y_LSB, &lsb);
		gsensor_read(ACC_Y_MSB, &msb);
		break;

	case GS_ACC_Z:
		gsensor_read(ACC_Z_LSB, &lsb);
		gsensor_read(ACC_Z_MSB, &msb);
		break;

	default:
		pr_err("gsensor_get_acc error, unknown type: %d\n", type);
		return -1;
	}

	acc = ((s16)(msb << 8 | lsb)) >> 2;
	return acc;
}

/* reset gsensor register to default value */
void gsensor_soft_reset(void)
{
	gsensor_write(SOFT_RESET, 0x20);
}

/* reset all latched int */
void gsensor_clr_int_status(void)
{
	gsensor_set_bit(INT_LATCH, 7, 0x01);
}

/* gsensor register config */
static void set_gs_cfg(void)
{
	gsensor_soft_reset();	/* reset register value */
	gsensor_set_measure_range(GS_MEASURE_RANGE_2G);

	/* set power mode */
	gsensor_set_pwr_mode(GS_MODE_NORMAL);

	/* Int_lvl [0] for pin INT, 0: level low; 1: level high */
	gsensor_write(INT_CONFIG, 0x01);

	/* set active INT map */
	gsensor_set_int_map(GS_INT_ACTIVE, true);

	/* active x, y, z interrupt */
	gsensor_set_int_en(GS_ACTIVE_INT_EN_X, true);
	gsensor_set_int_en(GS_ACTIVE_INT_EN_Y, true);
	gsensor_set_int_en(GS_ACTIVE_INT_EN_Z, true);

	/*
	latch_int, interrupt mode
	1111: latched, 0000: non-latched, 0011: temporary latched 1s
	*/
	gsensor_write(INT_LATCH, 0x83);

	/* active duration time */
	gsensor_write(ACTIVE_DUR, 0x03);
}

static int da380_gs_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	int ret;
	u8 val;
	pr_info("--------------- da380_gs_probe ------------\n");

	ret = 0;
	_gs_i2c_dev = kzalloc(sizeof(struct gsensor_i2c), GFP_KERNEL);
	if (!_gs_i2c_dev) {
		pr_err("kzalloc for _gs_i2c_dev error\n");
		ret = -ENOMEM;
		goto fail1;
	}

	_gs_i2c_dev->client = client;
	i2c_set_clientdata(client, _gs_i2c_dev);

	/* no need detect gsensor wakeup when device is running */
	csp_rtc_power_gs_wake_detect_disable();

	/* try to read data from gsensor module
	if retrun false, means gsensor not exist */
	ret = gsensor_read(MOTION_FLAG, &val);
	if (ret != 0) {
		pr_err("gsensor_read error, gsensor not exist\n");
		ret = -EIO;
		goto fail1;
	}

	return 0;

fail1:
	i2c_set_clientdata(client, NULL);
	kfree(_gs_i2c_dev);
	_gs_i2c_dev = NULL;

	return ret;
}

static int da380_gs_remove(struct i2c_client *client)
{
	struct gsensor_i2c *gs;

	gs = i2c_get_clientdata(client);
	kfree(gs);

	return 0;
}

static void da380_gs_shutdown(struct i2c_client *client)
{
	/* setup gsensor for device shutdown */
	set_gs_cfg();

	/* set rtc power manager for detect gsensor wakeup */
	csp_rtc_power_gs_int_trig_cfg(GS_TRIG_LEVEL_HIGH);
	csp_rtc_power_gs_wake_detect_enable();
}

static int da380_gs_suspend(struct device *dev)
{
	return 0;
}

static int da380_gs_resume(struct device *dev)
{
	return 0;
}

static SIMPLE_DEV_PM_OPS(da380_gs_pm_ops, da380_gs_suspend, da380_gs_resume);

static struct of_device_id da380_gs_dt_ids[] = {
	{ .compatible = "lombo,da380_gs" },
	{ /* sentinel */ }
};

static const struct i2c_device_id da380_gs_id[] = {
	{ "da380_gs", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, da380_gs_id);

static struct i2c_driver da380_gs_driver = {
	.driver = {
		.name	= "da380_gs",
		.owner	= THIS_MODULE,
		.pm	= &da380_gs_pm_ops,
		.of_match_table	= of_match_ptr(da380_gs_dt_ids),
	},
	.id_table	= da380_gs_id,
	.probe		= da380_gs_probe,
	.remove		= da380_gs_remove,
	.shutdown	= da380_gs_shutdown,
};

module_i2c_driver(da380_gs_driver);

MODULE_DESCRIPTION("LomboTech gsensor driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");


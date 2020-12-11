/*
 *  stmvl53l0x_module-i2c.c - Linux kernel modules for STM VL53L0 FlightSense TOF
 *							sensor
 *
 *  Copyright (C) 2016 STMicroelectronics Imaging Division.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/platform_device.h>
/*
 * power specific includes
 */
#include <linux/pwm.h>
#include <linux/regulator/consumer.h>
#include <linux/pinctrl/consumer.h>
#include <linux/clk.h>
#include <linux/of_gpio.h>
/*
 * API includes
 */
#include "vl53l0x_api.h"
#include "vl53l0x_def.h"
#include "vl53l0x_platform.h"
#include "stmvl53l0x-i2c.h"
#include "stmvl53l0x-cci.h"
#include "stmvl53l0x.h"
#ifndef CAMERA_CCI

/*
 * Global data
 */
static int stmvl53l0x_parse_vdd(struct device *dev, struct i2c_data *data);

/*
 * QCOM specific functions
 */
static int stmvl53l0x_parse_vdd(struct device *dev, struct i2c_data *data)
{
	int ret = 0;

	vl53l0x_dbgmsg("Enter\n");

	if (dev->of_node) {
		data->vana = regulator_get(dev, "vdd");
		if (IS_ERR(data->vana)) {
			vl53l0x_errmsg("vdd supply is not provided\n");
			ret = -1;
		}
	}
	vl53l0x_dbgmsg("End\n");

	return ret;
}

static int stmvl53l0x_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	int rc = 0;
	struct stmvl53l0x_data *vl53l0x_data = NULL;
	struct i2c_data *i2c_object = NULL;
	struct device_node *np = (client->dev).of_node;

	vl53l0x_dbgmsg("Enter\n");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
		rc = -EIO;
		return rc;
	}

	vl53l0x_data = kzalloc(sizeof(struct stmvl53l0x_data), GFP_KERNEL);
	if (!vl53l0x_data) {
		rc = -ENOMEM;
		return rc;
	}
	if (vl53l0x_data) {
		vl53l0x_data->client_object =
			kzalloc(sizeof(struct i2c_data), GFP_KERNEL);
		i2c_object = (struct i2c_data *)vl53l0x_data->client_object;
	}
	i2c_object->client = client;

	/* setup bus type */
	vl53l0x_data->bus_type = I2C_BUS;

	/* set int pin */
	vl53l0x_data->irq_gpio = of_get_named_gpio(np, "irq-gpios", 0);
	if (!gpio_is_valid(vl53l0x_data->irq_gpio)) {
		dev_err(&client->dev, "No valid irq gpio");
		rc = -EPERM;
		goto get_pin_failed;
	}
	pr_info("get irq_gpio = %d\n", vl53l0x_data->irq_gpio);

	/* setup regulator */
	stmvl53l0x_parse_vdd(&i2c_object->client->dev, i2c_object);

	/* setup device name */
	vl53l0x_data->dev_name = dev_name(&client->dev);

	/* setup device data */
	dev_set_drvdata(&client->dev, vl53l0x_data);

	/* setup client data */
	i2c_set_clientdata(client, vl53l0x_data);

	/* setup other stuff */
	rc = stmvl53l0x_setup(vl53l0x_data);

	/* init default value */
	i2c_object->power_up = 0;

	vl53l0x_dbgmsg("End\n");
	return rc;

get_pin_failed:
	return rc;
}

static int stmvl53l0x_remove(struct i2c_client *client)
{
	struct stmvl53l0x_data *data = i2c_get_clientdata(client);

	vl53l0x_dbgmsg("Enter\n");

	/* Power down the device */
	stmvl53l0x_power_down_i2c(data->client_object);
	stmvl53l0x_cleanup(data);
	kfree(data->client_object);
	kfree(data);
	vl53l0x_dbgmsg("End\n");
	return 0;
}

static const struct i2c_device_id stmvl53l0x_id[] = {
	{ STMVL53L0X_DRV_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, stmvl53l0x_id);

static const struct of_device_id st_stmvl53l0x_dt_match[] = {
	{ .compatible = "st,stmvl53l0", },
	{ },
};

static struct i2c_driver stmvl53l0x_driver = {
	.driver = {
		.name	= STMVL53L0X_DRV_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = st_stmvl53l0x_dt_match,
	},
	.probe	= stmvl53l0x_probe,
	.remove	= stmvl53l0x_remove,
	.id_table = stmvl53l0x_id,

};

int stmvl53l0x_power_up_i2c(void *i2c_object, unsigned int *preset_flag)
{
	int ret = 0;
#ifndef STM_TEST
	struct i2c_data *data = (struct i2c_data *)i2c_object;
#endif

	vl53l0x_dbgmsg("Enter\n");

	/* actual power on */
#ifndef STM_TEST
	ret = regulator_set_voltage(data->vana,	VL53L0X_VDD_MIN, VL53L0X_VDD_MAX);
	if (ret < 0) {
		vl53l0x_errmsg("set_vol(%p) fail %d\n", data->vana , ret);
		return ret;
	}
	ret = regulator_enable(data->vana);
	msleep(3);
	if (ret < 0) {
		vl53l0x_errmsg("reg enable(%p) failed.rc=%d\n", data->vana, ret);
		return ret;
	}
	data->power_up = 1;
	*preset_flag = 1;
#endif

	vl53l0x_dbgmsg("End\n");
	return ret;
}

int stmvl53l0x_power_down_i2c(void *i2c_object)
{
	int ret = 0;
#ifndef STM_TEST
	struct i2c_data *data = (struct i2c_data *)i2c_object;
#endif

	vl53l0x_dbgmsg("Enter\n");
#ifndef STM_TEST
	msleep(3);
	ret = regulator_disable(data->vana);
	if (ret < 0)
		vl53l0x_errmsg("reg disable(%p) failed.rc=%d\n",
			data->vana, ret);

	data->power_up = 0;
#endif

	vl53l0x_dbgmsg("End\n");
	return ret;
}

static u8 i2c_write_byte(struct i2c_client *client, u8 addr)
{
	struct i2c_msg msg;
	u8 data[2];
	int count;

	data[0] = 0x66;
	data[1] = 0x88;

	msg.addr = addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = data;

	count = i2c_transfer(client->adapter, &msg, 1);
	if (count != 1)
		dev_err(&client->dev, "i2c write err, ret = %d\n",
			count);

	return count;
}


int stmvl53l0x_init_i2c(void)
{
	int ret = 0;

#if 0
//#ifdef STM_TEST
	struct i2c_client *client = NULL;
	struct i2c_adapter *adapter;
	struct i2c_board_info info = {
		.type = "stmvl53l0",
		.addr = STMVL53L0X_SLAVE_ADDR,
	};
#endif

	vl53l0x_dbgmsg("Enter\n");

	/* register as a i2c client device */
	ret = i2c_add_driver(&stmvl53l0x_driver);
	if (ret)
		vl53l0x_errmsg("%d erro ret:%d\n", __LINE__, ret);

#if 0
//#ifdef STM_TEST
	if (!ret) {
		adapter = i2c_get_adapter(2);
		if (!adapter) {
			ret = -EINVAL;
		} else {
			client = i2c_new_device(adapter, &info);
		}
		
		if (!client) {
			ret = -EINVAL;
		}
	}

	if (!ret) {
		u8 addr = 0x29;
		int count;
		pr_info("!!!!!!!!!!!!!!! try i2c address !!!!!!!!!!!!!!!!\n");
		count = i2c_write_byte(client, addr);
		if (count == 1)
			pr_info("try i2c address success !!!!\n");
		else
			pr_info("try i2c address errrr #####\n");
	}
#endif

	vl53l0x_dbgmsg("End with rc:%d\n", ret);
	pr_info("End with rc:%d\n", ret);

	return ret;
}

void stmvl53l0x_exit_i2c(void *i2c_object)
{
	vl53l0x_dbgmsg("Enter\n");
	i2c_del_driver(&stmvl53l0x_driver);

	vl53l0x_dbgmsg("End\n");
}

#endif /* end of NOT CAMERA_CCI */

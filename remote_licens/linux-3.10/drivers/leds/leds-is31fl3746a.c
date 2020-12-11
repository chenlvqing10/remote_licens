/*
 * leds-is31fl3746a.c - is31fl3746a led driver module
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
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/leds.h>
#ifdef CONFIG_OF
#include <linux/of_gpio.h>
#endif
#include "leds-is31fl3746a.h"

/*
#undef dev_info
#define dev_info dev_err

#undef pr_info
#define pr_info pr_err
*/

uint8_t data[64];

static void init_3746a(struct led_3746a_dev *led);


static u8 i2c_write_byte(struct i2c_client *client, u8 addr, u8 send)
{
	struct i2c_msg msg;
	u8 data[2];
	int count;

	data[0] = addr;
	data[1] = send;

	msg.addr = client->addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = data;

	count = i2c_transfer(client->adapter, &msg, 1);
	if (count != 1)
		dev_err(&client->dev, "i2c write err, ret = %d\n",
			count);

	return count;
}

/*
static u8 I2C_ReadByte(struct i2c_client *client, u8 readAddress)
{
	int count;
	u8 value;

	struct i2c_msg msgs[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = 1,
			.buf = &readAddress,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = &value,
		},
	};

	count = i2c_transfer(client->adapter, msgs, 2);
	if (count != 2)
		dev_err(&client->dev, "i2c read err, ret = %d\n",
			count);

	return value;
}
*/

static void led_mode_1(struct led_3746a_dev *led)
{
	u8 i, j;

	init_3746a(led);

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x00);

	for (i = 0; i <= 0x48; i++)
		i2c_write_byte(led->client, i, 0x00);

	while ((led->mode == 1) && led->on) {
		for (j = 0; j <= 127; j++) {
			for (i = 0x01; i <= 0x48; i = i + 3) {
				/* PWM */
				i2c_write_byte(led->client, i, PWM_64_table[j]);
				if (led->mode != 1)
					return;
			}

			for (i = 0x02; i <= 0x48; i = i + 3) {
				/* PWM */
				i2c_write_byte(led->client, i, PWM_64_table[j]);
				if (led->mode != 1)
					return;
			}
			mdelay(10);
		}

		for (j = 0; j <= 127; j++) {
			for (i = 0x01; i <= 0x48; i = i + 3) {
				/* PWM */
				i2c_write_byte(led->client, i, PWM_64_table[j]);
				if (led->mode != 1)
					return;
			}

			for (i = 0x03; i <= 0x48; i = i + 3) {
				/* PWM */
				i2c_write_byte(led->client, i, PWM_64_table[j]);
				if (led->mode != 1)
					return;
			}
			mdelay(10);
		}

		for (j = 0; j <= 127; j++) {
			for (i = 0x02; i <= 0x48; i = i + 3) {
				/* PWM */
				i2c_write_byte(led->client, i, PWM_64_table[j]);
				if (led->mode != 1)
					return;
			}

			for (i = 0x03; i <= 0x48; i = i + 3) {
				/* PWM */
				i2c_write_byte(led->client, i, PWM_64_table[j]);
				if (led->mode != 1)
					return;
			}
			mdelay(10);
		}
	}
}

static void led_mode_2(struct led_3746a_dev *led)
{
	u8 com1;
	u8 x = 128;
	u8 y = 64;
	u8 z = 8;
	u8 i;

	init_3746a(led);

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x00);

	for (i = 0; i <= 0x48; i++)
		i2c_write_byte(led->client, i, 0x00); /* PWM */

	while ((led->mode == 2) && led->on) {
		for (com1 = 0; com1 < 195; com1++) {
			u8 addr = 0x01;
			for (i = 0; i <= 11; i++) {
				i2c_write_byte(led->client, addr,
					PWM_RGB[com1 + z * i]);
				addr++;
				i2c_write_byte(led->client, addr,
					PWM_RGB[com1 + x + z * i]);
				addr++;
				i2c_write_byte(led->client, addr,
					PWM_RGB[com1 + y + z * i]);
				addr++;
			}

			for (i = 0; i <= 11; i++) {
				i2c_write_byte(led->client, addr,
					PWM_RGB[com1 + z * (11 - i)]);
				addr++;
				i2c_write_byte(led->client, addr,
					PWM_RGB[com1 + x + z * (11 - i)]);
				addr++;
				i2c_write_byte(led->client, addr,
					PWM_RGB[com1 + y + z * (11 - i)]);
				addr++;
			}

			mdelay(20);
			if (led->mode != 2)
				return;
		}
	}
}

static void led_mode_3(struct led_3746a_dev *led)
{
	u8 i, j, k;

	init_3746a(led);

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x00);

	for (i = 0x01; i <= 0x48; i++)
		i2c_write_byte(led->client, i, 0x00);

	while ((led->mode == 3) && led->on) {
		k = 0x3f;
		for (i = 0x01; i <= 0x3f; i++) {
			for (j = 0x01; j <= 0x22; j = j + 3) {
				i2c_write_byte(led->client, j, a[i]);
				i2c_write_byte(led->client, j + 0x26, a[i]);
				i2c_write_byte(led->client, j + 1, a[k]);
				i2c_write_byte(led->client, j + 0x24, a[k]);

				if (led->mode != 3)
					return;
			}

			mdelay(100);
			k--;
			if (led->mode != 3)
				return;
		}

		k = 0x3f;
		for (i = 0x01; i <= 0x3f; i++) {
			for (j = 0x01; j <= 0x22; j = j + 3) {
				i2c_write_byte(led->client, j + 2, a[i]);
				i2c_write_byte(led->client, j + 0x25, a[i]);
				i2c_write_byte(led->client, j, a[k]);
				i2c_write_byte(led->client, j + 0x26, a[k]);

				if (led->mode != 3)
					return;
			}

			mdelay(100);
			k--;
			if (led->mode != 3)
				return;
		}

		k = 0x3f;
		for (i = 0x01; i <= 0x3f; i++) {
			for (j = 0x01; j <= 0x22; j = j + 3) {
				i2c_write_byte(led->client, j + 1, a[i]);
				i2c_write_byte(led->client, j + 0x24, a[i]);
				i2c_write_byte(led->client, j + 2, a[k]);
				i2c_write_byte(led->client, j + 0x25, a[k]);
				if (led->mode != 3)
					return;
			}

			mdelay(100);
			k--;
			if (led->mode != 3)
				return;
		}
	}
}

static void all_led_on_off(struct led_3746a_dev *led)
{
	u8 i;

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x01);
	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x01);

	if (data[5] > 0) {
		for (i = 0; i <= 0xc6; i++) {
			/* Scaling */
			i2c_write_byte(led->client, i, data[6]);
			i2c_write_byte(led->client, i, data[6]);
		}
	} else {
		for (i = 0; i <= 0xc6; i++) {
			/* Scaling */
			i2c_write_byte(led->client, i, 0x00);
			i2c_write_byte(led->client, i, 0X00);
		}
	}

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x00);
	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x00);

	for (i = 0; i <= 0xc6; i++) {
		/* PWM */
		i2c_write_byte(led->client, i, data[7]);
		i2c_write_byte(led->client, i, data[7]);
	}

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x02);
	/* GCC */
	i2c_write_byte(led->client, 0x01, data[8]);
	i2c_write_byte(led->client, 0x00, 0x01);
	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x02);
	/* GCC */
	i2c_write_byte(led->client, 0x01, data[8]);
	i2c_write_byte(led->client, 0x00, 0x01);
}

static void open_short(struct led_3746a_dev *led)
{
	u8 i;

	if (!led->on)
		return;

	i2c_write_byte(led->client, 0xFE, 0xC5);
	i2c_write_byte(led->client, 0xFD, 0x00);

	for (i = 0x01; i <= 0xc6; i++)
		i2c_write_byte(led->client, i, 0xFF);

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x01);
	for (i = 0x01; i <= 0xc6; i++)
		i2c_write_byte(led->client, i, 0xFF);

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x02);
	i2c_write_byte(led->client, 0x02, 0x00);
	i2c_write_byte(led->client, 0x01, 0xff);
	i2c_write_byte(led->client, 0x00, 0x00);
	i2c_write_byte(led->client, 0x00, 0x03);
}

static void led_mode_breath(struct led_3746a_dev *led)
{
	if (!led->on)
		return;

	init_3746a(led);

	i2c_write_byte(led->client, 0x00, 0x01);
	i2c_write_byte(led->client, 0x03, 0x70);
	i2c_write_byte(led->client, 0x06, 0x10);

	i2c_write_byte(led->client, 0x1a, 0x81);
	i2c_write_byte(led->client, 0x1b, 0x81);
	i2c_write_byte(led->client, 0x1c, 0x81);

	i2c_write_byte(led->client, 0x1d, 0x01);
	i2c_write_byte(led->client, 0x1e, 0x01);
	i2c_write_byte(led->client, 0x1f, 0x01);

	i2c_write_byte(led->client, 0x20, 0x01);
	i2c_write_byte(led->client, 0x21, 0x01);
	i2c_write_byte(led->client, 0x22, 0x01);

	i2c_write_byte(led->client, 0x23, 0x01);
	i2c_write_byte(led->client, 0x24, 0x01);
	i2c_write_byte(led->client, 0x25, 0x01);

	i2c_write_byte(led->client, 0x26, 0xff);

	i2c_write_byte(led->client, 0x07, 252);
	i2c_write_byte(led->client, 0x08, 124);
	i2c_write_byte(led->client, 0x09, 0);

	i2c_write_byte(led->client, 0x0a, 35);
	i2c_write_byte(led->client, 0x0b, 229);
	i2c_write_byte(led->client, 0x0c, 220);

	i2c_write_byte(led->client, 0x0d, 234);
	i2c_write_byte(led->client, 0x0e, 25);
	i2c_write_byte(led->client, 0x0f, 239);

	i2c_write_byte(led->client, 0x10, 0xff);
}


static void led_mode_show(struct led_3746a_dev *led)
{
	switch (led->mode) {
	case 0:
		all_led_on_off(led);
		break;
	case 1:
		led_mode_1(led);
		break;
	case 2:
		led_mode_2(led);
		break;
	case 3:
		led_mode_3(led);
		break;
	case 4:
		open_short(led);
		break;
	case 5:
		led_mode_breath(led);
		break;
	default:
		dev_warn(&led->client->dev, "unknown mode: %d\n",
			led->mode);
	}
}

static ssize_t led3746a_show_mode(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct led_3746a_dev *led = i2c_get_clientdata(to_i2c_client(dev));
	int ret;

	ret = sprintf(buf, "%d\n", led->mode);
	return ret;
}

static ssize_t led3746a_store_mode(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct led_3746a_dev *led = i2c_get_clientdata(to_i2c_client(dev));

	if (!count)
		return -EINVAL;

	if (!strncmp(buf, "0", 1)) {
		led->mode = 0;
		schedule_work(&led->work);
	} else if (!strncmp(buf, "1", 1)) {
		led->mode = 1;
		schedule_work(&led->work);
	} else if (!strncmp(buf, "2", 1)) {
		led->mode = 2;
		schedule_work(&led->work);
	} else if (!strncmp(buf, "3", 1)) {
		led->mode = 3;
		schedule_work(&led->work);
	} else if (!strncmp(buf, "4", 1)) {
		led->mode = 4;
		schedule_work(&led->work);
	} else if (!strncmp(buf, "5", 1)) {
		led->mode = 5;
		schedule_work(&led->work);
	} else {
		dev_warn(&led->client->dev, "unknown mode: %s\n",
			buf);
		if (!count)
			return -EINVAL;
	}

	return count;
}

static ssize_t led3746a_show_on_status(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct led_3746a_dev *led = i2c_get_clientdata(to_i2c_client(dev));
	int ret;

	ret = sprintf(buf, "%d\n", led->on);
	return ret;
}

static ssize_t led3746a_store_on_status(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct led_3746a_dev *led = i2c_get_clientdata(to_i2c_client(dev));

	if (!count)
		return -EINVAL;

	if (!strncmp(buf, "0", 1)) {
		led->on = 0;
		schedule_work(&led->work);
	} else if (!strncmp(buf, "1", 1)) {
		led->on = 1;
		schedule_work(&led->work);
	} else {
		dev_warn(&led->client->dev, "unknown mode: %s\n",
			buf);
		if (!count)
			return -EINVAL;
	}

	return count;
}

static struct device_attribute led3746a_mode_attr = {
	.attr = {
		.name = "mode",
		.mode = 0644,
	},
	.show = led3746a_show_mode,
	.store = led3746a_store_mode,
};

static struct device_attribute led3746a_on_status_attr = {
	.attr = {
		.name = "on",
		.mode = 0644,
	},
	.show = led3746a_show_on_status,
	.store = led3746a_store_on_status,
};


static struct device_attribute *led3746a_attributes[] = {
	&led3746a_mode_attr,
	&led3746a_on_status_attr,
};


#ifdef CONFIG_OF
static int led_parse_dt(struct device *dev, struct led_3746a_dev *led)
{
	int ret;
	struct device_node *np = dev->of_node;

	led->enable_pin = of_get_named_gpio(np, "enable-gpios", 0);
	if (!gpio_is_valid(led->enable_pin)) {
		dev_err(dev, "No valid enable gpio pin");
	} else {
		ret = gpio_request(led->enable_pin, "led_3746a_enable");
		if (ret) {
			dev_err(dev, "request enable pin failed: %d\n", ret);
			return ret;
		}

		/* high level enable */
		gpio_direction_output(led->enable_pin, 1);
		mdelay(1000);
	}

	return 0;
}
#endif

static void led_3746a_work(struct work_struct *work)
{
	struct led_3746a_dev *led = container_of(work,
				struct led_3746a_dev, work);

	if (led->on)
		led_mode_show(led);
	else
		all_led_on_off(led);
}

/*
static void led_brightness_set(struct led_classdev *led_cdev,
					  enum led_brightness brightness)
{
	struct led_3746a_dev *led = container_of(led_cdev,
				struct led_3746a_dev, cdev_led);

	schedule_work(&led->work);
}
*/

static int led_3746a_register(struct led_3746a_dev *led)
{
	int ret;

	INIT_WORK(&led->work, led_3746a_work);

	led->cdev_led.name = "led_3746a";
	/* led->cdev_led.brightness = LED_OFF; */
	/* led->cdev_led.brightness_set = led_brightness_set; */

	ret = led_classdev_register(&led->client->dev, &led->cdev_led);
	if (ret < 0) {
		dev_err(&led->client->dev, "couldn't register LED %s\n",
							led->cdev_led.name);
		return ret;
	}

	return 0;
}

static void led_3746a_unregister(struct led_3746a_dev *led)
{
	cancel_work_sync(&led->work);
	led_classdev_unregister(&led->cdev_led);
}

static void init_3746a(struct led_3746a_dev *led)
{
	u8 i;

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x01);
	i2c_write_byte(led->client, 0x51, 0xff); /* GCC */
	i2c_write_byte(led->client, 0x50, 0x01);

	for (i = 0; i <= 0x48; i++)
		i2c_write_byte(led->client, i, 0xff); /* Scaling */

	i2c_write_byte(led->client, 0xfe, 0xc5);
	i2c_write_byte(led->client, 0xfD, 0x00);

	for (i = 0; i <= 0x48; i++)
		i2c_write_byte(led->client, i, 0x00); /* PWM */
}


static int led_3746a_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct led_3746a_dev *led;
	int ret, i;

	dev_info(&client->dev, "LED 3746a Address: 0x%02x\n", client->addr);

	led = devm_kzalloc(&client->dev, sizeof(struct led_3746a_dev),
				GFP_KERNEL);
	if (!led) {
		dev_err(&client->dev, "failed to allocate driver data\n");
		return -ENOMEM;
	}

	led->mode = 5;
	led->on = 0;
	led->client = client;
	i2c_set_clientdata(client, led);

#ifdef CONFIG_OF
	/* get enable pin */
	if (client->dev.of_node) {
		ret = led_parse_dt(&client->dev, led);
		if (ret) {
			dev_err(&client->dev, "led_parse_dt err: %d\n", ret);
			return ret;
		}
	}
#endif

	/* add file node to handle led */
	for (i = 0; i < ARRAY_SIZE(led3746a_attributes); i++) {
		ret = device_create_file(&led->client->dev,
						led3746a_attributes[i]);
		if (ret) {
			dev_err(&led->client->dev, "failed: sysfs file %s\n",
					led3746a_attributes[i]->attr.name);
			goto failed_unregister_dev_file;
		}
	}

	ret = led_3746a_register(led);
	if (ret) {
		dev_err(&client->dev, "led_3746a_register err: %d\n", ret);
		goto failed_unregister_dev_file;
	}

	init_3746a(led);
	return 0;

failed_unregister_dev_file:
	for (i--; i >= 0; i--)
		device_remove_file(&led->client->dev, led3746a_attributes[i]);
	return ret;

}

static int led_3746a_remove(struct i2c_client *client)
{
	struct led_3746a_dev *led = i2c_get_clientdata(client);
	led_3746a_unregister(led);
	gpio_free(led->enable_pin);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id led_match_table[] = {
	{.compatible = "led,3746a",},
	{ },
};
#endif

static const struct i2c_device_id led_3746a_id[] = {
	{ "led_3746a", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, led_3746a_id);

static struct i2c_driver led_3746a_driver = {
	.driver = {
		.name	= "led_3746a",
#ifdef CONFIG_OF
		.of_match_table = led_match_table,
#endif
	},
	.probe		= led_3746a_probe,
	.remove		= led_3746a_remove,
	.id_table	= led_3746a_id,
};

module_i2c_driver(led_3746a_driver);


MODULE_DESCRIPTION("IS31FL        3746a LED Driver");
MODULE_LICENSE("GPL v2");


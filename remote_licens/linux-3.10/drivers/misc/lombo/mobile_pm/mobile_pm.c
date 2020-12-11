/*
 * mobile.c - Lombo mobile network module power driver
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/of.h>
#include <linux/rfkill.h>
#include <linux/of_gpio.h>
#include <linux/debugfs.h>

#include <mach/debug.h>

#define RESET_PIN	"fg_reset"
#define PWK_PIN		"fg_pwk_n"

struct mobile_pin_info {
	int num;
	enum of_gpio_flags flag;
};

struct mobile_pin_group {
	struct mobile_pin_info reset;
	struct mobile_pin_info pwk;
};

struct mobile_pin_group fg_pin;

static ssize_t show_reset_active(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	int active_low;
	int err;

	if (fg_pin.reset.num < 0) {
		PRT_INFO("%s property not defined.\n", RESET_PIN);
		return -ENODEV;
	}

	active_low = fg_pin.reset.flag & OF_GPIO_ACTIVE_LOW;
	if (active_low)
		err = gpio_direction_output(fg_pin.reset.num, 1);
	else
		err = gpio_direction_output(fg_pin.reset.num, 0);

	return err;
}

static ssize_t show_reset_invalid(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	int active_low;
	int err;

	if (fg_pin.reset.num < 0) {
		PRT_INFO("%s property not defined.\n", RESET_PIN);
		return -ENODEV;
	}

	active_low = fg_pin.reset.flag & OF_GPIO_ACTIVE_LOW;
	if (active_low)
		err = gpio_direction_output(fg_pin.reset.num, 0);
	else
		err = gpio_direction_output(fg_pin.reset.num, 1);

	return err;
}

static ssize_t show_pwk_active(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int active_low;
	int err;

	if (fg_pin.pwk.num < 0) {
		PRT_INFO("%s property not defined.\n", PWK_PIN);
		return -ENODEV;
	}

	active_low = fg_pin.pwk.flag & OF_GPIO_ACTIVE_LOW;
	if (active_low)
		err = gpio_direction_output(fg_pin.pwk.num, 1);
	else
		err = gpio_direction_output(fg_pin.pwk.num, 0);

	return err;
}

static ssize_t show_pwk_invalid(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int active_low;
	int err;

	if (fg_pin.pwk.num < 0) {
		PRT_INFO("%s property not defined.\n", PWK_PIN);
		return -ENODEV;
	}

	active_low = fg_pin.pwk.flag & OF_GPIO_ACTIVE_LOW;
	if (active_low)
		err = gpio_direction_output(fg_pin.pwk.num, 0);
	else
		err = gpio_direction_output(fg_pin.pwk.num, 1);

	return err;
}

static struct device_attribute fg_attrs[] = {
	__ATTR(reset_active, S_IRUGO, show_reset_active, NULL),
	__ATTR(reset_invalid, S_IRUGO, show_reset_invalid, NULL),
	__ATTR(pwk_active, S_IRUGO, show_pwk_active, NULL),
	__ATTR(pwk_invalid, S_IRUGO, show_pwk_invalid, NULL),
	__ATTR_NULL
};

static void lombo_mobile_release(struct device *dev)
{
	PRT_INFO("%s enter.\n", __func__);
}

static struct class fg_devclass = {
	.name = "mobile_pm",
	.dev_attrs = fg_attrs,
	.dev_release = lombo_mobile_release,
};

static int lombo_mobile_create_gpio(struct device *dev, struct device_node *np,
				  const char *label, enum of_gpio_flags *flag)
{
	int ret = 0;
	int num;
	int active_low;

	num = of_get_named_gpio_flags(np, label, 0, flag);
	if (num < 0) {
		PRT_INFO("%s property not found.\n", label);
		return -1;
	}

	if (!gpio_is_valid(num)) {
		PRT_ERR("gpio %d is invalid.\n", num);
		return -1;
	}

	active_low = *flag & OF_GPIO_ACTIVE_LOW;

	ret = devm_gpio_request(dev, num, label);
	if (ret < 0) {
		PRT_ERR("request gpio %d failed.\n", num);
		return -1;
	}

	if (active_low)
		ret = gpio_direction_output(num, 0);
	else
		ret = gpio_direction_output(num, 1);
	if (ret < 0) {
		PRT_ERR("set %d output failed.\n", num);
		return -1;
	}

	return num;
}

static int lombo_mobile_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device *pin_dev;
	struct device_node *np = pdev->dev.of_node;

	/* register test class */
	ret = class_register(&fg_devclass);
	if (ret) {
		PRT_ERR("4g class register failed\n");
		return ret;
	}

	pin_dev = kzalloc(sizeof(struct device), GFP_KERNEL);
	if (pin_dev == NULL) {
		PRT_ERR("alloc pin device failed\n");
		return ret;
	}

	pin_dev->class = &fg_devclass;
	pin_dev->parent = &pdev->dev;
	dev_set_name(pin_dev, "fg_pin");
	ret = device_register(pin_dev);
	if (ret) {
		PRT_ERR("register 4g_pin device failed\n");
		return ret;
	}

	fg_pin.reset.num = lombo_mobile_create_gpio(&pdev->dev, np, RESET_PIN,
						&fg_pin.reset.flag);

	fg_pin.pwk.num = lombo_mobile_create_gpio(&pdev->dev, np, PWK_PIN,
						&fg_pin.pwk.flag);

	PRT_INFO("%s ok.\n", __func__);

	return ret;
}

static int lombo_mobile_remove(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_mobile_dt_match[] = {
	{ .compatible = "lombo,mobile_pm" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_mobile_dt_match);
#endif

static struct platform_driver lombo_mobile_driver = {
	.probe		= lombo_mobile_probe,
	.remove		= lombo_mobile_remove,
	.driver		= {
		.name	= "lombo-fg",
		.owner	= THIS_MODULE,
		#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(lombo_mobile_dt_match),
		#endif
	},
};
module_platform_driver(lombo_mobile_driver);

MODULE_DESCRIPTION("LomboTech mobile module power driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");

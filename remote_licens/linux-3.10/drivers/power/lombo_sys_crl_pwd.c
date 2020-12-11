/*
 * sys_crl_pwd.c - Driver of LomboTech Watchdog
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

#define DEBUG

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/syscore_ops.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/clk.h>

#include <mach/debug.h>

#define SYS_CRL_5V	"sys_crl_5v"

static int lombo_sys_crl_pwd_probe(struct platform_device *pdev)
{
	int err = 0;
	int ret, num;
	struct device_node *np;

	dev_info(&pdev->dev, "[sys-crl-pwd]: lombo_sys_crl_pwd_probe\n");

	np = pdev->dev.of_node;
	if (!np) {
		dev_err(&pdev->dev, "err: device node is null\n");
		return -ENODEV;
	}

	/* need to set up sys crl 5v */
	num = of_get_named_gpio(np, SYS_CRL_5V, 0);
	if (num >= 0) {
		dev_info(&pdev->dev, "sys crl 5v %d set\n", num);
		ret = devm_gpio_request(&pdev->dev, num, SYS_CRL_5V);
		if (ret < 0) {
			dev_err(&pdev->dev, "sys crl 5v request gpio err\n");
			return -EBUSY;
		}
		ret = gpio_direction_output(num, 1);
		if (ret < 0) {
			dev_err(&pdev->dev, "sys crl 5v set gpio direction err\n");
			return -EBUSY;
		}

		gpio_set_value(num, 1);
	}

	return err;
}

static int lombo_sys_crl_pwd_remove(struct platform_device *pdev)
{
	int ret, num;
	struct device_node *np;

	dev_info(&pdev->dev, "[sys-crl-pwd]: lombo_sys_crl_pwd_remove\n");

	np = pdev->dev.of_node;
	if (!np) {
		dev_err(&pdev->dev, "err: device node is null\n");
		return -ENODEV;
	}

	/* need to set up sys crl 5v */
	num = of_get_named_gpio(np, SYS_CRL_5V, 0);
	if (num >= 0) {
		dev_info(&pdev->dev, "sys crl 5v %d set\n", num);
		gpio_set_value(num, 0);
		ret = gpio_direction_input(num);
		if (ret < 0) {
			dev_err(&pdev->dev, "sys crl 5v set gpio direction err\n");
			return -EBUSY;
		}

		devm_gpio_free(&pdev->dev, (unsigned int)num);
	}

	return 0;
}

static const struct of_device_id lombo_sys_crl_pwd_match[] = {
	{ .compatible = "lombo,sys-crl-pwd" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_sys_crl_pwd_match);

static struct platform_driver lombo_sys_crl_pwd_driver = {
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "lombo-sys-crl",
		.of_match_table	= lombo_sys_crl_pwd_match,
	},
	.probe		= lombo_sys_crl_pwd_probe,
	.remove		= lombo_sys_crl_pwd_remove,
};

module_platform_driver(lombo_sys_crl_pwd_driver);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("System Power Control for Lombo SoCs");
MODULE_LICENSE("GPL");

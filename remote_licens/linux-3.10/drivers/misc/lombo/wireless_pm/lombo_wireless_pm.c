/*
 * lombo_wireless_pm.c - Lombo wireless pm driver
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
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/sched.h>

#include <mach/debug.h>
#include <mach/lombo_wireless_pm.h>

/**
 * struct power_info - information of power on control pin.
 * @power_on: power on control gpio number.
 * @flag: flag of level validity.
 * @on_flag: flag of power on.
 * @off_flag: flag of power off.
 */
struct power_info {
	int power_on;
	enum of_gpio_flags flag;
	int on_flag;
	int off_flag;
};

/**
 * struct enabled_info - information of enabled pin.
 * @reg_on: enabled gpio number.
 * @flag: flag of level validity.
 */
struct enabled_info {
	int reg_on;
	enum of_gpio_flags flag;
};

/**
 * struct wireless_pm_info - power manager information of wireless module.
 * @wifi_power_info: power information of wifi.
 * @bt_power_info: power information of bt.
 * @wifi_enabled_info: enabled information of wifi.
 * @bt_enabled_info: enabled information of bluetooth.
 */
struct wireless_pm_info {
	struct power_info wifi_power_info;
	struct power_info bt_power_info;
	struct enabled_info wifi_enabled_info;
	struct enabled_info bt_enabled_info;
};

static struct wireless_pm_info info = {{-1, 0, 0, 1}, {-1, 0, 0, 1},
	{-1, 0}, {-1, 0} };

/**
 * power_pin_set_value - set power control pin level.
 * @info: power information of wireless module.
 * @pwr_type: type of power on.
 *
 */
static void power_pin_set_value(struct power_info *info,
		enum power_type pwr_type)
{
	int active_low;

	active_low = info->flag & OF_GPIO_ACTIVE_LOW;
	if (pwr_type == PWR_ON) {
		if (active_low)
			gpio_set_value(info->power_on, 1);
		else
			gpio_set_value(info->power_on, 0);
	} else {
		if (active_low)
			gpio_set_value(info->power_on, 0);
		else
			gpio_set_value(info->power_on, 1);
	}
}

/**
 * lombo_wireless_set_power - set power control pin state.
 * @wl_type: type of wireless module.
 * @pwr_type: type of power on.
 *
 * return 0 if success; otherwise failed.
 */
int lombo_wireless_set_power(enum wireless_type wl_type,
		enum power_type pwr_type)
{
	int ret = 0;

	switch (wl_type) {
	case WL_WIFI:
		if (info.wifi_power_info.power_on > 0 ||
				info.wifi_power_info.power_on == 0) {
#ifndef CONFIG_COMBO_MODULE
			power_pin_set_value(&(info.wifi_power_info), pwr_type);
			PRT_DBG("set wifi power control pin successed\n");
#else
			if (pwr_type == PWR_ON) {
				info.wifi_power_info.on_flag = 1;
				info.wifi_power_info.off_flag = 0;

				if (!info.bt_power_info.on_flag) {
					power_pin_set_value(
					&(info.wifi_power_info), pwr_type);
					PRT_DBG("wifi power is on!\n");
				} else {
					PRT_WARN
				("wifi power_on has been handled!\n");
				}
			} else {
				info.wifi_power_info.on_flag = 0;
				info.wifi_power_info.off_flag = 1;

				if ((info.wifi_power_info.off_flag &&
					info.bt_power_info.off_flag)) {
					power_pin_set_value(
					&(info.wifi_power_info), pwr_type);
					PRT_DBG("wifi power is off!\n");
				} else {
					PRT_WARN("bt is still working!\n");
				}
			}
#endif
		} else {
			ret = -1;
			PRT_WARN("wifi power control pin is not configured.\n");
		}
		break;

	case WL_BT:
#ifndef CONFIG_COMBO_MODULE
		if (info.bt_power_info.power_on > 0 ||
				info.bt_power_info.power_on == 0) {
			power_pin_set_value(&(info.bt_power_info), pwr_type);
			PRT_DBG("set bt power control pin successed\n");
#else
		/* For power supply control of combo module, wifi and bt share
		 * wifi_power attribute node for processing */
		if (info.wifi_power_info.power_on > 0 ||
			info.wifi_power_info.power_on == 0) {
			if (pwr_type == PWR_ON) {
				info.bt_power_info.on_flag = 1;
				info.bt_power_info.off_flag = 0;

				if (!info.wifi_power_info.on_flag) {
					power_pin_set_value(
					&(info.wifi_power_info), pwr_type);
					PRT_DBG("bt power is on!\n");
				} else {
					PRT_WARN
					("bt power_on has been handled!\n");
				}
			} else {
				info.bt_power_info.on_flag = 0;
				info.bt_power_info.off_flag = 1;

				if ((info.wifi_power_info.off_flag &&
					info.bt_power_info.off_flag)) {
					power_pin_set_value(
					&(info.wifi_power_info), pwr_type);
					PRT_DBG("bt power is off!\n");
				} else {
					PRT_WARN("wifi is still working!\n");
				}
			}
#endif
		} else {
			ret = -1;
			PRT_WARN("bt power control pin is not configured.\n");
		}
		break;

	default:
		PRT_WARN("invalid wireless type\n");
	}

	return 0;
}
EXPORT_SYMBOL(lombo_wireless_set_power);

/**
 * enable_pin_set_value - set enabled control pin level.
 * @info: enabled information of wireless module.
 * @su_type: type of startup.
 *
 */
static void enable_pin_set_value(struct enabled_info *info,
		enum startup_type su_type)
{
	int active_low;

	active_low = info->flag & OF_GPIO_ACTIVE_LOW;
	if (su_type == ENABLED) {
		if (active_low)
			gpio_set_value(info->reg_on, 1);
		else
			gpio_set_value(info->reg_on, 0);
	} else {
		if (active_low)
			gpio_set_value(info->reg_on, 0);
		else
			gpio_set_value(info->reg_on, 1);
	}
}

/**
 * lombo_wireless_set_enable - set enabled state of wireless state.
 * @wl_type: type of wireless module.
 * @su_type: type of startup.
 *
 * return 0 if success; otherwise failed.
 */
int lombo_wireless_set_enable(enum wireless_type wl_type,
		enum startup_type su_type)
{
	int ret = 0;

	switch (wl_type) {
	case WL_WIFI:
		if (info.wifi_enabled_info.reg_on > 0 ||
				info.wifi_enabled_info.reg_on == 0) {
			enable_pin_set_value(
			&(info.wifi_enabled_info), su_type);
			PRT_DBG("set wifi enabled pin successed\n");
		} else {
			ret = -1;
			PRT_WARN("wifi enabled pin is not configured.\n");
		}
		break;

	case WL_BT:
		if (info.bt_enabled_info.reg_on > 0 ||
				info.bt_enabled_info.reg_on == 0) {
			enable_pin_set_value(&(info.bt_enabled_info), su_type);
			PRT_DBG("set bt enabled pin successed\n");
		} else {
			ret = -1;
			PRT_WARN("bt enabled pin is not configured.\n");
		}
		break;

	default:
		PRT_WARN("invalid wireless type\n");
	}

	return 0;
}
EXPORT_SYMBOL(lombo_wireless_set_enable);

/**
 * configure_control_pin - configure control pin of wireless module.
 * @pdev: pointer to platform device struct.
 * @gpio_num: gpio number.
 * @property: property of device tree node.
 *
 * return 0 if success; otherwise failed.
 */
static int configure_control_pin(struct platform_device *pdev,
		int *gpio_num, enum of_gpio_flags *flag, const char *property)
{
	int ret = 0;

	*gpio_num = of_get_named_gpio_flags(pdev->dev.of_node,
			property, 0, flag);
	if (*gpio_num < 0) {
		PRT_INFO("%s property not found.\n", property);
		return -1;
	}

	if (!gpio_is_valid(*gpio_num)) {
		PRT_ERR("gpio %d is invalid.\n", *gpio_num);
		return -1;
	}

	ret = devm_gpio_request(&pdev->dev, *gpio_num, property);
	if (ret < 0) {
		PRT_ERR("request gpio %d failed.\n", *gpio_num);
		return -1;
	}

	if (*flag & OF_GPIO_ACTIVE_LOW)
		ret = gpio_direction_output(*gpio_num, 0);
	else
		ret = gpio_direction_output(*gpio_num, 1);

	if (ret < 0) {
		PRT_ERR("set %d output failed.\n", *gpio_num);
		return -1;
	}

	return 0;
}

/**
 * lombo_wireless_pm_probe - probe entry point of the driver.
 * @pdev: pointer to platform device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_wireless_pm_probe(struct platform_device *pdev)
{
	int ret = 0;

	ret = configure_control_pin(pdev, &(info.wifi_power_info.power_on),
			&(info.wifi_power_info.flag), "wifi_power");
	if (ret == 0) {
		PRT_DBG("configure wifi power control pin successed!\n");
	} else {
		PRT_INFO("configure wifi power control pin failed!\n");
	}

	ret = configure_control_pin(pdev, &(info.bt_power_info.power_on),
			&(info.bt_power_info.flag), "bt_power");
	if (ret == 0) {
		PRT_DBG("configure bt power control pin successed!\n");
	} else {
		PRT_INFO("configure bt power control pin failed!\n");
	}

	ret = configure_control_pin(pdev, &(info.wifi_enabled_info.reg_on),
			&(info.wifi_enabled_info.flag), "wifi_enable");
	if (ret == 0) {
		PRT_DBG("configure wifi enable pin successed!\n");
	} else {
		PRT_INFO("configure wifi enable pin failed!\n");
	}

	ret = configure_control_pin(pdev, &(info.bt_enabled_info.reg_on),
			&(info.bt_enabled_info.flag), "bt_enable");
	if (ret == 0) {
		PRT_DBG("configure bt enable pin successed!\n");
	} else {
		PRT_INFO("configure bt enable pin failed!\n");
	}

	return 0;
}

/**
 * lombo_wireless_pm_remove - remove entry point of the driver.
 * @pdev: pointer to platform device struct.
 *
 * return 0 if success; otherwise failed.
 */

static int lombo_wireless_pm_remove(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_wireless_pm_dt_match[] = {
	{ .compatible = "lombo,wireless-pm" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_wireless_pm_dt_match);
#endif

static struct platform_driver lombo_wireless_pm_driver = {
	.probe		= lombo_wireless_pm_probe,
	.remove		= lombo_wireless_pm_remove,
	.driver		= {
		.name	= "lombo-wireless-pm",
		.owner	= THIS_MODULE,
		#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(lombo_wireless_pm_dt_match),
		#endif
	},
};
module_platform_driver(lombo_wireless_pm_driver);

MODULE_DESCRIPTION("LomboTech wireless pm driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");

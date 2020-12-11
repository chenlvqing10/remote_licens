/*
 * lombo_thermal.c - Lombo Thermal Management Implementation
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

#include <linux/cpu_cooling.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/platform_data/lombo_thermal.h>
#include <linux/thermal.h>
#include "lombo_thermal.h"

/* Local function to check if thermal zone matches cooling devices */
static int lombo_thermal_match_cdev(struct thermal_cooling_device *cdev,
		struct lombo_trip_point *trip_point)
{
	int i;

	if (!strlen(cdev->type))
		return -EINVAL;

	for (i = 0; i < COOLING_DEV_MAX; i++) {
		if (!strcmp(trip_point->cdev_name[i], cdev->type))
			return 0;
	}

	return -ENODEV;
}

/* Callback to bind cooling device to thermal zone */
static int lombo_cdev_bind(struct thermal_zone_device *thermal,
		struct thermal_cooling_device *cdev)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;
	struct lombo_thsens_platform_data *ptrips = pzone->trip_tab;
	unsigned long max_state, upper, lower;
	int i, err = 0;
	struct device_node *np = cdev->np;
	char tmp_para[16] = {0};

	cdev->ops->get_max_state(cdev, &max_state);

	for (i = 0; i < ptrips->num_trips; i++) {
		if (lombo_thermal_match_cdev(cdev, &ptrips->trip_points[i]))
			continue;

		upper = lower = i > max_state ? max_state : i;

		snprintf(tmp_para, sizeof(tmp_para), "trip%d-lower", i);
		err = of_property_read_u32(np, tmp_para, (u32 *)&lower);
		if (err) {
			PRT_ERR("failed to get trip%d-lower\n", i);
			err = __LINE__;
			goto out;
		}

		snprintf(tmp_para, sizeof(tmp_para), "trip%d-upper", i);
		err = of_property_read_u32(np, tmp_para, (u32 *)&upper);
		if (err) {
			PRT_ERR("failed to get trip%d-upper\n", i);
			err = __LINE__;
			goto out;
		}

		err = thermal_zone_bind_cooling_device(thermal, i, cdev,
			upper, lower);

		PRT_INFO("%s bind to %d: %d-%s\n",
				cdev->type, i, err, err ? "fail" : "succeed");
	}

out:
	return err;
}

/* Callback to unbind cooling device from thermal zone */
static int lombo_cdev_unbind(struct thermal_zone_device *thermal,
		struct thermal_cooling_device *cdev)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;
	struct lombo_thsens_platform_data *ptrips = pzone->trip_tab;
	int i, ret = -EINVAL;

	for (i = 0; i < ptrips->num_trips; i++) {
		if (lombo_thermal_match_cdev(cdev, &ptrips->trip_points[i]))
			continue;

		ret = thermal_zone_unbind_cooling_device(thermal, i, cdev);

		PRT_INFO("%s unbind from %d: %s\n",
				cdev->type, i, ret ? "fail" : "succeed");
	}

	return ret;
}

/* Callback to get current temperature */
static int lombo_sys_get_temp(struct thermal_zone_device *thermal,
		int *temp)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;

	*temp = csp_sensor_get_temp(pzone->thermal_base, pzone->id);
	return 0;
}

/* Callback to get thermal zone mode */
static int lombo_sys_get_mode(struct thermal_zone_device *thermal,
		enum thermal_device_mode *mode)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;

	mutex_lock(&pzone->th_lock);
	*mode = pzone->mode;
	mutex_unlock(&pzone->th_lock);

	return 0;
}

/* Callback to set thermal zone mode */
static int lombo_sys_set_mode(struct thermal_zone_device *thermal,
		enum thermal_device_mode mode)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;

	mutex_lock(&pzone->th_lock);
	pzone->mode = mode;
	mutex_unlock(&pzone->th_lock);

	return 0;
}

/* Callback to get trip point type */
static int lombo_sys_get_trip_type(struct thermal_zone_device *thermal,
		int trip, enum thermal_trip_type *type)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;
	struct lombo_thsens_platform_data *ptrips = pzone->trip_tab;

	if (trip >= ptrips->num_trips)
		return -EINVAL;

	*type = ptrips->trip_points[trip].type;

	return 0;
}

/* Callback to get trip point temperature */
static int lombo_sys_get_trip_temp(struct thermal_zone_device *thermal,
		int trip, int *temp)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;
	struct lombo_thsens_platform_data *ptrips = pzone->trip_tab;

	if (trip >= ptrips->num_trips)
		return -EINVAL;

	*temp = ptrips->trip_points[trip].temp;

	return 0;
}

/* Callback to get critical trip point temperature */
static int lombo_sys_get_crit_temp(struct thermal_zone_device *thermal,
		int *temp)
{
	struct lombo_thermal_zone *pzone = thermal->devdata;
	struct lombo_thsens_platform_data *ptrips = pzone->trip_tab;
	int i;

	for (i = ptrips->num_trips - 1; i > 0; i--) {
		if (ptrips->trip_points[i].type == THERMAL_TRIP_CRITICAL) {
			*temp = ptrips->trip_points[i].temp;
			return 0;
		}
	}

	return -EINVAL;
}

static struct thermal_zone_device_ops thdev_ops = {
	.bind = lombo_cdev_bind,
	.unbind = lombo_cdev_unbind,
	.get_temp = lombo_sys_get_temp,
	.get_mode = lombo_sys_get_mode,
	.set_mode = lombo_sys_set_mode,
	.get_trip_type = lombo_sys_get_trip_type,
	.get_trip_temp = lombo_sys_get_trip_temp,
	.get_crit_temp = lombo_sys_get_crit_temp,
};

static struct lombo_thsens_platform_data *
		lombo_thermal_parse_dt(struct platform_device *pdev)
{
	struct lombo_thsens_platform_data *ptrips;
	struct device_node *np = pdev->dev.of_node;
	char prop_name[32];
	const char *tmp_str;
	u32 tmp_data;
	int i, j;

	ptrips = devm_kzalloc(&pdev->dev, sizeof(*ptrips), GFP_KERNEL);
	if (!ptrips) {
		PRT_ERR("failed to zalloc\n");
		return NULL;
	}

	if (of_property_read_u32(np, "num-trips", &tmp_data)) {
		PRT_ERR("failed to get 'num-trips'\n");
		goto err_parse_dt;
	}

	if (tmp_data > THERMAL_MAX_TRIPS) {
		PRT_ERR("num-trips is %d more than %d\n",
				tmp_data, THERMAL_MAX_TRIPS);
		goto err_parse_dt;
	}

	ptrips->num_trips = tmp_data;

	for (i = 0; i < ptrips->num_trips; i++) {
		sprintf(prop_name, "trip%d-temp", i);
		if (of_property_read_u32(np, prop_name, &tmp_data)) {
			PRT_ERR("failed to get 'trip%d-temp'\n", i);
			goto err_parse_dt;
		}

		ptrips->trip_points[i].temp = tmp_data;

		sprintf(prop_name, "trip%d-type", i);
		if (of_property_read_string(np, prop_name, &tmp_str)) {
			PRT_ERR("failed to get 'trip%d-type'\n", i);
			goto err_parse_dt;
		}

		if (!strcmp(tmp_str, "active")) {
			ptrips->trip_points[i].type = THERMAL_TRIP_ACTIVE;
		} else if (!strcmp(tmp_str, "passive")) {
			ptrips->trip_points[i].type = THERMAL_TRIP_PASSIVE;
		} else if (!strcmp(tmp_str, "hot")) {
			ptrips->trip_points[i].type = THERMAL_TRIP_HOT;
		} else if (!strcmp(tmp_str, "critical")) {
			ptrips->trip_points[i].type = THERMAL_TRIP_CRITICAL;
		} else {
			PRT_ERR("invalid 'trip%d-type': %s\n", i, tmp_str);
			goto err_parse_dt;
		}

		sprintf(prop_name, "trip%d-cdev-num", i);
		if (of_property_read_u32(np, prop_name, &tmp_data)) {
			PRT_ERR("failed to get 'trip%d-cdev-num'\n", i);
			goto err_parse_dt;
		}

		if (tmp_data > COOLING_DEV_MAX) {
			PRT_ERR("trip%d-cdev-num is %d more than %d\n",
				i, tmp_data, COOLING_DEV_MAX);
			goto err_parse_dt;
		}

		for (j = 0; j < tmp_data; j++) {
			sprintf(prop_name, "trip%d-cdev-name%d", i, j);
			if (of_property_read_string(np, prop_name, &tmp_str)) {
				PRT_ERR("failed to get 'trip%d-cdev-name%d'\n",
						i, j);
				goto err_parse_dt;
			}

			if (strlen(tmp_str) >= THERMAL_NAME_LENGTH) {
				PRT_ERR("trip%d-cdev-name%d is more than %d\n",
						i, j, THERMAL_NAME_LENGTH);
				goto err_parse_dt;
			}

			strcpy(ptrips->trip_points[i].cdev_name[j], tmp_str);
		}
	}

	return ptrips;

err_parse_dt:
	return NULL;
}

static int lombo_thermal_probe(struct platform_device *pdev)
{
	struct lombo_thermal_zone *pzone = NULL;
	struct lombo_thsens_platform_data *ptrips = NULL;
	struct device_node *np = pdev->dev.of_node;
	struct resource *iores;
	void __iomem *__base;
	int ret = 0;

	if (!np) {
		PRT_ERR("failed to get node\n");
		return -EINVAL;
	}

	iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iores) {
		PRT_ERR("%s get resources fail\n", __func__);
		return -EINVAL;
	}

	__base = devm_ioremap_resource(&pdev->dev, iores);

	if (IS_ERR(__base)) {
		PRT_ERR("%s io map fail\n", __func__);
		return PTR_ERR(__base);
	}

	ptrips = lombo_thermal_parse_dt(pdev);
	if (!ptrips)
		return -EINVAL;

	pzone = devm_kzalloc(&pdev->dev, sizeof(*pzone), GFP_KERNEL);
	if (!pzone)
		return -ENOMEM;

	mutex_init(&pzone->th_lock);
	mutex_lock(&pzone->th_lock);

	pzone->mode = THERMAL_DEVICE_DISABLED;
	pzone->trip_tab = ptrips;
	pzone->thermal_base = __base;
	of_property_read_u32(np, "sensor-id", &pzone->id);

	/* init hardware first, thermal zone device register will update temp */
	csp_sensor_hw_init(pzone->thermal_base);

	/* make sure sensor can work, sample rate */
	msleep(600);

	pzone->therm_dev = thermal_zone_device_register("lombo_thermal_zone",
		ptrips->num_trips, 0, pzone, &thdev_ops, NULL, 0, 1000);
	if (IS_ERR(pzone->therm_dev)) {
		PRT_ERR("Register thermal zone device failed.\n");
		ret = PTR_ERR(pzone->therm_dev);
		goto out_unlock;
	}

	platform_set_drvdata(pdev, pzone);
	pzone->mode = THERMAL_DEVICE_ENABLED;

	PRT_INFO("Thermal zone device registered.\n");

out_unlock:
	mutex_unlock(&pzone->th_lock);
	return ret;
}

static int lombo_thermal_remove(struct platform_device *pdev)
{
	struct lombo_thermal_zone *pzone = platform_get_drvdata(pdev);

	thermal_zone_device_unregister(pzone->therm_dev);
	mutex_destroy(&pzone->th_lock);

	return 0;
}

static int lombo_thermal_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	struct lombo_thermal_zone *pzone = platform_get_drvdata(pdev);

	cancel_delayed_work(&pzone->therm_dev->poll_queue);

	return 0;
}

static int lombo_thermal_resume(struct platform_device *pdev)
{
	struct lombo_thermal_zone *pzone = platform_get_drvdata(pdev);
	int delay;

	csp_sensor_hw_init(pzone->thermal_base);

	delay = pzone->therm_dev->polling_delay;
	if (delay > 1000)
		mod_delayed_work(system_freezable_wq,
				&pzone->therm_dev->poll_queue,
				round_jiffies(msecs_to_jiffies(delay)));
	else if (delay)
		mod_delayed_work(system_freezable_wq,
				&pzone->therm_dev->poll_queue,
				msecs_to_jiffies(delay));

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_thermal_match[] = {
	{ .compatible = "lombotech,lombo-thermal0" },
	{ .compatible = "lombotech,lombo-thermal1" },
	{ .compatible = "lombotech,lombo-thermal2" },
	{},
};
#endif

static struct platform_driver lombo_thermal_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "lombo-thermal",
		.of_match_table = of_match_ptr(lombo_thermal_match),
	},
	.probe = lombo_thermal_probe,
	.suspend = lombo_thermal_suspend,
	.resume = lombo_thermal_resume,
	.remove = lombo_thermal_remove,
};

static int __init lombo_thermal_init(void)
{
	return platform_driver_register(&lombo_thermal_driver);
}

/* Should be later than lombo_cpufreq_cooling_register */
late_initcall(lombo_thermal_init);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo thermal driver");
MODULE_LICENSE("GPL");

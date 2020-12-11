/*
 * lombo_cpufreq_cooling.c - lombo cpufreq works as cooling device.
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
#include <linux/cpufreq.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>


static int lombo_cpufreq_cooling_probe(struct platform_device *pdev)
{
	struct thermal_cooling_device *cdev;
	struct cpumask mask_val;

	/* make sure cpufreq driver has been initialized */
	if (!cpufreq_frequency_get_table(0))
		return -EPROBE_DEFER;

	cpumask_set_cpu(0, &mask_val);
	cdev = of_cpufreq_cooling_register(pdev->dev.of_node, &mask_val);

	if (IS_ERR(cdev)) {
		dev_err(&pdev->dev, "Failed to register cooling device\n");
		return PTR_ERR(cdev);
	}

	platform_set_drvdata(pdev, cdev);

	dev_info(&pdev->dev, "Cooling device registered: %s\n",	cdev->type);

	return 0;
}

static int lombo_cpufreq_cooling_remove(struct platform_device *pdev)
{
	struct thermal_cooling_device *cdev = platform_get_drvdata(pdev);

	cpufreq_cooling_unregister(cdev);

	return 0;
}

static int lombo_cpufreq_cooling_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	return 0;
}

static int lombo_cpufreq_cooling_resume(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_cpufreq_cooling_match[] = {
	{ .compatible = "lombotech,cpufreq-cooling" },
	{},
};
#endif

static struct platform_driver lombo_cpufreq_cooling_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "lombo-cpufreq-cooling",
		.of_match_table = of_match_ptr(lombo_cpufreq_cooling_match),
	},
	.probe = lombo_cpufreq_cooling_probe,
	.suspend = lombo_cpufreq_cooling_suspend,
	.resume = lombo_cpufreq_cooling_resume,
	.remove = lombo_cpufreq_cooling_remove,
};

static int __init lombo_cpufreq_cooling_init(void)
{
	return platform_driver_register(&lombo_cpufreq_cooling_driver);
}

static void __exit lombo_cpufreq_cooling_exit(void)
{
	platform_driver_unregister(&lombo_cpufreq_cooling_driver);
}

/* Should be later than lombo_cpufreq_register */
module_init(lombo_cpufreq_cooling_init);
module_exit(lombo_cpufreq_cooling_exit);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo cpufreq cooling driver");
MODULE_LICENSE("GPL");

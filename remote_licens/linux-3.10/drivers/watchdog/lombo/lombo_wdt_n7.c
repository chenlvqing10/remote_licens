/*
 * lombo_wdt.c - Driver of LomboTech Watchdog
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
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/of.h>
#include <asm/system_misc.h>
#include <asm/ptrace.h>
#include "lombo_wdt_n7.h"

static int tmr_margin = LOMBO_WATCHDOG_MAX_TIMEOUT;
static bool nowayout = WATCHDOG_NOWAYOUT;

module_param(tmr_margin,  int, 0);
module_param(nowayout,   bool, 0);

MODULE_PARM_DESC(tmr_margin, "Watchdog tmr_margin in seconds. (default="
		__MODULE_STRING(LOMBO_WATCHDOG_DEFAULT_TIME) ")");
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
		__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

static DEFINE_SPINLOCK(lombo_wdt_lock);
static void __iomem *restart_wdt_base;

/**
 * struct lombo_watchdog - Runtime control data for watchdog.
 * @base_addr: pointer to virtual address of watchdog.
 * @wdt_dev: watchdog device for wachdog core use.
 */
struct lombo_watchdog {
	void __iomem *base_addr;
	struct watchdog_device wdt_dev;
};

/**
 * lombo_wdt_restart - generate a system reset by watchdog.
 * @str: character for reboot.
 * @cmd: reboot command.
 */
static void lombo_wdt_restart(char str, const char *cmd)
{
	PRT_DBG("enter\n");

	csp_wdt_disable(restart_wdt_base);
	/* set system reset */
	csp_wdt_set_system_reset(restart_wdt_base);
	/* set timeout to 0.5s */
	csp_wdt_set_timeout(restart_wdt_base, 0);
	/* enable watchdog */
	csp_wdt_enable(restart_wdt_base);
	/* restart watchdog */
	csp_wdt_restart(restart_wdt_base);

	while (1)
		;
}

/**
 * lombo_wdt_keepalive - sends a keepalive ping to the watchdog device.
 * @wdd: pointer to the watchdog device.
 *
 * Retrun 0 if success.
 */
static int lombo_wdt_keepalive(struct watchdog_device *wdd)
{
	struct lombo_watchdog *lombo_wdt = watchdog_get_drvdata(wdd);

	PRT_DBG("enter\n");

	spin_lock(&lombo_wdt_lock);

	/* restart watchdog */
	csp_wdt_restart(lombo_wdt->base_addr);

	spin_unlock(&lombo_wdt_lock);

	return 0;
}

/**
 * lombo_wdt_stop - stopping the watchdog device.
 * @wdd: pointer to the watchdog device.
 *
 * Retrun 0 if success.
 */
static int lombo_wdt_stop(struct watchdog_device *wdd)
{
	struct lombo_watchdog *lombo_wdt = watchdog_get_drvdata(wdd);

	PRT_DBG("enter\n");

	spin_lock(&lombo_wdt_lock);

	/* disable watchdog */
	csp_wdt_disable(lombo_wdt->base_addr);

	spin_unlock(&lombo_wdt_lock);

	return 0;
}

/**
 * lombo_wdt_start - starting the watchdog device.
 * @wdd: pointer to the watchdog device.
 *
 * Retrun 0 if success.
 */
static int lombo_wdt_start(struct watchdog_device *wdd)
{
	struct lombo_watchdog *lombo_wdt = watchdog_get_drvdata(wdd);
	int status = 0;

	PRT_DBG("enter\n");

	spin_lock(&lombo_wdt_lock);

	csp_wdt_disable(lombo_wdt->base_addr);
	/* set timeout */
	csp_wdt_set_timeout(lombo_wdt->base_addr, wdd->timeout);
	/* set system reset */
	csp_wdt_set_system_reset(lombo_wdt->base_addr);
	/* enable watchdog */
	status = csp_wdt_get_status(lombo_wdt->base_addr);
	if (status == STATUS_READY_TO_ENABLE) {
		csp_wdt_enable(lombo_wdt->base_addr);
		spin_unlock(&lombo_wdt_lock);
		PRT_DBG("lombo_wdt_start end,success\n");
		return 0;
	} else {
		PRT_DBG("it is not ready to enable\n");
		spin_unlock(&lombo_wdt_lock);
		PRT_ERR("lombo_wdt_start end, fail\n");
		return -1;
	}
}

/**
 * lombo_wdt_set_timeout - setting the watchdog devices timeout value.
 * @wdd: pointer to the watchdog device.
 * @timeout: timeout value to set.
 *
 * Retrun 0 if success.
 */
static int lombo_wdt_set_timeout(struct watchdog_device *wdd, unsigned timeout)
{
	struct lombo_watchdog *lombo_wdt = watchdog_get_drvdata(wdd);

	PRT_DBG("enter, timeout=%d\n", timeout);

	spin_lock(&lombo_wdt_lock);

	/* update timeout in watchdog device */
	wdd->timeout = timeout;
	/* set timeout */
	csp_wdt_set_timeout(lombo_wdt->base_addr, wdd->timeout);
	/* restart watchdog */
	csp_wdt_restart(lombo_wdt->base_addr);

	spin_unlock(&lombo_wdt_lock);

	return 0;
}

static const struct watchdog_info lombo_wdt_info = {
	.options  = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.firmware_version = 1,
	.identity = "Lombo Watchdog",
};

static struct watchdog_ops lombo_wdt_ops = {
	.owner = THIS_MODULE,
	.start = lombo_wdt_start,
	.stop = lombo_wdt_stop,
	.ping = lombo_wdt_keepalive,
	.set_timeout = lombo_wdt_set_timeout,
};

static int lombo_wdt_probe(struct platform_device *pdev)
{
	struct lombo_watchdog *lombo_wdt;
	struct resource *res;
	int err = 0;

	lombo_wdt = kzalloc(sizeof(*lombo_wdt), GFP_KERNEL);
	if (!lombo_wdt) {
		PRT_ERR("fail to allocate memory\n");
		err = -ENOMEM;
		goto out;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		PRT_ERR("no memory resource specified\n");
		err = -ENOENT;
		goto out_kfree;
	}

	/* get the base address for the watchdog timer */
	lombo_wdt->base_addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(lombo_wdt->base_addr)) {
		PRT_ERR("fail to ioremap\n");
		err = PTR_ERR(lombo_wdt->base_addr);
		goto out_kfree;
	}

	lombo_wdt->wdt_dev.ops = &lombo_wdt_ops;
	lombo_wdt->wdt_dev.info = &lombo_wdt_info;
	lombo_wdt->wdt_dev.timeout = LOMBO_WATCHDOG_DEFAULT_TIME;
	lombo_wdt->wdt_dev.max_timeout = LOMBO_WATCHDOG_MAX_TIMEOUT;
	lombo_wdt->wdt_dev.min_timeout = LOMBO_WATCHDOG_MIN_TIMEOUT;
	lombo_wdt->wdt_dev.parent = &pdev->dev;

	platform_set_drvdata(pdev, lombo_wdt);
	watchdog_set_drvdata(&lombo_wdt->wdt_dev, lombo_wdt);

	lombo_wdt_stop(&lombo_wdt->wdt_dev);
	csp_wdt_clr_int_pend(&lombo_wdt->base_addr);

	watchdog_init_timeout(&lombo_wdt->wdt_dev,
			tmr_margin, &pdev->dev);
	watchdog_set_nowayout(&lombo_wdt->wdt_dev, nowayout);

	/* register watchdog device */
	err = watchdog_register_device(&lombo_wdt->wdt_dev);
	if (err) {
		PRT_ERR("cannot register watchdog (%d)\n", err);
		goto out_kfree;
	}

	/* register arm restart for reboot */
	arm_pm_restart = lombo_wdt_restart;
	restart_wdt_base = lombo_wdt->base_addr;

	PRT_INFO("lombo watchdog registered (timeout: %d sec, nowayout: %d)\n",
		lombo_wdt->wdt_dev.timeout, nowayout);

	return 0;

out_kfree:
	kfree(lombo_wdt);
out:
	return err;
}

static int lombo_wdt_remove(struct platform_device *pdev)
{
	struct lombo_watchdog *lombo_wdt = platform_get_drvdata(pdev);

	watchdog_unregister_device(&lombo_wdt->wdt_dev);
	watchdog_set_drvdata(&lombo_wdt->wdt_dev, NULL);
	kfree(lombo_wdt);

	arm_pm_restart = NULL;

	return 0;
}

static void lombo_wdt_shutdown(struct platform_device *pdev)
{
	struct lombo_watchdog *lombo_wdt = platform_get_drvdata(pdev);

	/* disable watchdog */
	lombo_wdt_stop(&lombo_wdt->wdt_dev);
}

static const struct of_device_id lombo_wdt_match[] = {
	{ .compatible = "lombo,n7-wdt" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_wdt_match);

static struct platform_driver lombo_wdt_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "lombo-wdt",
		.of_match_table = lombo_wdt_match,
	},
	.probe = lombo_wdt_probe,
	.remove = lombo_wdt_remove,
	.shutdown = lombo_wdt_shutdown,
};
module_platform_driver(lombo_wdt_driver);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Watchdog Driver for Lombo SoCs");
MODULE_LICENSE("GPL");

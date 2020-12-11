/*
 * lombo_rfkill_bt.c - Lombo bt rfkill driver
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

#include <mach/debug.h>
#include <mach/lombo_wireless_pm.h>

static struct rfkill *bt_rfkill;

static int bt_rfkill_set_enable(void *data, bool blocked)
{
	if (!blocked) {
		PRT_DBG("turn on bt!\n");
		lombo_wireless_set_enable(WL_BT, ENABLED);
	} else {
		PRT_DBG("turn off bt!\n");
		lombo_wireless_set_enable(WL_BT, DISABLED);
	}
	return 0;
}

static struct rfkill_ops bt_rfkill_ops = {
	.set_block = bt_rfkill_set_enable,
};

static int lombo_rfkill_bt_probe(struct platform_device *pdev)
{
	int ret = 0;

	bt_rfkill = rfkill_alloc("bluetooth", &pdev->dev, RFKILL_TYPE_BLUETOOTH,
			&bt_rfkill_ops, NULL);
	if (!bt_rfkill) {
		ret = -1;
		PRT_ERR("rfkill_alloc failed!\n");
		goto end;
	}

	/* power off bt default */
	rfkill_init_sw_state(bt_rfkill, true);
	ret = rfkill_register(bt_rfkill);
	if (ret) {
		PRT_ERR("rfkill_register failed!\n");
		rfkill_destroy(bt_rfkill);
		goto end;
	}

end:
	return ret;
}

static int lombo_rfkill_bt_remove(struct platform_device *pdev)
{
	if (bt_rfkill) {
		rfkill_unregister(bt_rfkill);
		rfkill_destroy(bt_rfkill);
	}

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_rfkill_bt_dt_match[] = {
	{ .compatible = "lombo,rfkill-bt" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_rfkill_bt_dt_match);
#endif

static struct platform_driver lombo_rfkill_bt_driver = {
	.probe		= lombo_rfkill_bt_probe,
	.remove		= lombo_rfkill_bt_remove,
	.driver		= {
		.name	= "lombo-rfkill-bt",
		.owner	= THIS_MODULE,
		#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(lombo_rfkill_bt_dt_match),
		#endif
	},
};
module_platform_driver(lombo_rfkill_bt_driver);

MODULE_DESCRIPTION("LomboTech rfkill bt driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");

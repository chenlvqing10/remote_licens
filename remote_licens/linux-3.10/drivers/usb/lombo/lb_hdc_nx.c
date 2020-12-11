/*
 * Lombo usb nx driver
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
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/of_device.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>

#include "lb.h"
#include "lombo_nx.h"

struct lb_hdc_nx_platform_flag {
	unsigned int flags;
	bool runtime_pm;
	u32 ahbburst_config;
	u32 burst_length;
};

static const struct lb_hdc_nx_platform_flag n7_usb_data = {
	/*.flags = LB_HDC_OVERRIDE_AHB_BURST | LB_HDC_OVERRIDE_BURST_LENGTH,*/
	.flags = LB_HDC_SUPPORTS_RUNTIME_PM | LB_HDC_OVERRIDE_AHB_BURST |
		LB_HDC_OVERRIDE_BURST_LENGTH | LB_HDC_NX_EHCI_QUIRK,
	.ahbburst_config = 0x3,
	.burst_length = 0x0404,
};

static const struct lb_hdc_nx_platform_flag n9_usb_data = {
	/*.flags = LB_HDC_OVERRIDE_AHB_BURST | LB_HDC_OVERRIDE_BURST_LENGTH,*/
	.flags = LB_HDC_SUPPORTS_RUNTIME_PM | LB_HDC_OVERRIDE_AHB_BURST |
		LB_HDC_OVERRIDE_BURST_LENGTH | LB_HDC_NX_EHCI_QUIRK,
	.ahbburst_config = 0x3,
	.burst_length = 0x0404,
};

static const struct of_device_id lb_hdc_nx_dt_ids[] = {
	{ .compatible = "lombo,n7-usb", .data = &n7_usb_data },
	{ .compatible = "lombo,n9-usb", .data = &n9_usb_data },
	{  }
};
MODULE_DEVICE_TABLE(of, lb_hdc_nx_dt_ids);

struct lb_hdc_nx_data {
	struct usb_phy *phy;
	struct platform_device *lb_pdev;
	struct clk *clk_gate;
	struct clk *clk_reset;
	bool supports_runtime_pm;
	bool in_lpm;
	struct pinctrl *pinctrl;
	const struct lb_hdc_nx_platform_flag *data;
};

static int lb_hdc_nx_notify_event(struct lb_hdc *lb, unsigned event)
{
	struct device *dev = lb->dev->parent;
	int ret = 0;

	switch (event) {
	case LB_HDC_CONTROLLER_VBUS_EVENT:
		break;
	case LB_HDC_CONTROLLER_CHARGER_POST_EVENT:
		break;
	default:
		dev_info(dev, "unsupport event %d\n", event);
	}

	return ret;
}

static int lb_hdc_nx_probe(struct platform_device *pdev)
{
	int ret;
	struct lb_hdc_nx_data *data;
	struct lb_hdc_platform_data pdata = {
		.name		= dev_name(&pdev->dev),
		.capoffset	= DEF_CAPOFFSET,
		.notify_event	= lb_hdc_nx_notify_event,
	};

	const struct of_device_id *of_id = of_match_device(lb_hdc_nx_dt_ids,
							   &pdev->dev);
	const struct lb_hdc_nx_platform_flag *nx_platform_flag = of_id->data;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		dev_err(&pdev->dev, "err alloc lb_hdc_nx_data\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, data);
	data->data = nx_platform_flag;

	/* clk resource */
	data->clk_gate = devm_clk_get(&pdev->dev, "usb_gate");
	data->clk_reset = devm_clk_get(&pdev->dev, "usb_reset");
	if (IS_ERR(data->clk_gate) || IS_ERR(data->clk_reset)) {
		dev_err(&pdev->dev, "get usb gate/reset clk failed\n");
		return -EINVAL;
	}

	ret = clk_prepare_enable(data->clk_gate);
	if (ret) {
		dev_err(&pdev->dev, "enable usb clk failed\n");
		return ret;
	}

	/* phy reset and clear siddq */
	nx_phy_reset(1);

	ret = clk_prepare_enable(data->clk_reset);
	if (ret) {
		clk_disable_unprepare(data->clk_gate);
		dev_err(&pdev->dev, "enable usb reset assert failed\n");
		return ret;
	}

	/* phy node in dts */
	data->phy = devm_usb_get_phy_by_phandle(&pdev->dev, "lombo,usbphy", 0);
	if (IS_ERR(data->phy)) {
		ret = PTR_ERR(data->phy);
		/* Return -EINVAL if no usbphy is available */
		if (ret == -ENODEV)
			ret = -EINVAL;
		goto err_out;
	}

	pdata.usb_phy = data->phy;
	pdata.flags |= nx_platform_flag->flags;
	if (pdata.flags & LB_HDC_SUPPORTS_RUNTIME_PM)
		data->supports_runtime_pm = true;

	ret = dma_coerce_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
	if (ret)
		goto err_out;

	if (nx_platform_flag->flags & LB_HDC_OVERRIDE_AHB_BURST)
		pdata.ahbburst_config = nx_platform_flag->ahbburst_config;

	if (nx_platform_flag->flags & LB_HDC_OVERRIDE_BURST_LENGTH)
		pdata.burst_length = nx_platform_flag->burst_length;

	data->lb_pdev = lb_hdc_add_device(&pdev->dev, pdev->resource,
					  pdev->num_resources, &pdata);
	if (IS_ERR(data->lb_pdev)) {
		ret = PTR_ERR(data->lb_pdev);
		dev_err(&pdev->dev, "register platform device err %d\n", ret);
		goto err_out;
	}

	if (data->supports_runtime_pm) {
		pm_runtime_set_active(&pdev->dev);
		pm_runtime_enable(&pdev->dev);
	}

	return 0;

err_out:
	dev_err(&pdev->dev, "%s err %d\n", __func__, ret);
	clk_disable_unprepare(data->clk_reset);
	clk_disable_unprepare(data->clk_gate);
	return ret;
}

static int lb_hdc_nx_remove(struct platform_device *pdev)
{
	struct lb_hdc_nx_data *data = platform_get_drvdata(pdev);

	if (data->supports_runtime_pm) {
		pm_runtime_get_sync(&pdev->dev);
		pm_runtime_disable(&pdev->dev);
		pm_runtime_put_noidle(&pdev->dev);
	}
	lb_hdc_remove_device(data->lb_pdev);
	/* release clk */
	if (data->clk_reset)
		clk_disable_unprepare(data->clk_reset);
	if (data->clk_gate)
		clk_disable_unprepare(data->clk_gate);

	return 0;
}

#ifdef CONFIG_PM
static int nx_controller_suspend(struct device *dev)
{
	struct lb_hdc_nx_data *data = dev_get_drvdata(dev);

	/* todo! release or disable clk? */
	data->in_lpm = true;

	return 0;
}

static int nx_controller_resume(struct device *dev)
{
	struct lb_hdc_nx_data *data = dev_get_drvdata(dev);

	if (!data->in_lpm) {
		WARN_ON(1);
		return 0;
	}

	/* todo! prepare and enable clk? */
	data->in_lpm = false;

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int lb_hdc_nx_suspend(struct device *dev)
{
	int ret;
	struct lb_hdc_nx_data *data = dev_get_drvdata(dev);

	if (data->in_lpm)
		/* The core's suspend doesn't run */
		return 0;

	/*controller suspend*/
	ret = nx_controller_suspend(dev);
	if (ret) {
		dev_err(dev, "controller suspend failed\n");
		return ret;
	}

	/*disable PRCM USB bus clk reaet,enter reset status*/
	clk_disable_unprepare(data->clk_reset);

	/*phy out of reset status and set siddq*/
	nx_phy_reset(0);

	/*disable PRCM USB bus clk gate*/
	clk_disable_unprepare(data->clk_gate);

	return 0;
}

static int lb_hdc_nx_resume(struct device *dev)
{
	int ret;
	struct lb_hdc_nx_data *data = dev_get_drvdata(dev);

	/*enable PRCM USB bus clk gate*/
	ret = clk_prepare_enable(data->clk_gate);
	if (ret) {
		dev_err(dev, "enable usb clk gate failed\n");
		return ret;
	}

	/*out of phy reset status*/
	nx_phy_reset(1);

	/*free PRCM USB bus clk reset,out of reset status*/
	ret = clk_prepare_enable(data->clk_reset);
	if (ret) {
		dev_err(dev, "enable usb clk reset failed\n");
		return ret;
	}

	/*controller resume*/
	ret = nx_controller_resume(dev);
	if (!ret && data->supports_runtime_pm) {
		pm_runtime_disable(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
	}

	return ret;
}
#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_PM_RUNTIME
static int lb_hdc_nx_runtime_suspend(struct device *dev)
{
	struct lb_hdc_nx_data *data = dev_get_drvdata(dev);

	if (data->in_lpm) {
		WARN_ON(1);
		return 0;
	}

	return nx_controller_suspend(dev);
}

static int lb_hdc_nx_runtime_resume(struct device *dev)
{
	return nx_controller_resume(dev);
}
#endif /* CONFIG_PM_RUNTIME */

#endif /* CONFIG_PM */

static const struct dev_pm_ops lb_hdc_nx_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(lb_hdc_nx_suspend, lb_hdc_nx_resume)
	SET_RUNTIME_PM_OPS(lb_hdc_nx_runtime_suspend,
			lb_hdc_nx_runtime_resume, NULL)
};
static struct platform_driver lb_hdc_nx_driver = {
	.probe = lb_hdc_nx_probe,
	.remove = lb_hdc_nx_remove,
	.driver = {
		.name = "nx_usb",
		.owner = THIS_MODULE,
		.of_match_table = lb_hdc_nx_dt_ids,
		.pm = &lb_hdc_nx_pm_ops,
	 },
};

static int __init lb_hdc_nx_init(void)
{
	return platform_driver_register(&lb_hdc_nx_driver);
}

subsys_initcall_sync(lb_hdc_nx_init);

MODULE_ALIAS("platform:nx-usb");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech usb");
MODULE_LICENSE("GPL");

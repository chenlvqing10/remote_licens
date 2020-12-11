/*
 * Lombo usb phy driver
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Driver of LomboTech usb phy
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
#include <linux/usb/otg.h>
#include <mach/common.h>

#ifdef CONFIG_ARCH_LOMBO_N7
#include <mach/efuse.h>
#endif

#define DRIVER_NAME	"nx_usb_phy"
#define PHY_VBUS	"phy_vbus"
#define PHY_HUB		"phy_hub"

static const char *const usb_phy_modes[] = {
	"phy0",
	"phy1",
};
u32 phy_sel;

/* register offset */
#define USBCTRL		0x0
#define USBDBG		0x8
#define PHYCFG		0x10
#define PHYTEST		0x14
#define PHYTUNE		0x18
#define USBDBG1		0x1C

#if defined(CONFIG_ARCH_LOMBO_N7)
#define PHYSTAT		0x20
#endif

#if defined(CONFIG_ARCH_LOMBO_N9)
#define USBCOMCFG	0x20
#define PHYCFG1		0x24
#define PHYSTAT		0x2C
#endif

/* control register */
#define PHY_SUSP	(1<<8)
#define PHY_SUSP_SIG	(1<<9)
#define PHY_SUSP_SEL	(1<<10)
#define PHY_LSCD_EN	(1<<16)
#define PHY_LSCD_FLG	(1<<17)
#define PHY_LSCI_EN	(1<<18)

/* config register */
#define PHY_COMMONONN(x)	(((x)&0x3)<<12)

/* phy mode base addr */
void __iomem *base_addr;
void __iomem *phy_reset_base;

/**
 * struct nx_phy_data - special soc private data
 * @flags: configuration
 * @reset_offset: usb phy reset control register offset
 */
struct nx_phy_data {
	unsigned int flags;
	u32 reset_offset;
};

/* usb phy reset control register in prcm module */
#define NX_PHY_RESET_PRCM	BIT(0)

static const struct nx_phy_data n7_phy_data = {
	.flags = NX_PHY_RESET_PRCM,
	.reset_offset = 0x0400ACE0,
};

static const struct nx_phy_data n9_phy_data = {
	.flags = NX_PHY_RESET_PRCM,
	.reset_offset = 0x0400ACE0,
};

static const struct of_device_id nx_phy_ids[] = {
	{ .compatible = "lombo,n7-usbphy", .data = &n7_phy_data, },
	{ .compatible = "lombo,n9-usbphy", .data = &n9_phy_data, },
	{}
};
MODULE_DEVICE_TABLE(of, nx_phy_ids);

struct nx_phy {
	struct				usb_phy phy;
	int				id;
	const struct nx_phy_data	*data;
};

void nx_phy_sel(void)
{
#if defined(CONFIG_ARCH_LOMBO_N9)
	u32 val;
	void *reg = base_addr + USBCTRL;

	val = readl(reg);
	val &= ~BIT(31);
	/* open USBCTRL's 21 bit to ensure write had finish */
	val |= BIT(21);
	if (phy_sel) {
		val |= BIT(30);
		writel(val, reg);
	} else {
		val &= ~BIT(30);
		writel(val, reg);
	}
#endif
}
EXPORT_SYMBOL_GPL(nx_phy_sel);

void nx_phy_reset(int enable)
{
	void __iomem *base;
	u32 val;

#if defined(CONFIG_ARCH_LOMBO_N7)
	/* usb phy reset */
	base = ioremap(n7_phy_data.reset_offset, 4);
	val = readl(base);
	if (enable)
		val |= BIT(0);
	else
		val &= ~BIT(0);
	writel(val, base);
	mdelay(1);

	/* clear phy siddq */
	if (base_addr) {
		val = readl(base_addr + PHYCFG);
		if (enable) {
			val &= ~BIT(10);
			pr_info("%s clear phy siddq\n", __func__);
		} else {
			val |= BIT(10);
			pr_info("%s set phy siddq\n", __func__);
		}
		writel(val, base_addr + PHYCFG);
	}
#endif

#if defined(CONFIG_ARCH_LOMBO_N9)
	/* usb phy reset */
	base = ioremap(n9_phy_data.reset_offset, 4);
	phy_reset_base = base;
	val = readl(base);
	if (phy_sel) {
		if (enable)
			val |= BIT(1);
		else
			val &= ~BIT(1);
	} else {
		if (enable)
			val |= BIT(0);
		else
			val &= ~BIT(0);
	}
	writel(val, base);
	mdelay(1);

	/* clear phy siddq */
	if (base_addr) {
		val = readl(base_addr + PHYCFG1);
		if (enable) {
			val &= ~BIT(7);
			pr_info("%s clear phy siddq\n", __func__);
		} else {
			val |= BIT(7);
			pr_info("%s set phy siddq\n", __func__);
		}
		writel(val, base_addr + PHYCFG1);
	}
#endif
}
EXPORT_SYMBOL_GPL(nx_phy_reset);

/* usb phy1 reset when device had disconnect from roothub */
void nx_phy1_reset(void)
{
#if defined(CONFIG_ARCH_LOMBO_N9V0)
	u32 val;

	if (phy_sel) {
		/* phy1 into reset status */
		val = readl(phy_reset_base);
		val &= ~BIT(1);
		writel(val, phy_reset_base);
		mdelay(5);

		/* phy1 exit reset status */
		val = readl(phy_reset_base);
		val |= BIT(1);
		writel(val, phy_reset_base);
	}
#endif
}
EXPORT_SYMBOL_GPL(nx_phy1_reset);

/* initialize the OTG controller */
static int nx_phy_init(struct usb_phy *phy)
{
#ifdef CONFIG_ARCH_LOMBO_N7
	u32 p1, p2, ret;
	u32 buf_len, id = 0x5;
	u8 buf;
	u32 e_val, val;

#if defined(CONFIG_ARCH_LOMBO_N7V1)
	int len;

	if (of_find_property(phy->dev->of_node, "force_vbus", &len)) {
		dev_info(phy->dev, "%s had force_vbus prop\n", __func__);

		ret = lombo_func2(&p1, &p2);
		if (ret == 0) {
			if (p1 == 1) {
				/* force select internal vbus valid signal */
				val = readl(phy->io_priv + PHYCFG);
				val |= BIT(0);
				val |= BIT(1);
				writel(val, phy->io_priv + PHYCFG);

				/* force a/b valid, for id to 1(device) */
				val = readl(phy->io_priv + USBDBG);
				val &= 0xFFFFFFC0;
				val |= 0x3F;
				writel(val, phy->io_priv + USBDBG);
			}
		}
	}
#endif

	dev_info(phy->dev, "%s set phy cts\n", __func__);
	ret = lombo_func2(&p1, &p2);
	if (ret == 0) {
		if (p1 == 1) {
			val = readl(phy->io_priv + PHYTUNE);
			val &= 0x04CB7387;
			val |= 0x00880083;
			writel(val, phy->io_priv + PHYTUNE);
		}

		if (p1 == 2) {
			ret = lombo_get_efuse_module_len(id, &buf_len);
			if (ret != 0) {
				dev_err(phy->dev, "get efuse len failed\n");
				return -1;
			}

			ret = lombo_efuse_read(id, (void *)&buf, buf_len);
			if (ret != 0) {
				dev_err(phy->dev, "read efuse failed\n");
				return -1;
			}

			if (buf == 0) {
				dev_err(phy->dev, "usb efuse is zero\n");
				return 0;
			}

			e_val = (128 * 250) / (buf + 144) - 92;
			dev_info(phy->dev, "efuse: %d, val: %d\n", buf, e_val);
			e_val &= 0x7f;

			val = readl(phy->io_priv + PHYTUNE);
			val &= 0x80FFFFF8;
			val |= e_val << 24;
			val |= BIT(3);
			val |= BIT(31);
			writel(val, phy->io_priv + PHYTUNE);

			val = readl(phy->io_priv + USBDBG1);
			val &= 0xFFFFFE0F;
			val |= 0x4 << 4;
			val |= BIT(7);
			writel(val, phy->io_priv + USBDBG1);
		}
	}
#endif

#if defined(CONFIG_ARCH_LOMBO_N9V0)
	u32 val;

	/* set n9v0's phy1 cts deploy */
	if (phy_sel) {
		val = readl(phy->io_priv + USBCOMCFG);
		val &= ~BIT(8);
		val |= BIT(9);
		writel(val, phy->io_priv + USBCOMCFG);

		val = readl(phy->io_priv + PHYTUNE);
		val &= ~BIT(0);
		val |= BIT(2);
		writel(val, phy->io_priv + PHYTUNE);
	}

	/* close phy0/phy1 ldo function */
	val = readl(phy->io_priv + PHYCFG);
	val &= ~BIT(3);
	writel(val, phy->io_priv + PHYCFG);

#endif
	return 0;
}

/* shutdown the OTG controller */
static void nx_phy_shutdown(struct usb_phy *phy)
{
	u32 val;

	/* set phy siddq */
#if defined(CONFIG_ARCH_LOMBO_N7)
	val = readl(phy->io_priv + PHYCFG);
	val |= BIT(10);
	writel(val, phy->io_priv + PHYCFG);
#endif

#if defined(CONFIG_ARCH_LOMBO_N9)
	val = readl(phy->io_priv + PHYCFG1);
	val |= BIT(7);
	writel(val, phy->io_priv + PHYCFG1);
#endif
}

/* for non-OTG B devices: set transceiver into suspend mode */
static int nx_phy_suspend(struct usb_phy *phy, int suspend)
{
	u32 crtlval, cfgval;

	crtlval = readl(phy->io_priv + USBCTRL);
	cfgval = readl(phy->io_priv + PHYCFG);

#if defined(CONFIG_ARCH_LOMBO_N7)
	if (suspend) {
		crtlval |= PHY_SUSP_SEL;
		crtlval &= ~PHY_SUSP;

		cfgval &= ~(PHY_COMMONONN(0x3));
		cfgval |= PHY_COMMONONN(0x1);
	} else {
		/* resume */
		crtlval |= (PHY_SUSP_SEL | PHY_SUSP);
		cfgval &= ~(PHY_COMMONONN(0x3));
	}
#endif

#if defined(CONFIG_ARCH_LOMBO_N9)
	if (suspend) {
		crtlval |= PHY_SUSP_SEL;
		crtlval &= ~PHY_SUSP;
	} else {
		/* resume */
		if (phy_sel) {
			/*phy1*/
			crtlval = readl(phy->io_priv + USBCTRL);
			crtlval |= 0xC;
			writel(crtlval, phy->io_priv + USBCTRL);

			udelay(10);
			crtlval = readl(phy->io_priv + USBCTRL);
			crtlval &= ~(0xC);
			writel(crtlval, phy->io_priv + USBCTRL);
		}
		crtlval |= (PHY_SUSP_SEL | PHY_SUSP);
	}
#endif

	writel(crtlval, phy->io_priv + USBCTRL);
	writel(cfgval, phy->io_priv + PHYCFG);

	/* wait at least 3ms to let bus idle */
	mdelay(3);

	crtlval = readl(phy->io_priv + USBCTRL);
	if (suspend)
		crtlval |= (PHY_LSCD_EN | PHY_LSCI_EN);
	else {
		if (crtlval & PHY_LSCD_EN) {
			crtlval &= ~(PHY_LSCD_EN | PHY_LSCI_EN);
			crtlval |= PHY_LSCD_FLG;
		}
	}

	writel(crtlval, phy->io_priv + USBCTRL);

	return 0;
}

/* notify phy connect status change -connect */
static int nx_phy_connect(struct usb_phy *phy, enum usb_device_speed speed)
{
	return 0;
}

/* notify phy connect status change - disconnect */
static int nx_phy_disconnect(struct usb_phy *phy, enum usb_device_speed speed)
{
	return 0;
}

static int nx_phy_probe(struct platform_device *pdev)
{
	struct nx_phy *nx_phy;
	struct resource *res;
#if defined(CONFIG_ARCH_LOMBO_N9)
	const char *phy_mode = NULL;
#endif
	int ret;
	int num;
	const struct of_device_id *of_id = of_match_device(nx_phy_ids,
							   &pdev->dev);

	struct device_node *np = pdev->dev.of_node;
	if (!np) {
		dev_err(&pdev->dev, "err: device node is null\n");
		return -ENODEV;
	}

#if defined(CONFIG_ARCH_LOMBO_N9)
	of_property_read_string(np, "phy_mode", &phy_mode);
	if (strcmp(phy_mode, usb_phy_modes[1]))
		phy_sel = 0;
	else
		phy_sel = 1;

	dev_info(&pdev->dev, "phy mode: %s, sel: %d\n", phy_mode, phy_sel);
#endif

	nx_phy = devm_kzalloc(&pdev->dev, sizeof(*nx_phy), GFP_KERNEL);
	if (!nx_phy) {
		dev_err(&pdev->dev, "allocate usb phy structure failed\n");
		return -ENOMEM;
	}

	ret = of_alias_get_id(np, "usbphy");
	if (ret < 0) {
		dev_warn(&pdev->dev, "get usb phy alias id fail %d\n", ret);
		nx_phy->id = 0;
	} else
		nx_phy->id = ret;

	nx_phy->data = of_id->data;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	nx_phy->phy.io_priv = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(nx_phy->phy.io_priv)) {
		dev_err(&pdev->dev, "get ioremap res failed\n");
		return PTR_ERR(nx_phy->phy.io_priv);
	}
	base_addr = nx_phy->phy.io_priv;

	nx_phy->phy.dev = &pdev->dev;
	nx_phy->phy.label = DRIVER_NAME;
	nx_phy->phy.init = nx_phy_init;
	nx_phy->phy.shutdown = nx_phy_shutdown;
	nx_phy->phy.set_suspend = nx_phy_suspend;
	nx_phy->phy.notify_connect = nx_phy_connect;
	nx_phy->phy.notify_disconnect = nx_phy_disconnect;
	nx_phy->phy.type = USB_PHY_TYPE_USB2;

	platform_set_drvdata(pdev, nx_phy);

	ret = usb_add_phy_dev(&nx_phy->phy);
	if (ret)
		return ret;

	/* usb need to set up hub sel */
	num = of_get_named_gpio(np, PHY_HUB, 0);
	if (num >= 0) {
		dev_info(&pdev->dev, "usb phy hub %d set\n", num);
		ret = devm_gpio_request(&pdev->dev, num, PHY_HUB);
		if (ret < 0) {
			dev_info(&pdev->dev, "phy hub request gpio err\n");
			return -EBUSY;
		}
		ret = gpio_direction_output(num, 1);
		if (ret < 0) {
			dev_info(&pdev->dev, "phy hub set value err\n");
			return -EBUSY;
		}
	}

	/* usb need to set up phy vbus */
	num = of_get_named_gpio(np, PHY_VBUS, 0);
	if (num >= 0) {
		dev_info(&pdev->dev, "usb phy vbus %d set\n", num);
		ret = devm_gpio_request(&pdev->dev, num, PHY_VBUS);
		if (ret < 0) {
			dev_info(&pdev->dev, "phy vbus request gpio err\n");
			return -EBUSY;
		}
		ret = gpio_direction_output(num, 1);
		if (ret < 0) {
			dev_info(&pdev->dev, "phy vbus set value err\n");
			return -EBUSY;
		}
	}

	return 0;
}

static int nx_phy_remove(struct platform_device *pdev)
{
	return 0;
}

/* power manager */
#ifdef CONFIG_PM_SLEEP
static int nx_phy_mod_suspend(struct device *dev)
{
	return 0;
}

static int nx_phy_mod_resume(struct device *dev)
{
	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(nx_phy_pm, nx_phy_mod_suspend, nx_phy_mod_resume);

/* driver init and exit */
static struct platform_driver nx_phy_driver = {
	.probe = nx_phy_probe,
	.remove = nx_phy_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = nx_phy_ids,
		.pm = &nx_phy_pm,
	},
};

static int __init nx_phy_module_init(void)
{
	pr_info("[usb-phy]: nx_phy_module_init\n");
	return platform_driver_register(&nx_phy_driver);
}
subsys_initcall(nx_phy_module_init);

static void __exit nx_phy_module_exit(void)
{
	platform_driver_unregister(&nx_phy_driver);
}
module_exit(nx_phy_module_exit);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech usb phy");
MODULE_LICENSE("GPL");

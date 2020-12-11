/*
 * Lombo usb controller core driver
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
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/idr.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>

#include "lb.h"
#include "bits.h"
#include "lombo_nx.h"
#include "otg.h"
#include "of.h"
#include "host.h"
#include "udc.h"
#include "db.h"

/* controller register map */
static const u8 lb_regs[] = {
	[CAP_CAPLENGTH]		= 0x00U,
	[CAP_HCCPARAMS]		= 0x08U,
	[CAP_DCCPARAMS]		= 0x24U,
	[OP_USBCMD]		= 0x00U,
	[OP_USBSTS]		= 0x04U,
	[OP_USBINTR]		= 0x08U,
	[OP_DEVICEADDR]		= 0x14U,
	[OP_ENDPTLISTADDR]	= 0x18U,
	[OP_BURSTSIZE]		= 0x20U,
	[OP_PORTSC]		= 0x44U,
	[OP_OTGSC]		= 0x64U,
	[OP_USBMODE]		= 0x68U,
	[OP_ENDPTSETUPSTAT]	= 0x6CU,
	[OP_ENDPTPRIME]		= 0x70U,
	[OP_ENDPTFLUSH]		= 0x74U,
	[OP_ENDPTSTAT]		= 0x78U,
	[OP_ENDPTCOMPLETE]	= 0x7CU,
	[OP_ENDPTCTRL]		= 0x80U,
};

static int lb_usb_phy_init(struct lb_hdc *lb);

static void hw_wait_phy_stable(void)
{
	/*
	 * The phy needs some delay to output the stable status from low
	 * power mode. And for OTGSC, the status inputs are debounced
	 * using a 1 ms time constant, so, delay 2ms for controller to get
	 * the stable status, like vbus and id when the phy leaves low power.
	 */
	usleep_range(2000, 2500);
}

static int hw_alloc_regmap(struct lb_hdc *lb)
{
	int i;

	for (i = 0; i < OP_ENDPTCTRL; i++)
		lb->hw_bank.regmap[i] =
			(i <= CAP_LAST ? lb->hw_bank.cap : lb->hw_bank.op) +
			lb_regs[i];

	for (; i <= OP_LAST; i++)
		lb->hw_bank.regmap[i] = lb->hw_bank.op +
			4 * (i - OP_ENDPTCTRL) + lb_regs[OP_ENDPTCTRL];

	return 0;
}

static enum lb_revision lb_get_revision(struct lb_hdc *lb)
{
	int ver = hw_read_id_reg(lb, ID_OS, VERSION) >> __ffs(VERSION);
	enum lb_revision rev = LB_REVISION_UNKNOWN;

	if (ver == 0x2) {
		rev = hw_read_id_reg(lb, ID_OS, REVISION) >> __ffs(REVISION);
		rev += LB_REVISION_20;
	} else if (ver == 0x0)
		rev = LB_REVISION_1X;

	return rev;
}

static void lb_set_ahb_port(void *base, bool is_sram)
{
	u32 val;
	void *reg = base + USBCTRL_OS;

	val = readl(reg);
	if (is_sram)
		val |= BIT(31);
	else
		val &= ~BIT(31);
	writel(val, reg);
	pr_info("[usb] usb ahb port %s\n", is_sram ? "sram" : "sdram");
}

/**
 * hw_read_intr_enable: returns interrupt enable register
 *
 * @lb: the controller
 *
 * returns register data
 */
u32 hw_read_intr_enable(struct lb_hdc *lb)
{
	return hw_read(lb, OP_USBINTR, ~0);
}

/**
 * hw_read_intr_status: returns interrupt status register
 *
 * @lb: the controller
 *
 * returns register data
 */
u32 hw_read_intr_status(struct lb_hdc *lb)
{
	return hw_read(lb, OP_USBSTS, ~0);
}

/**
 * hw_port_test_set: writes port test mode (execute without interruption)
 * @mode: new value
 *
 * returns an error code
 */
int hw_port_test_set(struct lb_hdc *lb, u8 mode)
{
	const u8 TEST_MODE_MAX = 7;

	if (mode > TEST_MODE_MAX)
		return -EINVAL;

	hw_write(lb, OP_PORTSC, PORTSC_PTC, mode << __ffs(PORTSC_PTC));
	return 0;
}

/**
 * hw_port_test_get: reads port test mode value
 *
 * @lb: the controller
 *
 * returns port test mode value
 */
u8 hw_port_test_get(struct lb_hdc *lb)
{
	return hw_read(lb, OP_PORTSC, PORTSC_PTC) >> __ffs(PORTSC_PTC);
}

/* The PHY enters/leaves low power mode */
static void lb_hdc_enter_lpm(struct lb_hdc *lb, bool enable)
{
	enum lb_hw_regs reg = OP_PORTSC;
	bool lpm = !!(hw_read(lb, reg, PORTSC_PHCD));

	dev_info(lb->dev, "%s lpm mode %s\n",
		 __func__, enable ? "enable" : "disable");

	if (enable && !lpm)
		hw_write(lb, reg, PORTSC_PHCD, PORTSC_PHCD);
	else if (!enable && lpm)
		hw_write(lb, reg, PORTSC_PHCD, 0);
}

static int hw_device_init(struct lb_hdc *lb, void __iomem *base)
{
	u32 reg;
	int ret;

	lb->hw_bank.abs = base;
	lb->hw_bank.cap = lb->hw_bank.abs + lb->platdata->capoffset;
	lb->hw_bank.op = lb->hw_bank.cap + (readl(lb->hw_bank.cap) & 0xff);
	dev_dbg(lb->dev, "abs addr: 0x%x, cap addr: 0x%x, op addr: 0x%x\n",
	       (u32)lb->hw_bank.abs, (u32)lb->hw_bank.cap, (u32)lb->hw_bank.op);

	hw_alloc_regmap(lb);
	lb->hw_bank.size = lb->hw_bank.op - lb->hw_bank.abs;
	lb->hw_bank.size += OP_LAST;
	lb->hw_bank.size /= sizeof(u32);

	lb->hw_bank.lpm = 0;
	ret = lb_usb_phy_init(lb);
	if (ret) {
		dev_err(lb->dev, "err to init phy: %d\n", ret);
		return ret;
	}

	/* device endpoint number */
	reg = hw_read(lb, CAP_DCCPARAMS, DCCPARAMS_DEN) >> __ffs(DCCPARAMS_DEN);
	lb->hw_ep_max = reg * 2;
	if (lb->hw_ep_max > EP_MAX)
		return -ENODEV;

	lb_hdc_enter_lpm(lb, false);

	/* disable all interrupts bits */
	hw_write(lb, OP_USBINTR, 0xffffffff, 0);

	/* clear all interrupts status bits*/
	hw_write(lb, OP_USBSTS, 0xffffffff, 0xffffffff);

	lb->rev = lb_get_revision(lb);

	return 0;
}

static void hw_phy_mode_config(struct lb_hdc *lb)
{
	u32 portsc;

#if defined(CONFIG_ARCH_LOMBO_N7V0_FPGA) || defined(CONFIG_ARCH_LOMBO_N9V0_FPGA)
	portsc = PORTSC_PTS(PTS_ULPI);
#else
	portsc = PORTSC_PTS(PTS_UTMI);
#endif
	hw_write(lb, OP_PORTSC, PORTSC_PTS(7), portsc);
}

/**
 * lb_usb_phy_init: initialize phy according to different phy type
 * @lb: the controller
 *
 * This function returns an error code if usb_phy_init has failed
 */
static int lb_usb_phy_init(struct lb_hdc *lb)
{
	int ret;

	/* phy select */
	hw_phy_mode_config(lb);

	/* set cts */
	ret = usb_phy_init(lb->usb_phy);

	return ret;
}

/**
 * lb_usb_phy_exit: deinitialize phy taking in account both phy and usb_phy
 * interfaces
 * @lb: the controller
 */
static void lb_usb_phy_exit(struct lb_hdc *lb)
{
	usb_phy_shutdown(lb->usb_phy);
}

/**
 * hw_controller_reset: do controller reset
 * @lb: the controller
  *
 * returns 0
 */
int hw_controller_reset(struct lb_hdc *lb)
{
	int count = 0;

	hw_write(lb, OP_USBCMD, USBCMD_RST, USBCMD_RST);
	while (hw_read(lb, OP_USBCMD, USBCMD_RST)) {
		udelay(10);
		if (count++ > 1000)
			return -ETIMEDOUT;
	}

	return 0;
}

/**
 * lb_hrdc_ahb_config: override default AHB configuration
 * @lb: the controller
 */
void lb_hdc_ahb_config(struct lb_hdc *lb)
{
	u32 value;
	u8 ahb_burst_size;
	bool ahb_burst = lb->platdata->flags & LB_HDC_OVERRIDE_AHB_BURST;
	bool burst_len = lb->platdata->flags & LB_HDC_OVERRIDE_BURST_LENGTH;

	/* AHB configuration */
	if (ahb_burst) {
		value = readl(lb->hw_bank.abs + SBUSCFG_OS);
		value &= ~SBUSCFG_AHBBRST;
		/* BAWR:0b'111 */
		value |= 0x7 << 3;
		/* BAWD:0b'111 */
		value |= 0x7 << 6;
		value |= lb->platdata->ahbburst_config & SBUSCFG_AHBBRST;
		writel(value, lb->hw_bank.abs + SBUSCFG_OS);
	}

	ahb_burst_size = readl(lb->hw_bank.abs + SBUSCFG_OS) & SBUSCFG_AHBBRST;
	dev_dbg(lb->dev, "burst size: %d, burst len: %s, burst_length: 0x%x\n",
		ahb_burst_size, burst_len ? "true" : "false",
		lb->platdata->burst_length);

	/* Change RX/TX burst size */
	if (ahb_burst_size == 0 && burst_len)
		hw_write(lb, OP_BURSTSIZE, BURST_BITS,
				lb->platdata->burst_length & BURST_BITS);
}

/**
 * hw_device_reset: resets chip (execute without interruption)
 * @lb: the controller
 */
int hw_device_reset(struct lb_hdc *lb)
{
	int ret;

	dev_dbg(lb->dev, "%s enter\n", __func__);
	/* should flush & stop before reset */
	hw_write(lb, OP_ENDPTFLUSH, ~0, ~0);
	hw_write(lb, OP_USBCMD, USBCMD_RS, 0);

	ret = hw_controller_reset(lb);
	if (ret) {
		dev_err(lb->dev, "error resetting controller, ret=%d\n", ret);
		return ret;
	}

	/* stream disable */
	if (lb->platdata->flags & LB_HDC_DISABLE_DEVICE_STREAMING)
		hw_write(lb, OP_USBMODE, USBMODE_CI_SDIS, USBMODE_CI_SDIS);

	/* transfer speed */
	if (lb->platdata->flags & LB_HDC_FORCE_FULLSPEED)
		hw_write(lb, OP_PORTSC, PORTSC_PFSC, PORTSC_PFSC);

	/* todo! note: for test, disable auto low power */
	hw_write(lb, OP_USBMODE, BIT(16), 0);

	/* USBMODE should be configured step by step */
	hw_write(lb, OP_USBMODE, USBMODE_CM, USBMODE_CM_IDLE);
	hw_write(lb, OP_USBMODE, USBMODE_CM, USBMODE_CM_DC);

	lb_hdc_ahb_config(lb);

	/*
	 * Set interrupt interval for device mode as 0 (immediately),
	 * ehci core will set it to 1 (1 Micro-frame) by default for host mode.
	 */
	hw_write(lb, OP_USBCMD, 0xff0000, 0);

	if (hw_read(lb, OP_USBMODE, USBMODE_CM) != USBMODE_CM_DC) {
		dev_err(lb->dev, "usb err: cannot set device mode failed\n");
		return -ENODEV;
	}

	return 0;
}

/**
 * hw_wait_reg: wait the register value
 *
 * Sometimes, it needs to wait register value before going on.
 * Eg, when switch to device mode, the vbus value should be lower
 * than OTGSC_BSV before connects to host.
 *
 * @lb: the controller
 * @reg: register index
 * @mask: mast bit
 * @value: the bit value to wait
 * @timeout_ms: timeout in millisecond
 *
 * This function returns an error code if timeout
 */
int hw_wait_reg(struct lb_hdc *lb, enum lb_hw_regs reg, u32 mask,
				u32 value, unsigned int timeout_ms)
{
	unsigned long elapse = jiffies + msecs_to_jiffies(timeout_ms);

	while (hw_read(lb, reg, mask) != value) {
		if (time_after(jiffies, elapse)) {
			dev_err(lb->dev, "timeout waiting for %08x in %d\n",
					mask, reg);
			return -ETIMEDOUT;
		}
		msleep(20);
	}

	return 0;
}

static irqreturn_t lb_irq(int irq, void *data)
{
	struct lb_hdc *lb = data;
	irqreturn_t ret = IRQ_NONE;
	u32 otgsc = 0;

	if (lb->in_lpm) {
		disable_irq_nosync(irq);
		lb->wakeup_int = true;
		pm_runtime_get(lb->dev);
		return IRQ_HANDLED;
	}

	if (lb->is_otg)
		otgsc = hw_read_otgsc(lb, ~0);

	/* id change interrupt, device/host function switch */
	if (lb->is_otg && (otgsc & OTGSC_IDIE) && (otgsc & OTGSC_IDIS)) {
		lb->id_event = true;
		/* Clear ID change irq status */
		hw_write_otgsc(lb, OTGSC_IDIS, OTGSC_IDIS);
		lb_otg_queue_work(lb);
		return IRQ_HANDLED;
	}

	/* vbus change interrupt,  device connection and disconnection events */
	if (lb->is_otg && (otgsc & OTGSC_BSVIE) && (otgsc & OTGSC_BSVIS)) {
		lb->vbus_glitch_check_event = true;
		/* Clear BSV irq */
		hw_write_otgsc(lb, OTGSC_BSVIS, OTGSC_BSVIS);
		lb_otg_queue_work(lb);
		return IRQ_HANDLED;
	}

	/* device/host interrupt */
	if (lb->role != LB_ROLE_END)
		ret = lb_role(lb)->irq(lb);

	return ret;
}

static int lb_get_platdata(struct device *dev,
			   struct lb_hdc_platform_data *platdata)
{
	if (!platdata->dr_mode)
		platdata->dr_mode = of_usb_get_dr_mode(dev->of_node);

	if (platdata->dr_mode == USB_DR_MODE_UNKNOWN)
		platdata->dr_mode = USB_DR_MODE_OTG;

	if (platdata->dr_mode != USB_DR_MODE_PERIPHERAL)
		/* todo!  use gpio as power controller? */

	if (of_usb_get_maximum_speed(dev->of_node) == USB_SPEED_FULL)
		platdata->flags |= LB_HDC_FORCE_FULLSPEED;

	return 0;
}

static DEFINE_IDA(lb_ida);

struct platform_device *lb_hdc_add_device(struct device *dev,
					struct resource *res, int nres,
					struct lb_hdc_platform_data *platdata)
{
	struct platform_device *pdev;
	int id, ret;

	ret = lb_get_platdata(dev, platdata);
	if (ret)
		return ERR_PTR(ret);

	id = ida_simple_get(&lb_ida, 0, 0, GFP_KERNEL);
	if (id < 0)
		return ERR_PTR(id);

	pdev = platform_device_alloc("lb_hdc", id);
	if (!pdev) {
		ret = -ENOMEM;
		goto put_id;
	}

	pdev->dev.parent = dev;
	pdev->dev.dma_mask = dev->dma_mask;
	pdev->dev.dma_parms = dev->dma_parms;
	dma_set_coherent_mask(&pdev->dev, dev->coherent_dma_mask);

	ret = platform_device_add_resources(pdev, res, nres);
	if (ret)
		goto err;

	ret = platform_device_add_data(pdev, platdata, sizeof(*platdata));
	if (ret)
		goto err;

	ret = platform_device_add(pdev);
	if (ret)
		goto err;

	return pdev;

err:
	platform_device_put(pdev);
put_id:
	ida_simple_remove(&lb_ida, id);
	return ERR_PTR(ret);
}
EXPORT_SYMBOL_GPL(lb_hdc_add_device);

void lb_hdc_remove_device(struct platform_device *pdev)
{
	int id = pdev->id;
	platform_device_unregister(pdev);
	ida_simple_remove(&lb_ida, id);
}
EXPORT_SYMBOL_GPL(lb_hdc_remove_device);

/**
 * lb_hdc_query_available_role: get runtime available operation mode
 *
 * The glue layer can get current operation mode (host/peripheral/otg)
 * This function should be called after core device has created.
 *
 * @pdev: the platform device of core.
 *
 * Return USB_DR_MODE_XXX.
 */
enum usb_dr_mode lb_hdc_query_available_role(struct platform_device *pdev)
{
	struct lb_hdc *lb = platform_get_drvdata(pdev);

	if (!lb)
		return USB_DR_MODE_UNKNOWN;
	if (lb->roles[LB_ROLE_HOST] && lb->roles[LB_ROLE_GADGET])
		return USB_DR_MODE_OTG;
	else if (lb->roles[LB_ROLE_HOST])
		return USB_DR_MODE_HOST;
	else if (lb->roles[LB_ROLE_GADGET])
		return USB_DR_MODE_PERIPHERAL;
	else
		return USB_DR_MODE_UNKNOWN;
}
EXPORT_SYMBOL_GPL(lb_hdc_query_available_role);

static inline void lb_role_destroy(struct lb_hdc *lb)
{
	if (lb->is_otg)
		lb_hdc_otg_destroy(lb);
	lb_hdc_gadget_destroy(lb);
	lb_hdc_host_destroy(lb);
}

static enum lb_role lb_get_role(struct lb_hdc *lb)
{
	if (lb->roles[LB_ROLE_HOST] && lb->roles[LB_ROLE_GADGET]) {
		if (lb->is_otg) {
			hw_write_otgsc(lb, OTGSC_IDIE, OTGSC_IDIE);
			return lb_otg_role(lb);
		} else
			return LB_ROLE_GADGET;
	} else
		return lb->roles[LB_ROLE_HOST] ? LB_ROLE_HOST : LB_ROLE_GADGET;
}

static void lb_start_new_role(struct lb_hdc *lb)
{
	enum lb_role role = lb_get_role(lb);

	if (lb->role != role)
		lb_handle_id_switch(lb);

	if (role == LB_ROLE_GADGET)
		lb_handle_vbus_connected(lb);
}

static void lb_power_lost_work(struct work_struct *work)
{
	struct lb_hdc *lb = container_of(work, struct lb_hdc, power_lost_work);

	pm_runtime_get_sync(lb->dev);
	lb_start_new_role(lb);
	pm_runtime_put_sync(lb->dev);
	enable_irq(lb->irq);
}

static int lb_hdc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct lb_hdc *lb;
	struct resource *res;
	void __iomem *base;
	int ret;
	enum usb_dr_mode dr_mode;

	if (!dev_get_platdata(dev)) {
		dev_err(dev, "err no platform data\n");
		return -ENODEV;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base)) {
		dev_err(dev, "err ioremap invalid\n");
		return PTR_ERR(base);
	}

	lb = devm_kzalloc(dev, sizeof(*lb), GFP_KERNEL);
	if (!lb) {
		dev_err(dev, "err alloc lb_hdc fail\n");
		return -ENOMEM;
	}

	lb->dev = dev;
	lb->platdata = dev_get_platdata(dev);
	lb->supports_runtime_pm = !!(lb->platdata->flags &
				     LB_HDC_SUPPORTS_RUNTIME_PM);

#if defined(CONFIG_ARCH_LOMBO_N7)
	/* ahb port select */
	lb_set_ahb_port(base, false);
#endif

	if (lb->platdata->phy) {
		lb->phy = lb->platdata->phy;
	} else if (lb->platdata->usb_phy) {
		lb->usb_phy = lb->platdata->usb_phy;
	} else {
		dev_err(dev, "err no usb phy\n\n");
		return -ENXIO;
	}

	/* after select phy mode, need to select phy */
	nx_phy_sel();

	ret = hw_device_init(lb, base);
	if (ret < 0) {
		dev_err(dev, "err initialize hardware\n");
		return -ENODEV;
	}

	lb->hw_bank.phys = res->start;

	lb->irq = platform_get_irq(pdev, 0);
	if (lb->irq < 0) {
		dev_err(dev, "err missing IRQ\n");
		ret = lb->irq;
		goto deinit_phy;
	}

	lb->is_otg = false;

	dr_mode = lb->platdata->dr_mode;

	/* initialize role(s) before the interrupt is requested */
	if (dr_mode == USB_DR_MODE_OTG || dr_mode == USB_DR_MODE_HOST) {
		ret = lb_hdc_host_init(lb);
		if (ret)
			dev_info(dev, "doesn't support host\n");
	}

	if (dr_mode == USB_DR_MODE_OTG || dr_mode == USB_DR_MODE_PERIPHERAL) {
		ret = lb_hdc_gadget_init(lb);
		if (ret)
			dev_info(dev, "doesn't support gadget\n");
	}

	if (!lb->roles[LB_ROLE_HOST] && !lb->roles[LB_ROLE_GADGET]) {
		dev_err(dev, "no supported roles\n");
		ret = -ENODEV;
		goto deinit_phy;
	}

	if (lb->is_otg && lb->roles[LB_ROLE_GADGET]) {
		dev_info(dev, "lb_hdc_otg_init\n");
		ret = lb_hdc_otg_init(lb);
		if (ret) {
			dev_err(dev, "init otg fails, ret = %d\n", ret);
			goto stop;
		}
	}

	lb->role = lb_get_role(lb);
	/* only update vbus status for peripheral */
	if (lb->role == LB_ROLE_GADGET)
		lb_handle_vbus_connected(lb);

	/* host or deviceÓ²¼þ³õÊ¼»¯ */
	ret = lb_role_start(lb, lb->role);
	if (ret) {
		dev_err(dev, "can't start %s role\n", lb_role(lb)->name);
		goto stop;
	}

	platform_set_drvdata(pdev, lb);
	ret = devm_request_irq(dev, lb->irq, lb_irq, IRQF_SHARED,
				lb->platdata->name, lb);
	if (ret)
		goto stop;

	if (lb->supports_runtime_pm) {
		pm_runtime_set_active(&pdev->dev);
		pm_runtime_enable(&pdev->dev);
		pm_runtime_set_autosuspend_delay(&pdev->dev, 2000);
		pm_runtime_mark_last_busy(lb->dev);
		pm_runtime_use_autosuspend(&pdev->dev);
	}

	device_set_wakeup_capable(&pdev->dev, true);
	/* todo! enable after system wakeup support ok */
	/* device_wakeup_enable(&pdev->dev); */

	/* Init workqueue for controller power lost handling */
	INIT_WORK(&lb->power_lost_work, lb_power_lost_work);

	ret = dbg_create_files(lb);
	if (!ret)
		return 0;

stop:
	lb_role_destroy(lb);
deinit_phy:
	lb_usb_phy_exit(lb);

	return ret;
}

static int lb_hdc_remove(struct platform_device *pdev)
{
	struct lb_hdc *lb = platform_get_drvdata(pdev);

	if (lb->supports_runtime_pm) {
		pm_runtime_get_sync(&pdev->dev);
		pm_runtime_disable(&pdev->dev);
		pm_runtime_put_noidle(&pdev->dev);
	}

	dbg_remove_files(lb);
	lb_role_destroy(lb);
	lb_hdc_enter_lpm(lb, true);
	lb_usb_phy_exit(lb);

	return 0;
}

#ifdef CONFIG_PM

static void lb_controller_suspend(struct lb_hdc *lb)
{
	disable_irq(lb->irq);
	lb_hdc_enter_lpm(lb, true);
	usleep_range(3000, 3500);
	usb_phy_set_suspend(lb->usb_phy, 1);
	lb->in_lpm = true;
	enable_irq(lb->irq);
}

static int lb_controller_resume(struct device *dev)
{
	struct lb_hdc *lb = dev_get_drvdata(dev);

	if (!lb->in_lpm) {
		WARN_ON(1);
		return 0;
	}

	lb_hdc_enter_lpm(lb, false);
	if (lb->usb_phy) {
		usb_phy_set_suspend(lb->usb_phy, 0);
		hw_wait_phy_stable();
	}

	lb->in_lpm = false;
	if (lb->wakeup_int) {
		lb->wakeup_int = false;
		pm_runtime_mark_last_busy(lb->dev);
		pm_runtime_put_autosuspend(lb->dev);
		enable_irq(lb->irq);
	}

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int lb_suspend(struct device *dev)
{
	struct lb_hdc *lb = dev_get_drvdata(dev);

	if (lb->wq)
		flush_workqueue(lb->wq);

	/*
	 * controller needs to be active during suspend, otherwise the core
	 * may run resume when the parent is at suspend if other driver's
	 * suspend fails, it occurs before parent's suspend has not started,
	 * but the core suspend has finished.
	 */
	if (lb->in_lpm)
		pm_runtime_resume(dev);

	if (lb->in_lpm) {
		WARN_ON(1);
		return 0;
	}

	/* extra routine per role before system suspend */
	if (lb->role != LB_ROLE_END && lb_role(lb)->suspend)
		lb_role(lb)->suspend(lb);

	if (device_may_wakeup(dev))
		enable_irq_wake(lb->irq);

	lb_controller_suspend(lb);

	return 0;
}

static int lb_resume(struct device *dev)
{
	struct lb_hdc *lb = dev_get_drvdata(dev);
	bool power_lost = false;
	u32 sample_reg_val;
	int ret;

	/* check if controller resume from power lost */
	sample_reg_val = hw_read(lb, OP_ENDPTLISTADDR, ~0);
	if (sample_reg_val == 0)
		power_lost = true;
	else if (sample_reg_val == 0xFFFFFFFF)
		/* Restore value 0 if it was set for power lost check */
		hw_write(lb, OP_ENDPTLISTADDR, ~0, 0);

	if (device_may_wakeup(dev))
		disable_irq_wake(lb->irq);

	ret = lb_controller_resume(dev);
	if (ret)
		return ret;

	if (power_lost) {
#if defined(CONFIG_ARCH_LOMBO_N7)
		/* after deepsleep, have to select sdram */
		lb_set_ahb_port(lb->hw_bank.abs, false);
#endif
		/* shutdown and re-init for phy */
		/*lb_usb_phy_exit(lb);*/
		lb_usb_phy_init(lb);
	}

	/* extra routine per role after system resume */
	if (lb->role != LB_ROLE_END && lb_role(lb)->resume)
		lb_role(lb)->resume(lb, power_lost);

	if (power_lost) {
		disable_irq_nosync(lb->irq);
		schedule_work(&lb->power_lost_work);
	}

	if (lb->supports_runtime_pm) {
		pm_runtime_disable(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
	}

	return ret;
}
#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_PM_RUNTIME
static int lb_runtime_suspend(struct device *dev)
{
	struct lb_hdc *lb = dev_get_drvdata(dev);

	if (lb->in_lpm) {
		WARN_ON(1);
		return 0;
	}

	lb_controller_suspend(lb);

	return 0;
}

static int lb_runtime_resume(struct device *dev)
{
	return lb_controller_resume(dev);
}
#endif /* CONFIG_PM_RUNTIME */

#endif /* CONFIG_PM */

static const struct dev_pm_ops lb_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(lb_suspend, lb_resume)
	SET_RUNTIME_PM_OPS(lb_runtime_suspend, lb_runtime_resume, NULL)
};

static struct platform_driver lb_hdc_driver = {
	.probe = lb_hdc_probe,
	.remove = lb_hdc_remove,
	.driver = {
		.name = "lb_hdc",
		.pm = &lb_pm_ops,
		.owner = THIS_MODULE,
	},
};

static int __init lb_hdc_init(void)
{
	return platform_driver_register(&lb_hdc_driver);
}

subsys_initcall_sync(lb_hdc_init);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech usb core");
MODULE_LICENSE("GPL");

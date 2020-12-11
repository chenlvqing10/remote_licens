/*
 * lombo usb host driver interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/platform_device.h>

#include "../host/ehci.h"
#include "lombo_nx.h"
#include "lb.h"
#include "bits.h"
#include "host.h"

#define NX_LB_NUM	8
static struct hc_driver __read_mostly lb_ehci_hc_driver[NX_LB_NUM];
static int (*orig_bus_suspend)(struct usb_hcd *hcd);
static int (*orig_bus_resume)(struct usb_hcd *hcd);
static int (*orig_hub_control)(struct usb_hcd *hcd, u16 type_req, u16 wvalue,
				u16 windex, char *buf, u16 wlength);

/* override WKCN, WKDN, and WKOC */
static void lb_ehci_override_wakeup_flag(struct ehci_hcd *ehci,
					 u32 __iomem *reg, u32 flag, bool set)
{
	u32 val = ehci_readl(ehci, reg);
	if (set)
		val |= flag;
	else
		val &= ~flag;

	ehci_writel(ehci, val, reg);
}

/* todo! power control: pin? */
static const struct ehci_driver_overrides ehci_lb_overrides = {

};

static int lb_nx_ehci_bus_resume(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int port;
	int ret = orig_bus_resume(hcd);

	if (ret)
		return ret;

	port = HCS_N_PORTS(ehci->hcs_params);
	while (port--) {
		u32 __iomem *reg = &ehci->regs->port_status[port];
		u32 portsc = ehci_readl(ehci, reg);
		/*
		 * Notify PHY after resume signal has finished, it is
		 * for global suspend case.
		 */
		if (hcd->phy && test_bit(port, &ehci->bus_suspended) &&
		    (portsc & PORT_CONNECT) &&
		    (ehci_port_speed(ehci, portsc) == USB_PORT_STAT_HIGH_SPEED))
			/* notify the USB PHY */
			ehci_dbg(ehci, "notify usb phy resume\n");
	}

	return 0;
}

#ifdef CONFIG_USB_OTG
static int lb_start_port_reset(struct usb_hcd *hcd, unsigned port)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	u32 __iomem *reg;
	u32 status;

	if (!port)
		return -EINVAL;
	port--;
	/* start port reset before HNP protocol time out */
	reg = &ehci->regs->port_status[port];
	status = ehci_readl(ehci, reg);
	if (!(status & PORT_CONNECT))
		return -ENODEV;

	/* khubd will finish the reset later */
	if (ehci_is_TDI(ehci))
		ehci_writel(ehci, status | (PORT_RESET & ~PORT_RWC_BITS), reg);
	else
		ehci_writel(ehci, status | PORT_RESET, reg);

	return 0;
}

#else
#define lb_start_port_reset	NULL

#endif /* CONFIG_USB_OTG */

static int lb_nx_ehci_hub_control(struct usb_hcd *hcd, u16 type_req, u16 wvalue,
				  u16 windex, char *buf, u16 wlength)
{
	struct ehci_hcd	*ehci = hcd_to_ehci(hcd);
	u32 __iomem	*status_reg;
	u32		temp;
	unsigned long	flags;
	int		retval = 0;
	struct device *dev = hcd->self.controller;
	struct lb_hdc *lb = dev_get_drvdata(dev);

	status_reg = &ehci->regs->port_status[(windex & 0xff) - 1];

	spin_lock_irqsave(&ehci->lock, flags);

	if (type_req == SetPortFeature && wvalue == USB_PORT_FEAT_SUSPEND) {
		temp = ehci_readl(ehci, status_reg);
		if ((temp & PORT_PE) == 0 || (temp & PORT_RESET) != 0) {
			retval = -EPIPE;
			goto done;
		}

		temp &= ~(PORT_RWC_BITS | PORT_WKCONN_E);
		temp |= PORT_WKDISC_E | PORT_WKOC_E;
		ehci_writel(ehci, temp | PORT_SUSPEND, status_reg);

		/*
		 * If a transaction is in progress, there may be a delay in
		 * suspending the port. Poll until the port is suspended.
		 */
		if (handshake(ehci, status_reg, PORT_SUSPEND, PORT_SUSPEND,
				   5000))
			ehci_err(ehci, "timeout waiting for SUSPEND\n");

		if (lb->platdata->flags & LB_HDC_NX_IS_HSIC) {
			if (lb->platdata->notify_event)
				lb->platdata->notify_event(lb,
						LB_HDC_NX_HSIC_SUSPEND_EVENT);
			lb_ehci_override_wakeup_flag(ehci, status_reg,
				PORT_WKDISC_E | PORT_WKCONN_E, false);
		}

		spin_unlock_irqrestore(&ehci->lock, flags);
		if (ehci_port_speed(ehci, temp) == USB_PORT_STAT_HIGH_SPEED &&
		    hcd->phy) {
			/* notify the USB PHY */
			/* usb_phy_notify_suspend(hcd->phy, USB_SPEED_HIGH); */
		}
		spin_lock_irqsave(&ehci->lock, flags);

		set_bit((windex & 0xff) - 1, &ehci->suspended_ports);
		goto done;
	} else if (type_req == ClearPortFeature &&
		   wvalue == USB_PORT_FEAT_C_SUSPEND) {
		/* Make sure the resume has finished, it should be finished */
		if (handshake(ehci, status_reg, PORT_RESUME, 0, 25000))
			ehci_err(ehci, "timeout waiting for resume\n");

		temp = ehci_readl(ehci, status_reg);

		if (ehci_port_speed(ehci, temp) ==
				USB_PORT_STAT_HIGH_SPEED && hcd->phy) {
			/* notify the USB PHY */
			/* usb_phy_notify_resume(hcd->phy, USB_SPEED_HIGH); */
		}
	}

	spin_unlock_irqrestore(&ehci->lock, flags);

	/* Handle the hub control events here */
	return orig_hub_control(hcd, type_req, wvalue, windex, buf, wlength);
done:
	spin_unlock_irqrestore(&ehci->lock, flags);
	return retval;
}

static irqreturn_t host_irq(struct lb_hdc *lb)
{
	if (lb->hcd)
		return usb_hcd_irq(lb->irq, lb->hcd);
	else
		return IRQ_NONE;
}

static int host_start(struct lb_hdc *lb)
{
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	int ret;
	u32 usbmode;
	struct platform_device *pdev = to_platform_device(lb->dev);

	if (usb_disabled())
		return -ENODEV;

	hcd = usb_create_hcd(&lb_ehci_hc_driver[pdev->id], lb->dev,
				dev_name(lb->dev));

	if (!hcd) {
		dev_err(lb->dev, "err create usb hcd\n");
		return -ENOMEM;
	}

	dev_set_drvdata(lb->dev, lb);
	hcd->rsrc_start = lb->hw_bank.phys;
	hcd->rsrc_len = lb->hw_bank.size;
	hcd->regs = lb->hw_bank.abs;
	hcd->has_tt = 1;
	hcd->power_budget = lb->platdata->power_budget;

	if (lb->usb_phy)
		hcd->phy = lb->usb_phy;
	else
		dev_err(lb->dev, "err: no usb host phy\n");

	ehci = hcd_to_ehci(hcd);
	ehci->caps = lb->hw_bank.cap;
	ehci->has_hostpc = lb->hw_bank.lpm;

	ret = usb_add_hcd(hcd, 0, 0);
	if (ret)
		goto put_hcd;
	else
		lb->hcd = hcd;

	/* enable port connect */
	usbmode = ehci_readl(ehci, &ehci->regs->usbmode);
	usbmode |= (USBMODE_EX_VBPS | USBMODE_SF_CON);
	ehci_writel(ehci, usbmode, &ehci->regs->usbmode);

	if (lb->platdata->notify_event &&
		(lb->platdata->flags & LB_HDC_NX_IS_HSIC))
		lb->platdata->notify_event
			(lb, LB_HDC_NX_HSIC_ACTIVE_EVENT);

	lb_hdc_ahb_config(lb);

	return ret;

put_hcd:
	usb_put_hcd(hcd);
	return ret;
}

static void host_stop(struct lb_hdc *lb)
{
	struct usb_hcd *hcd = lb->hcd;

	if (hcd) {
		usb_remove_hcd(hcd);
		usb_put_hcd(hcd);
	}
	lb->hcd = NULL;
}

bool lb_hdc_host_has_device(struct lb_hdc *lb)
{
	struct usb_device *roothub;
	int i;

	if ((lb->role == LB_ROLE_HOST) && lb->hcd) {
		roothub = lb->hcd->self.root_hub;
		for (i = 0; i < roothub->maxchild; ++i) {
			if (usb_hub_find_child(roothub, (i + 1)))
				return true;
		}
	}
	return false;
}

static void lb_hdc_host_save_for_power_lost(struct lb_hdc *lb)
{
	struct ehci_hcd *ehci;

	if (!lb->hcd)
		return;

	ehci = hcd_to_ehci(lb->hcd);
	/* save EHCI registers */
	lb->pm_usbmode = ehci_readl(ehci, &ehci->regs->usbmode);
	lb->pm_command = ehci_readl(ehci, &ehci->regs->command);
	lb->pm_command &= ~CMD_RUN;
	lb->pm_status = ehci_readl(ehci, &ehci->regs->status);
	lb->pm_intr_enable = ehci_readl(ehci, &ehci->regs->intr_enable);
	lb->pm_frame_index = ehci_readl(ehci, &ehci->regs->frame_index);
	lb->pm_segment = ehci_readl(ehci, &ehci->regs->segment);
	lb->pm_frame_list = ehci_readl(ehci, &ehci->regs->frame_list);
	lb->pm_async_next = ehci_readl(ehci, &ehci->regs->async_next);
	lb->pm_configured_flag = ehci_readl(ehci, &ehci->regs->configured_flag);
	lb->pm_portsc = ehci_readl(ehci, &ehci->regs->port_status[0]);
}

static void lb_hdc_host_restore_from_power_lost(struct lb_hdc *lb)
{
	struct ehci_hcd *ehci;
	unsigned long   flags;
	u32 tmp;

	if (!lb->hcd)
		return;

	hw_controller_reset(lb);

	ehci = hcd_to_ehci(lb->hcd);
	spin_lock_irqsave(&ehci->lock, flags);
	/* Restore EHCI registers */
	ehci_writel(ehci, lb->pm_usbmode, &ehci->regs->usbmode);
	ehci_writel(ehci, lb->pm_portsc, &ehci->regs->port_status[0]);
	ehci_writel(ehci, lb->pm_command, &ehci->regs->command);
	ehci_writel(ehci, lb->pm_intr_enable, &ehci->regs->intr_enable);
	ehci_writel(ehci, lb->pm_frame_index, &ehci->regs->frame_index);
	ehci_writel(ehci, lb->pm_segment, &ehci->regs->segment);
	ehci_writel(ehci, lb->pm_frame_list, &ehci->regs->frame_list);
	ehci_writel(ehci, lb->pm_async_next, &ehci->regs->async_next);
	ehci_writel(ehci, lb->pm_configured_flag, &ehci->regs->configured_flag);
	/* Restore the PHY's connect notifier setting */
	if (lb->pm_portsc & PORTSC_HSP)
		usb_phy_notify_connect(lb->usb_phy, USB_SPEED_HIGH);

	lb_hdc_ahb_config(lb);

	tmp = ehci_readl(ehci, &ehci->regs->command);
	tmp |= CMD_RUN;
	ehci_writel(ehci, tmp, &ehci->regs->command);
	spin_unlock_irqrestore(&ehci->lock, flags);
}

static void lb_hdc_host_suspend(struct lb_hdc *lb)
{
	/* todo! if host has device, need to save irq? */
	lb_hdc_host_save_for_power_lost(lb);
}

static void lb_hdc_host_resume(struct lb_hdc *lb, bool power_lost)
{
	if (power_lost)
		lb_hdc_host_restore_from_power_lost(lb);
}

void lb_hdc_host_destroy(struct lb_hdc *lb)
{
	if (lb->role == LB_ROLE_HOST && lb->hcd) {
		disable_irq_nosync(lb->irq);
		host_stop(lb);
		enable_irq(lb->irq);
	}
}

static int lb_ehci_bus_suspend(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	struct device *dev = hcd->self.controller;
	struct lb_hdc *lb = dev_get_drvdata(dev);
	int port;
	u32 tmp;
	int ret = orig_bus_suspend(hcd);
	if (ret) {
		dev_err(dev, "hcd orig bus suspend failed\n");
		return ret;
	}

	port = HCS_N_PORTS(ehci->hcs_params);
	while (port--) {
		u32 __iomem *reg = &ehci->regs->port_status[port];
		u32 portsc = ehci_readl(ehci, reg);
		if (port & PORT_CONNECT) {
			/*
			 * the resume signal will be ended automatically,
			 * so for remote wakeup case, the usbcmd.rs may not be
			 * set before the resume has ended if other resume
			 * paths consumes too much time (~24ms), in that case,
			 * the SOF will not send out within 3ms after resume
			 * ends, then the high speed device will enter
			 * full speed mode.
			 */
			tmp = ehci_readl(ehci, &ehci->regs->command);
			tmp |= CMD_RUN;
			ehci_writel(ehci, tmp, &ehci->regs->command);

			/* needs a short delay between set RS bit and PHCD */
			usleep_range(150, 200);

			/*
			 * If a transaction is in progress, there may be a
			 * delay in suspending the port.
			 * Poll until the port is suspended.
			 */
			if (test_bit(port, &ehci->bus_suspended) &&
			    handshake(ehci, reg, PORT_SUSPEND,
					   PORT_SUSPEND, 5000))
				ehci_err(ehci, "timeout waiting for SUSPEND\n");

			if (lb->platdata->flags & LB_HDC_NX_IS_HSIC)
				lb_ehci_override_wakeup_flag(ehci, reg,
					PORT_WKDISC_E | PORT_WKCONN_E, false);

			if (hcd->phy && test_bit(port, &ehci->bus_suspended)
				&& (ehci_port_speed(ehci, portsc) ==
					USB_PORT_STAT_HIGH_SPEED))
				/*
				 * notify the USB PHY, it is for global
				 * suspend case.
				 */
				/* usb_phy_notify_suspend(hcd->phy,
					USB_SPEED_HIGH); */
			break;
		}
	}
	return 0;
}

int lb_hdc_host_init(struct lb_hdc *lb)
{
	struct lb_role_driver *rdrv;
	struct platform_device *pdev = to_platform_device(lb->dev);
	struct hc_driver *lb_ehci_driver = &lb_ehci_hc_driver[pdev->id];

	if (!hw_read(lb, CAP_DCCPARAMS, DCCPARAMS_HC)) {
		dev_err(&pdev->dev, "%s no support host\n", __func__);
		return -ENXIO;
	}

	rdrv = devm_kzalloc(lb->dev, sizeof(struct lb_role_driver), GFP_KERNEL);
	if (!rdrv) {
		dev_err(&pdev->dev, "%s alloc role driver failed\n", __func__);
		return -ENOMEM;
	}

	rdrv->start	= host_start;
	rdrv->stop	= host_stop;
	rdrv->irq	= host_irq;
	rdrv->suspend	= lb_hdc_host_suspend;
	rdrv->resume	= lb_hdc_host_resume;
	rdrv->name	= "host";
	lb->roles[LB_ROLE_HOST] = rdrv;

	ehci_init_driver(lb_ehci_driver, &ehci_lb_overrides);
	orig_bus_suspend = lb_ehci_driver->bus_suspend;
	orig_bus_resume = lb_ehci_driver->bus_resume;
	orig_hub_control = lb_ehci_driver->hub_control;

	lb_ehci_driver->bus_suspend = lb_ehci_bus_suspend;
	if (lb->platdata->flags & LB_HDC_NX_EHCI_QUIRK) {
		lb_ehci_driver->bus_resume = lb_nx_ehci_bus_resume;
		lb_ehci_driver->hub_control = lb_nx_ehci_hub_control;
	}
	lb_ehci_driver->start_port_reset = lb_start_port_reset;

	return 0;
}

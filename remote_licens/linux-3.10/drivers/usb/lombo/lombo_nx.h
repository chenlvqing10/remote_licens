/*
 * platform data for lombo usb controller
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LOMBO_USB_NX_PLATFORM_H
#define __LOMBO_USB_NX_PLATFORM_H

#include <linux/usb/otg.h>

#include "lb.h"

struct lb_hdc;
struct lb_hdc_platform_data {
	const char				*name;
	/* offset of the capability registers */
	uintptr_t				capoffset;
	unsigned				power_budget;
	struct phy				*phy;
	/* old usb_phy interface */
	struct usb_phy				*usb_phy;
	unsigned long				flags;
#define LB_HDC_REGS_SHARED			BIT(0)
#define LB_HDC_SUPPORTS_RUNTIME_PM		BIT(2)
	/*
	 * only set it when DCCPARAMS.DC==1 and DCCPARAMS.HC==1,
	 * but otg is not supported (no register otgsc).
	 */
#define LB_HDC_DUAL_ROLE_NOT_OTG		BIT(4)
#define LB_HDC_FORCE_FULLSPEED			BIT(5)
#define LB_HDC_NX_EHCI_QUIRK			BIT(6)
#define LB_HDC_NX_IS_HSIC			BIT(7)
#define LB_HDC_DISABLE_DEVICE_STREAMING		BIT(8)
#define LB_HDC_DISABLE_HOST_STREAMING		BIT(9)
#define LB_HDC_DISABLE_STREAMING (LB_HDC_DISABLE_DEVICE_STREAMING |	\
		LB_HDC_DISABLE_HOST_STREAMING)
#define LB_HDC_OVERRIDE_AHB_BURST		BIT(10)
#define LB_HDC_OVERRIDE_BURST_LENGTH		BIT(11)
#define LB_HDC_VBUS_EARLY_ON			BIT(12)

	enum usb_dr_mode	dr_mode;
#define LB_HDC_CONTROLLER_RESET_EVENT		0
#define LB_HDC_CONTROLLER_STOPPED_EVENT		1
#define LB_HDC_CONTROLLER_VBUS_EVENT		2
#define LB_HDC_NOTIFY_RET_DEFER_EVENT		3
#define LB_HDC_CONTROLLER_CHARGER_POST_EVENT	4
#define LB_HDC_NX_HSIC_ACTIVE_EVENT		5
#define LB_HDC_NX_HSIC_SUSPEND_EVENT		6
#define LB_HDC_NX_ADP_PROBE_ENABLE		7
#define LB_HDC_NX_ADP_PROBE_START		8
#define LB_HDC_NX_ADP_SENSE_ENABLE		9
#define LB_HDC_NX_ADP_IS_PROBE_INT		10
#define LB_HDC_NX_ADP_IS_SENSE_INT		11
#define LB_HDC_NX_ADP_SENSE_CONNECTION		12
#define LB_HDC_NX_ADP_ATTACH_EVENT		13
	int (*notify_event)(struct lb_hdc *lb, unsigned event);
	bool					tpl_support;
	u32					ahbburst_config;
	u32					burst_length;
};

/* default offset of capability registers */
#define DEF_CAPOFFSET				0x100

/* add lb hdc device */
struct platform_device *lb_hdc_add_device(struct device *dev,
			struct resource *res, int nres,
			struct lb_hdc_platform_data *platdata);
/* remove lb hdc device */
void lb_hdc_remove_device(struct platform_device *pdev);

/* phy0 phy1 select */
void nx_phy_sel(void);

/* phy sources reset */
void nx_phy_reset(int enable);

/* get current available role */
enum usb_dr_mode lb_hdc_query_available_role(struct platform_device *pdev);

#endif /* __LOMBO_USB_NX_PLATFORM_H */

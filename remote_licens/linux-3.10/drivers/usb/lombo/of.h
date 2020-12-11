/*
 * lombo usb of helpers
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef LOMBO_USB_OF__H
#define LOMBO_USB_OF__H

#include <linux/usb/ch9.h>
#include <linux/usb/otg.h>
#include <linux/usb/phy.h>

#include "lb.h"

#if IS_ENABLED(CONFIG_OF)
enum usb_dr_mode of_usb_get_dr_mode(struct device_node *np);
enum usb_device_speed of_usb_get_maximum_speed(struct device_node *np);
#else
static inline enum usb_dr_mode of_usb_get_dr_mode(struct device_node *np)
{
	return USB_DR_MODE_UNKNOWN;
}

static inline enum usb_device_speed
of_usb_get_maximum_speed(struct device_node *np)
{
	return USB_SPEED_UNKNOWN;
}

#endif

#endif /* LOMBO_USB_OF__H */

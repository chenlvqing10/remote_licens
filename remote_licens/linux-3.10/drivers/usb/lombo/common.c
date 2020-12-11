/*
 * lombo usb common interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/usb/ch9.h>
#include <linux/usb/otg.h>

#include "of.h"
#include "lb.h"

static const char *const speed_names[] = {
	[USB_SPEED_UNKNOWN] = "UNKNOWN",
	[USB_SPEED_LOW] = "low-speed",
	[USB_SPEED_FULL] = "full-speed",
	[USB_SPEED_HIGH] = "high-speed",
	[USB_SPEED_WIRELESS] = "wireless",
	[USB_SPEED_SUPER] = "super-speed",
};

#ifdef CONFIG_OF
static const char *const usb_dr_modes[] = {
	[USB_DR_MODE_UNKNOWN]		= "",
	[USB_DR_MODE_HOST]		= "host",
	[USB_DR_MODE_PERIPHERAL]	= "peripheral",
	[USB_DR_MODE_OTG]		= "otg",
};

/**
 * of_usb_get_dr_mode - Get dual role mode for given device_node
 * @np:	Pointer to the given device_node
 *
 * The function gets phy interface string from property 'dr_mode',
 * and returns the correspondig enum usb_dr_mode
 */
enum usb_dr_mode of_usb_get_dr_mode(struct device_node *np)
{
	const char *dr_mode;
	int err, i;

	err = of_property_read_string(np, "dr_mode", &dr_mode);
	if (err < 0)
		return USB_DR_MODE_UNKNOWN;

	for (i = 0; i < ARRAY_SIZE(usb_dr_modes); i++)
		if (!strcmp(dr_mode, usb_dr_modes[i]))
			return i;

	return USB_DR_MODE_UNKNOWN;
}
EXPORT_SYMBOL_GPL(of_usb_get_dr_mode);

/**
 * of_usb_get_maximum_speed - Get maximum requested speed for a given USB
 * controller.
 * @np: Pointer to the given device_node
 *
 * The function gets the maximum speed string from property "maximum-speed",
 * and returns the corresponding enum usb_device_speed.
 */
enum usb_device_speed of_usb_get_maximum_speed(struct device_node *np)
{
	const char *maximum_speed;
	int err;
	int i;

	err = of_property_read_string(np, "maximum-speed", &maximum_speed);
	if (err < 0)
		return USB_SPEED_UNKNOWN;

	for (i = 0; i < ARRAY_SIZE(speed_names); i++)
		if (strcmp(maximum_speed, speed_names[i]) == 0)
			return i;

	return USB_SPEED_UNKNOWN;
}
EXPORT_SYMBOL_GPL(of_usb_get_maximum_speed);

#endif

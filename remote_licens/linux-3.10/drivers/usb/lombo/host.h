/*
 * lombo usb host driver interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef LOMBO_USB_HOST_DRIVER__H
#define LOMBO_USB_HOST_DRIVER__H

#ifdef CONFIG_USB_LOMBO_HOST
int lb_hdc_host_init(struct lb_hdc *lb);
void lb_hdc_host_destroy(struct lb_hdc *lb);
bool lb_hdc_host_has_device(struct lb_hdc *lb);

#else
static inline int lb_hdc_host_init(struct lb_hdc *lb)
{
	return -ENXIO;
}

static inline void lb_hdc_host_destroy(struct lb_hdc *lb)
{

}

static inline bool lb_hdc_host_has_device(struct lb_hdc *lb)
{
	return false;
}

#endif

#endif /* LOMBO_USB_HOST_DRIVER__H */

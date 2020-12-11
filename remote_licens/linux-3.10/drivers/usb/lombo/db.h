/*
 * lombo usb driver debug interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef LOMBO_USB_DEBUG__H
#define LOMBO_USB_DEBUG__H

#ifdef CONFIG_USB_LOMBO_DEBUG
int dbg_create_files(struct lb_hdc *lb);
void dbg_remove_files(struct lb_hdc *lb);
#else
static inline int dbg_create_files(struct lb_hdc *lb)
{
	return 0;
}

static inline void dbg_remove_files(struct lb_hdc *lb)
{
}

#endif /* CONFIG_USB_LOMBO_DEBUG */

#endif /* LOMBO_USB_DEBUG__H */

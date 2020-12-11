/*
 * lombo usb driver otg interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef LOMBO_USB_OTG__H
#define LOMBO_USB_OTG__H

u32 hw_read_otgsc(struct lb_hdc *lb, u32 mask);
void hw_write_otgsc(struct lb_hdc *lb, u32 mask, u32 data);
int lb_hdc_otg_init(struct lb_hdc *lb);
void lb_hdc_otg_destroy(struct lb_hdc *lb);
enum lb_role lb_otg_role(struct lb_hdc *lb);
void lb_handle_vbus_change(struct lb_hdc *lb);
void lb_handle_id_switch(struct lb_hdc *lb);
void lb_handle_vbus_connected(struct lb_hdc *lb);
static inline void lb_otg_queue_work(struct lb_hdc *lb)
{
	if (lb->wq) {
		disable_irq_nosync(lb->irq);
		if (!queue_work(lb->wq, &lb->work))
			enable_irq(lb->irq);
	} else {
		WARN_ON(!lb->wq);
	}
}

#endif /* LOMBO_USB_OTG__H */

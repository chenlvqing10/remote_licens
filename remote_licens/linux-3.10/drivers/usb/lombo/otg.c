/*
 * lombo usb driver otg interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * This file mainly handles otgsc register, OTG fsm operations for HNP and SRP
 * are also included.
 */

#include <linux/usb/otg.h>
#include <linux/usb/gadget.h>

#include "lb.h"
#include "bits.h"
#include "otg.h"
#include "host.h"

/* get otgsc register bits value */
u32 hw_read_otgsc(struct lb_hdc *lb, u32 mask)
{
	return hw_read(lb, OP_OTGSC, mask);
}

/* updates target bits of OTGSC register */
void hw_write_otgsc(struct lb_hdc *lb, u32 mask, u32 data)
{
	hw_write(lb, OP_OTGSC, mask | OTGSC_INT_STATUS_BITS, data);
}

/* pick role based on ID pin state */
enum lb_role lb_otg_role(struct lb_hdc *lb)
{
	enum lb_role role = hw_read_otgsc(lb, OTGSC_ID)
		? LB_ROLE_GADGET
		: LB_ROLE_HOST;

	return role;
}

/*
 * Handling vbus glitch
 * We only need to consider glitch for without usb connection,
 * With usb connection, we consider it as real disconnection.
 *
 * If the vbus can't be kept above B session valid for timeout value,
 * we think it is a vbus glitch, otherwise it's a valid vbus.
 */
#define LB_VBUS_CONNECT_TIMEOUT_MS 300
static int lb_is_vbus_glitch(struct lb_hdc *lb)
{
	int i;
	for (i = 0; i < LB_VBUS_CONNECT_TIMEOUT_MS/20; i++) {
		if (hw_read_otgsc(lb, OTGSC_AVV)) {
			return 0;
		} else if (!hw_read_otgsc(lb, OTGSC_BSV)) {
			dev_warn(lb->dev, "there is a vbus glitch\n");
			return 1;
		}
		msleep(20);
	}

	return 0;
}

void lb_handle_vbus_connected(struct lb_hdc *lb)
{
	/*
	 * TODO: if the platform does not supply 5v to udc, or use other way
	 * to supply 5v, it needs to use other conditions to call
	 * usb_gadget_vbus_connect.
	 */
	if (!lb->is_otg)
		return;

	if (hw_read_otgsc(lb, OTGSC_BSV) && !lb_is_vbus_glitch(lb))
		usb_gadget_vbus_connect(&lb->gadget);
}

void lb_handle_vbus_change(struct lb_hdc *lb)
{
	if (!lb->is_otg)
		return;

	if (hw_read_otgsc(lb, OTGSC_BSV))
		usb_gadget_vbus_connect(&lb->gadget);
	else
		usb_gadget_vbus_disconnect(&lb->gadget);
}

#define LB_VBUS_STABLE_TIMEOUT_MS 5000
void lb_handle_id_switch(struct lb_hdc *lb)
{
	enum lb_role role = lb_otg_role(lb);

	if (role != lb->role) {
		dev_dbg(lb->dev, "switching from %s to %s\n",
			lb_role(lb)->name, lb->roles[role]->name);

		while (lb_hdc_host_has_device(lb)) {
			enable_irq(lb->irq);
			usleep_range(10000, 15000);
			disable_irq_nosync(lb->irq);
		}

		lb_role_stop(lb);
		/* wait vbus lower than OTGSC_BSV */
		hw_wait_reg(lb, OP_OTGSC, OTGSC_BSV, 0,
				LB_VBUS_STABLE_TIMEOUT_MS);
		lb_role_start(lb, role);
	}
}

static void lb_handle_vbus_glitch(struct lb_hdc *lb)
{
	bool valid_vbus_change = false;

	if (hw_read_otgsc(lb, OTGSC_BSV)) {
		if (!lb_is_vbus_glitch(lb))
			valid_vbus_change = true;
	} else {
		if (lb->vbus_active && !lb_otg_is_fsm_mode(lb))
			valid_vbus_change = true;
	}

	if (valid_vbus_change) {
		lb->b_sess_valid_event = true;
		lb_otg_queue_work(lb);
	}
}

/* perform otg (vbus/id) event handle */
static void lb_otg_work(struct work_struct *work)
{
	struct lb_hdc *lb = container_of(work, struct lb_hdc, work);

	if (lb->vbus_glitch_check_event) {
		lb->vbus_glitch_check_event = false;
		pm_runtime_get_sync(lb->dev);
		lb_handle_vbus_glitch(lb);
		pm_runtime_put_sync(lb->dev);
		enable_irq(lb->irq);
		return;
	}

	pm_runtime_get_sync(lb->dev);
	if (lb->id_event) {
		lb->id_event = false;
		lb_handle_id_switch(lb);
	} else if (lb->b_sess_valid_event) {
		lb->b_sess_valid_event = false;
		lb_handle_vbus_change(lb);
	} else
		dev_err(lb->dev, "unexpected event occurs at %s\n", __func__);
	pm_runtime_put_sync(lb->dev);

	enable_irq(lb->irq);
}

/* initialize otg struct */
int lb_hdc_otg_init(struct lb_hdc *lb)
{
	INIT_WORK(&lb->work, lb_otg_work);
	lb->wq = create_singlethread_workqueue("lb_otg");
	if (!lb->wq) {
		dev_err(lb->dev, "can't create workqueue\n");
		return -ENODEV;
	}

	return 0;
}

/* destroy otg struct */
void lb_hdc_otg_destroy(struct lb_hdc *lb)
{
	/* Disable all OTG irq and clear status */
	hw_write_otgsc(lb, OTGSC_INT_EN_BITS | OTGSC_INT_STATUS_BITS,
						OTGSC_INT_STATUS_BITS);
	if (lb->wq) {
		flush_workqueue(lb->wq);
		destroy_workqueue(lb->wq);
		lb->wq = NULL;
	}
}

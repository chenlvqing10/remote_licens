/*
 * lombo usb udc driver interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dmapool.h>
#include <linux/err.h>
#include <linux/irqreturn.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

#include "lb.h"
#include "udc.h"
#include "bits.h"
#include "db.h"
#include "otg.h"
#include "lombo_nx.h"

/* control endpoint description */
static const struct usb_endpoint_descriptor ctrl_endpt_out_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_OUT,
	.bmAttributes		= USB_ENDPOINT_XFER_CONTROL,
	.wMaxPacketSize		= cpu_to_le16(CTRL_PAYLOAD_MAX),
};

static const struct usb_endpoint_descriptor ctrl_endpt_in_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_CONTROL,
	.wMaxPacketSize		= cpu_to_le16(CTRL_PAYLOAD_MAX),
};

/**
 * usb_ep_set_maxpacket_limit - set maximum packet size limit for endpoint
 * @ep:the endpoint being configured
 * @maxpacket_limit:value of maximum packet size limit
 *
 */
static inline void usb_ep_set_maxpacket_limit(struct usb_ep *ep,
					      unsigned maxpacket_limit)
{
	ep->maxpacket = maxpacket_limit;
}

/**
 * hw_ep_bit: calculates the bit number
 * @num: endpoint number
 * @dir: endpoint direction
 *
 * returns bit number
 */
static inline int hw_ep_bit(int num, int dir)
{
	return num + (dir ? 16 : 0);
}

static inline int ep_to_bit(struct lb_hdc *lb, int n)
{
	int fill = 16 - lb->hw_ep_max / 2;

	if (n >= lb->hw_ep_max / 2)
		n += fill;

	return n;
}

/**
 * hw_device_state: enables/disables interrupts (execute without interruption)
 * @addr: 0 => disable, !0 => enable and set addr
 *
 */
static int hw_device_state(struct lb_hdc *lb, u32 addr)
{
	if (addr) {
		hw_write(lb, OP_ENDPTLISTADDR, ~0, addr);
		/* interrupt, error, port change, reset, sleep/suspend */
		hw_write(lb, OP_USBINTR, ~0,
			     USBI_UI|USBI_UEI|USBI_PCI|USBI_URI|USBI_SLI);
		hw_write(lb, OP_USBCMD, USBCMD_RS, USBCMD_RS);
	} else {
		hw_write(lb, OP_USBINTR, ~0, 0);
		hw_write(lb, OP_USBCMD, USBCMD_RS, 0);
	}
	return 0;
}

/**
 * hw_ep_flush: flush endpoint fifo (execute without interruption)
 * @num: endpoint number
 * @dir: endpoint direction
 *
 */
static int hw_ep_flush(struct lb_hdc *lb, int num, int dir)
{
	int n = hw_ep_bit(num, dir);

	do {
		/* flush any pending transfer */
		hw_write(lb, OP_ENDPTFLUSH, ~0, BIT(n));
		while (hw_read(lb, OP_ENDPTFLUSH, BIT(n)))
			cpu_relax();
	} while (hw_read(lb, OP_ENDPTSTAT, BIT(n)));

	return 0;
}

/**
 * hw_ep_disable: disables endpoint (execute without interruption)
 * @num: endpoint number
 * @dir: endpoint direction
 *
 */
static int hw_ep_disable(struct lb_hdc *lb, int num, int dir)
{
	hw_ep_flush(lb, num, dir);
	hw_write(lb, OP_ENDPTCTRL + num,
		 dir ? ENDPTCTRL_TXE : ENDPTCTRL_RXE, 0);
	return 0;
}

/**
 * hw_ep_enable: enables endpoint (execute without interruption)
 * @num: endpoint number
 * @dir: endpoint direction
 * @type: endpoint type
 *
 */
static int hw_ep_enable(struct lb_hdc *lb, int num, int dir, int type)
{
	u32 mask, data;

	if (dir) {
		mask  = ENDPTCTRL_TXT;  /* type    */
		data  = type << __ffs(mask);

		mask |= ENDPTCTRL_TXS;  /* unstall */
		mask |= ENDPTCTRL_TXR;  /* reset data toggle */
		data |= ENDPTCTRL_TXR;
		mask |= ENDPTCTRL_TXE;  /* enable  */
		data |= ENDPTCTRL_TXE;
	} else {
		mask  = ENDPTCTRL_RXT;  /* type    */
		data  = type << __ffs(mask);

		mask |= ENDPTCTRL_RXS;  /* unstall */
		mask |= ENDPTCTRL_RXR;  /* reset data toggle */
		data |= ENDPTCTRL_RXR;
		mask |= ENDPTCTRL_RXE;  /* enable  */
		data |= ENDPTCTRL_RXE;
	}
	hw_write(lb, OP_ENDPTCTRL + num, mask, data);
	return 0;
}

/**
 * hw_ep_get_halt: return endpoint halt status
 * @num: endpoint number
 * @dir: endpoint direction
 *
 * returns 1 if endpoint halted
 */
static int hw_ep_get_halt(struct lb_hdc *lb, int num, int dir)
{
	u32 mask = dir ? ENDPTCTRL_TXS : ENDPTCTRL_RXS;

	return hw_read(lb, OP_ENDPTCTRL + num, mask) ? 1 : 0;
}

/**
 * hw_ep_prime: primes endpoint (execute without interruption)
 * @num: endpoint number
 * @dir: endpoint direction
 * @is_ctrl: true if control endpoint
 *
 */
static int hw_ep_prime(struct lb_hdc *lb, int num, int dir, int is_ctrl)
{
	int n = hw_ep_bit(num, dir);

	if (is_ctrl && dir == RX && hw_read(lb, OP_ENDPTSETUPSTAT, BIT(num)))
		return -EAGAIN;

	hw_write(lb, OP_ENDPTPRIME, ~0, BIT(n));

	while (hw_read(lb, OP_ENDPTPRIME, BIT(n)))
		cpu_relax();
	if (is_ctrl && dir == RX && hw_read(lb, OP_ENDPTSETUPSTAT, BIT(num)))
		return -EAGAIN;

	/* status shoult be tested according with manual but it doesn't work */
	return 0;
}

/**
 * hw_ep_set_halt: configures ep halt & resets data toggle after clear (execute
 * without interruption)
 * @num: endpoint number
 * @dir: endpoint direction
 * @value: true => stall, false => unstall
 *
 */
static int hw_ep_set_halt(struct lb_hdc *lb, int num, int dir, int value)
{
	if (value != 0 && value != 1)
		return -EINVAL;

	do {
		enum lb_hw_regs reg = OP_ENDPTCTRL + num;
		u32 mask_xs = dir ? ENDPTCTRL_TXS : ENDPTCTRL_RXS;
		u32 mask_xr = dir ? ENDPTCTRL_TXR : ENDPTCTRL_RXR;

		/* data toggle - reserved for EP0 but it's in ESS */
		hw_write(lb, reg, mask_xs | mask_xr, value ? mask_xs : mask_xr);
	} while (value != hw_ep_get_halt(lb, num, dir));

	return 0;
}

/**
 * hw_is_port_high_speed: test if port is high speed
 *
 * returns true if high speed port
 */
static int hw_port_is_high_speed(struct lb_hdc *lb)
{
	return hw_read(lb, OP_PORTSC, PORTSC_HSP);
}

/**
 * hw_test_and_clear_complete: test & clear complete status (execute without
 * interruption)
 * @n: endpoint number
 *
 * returns complete status
 */
static int hw_test_and_clear_complete(struct lb_hdc *lb, int n)
{
	n = ep_to_bit(lb, n);
	return hw_test_and_clear(lb, OP_ENDPTCOMPLETE, BIT(n));
}

/**
 * hw_test_and_clear_intr_active: test & clear active interrupts (execute
 * without interruption)
 *
 * returns active interrutps
 */
static u32 hw_test_and_clear_intr_active(struct lb_hdc *lb)
{
	u32 reg = hw_read_intr_status(lb) & hw_read_intr_enable(lb);

	hw_write(lb, OP_USBSTS, ~0, reg);
	return reg;
}

/**
 * hw_test_and_clear_setup_guard: test & clear setup guard (execute without
 * interruption)
 *
 * returns guard value
 */
static int hw_test_and_clear_setup_guard(struct lb_hdc *lb)
{
	return hw_test_and_write(lb, OP_USBCMD, USBCMD_SUTW, 0);
}

/**
 * hw_test_and_set_setup_guard: test & set setup guard (execute without
 * interruption)
 *
 * returns guard value
 */
static int hw_test_and_set_setup_guard(struct lb_hdc *lb)
{
	return hw_test_and_write(lb, OP_USBCMD, USBCMD_SUTW, USBCMD_SUTW);
}

/**
 * hw_usb_set_address: configures USB address (execute without interruption)
 * @value: new USB address
 *
 * This function explicitly sets the address, without the "USBADRA" (advance)
 * feature, which is not supported by older versions of the controller.
 */
static void hw_usb_set_address(struct lb_hdc *lb, u8 value)
{
	/* todo! note write usbadra bit first, then write addr value */
	/* 1. set address first */
	hw_write(lb, OP_DEVICEADDR, DEVICEADDR_USBADR,
		 value << __ffs(DEVICEADDR_USBADR));
	/* 2. set advance address */
	/*hw_write(lb, OP_DEVICEADDR, DEVICEADDR_USBADRA, DEVICEADDR_USBADRA);*/
}

/**
 * hw_usb_reset: restart device after a bus reset (execute without
 * interruption)
 *
 */
static int hw_usb_reset(struct lb_hdc *lb)
{
	hw_usb_set_address(lb, 0);

	/* ESS flushes only at end */
	hw_write(lb, OP_ENDPTFLUSH, ~0, ~0);

	/* clear setup token semaphores */
	hw_write(lb, OP_ENDPTSETUPSTAT, 0, 0);

	/* clear complete status */
	hw_write(lb, OP_ENDPTCOMPLETE, 0, 0);

	/* wait until all bits cleared */
	while (hw_read(lb, OP_ENDPTPRIME, ~0))
		udelay(10); /* not RTOS friendly */

	return 0;
}

static int add_td_to_list(struct lb_hw_ep *hwep, struct lb_hw_req *hwreq,
			  unsigned length)
{
	int i;
	u32 temp;
	struct td_node *lastnode, *node;

	node = kzalloc(sizeof(struct td_node), GFP_ATOMIC);
	if (node == NULL) {
		pr_err("[udc] %s alloc td node err\n", __func__);
		return -ENOMEM;
	}

	node->ptr = dma_pool_alloc(hwep->td_pool, GFP_ATOMIC, &node->dma);
	if (node->ptr == NULL) {
		pr_err("[udc] %s dma pool alloc err\n", __func__);
		kfree(node);
		return -ENOMEM;
	}

	memset(node->ptr, 0, sizeof(struct lb_hw_td));
	node->ptr->token = cpu_to_le32(length << __ffs(TD_TOTAL_BYTES));
	node->ptr->token &= cpu_to_le32(TD_TOTAL_BYTES);
	node->ptr->token |= cpu_to_le32(TD_STATUS_ACTIVE);
	if (hwep->type == USB_ENDPOINT_XFER_ISOC && hwep->dir == TX) {
		u32 mul = hwreq->req.length / hwep->ep.maxpacket;

		if (hwreq->req.length == 0 ||
		    hwreq->req.length % hwep->ep.maxpacket)
			mul++;
		node->ptr->token |= mul << __ffs(TD_MULTO);
	}

	temp = (u32)(hwreq->req.dma + hwreq->req.actual);
	if (length) {
		node->ptr->page[0] = cpu_to_le32(temp);
		for (i = 1; i < TD_PAGE_COUNT; i++) {
			u32 page = temp + i * LB_HDC_PAGE_SIZE;
			page &= ~TD_RESERVED_MASK;
			node->ptr->page[i] = cpu_to_le32(page);
		}
	}

	hwreq->req.actual += length;
	if (!list_empty(&hwreq->tds)) {
		/* get the last entry */
		lastnode = list_entry(hwreq->tds.prev, struct td_node, td);
		lastnode->ptr->next = cpu_to_le32(node->dma);
	}

	INIT_LIST_HEAD(&node->td);
	list_add_tail(&node->td, &hwreq->tds);

	return 0;
}

/**
 * _usb_addr: calculates endpoint address from direction & number
 * @ep:  endpoint
 */
static inline u8 _usb_addr(struct lb_hw_ep *ep)
{
	return ((ep->dir == TX) ? USB_ENDPOINT_DIR_MASK : 0) | ep->num;
}

/**
 * _hardware_queue: configures a request at hardware level
 * @gadget: gadget
 * @hwep:   endpoint
 *
 */
static int _hardware_enqueue(struct lb_hw_ep *hwep, struct lb_hw_req *hwreq)
{
	struct lb_hdc *lb = hwep->lb;
	int ret = 0;
	unsigned rest = hwreq->req.length;
	int pages = TD_PAGE_COUNT;
	struct td_node *firstnode, *lastnode;

	/* don't queue twice */
	if (hwreq->req.status == -EALREADY)
		return -EALREADY;

	hwreq->req.status = -EALREADY;

	ret = usb_gadget_map_request(&lb->gadget, &hwreq->req, hwep->dir);
	if (ret)
		return ret;

	/*
	 * The first buffer could be not page aligned.
	 * In that case we have to span into one extra td.
	 */
	if (hwreq->req.dma % PAGE_SIZE)
		pages--;

	if (rest == 0)
		add_td_to_list(hwep, hwreq, 0);

	while (rest > 0) {
		unsigned count = min(hwreq->req.length - hwreq->req.actual,
					(unsigned)(pages * LB_HDC_PAGE_SIZE));
		add_td_to_list(hwep, hwreq, count);
		rest -= count;
	}

	if (hwreq->req.zero && hwreq->req.length
	    && (hwreq->req.length % hwep->ep.maxpacket == 0))
		add_td_to_list(hwep, hwreq, 0);

	firstnode = list_first_entry(&hwreq->tds, struct td_node, td);
	lastnode = list_entry(hwreq->tds.prev, struct td_node, td);

	lastnode->ptr->next = cpu_to_le32(TD_TERMINATE);
	if (!hwreq->req.no_interrupt)
		lastnode->ptr->token |= cpu_to_le32(TD_IOC);
	wmb();

	hwreq->req.actual = 0;
	if (!list_empty(&hwep->qh.queue)) {
		struct lb_hw_req *hwreqprev;
		int n = hw_ep_bit(hwep->num, hwep->dir);
		int tmp_stat;
		struct td_node *prevlastnode;
		u32 next = firstnode->dma & TD_ADDR_MASK;

		hwreqprev = list_entry(hwep->qh.queue.prev,
				struct lb_hw_req, queue);
		prevlastnode = list_entry(hwreqprev->tds.prev,
				struct td_node, td);

		prevlastnode->ptr->next = cpu_to_le32(next);
		wmb();
		if (hw_read(lb, OP_ENDPTPRIME, BIT(n)))
			goto done;
		do {
			hw_write(lb, OP_USBCMD, USBCMD_ATDTW, USBCMD_ATDTW);
			tmp_stat = hw_read(lb, OP_ENDPTSTAT, BIT(n));
		} while (!hw_read(lb, OP_USBCMD, USBCMD_ATDTW));
		hw_write(lb, OP_USBCMD, USBCMD_ATDTW, 0);
		if (tmp_stat)
			goto done;
	}

	/*  QH configuration */
	hwep->qh.ptr->td.next = cpu_to_le32(firstnode->dma);
	hwep->qh.ptr->td.token &=
		cpu_to_le32(~(TD_STATUS_HALTED | TD_STATUS_ACTIVE));

	if (hwep->type == USB_ENDPOINT_XFER_ISOC && hwep->dir == RX) {
		u32 mul = hwreq->req.length / hwep->ep.maxpacket;

		if (hwreq->req.length == 0 ||
		    hwreq->req.length % hwep->ep.maxpacket)
			mul++;
		hwep->qh.ptr->cap |= mul << __ffs(QH_MULT);
	}

	wmb();   /* synchronize before ep prime */

	ret = hw_ep_prime(lb, hwep->num, hwep->dir,
			   hwep->type == USB_ENDPOINT_XFER_CONTROL);
done:
	return ret;
}

/*
 * free_pending_td: remove a pending request for the endpoint
 * @hwep: endpoint
 */
static void free_pending_td(struct lb_hw_ep *hwep)
{
	struct td_node *pending = hwep->pending_td;

	dma_pool_free(hwep->td_pool, pending->ptr, pending->dma);
	hwep->pending_td = NULL;
	kfree(pending);
}

static int reprime_dtd(struct lb_hdc *lb, struct lb_hw_ep *hwep,
					   struct td_node *node)
{
	hwep->qh.ptr->td.next = node->dma;
	hwep->qh.ptr->td.token &=
		cpu_to_le32(~(TD_STATUS_HALTED | TD_STATUS_ACTIVE));

	/* Synchronize before ep prime */
	wmb();

	return hw_ep_prime(lb, hwep->num, hwep->dir,
				hwep->type == USB_ENDPOINT_XFER_CONTROL);
}

/**
 * _hardware_dequeue: handles a request at hardware level
 * @gadget: gadget
 * @hwep: endpoint
 *
 */
static int _hardware_dequeue(struct lb_hw_ep *hwep, struct lb_hw_req *hwreq)
{
	u32 tmptoken;
	struct td_node *node, *tmpnode;
	unsigned remaining_length;
	unsigned actual = hwreq->req.length;
	struct lb_hdc *lb = hwep->lb;

	if (hwreq->req.status != -EALREADY)
		return -EINVAL;

	hwreq->req.status = 0;

	list_for_each_entry_safe(node, tmpnode, &hwreq->tds, td) {
		tmptoken = le32_to_cpu(node->ptr->token);
		if ((TD_STATUS_ACTIVE & tmptoken) != 0) {
			int n = hw_ep_bit(hwep->num, hwep->dir);

			if (lb->rev == LB_REVISION_24)
				if (!hw_read(lb, OP_ENDPTSTAT, BIT(n)))
					reprime_dtd(lb, hwep, node);
			hwreq->req.status = -EALREADY;
			return -EBUSY;
		}

		remaining_length = (tmptoken & TD_TOTAL_BYTES);
		remaining_length >>= __ffs(TD_TOTAL_BYTES);
		actual -= remaining_length;

		hwreq->req.status = tmptoken & TD_STATUS;
		if ((TD_STATUS_HALTED & hwreq->req.status)) {
			hwreq->req.status = -EPIPE;
			break;
		} else if ((TD_STATUS_DT_ERR & hwreq->req.status)) {
			hwreq->req.status = -EPROTO;
			break;
		} else if ((TD_STATUS_TR_ERR & hwreq->req.status)) {
			hwreq->req.status = -EILSEQ;
			break;
		}

		if (remaining_length) {
			if (hwep->dir) {
				hwreq->req.status = -EPROTO;
				break;
			}
		}
		/*
		 * As the hardware could still address the freed td
		 * which will run the udc unusable, the cleanup of the
		 * td has to be delayed by one.
		 */
		if (hwep->pending_td)
			free_pending_td(hwep);

		hwep->pending_td = node;
		list_del_init(&node->td);
	}

	usb_gadget_unmap_request(&hwep->lb->gadget, &hwreq->req, hwep->dir);

	hwreq->req.actual += actual;

	if (hwreq->req.status)
		return hwreq->req.status;

	return hwreq->req.actual;
}

/**
 * _ep_nuke: dequeues all endpoint requests
 * @hwep: endpoint
 *
 * This function returns an error code
 * Caller must hold lock
 */
static int _ep_nuke(struct lb_hw_ep *hwep)
__releases(hwep->lock)
__acquires(hwep->lock)
{
	struct td_node *node, *tmpnode;
	if (hwep == NULL)
		return -EINVAL;

	hw_ep_flush(hwep->lb, hwep->num, hwep->dir);

	while (!list_empty(&hwep->qh.queue)) {

		/* pop oldest request */
		struct lb_hw_req *hwreq = list_entry(hwep->qh.queue.next,
						     struct lb_hw_req, queue);

		list_for_each_entry_safe(node, tmpnode, &hwreq->tds, td) {
			dma_pool_free(hwep->td_pool, node->ptr, node->dma);
			list_del_init(&node->td);
			node->ptr = NULL;
			kfree(node);
		}

		list_del_init(&hwreq->queue);
		hwreq->req.status = -ESHUTDOWN;

		if (hwreq->req.complete != NULL) {
			spin_unlock(hwep->lock);
			hwreq->req.complete(&hwep->ep, &hwreq->req);
			spin_lock(hwep->lock);
		}
	}

	if (hwep->pending_td)
		free_pending_td(hwep);

	return 0;
}

/**
 * _gadget_stop_activity: stops all USB activity, flushes & disables all endpts
 * @gadget: gadget
 *
 */
static int _gadget_stop_activity(struct usb_gadget *gadget)
{
	struct usb_ep *ep;
	struct lb_hdc *lb = container_of(gadget, struct lb_hdc, gadget);
	unsigned long flags;

	/* flush all endpoints */
	gadget_for_each_ep(ep, gadget) {
		usb_ep_fifo_flush(ep);
	}
	usb_ep_fifo_flush(&lb->ep0out->ep);
	usb_ep_fifo_flush(&lb->ep0in->ep);

	/* make sure to disable all endpoints */
	gadget_for_each_ep(ep, gadget) {
		usb_ep_disable(ep);
	}

	if (lb->status != NULL) {
		usb_ep_free_request(&lb->ep0in->ep, lb->status);
		lb->status = NULL;
	}

	spin_lock_irqsave(&lb->lock, flags);
	lb->gadget.speed = USB_SPEED_UNKNOWN;
	lb->remote_wakeup = 0;
	lb->suspended = 0;
	spin_unlock_irqrestore(&lb->lock, flags);

	return 0;
}

/**
 * isr_reset_handler: USB reset interrupt handler
 * @lb: UDC device
 *
 * resets USB engine after a bus reset occurred
 */
static void isr_reset_handler(struct lb_hdc *lb)
__releases(lb->lock)
__acquires(lb->lock)
{
	int retval;

	spin_unlock(&lb->lock);
	if (lb->gadget.speed != USB_SPEED_UNKNOWN) {
		if (lb->driver)
			lb->driver->disconnect(&lb->gadget);
	}

	retval = _gadget_stop_activity(&lb->gadget);
	if (retval)
		goto done;

	retval = hw_usb_reset(lb);
	if (retval)
		goto done;

	lb->status = usb_ep_alloc_request(&lb->ep0in->ep, GFP_ATOMIC);
	if (lb->status == NULL)
		retval = -ENOMEM;

	lb->gadget_state = USB_STATE_DEFAULT;
	schedule_work(&lb->set_gadget_state);

done:
	spin_lock(&lb->lock);

	if (retval)
		dev_err(lb->dev, "error: %i\n", retval);
}

/**
 * isr_get_status_complete: get_status request complete function
 * @ep:  endpoint
 * @req: request handled
 *
 */
static void isr_get_status_complete(struct usb_ep *ep, struct usb_request *req)
{
	if (ep == NULL || req == NULL)
		return;

	kfree(req->buf);
	usb_ep_free_request(ep, req);
}

/**
 * _ep_queue: queues (submits) an I/O request to an endpoint
 *
 */
static int _ep_queue(struct usb_ep *ep, struct usb_request *req,
		    gfp_t __maybe_unused gfp_flags)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	struct lb_hw_req *hwreq = container_of(req, struct lb_hw_req, req);
	struct lb_hdc *lb = hwep->lb;
	int retval = 0;

	if (ep == NULL || req == NULL || hwep->ep.desc == NULL)
		return -EINVAL;

	if (hwep->type == USB_ENDPOINT_XFER_CONTROL) {
		if (req->length)
			hwep = (lb->ep0_dir == RX) ? lb->ep0out : lb->ep0in;
		if (!list_empty(&hwep->qh.queue)) {
			_ep_nuke(hwep);
			retval = -EOVERFLOW;
			dev_warn(hwep->lb->dev, "endpoint ctrl %X nuked\n",
				 _usb_addr(hwep));
		}
	}

	if (usb_endpoint_xfer_isoc(hwep->ep.desc) &&
	    hwreq->req.length > (1 + hwep->ep.mult) * hwep->ep.maxpacket) {
		dev_err(hwep->lb->dev, "request length too big for iso\n");
		return -EMSGSIZE;
	}

	/* first nuke then test link, e.g. previous status has not sent */
	if (!list_empty(&hwreq->queue)) {
		dev_err(hwep->lb->dev, "request already in queue\n");
		return -EBUSY;
	}

	/* push request */
	hwreq->req.status = -EINPROGRESS;
	hwreq->req.actual = 0;

	retval = _hardware_enqueue(hwep, hwreq);

	if (retval == -EALREADY)
		retval = 0;
	if (!retval)
		list_add_tail(&hwreq->queue, &hwep->qh.queue);

	return retval;
}

/**
 * isr_get_status_response: get_status request response
 * @ci: ci struct
 * @setup: setup request packet
 *
 */
static int isr_get_status_response(struct lb_hdc *lb,
				   struct usb_ctrlrequest *setup)
__releases(hwep->lock)
__acquires(hwep->lock)
{
	struct lb_hw_ep *hwep = lb->ep0in;
	struct usb_request *req = NULL;
	gfp_t gfp_flags = GFP_ATOMIC;
	int dir, num, retval;

	if (hwep == NULL || setup == NULL)
		return -EINVAL;

	spin_unlock(hwep->lock);
	req = usb_ep_alloc_request(&hwep->ep, gfp_flags);
	spin_lock(hwep->lock);
	if (req == NULL)
		return -ENOMEM;

	req->complete = isr_get_status_complete;
	req->length = 2;
	req->buf = kzalloc(req->length, gfp_flags);
	if (req->buf == NULL) {
		retval = -ENOMEM;
		goto err_free_req;
	}

	if ((setup->bRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE) {
		*(u16 *)req->buf = (lb->remote_wakeup << 1) |
				    lb->gadget.is_selfpowered;
	} else if ((setup->bRequestType & USB_RECIP_MASK) ==
		    USB_RECIP_ENDPOINT) {
		dir = (le16_to_cpu(setup->wIndex) & USB_ENDPOINT_DIR_MASK) ?
			TX : RX;
		num =  le16_to_cpu(setup->wIndex) & USB_ENDPOINT_NUMBER_MASK;
		*(u16 *)req->buf = hw_ep_get_halt(lb, num, dir);
	}
	/* else do nothing; reserved for future use */

	retval = _ep_queue(&hwep->ep, req, gfp_flags);
	if (retval)
		goto err_free_buf;

	return 0;

err_free_buf:
	kfree(req->buf);
err_free_req:
	spin_unlock(hwep->lock);
	usb_ep_free_request(&hwep->ep, req);
	spin_lock(hwep->lock);
	return retval;
}

/**
 * isr_setup_status_complete: setup_status request complete function
 * @ep:  endpoint
 * @req: request handled
 *
 * Caller must release lock. Put the port in test mode if test mode
 * feature is selected.
 */
static void isr_setup_status_complete(struct usb_ep *ep,
				      struct usb_request *req)
{
	struct lb_hdc *lb = req->context;
	unsigned long flags;

	if (lb->setaddr) {
		hw_usb_set_address(lb, lb->address);
		lb->setaddr = false;
		if (lb->address) {
			dev_info(lb->dev, "%s usb state: address\n", __func__);
			lb->gadget_state = USB_STATE_ADDRESS;
			schedule_work(&lb->set_gadget_state);
		}
	}

	spin_lock_irqsave(&lb->lock, flags);
	if (lb->test_mode)
		hw_port_test_set(lb, lb->test_mode);
	spin_unlock_irqrestore(&lb->lock, flags);
}

/**
 * isr_setup_status_phase: queues the status phase of a setup transation
 * @ci: ci struct
 *
 */
static int isr_setup_status_phase(struct lb_hdc *lb)
{
	int retval;
	struct lb_hw_ep *hwep;

	hwep = (lb->ep0_dir == TX) ? lb->ep0out : lb->ep0in;
	lb->status->context = lb;
	lb->status->complete = isr_setup_status_complete;

	retval = _ep_queue(&hwep->ep, lb->status, GFP_ATOMIC);

	return retval;
}

/**
 * isr_tr_complete_low: transaction complete low level handler
 * @hwep: endpoint
 *
 */
static int isr_tr_complete_low(struct lb_hw_ep *hwep)
__releases(hwep->lock)
__acquires(hwep->lock)
{
	struct lb_hw_req *hwreq, *hwreqtemp;
	struct lb_hw_ep *hweptemp = hwep;
	int retval = 0;

	list_for_each_entry_safe(hwreq, hwreqtemp, &hwep->qh.queue, queue) {
		retval = _hardware_dequeue(hwep, hwreq);
		if (retval < 0)
			break;
		list_del_init(&hwreq->queue);
		if (hwreq->req.complete != NULL) {
			spin_unlock(hwep->lock);
			if ((hwep->type == USB_ENDPOINT_XFER_CONTROL) &&
					hwreq->req.length)
				hweptemp = hwep->lb->ep0in;
			hwreq->req.complete(&hweptemp->ep, &hwreq->req);
			spin_lock(hwep->lock);
		}
	}

	if (retval == -EBUSY)
		retval = 0;

	return retval;
}

/**
 * isr_setup_packet_handler: setup packet handler
 * @lb: UDC descriptor
 *
 */
static void isr_setup_packet_handler(struct lb_hdc *lb)
__releases(lb->lock)
__acquires(lb->lock)
{
	struct lb_hw_ep *hwep = &lb->lb_hw_ep[0];
	struct usb_ctrlrequest req;
	int type, num, dir, err = -EINVAL;
	u8 tmode = 0;

	/* flush data and handshake transactions of previous setup packet */
	_ep_nuke(lb->ep0out);
	_ep_nuke(lb->ep0in);

	/* read_setup_packet */
	do {
		hw_test_and_set_setup_guard(lb);
		memcpy(&req, &hwep->qh.ptr->setup, sizeof(req));
	} while (!hw_test_and_clear_setup_guard(lb));

	type = req.bRequestType;
	lb->ep0_dir = (type & USB_DIR_IN) ? TX : RX;

	switch (req.bRequest) {
	case USB_REQ_CLEAR_FEATURE:
		if (type == (USB_DIR_OUT | USB_RECIP_ENDPOINT) &&
		    le16_to_cpu(req.wValue) == USB_ENDPOINT_HALT) {
			if (req.wLength != 0)
				break;
			num  = le16_to_cpu(req.wIndex);
			dir = num & USB_ENDPOINT_DIR_MASK;
			num &= USB_ENDPOINT_NUMBER_MASK;
			if (dir) /* TX */
				num += lb->hw_ep_max / 2;
			if (!lb->lb_hw_ep[num].wedge) {
				spin_unlock(&lb->lock);
				err = usb_ep_clear_halt(
					&lb->lb_hw_ep[num].ep);
				spin_lock(&lb->lock);
				if (err)
					break;
			}
			err = isr_setup_status_phase(lb);
		} else if (type == (USB_DIR_OUT | USB_RECIP_DEVICE) &&
			  le16_to_cpu(req.wValue) == USB_DEVICE_REMOTE_WAKEUP) {
			if (req.wLength != 0)
				break;
			lb->remote_wakeup = 0;
			err = isr_setup_status_phase(lb);
		} else {
			goto delegate;
		}
		break;
	case USB_REQ_GET_STATUS:
		if (type != (USB_DIR_IN | USB_RECIP_DEVICE)   &&
		    type != (USB_DIR_IN | USB_RECIP_ENDPOINT) &&
		    type != (USB_DIR_IN | USB_RECIP_INTERFACE))
			goto delegate;
		if ((le16_to_cpu(req.wLength) != 2 &&
			le16_to_cpu(req.wLength) != 1) ||
				le16_to_cpu(req.wValue) != 0)
			break;
		err = isr_get_status_response(lb, &req);
		break;
	case USB_REQ_SET_ADDRESS:
		if (type != (USB_DIR_OUT | USB_RECIP_DEVICE))
			goto delegate;
		if (le16_to_cpu(req.wLength) != 0 ||
		    le16_to_cpu(req.wIndex) != 0)
			break;
		lb->address = (u8)le16_to_cpu(req.wValue);
		lb->setaddr = true;
		err = isr_setup_status_phase(lb);
		break;
	case USB_REQ_SET_FEATURE:
		if (type == (USB_DIR_OUT | USB_RECIP_ENDPOINT) &&
		    le16_to_cpu(req.wValue) == USB_ENDPOINT_HALT) {
			if (req.wLength != 0)
				break;
			num = le16_to_cpu(req.wIndex);
			dir = num & USB_ENDPOINT_DIR_MASK;
			num &= USB_ENDPOINT_NUMBER_MASK;
			if (dir) /* TX */
				num += lb->hw_ep_max / 2;

			spin_unlock(&lb->lock);
			err = usb_ep_set_halt(&lb->lb_hw_ep[num].ep);
			spin_lock(&lb->lock);
			if (!err)
				isr_setup_status_phase(lb);
		} else if (type == (USB_DIR_OUT | USB_RECIP_DEVICE)) {
			if (req.wLength != 0)
				break;
			switch (le16_to_cpu(req.wValue)) {
			case USB_DEVICE_REMOTE_WAKEUP:
				lb->remote_wakeup = 1;
				err = isr_setup_status_phase(lb);
				break;
			case USB_DEVICE_TEST_MODE:
				tmode = le16_to_cpu(req.wIndex) >> 8;
				switch (tmode) {
				case TEST_J:
				case TEST_K:
				case TEST_SE0_NAK:
				case TEST_PACKET:
				case TEST_FORCE_EN:
					lb->test_mode = tmode;
					err = isr_setup_status_phase(lb);
					break;
				default:
					break;
				}
				break;
			default:
				goto delegate;
			}
		} else {
			goto delegate;
		}
		break;
	default:
delegate:
		if (req.wLength == 0)   /* no data phase */
			lb->ep0_dir = TX;

		spin_unlock(&lb->lock);
		err = lb->driver->setup(&lb->gadget, &req);
		spin_lock(&lb->lock);
		break;
	}

	if (err < 0) {
		spin_unlock(&lb->lock);
		if (usb_ep_set_halt(&hwep->ep))
			dev_err(lb->dev, "error: ep_set_halt\n");
		spin_lock(&lb->lock);
	}
}

/**
 * isr_tr_complete_handler: transaction complete interrupt handler
 * @lb: UDC descriptor
 *
 */
static void isr_tr_complete_handler(struct lb_hdc *lb)
__releases(lb->lock)
__acquires(lb->lock)
{
	unsigned i;
	int err;

	for (i = 0; i < lb->hw_ep_max; i++) {
		struct lb_hw_ep *hwep  = &lb->lb_hw_ep[i];

		if (hwep->ep.desc == NULL)
			continue;   /* not configured */

		if (hw_test_and_clear_complete(lb, i)) {
			err = isr_tr_complete_low(hwep);
			if (hwep->type == USB_ENDPOINT_XFER_CONTROL) {
				if (err > 0)   /* needs status phase */
					err = isr_setup_status_phase(lb);
				if (err < 0) {
					spin_unlock(&lb->lock);
					if (usb_ep_set_halt(&hwep->ep))
						dev_err(lb->dev,
							"error: ep_set_halt\n");
					spin_lock(&lb->lock);
				}
			}
		}

		/* Only handle setup packet below */
		if (i == 0 && hw_test_and_clear(lb, OP_ENDPTSETUPSTAT, BIT(0)))
			isr_setup_packet_handler(lb);
	}
}

/**
 * ep_enable: configure endpoint, making it usable
 *
 * Check usb_ep_enable() at "usb_gadget.h" for details
 */
static int ep_enable(struct usb_ep *ep,
		     const struct usb_endpoint_descriptor *desc)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	int retval = 0;
	unsigned long flags;
	u32 cap = 0;

	if (ep == NULL || desc == NULL)
		return -EINVAL;

	spin_lock_irqsave(hwep->lock, flags);

	/* only internal SW should enable ctrl endpts */
	if (!list_empty(&hwep->qh.queue)) {
		dev_warn(hwep->lb->dev, "enabling a non-empty endpoint!\n");
		spin_unlock_irqrestore(hwep->lock, flags);
		return -EBUSY;
	}

	hwep->ep.desc = desc;

	hwep->dir  = usb_endpoint_dir_in(desc) ? TX : RX;
	hwep->num  = usb_endpoint_num(desc);
	hwep->type = usb_endpoint_type(desc);

	hwep->ep.maxpacket = usb_endpoint_maxp(desc) & 0x07ff;
	hwep->ep.mult = QH_ISO_MULT(usb_endpoint_maxp(desc));

	if (hwep->type == USB_ENDPOINT_XFER_CONTROL)
		cap |= QH_IOS;

	cap |= QH_ZLT;
	cap |= (hwep->ep.maxpacket << __ffs(QH_MAX_PKT)) & QH_MAX_PKT;
	/*
	 * For ISO-TX, we set mult at QH as the largest value, and use
	 * MultO at TD as real mult value.
	 */
	if (hwep->type == USB_ENDPOINT_XFER_ISOC && hwep->dir == TX)
		cap |= 3 << __ffs(QH_MULT);

	hwep->qh.ptr->cap = cpu_to_le32(cap);
	hwep->qh.ptr->td.next |= cpu_to_le32(TD_TERMINATE);   /* needed? */
	if (hwep->num != 0 && hwep->type == USB_ENDPOINT_XFER_CONTROL) {
		dev_err(hwep->lb->dev, "Set control xfer at non-ep0\n");
		retval = -EINVAL;
	}

	/*
	 * Enable endpoints in the HW other than ep0 as ep0
	 * is always enabled
	 */
	if (hwep->num)
		retval |= hw_ep_enable(hwep->lb, hwep->num, hwep->dir,
					hwep->type);

	spin_unlock_irqrestore(hwep->lock, flags);
	return retval;
}

/**
 * ep_disable: endpoint is no longer usable
 *
 * Check usb_ep_disable() at "usb_gadget.h" for details
 */
static int ep_disable(struct usb_ep *ep)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	int direction, retval = 0;
	unsigned long flags;

	if (ep == NULL)
		return -EINVAL;
	else if (hwep->ep.desc == NULL)
		return -EBUSY;

	spin_lock_irqsave(hwep->lock, flags);
	if (hwep->lb->gadget.speed == USB_SPEED_UNKNOWN) {
		spin_unlock_irqrestore(hwep->lock, flags);
		return 0;
	}

	/* only internal SW should disable ctrl endpts */
	direction = hwep->dir;
	do {
		retval |= _ep_nuke(hwep);
		retval |= hw_ep_disable(hwep->lb, hwep->num, hwep->dir);

		if (hwep->type == USB_ENDPOINT_XFER_CONTROL)
			hwep->dir = (hwep->dir == TX) ? RX : TX;

	} while (hwep->dir != direction);

	hwep->ep.desc = NULL;

	spin_unlock_irqrestore(hwep->lock, flags);
	return retval;
}

/**
 * ep_alloc_request: allocate a request object to use with this endpoint
 *
 * Check usb_ep_alloc_request() at "usb_gadget.h" for details
 */
static struct usb_request *ep_alloc_request(struct usb_ep *ep, gfp_t gfp_flags)
{
	struct lb_hw_req *hwreq = NULL;

	if (ep == NULL)
		return NULL;

	hwreq = kzalloc(sizeof(struct lb_hw_req), gfp_flags);
	if (hwreq != NULL) {
		INIT_LIST_HEAD(&hwreq->queue);
		INIT_LIST_HEAD(&hwreq->tds);
	}

	return (hwreq == NULL) ? NULL : &hwreq->req;
}

/**
 * ep_free_request: frees a request object
 *
 * Check usb_ep_free_request() at "usb_gadget.h" for details
 */
static void ep_free_request(struct usb_ep *ep, struct usb_request *req)
{
	struct lb_hw_ep *hwep = container_of(ep,  struct lb_hw_ep, ep);
	struct lb_hw_req *hwreq = container_of(req, struct lb_hw_req, req);
	struct td_node *node, *tmpnode;
	unsigned long flags;

	if (ep == NULL || req == NULL) {
		return;
	} else if (!list_empty(&hwreq->queue)) {
		dev_err(hwep->lb->dev, "freeing queued request\n");
		return;
	}

	spin_lock_irqsave(hwep->lock, flags);

	list_for_each_entry_safe(node, tmpnode, &hwreq->tds, td) {
		dma_pool_free(hwep->td_pool, node->ptr, node->dma);
		list_del_init(&node->td);
		node->ptr = NULL;
		kfree(node);
	}

	kfree(hwreq);
	spin_unlock_irqrestore(hwep->lock, flags);
}

/**
 * ep_queue: queues (submits) an I/O request to an endpoint
 *
 * Check usb_ep_queue()* at usb_gadget.h" for details
 */
static int ep_queue(struct usb_ep *ep, struct usb_request *req,
		    gfp_t __maybe_unused gfp_flags)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	int retval = 0;
	unsigned long flags;

	if (ep == NULL || req == NULL || hwep->ep.desc == NULL)
		return -EINVAL;

	spin_lock_irqsave(hwep->lock, flags);
	if (hwep->lb->gadget.speed == USB_SPEED_UNKNOWN) {
		spin_unlock_irqrestore(hwep->lock, flags);
		return 0;
	}
	retval = _ep_queue(ep, req, gfp_flags);
	spin_unlock_irqrestore(hwep->lock, flags);
	return retval;
}

/**
 * ep_dequeue: dequeues (cancels, unlinks) an I/O request from an endpoint
 *
 * Check usb_ep_dequeue() at "usb_gadget.h" for details
 */
static int ep_dequeue(struct usb_ep *ep, struct usb_request *req)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	struct lb_hw_req *hwreq = container_of(req, struct lb_hw_req, req);
	unsigned long flags;
	struct td_node *node, *tmpnode;

	if (ep == NULL || req == NULL || hwreq->req.status != -EALREADY ||
		hwep->ep.desc == NULL || list_empty(&hwreq->queue) ||
		list_empty(&hwep->qh.queue))
		return -EINVAL;

	spin_lock_irqsave(hwep->lock, flags);
	if (hwep->lb->gadget.speed != USB_SPEED_UNKNOWN)
		hw_ep_flush(hwep->lb, hwep->num, hwep->dir);

	list_for_each_entry_safe(node, tmpnode, &hwreq->tds, td) {
		dma_pool_free(hwep->td_pool, node->ptr, node->dma);
		list_del(&node->td);
		kfree(node);
	}

	/* pop request */
	list_del_init(&hwreq->queue);
	usb_gadget_unmap_request(&hwep->lb->gadget, req, hwep->dir);
	req->status = -ECONNRESET;

	if (hwreq->req.complete != NULL) {
		spin_unlock(hwep->lock);
		hwreq->req.complete(&hwep->ep, &hwreq->req);
		spin_lock(hwep->lock);
	}

	spin_unlock_irqrestore(hwep->lock, flags);
	return 0;
}

/**
 * ep_set_halt: sets the endpoint halt feature
 *
 * Check usb_ep_set_halt() at "usb_gadget.h" for details
 */
static int ep_set_halt(struct usb_ep *ep, int value)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	int direction, retval = 0;
	unsigned long flags;

	if (ep == NULL || hwep->ep.desc == NULL)
		return -EINVAL;

	if (usb_endpoint_xfer_isoc(hwep->ep.desc))
		return -EOPNOTSUPP;

	spin_lock_irqsave(hwep->lock, flags);

	if (hwep->lb->gadget.speed == USB_SPEED_UNKNOWN) {
		spin_unlock_irqrestore(hwep->lock, flags);
		return 0;
	}
#ifndef STALL_IN
	/* g_file_storage MS compliant but g_zero fails chapter 9 compliance */
	if (value && hwep->type == USB_ENDPOINT_XFER_BULK && hwep->dir == TX &&
	    !list_empty(&hwep->qh.queue)) {
		spin_unlock_irqrestore(hwep->lock, flags);
		return -EAGAIN;
	}
#endif

	direction = hwep->dir;
	do {
		retval |= hw_ep_set_halt(hwep->lb, hwep->num, hwep->dir, value);

		if (!value)
			hwep->wedge = 0;

		if (hwep->type == USB_ENDPOINT_XFER_CONTROL)
			hwep->dir = (hwep->dir == TX) ? RX : TX;

	} while (hwep->dir != direction);

	spin_unlock_irqrestore(hwep->lock, flags);
	return retval;
}

/**
 * ep_set_wedge: sets the halt feature and ignores clear requests
 *
 * Check usb_ep_set_wedge() at "usb_gadget.h" for details
 */
static int ep_set_wedge(struct usb_ep *ep)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	unsigned long flags;

	if (ep == NULL || hwep->ep.desc == NULL)
		return -EINVAL;

	spin_lock_irqsave(hwep->lock, flags);
	hwep->wedge = 1;
	spin_unlock_irqrestore(hwep->lock, flags);

	return usb_ep_set_halt(ep);
}

/**
 * ep_fifo_flush: flushes contents of a fifo
 *
 * Check usb_ep_fifo_flush() at "usb_gadget.h" for details
 */
static void ep_fifo_flush(struct usb_ep *ep)
{
	struct lb_hw_ep *hwep = container_of(ep, struct lb_hw_ep, ep);
	unsigned long flags;

	if (ep == NULL) {
		dev_err(hwep->lb->dev, "%02X: -EINVAL\n", _usb_addr(hwep));
		return;
	}

	spin_lock_irqsave(hwep->lock, flags);
	if (hwep->lb->gadget.speed == USB_SPEED_UNKNOWN) {
		spin_unlock_irqrestore(hwep->lock, flags);
		return;
	}

	hw_ep_flush(hwep->lb, hwep->num, hwep->dir);
	spin_unlock_irqrestore(hwep->lock, flags);
}

/**
 * Endpoint-specific part of the API to the USB controller hardware
 * Check "usb_gadget.h" for details
 */
static const struct usb_ep_ops usb_ep_ops = {
	.enable		= ep_enable,
	.disable	= ep_disable,
	.alloc_request	= ep_alloc_request,
	.free_request	= ep_free_request,
	.queue		= ep_queue,
	.dequeue	= ep_dequeue,
	.set_halt	= ep_set_halt,
	.set_wedge	= ep_set_wedge,
	.fifo_flush	= ep_fifo_flush,
};

static int lb_udc_vbus_session(struct usb_gadget *_gadget, int is_active)
{
	struct lb_hdc *lb = container_of(_gadget, struct lb_hdc, gadget);
	unsigned long flags;
	int gadget_ready = 0;

	spin_lock_irqsave(&lb->lock, flags);
	lb->vbus_active = is_active;
	if (lb->driver)
		gadget_ready = 1;
	spin_unlock_irqrestore(&lb->lock, flags);

	/* Charger Detection */
	lb_usb_charger_connect(lb, is_active);

	if (gadget_ready)
		lb_hdc_gadget_connect(_gadget, is_active);

	return 0;
}

static int lb_udc_wakeup(struct usb_gadget *_gadget)
{
	struct lb_hdc *lb = container_of(_gadget, struct lb_hdc, gadget);
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&lb->lock, flags);
	if (lb->gadget.speed == USB_SPEED_UNKNOWN) {
		spin_unlock_irqrestore(&lb->lock, flags);
		return 0;
	}
	if (!lb->remote_wakeup) {
		ret = -EOPNOTSUPP;
		goto out;
	}
	if (!hw_read(lb, OP_PORTSC, PORTSC_SUSP)) {
		ret = -EINVAL;
		goto out;
	}
	hw_write(lb, OP_PORTSC, PORTSC_FPR, PORTSC_FPR);
out:
	spin_unlock_irqrestore(&lb->lock, flags);
	return ret;
}

static int lb_udc_vbus_draw(struct usb_gadget *_gadget, unsigned ma)
{
	struct lb_hdc *lb = container_of(_gadget, struct lb_hdc, gadget);

	if (lb->usb_phy)
		return usb_phy_set_power(lb->usb_phy, ma);
	return -ENOTSUPP;
}

static int lb_udc_selfpowered(struct usb_gadget *_gadget, int is_on)
{
	struct lb_hdc *lb = container_of(_gadget, struct lb_hdc, gadget);
	struct lb_hw_ep *hwep = lb->ep0in;
	unsigned long flags;

	spin_lock_irqsave(hwep->lock, flags);
	_gadget->is_selfpowered = (is_on != 0);
	spin_unlock_irqrestore(hwep->lock, flags);

	return 0;
}

/* Change Data+ pullup status
 * this func is used by usb_gadget_connect/disconnet
 */
static int lb_udc_pullup(struct usb_gadget *_gadget, int is_on)
{
	struct lb_hdc *lb = container_of(_gadget, struct lb_hdc, gadget);
	if (!lb->vbus_active)
		return -EOPNOTSUPP;

	if (is_on)
		hw_write(lb, OP_USBCMD, USBCMD_RS, USBCMD_RS);
	else
		hw_write(lb, OP_USBCMD, USBCMD_RS, 0);

	return 0;
}

static int lb_udc_start(struct usb_gadget *gadget,
			 struct usb_gadget_driver *driver);
static int lb_udc_stop(struct usb_gadget *gadget,
			struct usb_gadget_driver *driver);

/**
 * Device operations part of the API to the USB controller hardware,
 * which don't involve endpoints (or i/o)
 * Check  "usb_gadget.h" for details
 */
static const struct usb_gadget_ops usb_gadget_ops = {
	.wakeup			= lb_udc_wakeup,
	.set_selfpowered	= lb_udc_selfpowered,
	.pullup			= lb_udc_pullup,
	.vbus_draw		= lb_udc_vbus_draw,
	.udc_start		= lb_udc_start,
	.udc_stop		= lb_udc_stop,
};

static int init_eps(struct lb_hdc *lb)
{
	int retval = 0, i, j;

	for (i = 0; i < lb->hw_ep_max/2; i++)
		for (j = RX; j <= TX; j++) {
			int k = i + j * lb->hw_ep_max/2;
			struct lb_hw_ep *hwep = &lb->lb_hw_ep[k];

			scnprintf(hwep->name, sizeof(hwep->name), "ep%i%s", i,
					(j == TX) ? "in" : "out");

			hwep->lb	= lb;
			hwep->lock	= &lb->lock;
			hwep->td_pool	= lb->td_pool;
			hwep->ep.name	= hwep->name;
			hwep->ep.ops	= &usb_ep_ops;
			/*
			 * for ep0: maxP defined in desc, for other
			 * eps, maxP is set by epautoconfig() called
			 * by gadget layer
			 */
			usb_ep_set_maxpacket_limit(&hwep->ep,
						   (unsigned short)~0);
			INIT_LIST_HEAD(&hwep->qh.queue);
			hwep->qh.ptr = dma_pool_alloc(lb->qh_pool, GFP_KERNEL,
						     &hwep->qh.dma);
			if (hwep->qh.ptr == NULL)
				retval = -ENOMEM;
			else
				memset(hwep->qh.ptr, 0, sizeof(*hwep->qh.ptr));

			/*
			 * set up shorthands for ep0 out and in endpoints,
			 * don't add to gadget's ep_list
			 */
			if (i == 0) {
				if (j == RX)
					lb->ep0out = hwep;
				else
					lb->ep0in = hwep;

				usb_ep_set_maxpacket_limit(&hwep->ep,
							   CTRL_PAYLOAD_MAX);
				continue;
			}

			list_add_tail(&hwep->ep.ep_list, &lb->gadget.ep_list);
		}

	return retval;
}

static void destroy_eps(struct lb_hdc *lb)
{
	int i;

	for (i = 0; i < lb->hw_ep_max; i++) {
		struct lb_hw_ep *hwep = &lb->lb_hw_ep[i];

		if (hwep->pending_td)
			free_pending_td(hwep);
		dma_pool_free(lb->qh_pool, hwep->qh.ptr, hwep->qh.dma);
	}
}

/**
 * lb_udc_start: register a gadget driver
 * @gadget: our gadget
 * @driver: the driver being registered
 *
 * Interrupts are enabled here.
 */
static int lb_udc_start(struct usb_gadget *gadget,
			 struct usb_gadget_driver *driver)
{
	struct lb_hdc *lb = container_of(gadget, struct lb_hdc, gadget);
	int retval = -ENOMEM;

	if (driver->disconnect == NULL)
		return -EINVAL;

	/* todo! for test */
	lb_udc_vbus_session(gadget, 1);

	lb->ep0out->ep.desc = &ctrl_endpt_out_desc;
	retval = usb_ep_enable(&lb->ep0out->ep);
	if (retval)
		return retval;

	lb->ep0in->ep.desc = &ctrl_endpt_in_desc;
	retval = usb_ep_enable(&lb->ep0in->ep);
	if (retval)
		return retval;

	lb->driver = driver;

	if (lb->vbus_active)
		lb_hdc_gadget_connect(&lb->gadget, 1);

	return retval;
}

/**
 * lb_udc_stop: unregister a gadget driver
 */
static int lb_udc_stop(struct usb_gadget *gadget,
			struct usb_gadget_driver *driver)
{
	struct lb_hdc *lb = container_of(gadget, struct lb_hdc, gadget);
	unsigned long flags;

	spin_lock_irqsave(&lb->lock, flags);

	if (lb->vbus_active) {
		hw_device_state(lb, 0);
		if (lb->platdata->notify_event)
			lb->platdata->notify_event(lb,
					LB_HDC_CONTROLLER_STOPPED_EVENT);
		spin_unlock_irqrestore(&lb->lock, flags);
		_gadget_stop_activity(&lb->gadget);
		spin_lock_irqsave(&lb->lock, flags);
		pm_runtime_put(&lb->gadget.dev);
	}

	lb->driver = NULL;
	spin_unlock_irqrestore(&lb->lock, flags);

	return 0;
}

/**
 * udc_irq: lb interrupt handler
 *
 * This function returns IRQ_HANDLED if the IRQ has been handled
 * It locks access to registers
 */
static irqreturn_t udc_irq(struct lb_hdc *lb)
{
	irqreturn_t retval;
	u32 intr;

	if (lb == NULL)
		return IRQ_HANDLED;

	spin_lock(&lb->lock);

	if (lb->platdata->flags & LB_HDC_REGS_SHARED) {
		if (hw_read(lb, OP_USBMODE, USBMODE_CM) != USBMODE_CM_DC) {
			spin_unlock(&lb->lock);
			return IRQ_NONE;
		}
	}
	intr = hw_test_and_clear_intr_active(lb);

	if (intr) {
		/* order defines priority - do NOT change it */
		if (USBI_URI & intr)
			isr_reset_handler(lb);

		if (USBI_PCI & intr) {
			lb->gadget.speed = hw_port_is_high_speed(lb) ?
				USB_SPEED_HIGH : USB_SPEED_FULL;
			if (lb->suspended && lb->driver->resume) {
				spin_unlock(&lb->lock);
				lb->driver->resume(&lb->gadget);
				spin_lock(&lb->lock);
				lb->suspended = 0;
			}
		}

		if (USBI_UI & intr)
			isr_tr_complete_handler(lb);

		if (USBI_SLI & intr) {
			if (lb->gadget.speed != USB_SPEED_UNKNOWN &&
			    lb->driver->suspend) {
				lb->suspended = 1;
				spin_unlock(&lb->lock);
				lb->driver->suspend(&lb->gadget);
				lb->gadget_state = USB_STATE_SUSPENDED;
				schedule_work(&lb->set_gadget_state);
				spin_lock(&lb->lock);
			}
		}
		retval = IRQ_HANDLED;
	} else {
		retval = IRQ_NONE;
	}
	spin_unlock(&lb->lock);
	return retval;
}

/**
 * udc_start: initialize gadget role
 * @lb: chipidea controller
 */
static int udc_start(struct lb_hdc *lb)
{
	struct device *dev = lb->dev;
	int retval = 0;

	spin_lock_init(&lb->lock);
	lb->gadget.ops		= &usb_gadget_ops;
	lb->gadget.speed	= USB_SPEED_UNKNOWN;
	lb->gadget.max_speed	= USB_SPEED_HIGH;
	lb->gadget.name		= lb->platdata->name;

	INIT_LIST_HEAD(&lb->gadget.ep_list);

	/* alloc resources */
	lb->qh_pool = dma_pool_create("lb_hw_qh", dev,
					sizeof(struct lb_hw_qh),
					64, LB_HDC_PAGE_SIZE);
	if (lb->qh_pool == NULL)
		return -ENOMEM;

	lb->td_pool = dma_pool_create("lb_hw_td", dev,
					sizeof(struct lb_hw_td),
					64, LB_HDC_PAGE_SIZE);
	if (lb->td_pool == NULL) {
		retval = -ENOMEM;
		goto free_qh_pool;
	}

	retval = init_eps(lb);
	if (retval) {
		dev_err(lb->dev, "udc_start init eps err %d\n", retval);
		goto free_pools;
	}

	lb->gadget.ep0 = &lb->ep0in->ep;
	retval = usb_add_gadget_udc(dev, &lb->gadget);
	if (retval) {
		dev_err(lb->dev, "udc_start add gadget udc err %d\n", retval);
		goto destroy_eps;
	}

	pm_runtime_no_callbacks(&lb->gadget.dev);
	pm_runtime_enable(&lb->gadget.dev);

	dev_info(lb->dev, "udc_start ok\n");
	return retval;

destroy_eps:
	destroy_eps(lb);
free_pools:
	dma_pool_destroy(lb->td_pool);
free_qh_pool:
	dma_pool_destroy(lb->qh_pool);
	return retval;
}

/**
 * lb_hdc_gadget_destroy: parent remove must call this to remove UDC
 *
 * No interrupts active, the IRQ has been released
 */
void lb_hdc_gadget_destroy(struct lb_hdc *lb)
{
	if (!lb->roles[LB_ROLE_GADGET])
		return;

	usb_del_gadget_udc(&lb->gadget);
	destroy_eps(lb);

	dma_pool_destroy(lb->td_pool);
	dma_pool_destroy(lb->qh_pool);
}

int lb_usb_charger_connect(struct lb_hdc *lb, int is_active)
{
	int ret = 0;

	if (is_active)
		pm_runtime_get_sync(lb->dev);

	if (lb->platdata->notify_event) {
		if (is_active)
			hw_write(lb, OP_USBCMD, USBCMD_RS, 0);

		ret = lb->platdata->notify_event(lb,
				LB_HDC_CONTROLLER_VBUS_EVENT);
		if (ret == LB_HDC_NOTIFY_RET_DEFER_EVENT) {
			hw_device_reset(lb);
			/* Pull up dp */
			hw_write(lb, OP_USBCMD, USBCMD_RS, USBCMD_RS);
			lb->platdata->notify_event(lb,
				LB_HDC_CONTROLLER_CHARGER_POST_EVENT);
			/* Pull down dp */
			hw_write(lb, OP_USBCMD, USBCMD_RS, 0);
		}
	}

	if (!is_active)
		pm_runtime_put_sync(lb->dev);

	return ret;
}

/**
 * lb_hdc_gadget_connect: caller make sure gadget driver is binded
 */
void lb_hdc_gadget_connect(struct usb_gadget *gadget, int is_active)
{
	struct lb_hdc *lb = container_of(gadget, struct lb_hdc, gadget);

	if (is_active) {
		pm_runtime_get_sync(&gadget->dev);
		hw_device_reset(lb);
		hw_device_state(lb, lb->ep0out->qh.dma);
		usb_gadget_set_state(gadget, USB_STATE_POWERED);
	} else {
		if (lb->driver)
			lb->driver->disconnect(gadget);
		hw_device_state(lb, 0);
		if (lb->platdata->notify_event)
			lb->platdata->notify_event(lb,
			LB_HDC_CONTROLLER_STOPPED_EVENT);
		_gadget_stop_activity(gadget);
		pm_runtime_put_sync(&gadget->dev);
		usb_gadget_set_state(gadget, USB_STATE_NOTATTACHED);
	}
}

/* todo! */
static int udc_id_switch_for_device(struct lb_hdc *lb)
{
	return 0;
}

/* todo! */
static void udc_id_switch_for_host(struct lb_hdc *lb)
{

}

static void udc_suspend_for_power_lost(struct lb_hdc *lb)
{
	/*
	 * Set OP_ENDPTLISTADDR to be non-zero for
	 * checking if controller resume from power lost
	 * in non-host mode.
	 */
	if (hw_read(lb, OP_ENDPTLISTADDR, ~0) == 0)
		hw_write(lb, OP_ENDPTLISTADDR, ~0, ~0);
}

/* Power lost with device mode */
static void udc_resume_from_power_lost(struct lb_hdc *lb)
{
	int ret;

	/* While power lost,need reset device's reg. */
	ret = hw_device_reset(lb);
	if (ret)
		dev_err(lb->dev, "reset device's reg failed\n");

	/* While power lost,need enable interrupt. */
	ret = hw_device_state(lb, lb->ep0out->qh.dma);
	if (ret)
		dev_err(lb->dev, "enable interrupt failed\n");

	/* Force disconnect if power lost with vbus on */
	if (lb->vbus_active)
		usb_gadget_vbus_disconnect(&lb->gadget);
}

static void udc_set_gadget_state(struct work_struct *work)
{
	struct lb_hdc *lb = container_of(work, struct lb_hdc, set_gadget_state);
	usb_gadget_set_state(&lb->gadget, lb->gadget_state);
}

static void udc_suspend(struct lb_hdc *lb)
{
	udc_suspend_for_power_lost(lb);

	if (lb->driver && lb->vbus_active &&
		(lb->gadget.state != USB_STATE_SUSPENDED))
		usb_gadget_disconnect(&lb->gadget);
}

static void udc_resume(struct lb_hdc *lb, bool power_lost)
{
	if (power_lost)
		udc_resume_from_power_lost(lb);
	else {
		if (lb->driver && lb->vbus_active)
			usb_gadget_connect(&lb->gadget);
	}
}

/* initialize device gadget */
int lb_hdc_gadget_init(struct lb_hdc *lb)
{
	struct lb_role_driver *rdrv;

	if (!hw_read(lb, CAP_DCCPARAMS, DCCPARAMS_DC)) {
		dev_err(lb->dev, "%s no support device\n", __func__);
		return -ENXIO;
	}

	rdrv = devm_kzalloc(lb->dev, sizeof(struct lb_role_driver), GFP_KERNEL);
	if (!rdrv) {
		dev_err(lb->dev, "%s alloc role driver failed\n", __func__);
		return -ENOMEM;
	}

	rdrv->start	= udc_id_switch_for_device;
	rdrv->stop	= udc_id_switch_for_host;
	rdrv->irq	= udc_irq;
	rdrv->suspend	= udc_suspend;
	rdrv->resume	= udc_resume;
	rdrv->name	= "gadget";
	lb->roles[LB_ROLE_GADGET] = rdrv;

	/* Init workqueue for gadget state handling */
	INIT_WORK(&lb->set_gadget_state, udc_set_gadget_state);

	return udc_start(lb);
}

/*
 * lombo usb udc structures
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef LOMBO_USB_UDC_DRIVER__H
#define LOMBO_USB_UDC_DRIVER__H

#include <linux/list.h>

#define CTRL_PAYLOAD_MAX	64
#define RX			0
#define TX			1

/* transfer desctriptors */
struct lb_hw_td {
	/* 0 */
	u32 next;
#define TD_TERMINATE		BIT(0)
#define TD_ADDR_MASK		(0xFFFFFFEUL << 5)
	/* 1 */
	u32 token;
#define TD_STATUS		(0x00FFUL << 0)
#define TD_STATUS_TR_ERR	BIT(3)
#define TD_STATUS_DT_ERR	BIT(5)
#define TD_STATUS_HALTED	BIT(6)
#define TD_STATUS_ACTIVE	BIT(7)
#define TD_MULTO		(0x0003UL << 10)
#define TD_IOC			BIT(15)
#define TD_TOTAL_BYTES		(0x7FFFUL << 16)
	/* 2 */
	u32 page[5];
#define TD_CURR_OFFSET		(0x0FFFUL << 0)
#define TD_FRAME_NUM		(0x07FFUL << 0)
#define TD_RESERVED_MASK	(0x0FFFUL << 0)
} __packed __aligned(4);

/* queue heads */
struct lb_hw_qh {
	/* 0 */
	u32 cap;
#define QH_IOS			BIT(15)
#define QH_MAX_PKT		(0x07FFUL << 16)
#define QH_ZLT			BIT(29)
#define QH_MULT			(0x0003UL << 30)
#define QH_ISO_MULT(x)		((x >> 11) & 0x03)
	/* 1 */
	u32 curr;
	/* 2 - 8 */
	struct lb_hw_td td;
	/* 9 */
	u32 RESERVED;
	struct usb_ctrlrequest setup;
} __packed __aligned(4);

struct td_node {
	struct list_head	td;
	dma_addr_t		dma;
	struct lb_hw_td		*ptr;
};

/**
 * struct lb_hw_req - usb request representation
 * @req: request structure for gadget drivers
 * @queue: link to QH list
 * @tds: link to td list
 */
struct lb_hw_req {
	struct usb_request	req;
	struct list_head	queue;
	struct list_head	tds;
};

#ifdef CONFIG_USB_LOMBO_UDC
int lb_hdc_gadget_init(struct lb_hdc *lb);
void lb_hdc_gadget_destroy(struct lb_hdc *lb);
int lb_usb_charger_connect(struct lb_hdc *ci, int is_active);
void lb_hdc_gadget_connect(struct usb_gadget *gadget, int is_active);

#else
static inline int lb_hdc_gadget_init(struct lb_hdc *lb)
{
	return -ENXIO;
}

static inline void lb_hdc_gadget_destroy(struct lb_hdc *lb)
{

}

static inline int lb_usb_charger_connect(struct lb_hdc *ci, int is_active)
{
	return 0;
}

static inline void lb_hdc_gadget_connect(struct usb_gadget *gadget,
					 int is_active)
{

}

#endif /* CONFIG_USB_LOMBO_UDC */

#endif /* LOMBO_USB_UDC_DRIVER__H */

/*
 * common macros, structures and functions for lombo usb driver
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LOMBO_USB_DRIVER_H
#define __LOMBO_USB_DRIVER_H

#include <linux/usb.h>
#include <linux/usb/otg.h>
#include <linux/usb/gadget.h>
#include <linux/list.h>
#include <linux/irqreturn.h>

/* transfer descriptor page number and size */
#define TD_PAGE_COUNT		5
#define LB_HDC_PAGE_SIZE	4096UL
#define EP_MAX			32

/*
 * register offset and emun
 */

/* identification register offset define */
#define ID_OS			0x0
#define HWGENERAL_OS		0x4
#define HWHOST_OS		0x8
#define HWDEVICE_OS		0xc
#define HWTXBUF_OS		0x10
#define HWRXBUF_OS		0x14
#define SBUSCFG_OS		0x90
#define USBCTRL_OS		0x400

struct lb_hdc;

/* register index */
enum lb_hw_regs {
	CAP_CAPLENGTH,
	CAP_HCCPARAMS,
	CAP_DCCPARAMS,
	CAP_LAST = CAP_DCCPARAMS,
	OP_USBCMD,
	OP_USBSTS,
	OP_USBINTR,
	OP_DEVICEADDR,
	OP_ENDPTLISTADDR,
	OP_PORTSC,
	OP_BURSTSIZE,
	OP_OTGSC,
	OP_USBMODE,
	OP_ENDPTSETUPSTAT,
	OP_ENDPTPRIME,
	OP_ENDPTFLUSH,
	OP_ENDPTSTAT,
	OP_ENDPTCOMPLETE,
	OP_ENDPTCTRL,
	/* endptctrl1..15 follow */
	OP_LAST = OP_ENDPTCTRL + EP_MAX / 2,
};

/*
 * structurs
 */

/**
 * struct lb_hw_ep - endpoint representation
 * @ep: endpoint structure for gadget drivers
 * @dir: endpoint direction (TX/RX)
 * @num: endpoint number
 * @type: endpoint type
 * @name: string description of the endpoint
 * @qh: queue head for this endpoint
 * @wedge: is the endpoint wedged
 * @lb: pointer to the controller
 * @lock: pointer to controller's spinlock
 * @td_pool: pointer to controller's TD pool
 * @pending_td: endpoint transfer descriptors list
 */
struct lb_hw_ep {
	struct usb_ep ep;
	u8 dir;
	u8 num;
	u8 type;
	char name[16];
	struct {
		struct list_head queue;
		struct lb_hw_qh *ptr;
		dma_addr_t dma;
	} qh;
	int wedge;

	/* global resources */
	struct lb_hdc *lb;
	spinlock_t *lock;
	struct dma_pool *td_pool;
	struct td_node *pending_td;
};

/* controller role */
enum lb_role {
	LB_ROLE_HOST = 0,
	LB_ROLE_GADGET,
	LB_ROLE_END,
};

/* controller version */
enum lb_revision {
	LB_REVISION_1X = 10,	/* Revision 1.x */
	LB_REVISION_20 = 20,	/* Revision 2.0 */
	LB_REVISION_21,		/* Revision 2.1 */
	LB_REVISION_22,		/* Revision 2.2 */
	LB_REVISION_23,		/* Revision 2.3 */
	LB_REVISION_24,		/* Revision 2.4 */
	LB_REVISION_25,		/* Revision 2.5 */
	LB_REVISION_25_PLUS,	/* Revision above than 2.5 */
	LB_REVISION_UNKNOWN = 99, /* Unknown Revision */
};

/* driver mode */
enum usb_dr_mode {
	USB_DR_MODE_UNKNOWN,
	USB_DR_MODE_HOST,
	USB_DR_MODE_PERIPHERAL,
	USB_DR_MODE_OTG,
};

/**
 * struct lb_role_driver - host/gadget role driver
 * @start: start this role
 * @stop: stop this role
 * @irq: irq handler for this role
 * @suspend: system suspend handler for this role
 * @resume: system resume handler for this role
 * @name: role name string (host/gadget)
 */
struct lb_role_driver {
	int		(*start)(struct lb_hdc *lb);
	void		(*stop)(struct lb_hdc *lb);
	irqreturn_t	(*irq)(struct lb_hdc *lb);
	void		(*suspend)(struct lb_hdc *lb);
	void		(*resume)(struct lb_hdc *lb, bool power_lost);
	const char	*name;
};

/**
 * struct hw_bank - hardware register mapping representation
 * @lpm: set if the device is LPM capable
 * @phys: physical address of the controller's registers
 * @abs: absolute address of the beginning of register window
 * @cap: capability registers
 * @op: operational registers
 * @size: size of the register window
 * @regmap: register lookup table
 */
struct hw_bank {
	unsigned	lpm;
	resource_size_t	phys;
	void __iomem	*abs;
	void __iomem	*cap;
	void __iomem	*op;
	size_t		size;
	void __iomem	*regmap[OP_LAST + 1];
};

/**
 * struct lb_hdrc - lombo usb device representation
 * @dev: pointer to parent device
 * @lock: access synchronization
 * @hw_bank: hardware register mapping
 * @irq: IRQ number
 * @roles: array of supported roles for this controller
 * @role: current role
 * @is_otg: if the device is otg-capable
 * @otg_fsm_hrtimer: hrtimer for otg fsm timers
 * @hr_timeouts: time out list for active otg fsm timers
 * @enabled_otg_timer_bits: bits of enabled otg timers
 * @next_otg_timer: next nearest enabled timer to be expired
 * @hnp_polling_work: work for hnp polling
 * @work: work for role changing
 * @wq: workqueue thread
 * @qh_pool: allocation pool for queue heads
 * @td_pool: allocation pool for transfer descriptors
 * @gadget: device side representation for peripheral controller
 * @driver: gadget driver
 * @hw_ep_max: total number of endpoints supported by hardware
 * @ci_hw_ep: array of endpoints
 * @ep0_dir: ep0 direction
 * @ep0out: pointer to ep0 OUT endpoint
 * @ep0in: pointer to ep0 IN endpoint
 * @status: ep0 status request
 * @setaddr: if we should set the address on status completion
 * @address: usb address received from the host
 * @remote_wakeup: host-enabled remote wakeup
 * @suspended: suspended by host
 * @test_mode: the selected test mode
 * @platdata: platform specific information supplied by parent device
 * @vbus_active: is VBUS active
 * @phy: pointer to PHY, if any
 * @usb_phy: pointer to USB PHY, if any and if using the USB PHY framework
 * @hcd: pointer to usb_hcd for ehci host driver
 * @debugfs: root dentry for this controller in debugfs
 * @id_event: indicates there is an id event, and handled at ci_otg_work
 * @b_sess_valid_event: indicates there is a vbus event, and handled
 * @vbus_glitch_check_event: check if vbus change is a glitch
 * at ci_otg_work
 * @supports_runtime_pm: if runtime pm is supported
 * @in_lpm: if the core in low power mode
 * @wakeup_int: if wakeup interrupt occur
 * @rev: The revision number for controller
 * @adp_probe_event: indicates to enable adp probe
 * @adp_sense_event: indicates to enable adp sense
 */
struct lb_hdc {
	struct device			*dev;
	spinlock_t			lock;
	struct hw_bank			hw_bank;
	int				irq;
	struct lb_role_driver		*roles[LB_ROLE_END];
	enum lb_role			role;
	bool				is_otg;
	struct usb_otg			otg;
	struct hrtimer			otg_fsm_hrtimer;
	unsigned			enabled_otg_timer_bits;
	struct timer_list		hnp_polling_timer;
	struct work_struct		hnp_polling_work;
	struct work_struct		work;
	struct workqueue_struct		*wq;

	struct dma_pool			*qh_pool;
	struct dma_pool			*td_pool;

	struct usb_gadget		gadget;
	struct usb_gadget_driver	*driver;
	unsigned			hw_ep_max;
	struct lb_hw_ep			lb_hw_ep[EP_MAX];
	u32				ep0_dir;
	struct lb_hw_ep			*ep0out, *ep0in;

	struct usb_request		*status;
	bool				setaddr;
	u8				address;
	u8				remote_wakeup;
	u8				suspended;
	u8				test_mode;

	struct lb_hdc_platform_data	*platdata;
	int				vbus_active;
	struct phy			*phy;
	/* old usb_phy interface */
	struct usb_phy			*usb_phy;
	struct usb_hcd			*hcd;
	struct dentry			*debugfs;
	bool				id_event;
	bool				b_sess_valid_event;
	bool				vbus_glitch_check_event;
	bool				supports_runtime_pm;
	bool				in_lpm;
	bool				wakeup_int;
	enum lb_revision		rev;
	/* register save area for suspend&resume */
	u32				pm_command;
	u32				pm_status;
	u32				pm_intr_enable;
	u32				pm_frame_index;
	u32				pm_segment;
	u32				pm_frame_list;
	u32				pm_async_next;
	u32				pm_configured_flag;
	u32				pm_portsc;
	u32				pm_usbmode;
	struct work_struct		power_lost_work;
	bool				adp_probe_event;
	bool				adp_sense_event;
	struct work_struct		set_gadget_state;
	enum usb_device_state		gadget_state;
};

static inline struct lb_role_driver *lb_role(struct lb_hdc *lb)
{
	BUG_ON(lb->role >= LB_ROLE_END || !lb->roles[lb->role]);
	return lb->roles[lb->role];
}

static inline int lb_role_start(struct lb_hdc *lb, enum lb_role role)
{
	int ret;

	if (role > LB_ROLE_END)
		return -EINVAL;

	if (!lb->roles[role])
		return -ENXIO;

	ret = lb->roles[role]->start(lb);
	if (!ret)
		lb->role = role;

	return ret;
}

static inline void lb_role_stop(struct lb_hdc *lb)
{
	enum lb_role role = lb->role;

	if (role == LB_ROLE_END)
		return;

	lb->role = LB_ROLE_END;
	lb->roles[role]->stop(lb);
}

/**
 * hw_read_id_reg: reads from a identification register
 * @lb: the controller
 * @offset: offset from the beginning of identification registers region
 * @mask: bitfield mask
 *
 * returns register contents
 */
static inline u32 hw_read_id_reg(struct lb_hdc *lb, u32 offset, u32 mask)
{
	return readl(lb->hw_bank.abs + offset) & mask;
}

/**
 * hw_read: reads from a hw register
 * @lb: the controller
 * @reg: register index
 * @mask: bitfield mask
 *
 * returns register contents
 */
static inline u32 hw_read(struct lb_hdc *lb, enum lb_hw_regs reg, u32 mask)
{
	return readl(lb->hw_bank.regmap[reg]) & mask;
}

/**
 * hw_write: writes to a hw register
 * @lb: the controller
 * @reg:  register index
 * @mask: bitfield mask
 * @data: new value
 */
static inline void hw_write(struct lb_hdc *lb, enum lb_hw_regs reg,
			    u32 mask, u32 data)
{
	if (~mask)
		data = (readl(lb->hw_bank.regmap[reg]) & ~mask) | (data & mask);

	writel(data, lb->hw_bank.regmap[reg]);
}

/**
 * hw_test_and_clear: tests & clears a hw register
 * @lb: the controller
 * @reg: register index
 * @mask: bitfield mask
 *
 * returns register contents
 */
static inline u32 hw_test_and_clear(struct lb_hdc *lb, enum lb_hw_regs reg,
				    u32 mask)
{
	u32 val = readl(lb->hw_bank.regmap[reg]) & mask;

	writel(val, lb->hw_bank.regmap[reg]);
	return val;
}

/**
 * hw_test_and_write: tests & writes a hw register
 * @lb: the controller
 * @reg: register index
 * @mask: bitfield mask
 * @data: new value
 *
 * returns register contents
 */
static inline u32 hw_test_and_write(struct lb_hdc *lb, enum lb_hw_regs reg,
				    u32 mask, u32 data)
{
	u32 val = hw_read(lb, reg, ~0);

	hw_write(lb, reg, mask, data);
	return (val & mask) >> __ffs(mask);
}

/**
 * lb_otg_is_fsm_mode: runtime check if otg controller is in otg fsm mode.
 *
 * @lb: usb device
 */
static inline bool lb_otg_is_fsm_mode(struct lb_hdc *lb)
{
	return false;
}

/* other register ops function */
u32 hw_read_intr_enable(struct lb_hdc *lb);
u32 hw_read_intr_status(struct lb_hdc *lb);
int hw_device_reset(struct lb_hdc *lb);
int hw_port_test_set(struct lb_hdc *lb, u8 mode);
u8 hw_port_test_get(struct lb_hdc *lb);
int hw_wait_reg(struct lb_hdc *lb, enum lb_hw_regs reg, u32 mask,
				u32 value, unsigned int timeout_ms);
int hw_controller_reset(struct lb_hdc *lb);
void lb_hdc_ahb_config(struct lb_hdc *lb);

#endif /* __LOMBO_USB_DRIVER_H */

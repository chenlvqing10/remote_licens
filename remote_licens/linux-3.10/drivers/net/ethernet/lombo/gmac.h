/*
 * gmac.h - Generic definitions of LomboTech GMAC Driver
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __GMAC_H__
#define __GMAC_H__

#define LGMAC_DRV_NAME			"lombo gmac"
#define LGMAC_DRV_VERSION		"lombo gmac driver v0.1"

#define LGMAC_DMA_BUF_SIZE		(SZ_2K)

#define LGMAC_RX_DESC_CNT		(256)
#define LGMAC_TX_DESC_CNT		(256)

#define LGMAC_TX_DESC_THRESH(x)		((x)->tx_count / 4)

#define LGMAC_LPI_TIMER_MS		(1000)

#define LGMAC_FLOW_CTRL_TX		BIT(1)
#define LGMAC_FLOW_CTRL_RX		BIT(0)
#define LGMAC_FLOW_CTRL_OFF		(0)
#define LGMAC_FLOW_CTRL_BOTH		(LGMAC_FLOW_CTRL_TX | \
					 LGMAC_FLOW_CTRL_RX)

#define LGMAC_FLOW_CTRL_PAUSE		(0x200)

#define LGMAC_TX_COAL_TIMER_US		(40000)
#define LGMAC_TX_COAL_FRAME_MAX		(256)
#define LGMAC_TX_COAL_FRAME		(32)

#define LGMAC_TX_TIMEOUT_MS		(5000)

#define LGMAC_MII_BUS_NAME		"lbmii"

/**
 * enum lgmac_rx_frame_sta - Rx frame status.
 * @LGMAC_RX_GOOD_FRAME:	IPv4/6 no checksum error.
 * @LGMAC_RX_DISCARD_FRAME:	IPv4/6 unsupported IP Payload.
 * @LGMAC_RX_CSUM_NONE:		IPv4/6 checksum error.
 * @LGMAC_RX_LLC_SNAP:		IEEE 802.3 Type frame (length < 1536 octects)
 */
enum lgmac_rx_frame_sta {
	LGMAC_RX_GOOD_FRAME	= 0,
	LGMAC_RX_DISCARD_FRAME	= 1,
	LGMAC_RX_CSUM_NONE	= 2,
	LGMAC_RX_LLC_SNAP	= 4,
};

/**
 * enum lgmac_rx_coe_type - IP Checksum Offload in Rx.
 * @LGMAC_RX_COE_TYPE_NONE:	no IP Checksum Offload.
 * @LGMAC_RX_COE_TYPE_1:	IP Checksum Offload(Type 1) in Rx.
 * @LGMAC_RX_COE_TYPE_2:	IP Checksum Offload(Type 2) in Rx.
 */
enum lgmac_rx_coe_type {
	LGMAC_RX_COE_TYPE_NONE	= 0,
	LGMAC_RX_COE_TYPE_1	= 1,
	LGMAC_RX_COE_TYPE_2	= 2,
};

/* idma tx desc0 */
struct idma_tx_des0 {
	u32 deferred:1;
	u32 underflow_error:1;
	u32 excessive_deferral:1;
	u32 collision_count:4;
	u32 vlan_frame:1;
	u32 excessive_collisions:1;
	u32 late_collision:1;
	u32 no_carrier:1;
	u32 loss_carrier:1;
	u32 payload_error:1;
	u32 frame_flushed:1;
	u32 jabber_timeout:1;
	u32 error_summary:1;
	u32 ip_header_error:1;
	u32 time_stamp_status:1;
	u32 reserved1:13;
	u32 own:1;
};

/* idma tx desc1 */
struct idma_tx_des1 {
	u32 buffer1_size:11;
	u32 buffer2_size:11;
	u32 time_stamp_enable:1;
	u32 disable_padding:1;
	u32 second_address_chained:1;
	u32 end_ring:1;
	u32 crc_disable:1;
	u32 checksum_insertion:2;
	u32 first_segment:1;
	u32 last_segment:1;
	u32 interrupt:1;
};

/* idma rx desc0 */
struct idma_rx_des0 {
	u32 payload_csum_error:1;
	u32 crc_error:1;
	u32 dribbling:1;
	u32 mii_error:1;
	u32 receive_watchdog:1;
	u32 frame_type:1;
	u32 collision:1;
	u32 ipc_csum_error:1;
	u32 last_descriptor:1;
	u32 first_descriptor:1;
	u32 vlan_tag:1;
	u32 overflow_error:1;
	u32 length_error:1;
	u32 sa_filter_fail:1;
	u32 descriptor_error:1;
	u32 error_summary:1;
	u32 frame_length:14;
	u32 da_filter_fail:1;
	u32 own:1;
};

/* idma rx desc1 */
struct idma_rx_des1 {
	u32 buffer1_size:11;
	u32 buffer2_size:11;
	u32 reserved1:2;
	u32 second_address_chained:1;
	u32 end_ring:1;
	u32 reserved2:5;
	u32 disable_ic:1;
};

/* idma desc0 */
union idma_des0 {
	struct idma_tx_des0	tx;
	struct idma_rx_des0	rx;
	u32			val;
};

/* idma desc1 */
union idma_des1 {
	struct idma_tx_des1	tx;
	struct idma_rx_des1	rx;
	u32			val;
};

/* idma desc */
struct idma_desc {
	union idma_des0		des0;
	union idma_des1		des1;
	u32			des2;
	u32			des3;
};

/**
 * struct lombo_gmac - runtime info holder for gmac driver.
 * @base_addr:		base address.
 * @clk_gate:		ahb gate clock.
 * @clk_reset:		ahb reset clock.
 * @clk_axi:		axi gate clock.
 * @clk_parent:		parent clock.
 * @clk_core:		core clock.
 * @clk_sclk0:		sclk0 clock.
 * @clk_sclk1:		sclk1 clock.
 * @dev:		parent device.
 * @net_dev:		net device.
 * @napi:		napi struct.
 * @lock:		spin lock.
 * @tx_lock:		tx lock.
 * @tx_desc:		tx desc list.
 * @tx_skb:		tx skb pointer array.
 * @tx_skb_dma:		tx skb dma address array.
 * @tx_desc_phy:	physical address of tx desc list.
 * @tx_count:		tx desc count.
 * @tx_clean:		index of current tx desc.
 * @tx_dirty:		index of dirty tx desc.
 * @rx_desc:		rx desc list.
 * @rx_skb:		rx skb pointer array.
 * @rx_skb_dma:		rx skb dma address array.
 * @rx_desc_phy:	physical address of rx desc list.
 * @rx_count:		rx desc count.
 * @rx_clean:		index of current rx desc.
 * @rx_dirty:		index of dirty rx desc.
 * @dma_buf_sz:		dma buffer size.
 * @rx_wdt_cnt:		rx watchdog count.
 * @flow_ctrl:		flow control flags.
 * @phy_interface:	phy interface mode.
 * @phy_dev:		phy device.
 * @mii_bus:		mii bus.
 * @old_link:		current link status.
 * @old_speed:		current speed.
 * @old_duplex:		current duplex mode.
 * @phy_addr:		phy device's address.
 * @phy_rst_pin:	phy reset pin number.
 * @phy_rst_level:	phy reset level.
 * @tx_delay:		tx delay parameter.
 * @rx_delay:		rx delay parameter.
 * @rmii_osc_ext:	rmii select external-osc as clock source or not.
 * @tx_coal_cnt:	tx coalesce frame count.
 * @tx_coal_frames:	tx coalesce frame threshod.
 * @tx_coal_time:	tx coalesce time in us.
 * @tx_coal_timer:	tx coalesce timer.
 * @eee_enable:		eee enable or not.
 * @eee_active:		eee active or not.
 * @tx_lpi_enter:	tx enter lpi mode or not.
 * @tx_lpi_time:	tx waitting time in ms.
 * @eee_ctrl_timer:	eee control timer.
 */
struct lombo_gmac {
	void __iomem		*base_addr;

	struct clk		*clk_gate;
	struct clk		*clk_reset;
	struct clk		*clk_axi;
	struct clk		*clk_parent;
	struct clk		*clk_core;
	struct clk		*clk_sclk0;
	struct clk		*clk_sclk1;

	struct device		*dev;
	struct net_device	*net_dev;
	struct napi_struct	napi;
	spinlock_t		lock;
	spinlock_t		tx_lock;

	struct idma_desc	*tx_desc;
	struct sk_buff		**tx_skb;
	dma_addr_t		*tx_skb_dma;
	dma_addr_t		tx_desc_phy;
	u32			tx_count;
	u32			tx_clean;
	u32			tx_dirty;

	struct idma_desc	*rx_desc;
	struct sk_buff		**rx_skb;
	dma_addr_t		*rx_skb_dma;
	dma_addr_t		rx_desc_phy;
	u32			rx_count;
	u32			rx_clean;
	u32			rx_dirty;

	u32			dma_buf_sz;
	u32			rx_wdt_cnt;
	u32			flow_ctrl;

	phy_interface_t		phy_interface;
	struct phy_device	*phy_dev;
	struct mii_bus		*mii_bus;
	int			old_link;
	int			old_speed;
	int			old_duplex;
	int			phy_addr;
	int			phy_rst_pin;
	int			phy_rst_level;

	u32			tx_delay[3];
	u32			rx_delay[3];
	u32			rmii_osc_ext;

	u32			tx_coal_cnt;
	u32			tx_coal_frames;
	u32			tx_coal_time;
	struct timer_list	tx_coal_timer;

	int			eee_enable;
	int			eee_active;
	u32			tx_lpi_enter;
	u32			tx_lpi_time;
	struct timer_list	eee_ctrl_timer;
};

#endif /* __GMAC_H__ */

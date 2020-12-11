/*
 * gmac_const.h - CSP definitions of LomboTech GMAC Driver
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

#ifndef __GMAC_CONST_H__
#define __GMAC_CONST_H__

/* GMAC Configuration */
#define LGMAC_CONFIG_JD		BIT(22)		/* Jabber Disable */
#define LGMAC_CONFIG_BE		BIT(21)		/* Frame Burst Enable */
#define LGMAC_CONFIG_IPC	BIT(10)		/* Checksum Offload */
#define LGMAC_CONFIG_ACS	BIT(7)		/* Auto Pad or CRC Stripping */

#define LGMAC_DEF_CONFIG	(LGMAC_CONFIG_JD  | LGMAC_CONFIG_BE | \
				 LGMAC_CONFIG_IPC | LGMAC_CONFIG_ACS)

/* Frame Filter */
#define LGMAC_FFILTER_PM	BIT(4)		/* Pass All Multicast */
#define LGMAC_FFILTER_HMC	BIT(2)		/* Hash Multicast */
#define LGMAC_FFILTER_PR	BIT(0)		/* Promiscuous Mode */

#define LGMAC_HASH_TABLE_SIZE	(64)

/* LPI Control and Status */
#define LGMAC_LPI_TX_LPI_EXIT	BIT(1)		/* Transmit LPI Exit */
#define LGMAC_LPI_TX_LPI_ENTRY	BIT(0)		/* Transmit LPI Entry */

/* LPI Timer */
#define LGMAC_DEF_LPI_LS_TIMER	(1000)
#define LGMAC_DEF_LPI_TW_TIMER	(0)

/* GMAC Interrupt */
#define LGMAC_INT_LPI		BIT(10)		/* LPI Interrupt */
#define LGMAC_INT_RGMII		BIT(0)		/* RGMII Interrupt */

#define LGMAC_DEF_INT_MASK	(LGMAC_INT_LPI | LGMAC_INT_RGMII)

/* RGMII Control and Status */
#define LGMAC_RGMII_LINK_STA	BIT(3)		/* Link Status */
#define LGMAC_RGMII_SPEED_MASK	GENMASK(2, 1)	/* Link Speed Mask */
#define LGMAC_RGMII_SPEED_125	BIT(2)		/* Link Speed 125MHz */
#define LGMAC_RGMII_SPEED_25	BIT(1)		/* Link Speed 25MHz */
#define LGMAC_RGMII_SPEED_2_5	(0UL)		/* Link Speed 2.5MHz */
#define LGMAC_RGMII_LINK_MODE	BIT(0)		/* Link Mode */

/* DMA Programmable Burst Length */
#define LGMAC_DEF_DMA_PBL	(8)

/* DMA Status */
#define LGMAC_DMA_STA_FBI	BIT(13)		/* Fatal Bus Error Interrupt */
#define LGMAC_DMA_STA_RI	BIT(6)		/* Receive Interrupt */
#define LGMAC_DMA_STA_TPS	BIT(1)		/* Transmit Process Stopped */
#define LGMAC_DMA_STA_TI	BIT(0)		/* Transmit Interrupt */

#define LGMAC_DMA_STA_MASK	GENMASK(16, 0)	/* DMA Status Mask */

/* DMA Normal Interrupt */
#define LGMAC_DMA_INT_NIE	BIT(16)		/* Normal Interrupt Summary */
#define LGMAC_DMA_INT_TIE	BIT(0)		/* Transmit Interrupt */
#define LGMAC_DMA_INT_RIE	BIT(6)		/* Receive Interrupt */

#define LGMAC_DMA_INT_NORMAL	(LGMAC_DMA_INT_NIE | LGMAC_DMA_INT_RIE | \
				 LGMAC_DMA_INT_TIE)

/* DMA Abnormal Interrupt */
#define LGMAC_DMA_INT_AIE	BIT(15)		/* Abnormal Interrupt Summary */
#define LGMAC_DMA_INT_FBE	BIT(13)		/* Fatal Bus Error */
#define LGMAC_DMA_INT_UNE	BIT(5)		/* Underflow Interrupt */

#define LGMAC_DMA_INT_ABNORMAL	(LGMAC_DMA_INT_AIE | LGMAC_DMA_INT_FBE | \
				 LGMAC_DMA_INT_UNE)

#define LGMAC_DEF_DMA_INT_MASK	(LGMAC_DMA_INT_NORMAL | LGMAC_DMA_INT_ABNORMAL)

/* RI Watchdog Timer Count */
#define LGMAC_MAX_RX_WDT_CNT	(0xff)
#define LGMAC_MIN_RX_WDT_CNT	(0x20)

/* PHY Interface Type */
#define LGMAC_PHY_TYPE_MII_GMII	(0)		/* MII/GMII */
#define LGMAC_PHY_TYPE_RGMII	(1)		/* RGMII */
#define LGMAC_PHY_TYPE_RMII	(4)		/* RMII */

/* Clock Gate */
#define LGMAC_CLK_GATE_RMII_O	BIT(6)		/* RMII_O */
#define LGMAC_CLK_GATE_TXC_O	BIT(5)		/* TXC_O */
#define LGMAC_CLK_GATE_RMII_I	BIT(4)		/* CLK_RMII_I */
#define LGMAC_CLK_GATE_RX_180_I	BIT(3)		/* CLK_RX_180_I */
#define LGMAC_CLK_GATE_RX_I	BIT(2)		/* CLK_RX_I */
#define LGMAC_CLK_GATE_TX_180_I	BIT(1)		/* CLK_TX_180_I */
#define LGMAC_CLK_GATE_TX_I	BIT(0)		/* CLK_TX_I */

#define LGMAC_CLK_GATE_ALL	(LGMAC_CLK_GATE_RMII_O | \
				 LGMAC_CLK_GATE_TXC_O | \
				 LGMAC_CLK_GATE_RMII_I | \
				 LGMAC_CLK_GATE_RX_180_I | \
				 LGMAC_CLK_GATE_RX_I | \
				 LGMAC_CLK_GATE_TX_180_I | \
				 LGMAC_CLK_GATE_TX_I)

#endif /* __GMAC_CONST_H__ */

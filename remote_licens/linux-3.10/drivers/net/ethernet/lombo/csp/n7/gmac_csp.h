/*
 * gmac_csp.h - CSP definitions of LomboTech GMAC Driver
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

#ifndef __GMAC_CSP_H__
#define __GMAC_CSP_H__

/**
 * csp_gmac_set_base_config - set base configuration.
 * @base_addr: base address of gmac controller.
 * @mask: base configuration bits to be set.
 */
void csp_gmac_set_base_config(void *base_addr, u32 mask);

/**
 * csp_gmac_clr_base_config - clear base configuration.
 * @base_addr: base address of gmac controller.
 * @mask: base configuration bits to be cleared.
 */
void csp_gmac_clr_base_config(void *base_addr, u32 mask);

/**
 * csp_gmac_set_duplex_mode - set duplex mode.
 * @base_addr: base address of gmac controller.
 * @enable: 0, half-duplex mode; !0, full-duplex mode.
 */
void csp_gmac_set_duplex_mode(void *base_addr, u32 enable);

/**
 * csp_gmac_set_speed - set speed in MII/RMII/RGMII interface.
 * @base_addr: base address of gmac controller.
 * @speed: speed to set, should be 10/100/1000.
 */
void csp_gmac_set_speed(void *base_addr, u32 speed);

/**
 * csp_gmac_set_mac_enable - enable or disable the Transmitter and Receiver.
 * @base_addr: base address of gmac controller.
 * @enable: 0, disable; !0, enable.
 */
void csp_gmac_set_mac_enable(void *base_addr, u32 enable);

/**
 * csp_gmac_set_auto_strip - set automatic Pad or FCS stripping.
 * @base_addr: base address of gmac controller.
 * @acs: 0, no strip Pad or FCS field; !0 strip if frame length less than 1536.
 */
void csp_gmac_set_auto_strip(void *base_addr, u32 acs);

/**
 * csp_gmac_set_frame_filter - set frame filter configuration.
 * @base_addr: base address of gmac controller.
 * @flr: frame filter configuration bits to be set.
 */
void csp_gmac_set_frame_filter(void *base_addr, u32 flr);

/**
 * csp_gmac_set_filter_hash_hi - set upper 32 bits of address filter Hash table.
 * @base_addr: base address of gmac controller.
 * @hi: upper 32 bits of the Hash table.
 */
void csp_gmac_set_filter_hash_hi(void *base_addr, u32 hi);

/**
 * csp_gmac_set_filter_hash_lo - set low 32 bits of address filter Hash table.
 * @base_addr: base address of gmac controller.
 * @lo: low 32 bits of the Hash table.
 */
void csp_gmac_set_filter_hash_lo(void *base_addr, u32 lo);

/**
 * csp_gmac_set_csr_clock_range - set csr clock range.
 * @base_addr: base address of gmac controller.
 * @clk_rate: core clock rate of gmac controller.
 */
void csp_gmac_set_csr_clock_range(void *base_addr, u32 clk_rate);

/**
 * csp_gmac_check_mdio_busy - check mdio busy.
 * @base_addr: base address of gmac controller.
 *
 * return 0 if not busy; 1 if busy.
 */
u32 csp_gmac_check_mdio_busy(void *base_addr);

/**
 * csp_gmac_config_mdio_read - config and prepare for mdio read.
 * @base_addr: base address of gmac controller.
 * @phy_addr: phy device address.
 * @phy_reg: phy register address.
 */
void csp_gmac_config_mdio_read(void *base_addr, u32 phy_addr, u32 phy_reg);

/**
 * csp_gmac_config_mdio_write - config and click mdio write.
 * @base_addr: base address of gmac controller.
 * @phy_addr: phy device address.
 * @phy_reg: phy register address.
 */
void csp_gmac_config_mdio_write(void *base_addr, u32 phy_addr, u32 phy_reg);

/**
 * csp_gmac_mdio_reset - mdio reset.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_mdio_reset(void *base_addr);

/**
 * csp_gmac_mdio_read_data - mdio read data.
 * @base_addr: base address of gmac controller.
 *
 * return the read data.
 */
u32 csp_gmac_mdio_read_data(void *base_addr);

/**
 * csp_gmac_mdio_write_data - mdio write data.
 * @base_addr: base address of gmac controller.
 * @data: data to write.
 */
void csp_gmac_mdio_write_data(void *base_addr, u32 data);

/**
 * csp_gmac_flow_control - set flow control.
 * @base_addr: base address of gmac controller.
 * @duplex: 0, means half-duplex mode; !0 means full-duplex mode.
 * @rx_fc: 0, disable rx flow control; !0, enable.
 * @tx_fc: 0, disable tx flow control; !0, enable.
 * @time: pause time.
 */
void csp_gmac_flow_control(void *base_addr, u32 duplex, u32 rx_fc,
			   u32 tx_fc, u32 time);

/**
 * csp_gmac_set_lpi_mode - enable or disable lpi mode.
 * @base_addr: base address of gmac controller.
 * @enable: 0, disable; !0, enable.
 */
void csp_gmac_set_lpi_mode(void *base_addr, u32 enable);

/**
 * csp_gmac_get_lpi_ctrl_status - get lpi control and interrupt status.
 * @base_addr: base address of gmac controller.
 *
 * Some bits of this register will be cleared by a read into this register.
 *
 * return lpi control and interrupt status.
 */
u32 csp_gmac_get_lpi_ctrl_status(void *base_addr);

/**
 * csp_gmac_set_lpi_pls - set lpi phy link status.
 * @base_addr: base address of gmac controller.
 * @link: 0, link down; !0, link up(okay).
 */
void csp_gmac_set_lpi_pls(void *base_addr, u32 link);

/**
 * csp_gmac_set_lpi_timer - set lpi timer parameter.
 * @base_addr: base address of gmac controller.
 * @lst: link status timer, unit: ms.
 * @twt: transmit waitting timer, unit: us.
 */
void csp_gmac_set_lpi_timer(void *base_addr, u32 lst, u32 twt);

/**
 * csp_gmac_get_int_status - get interrupt status.
 * @base_addr: base address of gmac controller.
 *
 * return interrupt status.
 */
u32 csp_gmac_get_int_status(void *base_addr);

/**
 * csp_gmac_set_int_mask - set interrupt enable bits.
 * @base_addr: base address of gmac controller.
 * @mask: interrupt enable bits to be set.
 */
void csp_gmac_set_int_mask(void *base_addr, u32 mask);

/**
 * csp_gmac_get_mac_addr - get mac address.
 * @base_addr: base address of gmac controller.
 * @addr: buffer to receive the mac address.
 */
void csp_gmac_get_mac_addr(void *base_addr, u8 *addr);

/**
 * csp_gmac_set_mac_addr - set mac address.
 * @base_addr: base address of gmac controller.
 * @addr: buffer of mac address.
 */
void csp_gmac_set_mac_addr(void *base_addr, u8 *addr);

/**
 * csp_gmac_get_rgmii_status - get RGMII interface status.
 * @base_addr: base address of gmac controller.
 *
 * return RGMII interface status.
 */
u32 csp_gmac_get_rgmii_status(void *base_addr);

/**
 * csp_gmac_dma_soft_reset - dma software reset.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_dma_soft_reset(void *base_addr);

/**
 * csp_gmac_get_dma_reset_status - get dma software reset status.
 * @base_addr: base address of gmac controller.
 *
 * return 0 if reset operation is completed; 1 not complete.
 */
u32 csp_gmac_get_dma_reset_status(void *base_addr);

/**
 * csp_gmac_set_dma_burst_len - set dma programmable burst length.
 * @base_addr: base address of gmac controller.
 * @pbl: programmable burst length.
 * @fixed: 0, disable fixed burst; !0, enable.
 */
void csp_gmac_set_dma_burst_len(void *base_addr, u32 pbl, u32 fixed);

/**
 * csp_gmac_dma_tx_poll_demand - tx poll demand, click dma transmit.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_dma_tx_poll_demand(void *base_addr);

/**
 * csp_gmac_set_rx_desc_addr - set rx desc base address.
 * @base_addr: base address of gmac controller.
 * @addr: base address of the rx desc list.
 */
void csp_gmac_set_rx_desc_addr(void *base_addr, u32 addr);

/**
 * csp_gmac_set_tx_desc_addr - set tx desc base address.
 * @base_addr: base address of gmac controller.
 * @addr: base address of the tx desc list.
 */
void csp_gmac_set_tx_desc_addr(void *base_addr, u32 addr);

/**
 * csp_gmac_get_dma_status - get dma interrupt status.
 * @base_addr: base address of gmac controller.
 *
 * return dma interrupt status.
 */
u32 csp_gmac_get_dma_status(void *base_addr);

/**
 * csp_gmac_clr_dma_status - clear dma interrupt bits.
 * @base_addr: base address of gmac controller.
 * @clr_mask: interrupt bits to be cleared.
 */
void csp_gmac_clr_dma_status(void *base_addr, u32 clr_mask);

/**
 * csp_gmac_dma_start_rx - start dma rx process.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_dma_start_rx(void *base_addr);

/**
 * csp_gmac_dma_stop_rx - stop dma rx process.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_dma_stop_rx(void *base_addr);

/**
 * csp_gmac_dma_start_tx - start dma tx process.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_dma_start_tx(void *base_addr);

/**
 * csp_gmac_dma_stop_tx - stop dma tx process.
 * @base_addr: base address of gmac controller.
 */
void csp_gmac_dma_stop_tx(void *base_addr);

/**
 * csp_gmac_set_rx_threshold - set rx Store-and-Forward, or set rx threshold.
 * @base_addr: base address of gmac controller.
 * @sf: 0, disable rx Store-and-Forward; !0, enable.
 * @th: rx threshold to set, if rx Store-and-Forward is disabled.
 */
void csp_gmac_set_rx_threshold(void *base_addr, u32 sf, u32 th);

/**
 * csp_gmac_set_tx_threshold - set tx Store-and-Forward, or set tx threshold.
 * @base_addr: base address of gmac controller.
 * @sf: 0, disable tx Store-and-Forward; !0, enable.
 * @th: tx threshold to set, if tx Store-and-Forward is disabled.
 */
void csp_gmac_set_tx_threshold(void *base_addr, u32 sf, u32 th);

/**
 * csp_gmac_set_dma_int_enable - set dma interrupt enable bits.
 * @base_addr: base address of gmac controller.
 * @int_mask: interrupt enable bits to be set.
 */
void csp_gmac_set_dma_int_enable(void *base_addr, u32 int_mask);

/**
 * csp_gmac_get_dma_int_enable - get dma interrupt enable bits.
 * @base_addr: base address of gmac controller.
 *
 * return dma interrupt enable bits.
 */
u32 csp_gmac_get_dma_int_enable(void *base_addr);

/**
 * csp_gmac_set_rx_watchdog - set rx watchdog timer count.
 * @base_addr: base address of gmac controller.
 * @riwt: rx watchdog timer count to be set.
 */
void csp_gmac_set_rx_watchdog(void *base_addr, u32 riwt);

/**
 * csp_gmac_set_axi_bus_mode - set axi bus mode.
 * @base_addr: base address of gmac controller.
 * @axi_mode: value of axi bus mode to be set.
 */
void csp_gmac_set_axi_bus_mode(void *base_addr, u32 axi_mode);

/**
 * csp_gmac_set_rmii_tx_clk_invert - enable or disable rmii tx clock invert.
 * @base_addr: base address of gmac controller.
 * @enable: 0, disable; !0, enable.
 */
void csp_gmac_set_rmii_tx_clk_invert(void *base_addr, u32 enable);

/**
 * csp_gmac_set_rmii_tx_clk_src - set rmii tx clock source.
 * @base_addr: base address of gmac controller.
 * @src: 0, use internal-OSC; !0, use external-OSC.
 */
void csp_gmac_set_rmii_tx_clk_src(void *base_addr, u32 src);

/**
 * csp_gmac_set_phy_interface - set phy interface type.
 * @base_addr: base address of gmac controller.
 * @phy_if: phy interface type to be set.
 */
void csp_gmac_set_phy_interface(void *base_addr, u32 phy_if);

/**
 * csp_gmac_enable_clk_gate - enable clock gate.
 * @base_addr: base address of gmac controller.
 * @mask: clock gate bits to be enabled.
 */
void csp_gmac_enable_clk_gate(void *base_addr, u32 mask);

/**
 * csp_gmac_disable_clk_gate - disable clock gate.
 * @base_addr: base address of gmac controller.
 * @mask: clock gate bits to be disabled.
 */
void csp_gmac_disable_clk_gate(void *base_addr, u32 mask);

/**
 * csp_gmac_set_rx_smp_delay - set rx sample clock delay.
 * @base_addr: base address of gmac controller.
 * @delay: delay value to set.
 */
void csp_gmac_set_rx_smp_delay(void *base_addr, u32 delay);

/**
 * csp_gmac_set_tx_drv_delay - set tx driver clock delay.
 * @base_addr: base address of gmac controller.
 * @delay: delay value to set.
 */
void csp_gmac_set_tx_drv_delay(void *base_addr, u32 delay);

/**
 * csp_gmac_default_mac_addr - get default mac address.
 * @base_addr: base address of gmac controller.
 * @addr: buffer to receive the mac address.
 *
 * return 0 if success; otherwise failed.
 */
int csp_gmac_default_mac_addr(void *base_addr, u8 *addr);

#endif /* __GMAC_CSP_H__ */

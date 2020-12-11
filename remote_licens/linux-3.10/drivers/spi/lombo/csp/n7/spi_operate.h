/*
 * spi_operate.h - CSP define of LomboTech SPI Controller
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

#include <linux/types.h>

#define LOMBO_SPI_NUM         (3)

/* register offset define */
#define LOMBO_SPI_CTL0           (0x04)  /* control register 0 */
#define LOMBO_SPI_CTL1           (0x08)  /* control register 1 */
#define LOMBO_SPI_CTL2           (0x0C)  /* control register 2 */
#define LOMBO_SPI_EN             (0x10)  /* enable register */
#define LOMBO_SPI_MWCTL          (0x14)  /* microwire control register */
#define LOMBO_SPI_SSE            (0x18)  /* slave select enable register */
#define LOMBO_SPI_BR             (0x1C)  /* baud rate register */
#define LOMBO_SPI_FIFO_TH        (0x20)  /* FIFO threshold level  register */
#define LOMBO_SPI_FIFO_FL        (0x24)  /* FIFO fill level register */
#define LOMBO_SPI_STATUS         (0x28)  /* status register */
#define LOMBO_SPI_INT_EN         (0x2C)  /* interrupt enable register */
#define LOMBO_SPI_INT_PD         (0x30)  /* interrupt pending register */
#define LOMBO_SPI_INT_CLR        (0x34)  /* interrupt clear register */
#define LOMBO_SPI_DMA_CTL        (0x38)  /* DMA control register */
#define LOMBO_SPI_RX_SD          (0x3C)  /* rx sample delay register */
#define LOMBO_SPI_DDR_TDE        (0x40)  /* DDR tx driver edge register */
#define LOMBO_SPI_XIP_MOD_BITS   (0x44)  /* XIP mode bit register */
#define LOMBO_SPI_XIP_INST       (0x48)  /* XIP instruction register */
#define LOMBO_SPI_INST           (0x4C)  /* instruction register */
#define LOMBO_SPI_ADDR0          (0x50)  /* address register 0 */
#define LOMBO_SPI_ADDR1          (0x54)  /* address register 1 */
#define LOMBO_SPI_DATA           (0x60)  /* data register */
#define LOMBO_SPI_FTR            (0xE0)  /* feature register */
#define LOMBO_SPI_DBG0           (0xF0)  /* debug 0 register */

#define LOMBO_SPI_INT_PD_TX_FO   (0x1 << 1)
#define LOMBO_SPI_INT_PD_RX_FU   (0x1 << 2)
#define LOMBO_SPI_INT_PD_RX_FO   (0x1 << 3)

#define LOMBO_SPI_CLK_FACTOR     (2)

extern void csp_spi_set_baud_rate(void *base, u32 clk_rate, u32 baud_rate);
extern u32 csp_spi_get_baud_rate(void *base, u32 clk_rate);
extern void csp_spi_en_cfg(void *base, u32 enable);
extern void csp_spi_xip_disable(void *base);
extern void csp_spi_frame_format_cfg(void *base, u32 frame_format);
extern void csp_spi_trans_type_cfg(void *base, u32 trans_type);
extern void csp_spi_wait_cycle_cfg(void *base, u32 wait_cycle);
extern void csp_spi_all_pending_clear(void *base);
extern void csp_spi_transfer_mode_cfg(void *base, u32 trans_mode);
extern void csp_spi_inst_len_cfg(void *base, u32 inst_len);
extern void csp_spi_addr_len_cfg(void *base, u32 addr_len);
extern void csp_spi_dfs_cfg(void *base, u32 dfs);
extern void csp_spi_ndf_cfg(void *base, u16 ndf_tx, u16 ndf_rx);
extern void csp_spi_inst_cfg(void *base, u32 inst);
extern void csp_spi_addr0_cfg(void *base, u32 addr0);
extern void csp_spi_addr1_cfg(void *base, u32 addr1);
extern u32 csp_spi_is_rx_fifo_not_empty(void *base);
extern u32 csp_spi_read_data(void *base);
extern u32 csp_spi_is_tx_fifo_not_full(void *base);
extern void csp_spi_write_data(void *base, u32 data);
extern void csp_spi_cs_active_cfg(void *base, u32 cs_num, u32 active);
extern void csp_spi_cs_by_sw(void *base, u32 cs_num);
extern void csp_spi_cs_by_hw(void *base, u32 cs_num);
extern void csp_spi_cs_en_cfg(void *base, u32 cs_num, u32 enable);
extern void csp_spi_dma_tx_th_cfg(void *base, u32 threshold);
extern void csp_spi_dma_rx_th_cfg(void *base, u32 threshold);
extern void csp_spi_dma_tx_en_cfg(void *base, u32 enable);
extern void csp_spi_dma_rx_en_cfg(void *base, u32 enable);
extern void csp_spi_mode_cfg(void *base, u32 mode);
extern void csp_spi_int_en_err_cfg(void *base, u32 enable);
extern u32 csp_spi_qry_irq_pending(void *base);
extern void csp_spi_clr_irq_pending(void *base, u32 pending);
extern u32 csp_spi_fsm_finish(void *base);
extern u32 csp_spi_int_en_err_txfo(void *base, u32 pending);
extern u32 csp_spi_int_en_err_rxfu(void *base, u32 pending);
extern u32 csp_spi_int_en_err_rxfo(void *base, u32 pending);
extern u32 csp_spi_get_dma_src_addr(u32 base);
extern u32 csp_spi_get_dma_dst_addr(u32 base);
extern void csp_spi_rx_sample_delay_cfg(void *base, u32 baud);
extern void csp_spi_rx_clk_delay_cfg(void *base, u32 baud);
extern void csp_spi_frame_format_sel(void *base, u32 value);
extern void csp_spi_set_little_endian(void *base);

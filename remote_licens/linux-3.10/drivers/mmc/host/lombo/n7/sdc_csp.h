/*
 * sdc_csp.c - SDC host driver code for LomboTech
 * sdc hardware operation interface
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
#ifndef ___SDC___CSP__H___
#define ___SDC___CSP__H___

/******************************************************************************
 * sdc clock control
 *****************************************************************************/

/* Make the clk register take effect at once */
void csp_sdc_update_clk_pre(void *base_addr);
/* Restore the cmd register after clock updated. */
/* We should called this after we have got the UCF pending. */
void csp_sdc_update_clk_post(void *base_addr);

/**
 * csp_sdc_set_clk_div - set the internal clock divder
 * @base_addr:	base address for SDC
 * @val:	clock divder value, we always set it 0 to bypass the divder
 */
void csp_sdc_set_clk_div(void *base_addr, u32 val);

/**
 * csp_sdc_set_clk_src - set the internal clock source
 * @base_addr:	base address for SDC
 * @val:	clock source index, always 0
 */
void csp_sdc_set_clk_src(void *base_addr, u32 val);

/**
 * csp_sdc_enable_clk - enable/disable the SDC clk
 * @base_addr:	base address for SDC
 * @enable:	0, disable; 1, enable
 */
void csp_sdc_enable_clk(void *base_addr, u32 enable);

/**
 * csp_sdc_enable_clk_lp - enable/disable the SDC clk low power mode
 * @base_addr:	base address for SDC
 * @enable:	0, disable; 1, enable
 */
void csp_sdc_enable_clk_lp(void *base_addr, u32 enable);

/* config the SDC clk */
void csp_sdc_set_clk(void *base_addr);

/******************************************************************************
 * sdc reset control
 *****************************************************************************/

/* reset the IDMA interface and FIFO */
void csp_sdc_data_reset(void *base_addr);

#if 0
/* reset the IDMA register */
void csp_sdc_idma_soft_reset(void *base_addr);
#endif

/* reset the sdc controller
 * Note: does not affect any registers or DMA interface */
void csp_sdc_host_reset(void *base_addr);

/* reset the whole SDC */
void csp_sdc_reset_all(void *base_addr);

/******************************************************************************
 * sdc voltage control
 *****************************************************************************/

/**
 * csp_sdc_set_voltage - Info the signal voltage level to the SDC
 * @base_addr:	base address for SDC
 * @volt:	voltage level bit, can be SDC_VOLT_3_3 or SDC_VOLT_1_8
 */
void csp_sdc_set_voltage(void *base_addr, u32 volt);

/******************************************************************************
 * sdc calibrate control
 *****************************************************************************/

/* start calibrate */
void csp_sdc_clbr_start(void *base_addr);

/* check whether calibrate done */
u32 csp_sdc_is_clbr_done(void *base_addr);

/* check whether calibrate locked */
u32 csp_sdc_is_clbr_locked(void *base_addr);

/* get calibrate result */
u32 csp_sdc_get_clbr_status(void *base_addr);

/******************************************************************************
 * sdc timing control
 *****************************************************************************/

/**
 * csp_sdc_set_timing - Set the signal timing
 * @base_addr:	base address for SDC
 * @tx_phase:	TX phase (can be SDC_TX_PHASE_90、180)
 * @rx_phase:	RX phase (can be SDC_RX_PHASE_0、90、180、270)
 * @rx_cclk_dly:	delay between cclk_in_sample and cclk_in
 */
void csp_sdc_set_timing(void *base_addr, u32 tx_phase, u32 rx_phase,
			u32 rx_cclk_dly);

/**
 * csp_sdc_set_timing - Set the signal timing
 * @base_addr:	base address for SDC
 * @rx_cclk_dly:	delay between cclk_in_sample and cclk_in
 */
void csp_sdc_set_rx_dly(void *base_addr, u32 rx_cclk_dly);

/******************************************************************************
 * sdc timeout control
 *****************************************************************************/

/**
 * csp_sdc_set_rsp_timeout - Set the response timeout clock count
 * @base_addr:	base address for SDC
 * @rto:	response timeout clock count
 */
void csp_sdc_set_rsp_timeout(void *base_addr, u32 rto);

/**
 * csp_sdc_set_data_timeout - Set the response timeout clock count
 * @base_addr:	base address for SDC
 * @long_step:	0, means step is 1; 1, means step is SDC_LONG_TO_STEP_CNT
 * @dto_cnt:	data timeout count
 */
void csp_sdc_set_data_timeout(void *base_addr, u32 long_step, u32 dto_cnt);

/******************************************************************************
 * sdc bus width control
 *****************************************************************************/

/**
 * csp_sdc_set_bus_width - Set the host bus width
 * @base_addr:	base address for SDC
 * @width:	bus width indicator (can be SDC_BUS_WIDTH_1、4、8)
 */
void csp_sdc_set_bus_width(void *base_addr, u32 width);

/******************************************************************************
 * sdc data transfer configure
 *****************************************************************************/

/**
 * csp_sdc_set_fifo_th - Set fifo threshold for RX and TX
 * @base_addr:	base address for SDC
 * @rx_wm:	rx water mark
 * @tx_wm:	tx water mark
 */
void csp_sdc_set_fifo_th(void *base_addr, u32 rx_wm, u32 tx_wm);

/**
 * csp_sdc_set_fifo_th - Set fifo threshold for RX and TX
 * @base_addr:	base address for SDC
 * @burst_len:	IDMA burst len (can be SDC_IDMA_BURST_1、4、8、16)
 */
void csp_sdc_set_idma_burst(void *base_addr, u32 burst_len);

/**
 * csp_sdc_set_card_th - Set card threshold for read and write
 * @base_addr:	base address for SDC
 * @cth:	card threshold
 * @cwte:	enable card write threshold or not; 0, disable; 1, enable
 * @crte:	enable card read threshold or not; 0, disable; 1, enable
 */
void csp_sdc_set_card_th(void *base_addr, u32 cth, u32 cwte, u32 crte);

/**
 * csp_sdc_set_trans_mode - Set card threshold for read and write
 * @base_addr:	base address for SDC
 * @trans_mode:	transfer mode (SDC_TRANS_MODE_POLL or SDC_TRANS_MODE_IDMA)
 */
void csp_sdc_set_trans_mode(void *base_addr, u32 trans_mode);

/**
 * csp_sdc_set_byte_cnt - Info SDC how many bytes to transfer
 * @base_addr:	base address for SDC
 * @byte_cnt:	bytes to transfer
 */
void csp_sdc_set_byte_cnt(void *base_addr, u32 byte_cnt);

/**
 * csp_sdc_set_byte_cnt - Info SDC the data block size
 * @base_addr:	base address for SDC
 * @blk_size:	data block size
 */
void csp_sdc_set_blk_size(void *base_addr, u32 blk_size);

/******************************************************************************
 * sdc command configure
 *****************************************************************************/

/**
 * csp_sdc_set_init_seq - Set clocks count for init seq
 * @base_addr:	base address for SDC
 * @seq_cnt:	clocks count for init seq, should be multiple of 8,
 *		upper limit is 128, typically 8 or 80.
 */
void csp_sdc_set_init_seq(void *base_addr, u32 seq_cnt);

/**
 * csp_sdc_send_cmd - Set clocks count for init seq
 * @base_addr:	base address for SDC
 * @cmd_index:	command index
 * @arg:	command argument
 * @conf:	command configure
 *
 * return 0 when success, otherwise error line number
 */
u32 csp_sdc_send_cmd(void *base_addr, u32 cmd_index, u32 arg, u32 conf);

/******************************************************************************
 * sdc interrput enable/pending/clear
 *****************************************************************************/

/**
 * csp_sdc_int_enable - Global interrupt enable/disable
 * @base_addr:	base address for SDC
 * @enable:	0: disable, 1: enable
 */
void csp_sdc_int_enable(void *base_addr, u32 enable);

/**
 * csp_sdc_clear_set_ints - clear some int enable bit,
 *			    and than set other int enable bit
 * @base_addr:	base address for SDC
 * @clear_mask:	int enable bits to be clear
 * @set_mask:	int enable bits to be set
 */
void csp_sdc_clear_set_ints(void *base_addr, u32 clear_mask, u32 set_mask);

/**
 * csp_sdc_enable_ints - enable the specified interrputs
 * @base_addr:	base address for SDC
 * @int_mask:	int enable bits to be set
 */
void csp_sdc_enable_ints(void *base_addr, u32 int_mask);

/**
 * csp_sdc_disable_ints - enable the specified interrputs
 * @base_addr:	base address for SDC
 * @int_mask:	int enable bits to be set
 */
void csp_sdc_disable_ints(void *base_addr, u32 int_mask);

/* get int pending status */
u32 csp_sdc_get_int_pd(void *base_addr);

/**
 * csp_sdc_clr_pd - clear int pending status
 * @base_addr:	base address for SDC
 * @pd_mask:	int pending bits status to be clear
 */
void csp_sdc_clr_pd(void *base_addr, u32 pd_mask);

/**
 * csp_sdc_chk_err - check whether there are error pendings occur
 * @base_addr:	base address for SDC
 *
 * return 0 when not error, otherwise 1
 */
u32 csp_sdc_chk_err(void *base_addr);

/**
 * csp_sdc_chk_pd - check whether all the expected int pending have been raise
 * @base_addr:	base address for SDC
 * @pd_mask:	int pending bits to be check
 *
 * return 1 when all the expected pending bits are set, otherwise 0
 */
u32 csp_sdc_chk_pd(void *base_addr, u32 pd_mask);

/******************************************************************************
 * sdc status
 *****************************************************************************/

/* get the raw status of SDC */
u32 csp_sdc_get_status(void *base_addr);

/**
 * csp_sdc_chk_rsp_index - check whether the command index is right
 * @base_addr:	base address for SDC
 * @cmd_index:	expected command index
 *
 * return 0 when right, otherwise 1
 */
u32 csp_sdc_chk_rsp_index(void *base_addr, u32 cmd_index);

/******************************************************************************
 * sdc response
 *****************************************************************************/

/* get the raw response from rsp0 register */
u32 csp_sdc_get_rsp0(void *base_addr);

/* get the raw response from rsp1 register */
u32 csp_sdc_get_rsp1(void *base_addr);

/* get the raw response from rsp2 register */
u32 csp_sdc_get_rsp2(void *base_addr);

/* get the raw response from rsp3 register */
u32 csp_sdc_get_rsp3(void *base_addr);

/******************************************************************************
 * sdc FIFO status
 *****************************************************************************/

/* get data count in FIFO */
u32 csp_sdc_get_fifo_cnt(void *base_addr);

/******************************************************************************
 * sdc polling read/write
 *****************************************************************************/

/**
 * csp_sdc_pio_write - polling write (CPU mode write)
 * @base_addr:	base address for SDC
 * @buf:	point to the data buffer
 * @byte_cnt:	bytes to write (should be multiple of 4)
 */
u32 csp_sdc_pio_write(void *base_addr, void *buf, u32 byte_cnt);

/**
 * csp_sdc_pio_read - polling read (CPU mode read)
 * @base_addr:	base address for SDC
 * @buf:	point to the data buffer
 * @byte_cnt:	bytes to read (should be multiple of 4)
 */
u32 csp_sdc_pio_read(void *base_addr, void *buf, u32 byte_cnt);

/******************************************************************************
 * sdc IDMA
 *****************************************************************************/

/**
 * csp_sdc_idma_desc_conf - configure the idma descriptors table
 * @base_addr:	the first descriptor's start address
 * @skip_len:	gap between two descriptors (only make sense in dual-buf mode )
 * @ram_sle:	select descriptors location
 *		(can be SDC_IDMA_DESC_IN_SRAM or SDC_IDMA_DESC_IN_DDR)
 */
void csp_sdc_idma_desc_conf(void *base_addr, u32 desc_ba, u32 skip_len,
			    u32 ram_sle);

/**
 * csp_sdc_chk_idma_err - configure the idma descriptors table
 * @base_addr:	the first descriptor's start address
 * @skip_len:	gap between two descriptors (only make sense in dual-buf mode )
 * @ram_sle:	select descriptors location
 *		(can be SDC_IDMA_DESC_IN_SRAM or SDC_IDMA_DESC_IN_DDR)
 */
u32 csp_sdc_chk_idma_err(void *base_addr);

/**
 * csp_sdc_chk_idma_pd - check whether all the expected idma-int pending have been raise
 * @base_addr:	base address for SDC
 * @pd_mask:	idma-int pending bits to be check
 *
 * return 1 when all the expected pending bits are set, otherwise 0
 */
u32 csp_sdc_chk_idma_pd(void *base_addr, u32 pd_mask);

/* dump SDC registers for debug */
void csp_sdc_dumpregs(void *base_addr, const char *host_name);

/******************************************************************************
 * sau ldo
 *****************************************************************************/
/**
 * csp_sau_cfg_1v8_ldo - set sdc ldo
 * @enable:	0, use 3.3V voltage;
 *		1, use 1.8V voltage;
 */
void csp_sau_cfg_1v8_ldo(u32 sdc_idx, u32 enable);

#endif /* ___SDC___CSP__H___ */

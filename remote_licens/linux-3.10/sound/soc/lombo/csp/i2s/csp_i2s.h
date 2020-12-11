/*
 * csp_i2s.h - Generic definitions for LomboTech SoCs
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

#ifndef __CSP_I2S_H
#define __CSP_I2S_H

#define LOMBO_DIV_MCLK	0
#define LOMBO_DIV_BCLK	1

#define LOMBO_I2S_TX	0
#define LOMBO_I2S_RX	1

/* master/slave mode */
enum i2s_ms_mode {
	I2S_MASTER,
	I2S_SLAVE
};

/* I2S data fmt */
enum i2s_format {
	I2S_STANDARD,
	I2S_MSB,
	I2S_LSB
};

/* I2S/PCM mode */
enum i2s_mode {
	I2S_MODE,
	LEFT_I2S_MODE,
	RIGHT_I2S_MODE,
	PCM_MODE
};

/* tdm mode */
enum tdm_mode {
	TDM_MOD1x1,
	TDM_MOD1x2,
	TDM_MOD1x4,
	TDM_MOD1x8,
	TDM_MOD2x1,
	TDM_MOD2x2,
	TDM_MOD2x4,
	TDM_MOD2x8,
	TDM_MOD4x1,
	TDM_MOD4x2,
	TDM_MOD4x4
};


/* PCM tx mode */
enum pcm_mode {
	PCM_MOD0,
	PCM_MOD1
};

/* PCM data mode */
enum pcm_data_mode {
	LINEAR,
	A_LAW,
	U_LAW
};

/* PCM law mode */
enum pcm_law_mode {
	NO_COMPAND,
	A_LAW_DECOMPRESS,
	U_LAW_DECOMPRESS,
	A_LAW_COMPAND,
	U_LAW_COMPAND
};

/* enable i2s block */
void csp_i2s_enable(void *regs);

/* disable i2s block */
void csp_i2s_disable(void *regs);

/* open the tx channel */
void csp_i2s_tx_open(void *regs);

/* close i2s tx channel */
void csp_i2s_tx_close(void *regs);

/* open i2s rx channel */
void csp_i2s_rx_open(void *regs);

/* close  i2s rx channel */
void csp_i2s_rx_close(void *regs);

/* set I2S/PCM mode */
void csp_i2s_set_mode(void *regs, enum i2s_mode mode);

/* set master/slave mode */
void csp_i2s_set_ms_mode(void *regs, enum i2s_ms_mode mode);

/* set sample bitwidth */
void csp_i2s_set_wlen(void *regs, u32 wlen, int dir);

/* set tdm mode */
void csp_i2s_set_tdm_mode(void *regs, enum tdm_mode mode);

/* set pcm data fmt, a-law¡¢u-law */
void csp_i2s_set_law(void *regs, enum pcm_law_mode mode, int dir);

/* set pcm tx mode */
void csp_i2s_set_pcm_mode(void *regs, enum pcm_mode mode);

/* set pcm data fmt, a-law¡¢u-law */
void csp_i2s_set_pdmod(void *regs, enum pcm_data_mode mode);

/* set WSS£¬default 32 */
void csp_i2s_set_wss(void *regs, u32 wss);

/* set clk divider */
void csp_i2s_set_div(void *regs, u32 div_id, u32 div);

/* set i2s data fmt */
void csp_i2s_set_format(void *regs,  enum i2s_format fmt);

/* SDIN dma addr */
u32 csp_i2s_dmaaddr_in(void);

/* SDOUT dma addr */
u32 csp_i2s_dmaaddr_out(void);

/* if tx fifo empty triggered */
u32 csp_i2s_tx_empty(void *regs);

/* if rx fifo full triggered */
u32 csp_i2s_rx_full(void *regs);

void csp_i2s_clear_txfe(void *regs);

void csp_i2s_clear_rxda(void *regs);

u32 csp_i2s_readl(void *regs);

u32 csp_i2s_readr(void *regs);

void csp_i2s_writel(void *regs, u32 val);

void csp_i2s_writer(void *regs, u32 val);


#endif /* __CSP_I2S_H */


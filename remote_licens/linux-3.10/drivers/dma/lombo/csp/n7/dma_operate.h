/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Lombo n7 DMA controller register operation interface
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

#ifndef ___N7_DMA___H___
#define ___N7_DMA___H___

#include <linux/types.h>

/**
 * constant define
 */
#define DMA_MAX_DESC_BYTES	33554432
#define CH_ENABLE		0x1
#define CH_DISABLE		0x0

#define CH_ABORT_EN		0x1
#define CH_ABORT_DIS		0x0

#define CH_RESUME_TRANS		0x1
/* channel control0 */
#define CH_1_BURST_LEN		0x0
#define CH_4_BURST_LEN		0x1
#define CH_8_BURST_LEN		0x2
#define CH_1_BYTE_WIDTH		0x0
#define CH_2_BYTES_WIDTH	0x1
#define CH_4_BYTES_WIDTH	0x2
#define CH_8_BYTES_WIDTH	0x3
#define CH_ADDR_INC		0x0
#define CH_ADDR_STA		0x1
/* channel control1 */
#define CH_LLI_VALID		0x1
#define CH_LLI_INVALID		0x0
#define CH_LLI_LAST		0x1
#define CH_LLI_NO_LAST		0x0
#define CH_LLI_INT_ENABLE	0x1
#define CH_LLI_INT_DISABLE	0x0
/* channel cfg */
#define CH_MEM_TO_MEM		0x0
#define CH_MEM_TO_DEV		0x1
#define CH_DEV_TO_MEM		0x2
#define CH_REG_TRANS		0x0
#define CH_CHAINS_TRANS		0x3
/* channel interrupt */
#define CH_INT_ENABLE		0x1
#define CH_INT_DISABLE		0x0
#define CH_ALL_BITS		0xffffffff
/* channel clear pending */
#define CH_CLEAR_PENDING	0x1

void csp_dma_set_channel_enable(u32 index, u32 value, void *base);
void csp_dma_set_channel_abort(u32 index, u32 value, void *base);
void csp_dma_resume_channel_trans(u32 index, void *base);
void csp_dma_halt_channel_trans(u32 index, void *base);
void csp_dma_set_channel_cfg(u32 index, u32 direction, u32 priority,
				u32 slave_id, void *base);
u32 csp_dma_org_channel_ctrl0(u32 sag, u32 dag, u32 swidth, u32 dwidth,
				u32 src_blen, u32 dst_blen);
u32 csp_dma_org_channel_ctrl1(void);
void csp_dma_set_channel_lli_last(u32 *value, bool is_last);
void csp_dma_set_channel_llp(u32 index, u32 addr, void *base);
void csp_dma_set_channel_int_en(u32 index, u32 half_blk, u32 tran,
				u32 blk, void *base);
void csp_dma_clear_channel_all_int(u32 index, void *base);
u32 csp_dma_is_ch_comb_int_pd(u32 index, void *base);
u32 csp_dma_is_ch_tran_int_pd(u32 index, void *base);
u32 csp_dma_is_ch_blk_int_pd(u32 index, void *base);
u32 csp_dma_is_ch_hblk_int_pd(u32 index, void *base);
u32 csp_dma_is_ch_err_int_pd(u32 index, void *base);
u32 csp_dma_is_ch_abort_int_pd(u32 index, void *base);
u32 csp_dma_is_ch_halt_int_pd(u32 index, void *base);
void csp_dma_clear_ch_tran_int_pd(u32 index, void *base);
void csp_dma_clear_ch_blk_int_pd(u32 index, void *base);
void csp_dma_clear_ch_hblk_int_pd(u32 index, void *base);
void csp_dma_clear_ch_err_int_pd(u32 index, void *base);
void csp_dma_clear_ch_abort_int_pd(u32 index, void *base);
void csp_dma_clear_ch_halt_int_pd(u32 index, void *base);

#endif /* ___N7_DMA___H___ */

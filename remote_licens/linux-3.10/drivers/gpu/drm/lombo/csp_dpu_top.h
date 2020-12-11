/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __CSP_DPU_TOP_H__
#define __CSP_DPU_TOP_H__

#include <linux/types.h>

#define DPU_CLK_PARENT_MAX 2
#define DPU_CLK_DIV_MAX 16
#define DPU_ADPLL_NUM 2

#define DPU_ADPLL_STATUS_MASK_FERR	(0x1FF << 0)
#define DPU_ADPLL_STATUS_MASK_FLOCK	0x00008000 /* (1 << 15) */
#define DPU_ADPLL_STATUS_MASK_PERR	(0x7F << 16)
#define DPU_ADPLL_STATUS_MASK_PLOCK	0x80000000 /* (1 << 31) */

#define DPU_SCLK_PARENT_EXT 0x0
#define DPU_SCLK_PARENT_ADPLL 0x1

#define DPU_SCLK_ADPLL_N_MIN	13
#define DPU_SCLK_ADPLL_N_MAX	34

typedef enum tag_dpu_mod_t {
	DPU_MOD_DC0,
	DPU_MOD_DC1,
	DPU_MOD_SE0,
	DPU_MOD_SE1,
	DPU_MOD_ROT,
	DPU_MOD_DIT,
	DPU_MOD_WB,
	DPU_MOD_SUB_MAX,
	DPU_MOD_ADPLL1 = 0xc,
	DPU_MOD_ADPLL0 = 0xd,
	DPU_MOD_TIMER = 0xe,
	DPU_MOD_TOP = 0xf,
} dpu_mod;

/* config of adpll enable */
struct dpu_adpll_config {
	u32 n; /* pll n factor, must be >= 5 */
	u32 tune0;
	u32 tune1;
};

s32 csp_dpu_set_register_base(void *base);
s32 csp_dpu_get_register_base(unsigned long *addr);

s32 csp_dpu_set_udelay_func(void (*udelay)(unsigned long));

/* csp_dpu_clk_assert - reset the specified clk */
s32 csp_dpu_clk_assert(u32 id);
/* csp_dpu_clk_deassert - release the specified clk(normal work state) */
s32 csp_dpu_clk_deassert(u32 id);
/*
 * csp_dpu_clk_set_parent - set parent
 * @id: module id
 * @parent: clk index of parent clk
 */
s32 csp_dpu_clk_set_parent(u32 id, u32 parent);
s32 csp_dpu_clk_get_parent(u32 id, u32 *parent);

/*
 * csp_dpu_clk_set_divider - set clk divider
 * @div: divider, [1, 16]
 */
s32 csp_dpu_clk_set_divider(u32 id, u32 div);
s32 csp_dpu_clk_get_divider(u32 id, u32 *div);

/* csp_dpu_clk_enable - enable sub module clk gating */
s32 csp_dpu_clk_enable(u32 id);
s32 csp_dpu_clk_disable(u32 id);

s32 csp_dpu_adpll_enable(u32 adpll_id, struct dpu_adpll_config *cfg);
s32 csp_dpu_adpll_disable(u32 adpll_id);
u32 csp_dpu_adpll_get_status(u32 adpll_id);
s32 csp_dpu_set_sclk_parent(u32 sclk_id, u32 parent);

#endif /* __CSP_DPU_TOP_H__ */

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

#ifndef __CSP_DPU_TOP_C__
#define __CSP_DPU_TOP_C__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/io.h>

#include "csp_dpu_top.h"
#include "csp_dpu_reg.h"

#define WRITEREG32(reg, val)   writel(val, (void __iomem *)reg)
#define READREG32(reg)         readl((void __iomem *)reg)

static reg_dpu_t *dpu = (reg_dpu_t *)BASE_DPU;

/* only for test, change it to cpu-related address */
s32 csp_dpu_set_register_base(void *base)
{
	dpu = (reg_dpu_t *)base;
	return 0;
}

/* Get register base address */
s32 csp_dpu_get_register_base(unsigned long *addr)
{
	*addr = (unsigned long)dpu;
	return 0;
}

static void (*__dpu_udelay)(unsigned long);

static void dpu_udelay(unsigned long us)
{
	if (__dpu_udelay)
		__dpu_udelay(us);
	else {
		/* Assume cpu runs at 1Ghz */
		unsigned long i = 1000 * us;
		while (i--)
			;
	}
}

/**
 *csp_dpu_set_udelay_func - set udelay func
 */
s32 csp_dpu_set_udelay_func(void (*udelay)(unsigned long))
{
	__dpu_udelay = udelay;

	return 0;
}

/* csp_dpu_clk_assert - reset the specified clk */
s32 csp_dpu_clk_assert(u32 id)
{
	s32 ret = 0;
	reg_dpu_ctl_t reg;
	u32 *reg_addr;

	reg_addr = (unsigned int *)&dpu->ctl;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		reg.bits.dc0_rst = 1;
		break;
	case DPU_MOD_DC1:
		reg.bits.dc1_rst = 1;
		break;
	case DPU_MOD_SE0:
		reg.bits.se0_rst = 1;
		break;
	case DPU_MOD_SE1:
		reg.bits.se1_rst = 1;
		break;
	case DPU_MOD_ROT:
		reg.bits.rot_rst = 1;
		break;
	case DPU_MOD_DIT:
		reg.bits.dit_rst = 1;
		break;
	case DPU_MOD_WB:
		reg.bits.wb_rst = 1;
		break;
	case DPU_MOD_TOP:
		reg.bits.top_rst = 1;
		break;
	case DPU_MOD_TIMER:
		/* tck not need assert at present */
		return 0;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		goto exit;
	};

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* csp_dpu_clk_deassert - release the specified clk(normal work state) */
s32 csp_dpu_clk_deassert(u32 id)
{
	s32 ret = 0;
	reg_dpu_ctl_t reg;
	u32 *reg_addr;

	reg_addr = (unsigned int *)&dpu->ctl;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		reg.bits.dc0_rst = 0;
		break;
	case DPU_MOD_DC1:
		reg.bits.dc1_rst = 0;
		break;
	case DPU_MOD_SE0:
		reg.bits.se0_rst = 0;
		break;
	case DPU_MOD_SE1:
		reg.bits.se1_rst = 0;
		break;
	case DPU_MOD_ROT:
	#ifdef CONFIG_ARCH_LOMBO_N9V0
		ret = __LINE__;
		goto exit;
	#else
		reg.bits.rot_rst = 0;
		break;
	#endif
	case DPU_MOD_DIT:
		reg.bits.dit_rst = 0;
		break;
	case DPU_MOD_WB:
		reg.bits.wb_rst = 0;
		break;
	case DPU_MOD_TOP:
		reg.bits.top_rst = 0;
		break;
	case DPU_MOD_TIMER:
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		goto exit;
	};

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * csp_dpu_clk_set_parent - set parent
 * @id: module id
 * @parent: clk index of parent clk
 */
s32 csp_dpu_clk_set_parent(u32 id, u32 parent)
{
	s32 ret = 0;
	reg_dpu_clk_src_t reg;
	u32 *reg_addr;

	/* tck not need to set parent at present */
	if (id == DPU_MOD_TIMER)
		return 0;

	if (parent >= DPU_CLK_PARENT_MAX) {
		pr_err("%s()L%d, error parent %d\n",
		       __func__, __LINE__, parent);
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (u32 *)&dpu->clk_src;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		reg.bits.dc0_cs = parent;
		break;
	case DPU_MOD_DC1:
		reg.bits.dc1_cs = parent;
		break;
	case DPU_MOD_SE0:
		reg.bits.se0_cs = parent;
		break;
	case DPU_MOD_SE1:
		reg.bits.se1_cs = parent;
		break;
	case DPU_MOD_ROT:
		reg.bits.rot_cs = parent;
		break;
	case DPU_MOD_DIT:
		reg.bits.dit_cs = parent;
		break;
	case DPU_MOD_WB:
		reg.bits.wb_cs = parent;
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		goto exit;
	};

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

s32 csp_dpu_clk_get_parent(u32 id, u32 *parent)
{
	s32 ret = 0;
	reg_dpu_clk_src_t reg;
	u32 *reg_addr;

	if (parent == NULL) {
		pr_err("%s()L%d, parent is NULL\n", __func__, __LINE__);
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (u32 *)&dpu->clk_src;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		*parent = reg.bits.dc0_cs;
		break;
	case DPU_MOD_DC1:
		*parent = reg.bits.dc1_cs;
		break;
	case DPU_MOD_SE0:
		*parent = reg.bits.se0_cs;
		break;
	case DPU_MOD_SE1:
		*parent = reg.bits.se1_cs;
		break;
	case DPU_MOD_ROT:
		*parent = reg.bits.rot_cs;
		break;
	case DPU_MOD_DIT:
		*parent = reg.bits.dit_cs;
		break;
	case DPU_MOD_WB:
		*parent = reg.bits.wb_cs;
		break;
	case DPU_MOD_TIMER:
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		break;
	};

exit:
	return ret;
}

/*
 * csp_dpu_clk_set_divider - set clk divider
 * @div: divider, [1, 16]
 */
s32 csp_dpu_clk_set_divider(u32 id, u32 div)
{
	s32 ret = 0;
	reg_dpu_clk_div_t reg;
	u32 *reg_addr;

	if ((div == 0) || (div > DPU_CLK_DIV_MAX)) {
		pr_err("%s()L%d, error div %d\n", __func__, __LINE__, div);
		ret = __LINE__;
		goto exit;
	}

	div -= 1;
	reg_addr = (unsigned int *)&dpu->clk_div;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		reg.bits.dc0_div = div;
		break;
	case DPU_MOD_DC1:
		reg.bits.dc1_div = div;
		break;
	case DPU_MOD_SE0:
		reg.bits.se0_div = div;
		break;
	case DPU_MOD_SE1:
		reg.bits.se1_div = div;
		break;
	case DPU_MOD_ROT:
		reg.bits.rot_div = div;
		break;
	case DPU_MOD_DIT:
		reg.bits.dit_div = div;
		break;
	case DPU_MOD_WB:
		reg.bits.wb_div = div;
		break;
	case DPU_MOD_TIMER:
		reg.bits.tck_div = div;
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		goto exit;
	};

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

s32 csp_dpu_clk_get_divider(u32 id, u32 *div)
{
	s32 ret = 0;
	reg_dpu_clk_div_t reg;
	u32 *reg_addr;

	if (div == NULL) {
		pr_err("%s()L%d, div is NULL\n", __func__, __LINE__);
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dpu->clk_div;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		*div = reg.bits.dc0_div;
		break;
	case DPU_MOD_DC1:
		*div = reg.bits.dc1_div;
		break;
	case DPU_MOD_SE0:
		*div = reg.bits.se0_div;
		break;
	case DPU_MOD_SE1:
		*div = reg.bits.se1_div;
		break;
	case DPU_MOD_ROT:
		*div = reg.bits.rot_div;
		break;
	case DPU_MOD_DIT:
		*div = reg.bits.dit_div;
		break;
	case DPU_MOD_WB:
		*div = reg.bits.wb_div;
		break;
	case DPU_MOD_TIMER:
		*div = reg.bits.tck_div;
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		break;
	};
	*div = *div + 1;

exit:
	return ret;
}

/* csp_dpu_clk_enable - enable sub module clk gating */
s32 csp_dpu_clk_enable(u32 id)
{
	s32 ret = 0;
	reg_dpu_clk_gating_t reg;
	u32 *reg_addr;

	reg_addr = (u32 *)&dpu->clk_gating;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		reg.bits.dc0_gt = 1;
		break;
	case DPU_MOD_DC1:
		reg.bits.dc1_gt = 1;
		break;
	case DPU_MOD_SE0:
		reg.bits.se0_gt = 1;
		break;
	case DPU_MOD_SE1:
		reg.bits.se1_gt = 1;
		break;
	case DPU_MOD_ROT:
		reg.bits.rot_gt = 1;
		break;
	case DPU_MOD_DIT:
		reg.bits.dit_gt = 1;
		break;
	case DPU_MOD_WB:
		reg.bits.wb_gt = 1;
		break;
	case DPU_MOD_TIMER:
		reg.bits.tck_gt = 1;
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		goto exit;
	};

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

s32 csp_dpu_clk_disable(u32 id)
{
	s32 ret = 0;
	reg_dpu_clk_gating_t reg;
	u32 *reg_addr;

	reg_addr = (u32 *)&dpu->clk_gating;
	reg.val = READREG32(reg_addr);
	switch (id) {
	case DPU_MOD_DC0:
		reg.bits.dc0_gt = 0;
		break;
	case DPU_MOD_DC1:
		reg.bits.dc1_gt = 0;
		break;
	case DPU_MOD_SE0:
		reg.bits.se0_gt = 0;
		break;
	case DPU_MOD_SE1:
		reg.bits.se1_gt = 0;
		break;
	case DPU_MOD_ROT:
		reg.bits.rot_gt = 0;
		break;
	case DPU_MOD_DIT:
		reg.bits.dit_gt = 0;
		break;
	case DPU_MOD_WB:
		reg.bits.wb_gt = 0;
		break;
	case DPU_MOD_TIMER:
		reg.bits.tck_gt = 0;
		break;
	default:
		pr_err("%s()L%d, error id %d\n", __func__, __LINE__, id);
		ret = __LINE__;
		goto exit;
	};

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

s32 csp_dpu_adpll_enable(u32 adpll_id, struct dpu_adpll_config *cfg)
{
	s32 ret = 0;
	reg_dpu_adpll0_ctl_t adpll_ctl;
	u32 *reg_addr;
	u32 status_ok = DPU_ADPLL_STATUS_MASK_FLOCK
		      | DPU_ADPLL_STATUS_MASK_PLOCK;
	u32 status;
	u32 dly_us = 50;
	u32 dly_cnt = 100;

	if ((adpll_id >= DPU_ADPLL_NUM) || !cfg ||
		(cfg->n < DPU_SCLK_ADPLL_N_MIN ||
		cfg->n > DPU_SCLK_ADPLL_N_MAX)) {
		ret = __LINE__;
		pr_err("%s()L%d, cfg=%p, n=%d, adpll_id=%d\n",
			__func__, ret, cfg, cfg ? cfg->n : 0, adpll_id);
		goto exit;
	}

	reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
	adpll_ctl.val = READREG32(reg_addr);
	adpll_ctl.bits.n = cfg->n;
	adpll_ctl.bits.en = 0;
	WRITEREG32(reg_addr, adpll_ctl.val);

	/* tune can be config only when adpll is disable */
	reg_addr = (u32 *)DPU_REG_ADPLL_TUNE0(dpu, adpll_id);
	WRITEREG32(reg_addr, cfg->tune0);
	reg_addr = (u32 *)DPU_REG_ADPLL_TUNE1(dpu, adpll_id);
	WRITEREG32(reg_addr, cfg->tune1);

	reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
	adpll_ctl.val = READREG32(reg_addr);
	adpll_ctl.bits.n = cfg->n;
	adpll_ctl.bits.en = 1;
	WRITEREG32(reg_addr, adpll_ctl.val);

	do {
		status = csp_dpu_adpll_get_status(adpll_id);
		if ((status & status_ok) == status_ok) {
			ret = 0;
			break;
		}
		if (dly_cnt)
			dpu_udelay(dly_us);
		else {
			ret = __LINE__;
			break;
		}
		dly_cnt--;
	} while (1);

	if (ret != 0) {
		reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
		adpll_ctl.val = READREG32(reg_addr);
		adpll_ctl.bits.en = 0;
		WRITEREG32(reg_addr, adpll_ctl.val);

		reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
		adpll_ctl.val = READREG32(reg_addr);
		adpll_ctl.bits.n = 42;
		WRITEREG32(reg_addr, adpll_ctl.val);

		reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
		adpll_ctl.val = READREG32(reg_addr);
		adpll_ctl.bits.en = 1;
		WRITEREG32(reg_addr, adpll_ctl.val);

		dpu_udelay(10);

		reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
		adpll_ctl.val = READREG32(reg_addr);
		adpll_ctl.bits.n = cfg->n;
		WRITEREG32(reg_addr, adpll_ctl.val);

		dly_cnt = 100;
		do {
			status = csp_dpu_adpll_get_status(adpll_id);
			if ((status & status_ok) == status_ok) {
				ret = 0;
				break;
			}
			if (dly_cnt)
				dpu_udelay(dly_us);
			else {
				ret = __LINE__;
				break;
			}
			dly_cnt--;
		} while (1);
	}

exit:
	return ret;
}

s32 csp_dpu_adpll_disable(u32 adpll_id)
{
	s32 ret = 0;
	reg_dpu_adpll0_ctl_t adpll_ctl;
	u32 *reg_addr;

	if (adpll_id >= DPU_ADPLL_NUM) {
		ret = __LINE__;
		pr_err("%s()L%d, adpll_id=%d\n",
			__func__, ret, adpll_id);
		goto exit;
	}

	reg_addr = (u32 *)DPU_REG_ADPLL_CTL(dpu, adpll_id);
	adpll_ctl.val = READREG32(reg_addr);
	adpll_ctl.bits.en = 0;
	WRITEREG32(reg_addr, adpll_ctl.val);

exit:
	return ret;
}

u32 csp_dpu_adpll_get_status(u32 adpll_id)
{
	reg_dpu_adpll0_stat_t adpll_stat;
	u32 *reg_addr;

	if (adpll_id >= DPU_ADPLL_NUM) {
		pr_err("%s()L%d, adpll_id=%d\n",
			__func__, __LINE__, adpll_id);
		return 0;
	}

	reg_addr = (u32 *)DPU_REG_ADPLL_STATUS(dpu, adpll_id);
	adpll_stat.val = READREG32(reg_addr);

	return adpll_stat.val;
}

/* parent: DPU_SCLK_PARENT_EXT or DPU_SCLK_PARENT_ADPLL */
s32 csp_dpu_set_sclk_parent(u32 sclk_id, u32 parent)
{
	reg_dpu_sclk_sel_t sclk_sel;
	u32 *reg_addr;
	int ret = 0;

	reg_addr = (u32 *)&dpu->sclk_sel;
	sclk_sel.val = READREG32(reg_addr);
	if (sclk_id == 0)
		sclk_sel.bits.sclk0_sel = parent;
	else if (sclk_id == 1)
		sclk_sel.bits.sclk1_sel = parent;
	else {
		ret = __LINE__;
		goto exit;
	}
	WRITEREG32(reg_addr, sclk_sel.val);

exit:
	return ret;
}

#endif /* __CSP_DPU_TOP_C__ */


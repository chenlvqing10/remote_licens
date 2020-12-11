/*
 * for-audio.c - audio clocks operations
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

#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <mach/csp.h>

#include <linux/clk-private.h>
#include "clk.h"

#ifdef CONFIG_OF

#if 0 /* nfrac is set inside clock_pll_set_rate, same as n/prev_val/post_val */
void clk_pll_set_nfrac(struct clk *clk, int nfrac)
{
	struct clock_pll *pll = to_clock_pll(clk->hw);
	unsigned long flags = 0;

	spin_lock_irqsave(&pll->lock, flags);
	writel(nfrac, pll->reg[NFAC]);
	spin_unlock_irqrestore(&pll->lock, flags);
}
EXPORT_SYMBOL_GPL(clk_pll_set_nfrac);

int clk_pll_get_nfrac(struct clk *clk)
{
	struct clock_pll *pll = to_clock_pll(clk->hw);
	unsigned long flags = 0;
	int ret;

	spin_lock_irqsave(&pll->lock, flags);
	ret = readl(pll->reg[NFAC]);
	spin_unlock_irqrestore(&pll->lock, flags);
	return ret;
}
EXPORT_SYMBOL_GPL(clk_pll_get_nfrac);
#endif

int is_audio_pll_clk(const char *name)
{
	return !strcmp(AUDIO_PLL_DIVM, name);
}

/**
 * audio_pll_recalc_rate - get audio pll clk rate from hw
 * @hw: clk hw structure
 * @parent_rate: parent rate
 *
 * query the hw for pre_val, post_val, n, nfrac, and then query the
 * freq-div-table, get the matched rate
 *
 * return the matched rate if success, 0 if failed
 */
unsigned long audio_pll_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate)
{
	struct clock_pll *pll = to_clock_pll(hw);
	const char *name = __clk_get_name(hw->clk);
	int prev_div, post_div, n, nfrac;
	u32 reg_fac, reg_nfrac, i;
	unsigned long flags = 0;

	spin_lock_irqsave(&pll->lock, flags);
	reg_fac = readl(pll->reg[FACTOR]);
	reg_nfrac = readl(pll->reg[NFAC]);
	spin_unlock_irqrestore(&pll->lock, flags);

	n = FAC_N(reg_fac);
	prev_div = PREV_DIV(reg_fac);
	post_div = POST_DIV(reg_fac);
	nfrac = (int)(reg_nfrac & 0x3ffff); /* bit0~17 for nfrac */

	for (i = 0; i < ARRAY_SIZE(pll->fd_table); i++) {
		if (pll->fd_table[i].prev_val == prev_div - 1
				&& pll->fd_table[i].post_val == post_div - 1
				&& pll->fd_table[i].factor == n
				&& pll->fd_table[i].nfrac == nfrac)
			return pll->fd_table[i].rate;
	}

	PRT_ERR("cannot find matched rate for clock %s", name);
	PRT_ERR("prev_div %d, post_div %d, n %d, nfrac %d\n",
		prev_div, post_div, n, nfrac);
	return 0;
}

#endif /* CONFIG_OF */

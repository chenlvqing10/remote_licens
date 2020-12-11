/*
 * clk-pll.c - pll clocks registration code
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
#include <linux/clk-private.h>

#include "clk.h"

#ifdef CONFIG_OF

/* start bit index and width for mode field */
#define PLL_MODE_SHIFT		0
#define PLL_MODE_WIDTH		2

void clk_pll_set_mode(struct clk *clk, int mode)
{
	struct clk_hw *hw = clk->hw;
	struct clock_pll *pll = to_clock_pll(hw);
	unsigned long flags = 0;
	u32 val;

	BUG_ON(mode < 0 || mode >= PLL_MODE_MAX);

	spin_lock_irqsave(&pll->lock, flags);
	val = readl(pll->reg[MODE]);
	val &= ~(((1 << PLL_MODE_WIDTH) - 1) << PLL_MODE_SHIFT);
	val |= (mode << PLL_MODE_SHIFT);
	writel(val, pll->reg[MODE]);
	spin_unlock_irqrestore(&pll->lock, flags);
}
EXPORT_SYMBOL_GPL(clk_pll_set_mode);

int clk_pll_get_mode(struct clk *clk)
{
	struct clk_hw *hw = clk->hw;
	struct clock_pll *pll = to_clock_pll(hw);
	unsigned long flags = 0;
	int val, mode = 0;

	spin_lock_irqsave(&pll->lock, flags);
	val = readl(pll->reg[MODE]);
	mode = (val >> PLL_MODE_SHIFT) & ((1 << PLL_MODE_WIDTH) - 1);
	spin_unlock_irqrestore(&pll->lock, flags);

	BUG_ON(mode < 0 || mode >= PLL_MODE_MAX);
	return mode;
}
EXPORT_SYMBOL_GPL(clk_pll_get_mode);

int clock_pll_prepare(struct clk_hw *hw)
{
	struct clock_pll *pll = to_clock_pll(hw);
	u32 val;

	mutex_lock(&pll->prepare_lock);

	/* enable enm */
	val = readl(pll->reg[ENABLE]);
	val |= BIT(pll->en_shift[ENM]);
	writel(val, pll->reg[ENABLE]);
	/* delay 100us */
	usleep_range(100, 200);

	/* enable enp */
	val |= BIT(pll->en_shift[ENP]);
	writel(val, pll->reg[ENABLE]);
	/* delay 10us */
	usleep_range(10, 20);

	/* wait lock status set, usually <= 100us */
	val = 5; /* timout 250~500us */
	while (!(readl(pll->reg[STATUS]) & BIT(pll->en_shift[LOCK]))
			&& val--)
		usleep_range(50, 100);

	mutex_unlock(&pll->prepare_lock);

	if (!val) {
		PRT_ERR("clock %s prepare failed\n", __clk_get_name(hw->clk));
		return -EINVAL;
	}
	return 0;
}

void clock_pll_unprepare(struct clk_hw *hw)
{
	struct clock_pll *pll = to_clock_pll(hw);
	const char *name = __clk_get_name(hw->clk);
	u32 val;

	mutex_lock(&pll->prepare_lock);

	/* disable enp */
	val = readl(pll->reg[ENABLE]);
	val &= ~BIT(pll->en_shift[ENP]);
	writel(val, pll->reg[ENABLE]);
	/* delay 10us */
	usleep_range(10, 20);

	/* disable enm */
	val &= ~BIT(pll->en_shift[ENM]);
	writel(val, pll->reg[ENABLE]);
	/* delay 100us */
	usleep_range(100, 200);

	/* audio pll: disable DIV_ENDIV7 and DIV_ENDIV17 bit */
#ifndef CONFIG_ARCH_LOMBO_N9
	if (is_audio_pll_clk(name)) {
		val &= ~BIT(AUDIO_PLL_DIV7_BIT_IDX);
		val &= ~BIT(AUDIO_PLL_DIV17_BIT_IDX);
	}
	writel(val, pll->reg[ENABLE]);
#endif

	mutex_unlock(&pll->prepare_lock);
}

int clock_pll_enable(struct clk_hw *hw)
{
	struct clock_pll *pll = to_clock_pll(hw);
	unsigned long flags = 0;
	const char *name = __clk_get_name(hw->clk);
	u32 val;

	spin_lock_irqsave(&pll->lock, flags);

	/* enable oen */
	val = readl(pll->reg[ENABLE]);
	val |= BIT(pll->en_shift[OEN]);
	writel(val, pll->reg[ENABLE]);

	/* audio pll: enable DIV_ENDIV7 and DIV_ENDIV17 bit */
	if (is_audio_pll_clk(name)) {
		val |= BIT(AUDIO_PLL_DIV7_BIT_IDX);
		val |= BIT(AUDIO_PLL_DIV17_BIT_IDX);
	}
	writel(val, pll->reg[ENABLE]);

	spin_unlock_irqrestore(&pll->lock, flags);
	return 0;
}

void clock_pll_disable(struct clk_hw *hw)
{
	struct clock_pll *pll = to_clock_pll(hw);
	unsigned long flags = 0;
	u32 val;

	spin_lock_irqsave(&pll->lock, flags);

	/* disable oen */
	val = readl(pll->reg[ENABLE]);
	val &= ~BIT(pll->en_shift[OEN]);
	writel(val, pll->reg[ENABLE]);

	spin_unlock_irqrestore(&pll->lock, flags);
}

unsigned long clock_pll_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate)
{
	struct clock_pll *pll = to_clock_pll(hw);
	const char *name = __clk_get_name(hw->clk);
	u32 val, n, prev_div, post_div;
	unsigned long ret = 0;

	if (is_audio_pll_clk(name)) {
		PRT_DBG("clk(%s) is audio clk\n", name);
#if defined(CONFIG_ARCH_LOMBO_N7V1)
		return audio_clk_recalc_rate(name);
#elif defined(CONFIG_ARCH_LOMBO_N9V0)
		return audio_pll_recalc_rate(hw, parent_rate);
#endif
	}

	val = readl(pll->reg[FACTOR]);
	n = FAC_N(val);
	prev_div = PREV_DIV(val);
	post_div = POST_DIV(val);
	WARN_ON(!n);

	ret = parent_rate / prev_div;
	ret = ret * n / post_div;
	return ret;
}

struct pll_fac_dval_table *__get_rate_from_fd_table(struct clock_pll *pll,
			unsigned long rate, unsigned long parent_rate)
{
	unsigned long flags = 0;
	int i;

	spin_lock_irqsave(&pll->fd_lock, flags);

	for (i = 0; i < pll->fd_cnt; i++) {
		if (pll->fd_table[i].rate == rate
			&& pll->fd_table[i].parent_rate
				== parent_rate) {
			spin_unlock_irqrestore(&pll->fd_lock, flags);
			return &pll->fd_table[i];
		}
	}

	spin_unlock_irqrestore(&pll->fd_lock, flags);

	PRT_DBG("rate(%d) not in table, need append, clock %s\n",
		(int)rate, __clk_get_name(pll->hw.clk));
	return NULL;
}

void __dump_pll_fd_table(struct clock_pll *pll)
{
#ifdef CONFIG_DEBUG_KERNEL
	int i;

	pr_info("\n---- dump rate div table for pll %s ----\n",
			__clk_get_name(pll->hw.clk));

	pr_info("%-16s%-16s%-12s%-12s%-12s\n", "rate", "parent_rate",
			"factor", "prev_div", "post_div");

	for (i = 0; i < pll->fd_cnt; i++)
		pr_info("%-16d%-16d%-12d%-12d%-12d\n",
			(int)pll->fd_table[i].rate,
			(int)pll->fd_table[i].parent_rate,
			pll->fd_table[i].factor,
			(int)__get_div(pll->flags[0],
				pll->fd_table[i].prev_val, pll->table[0]),
			(int)__get_div(pll->flags[1],
				pll->fd_table[i].post_val, pll->table[1]));
#endif
}

void __append_pll_fd_table(struct clock_pll *pll, unsigned long rate,
			unsigned long parent_rate, int factor,
			int prev_val, int post_val)
{
	unsigned long flags = 0;
	int i;

	spin_lock_irqsave(&pll->fd_lock, flags);

	/* check if already exist */
	for (i = 0; i < pll->fd_cnt; i++) {
		if (pll->fd_table[i].rate == rate
			&& pll->fd_table[i].parent_rate
				== parent_rate) {
			/*
			 * this may occur: if rate input cannot exactly be
			 * divided by parent_rate, since user does not
			 * guarantee the exact division in most case
			 */
			/* WARN_ON(1); */
			goto end;
		}
	}

	if (pll->fd_cnt >= MAX_FD_CNT_PLL) {
		PRT_WARN("freq-div table full for clock %s, save to end\n",
			__clk_get_name(pll->hw.clk));
		pll->fd_table[MAX_FD_CNT_PLL - 1].rate = rate;
		pll->fd_table[MAX_FD_CNT_PLL - 1].parent_rate = parent_rate;
		pll->fd_table[MAX_FD_CNT_PLL - 1].factor = factor;
		pll->fd_table[MAX_FD_CNT_PLL - 1].prev_val = prev_val;
		pll->fd_table[MAX_FD_CNT_PLL - 1].post_val = post_val;
		__dump_pll_fd_table(pll);
		goto end;
	}

	/* append to table */
	pll->fd_table[pll->fd_cnt].rate = rate;
	pll->fd_table[pll->fd_cnt].parent_rate = parent_rate;
	pll->fd_table[pll->fd_cnt].factor = factor;
	pll->fd_table[pll->fd_cnt].prev_val = prev_val;
	pll->fd_table[pll->fd_cnt].post_val = post_val;
	pll->fd_cnt++;

end:
	spin_unlock_irqrestore(&pll->fd_lock, flags);
}

#define PRE_DIV_MAX	2
#define POST_DIV_MAX	4
#define FAC_N_MIN	11 /* factor N must > 10 */

/* get the max factor value, according to pll's recommended freq-div table */
int __get_fac_max(struct clk_hw *hw, int pre_div)
{
	const char *name = __clk_get_name(hw->clk);

	BUG_ON(!(pre_div > 0 && pre_div <= PRE_DIV_MAX));

	if (!strcmp(name, "sdram_pll") || !strcmp(name, "perh0_pll_vco"))
		return pre_div == 1 ? 73 :
			(pre_div == 2 ? 146 : (WARN_ON(1), 73));
	else if (!strcmp(name, "perh2_pll"))
		return pre_div == 1 ? 66 :
			(pre_div == 2 ? 132 : (WARN_ON(1), 66));
	else
		return pre_div == 1 ? 50 :
			(pre_div == 2 ? 100 : (WARN_ON(1), 50));
}

long clock_pll_round_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long *prate)
{
	struct clock_pll *pll = to_clock_pll(hw);
	const char *name = __clk_get_name(hw->clk);
	struct pll_fac_dval_table *pfdt = NULL;
	int i, j, k, prev_div, post_div, max_val[3], max_fac, itmp;
	unsigned long best_rate = 0, tmp;
	int best[3] = {1, 0, 0}; /* n,prev,post val correspond to best_rate */

	/* pll rate should be always >0 */
	if (!rate) {
		PRT_ERR("pll clk %s rate to set is 0\n", name);
		return -EINVAL;
	}

	/* 9006 audio_pll_divm is fraction clock  */
#ifndef CONFIG_ARCH_LOMBO_N9V0
	if (is_audio_pll_clk(name)) {
		PRT_ERR("audio clk(%s) cannot call clk_set_rate\n", name);
		return -EINVAL; /* abort clk_set_rate */
	}
#endif

	/* get the matched item in freq-div table */
	pfdt = __get_rate_from_fd_table(pll, rate, *prate);
	if (pfdt)
		return (long)rate;

	if ((__clk_get_flags(hw->clk) & CLK_SET_RATE_PARENT)) {
		PRT_ERR("CLK_SET_RATE_PARENT set for clock(%s)\n", name);
		return (long)rate;
	}

	/* get max prev_val */
	max_val[PREV] = BIT(pll->fac_width[PREV]) - 1;
	max_val[PREV] = min(max_val[PREV], PRE_DIV_MAX - 1);
	/* get max post_val */
	max_val[POST] = BIT(pll->fac_width[POST]) - 1;
	max_val[POST] = min(max_val[POST], POST_DIV_MAX - 1);
	/* get max factor */
	max_fac = BIT(pll->fac_width[FAC]) - 1;

	/* try all [prev_val, post_val, factor] case, to catch the best rate */
	for (i = 0; i <= max_val[PREV]; i++) {
		prev_div = __get_div(pll->flags[0], i, pll->table[0]);
		for (j = 0; j <= max_val[POST]; j++) {
			post_div = __get_div(pll->flags[1], j, pll->table[1]);
			/*
			 * correct the max factor, according to recommended val
			 */
			itmp = __get_fac_max(hw, prev_div);
			max_val[FAC] = min(max_fac, itmp);
			for (k = FAC_N_MIN; k <= max_val[FAC]; k++) {
				tmp = *prate / prev_div;
				tmp = tmp * k / post_div;
				if (tmp < rate) {
					if (best_rate < tmp) {
						best_rate = tmp;
						best[0] = k;
						best[1] = i;
						best[2] = j;
					}
				}  else if (tmp == rate) {
					best_rate = rate;
					best[0] = k;
					best[1] = i;
					best[2] = j;
					goto exit;
				} else {
					/* factor too big */
					break;
				}
			}
		}
	}

exit:
	/*
	 * rate to set too small, less than (24m/max_prev_div)x1/max_post_div,
	 * that is about 6MHz. so we let round_rate failed, which will
	 * abort clk_set_rate
	 */
	if (!best_rate) {
		PRT_ERR("pll clk %s rate to set(%d) too small\n",
			name, (int)rate);
		return -EINVAL;
	}

	__append_pll_fd_table(pll, best_rate, *prate,
		best[0], best[1], best[2]);
	return best_rate;
}

int clock_pll_set_rate(struct clk_hw *hw, unsigned long rate,
			    unsigned long parent_rate)
{
	struct clock_pll *pll = to_clock_pll(hw);
	const char *name = __clk_get_name(hw->clk);
	struct pll_fac_dval_table *pfdt = NULL;
	unsigned long flags = 0;
	u32 reg_val;

	/* 9006 audio_pll_divm is fraction clock  */
#ifndef CONFIG_ARCH_LOMBO_N9V0
	if (is_audio_pll_clk(name)) {
		PRT_ERR("audio clk(%s) should not run here\n", name);
		/*
		 * clk_set_rate will be aborted by clock_pll_round_rate,
		 * so it should not be running here. BTW, we return ok
		 */
		return 0;
	}
#endif

	pfdt = __get_rate_from_fd_table(pll, rate, parent_rate);
	if (!pfdt) {
		PRT_ERR("rate(%d) not in table, clock(%s)\n", (int)rate, name);
		return -EINVAL;
	}

	spin_lock_irqsave(&pll->lock, flags);

	reg_val = readl(pll->reg[FACTOR]);
	reg_val = SET_FAC_N(reg_val, pfdt->factor);
	reg_val = SET_PREV_DIV(reg_val, pfdt->prev_val);
	reg_val = SET_POST_DIV(reg_val, pfdt->post_val);
	writel(reg_val, pll->reg[FACTOR]);

#ifdef CONFIG_ARCH_LOMBO_N9V0
	/* set nfrac if it is valid */
	if (pfdt->nfrac)
		writel(pfdt->nfrac, pll->reg[NFAC]);
#endif

	spin_unlock_irqrestore(&pll->lock, flags);
	return 0;
}

const struct clk_ops clk_pll_ops = {
	.enable = clock_pll_enable,
	.disable = clock_pll_disable,
	.prepare = clock_pll_prepare,
	.unprepare = clock_pll_unprepare,
	.recalc_rate = clock_pll_recalc_rate,
	.round_rate = clock_pll_round_rate,
	.set_rate = clock_pll_set_rate,
};

/* read freq-div paras from dts files, and initial the freq-div table */
int __init_pll_fd_table(struct device_node *node,
				struct clock_pll *pll)
{
	int i, cnt, val, size_in_ints;

	/* struct pll_fac_dval_table's size in ints */
	size_in_ints = 5;
#ifdef CONFIG_ARCH_LOMBO_N9V0
	/* 9006 pll_fac_dval_table add nfrac attribute */
	size_in_ints = 6;
#endif

	cnt = of_property_count_u32_elems(node, "freq-table");
	if (cnt <= 0) { /* not exist, ok */
		PRT_DBG("no freq-table config, clock(%s)", node->name);
		return 0;
	}

	if ((cnt % size_in_ints) || (cnt / size_in_ints) > MAX_FD_CNT_PLL) {
		PRT_ERR("freq-table cnt(%d) err, clock(%s)", cnt, node->name);
		goto err;
	}

	if (of_property_read_u32_array(node, "freq-table",
			(u32 *)pll->fd_table, cnt)) {
		PRT_ERR("read freq-table err, clock(%s)", node->name);
		goto err;
	}

	/* convert from division value to orignal reg bits value */
	cnt /= size_in_ints; /* freq table element cnt */
	for (i = 0; i < cnt; i++) {
		/* calc val0 */
		val = __calc_val(pll->flags[0],	pll->fd_table[i].prev_val,
				pll->fac_width[0], pll->table[0], true);
		if (val < 0) {
			PRT_ERR("prev_div(%d) parse err, clock(%s)",
				pll->fd_table[i].prev_val, node->name);
			goto err;
		}
		pll->fd_table[i].prev_val = val;

		/* calc val1 */
		val = __calc_val(pll->flags[1],	pll->fd_table[i].post_val,
				pll->fac_width[1], pll->table[1], true);
		if (val < 0) {
			PRT_ERR("post_div(%d) parse err, clock(%s)",
				pll->fd_table[i].post_val, node->name);
			goto err;
		}
		pll->fd_table[i].post_val = val;
	}

	pll->fd_cnt = cnt;
	return 0; /* success */
err:
	memset(pll->fd_table, 0, sizeof(pll->fd_table));
	return -EINVAL;
}

/**
 * setup_pll_regs - set pll reg's vaddr
 * @clk_name: the pll clock's name
 * @pll_clk: the pll clock's struct
 *
 */
void setup_pll_regs(const char *clk_name, struct clock_pll *pclk)
{
	int i;

#if defined(CONFIG_ARCH_LOMBO_N9V0)
	if (!strcmp("cpu_pll", clk_name)) {
		pclk->reg[FACTOR] = pclk->reg[ENABLE] + 0x4;
		pclk->reg[TUNE0] = pclk->reg[ENABLE] + 0x8;
		pclk->reg[TEST] = pclk->reg[ENABLE] + 0xc;
		pclk->reg[STATUS] = pclk->reg[ENABLE] + 0x10;
		pclk->reg[MODE] = pclk->reg[ENABLE] + 0x14;
		pclk->reg[NFAC] = pclk->reg[ENABLE] + 0x18;
		pclk->reg[TUNE1] = pclk->reg[ENABLE] + 0x1c;
		pclk->reg[TUNE2] = pclk->reg[ENABLE] + 0x20;
		return;
	}
#else
	if (!strcmp("cpu_pll", clk_name) || !strcmp("ax_pll", clk_name)) {
		pclk->reg[FACTOR] = pclk->reg[ENABLE] + 0x8;
		pclk->reg[TUNE0] = pclk->reg[ENABLE] + 0xc;
		pclk->reg[TEST] = pclk->reg[ENABLE] + 0x14;
		pclk->reg[STATUS] = pclk->reg[ENABLE] + 0x18;
		pclk->reg[MODE] = pclk->reg[ENABLE] + 0x1c;
		pclk->reg[NFAC] = pclk->reg[ENABLE] + 0x20;
		pclk->reg[TUNE2] = pclk->reg[ENABLE] + 0x2c;
		return;
	}
#endif

	for (i = 1; i < pclk->reg_cnt; i++) /* omit ENABLE reg */
		pclk->reg[i] = pclk->reg[ENABLE] + (i << 2);
}

/**
 * of_pll_clk_setup - setup function for pll clock
 * @node: the matched device tree node in dts file
 *
 * xxx_pll: xxx_pll {			clock name
 *	compatible = "pll-clock";	identify string, should not be changed
 *	#clock-cells = <0>;		should be 0
 *	clocks = <&osc24m>;		all pll's parent is osc24m
 *	reg = <0x0400a460 0x24>;	reg addr and length
 *	    note: 1. reg list order: enable, factor, tune0, test,
 *			status, mode, N-factor, tune1, tune2.
 *		  2. the last regs(one or more) may be not exist according
 *			to spec, eg: if tune1 and tune2 reg not exist, length
 *			should be 0x1c
 *
 *	en-shift = <enp enm oen lock>;	bit index for pll enable operation
 *	    note: 1. bit index for cpu_enp, cpu_enm, cpu_oen and lock status
 *		  2. lock status is in status reg, the others are in enable reg
 *
 *	factor-shift = <prev post n>;	bit index for prediv postdiv and factor
 *	factor-width = <prev post n>;	bit width for prediv postdiv and factor
 *
 * the below configurations are optional:
 *	freq-table = <freq parent_freq n prev_div post_div
 *		      freq parent_freq n prev_div post_div ...>; freq div table
 *	    note: 1. n is the factor N. parent_freq is 24000000(osc24m)
 *		  2. prev_div/post_div is the division value calculated from
 *			the orignal reg bits value, but it's NOT the orignal
 *			bits value. eg. prev_div is 4 and orignal bits value
 *			is 3(3+1=4)
 *	div-table0 = <val div val div...>;	prediv table
 *	div-table1 = <val div val div...>;	postdiv table
 * };
 *
 * eg:
 * cpu_pll {
 *	compatible = "pll-clock";
 *	#clock-cells = <0>;
 *	clocks = <&osc24m>;
 *	reg = <0x0400a460 0x24>;
 *	en-shift = <0 1 2 0>;
 *	factor-shift = <0 4 8>;
 *	factor-width = <2 2 8>;
 *	// div-table0 = <0 1 1 2 2 4>; // prediv table absent, ok!
 *	div-table1 = <1 2 2 5 3 7 4 8>;
 *	freq-table = <480000000 24000000 1 1 20
 *		      320000000 24000000 1 2 40>;
 * };
 *
 */
void of_pll_clk_setup(struct device_node *node)
{
	const char *clk_name = node->name, *parent_name;
	struct clock_pll *pll = NULL;
	struct clk_init_data init;
	struct clk *clk;
	void __iomem *reg = NULL;
	struct resource res;

	pll = kzalloc(sizeof(struct clock_pll), GFP_KERNEL);
	WARN_ON(!pll);

	/* parse regs */
	if (of_address_to_resource(node, 0, &res)) {
		PRT_ERR("reg config invalid, clock %s\n", node->name);
		goto err;
	}
	reg = ioremap(res.start, resource_size(&res));
	pll->reg_cnt = resource_size(&res) >> 2;
	pll->reg[ENABLE] = reg;
	setup_pll_regs(clk_name, pll);

	/* parse en-shift, factor-shift, factor-width */
	if (of_property_read_u32_array(node, "en-shift", pll->en_shift, 4)) {
		PRT_ERR("read en-shift failed for clock %s\n", node->name);
		goto err;
	}
	if (of_property_read_u32_array(node, "factor-shift",
			pll->fac_shift, 3)) {
		PRT_ERR("read factor-shift failed for clock %s\n", node->name);
		goto err;
	}
	if (of_property_read_u32_array(node, "factor-width",
			pll->fac_width, 3)) {
		PRT_ERR("read factor-width failed for clock %s\n", node->name);
		goto err;
	}

	/* parse prediv and postdiv table */
	if (__init_divider_table(node, (unsigned long *)pll->table))
		goto err;

	/* parse pll freq-div table */
	if (__init_pll_fd_table(node, pll))
		goto err;

	mutex_init(&pll->prepare_lock);
	spin_lock_init(&pll->fd_lock); /* for fd_table */
	spin_lock_init(&pll->lock);

	/* register pll clock */
	init.name = clk_name;
	init.ops = &clk_pll_ops;
	init.flags = CLK_IS_BASIC;
	if (is_audio_pll_clk(clk_name) || !strcmp("sdram_pll", clk_name))
		init.flags |= CLK_GET_RATE_NOCACHE;
	parent_name = of_clk_get_parent_name(node, 0);
	init.parent_names = &parent_name;
	init.num_parents = 1;

	pll->hw.init = &init;
	clk = clk_register(NULL, &pll->hw);
	if (!IS_ERR(clk)) {
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
		WARN_ON(clk_register_clkdev(clk, clk_name, NULL));
	} else {
		PRT_ERR("failed to register pll clock %s\n", clk_name);
		goto err;
	}

	return;
err:
	if (pll) {
		/* if (pll->table[0]) remove checkpatch warning */
		kfree(pll->table[0]);
		/* if (pll->table[1]) */
		kfree(pll->table[1]);
		kfree(pll);
	}
}
EXPORT_SYMBOL_GPL(of_pll_clk_setup);
CLK_OF_DECLARE(pll_clk, "pll-clock", of_pll_clk_setup);

#endif


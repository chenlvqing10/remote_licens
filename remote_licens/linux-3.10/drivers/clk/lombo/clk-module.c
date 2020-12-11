/*
 * clk-module.c - module clocks registration code
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

#include "clk.h"

#ifdef CONFIG_OF

/**
 * __get_div - get the division from raw value(directly read from reg)
 * @flags:	the clock divider flags
 * @val:	raw value directly read from reg
 * @table:	the division-val table
 *
 * return >0 when success, otherwise 0
 */
unsigned int __get_div(u8 flags, unsigned int val,
			const struct clk_div_table *table)
{
	if (flags & CLK_DIVIDER_ONE_BASED)
		return val;
	if (flags & CLK_DIVIDER_POWER_OF_TWO)
		return 1 << val;
	if (table)
		return _get_table_div(table, val);
	return val + 1;
}

unsigned long clk_divider2_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct clk_divider2 *divider = to_clk_divider2(hw);
	unsigned int div[2], val[2];
	int i;

	for (i = 0; i < 2; i++) {
		val[i] = readl(divider->reg) >> divider->shift[i];
		val[i] &= (1 << divider->width[i]) - 1;
		div[i] = __get_div(divider->flags[i], val[i],
				divider->table[i]);
		if (!div[i]) {
			WARN(!(divider->flags[i] & CLK_DIVIDER_ALLOW_ZERO),
				"%s: Zero divisor and CLK_DIVIDER_ALLOW_ZERO "
				"not set\n", __clk_get_name(hw->clk));
			return parent_rate;
		}
	}

	parent_rate = DIV_ROUND_UP(parent_rate, div[0]);
	return DIV_ROUND_UP(parent_rate, div[1]);
}

/**
 * __calc_val - get the val(orignal reg bits value) corresponding to the div
 * @flags:	the clock divider flags
 * @div:	the div value
 * @div_width:	reg bits width of divider
 * @table:	the div-val table
 * @exact_match: true indicate the return val must exactly match div.
 *		    so if cannot find the match val, return -EEROR
 *		 if false, just return the minimal val which meet
 *			the condition divider(val) >= div
 *
 * return >=0 when success, otherwise -EEROR
 */
int __calc_val(u8 flags, int div, u32 div_width,
			const struct clk_div_table *table,
			bool exact_match)
{
	int val = -EINVAL;

	if (!div) {
		WARN(1, "div is 0, to check!\n");
		return 0; /* to make it work temporarily */
	}

	if (flags & CLK_DIVIDER_ONE_BASED)
		val = div;
	else if (flags & CLK_DIVIDER_POWER_OF_TWO) {
		val = __fls(div);
		if (exact_match) {
			if (!is_power_of_2(div))
				return -EINVAL; /* exact value not exist */
			goto end;
		}
		if (!is_power_of_2(div))
			val += 1; /* 2^val should >= div */
	} else if (table) {
		val = INT_MAX;
		while (table->div) {
			if (table->div == div) {
				val = table->val;
				goto end;
			}
			if (table->div > div && table->val < val)
				val = table->val;
			table++;
		}
		if (exact_match) /* cannot find the exact value */
			return -EINVAL;
	} else
		val = div - 1; /* default case */

end:
	return min(val, ((1 << div_width) - 1));
}

void __dump_fd_table(struct clk_divider2 *dvd)
{
#ifdef CONFIG_DEBUG_KERNEL
	int i;

	pr_info("\n---- dump rate div table for clock %s(parent %s) ----\n",
		__clk_get_name(dvd->hw.clk),
		__clk_get_name(clk_get_parent(dvd->hw.clk)));

	pr_info("%-16s%-16s%-8s%-8s\n", "rate", "parent_rate", "div0", "div1");

	for (i = 0; i < dvd->fd_cnt; i++)
		pr_info("%-16d%-16d%-8d%-8d\n",
			(int)dvd->fd_table[i].rate,
			(int)dvd->fd_table[i].parent_rate,
			(int)__get_div(dvd->flags[0],
				dvd->fd_table[i].val0, dvd->table[0]),
			(int)__get_div(dvd->flags[1],
				dvd->fd_table[i].val1, dvd->table[1]));
#endif
}

int __get_dval_from_fd_table(struct clk_divider2 *dvd, unsigned long rate,
				unsigned long parent_rate, int *val)
{
	unsigned long flags = 0;
	int i;

	spin_lock_irqsave(&dvd->fd_lock, flags);

	for (i = 0; i < dvd->fd_cnt; i++) {
		if (dvd->fd_table[i].rate == rate
			&& dvd->fd_table[i].parent_rate
				== parent_rate) {
			val[0] = dvd->fd_table[i].val0;
			val[1] = dvd->fd_table[i].val1;

			spin_unlock_irqrestore(&dvd->fd_lock, flags);
			return 0;
		}
	}

	spin_unlock_irqrestore(&dvd->fd_lock, flags);

	/*
	 * this may occur: if rate input cannot exactly be divided by
	 * parent_rate, since we only save the actual rate in table, not the
	 * input one
	 */
	PRT_ERR("clock %s cannot find (rate %d, parent %d) in table\n",
		__clk_get_name(dvd->hw.clk), (int)rate, (int)parent_rate);
	return -EINVAL;
}

void __append_fd_table(struct clk_divider2 *dvd, unsigned long rate,
				unsigned long parent_rate, int val0, int val1)
{
	unsigned long flags = 0;
	int i;

	spin_lock_irqsave(&dvd->fd_lock, flags);

	/* check if already exist */
	for (i = 0; i < dvd->fd_cnt; i++) {
		if (dvd->fd_table[i].rate == rate
			&& dvd->fd_table[i].parent_rate
				== parent_rate) {
			/*
			 * this may occur: if rate input cannot exactly be
			 * divided by parent_rate, since user does not guarantee
			 * the exact division in most case
			 */
			/* WARN_ON(1); */
			goto end;
		}
	}

	if (dvd->fd_cnt >= MAX_FD_CNT_MOD) {
		PRT_WARN("rate_dval_table full for clock %s, save to end\n",
			__clk_get_name(dvd->hw.clk));
		dvd->fd_table[MAX_FD_CNT_MOD - 1].rate = rate;
		dvd->fd_table[MAX_FD_CNT_MOD - 1].parent_rate = parent_rate;
		dvd->fd_table[MAX_FD_CNT_MOD - 1].val0 = val0;
		dvd->fd_table[MAX_FD_CNT_MOD - 1].val1 = val1;
		__dump_fd_table(dvd);
		goto end;
	}

	/* append to table */
	dvd->fd_table[dvd->fd_cnt].rate = rate;
	dvd->fd_table[dvd->fd_cnt].parent_rate = parent_rate;
	dvd->fd_table[dvd->fd_cnt].val0 = val0;
	dvd->fd_table[dvd->fd_cnt].val1 = val1;
	dvd->fd_cnt++;

end:
	spin_unlock_irqrestore(&dvd->fd_lock, flags);
}

/* get the best div0 and div1, and return the corresponding rate */
long clk_divider2_round_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long *prate)
{
	struct clk_divider2 *dvd = to_clk_divider2(hw);
	char *name = (char *)__clk_get_name(hw->clk);
	unsigned long tmp, best_rate = 0;
	int dest_val[2] = {0}; /* correspond to best_rate */
	int i, j, mval[2], div[2];

	/* clk src is null_clk, or clk src's factor is 0(fpga debug?) */
	if (!*prate) {
		PRT_WARN("clk %s, rate %d, *prate 0\n",	name, (int)rate);
		return 0;
	} else {
		if (!rate) { /* avoid div0 err below, so deal firstly */
			PRT_ERR("clk %s, rate is 0, *prate %d\n",
				name, (int)*prate);
			return -EINVAL;
		}
	}

	/* get the matched item in freq-div table */
	if (!__get_dval_from_fd_table(dvd, rate, *prate, mval)) {
		PRT_DBG("clock %s's rate %d, prate %d in fd_table\n",
			name, (int)rate, (int)*prate);
		return rate; /* rate already saved in table */
	}

	/* if cannot find matched item in table, calculate manually */
	if ((__clk_get_flags(hw->clk) & CLK_SET_RATE_PARENT)) {
		PRT_ERR("CLK_SET_RATE_PARENT set for clock(%s)\n", name);
		return rate;
	}

	/* calc the max val0 and val1 */
	tmp = DIV_ROUND_UP(*prate, rate);
	mval[0] = __calc_val(dvd->flags[0], tmp, dvd->width[0],
			dvd->table[0], false);
	mval[1] = __calc_val(dvd->flags[1], tmp, dvd->width[1],
			dvd->table[1], false);

	/* try all [val0, val1] case, to catch the best rate */
	for (i = 0; i <= mval[0]; i++) {
		div[0] = __get_div(dvd->flags[0], i, dvd->table[0]);
		if (!div[0]) /* i is invalid */
			continue;

		for (j = 0; j <= mval[1]; j++) {
			div[1] = __get_div(dvd->flags[1], j, dvd->table[1]);
			if (!div[1])
				continue;

			/* calc the rate for cur [val0, val1] */
			tmp = *prate;
			tmp = DIV_ROUND_UP(tmp, div[0]);
			tmp = DIV_ROUND_UP(tmp, div[1]);
			/*
			 * donot consider CLK_DIVIDER_ROUND_CLOSEST
			 * flag right now
			 */
			if (tmp > rate)
				continue;
			else {
				if (tmp == rate) {
					best_rate = tmp;
					dest_val[0] = i;
					dest_val[1] = j;
					goto exit;
				}
				if (tmp > best_rate) {
					best_rate = tmp;
					dest_val[0] = i;
					dest_val[1] = j;
				}
			}
		}
	}

exit:
	/*
	 * rate to set is too small(less than *prate/max_divs), so we let
	 * round_rate failed, which will abort clk_set_rate
	 */
	if (!best_rate) {
		PRT_ERR("clk %s rate to set(%d) too small, *prate %d\n",
			name, (int)rate, (int)*prate);
		return -EINVAL;
	}

	__append_fd_table(dvd, best_rate, *prate, dest_val[0], dest_val[1]);
	return best_rate;

}

int clk_divider2_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct clk_divider2 *divider = to_clk_divider2(hw);
	char *name = (char *)__clk_get_name(hw->clk);
	unsigned long flags = 0;
	int i, reg_val, val[2];

	if (!parent_rate && !rate) {
		/*
		 * the case that clk src is null_clk, or clk src's factor
		 * is 0(fpga debug?), is normal case. so return success
		 * here. this also void the below finding fd table operation
		 * which will be failed
		 */
		return 0;
	} else if (!parent_rate || !rate) {
		/*
		 * these cases will never occur:
		 * 1. !parent_rate && rate: round_rate will set rate to 0
		 * 2. parent_rate && !rate: round_rate will return -EINVAL,
		 *    which will abort clk_set_rate(will not run to here)
		 */
		PRT_ERR("clk %s parent_rate(%d) or rate(%d) is 0\n",
			name, (int)parent_rate, (int)rate);
		return -EINVAL;
	}

	if (__get_dval_from_fd_table(divider, rate, parent_rate, val)) {
		PRT_ERR("rate(%d) not in fd table, clock(%s)\n",
			(int)rate, name);
		return -EINVAL; /* to debug.. catch the case */
	}

	spin_lock_irqsave(&divider->lock, flags);

	reg_val = readl(divider->reg);
	for (i = 0; i < 2; i++) {
		reg_val &= ~(((1 << divider->width[i]) - 1) <<
				divider->shift[i]);
		reg_val |= val[i] << divider->shift[i];
	}
	writel(reg_val, divider->reg);

	spin_unlock_irqrestore(&divider->lock, flags);
	return 0;
}

const struct clk_ops clk_divider2_ops = {
	.recalc_rate = clk_divider2_recalc_rate,
	.round_rate = clk_divider2_round_rate,
	.set_rate = clk_divider2_set_rate,
};

/* parse clock gate */
struct clk_gate *setup_clk_gate(struct device_node *node, void __iomem *reg)
{
	struct clk_gate *gate = NULL;
	int tmp;

	if (!of_property_read_u32(node, "enable-shift", (u32 *)&tmp)) {
		gate = kzalloc(sizeof(struct clk_gate), GFP_KERNEL);
		WARN_ON(!gate);
		gate->bit_idx = (u8)tmp;
		gate->reg = reg;
		/* spin_lock_init(&gate->lock); compile err, tbd */
	} else {
		PRT_DBG("read enable-shift failed for clock %s\n", node->name);
	}

	return gate;
}

/* parse clock parents */
struct clk_mux *setup_clk_mux(struct device_node *node, void __iomem *reg,
			int *num_parents, unsigned long *pparent_names)
{
	struct clk_mux *mux = NULL;
	char **pnames = NULL;
	int i, cnt, tmp;

	cnt = of_clk_get_parent_count(node);
	if (cnt > 0) {
		if (of_property_read_u32(node, "clk-src-shift", &tmp)) {
			PRT_ERR("clk-src-shift invalid, clk %s\n", node->name);
			return NULL;
		}

		mux = kzalloc(sizeof(struct clk_mux), GFP_KERNEL);
		WARN_ON(!mux);
		mux->shift = (u8)tmp;
		mux->mask = BIT(fls(cnt)) - 1;
		mux->reg = reg;
		/* spin_lock_init(&mux->lock); compile err, tbd */

		pnames = kzalloc(sizeof(char *) * cnt, GFP_KERNEL);
		WARN_ON(!pnames);
		for (i = 0; i < cnt; i++)
			pnames[i] = (char *)of_clk_get_parent_name(node, i);

		*num_parents = cnt;
		*pparent_names = (unsigned long)pnames;
		return mux;
	} else {
		PRT_ERR("parent clock config err for clock %s\n", node->name);
		return NULL;
	}
}

/* parse clock with single diviver */
struct clk_divider *setup_clk_divider(struct device_node *node,
					void __iomem *reg)
{
	struct clk_div_table *table = NULL;
	struct clk_divider *div = NULL;
	int tmp;

	div = kzalloc(sizeof(struct clk_divider), GFP_KERNEL);
	WARN_ON(!div);

	div->reg = reg;

	if (of_property_read_u32(node, "div-shift", &tmp)) {
		PRT_ERR("read div-shift failed for clock %s\n", node->name);
		goto err;
	}
	div->shift = (u8)tmp;

	if (of_property_read_u32(node, "div-width", &tmp)) {
		PRT_ERR("read div-width failed for clock %s\n", node->name);
		goto err;
	}
	div->width = (u8)tmp;

	tmp = of_property_count_u32_elems(node, "div-table");
	if (tmp > 0) {
		/* must be even */
		if (tmp % 2) {
			PRT_ERR("div-table config err for clock %s\n",
				node->name);
			goto err;
		}

		/* read config to div table */
		tmp >>= 1; /* table's element count */
		table = kzalloc((tmp + 1) *
				sizeof(struct clk_div_table),
				GFP_KERNEL);
		WARN_ON(!table);
		if (of_property_read_u32_array(node, "div-table",
				(u32 *)table, tmp << 1)) {
			PRT_ERR("read div-table err for clk %s\n", node->name);
			goto err;
		}

		div->table = (const struct clk_div_table *)table;
	}

	/* spin_lock_init(&div->lock); compile err, tbd */
	return div;
err:
	if (div) {
		/* if (div->table) remove checkpatch warning */
		kfree(div->table);
		kfree(div);
	}
	return NULL;
}

/* read freq-div paras from dts files, and initial the freq-div table */
int __init_fd_table(struct device_node *node, struct clk_divider2 *dvd)
{
	int i, cnt, val;

	cnt = of_property_count_u32_elems(node, "freq-div-table");
	if (cnt <= 0) /* not exist, ok */
		return 0;

	if ((cnt % 4) || (cnt >> 2) > MAX_FD_CNT_MOD) {
		PRT_ERR("freq-div-table cnt(%d) err, clock(%s)",
			cnt, node->name);
		goto err;
	}
	if (of_property_read_u32_array(node, "freq-div-table",
			(u32 *)dvd->fd_table, cnt)) {
		PRT_ERR("read freq-div-table err, clock(%s)", node->name);
		goto err;
	}

	/* convert from division value to orignal reg bits value */
	cnt >>= 2; /* element cnt */
	for (i = 0; i < cnt; i++) {
		/* calc val0 */
		val = __calc_val(dvd->flags[0],	dvd->fd_table[i].val0,
				dvd->width[0], dvd->table[0], true);
		if (val < 0) {
			PRT_ERR("clock(%s) div0(%d) invalid\n",
				node->name, dvd->fd_table[i].val0);
			goto err;
		}
		dvd->fd_table[i].val0 = val;

		/* calc val1 */
		val = __calc_val(dvd->flags[1],	dvd->fd_table[i].val1,
				dvd->width[1], dvd->table[1], true);
		if (val < 0) {
			PRT_ERR("clock(%s) div1(%d) invalid\n",
				node->name, dvd->fd_table[i].val1);
			goto err;
		}
		dvd->fd_table[i].val1 = val;
	}

	dvd->fd_cnt = cnt;
	return 0; /* success */
err:
	memset(dvd->fd_table, 0, sizeof(dvd->fd_table));
	return -EINVAL;
}

/* read div table paras from dts files, and initial the div table */
int __init_divider_table(struct device_node *node, unsigned long *table_out)
{
	struct clk_div_table *table[2] = {NULL};
	char strbuf[32];
	int i, cnt;

	for (i = 0; i < 2; i++) {
		sprintf(strbuf, "div-table%d", i);
		cnt = of_property_count_u32_elems(node, strbuf);
		if (cnt <= 0)
			continue;
		if (cnt % 2) { /* must be even */
			PRT_ERR("clock %s div-table%d count %d\n",
				node->name, i, cnt);
			goto err;
		}

		/* alloc and initialize div table */
		table[i] = kzalloc(((cnt >> 1) + 1) *
				sizeof(struct clk_div_table), GFP_KERNEL);
		WARN_ON(!table[i]);
		if (of_property_read_u32_array(node, strbuf,
				(u32 *)table[i], cnt)) {
			PRT_ERR("read %s failed, clock %s\n",
				strbuf, node->name);
			goto err;
		}

		table_out[i] = (unsigned long)table[i];
	}

	return 0;
err:
	/* if (table[0]) remove checkpatch warning */
	kfree(table[0]);
	kfree(table[1]);
	return -EINVAL;
}

/* parse clock with double divivers: div0 and div1 */
struct clk_divider2 *setup_clk_divider2(struct device_node *node,
					void __iomem *reg)
{
	struct clk_divider2 *div2 = NULL;

	div2 = kzalloc(sizeof(struct clk_divider2), GFP_KERNEL);
	WARN_ON(!div2);

	div2->reg = reg;

	if (of_property_read_u32_array(node, "div-shift", div2->shift, 2)) {
		PRT_ERR("read div-shift failed for clock %s\n", node->name);
		goto err;
	}
	if (of_property_read_u32_array(node, "div-width", div2->width, 2)) {
		PRT_ERR("read div-width failed for clock %s\n", node->name);
		goto err;
	}

	/* parse div table */
	if (__init_divider_table(node, (unsigned long *)div2->table))
		goto err;

	/* parse freq-div table */
	if (__init_fd_table(node, div2))
		goto err;

	spin_lock_init(&div2->fd_lock); /* for fd_table */
	spin_lock_init(&div2->lock);
	return div2;
err:
	if (div2) {
		/* if (div2->table[0]) remove checkpatch warning */
		kfree(div2->table[0]);
		kfree(div2->table[1]);
		kfree(div2);
	}
	return NULL;
}

/**
 * of_module_clk_setup - setup function for module clock
 * @node: the matched device tree node in dts file
 *
 * xxx_clk: xxx_clk {			clock name
 *	compatible = "module-clock";	identify string, should not be changed
 *	reg = <0x0400a900 0x4>;		the register addr and len
 *	#clock-cells = <0>;		should be 0
 *	enable-shift = <xxx>;		optional, enable bit's offset
 *	clk-src-shift = <0>;		the src clock select bit's offset
 *	clocks = <&src_clk1>, <&src_clk2>;	parent clocks list
 *
 * the below configurations are specific for different modules:
 *   for clocks with only div0:
 *	div-shift = <div0_shift>;	bit offset of div0
 *	div-width = <div0_width>;	bit width of div0
 *	div-table = <val div val div...>;	optional, div0 table
 *	  note:	1. if div-table not exist, the division value default
 *			is (div_val+1), while div_val is read from reg
 *
 *   for clocks with div0 and div1:
 *	div-shift = <div0_shift div1_shift>;	bit offset of div0 and div1
 *	div-width = <div0_width div1_width>;	bit width of div0 and div1
 *	div-table0 = <val div val div...>;	optional, div0 table
 *	div-table1 = <val div val div...>;	optional, div1 table
 *	  note:	1. if div-table not exist, the division value default
 *			is (div_val+1), while div_val is read from reg
 *	freq-div-table = <rate parent_rate div0 div1
 *		          rate parent_rate div0 div1 ...>;   optional
 *	  note:	1. freq-div-table is optional
 *		2. div0/div1 is the division value calculated from the orignal
 *			reg bits value, but it's NOT the orignal bits value
 *			eg. div0 is 8 and the orignal bits value is 3 (2^3=8)
 * };
 *
 * for example:
 * sdc0_clk {
 *	compatible = "module-clock";
 *	reg = <0x0400a960 0x4>;
 *	#clock-cells = <0>;
 *	enable-shift = <31>;
 *	clk-src-shift = <0>;
 *	clocks = <&null_clk>, <&osc24m>,
 *		<&perh0_pll_div2>, <&perh1_pll>;
 *	div-shift = <4 8>;
 *	div-width = <2 3>;
 *	// div-table0 = <0 1 1 2 2 4>; // div-table0 absent, ok!
 *	div-table1 = <1 2 2 4 3 8 4 7>;
 *	freq-div-table = <12000000 24000000 2 1
 *			  33000000 1188000000 3 12>;
 * };
 *
 * vc_clk {
 *	compatible = "module-clock";
 *	reg = <0x0400a9c0 0x4>;
 *	#clock-cells = <0>;
 *	enable-shift = <31>;
 *	clk-src-shift = <0>;
 *	clocks = <&null_clk>, <&vc_pll>;
 *	div-shift = <4>;
 *	div-width = <2>;
 *	div-table = <0 1 1 2 2 4>;
 * };
 */
void of_module_clk_setup(struct device_node *node)
{
	const char *clk_name = node->name;
	struct clk_gate *gate = NULL;
	struct clk_mux *mux = NULL;
	struct clk_divider *div = NULL;
	struct clk_divider2 *div2 = NULL;
	unsigned long parent_names = 0; /* initial to NULL */
	unsigned long flags = 0;
	struct clk *clk;
	int cnt, num_parents;
	void __iomem *reg;

	reg = of_iomap(node, 0);
	WARN_ON(!reg);

	/* parse clock gate */
	gate = setup_clk_gate(node, reg); /* gate maybe NULL */

	/* parse clock mux */
	mux = setup_clk_mux(node, reg, &num_parents, &parent_names);
	if (!mux)
		goto err;

	/* parse clock divider */
	cnt = of_property_count_u32_elems(node, "div-shift");
	if (cnt == 1) { /* module clock with single div */
		div = setup_clk_divider(node, reg);
		if (!div)
			goto err;
	} else if (cnt == 2) { /* module clock with div0 and div1 */
		div2 = setup_clk_divider2(node, reg);
		if (!div2)
			goto err;
	} else {
		PRT_WARN("div-shift invalid for clock %s\n", clk_name);
		/* goto err; */
	}

	/* prevent the clk disabled by clk_disable_unused */
	if (!strcmp("sdram_clk", clk_name) || !strcmp("mem_axi_clk", clk_name)
			|| !strcmp("uart0_clk", clk_name))
		flags = CLK_IGNORE_UNUSED;

	if (!strcmp("sdram_clk", clk_name) || !strcmp("sdram_bandw_clk", clk_name))
		flags |= CLK_GET_RATE_NOCACHE;

	clk = clk_register_composite(NULL, clk_name,
		(const char **)parent_names, num_parents,
		(mux ? &mux->hw : NULL), (mux ? &clk_mux_ops : NULL),
		(div ? &div->hw : (div2 ? &div2->hw : NULL)),
		(div ? &clk_divider_ops : (div2 ? &clk_divider2_ops : NULL)),
		(gate ? &gate->hw : NULL), (gate ? &clk_gate_ops : NULL),
		flags);
	if (!IS_ERR(clk)) {
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
		WARN_ON(clk_register_clkdev(clk, clk_name, NULL));
	} else {
		PRT_ERR("register err for clock %s\n", clk_name);
		goto err;
	}

	/* success */
	return;
err:
	if (parent_names)
		kfree((void *)parent_names);
	/* if (mux) remove checkpatch warning */
	kfree(mux);
	/* if (gate) */
	kfree(gate);
	if (div) {
		/* if (div->table) */
		kfree(div->table);
		kfree(div);
	}
	if (div2) {
		/* if (div2->table[0]) */
		kfree(div2->table[0]);
		/* if (div2->table[1]) */
		kfree(div2->table[1]);
		kfree(div2);
	}
}
EXPORT_SYMBOL_GPL(of_module_clk_setup);
CLK_OF_DECLARE(module_clk, "module-clock", of_module_clk_setup);

#endif


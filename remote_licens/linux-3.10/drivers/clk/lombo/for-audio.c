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
#include "clk.h"

#ifdef CONFIG_OF

/*
 * audio type clocks: audio pll relevant clocks, include audio_pll_div0,
 *      audio_pll_div7/17, audio_pll_divm, i2s_clk...
 *
 * for audio type clocks:
 *  1. only "module-clock" can actually update the clock rate
 *  2. the only way to get rate is query the freq-div-table
 *  3. everytime get clock rate(clk_get_rate), should query the hardware,
 *       then query the freq-div-table
 *
 * so these are realized:
 *   (1) clk_ops.set_rate will be bypassed in no "module-clock", such as
 *          audio_pll_div0, audio_pll_div7/17, audio_pll_divm...
 *   (2) clk_ops.set_rate is only valid in "module-clock", such as i2s_clk
 *   (3) clk_ops.round_rate will be bypassed
 *   (4) clk_ops.recalc_rate will everytime query the hardware and then query
 *          the freq-div-table
 *   (5) should set CLK_GET_RATE_NOCACHE when register
 */

int is_audio_pll_clk(const char *name)
{
	return !strcmp(AUDIO_PLL_NAME, name);
}

int is_audio_fix_divider_clk(const char *name)
{
	return !strcmp(AUDIO_PLL_DIV7, name) ||
		!strcmp(AUDIO_PLL_DIV17, name);
}

int is_audio_divider_clk(const char *name)
{
	return !strcmp(AUDIO_PLL_DIVM, name);
}

int is_audio_module_clk(const char *name)
{
#if defined(CONFIG_ARCH_LOMBO_N7V0)
	return !strcmp(AUDIO_CLK_I2S, name);
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
	return !strcmp(AUDIO_CLK_I2S0, name)
		|| !strcmp(AUDIO_CLK_I2S1, name);
#endif
}

enum {
	RATE_MOD = 0,	/* mod clock(i2s_clk) clock rate index */
	RATE_DIV0,	/* audio_pll_div0 clock rate index */
	RATE_DIV7,	/* audio_pll_div7 clock rate index */
	RATE_DIV17,	/* audio_pll_div17 clock rate index */
	RATE_DIVM,	/* audio_pll_divm clock rate index */
	RATE_CNT	/* clock rate count in table */
};

struct audio_fd_table {
	int prev_div;		/* pll PREV_DIV bit field value */
	int post_div;		/* pll POST_DIV bit field value */
	int fac_n;		/* pll N(factor) bit field value */
	int mod_div;		/* mod DIV bit field value */
	int mod_clksrc;		/* mod SRC_SEL(clksrc sel) bit field value */
	unsigned long rate[RATE_CNT];	/* clock rate table */
};

/*
 * audio clock div-freq-table
 */
enum {
	I2S_CLKSRC_NULL,
	I2S_CLKSRC_DIVM,
	I2S_CLKSRC_DIV7,
	I2S_CLKSRC_DIV17,
};
enum {
	CLK_24571M	= 24571000,
	CLK_24571M_X2	= 49142000,
	CLK_24571M_X3	= 73713000,
	CLK_24571M_X6	= 147426000,
	CLK_225882M	= 22588200,
	CLK_225882M_X2	= 45176400,
	CLK_225882M_X3	= 67764600,
};
const struct audio_fd_table fd_table[] = {
	/*
	 * the table item data arranged as below:
	 * {pll_prev_div, pll_post_div, pll_fac_n, mod_div, mod_clksrc,
	 *	mod_rate, rate_div0, rate_div7, rate_div17, rate_divm},
	 */
	{1, 2, 43, 2, I2S_CLKSRC_DIV7,
		{CLK_24571M, 516000000, CLK_24571M_X3, 0, 172000000} },
	{0, 2, 43, 2, I2S_CLKSRC_DIV7,
		{CLK_24571M_X2, 1032000000, CLK_24571M_X6, 0, 344000000} },
	{1, 0, 43, 0, I2S_CLKSRC_DIV7,
		{CLK_24571M_X3, 516000000, CLK_24571M_X3, 0, 516000000} },
	{1, 0, 32, 0, I2S_CLKSRC_DIV17,
		{CLK_225882M, 384000000, 0, CLK_225882M, 384000000} },
	{0, 0, 32, 0, I2S_CLKSRC_DIV17,
		{CLK_225882M_X2, 768000000, 0, CLK_225882M_X2, 768000000} },
	{1, 2, 96, 0, I2S_CLKSRC_DIV17,
		{CLK_225882M_X3, 1152000000, 0, CLK_225882M_X3, 384000000} },
	/* this kill err log when booting, to be removed in release version */
	{1, 2, 43, 0, I2S_CLKSRC_NULL,
		{0, 0, 0, 0, 0} },
};

/**
 * clk_name_to_rate_index - get clock's rate index in audio_fd_table.rate[]
 * @name: clock name
 *
 * return the index if success, -EINVAL if failed
 */
int clk_name_to_rate_index(const char *name)
{
#if defined(CONFIG_ARCH_LOMBO_N7V0)
	if (!strcmp(AUDIO_CLK_I2S, name))
		return RATE_MOD;
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
	if (!strcmp(AUDIO_CLK_I2S0, name) || !strcmp(AUDIO_CLK_I2S1, name))
		return RATE_MOD;
#endif
	if (!strcmp(AUDIO_PLL_DIV7, name))
		return RATE_DIV7;
	else if (!strcmp(AUDIO_PLL_DIV17, name))
		return RATE_DIV17;
	else if (!strcmp(AUDIO_PLL_DIVM, name))
		return RATE_DIVM;
	else if (!strcmp(AUDIO_PLL_NAME, name))
		return RATE_DIV0;
	else
		WARN(1, "clock name %s\n", name);

	return -EINVAL;
}

/**
 * fd_table_match_for_recalc - find the item in fd_table, whose div
 *     and factor matched with para. for recalc rate
 * @apf: audio pll factor reg value
 * @icc: i2s clock ctrl reg value
 *
 * return the matched item if success, NULL if failed
 */
struct audio_fd_table *
fd_table_match_for_recalc(reg_prcm_audio_pll_fac_t *apf, void *picc)
{
#if defined(CONFIG_ARCH_LOMBO_N7V0)
	reg_prcm_i2s_clk_ctrl_t *icc = picc;
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
	reg_prcm_i2s0_clk_ctrl_t *icc = picc;
#endif

#ifndef CONFIG_ARCH_LOMBO_N9
	int i;

	for (i = 0; i < ARRAY_SIZE(fd_table); i++) {
		if (apf->bits.pre_div == fd_table[i].prev_div
				&& apf->bits.post_div == fd_table[i].post_div
				&& apf->bits.n == fd_table[i].fac_n) {
			if (NULL == icc)
				return (struct audio_fd_table *)&fd_table[i];
			else if (icc->bits.div == fd_table[i].mod_div
				&& icc->bits.src_sel == fd_table[i].mod_clksrc)
				return (struct audio_fd_table *)&fd_table[i];
		}
	}
#endif /* CONFIG_ARCH_LOMBO_N9 */

	return NULL;
}

/**
 * audio_clk_recalc_rate - get clock rate from hw
 * @name: clk name
 *
 * query the hw for div and factor, and then query the freq-div-table,
 * get the matched rate
 *
 * return the matched rate if success, 0 if failed
 */
unsigned long audio_clk_recalc_rate(const char *name)
{

#ifndef CONFIG_ARCH_LOMBO_N9
	struct audio_fd_table *pft = NULL;
	reg_prcm_audio_pll_fac_t apf;

	apf.val = READREG32(VA_PRCM_AUDIO_PLL_FAC);

	if (!is_audio_module_clk(name)) /* eg: audio_clk_div7/17/.. */
		pft = fd_table_match_for_recalc(&apf, NULL);
	else { /* for audio module clk, eg: i2s clk.. */
	#if defined(CONFIG_ARCH_LOMBO_N7V0)
		reg_prcm_i2s_clk_ctrl_t icc;
		icc.val = READREG32(VA_PRCM_I2S_CLK_CTRL);
	#elif defined(CONFIG_ARCH_LOMBO_N7V1)
		reg_prcm_i2s0_clk_ctrl_t icc;

		if (!strcmp(AUDIO_CLK_I2S0, name))
			icc.val = READREG32(VA_PRCM_I2S0_CLK_CTRL);
		else if (!strcmp(AUDIO_CLK_I2S1, name))
			icc.val = READREG32(VA_PRCM_I2S1_CLK_CTRL);
		else
			WARN_ON(1);
	#endif

		/* get matched item in fd_table */
		pft = fd_table_match_for_recalc(&apf, &icc);
	}

	if (pft) {
		int index = clk_name_to_rate_index(name);
		if (index >= 0 && index < RATE_CNT)
			return pft->rate[index];
		else
			WARN_ON(1);
	}
#endif /* CONFIG_ARCH_LOMBO_N9 */

	/*
	 * maybe called by clk_set_parent->__clk_recalc_rates, which
	 * is ok(followed by clk_set_rate..). so just warn here.
	 */
	PRT_WARN("cannot find matched rate for audio clk %s\n", name);
	return 0;
}

/**
 * fd_table_match_for_set - find the item in fd_table, whose rate
 *     matched with para. for set rate
 * @rate: clock rate
 * @index: index in audio_fd_table.rate[]
 *
 * return the matched item if success, NULL if failed
 */
struct audio_fd_table *
fd_table_match_for_set(unsigned long rate, int index)
{
	int i;

	WARN_ON(index < 0 || index >= RATE_CNT);

	for (i = 0; i < ARRAY_SIZE(fd_table); i++) {
		if (fd_table[i].rate[index] == rate)
			return (struct audio_fd_table *)&fd_table[i];
	}

	return NULL;
}

/**
 * set_rate_hw - set audio clock's div and factor in reg
 * @ptbl: the fd_table item, which contain the value to be set
 */
void set_rate_hw(struct audio_fd_table *ptbl, const char *name)
{
	reg_prcm_audio_pll_fac_t apf;

	/* set audio pll reg */
	apf.val = READREG32(VA_PRCM_AUDIO_PLL_FAC);
	apf.bits.pre_div = ptbl->prev_div;
	apf.bits.post_div = ptbl->post_div;
	apf.bits.n = ptbl->fac_n;
	WRITEREG32(VA_PRCM_AUDIO_PLL_FAC, apf.val);

#ifndef CONFIG_ARCH_LOMBO_N9
	/* set audio module clk reg */
	if (is_audio_module_clk(name)) {
	#if defined(CONFIG_ARCH_LOMBO_N7V0)
		reg_prcm_i2s_clk_ctrl_t icc;

		icc.val = READREG32(VA_PRCM_I2S_CLK_CTRL);
	#elif defined(CONFIG_ARCH_LOMBO_N7V1)
		reg_prcm_i2s0_clk_ctrl_t icc;

		if (!strcmp(AUDIO_CLK_I2S0, name))
			icc.val = READREG32(VA_PRCM_I2S0_CLK_CTRL);
		else if (!strcmp(AUDIO_CLK_I2S1, name))
			icc.val = READREG32(VA_PRCM_I2S1_CLK_CTRL);
		else
			WARN_ON(1);
	#endif

		icc.bits.div = ptbl->mod_div;
		if (icc.bits.src_sel != ptbl->mod_clksrc) {
			/*
			 * for i2s_clk, we should call clk_set_parent to select
			 * the correct clk src before set rate(the rate is
			 * corresponding to the clk src).
			 *
			 * if donot warn here, clk_get_parent(i2s_clk) may
			 * return the old(cached, wrong) parent handle.
			 */
			PRT_WARN("clk %s src reg field(%d) wrong",
				name, icc.bits.src_sel);
			icc.bits.src_sel = ptbl->mod_clksrc;
		}

	#if defined(CONFIG_ARCH_LOMBO_N7V0)
		WRITEREG32(VA_PRCM_I2S_CLK_CTRL, icc.val);
	#elif defined(CONFIG_ARCH_LOMBO_N7V1)
		if (!strcmp(AUDIO_CLK_I2S0, name))
			WRITEREG32(VA_PRCM_I2S0_CLK_CTRL, icc.val);
		else if (!strcmp(AUDIO_CLK_I2S1, name))
			WRITEREG32(VA_PRCM_I2S1_CLK_CTRL, icc.val);
		else
			WARN_ON(1);
	#endif
	}
#endif /* CONFIG_ARCH_LOMBO_N9 */

}

/**
 * audio_clk_set_rate - set audio module clock rate
 * @rate: rate to be set
 * @name: clk name
 *
 * query the freq-div-table for the rate, if rate is valid,
 * then set the hw div and factor
 *
 * return 0 if success, <0 if failed
 */
int audio_clk_set_rate(unsigned long rate, const char *name)
{
	struct audio_fd_table *ptbl = NULL;
	int index;

	WARN_ON(!is_audio_module_clk(name));

	/* get clock rate's index in audio_fd_table.rate[] */
	index = clk_name_to_rate_index(name);
	if (index < 0 || index >= RATE_CNT) {
		PRT_ERR("clock name %s invalid\n", name);
		return -EINVAL;
	}

	/* get the matched table item */
	ptbl = fd_table_match_for_set(rate, index);
	/* must be in table, because the rate was passed by round_rate */
	WARN_ON(!ptbl);

	set_rate_hw(ptbl, name);
	return 0;
}

/**
 * audio_clk_round_rate - round audio module clock rate
 * @rate: rate to be rounded
 * @name: clk name
 *
 * query the freq-div-table for the rate, if rate is valid,
 * then return the rate. otherwise return <0, which will
 * abort the clk_set_rate
 */
long audio_clk_round_rate(char *name, unsigned long rate)
{
	struct audio_fd_table *ptbl = NULL;
	int index;

	WARN_ON(!is_audio_module_clk(name));

	/* get clock rate's index in audio_fd_table.rate[] */
	index = clk_name_to_rate_index(name);
	if (index < 0 || index >= RATE_CNT) {
		PRT_ERR("clock name %s invalid\n", name);
		return -EINVAL;
	}

	/* get the matched table item */
	ptbl = fd_table_match_for_set(rate, index);
	if (ptbl)
		return rate;

	/* cannot find matched item */
	PRT_ERR("rate %d invalid for clock %s\n", (int)rate, name);
	return -EINVAL;
}

#endif


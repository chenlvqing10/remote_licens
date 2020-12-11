/*
 * clk.h - head file for lombo clk dirver
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

#ifndef __LOMBO_CLK_H
#define __LOMBO_CLK_H

#include <mach/debug.h>

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"CLK"

/*
 * the below definitions are for module clocks
 */

/* max freq-div count for modules */
#define MAX_FD_CNT_MOD		48

#define to_clk_divider2(_hw) container_of(_hw, struct clk_divider2, hw)

struct rate_dval_table {
	unsigned long	rate;
	unsigned long	parent_rate;
	/*
	 * val0 and val1 are the orignal val read from reg bits
	 * NOT the corresponding division value. eg: val0 is 3
	 * while the corresponding division is 8(2^3)
	 */
	int		val0;
	int		val1;
};

/* module clock with two dividers */
struct clk_divider2 {
	struct clk_hw	hw;
	void __iomem	*reg;
	/*
	 * use u32(not u8) for of_property_read_u32_array
	 */
	u32		shift[2];
	u32		width[2];
	u8		flags[2]; /* divider flags for div0 and div1 */
	const struct clk_div_table *table[2];
	spinlock_t	lock;
	/*
	 * the freq-div table
	 */
	int		fd_cnt;	/* element cnt of table */
	struct rate_dval_table fd_table[MAX_FD_CNT_MOD];
	spinlock_t	fd_lock;
};

/*
 * the below definitions are for pll clocks
 */

/* max freq-div count for pll */
#define MAX_FD_CNT_PLL		32

/* reg index in clock_pll->reg[] */
enum {
	ENABLE = 0, FACTOR, TUNE0, TEST,
	STATUS, MODE, NFAC, TUNE1, TUNE2,
	MAX_PLL_REGS = TUNE2 + 1,
};

/* element index in clock_pll->en_shift[] */
enum { ENP = 0, ENM, OEN, LOCK };

/* element index in clock_pll->fac_shift[] or fac_width[] */
enum { PREV = 0, POST, FAC };

#define MASK(width)	((1 << (width)) - 1)

/* get factor, prev/post div from reg value */
#define GET(val, shift, width)	(((val) >> (shift)) & MASK(width))
#define FAC_N(val)	GET(val, pll->fac_shift[FAC], pll->fac_width[FAC])
#define PREV_DIV(val) (1 + GET(val, pll->fac_shift[PREV], pll->fac_width[PREV]))
#define POST_DIV(val) (1 + GET(val, pll->fac_shift[POST], pll->fac_width[POST]))

#define CLR_BITS(reg, shift, width)	((reg) & ~(MASK(width) << shift))
#define FORM_BITS(val, shift, width)	((val & MASK(width)) << shift)
#define SET(reg, val, shift, width)	(CLR_BITS(reg, shift, width) |	\
						FORM_BITS(val, shift, width))
/* set factor, prev/post div from reg value */
#define SET_FAC_N(reg, val)	SET(reg, val, pll->fac_shift[FAC],	\
					pll->fac_width[FAC])
#define SET_PREV_DIV(reg, val)	SET(reg, val, pll->fac_shift[PREV],	\
					pll->fac_width[PREV])
#define SET_POST_DIV(reg, val)	SET(reg, val, pll->fac_shift[POST],	\
					pll->fac_width[POST])

/* pll clock's freq-factor-div table */
struct pll_fac_dval_table {
	u32		rate;		/* pll clock rate in HZ */
	u32		parent_rate;	/* parent clock rate in HZ */
	u32		factor;		/* pll factor N */
	u32		prev_val;	/* PREV_DIV = prev_val + 1 */
	u32		post_val;	/* PREV_DIV = prev_val + 1 */

#ifdef CONFIG_ARCH_LOMBO_N9V0
	/* pll factor fraction val, valid only when pll in fraction mode */
	u32		nfrac;
#endif
};

/* pll clock struct */
struct clock_pll {
	struct clk_hw	hw;
	void __iomem	*reg[MAX_PLL_REGS]; /* regs' address list for pll */
	u32		reg_cnt;	/* actual reg cnt, <= MAX_PLL_REGS */
	u32		en_shift[4];	/* bit index for enm enp oen lock */
	u32		fac_shift[3];	/* bit index for prevdiv postdiv n */
	u32		fac_width[3];	/* bit width for prevdiv postdiv n */
	u8		flags[2];	/* divider flags for prevdiv postdiv */
	const struct clk_div_table *table[2]; /* prevdiv and postdiv table */
	spinlock_t	lock;
	struct mutex	prepare_lock;	/* lock for prepare, can sleep */

	/*
	 * the pll freq and factor/div table
	 */
	int		fd_cnt;		/* element cnt of table */
	struct pll_fac_dval_table fd_table[MAX_FD_CNT_PLL];
	spinlock_t	fd_lock;
};

/*
 * the below definitions are for audio clk
 */

/* audio pll name (POST_DIV reg field bypassed) */
#define AUDIO_PLL_NAME		"audio_pll_div0"
/* bit index in audio pll enable reg for DIV_ENDIV7 */
#define AUDIO_PLL_DIV7_BIT_IDX	9
/* bit index in audio pll enable reg for DIV_ENDIV17 */
#define AUDIO_PLL_DIV17_BIT_IDX	8
/* audio pll divider clk name */
#define AUDIO_PLL_DIV7		"audio_pll_div7"
#define AUDIO_PLL_DIV17		"audio_pll_div17"
#define AUDIO_PLL_DIVM		"audio_pll_divm"
#if defined(CONFIG_ARCH_LOMBO_N7V0)
#define AUDIO_CLK_I2S		"i2s_clk"
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
#define AUDIO_CLK_I2S0		"i2s0_clk"
#define AUDIO_CLK_I2S1		"i2s1_clk"
#elif defined(CONFIG_ARCH_LOMBO_N9V0)
#define AUDIO_CLK_AC		"ac_clk"
#endif

int is_audio_pll_clk(const char *name);

#ifdef CONFIG_ARCH_LOMBO_N9
unsigned long audio_pll_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate);
#endif

#ifdef CONFIG_ARCH_LOMBO_N7
int is_audio_fix_divider_clk(const char *name);
int is_audio_divider_clk(const char *name);
int is_audio_module_clk(const char *name);
unsigned long audio_clk_recalc_rate(const char *name);
int audio_clk_set_rate(unsigned long rate, const char *name);
long audio_clk_round_rate(char *name, unsigned long rate);
#endif

/*
 * the below definitions are common functions and operations
 */

#define to_clock_pll(_hw) container_of(_hw, struct clock_pll, hw)

unsigned int __get_div(u8 flags, unsigned int val,
			const struct clk_div_table *table);
int __calc_val(u8 flags, int div, u32 div_width,
			const struct clk_div_table *table,
			bool exact_match);
int __init_divider_table(struct device_node *node, unsigned long *table_out);
unsigned int _get_table_div(const struct clk_div_table *table,
					unsigned int val);

#endif /* __LOMBO_CLK_H */


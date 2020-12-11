/*
 * clk-test.c - clk test driver
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
#include <linux/device.h>
#include <linux/err.h>

#include "common.h"

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"CLK_TEST" /* for print message */

#define CLK_1M		1000000
#define CLK_1K		1000

#define FIX_RATE_NAME	"osc24m"
#define FIX_RATE	24000000

#define PLL_NAME	"cpu_pll"
#define PLL_SRC_NAME	"osc24m"

#define MODULE_NAME	"spi1_clk"
#define MODULE_PARENT	"perh0_pll_div2" /* clk parent to test */

#if defined(CONFIG_ARCH_LOMBO_N7V0)
#define AUDIO_MOD_NAME	"i2s_clk"
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
#define AUDIO_MOD_NAME	"i2s1_clk"
#endif

#define FIX_FACTOR_NAME	"apb_clk"
#define GATE_NAME	"ahb_sdc0_gate"
#if defined(CONFIG_ARCH_LOMBO_N7V0)
#define DIVIDER_NAME	"cpu_axi_clk"
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
#define DIVIDER_NAME	"perh0_pll_divm"
#endif

/* rates to test for pll, module and divider clk */
int rates[] = {
	/* [0, 10M] */
	0, 1, 10, 100, 1111, 8021, 16000, 64000, 50000, 250000,
	700000,	1000000, 4000051, 7234567, 8000000, 9500000,
	/* [10M, 100M] */
	10000000, 11000000, 12000000, 13000000, 14000000, 15000000,
	16000000, 17000000, 18000000, 19000000, 24000000, 25000000,
	26000000, 27000000, 28000000, 29000000, 30000000, 31000000,
	32000000, 33000000, 40000000, 50000000, 59000000, 74000000,
	/* [100M, 1000M] */
	130000000, 170000000, 180000000, 190000000, 200000000,
	210000000, 220000000, 230000000, 240000000, 250000000,
	260000000, 270000000, 280000000, 290000000, 300000000,
	310000000, 320000000, 330000000, 340000000, 350000000,
	360000000, 370000000, 380000000, 390000000, 400000000,
	410000000, 600000000, 720000000, 850000000, /* 1008000000 */
};

/* rates to test for audio clk */
int audio_rates[] = {
	0, 1, 10, 100, 1000, 1000000, 8000000,
	24571000, 24571000 * 2, 24571000 * 3,
	22588200, 22588200 * 2, 22588200 * 3,
	24571000 * 6, 22588200 * 6, 876543210,
};

/**
 * fix_rate_clk_test - test fixed-clock(fix rate)
 */
void fix_rate_clk_test(void)
{
	char clk_name[32] = FIX_RATE_NAME;
	struct clk *clock = NULL;
	int rate;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success\n", clk_name);

	/* get clock rate */
	rate = (int)clk_get_rate(clock);
	PRT_INFO("%s: clock %s rate is %d\n",
		(rate == FIX_RATE ? "success" : "err"), clk_name, rate);

	/* put clock handle */
	clk_put(clock);

	PRT_INFO("end\n");
}

/**
 * fix_factor_clk_test - test fix-factor-clock
 */
void fix_factor_clk_test(void)
{
	char clk_name[32] = FIX_FACTOR_NAME;
	struct clk *clock = NULL;
	int rate, rate2;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_name, (int)clock);

	/* get clock rate */
	rate = (int)clk_get_rate(clock);
	PRT_INFO("clock rate is %d\n", rate);

	/* set clock rate */
	if (clk_set_rate(clock, rate / 2)) {
		PRT_ERR("set clk %s rate to %d failed\n", clk_name, rate / 2);
		goto end;
	}
	PRT_INFO("set clock %s rate to %d return 0, ok!\n", clk_name, rate / 2);

	/* check if set rate success */
	rate2 = (int)clk_get_rate(clock);
	PRT_INFO("%s: after set clock, clock %s rate is %d\n",
		(rate2 == rate ? "success" : "err"), clk_name, rate2);

	/* recover the environment */
	clk_set_rate(clock, rate);

end:
	/* put clock handle */
	clk_put(clock);

	PRT_INFO("end\n");
}

/**
 * gate_clk_test - test gate-clock
 */
void gate_clk_test(void)
{
	struct clk *clock = NULL;
	char clk_name[32] = GATE_NAME;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_name, (int)clock);

	/* enable clock */
	if (clk_prepare_enable(clock)) {
		PRT_ERR("clock %s prepare enable failed\n", clk_name);
		goto end;
	}
	PRT_INFO("clock %s prepare enable sucess\n", clk_name);

	/* recover the environment */
	clk_disable_unprepare(clock);

end:
	/* put clock handle */
	clk_put(clock);

	PRT_INFO("end\n");
}

/**
 * divider_clk_test - test divider-clk
 */
void divider_clk_test(void)
{
	char clk_name[32] = DIVIDER_NAME;
	struct clk *clock = NULL;
	int i, rate, rate2;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_name, (int)clock);

	/* get clock rate */
	rate = (int)clk_get_rate(clock);
	PRT_INFO("clock %s rate is %d\n", clk_name, rate);

	/* set clock rate */
	if (clk_set_rate(clock, rate / 2)) {
		PRT_ERR("set clock %s rate to %d failed\n",
			clk_name, rate / 2);
		goto end;
	}
	PRT_INFO("set clock %s rate to %d return 0, ok!\n", clk_name, rate / 2);

	/* get clock rate */
	rate2 = (int)clk_get_rate(clock);
	PRT_INFO("after set rate, clock %s rate is %d\n", clk_name, rate2);

	for (i = 0; i < ARRAY_SIZE(rates); i++) {
		if (clk_set_rate(clock, rates[i])) {
			PRT_ERR("set clock %s rate to %d failed\n",
				clk_name, rates[i]);
			/* although set rate failed, we let test continue */
			/* goto end; */
		} else {
			PRT_INFO("set clock %s rate to %d success!\n\t"
				"after set rate, the clock's rate is %d\n",
				clk_name, rates[i], (int)clk_get_rate(clock));
		}
	}

	/* recover the environment */
	clk_set_rate(clock, rate);

end:
	/* put clock handle */
	clk_put(clock);

	PRT_INFO("end\n");
}

/**
 * pll_clk_test - test pll-clk
 */
void pll_clk_test(void)
{
	char clk_name[32] = PLL_NAME;
	struct clk *clock = NULL;
	struct clk *clock_p = NULL;
	int i, rate;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_name, (int)clock);

	/* get parent clock */
	clock_p = clk_get_parent(clock);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s parent failed\n", clk_name);
		goto end_put_clk;
	}
	PRT_INFO("get clock %s parent success, parent is %s\n",
		clk_name, __clk_get_name(clock_p));

	/* prepare enable clock */
	if (clk_prepare_enable(clock)) {
		PRT_ERR("clock %s prepare enable failed\n", clk_name);
		goto end_put_clk;
	}
	PRT_INFO("clock %s prepare enable sucess\n", clk_name);

	/* get clock rate */
	rate = (int)clk_get_rate(clock);
	PRT_INFO("clock %s rate is %d\n", clk_name, rate);

	/* set clock rate */
	for (i = 0; i < ARRAY_SIZE(rates); i++) {
		if (clk_set_rate(clock, rates[i])) {
			PRT_ERR("set clock %s rate to %d failed\n",
				clk_name, rates[i]);
			/* although set rate failed, we let test continue */
			/* goto end; */
		} else {
			PRT_INFO("set clock %s rate to %d success!\n\t"
				"after set rate, the clock's rate is %d\n",
				clk_name, rates[i], (int)clk_get_rate(clock));
		}
	}

	/* recover the environment */
	PRT_INFO("recover clock %s rate to %d\n", clk_name, rate);
	clk_set_rate(clock, rate);

	PRT_INFO("disable unprepare clock %s\n", clk_name);
	clk_disable_unprepare(clock);
end_put_clk:
	/* put clock handle */
	clk_put(clock);

	PRT_INFO("end\n");
}

/**
 * module_clk_test - test module-clk
 */
void module_clk_test(void)
{
	char clk_name[32] = MODULE_NAME;
	char clk_p_name[32] = MODULE_PARENT;
	struct clk *clock = NULL;
	struct clk *clock_tmp = NULL;
	struct clk *clock_p_org = NULL;
	struct clk *clock_p_test = NULL; /* handle for clk_p_name */
	int i, rate;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_name, (int)clock);

	/* get parent(to test) clock handle */
	clock_p_test = clk_get(NULL, clk_p_name);
	if (IS_ERR(clock_p_test)) {
		PRT_ERR("get clock %s handle failed\n", clk_p_name);
		clk_put(clock);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_p_name, (int)clock_p_test);

	/*
	 * test set clock parent
	 */

	/* get original parent clock */
	clock_p_org = clk_get_parent(clock);
	if (IS_ERR(clock_p_org)) {
		PRT_ERR("get clock %s original parent failed\n", clk_name);
		goto end_put_clk;
	}
	PRT_INFO("get clock %s original parent success, is %s\n",
		clk_name, __clk_get_name(clock_p_org));

	/* set clock parent */
	if (clk_set_parent(clock, clock_p_test)) {
		PRT_ERR("set clock %s parent to %s failed\n",
			clk_name, clk_p_name);
		goto end_put_clk;
	}
	PRT_INFO("set clock %s parent to %s return 0, ok!\n",
		clk_name, clk_p_name);

	/* check if set parent success */
	clock_tmp = clk_get_parent(clock);
	if (clock_tmp == clock_p_test)
		PRT_INFO("set clock %s parent to %s success\n",
			clk_name, clk_p_name);
	else
		PRT_ERR("after set, clock %s parent is %s\n",
			clk_name, __clk_get_name(clock_tmp));
	/*
	 * test set rate
	 */

	/* prepare enable clock */
	if (clk_prepare_enable(clock)) {
		PRT_ERR("clock %s prepare enable failed\n", clk_name);
		goto recover_parent;
	}
	PRT_INFO("clock %s prepare enable sucess\n", clk_name);

	/* get clock rate */
	rate = (int)clk_get_rate(clock);
	PRT_INFO("clock %s rate is %d\n", clk_name, rate);

	/* set clock rate */
	for (i = 0; i < ARRAY_SIZE(rates); i++) {
		if (clk_set_rate(clock, rates[i])) {
			PRT_ERR("set clock %s rate to %d failed\n",
				clk_name, rates[i]);
			/* although set rate failed, we let test continue */
			/* goto end; */
		} else {
			PRT_INFO("set clock %s rate to %d success!\n\t"
				"after set rate, the clock's rate is %d\n",
				clk_name, rates[i], (int)clk_get_rate(clock));
		}
	}

	/* recover the environment */
	clk_set_rate(clock, rate);
	clk_disable_unprepare(clock);
recover_parent:
	/* recover clock parent */
	PRT_INFO("recover clock %s parent to %s\n", clk_name,
		__clk_get_name(clock_p_org));
	clk_set_parent(clock, clock_p_org);
end_put_clk:
	/* put clock handle */
	clk_put(clock);
	clk_put(clock_p_test);

	PRT_INFO("end\n");
}

/**
 * dump_audio_clk_rate - dump all the audio clk's rate
 */
void dump_audio_clk_rate(void)
{
	char audio_clk[][32] = {AUDIO_MOD_NAME, "audio_pll_div0",
			"audio_pll_div7", "audio_pll_div17", "audio_pll_divm"};
	struct clk **clocks = NULL;
	int *rates = NULL;
	int i, clk_cnt;

	clk_cnt = ARRAY_SIZE(audio_clk);

	/* alloc resources */
	clocks = kzalloc(clk_cnt * sizeof(struct clk *), GFP_KERNEL);
	rates = kzalloc(clk_cnt * sizeof(int *), GFP_KERNEL);
	WARN_ON(!clocks || !rates);

	/* get all clock's rate */
	for (i = 0; i < clk_cnt; i++) {
		clocks[i] = clk_get(NULL, audio_clk[i]);
		if (IS_ERR(clocks[i]))
			continue;
		rates[i] = (int)clk_get_rate(clocks[i]);
	}

	/* dump the rate table */
	PRT_INFO("+++++++++++++ %s(%d) +++++++++++++\n", __func__, __LINE__);
	PRT_INFO("%-24s%-24s\n", "clk_name", "clk_rate");
	for (i = 0; i < clk_cnt; i++)
		PRT_INFO("%-24s%-24d\n", audio_clk[i], rates[i]);
	PRT_INFO("------------- %s(%d) -------------\n", __func__, __LINE__);

	/* free resources */
	while (clk_cnt--) {
		if (!IS_ERR(clocks[clk_cnt]))
			clk_put(clocks[clk_cnt]);
	}
	kfree(clocks);
	kfree(rates);
}

/**
 * audio_clk_test - test audio clk, such as i2s_clk, audio_pll_divX
 */
void audio_clk_test(void)
{
	char clk_name[32] = AUDIO_MOD_NAME;
	struct clk *clock = NULL;
	struct clk *clock_p = NULL;
	int i, rate;

	PRT_INFO("start\n");

	/* get clock handle */
	clock = clk_get(NULL, clk_name);
	if (IS_ERR(clock)) {
		PRT_ERR("get clock %s handle failed\n", clk_name);
		return;
	}
	PRT_INFO("get clock %s handle success, ret 0x%08x\n",
		clk_name, (int)clock);

	/* get parent clock */
	clock_p = clk_get_parent(clock);
	if (IS_ERR(clock_p)) {
		PRT_ERR("get clock %s parent failed\n", clk_name);
		goto end_put_clk;
	}
	PRT_INFO("get clock %s parent success, is %s\n",
		clk_name, __clk_get_name(clock_p));

	/* prepare enable clock */
	if (clk_prepare_enable(clock)) {
		PRT_ERR("clock %s prepare enable failed\n", clk_name);
		goto end_put_clk;
	}
	PRT_INFO("clock %s prepare enable sucess\n", clk_name);

	/* dump audio clocks before change rate */
	PRT_INFO("before change clock %s's rate:\n", clk_name);
	dump_audio_clk_rate();

	/* get clock rate */
	rate = (int)clk_get_rate(clock);
	PRT_INFO("clock %s rate is %d\n", clk_name, rate);

	/* set clock rate */
	for (i = 0; i < ARRAY_SIZE(audio_rates); i++) {
		if (clk_set_rate(clock, audio_rates[i])) {
			PRT_ERR("set clock %s rate to %d failed\n",
				clk_name, audio_rates[i]);
			/* although set rate failed, we let test continue */
			/* goto end; */
		} else {
			PRT_INFO("set clock %s rate to %d success!\n\t"
				"after change clock's rate:\n",
				clk_name, audio_rates[i]);
			/* dump audio clocks after change rate */
			dump_audio_clk_rate();
		}
	}

	/* recover the environment */
	clk_set_rate(clock, rate);
	clk_disable_unprepare(clock);
end_put_clk:
	/* put clock handle */
	clk_put(clock);

	PRT_INFO("end\n");
}

/**
 * clk_test - clk drv test function
 */
void clk_test(void)
{
	PRT_INFO("start\n");

	fix_rate_clk_test();
	fix_factor_clk_test();
	gate_clk_test();
	divider_clk_test();
	pll_clk_test();
	module_clk_test();
	audio_clk_test();

	PRT_INFO("end\n");
}


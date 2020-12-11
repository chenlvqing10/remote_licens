/*
 * clk.c - clocks basic code
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
#include <linux/clk-private.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/io.h>
#include <mach/csp.h>

#include "clk.h"
#include "../pm/deepsleep/include.h"

#define SYNC_UBOOT_DISP_CLK	1

#ifdef CONFIG_OF

/**
 * of_divider_clk_setup - setup function for divider clock
 * @node: the matched device tree node in dts file
 *
 * the divider clock's config should be as below:
 * axi_clk {				the clock' name
 *	compatible = "divider-clock";	identify string, should not be changed
 *	reg = <0x0400a900 0x4>;		the register addr and len
 *	#clock-cells = <0>;		should be 0
 *	clocks = <&cpu>;		parent clock of axi_clk
 *	div-shift = <16>;		the bit offset for the divider
 *	div-width = <2>;		the bit length for the divider
 * };
 *
 */
void of_divider_clk_setup(struct device_node *node)
{
	const char *clk_name = node->name;
	const char *parent_name;
	u32 div_shift, div_width;
	struct clk *clk = NULL;
	unsigned long flags = 0;
	void __iomem *reg;

	reg = of_iomap(node, 0);
	WARN_ON(!reg);

	if (of_property_read_u32(node, "div-shift", &div_shift)
		|| of_property_read_u32(node, "div-width", &div_width)) {
		PRT_ERR("div-shift/div-width invalid, clock %s\n", clk_name);
		return;
	}

	parent_name = of_clk_get_parent_name(node, 0);
	if (NULL == parent_name) {
		PRT_ERR("parent clock config err, clock %s\n", clk_name);
		return;
	}

#ifdef CONFIG_ARCH_LOMBO_N7V1
	if (is_audio_module_clk(clk_name) || is_audio_divider_clk(clk_name))
		flags |= CLK_GET_RATE_NOCACHE;
#endif

	clk = clk_register_divider(NULL, clk_name, parent_name, flags, reg,
			(u8)div_shift, (u8)div_width, 0, NULL);
	if (IS_ERR(clk)) {
		PRT_ERR("register clock %s failed\n", clk_name);
		return;
	}

	of_clk_add_provider(node, of_clk_src_simple_get, clk);
	WARN_ON(clk_register_clkdev(clk, clk_name, NULL));
}
EXPORT_SYMBOL_GPL(of_divider_clk_setup);
CLK_OF_DECLARE(divider_clk, "divider-clock", of_divider_clk_setup);

/**
 * of_gate_clk_setup - setup function for gate clock
 * @node: the matched device tree node in dts file
 *
 * apb1_gate {		node name(not the actual clock name), which be used by
 *				other module as "clocks = <&apb1_gate index>;"
 *	compatible = "gate-clock";	identify string, should not be changed
 *	reg = <0x0400a2c0 0x4>;		the register addr and len
 *	#clock-cells = <1>;		should be 1 (one para when used above)
 *	clocks = <&apb_clk>;		parent clock of apb1_gate
 *	clock-output-names = "apb_audio_gate", "apb_i2s_gate", ...;
 *				optional, string list of output clock names
 *	enable_bit_mask = <0x00000246>; bit mask for the reg, the bit sequence
 *					must be consistent with spec
 * };
 *
 */
void of_gate_clk_setup(struct device_node *node)
{
	const char *clk_name = node->name;
	struct clk_onecell_data *cod;
	const char *parent_name;
	struct clk *clk = NULL;
	void __iomem *reg;
	u32 bit_mask, last_bit_idx;
	int bit_idx, string_idx = 0, tmp;
	unsigned long flags = CLK_IGNORE_UNUSED;

	reg = of_iomap(node, 0);
	WARN_ON(!reg);

	parent_name = of_clk_get_parent_name(node, 0);
	if (NULL == parent_name) {
		PRT_ERR("parent clock config err, clock %s\n", clk_name);
		return;
	}

	if (of_property_read_u32(node, "enable_bit_mask", &bit_mask)) {
		PRT_ERR("enable_bit_mask not found for clock %s\n", clk_name);
		return;
	}
	WARN_ON(0 == bit_mask);

	last_bit_idx = find_last_bit((const unsigned long *)&bit_mask,
					BITS_PER_LONG);

	/* gate clocks without provider function */
	if (!of_get_property(node, "clock-output-names", &tmp)) {
		/* only one bit(the gate bit) should be set */
		WARN_ON(bit_mask & ~(1 << last_bit_idx));
		clk = clk_register_gate(NULL, clk_name, parent_name,
				0, reg, last_bit_idx, 0, NULL);
		WARN_ON(IS_ERR_OR_NULL(clk));
		WARN_ON(clk_register_clkdev(clk, clk_name, NULL));
		WARN_ON(of_clk_add_provider(node, of_clk_src_simple_get, clk));
		return;
	}

	/* gate clocks with provider function, such as ahb gate/reset */
	cod = kzalloc(sizeof(struct clk_onecell_data), GFP_KERNEL);
	WARN_ON(NULL == cod);

	cod->clks = kzalloc(sizeof(struct clk *) * (last_bit_idx + 1),
			GFP_KERNEL);
	WARN_ON(NULL == cod->clks);
	cod->clk_num = last_bit_idx + 1;

	for_each_set_bit(bit_idx, (const unsigned long *)&bit_mask,
				(unsigned long)(last_bit_idx + 1)) {
		tmp = of_property_read_string_index(node, "clock-output-names",
				string_idx, &clk_name);
		if (tmp) {
			PRT_ERR("cannot read clock-%d's name\n", string_idx);
			goto err;
		}

		clk = clk_register_gate(NULL, clk_name, parent_name,
				flags, reg, bit_idx, 0, NULL);
		if (IS_ERR(clk)) {
			PRT_ERR("register clock %s failed\n", clk_name);
			goto err;
		}

		cod->clks[string_idx] = clk;

		WARN_ON(clk_register_clkdev(clk, clk_name, NULL));
		string_idx++;
	}

	of_clk_add_provider(node, of_clk_src_onecell_get, cod);
	return;

err:
	if (NULL != cod->clks)
		kfree(cod->clks);
	if (NULL != cod)
		kfree(cod);
}
EXPORT_SYMBOL_GPL(of_gate_clk_setup);
CLK_OF_DECLARE(gate_clk, "gate-clock", of_gate_clk_setup);

#endif

#if defined(CONFIG_ARCH_LOMBO_N7V1)
static void __sync_prepare_clk(const char *clk_name)
{
	struct clk *clk = NULL;
	int ret;

	pr_info("sync u-boot clk: %s\n", clk_name);

	/* get clock handle */
	clk = clk_get(NULL, clk_name);
	if (!IS_ERR(clk)) {
		/* enable the clock */
		ret = clk_prepare_enable(clk);
		if (ret != 0)
			PRT_ERR("clk_prepare_enable clk failed: %s\n",
				clk_name);

		/* put clock handle */
		clk_put(clk);
	} else
		PRT_ERR("clk_get failed: %s\n", clk_name);
}

static int __get_clk_en_bit(u32 reg_addr, u32 b)
{
	u32 rdval;

	rdval = READREG32(reg_addr);
	return rdval & BIT(b);
}
#endif

static void __sync_uboot_disp_clk(void)
{
#if defined(CONFIG_ARCH_LOMBO_N7V1)
	u32 rdval;
	u32 reg_addr;
	char *clk_name;

	pr_info("__sync_uboot_disp_clk\n");

	/* ahb_dpu_gate */
	if (__get_clk_en_bit(VA_PRCM_AHB_GAT1, 2)) {
		clk_name = "ahb_dpu_gate";
		__sync_prepare_clk(clk_name);
	}

	/* ahb_dpu_reset */
	if (__get_clk_en_bit(VA_PRCM_AHB_RST1, 2)) {
		clk_name = "ahb_dpu_reset";
		__sync_prepare_clk(clk_name);
	}

	/* ahb_doss_gate */
	if (__get_clk_en_bit(VA_PRCM_AHB_GAT1, 4)) {
		clk_name = "ahb_doss_gate";
		__sync_prepare_clk(clk_name);
	}

	/* ahb_doss_reset */
	if (__get_clk_en_bit(VA_PRCM_AHB_RST1, 4)) {
		clk_name = "ahb_doss_reset";
		__sync_prepare_clk(clk_name);
	}

	/* maxi_dpu_gate */
	if (__get_clk_en_bit(VA_PRCM_AXI_GAT, 8)) {
		clk_name = "maxi_dpu_gate";
		__sync_prepare_clk(clk_name);
	}

	/* perh0_pll_div2 */
	reg_addr = VA_PRCM_PERH0_PLL_EN;
	rdval = READREG32(reg_addr);
	if ((rdval & BIT(0)) && (rdval & BIT(1)) && (rdval & BIT(2))) {
		clk_name = "perh0_pll_div2";
		__sync_prepare_clk(clk_name);
	}

	/* dpu_sclk0 */
	if (__get_clk_en_bit(VA_PRCM_DPU_SCLK0_CLK_CTRL, 31)) {
		clk_name = "dpu_sclk0";
		__sync_prepare_clk(clk_name);
	}

	/* dpu_sclk2 */
	if (__get_clk_en_bit(VA_PRCM_DPU_SCLK2_CLK_CTRL, 31)) {
		clk_name = "dpu_sclk2";
		__sync_prepare_clk(clk_name);
	}

	/* doss_sclk0 and doss_osc24m use the same register */
	reg_addr = VA_PRCM_DOSS_CLK_CTRL;
	rdval = READREG32(reg_addr);
	if ((rdval & BIT(0)) || (rdval & BIT(8))) {
		if (rdval & BIT(0)) {
			/* 0 bit for HFEOSC_EN */
			clk_name = "doss_osc24m";
			__sync_prepare_clk(clk_name);
		}

		if (rdval & BIT(8)) {
			/* 8 bit for SCLK0_EN */
			clk_name = "doss_sclk0";
			__sync_prepare_clk(clk_name);
		}
	}
#endif
}

static void __sync_uboot_bus_clk(void)
{
#if defined(CONFIG_ARCH_LOMBO_N7V1)
	char *clk_name;

	pr_info("__sync_uboot_bus_clk\n");

	if (__get_clk_en_bit(VA_PRCM_MAXI_CLK_CTRL, 31)) {
		clk_name = "mem_axi_clk";

		/* enable mem_axi_gate to let maxi bus enabled */
		__sync_prepare_clk(clk_name);
	}
#endif
}

/**
 * lombo_clk_init - intialize the pll & gate &.. clocks
 *
 */
int __init lombo_clk_init(void)
{
	struct clk *clk = NULL;
	int i;

#if defined(CONFIG_ARCH_LOMBO_N9V0)
	char clk_to_en[][32] = {
		"osc24m", "osc32k", "null_clk", "sdram_pll",
	};
#else
	char clk_to_en[][32] = {
		"osc24m", "osc32k", "null_clk", "sdram_pll",
		"perh1_pll", "perh2_pll",
		/* "perh0_pll_vco", "audio_pll_div0", "vc_pll", "video_pll" */
	};
#endif

	for (i = 0; i < ARRAY_SIZE(clk_to_en); i++) {
		/* get clock handle */
		clk = clk_get(NULL, clk_to_en[i]);
		if (IS_ERR(clk)) {
			PRT_ERR("get clk %s handle failed\n", clk_to_en[i]);
			continue;
		}

#ifndef CONFIG_ARCH_LOMBO_N9
		/* set perh1 clock 600m, spread mode, tune1, nfrac */
		if (!strcmp(clk_to_en[i], "perh1_pll")) {
			clk_set_rate(clk, 600000000);
			writel(PLL_MOD_SPREAD, (void *)VA_PRCM_PERH1_PLL_MOD);
			writel(0, (void *)VA_PRCM_PERH1_PLL_NFRAC);
			writel((492 << 16) | 400,
				(void *)VA_PRCM_PERH1_PLL_TUNE1);
		}
#endif

		/* enable the clock */
		if (clk_prepare_enable(clk))
			PRT_ERR("prepare enable %s failed\n", clk_to_en[i]);

		/* put clock handle */
		clk_put(clk);
	}

#if SYNC_UBOOT_DISP_CLK
	/* synchronous enabled display clock at u-boot to clock framework */
	__sync_uboot_disp_clk();
#endif

	/*
	 * fix deepsleep->suspend hung due to maxi disabled
	 * while cpu accessing dram
	 */
	__sync_uboot_bus_clk();
	return 0;
}
#if defined(CONFIG_ARCH_LOMBO_N9V0)
subsys_initcall_sync(lombo_clk_init);
#else
arch_initcall(lombo_clk_init);
#endif


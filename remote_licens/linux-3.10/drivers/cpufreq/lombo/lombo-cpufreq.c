/*
 * lombo-cpufreq.c - Driver of LomboTech CPUfreq
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/cpu.h>
#include <linux/slab.h>
#include <mach/debug.h>
#ifdef CONFIG_LOMBO_CPU_ADPLL
#include "csp/cpu_adpll.h"
#endif

#undef MOD_NAME
#define MOD_NAME "CPUFREQ"

#define LOMBO_CPUFREQ_MAX         (1200000) /* KHz */
#define LOMBO_CPUFREQ_MIN          (312000) /* KHz */

/**
 * struct lombo_cpufreq - Runtime control data for cpufreq.
 * @clk_pll: pointer to cpu pll clock.
 * @clk_cpu: pointer to cpu clock.
 * @freq_table: pointer to cpufreq frequency table.
 * @transition_latency: the time of switching two frequencies in nanoseconds.
 * @max_freq: max frequency supported, based on KHz.
 * @min_freq: min frequency supported, based on KHz.
 */
struct lombo_cpufreq {
#ifndef CONFIG_LOMBO_CPU_ADPLL
	struct clk *clk_pll;
	struct clk *clk_cpu;
#endif
	struct cpufreq_frequency_table *freq_table;

	u32 transition_latency;
	u32 max_freq;
	u32 min_freq;
};

static struct lombo_cpufreq e_cpufreq;
static DEFINE_MUTEX(lombo_cpufreq_lock);

/**
 * lombo_cpufreq_verify - check if the cpu frequency policy is valid.
 * @policy: pointer to cpufreq policy.
 *
 * Return 0 if valid.
 */
static int lombo_cpufreq_verify(struct cpufreq_policy *policy)
{
	return cpufreq_frequency_table_verify(policy, e_cpufreq.freq_table);
}

/**
 * lombo_cpufreq_get - get cpu frequency from hardware.
 * @cpu: cpu number.
 *
 * Return cpu frequency, based on KHz.
 */
static unsigned int lombo_cpufreq_get(unsigned int cpu)
{
#ifndef CONFIG_LOMBO_CPU_ADPLL
	return clk_get_rate(e_cpufreq.clk_cpu) / 1000;
#else
	return csp_get_cpu_rate() / 1000;
#endif
}

/**
 * lombo_cpufreq_set_rate - set target frequency to hardware.
 * @freq: target frequency.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_cpufreq_set_rate(unsigned int freq)
{
	int err = 0;

#ifndef CONFIG_LOMBO_CPU_ADPLL
	if (clk_set_rate(e_cpufreq.clk_pll, freq * 1000)) {
		PRT_ERR("set cpu pll to %uKHz failed!\n", freq);
		err = -EINVAL;
		goto out;
	}
#else
	if (csp_set_cpu_rate(freq * 1000)) {
		PRT_ERR("set cpu pll to %uKHz failed!\n", freq);
		err = -EINVAL;
		goto out;
	}
#endif

out:
	return err;
}

/**
 * lombo_cpufreq_target - set target frequency to cpu.
 * @policy: pointer to cpufreq policy.
 * @target_freq: target frequency to be set, based on KHz.
 * @relation: method for selecting the target frequency.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_cpufreq_target(struct cpufreq_policy *policy,
				unsigned int target_freq, unsigned int relation)
{
	int err = 0;
	unsigned int index;
	struct cpufreq_freqs freqs;
	unsigned int freq = 0;

	mutex_lock(&lombo_cpufreq_lock);

	if (target_freq == policy->cur)
		goto out;

	PRT_DBG("request frequency: %uKHz\n", target_freq);

	/* try to look for a valid frequency value from cpu frequency table */
	if (cpufreq_frequency_table_target(policy, e_cpufreq.freq_table,
					target_freq, relation, &index)) {
		PRT_ERR("try to find %uKHz failed!\n", target_freq);
		err = -EINVAL;
		goto out;
	}

	/* target frequency is the same as current, not set */
	if (e_cpufreq.freq_table[index].frequency == policy->cur)
		goto out;

	/* find target frequency in the cpu frequency table */
	freq = e_cpufreq.freq_table[index].frequency;

	PRT_DBG("target frequency: %uKHz, index:%u\n", freq, index);

	freqs.cpu = policy->cpu;
	freqs.old = policy->cur;
	freqs.new = freq;
	cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);

	/* set cpu frequency to hw */
	if (lombo_cpufreq_set_rate(freq)) {
		PRT_ERR("set cpu frequency to %uKHz failed!\n", freq);

		/* set cpu frequency failed */
		freqs.cpu = policy->cpu;
		freqs.old = freqs.new;
		freqs.new = policy->cur;
		cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);

		err = -EINVAL;
		goto out;
	}

	cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);

	PRT_DBG("set frequency: %uKHz success\n", lombo_cpufreq_get(0));

out:
	mutex_unlock(&lombo_cpufreq_lock);

	return err;
}

/**
 * lombo_cpufreq_init - initialise a policy;
 * @policy:  cpu frequency policy;
 *
 * Return 0 if success, !0 error;
 */
static int lombo_cpufreq_init(struct cpufreq_policy *policy)
{
	int err = 0;

	err = cpufreq_frequency_table_cpuinfo(policy, e_cpufreq.freq_table);
	if (err) {
		PRT_ERR("init cpufreq table cpuinfo failed\n");
		goto out;
	}

	cpufreq_frequency_table_get_attr(e_cpufreq.freq_table, policy->cpu);
	policy->cpuinfo.transition_latency = e_cpufreq.transition_latency;

	policy->governor = CPUFREQ_DEFAULT_GOVERNOR;
	policy->cur = lombo_cpufreq_get(0);

	if (policy->max > e_cpufreq.max_freq)
		policy->max = e_cpufreq.max_freq;

	if (policy->min < e_cpufreq.min_freq)
		policy->min = e_cpufreq.min_freq;

#ifdef CONFIG_SMP
	policy->shared_type = CPUFREQ_SHARED_TYPE_ANY;
	cpumask_copy(policy->cpus, cpu_possible_mask);
#endif

	PRT_DBG("boot freq: %uKHz\n", policy->cur);

out:
	return err;
}

/**
 * lombo_cpufreq_exit - exit a policy;
 * @policy:  cpu frequency policy;
 *
 * Return 0 if success, !0 error;
 */
static int lombo_cpufreq_exit(struct cpufreq_policy *policy)
{
	cpufreq_frequency_table_put_attr(policy->cpu);

	return 0;
}

static struct freq_attr *lombo_cpufreq_attr[] = {
	 &cpufreq_freq_attr_scaling_available_freqs,
	 NULL,
};

static struct cpufreq_driver lombo_cpufreq_driver = {
	.name   = "cpufreq-lombo",
	.flags  = CPUFREQ_STICKY,
	.init   = lombo_cpufreq_init,
	.exit   = lombo_cpufreq_exit,
	.verify = lombo_cpufreq_verify,
	.target = lombo_cpufreq_target,
	.get    = lombo_cpufreq_get,
	.attr   = lombo_cpufreq_attr,
};

/**
 * lombo_cpufreq_get_valid_freq - get a valid frequency from frequency table;
 * target_freq: target frequency to be judge, based on KHz;
 *
 * Return cpu frequency, based on KHz;
 */
static unsigned int lombo_cpufreq_get_valid_freq(unsigned int target_freq)
{
	struct cpufreq_frequency_table *tmp_tbl = e_cpufreq.freq_table;

	while (tmp_tbl->frequency != CPUFREQ_TABLE_END) {
		if ((tmp_tbl + 1)->frequency <= target_freq)
			tmp_tbl++;
		else
			break;
	}

	return tmp_tbl->frequency;
}

/**
 * lombo_cpufreq_range_select - init cpu max/min frequency from dts.
 *
 * Return 0 if init cpu max/min success, !0 error.
 */
static int lombo_cpufreq_range_select(void)
{
	struct device_node *np;
	int err = 0;

	np = of_find_node_by_path("/cpufreq_limit_table");
	if (!np) {
		PRT_ERR("no cpufreq limit table node found\n");
		err = -ENODEV;
		goto out;
	}

	if (of_property_read_u32(np, "max_freq", &e_cpufreq.max_freq)) {
		PRT_ERR("get max freq failed\n");
		goto out_default;
	}

	if (of_property_read_u32(np, "min_freq", &e_cpufreq.min_freq)) {
		PRT_ERR("get min freq failed\n");
		goto out_default;
	}

	if (e_cpufreq.min_freq > e_cpufreq.max_freq) {
		PRT_ERR("min freq(%d) > max freq(%d)\n",
				e_cpufreq.min_freq, e_cpufreq.max_freq);
		goto out_default;
	}

	if (e_cpufreq.max_freq > LOMBO_CPUFREQ_MAX) {
		PRT_ERR("max freq(%d) > %d\n",
				e_cpufreq.max_freq, LOMBO_CPUFREQ_MAX);
		goto out_default;
	}

	if (e_cpufreq.max_freq < LOMBO_CPUFREQ_MIN) {
		PRT_ERR("max freq(%d) < %d\n",
				e_cpufreq.max_freq, LOMBO_CPUFREQ_MIN);
		goto out_default;
	}

	if (e_cpufreq.min_freq > LOMBO_CPUFREQ_MAX) {
		PRT_ERR("min freq(%d) > %d\n",
				e_cpufreq.min_freq, LOMBO_CPUFREQ_MAX);
		goto out_default;
	}

	if (e_cpufreq.min_freq < LOMBO_CPUFREQ_MIN) {
		PRT_ERR("min freq(%d) < %d\n",
				e_cpufreq.min_freq, LOMBO_CPUFREQ_MIN);
		goto out_default;
	}

	/* get valid max/min frequency */
	e_cpufreq.max_freq = lombo_cpufreq_get_valid_freq(e_cpufreq.max_freq);
	e_cpufreq.min_freq = lombo_cpufreq_get_valid_freq(e_cpufreq.min_freq);

	PRT_DBG("max: %uKHz, min: %uKHz\n",
				e_cpufreq.max_freq, e_cpufreq.min_freq);

	goto out;

out_default:
	/* use default */
	e_cpufreq.max_freq = LOMBO_CPUFREQ_MAX;
	e_cpufreq.min_freq = LOMBO_CPUFREQ_MIN;
out:
	return err;
}

/**
 * lombo_cpufreq_initcall - init cpufreq driver
 *
 * Return 0 if success, 0! error.
 */
static int __init lombo_cpufreq_initcall(void)
{
	struct device_node *np;
	const struct property *prop;
	struct cpufreq_frequency_table *freq_table;
	const __be32 *val;
	int err = 0, cnt, i;

	np = of_find_node_by_path("/cpus/cpu@0");
	if (!np) {
		PRT_ERR("no cpu node found\n");
		return -ENODEV;
	}

	if (of_property_read_u32(np, "clock-latency",
				&e_cpufreq.transition_latency))
		e_cpufreq.transition_latency = CPUFREQ_ETERNAL;

	prop = of_find_property(np, "cpufreq_table", NULL);
	if (!prop || !prop->value) {
		PRT_ERR("invalid cpufreq_table in dts\n");
		err = -ENODEV;
		goto out_node_put;
	}

	cnt = prop->length / sizeof(u32);
	freq_table = kzalloc(sizeof(*freq_table) * (cnt + 1), GFP_KERNEL);
	if (!freq_table) {
		err = -ENOMEM;
		goto out_node_put;
	}

	val = prop->value;
	for (i = 0; i < cnt; i++) {
		freq_table[i].index = i;
		freq_table[i].frequency = be32_to_cpup(val++);
	}
	freq_table[i].index = i;
	freq_table[i].frequency = CPUFREQ_TABLE_END;
	e_cpufreq.freq_table = freq_table;

	err = lombo_cpufreq_range_select();
	if (err)
		goto out_kfree;

#ifndef CONFIG_LOMBO_CPU_ADPLL
	/* get cpu module clk */
	e_cpufreq.clk_cpu = of_clk_get(np, 0);
	if (IS_ERR_OR_NULL(e_cpufreq.clk_cpu)) {
		PRT_ERR("unable to get PLL CPU clock\n");
		err = PTR_ERR(e_cpufreq.clk_cpu);
		goto out_kfree;
	}

	/* get pll cpu clk */
	e_cpufreq.clk_pll = of_clk_get(np, 1);
	if (IS_ERR_OR_NULL(e_cpufreq.clk_pll)) {
		PRT_ERR("unable to get CPU clock\n");
		err = PTR_ERR(e_cpufreq.clk_pll);
		goto out_clk_cpu_put;
	}

	/* set cpu parent clk */
	err = clk_set_parent(e_cpufreq.clk_cpu, e_cpufreq.clk_pll);
	if (err) {
		PRT_ERR("set cpu parent clk failed\n");
		goto out_clk_pll_put;
	}

	/* enable cpu clk */
	err = clk_prepare_enable(e_cpufreq.clk_cpu);
	if (err) {
		PRT_ERR("enable cpu clock failed\n");
		goto out_clk_pll_put;
	}
#endif

	/* register cpufreq driver to kernel */
	err = cpufreq_register_driver(&lombo_cpufreq_driver);
	if (err) {
		PRT_ERR("register cpufreq driver failed\n");
		goto out_clk_pll_put;
	}

	PRT_DBG("cpufreq driver registered\n");

	goto out_node_put;

out_clk_pll_put:
#ifndef CONFIG_LOMBO_CPU_ADPLL
	clk_put(e_cpufreq.clk_pll);
out_clk_cpu_put:
	clk_put(e_cpufreq.clk_cpu);
#endif
out_kfree:
	kfree(freq_table);
out_node_put:
	of_node_put(np);

	return err;
}
fs_initcall(lombo_cpufreq_initcall);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Cpufreq Driver for Lombo SoCs");
MODULE_LICENSE("GPL");

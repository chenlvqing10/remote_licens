/*
 * drivers/devfreq/lombo_dramfreq.c
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Driver of LomboTech DRAM frequency
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define DEBUG

#include <asm/tlbflush.h>
#include <asm/cacheflush.h>
#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/devfreq.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/suspend.h>
#include <mach/debug.h>
#include <mach/memory.h>
#include <mach/common.h>
#include <mach/dramfreq.h>
#include "dfs/lombo_dram_dfs.h"

#undef MOD_NAME
#define MOD_NAME "DRAMFREQ"

#define LOMBO_DRAMFREQ_MDFS_RETRIES (3)
#define LOMBO_DRAMFREQ_MAX_STATE    (2)

/**
 * struct lombo_dramfreq - Runtime info holder for SPI driver.
 * @max: max frequency for dram scaling, based on KHz.
 * @min: min frequency for dram scaling, based on KHz.
 * @clk_dram_pll: pointer to pll dram clock.
 * @clk_dram: pointer to dram module clock.
 * @lock: mutex lock for sync.
 */
struct lombo_dramfreq {
	unsigned int max;
	unsigned int min;

	struct devfreq *devfreq;

#if 0
	void __iomem *dramcom_base;
	void __iomem *dramctl_base;
#endif

	struct clk *clk_dram_pll;
	struct clk *clk_dram;

	struct mutex lock;
};

/**
 * dram frequency table.
 */
static unsigned int lombo_dramfreq_table[LOMBO_DRAMFREQ_MAX_STATE] = {
	672000,
	336000,
};

#ifdef CONFIG_ARCH_LOMBO_N7V1
static uint32_t dram_num;
#endif

#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
struct dramfreq_vb_time_ops dramfreq_vbtime_ops;

int dramfreq_set_vb_time_ops(struct dramfreq_vb_time_ops *ops)
{
	dramfreq_vbtime_ops.get_vb_time = ops->get_vb_time;
	dramfreq_vbtime_ops.get_next_vb_time = ops->get_next_vb_time;
	dramfreq_vbtime_ops.is_in_vb = ops->is_in_vb;

	return 0;
}
EXPORT_SYMBOL(dramfreq_set_vb_time_ops);
#endif

#ifdef CONFIG_SMP
struct cpumask dramfreq_ipi_mask;
static bool cpu_pause[NR_CPUS];
static bool pause_flag;
static bool dramfreq_is_paused(void)
{
	smp_rmb();
	return pause_flag;
}
static void dramfreq_set_pause(bool pause)
{
	pause_flag = pause;
	smp_wmb();
}
static bool dramfreq_cpu_is_paused(unsigned int cpu)
{
	smp_rmb();
	return cpu_pause[cpu];
}
static void dramfreq_cpu_set_pause(unsigned int cpu, bool pause)
{
	cpu_pause[cpu] = pause;
	smp_wmb();
}
static void dramfreq_cpu_pause(void *info)
{
	unsigned int cpu = raw_smp_processor_id();

	dsb();
	isb();

	dramfreq_cpu_set_pause(cpu, true);
	while (dramfreq_is_paused())
		;
	dramfreq_cpu_set_pause(cpu, false);
}
static void dramfreq_cpu_wait(void *info) {}
#endif /* CONFIG_SMP */

/**
 * lombo_dramfreq_dfs - set frequency to hw in dfs.
 * @dramfreq: pointer to lombo_dramfreq struct.
 * @freq: target freq to be set.
 *
 * Return 0 if valid, !0 error.
 */
static int lombo_dramfreq_dfs(struct lombo_dramfreq *dramfreq,
				unsigned int freq)
{
	int err = 0;
	u32 div = dramfreq->max / freq;
	ddr_dfs_para_t ddr_dfs_para;
	int (*dramfreq_target_entry)(u32 div, ddr_dfs_para_t *para);

	memset(&ddr_dfs_para, 0, sizeof(ddr_dfs_para_t));

	dramfreq_target_entry = (int (*)(u32 div, ddr_dfs_para_t *para))LOMBO_DRAMFREQ_START;
	ddr_dfs_para.freq = dramfreq->max / 1000;
	ddr_dfs_para.rankn = RNKO0;
	ddr_dfs_para.ddr_type = DDR3;
	ddr_dfs_para.size = lombo_get_sdram_capacity();

	memcpy((void *)LOMBO_DRAMFREQ_START, (void *)&dfs_bin_start,
				(int)&dfs_bin_end - (int)&dfs_bin_start);

	flush_tlb_all();
	flush_cache_all();
	dmb();
	dsb();
	isb();

	err = dramfreq_target_entry(div, &ddr_dfs_para);
	if (err) {
		PRT_ERR("dram freq target entry failed, ret=%d\n", err);
		err = __LINE__;
	}

	return err;
}

/**
 * lombo_dramfreq_get_cur_freq - get the current dram frequency.
 * @dev: pointer to dramfreq device.
 * @freq: pointer to the frequency which to be get.
 *
 * Return the current dram frequency.
 */
static int lombo_dramfreq_get_cur_freq(struct device *dev, unsigned long *freq)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct lombo_dramfreq *dramfreq = platform_get_drvdata(pdev);
	u32 dram_rate = clk_get_rate(dramfreq->clk_dram) / 1000;

#ifdef CONFIG_ARCH_LOMBO_N7V1
	if (2 == dram_num)
		*freq = dram_rate / 2;
	else
		*freq = dram_rate * 2;
#else
	*freq = dram_rate / 2;
#endif

	return 0;
}

/**
 * lombo_dramfreq_frequency_table_target - get target frequency from the table.
 * @freq: frequency to be check.
 *
 * Return the target dram frequency.
 */
static int lombo_dramfreq_frequency_table_target(unsigned long freq)
{
	unsigned int *valid_freq = lombo_dramfreq_table;
	int index = 0;
	int size = ARRAY_SIZE(lombo_dramfreq_table) - 1;

	while ((*(valid_freq + 1) >= freq) && (index < size)) {
		valid_freq++;
		index++;
	}

	return *valid_freq;
}

/**
 * lombo_dramfreq_set_rate - set dram rate to target frequency.
 * @dramfreq: pointer to lombo_dramfreq struct.
 * @freq_target: target frequency to be set.
 *
 * Return the target dram frequency.
 */
static int lombo_dramfreq_set_rate(struct lombo_dramfreq *dramfreq,
				unsigned int freq_target)
{
	int err = 0;
	s64 dramfreq_time_us = 0;
	ktime_t calltime;
	unsigned int cur_cpu = 0;
#ifdef CONFIG_SMP
	unsigned int cpu, timeout = 0;

	cpumask_clear(&dramfreq_ipi_mask);
	cpumask_copy(&dramfreq_ipi_mask, cpu_online_mask);
	cur_cpu = raw_smp_processor_id();
	cpumask_clear_cpu(cur_cpu, &dramfreq_ipi_mask);

	local_bh_disable();
	dramfreq_set_pause(true);

	preempt_disable();
	smp_call_function_many(&dramfreq_ipi_mask,
				dramfreq_cpu_pause, NULL, false);
	preempt_enable();

	dsb();
	isb();

	for_each_online_cpu(cpu) {
		if (cpu == cur_cpu)
			continue;

		while (!dramfreq_cpu_is_paused(cpu) && timeout < 100) {
			udelay(100);
			timeout++;
		}

		if (timeout >= 100) {
			PRT_ERR("pause cpu%d time out!\n", cpu);
			err = -EAGAIN;
			goto out;
		}
	}
#endif

	PRT_DBG("on CPU%d\n", cur_cpu);

	local_irq_disable();

#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
	while (!dramfreq_vbtime_ops.is_in_vb())
		;
#endif

	calltime = ktime_get();

	err = lombo_dramfreq_dfs(dramfreq, freq_target);
	if (err) {
		PRT_ERR("dram freq dfs failed, ret=%d\n", err);
		err = __LINE__;
		local_irq_enable();
		goto out;
	}

	dramfreq_time_us = ktime_to_us(ktime_sub(ktime_get(), calltime));
	local_irq_enable();

	PRT_DBG("elapsed time: %lldus on CPU%d\n", dramfreq_time_us, cur_cpu);

out:
#ifdef CONFIG_SMP
	dramfreq_set_pause(false);
	local_bh_enable();
	preempt_disable();
	smp_call_function_many(&dramfreq_ipi_mask,
				dramfreq_cpu_wait, NULL, true);
	preempt_enable();
#endif

	return err;
}

/**
 * lombo_dramfreq_target - set target frequency to dram.
 * @dev: pointer to dramfreq device.
 * @freq: pointer to target frequency.
 * @flags: rule of frequency selection.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_dramfreq_target(struct device *dev,
				unsigned long *freq, u32 flags)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct lombo_dramfreq *dramfreq = platform_get_drvdata(pdev);
	int err = 0, retries = 0;
#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
	int next_vbtime_us = 0, next_vb_retries = 0;
#endif
	unsigned long cur_freq, target_freq;
#ifdef CONFIG_CPU_FREQ
	struct cpufreq_policy policy;
#endif

	mutex_lock(&dramfreq->lock);
	get_online_cpus();

	target_freq = lombo_dramfreq_frequency_table_target(*freq);
	if (target_freq == dramfreq->devfreq->previous_freq) {
		if (*freq != target_freq)
			*freq = target_freq;
		goto unlock;
	}

	PRT_DBG("request frequency: %luKHz\n", target_freq);

#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
	if (!(dramfreq_vbtime_ops.get_next_vb_time
				&& dramfreq_vbtime_ops.is_in_vb)) {
		PRT_ERR("dramfreq_vbtime_ops is not initialized!\n");
		err = -EINVAL;
		goto unlock;
	}

	do {
		next_vbtime_us = dramfreq_vbtime_ops.get_next_vb_time();
		if (next_vbtime_us < DRAMFREQ_LEAST_TIME_US) {
			next_vb_retries++;
			usleep_range(1000, 1050);
		} else {
			goto prepare;
		}
	} while (next_vb_retries < DRAMFREQ_RETRIES_CNT);

	PRT_ERR("get next vb time failed\n");
	err = -EINVAL;
	goto unlock;

prepare:
	usleep_range(next_vbtime_us - DRAMFREQ_LEAST_TIME_US,
				next_vbtime_us - DRAMFREQ_LEAST_TIME_US);
#endif

#ifdef CONFIG_CPU_FREQ
	if ((!cpufreq_get_policy(&policy, 0)) && (policy.cur < policy.max))
		__cpufreq_driver_target(&policy, policy.max,
						CPUFREQ_RELATION_H);
#endif

	do {
		err = lombo_dramfreq_set_rate(dramfreq, target_freq);
		if (err == -EAGAIN)
			retries++;
		else
			goto success;
	} while (retries < LOMBO_DRAMFREQ_MDFS_RETRIES);

	PRT_ERR("fail to set dram freq\n");
	goto unlock;

success:
	lombo_dramfreq_get_cur_freq(dev, &cur_freq);
	if (cur_freq != target_freq) {
		PRT_ERR("current freq is %lu != %lu\n", cur_freq, target_freq);
		goto unlock;
	}

	if (*freq != cur_freq)
		*freq = cur_freq;

	PRT_DBG("set frequency: %luKHz success\n", *freq);

unlock:
	put_online_cpus();
	mutex_unlock(&dramfreq->lock);

	return err;
}

static struct devfreq_dev_profile lombo_dramfreq_profile = {
	.get_cur_freq = lombo_dramfreq_get_cur_freq,
	.target       = lombo_dramfreq_target,
	.freq_table   = lombo_dramfreq_table,
	.max_state    = LOMBO_DRAMFREQ_MAX_STATE,
	.initial_freq = 672000,
};

/**
 * lombo_dramfreq_probe - probe function of dramfreq.
 * @pdev: pointer to dramfreq platform device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_dramfreq_probe(struct platform_device *pdev)
{
	struct lombo_dramfreq *dramfreq;
	int i, err = 0;
#ifdef CONFIG_ARCH_LOMBO_N7V1
	uint32_t p1 = 0, p2 = 0;

	if (lombo_func1())
		return -EINVAL;

	if (lombo_func2(&p1, &p2))
		return -EINVAL;

	dram_num = p1;
#endif

	dev_set_name(&pdev->dev, "lombo_dramfreq");

	dramfreq = kzalloc(sizeof(struct lombo_dramfreq), GFP_KERNEL);
	if (dramfreq == NULL) {
		PRT_ERR("fail to allocate memory\n");
		err = -ENOMEM;
		goto out;
	}

	for (i = 0; i < LOMBO_DRAMFREQ_MAX_STATE; i++) {
		err = opp_add(&pdev->dev, lombo_dramfreq_table[i], 0);
		if (err) {
			PRT_ERR("cannot add opp entries[%d]=%d, ret=%d\n",
				i, lombo_dramfreq_table[i], err);
			goto out_kfree;
		}
	}

	dramfreq->max = lombo_dramfreq_table[0];
	dramfreq->min = lombo_dramfreq_table[LOMBO_DRAMFREQ_MAX_STATE - 1];

#if 0
	dramfreq->dramcom_base = of_iomap(pdev->dev.of_node, 0);
	if (!dramfreq->dramcom_base) {
		PRT_ERR("Map dramcom_base failed!\n");
		err = -EBUSY;
		goto out;
	}

	dramfreq->dramctl_base = of_iomap(pdev->dev.of_node, 1);
	if (!dramfreq->dramctl_base) {
		PRT_ERR("Map dramctl_base failed!\n");
		err = -EBUSY;
		goto out;
	}

	dramfreq->ccu_base = of_iomap(pdev->dev.of_node, 2);
	if (!dramfreq->ccu_base) {
		PRT_ERR("Map ccu_base failed!\n");
		err = -EBUSY;
		goto out;
	}
#endif

	/* get dram module clk */
	dramfreq->clk_dram = of_clk_get(pdev->dev.of_node, 0);
	if (IS_ERR_OR_NULL(dramfreq->clk_dram)) {
		PRT_ERR("unable to get DRAM clock\n");
		err = PTR_ERR(dramfreq->clk_dram);
		goto out_kfree;
	}

	/* get pll dram clk */
	dramfreq->clk_dram_pll = of_clk_get(pdev->dev.of_node, 1);
	if (IS_ERR_OR_NULL(dramfreq->clk_dram_pll)) {
		PRT_ERR("unable to get PLL DRAM clock\n");
		err = PTR_ERR(dramfreq->clk_dram_pll);
		goto out_clk_cpu_put;
	}

	/* set dram parent clk */
	err = clk_set_parent(dramfreq->clk_dram, dramfreq->clk_dram_pll);
	if (err) {
		PRT_ERR("set dram parent clk failed\n");
		goto out_clk_dram_pll_put;
	}

	/* enable dram clk */
	err = clk_prepare_enable(dramfreq->clk_dram);
	if (err) {
		PRT_ERR("enable dram clock failed\n");
		goto out_clk_dram_pll_put;
	}

#if 0
	lombo_dramfreq_get_cur_freq(&pdev->dev,
				&lombo_dramfreq_profile.initial_freq);
#endif

	dramfreq->devfreq = devfreq_add_device(&pdev->dev,
				&lombo_dramfreq_profile, "userspace", NULL);
	if (IS_ERR(dramfreq->devfreq)) {
		PRT_ERR("fail to add devfreq device\n");
		err = PTR_ERR(dramfreq->devfreq);
		goto out_clk_dram_pll_put;
	}

	dramfreq->devfreq->max_freq = dramfreq->max;
	dramfreq->devfreq->min_freq = dramfreq->min;

	platform_set_drvdata(pdev, dramfreq);
	mutex_init(&dramfreq->lock);

	PRT_DBG("dramfreq driver registered\n");

	return 0;

out_clk_dram_pll_put:
	clk_put(dramfreq->clk_dram_pll);
out_clk_cpu_put:
	clk_put(dramfreq->clk_dram);
out_kfree:
	kfree(dramfreq);
out:
	return err;
}

/**
 * lombo_dramfreq_remove - remove function of dramfreq.
 * @pdev: pointer to dramfreq platform device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_dramfreq_remove(struct platform_device *pdev)
{
	struct lombo_dramfreq *dramfreq = platform_get_drvdata(pdev);

	mutex_destroy(&dramfreq->lock);
	devfreq_remove_device(dramfreq->devfreq);
	clk_put(dramfreq->clk_dram);
	clk_put(dramfreq->clk_dram_pll);

	kfree(dramfreq);

	return 0;
}

#ifdef CONFIG_PM
/**
 * lombo_dramfreq_suspend - suspend function of dramfreq.
 * @pdev: pointer to dramfreq platform device.
 * @state: pm message.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_dramfreq_suspend(struct platform_device *pdev,
				pm_message_t state)
{
	return 0;
}

/**
 * lombo_dramfreq_resume - resume function of dramfreq.
 * @pdev: pointer to dramfreq platform device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_dramfreq_resume(struct platform_device *pdev)
{
	return 0;
}
#endif

static const struct of_device_id lombo_dramfreq_match[] = {
	{ .compatible = "lombo,n7-dramfreq", },
	{},
};

static struct platform_driver lombo_dramfreq_driver = {
	.driver = {
		.name  = "lombo-dramfreq",
		.owner = THIS_MODULE,
		.of_match_table = lombo_dramfreq_match,
	},
	.probe  = lombo_dramfreq_probe,
	.remove = lombo_dramfreq_remove,
#ifdef CONFIG_PM
	.suspend = lombo_dramfreq_suspend,
	.resume  = lombo_dramfreq_resume,
#endif
};

/**
 * lombo_dramfreq_initcall - init dramfreq driver
 *
 * Return 0 if success, 0! error.
 */
static int __init lombo_dramfreq_initcall(void)
{
	int err = 0;

	err = platform_driver_register(&lombo_dramfreq_driver);
	if (err)
		PRT_ERR("register dramfreq driver failed!\n");

	return err;
}
late_initcall(lombo_dramfreq_initcall);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("DRAM Frequency Driver for Lombo SoCs");
MODULE_LICENSE("GPL");

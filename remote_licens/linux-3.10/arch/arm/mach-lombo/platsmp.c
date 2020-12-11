/*
 * platsmp.c - code for smp platform
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

#include <linux/sched.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <mach/csp.h>
#include "lombo.h"

static DEFINE_SPINLOCK(boot_lock);

/**
 * phys_cpu - Check if the dying cpu is in wfi state
 * @phys_cpu: the cpu's physic index
 *
 * Called by main cpu
 *
 * Return 1 if the dying cpu is in wfi, 0 otherwise
 */
bool cpu_is_in_wfi(u32 phys_cpu)
{
	u32 cpu_status, cmp;

	cpu_status = readl((void *)VA_CPU_CTRL_STATUS_R);
	WARN_ON(phys_cpu > 1);
	cmp = (phys_cpu == 0 ? BIT(16) : BIT(17));
	return !!(cpu_status & cmp);
}

/**
 * get_cpu_boot_reg - get the specific cpu's boot reg vaddr
 * @phys_cpu: the physic cpu to be got
 *
 * return the boot reg vaddr of phys_cpu
 */
u32 get_cpu_boot_reg(u32 phys_cpu)
{
	return VA_RTC_HOT_RST_EXIT;
}

/**
 * get_cpu_core_cnt - get cpu core count of the system
 *
 * return the cpu core count
 */
u32 get_cpu_core_cnt(void)
{
	reg_gicd_typer_t reg;

	reg.val = readl((void *)VA_GICD_TYPER);
	return reg.bits.cpunumber + 1;
}

/**
 * cpu_on - open a cpu
 * @phys_cpu: the physic cpu to be opened
 */
void cpu_on(u32 phys_cpu)
{
	u32 val;

	/* cpu clk gating on */
	val = readl((void *)VA_CPU_CTRL_CTRL_R);
	val |= (1 << (phys_cpu + 16));
	writel(val, (void *)VA_CPU_CTRL_CTRL_R);

	/* cpu reset de-asserted */
	val = readl((void *)VA_CPU_CTRL_RST_CTRL);
	val |= (1 << phys_cpu);
	writel(val, (void *)VA_CPU_CTRL_RST_CTRL);
}

/**
 * cpu_off - close a cpu
 * @phys_cpu: the physic cpu to be closed
 */
void cpu_off(u32 phys_cpu)
{
	u32 val;
	/* cpu reset asserted */
	val = readl((void *)VA_CPU_CTRL_RST_CTRL);
	val &= ~(1 << phys_cpu);
	writel(val, (void *)VA_CPU_CTRL_RST_CTRL);

	/* cpu clk gating off */
	val = readl((void *)VA_CPU_CTRL_CTRL_R);
	val &= ~(1 << (phys_cpu + 16));
	writel(val, (void *)VA_CPU_CTRL_CTRL_R);
}

/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not.  This is necessary for the hotplug code to work reliably.
 */
void write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

/**
 * lombo_smp_init_cpus - Setup the set of possible CPUs (via set_cpu_possible)
 *
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
void __init lombo_smp_init_cpus(void)
{
	u32 i, cpu_cnt;

	PRT_INFO("enter\n");

	cpu_cnt = get_cpu_core_cnt();
	if (cpu_cnt > nr_cpu_ids) {
		PRT_WARN("cpu_cnt(%d) greater than maximum(%d)\n",
			cpu_cnt, nr_cpu_ids);
		cpu_cnt = nr_cpu_ids;
	}
	PRT_INFO("cpu_cnt %d\n", cpu_cnt);

	for (i = 0; i < cpu_cnt; i++)
		set_cpu_possible(i, true);

	PRT_INFO("end\n");
}

/**
 * lombo_smp_prepare_cpus - Initialize cpu_possible map, and enable coherency
 * @max_cpus: the max cpu number. Initialize to NR_CPUS, and it can be
 *            modified by "maxcpus" early params
 */
void __init lombo_smp_prepare_cpus(unsigned int max_cpus)
{
	u32 boot_addr;

	PRT_INFO("enter\n");

	boot_addr = virt_to_phys(lombo_secondary_startup);

	/* set the cpu boot entry */
	rtc_set_hot_rst_exit(boot_addr);

	PRT_INFO("end\n");
}

/**
 * lombo_secondary_init - initialize a secondary CPU
 * @cpu: the cpu index
 *
 * Perform platform specific initialisation of the specified CPU.
 */
void __cpuinit lombo_secondary_init(unsigned int cpu)
{
	u32 val;

	PRT_INFO("enter\n");

	/* enable the corresponding cpu interface */
	val = readl((void *)VA_GICC_CTLR);
	val |= (1 << 0);
	writel(val, (void *)VA_GICC_CTLR);

	val = readl((void *)VA_GICC_CTLR);
	val |= (1 << 1);
	writel(val, (void *)VA_GICC_CTLR);

	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	write_pen_release(-1);

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);

	PRT_INFO("end\n");
}

/**
 * lombo_boot_secondary - Boot a secondary CPU
 * @cpu: the cpu index
 * @idle: the idle task's handle
 *
 * Boot a secondary CPU, and assign it the specified idle task. This also
 * gives us the initial stack to use for this CPU.
 *
 * Return 0 if success, <0 otherwise
 */
int __cpuinit lombo_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;
	u32 phys_cpu;

	PRT_INFO("enter\n");

	phys_cpu = cpu_logical_map(cpu);

	/*
	 * Set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 *
	 * Note that "pen_release" is the hardware CPU ID, whereas
	 * "cpu" is Linux's internal ID.
	 */
	write_pen_release(phys_cpu);

	cpu_on(phys_cpu);

	/*
	 * wait phys_cpu boot ok, that is, the booted cpu run
	 * lombo_secondary_init to relese the pen
	 */
	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1) /* wait the pen be released */
			break;

		udelay(10);
	}
	if (pen_release != -1)
		BUG();

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	PRT_INFO("end\n");
	return pen_release != -1 ? -ENOSYS : 0;
}

#ifdef CONFIG_HOTPLUG_CPU
/**
 * lombo_cpu_die - platform-specific code to shutdown a CPU
 * @cpu: the cpu index
 *
 * This fountion run on cpu that is being killed, and called
 * with IRQs disabled. So never printk in this function, or
 * may lead to dead lock
 */
void __ref lombo_cpu_die(unsigned int cpu)
{
	u32 val;

	/* PRT_INFO("enter\n"); */
	/* disable the corresponding cpu interface */
	val = readl((void *)VA_GICC_CTLR);
	val &= ~(1 << 0);
	writel(val, (void *)VA_GICC_CTLR);

	val = readl((void *)VA_GICC_CTLR);
	val &= ~(1 << 1);
	writel(val, (void *)VA_GICC_CTLR);

	/* enter low power state */
	for (;;) {
		__asm__ __volatile__("dsb\n\t" "wfi\n\t"
				: : : "memory");

		/*
		 * the programe is already wrong, so give the prompt, in despite
		 * of the possible deadlock
		 */
		PRT_ERR("cpu %d whose irq disabed wakeup by others\n",
			cpu_logical_map(cpu));

		/*
		 * noramlly the wfi will never return. but when other cpus call
		 * cpu_up(this cpu) before lombo_cpu_kill called(by other cpu),
		 * the wfi will return. anyway, it will jump to
		 * secondary_start_kernel correctly by cpu_die()
		 */
		if (pen_release == cpu_logical_map(cpu)) {
			WARN(1, "cpu%d is not died properly\n",
				cpu_logical_map(cpu));
			break;
		}
	}

	/* PRT_INFO("end\n"); */
}

/**
 * lombo_cpu_kill - power down the cpu
 * @cpu: the cpu index to be powered down
 *
 * this fountion run on cpu that kill the other cpus,
 * not on cpu that is being killed
 *
 * return 1 if success, 0 if failed
 */
int lombo_cpu_kill(unsigned int cpu)
{
	u32 phys_cpu;

	PRT_INFO("enter\n");

	phys_cpu = cpu_logical_map(cpu);

	/*
	 * wait the dying cpu enter wfi, if not so, the cpu may
	 * not boot normally next time
	 */
	while (!cpu_is_in_wfi(phys_cpu))
		udelay(10);

	/* power down the cpu */
	cpu_off(phys_cpu);

	PRT_INFO("end\n");
	return 1;
}

/**
 * lombo_cpu_disable - disable the cpu
 * @cpu: the cpu index
 *
 * this fountion run on cpu that is being killed, and it
 * is usually null
 *
 * return -EPERM for cpu0, 0 for other cpus
 */
int lombo_cpu_disable(unsigned int cpu)
{
	PRT_INFO("enter\n");

	if (0 == cpu)
		return -EPERM;

	PRT_INFO("end\n");
	return 0;
}
#endif

/* smp operations for lombo platform */
struct smp_operations lombo_smp_ops __initdata = {
	.smp_init_cpus		= lombo_smp_init_cpus,
	.smp_prepare_cpus	= lombo_smp_prepare_cpus,
	.smp_secondary_init	= lombo_secondary_init,
	.smp_boot_secondary	= lombo_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_kill		= lombo_cpu_kill,
	.cpu_die		= lombo_cpu_die,
	.cpu_disable		= lombo_cpu_disable,
#endif
};


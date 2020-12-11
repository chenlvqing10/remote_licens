/*
 * core_sync.c - SMP cpu sync operations
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
#include <linux/spinlock.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <mach/csp.h>
#include <mach/debug.h>
#include <mach/core_sync.h>

/*
 * core sync functions.
 * during the sync procedure, the irq and scheduler was disabled for all cores
 *
 * so the procedure was like this:
 *      core-a                        core-b
 *  ------------------------   ------------------------
 *  core_sync_1_start
 *     ..(wait 2_start over)         ...
 *     ..(wait 2_start over)     core_sync_2_start
 *     ..(do)                        ..(do)
 *     ...                           ...
 *     ...                       core_sync_2_end
 *     ...                           ..(wait 1_end start)
 *  core_sync_1_end                  ..(wait 1_end start)
 *     ..(wait 2_end over)           ..(do)
 *     ..(wait 2_end over)           ..(do)
 *     ..(do)                        ...
 *
 * in n7 platform, when one core write dma regs, the other core should
 * be hung (do nothing).
 */
enum {
	SYNC_PHASE_1 = 1,	/* initial */
	SYNC_PHASE_2 = 2,
	SYNC_PHASE_3 = 3,
	SYNC_PHASE_4 = 4,
};

int sync_phase = SYNC_PHASE_1;

DEFINE_SPINLOCK(core_sync);

/**
 * gic_send_sgi - send a SGI to another CPU
 * @cpu_id: The cpu id. must < CONFIG_NR_CPUS
 * @irq: the irq id to generate
 *
 * cpu_id: the ID for the destination CPU interface
 * irq: the IPI number to send a SGI for
 */
void gic_send_sgi(unsigned int cpu_id, unsigned int irq)
{
	BUG_ON(cpu_id >= CONFIG_NR_CPUS);

	cpu_id = 1 << cpu_id;

	/* this always happens on GIC0 */
	writel_relaxed((cpu_id << 16) | irq, (volatile void *)VA_GICD_SGIR);
}

u32 core_sync_1_start(void)
{
	unsigned long flags = 0;
	u32 cpu_id;

	while (!spin_trylock_irqsave(&core_sync, flags)) {
		cpu_relax();
		/* make sure irq was enabled, or it cannot ack IPI interrupt */
		if (flags & PSR_I_BIT) {
			PRT_ERR("fatal err: local irq disabled!");
			dump_stack();
			BUG_ON(1);
		}
	}

	BUG_ON(SYNC_PHASE_1 != sync_phase);

	if (2 != CONFIG_NR_CPUS) /* only support in 2 cpus platform */
		goto end;

	/* donot send ipi if other cpu was not ready */
	cpu_id = !smp_processor_id(); /* must after spinlock acuqired */
	if (!cpu_online(cpu_id))
		goto end; /* sync_phase is SYNC_PHASE_1 */

	sync_phase = SYNC_PHASE_2;

	/* wait sync_phase completed on bus */
	/* dsb(); the dsb will slower the performance */

	gic_send_sgi(cpu_id, IPI_CORE_SYNC);

	/* wait the other core to start */
	while (SYNC_PHASE_3 != sync_phase) {
		cpu_relax();
		if (!cpu_online(cpu_id)) /* recheck */
			break;
	}

end:
	return flags;
}

void core_sync_1_end(u32 flags)
{
	u32 cpu_id = !smp_processor_id();

	if (2 != CONFIG_NR_CPUS) /* only support in 2 cpus platform */
		goto end;

	/* if core_sync_1_start has not send IPI, or the other core is dying */
	if (SYNC_PHASE_1 == sync_phase || !cpu_online(cpu_id))
		goto end;

	BUG_ON(SYNC_PHASE_3 != sync_phase);

	sync_phase = SYNC_PHASE_4;

	/* wait the other core to finish */
	while (SYNC_PHASE_1 != sync_phase) {
		cpu_relax();
		if (!cpu_online(cpu_id))
			break;
	}

end:
	/* reset to initial state */
	sync_phase = SYNC_PHASE_1;

	spin_unlock_irqrestore(&core_sync, flags);
}

u32 core_sync_2_start(void)
{
	BUG_ON(2 != CONFIG_NR_CPUS);
	BUG_ON(!cpu_online(!smp_processor_id()));

	/* the dsb will slower the performance, so use quering here */
	/* BUG_ON(SYNC_PHASE_2 == sync_phase); */
	while (SYNC_PHASE_2 != sync_phase)
		cpu_relax();

	sync_phase = SYNC_PHASE_3;
	return 0;
}

void core_sync_2_end(u32 flags)
{
	BUG_ON(2 != CONFIG_NR_CPUS);
	BUG_ON(!cpu_online(!smp_processor_id()));

	/* wait the main core to finish */
	while (SYNC_PHASE_4 != sync_phase)
		cpu_relax();

	sync_phase = SYNC_PHASE_1;
}


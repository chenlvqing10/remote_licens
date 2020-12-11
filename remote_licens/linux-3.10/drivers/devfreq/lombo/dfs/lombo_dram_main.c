/*
 * drivers/devfreq/lombo_dram_entry.c
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

#include <mach/memory.h>
#include "lombo_dram_printk.h"
#include "lombo_dram_dfs.h"

unsigned int __sram_div;
ddr_dfs_para_t __sram_ddr_dfs_para;

void __dramfreq_load_tlb(unsigned int addr, unsigned int len)
{
	unsigned int tmp_value = 0;
	unsigned int tmp_addr  = addr;

	while (tmp_addr < (addr + len)) {
		/* access the mapping virtual addr of page table entry,
		 * it is use for load pte to TLB.
		 */
		tmp_value = *((unsigned int *)tmp_addr);

		/* access the next page table entry,
		 * the least pte size of A7 is 4 * 1024
		 */
		tmp_addr += 4 * 1024;
	}
}

void __dramfreq_init_tlb(void)
{
	/* load sram code space to tlb */
	__dramfreq_load_tlb(LOMBO_DRAMFREQ_START,
				LOMBO_DRAMFREQ_SP_ADDR - LOMBO_DRAMFREQ_START);

	/* load dram controller to tlb */
	__dramfreq_load_tlb(BASE_DRAMC    + 0xF0000000, 0x1000);
	__dramfreq_load_tlb(BASE_DRAMPUB  + 0xF0000000, 0x1000);
	__dramfreq_load_tlb(BASE_MEM_CTRL + 0xF0000000, 0x2000);

	/* load prcm to tlb */
	__dramfreq_load_tlb(BASE_PRCM + 0xF0000000, 0x1000);

#ifdef __SRAM_DRAMFREQ_DEBUG
	/* load uart0 to tlb */
	__dramfreq_load_tlb(UART_BASE, 0x1000);
#endif

	/* load gtimer1 to tlb */
	__dramfreq_load_tlb(BASE_GTIMER + 0xF0000000,   0x1000);
}

void __dramfreq_init_para(unsigned int div, ddr_dfs_para_t *ddr_dfs_para)
{
	__sram_div = div;
	__sram_ddr_dfs_para.freq = ddr_dfs_para->freq;
	__sram_ddr_dfs_para.rankn = ddr_dfs_para->rankn;
	__sram_ddr_dfs_para.ddr_type = ddr_dfs_para->ddr_type;
	__sram_ddr_dfs_para.size = ddr_dfs_para->size;
}

int dramfreq_target_main(unsigned int div, ddr_dfs_para_t *ddr_dfs_para)
{
	/* init tlb */
	__dramfreq_init_tlb();

	/* init para */
	__dramfreq_init_para(div, ddr_dfs_para);

	dramfreq_printk("%s:%d\n", __func__, __LINE__);

	asm volatile ("dsb");
	asm volatile ("isb");

	ddr_dfs(__sram_div, &__sram_ddr_dfs_para);

	dramfreq_printk("%s:%d\n", __func__, __LINE__);

	return 0;
}

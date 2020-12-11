/*
 * memory.h - memory space division for LomboTech Socs
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

#ifndef __ARCH_MEMORY_H
#define __ARCH_MEMORY_H

#include <linux/sizes.h>

#define LOMBO_IO_AHB_PBASE	0x01000000
#define LOMBO_IO_AHB_VBASE	0xF1000000
#define LOMBO_IO_AHB_SIZE	SZ_8M

#define LOMBO_IO_APB_PBASE	0x04000000
#define LOMBO_IO_APB_VBASE	0xF4000000
#define LOMBO_IO_APB_SIZE	SZ_1M

#define LOMBO_ION_CARVOUT_BASE	0x4BB00000
#define LOMBO_ION_CARVOUT_SIZE	SZ_64M

#define LOMBO_SRAM_PBASE	0x00800000
#define LOMBO_SRAM_VBASE	0xF0800000
#define LOMBO_SRAM_SIZE		SZ_32K

/*
 * sram area for deep sleep
 *
 * the LOMBO_SUSPEND/RESUME_START are parsed by arch/../deepsleep/suspend
 * |resume/Makefile, so the defination must begin with "#define", and the
 * comment(if exist) can only be in the end of line(not in middle).
 *
 * The right defination style:
 * "#define LOMBO_SUSPEND_START 0xF0804000"
 * "#define LOMBO_SUSPEND_START 0xF0804000 *comment*"
 *
 * The wrong defination style:
 * "#define LOMBO_SUSPEND_START	*comment* 0xF0804000"
 * "  #define LOMBO_SUSPEND_START 0xF0804000"
 *
 * for deep sleep (cpu power off in the end of suspend):
 *   mmu is on for suspend flow, so it use vaddr(0xFxxxxxx)
 *   mmu is off for resume flow, so it use paddr(0x0xxxxxx)
 *
 * for standby (cpu wfi in the end of suspend):
 *   mmu is on for suspend flow, so it use vaddr(0xFxxxxxx)
 *     (there is no seprate resume flow for standby)
 */
#define LOMBO_SUSPEND_START	0xF0800000	/* virt addr */
#define LOMBO_SUSPEND_SIZE	SZ_16K
#define SUSPEND_STACK_SIZE	SZ_4K
#define SUSPEND_STACK_START	(LOMBO_SUSPEND_START + \
				LOMBO_SUSPEND_SIZE - SUSPEND_STACK_SIZE)
#define LOMBO_RESUME_START	0x00808000	/* phys addr */
#define LOMBO_RESUME_SIZE	(SZ_4K + SZ_2K)

#define LOMBO_DRAMFREQ_START	0xF0804000
#define LOMBO_DRAMFREQ_SIZE	SZ_16K
#define LOMBO_DRAMFREQ_SP_ADDR	(LOMBO_DRAMFREQ_START + LOMBO_DRAMFREQ_SIZE)

/*
 * ddr para and timing, used only for standby
 *
 * they are used when ddr exit from self-refresh
 */
#define DDR_TIMING_START	(LOMBO_SRAM_VBASE + LOMBO_SRAM_SIZE - SZ_512)
#define DDR_TIMING_SIZE		SZ_512
#define DDR_PARA_START		(DDR_TIMING_START - SZ_512)
#define DDR_PARA_SIZE		SZ_512

#endif /* __ARCH_MEMORY_H */

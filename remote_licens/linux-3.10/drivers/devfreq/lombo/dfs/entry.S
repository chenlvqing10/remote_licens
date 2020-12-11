/*
 * entry.S - platform deep sleep entry
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

#include <linux/linkage.h>
#include <asm/assembler.h>
#include <mach/memory.h>

	.text
	.align	5

ENTRY(dramfreq_target_entry)
	@ for debug
	@ b	.

	@ save stack pointer in dram
	mov	r3, sp

	@ initialize the stack
	ldr	sp, =LOMBO_DRAMFREQ_SP_ADDR

	@ save lr reg which is in dram
	stmfd	sp!, {r3, lr}

	@ the lr orign is dram space, so correct it
	adr	lr, 1f

	@ flush the btac before ddr go to sr.
	@
	@ the btac contain 8 entry, each entry consist of compare addr(bit[11:1]) and
	@ target addr(32bit). for indirect branch instruction(eg: ldr pc, [pc, 8]), if
	@ compare addr hit with cur instruction, the target addr instuction will be
	@ prefetched out. the target addr maybe in dram, so when ddr enter sr, this
	@ prefetch behaviour lead to system hung.
	@
	@ the btac cannot be flushed by sw(DDI0464F, p25), but we can flush it as flow,
	@ after flush, the compare/target addr of the 8 entry are all in sram, so not
	@ cause system hung any more.
	b	btac_flush0
1:
	bl	btac_flush1
	bl	btac_flush2
	bl	btac_flush3
	bl	btac_flush4
	bl	btac_flush5
	bl	btac_flush6
	bl	btac_flush7
	bl	btac_flush8
	bl	btac_flush9
	bl	btac_flusha
	bl	btac_flushb
	bl	btac_flushc
	bl	btac_flushd
	bl	btac_flushe
	bl	btac_flushf

	/*
	 * set TTBR0 0 entry for 0 predict && backup pte 0
	 *
	 * the branch predict will access 0 addr(illegal addr, why it access?),
	 * which may lead to system hung. so change pte to make 0 addr valid
	 */
	@ ---------------------------------------------------------------------
	mrc	p15, 0, r3, c2, c0, 0
	ldr	r4, =#0xFFFFFC00
	and	r3, r3, r4
	sub	r3, r3, #0x40000000
	ldr	r4, =#0xC0000000
	add	r3, r3, r4
	ldr	r4, [r3]		@ backup pte 0
	push	{r3, r4}
	ldr	r4, =#0x00000C0A	@ change pte0 att
	str	r4, [r3]
	@ ---------------------------------------------------------------------

	@ invalidate tlb before hit tlbs
	bl	n7_inv_tlb

	@ invalidate branch predict cache
	bl	n7_inv_branch_pred_cache

	bl dramfreq_target_main

	@ the cpu may prefetch instructions before lombo_ddr_sleep_exit return,
	@ when ddr are unaccessible. so we add instructions here to prevent cpu
	@ prefetch instructions from dram before ddr exit sr.
	bl	nop_pre_t0
	bl	nop_pre_t1
	bl	nop_pre_t2
	bl	nop_pre_t3
	bl	nop_pre_t4
	bl	nop_pre_t5
	bl	nop_pre_t6
	bl	nop_pre_t7
	bl	nop_pre_t8
	bl	nop_pre_t9
	bl	nop_pre_ta
	bl	nop_pre_tb
	bl	nop_pre_tc
	bl	nop_pre_td
	bl	nop_pre_te
	bl	nop_pre_tf

	/*
	 * restore pte 0
	 */
	@ ---------------------------------------------------------------------
	pop	{r3, r4}
	str	r4, [r3]
	@ ---------------------------------------------------------------------

	@ invalidate tlb
	bl	n7_inv_tlb

	@ invalidate branch predict cache
	bl	n7_inv_branch_pred_cache

	@ restore lr reg
	ldmfd	sp!, {r3, r4}
	mov	sp, r3
	mov	pc, r4
ENDPROC(dramfreq_target_entry)

/**
 * n7_inv_tlb - Invalidate the whole tlb.
 *
 * void n7_inv_tlb(void);
 */
.globl n7_inv_tlb
n7_inv_tlb:
	mov	r3, #1

	@ Invalidate first level page table tlb
	mcr	p15, 0, r3, c8, c5, 0	@ Invalidate instruction TLB
	mcr	p15, 0, r3, c8, c6, 0	@ Invalidate data TLB

	@
	@ Invalidate second level page table tlb
	@
	@ the inner shareable and out shareable is relevant with system bus,
	@ it is the same for out platform. so the below operations are same
	@
	mcr	p15, 0, r3, c8, c7, 0	@ Invalidate unified TLB
	@mcr	p15, 0, r3, c8, c3, 0	@ Invalidate entire TLB Inner Shareable

	dsb
	bx	lr
ENDPROC(n7_inv_tlb)

/**
 * n7_inv_branch_pred_cache - Invalidate entire branch predictor array
 *
 * void n7_inv_branch_pred_cache(void);
 *
 * The branch predictor is used to improve the efficiency of jump instructions.
 * If this function(operation) is absent, the program will not run wrongly, because
 * the cpu will automatic clear instruction pipeline if the content of branch
 * predictor array is wrong.
 */
.globl n7_inv_branch_pred_cache
n7_inv_branch_pred_cache:
	mov	r3, #0
	mcr	p15, 0, r3, c7, c5, 6	@ invalid entire branch predictor array
	dsb
	isb
	bx	lr
ENDPROC(n7_inv_branch_pred_cache)

/*
 * nop function, for branch predict cache fill btac
 */
.macro btac_flush_t fname jmp
ENTRY(\fname)
	ldr	pc, =\jmp
\jmp:
	bx	lr
ENDPROC(\fname)
.endm

btac_flush_t btac_flush0 btac_t0
btac_flush_t btac_flush1 btac_t1
btac_flush_t btac_flush2 btac_t2
btac_flush_t btac_flush3 btac_t3
btac_flush_t btac_flush4 btac_t4
btac_flush_t btac_flush5 btac_t5
btac_flush_t btac_flush6 btac_t6
btac_flush_t btac_flush7 btac_t7
btac_flush_t btac_flush8 btac_t8
btac_flush_t btac_flush9 btac_t9
btac_flush_t btac_flusha btac_ta
btac_flush_t btac_flushb btac_tb
btac_flush_t btac_flushc btac_tc
btac_flush_t btac_flushd btac_td
btac_flush_t btac_flushe btac_te
btac_flush_t btac_flushf btac_tf

/*
 * nop function(in sram), for the instruction prefetch
 */
.macro nop_func fname jmp0 jmp1
ENTRY(\fname)
	cmp	r0, #0
	beq	\jmp0
	bgt	\jmp1
\jmp0:
	add	r0, r0, #1
	bx	lr
\jmp1:
	lsr	r0, r0, #1
	bx	lr
ENDPROC(\fname)
.endm

nop_func nop_pre_t0 nt00 nt01
nop_func nop_pre_t1 nt10 nt11
nop_func nop_pre_t2 nt20 nt21
nop_func nop_pre_t3 nt30 nt31
nop_func nop_pre_t4 nt40 nt41
nop_func nop_pre_t5 nt50 nt51
nop_func nop_pre_t6 nt60 nt61
nop_func nop_pre_t7 nt70 nt71
nop_func nop_pre_t8 nt80 nt81
nop_func nop_pre_t9 nt90 nt91
nop_func nop_pre_ta nta0 nta1
nop_func nop_pre_tb ntb0 ntb1
nop_func nop_pre_tc ntc0 ntc1
nop_func nop_pre_td ntd0 ntd1
nop_func nop_pre_te nte0 nte1
nop_func nop_pre_tf ntf0 ntf1

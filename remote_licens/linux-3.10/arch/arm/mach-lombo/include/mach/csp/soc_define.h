/*
 * soc_define.h - common definitions for register operation
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

#ifndef __SOC_DEFINE_H
#define __SOC_DEFINE_H

#ifdef __KERNEL__
#include <linux/io.h>
#include <linux/string.h>
#include <linux/kernel.h>

#define msg			printk

#define def_assert(expr)	do {if (!(expr)) pr_err("err %s %d", __func__, __LINE__); } while (0)

#define def_sprintf		sprintf
#define def_strcat		strcat
#define def_memset		memset

#define READREG32(reg)			readl((void __iomem *)reg)
#define READREG32_RELAX(reg)		readl_relaxed((void __iomem *)reg)
#define READREG8(reg)			readb((void __iomem *)reg)
#define READREG8_RELAX(reg)		readb_relaxed((void __iomem *)reg)
#define WRITEREG32(reg, val)		writel(val, (void __iomem *)reg)
#define WRITEREG32_RELAX(reg, val)	writel_relaxed(val, (void __iomem *)reg)
#define WRITEREG8(reg, val)		writeb(val, (void __iomem *)reg)
#define WRITEREG8_RELAX(reg, val)	writeb_relaxed(val, (void __iomem *)reg)

#else /* not linux environment, eg: suspend code */

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;
typedef short s16;
typedef char s8;

#ifndef NULL
#define NULL	0
#endif

#define mb()			({ asm volatile ("" : : : "memory"); })
#define rmb()			mb()
#define wmb()			({ asm volatile ("" : : : "memory"); })
#define __iormb()		rmb()
#define __iowmb()		wmb()

#define READREG32(reg)		({ u32 __v = *(const volatile u32 *)(reg); \
						__iormb(); __v; })
#define READREG8(reg)		({ u32 __v = *(const volatile u8 *)(reg); \
						__iormb(); __v; })
#define WRITEREG32(reg, val)	({ __iowmb(); *(volatile u32 *)(reg) = (val); })
#define WRITEREG8(reg, val)	({ __iowmb(); *(volatile u8 *)(reg) = (val); })
#endif /* __KERNEL__ */

/* use reg phys addr in mmu off environment, eg: resume */
#ifdef MMU_OFF
#define VA_REG			0
#else
#define VA_REG			0xF0000000
#endif

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif

/* dump macro */
#define DUMP_TYPE_DATA		0
#define DUMP_TYPE_READ		1
#define DUMP_TYPE_WRITE		2

/* set_reg  m_or_r ³£Á¿ */
#define MEM_MODE		0
#define REG_MODE		1

#define DUMP_BUF_LEN		1024

/*
 * DO NOT modify the macros' defination below!!
 */

/* get_mod_reg_bit  def_get_mod_reg_bit fun_get_mod_reg_bit */

/* function defination */
#define def_get_mod_reg_bit(mod, reg, bit, addr)	u32 get_##mod##_##reg##_##bit(u32 addr)

/* function realization */
#define fun_get_mod_reg_bit(mod, reg, bit, addr)	\
u32 get_##mod##_##reg##_##bit(u32 addr)			\
{							\
	reg_##mod##_##reg##_t r;			\
	r.val = READREG32(addr);			\
	return r.bits.bit;				\
}

/* set_mod_reg_bit  def_set_mod_reg_bit fun_set_mod_reg_bit */

/* function defination */
#define def_set_mod_reg_bit(mod, reg, bit, addr, val, m_or_r)	void set_##mod##_##reg##_##bit(u32 addr, u32 val, u32 m_or_r)

/* function realization */
#define fun_set_mod_reg_bit(mod, reg, bit, addr, val, m_or_r)	\
void set_##mod##_##reg##_##bit(u32 addr, u32 val, u32 m_or_r)	\
{								\
	reg_##mod##_##reg##_t r;				\
	if (0 == m_or_r)						\
		r.val = 0;					\
	else							\
		r.val = READREG32(addr);			\
	r.bits.bit = val;					\
	WRITEREG32(addr, r.val);				\
}

/* bitfield function pointer */

#define pfn_set_mod_reg_bit(mod, reg, bit)	set_##mod##_##reg##_##bit
#define pfn_get_mod_reg_bit(mod, reg, bit)	get_##mod##_##reg##_##bit

/* the 2 macros below can be used as functions by user */

/* get_mod_reg_bit defination */
#define get_mod_reg_bit(mod, reg, bit, addr)    get_##mod##_##reg##_##bit(addr)

/* set_mod_reg_bit defination */
#define set_mod_reg_bit(mod, reg, bit, addr, val, m_or_r)    set_##mod##_##reg##_##bit(addr, val, m_or_r)

#endif /* __SOC_DEFINE_H */


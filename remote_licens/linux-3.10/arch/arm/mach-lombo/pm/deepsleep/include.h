/*
 * include.h - deep sleep head file
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

#ifndef ___DS_INCLUDE___H___
#define ___DS_INCLUDE___H___

/* cpu wfi(not power off) in suspend end, correspond to deepsleep */
/* #define PM_STANDBY */

/* n7v1 board list */
#define N7V1_BOARD_EVB		0
#define N7V1_BOARD_VOYAGE	1

/* board select, may need change */
#define BOARD_SEL		N7V1_BOARD_VOYAGE
/* #define BOARD_SEL		N7V1_BOARD_EVB */

/*
 * prcm definations
 */
#define BIT_ENM			1
#define BIT_ENP			0
#define BIT_OEN			2

#define PLL_MOD_INT		0
#define PLL_MOD_FRAC		1
#define PLL_MOD_SPREAD		2
#define PLL_MOD_LINEAL		3

#define CPU_CLKSRC_HOSC		0
#define CPU_CLKSRC_RTCCLK	1
#define CPU_CLKSRC_CPUPLL	2
#define CPU_CLKDIV_1		0

#define MAXI_CLKSRC_NULL	0
#define MAXI_CLKSRC_HOSC	1
#define MAXI_CLKSRC_RTCCLK	2
#define MAXI_CLKSRC_PERH0_VCO	3
#define MAXI_CLKDIV_1		0

#ifndef __KERNEL__ /* only for pm */

#include "common.h"
#include "dram_sleep.h"

/* return val definations */
#define DS_OK		0
#define DS_ERR		-1

#define read_reg_bits(reg, pos, width)				\
		((READREG32(reg) >> pos) & ((1U << width) - 1))

#define write_reg_bits(reg, pos, width, val)			\
		WRITEREG32(reg, (READREG32(reg) &		\
		(u32)(~(((1U << width) - 1) << pos)))|	\
		(u32)((val&((1U<<width)-1))<<pos))

void uart_init(void);
void uart_clk_set(int enable);
int uart_clk_status(void);

#ifndef NO_UART
/* print operations */
void ds_printk(u8 *str, ...);
#else
static inline void ds_printk(u8 *str, ...) {}
#endif /* NO_UART */

/* float operations */
/* void __aeabi_uidiv(u32 ..); */
/* void __aeabi_uidivmod(u32 ..); */

/* print operations */
#define PRT_ERR(fmt, ...)				\
	ds_printk("%s:%d err: "fmt, __func__, __LINE__, ##__VA_ARGS__)

#define PRT_WARN(fmt, ...)				\
	ds_printk("%s:%d warn: "fmt, __func__, __LINE__, ##__VA_ARGS__)

#define PRT_MODIFY(fmt, ...)				\
	ds_printk("%s:%d modify: "fmt, __func__, __LINE__, ##__VA_ARGS__)

#define PRT_INFO(fmt, ...)				\
	ds_printk("%s:%d "fmt, __func__, __LINE__, ##__VA_ARGS__)

#define PRT_DBG(fmt, ...)				\
	ds_printk("%s:%d "fmt, __func__, __LINE__, ##__VA_ARGS__)

#define assert(expr)	do { if (!(expr))				\
				ds_printk("assert fail: %s %d",		\
					__func__, __LINE__); } while (0)
#endif /* __KERNEL__ */

#endif /* ___DS_INCLUDE___H___ */

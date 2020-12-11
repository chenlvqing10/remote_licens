/*
 * lombo_dram_entry.h - dram frequency define for LomboTech Socs
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
#include <stdarg.h>
#include "types.h"

/* #define __SRAM_DRAMFREQ_DEBUG */

#define UART_BASE               (0xF4003800)
#define UART_THR                (UART_BASE     +  0x0C)
#define UART_FSR                (UART_BASE     +  0x28)

#define read_reg(n)             (*((volatile unsigned int *)(n)))
#define write_reg(c, n)         (*((volatile unsigned int *)(n)) = (c))

#ifdef __SRAM_DRAMFREQ_DEBUG
extern int dramfreq_printk(u8 *str, ...);
#else
#define dramfreq_printk(...)    do {} while (0)
#endif

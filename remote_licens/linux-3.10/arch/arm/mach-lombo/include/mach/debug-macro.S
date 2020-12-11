/*
 * debug-macro.S - debug macros for LomboTech SoCs
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

#define UART_SHIFT	2

.macro	addruart, rp, rv, tmp
#if defined(CONFIG_SERIAL_LOMBO_CONSOLE_UART0)
	ldr	\rp, =0x04003000
	ldr	\rv, =0xf4003000
#elif defined(CONFIG_SERIAL_LOMBO_CONSOLE_UART1)
	ldr	\rp, =0x04003800
	ldr	\rv, =0xf4003800
#elif defined(CONFIG_SERIAL_LOMBO_CONSOLE_UART3)
	ldr	\rp, =0x04004800
	ldr	\rv, =0xf4004800
#endif
.endm

#include <asm/hardware/debug-8250.S>


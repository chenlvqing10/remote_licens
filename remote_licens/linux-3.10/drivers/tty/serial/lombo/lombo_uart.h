/*
 * lombo_uart.h - Generic definitions for LomboTech UART Controller
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

#ifndef __LOMBO_UART_H
#define __LOMBO_UART_H

#include <mach/debug.h>

 /* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME "UART"

#if defined(CONFIG_ARCH_LOMBO_N7V0) || defined(CONFIG_ARCH_LOMBO_N7V1) || defined(CONFIG_ARCH_LOMBO_N9V0)
#include "csp/n7/uart_operate.h"
#endif

#define LOMBO_UART_DEV_NAME       "uart"

struct lombo_uart_port {
	struct uart_port uport;

	unsigned int id;
	unsigned int io_num;
	unsigned int rs485_enable;
	unsigned int rs485_sw_enable;
	unsigned int rs485_gpio_ctrl;
	int          rs485_pin;
	char         name[16];

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	struct pinctrl *pctrl;
	struct clk     *uart_clk;
	struct clk     *apb_gate;
	struct clk     *apb_reset;
	struct clk     *parent;
#endif
};

static inline struct lombo_uart_port *to_lombo_uport(struct uart_port *u)
{
	return container_of(u, struct lombo_uart_port, uport);
}

static inline bool lombo_is_console_port(struct uart_port *port)
{
	return port->cons && port->cons->index == port->line;
}

#endif /* __LOMBO_UART_H */


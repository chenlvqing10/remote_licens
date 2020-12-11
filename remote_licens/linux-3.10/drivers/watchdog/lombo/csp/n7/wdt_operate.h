/*
 * wdt_operate.h - CSP define of LomboTech SPI Controller
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

#include <linux/types.h>
#include <mach/csp.h>

/* register offset define */
#define LOMBO_WDOG_CLK_CTRL        (0x08)  /* clock control register */
#define LOMBO_WDOG_CTRL            (0x10)  /* control register */
#define LOMBO_WDOG_TMRPERIOD       (0x14)  /* timeout period register */
#define LOMBO_WDOG_CNT_CUR_VAL     (0x18)  /* counter current value register */
#define LOMBO_WDOG_RESTART         (0x1C)  /* restart register */
#define LOMBO_WDOG_INT_EN          (0x20)  /* interrupt enable register */
#define LOMBO_WDOG_CLR_INT_PENDING (0x24)  /* clear int pending register */
#define LOMBO_WDOG_INT_PENDING     (0x28)  /* interrupt pending register */

#define LOMBO_WDOG_MODE_SYSTEM_RESET  (0)
#define STATUS_READY_TO_ENABLE	      (0x30)

extern void csp_wdt_set_system_reset(void *base);
extern void csp_wdt_set_timeout(void *base, u32 value);
extern void csp_wdt_restart(void *base);
extern void csp_wdt_enable(void *base);
extern void csp_wdt_disable(void *base);
extern void csp_wdt_clr_int_pend(void *base);
extern int csp_wdt_get_status(void *base);

/*
 * lombo_wdt.h - Generic definitions for LomboTech Watchdog Driver
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

#ifndef __LOMBO_WDT_H__
#define __LOMBO_WDT_H__

#include <mach/debug.h>

#undef MOD_NAME
#define MOD_NAME "WDT"

#include "csp/n7/wdt_operate.h"
#define LOMBO_WATCHDOG_DEFAULT_TIME	(16)
#define LOMBO_WATCHDOG_MAX_TIMEOUT	LOMBO_WATCHDOG_DEFAULT_TIME
#define LOMBO_WATCHDOG_MIN_TIMEOUT	(1)

#endif /* __LOMBO_WDT_H__ */


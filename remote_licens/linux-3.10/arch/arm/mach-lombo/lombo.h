/*
 * lombo.h - Generic definitions for LomboTech SoCs
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

#ifndef __MACH_LOMBO_H
#define __MACH_LOMBO_H

#include <linux/smp.h>
#include <mach/debug.h>
#include <mach/memory.h>

void rtc_set_hot_rst_flag(u32 value);
void rtc_set_hot_rst_exit(u32 addr);
void rtc_set_deep_slp_flag(u32 value);
void rtc_set_deep_slp_exit(u32 addr);

#ifdef CONFIG_SMP
extern struct smp_operations lombo_smp_ops;
void lombo_secondary_startup(void);
#endif

#ifdef CONFIG_PM
extern void *suspend_start;
extern void *suspend_end;
extern void *standby_start;
extern void *standby_end;
int lombo_pm_init(void);
void lombo_cpu_resume(void);
typedef int (*suspend_fn)(unsigned long);
void printascii(const char *);
#endif

#endif /* __MACH_LOMBO_H */


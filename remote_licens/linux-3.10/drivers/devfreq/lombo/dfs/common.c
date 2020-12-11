/*
 * common.c - common operations
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

#include "lombo_dram_printk.h"

#if 0
unsigned long dramfreq_get_sp(void)
{
	unsigned long sp_old;
	asm volatile ("mov %0, sp" : "=r" (sp_old));
	return sp_old;
}

void dramfreq_set_sp(unsigned long sp_new)
{
	asm volatile ("mov sp, %0" : : "r" (sp_new));
}
#endif

/**
 * __div0 - warn divided by 0
 *
 */
void __div0(void)
{
	dramfreq_printk("Attempting division by 0!");
}


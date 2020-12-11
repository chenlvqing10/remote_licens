/*
 * dram.h - Generic definitions for LomboTech Sdram API
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

#ifndef __MACH_COMMON_H
#define __MACH_COMMON_H

/* Hibernation and suspend events */
/* #define PM_HIBERNATION_PREPARE  0x0001 ;Going to hibernate */
/* #define PM_POST_HIBERNATION 0x0002 ;Hibernation finished */
/* #define PM_SUSPEND_PREPARE  0x0003 ;Going to suspend the system */
/* #define PM_POST_SUSPEND     0x0004 ;Suspend finished */
/* #define PM_RESTORE_PREPARE  0x0005 ;Going to restore a saved image */
/* #define PM_POST_RESTORE     0x0006 ;Restore failed */

/* tell devices the system will enter standby */
#define LOMBO_PM_STANDBY	0x0007
/* tell devices the system will enter deepsleep */
#define LOMBO_PM_DEEPSLEEP	0x0008

enum BOOT_TYPE {
	BOOT_TYPE_SD0      = 0,
	BOOT_TYPE_SD1      = 1,
	BOOT_TYPE_SPI_NOR  = 2,
	BOOT_TYPE_SPI_NAND = 3,
	BOOT_TYPE_NAND     = 5,
	BOOT_TYPE_INVALID  = 7,
};

extern enum BOOT_TYPE lombo_get_boot_type(void);

extern int pm_notifier_call_chain(unsigned long val);

extern u32 lombo_get_sdram_capacity(void);
extern u32 lombo_get_ion_carvout(u32 *base, u32 *size);

extern u32 lombo_func1(void);
extern u32 lombo_func2(u32 *p1, u32 *p2);

extern u32 boot_get_dma_status(void);
extern u32 pdef_rd(u32 rd_value);

#endif /* __MACH_COMMON_H */


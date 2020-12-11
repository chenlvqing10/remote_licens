/*
 * core_sync.h - SMP cpu sync operations
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

#ifdef CONFIG_ARCH_LOMBO_N7

/*
 * The SGI which already used is defined in smp.c:
 *   enum ipi_msg_type {
 *	IPI_WAKEUP,		(0)
 *	IPI_TIMER,		(1)
 *	IPI_RESCHEDULE,		(2)
 *	IPI_CALL_FUNC,		(3)
 *	IPI_CALL_FUNC_SINGLE,	(4)
 *	IPI_CPU_STOP,		(5)
 *	IPI_COMPLETION,		(6)
 *   };
 * so we use sgi 7 as core sync function
 */
#define IPI_CORE_SYNC	7

#ifdef CONFIG_SMP
u32 core_sync_1_start(void);
void core_sync_1_end(u32 flags);
#else
inline u32 core_sync_1_start(void)
{
	return 0;
}
inline void core_sync_1_end(u32 flags)
{
}
#endif

u32 core_sync_2_start(void);
void core_sync_2_end(u32 flags);

#endif


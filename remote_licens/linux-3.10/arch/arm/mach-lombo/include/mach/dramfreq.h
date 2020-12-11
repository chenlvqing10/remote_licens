/*
 * dram.h - Generic definitions for LomboTech Dramfreq API
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

#ifndef __MACH_DRAMFREQ_H
#define __MACH_DRAMFREQ_H

#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
#define DRAMFREQ_RETRIES_CNT       (5)
#define DRAMFREQ_LEAST_TIME_US     (2000)

struct dramfreq_vb_time_ops {
	int (*get_vb_time) (void);
	int (*get_next_vb_time) (void);
	int (*is_in_vb) (void);
};

int dramfreq_set_vb_time_ops(struct dramfreq_vb_time_ops *ops);
#endif

extern char *dfs_bin_start;
extern char *dfs_bin_end;

#endif /* __MACH_DRAMFREQ_H */


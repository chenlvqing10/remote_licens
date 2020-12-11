/*
 * memctrl.h - memctrl head file
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

#ifndef __MEMCTRL_H__
#define __MEMCTRL_H__

#include <linux/fs.h>

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"MEMCTRl"

#define LAT_CYCTLES	256

#undef u32
#define u32	unsigned int

enum memctrl_channel_id {
	MEMCTRL_CPU = 0,
	MEMCTRL_VC,
	MEMCTRL_AX,
	MEMCTRL_VISS0,
	MEMCTRL_VISS1,
	MEMCTRL_DPU,
	MEMCTRL_DMA,
	MEMCTRL_USB,	/* Not in statistics, only support prioriry setting */
	MEMCTRL_SDC0_2,	/* Not in statistics, only support prioriry setting */
	MEMCTRL_SDC1,	/* Not in statistics, only support prioriry setting */
	MEMCRTL_MAX
};

enum memctrl_bw_bits {
	BW_TIMEWINDOW = 0,
	BW_DATASIZE,
	BW_RANDOMMODE,
	BW_CTRLMODE,
	BW_RANDOMSEED
};

/*
* Channel data statistic
*/
struct channel_data {
	u64 rd_size;		/* The data size of reading */
	u64 wr_size;		/* The data size of writing */
	u64 rd_peak;		/* The peak data size of reading */
	u64 wr_peak;		/* The peak data size of writting */
	u64 rd_num;		/* The number of reading */
	u64 wr_num;		/* The number of writting */
	u64 rd_latency;		/* Unit: us*/
	u64 wr_latency;		/* Unit: us*/
};

/*
* Global data statistic
*/
struct global_data {
	u32 total_size;		/* Total Read/Write data size */
	u32 total_rd_size;	/* Total read data size */
	u32 total_wr_size;	/* Total write data size */
	u32 total_rd_peak;	/* Read peak size in total */
	u32 total_wr_peak;	/* Write peak size in total */
};

struct channel_info {
	const char *name;
	u32 hwnum;
};

struct data_unit {
	const char *name;
	u32 size;
};

struct cfg_unit {
	const char *name;
	u32 cfg;
	u32 size;
	u32 div;
	const char *unit;
};

struct bw_bits_name {
	const char *name;
	const char *ref;
	u32 val;
};

enum MOP {
	OP_CONSOLE = 0,
	OP_FILE
};

struct lombo_memctrl {
	struct miscdevice miscdev;

	void __iomem *reg;
	int irq;		/* irq in linux */

	struct clk *sdram_bw_clk;
	int lat_div_us;		/* time for a latency */

	u32 time;		/* Statistic time, unit:cycles */
	u32 period;		/* Dump period, uint:s, useless for now*/
	u32 count;		/* Times the program will run, useless for now*/
	u32 size_unit;		/* Data size unit: B/KB/MB */
	u32 flaw_unit;		/* Flaw data unit */
	u32 sample_unit;	/* Sample unit */
	enum MOP op;		/* Output option */
	char log_buf[PAGE_SIZE];
	u32 log_offset;
	char file[128];		/* File path */
	struct file *pfile;	/* FIle handle */
	loff_t fpos;
	bool running;		/* Is program runding */
};

extern int lombo_memctrl_wr_bw_limit(int master_id, u32 cycle,
					u32 band_width);
extern int lombo_memctrl_rd_bw_limit(int master_id, u32 cycle,
					u32 band_width);

#endif /* __MEMCTRL_H__ */

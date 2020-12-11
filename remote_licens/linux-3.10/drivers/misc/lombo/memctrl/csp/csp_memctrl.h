/*
 * csp_memctrl.h - pwm csp head file
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

#ifndef __CSP_MEMCTRL_H__
#define __CSP_MEMCTRL_H__

#include <linux/types.h>
#include <mach/csp.h>

#define MEMCTRL_PRIO_BASE       0x01210000
#define MC_PRI_R(n)	(MEMCTRL_PRIO_BASE + VA_MEM_CTRL + 0x404 + 0xb0 * (n))
#define MC_PRI_W(n)	(MEMCTRL_PRIO_BASE + VA_MEM_CTRL + 0x408 + 0xb0 * (n))

#define MEMCRTL_VER_OFFSET			0x00000000
#define MEMCRTL_MON_MODE_CTRL_OFFSET		0x00000004
#define MEMCRTL_BW_CTRL_OFFSET			0x00000008
#define MEMCRTL_INT_EN_OFFSET			0x0000000C
#define MEMCRTL_UNITS_CFG_OFFSET		0x00000010
#define MEMCRTL_MON_STATUS_OFFSET		0x00000014
#define MEMCRTL_BW_STM_OFFSET			0x00000018
#define MEMCRTL_RD_LAT_OF_OFFSET		0x00000020
#define MEMCRTL_WR_LAT_OF_OFFSET		0x00000024
#define MEMCRTL_TOTAL_DAT_OFFSET		0x00000040
#define MEMCRTL_TOTAL_RDAT_OFFSET		0x00000044
#define MEMCRTL_TOTAL_WDAT_OFFSET		0x00000048
#define MEMCRTL_TOTAL_RPDAT_OFFSET		0x00000050
#define MEMCRTL_TOTAL_WPDAT_OFFSET		0x00000054

#define MEMCRTL_RD_DAT_OFFSET			0x00000080
#define MEMCRTL_WR_DAT_OFFSET			0x00000084
#define MEMCRTL_RD_DAT_PEAK_OFFSET		0x00000088
#define MEMCRTL_WR_DAT_PEAK_OFFSET		0x0000008C

#define MEMCRTL_RD_CMD_NUM_OFFSET		0x00000200
#define MEMCRTL_RD_LATENCY_OFFSET		0x00000204
#define MEMCRTL_WR_CMD_NUM_OFFSET		0x00000208
#define MEMCRTL_WR_LATENCY_OFFSET		0x0000020C

#define MEMCRTL_WR_BW_LIMIT_OFFSET		0x00000800
#define MEMCRTL_WR_BW_TIMEWIN_OFFSET		0x00000808
#define MEMCRTL_WR_BW_DAT_SIZE_OFFSET		0x0000080C
#define MEMCRTL_RD_BW_LIMIT_OFFSET		0x00000810
#define MEMCRTL_RD_BW_TIMEWIN_OFFSET		0x00000818
#define MEMCRTL_RD_BW_DAT_SIZE_OFFSET		0x0000081C

#define MEMCTRL_MASTER_OFFSET			0x10

enum memctrl_clk_src {
	MEMCTRL_CLK_DISABLE = 0,
	MEMCTRL_CLK_24M,
	MEMCTRL_CLK_RSVD
};

enum memctrl_sample {
	MEMCTRL_SAMPLE_1MS = 0,
	MEMCTRL_SAMPLE_10MS,
	MEMCTRL_SAMPLE_100MS,
	MEMCTRL_SAMPLE_1S,
	MEMCTRL_SAMPLE_10S
};

enum memctrl_data_unit {
	MEMCTRL_UNIT_1B = 0,
	MEMCTRL_UNIT_16B,
	MEMCTRL_UNIT_64B,
	MEMCTRL_UNIT_256B,
	MEMCTRL_UNIT_1KB,
	MEMCTRL_UNIT_4KB,
	MEMCTRL_UNIT_64KB,
	MEMCTRL_UNIT_128KB,
};

enum memctrl_bw_ctrl_mode {
	MEMCTRL_NOT_LIMIT_MODE = 0,
	MEMCTRL_RANDOM_MODE,
	MEMCTRL_PEEK_LIMIT_MODE,
	MEMCTRL_AVERAGE_MODE,
};

enum memctrl_bw_random_mode {
	MEMCTRL_RANDOM_MODE1 = 0,
	MEMCTRL_RANDOM_MODE2,
	MEMCTRL_RANDOM_MODE3,
	MEMCTRL_RANDOM_MODE4,
};

void csp_memctrl_set_mode(void *base, bool manual);
void csp_memctrl_enable(void *base, bool enable);
void csp_memctrl_interrupt_enable(void *base, bool enable);
void csp_memctrl_set_clk(void *base, enum memctrl_clk_src clk_sel);
void csp_memctrl_set_sample(void *base, enum memctrl_sample sample);
void csp_memctrl_set_data_unit(void *base, enum memctrl_data_unit data);
u32 csp_memctrl_is_data_overflow(void *base);
u32 csp_memctrl_is_cycle_overflow(void *base);
void csp_memctrl_clear_status(void *base);
u32 csp_memctrl_get_rd_latency_status(void *base);
void csp_memctrl_clear_rd_latency_status(void *base);
u32 csp_memctrl_get_wr_latency_status(void *base);
void csp_memctrl_clear_wr_latency_status(void *base);
u32 csp_memctrl_get_cycle(void *base);
u32 csp_memctrl_get_total_data_size(void *base);
u32 csp_memctrl_get_total_rd_data_size(void *base);
u32 csp_memctrl_get_total_wr_data_size(void *base);
u32 csp_memctrl_get_total_rd_peak(void *base);
u32 csp_memctrl_get_total_wr_peak(void *base);
u32 csp_memctrl_get_rd_data_size(void *base, int channel);
u32 csp_memctrl_get_wr_data_size(void *base, int channel);
u32 csp_memctrl_get_rd_peak_data_size(void *base, int channel);
u32 csp_memctrl_get_wr_peak_data_size(void *base, int channel);
u32 csp_memctrl_get_rd_cmd_num(void *base, int channel);
u32 csp_memctrl_get_rd_latency(void *base, int channel);
u32 csp_memctrl_get_wr_cmd_num(void *base, int channel);
u32 csp_memctrl_get_wr_latency(void *base, int channel);
u32 csp_memctrl_get_pri_r(int channel);
u32 csp_memctrl_get_pri_w(int channel);
void csp_memctrl_set_ax_wr_timewin(void *base, u32 cycle);
u32 csp_memctrl_get_ax_wr_bw_timewin(void *base);
void csp_memctrl_set_ax_wr_datasize(void *base, u32 bandwidth);
u32 csp_memctrl_get_ax_wr_bw_datasize(void *base);
void csp_memctrl_set_ax_wr_ctrl_mode(void *base,
				enum memctrl_bw_ctrl_mode mode);
u32 csp_memctrl_get_ax_wr_ctrl_mode(void *base);
void csp_memctrl_set_ax_wr_bw_randomode(void *base,
				enum memctrl_bw_random_mode mode);
u32 csp_memctrl_get_ax_wr_bw_randomode(void *base);
void csp_memctrl_set_ax_wr_bw_seed(void *base, u32 seed);
u32 csp_memctrl_get_ax_wr_bw_seed(void *base);
void csp_memctrl_set_ax_rd_timewin(void *base, u32 cycle);
u32 csp_memctrl_get_ax_rd_bw_timewin(void *base);
void csp_memctrl_set_ax_rd_datasize(void *base, u32 bandwidth);
u32 csp_memctrl_get_ax_rd_bw_datasize(void *base);
void csp_memctrl_set_ax_rd_ctrl_mode(void *base,
					enum memctrl_bw_ctrl_mode mode);
u32 csp_memctrl_get_ax_rd_ctrl_mode(void *base);
void csp_memctrl_set_ax_rd_bw_randomode(void *base,
				enum memctrl_bw_random_mode mode);
u32 csp_memctrl_get_ax_rd_bw_randomode(void *base);
void csp_memctrl_set_ax_rd_bw_seed(void *base, u32 seed);
u32 csp_memctrl_get_ax_rd_bw_seed(void *base);




#endif/* __CSP_MEMCTRL_H__ */


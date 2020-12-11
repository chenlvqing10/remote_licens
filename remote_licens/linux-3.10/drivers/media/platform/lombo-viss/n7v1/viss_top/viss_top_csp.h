/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Lombo n7 VISS-TOP controller register definitions header
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

#ifndef ___VISS_TOP___H___
#define ___VISS_TOP___H___

enum viss_top_module {
	VIC		= 0,
	MIPI_CSI	= 1,
	ISP		= 2,
	ISP_LITE	= 3,
};

enum viss_top_clk_src {
	PERH0_PLL_DIV2	= 0,
	VC_PLL		= 1,
};

enum viss_top_mclk_src {
	MCLK_HOSC		= 0,
	MCLK_PERH0_PLL_DIV2	= 1,
};

void csp_viss_top_set_register_base(void *base);
u32 csp_viss_top_gat_rst_init(void);
u32 csp_viss_top_clock_init(void);
u32 csp_viss_top_pllmux_clock_source(u32 src);
u32 csp_viss_top_adpll_prediv(u32 div);
u32 csp_viss_top_setup_adpll(u32 prediv, u32 factor);
u32 csp_viss_top_adpll_enable(void);
u32 csp_viss_top_adpll_disable(void);
u32 csp_viss_top_adpll_lock(void);
u32 csp_viss_top_adpll_init(void);
u32 csp_viss_top_vic_init(u32 clk_freq);
u32 csp_viss_top_vic_reset(void);
u32 csp_viss_top_mcsi_init(u32 clk_freq);
u32 csp_viss_top_mcsi_reset(void);
u32 csp_viss_top_isp_init(u32 clk_freq);
u32 csp_viss_top_isp_clk_enable(u32 en);
u32 csp_viss_top_isp_lite_init(u32 clk_freq);
u32 csp_viss_top_vic_mclk_init(u32 clk_freq);
u32 csp_viss_top_vic_mclk_gate(u32 enable);
u32 csp_viss_top_mcsi_mclk_init(u32 clk_freq);
u32 csp_viss_top_mcsi_mclk_gate(u32 enable);
u32 csp_viss_top_isp_data_source(u32 src);
u32 csp_viss_top_isp_lite_data_source(u32 src);
u32 csp_viss_top_init(void);

void csp_viss_cam_ldo_enable(void);
void csp_viss_cam_ldo_disable(void);
#endif	/* ___VISS_TOP___H___ */

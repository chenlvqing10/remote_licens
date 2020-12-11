/* viss_set.h */

#ifndef ___VISS__SET___H___
#define ___VISS__SET___H___

#ifdef DEF_SET_VISS_VER
void set_viss_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_VER_VER_L
def_set_mod_reg_bit(viss, ver, ver_l, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VER_VER_H
def_set_mod_reg_bit(viss, ver, ver_h, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VER_COMP
def_set_mod_reg_bit(viss, ver, comp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_FTR
void set_viss_ftr(u32 reg_addr,
		u32 has_vic,
		u32 has_mcsi,
		u32 has_tvd,
		u32 has_isp,
		u32 has_isp_lite,
		u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_FTR_HAS_VIC
def_set_mod_reg_bit(viss, ftr, has_vic, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_FTR_HAS_MCSI
def_set_mod_reg_bit(viss, ftr, has_mcsi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_FTR_HAS_TVD
def_set_mod_reg_bit(viss, ftr, has_tvd, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_FTR_HAS_ISP
def_set_mod_reg_bit(viss, ftr, has_isp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_FTR_HAS_ISP_LITE
def_set_mod_reg_bit(viss, ftr, has_isp_lite, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG
void set_viss_vic_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 mclk_gat,
			u32 mclk_src,
			u32 clk_dir,
			u32 mclk_dir,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_RST
def_set_mod_reg_bit(viss, vic_cfg, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_HCLK_GAT
def_set_mod_reg_bit(viss, vic_cfg, hclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_CLK_GAT
def_set_mod_reg_bit(viss, vic_cfg, clk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_CLK_SRC
def_set_mod_reg_bit(viss, vic_cfg, clk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_MCLK_GAT
def_set_mod_reg_bit(viss, vic_cfg, mclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_MCLK_SRC
def_set_mod_reg_bit(viss, vic_cfg, mclk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_CLK_DIR
def_set_mod_reg_bit(viss, vic_cfg, clk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_VIC_CFG_MCLK_DIR
def_set_mod_reg_bit(viss, vic_cfg, mclk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG
void set_viss_mcsi_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 mclk_gat,
			u32 mclk_src,
			u32 cfgclk_gat,
			u32 clk_dir,
			u32 mclk_dir,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_RST
def_set_mod_reg_bit(viss, mcsi_cfg, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_HCLK_GAT
def_set_mod_reg_bit(viss, mcsi_cfg, hclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CLK_GAT
def_set_mod_reg_bit(viss, mcsi_cfg, clk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CLK_SRC
def_set_mod_reg_bit(viss, mcsi_cfg, clk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_MCLK_GAT
def_set_mod_reg_bit(viss, mcsi_cfg, mclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_MCLK_SRC
def_set_mod_reg_bit(viss, mcsi_cfg, mclk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CFGCLK_GAT
def_set_mod_reg_bit(viss, mcsi_cfg, cfgclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CLK_DIR
def_set_mod_reg_bit(viss, mcsi_cfg, clk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_MCLK_DIR
def_set_mod_reg_bit(viss, mcsi_cfg, mclk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_TVD_CFG
void set_viss_tvd_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 clk_dir,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_TVD_CFG_RST
def_set_mod_reg_bit(viss, tvd_cfg, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_TVD_CFG_HCLK_GAT
def_set_mod_reg_bit(viss, tvd_cfg, hclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_TVD_CFG_CLK_GAT
def_set_mod_reg_bit(viss, tvd_cfg, clk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_TVD_CFG_CLK_SRC
def_set_mod_reg_bit(viss, tvd_cfg, clk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_TVD_CFG_CLK_DIR
def_set_mod_reg_bit(viss, tvd_cfg, clk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_CFG
void set_viss_isp_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 clk_dir,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_CFG_RST
def_set_mod_reg_bit(viss, isp_cfg, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_CFG_HCLK_GAT
def_set_mod_reg_bit(viss, isp_cfg, hclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_CFG_CLK_GAT
def_set_mod_reg_bit(viss, isp_cfg, clk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_CFG_CLK_SRC
def_set_mod_reg_bit(viss, isp_cfg, clk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_CFG_CLK_DIR
def_set_mod_reg_bit(viss, isp_cfg, clk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG
void set_viss_isp_lite_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 clk_dir,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_RST
def_set_mod_reg_bit(viss, isp_lite_cfg, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_HCLK_GAT
def_set_mod_reg_bit(viss, isp_lite_cfg, hclk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_CLK_GAT
def_set_mod_reg_bit(viss, isp_lite_cfg, clk_gat, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_CLK_SRC
def_set_mod_reg_bit(viss, isp_lite_cfg, clk_src, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_CLK_DIR
def_set_mod_reg_bit(viss, isp_lite_cfg, clk_dir, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_PIX_SEL
void set_viss_isp_pix_sel(u32 reg_addr,
			u32 isp_pix_sel,
			u32 ispl_pix_sel,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_PIX_SEL_ISP_PIX_SEL
def_set_mod_reg_bit(viss, isp_pix_sel, isp_pix_sel, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ISP_PIX_SEL_ISPL_PIX_SEL
def_set_mod_reg_bit(viss, isp_pix_sel, ispl_pix_sel, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_PLL_MUX_CTRL
void set_viss_pll_mux_ctrl(u32 reg_addr,
			u32 pllmux_clk_sel,
			u32 adpll_pre_div,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_PLL_MUX_CTRL_PLLMUX_CLK_SEL
def_set_mod_reg_bit(viss, pll_mux_ctrl, pllmux_clk_sel, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_PLL_MUX_CTRL_ADPLL_PRE_DIV
def_set_mod_reg_bit(viss, pll_mux_ctrl, adpll_pre_div, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_FAC
void set_viss_adpll_fac(u32 reg_addr,
			u32 n,
			u32 en,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_FAC_N
def_set_mod_reg_bit(viss, adpll_fac, n, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_FAC_EN
def_set_mod_reg_bit(viss, adpll_fac, en, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0
void set_viss_adpll_tune0(u32 reg_addr,
			u32 tune0,
			u32 tune1,
			u32 auto_adrange,
			u32 tune2,
			u32 tune3,
			u32 tune4,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE0
def_set_mod_reg_bit(viss, adpll_tune0, tune0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE1
def_set_mod_reg_bit(viss, adpll_tune0, tune1, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_AUTO_ADRANGE
def_set_mod_reg_bit(viss, adpll_tune0, auto_adrange, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE2
def_set_mod_reg_bit(viss, adpll_tune0, tune2, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE3
def_set_mod_reg_bit(viss, adpll_tune0, tune3, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE4
def_set_mod_reg_bit(viss, adpll_tune0, tune4, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1
void set_viss_adpll_tune1(u32 reg_addr,
			u32 tune0,
			u32 tune1,
			u32 tune2,
			u32 tune3,
			u32 slewrate,
			u32 tune4,
			u32 tune5,
			u32 tune6,
			u32 tune7,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE0
def_set_mod_reg_bit(viss, adpll_tune1, tune0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE1
def_set_mod_reg_bit(viss, adpll_tune1, tune1, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE2
def_set_mod_reg_bit(viss, adpll_tune1, tune2, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE3
def_set_mod_reg_bit(viss, adpll_tune1, tune3, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_SLEWRATE
def_set_mod_reg_bit(viss, adpll_tune1, slewrate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE4
def_set_mod_reg_bit(viss, adpll_tune1, tune4, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE5
def_set_mod_reg_bit(viss, adpll_tune1, tune5, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE6
def_set_mod_reg_bit(viss, adpll_tune1, tune6, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE7
def_set_mod_reg_bit(viss, adpll_tune1, tune7, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT
void set_viss_adpll_stat(u32 reg_addr,
			u32 ferr,
			u32 flock,
			u32 perr,
			u32 plock,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_FERR
def_set_mod_reg_bit(viss, adpll_stat, ferr, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_FLOCK
def_set_mod_reg_bit(viss, adpll_stat, flock, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_PERR
def_set_mod_reg_bit(viss, adpll_stat, perr, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_PLOCK
def_set_mod_reg_bit(viss, adpll_stat, plock, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG
void set_viss_mbist_cfg(u32 reg_addr,
			u32 vic_cfg,
			u32 mcsi_cfg,
			u32 tvd_cfg,
			u32 isp_cfg,
			u32 isp_lite_cfg,
			u32 vic_sta,
			u32 mcsi_sta,
			u32 tvd_sta,
			u32 isp_sta,
			u32 isp_lite_sta,
			u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_VIC_CFG
def_set_mod_reg_bit(viss, mbist_cfg, vic_cfg, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_MCSI_CFG
def_set_mod_reg_bit(viss, mbist_cfg, mcsi_cfg, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_TVD_CFG
def_set_mod_reg_bit(viss, mbist_cfg, tvd_cfg, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_CFG
def_set_mod_reg_bit(viss, mbist_cfg, isp_cfg, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_LITE_CFG
def_set_mod_reg_bit(viss, mbist_cfg, isp_lite_cfg, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_VIC_STA
def_set_mod_reg_bit(viss, mbist_cfg, vic_sta, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_MCSI_STA
def_set_mod_reg_bit(viss, mbist_cfg, mcsi_sta, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_TVD_STA
def_set_mod_reg_bit(viss, mbist_cfg, tvd_sta, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_STA
def_set_mod_reg_bit(viss, mbist_cfg, isp_sta, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_LITE_STA
def_set_mod_reg_bit(viss, mbist_cfg, isp_lite_sta, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL
void set_viss_axi_rd_qos_lvl(u32 reg_addr,
				u32 vic,
				u32 mcsi,
				u32 isp,
				u32 isp_lite,
				u32 tvd_wb,
				u32 tvd_3d,
				u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_VIC
def_set_mod_reg_bit(viss, axi_rd_qos_lvl, vic, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_MCSI
def_set_mod_reg_bit(viss, axi_rd_qos_lvl, mcsi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_ISP
def_set_mod_reg_bit(viss, axi_rd_qos_lvl, isp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_ISP_LITE
def_set_mod_reg_bit(viss, axi_rd_qos_lvl, isp_lite, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_TVD_WB
def_set_mod_reg_bit(viss, axi_rd_qos_lvl, tvd_wb, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_TVD_3D
def_set_mod_reg_bit(viss, axi_rd_qos_lvl, tvd_3d, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL
void set_viss_axi_wt_qos_lvl(u32 reg_addr,
				u32 vic,
				u32 mcsi,
				u32 isp,
				u32 isp_lite,
				u32 tvd_wb,
				u32 tvd_3d,
				u32 m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_VIC
def_set_mod_reg_bit(viss, axi_wt_qos_lvl, vic, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_MCSI
def_set_mod_reg_bit(viss, axi_wt_qos_lvl, mcsi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_ISP
def_set_mod_reg_bit(viss, axi_wt_qos_lvl, isp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_ISP_LITE
def_set_mod_reg_bit(viss, axi_wt_qos_lvl, isp_lite, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_TVD_WB
def_set_mod_reg_bit(viss, axi_wt_qos_lvl, tvd_wb, addr, val, m_or_r);
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_TVD_3D
def_set_mod_reg_bit(viss, axi_wt_qos_lvl, tvd_3d, addr, val, m_or_r);
#endif

#endif /* ___VISS__SET___H___ */

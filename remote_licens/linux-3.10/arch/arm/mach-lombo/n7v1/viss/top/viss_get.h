/* viss_get.h */

#ifndef ___VISS__GET___H___
#define ___VISS__GET___H___

#ifdef DEF_GET_VISS_VER_VER_L
def_get_mod_reg_bit(viss, ver, ver_l, addr);
#endif

#ifdef DEF_GET_VISS_VER_VER_H
def_get_mod_reg_bit(viss, ver, ver_h, addr);
#endif

#ifdef DEF_GET_VISS_VER_COMP
def_get_mod_reg_bit(viss, ver, comp, addr);
#endif

#ifdef DEF_GET_VISS_FTR_HAS_VIC
def_get_mod_reg_bit(viss, ftr, has_vic, addr);
#endif

#ifdef DEF_GET_VISS_FTR_HAS_MCSI
def_get_mod_reg_bit(viss, ftr, has_mcsi, addr);
#endif

#ifdef DEF_GET_VISS_FTR_HAS_TVD
def_get_mod_reg_bit(viss, ftr, has_tvd, addr);
#endif

#ifdef DEF_GET_VISS_FTR_HAS_ISP
def_get_mod_reg_bit(viss, ftr, has_isp, addr);
#endif

#ifdef DEF_GET_VISS_FTR_HAS_ISP_LITE
def_get_mod_reg_bit(viss, ftr, has_isp_lite, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_RST
def_get_mod_reg_bit(viss, vic_cfg, rst, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_HCLK_GAT
def_get_mod_reg_bit(viss, vic_cfg, hclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_CLK_GAT
def_get_mod_reg_bit(viss, vic_cfg, clk_gat, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_CLK_SRC
def_get_mod_reg_bit(viss, vic_cfg, clk_src, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_MCLK_GAT
def_get_mod_reg_bit(viss, vic_cfg, mclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_MCLK_SRC
def_get_mod_reg_bit(viss, vic_cfg, mclk_src, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_CLK_DIR
def_get_mod_reg_bit(viss, vic_cfg, clk_dir, addr);
#endif

#ifdef DEF_GET_VISS_VIC_CFG_MCLK_DIR
def_get_mod_reg_bit(viss, vic_cfg, mclk_dir, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_RST
def_get_mod_reg_bit(viss, mcsi_cfg, rst, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_HCLK_GAT
def_get_mod_reg_bit(viss, mcsi_cfg, hclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_CLK_GAT
def_get_mod_reg_bit(viss, mcsi_cfg, clk_gat, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_CLK_SRC
def_get_mod_reg_bit(viss, mcsi_cfg, clk_src, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_MCLK_GAT
def_get_mod_reg_bit(viss, mcsi_cfg, mclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_MCLK_SRC
def_get_mod_reg_bit(viss, mcsi_cfg, mclk_src, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_CFGCLK_GAT
def_get_mod_reg_bit(viss, mcsi_cfg, cfgclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_CLK_DIR
def_get_mod_reg_bit(viss, mcsi_cfg, clk_dir, addr);
#endif

#ifdef DEF_GET_VISS_MCSI_CFG_MCLK_DIR
def_get_mod_reg_bit(viss, mcsi_cfg, mclk_dir, addr);
#endif

#ifdef DEF_GET_VISS_TVD_CFG_RST
def_get_mod_reg_bit(viss, tvd_cfg, rst, addr);
#endif

#ifdef DEF_GET_VISS_TVD_CFG_HCLK_GAT
def_get_mod_reg_bit(viss, tvd_cfg, hclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_TVD_CFG_CLK_GAT
def_get_mod_reg_bit(viss, tvd_cfg, clk_gat, addr);
#endif

#ifdef DEF_GET_VISS_TVD_CFG_CLK_SRC
def_get_mod_reg_bit(viss, tvd_cfg, clk_src, addr);
#endif

#ifdef DEF_GET_VISS_TVD_CFG_CLK_DIR
def_get_mod_reg_bit(viss, tvd_cfg, clk_dir, addr);
#endif

#ifdef DEF_GET_VISS_ISP_CFG_RST
def_get_mod_reg_bit(viss, isp_cfg, rst, addr);
#endif

#ifdef DEF_GET_VISS_ISP_CFG_HCLK_GAT
def_get_mod_reg_bit(viss, isp_cfg, hclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_ISP_CFG_CLK_GAT
def_get_mod_reg_bit(viss, isp_cfg, clk_gat, addr);
#endif

#ifdef DEF_GET_VISS_ISP_CFG_CLK_SRC
def_get_mod_reg_bit(viss, isp_cfg, clk_src, addr);
#endif

#ifdef DEF_GET_VISS_ISP_CFG_CLK_DIR
def_get_mod_reg_bit(viss, isp_cfg, clk_dir, addr);
#endif

#ifdef DEF_GET_VISS_ISP_LITE_CFG_RST
def_get_mod_reg_bit(viss, isp_lite_cfg, rst, addr);
#endif

#ifdef DEF_GET_VISS_ISP_LITE_CFG_HCLK_GAT
def_get_mod_reg_bit(viss, isp_lite_cfg, hclk_gat, addr);
#endif

#ifdef DEF_GET_VISS_ISP_LITE_CFG_CLK_GAT
def_get_mod_reg_bit(viss, isp_lite_cfg, clk_gat, addr);
#endif

#ifdef DEF_GET_VISS_ISP_LITE_CFG_CLK_SRC
def_get_mod_reg_bit(viss, isp_lite_cfg, clk_src, addr);
#endif

#ifdef DEF_GET_VISS_ISP_LITE_CFG_CLK_DIR
def_get_mod_reg_bit(viss, isp_lite_cfg, clk_dir, addr);
#endif

#ifdef DEF_GET_VISS_ISP_PIX_SEL_ISP_PIX_SEL
def_get_mod_reg_bit(viss, isp_pix_sel, isp_pix_sel, addr);
#endif

#ifdef DEF_GET_VISS_ISP_PIX_SEL_ISPL_PIX_SEL
def_get_mod_reg_bit(viss, isp_pix_sel, ispl_pix_sel, addr);
#endif

#ifdef DEF_GET_VISS_PLL_MUX_CTRL_PLLMUX_CLK_SEL
def_get_mod_reg_bit(viss, pll_mux_ctrl, pllmux_clk_sel, addr);
#endif

#ifdef DEF_GET_VISS_PLL_MUX_CTRL_ADPLL_PRE_DIV
def_get_mod_reg_bit(viss, pll_mux_ctrl, adpll_pre_div, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_FAC_N
def_get_mod_reg_bit(viss, adpll_fac, n, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_FAC_EN
def_get_mod_reg_bit(viss, adpll_fac, en, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE0_TUNE0
def_get_mod_reg_bit(viss, adpll_tune0, tune0, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE0_TUNE1
def_get_mod_reg_bit(viss, adpll_tune0, tune1, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE0_AUTO_ADRANGE
def_get_mod_reg_bit(viss, adpll_tune0, auto_adrange, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE0_TUNE2
def_get_mod_reg_bit(viss, adpll_tune0, tune2, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE0_TUNE3
def_get_mod_reg_bit(viss, adpll_tune0, tune3, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE0_TUNE4
def_get_mod_reg_bit(viss, adpll_tune0, tune4, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE0
def_get_mod_reg_bit(viss, adpll_tune1, tune0, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE1
def_get_mod_reg_bit(viss, adpll_tune1, tune1, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE2
def_get_mod_reg_bit(viss, adpll_tune1, tune2, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE3
def_get_mod_reg_bit(viss, adpll_tune1, tune3, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_SLEWRATE
def_get_mod_reg_bit(viss, adpll_tune1, slewrate, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE4
def_get_mod_reg_bit(viss, adpll_tune1, tune4, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE5
def_get_mod_reg_bit(viss, adpll_tune1, tune5, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE6
def_get_mod_reg_bit(viss, adpll_tune1, tune6, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_TUNE1_TUNE7
def_get_mod_reg_bit(viss, adpll_tune1, tune7, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_STAT_FERR
def_get_mod_reg_bit(viss, adpll_stat, ferr, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_STAT_FLOCK
def_get_mod_reg_bit(viss, adpll_stat, flock, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_STAT_PERR
def_get_mod_reg_bit(viss, adpll_stat, perr, addr);
#endif

#ifdef DEF_GET_VISS_ADPLL_STAT_PLOCK
def_get_mod_reg_bit(viss, adpll_stat, plock, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_VIC_CFG
def_get_mod_reg_bit(viss, mbist_cfg, vic_cfg, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_MCSI_CFG
def_get_mod_reg_bit(viss, mbist_cfg, mcsi_cfg, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_TVD_CFG
def_get_mod_reg_bit(viss, mbist_cfg, tvd_cfg, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_ISP_CFG
def_get_mod_reg_bit(viss, mbist_cfg, isp_cfg, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_ISP_LITE_CFG
def_get_mod_reg_bit(viss, mbist_cfg, isp_lite_cfg, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_VIC_STA
def_get_mod_reg_bit(viss, mbist_cfg, vic_sta, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_MCSI_STA
def_get_mod_reg_bit(viss, mbist_cfg, mcsi_sta, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_TVD_STA
def_get_mod_reg_bit(viss, mbist_cfg, tvd_sta, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_ISP_STA
def_get_mod_reg_bit(viss, mbist_cfg, isp_sta, addr);
#endif

#ifdef DEF_GET_VISS_MBIST_CFG_ISP_LITE_STA
def_get_mod_reg_bit(viss, mbist_cfg, isp_lite_sta, addr);
#endif

#ifdef DEF_GET_VISS_AXI_RD_QOS_LVL_VIC
def_get_mod_reg_bit(viss, axi_rd_qos_lvl, vic, addr);
#endif

#ifdef DEF_GET_VISS_AXI_RD_QOS_LVL_MCSI
def_get_mod_reg_bit(viss, axi_rd_qos_lvl, mcsi, addr);
#endif

#ifdef DEF_GET_VISS_AXI_RD_QOS_LVL_ISP
def_get_mod_reg_bit(viss, axi_rd_qos_lvl, isp, addr);
#endif

#ifdef DEF_GET_VISS_AXI_RD_QOS_LVL_ISP_LITE
def_get_mod_reg_bit(viss, axi_rd_qos_lvl, isp_lite, addr);
#endif

#ifdef DEF_GET_VISS_AXI_RD_QOS_LVL_TVD_WB
def_get_mod_reg_bit(viss, axi_rd_qos_lvl, tvd_wb, addr);
#endif

#ifdef DEF_GET_VISS_AXI_RD_QOS_LVL_TVD_3D
def_get_mod_reg_bit(viss, axi_rd_qos_lvl, tvd_3d, addr);
#endif

#ifdef DEF_GET_VISS_AXI_WT_QOS_LVL_VIC
def_get_mod_reg_bit(viss, axi_wt_qos_lvl, vic, addr);
#endif

#ifdef DEF_GET_VISS_AXI_WT_QOS_LVL_MCSI
def_get_mod_reg_bit(viss, axi_wt_qos_lvl, mcsi, addr);
#endif

#ifdef DEF_GET_VISS_AXI_WT_QOS_LVL_ISP
def_get_mod_reg_bit(viss, axi_wt_qos_lvl, isp, addr);
#endif

#ifdef DEF_GET_VISS_AXI_WT_QOS_LVL_ISP_LITE
def_get_mod_reg_bit(viss, axi_wt_qos_lvl, isp_lite, addr);
#endif

#ifdef DEF_GET_VISS_AXI_WT_QOS_LVL_TVD_WB
def_get_mod_reg_bit(viss, axi_wt_qos_lvl, tvd_wb, addr);
#endif

#ifdef DEF_GET_VISS_AXI_WT_QOS_LVL_TVD_3D
def_get_mod_reg_bit(viss, axi_wt_qos_lvl, tvd_3d, addr);
#endif

#endif /* ___VISS__GET___H___ */

/* viss_set.c */

#ifndef ___VISS__SET___C___
#define ___VISS__SET___C___

#ifdef DEF_SET_VISS_VER
void set_viss_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r)
{
	reg_viss_ver_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ver_l = ver_l;
	reg.bits.ver_h = ver_h;
	reg.bits.comp = comp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_VER_VER_L
fun_set_mod_reg_bit(viss, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VER_VER_H
fun_set_mod_reg_bit(viss, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VER_COMP
fun_set_mod_reg_bit(viss, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_FTR
void set_viss_ftr(u32 reg_addr,
		u32 has_vic,
		u32 has_mcsi,
		u32 has_tvd,
		u32 has_isp,
		u32 has_isp_lite,
		u32 m_or_r)
{
	reg_viss_ftr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.has_vic = has_vic;
	reg.bits.has_mcsi = has_mcsi;
	reg.bits.has_tvd = has_tvd;
	reg.bits.has_isp = has_isp;
	reg.bits.has_isp_lite = has_isp_lite;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_FTR_HAS_VIC
fun_set_mod_reg_bit(viss, ftr, has_vic, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_FTR_HAS_MCSI
fun_set_mod_reg_bit(viss, ftr, has_mcsi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_FTR_HAS_TVD
fun_set_mod_reg_bit(viss, ftr, has_tvd, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_FTR_HAS_ISP
fun_set_mod_reg_bit(viss, ftr, has_isp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_FTR_HAS_ISP_LITE
fun_set_mod_reg_bit(viss, ftr, has_isp_lite, addr, val, m_or_r)
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
			u32 m_or_r)
{
	reg_viss_vic_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rst = rst;
	reg.bits.hclk_gat = hclk_gat;
	reg.bits.clk_gat = clk_gat;
	reg.bits.clk_src = clk_src;
	reg.bits.mclk_gat = mclk_gat;
	reg.bits.mclk_src = mclk_src;
	reg.bits.clk_dir = clk_dir;
	reg.bits.mclk_dir = mclk_dir;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_VIC_CFG_RST
fun_set_mod_reg_bit(viss, vic_cfg, rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_HCLK_GAT
fun_set_mod_reg_bit(viss, vic_cfg, hclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_CLK_GAT
fun_set_mod_reg_bit(viss, vic_cfg, clk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_CLK_SRC
fun_set_mod_reg_bit(viss, vic_cfg, clk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_MCLK_GAT
fun_set_mod_reg_bit(viss, vic_cfg, mclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_MCLK_SRC
fun_set_mod_reg_bit(viss, vic_cfg, mclk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_CLK_DIR
fun_set_mod_reg_bit(viss, vic_cfg, clk_dir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_VIC_CFG_MCLK_DIR
fun_set_mod_reg_bit(viss, vic_cfg, mclk_dir, addr, val, m_or_r)
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
			u32 m_or_r)
{
	reg_viss_mcsi_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rst = rst;
	reg.bits.hclk_gat = hclk_gat;
	reg.bits.clk_gat = clk_gat;
	reg.bits.clk_src = clk_src;
	reg.bits.mclk_gat = mclk_gat;
	reg.bits.mclk_src = mclk_src;
	reg.bits.cfgclk_gat = cfgclk_gat;
	reg.bits.clk_dir = clk_dir;
	reg.bits.mclk_dir = mclk_dir;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_RST
fun_set_mod_reg_bit(viss, mcsi_cfg, rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_HCLK_GAT
fun_set_mod_reg_bit(viss, mcsi_cfg, hclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CLK_GAT
fun_set_mod_reg_bit(viss, mcsi_cfg, clk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CLK_SRC
fun_set_mod_reg_bit(viss, mcsi_cfg, clk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_MCLK_GAT
fun_set_mod_reg_bit(viss, mcsi_cfg, mclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_MCLK_SRC
fun_set_mod_reg_bit(viss, mcsi_cfg, mclk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CFGCLK_GAT
fun_set_mod_reg_bit(viss, mcsi_cfg, cfgclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_CLK_DIR
fun_set_mod_reg_bit(viss, mcsi_cfg, clk_dir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MCSI_CFG_MCLK_DIR
fun_set_mod_reg_bit(viss, mcsi_cfg, mclk_dir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_TVD_CFG
void set_viss_tvd_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 clk_dir,
			u32 m_or_r)
{
	reg_viss_tvd_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rst = rst;
	reg.bits.hclk_gat = hclk_gat;
	reg.bits.clk_gat = clk_gat;
	reg.bits.clk_src = clk_src;
	reg.bits.clk_dir = clk_dir;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_TVD_CFG_RST
fun_set_mod_reg_bit(viss, tvd_cfg, rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_TVD_CFG_HCLK_GAT
fun_set_mod_reg_bit(viss, tvd_cfg, hclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_TVD_CFG_CLK_GAT
fun_set_mod_reg_bit(viss, tvd_cfg, clk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_TVD_CFG_CLK_SRC
fun_set_mod_reg_bit(viss, tvd_cfg, clk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_TVD_CFG_CLK_DIR
fun_set_mod_reg_bit(viss, tvd_cfg, clk_dir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_CFG
void set_viss_isp_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 clk_dir,
			u32 m_or_r)
{
	reg_viss_isp_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rst = rst;
	reg.bits.hclk_gat = hclk_gat;
	reg.bits.clk_gat = clk_gat;
	reg.bits.clk_src = clk_src;
	reg.bits.clk_dir = clk_dir;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ISP_CFG_RST
fun_set_mod_reg_bit(viss, isp_cfg, rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_CFG_HCLK_GAT
fun_set_mod_reg_bit(viss, isp_cfg, hclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_CFG_CLK_GAT
fun_set_mod_reg_bit(viss, isp_cfg, clk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_CFG_CLK_SRC
fun_set_mod_reg_bit(viss, isp_cfg, clk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_CFG_CLK_DIR
fun_set_mod_reg_bit(viss, isp_cfg, clk_dir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG
void set_viss_isp_lite_cfg(u32 reg_addr,
			u32 rst,
			u32 hclk_gat,
			u32 clk_gat,
			u32 clk_src,
			u32 clk_dir,
			u32 m_or_r)
{
	reg_viss_isp_lite_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rst = rst;
	reg.bits.hclk_gat = hclk_gat;
	reg.bits.clk_gat = clk_gat;
	reg.bits.clk_src = clk_src;
	reg.bits.clk_dir = clk_dir;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_RST
fun_set_mod_reg_bit(viss, isp_lite_cfg, rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_HCLK_GAT
fun_set_mod_reg_bit(viss, isp_lite_cfg, hclk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_CLK_GAT
fun_set_mod_reg_bit(viss, isp_lite_cfg, clk_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_CLK_SRC
fun_set_mod_reg_bit(viss, isp_lite_cfg, clk_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_LITE_CFG_CLK_DIR
fun_set_mod_reg_bit(viss, isp_lite_cfg, clk_dir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_PIX_SEL
void set_viss_isp_pix_sel(u32 reg_addr,
			u32 isp_pix_sel,
			u32 ispl_pix_sel,
			u32 m_or_r)
{
	reg_viss_isp_pix_sel_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.isp_pix_sel = isp_pix_sel;
	reg.bits.ispl_pix_sel = ispl_pix_sel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ISP_PIX_SEL_ISP_PIX_SEL
fun_set_mod_reg_bit(viss, isp_pix_sel, isp_pix_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ISP_PIX_SEL_ISPL_PIX_SEL
fun_set_mod_reg_bit(viss, isp_pix_sel, ispl_pix_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_PLL_MUX_CTRL
void set_viss_pll_mux_ctrl(u32 reg_addr,
			u32 pllmux_clk_sel,
			u32 adpll_pre_div,
			u32 m_or_r)
{
	reg_viss_pll_mux_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pllmux_clk_sel = pllmux_clk_sel;
	reg.bits.adpll_pre_div = adpll_pre_div;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_PLL_MUX_CTRL_PLLMUX_CLK_SEL
fun_set_mod_reg_bit(viss, pll_mux_ctrl, pllmux_clk_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_PLL_MUX_CTRL_ADPLL_PRE_DIV
fun_set_mod_reg_bit(viss, pll_mux_ctrl, adpll_pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_FAC
void set_viss_adpll_fac(u32 reg_addr,
			u32 n,
			u32 en,
			u32 m_or_r)
{
	reg_viss_adpll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.n = n;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ADPLL_FAC_N
fun_set_mod_reg_bit(viss, adpll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_FAC_EN
fun_set_mod_reg_bit(viss, adpll_fac, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0
void set_viss_adpll_tune0(u32 reg_addr,
			u32 tune0,
			u32 tune1,
			u32 auto_adrange,
			u32 tune2,
			u32 tune3,
			u32 tune4,
			u32 m_or_r)
{
	reg_viss_adpll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune0 = tune0;
	reg.bits.tune1 = tune1;
	reg.bits.auto_adrange = auto_adrange;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE0
fun_set_mod_reg_bit(viss, adpll_tune0, tune0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE1
fun_set_mod_reg_bit(viss, adpll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_AUTO_ADRANGE
fun_set_mod_reg_bit(viss, adpll_tune0, auto_adrange, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE2
fun_set_mod_reg_bit(viss, adpll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE3
fun_set_mod_reg_bit(viss, adpll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE0_TUNE4
fun_set_mod_reg_bit(viss, adpll_tune0, tune4, addr, val, m_or_r)
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
			u32 m_or_r)
{
	reg_viss_adpll_tune1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune0 = tune0;
	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.slewrate = slewrate;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE0
fun_set_mod_reg_bit(viss, adpll_tune1, tune0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE1
fun_set_mod_reg_bit(viss, adpll_tune1, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE2
fun_set_mod_reg_bit(viss, adpll_tune1, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE3
fun_set_mod_reg_bit(viss, adpll_tune1, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_SLEWRATE
fun_set_mod_reg_bit(viss, adpll_tune1, slewrate, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE4
fun_set_mod_reg_bit(viss, adpll_tune1, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE5
fun_set_mod_reg_bit(viss, adpll_tune1, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE6
fun_set_mod_reg_bit(viss, adpll_tune1, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_TUNE1_TUNE7
fun_set_mod_reg_bit(viss, adpll_tune1, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT
void set_viss_adpll_stat(u32 reg_addr,
			u32 ferr,
			u32 flock,
			u32 perr,
			u32 plock,
			u32 m_or_r)
{
	reg_viss_adpll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ferr = ferr;
	reg.bits.flock = flock;
	reg.bits.perr = perr;
	reg.bits.plock = plock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_FERR
fun_set_mod_reg_bit(viss, adpll_stat, ferr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_FLOCK
fun_set_mod_reg_bit(viss, adpll_stat, flock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_PERR
fun_set_mod_reg_bit(viss, adpll_stat, perr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_ADPLL_STAT_PLOCK
fun_set_mod_reg_bit(viss, adpll_stat, plock, addr, val, m_or_r)
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
			u32 m_or_r)
{
	reg_viss_mbist_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.vic_cfg = vic_cfg;
	reg.bits.mcsi_cfg = mcsi_cfg;
	reg.bits.tvd_cfg = tvd_cfg;
	reg.bits.isp_cfg = isp_cfg;
	reg.bits.isp_lite_cfg = isp_lite_cfg;
	reg.bits.vic_sta = vic_sta;
	reg.bits.mcsi_sta = mcsi_sta;
	reg.bits.tvd_sta = tvd_sta;
	reg.bits.isp_sta = isp_sta;
	reg.bits.isp_lite_sta = isp_lite_sta;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_VIC_CFG
fun_set_mod_reg_bit(viss, mbist_cfg, vic_cfg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_MCSI_CFG
fun_set_mod_reg_bit(viss, mbist_cfg, mcsi_cfg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_TVD_CFG
fun_set_mod_reg_bit(viss, mbist_cfg, tvd_cfg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_CFG
fun_set_mod_reg_bit(viss, mbist_cfg, isp_cfg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_LITE_CFG
fun_set_mod_reg_bit(viss, mbist_cfg, isp_lite_cfg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_VIC_STA
fun_set_mod_reg_bit(viss, mbist_cfg, vic_sta, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_MCSI_STA
fun_set_mod_reg_bit(viss, mbist_cfg, mcsi_sta, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_TVD_STA
fun_set_mod_reg_bit(viss, mbist_cfg, tvd_sta, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_STA
fun_set_mod_reg_bit(viss, mbist_cfg, isp_sta, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_MBIST_CFG_ISP_LITE_STA
fun_set_mod_reg_bit(viss, mbist_cfg, isp_lite_sta, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL
void set_viss_axi_rd_qos_lvl(u32 reg_addr,
				u32 vic,
				u32 mcsi,
				u32 isp,
				u32 isp_lite,
				u32 tvd_wb,
				u32 tvd_3d,
				u32 m_or_r)
{
	reg_viss_axi_rd_qos_lvl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.vic = vic;
	reg.bits.mcsi = mcsi;
	reg.bits.isp = isp;
	reg.bits.isp_lite = isp_lite;
	reg.bits.tvd_wb = tvd_wb;
	reg.bits.tvd_3d = tvd_3d;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_VIC
fun_set_mod_reg_bit(viss, axi_rd_qos_lvl, vic, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_MCSI
fun_set_mod_reg_bit(viss, axi_rd_qos_lvl, mcsi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_ISP
fun_set_mod_reg_bit(viss, axi_rd_qos_lvl, isp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_ISP_LITE
fun_set_mod_reg_bit(viss, axi_rd_qos_lvl, isp_lite, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_TVD_WB
fun_set_mod_reg_bit(viss, axi_rd_qos_lvl, tvd_wb, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_RD_QOS_LVL_TVD_3D
fun_set_mod_reg_bit(viss, axi_rd_qos_lvl, tvd_3d, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL
void set_viss_axi_wt_qos_lvl(u32 reg_addr,
				u32 vic,
				u32 mcsi,
				u32 isp,
				u32 isp_lite,
				u32 tvd_wb,
				u32 tvd_3d,
				u32 m_or_r)
{
	reg_viss_axi_wt_qos_lvl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.vic = vic;
	reg.bits.mcsi = mcsi;
	reg.bits.isp = isp;
	reg.bits.isp_lite = isp_lite;
	reg.bits.tvd_wb = tvd_wb;
	reg.bits.tvd_3d = tvd_3d;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_VIC
fun_set_mod_reg_bit(viss, axi_wt_qos_lvl, vic, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_MCSI
fun_set_mod_reg_bit(viss, axi_wt_qos_lvl, mcsi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_ISP
fun_set_mod_reg_bit(viss, axi_wt_qos_lvl, isp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_ISP_LITE
fun_set_mod_reg_bit(viss, axi_wt_qos_lvl, isp_lite, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_TVD_WB
fun_set_mod_reg_bit(viss, axi_wt_qos_lvl, tvd_wb, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VISS_AXI_WT_QOS_LVL_TVD_3D
fun_set_mod_reg_bit(viss, axi_wt_qos_lvl, tvd_3d, addr, val, m_or_r)
#endif

#endif /* ___VISS__SET___C___ */

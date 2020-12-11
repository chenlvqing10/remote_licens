/* cpu_ctrl_set.c */

#ifndef ___CPU_CTRL__SET___C___
#define ___CPU_CTRL__SET___C___

#ifdef DEF_SET_CPU_CTRL_RST_CTRL
void set_cpu_ctrl_rst_ctrl(u32 reg_addr,
			u32 ncorereset0,
			u32 ncorereset1,
			u32 ncoreporeset0,
			u32 ncoreporeset1,
			u32 ndbgreset0,
			u32 ndbgreset1,
			u32 l1rstdisable0,
			u32 l1rstdisable1,
			u32 m_or_r)
{
	reg_cpu_ctrl_rst_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ncorereset0 = ncorereset0;
	reg.bits.ncorereset1 = ncorereset1;
	reg.bits.ncoreporeset0 = ncoreporeset0;
	reg.bits.ncoreporeset1 = ncoreporeset1;
	reg.bits.ndbgreset0 = ndbgreset0;
	reg.bits.ndbgreset1 = ndbgreset1;
	reg.bits.l1rstdisable0 = l1rstdisable0;
	reg.bits.l1rstdisable1 = l1rstdisable1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_NCORERESET0
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, ncorereset0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_NCORERESET1
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, ncorereset1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_NCOREPORESET0
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, ncoreporeset0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_NCOREPORESET1
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, ncoreporeset1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_NDBGRESET0
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, ndbgreset0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_NDBGRESET1
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, ndbgreset1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_L1RSTDISABLE0
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, l1rstdisable0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_RST_CTRL_L1RSTDISABLE1
fun_set_mod_reg_bit(cpu_ctrl, rst_ctrl, l1rstdisable1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R
void set_cpu_ctrl_ctrl_r(u32 reg_addr,
			u32 eventi,
			u32 cfgsdisable,
			u32 cp15sdisable0,
			u32 cp15sdisable1,
			u32 cpu1_gat,
			u32 acinactm,
			u32 m_or_r)
{
	reg_cpu_ctrl_ctrl_r_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.eventi = eventi;
	reg.bits.cfgsdisable = cfgsdisable;
	reg.bits.cp15sdisable0 = cp15sdisable0;
	reg.bits.cp15sdisable1 = cp15sdisable1;
	reg.bits.cpu1_gat = cpu1_gat;
	reg.bits.acinactm = acinactm;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R_EVENTI
fun_set_mod_reg_bit(cpu_ctrl, ctrl_r, eventi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R_CFGSDISABLE
fun_set_mod_reg_bit(cpu_ctrl, ctrl_r, cfgsdisable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R_CP15SDISABLE0
fun_set_mod_reg_bit(cpu_ctrl, ctrl_r, cp15sdisable0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R_CP15SDISABLE1
fun_set_mod_reg_bit(cpu_ctrl, ctrl_r, cp15sdisable1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R_CPU1_GAT
fun_set_mod_reg_bit(cpu_ctrl, ctrl_r, cpu1_gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CTRL_R_ACINACTM
fun_set_mod_reg_bit(cpu_ctrl, ctrl_r, acinactm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CFG_R
void set_cpu_ctrl_cfg_r(u32 reg_addr,
			u32 broadcastinner,
			u32 broadcastouter,
			u32 broadcastcachemaint,
			u32 sysbardisable,
			u32 m_or_r)
{
	reg_cpu_ctrl_cfg_r_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.broadcastinner = broadcastinner;
	reg.bits.broadcastouter = broadcastouter;
	reg.bits.broadcastcachemaint = broadcastcachemaint;
	reg.bits.sysbardisable = sysbardisable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_CFG_R_BROADCASTINNER
fun_set_mod_reg_bit(cpu_ctrl, cfg_r, broadcastinner, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CFG_R_BROADCASTOUTER
fun_set_mod_reg_bit(cpu_ctrl, cfg_r, broadcastouter, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CFG_R_BROADCASTCACHEMAINT
fun_set_mod_reg_bit(cpu_ctrl, cfg_r, broadcastcachemaint, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CFG_R_SYSBARDISABLE
fun_set_mod_reg_bit(cpu_ctrl, cfg_r, sysbardisable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R
void set_cpu_ctrl_status_r(u32 reg_addr,
			u32 smpnamp0,
			u32 smpnamp1,
			u32 standbywfe0,
			u32 standbywfe1,
			u32 standbywfi0,
			u32 standbywfi1,
			u32 standbywfil2,
			u32 m_or_r)
{
	reg_cpu_ctrl_status_r_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.smpnamp0 = smpnamp0;
	reg.bits.smpnamp1 = smpnamp1;
	reg.bits.standbywfe0 = standbywfe0;
	reg.bits.standbywfe1 = standbywfe1;
	reg.bits.standbywfi0 = standbywfi0;
	reg.bits.standbywfi1 = standbywfi1;
	reg.bits.standbywfil2 = standbywfil2;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_SMPNAMP0
fun_set_mod_reg_bit(cpu_ctrl, status_r, smpnamp0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_SMPNAMP1
fun_set_mod_reg_bit(cpu_ctrl, status_r, smpnamp1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_STANDBYWFE0
fun_set_mod_reg_bit(cpu_ctrl, status_r, standbywfe0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_STANDBYWFE1
fun_set_mod_reg_bit(cpu_ctrl, status_r, standbywfe1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_STANDBYWFI0
fun_set_mod_reg_bit(cpu_ctrl, status_r, standbywfi0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_STANDBYWFI1
fun_set_mod_reg_bit(cpu_ctrl, status_r, standbywfi1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_STATUS_R_STANDBYWFIL2
fun_set_mod_reg_bit(cpu_ctrl, status_r, standbywfil2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_GENER_REG
void set_cpu_ctrl_gener_reg(u32 reg_addr,
				u32 gpr,
				u32 m_or_r)
{
	reg_cpu_ctrl_gener_reg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.gpr = gpr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_GENER_REG_GPR
fun_set_mod_reg_bit(cpu_ctrl, gener_reg, gpr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CPU_CLK_SEL
void set_cpu_ctrl_cpu_clk_sel(u32 reg_addr,
				u32 src,
				u32 src_div,
				u32 dbg_en,
				u32 en,
				u32 m_or_r)
{
	reg_cpu_ctrl_cpu_clk_sel_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src = src;
	reg.bits.src_div = src_div;
	reg.bits.dbg_en = dbg_en;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_CPU_CLK_SEL_SRC
fun_set_mod_reg_bit(cpu_ctrl, cpu_clk_sel, src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CPU_CLK_SEL_SRC_DIV
fun_set_mod_reg_bit(cpu_ctrl, cpu_clk_sel, src_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CPU_CLK_SEL_DBG_EN
fun_set_mod_reg_bit(cpu_ctrl, cpu_clk_sel, dbg_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CPU_CLK_SEL_EN
fun_set_mod_reg_bit(cpu_ctrl, cpu_clk_sel, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_FAC
void set_cpu_ctrl_adpll_fac(u32 reg_addr,
				u32 pre_div,
				u32 n,
				u32 m_or_r)
{
	reg_cpu_ctrl_adpll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_FAC_PRE_DIV
fun_set_mod_reg_bit(cpu_ctrl, adpll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_FAC_N
fun_set_mod_reg_bit(cpu_ctrl, adpll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0
void set_cpu_ctrl_adpll_tune0(u32 reg_addr,
				u32 tune0,
				u32 tune1,
				u32 auto_adrange,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 m_or_r)
{
	reg_cpu_ctrl_adpll_tune0_t reg;

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

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0_TUNE0
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune0, tune0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0_TUNE1
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0_AUTO_ADRANGE
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune0, auto_adrange, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0_TUNE2
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0_TUNE3
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE0_TUNE4
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1
void set_cpu_ctrl_adpll_tune1(u32 reg_addr,
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
	reg_cpu_ctrl_adpll_tune1_t reg;

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

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE0
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE1
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE2
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE3
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_SLEWRATE
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, slewrate, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE4
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE5
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE6
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_TUNE1_TUNE7
fun_set_mod_reg_bit(cpu_ctrl, adpll_tune1, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_STAT
void set_cpu_ctrl_adpll_stat(u32 reg_addr,
				u32 ferr,
				u32 flock,
				u32 perr,
				u32 plock,
				u32 m_or_r)
{
	reg_cpu_ctrl_adpll_stat_t reg;

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

#ifdef DEF_SET_CPU_CTRL_ADPLL_STAT_FERR
fun_set_mod_reg_bit(cpu_ctrl, adpll_stat, ferr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_STAT_FLOCK
fun_set_mod_reg_bit(cpu_ctrl, adpll_stat, flock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_STAT_PERR
fun_set_mod_reg_bit(cpu_ctrl, adpll_stat, perr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_ADPLL_STAT_PLOCK
fun_set_mod_reg_bit(cpu_ctrl, adpll_stat, plock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CACHE_CFG0
void set_cpu_ctrl_cache_cfg0(u32 reg_addr,
				u32 parameter,
				u32 m_or_r)
{
	reg_cpu_ctrl_cache_cfg0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.parameter = parameter;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_CACHE_CFG0_PARAMETER
fun_set_mod_reg_bit(cpu_ctrl, cache_cfg0, parameter, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_CACHE_CFG1
void set_cpu_ctrl_cache_cfg1(u32 reg_addr,
				u32 parameter,
				u32 m_or_r)
{
	reg_cpu_ctrl_cache_cfg1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.parameter = parameter;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_CACHE_CFG1_PARAMETER
fun_set_mod_reg_bit(cpu_ctrl, cache_cfg1, parameter, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0
void set_cpu_ctrl_dbg_reg0(u32 reg_addr,
			u32 dbgpwrdup0,
			u32 dbgpwrdup1,
			u32 dbgpwrupreq0,
			u32 dbgpwrupreq1,
			u32 dbgnopwrdwn0,
			u32 dbgnopwrdwn1,
			u32 m_or_r)
{
	reg_cpu_ctrl_dbg_reg0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dbgpwrdup0 = dbgpwrdup0;
	reg.bits.dbgpwrdup1 = dbgpwrdup1;
	reg.bits.dbgpwrupreq0 = dbgpwrupreq0;
	reg.bits.dbgpwrupreq1 = dbgpwrupreq1;
	reg.bits.dbgnopwrdwn0 = dbgnopwrdwn0;
	reg.bits.dbgnopwrdwn1 = dbgnopwrdwn1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0_DBGPWRDUP0
fun_set_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrdup0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0_DBGPWRDUP1
fun_set_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrdup1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0_DBGPWRUPREQ0
fun_set_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrupreq0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0_DBGPWRUPREQ1
fun_set_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrupreq1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0_DBGNOPWRDWN0
fun_set_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgnopwrdwn0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_DBG_REG0_DBGNOPWRDWN1
fun_set_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgnopwrdwn1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_VER
void set_cpu_ctrl_ver(u32 reg_addr,
			u32 ver_l,
			u32 ver_h,
			u32 comp,
			u32 m_or_r)
{
	reg_cpu_ctrl_ver_t reg;

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

#ifdef DEF_SET_CPU_CTRL_VER_VER_L
fun_set_mod_reg_bit(cpu_ctrl, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_VER_VER_H
fun_set_mod_reg_bit(cpu_ctrl, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_VER_COMP
fun_set_mod_reg_bit(cpu_ctrl, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL
void set_cpu_ctrl_c_rst_ctrl(u32 reg_addr,
				u32 cluster_rst,
				u32 soc_dbg_rst,
				u32 dap_rst,
				u32 jtag_rst,
				u32 gic_rst,
				u32 l2_rst,
				u32 l2rstdisable,
				u32 m_or_r)
{
	reg_cpu_ctrl_c_rst_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cluster_rst = cluster_rst;
	reg.bits.soc_dbg_rst = soc_dbg_rst;
	reg.bits.dap_rst = dap_rst;
	reg.bits.jtag_rst = jtag_rst;
	reg.bits.gic_rst = gic_rst;
	reg.bits.l2_rst = l2_rst;
	reg.bits.l2rstdisable = l2rstdisable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_CLUSTER_RST
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, cluster_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_SOC_DBG_RST
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, soc_dbg_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_DAP_RST
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, dap_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_JTAG_RST
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, jtag_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_GIC_RST
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, gic_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_L2_RST
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, l2_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_CPU_CTRL_C_RST_CTRL_L2RSTDISABLE
fun_set_mod_reg_bit(cpu_ctrl, c_rst_ctrl, l2rstdisable, addr, val, m_or_r)
#endif

#endif /* ___CPU_CTRL__SET___C___ */

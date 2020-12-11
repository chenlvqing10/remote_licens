/* cpu_ctrl_get.c */

#ifndef ___CPU_CTRL__GET___C___
#define ___CPU_CTRL__GET___C___

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_NCORERESET0
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, ncorereset0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_NCORERESET1
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, ncorereset1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_NCOREPORESET0
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, ncoreporeset0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_NCOREPORESET1
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, ncoreporeset1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_NDBGRESET0
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, ndbgreset0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_NDBGRESET1
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, ndbgreset1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_L1RSTDISABLE0
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, l1rstdisable0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_RST_CTRL_L1RSTDISABLE1
fun_get_mod_reg_bit(cpu_ctrl, rst_ctrl, l1rstdisable1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CTRL_R_EVENTI
fun_get_mod_reg_bit(cpu_ctrl, ctrl_r, eventi, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CTRL_R_CFGSDISABLE
fun_get_mod_reg_bit(cpu_ctrl, ctrl_r, cfgsdisable, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CTRL_R_CP15SDISABLE0
fun_get_mod_reg_bit(cpu_ctrl, ctrl_r, cp15sdisable0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CTRL_R_CP15SDISABLE1
fun_get_mod_reg_bit(cpu_ctrl, ctrl_r, cp15sdisable1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CTRL_R_CPU1_GAT
fun_get_mod_reg_bit(cpu_ctrl, ctrl_r, cpu1_gat, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CTRL_R_ACINACTM
fun_get_mod_reg_bit(cpu_ctrl, ctrl_r, acinactm, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CFG_R_BROADCASTINNER
fun_get_mod_reg_bit(cpu_ctrl, cfg_r, broadcastinner, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CFG_R_BROADCASTOUTER
fun_get_mod_reg_bit(cpu_ctrl, cfg_r, broadcastouter, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CFG_R_BROADCASTCACHEMAINT
fun_get_mod_reg_bit(cpu_ctrl, cfg_r, broadcastcachemaint, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CFG_R_SYSBARDISABLE
fun_get_mod_reg_bit(cpu_ctrl, cfg_r, sysbardisable, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_SMPNAMP0
fun_get_mod_reg_bit(cpu_ctrl, status_r, smpnamp0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_SMPNAMP1
fun_get_mod_reg_bit(cpu_ctrl, status_r, smpnamp1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_STANDBYWFE0
fun_get_mod_reg_bit(cpu_ctrl, status_r, standbywfe0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_STANDBYWFE1
fun_get_mod_reg_bit(cpu_ctrl, status_r, standbywfe1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_STANDBYWFI0
fun_get_mod_reg_bit(cpu_ctrl, status_r, standbywfi0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_STANDBYWFI1
fun_get_mod_reg_bit(cpu_ctrl, status_r, standbywfi1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_STATUS_R_STANDBYWFIL2
fun_get_mod_reg_bit(cpu_ctrl, status_r, standbywfil2, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_GENER_REG_GPR
fun_get_mod_reg_bit(cpu_ctrl, gener_reg, gpr, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CPU_CLK_SEL_SRC
fun_get_mod_reg_bit(cpu_ctrl, cpu_clk_sel, src, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CPU_CLK_SEL_SRC_DIV
fun_get_mod_reg_bit(cpu_ctrl, cpu_clk_sel, src_div, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CPU_CLK_SEL_DBG_EN
fun_get_mod_reg_bit(cpu_ctrl, cpu_clk_sel, dbg_en, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CPU_CLK_SEL_EN
fun_get_mod_reg_bit(cpu_ctrl, cpu_clk_sel, en, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_FAC_PRE_DIV
fun_get_mod_reg_bit(cpu_ctrl, adpll_fac, pre_div, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_FAC_N
fun_get_mod_reg_bit(cpu_ctrl, adpll_fac, n, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE0_TUNE0
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune0, tune0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE0_TUNE1
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune0, tune1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE0_AUTO_ADRANGE
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune0, auto_adrange, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE0_TUNE2
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune0, tune2, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE0_TUNE3
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune0, tune3, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE0_TUNE4
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune0, tune4, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE0
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE1
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE2
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune2, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE3
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune3, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_SLEWRATE
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, slewrate, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE4
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune4, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE5
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune5, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE6
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune6, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_TUNE1_TUNE7
fun_get_mod_reg_bit(cpu_ctrl, adpll_tune1, tune7, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_STAT_FERR
fun_get_mod_reg_bit(cpu_ctrl, adpll_stat, ferr, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_STAT_FLOCK
fun_get_mod_reg_bit(cpu_ctrl, adpll_stat, flock, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_STAT_PERR
fun_get_mod_reg_bit(cpu_ctrl, adpll_stat, perr, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_ADPLL_STAT_PLOCK
fun_get_mod_reg_bit(cpu_ctrl, adpll_stat, plock, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CACHE_CFG0_PARAMETER
fun_get_mod_reg_bit(cpu_ctrl, cache_cfg0, parameter, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_CACHE_CFG1_PARAMETER
fun_get_mod_reg_bit(cpu_ctrl, cache_cfg1, parameter, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_DBG_REG0_DBGPWRDUP0
fun_get_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrdup0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_DBG_REG0_DBGPWRDUP1
fun_get_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrdup1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_DBG_REG0_DBGPWRUPREQ0
fun_get_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrupreq0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_DBG_REG0_DBGPWRUPREQ1
fun_get_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgpwrupreq1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_DBG_REG0_DBGNOPWRDWN0
fun_get_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgnopwrdwn0, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_DBG_REG0_DBGNOPWRDWN1
fun_get_mod_reg_bit(cpu_ctrl, dbg_reg0, dbgnopwrdwn1, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_VER_VER_L
fun_get_mod_reg_bit(cpu_ctrl, ver, ver_l, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_VER_VER_H
fun_get_mod_reg_bit(cpu_ctrl, ver, ver_h, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_VER_COMP
fun_get_mod_reg_bit(cpu_ctrl, ver, comp, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_CLUSTER_RST
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, cluster_rst, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_SOC_DBG_RST
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, soc_dbg_rst, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_DAP_RST
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, dap_rst, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_JTAG_RST
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, jtag_rst, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_GIC_RST
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, gic_rst, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_L2_RST
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, l2_rst, addr)
#endif

#ifdef DEF_GET_CPU_CTRL_C_RST_CTRL_L2RSTDISABLE
fun_get_mod_reg_bit(cpu_ctrl, c_rst_ctrl, l2rstdisable, addr)
#endif

#endif /* ___CPU_CTRL__GET___C___ */

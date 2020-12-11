/* wdog_set.c */

#ifndef ___WDOG__SET___C___
#define ___WDOG__SET___C___

#ifdef DEF_SET_WDOG_WDOG_VER
void set_wdog_wdog_ver(u32 reg_addr,
			u32 ver_l,
			u32 ver_h,
			u32 comp,
			u32 m_or_r)
{
	reg_wdog_wdog_ver_t reg;

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

#ifdef DEF_SET_WDOG_WDOG_VER_VER_L
fun_set_mod_reg_bit(wdog, wdog_ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_VER_VER_H
fun_set_mod_reg_bit(wdog, wdog_ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_VER_COMP
fun_set_mod_reg_bit(wdog, wdog_ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CLK_CTRL
void set_wdog_wdog_clk_ctrl(u32 reg_addr,
				u32 sel,
				u32 m_or_r)
{
	reg_wdog_wdog_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sel = sel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_CLK_CTRL_SEL
fun_set_mod_reg_bit(wdog, wdog_clk_ctrl, sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CTRL
void set_wdog_wdog_ctrl(u32 reg_addr,
			u32 en,
			u32 rmod,
			u32 rpl,
			u32 su,
			u32 m_or_r)
{
	reg_wdog_wdog_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.rmod = rmod;
	reg.bits.rpl = rpl;
	reg.bits.su = su;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_CTRL_EN
fun_set_mod_reg_bit(wdog, wdog_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CTRL_RMOD
fun_set_mod_reg_bit(wdog, wdog_ctrl, rmod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CTRL_RPL
fun_set_mod_reg_bit(wdog, wdog_ctrl, rpl, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CTRL_SU
fun_set_mod_reg_bit(wdog, wdog_ctrl, su, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_TMRPERIOD
void set_wdog_wdog_tmrperiod(u32 reg_addr,
				u32 tmrperiod,
				u32 m_or_r)
{
	reg_wdog_wdog_tmrperiod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tmrperiod = tmrperiod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_TMRPERIOD_TMRPERIOD
fun_set_mod_reg_bit(wdog, wdog_tmrperiod, tmrperiod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CNT_CUR_VAL
void set_wdog_wdog_cnt_cur_val(u32 reg_addr,
				u32 cnt_cur_val,
				u32 m_or_r)
{
	reg_wdog_wdog_cnt_cur_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cnt_cur_val = cnt_cur_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_CNT_CUR_VAL_CNT_CUR_VAL
fun_set_mod_reg_bit(wdog, wdog_cnt_cur_val, cnt_cur_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_RESTART
void set_wdog_wdog_restart(u32 reg_addr,
			u32 restart,
			u32 m_or_r)
{
	reg_wdog_wdog_restart_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.restart = restart;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_RESTART_RESTART
fun_set_mod_reg_bit(wdog, wdog_restart, restart, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_INT_EN
void set_wdog_wdog_int_en(u32 reg_addr,
			u32 enable,
			u32 m_or_r)
{
	reg_wdog_wdog_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_INT_EN_ENABLE
fun_set_mod_reg_bit(wdog, wdog_int_en, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_CLR_INT_PENDING
void set_wdog_wdog_clr_int_pending(u32 reg_addr,
				u32 clr,
				u32 m_or_r)
{
	reg_wdog_wdog_clr_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_CLR_INT_PENDING_CLR
fun_set_mod_reg_bit(wdog, wdog_clr_int_pending, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_WDOG_WDOG_INT_PENDING
void set_wdog_wdog_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_wdog_wdog_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_WDOG_WDOG_INT_PENDING_PENDING
fun_set_mod_reg_bit(wdog, wdog_int_pending, pending, addr, val, m_or_r)
#endif

#endif /* ___WDOG__SET___C___ */

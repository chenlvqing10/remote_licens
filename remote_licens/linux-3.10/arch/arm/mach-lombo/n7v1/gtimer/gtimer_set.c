/* gtimer_set.c */

#ifndef ___GTIMER__SET___C___
#define ___GTIMER__SET___C___

#ifdef DEF_SET_GTIMER_GTMR_VER
void set_gtimer_gtmr_ver(u32 reg_addr,
			u32 ver_l,
			u32 ver_h,
			u32 comp,
			u32 m_or_r)
{
	reg_gtimer_gtmr_ver_t reg;

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

#ifdef DEF_SET_GTIMER_GTMR_VER_VER_L
fun_set_mod_reg_bit(gtimer, gtmr_ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR_VER_VER_H
fun_set_mod_reg_bit(gtimer, gtmr_ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR_VER_COMP
fun_set_mod_reg_bit(gtimer, gtmr_ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CLK_CTRL
void set_gtimer_gtmr0_clk_ctrl(u32 reg_addr,
				u32 clk_div,
				u32 clk_sel,
				u32 m_or_r)
{
	reg_gtimer_gtmr0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clk_div = clk_div;
	reg.bits.clk_sel = clk_sel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CLK_CTRL_CLK_DIV
fun_set_mod_reg_bit(gtimer, gtmr0_clk_ctrl, clk_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CLK_CTRL_CLK_SEL
fun_set_mod_reg_bit(gtimer, gtmr0_clk_ctrl, clk_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_STORE_VAL
void set_gtimer_gtmr0_store_val(u32 reg_addr,
				u32 store_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr0_store_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store_val = store_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_STORE_VAL_STORE_VAL
fun_set_mod_reg_bit(gtimer, gtmr0_store_val, store_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CUR_VAL
void set_gtimer_gtmr0_cur_val(u32 reg_addr,
				u32 cur_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr0_cur_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cur_val = cur_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CUR_VAL_CUR_VAL
fun_set_mod_reg_bit(gtimer, gtmr0_cur_val, cur_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CTRL
void set_gtimer_gtmr0_ctrl(u32 reg_addr,
			u32 enable,
			u32 mode,
			u32 direction,
			u32 nrdy,
			u32 m_or_r)
{
	reg_gtimer_gtmr0_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;
	reg.bits.mode = mode;
	reg.bits.direction = direction;
	reg.bits.nrdy = nrdy;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CTRL_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr0_ctrl, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CTRL_MODE
fun_set_mod_reg_bit(gtimer, gtmr0_ctrl, mode, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CTRL_DIRECTION
fun_set_mod_reg_bit(gtimer, gtmr0_ctrl, direction, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CTRL_NRDY
fun_set_mod_reg_bit(gtimer, gtmr0_ctrl, nrdy, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_INT_EN
void set_gtimer_gtmr0_int_en(u32 reg_addr,
				u32 enable,
				u32 m_or_r)
{
	reg_gtimer_gtmr0_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_INT_EN_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr0_int_en, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CLR_INT_PENDING
void set_gtimer_gtmr0_clr_int_pending(u32 reg_addr,
					u32 clr,
					u32 m_or_r)
{
	reg_gtimer_gtmr0_clr_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_CLR_INT_PENDING_CLR
fun_set_mod_reg_bit(gtimer, gtmr0_clr_int_pending, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR0_INT_PENDING
void set_gtimer_gtmr0_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_gtimer_gtmr0_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR0_INT_PENDING_PENDING
fun_set_mod_reg_bit(gtimer, gtmr0_int_pending, pending, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CLK_CTRL
void set_gtimer_gtmr1_clk_ctrl(u32 reg_addr,
				u32 clk_div,
				u32 clk_sel,
				u32 m_or_r)
{
	reg_gtimer_gtmr1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clk_div = clk_div;
	reg.bits.clk_sel = clk_sel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CLK_CTRL_CLK_DIV
fun_set_mod_reg_bit(gtimer, gtmr1_clk_ctrl, clk_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CLK_CTRL_CLK_SEL
fun_set_mod_reg_bit(gtimer, gtmr1_clk_ctrl, clk_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_STORE_VAL
void set_gtimer_gtmr1_store_val(u32 reg_addr,
				u32 store_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr1_store_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store_val = store_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_STORE_VAL_STORE_VAL
fun_set_mod_reg_bit(gtimer, gtmr1_store_val, store_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CUR_VAL
void set_gtimer_gtmr1_cur_val(u32 reg_addr,
				u32 cur_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr1_cur_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cur_val = cur_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CUR_VAL_CUR_VAL
fun_set_mod_reg_bit(gtimer, gtmr1_cur_val, cur_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CTRL
void set_gtimer_gtmr1_ctrl(u32 reg_addr,
			u32 enable,
			u32 mode,
			u32 direction,
			u32 nrdy,
			u32 m_or_r)
{
	reg_gtimer_gtmr1_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;
	reg.bits.mode = mode;
	reg.bits.direction = direction;
	reg.bits.nrdy = nrdy;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CTRL_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr1_ctrl, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CTRL_MODE
fun_set_mod_reg_bit(gtimer, gtmr1_ctrl, mode, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CTRL_DIRECTION
fun_set_mod_reg_bit(gtimer, gtmr1_ctrl, direction, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CTRL_NRDY
fun_set_mod_reg_bit(gtimer, gtmr1_ctrl, nrdy, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_INT_EN
void set_gtimer_gtmr1_int_en(u32 reg_addr,
				u32 enable,
				u32 m_or_r)
{
	reg_gtimer_gtmr1_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_INT_EN_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr1_int_en, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CLR_INT_PENDING
void set_gtimer_gtmr1_clr_int_pending(u32 reg_addr,
					u32 clr,
					u32 m_or_r)
{
	reg_gtimer_gtmr1_clr_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_CLR_INT_PENDING_CLR
fun_set_mod_reg_bit(gtimer, gtmr1_clr_int_pending, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR1_INT_PENDING
void set_gtimer_gtmr1_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_gtimer_gtmr1_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR1_INT_PENDING_PENDING
fun_set_mod_reg_bit(gtimer, gtmr1_int_pending, pending, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CLK_CTRL
void set_gtimer_gtmr2_clk_ctrl(u32 reg_addr,
				u32 clk_div,
				u32 clk_sel,
				u32 m_or_r)
{
	reg_gtimer_gtmr2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clk_div = clk_div;
	reg.bits.clk_sel = clk_sel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CLK_CTRL_CLK_DIV
fun_set_mod_reg_bit(gtimer, gtmr2_clk_ctrl, clk_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CLK_CTRL_CLK_SEL
fun_set_mod_reg_bit(gtimer, gtmr2_clk_ctrl, clk_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_STORE_VAL
void set_gtimer_gtmr2_store_val(u32 reg_addr,
				u32 store_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr2_store_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store_val = store_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_STORE_VAL_STORE_VAL
fun_set_mod_reg_bit(gtimer, gtmr2_store_val, store_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CUR_VAL
void set_gtimer_gtmr2_cur_val(u32 reg_addr,
				u32 cur_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr2_cur_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cur_val = cur_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CUR_VAL_CUR_VAL
fun_set_mod_reg_bit(gtimer, gtmr2_cur_val, cur_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CTRL
void set_gtimer_gtmr2_ctrl(u32 reg_addr,
			u32 enable,
			u32 mode,
			u32 direction,
			u32 nrdy,
			u32 m_or_r)
{
	reg_gtimer_gtmr2_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;
	reg.bits.mode = mode;
	reg.bits.direction = direction;
	reg.bits.nrdy = nrdy;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CTRL_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr2_ctrl, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CTRL_MODE
fun_set_mod_reg_bit(gtimer, gtmr2_ctrl, mode, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CTRL_DIRECTION
fun_set_mod_reg_bit(gtimer, gtmr2_ctrl, direction, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CTRL_NRDY
fun_set_mod_reg_bit(gtimer, gtmr2_ctrl, nrdy, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_INT_EN
void set_gtimer_gtmr2_int_en(u32 reg_addr,
				u32 enable,
				u32 m_or_r)
{
	reg_gtimer_gtmr2_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_INT_EN_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr2_int_en, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CLR_INT_PENDING
void set_gtimer_gtmr2_clr_int_pending(u32 reg_addr,
					u32 clr,
					u32 m_or_r)
{
	reg_gtimer_gtmr2_clr_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_CLR_INT_PENDING_CLR
fun_set_mod_reg_bit(gtimer, gtmr2_clr_int_pending, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR2_INT_PENDING
void set_gtimer_gtmr2_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_gtimer_gtmr2_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR2_INT_PENDING_PENDING
fun_set_mod_reg_bit(gtimer, gtmr2_int_pending, pending, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CLK_CTRL
void set_gtimer_gtmr3_clk_ctrl(u32 reg_addr,
				u32 clk_div,
				u32 clk_sel,
				u32 m_or_r)
{
	reg_gtimer_gtmr3_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clk_div = clk_div;
	reg.bits.clk_sel = clk_sel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CLK_CTRL_CLK_DIV
fun_set_mod_reg_bit(gtimer, gtmr3_clk_ctrl, clk_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CLK_CTRL_CLK_SEL
fun_set_mod_reg_bit(gtimer, gtmr3_clk_ctrl, clk_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_STORE_VAL
void set_gtimer_gtmr3_store_val(u32 reg_addr,
				u32 store_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr3_store_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store_val = store_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_STORE_VAL_STORE_VAL
fun_set_mod_reg_bit(gtimer, gtmr3_store_val, store_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CUR_VAL
void set_gtimer_gtmr3_cur_val(u32 reg_addr,
				u32 cur_val,
				u32 m_or_r)
{
	reg_gtimer_gtmr3_cur_val_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cur_val = cur_val;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CUR_VAL_CUR_VAL
fun_set_mod_reg_bit(gtimer, gtmr3_cur_val, cur_val, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CTRL
void set_gtimer_gtmr3_ctrl(u32 reg_addr,
			u32 enable,
			u32 mode,
			u32 direction,
			u32 nrdy,
			u32 m_or_r)
{
	reg_gtimer_gtmr3_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;
	reg.bits.mode = mode;
	reg.bits.direction = direction;
	reg.bits.nrdy = nrdy;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CTRL_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr3_ctrl, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CTRL_MODE
fun_set_mod_reg_bit(gtimer, gtmr3_ctrl, mode, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CTRL_DIRECTION
fun_set_mod_reg_bit(gtimer, gtmr3_ctrl, direction, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CTRL_NRDY
fun_set_mod_reg_bit(gtimer, gtmr3_ctrl, nrdy, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_INT_EN
void set_gtimer_gtmr3_int_en(u32 reg_addr,
				u32 enable,
				u32 m_or_r)
{
	reg_gtimer_gtmr3_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enable = enable;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_INT_EN_ENABLE
fun_set_mod_reg_bit(gtimer, gtmr3_int_en, enable, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CLR_INT_PENDING
void set_gtimer_gtmr3_clr_int_pending(u32 reg_addr,
					u32 clr,
					u32 m_or_r)
{
	reg_gtimer_gtmr3_clr_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_CLR_INT_PENDING_CLR
fun_set_mod_reg_bit(gtimer, gtmr3_clr_int_pending, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GTIMER_GTMR3_INT_PENDING
void set_gtimer_gtmr3_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_gtimer_gtmr3_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GTIMER_GTMR3_INT_PENDING_PENDING
fun_set_mod_reg_bit(gtimer, gtmr3_int_pending, pending, addr, val, m_or_r)
#endif

#endif /* ___GTIMER__SET___C___ */

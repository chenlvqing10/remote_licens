/* pwm_set.c */

#ifndef ___PWM__SET___C___
#define ___PWM__SET___C___

#ifdef DEF_SET_PWM_VER
void set_pwm_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r)
{
	reg_pwm_ver_t reg;

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

#ifdef DEF_SET_PWM_VER_VER_L
fun_set_mod_reg_bit(pwm, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_VER_VER_H
fun_set_mod_reg_bit(pwm, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_VER_COMP
fun_set_mod_reg_bit(pwm, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE
void set_pwm_pwm_ce(u32 reg_addr,
			u32 ce0,
			u32 ce1,
			u32 ce2,
			u32 ce3,
			u32 ce4,
			u32 ce5,
			u32 ce6,
			u32 ce7,
			u32 ce8,
			u32 ce9,
			u32 m_or_r)
{
	reg_pwm_pwm_ce_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ce0 = ce0;
	reg.bits.ce1 = ce1;
	reg.bits.ce2 = ce2;
	reg.bits.ce3 = ce3;
	reg.bits.ce4 = ce4;
	reg.bits.ce5 = ce5;
	reg.bits.ce6 = ce6;
	reg.bits.ce7 = ce7;
	reg.bits.ce8 = ce8;
	reg.bits.ce9 = ce9;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE0
fun_set_mod_reg_bit(pwm, pwm_ce, ce0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE1
fun_set_mod_reg_bit(pwm, pwm_ce, ce1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE2
fun_set_mod_reg_bit(pwm, pwm_ce, ce2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE3
fun_set_mod_reg_bit(pwm, pwm_ce, ce3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE4
fun_set_mod_reg_bit(pwm, pwm_ce, ce4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE5
fun_set_mod_reg_bit(pwm, pwm_ce, ce5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE6
fun_set_mod_reg_bit(pwm, pwm_ce, ce6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE7
fun_set_mod_reg_bit(pwm, pwm_ce, ce7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE8
fun_set_mod_reg_bit(pwm, pwm_ce, ce8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CE_CE9
fun_set_mod_reg_bit(pwm, pwm_ce, ce9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE
void set_pwm_pwm_pe(u32 reg_addr,
			u32 pe0,
			u32 pe1,
			u32 pe2,
			u32 pe3,
			u32 pe4,
			u32 pe5,
			u32 pe6,
			u32 pe7,
			u32 pe8,
			u32 pe9,
			u32 m_or_r)
{
	reg_pwm_pwm_pe_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pe0 = pe0;
	reg.bits.pe1 = pe1;
	reg.bits.pe2 = pe2;
	reg.bits.pe3 = pe3;
	reg.bits.pe4 = pe4;
	reg.bits.pe5 = pe5;
	reg.bits.pe6 = pe6;
	reg.bits.pe7 = pe7;
	reg.bits.pe8 = pe8;
	reg.bits.pe9 = pe9;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE0
fun_set_mod_reg_bit(pwm, pwm_pe, pe0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE1
fun_set_mod_reg_bit(pwm, pwm_pe, pe1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE2
fun_set_mod_reg_bit(pwm, pwm_pe, pe2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE3
fun_set_mod_reg_bit(pwm, pwm_pe, pe3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE4
fun_set_mod_reg_bit(pwm, pwm_pe, pe4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE5
fun_set_mod_reg_bit(pwm, pwm_pe, pe5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE6
fun_set_mod_reg_bit(pwm, pwm_pe, pe6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE7
fun_set_mod_reg_bit(pwm, pwm_pe, pe7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE8
fun_set_mod_reg_bit(pwm, pwm_pe, pe8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_PE_PE9
fun_set_mod_reg_bit(pwm, pwm_pe, pe9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN
void set_pwm_pwm_int_en(u32 reg_addr,
			u32 en0,
			u32 en1,
			u32 en2,
			u32 en3,
			u32 en4,
			u32 en5,
			u32 en6,
			u32 en7,
			u32 en8,
			u32 en9,
			u32 m_or_r)
{
	reg_pwm_pwm_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en0 = en0;
	reg.bits.en1 = en1;
	reg.bits.en2 = en2;
	reg.bits.en3 = en3;
	reg.bits.en4 = en4;
	reg.bits.en5 = en5;
	reg.bits.en6 = en6;
	reg.bits.en7 = en7;
	reg.bits.en8 = en8;
	reg.bits.en9 = en9;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN0
fun_set_mod_reg_bit(pwm, pwm_int_en, en0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN1
fun_set_mod_reg_bit(pwm, pwm_int_en, en1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN2
fun_set_mod_reg_bit(pwm, pwm_int_en, en2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN3
fun_set_mod_reg_bit(pwm, pwm_int_en, en3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN4
fun_set_mod_reg_bit(pwm, pwm_int_en, en4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN5
fun_set_mod_reg_bit(pwm, pwm_int_en, en5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN6
fun_set_mod_reg_bit(pwm, pwm_int_en, en6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN7
fun_set_mod_reg_bit(pwm, pwm_int_en, en7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN8
fun_set_mod_reg_bit(pwm, pwm_int_en, en8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_EN_EN9
fun_set_mod_reg_bit(pwm, pwm_int_en, en9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING
void set_pwm_pwm_clr_int_pending(u32 reg_addr,
				u32 clr0,
				u32 clr1,
				u32 clr2,
				u32 clr3,
				u32 clr4,
				u32 clr5,
				u32 clr6,
				u32 clr7,
				u32 clr8,
				u32 clr9,
				u32 m_or_r)
{
	reg_pwm_pwm_clr_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr0 = clr0;
	reg.bits.clr1 = clr1;
	reg.bits.clr2 = clr2;
	reg.bits.clr3 = clr3;
	reg.bits.clr4 = clr4;
	reg.bits.clr5 = clr5;
	reg.bits.clr6 = clr6;
	reg.bits.clr7 = clr7;
	reg.bits.clr8 = clr8;
	reg.bits.clr9 = clr9;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR0
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR1
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR2
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR3
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR4
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR5
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR6
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR7
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR8
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_CLR_INT_PENDING_CLR9
fun_set_mod_reg_bit(pwm, pwm_clr_int_pending, clr9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING
void set_pwm_pwm_int_pending(u32 reg_addr,
				u32 pending0,
				u32 pending1,
				u32 pending2,
				u32 pending3,
				u32 pending4,
				u32 pending5,
				u32 pending6,
				u32 pending7,
				u32 pending8,
				u32 pending9,
				u32 m_or_r)
{
	reg_pwm_pwm_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending0 = pending0;
	reg.bits.pending1 = pending1;
	reg.bits.pending2 = pending2;
	reg.bits.pending3 = pending3;
	reg.bits.pending4 = pending4;
	reg.bits.pending5 = pending5;
	reg.bits.pending6 = pending6;
	reg.bits.pending7 = pending7;
	reg.bits.pending8 = pending8;
	reg.bits.pending9 = pending9;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING0
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING1
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING2
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING3
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING4
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING5
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING6
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING7
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING8
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM_INT_PENDING_PENDING9
fun_set_mod_reg_bit(pwm, pwm_int_pending, pending9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CLK_CTRL
void set_pwm_pwm0_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm0_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm0_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm0_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CLK_CTRL
void set_pwm_pwm1_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm1_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm1_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm1_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CLK_CTRL
void set_pwm_pwm2_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm2_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm2_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm2_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm2_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CLK_CTRL
void set_pwm_pwm3_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm3_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm3_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm3_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm3_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm3_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CLK_CTRL
void set_pwm_pwm4_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm4_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm4_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm4_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm4_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm4_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CLK_CTRL
void set_pwm_pwm5_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm5_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm5_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm5_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm5_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm5_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CLK_CTRL
void set_pwm_pwm6_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm6_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm6_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm6_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm6_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm6_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CLK_CTRL
void set_pwm_pwm7_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm7_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm7_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm7_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm7_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm7_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CLK_CTRL
void set_pwm_pwm8_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm8_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm8_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm8_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm8_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm8_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CLK_CTRL
void set_pwm_pwm9_clk_ctrl(u32 reg_addr,
			u32 div,
			u32 src_sel,
			u32 gat,
			u32 bypass,
			u32 m_or_r)
{
	reg_pwm_pwm9_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.div = div;
	reg.bits.src_sel = src_sel;
	reg.bits.gat = gat;
	reg.bits.bypass = bypass;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_CLK_CTRL_DIV
fun_set_mod_reg_bit(pwm, pwm9_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(pwm, pwm9_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CLK_CTRL_GAT
fun_set_mod_reg_bit(pwm, pwm9_clk_ctrl, gat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CLK_CTRL_BYPASS
fun_set_mod_reg_bit(pwm, pwm9_clk_ctrl, bypass, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_STORE
void set_pwm_pwm0_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm0_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm0_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_STORE
void set_pwm_pwm1_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm1_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm1_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_STORE
void set_pwm_pwm2_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm2_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm2_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_STORE
void set_pwm_pwm3_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm3_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm3_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_STORE
void set_pwm_pwm4_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm4_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm4_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_STORE
void set_pwm_pwm5_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm5_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm5_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_STORE
void set_pwm_pwm6_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm6_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm6_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_STORE
void set_pwm_pwm7_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm7_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm7_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_STORE
void set_pwm_pwm8_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm8_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm8_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_STORE
void set_pwm_pwm9_cnt_store(u32 reg_addr,
				u32 store,
				u32 m_or_r)
{
	reg_pwm_pwm9_cnt_store_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.store = store;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_STORE_STORE
fun_set_mod_reg_bit(pwm, pwm9_cnt_store, store, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_CMP
void set_pwm_pwm0_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm0_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm0_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_CMP
void set_pwm_pwm1_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm1_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm1_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_CMP
void set_pwm_pwm2_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm2_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm2_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_CMP
void set_pwm_pwm3_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm3_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm3_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_CMP
void set_pwm_pwm4_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm4_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm4_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_CMP
void set_pwm_pwm5_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm5_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm5_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_CMP
void set_pwm_pwm6_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm6_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm6_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_CMP
void set_pwm_pwm7_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm7_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm7_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_CMP
void set_pwm_pwm8_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm8_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm8_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_CMP
void set_pwm_pwm9_cnt_cmp(u32 reg_addr,
			u32 cmp,
			u32 m_or_r)
{
	reg_pwm_pwm9_cnt_cmp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cmp = cmp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_CMP_CMP
fun_set_mod_reg_bit(pwm, pwm9_cnt_cmp, cmp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL
void set_pwm_pwm0_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm0_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm0_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm0_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm0_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm0_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm0_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm0_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm0_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm0_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm0_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm0_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm0_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL
void set_pwm_pwm1_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm1_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm1_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm1_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm1_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm1_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm1_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm1_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm1_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm1_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm1_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm1_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm1_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL
void set_pwm_pwm2_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm2_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm2_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm2_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm2_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm2_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm2_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm2_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm2_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm2_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm2_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm2_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm2_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL
void set_pwm_pwm3_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm3_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm3_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm3_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm3_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm3_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm3_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm3_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm3_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm3_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm3_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm3_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm3_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL
void set_pwm_pwm4_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm4_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm4_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm4_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm4_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm4_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm4_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm4_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm4_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm4_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm4_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm4_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm4_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL
void set_pwm_pwm5_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm5_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm5_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm5_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm5_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm5_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm5_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm5_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm5_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm5_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm5_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm5_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm5_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL
void set_pwm_pwm6_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm6_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm6_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm6_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm6_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm6_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm6_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm6_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm6_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm6_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm6_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm6_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm6_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL
void set_pwm_pwm7_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm7_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm7_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm7_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm7_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm7_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm7_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm7_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm7_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm7_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm7_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm7_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm7_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL
void set_pwm_pwm8_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm8_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm8_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm8_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm8_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm8_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm8_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm8_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm8_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm8_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm8_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm8_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm8_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL
void set_pwm_pwm9_ctrl(u32 reg_addr,
			u32 dt,
			u32 dt_en,
			u32 mod,
			u32 p_oe,
			u32 p_pol,
			u32 n_oe,
			u32 n_pol,
			u32 ren,
			u32 fen,
			u32 p_rev,
			u32 p_ie,
			u32 m_or_r)
{
	reg_pwm_pwm9_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dt = dt;
	reg.bits.dt_en = dt_en;
	reg.bits.mod = mod;
	reg.bits.p_oe = p_oe;
	reg.bits.p_pol = p_pol;
	reg.bits.n_oe = n_oe;
	reg.bits.n_pol = n_pol;
	reg.bits.ren = ren;
	reg.bits.fen = fen;
	reg.bits.p_rev = p_rev;
	reg.bits.p_ie = p_ie;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_DT
fun_set_mod_reg_bit(pwm, pwm9_ctrl, dt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_DT_EN
fun_set_mod_reg_bit(pwm, pwm9_ctrl, dt_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_MOD
fun_set_mod_reg_bit(pwm, pwm9_ctrl, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_P_OE
fun_set_mod_reg_bit(pwm, pwm9_ctrl, p_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_P_POL
fun_set_mod_reg_bit(pwm, pwm9_ctrl, p_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_N_OE
fun_set_mod_reg_bit(pwm, pwm9_ctrl, n_oe, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_N_POL
fun_set_mod_reg_bit(pwm, pwm9_ctrl, n_pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_REN
fun_set_mod_reg_bit(pwm, pwm9_ctrl, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_FEN
fun_set_mod_reg_bit(pwm, pwm9_ctrl, fen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_P_REV
fun_set_mod_reg_bit(pwm, pwm9_ctrl, p_rev, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CTRL_P_IE
fun_set_mod_reg_bit(pwm, pwm9_ctrl, p_ie, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_CAP
void set_pwm_pwm0_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm0_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm0_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm0_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_CAP
void set_pwm_pwm1_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm1_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm1_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm1_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_CAP
void set_pwm_pwm2_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm2_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm2_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm2_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_CAP
void set_pwm_pwm3_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm3_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm3_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm3_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_CAP
void set_pwm_pwm4_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm4_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm4_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm4_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_CAP
void set_pwm_pwm5_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm5_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm5_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm5_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_CAP
void set_pwm_pwm6_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm6_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm6_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm6_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_CAP
void set_pwm_pwm7_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm7_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm7_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm7_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_CAP
void set_pwm_pwm8_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm8_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm8_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm8_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_CAP
void set_pwm_pwm9_cnt_cap(u32 reg_addr,
			u32 r_cap,
			u32 f_cap,
			u32 m_or_r)
{
	reg_pwm_pwm9_cnt_cap_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.r_cap = r_cap;
	reg.bits.f_cap = f_cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_CAP_R_CAP
fun_set_mod_reg_bit(pwm, pwm9_cnt_cap, r_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_CNT_CAP_F_CAP
fun_set_mod_reg_bit(pwm, pwm9_cnt_cap, f_cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_IN_STS
void set_pwm_pwm0_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm0_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm0_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm0_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm0_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm0_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_IN_STS
void set_pwm_pwm1_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm1_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm1_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm1_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm1_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm1_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_IN_STS
void set_pwm_pwm2_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm2_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm2_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm2_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm2_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm2_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_IN_STS
void set_pwm_pwm3_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm3_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm3_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm3_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm3_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm3_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_IN_STS
void set_pwm_pwm4_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm4_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm4_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm4_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm4_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm4_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_IN_STS
void set_pwm_pwm5_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm5_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm5_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm5_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm5_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm5_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_IN_STS
void set_pwm_pwm6_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm6_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm6_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm6_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm6_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm6_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_IN_STS
void set_pwm_pwm7_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm7_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm7_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm7_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm7_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm7_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_IN_STS
void set_pwm_pwm8_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm8_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm8_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm8_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm8_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm8_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_IN_STS
void set_pwm_pwm9_in_sts(u32 reg_addr,
			u32 rflag,
			u32 orflag,
			u32 fflag,
			u32 offlag,
			u32 m_or_r)
{
	reg_pwm_pwm9_in_sts_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rflag = rflag;
	reg.bits.orflag = orflag;
	reg.bits.fflag = fflag;
	reg.bits.offlag = offlag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_IN_STS_RFLAG
fun_set_mod_reg_bit(pwm, pwm9_in_sts, rflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_IN_STS_ORFLAG
fun_set_mod_reg_bit(pwm, pwm9_in_sts, orflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_IN_STS_FFLAG
fun_set_mod_reg_bit(pwm, pwm9_in_sts, fflag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_IN_STS_OFFLAG
fun_set_mod_reg_bit(pwm, pwm9_in_sts, offlag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_DLY_CNT
void set_pwm_pwm0_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm0_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm0_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_DLY_CNT
void set_pwm_pwm1_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm1_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm1_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_DLY_CNT
void set_pwm_pwm2_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm2_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm2_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_DLY_CNT
void set_pwm_pwm3_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm3_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm3_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_DLY_CNT
void set_pwm_pwm4_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm4_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm4_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_DLY_CNT
void set_pwm_pwm5_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm5_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm5_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_DLY_CNT
void set_pwm_pwm6_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm6_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm6_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_DLY_CNT
void set_pwm_pwm7_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm7_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm7_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_DLY_CNT
void set_pwm_pwm8_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm8_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm8_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_DLY_CNT
void set_pwm_pwm9_dly_cnt(u32 reg_addr,
			u32 delay,
			u32 m_or_r)
{
	reg_pwm_pwm9_dly_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.delay = delay;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_DLY_CNT_DELAY
fun_set_mod_reg_bit(pwm, pwm9_dly_cnt, delay, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM0_PULSE_CNT
void set_pwm_pwm0_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm0_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM0_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm0_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM1_PULSE_CNT
void set_pwm_pwm1_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm1_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM1_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm1_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM2_PULSE_CNT
void set_pwm_pwm2_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm2_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM2_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm2_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM3_PULSE_CNT
void set_pwm_pwm3_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm3_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM3_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm3_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM4_PULSE_CNT
void set_pwm_pwm4_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm4_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM4_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm4_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM5_PULSE_CNT
void set_pwm_pwm5_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm5_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM5_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm5_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM6_PULSE_CNT
void set_pwm_pwm6_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm6_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM6_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm6_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM7_PULSE_CNT
void set_pwm_pwm7_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm7_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM7_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm7_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM8_PULSE_CNT
void set_pwm_pwm8_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm8_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM8_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm8_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PWM_PWM9_PULSE_CNT
void set_pwm_pwm9_pulse_cnt(u32 reg_addr,
				u32 pcnt,
				u32 m_or_r)
{
	reg_pwm_pwm9_pulse_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pcnt = pcnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PWM_PWM9_PULSE_CNT_PCNT
fun_set_mod_reg_bit(pwm, pwm9_pulse_cnt, pcnt, addr, val, m_or_r)
#endif

#endif /* ___PWM__SET___C___ */

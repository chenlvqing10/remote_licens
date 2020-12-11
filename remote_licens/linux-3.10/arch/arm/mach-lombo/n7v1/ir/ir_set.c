/* ir_set.c */

#ifndef ___IR__SET___C___
#define ___IR__SET___C___

#ifdef DEF_SET_IR_VER
void set_ir_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r)
{
	reg_ir_ver_t reg;

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

#ifdef DEF_SET_IR_VER_VER_L
fun_set_mod_reg_bit(ir, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_VER_VER_H
fun_set_mod_reg_bit(ir, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_VER_COMP
fun_set_mod_reg_bit(ir, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR
void set_ir_ftr(u32 reg_addr,
		u32 nec_pro,
		u32 sirc_pro,
		u32 sirc_ext_15,
		u32 sirc_ext_20,
		u32 rc5_pro,
		u32 rc6_pro,
		u32 ip_ver_l,
		u32 ip_ver_h,
		u32 m_or_r)
{
	reg_ir_ftr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nec_pro = nec_pro;
	reg.bits.sirc_pro = sirc_pro;
	reg.bits.sirc_ext_15 = sirc_ext_15;
	reg.bits.sirc_ext_20 = sirc_ext_20;
	reg.bits.rc5_pro = rc5_pro;
	reg.bits.rc6_pro = rc6_pro;
	reg.bits.ip_ver_l = ip_ver_l;
	reg.bits.ip_ver_h = ip_ver_h;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_FTR_NEC_PRO
fun_set_mod_reg_bit(ir, ftr, nec_pro, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_SIRC_PRO
fun_set_mod_reg_bit(ir, ftr, sirc_pro, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_SIRC_EXT_15
fun_set_mod_reg_bit(ir, ftr, sirc_ext_15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_SIRC_EXT_20
fun_set_mod_reg_bit(ir, ftr, sirc_ext_20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_RC5_PRO
fun_set_mod_reg_bit(ir, ftr, rc5_pro, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_RC6_PRO
fun_set_mod_reg_bit(ir, ftr, rc6_pro, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_IP_VER_L
fun_set_mod_reg_bit(ir, ftr, ip_ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_FTR_IP_VER_H
fun_set_mod_reg_bit(ir, ftr, ip_ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_CTL
void set_ir_ctl(u32 reg_addr,
		u32 clk_div,
		u32 sirc_ext,
		u32 protocol_sel,
		u32 ir_invert,
		u32 ir_en,
		u32 m_or_r)
{
	reg_ir_ctl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clk_div = clk_div;
	reg.bits.sirc_ext = sirc_ext;
	reg.bits.protocol_sel = protocol_sel;
	reg.bits.ir_invert = ir_invert;
	reg.bits.ir_en = ir_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_CTL_CLK_DIV
fun_set_mod_reg_bit(ir, ctl, clk_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_CTL_SIRC_EXT
fun_set_mod_reg_bit(ir, ctl, sirc_ext, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_CTL_PROTOCOL_SEL
fun_set_mod_reg_bit(ir, ctl, protocol_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_CTL_IR_INVERT
fun_set_mod_reg_bit(ir, ctl, ir_invert, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_CTL_IR_EN
fun_set_mod_reg_bit(ir, ctl, ir_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_TIMEOUT_TH
void set_ir_timeout_th(u32 reg_addr,
			u32 thd,
			u32 m_or_r)
{
	reg_ir_timeout_th_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.thd = thd;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_TIMEOUT_TH_THD
fun_set_mod_reg_bit(ir, timeout_th, thd, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_NOISE_TH
void set_ir_noise_th(u32 reg_addr,
			u32 thd,
			u32 m_or_r)
{
	reg_ir_noise_th_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.thd = thd;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_NOISE_TH_THD
fun_set_mod_reg_bit(ir, noise_th, thd, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_EN
void set_ir_int_en(u32 reg_addr,
		u32 ir_start,
		u32 ir_frame,
		u32 ir_repeat,
		u32 ir_f_err,
		u32 ir_to,
		u32 m_or_r)
{
	reg_ir_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ir_start = ir_start;
	reg.bits.ir_frame = ir_frame;
	reg.bits.ir_repeat = ir_repeat;
	reg.bits.ir_f_err = ir_f_err;
	reg.bits.ir_to = ir_to;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_INT_EN_IR_START
fun_set_mod_reg_bit(ir, int_en, ir_start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_EN_IR_FRAME
fun_set_mod_reg_bit(ir, int_en, ir_frame, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_EN_IR_REPEAT
fun_set_mod_reg_bit(ir, int_en, ir_repeat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_EN_IR_F_ERR
fun_set_mod_reg_bit(ir, int_en, ir_f_err, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_EN_IR_TO
fun_set_mod_reg_bit(ir, int_en, ir_to, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_PD
void set_ir_int_pd(u32 reg_addr,
		u32 ir_start,
		u32 ir_frame,
		u32 ir_repeat,
		u32 ir_f_err,
		u32 ir_to,
		u32 m_or_r)
{
	reg_ir_int_pd_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ir_start = ir_start;
	reg.bits.ir_frame = ir_frame;
	reg.bits.ir_repeat = ir_repeat;
	reg.bits.ir_f_err = ir_f_err;
	reg.bits.ir_to = ir_to;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_INT_PD_IR_START
fun_set_mod_reg_bit(ir, int_pd, ir_start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_PD_IR_FRAME
fun_set_mod_reg_bit(ir, int_pd, ir_frame, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_PD_IR_REPEAT
fun_set_mod_reg_bit(ir, int_pd, ir_repeat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_PD_IR_F_ERR
fun_set_mod_reg_bit(ir, int_pd, ir_f_err, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_PD_IR_TO
fun_set_mod_reg_bit(ir, int_pd, ir_to, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_CLR
void set_ir_int_clr(u32 reg_addr,
			u32 ir_start,
			u32 ir_frame,
			u32 ir_repeat,
			u32 ir_f_err,
			u32 ir_to,
			u32 m_or_r)
{
	reg_ir_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ir_start = ir_start;
	reg.bits.ir_frame = ir_frame;
	reg.bits.ir_repeat = ir_repeat;
	reg.bits.ir_f_err = ir_f_err;
	reg.bits.ir_to = ir_to;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_INT_CLR_IR_START
fun_set_mod_reg_bit(ir, int_clr, ir_start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_CLR_IR_FRAME
fun_set_mod_reg_bit(ir, int_clr, ir_frame, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_CLR_IR_REPEAT
fun_set_mod_reg_bit(ir, int_clr, ir_repeat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_CLR_IR_F_ERR
fun_set_mod_reg_bit(ir, int_clr, ir_f_err, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_INT_CLR_IR_TO
fun_set_mod_reg_bit(ir, int_clr, ir_to, addr, val, m_or_r)
#endif

#ifdef DEF_SET_IR_DATA
void set_ir_data(u32 reg_addr,
		u32 data,
		u32 m_or_r)
{
	reg_ir_data_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.data = data;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_IR_DATA_DATA
fun_set_mod_reg_bit(ir, data, data, addr, val, m_or_r)
#endif

#endif /* ___IR__SET___C___ */

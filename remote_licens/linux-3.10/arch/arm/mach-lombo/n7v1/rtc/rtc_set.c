/* rtc_set.c */

#ifndef ___RTC__SET___C___
#define ___RTC__SET___C___

#ifdef DEF_SET_RTC_VER
void set_rtc_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r)
{
	reg_rtc_ver_t reg;

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

#ifdef DEF_SET_RTC_VER_VER_L
fun_set_mod_reg_bit(rtc, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_VER_VER_H
fun_set_mod_reg_bit(rtc, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_VER_COMP
fun_set_mod_reg_bit(rtc, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CLK_CTRL
void set_rtc_rtc_clk_ctrl(u32 reg_addr,
			u32 sel,
			u32 stat,
			u32 rcosc_div,
			u32 key_field,
			u32 m_or_r)
{
	reg_rtc_rtc_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sel = sel;
	reg.bits.stat = stat;
	reg.bits.rcosc_div = rcosc_div;
	reg.bits.key_field = key_field;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_CLK_CTRL_SEL
fun_set_mod_reg_bit(rtc, rtc_clk_ctrl, sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CLK_CTRL_STAT
fun_set_mod_reg_bit(rtc, rtc_clk_ctrl, stat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CLK_CTRL_RCOSC_DIV
fun_set_mod_reg_bit(rtc, rtc_clk_ctrl, rcosc_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CLK_CTRL_KEY_FIELD
fun_set_mod_reg_bit(rtc, rtc_clk_ctrl, key_field, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LD_EN
void set_rtc_rtc_ld_en(u32 reg_addr,
			u32 en,
			u32 m_or_r)
{
	reg_rtc_rtc_ld_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_LD_EN_EN
fun_set_mod_reg_bit(rtc, rtc_ld_en, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LD_INT_EN
void set_rtc_rtc_ld_int_en(u32 reg_addr,
			u32 en,
			u32 m_or_r)
{
	reg_rtc_rtc_ld_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_LD_INT_EN_EN
fun_set_mod_reg_bit(rtc, rtc_ld_int_en, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LD_INT_CLR
void set_rtc_rtc_ld_int_clr(u32 reg_addr,
				u32 clr,
				u32 m_or_r)
{
	reg_rtc_rtc_ld_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_LD_INT_CLR_CLR
fun_set_mod_reg_bit(rtc, rtc_ld_int_clr, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LD_INT_PENDING
void set_rtc_rtc_ld_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_rtc_rtc_ld_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_LD_INT_PENDING_PENDING
fun_set_mod_reg_bit(rtc, rtc_ld_int_pending, pending, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_YMD
void set_rtc_rtc_ymd(u32 reg_addr,
			u32 day,
			u32 month,
			u32 year,
			u32 m_or_r)
{
	reg_rtc_rtc_ymd_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.day = day;
	reg.bits.month = month;
	reg.bits.year = year;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_YMD_DAY
fun_set_mod_reg_bit(rtc, rtc_ymd, day, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_YMD_MONTH
fun_set_mod_reg_bit(rtc, rtc_ymd, month, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_YMD_YEAR
fun_set_mod_reg_bit(rtc, rtc_ymd, year, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_WEEK
void set_rtc_rtc_week(u32 reg_addr,
			u32 week,
			u32 m_or_r)
{
	reg_rtc_rtc_week_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.week = week;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_WEEK_WEEK
fun_set_mod_reg_bit(rtc, rtc_week, week, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HMS
void set_rtc_rtc_hms(u32 reg_addr,
			u32 second,
			u32 minute,
			u32 hour,
			u32 m_or_r)
{
	reg_rtc_rtc_hms_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.second = second;
	reg.bits.minute = minute;
	reg.bits.hour = hour;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_HMS_SECOND
fun_set_mod_reg_bit(rtc, rtc_hms, second, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HMS_MINUTE
fun_set_mod_reg_bit(rtc, rtc_hms, minute, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HMS_HOUR
fun_set_mod_reg_bit(rtc, rtc_hms, hour, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_INT_EN
void set_rtc_rtc_alarm_int_en(u32 reg_addr,
				u32 en,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_INT_EN_EN
fun_set_mod_reg_bit(rtc, rtc_alarm_int_en, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_INT_CLR
void set_rtc_rtc_alarm_int_clr(u32 reg_addr,
				u32 clr,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr = clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_INT_CLR_CLR
fun_set_mod_reg_bit(rtc, rtc_alarm_int_clr, clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_INT_PENDING
void set_rtc_rtc_alarm_int_pending(u32 reg_addr,
				u32 pending,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_int_pending_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pending = pending;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_INT_PENDING_PENDING
fun_set_mod_reg_bit(rtc, rtc_alarm_int_pending, pending, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_MATCH_EN
void set_rtc_rtc_alarm_match_en(u32 reg_addr,
				u32 sec_alarm_match_en,
				u32 min_alarm_match_en,
				u32 hour_alarm_match_en,
				u32 week_alarm_match_en,
				u32 day_alarm_match_en,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_match_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sec_alarm_match_en = sec_alarm_match_en;
	reg.bits.min_alarm_match_en = min_alarm_match_en;
	reg.bits.hour_alarm_match_en = hour_alarm_match_en;
	reg.bits.week_alarm_match_en = week_alarm_match_en;
	reg.bits.day_alarm_match_en = day_alarm_match_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_MATCH_EN_SEC_ALARM_MATCH_EN
fun_set_mod_reg_bit(rtc, rtc_alarm_match_en, sec_alarm_match_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_MATCH_EN_MIN_ALARM_MATCH_EN
fun_set_mod_reg_bit(rtc, rtc_alarm_match_en, min_alarm_match_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_MATCH_EN_HOUR_ALARM_MATCH_EN
fun_set_mod_reg_bit(rtc, rtc_alarm_match_en, hour_alarm_match_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_MATCH_EN_WEEK_ALARM_MATCH_EN
fun_set_mod_reg_bit(rtc, rtc_alarm_match_en, week_alarm_match_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_MATCH_EN_DAY_ALARM_MATCH_EN
fun_set_mod_reg_bit(rtc, rtc_alarm_match_en, day_alarm_match_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_DAY_MATCH
void set_rtc_rtc_alarm_day_match(u32 reg_addr,
				u32 day_match,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_day_match_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.day_match = day_match;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_DAY_MATCH_DAY_MATCH
fun_set_mod_reg_bit(rtc, rtc_alarm_day_match, day_match, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_WEEK_MATCH
void set_rtc_rtc_alarm_week_match(u32 reg_addr,
				u32 week_match,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_week_match_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.week_match = week_match;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_WEEK_MATCH_WEEK_MATCH
fun_set_mod_reg_bit(rtc, rtc_alarm_week_match, week_match, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_HMS_MATCH
void set_rtc_rtc_alarm_hms_match(u32 reg_addr,
				u32 second_match,
				u32 minute_match,
				u32 hour_match,
				u32 m_or_r)
{
	reg_rtc_rtc_alarm_hms_match_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.second_match = second_match;
	reg.bits.minute_match = minute_match;
	reg.bits.hour_match = hour_match;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_HMS_MATCH_SECOND_MATCH
fun_set_mod_reg_bit(rtc, rtc_alarm_hms_match, second_match, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_HMS_MATCH_MINUTE_MATCH
fun_set_mod_reg_bit(rtc, rtc_alarm_hms_match, minute_match, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALARM_HMS_MATCH_HOUR_MATCH
fun_set_mod_reg_bit(rtc, rtc_alarm_hms_match, hour_match, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_WAKEUP_EN
void set_rtc_rtc_wakeup_en(u32 reg_addr,
			u32 wakeup_en,
			u32 m_or_r)
{
	reg_rtc_rtc_wakeup_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.wakeup_en = wakeup_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_WAKEUP_EN_WAKEUP_EN
fun_set_mod_reg_bit(rtc, rtc_wakeup_en, wakeup_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_FANOUT_CFG
void set_rtc_rtc_lfeosc_fanout_cfg(u32 reg_addr,
				u32 func,
				u32 dat,
				u32 lfeosc_fanout_en,
				u32 m_or_r)
{
	reg_rtc_rtc_lfeosc_fanout_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.func = func;
	reg.bits.dat = dat;
	reg.bits.lfeosc_fanout_en = lfeosc_fanout_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_FANOUT_CFG_FUNC
fun_set_mod_reg_bit(rtc, rtc_lfeosc_fanout_cfg, func, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_FANOUT_CFG_DAT
fun_set_mod_reg_bit(rtc, rtc_lfeosc_fanout_cfg, dat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_FANOUT_CFG_LFEOSC_FANOUT_EN
fun_set_mod_reg_bit(rtc, rtc_lfeosc_fanout_cfg, lfeosc_fanout_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_SU_EN
void set_rtc_rtc_su_en(u32 reg_addr,
			u32 en,
			u32 rate,
			u32 m_or_r)
{
	reg_rtc_rtc_su_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.rate = rate;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_SU_EN_EN
fun_set_mod_reg_bit(rtc, rtc_su_en, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_SU_EN_RATE
fun_set_mod_reg_bit(rtc, rtc_su_en, rate, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE
void set_rtc_rtc_core(u32 reg_addr,
			u32 osc1_en,
			u32 vref_sel,
			u32 ldo_sel,
			u32 div_sel,
			u32 div_en,
			u32 tsd,
			u32 tsa,
			u32 bg1_en,
			u32 bg0_en,
			u32 m_or_r)
{
	reg_rtc_rtc_core_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.osc1_en = osc1_en;
	reg.bits.vref_sel = vref_sel;
	reg.bits.ldo_sel = ldo_sel;
	reg.bits.div_sel = div_sel;
	reg.bits.div_en = div_en;
	reg.bits.tsd = tsd;
	reg.bits.tsa = tsa;
	reg.bits.bg1_en = bg1_en;
	reg.bits.bg0_en = bg0_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_CORE_OSC1_EN
fun_set_mod_reg_bit(rtc, rtc_core, osc1_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_VREF_SEL
fun_set_mod_reg_bit(rtc, rtc_core, vref_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_LDO_SEL
fun_set_mod_reg_bit(rtc, rtc_core, ldo_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_DIV_SEL
fun_set_mod_reg_bit(rtc, rtc_core, div_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_DIV_EN
fun_set_mod_reg_bit(rtc, rtc_core, div_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_TSD
fun_set_mod_reg_bit(rtc, rtc_core, tsd, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_TSA
fun_set_mod_reg_bit(rtc, rtc_core, tsa, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_BG1_EN
fun_set_mod_reg_bit(rtc, rtc_core, bg1_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_CORE_BG0_EN
fun_set_mod_reg_bit(rtc, rtc_core, bg0_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_TOP
void set_rtc_rtc_top(u32 reg_addr,
			u32 eno,
			u32 ad,
			u32 seld,
			u32 m_or_r)
{
	reg_rtc_rtc_top_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.eno = eno;
	reg.bits.ad = ad;
	reg.bits.seld = seld;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_TOP_ENO
fun_set_mod_reg_bit(rtc, rtc_top, eno, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_TOP_AD
fun_set_mod_reg_bit(rtc, rtc_top, ad, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_TOP_SELD
fun_set_mod_reg_bit(rtc, rtc_top, seld, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC
void set_rtc_rtc_hfeosc(u32 reg_addr,
			u32 en,
			u32 capen,
			u32 smtsel,
			u32 smten,
			u32 lpfen,
			u32 capsel_i,
			u32 capsel_o,
			u32 gmsel,
			u32 rsel,
			u32 m_or_r)
{
	reg_rtc_rtc_hfeosc_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.capen = capen;
	reg.bits.smtsel = smtsel;
	reg.bits.smten = smten;
	reg.bits.lpfen = lpfen;
	reg.bits.capsel_i = capsel_i;
	reg.bits.capsel_o = capsel_o;
	reg.bits.gmsel = gmsel;
	reg.bits.rsel = rsel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_EN
fun_set_mod_reg_bit(rtc, rtc_hfeosc, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_CAPEN
fun_set_mod_reg_bit(rtc, rtc_hfeosc, capen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_SMTSEL
fun_set_mod_reg_bit(rtc, rtc_hfeosc, smtsel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_SMTEN
fun_set_mod_reg_bit(rtc, rtc_hfeosc, smten, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_LPFEN
fun_set_mod_reg_bit(rtc, rtc_hfeosc, lpfen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_CAPSEL_I
fun_set_mod_reg_bit(rtc, rtc_hfeosc, capsel_i, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_CAPSEL_O
fun_set_mod_reg_bit(rtc, rtc_hfeosc, capsel_o, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_GMSEL
fun_set_mod_reg_bit(rtc, rtc_hfeosc, gmsel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_HFEOSC_RSEL
fun_set_mod_reg_bit(rtc, rtc_hfeosc, rsel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC
void set_rtc_rtc_lfeosc(u32 reg_addr,
			u32 en,
			u32 capen,
			u32 smtsel,
			u32 smten,
			u32 lpfen,
			u32 capsel_i,
			u32 capsel_o,
			u32 gmsel,
			u32 m_or_r)
{
	reg_rtc_rtc_lfeosc_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.capen = capen;
	reg.bits.smtsel = smtsel;
	reg.bits.smten = smten;
	reg.bits.lpfen = lpfen;
	reg.bits.capsel_i = capsel_i;
	reg.bits.capsel_o = capsel_o;
	reg.bits.gmsel = gmsel;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_EN
fun_set_mod_reg_bit(rtc, rtc_lfeosc, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_CAPEN
fun_set_mod_reg_bit(rtc, rtc_lfeosc, capen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_SMTSEL
fun_set_mod_reg_bit(rtc, rtc_lfeosc, smtsel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_SMTEN
fun_set_mod_reg_bit(rtc, rtc_lfeosc, smten, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_LPFEN
fun_set_mod_reg_bit(rtc, rtc_lfeosc, lpfen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_CAPSEL_I
fun_set_mod_reg_bit(rtc, rtc_lfeosc, capsel_i, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_CAPSEL_O
fun_set_mod_reg_bit(rtc, rtc_lfeosc, capsel_o, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_LFEOSC_GMSEL
fun_set_mod_reg_bit(rtc, rtc_lfeosc, gmsel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG0
void set_rtc_rtc_alive_reg0(u32 reg_addr,
				u32 reg,
				u32 m_or_r)
{
	reg_rtc_rtc_alive_reg0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.reg = reg;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG0_REG
fun_set_mod_reg_bit(rtc, rtc_alive_reg0, reg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG1
void set_rtc_rtc_alive_reg1(u32 reg_addr,
				u32 reg,
				u32 m_or_r)
{
	reg_rtc_rtc_alive_reg1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.reg = reg;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG1_REG
fun_set_mod_reg_bit(rtc, rtc_alive_reg1, reg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG2
void set_rtc_rtc_alive_reg2(u32 reg_addr,
				u32 reg,
				u32 m_or_r)
{
	reg_rtc_rtc_alive_reg2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.reg = reg;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG2_REG
fun_set_mod_reg_bit(rtc, rtc_alive_reg2, reg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG3
void set_rtc_rtc_alive_reg3(u32 reg_addr,
				u32 reg,
				u32 m_or_r)
{
	reg_rtc_rtc_alive_reg3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.reg = reg;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_RTC_ALIVE_REG3_REG
fun_set_mod_reg_bit(rtc, rtc_alive_reg3, reg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE
void set_rtc_pm_pe(u32 reg_addr,
		u32 en,
		u32 ddr_en,
		u32 key_filed,
		u32 m_or_r)
{
	reg_rtc_pm_pe_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.ddr_en = ddr_en;
	reg.bits.key_filed = key_filed;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_PE_EN
fun_set_mod_reg_bit(rtc, pm_pe, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE_DDR_EN
fun_set_mod_reg_bit(rtc, pm_pe, ddr_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE_KEY_FILED
fun_set_mod_reg_bit(rtc, pm_pe, key_filed, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG
void set_rtc_pm_cfg(u32 reg_addr,
			u32 g_int_trig,
			u32 pir_wake_trig,
			u32 ring_key_trig,
			u32 gs_wake_detect_en,
			u32 pir_wake_detect_en,
			u32 ring_key_wake_det_en,
			u32 key_slong_en,
			u32 key_filed,
			u32 m_or_r)
{
	reg_rtc_pm_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.g_int_trig = g_int_trig;
	reg.bits.pir_wake_trig = pir_wake_trig;
	reg.bits.ring_key_trig = ring_key_trig;
	reg.bits.gs_wake_detect_en = gs_wake_detect_en;
	reg.bits.pir_wake_detect_en = pir_wake_detect_en;
	reg.bits.ring_key_wake_det_en = ring_key_wake_det_en;
	reg.bits.key_slong_en = key_slong_en;
	reg.bits.key_filed = key_filed;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_CFG_G_INT_TRIG
fun_set_mod_reg_bit(rtc, pm_cfg, g_int_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_PIR_WAKE_TRIG
fun_set_mod_reg_bit(rtc, pm_cfg, pir_wake_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_RING_KEY_TRIG
fun_set_mod_reg_bit(rtc, pm_cfg, ring_key_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_GS_WAKE_DETECT_EN
fun_set_mod_reg_bit(rtc, pm_cfg, gs_wake_detect_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_PIR_WAKE_DETECT_EN
fun_set_mod_reg_bit(rtc, pm_cfg, pir_wake_detect_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_RING_KEY_WAKE_DET_EN
fun_set_mod_reg_bit(rtc, pm_cfg, ring_key_wake_det_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_KEY_SLONG_EN
fun_set_mod_reg_bit(rtc, pm_cfg, key_slong_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_CFG_KEY_FILED
fun_set_mod_reg_bit(rtc, pm_cfg, key_filed, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN
void set_rtc_pm_int_en(u32 reg_addr,
			u32 pwr_en,
			u32 key_press,
			u32 key_short,
			u32 key_long,
			u32 key_release,
			u32 gs_int,
			u32 pir_wake,
			u32 ring_key,
			u32 m_or_r)
{
	reg_rtc_pm_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pwr_en = pwr_en;
	reg.bits.key_press = key_press;
	reg.bits.key_short = key_short;
	reg.bits.key_long = key_long;
	reg.bits.key_release = key_release;
	reg.bits.gs_int = gs_int;
	reg.bits.pir_wake = pir_wake;
	reg.bits.ring_key = ring_key;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_PWR_EN
fun_set_mod_reg_bit(rtc, pm_int_en, pwr_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_KEY_PRESS
fun_set_mod_reg_bit(rtc, pm_int_en, key_press, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_KEY_SHORT
fun_set_mod_reg_bit(rtc, pm_int_en, key_short, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_KEY_LONG
fun_set_mod_reg_bit(rtc, pm_int_en, key_long, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_KEY_RELEASE
fun_set_mod_reg_bit(rtc, pm_int_en, key_release, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_GS_INT
fun_set_mod_reg_bit(rtc, pm_int_en, gs_int, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_PIR_WAKE
fun_set_mod_reg_bit(rtc, pm_int_en, pir_wake, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_INT_EN_RING_KEY
fun_set_mod_reg_bit(rtc, pm_int_en, ring_key, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR
void set_rtc_pm_pend_clr(u32 reg_addr,
			u32 pw_con,
			u32 pw_discon,
			u32 key_press,
			u32 key_short,
			u32 key_long,
			u32 key_release,
			u32 gs_int,
			u32 pir_wake,
			u32 ring_key,
			u32 m_or_r)
{
	reg_rtc_pm_pend_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pw_con = pw_con;
	reg.bits.pw_discon = pw_discon;
	reg.bits.key_press = key_press;
	reg.bits.key_short = key_short;
	reg.bits.key_long = key_long;
	reg.bits.key_release = key_release;
	reg.bits.gs_int = gs_int;
	reg.bits.pir_wake = pir_wake;
	reg.bits.ring_key = ring_key;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_PW_CON
fun_set_mod_reg_bit(rtc, pm_pend_clr, pw_con, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_PW_DISCON
fun_set_mod_reg_bit(rtc, pm_pend_clr, pw_discon, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_KEY_PRESS
fun_set_mod_reg_bit(rtc, pm_pend_clr, key_press, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_KEY_SHORT
fun_set_mod_reg_bit(rtc, pm_pend_clr, key_short, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_KEY_LONG
fun_set_mod_reg_bit(rtc, pm_pend_clr, key_long, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_KEY_RELEASE
fun_set_mod_reg_bit(rtc, pm_pend_clr, key_release, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_GS_INT
fun_set_mod_reg_bit(rtc, pm_pend_clr, gs_int, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_PIR_WAKE
fun_set_mod_reg_bit(rtc, pm_pend_clr, pir_wake, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_CLR_RING_KEY
fun_set_mod_reg_bit(rtc, pm_pend_clr, ring_key, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND
void set_rtc_pm_pend(u32 reg_addr,
			u32 pw_con,
			u32 pw_discon,
			u32 key_press,
			u32 key_short,
			u32 key_long,
			u32 key_release,
			u32 gs_int,
			u32 pir_wake,
			u32 ring_key,
			u32 m_or_r)
{
	reg_rtc_pm_pend_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pw_con = pw_con;
	reg.bits.pw_discon = pw_discon;
	reg.bits.key_press = key_press;
	reg.bits.key_short = key_short;
	reg.bits.key_long = key_long;
	reg.bits.key_release = key_release;
	reg.bits.gs_int = gs_int;
	reg.bits.pir_wake = pir_wake;
	reg.bits.ring_key = ring_key;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_PEND_PW_CON
fun_set_mod_reg_bit(rtc, pm_pend, pw_con, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_PW_DISCON
fun_set_mod_reg_bit(rtc, pm_pend, pw_discon, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_KEY_PRESS
fun_set_mod_reg_bit(rtc, pm_pend, key_press, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_KEY_SHORT
fun_set_mod_reg_bit(rtc, pm_pend, key_short, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_KEY_LONG
fun_set_mod_reg_bit(rtc, pm_pend, key_long, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_KEY_RELEASE
fun_set_mod_reg_bit(rtc, pm_pend, key_release, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_GS_INT
fun_set_mod_reg_bit(rtc, pm_pend, gs_int, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_PIR_WAKE
fun_set_mod_reg_bit(rtc, pm_pend, pir_wake, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PEND_RING_KEY
fun_set_mod_reg_bit(rtc, pm_pend, ring_key, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE1
void set_rtc_pm_pe1(u32 reg_addr,
			u32 en,
			u32 pol,
			u32 tp,
			u32 ap,
			u32 m_or_r)
{
	reg_rtc_pm_pe1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.pol = pol;
	reg.bits.tp = tp;
	reg.bits.ap = ap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_PE1_EN
fun_set_mod_reg_bit(rtc, pm_pe1, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE1_POL
fun_set_mod_reg_bit(rtc, pm_pe1, pol, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE1_TP
fun_set_mod_reg_bit(rtc, pm_pe1, tp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE1_AP
fun_set_mod_reg_bit(rtc, pm_pe1, ap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_STAT
void set_rtc_pm_stat(u32 reg_addr,
			u32 pwr_con,
			u32 m_or_r)
{
	reg_rtc_pm_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pwr_con = pwr_con;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_STAT_PWR_CON
fun_set_mod_reg_bit(rtc, pm_stat, pwr_con, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PKT
void set_rtc_pm_pkt(u32 reg_addr,
			u32 pwr_up,
			u32 key_long,
			u32 key_slong,
			u32 m_or_r)
{
	reg_rtc_pm_pkt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pwr_up = pwr_up;
	reg.bits.key_long = key_long;
	reg.bits.key_slong = key_slong;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_PKT_PWR_UP
fun_set_mod_reg_bit(rtc, pm_pkt, pwr_up, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PKT_KEY_LONG
fun_set_mod_reg_bit(rtc, pm_pkt, key_long, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PKT_KEY_SLONG
fun_set_mod_reg_bit(rtc, pm_pkt, key_slong, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE2
void set_rtc_pm_pe2(u32 reg_addr,
			u32 swi,
			u32 key_filed,
			u32 m_or_r)
{
	reg_rtc_pm_pe2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.swi = swi;
	reg.bits.key_filed = key_filed;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PM_PE2_SWI
fun_set_mod_reg_bit(rtc, pm_pe2, swi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PM_PE2_KEY_FILED
fun_set_mod_reg_bit(rtc, pm_pe2, key_filed, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL
void set_rtc_lock_ctrl(u32 reg_addr,
			u32 deep_slp_exit,
			u32 deep_slp_flag,
			u32 hot_rst_exit,
			u32 hot_rst_flag,
			u32 pad_hold,
			u32 sys_stat,
			u32 keyfield,
			u32 m_or_r)
{
	reg_rtc_lock_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.deep_slp_exit = deep_slp_exit;
	reg.bits.deep_slp_flag = deep_slp_flag;
	reg.bits.hot_rst_exit = hot_rst_exit;
	reg.bits.hot_rst_flag = hot_rst_flag;
	reg.bits.pad_hold = pad_hold;
	reg.bits.sys_stat = sys_stat;
	reg.bits.keyfield = keyfield;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_DEEP_SLP_EXIT
fun_set_mod_reg_bit(rtc, lock_ctrl, deep_slp_exit, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_DEEP_SLP_FLAG
fun_set_mod_reg_bit(rtc, lock_ctrl, deep_slp_flag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_HOT_RST_EXIT
fun_set_mod_reg_bit(rtc, lock_ctrl, hot_rst_exit, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_HOT_RST_FLAG
fun_set_mod_reg_bit(rtc, lock_ctrl, hot_rst_flag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_PAD_HOLD
fun_set_mod_reg_bit(rtc, lock_ctrl, pad_hold, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_SYS_STAT
fun_set_mod_reg_bit(rtc, lock_ctrl, sys_stat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_LOCK_CTRL_KEYFIELD
fun_set_mod_reg_bit(rtc, lock_ctrl, keyfield, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_DEEP_SLP_FLAG
void set_rtc_deep_slp_flag(u32 reg_addr,
			u32 flag,
			u32 m_or_r)
{
	reg_rtc_deep_slp_flag_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.flag = flag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_DEEP_SLP_FLAG_FLAG
fun_set_mod_reg_bit(rtc, deep_slp_flag, flag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_DEEP_SLP_EXIT
void set_rtc_deep_slp_exit(u32 reg_addr,
			u32 exit_addr,
			u32 m_or_r)
{
	reg_rtc_deep_slp_exit_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.exit_addr = exit_addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_DEEP_SLP_EXIT_EXIT_ADDR
fun_set_mod_reg_bit(rtc, deep_slp_exit, exit_addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_HOT_RST_FLAG
void set_rtc_hot_rst_flag(u32 reg_addr,
			u32 flag,
			u32 m_or_r)
{
	reg_rtc_hot_rst_flag_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.flag = flag;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_HOT_RST_FLAG_FLAG
fun_set_mod_reg_bit(rtc, hot_rst_flag, flag, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_HOT_RST_EXIT
void set_rtc_hot_rst_exit(u32 reg_addr,
			u32 exit_addr,
			u32 m_or_r)
{
	reg_rtc_hot_rst_exit_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.exit_addr = exit_addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_HOT_RST_EXIT_EXIT_ADDR
fun_set_mod_reg_bit(rtc, hot_rst_exit, exit_addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_PAD_HOLD
void set_rtc_pad_hold(u32 reg_addr,
			u32 ddr_pad,
			u32 m_or_r)
{
	reg_rtc_pad_hold_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ddr_pad = ddr_pad;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_PAD_HOLD_DDR_PAD
fun_set_mod_reg_bit(rtc, pad_hold, ddr_pad, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SYS_STAT
void set_rtc_sys_stat(u32 reg_addr,
			u32 stat,
			u32 m_or_r)
{
	reg_rtc_sys_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.stat = stat;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SYS_STAT_STAT
fun_set_mod_reg_bit(rtc, sys_stat, stat, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_AX_CLAMP
void set_rtc_ax_clamp(u32 reg_addr,
			u32 ax,
			u32 m_or_r)
{
	reg_rtc_ax_clamp_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ax = ax;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_AX_CLAMP_AX
fun_set_mod_reg_bit(rtc, ax_clamp, ax, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0
void set_rtc_sio_func_r0(u32 reg_addr,
			u32 sio0,
			u32 sio1,
			u32 sio2,
			u32 sio3,
			u32 sio4,
			u32 sio5,
			u32 sio6,
			u32 sio7,
			u32 m_or_r)
{
	reg_rtc_sio_func_r0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio0 = sio0;
	reg.bits.sio1 = sio1;
	reg.bits.sio2 = sio2;
	reg.bits.sio3 = sio3;
	reg.bits.sio4 = sio4;
	reg.bits.sio5 = sio5;
	reg.bits.sio6 = sio6;
	reg.bits.sio7 = sio7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO0
fun_set_mod_reg_bit(rtc, sio_func_r0, sio0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO1
fun_set_mod_reg_bit(rtc, sio_func_r0, sio1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO2
fun_set_mod_reg_bit(rtc, sio_func_r0, sio2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO3
fun_set_mod_reg_bit(rtc, sio_func_r0, sio3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO4
fun_set_mod_reg_bit(rtc, sio_func_r0, sio4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO5
fun_set_mod_reg_bit(rtc, sio_func_r0, sio5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO6
fun_set_mod_reg_bit(rtc, sio_func_r0, sio6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_FUNC_R0_SIO7
fun_set_mod_reg_bit(rtc, sio_func_r0, sio7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0
void set_rtc_sio_pud_r0(u32 reg_addr,
			u32 sio0,
			u32 sio1,
			u32 sio2,
			u32 sio3,
			u32 sio4,
			u32 sio5,
			u32 sio6,
			u32 sio7,
			u32 m_or_r)
{
	reg_rtc_sio_pud_r0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio0 = sio0;
	reg.bits.sio1 = sio1;
	reg.bits.sio2 = sio2;
	reg.bits.sio3 = sio3;
	reg.bits.sio4 = sio4;
	reg.bits.sio5 = sio5;
	reg.bits.sio6 = sio6;
	reg.bits.sio7 = sio7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO0
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO1
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO2
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO3
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO4
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO5
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO6
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PUD_R0_SIO7
fun_set_mod_reg_bit(rtc, sio_pud_r0, sio7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0
void set_rtc_sio_drv_r0(u32 reg_addr,
			u32 sio0,
			u32 sio1,
			u32 sio2,
			u32 sio3,
			u32 sio4,
			u32 sio5,
			u32 sio6,
			u32 sio7,
			u32 m_or_r)
{
	reg_rtc_sio_drv_r0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio0 = sio0;
	reg.bits.sio1 = sio1;
	reg.bits.sio2 = sio2;
	reg.bits.sio3 = sio3;
	reg.bits.sio4 = sio4;
	reg.bits.sio5 = sio5;
	reg.bits.sio6 = sio6;
	reg.bits.sio7 = sio7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO0
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO1
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO2
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO3
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO4
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO5
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO6
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DRV_R0_SIO7
fun_set_mod_reg_bit(rtc, sio_drv_r0, sio7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_DATA_R0
void set_rtc_sio_data_r0(u32 reg_addr,
			u32 sio_data,
			u32 m_or_r)
{
	reg_rtc_sio_data_r0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio_data = sio_data;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_DATA_R0_SIO_DATA
fun_set_mod_reg_bit(rtc, sio_data_r0, sio_data, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_PR_R
void set_rtc_sio_pr_r(u32 reg_addr,
			u32 pull_cfg,
			u32 m_or_r)
{
	reg_rtc_sio_pr_r_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pull_cfg = pull_cfg;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_PR_R_PULL_CFG
fun_set_mod_reg_bit(rtc, sio_pr_r, pull_cfg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0
void set_rtc_sio_int_tri_r0(u32 reg_addr,
				u32 sio0,
				u32 sio1,
				u32 sio2,
				u32 sio3,
				u32 sio4,
				u32 sio5,
				u32 sio6,
				u32 sio7,
				u32 m_or_r)
{
	reg_rtc_sio_int_tri_r0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio0 = sio0;
	reg.bits.sio1 = sio1;
	reg.bits.sio2 = sio2;
	reg.bits.sio3 = sio3;
	reg.bits.sio4 = sio4;
	reg.bits.sio5 = sio5;
	reg.bits.sio6 = sio6;
	reg.bits.sio7 = sio7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO0
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO1
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO2
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO3
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO4
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO5
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO6
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_TRI_R0_SIO7
fun_set_mod_reg_bit(rtc, sio_int_tri_r0, sio7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_MASK0
void set_rtc_sio_int_mask0(u32 reg_addr,
			u32 sio_int_mask,
			u32 m_or_r)
{
	reg_rtc_sio_int_mask0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio_int_mask = sio_int_mask;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_INT_MASK0_SIO_INT_MASK
fun_set_mod_reg_bit(rtc, sio_int_mask0, sio_int_mask, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_PEND0
void set_rtc_sio_int_pend0(u32 reg_addr,
			u32 sio_int_pend,
			u32 m_or_r)
{
	reg_rtc_sio_int_pend0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sio_int_pend = sio_int_pend;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_INT_PEND0_SIO_INT_PEND
fun_set_mod_reg_bit(rtc, sio_int_pend0, sio_int_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_SAMP_CTRL0
void set_rtc_sio_int_samp_ctrl0(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_rtc_sio_int_samp_ctrl0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_RTC_SIO_INT_SAMP_CTRL0_SRC_SEL
fun_set_mod_reg_bit(rtc, sio_int_samp_ctrl0, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_SAMP_CTRL0_DIV
fun_set_mod_reg_bit(rtc, sio_int_samp_ctrl0, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_RTC_SIO_INT_SAMP_CTRL0_EN
fun_set_mod_reg_bit(rtc, sio_int_samp_ctrl0, en, addr, val, m_or_r)
#endif

#endif /* ___RTC__SET___C___ */

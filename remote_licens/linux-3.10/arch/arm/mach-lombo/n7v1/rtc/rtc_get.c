/* rtc_get.c */

#ifndef ___RTC__GET___C___
#define ___RTC__GET___C___

#ifdef DEF_GET_RTC_VER_VER_L
fun_get_mod_reg_bit(rtc, ver, ver_l, addr)
#endif

#ifdef DEF_GET_RTC_VER_VER_H
fun_get_mod_reg_bit(rtc, ver, ver_h, addr)
#endif

#ifdef DEF_GET_RTC_VER_COMP
fun_get_mod_reg_bit(rtc, ver, comp, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CLK_CTRL_SEL
fun_get_mod_reg_bit(rtc, rtc_clk_ctrl, sel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CLK_CTRL_STAT
fun_get_mod_reg_bit(rtc, rtc_clk_ctrl, stat, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CLK_CTRL_RCOSC_DIV
fun_get_mod_reg_bit(rtc, rtc_clk_ctrl, rcosc_div, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CLK_CTRL_KEY_FIELD
fun_get_mod_reg_bit(rtc, rtc_clk_ctrl, key_field, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LD_EN_EN
fun_get_mod_reg_bit(rtc, rtc_ld_en, en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LD_INT_EN_EN
fun_get_mod_reg_bit(rtc, rtc_ld_int_en, en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LD_INT_CLR_CLR
fun_get_mod_reg_bit(rtc, rtc_ld_int_clr, clr, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LD_INT_PENDING_PENDING
fun_get_mod_reg_bit(rtc, rtc_ld_int_pending, pending, addr)
#endif

#ifdef DEF_GET_RTC_RTC_YMD_DAY
fun_get_mod_reg_bit(rtc, rtc_ymd, day, addr)
#endif

#ifdef DEF_GET_RTC_RTC_YMD_MONTH
fun_get_mod_reg_bit(rtc, rtc_ymd, month, addr)
#endif

#ifdef DEF_GET_RTC_RTC_YMD_YEAR
fun_get_mod_reg_bit(rtc, rtc_ymd, year, addr)
#endif

#ifdef DEF_GET_RTC_RTC_WEEK_WEEK
fun_get_mod_reg_bit(rtc, rtc_week, week, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HMS_SECOND
fun_get_mod_reg_bit(rtc, rtc_hms, second, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HMS_MINUTE
fun_get_mod_reg_bit(rtc, rtc_hms, minute, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HMS_HOUR
fun_get_mod_reg_bit(rtc, rtc_hms, hour, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_INT_EN_EN
fun_get_mod_reg_bit(rtc, rtc_alarm_int_en, en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_INT_CLR_CLR
fun_get_mod_reg_bit(rtc, rtc_alarm_int_clr, clr, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_INT_PENDING_PENDING
fun_get_mod_reg_bit(rtc, rtc_alarm_int_pending, pending, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_MATCH_EN_SEC_ALARM_MATCH_EN
fun_get_mod_reg_bit(rtc, rtc_alarm_match_en, sec_alarm_match_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_MATCH_EN_MIN_ALARM_MATCH_EN
fun_get_mod_reg_bit(rtc, rtc_alarm_match_en, min_alarm_match_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_MATCH_EN_HOUR_ALARM_MATCH_EN
fun_get_mod_reg_bit(rtc, rtc_alarm_match_en, hour_alarm_match_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_MATCH_EN_WEEK_ALARM_MATCH_EN
fun_get_mod_reg_bit(rtc, rtc_alarm_match_en, week_alarm_match_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_MATCH_EN_DAY_ALARM_MATCH_EN
fun_get_mod_reg_bit(rtc, rtc_alarm_match_en, day_alarm_match_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_DAY_MATCH_DAY_MATCH
fun_get_mod_reg_bit(rtc, rtc_alarm_day_match, day_match, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_WEEK_MATCH_WEEK_MATCH
fun_get_mod_reg_bit(rtc, rtc_alarm_week_match, week_match, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_HMS_MATCH_SECOND_MATCH
fun_get_mod_reg_bit(rtc, rtc_alarm_hms_match, second_match, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_HMS_MATCH_MINUTE_MATCH
fun_get_mod_reg_bit(rtc, rtc_alarm_hms_match, minute_match, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALARM_HMS_MATCH_HOUR_MATCH
fun_get_mod_reg_bit(rtc, rtc_alarm_hms_match, hour_match, addr)
#endif

#ifdef DEF_GET_RTC_RTC_WAKEUP_EN_WAKEUP_EN
fun_get_mod_reg_bit(rtc, rtc_wakeup_en, wakeup_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_FANOUT_CFG_FUNC
fun_get_mod_reg_bit(rtc, rtc_lfeosc_fanout_cfg, func, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_FANOUT_CFG_DAT
fun_get_mod_reg_bit(rtc, rtc_lfeosc_fanout_cfg, dat, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_FANOUT_CFG_LFEOSC_FANOUT_EN
fun_get_mod_reg_bit(rtc, rtc_lfeosc_fanout_cfg, lfeosc_fanout_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_SU_EN_EN
fun_get_mod_reg_bit(rtc, rtc_su_en, en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_SU_EN_RATE
fun_get_mod_reg_bit(rtc, rtc_su_en, rate, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_OSC1_EN
fun_get_mod_reg_bit(rtc, rtc_core, osc1_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_VREF_SEL
fun_get_mod_reg_bit(rtc, rtc_core, vref_sel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_LDO_SEL
fun_get_mod_reg_bit(rtc, rtc_core, ldo_sel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_DIV_SEL
fun_get_mod_reg_bit(rtc, rtc_core, div_sel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_DIV_EN
fun_get_mod_reg_bit(rtc, rtc_core, div_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_TSD
fun_get_mod_reg_bit(rtc, rtc_core, tsd, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_TSA
fun_get_mod_reg_bit(rtc, rtc_core, tsa, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_BG1_EN
fun_get_mod_reg_bit(rtc, rtc_core, bg1_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_CORE_BG0_EN
fun_get_mod_reg_bit(rtc, rtc_core, bg0_en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_TOP_ENO
fun_get_mod_reg_bit(rtc, rtc_top, eno, addr)
#endif

#ifdef DEF_GET_RTC_RTC_TOP_AD
fun_get_mod_reg_bit(rtc, rtc_top, ad, addr)
#endif

#ifdef DEF_GET_RTC_RTC_TOP_SELD
fun_get_mod_reg_bit(rtc, rtc_top, seld, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_EN
fun_get_mod_reg_bit(rtc, rtc_hfeosc, en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_CAPEN
fun_get_mod_reg_bit(rtc, rtc_hfeosc, capen, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_SMTSEL
fun_get_mod_reg_bit(rtc, rtc_hfeosc, smtsel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_SMTEN
fun_get_mod_reg_bit(rtc, rtc_hfeosc, smten, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_LPFEN
fun_get_mod_reg_bit(rtc, rtc_hfeosc, lpfen, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_CAPSEL_I
fun_get_mod_reg_bit(rtc, rtc_hfeosc, capsel_i, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_CAPSEL_O
fun_get_mod_reg_bit(rtc, rtc_hfeosc, capsel_o, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_GMSEL
fun_get_mod_reg_bit(rtc, rtc_hfeosc, gmsel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_HFEOSC_RSEL
fun_get_mod_reg_bit(rtc, rtc_hfeosc, rsel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_EN
fun_get_mod_reg_bit(rtc, rtc_lfeosc, en, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_CAPEN
fun_get_mod_reg_bit(rtc, rtc_lfeosc, capen, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_SMTSEL
fun_get_mod_reg_bit(rtc, rtc_lfeosc, smtsel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_SMTEN
fun_get_mod_reg_bit(rtc, rtc_lfeosc, smten, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_LPFEN
fun_get_mod_reg_bit(rtc, rtc_lfeosc, lpfen, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_CAPSEL_I
fun_get_mod_reg_bit(rtc, rtc_lfeosc, capsel_i, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_CAPSEL_O
fun_get_mod_reg_bit(rtc, rtc_lfeosc, capsel_o, addr)
#endif

#ifdef DEF_GET_RTC_RTC_LFEOSC_GMSEL
fun_get_mod_reg_bit(rtc, rtc_lfeosc, gmsel, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALIVE_REG0_REG
fun_get_mod_reg_bit(rtc, rtc_alive_reg0, reg, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALIVE_REG1_REG
fun_get_mod_reg_bit(rtc, rtc_alive_reg1, reg, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALIVE_REG2_REG
fun_get_mod_reg_bit(rtc, rtc_alive_reg2, reg, addr)
#endif

#ifdef DEF_GET_RTC_RTC_ALIVE_REG3_REG
fun_get_mod_reg_bit(rtc, rtc_alive_reg3, reg, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE_EN
fun_get_mod_reg_bit(rtc, pm_pe, en, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE_DDR_EN
fun_get_mod_reg_bit(rtc, pm_pe, ddr_en, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE_KEY_FILED
fun_get_mod_reg_bit(rtc, pm_pe, key_filed, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_G_INT_TRIG
fun_get_mod_reg_bit(rtc, pm_cfg, g_int_trig, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_PIR_WAKE_TRIG
fun_get_mod_reg_bit(rtc, pm_cfg, pir_wake_trig, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_RING_KEY_TRIG
fun_get_mod_reg_bit(rtc, pm_cfg, ring_key_trig, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_GS_WAKE_DETECT_EN
fun_get_mod_reg_bit(rtc, pm_cfg, gs_wake_detect_en, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_PIR_WAKE_DETECT_EN
fun_get_mod_reg_bit(rtc, pm_cfg, pir_wake_detect_en, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_RING_KEY_WAKE_DET_EN
fun_get_mod_reg_bit(rtc, pm_cfg, ring_key_wake_det_en, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_KEY_SLONG_EN
fun_get_mod_reg_bit(rtc, pm_cfg, key_slong_en, addr)
#endif

#ifdef DEF_GET_RTC_PM_CFG_KEY_FILED
fun_get_mod_reg_bit(rtc, pm_cfg, key_filed, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_PWR_EN
fun_get_mod_reg_bit(rtc, pm_int_en, pwr_en, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_KEY_PRESS
fun_get_mod_reg_bit(rtc, pm_int_en, key_press, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_KEY_SHORT
fun_get_mod_reg_bit(rtc, pm_int_en, key_short, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_KEY_LONG
fun_get_mod_reg_bit(rtc, pm_int_en, key_long, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_KEY_RELEASE
fun_get_mod_reg_bit(rtc, pm_int_en, key_release, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_GS_INT
fun_get_mod_reg_bit(rtc, pm_int_en, gs_int, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_PIR_WAKE
fun_get_mod_reg_bit(rtc, pm_int_en, pir_wake, addr)
#endif

#ifdef DEF_GET_RTC_PM_INT_EN_RING_KEY
fun_get_mod_reg_bit(rtc, pm_int_en, ring_key, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_PW_CON
fun_get_mod_reg_bit(rtc, pm_pend_clr, pw_con, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_PW_DISCON
fun_get_mod_reg_bit(rtc, pm_pend_clr, pw_discon, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_KEY_PRESS
fun_get_mod_reg_bit(rtc, pm_pend_clr, key_press, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_KEY_SHORT
fun_get_mod_reg_bit(rtc, pm_pend_clr, key_short, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_KEY_LONG
fun_get_mod_reg_bit(rtc, pm_pend_clr, key_long, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_KEY_RELEASE
fun_get_mod_reg_bit(rtc, pm_pend_clr, key_release, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_GS_INT
fun_get_mod_reg_bit(rtc, pm_pend_clr, gs_int, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_PIR_WAKE
fun_get_mod_reg_bit(rtc, pm_pend_clr, pir_wake, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_CLR_RING_KEY
fun_get_mod_reg_bit(rtc, pm_pend_clr, ring_key, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_PW_CON
fun_get_mod_reg_bit(rtc, pm_pend, pw_con, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_PW_DISCON
fun_get_mod_reg_bit(rtc, pm_pend, pw_discon, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_KEY_PRESS
fun_get_mod_reg_bit(rtc, pm_pend, key_press, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_KEY_SHORT
fun_get_mod_reg_bit(rtc, pm_pend, key_short, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_KEY_LONG
fun_get_mod_reg_bit(rtc, pm_pend, key_long, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_KEY_RELEASE
fun_get_mod_reg_bit(rtc, pm_pend, key_release, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_GS_INT
fun_get_mod_reg_bit(rtc, pm_pend, gs_int, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_PIR_WAKE
fun_get_mod_reg_bit(rtc, pm_pend, pir_wake, addr)
#endif

#ifdef DEF_GET_RTC_PM_PEND_RING_KEY
fun_get_mod_reg_bit(rtc, pm_pend, ring_key, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE1_EN
fun_get_mod_reg_bit(rtc, pm_pe1, en, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE1_POL
fun_get_mod_reg_bit(rtc, pm_pe1, pol, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE1_TP
fun_get_mod_reg_bit(rtc, pm_pe1, tp, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE1_AP
fun_get_mod_reg_bit(rtc, pm_pe1, ap, addr)
#endif

#ifdef DEF_GET_RTC_PM_STAT_PWR_CON
fun_get_mod_reg_bit(rtc, pm_stat, pwr_con, addr)
#endif

#ifdef DEF_GET_RTC_PM_PKT_PWR_UP
fun_get_mod_reg_bit(rtc, pm_pkt, pwr_up, addr)
#endif

#ifdef DEF_GET_RTC_PM_PKT_KEY_LONG
fun_get_mod_reg_bit(rtc, pm_pkt, key_long, addr)
#endif

#ifdef DEF_GET_RTC_PM_PKT_KEY_SLONG
fun_get_mod_reg_bit(rtc, pm_pkt, key_slong, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE2_SWI
fun_get_mod_reg_bit(rtc, pm_pe2, swi, addr)
#endif

#ifdef DEF_GET_RTC_PM_PE2_KEY_FILED
fun_get_mod_reg_bit(rtc, pm_pe2, key_filed, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_DEEP_SLP_EXIT
fun_get_mod_reg_bit(rtc, lock_ctrl, deep_slp_exit, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_DEEP_SLP_FLAG
fun_get_mod_reg_bit(rtc, lock_ctrl, deep_slp_flag, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_HOT_RST_EXIT
fun_get_mod_reg_bit(rtc, lock_ctrl, hot_rst_exit, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_HOT_RST_FLAG
fun_get_mod_reg_bit(rtc, lock_ctrl, hot_rst_flag, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_PAD_HOLD
fun_get_mod_reg_bit(rtc, lock_ctrl, pad_hold, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_SYS_STAT
fun_get_mod_reg_bit(rtc, lock_ctrl, sys_stat, addr)
#endif

#ifdef DEF_GET_RTC_LOCK_CTRL_KEYFIELD
fun_get_mod_reg_bit(rtc, lock_ctrl, keyfield, addr)
#endif

#ifdef DEF_GET_RTC_DEEP_SLP_FLAG_FLAG
fun_get_mod_reg_bit(rtc, deep_slp_flag, flag, addr)
#endif

#ifdef DEF_GET_RTC_DEEP_SLP_EXIT_EXIT_ADDR
fun_get_mod_reg_bit(rtc, deep_slp_exit, exit_addr, addr)
#endif

#ifdef DEF_GET_RTC_HOT_RST_FLAG_FLAG
fun_get_mod_reg_bit(rtc, hot_rst_flag, flag, addr)
#endif

#ifdef DEF_GET_RTC_HOT_RST_EXIT_EXIT_ADDR
fun_get_mod_reg_bit(rtc, hot_rst_exit, exit_addr, addr)
#endif

#ifdef DEF_GET_RTC_PAD_HOLD_DDR_PAD
fun_get_mod_reg_bit(rtc, pad_hold, ddr_pad, addr)
#endif

#ifdef DEF_GET_RTC_SYS_STAT_STAT
fun_get_mod_reg_bit(rtc, sys_stat, stat, addr)
#endif

#ifdef DEF_GET_RTC_AX_CLAMP_AX
fun_get_mod_reg_bit(rtc, ax_clamp, ax, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO0
fun_get_mod_reg_bit(rtc, sio_func_r0, sio0, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO1
fun_get_mod_reg_bit(rtc, sio_func_r0, sio1, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO2
fun_get_mod_reg_bit(rtc, sio_func_r0, sio2, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO3
fun_get_mod_reg_bit(rtc, sio_func_r0, sio3, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO4
fun_get_mod_reg_bit(rtc, sio_func_r0, sio4, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO5
fun_get_mod_reg_bit(rtc, sio_func_r0, sio5, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO6
fun_get_mod_reg_bit(rtc, sio_func_r0, sio6, addr)
#endif

#ifdef DEF_GET_RTC_SIO_FUNC_R0_SIO7
fun_get_mod_reg_bit(rtc, sio_func_r0, sio7, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO0
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio0, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO1
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio1, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO2
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio2, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO3
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio3, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO4
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio4, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO5
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio5, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO6
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio6, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PUD_R0_SIO7
fun_get_mod_reg_bit(rtc, sio_pud_r0, sio7, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO0
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio0, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO1
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio1, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO2
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio2, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO3
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio3, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO4
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio4, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO5
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio5, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO6
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio6, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DRV_R0_SIO7
fun_get_mod_reg_bit(rtc, sio_drv_r0, sio7, addr)
#endif

#ifdef DEF_GET_RTC_SIO_DATA_R0_SIO_DATA
fun_get_mod_reg_bit(rtc, sio_data_r0, sio_data, addr)
#endif

#ifdef DEF_GET_RTC_SIO_PR_R_PULL_CFG
fun_get_mod_reg_bit(rtc, sio_pr_r, pull_cfg, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO0
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio0, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO1
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio1, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO2
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio2, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO3
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio3, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO4
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio4, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO5
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio5, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO6
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio6, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_TRI_R0_SIO7
fun_get_mod_reg_bit(rtc, sio_int_tri_r0, sio7, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_MASK0_SIO_INT_MASK
fun_get_mod_reg_bit(rtc, sio_int_mask0, sio_int_mask, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_PEND0_SIO_INT_PEND
fun_get_mod_reg_bit(rtc, sio_int_pend0, sio_int_pend, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_SAMP_CTRL0_SRC_SEL
fun_get_mod_reg_bit(rtc, sio_int_samp_ctrl0, src_sel, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_SAMP_CTRL0_DIV
fun_get_mod_reg_bit(rtc, sio_int_samp_ctrl0, div, addr)
#endif

#ifdef DEF_GET_RTC_SIO_INT_SAMP_CTRL0_EN
fun_get_mod_reg_bit(rtc, sio_int_samp_ctrl0, en, addr)
#endif

#endif /* ___RTC__GET___C___ */

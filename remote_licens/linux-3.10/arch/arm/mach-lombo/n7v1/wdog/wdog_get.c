/* wdog_get.c */

#ifndef ___WDOG__GET___C___
#define ___WDOG__GET___C___

#ifdef DEF_GET_WDOG_WDOG_VER_VER_L
fun_get_mod_reg_bit(wdog, wdog_ver, ver_l, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_VER_VER_H
fun_get_mod_reg_bit(wdog, wdog_ver, ver_h, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_VER_COMP
fun_get_mod_reg_bit(wdog, wdog_ver, comp, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CLK_CTRL_SEL
fun_get_mod_reg_bit(wdog, wdog_clk_ctrl, sel, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CTRL_EN
fun_get_mod_reg_bit(wdog, wdog_ctrl, en, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CTRL_RMOD
fun_get_mod_reg_bit(wdog, wdog_ctrl, rmod, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CTRL_RPL
fun_get_mod_reg_bit(wdog, wdog_ctrl, rpl, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CTRL_SU
fun_get_mod_reg_bit(wdog, wdog_ctrl, su, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_TMRPERIOD_TMRPERIOD
fun_get_mod_reg_bit(wdog, wdog_tmrperiod, tmrperiod, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CNT_CUR_VAL_CNT_CUR_VAL
fun_get_mod_reg_bit(wdog, wdog_cnt_cur_val, cnt_cur_val, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_RESTART_RESTART
fun_get_mod_reg_bit(wdog, wdog_restart, restart, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_INT_EN_ENABLE
fun_get_mod_reg_bit(wdog, wdog_int_en, enable, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_CLR_INT_PENDING_CLR
fun_get_mod_reg_bit(wdog, wdog_clr_int_pending, clr, addr)
#endif

#ifdef DEF_GET_WDOG_WDOG_INT_PENDING_PENDING
fun_get_mod_reg_bit(wdog, wdog_int_pending, pending, addr)
#endif

#endif /* ___WDOG__GET___C___ */

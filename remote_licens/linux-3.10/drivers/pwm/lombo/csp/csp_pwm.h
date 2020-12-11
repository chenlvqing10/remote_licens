/*
 * csp_pwm.h - pwm csp head file
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __CSP_PWM_H___
#define __CSP_PWM_H___

#include <linux/types.h>
#include <mach/csp.h>
#include <mach/debug.h>

#define NPWM		10	/* pwm channel number */
#define PWM_IRQ_NUM	NPWM	/* pwm irq channel number */

/*
 * the below are for pwm reg relevant defination
 */

/* pwm register offset defination */
#define PWM_VER_OFF		0x0000	/* version reg */
#define PWM_CE_OFF		0x0004	/* counter enable reg */
#define PWM_OPE_OFF		0x0008	/* one pulse enable reg */
#define PWM_INT_EN_OFF		0x0010	/* int enable reg */
#define PWM_CLR_INT_PEND_OFF	0x0014	/* clear int pending reg */
#define PWM_INT_PEND_OFF	0x0018	/* int pending reg */
/* note: n<<6 is equal to n*0x40 */
#define PWM_CLK_CTRL_OFF(n)	(0x00 + (((n) + 1) << 6)) /* clock ctrl reg */
#define PWM_CNT_STORE_OFF(n)	(0x04 + (((n) + 1) << 6)) /* cnt store reg */
#define PWM_CNT_CMP_OFF(n)	(0x08 + (((n) + 1) << 6)) /* cnt cmp reg */
#define PWM_CTRL_OFF(n)		(0x0c + (((n) + 1) << 6)) /* control reg */
#define PWM_CNT_CAP_OFF(n)	(0x10 + (((n) + 1) << 6)) /* cnt capture reg */
#define PWM_IN_STS_OFF(n)	(0x14 + (((n) + 1) << 6)) /* input sta reg */
#ifdef CONFIG_ARCH_LOMBO_N7V1
#define PWM_DLY_CNT_OFF(n)	(0x18 + (((n) + 1) << 6)) /* delay cnt reg */
#define PWM_PULSE_CNT_OFF(n)	(0x1c + (((n) + 1) << 6)) /* pulse cnt reg */
#endif

/*
 * pwm control command, relevant to control reg
 */
#define PWM_PI_CAPTURE_EN	0	/* pwm p input capture enable */
#define PWM_PI_CAPTURE_RVS	1	/* pwm p input capture reverse */
#define PWM_FE_DETECT_EN	2	/* falling edge detect enable */
#define PWM_RE_DETECT_EN	3	/* rising edge detect enable */
#define PWM_NO_POLARITY_SET	4	/* pwm n output polarity set */
#define PWM_NO_EN		5	/* pwm n output enable */
#define PWM_PO_POLARITY_SET	6	/* pwm p output polarity set */
#define PWM_PO_EN		7	/* pwm p output enable */
#define PWM_MODE_SET		8	/* pwm mode set */
#define PWM_DEADTIME_SET	9	/* pwm dead-time set */
#define PWM_DEADTIME_EN		10	/* pwm dead-time enable */

/*
 * pwm clock config defination
 */
enum n7_pwm_clk_src {
	CLKSRC_HFEOSC,	/* clock source is HFEOSC 24m */
	CLKSRC_NULL	/* clock source is NULL */
};

struct n7_pwm_clk_cfg {
	enum n7_pwm_clk_src src; /* clock source */
	u8 bypass;	/* 1: clk src bypass enable, 0: disable */
	u8 gating;	/* 1: clk src gating enable, 0: disable */
	u8 div;		/* clock source divider(reg value should be div-1) */
};

/*
 * pwm control defination
 */

enum n7_pwm_mode {
	PERIOD_OUT,	/* period output mode */
	PULSE_OUT,	/* pulse output mode */
	IN_CAP		/* input capture mode */
};

/* pwm output polarity(for n7)
 * note: if polarity is POLARITY_LOW,  cmp=3, cnt=5, then output is llhhh
 *       if polarity is POLARITY_HIGH, cmp=3, cnt=5, then output is hhlll
 *
 * linux pwm standard flag:
 *   PWM_POLARITY_NORMAL: a high signal for the duration of the duty-
 *     cycle, followed by a low signal for the remainder of the pulse period
 *   PWM_POLARITY_INVERSED: a low signal for the duration of the duty-
 *     cycle, followed by a high signal for the remainder of the pulse period
 *
 * so: POLARITY_LOW is corresponding to PWM_POLARITY_NORMAL, and
 *     POLARITY_HIGH is corresponding to PWM_POLARITY_INVERSED
 */
enum n7_pwm_pol {
	POLARITY_LOW,	/* output polarity low */
	POLARITY_HIGH	/* output polarity high */
};

void csp_pwm_cnt_endisable(int pwm, void *reg_base, bool en);
void csp_pwm_ops_endisable(int pwm, void *reg_base, bool en);
void csp_pwm_irq_endisable(int irq, void *reg_base, bool en);
void csp_pwm_irq_clr_pend(int irq, void *reg_base);
int csp_pwm_irq_get_pend(int irq, void *reg_base);
int csp_pwm_get_irq_pend_regval(void *reg_base);
void csp_pwm_clk_config(int pwm, void *reg_base, struct n7_pwm_clk_cfg *cfg);
int csp_pwm_clk_get_rate(int pwm, void *reg_base);
void csp_pwm_set_store(int pwm, void *reg_base, int val);
void csp_pwm_set_cmp(int pwm, void *reg_base, int val);
void csp_pwm_control(int pwm, int cmd, void *reg_base, int para);
int csp_pwm_get_f_cap(int pwm, void *reg_base);
int csp_pwm_get_r_cap(int pwm, void *reg_base);
int csp_pwm_get_fflag(int pwm, void *reg_base);
int csp_pwm_get_rflag(int pwm, void *reg_base);
int csp_pwm_get_offlag(int pwm, void *reg_base);
int csp_pwm_get_orflag(int pwm, void *reg_base);

#endif /* __CSP_PWM_H___ */


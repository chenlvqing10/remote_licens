/*
 * csp.h - the chip operations file for RTC
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

#ifndef __RTC_CSP_H__
#define __RTC_CSP_H__
#include <mach/csp.h>
#include <mach/debug.h>

#define LOMBO_RTC_VER		(0x00) /* version register */
#define LOMBO_RTC_CLK_CTRL	(0x20) /* clock control register */
#define LOMBO_RTC_LD_EN		(0x24) /* LFEOSC detect enable register */
#define LOMBO_RTC_LD_INT_EN	(0x28) /* LFEOSC detect IRQ enable register */
#define LOMBO_RTC_LD_INT_CLR	(0x2C) /* LFEOSC detect clear IRQ pending
					* register */
#define LOMBO_RTC_LD_INT_PD	(0x30) /* LFEOSC detect IRQ pending register */
#define LOMBO_RTC_YMD		(0x40) /* year-mon-day register */
#define LOMBO_RTC_WEEK		(0x44) /* week register */
#define LOMBO_RTC_HMS		(0x48) /* hour-min-sec register */
#define LOMBO_RTC_ALM_INT_EN	(0x4C) /* alarm IRQ enable register */
#define LOMBO_RTC_ALM_INT_CLR	(0x50) /* alarm clear IRQ pending register */
#define LOMBO_RTC_ALM_INT_PD	(0x54) /* alarm IRQ pending register */
#define LOMBO_RTC_ALM_MAT_EN	(0x58) /* alarm match enable register */
#define LOMBO_RTC_ALM_DAY_MAT	(0x5C) /* alarm day match register */
#define LOMBO_RTC_ALM_WEEK_MAT	(0x60) /* alarm week match register */
#define LOMBO_RTC_ALM_HMS_MAT	(0x64) /* alarm hour-min-sec match register */
#define LOMBO_RTC_WAKEUP_EN	(0x68) /* wakeup enable register */
#define LOMBO_RTC_LFEOSC_OUT_EN	(0x6C) /* LFEOSC fanout enable register */
#define LOMBO_RTC_SU_EN		(0x70) /* speed up enable register */
#define RTC_GET_ALM_PENDING	(0xFE)

/* ALARM MATCH ENABLE ITEM */
#define ALARM_DAY_MATCH_EN	0x0010
#define ALARM_WEEK_MATCH_EN	0x0008
#define ALARM_HOUR_MATCH_EN	0x0004
#define ALARM_MINUTE_MATCH_EN	0x0002
#define ALARM_SECOND_MATCH_EN	0x0001

#define assert(expr)	do { if (!(expr))				\
				PRT_ERR("assert fail: %s %d",		\
					__func__, __LINE__); } while (0)

/* ALARM RANGE */
#define YEAR_MAX		2100
#define YEAR_MIN		1900
#define MONTH_MAX		12
#define MONTH_MIN		1
#define LPYEAR_MON_DAYS		29 /* days of February in leap year */
#define NLPYEAR_MON_DAYS	28 /* days of February in non leap year */
#define LIT_MON_DAYS		30 /* a solar month of 30 days */
#define BIG_MON_DAYS		31 /* a solar month of 31 days */
#define DAYS_MIN		1
#define WEEK_MIN		0 /* 0, sunday; 1~6, monday~satturday */
#define WEEK_MAX		6
#define HOUR_MIN		0
#define HOUR_MAX		23
#define MINUTE_MIN		0
#define MINUTE_MAX		59
#define SECOND_MIN		0
#define SECOND_MAX		59

/*
 * csp_rtc_get_ver - get the version of RTC
 * @param: none
 *
 * return value of the RTC version
 */
u32 csp_rtc_get_ver(void *base);

/*
 * csp_rtc_set_clk_src - set the CLK source of RTC
 * @clk_src: 1, LFEOSC; 0, RCOSC
 *
 * LFEOSC is recommended because it is high precision
 */
void csp_rtc_set_clk_src(void *base, u32 clk_src);

/*
 * csp_rtc_set_clk_rcosc_div - divide the RCOSC
 * @rc_div: frequency division
 *
 * frequency obtained = RCOSC / 32 / (rc_div + 1)
 */
void csp_rtc_set_clk_rcosc_div(void *base, u32 rc_div);

/*
 * csp_rtc_get_clk_src_stat - get source of RTC clock
 * @param: none
 *
 * return: 1, LFEOSC; 0, RCOSC
 */
u32 csp_rtc_get_clk_src_stat(void *base);

/*
 * csp_rtc_ld_enable - RTC LFEOSC detect enable
 * @ld_enable: 1, enable; 0, disable
 *
 */
void csp_rtc_ld_enable(void *base, u32 ld_enable);

/*
 * csp_rtc_ld_int_enable - RTC LFEOSC detect interrupt enable
 * @ld_int_enable: 1, interrupt enable; 0, interrupt disable
 *
 */
void csp_rtc_ld_int_enable(void *base, u32 ld_int_enable);

/*
 * csp_rtc_get_ld_int_enable - get state of RTC LFEOSC detect interrupt enable
 * @param: none
 *
 * return: 1, interrupt enable; 0, interrupt disable
 */
u32 csp_rtc_get_ld_int_enable(void *base);

/*
 * csp_rtc_ld_int_clr - clear the state of RTC LFEOSC detect interrupt pending
 * @param: none
 *
 * the bit of interrupt is writing 1 clears pending
 */
void csp_rtc_ld_int_clr(void *base);

/*
 * csp_rtc_get_ld_int_pending - get state of LFEOSC detect interrupt pending
 * @param: none
 *
 * return: 1, interrupt is pending; 0, interrupt is not pending
 */
u32 csp_rtc_get_ld_int_pending(void *base);

/*
 * csp_rtc_set_ymd - set date(year, month, day)
 * @day: range is depend on month
 * @month: 1~12
 * @year: 1900~2100
 *
 */
void csp_rtc_set_ymd(void *base, u32 day, u32 month, u32 year);

/*
 * csp_rtc_get_ymd - get date(year, month, day)
 * @param: none
 *
 * return: date
 */
u32 csp_rtc_get_ymd(void *base);

/*
 * csp_rtc_set_week - set week
 * @week: range 0~6; 0, sunday; 1~6, monday~saturday
 *
 */
void csp_rtc_set_week(void *base, u32 week);

/*
 * csp_rtc_get_week - get week
 * @param: none
 *
 * return: week: 0, sunday; 1~6, monday~saturday
 */
u32 csp_rtc_get_week(void *base);

/*
 * csp_rtc_set_hms - set time(second, minute, hour)
 * @second: 0~60
 * @minute: 0~60
 * @hour: 0~23
 *
 */
void csp_rtc_set_hms(void *base, u32 second, u32 minute, u32 hour);

/*
 * csp_rtc_get_hms - get time(second, minute, hour)
 * @param: none
 *
 * retrun: time
 */
u32 csp_rtc_get_hms(void *base);

/*
 * csp_rtc_alarm_int_enable - RTC alarm interrupt enable
 * @alarm_int_enable: 1, interrupt enable; 0, interrupt disable
 *
 */
void csp_rtc_alarm_int_enable(void *base, u32 alarm_int_enable);

/*
 * csp_rtc_get_alarm_int_enable - get the state of RTC alarm interrupt enable
 * @param: none
 *
 * return: 1, interrupt enable; 0, interrupt disable
 */
u32 csp_rtc_get_alarm_int_enable(void *base);

/*
 * csp_rtc_alarm_int_clr - clear RTC alarm interrupt pending
 * @param: none
 *
 * the bit of interrupt is writing 1 clears pending
 */
void csp_rtc_alarm_int_clr(void *base);

/*
 * csp_rtc_get_alarm_init_pending - get the state of RTC alarm interrupt pending
 * @param: none
 *
 * return: 1, interrupt is pending; 0, interrupt is not pending
 */
u32 csp_rtc_get_alarm_init_pending(void *base);

/*
 * csp_rtc_alarm_match_enable - enable alarm match for counter
 * @alarm_match_enable_item:	0x0010, ALARM_DAY_MATCH_EN;
 *				0x0008, ALARM_WEEK_MATCH_EN;
 *				0x0004, ALARM_HOUR_MATCH_EN;
 *				0x0002, ALARM_MINUTE_MATCH_EN;
 *				0x0001, ALARM_SECOND_MATCH_EN;
 *
 */
void csp_rtc_alarm_match_enable(void *base, int alarm_match_enable_item);

/*
 * csp_rtc_alarm_match_disable - disable alarm match
 * @param: none
 *
 * disable all match of alarm
 */
void csp_rtc_alarm_match_disable(void *base);

/*
 * csp_rtc_alarm_match_get_enable - get alarm match enable stat
 * @param: none
 *
 * return alarm match enable stat
 */
u32 csp_rtc_alarm_match_get_enable(void *base);

/*
 * csp_rtc_set_alarm_day - set the alarm day for match
 * @day: range depends on month
 *
 */
void csp_rtc_set_alarm_day(void *base, u32 day);

/*
 * csp_rtc_get_alarm_day - get the alarm day
 * @param: none
 *
 * return: alarm day
 */
u32 csp_rtc_get_alarm_day(void *base);

/*
 * csp_rtc_set_alarm_week - set the alarm day for match
 * @week: 0~6
 *
 */
void csp_rtc_set_alarm_week(void *base, u32 week);

/*
 * csp_rtc_get_alarm_week - get the alarm week
 * @param: none
 *
 * return: alarm week
 */
u32 csp_rtc_get_alarm_week(void *base);

/*
 * csp_rtc_set_alarm_hms - set the alarm time(hour, minute, second) for match
 * @hour: 0~23
 * @minute: 0~59
 * @second: 0~59
 *
 */
void csp_rtc_set_alarm_hms(void *base, u32 hour, u32 minute, u32 second);

/*
 * csp_rtc_get_alarm_hms - get the alarm time(hour, minute, second)
 * @param: none
 *
 * return: alarm time(hour, minute, second)
 */
u32 csp_rtc_get_alarm_hms(void *base);

/*
 * csp_rtc_wakeup_enable - enable RTC wakeup for power
 * @wakeup_enable: 1, enable; 0, disable
 *
 */
void csp_rtc_wakeup_enable(void *base, u32 wakeup_enable);

/*
 * csp_rtc_lfeosc_fanout_enable - enable LFEOSC fanout
 * @lfeosc_out_enable: 1, enable; 0, disable
 *
 */
void csp_rtc_lfeosc_fanout_enable(void *base, u32 lfeosc_out_enable);

/*
 * csp_rtc_su_enable - enable RTC su
 * @lfeosc_out_enable: 1, enable; 0, disable
 *
 */
void csp_rtc_su_enable(void *base, u32 su_enable);

/*
 * csp_rtc_set_speed - set speed of clock
 * @speed: speed up second rate
 *
 */
void csp_rtc_set_speed(void *base, u32 speed);

#endif

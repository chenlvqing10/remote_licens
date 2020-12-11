/*
 * rtc_power.h - RTC power manage defines for LomboTech SoCs
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

#ifndef __RTC_PM_H
#define __RTC_PM_H

#define PM_INT_EN_RING_KEY	BIT(8)
#define PM_INT_EN_PIR_WAKE	BIT(7)
#define PM_INT_EN_GS_INT	BIT(6)
#define PM_INT_EN_KEY_RELEASE	BIT(5)
#define PM_INT_EN_KEY_LONG	BIT(4)
#define PM_INT_EN_KEY_SHORT	BIT(3)
#define PM_INT_EN_KEY_PRESS	BIT(2)
#define PM_INT_EN_PWR_EN	BIT(0)

#define PM_PEND_RING_KEY	BIT(8)
#define PM_PEND_PIR_WAKE	BIT(7)
#define PM_PEND_GS_INT		BIT(6)
#define PM_PEND_KEY_RELEASE	BIT(5)
#define PM_PEND_KEY_LONG	BIT(4)
#define PM_PEND_KEY_SHORT	BIT(3)
#define PM_PEND_KEY_PRESS	BIT(2)
#define PM_PEND_PM_DISCON	BIT(1)
#define PM_PEND_PM_CON		BIT(0)
#define PM_PEND_ALL		(PM_PEND_GS_INT     |\
				PM_PEND_KEY_RELEASE |\
				PM_PEND_KEY_LONG    |\
				PM_PEND_KEY_SHORT   |\
				PM_PEND_KEY_PRESS   |\
				PM_PEND_PM_DISCON   |\
				PM_PEND_PM_CON	    |\
				PM_PEND_PIR_WAKE    |\
				PM_PEND_RING_KEY)

enum GS_TRIG_LEVEL {
	GS_TRIG_LEVEL_LOW  = 0,
	GS_TRIG_LEVEL_HIGH = 1,
};

/* rtc power manager wake type */
enum PM_WAKE_TYPE {
	PM_WAKE_TYPE_GS = 0,	/* Gsensor or WIFI wake*/
	PM_WAKE_TYPE_PIR,		/* PIR_WAKE */
	PM_WAKE_TYPE_RING,		/* RING_KEY_WAKE */
};

enum PM_WAKE_PIN {
	PM_WAKE_PIN_0 = PM_WAKE_TYPE_GS,
	PM_WAKE_PIN_1 = PM_WAKE_TYPE_PIR,
	PM_WAKE_PIN_2 = PM_WAKE_TYPE_RING,
};


/* wake interrupt level trigger config */
#define PM_WAKE_TRIG_LEVEL_LOW		0
#define PM_WAKE_TRIG_LEVEL_HIGH		1

#if defined(CONFIG_LOMBO_RTC_POWER) || defined(CONFIG_RTC_LOMBO)
extern void csp_rtc_power_en_disable(void *base);
extern void csp_rtc_power_gs_wake_detect_enable(void *base);
extern void csp_rtc_power_gs_wake_detect_disable(void *base);
extern void csp_rtc_power_gs_int_trig_cfg(void *base, u32 trig_level);
extern void csp_rtc_power_enable_irq_by_source(void *base, int source);
extern void csp_rtc_power_disable_irq_by_source(void *base, int source);
extern u32  csp_rtc_power_query_irq_pending(void *base);
extern void csp_rtc_power_clear_irq_pending(void *base, u32 pending);

/* power manager wake */
extern void csp_pm_set_wake_int_en(void *base, enum PM_WAKE_TYPE t, u32 en);
extern void csp_pm_set_wake_detect_en(void *base, enum PM_WAKE_TYPE t, u32 en);
extern void csp_pm_set_wake_trig_cfg(void *base, enum PM_WAKE_TYPE t, u32 trig);
extern void csp_pm_set_wake_func(void *base, enum PM_WAKE_TYPE t);

extern void rtc_pm_set_wake_pin_enable(enum PM_WAKE_PIN pin, u32 en, u32 trig);

#else
void csp_rtc_power_en_disable(void *base)
{
};
void csp_rtc_power_gs_wake_detect_enable(void *base)
{
};
void csp_rtc_power_gs_wake_detect_disable(void *base)
{
};
void csp_rtc_power_gs_int_trig_cfg(void *base, u32 trig_level)
{
};
void csp_rtc_power_enable_irq_by_source(void *base, int source)
{
};
void csp_rtc_power_disable_irq_by_source(void *base, int source)
{
};
u32  csp_rtc_power_query_irq_pending(void *base)
{
	return 0;
};
void csp_rtc_power_clear_irq_pending(void *base, u32 pending)
{
};

/* power manager wake */
void csp_pm_set_wake_int_en(void *base, enum PM_WAKE_TYPE t, u32 en)
{
};
void csp_pm_set_wake_detect_en(void *base, enum PM_WAKE_TYPE t, u32 en)
{
};
void csp_pm_set_wake_trig_cfg(void *base, enum PM_WAKE_TYPE t, u32 trig)
{
};
void csp_pm_set_wake_func(void *base, enum PM_WAKE_TYPE t)
{
};

void rtc_pm_set_wake_pin_enable(enum PM_WAKE_PIN pin, u32 en, u32 trig)
{
}

#endif

#endif /* __RTC_PM_H */

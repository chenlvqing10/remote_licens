/*
 * lombo_rtc.c - Driver of LomboTech RTC module
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

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/irq.h>
#include <mach/rtc_power.h>
#include "n7_rtc.h"
#include "csp/n7/rtc_csp.h"

#undef MOD_NAME
#define MOD_NAME "RTC"

static void __iomem *lombo_rtc_base;

void lombo_lfeosc_fanout_enable(u32 enable)
{
	void __iomem *base = lombo_rtc_base;

	csp_rtc_lfeosc_fanout_enable(base, enable);
}
EXPORT_SYMBOL(lombo_lfeosc_fanout_enable);

/*
 * rtc_pm_set_wake_pin_enable - enable or disable the pmc wake pin
 * @pin: specified wake pin
 * @en: enable or disable
 * @trig: trigger level config, this param will no effect if en is 0
 *
 */
void rtc_pm_set_wake_pin_enable(enum PM_WAKE_PIN pin, u32 en, u32 trig)
{
	void __iomem *base = lombo_rtc_base;
	enum PM_WAKE_TYPE t;

	if (!base) {
		pr_warn("lombo_rtc_base is null.");
		return;
	}

	t = (enum PM_WAKE_TYPE)pin;

	/* wake interrupt enable */
	csp_pm_set_wake_int_en(base, t, en);
	/* wake detected enable */
	csp_pm_set_wake_detect_en(base, t, en);

	/* need set trigger config and sio function when set wake enable */
	if (en) {
		/* wake trigger config */
		csp_pm_set_wake_trig_cfg(base, t, trig);
		/* set sio function for wake */
		csp_pm_set_wake_func(base, t);
	}
}
EXPORT_SYMBOL(rtc_pm_set_wake_pin_enable);

static int lombo_rtc_alarm_irq_enable(struct device *dev, u32 enabled)
{
	void __iomem *base = lombo_rtc_base;

	csp_rtc_alarm_int_enable(base, enabled);

	return 0;
}

static int lombo_rtc_ld_int_enable(void)
{
	void __iomem *base = lombo_rtc_base;

	/* enable LFEOSC detect */
	csp_rtc_ld_enable(base, ENABLE);
	/* enable LFEOSC detect interrupt */
	csp_rtc_ld_int_enable(base, ENABLE);

	return 0;
}

static int lombo_rtc_remove(struct platform_device *pdev)
{
	struct lombo_rtc *rtc = platform_get_drvdata(pdev);

	free_irq(rtc->irq_no, rtc);
	rtc_device_unregister(rtc->rtc_dev);
	platform_set_drvdata(pdev, NULL);

	iounmap(lombo_rtc_base);
	lombo_rtc_alarm_irq_enable(&pdev->dev, 0);

	input_unregister_device(rtc->idev);
	input_free_device(rtc->idev);
	kfree(rtc);

	return 0;
}

static void lombo_rtc_sys_init(void)
{
	u32 clk_src_stat = 0;
	void __iomem *base = lombo_rtc_base;

	/* get RTCCLK src and set to LFEOSC */
	clk_src_stat = csp_rtc_get_clk_src_stat(base);
	if (CLK_SRC_LFEOSC != clk_src_stat)
		csp_rtc_set_clk_src(base, CLK_SRC_LFEOSC);
	/* clr pending */
	csp_rtc_ld_int_clr(base);
	csp_rtc_alarm_int_clr(base);
	/* clear all irq pending when init */
	csp_rtc_power_clear_irq_pending(base, PM_PEND_ALL);

	/* enable rtc power irq */
	csp_rtc_power_enable_irq_by_source(base, PM_INT_EN_KEY_RELEASE |
				PM_INT_EN_KEY_PRESS | PM_INT_EN_PWR_EN);
	lombo_rtc_ld_int_enable();
}

static irqreturn_t lombo_rtc_alarmirq(int irq, void *id)
{
	void __iomem *base = lombo_rtc_base;
	struct lombo_rtc *rtc = (struct lombo_rtc *)id;
	unsigned long flags = 0;

	u32 ld_int_pending	= 0;
	u32 ld_int_enable	= 0;
	u32 clk_src_stat	= 1;
	u32 alarm_int_enable	= 0;

	spin_lock_irqsave(&rtc->lock, flags);
	ld_int_enable = csp_rtc_get_ld_int_enable(base);
	ld_int_pending = csp_rtc_get_ld_int_pending(base);
	alarm_int_enable = csp_rtc_get_alarm_int_enable(base);
	rtc->alarm_int_pending = csp_rtc_get_alarm_init_pending(base);
	rtc->power_irq_pending = csp_rtc_power_query_irq_pending(base);
	spin_unlock_irqrestore(&rtc->lock, flags);

	if ((ld_int_enable & ld_int_pending) == 1) {
		clk_src_stat = csp_rtc_get_clk_src_stat(base);
		if (CLK_SRC_LFEOSC != clk_src_stat) {
			dev_err(&(rtc->rtc_dev->dev), "No external low-frequency crystal oscillator detected");
			while (CLK_SRC_RCOSC != clk_src_stat) {
				clk_src_stat = csp_rtc_get_clk_src_stat(base);
				csp_rtc_set_clk_src(base, CLK_SRC_RCOSC);
			}
		}
		csp_rtc_ld_int_clr(base);
	}
	if ((alarm_int_enable & rtc->alarm_int_pending) == 1) {
		pm_stay_awake(&(rtc->rtc_dev->dev));
		rtc_update_irq(rtc->rtc_dev, 1, RTC_AF | RTC_IRQF);
		csp_rtc_alarm_int_clr(base);
		pm_relax(&(rtc->rtc_dev->dev));
	}
	if (rtc->power_irq_pending != 0) {
		PRT_DBG("irq_pending=0x%x\n", rtc->power_irq_pending);
		csp_rtc_power_clear_irq_pending(base, rtc->power_irq_pending);
		schedule_work(&rtc->irq_work);
	}

	return IRQ_HANDLED;
}

static int lombo_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
	reg_rtc_rtc_ymd_t	reg_ymd;
	reg_rtc_rtc_week_t	reg_week;
	reg_rtc_rtc_hms_t	reg_hms;
	u32 have_retried = 0;
	void __iomem *base = lombo_rtc_base;

retry_get_time:
	reg_ymd.val	= csp_rtc_get_ymd(base);
	reg_week.val	= csp_rtc_get_week(base);
	reg_hms.val	= csp_rtc_get_hms(base);

	if ((0 == reg_hms.bits.second) && (have_retried == 0)) {
		have_retried = 1;
		goto retry_get_time;
	}

	rtc_tm->tm_year	= reg_ymd.bits.year - 1900;
	rtc_tm->tm_mon	= reg_ymd.bits.month - 1;
	rtc_tm->tm_mday	= reg_ymd.bits.day;
	rtc_tm->tm_wday	= reg_week.bits.week;
	rtc_tm->tm_hour	= reg_hms.bits.hour;
	rtc_tm->tm_min	= reg_hms.bits.minute;
	rtc_tm->tm_sec	= reg_hms.bits.second;

	return 0;
}

static int lombo_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	u32 year	= 0;
	u32 mon		= 0;
	u32 day		= 0;
	u32 week	= 0;
	u32 hour	= 0;
	u32 min		= 0;
	u32 sec		= 0;
	struct rtc_time ti;
	void __iomem *base = lombo_rtc_base;

	year	= tm->tm_year + 1900;
	mon	= tm->tm_mon + 1;
	day	= tm->tm_mday;
	week	= tm->tm_wday;
	hour	= tm->tm_hour;
	min	= tm->tm_min;
	sec	= tm->tm_sec;

	do {
		csp_rtc_set_ymd(base, day, mon, year);
		csp_rtc_set_week(base, week);
		csp_rtc_set_hms(base, sec, min, hour);
		mdelay(3);
		lombo_rtc_gettime(base, &ti);
	} while ((ti.tm_sec != sec) || (ti.tm_mday != day));

	return 0;
}

static int lombo_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	int year	= -1;
	int month	= -1;
	int day		= -1;
	int week	= -1;
	int hour	= -1;
	int minute	= -1;
	int second	= -1;
	void __iomem *base = lombo_rtc_base;
	struct rtc_time *alm_tm = &alrm->time;
	reg_rtc_rtc_alarm_hms_match_t reg_alm_hms;
	u32 alarm_match_en_stat = 0;

	alarm_match_en_stat = csp_rtc_alarm_match_get_enable(base);
	reg_alm_hms.val = csp_rtc_get_alarm_hms(base);
	if (alarm_match_en_stat &  ALARM_DAY_MATCH_EN)
		day = csp_rtc_get_alarm_day(base);
	if (alarm_match_en_stat & ALARM_WEEK_MATCH_EN)
		week = csp_rtc_get_alarm_week(base);
	if (alarm_match_en_stat & ALARM_HOUR_MATCH_EN)
		hour = reg_alm_hms.bits.hour_match;
	if (alarm_match_en_stat & ALARM_MINUTE_MATCH_EN)
		minute = reg_alm_hms.bits.minute_match;
	if (alarm_match_en_stat & ALARM_SECOND_MATCH_EN)
		second = reg_alm_hms.bits.second_match;

	alm_tm->tm_year	= year;
	alm_tm->tm_mon	= month;
	alm_tm->tm_mday	= day;
	alm_tm->tm_wday	= week;
	alm_tm->tm_hour	= hour;
	alm_tm->tm_min	= minute;
	alm_tm->tm_sec	= second;

	return 0;
}

static int lombo_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	int hour	= -1;
	int minute	= -1;
	int second	= -1;
	u32 alarm_match_en = 0;
	void __iomem *base = lombo_rtc_base;
	struct lombo_rtc *rtc = dev_get_drvdata(dev);

	rtc->alarm_int_pending = 0;
	alarm_match_en = csp_rtc_alarm_match_get_enable(base);
	hour	= alrm->time.tm_hour;
	minute	= alrm->time.tm_min;
	second	= alrm->time.tm_sec;

	if (hour < 24 && hour >= 0)
		alarm_match_en |= ALARM_HOUR_MATCH_EN;
	if (minute < 60 && minute >= 0)
		alarm_match_en |= ALARM_MINUTE_MATCH_EN;
	if (second < 60 && second >= 0)
		alarm_match_en |= ALARM_SECOND_MATCH_EN;

	csp_rtc_alarm_match_enable(base, alarm_match_en);
	csp_rtc_set_alarm_hms(base, hour, minute, second);

	lombo_rtc_alarm_irq_enable(dev, alrm->enabled);

	return 0;
}

#if CONFIG_PM
static int lombo_rtc_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int lombo_rtc_resume(struct platform_device *pdev)
{
	return 0;
}
#else
#define lombo_rtc_suspend NULL
#define lombo_rtc_resume NULL
#endif

static int lombo_rtc_ioctl(struct device *dev,
				unsigned int cmd, unsigned long args)
{
	struct lombo_rtc *rtc = dev_get_drvdata(dev);
	void __user *uarg = (void __user *) args;

	switch (cmd) {
	case RTC_GET_ALM_PENDING:
		*(int *)uarg = rtc->alarm_int_pending;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct rtc_class_ops lombo_rtc_ops = {
	.read_time		= lombo_rtc_gettime,
	.set_time		= lombo_rtc_settime,
	.read_alarm		= lombo_rtc_getalarm,
	.set_alarm		= lombo_rtc_setalarm,
	.ioctl			= lombo_rtc_ioctl,
	.alarm_irq_enable	= lombo_rtc_alarm_irq_enable,
};

static const struct of_device_id lombo_rtc_match[] = {
	{ .compatible = "lombo,n7-rtc" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_rtc_match);

static int lombo_rtc_probe(struct platform_device *pdev)
{
	int ret		= 0;
	unsigned long flags	= 0;
	struct lombo_rtc *rtc	= NULL;
	struct resource *res	= NULL;
	struct rtc_time rtc_tm	= {0};
	struct input_dev *powerkey_input_dev = NULL;

	rtc = kzalloc(sizeof(struct lombo_rtc), GFP_KERNEL);
	if (!rtc) {
		PRT_ERR("failed to malloc memory\n");
		return -ENOMEM;
	}

	/* get IRQ */
	rtc->irq_no = platform_get_irq(pdev, 0);
	if (rtc->irq_no < 0) {
		dev_err(&pdev->dev, "no irq for rtc!\n");
		ret = __LINE__;
		goto out;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (NULL == res) {
		dev_err(&pdev->dev, "failed to get memory region resource\n");
		ret = __LINE__;
		goto out;
	}

	lombo_rtc_base = ioremap(res->start, resource_size(res));
	if (NULL == lombo_rtc_base) {
		dev_err(&pdev->dev, "failed ioremap()!\n");
		ret = __LINE__;
		goto out;
	}

	/* register rtc device */
	rtc->rtc_dev = rtc_device_register("n7-rtc", &pdev->dev,
				&lombo_rtc_ops, THIS_MODULE);
	if (IS_ERR(rtc->rtc_dev)) {
		dev_err(&pdev->dev, "cannot attach rtc\n");
		ret = PTR_ERR(rtc->rtc_dev);
		goto err_nortc;
	}

	/* register input device */
	powerkey_input_dev = input_allocate_device();
	if (!powerkey_input_dev) {
		PRT_ERR("alloc powerkey input device error\n");
		ret = __LINE__;
		goto err_alloc_idev;
	}
	powerkey_input_dev->name = "lombo_rtc_power";
	powerkey_input_dev->dev.parent = &pdev->dev;
	set_bit(EV_KEY, powerkey_input_dev->evbit);
	set_bit(KEY_POWER, powerkey_input_dev->keybit);

	ret = input_register_device(powerkey_input_dev);
	if (ret) {
		PRT_ERR("failed to register lombo powerkey\n");
		ret = __LINE__;
		goto err_register_idev;
	}
	rtc->idev = powerkey_input_dev;

	mutex_init(&rtc->m_lock);
	spin_lock_init(&rtc->lock);

	spin_lock_irqsave(&rtc->lock, flags);
	/* rtc clk init */
	rtc->power_irq_pending = 0;
	lombo_rtc_sys_init();
	spin_unlock_irqrestore(&rtc->lock, flags);

	INIT_WORK(&rtc->irq_work, lombo_rtc_power_irq_work);
	PRT_INFO("irq pending=0x%x\n",
			csp_rtc_power_query_irq_pending(lombo_rtc_base));

	device_init_wakeup(&pdev->dev, 1);

	lombo_rtc_gettime(NULL, &rtc_tm);
	if (rtc_valid_tm(&rtc_tm)) {
		rtc_tm.tm_year	= 70;
		rtc_tm.tm_mon	= 0;
		rtc_tm.tm_mday	= 1;
		rtc_tm.tm_hour	= 0;
		rtc_tm.tm_min	= 0;
		rtc_tm.tm_sec	= 0;

		lombo_rtc_settime(NULL, &rtc_tm);
		dev_warn(&pdev->dev, "warning: invalid RTC value so initializing it\n");
	}
	platform_set_drvdata(pdev, rtc);

	ret = request_irq(rtc->irq_no, lombo_rtc_alarmirq, IRQF_DISABLED,
				"n7-rtc", rtc);
	if (0 != ret) {
		dev_err(&pdev->dev, "IRQ%d error %d\n", rtc->irq_no, ret);
		goto err_reguest_irq;
	}

	device_init_wakeup(&powerkey_input_dev->dev, true);

	return 0;

err_reguest_irq:
	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(rtc->rtc_dev);
err_nortc:
	input_unregister_device(powerkey_input_dev);
err_register_idev:
	input_free_device(powerkey_input_dev);
err_alloc_idev:
	iounmap(lombo_rtc_base);
out:
	kfree(rtc);
	return ret;
}

static struct platform_driver lombo_rtc_driver = {
	.probe		= lombo_rtc_probe,
	.remove		= lombo_rtc_remove,
	.suspend	= lombo_rtc_suspend,
	.resume		= lombo_rtc_resume,
	.driver		= {
		.name	= "lombo_rtc",
		.owner	= THIS_MODULE,
		.of_match_table	= lombo_rtc_match,
	},
};

static int __init lombo_rtc_init(void)
{
	return platform_driver_register(&lombo_rtc_driver);
}

static void __exit lombo_rtc_exit(void)
{
	platform_driver_unregister(&lombo_rtc_driver);
}

module_init(lombo_rtc_init);
module_exit(lombo_rtc_exit);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech RTC module");
MODULE_LICENSE("GPL");

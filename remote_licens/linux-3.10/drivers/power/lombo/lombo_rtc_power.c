/*
 * lombo_rtc_power.c - Driver of LomboTech RTC Power Manage
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
#define DEBUG

#include <linux/delay.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include <mach/debug.h>
#include <mach/rtc_power.h>
#include <mach/csp.h>

 /* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME "RPM"

/**
 * struct lombo_rtc_power - Runtime info holder for RTC power driver.
 * @irq: Irq No.
 * @irq_pending: Irq pending status.
 * @idev: Pointer to input device.
 * @lock: Spin lock.
 * @m_lock: Mutex.
 * @irq_work: Irq work struct.
 */
struct lombo_rtc_power {
	u32 irq;
	u32 irq_pending;
	struct input_dev *idev;
	spinlock_t lock;
	struct mutex m_lock;
	struct work_struct irq_work;
};

static void __iomem *lombo_rtc_base;
static struct lombo_rtc_power *rtc_power;

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

bool rtc_power_int_happen(void)
{
	u32 pending;
	void __iomem *base = lombo_rtc_base;

	if (!base) {
		PRT_WARN("lombo_rtc_base == NULL\n");
		return false;
	}

	pending = csp_rtc_power_query_irq_pending(base);
	if (pending & PM_PEND_ALL)
		return true;

	return false;
};

void rtc_power_int_handle(void)
{
	unsigned long flags = 0;
	void __iomem *base = lombo_rtc_base;

	if (rtc_power == NULL) {
		PRT_WARN("rtc_power == NULL\n");
		return;
	}

	spin_lock_irqsave(&rtc_power->lock, flags);
	rtc_power->irq_pending = csp_rtc_power_query_irq_pending(base);
	csp_rtc_power_clear_irq_pending(base, rtc_power->irq_pending);
	spin_unlock_irqrestore(&rtc_power->lock, flags);

	PRT_DBG("SIO_INT: irq_pending = 0x%x\n", rtc_power->irq_pending);

	schedule_work(&rtc_power->irq_work);
};

static void lombo_key_press_handler(struct input_dev *idev)
{
	PRT_DBG("\n");
	input_report_key(idev, KEY_POWER, 1);
	input_sync(idev);
}

static void lombo_key_short_handler(struct input_dev *idev)
{
	PRT_DBG("\n");
	input_report_key(idev, KEY_POWER, 1);
	input_sync(idev);
	msleep(200);
	input_report_key(idev, KEY_POWER, 0);
	input_sync(idev);
}

static void lombo_key_long_handler(struct input_dev *idev)
{
	PRT_DBG("\n");
	input_report_key(idev, KEY_POWER, 1);
	input_sync(idev);
	ssleep(3);
	input_report_key(idev, KEY_POWER, 0);
	input_sync(idev);
}

static void lombo_key_release_handler(struct input_dev *idev)
{
	PRT_DBG("\n");
	input_report_key(idev, KEY_POWER, 0);
	input_sync(idev);
}

static void lombo_power_wake_discon_handler(void)
{
	PRT_DBG("\n");
}

static void lombo_power_wake_con_handler(void)
{
	PRT_DBG("\n");
}

static void lombo_rtc_power_irq_work(struct work_struct *work)
{
	struct lombo_rtc_power *erp = container_of(work,
					struct lombo_rtc_power, irq_work);
	unsigned long flags = 0;

	mutex_lock(&erp->m_lock);

	if (erp->irq_pending & PM_PEND_KEY_PRESS) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->irq_pending &= ~(PM_PEND_KEY_PRESS);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_key_press_handler(erp->idev);
	}

	if (erp->irq_pending & PM_PEND_KEY_SHORT) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->irq_pending &= ~(PM_PEND_KEY_SHORT);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_key_short_handler(erp->idev);
	}

	if (erp->irq_pending & PM_PEND_KEY_LONG) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->irq_pending &= ~(PM_PEND_KEY_LONG);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_key_long_handler(erp->idev);
	}

	if (erp->irq_pending & PM_PEND_KEY_RELEASE) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->irq_pending &= ~(PM_PEND_KEY_RELEASE);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_key_release_handler(erp->idev);
	}

	if (erp->irq_pending & PM_PEND_PM_DISCON) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->irq_pending &= ~(PM_PEND_PM_DISCON);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_power_wake_discon_handler();
	}

	if (erp->irq_pending & PM_PEND_PM_CON) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->irq_pending &= ~(PM_PEND_PM_CON);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_power_wake_con_handler();
	}

	mutex_unlock(&erp->m_lock);
}

/**
 * lombo_rtc_power_handler - powerkey IRQ handler.
 * @irq: irq number.
 * @dev_id: cookie to identify the device.
 *
 * Return IRQ_HANDLED if success.
 */
static irqreturn_t lombo_rtc_power_handler(int irq, void *dev_id)
{
	struct lombo_rtc_power *erp = (struct lombo_rtc_power *)dev_id;
	unsigned long flags = 0;

	spin_lock_irqsave(&erp->lock, flags);
	erp->irq_pending = csp_rtc_power_query_irq_pending(lombo_rtc_base);
	csp_rtc_power_clear_irq_pending(lombo_rtc_base, erp->irq_pending);
	spin_unlock_irqrestore(&erp->lock, flags);

	PRT_DBG("irq_pending=0x%x\n", erp->irq_pending);

	schedule_work(&erp->irq_work);

	return IRQ_HANDLED;
}

#ifdef CONFIG_ARCH_LOMBO_N9
static void lombo_poweroff(void)
{
	csp_rtc_power_unlock(lombo_rtc_base);
	csp_rtc_power_en_disable(lombo_rtc_base);
	csp_rtc_power_lock(lombo_rtc_base);
}
#endif

static int lombo_rtc_power_probe(struct platform_device *pdev)
{
	struct lombo_rtc_power *erp;
	struct resource *res	= NULL;
	struct input_dev *powerkey_input_dev;
	unsigned long flags = 0;
	int err = 0;

	erp = kzalloc(sizeof(struct lombo_rtc_power), GFP_KERNEL);
	if (!erp) {
		PRT_ERR("failed to malloc memory\n");
		return -ENOMEM;
	}

	/* get IRQ */
	erp->irq = platform_get_irq(pdev, 0);
	if (erp->irq < 0) {
		PRT_ERR("failed to get irq\n");
		err = __LINE__;
		goto out;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (NULL == res) {
		PRT_ERR("failed to get memory region resource\n");
		err = __LINE__;
		goto out;
	}

	lombo_rtc_base = ioremap(res->start, resource_size(res));
	if (NULL == lombo_rtc_base) {
		PRT_ERR("failed ioremap()!\n");
		err = __LINE__;
		goto out;
	}

	/* register input device */
	powerkey_input_dev = input_allocate_device();
	if (!powerkey_input_dev) {
		PRT_ERR("alloc powerkey input device error\n");
		err = __LINE__;
		goto out;
	}

	powerkey_input_dev->name = "lombo_rtc_power";
	powerkey_input_dev->dev.parent = &pdev->dev;
	set_bit(EV_KEY, powerkey_input_dev->evbit);
	set_bit(KEY_POWER, powerkey_input_dev->keybit);

	err = input_register_device(powerkey_input_dev);
	if (err) {
		PRT_ERR("failed to register lombo powerkey\n");
		err = __LINE__;
		goto out_register;
	}

	erp->idev = powerkey_input_dev;

	mutex_init(&erp->m_lock);
	spin_lock_init(&erp->lock);

	spin_lock_irqsave(&erp->lock, flags);
	erp->irq_pending = 0;
	spin_unlock_irqrestore(&erp->lock, flags);

	INIT_WORK(&erp->irq_work, lombo_rtc_power_irq_work);

	PRT_INFO("irq pending=0x%x\n",
			csp_rtc_power_query_irq_pending(lombo_rtc_base));

	/* clear all irq pending when init */
	csp_rtc_power_clear_irq_pending(lombo_rtc_base, PM_PEND_ALL);

	/* enable rtc power irq */
#ifdef CONFIG_ARCH_LOMBO_N7
	csp_rtc_power_enable_irq_by_source(lombo_rtc_base,
			PM_INT_EN_KEY_RELEASE | PM_INT_EN_KEY_LONG |
			PM_INT_EN_KEY_SHORT | PM_INT_EN_KEY_PRESS |
			PM_INT_EN_PWR_EN);
#endif
#ifdef CONFIG_ARCH_LOMBO_N9
	csp_rtc_power_enable_irq_by_source(lombo_rtc_base,
			PM_INT_EN_KEY_RELEASE | PM_INT_EN_KEY_LONG |
			PM_INT_EN_KEY_SHORT | PM_INT_EN_KEY_PRESS |
			PM_INT_EN_PWR_CON | PM_INT_EN_PWR_DISCON);
#endif

	platform_set_drvdata(pdev, erp);

#ifdef CONFIG_ARCH_LOMBO_N9
	#ifndef CONFIG_PINCTRL_N9_SIO
	err = request_irq(erp->irq, lombo_rtc_power_handler,
				0, "rtc_power", erp);
	if (unlikely(err)) {
		PRT_ERR("can not request irq %d\n", erp->irq);
		err = __LINE__;
		goto out_irq;
	}
	#endif /* CONFIG_PINCTRL_N9_SIO */
#else
	err = request_irq(erp->irq, lombo_rtc_power_handler,
				0, "rtc_power", erp);
	if (unlikely(err)) {
		PRT_ERR("can not request irq %d\n", erp->irq);
		err = __LINE__;
		goto out_irq;
	}
#endif

#ifdef CONFIG_ARCH_LOMBO_N9
	pm_power_off = &lombo_poweroff;
#endif

	PRT_INFO("probe succeed, irq:%d\n", erp->irq);
	rtc_power = erp;
	return 0;

out_irq:
	input_unregister_device(powerkey_input_dev);
out_register:
	input_free_device(powerkey_input_dev);
out:
	kfree(erp);
	return err;
}

static int lombo_rtc_power_remove(struct platform_device *pdev)
{
	struct lombo_rtc_power *erp = platform_get_drvdata(pdev);

	input_unregister_device(erp->idev);
	input_free_device(erp->idev);
	kfree(erp);
#ifdef CONFIG_ARCH_LOMBO_N9
	if (pm_power_off == &lombo_poweroff)
		pm_power_off = NULL;
#endif

	return 0;
}

static const struct of_device_id lombo_rtc_power_dt_ids[] = {
	{ .compatible = "lombotech,rtc_power", },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_rtc_power_dt_ids);

static struct platform_driver lombo_rtc_power_driver = {
	.driver = {
		.name = "lombo-rtc-power",
		.of_match_table = lombo_rtc_power_dt_ids,
	},
	.probe  = lombo_rtc_power_probe,
	.remove = lombo_rtc_power_remove,
};
module_platform_driver(lombo_rtc_power_driver);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("RTC Power Manage Driver for Lombo SoCs");
MODULE_LICENSE("GPL");

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
#include "n7_rtc.h"

static void lombo_key_press_handler(struct input_dev *idev)
{
	PRT_DBG("\n");
	input_report_key(idev, KEY_POWER, 1);
	input_sync(idev);
}

static void lombo_key_release_handler(struct input_dev *idev)
{
	PRT_DBG("\n");
	input_report_key(idev, KEY_POWER, 0);
	input_sync(idev);
}

void lombo_rtc_power_irq_work(struct work_struct *work)
{
	struct lombo_rtc *erp = container_of(work,
					struct lombo_rtc, irq_work);
	unsigned long flags = 0;

	mutex_lock(&erp->m_lock);


	if (erp->power_irq_pending & PM_PEND_KEY_PRESS) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->power_irq_pending &= ~(PM_PEND_KEY_PRESS);
		spin_unlock_irqrestore(&erp->lock, flags);
		/* the system can't enter suspend */
		pm_stay_awake(&erp->idev->dev);
		lombo_key_press_handler(erp->idev);
	}

	if (erp->power_irq_pending & PM_PEND_KEY_RELEASE) {
		spin_lock_irqsave(&erp->lock, flags);
		erp->power_irq_pending &= ~(PM_PEND_KEY_RELEASE);
		spin_unlock_irqrestore(&erp->lock, flags);
		lombo_key_release_handler(erp->idev);
		/* we can enter suspend now */
		pm_relax(&erp->idev->dev);
	}

	mutex_unlock(&erp->m_lock);
}

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("RTC Power Manage Driver for Lombo SoCs");
MODULE_LICENSE("GPL");

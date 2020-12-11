/*
 * n7-rtc.h - rtc head file
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

#ifndef __LOMBO_RTC_H___
#define __LOMBO_RTC_H___

#define ENABLE			1
#define DISABLE			0

/* RTC CLK source */
#define CLK_SRC_LFEOSC		1
#define CLK_SRC_RCOSC		0

/**
 * struct lombo_rtc - Runtime info holder for RTC and RTC power driver.
 * @irq_no: Irq No.
 * @power_irq_pending: Power irq pending status.
 * @idev: Pointer to input device.
 * @rtc: Pointer to RTC device.
 * @lock: Spin lock.
 * @m_lock: Mutex.
 * @irq_work: Irq work struct.
 */
struct lombo_rtc {
	u32 irq_no;
	u32 power_irq_pending;
	u32 alarm_int_pending;
	struct input_dev *idev;
	struct rtc_device *rtc_dev;
	spinlock_t lock;
	struct mutex m_lock;
	struct work_struct irq_work;
};

void lombo_rtc_power_irq_work(struct work_struct *work);

#endif

/*
 * rtc_pm.c - rtc power management operations
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

#include <mach/csp.h>
#include "../lombo.h"

#ifdef CONFIG_SMP
/**
 * rtc_set_hot_rst_flag - Set the HOT_RST_FLAG reg
 * @value: value to set
 *
 * The HOT_RST_FLAG reg write flow:
 * 1. Enable the write access by setting hot_rst_flag flag in LOCK_CTRL reg
 * 2. Write the HOT_RST_FLAG reg
 * 3. Disable the write access by clring hot_rst_flag flag, to prevent
 *     incorrectly written
 *
 * The HOT_RST_FLAG reg's usage:
 * 1. It is used by cpu0 in crom
 * 2. When it's set, cpu0 will jump to lombo_secondary_startup which stored
 *     in HOT_RST_EXIT reg. And in lombo_secondary_startup it will goto the
 *     resume addr that set by cpuidle. cpu0 can never be hotpluged out.
 * 3. When it's not set, cpu0 will run cold boot procedure
 */
void rtc_set_hot_rst_flag(u32 value)
{
	u32 reg;

	/* enable the write access of the HOT_RST_FLAG reg */
	reg = readl((void *)VA_RTC_LOCK_CTRL);
	reg |= 0xea5 << 20;
	reg |= BIT(5);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);

	/* write the HOT_RST_FLAG reg */
	__raw_writel(value, (void *)VA_RTC_HOT_RST_FLAG);

	/* disable the write access of the HOT_RST_FLAG reg */
	reg &= ~BIT(5);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);
}

/**
 * rtc_set_hot_rst_exit - Set the HOT_RST_EXIT reg
 * @addr: the boot physical addr to set
 *
 * The HOT_RST_EXIT reg write flow:
 * 1. Enable the write access by setting hot_rst_exit flag in LOCK_CTRL reg
 * 2. Write the HOT_RST_EXIT reg
 * 3. Disable the write access by clring hot_rst_exit flag, to prevent
 *     incorrectly written
 *
 * The HOT_RST_EXIT reg's usage:
 * 1. It store the addr of lombo_secondary_startup, which executed by crom
 * 2. For cpu0, it's valid only when HOT_RST_FLAG is set(by cpuidle), and the
 *     lombo_secondary_startup will jump to resume addr that set by cpuidle.
 *     cpu0 can never be hotpluged out. If HOT_RST_FLAG isnot set, cpu0 will
 *     run cold boot procedure
 * 3. For other cpu, it's always valid. In lombo_secondary_startup, we will
 *     will check whether cpu powered off by cpuidle or hotplug, and jump
 *     to cpu_resume or secondary_startup
 */
void rtc_set_hot_rst_exit(u32 addr)
{
	u32 reg;

	/* enable the write access of the HOT_RST_EXIT reg */
	reg = readl((void *)VA_RTC_LOCK_CTRL);
	reg |= (0xea5 << 20);
	reg |= BIT(4);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);

	/* write the HOT_RST_EXIT reg */
	__raw_writel(addr, (void *)VA_RTC_HOT_RST_EXIT);

	/* disable the write access of the HOT_RST_EXIT reg */
	reg &= ~BIT(4);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);
}
#endif

#ifdef CONFIG_PM
/**
 * rtc_set_deep_slp_flag - Set the DEEP_SLP_FLAG reg
 * @value: value to set
 *
 * The DEEP_SLP_FLAG reg write flow:
 * 1. Enable the write access by setting deep_slp_flag flag in LOCK_CTRL reg
 * 2. Write the DEEP_SLP_FLAG reg
 * 3. Disable the write access by clring deep_slp_flag flag, to prevent
 *     incorrectly written
 *
 * The DEEP_SLP_FLAG reg is used in booster for cpu0, when DEEP_SLP_FLAG reg is
 *   set(to 1?), cpu0 will jump to the resume addr in DEEP_SLP_EXIT reg.
 */
void rtc_set_deep_slp_flag(u32 value)
{
	u32 reg;

	/* enable the write access of the DEEP_SLP_FLAG reg */
	reg = readl((void *)VA_RTC_LOCK_CTRL);
	reg |= (0xea5 << 20);
	reg |= BIT(1);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);

	/* write the DEEP_SLP_FLAG reg */
	__raw_writel(value, (void *)VA_RTC_DEEP_SLP_FLAG);

	/* disable the write access of the DEEP_SLP_FLAG reg */
	reg &= ~BIT(1);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);
}

/**
 * rtc_set_deep_slp_exit - Set the DEEP_SLP_EXIT reg
 * @addr: the resume physical addr to set
 *
 * The DEEP_SLP_EXIT reg write flow:
 * 1. Enable the write access by setting deep_slp_exit flag in LOCK_CTRL reg
 * 2. Write the DEEP_SLP_EXIT reg
 * 3. Disable the write access by clring deep_slp_exit flag, to prevent
 *     incorrectly written
 *
 * The DEEP_SLP_EXIT reg is used in booster for cpu0, it store the resume addr.
 *   When DEEP_SLP_FLAG reg is set, cpu0 will jump to the resume addr
 */
void rtc_set_deep_slp_exit(u32 addr)
{
	u32 reg;

	/* enable the write access of the DEEP_SLP_EXIT reg */
	reg = readl((void *)VA_RTC_LOCK_CTRL);
	reg |= (0xea5 << 20);
	reg |= BIT(0);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);

	/* write the DEEP_SLP_EXIT reg */
	__raw_writel(addr, (void *)VA_RTC_DEEP_SLP_EXIT);

	/* disable the write access of the DEEP_SLP_EXIT reg */
	reg &= ~BIT(0);
	writel(reg, (void *)VA_RTC_LOCK_CTRL);
}
#endif

